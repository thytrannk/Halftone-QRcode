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

add_executable(project
        project.cpp halftoneQR.cpp halftoneQR.h
        halftone.cpp halftone.h
        lib/qrenc/qrenc.cpp lib/qrenc/qrenc.h
        lib/zxing/ImageReaderSource.cpp lib/zxing/ImageReaderSource.h
        lib/zxing/lodepng.cpp lib/zxing/lodepng.h
        lib/zxing/jpgd.cpp lib/zxing/jpgd.h
        lib/zxing/readQR.cpp lib/zxing/readQR.h)

target_link_libraries(project freeimage qrencode png zxing iconv)
#target_link_libraries(project freeimage qrencode png zxing)