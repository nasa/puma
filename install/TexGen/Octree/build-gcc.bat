@echo off


rem --- using: MinGW 3.1.0 GCC 3.4.2 ---

set GCC_LOCATION=c:\h\dev\mingw\bin
set GCC=mingw32-g++

set COMPILER=%GCC_LOCATION%\%GCC%
set COMPILE_OPTIONS=-c -x c++ -ansi -std=c++98 -pedantic -fno-gnu-keywords -fno-enforce-eh-specs -O0 -march=pentium3 -Wall -Wextra -Wabi -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wcast-align -Wwrite-strings -Wunreachable-code -Winline -Wdisabled-optimization

set LINKER=%GCC_LOCATION%\%GCC%
set LINK_OPTIONS=


rem -- complile core --
%COMPILER% %COMPILE_OPTIONS% Array.cpp Octree.cpp OctreeAuxiliary.cpp OctreeImplementation.cpp OctreeStreamOut.cpp Vector3f.cpp

rem -- compile 'applications' --
%COMPILER% %COMPILE_OPTIONS% OctreeTest.cpp OctreeExample.cpp


rem -- link test 'application' --
%LINKER% %LINK_OPTIONS% -o OctreeTest.exe Array.o Octree.o OctreeAuxiliary.o OctreeImplementation.o OctreeStreamOut.o Vector3f.o OctreeTest.o

rem -- link example 'application' --
%LINKER% %LINK_OPTIONS% -o OctreeExample.exe Array.o Octree.o OctreeAuxiliary.o OctreeImplementation.o OctreeStreamOut.o Vector3f.o OctreeExample.o
