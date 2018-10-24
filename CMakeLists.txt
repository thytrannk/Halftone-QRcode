cmake_minimum_required(VERSION 3.12)
project(project)

set(CMAKE_CXX_STANDARD 14)

set(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY libs/FreeImage)
find_path(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY FreeImage.h)
find_library(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY freeimage)
include_directories(${FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -framework GLUT -framework OpenGL") # compiler version and compiler flags

add_executable(project
        project.cpp)

target_link_libraries(project freeimage)