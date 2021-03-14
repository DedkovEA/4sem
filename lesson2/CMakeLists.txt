cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "-std=c++11 -g")

project(vtkdemo)
add_executable(model model.cpp)

# Найдём VTK
find_package(VTK REQUIRED)
# ... и подключим её со всеми её компонентами её нативными средствами
include(${VTK_USE_FILE})


# gmsh укажем явно (потому что он был просто распакован руками)
add_library(gmsh SHARED IMPORTED)
set_target_properties(gmsh PROPERTIES
    IMPORTED_LOCATION "/home/eugene/labs/gmsh/gmsh-4.7.1-Linux64-sdk/lib/libgmsh.so"
    INTERFACE_INCLUDE_DIRECTORIES "/home/eugene/labs/gmsh/gmsh-4.7.1-Linux64-sdk/include"
)
set(GMSH_LIBRARY gmsh)

# Укажем библиотеки для линковки
target_link_libraries(model ${VTK_LIBRARIES})
target_link_libraries(model ${GMSH_LIBRARY})
