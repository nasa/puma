cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

project(example)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MACOSX_RPATH TRUE)

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp --std=c++0x -Wno-format -Wno-literal-conversion -Wno-deprecated-register -Wno-return-type")
    set(CMAKE_MACOSX_RPATH 1)

elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp --std=c++0x")
endif()

set(CMAKE_INSTALL_RPATH "${CONDA_PREFIX}/lib")

include_directories(${CONDA_PREFIX}/include)
include_directories(${CONDA_PREFIX}/include/eigen3/Eigen)

SET(DEP_LIBS "-ldl -L${CONDA_PREFIX}/lib -lPuMA -ltiff -lfftw3_threads -lfftw3")
if(APPLE)
    SET(DEP_LIBS "${DEP_LIBS} -lomp")
endif()

add_executable(straight_circle straight_circle.cpp)
target_link_libraries(straight_circle ${DEP_LIBS})
install(TARGETS straight_circle DESTINATION .)
