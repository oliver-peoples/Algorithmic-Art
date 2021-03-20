# CMakeList.txt : CMake project for FLIR Chameleon Camera Calibration, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.19)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

find_package( OpenCV REQUIRED )
include_directories(~/Hamiltonian-Mathematics)
include_directories(~/C-Geometry)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} source.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# TODO: Add tests and install targets if needed.