
INCLUDE (CheckFunctionExists)
INCLUDE (CheckLibraryExists)
INCLUDE (CheckTypeSize)
INCLUDE (CheckIncludeFile)
INCLUDE (CheckCSourceCompiles)

INCLUDE (FindPkgConfig)
INCLUDE (TestBigEndian)


set(PACKAGE_NAME "Cadius")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE "Release")

set(__MSVC__ ${MSVC})
set(__CLANG__ FALSE)
set(__GCC__ FALSE)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(__CLANG__ TRUE)
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(__GCC__ TRUE)
endif()

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

if(__CLANG__)
	add_compile_options(
		-Wall -fomit-frame-pointer
	)
endif()


if(__GCC__)
	add_compile_options(
		-Wall -fomit-frame-pointer
	)
endif()



#configure_file(string_extra.h.in string_extra.h)
#configure_file(version.h.in version.h)
#configure_file(version.txt.in version.txt)
#configure_file(assets/control.in assets/control)

add_executable(Cadius
  Dc_Memory.c Dc_OS.c Dc_Prodos.c Dc_Shared.c Main.c Prodos_Add.c
	Prodos_Check.c Prodos_Create.c Prodos_Delete.c Prodos_Dump.c
	Prodos_Extract.c Prodos_Move.c Prodos_Rename.c Prodos_Source.c
)
