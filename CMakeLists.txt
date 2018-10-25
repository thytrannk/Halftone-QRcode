cmake_minimum_required(VERSION 3.12)
project(project)

set(CMAKE_CXX_STANDARD 14)

set(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY libs/FreeImage)
find_path(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY FreeImage.h)
find_library(FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY freeimage)
include_directories(${FREEIMAGE_LIBRARY_AND_HEADER_DIRRECTORY})

set(QRENCODE_LIBRARY_AND_HEADER_DIRRECTORY libs/qrencode)
find_path(QRENCODE_LIBRARY_AND_HEADER_DIRRECTORY qrencode.h)
find_library(QRENCODE_LIBRARY_AND_HEADER_DIRRECTORY qrencode)
include_directories(qrencode)

set(PNG_LIBRARY_AND_HEADER_DIRRECTORY libs/png)
find_path(PNG_LIBRARY_AND_HEADER_DIRRECTORY png.h)
find_library(PNG_LIBRARY_AND_HEADER_DIRRECTORY png)
include_directories(png)

#set(ZXING_LIBRARY_AND_HEADER_DIRRECTORY libs/zxing-cpp)
#find_path(ZXING_LIBRARY_AND_HEADER_DIRRECTORY
#        BarcodeFormat.h           LuminanceSource.h
#        Binarizer.h               MultiFormatReader.h
#        BinaryBitmap.h            NotFoundException.h
#        ChecksumException.h       Reader.h
#        DecodeHints.h             ReaderException.h
#        Exception.h               Result.h
#        FormatException.h         ResultPoint.h
#        IllegalStateException.h   ResultPointCallback.h
#        InvertedLuminanceSource.h ZXing.h)
#find_library(ZXING_LIBRARY_AND_HEADER_DIRRECTORY zxing)
#include_directories(zxing)

message("system: ${CMAKE_SYSTEM_PREFIX_PATH}")
find_library(ZXING_LIBRARY zxing)
message("zxing: ${ZXING_LIBRARY}")
include_directories(zxing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -framework GLUT -framework OpenGL") # compiler version and compiler flags

add_executable(project
        project.cpp qrenc.cpp qrenc.h halftoneQR.cpp halftoneQR.h readQR.cpp readQR.h
        ImageReaderSource.cpp ImageReaderSource.h
        lodepng.cpp lodepng.h
        jpgd.cpp jpgd.h)

target_link_libraries(project freeimage qrencode png zxing)