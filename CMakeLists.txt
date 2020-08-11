######## A simple cmakelists.txt file for OpenCV() #############  
cmake_minimum_required(VERSION 2.8)                         

PROJECT(rsdvscalib)

set (CMAKE_STANDAR)

FIND_PACKAGE( OpenCV REQUIRED )                            
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
#include_directories(${PROJECT_SOURCE_DIR})

set(WORKER_DEFS_DIR davis-dc-workers-defs)
include_directories(${WORKER_DEFS_DIR})

set(WORKERS_LIBS)
include(calib-worker.cmake)



aux_source_directory( . SRC_LIST)
set (CMAKE_SHARED_LINKER_FLAGS "-lcaer -caer")
set (CMAKE_SHARED_LINKER_FLAGS "-lpthread -pthread")
set (CMAKE_SHARED_LINKER_FLAGS "-lstdc++ -stdc++")

list(APPEND SRC_LIST  ${WORKER_DEFS_DIR}/workersconfig.cpp ${WORKER_DEFS_DIR}/sharedimage.cpp
     "main.cpp"  "cameracalibration.cpp"  "intrinsiccalibration.cpp" )

ADD_EXECUTABLE(rsdvscalib ${SRC_LIST})

TARGET_LINK_LIBRARIES (rsdvscalib ${OpenCV_LIBS} ${WORKERS_LIBS} caer pthread stdc++ )

