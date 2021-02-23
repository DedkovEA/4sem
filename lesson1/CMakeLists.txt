cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")

project(gmshdemo)
add_executable(cube cube.cpp)
add_executable(circle circle.cpp)
add_executable(cylinder cylinder.cpp)

# gmsh укажем явно (потому что он был просто распакован руками)
add_library(gmsh SHARED IMPORTED)
set_target_properties(gmsh PROPERTIES
    IMPORTED_LOCATION "/home/eugene/labs/gmsh/gmsh-4.7.1-Linux64-sdk/lib/libgmsh.so"
    INTERFACE_INCLUDE_DIRECTORIES "/home/eugene/labs/gmsh/gmsh-4.7.1-Linux64-sdk/include"
)
set(GMSH_LIBRARY gmsh)

# Укажем библиотеки для линковки
target_link_libraries(cube ${GMSH_LIBRARY})
target_link_libraries(circle ${GMSH_LIBRARY})
target_link_libraries(cylinder ${GMSH_LIBRARY})
