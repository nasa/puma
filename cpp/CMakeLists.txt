# Minimum cmake
cmake_minimum_required(VERSION 3.0)

# project name
project(PuMA VERSION 3.0.0)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MACOSX_RPATH TRUE)

# set cxx flags
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp --std=c++0x -Wno-format -Wno-literal-conversion -Wno-deprecated-register -Wno-return-type")
    add_definitions(-DMAC) # preprocessor directive
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp --std=c++0x")
    add_definitions(-DLINUX) # preprocessor directive
endif()

# src/CMakeLists.txt: add source and header files
add_subdirectory(src)

# include directories
MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list src/*.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()
HEADER_DIRECTORIES(HEADERS_DIRS)
include_directories(${HEADERS_DIRS})
include_directories(${BUILD_PREFIX}/include)
include_directories(${BUILD_PREFIX}/include/eigen3/Eigen)

#########################
# Creating PuMA library #
#########################
add_library(PuMA SHARED ${SOURCES})

# public header files
set_target_properties(PuMA PROPERTIES PUBLIC_HEADER "${PUB_HEADERS}")

# Dependencies
SET(DEP_LIBS "-ldl -L${BUILD_PREFIX}/lib -ltiff -lfftw3_threads -lfftw3")
if(APPLE)
    SET(DEP_LIBS "${DEP_LIBS} -lomp")
endif()

# link dependency libraries to PuMA library
target_link_libraries(PuMA ${DEP_LIBS})

# Install PuMA library
install(TARGETS PuMA
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

############################
# Creating PuMA executable #
############################
add_executable(pumaX_main src/main.cpp)

# link PuMA and dependency libraries to pumaX_main executable
target_link_libraries(pumaX_main PuMA)
target_link_libraries(pumaX_main ${DEP_LIBS})

# Install pumaX_main executable
install(TARGETS pumaX_main
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

###################
# Examples option #
###################
# create examples executable
add_executable(pumaX_examples examples/examples.cpp)

# link PuMA and dependency libraries to pumaX_examples executable
target_link_libraries(pumaX_examples PuMA)
target_link_libraries(pumaX_examples ${DEP_LIBS})

# Install pumaX_examples
install(TARGETS pumaX_examples
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

##########################
# Unit-testing framework #
##########################
# test/CMakeLists.txt: add source and header files
add_subdirectory(test)

# create puma executable
add_executable(pumaX_testing ${SOURCES})

# link PuMA and dependency libraries to pumaX_testing executable
target_link_libraries(pumaX_testing PuMA)
target_link_libraries(pumaX_testing ${DEP_LIBS})

# public header files
set_target_properties(pumaX_testing PROPERTIES PUBLIC_HEADER "${PUB_HEADERS}")

# install pumaX_testing
install(TARGETS pumaX_testing
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
