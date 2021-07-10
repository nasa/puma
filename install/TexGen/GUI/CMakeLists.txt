FILE(GLOB GUI_SOURCE_FILES *.cpp)
FILE(GLOB GUI_HEADER_FILES *.h *.hxx)

SET(GUI_SOURCE_FILES ${GUI_SOURCE_FILES} wxVTKRenderWindowInteractor.cxx)

SET(CMAKE_MODULE_PATH ${TexGen_SOURCE_DIR}/GUI ${CMAKE_MODULE_PATH})

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

ADD_EXECUTABLE(TexGenGUI WIN32 ${GUI_SOURCE_FILES} ${GUI_HEADER_FILES} Embedded.i GUI.rc)
# The header files are not strictly necessary but it adds them to the project IDE making development easier

# Make use of precompiled headers if running on windows (note this probably only applies to MSVC, so it might screw things up for non-MSVC windows builds)
# GCC supports precompiled headers on linux too, but not sure how to activate it (only really necessary for somebody who is actively developing under linux)
IF(WIN32)
	SET_SOURCE_FILES_PROPERTIES(${GUI_SOURCE_FILES} PROPERTIES COMPILE_FLAGS /Yu"PrecompiledHeaders.h")
	SET_SOURCE_FILES_PROPERTIES(PrecompiledHeaders.cpp PROPERTIES COMPILE_FLAGS /Yc"PrecompiledHeaders.h")
ENDIF(WIN32)

IF(UNIX)
	SET(wxWidgets_USE_LIBS base core aui adv xrc xml html gl)
ELSE(UNIX)
	SET(wxWidgets_USE_LIBS base core aui adv xrc xml html)
ENDIF(UNIX)

FIND_PACKAGE(VTK 6.3 REQUIRED NO_MODULE)
FIND_PACKAGE(wxWidgets REQUIRED)
FIND_PACKAGE(SWIG REQUIRED)
FIND_PACKAGE(PythonLibs REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/Include)
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS} ${PYTHON_INCLUDE_PATH})
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${VTK_DEFINITIONS})

ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})
LINK_DIRECTORIES(${VTK_LIBRARY_DIRS} )
IF(UNIX)
	ADD_DEFINITIONS("`pkg-config --cflags gtk+-2.0`")
	ADD_DEFINITIONS("`wx-config --cflags`")
	TARGET_LINK_LIBRARIES(TexGenGUI
		TexGenRenderer
		TexGenCore
		${VTK_LIBRARIES}
		LatinHypercube
		${wxWidgets_LIBRARIES}
		#optimized ${PYTHON_LIBRARIES})
		${PYTHON_LIBRARIES})
ENDIF(UNIX)

ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Embedded_wrap.cxx"
#COMMAND "${SWIG_EXECUTABLE}"
#ARGS -c++ -python -outdir "${REAL_OUTPUT_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/Embedded.i"
COMMAND "${SWIG_EXECUTABLE}" -c++ -python -outdir "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Embedded.i"
MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/Embedded.i"
DEPENDS "EmbeddedPythonFunctions.h"
COMMENT "Generating SWIG wrapper code...")

#ADD_CUSTOM_COMMAND(TARGET TexGenGUI POST_BUILD
#   COMMAND copy "${CMAKE_CURRENT_SOURCE_DIR}/TexGen.xrc" "${REAL_OUTPUT_PATH}/TexGen.xrc"
#   COMMENT "Copying TexGen.xrc to binary directory...")


IF(WIN32)
	# Note ${VTK_LIBRARY_DIRS} are necessary for MSVC++, libs arn't found otherwise
	# Please do not remove unless the source of the problem is resolved
	TARGET_LINK_LIBRARIES(TexGenGUI
	TexGenRenderer
	TexGenCore
	opengl32 
	glu32
	${VTK_LIBRARIES}
	${wxWidgets_LIBRARIES}
	optimized ${PYTHON_LIBRARIES}
	comctl32
	rpcrt4
	LatinHypercube)
	#Shiny)
ENDIF(WIN32)


IF(PYTHON_DEBUG_LIBRARIES)
	TARGET_LINK_LIBRARIES(TexGenGUI debug ${PYTHON_DEBUG_LIBRARIES})
ENDIF(PYTHON_DEBUG_LIBRARIES)


IF(UNIX)
	SET_TARGET_PROPERTIES(TexGenGUI PROPERTIES COMPILE_FLAGS "${wxWidgets_CXX_FLAGS} -Wno-deprecated")
	#SET_TARGET_PROPERTIES(TexGenGUI PROPERTIES LINK_FLAGS "-Wl,-rpath, -lpython2.5")
	SET_TARGET_PROPERTIES(TexGenGUI PROPERTIES LINK_FLAGS "-Wl,-rpath,${VTK_DIR} -L${VTK_DIR}")
ELSE(UNIX)
	SET_TARGET_PROPERTIES(TexGenGUI PROPERTIES COMPILE_FLAGS "${wxWidgets_CXX_FLAGS}")
ENDIF(UNIX)

#ADD_DEPENDENCIES(TexGenGUI _TexGen _TexGenVTK)

IF(WIN32)
	INSTALL(TARGETS TexGenGUI
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
	INSTALL(FILES TexGen.xrc
	DESTINATION ${CMAKE_INSTALL_PREFIX})

	INSTALL(TARGETS TexGenGUI
	RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../Installer)

# Install the manifest file if VS2008. Haven't found way to access at build time from installation to get input folder
	IF(MSVC90 OR MSVC11 OR MSVC14)
		INSTALL(FILES ${CMAKE_BINARY_DIR}/Release/TexGenGUI.exe.manifest
		DESTINATION ${CMAKE_INSTALL_PREFIX})
		INSTALL(FILES ${CMAKE_BINARY_DIR}/Release/TexGenGUI.exe.manifest
		DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../Installer)
	ENDIF(MSVC90 OR MSVC11 OR MSVC14)	
ELSE(WIN32)
	INSTALL(TARGETS TexGenGUI
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# NOTE THIS DOES NOT WORK CORRECTLY ON UNIX SYSTEMS... THINK ABOUT EMBEDDING IT INTO THE EXECUTABLE
	INSTALL(FILES TexGen.xrc
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/TexGen)
ENDIF(WIN32)


