CADIUS_BIN="../Cadius"
OUTPUT_DIR="/Users/dbrock/Google Drive/appleiigs/BDPorts/Cadius_v1.1/test"
VOLUME_NAME="TESTDISK.2mg"
PRODOS_VOLUME_NAME="TestDisk"
PRODOS_FOLDER_NAME="Test.Folder"

## semi-hardcoded setup for test files
TEST_FILES_DIR="testfiles/mydir"
mkdir -p $TEST_FILES_DIR
mkdir -p $TEST_FILES_DIR/dir2
cp testfiles/test.s.src $TEST_FILES_DIR/test1.s
cp testfiles/test.s.src $TEST_FILES_DIR/test2.s
cp testfiles/test.s.src $TEST_FILES_DIR/dir2/test3.s




## CREATE VOLUME
#
#Syntax
#    Cadius.exe CREATEVOLUME  <[2mg|hdv|po]_image_path>   <volume_name>         <volume_size>
#
#Example
#    Cadius.exe CREATEVOLUME  c:\AppleIIgs\CogitoSecondSight.2mg  CogitoVGA  1600KB

echo $CADIUS_BIN CREATEVOLUME  "$OUTPUT_DIR/$VOLUME_NAME" $PRODOS_VOLUME_NAME 1600KB
$CADIUS_BIN CREATEVOLUME  "$OUTPUT_DIR/$VOLUME_NAME" $PRODOS_VOLUME_NAME 1600KB
echo

## CREATE FOLDER
#
#Syntax
#   Cadius.exe CREATEFOLDER  <[2mg|hdv|po]_image_path>   <prodos_folder_path>
#
#Example
#   Cadius.exe CREATEFOLDER  c:\AppleIIgs\Cogito.2mg  /Cogito/COGITO.MUSIC/

echo $CADIUS_BIN CREATEFOLDER  "$OUTPUT_DIR/$VOLUME_NAME" $PRODOS_VOLUME_NAME/$PRODOS_FOLDER_NAME
$CADIUS_BIN CREATEFOLDER  "$OUTPUT_DIR/$VOLUME_NAME" $PRODOS_VOLUME_NAME/$PRODOS_FOLDER_NAME
echo


## ADD FOLDER
#
#Syntax
#   Cadius.exe ADDFOLDER     <[2mg|hdv|po]_image_path>   <prodos_folder_path>  <folder_path>
#
#Example
#   Cadius.exe ADDFOLDER  c:\AppleIIgs\Cogito.2mg  /Cogito/COGITO.MUSIC/  c:\AppleIIgs\Data\Instruments\

echo $CADIUS_BIN ADDFOLDER  "$OUTPUT_DIR/$VOLUME_NAME" $PRODOS_VOLUME_NAME/$PRODOS_FOLDER_NAME $TEST_FILES_DIR
$CADIUS_BIN ADDFOLDER  "$OUTPUT_DIR/$VOLUME_NAME" $PRODOS_VOLUME_NAME/$PRODOS_FOLDER_NAME $TEST_FILES_DIR
