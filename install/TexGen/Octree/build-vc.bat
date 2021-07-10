@echo off


rem --- using: VC++ toolkit 2003 ---

set COMPILER=cl
set COMPILE_OPTIONS=/c /Ox /GL /G6 /QIfist /TP /GX /GF /MT /W4 /nologo /V"(c) 2005 HXA7241"

set LINKER=link
set LINK_OPTIONS=/LTCG /OPT:REF /OPT:NOWIN98 /VERSION:2.0
set LINK_LIBS=kernel32.lib


rem -- complile core --
%COMPILER% %COMPILE_OPTIONS% Array.cpp Octree.cpp OctreeAuxiliary.cpp OctreeImplementation.cpp OctreeStreamOut.cpp Vector3f.cpp

rem -- complile 'applications' --
%COMPILER% %COMPILE_OPTIONS% OctreeTest.cpp OctreeExample.cpp


rem -- link test 'application' --
%LINKER% %LINK_OPTIONS% /OUT:OctreeTest.exe %LINK_LIBS% Array.obj Octree.obj OctreeAuxiliary.obj OctreeImplementation.obj OctreeStreamOut.obj Vector3f.obj OctreeTest.obj

rem -- link example 'application' --
%LINKER% %LINK_OPTIONS% /OUT:OctreeExample.exe %LINK_LIBS% Array.obj Octree.obj OctreeAuxiliary.obj OctreeImplementation.obj OctreeStreamOut.obj Vector3f.obj OctreeExample.obj
