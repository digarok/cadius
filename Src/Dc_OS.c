/***********************************************************************/
/*                                                                     */
/*   Dc_OS.c : Module pour les fonctions sp�cifiques � l'OS.           */
/*                                                                     */
/***********************************************************************/
/*  Auteur : Olivier ZARDINI  *  Brutal Deluxe Software  *  Mar 2012   */
/***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32) || defined(WIN64)
#include <io.h>
#include <malloc.h>
#include <direct.h>
#include <windows.h>
#elif defined(__APPLE__) || defined (linux)
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <ftw.h>
#endif
#include <string.h>
#ifdef LINUX
#include <strings.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <errno.h>

#include "Dc_Shared.h"
#include "Dc_Prodos.h"
#include "Dc_Memory.h"
#include "Dc_OS.h"

#if !defined(S_ISDIR) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

static int MakeAllDir(char *);

/******************************************************/
/*  my_DeleteFile() :  Supprime un fichier du disque. */
/******************************************************/
void my_DeleteFile(char *file_path)
{
  /* Rend le fichier visible */
  my_SetFileAttribute(file_path,SET_FILE_VISIBLE);

  /* Supprime le fichier */
  unlink(file_path);
}

int myfunc(const char *path, const struct stat *sptr, int type)
{
#if defined(__APPLE__) || defined(linux)
  /* Types: FTW_D = directory, FTW_F = normal file, FTW_DNR = non-traversable
   *            directory.  Do a man ftw for the full scoop!!!!*/
  if(!my_stricmp((char *)path,".") || !my_stricmp((char *)path,".."))
    return 0;
  if(path && *path && path[strlen(path) - 1] == '/') 
    return 0;
  printf("Name = %s\n", path);
  if (type == FTW_DNR) printf("Directory %s cannot be traversed.\n", path);
  if (type == FTW_D)
    return 0; 

  //if(MatchHierarchie(buffer_file_path,hierarchy))
   my_Memory(MEMORY_ADD_FILE,(char *)path,NULL);
#endif
  return 0;
} 

/********************************************************************/
/*  GetFolderFiles() :  R�cup�re tous les fichiers d'un r�pertoire. */
/********************************************************************/
int GetFolderFiles(char *folder_path, char *hierarchy)
{
  int error = 0;
#if defined(__APPLE__) || defined(linux)
  ftw(folder_path, myfunc, 7);
  return(0);
#elif defined(WIN32) || defined(WIN64)
  int rc;
  long hFile;
  int first_time = 1;
  struct _finddata_t c_file;
  char *buffer_folder_path = NULL;
  char *buffer_file_path = NULL;

  /* Rien � faire */
  if(folder_path == NULL)
    return(0);
  if(strlen(folder_path) == 0)
    return(0);
  error = 0;

  /* Allocation m�moire */
  buffer_folder_path = (char *) calloc(1,1024);
  buffer_file_path = (char *) calloc(1,1024);
  if(buffer_folder_path == NULL || buffer_file_path == NULL)
    return(1);
  strcpy(buffer_folder_path,folder_path);
  if(buffer_folder_path[strlen(buffer_folder_path)-1] != '\\' && buffer_folder_path[strlen(buffer_folder_path)-1] != '/')
    strcat(buffer_folder_path,FOLDER_CHARACTER);
  strcat(buffer_folder_path,"*.*");

  /** Fr: On boucle sur tous les fichiers pr�sents **/
  /** En: We loop through all the present files    **/
  while(1)
    {
      if(first_time == 1)
        {
          hFile = _findfirst(buffer_folder_path,&c_file);
          rc = (int) hFile;
        }
      else
        rc = _findnext(hFile,&c_file);

      /* Fr: On analyse le r�sultat */
      /* En: We inspect the result  */
    	if(rc == -1)
          break;    /* no more files */
 
      /** Fr: On traite cette entr�e **/
      /** En: We handle this entry   **/
      first_time++;
      strcpy(buffer_file_path,folder_path);
      if(buffer_file_path[strlen(buffer_file_path)-1] != '\\' && buffer_file_path[strlen(buffer_file_path)-1] != '/')
        strcat(buffer_file_path,FOLDER_CHARACTER);
      strcat(buffer_file_path,c_file.name);

      /** Fr: Traite le dossier de fa�on r�cursive     **/
      /** En: Handle the folder in a recursive fashion **/
      if((c_file.attrib & _A_SUBDIR) == _A_SUBDIR)
        {
          /* Fr: On ne traite ni . ni .. */
          /* En: We don't handle . or .. */
          if(!my_stricmp(c_file.name,".") || !my_stricmp(c_file.name,".."))
            continue;

          /* Fr: Recherche dans le contenu du dossier */
          /* En: Search the contents of the folder    */
          error = GetFolderFiles(buffer_file_path,hierarchy);
          if(error)
            break;
        }
      else
        {
          /* Fr: Conserve le nom du fichier */
          /* En: Store the filename         */
          if(MatchHierarchie(buffer_file_path,hierarchy))
            my_Memory(MEMORY_ADD_FILE,buffer_file_path,NULL);
        }
    }

  /* On ferme */
  _findclose(hFile);

  /* Lib�ration m�moire */
  free(buffer_folder_path);
  free(buffer_file_path);
#endif
  return(error);

}



