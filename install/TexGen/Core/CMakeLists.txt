FILE(GLOB CORE_SOURCE_FILES *.cpp)
FILE(GLOB CORE_HEADER_FILES *.h)

IF(COMMAND CMAKE_POLICY)
      CMAKE_POLICY(SET CMP0003 NEW)
    ENDIF(COMMAND CMAKE_POLICY)

IF(BUILD_SHARED)
	ADD_LIBRARY(TexGenCore SHARED ${CORE_SOURCE_FILES} ${CORE_HEADER_FILES} ../units/units.c  ../units/units.h)
ELSE(BUILD_SHARED)
	ADD_LIBRARY(TexGenCore STATIC ${CORE_SOURCE_FILES} ${CORE_HEADER_FILES} ../units/units.c  ../units/units.h)
ENDIF(BUILD_SHARED)
# The header files are not strictly necessary but it adds them to the project IDE making development easier

# Make use of precompiled headers if running on windows (note this probably only applies to MSVC, so it might screw things up for non-MSVC windows builds)
# GCC supports precompiled headers on linux too, but not sure how to activate it (only really necessary for somebody who is actively developing under linux)
IF(WIN32)
	SET_SOURCE_FILES_PROPERTIES(${CORE_SOURCE_FILES} PROPERTIES COMPILE_FLAGS /Yu"PrecompiledHeaders.h")
	SET_SOURCE_FILES_PROPERTIES(PrecompiledHeaders.cpp PROPERTIES COMPILE_FLAGS /Yc"PrecompiledHeaders.h")
ENDIF(WIN32)

# This doesn't seem to be working correctly, forget about it

#SOURCE_GROUP("Sources\\Domains" REGULAR_EXPRESSION "/Domain[A-Za-z0-9]*\\.cpp$")
#SOURCE_GROUP("Sources\\Interpolations" REGULAR_EXPRESSION "/Interpolation[A-Za-z0-9]*\\.cpp$")
#SOURCE_GROUP("Sources\\Sections" REGULAR_EXPRESSION "/Section[^Mesh][A-Za-z0-9]*\\.cpp$")
#SOURCE_GROUP("Sources\\SectionMeshes" REGULAR_EXPRESSION "/SectionMesh[A-Za-z0-9]*\\.cpp$")
#SOURCE_GROUP("Sources\\Textiles" REGULAR_EXPRESSION "/Textile[A-Za-z0-9]*\\.cpp$")
#SOURCE_GROUP("Sources\\YarnSections" REGULAR_EXPRESSION "/YarnSection[A-Za-z0-9]*\\.cpp$")

#SOURCE_GROUP("Headers\\Domains" REGULAR_EXPRESSION "/Domain[A-Za-z0-9]*\\.h$")
#SOURCE_GROUP("Headers\\Interpolations" REGULAR_EXPRESSION "/Interpolation[A-Za-z0-9]*\\.h$")
#SOURCE_GROUP("Headers\\Sections" REGULAR_EXPRESSION "/Section[^Mesh][A-Za-z0-9]*\\.h$")
#SOURCE_GROUP("Headers\\SectionMeshes" REGULAR_EXPRESSION "/SectionMesh[A-Za-z0-9]*\\.h$")
#SOURCE_GROUP("Headers\\Textiles" REGULAR_EXPRESSION "/Textile[A-Za-z0-9]*\\.h$")
#SOURCE_GROUP("Headers\\YarnSections" REGULAR_EXPRESSION "/YarnSection[A-Za-z0-9]*\\.h$")

#SET_TARGET_PROPERTIES(TexGenCore PROPERTIES DEFINE_SYMBOL EXPORT)
ADD_DEFINITIONS(-DEXPORT)

IF(WIN32)
TARGET_LINK_LIBRARIES(TexGenCore triangle triangle-api Octree TinyXML CSparse tetgenlib)
#${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/libsc
#${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/libp4est)
ELSE(WIN32)
TARGET_LINK_LIBRARIES(TexGenCore triangle triangle-api Octree TinyXML CSparse tetgenlib)
ENDIF(WIN32)

#TARGET_INCLUDE_DIRECTORIES(TexGenCore PUBLIC ../OctreeRefinement/include)

IF(UNIX)
TARGET_LINK_LIBRARIES(TexGenCore triangle triangle-api Octree TinyXML CSparse tetgenlib)
#${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/libp4est-2.0.so
#${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/libp4est.so
#${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/libsc-2.0.so
#${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/libsc.so)
ENDIF(UNIX)

INCLUDE(../Python/FindPythonSitePackages.cmake)
IF(WIN32)
	INSTALL(TARGETS TexGenCore
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
	INSTALL(TARGETS TexGenCore
	RUNTIME DESTINATION ${PYTHON_SITEPACKAGES_DIR}/TexGen)
	INSTALL(TARGETS TexGenCore
	RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../Installer)
ELSE(WIN32)
	IF(BUILD_SHARED)
		INSTALL(TARGETS TexGenCore
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
	ELSE(BUILD_SHARED)
		INSTALL(TARGETS TexGenCore 
		ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
	ENDIF(BUILD_SHARED)
ENDIF(WIN32)
