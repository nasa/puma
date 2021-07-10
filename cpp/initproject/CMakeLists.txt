cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

project(initproject) # Name of the project

# set cxx flags
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp --std=c++0x -Wno-format -Wno-literal-conversion -Wno-deprecated-register -Wno-return-type")
    add_definitions(-DMAC) # preprocessor directive
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp --std=c++0x")
    add_definitions(-DLINUX) # preprocessor directive
endif()

# include directories
include_directories(${CONDA_PREFIX}/include)
include_directories(${CONDA_PREFIX}/include/eigen3/Eigen)

# dependencies
SET(DEP_LIBS "-ldl -L${CONDA_PREFIX}/lib -lPuMA -ltiff -lfftw3_threads -lfftw3")
if(APPLE)
    SET(DEP_LIBS "${DEP_LIBS} -lomp")
endif()

# compiling initproject executable
add_executable(initproject initproject.cpp)

# linking it to PuMA C++ library
target_link_libraries(initproject ${DEP_LIBS})