/******************************************************/
/*  my_CreateDirectory() :  Cr�ation d'un r�pertoire. */
/******************************************************/
int my_CreateDirectory(char *directory)
{
  int i, error;
  struct stat sts;
  char buffer[1024];

  /* Isole le nom du r�pertoire */
  /* Isolate the directory name */
  strcpy(buffer,directory);
  for(i=strlen(directory); i>=0; i--)
    if(buffer[i] == '\\' || buffer[i] == '/')
      {
        buffer[i+1] = '\0';
        break;
      }

  /* V�rifie s'il existe */
  /* Verify that it exists */
  error = stat(buffer,&sts);
  if(error == 0)
    if(S_ISDIR(sts.st_mode))
      return(0);

  /* Cr�ation des r�pertoires */
  /* Creation of the directories */
  error = MakeAllDir(buffer);
  if(error == 0)
    return(1);
  
  /* On veut savoir si le ce repertoire existe et on verifie qu'on a un repertoire */
  /* We need to check that the dir exists and verify that it's a directory */
  if(stat(buffer,&sts)) {
#if defined(WIN32) || defined(WIN64)
    mkdir(buffer);
#elif  defined(__APPLE__) || defined(linux)
    /* r/w/search permissions for owner and group, and read/search permissions for others */
    mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
  } else if(!S_ISDIR(sts.st_mode)) {
    return(1);
  }

  return(0);
}


