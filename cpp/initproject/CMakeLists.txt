cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

project(initproject) # Name of the project

# importing environment variables
SET(PuMA_OS "$ENV{PuMA_OS}" CACHE BOOL
        "Detects OS." FORCE)
SET(PuMA_DIR  "$ENV{PuMA_DIR}" CACHE BOOL
        "Obtaining PuMA_DIR from environment." FORCE)
SET(CONDA_PREFIX "$ENV{CONDA_PREFIX}" CACHE BOOL
        "Get CONDA_PREFIX." FORCE)

# set cxx flags
if ("${PuMA_OS}" STREQUAL "Mac")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp --std=c++0x -Wno-format -Wno-literal-conversion -Wno-deprecated-register -Wno-return-type")
    add_definitions(-DMAC) # preprocessor directive
elseif ("${PuMA_OS}" STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp --std=c++0x")
    add_definitions(-DLINUX) # preprocessor directive
endif()

# include directories
include_directories(${PuMA_DIR}/install/include)
include_directories(${CONDA_PREFIX}/include)
include_directories(${CONDA_PREFIX}/include/eigen3/Eigen)

# dependencies
SET(DEP_LIBS "-L${PuMA_DIR}/install/lib -lPuMA -ldl -L${CONDA_PREFIX}/lib -ltiff -lfftw3_threads -lfftw3")
if ("${PuMA_OS}" STREQUAL "Mac")
    SET(DEP_LIBS "${DEP_LIBS} -lomp")
endif()

# compiling initproject executable
add_executable(initproject initproject.cpp)

# linking it to PuMA C++ library
target_link_libraries(initproject ${DEP_LIBS})
