# cmake_minimum_required(VERSION 3.5.0)

#project(Triangle
   # triangle 1.6 was the last "official" version; i.e. 1.7 should be good to go:
#   VERSION 1.7.0
#   )

option(TRIANGLE_ENABLE_ACUTE "Enable the aCute extension." ON)
option(BUILD_SHARED_LIBS "Build libraries as shared libraries." ON)
option(BUILD_TESTING "Enable unit tests." OFF)
if(BUILD_TESTING)
   enable_testing()
endif()
option(BUILD_EXAMPLES "Enable examples." OFF)
if(BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()




# triangle is the legacy interface:
add_library(triangle STATIC
   acute.c
   eps_writer.c
   predicates.c
   triangle.c
   triangle_io.c
   )
add_library(Triangle::triangle ALIAS triangle)

set_target_properties(triangle
   PROPERTIES
   CXX_VISIBILITY_PRESET hidden
   POSITION_INDEPENDENT_CODE TRUE
   )

target_link_libraries(triangle PRIVATE -lm)
target_include_directories(triangle
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>
	PRIVATE private
   )

if(NOT TRIANGLE_ENABLE_ACUTE)
   message(STATUS "Building without aCute...")
	target_compile_options(triangle PUBLIC -DNO_ACUTE)
else()
   message(STATUS "Building with aCute...")
endif()

set_property(TARGET triangle PROPERTY VERSION 1.7.0)
set_property(TARGET triangle PROPERTY SOVERSION 1)

# triangle-api is the "modern" interface:
add_library(triangle-api STATIC
   triangle_api.c
   triangle_helper.c
   )
add_library(Triangle::triangle-api ALIAS triangle-api)

target_link_libraries(triangle-api
	PUBLIC
 	Triangle::triangle
   )
target_include_directories(triangle-api
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
   $<INSTALL_INTERFACE:include>
	PRIVATE private
   )

set_property(TARGET triangle-api PROPERTY VERSION 1.7.0)
set_property(TARGET triangle-api PROPERTY SOVERSION 1)
set_property(TARGET triangle-api PROPERTY C_VISIBILITY_PRESET hidden)

configure_file(triangle_version.h.in ${CMAKE_BINARY_DIR}/triangle_version.h @ONLY)

include(GenerateExportHeader)
generate_export_header(triangle-api BASE_NAME triangle)