/******************************************************/
/*  MakeAllDir() :  Creation d'un nouveau r�pertoire  */
/*               :  Creation of a new directory       */
/******************************************************/
static int MakeAllDir(char *newdir)
{
  int len = (int) strlen(newdir);
  char buffer[1024];
  char *p;

  if(len <= 0)
    return(0);
  strcpy(buffer,newdir);

  if(buffer[len-1] == '/' || buffer[len-1] == '\\')
    buffer[len-1] = '\0';


#if defined(WIN32) || defined(WIN64)
  if(mkdir(buffer) == 0)
    return(1);
#elif  defined(__APPLE__) || defined(linux)
    /* r/w/search permissions for owner and group, and read/search permissions for others */
  if(mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
    return(1);
#endif


  p = buffer+1;
  while(1)
    {
      char hold;

      while(*p && *p != '\\' && *p != '/')
        p++;
      hold = *p;
      *p = 0;

#if defined(WIN32) || defined(WIN64)
      if((mkdir(buffer) == -1) && (errno == ENOENT))
        return(0);
#elif  defined(__APPLE__) || defined(linux)
      /* r/w/search permissions for owner and group, and read/search permissions for others */
      if((mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0) && (errno == ENOENT))
        return(0);
#endif
      
      if(hold == 0)
        break;
      *p++ = hold;
    }

  return(1);
}


/**********************************************************************************************/
/*  my_SetFileCreationModificationDate() :  Positionne les dates de Cr�ation et Modification. */
/**********************************************************************************************/
void my_SetFileCreationModificationDate(char *file_data_path, struct file_descriptive_entry *current_entry)
{
#if defined(WIN32) || defined(WIN64)
  BOOL result;
  HANDLE fd;
  SYSTEMTIME system_date;
  SYSTEMTIME system_utc;
  FILETIME creation_date;
  FILETIME modification_date;

  /* Init */
  memset(&creation_date,0,sizeof(FILETIME));
  memset(&modification_date,0,sizeof(FILETIME));

  /* R�cup�re un Handle sur le fichier */
  fd = CreateFile((LPCTSTR)file_data_path,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(fd == INVALID_HANDLE_VALUE)
    return;

  /** Cr�ation des dates au format Windows **/
  /* Cr�ation Date */
  memset(&system_date,0,sizeof(SYSTEMTIME));
  system_date.wYear = current_entry->file_creation_date.year + ((current_entry->file_creation_date.year < 70) ? 2000 : 1900);
  system_date.wMonth = current_entry->file_creation_date.month;
  system_date.wDay = current_entry->file_creation_date.day;
  system_date.wHour = current_entry->file_creation_time.hour;
  system_date.wMinute = current_entry->file_creation_time.minute;
  TzSpecificLocalTimeToSystemTime(NULL,&system_date,&system_utc);
  result = SystemTimeToFileTime(&system_utc,&creation_date);
  /* Modification Date */
  memset(&system_date,0,sizeof(SYSTEMTIME));
  system_date.wYear = current_entry->file_modification_date.year + ((current_entry->file_modification_date.year < 70) ? 2000 : 1900);
  system_date.wMonth = current_entry->file_modification_date.month;
  system_date.wDay = current_entry->file_modification_date.day;
  system_date.wHour = current_entry->file_modification_time.hour;
  system_date.wMinute = current_entry->file_modification_time.minute;
  TzSpecificLocalTimeToSystemTime(NULL,&system_date,&system_utc);
  result = SystemTimeToFileTime(&system_utc,&modification_date);

  /** Changement des Dates du fichier **/
  result = SetFileTime(fd,&creation_date,(LPFILETIME)NULL,&modification_date);

  /* Fermeture du fichier */
  CloseHandle(fd);
#endif
}


/********************************************************************************************/
/*  my_GetFileCreationModificationDate() :  R�cup�re les dates de Cr�ation et Modification. */
/********************************************************************************************/
void my_GetFileCreationModificationDate(char *file_data_path, struct prodos_file *current_file)
{
#if defined(WIN32) || defined(WIN64)
  BOOL result;
  HANDLE fd;
  SYSTEMTIME system_utc;
  SYSTEMTIME system_date;
  FILETIME creation_date;
  FILETIME access_date;
  FILETIME modification_date;

  /* Init */
  memset(&creation_date,0,sizeof(FILETIME));
  memset(&modification_date,0,sizeof(FILETIME));

  /* R�cup�re un Handle sur le fichier */
  fd = CreateFile((LPCTSTR)file_data_path,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(fd == INVALID_HANDLE_VALUE)
    return;

  /* R�cup�ration des dates */
  result = GetFileTime(fd,&creation_date,&access_date,&modification_date);

  /* Fermeture du fichier */
  CloseHandle(fd);

  /** Cr�ation des dates au format Prodos **/
  /* Cr�ation Date */
  FileTimeToSystemTime(&creation_date,&system_utc);
  SystemTimeToTzSpecificLocalTime(NULL,&system_utc,&system_date);
  current_file->file_creation_date = BuildProdosDate(system_date.wDay,system_date.wMonth,system_date.wYear);
  current_file->file_creation_time = BuildProdosTime(system_date.wMinute,system_date.wHour);

  /* Modification Date */
  FileTimeToSystemTime(&modification_date,&system_utc);
  SystemTimeToTzSpecificLocalTime(NULL,&system_utc,&system_date);
  current_file->file_modification_date = BuildProdosDate(system_date.wDay,system_date.wMonth,system_date.wYear);
  current_file->file_modification_time = BuildProdosTime(system_date.wMinute,system_date.wHour);
#endif
}


/****************************************************************/
/*  my_SetFileAttribute() :  Change la visibilit� d'un fichier. */
/****************************************************************/
void my_SetFileAttribute(char *file_path, int flag)
{
#if defined(WIN32) || defined(WIN64)
  DWORD file_attributes;

  /* Attributs du fichier */
  file_attributes = GetFileAttributes(file_path);

  /* Change la visibilit� */
  if(flag == SET_FILE_VISIBLE)
    {
      /* Montre le fichier */
      if((file_attributes | FILE_ATTRIBUTE_HIDDEN) == file_attributes)
        SetFileAttributes(file_path,file_attributes - FILE_ATTRIBUTE_HIDDEN);
    }
  else if(flag == SET_FILE_HIDDEN)
    {
      /* Cache le fichier */
      if((file_attributes | FILE_ATTRIBUTE_HIDDEN) != file_attributes)
        SetFileAttributes(file_path,file_attributes | FILE_ATTRIBUTE_HIDDEN);
    }
#endif
}


/*********************************************************/
/*  my_stricmp() :  Comparaison de chaine sans la casse. */
/*********************************************************/
int my_stricmp(char *string1, char *string2)
{
#if defined(WIN32) || defined(WIN64)
  return(stricmp(string1,string2));
#elif defined(__APPLE__) || defined (linux)
  return(strcasecmp(string1,string2));
#endif
}


/**********************************************************/
/*  my_strnicmp() :  Comparaison de chaine sans la casse. */
/**********************************************************/
int my_strnicmp(char *string1, char *string2, size_t length)
{
#if defined(WIN32) || defined(WIN64)
  return(strnicmp(string1,string2,length));
#elif defined(__APPLE__) || defined (linux)
  return(strncasecmp(string1,string2,length));
#endif
}

/***********************************************************************/
