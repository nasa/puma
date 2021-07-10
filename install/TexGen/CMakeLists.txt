PROJECT(TexGen)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#This will prevent a bunch of warnings when compiling under MSVC8, the second define stops
#swig from redefining the same macro causing additional warnings
ADD_DEFINITIONS(-DSWIG_NO_CRT_SECURE_NO_DEPRECATE)
IF(MSVC80)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(MSVC80)

# the -Wall addition returns warnings for everything (removed because produces
# some unnecessary warnings making it hard to spot the useful warnings)
# -Wno-deprecated filters out the known warning that VTK throws up as it uses
# an old style stl header
IF(UNIX)
	ADD_DEFINITIONS(-Wno-deprecated -fPIC -Wno-error=implicit-function-declaration)
ENDIF(UNIX)

# ADD_DEFINITIONS(-DTIXML_USE_STL)

IF(WIN32)
	SET(REAL_OUTPUT_PATH ${TexGen_BINARY_DIR}/$(OutDir))
ELSE(WIN32)
	SET(REAL_OUTPUT_PATH ${TexGen_BINARY_DIR})
ENDIF(WIN32)

SET(BUILD_RENDERER ON CACHE BOOL "Enable rendering using VTK.")
SET(BUILD_PYTHON_INTERFACE ON CACHE BOOL "Enable the Python interface.")
SET(BUILD_GUI ON CACHE BOOL "Build the graphical user interface.")
SET(BUILD_EXAMPLES OFF CACHE BOOL "Build the examples.")
SET(BUILD_UNIT_TESTS OFF CACHE BOOL "Build the unit tests to ensure the software is working as expected.")
SET(BUILD_CASCADE_EXPORT OFF CACHE BOOL "Build module to export geometry to Open CASCADE.")
SET(BUILD_SHARED ON CACHE BOOL "Build shared libraries")
SET(BUILD_DOCUMENTATION OFF CACHE BOOL "Build documentation using doxygen")
SET(BUILD_PROFILE OFF CACHE BOOL "Build profiling")

IF(BUILD_GUI)
	IF(NOT BUILD_RENDERER)
		MESSAGE("The renderer is needed in order to build the graphical user interface.\nSet BUILD_RENDERER ON.")
	ENDIF(NOT BUILD_RENDERER)
	IF(NOT BUILD_PYTHON_INTERFACE)
		MESSAGE("The Python interface is needed in order to build the graphical user interface.\nSet BUILD_PYTHON_INTERFACE ON.")
	ENDIF(NOT BUILD_PYTHON_INTERFACE)
ENDIF(BUILD_GUI)

SUBDIRS(Core Triangle Octree tinyxml CSparse tetgenlib LatinHypercube)

IF(BUILD_RENDERER)
	SUBDIRS(Renderer)
ENDIF(BUILD_RENDERER)
IF(BUILD_CASCADE_EXPORT)
	SUBDIRS(Export)
ENDIF(BUILD_CASCADE_EXPORT)
IF(BUILD_PYTHON_INTERFACE)
	SUBDIRS(Python)
ENDIF(BUILD_PYTHON_INTERFACE)
IF(BUILD_GUI)
	SUBDIRS(GUI)
ENDIF(BUILD_GUI)
IF(BUILD_EXAMPLES)
	SUBDIRS(Examples)
ENDIF(BUILD_EXAMPLES)
IF(BUILD_UNIT_TESTS)
	SUBDIRS(UnitTests)
ENDIF(BUILD_UNIT_TESTS)
#IF(BUILD_DOCUMENTATION)
#	SUBDIRS(Docs)
#ENDIF(BUILD_DOCUMENTATION)
IF(BUILD_PROFILE)
	SUBDIRS(ProfileTests)
ENDIF(BUILD_PROFILE)			

SET(LIBRARY_OUTPUT_PATH ${TexGen_BINARY_DIR} CACHE INTERNAL "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${TexGen_BINARY_DIR} CACHE INTERNAL "Single output directory for building all executables.")
CONFIGURE_FILE(${TexGen_SOURCE_DIR}/TexGenConfig.cmake.in
               ${TexGen_BINARY_DIR}/TexGenConfig.cmake )
#@ONLY IMMEDIATE




