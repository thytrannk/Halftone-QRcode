cmake_minimum_required(VERSION 3.12)
project(project)

set(CMAKE_CXX_STANDARD 14)

find_library(FREEIMAGE_LIBRARY freeimage)
include_directories(freeimage)

find_library(QRENCODE_LIBRARY qrencode)
include_directories(qrencode)

find_library(PNG_LIBRARY png)
include_directories(png)

find_library(ZXING_LIBRARY zxing)
include_directories(zxing)

find_library(ICONV_LIBRARY iconv)
include_directories(iconv)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -framework GLUT -framework OpenGL") # compiler version and compiler flags

add_executable(project
        project.cpp qrenc.cpp qrenc.h halftoneQR.cpp halftoneQR.h readQR.cpp readQR.h
        ImageReaderSource.cpp ImageReaderSource.h
        lodepng.cpp lodepng.h
        jpgd.cpp jpgd.h)

target_link_libraries(project freeimage qrencode png zxing iconv)