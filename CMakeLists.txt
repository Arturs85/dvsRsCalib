######## A simple cmakelists.txt file for OpenCV() #############  
cmake_minimum_required(VERSION 2.8)                         

PROJECT(rsdvscalib)

set (CMAKE_STANDAR)

FIND_PACKAGE( OpenCV REQUIRED )                            
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
#include_directories(${PROJECT_SOURCE_DIR})

aux_source_directory( . SRC_LIST)
set (CMAKE_SHARED_LINKER_FLAGS "-lcaer -caer")
set (CMAKE_SHARED_LINKER_FLAGS "-lpthread -pthread")
set (CMAKE_SHARED_LINKER_FLAGS "-lstdc++ -stdc++")

list(APPEND SRC_LIST "main.cpp"  "cameracalibration.cpp"  )

ADD_EXECUTABLE(rsdvscalib ${SRC_LIST})

TARGET_LINK_LIBRARIES (rsdvscalib ${OpenCV_LIBS} caer pthread stdc++ )

