#      ____       __       __        __         _
#     |  _ \     / /       \ \      / /__  _ __| | _____ _ __
#     | |       / /\        \ \ /\ / / _ \| '__| |/ / _ \ '__|
#     | |_       / |         \ V  V / (_) | |  |   <  __/ |
#     |____/   _/  |____/     \_/\_/ \___/|_|  |_|\_\___|_|
#
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)


# Headers:
include_directories(${CMAKE_CURRENT_LIST_DIR})
find_library(CAER_LIBRARY
    NAMES caer
    HINTS "${CMAKE_PREFIX_PATH}/lib"
)
if(NOT CAER_LIBRARY)
  message(FATAL_ERROR "dvs-worker: library 'caer' not found! Specify path using CMAKE_PREFIX_PATH.")
else()
  message(STATUS "dvs-worker: library 'caer' found at ${CAER_LIBRARY}.")
endif()
get_filename_component(CAER_LIBRARY_DIR ${CAER_LIBRARY} DIRECTORY)
include_directories(${CAER_LIBRARY_DIR}/../include)


# Sources:
set(SRC_LIST)
list(APPEND SRC_LIST
    ${CMAKE_CURRENT_LIST_DIR}/calibworker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cameracalibration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/intrinsiccalibration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/extrinsiccalibration.cpp    
${CMAKE_CURRENT_LIST_DIR}/socketserver.cpp
    ${WORKER_DEFS_DIR}/sharedimage.cpp
    ${WORKER_DEFS_DIR}/iworker.cpp
    ${WORKER_DEFS_DIR}/iworkerthread.cpp
    ${WORKER_DEFS_DIR}/workersconfig.cpp

)


# Compiling:
add_library(calib-worker STATIC ${SRC_LIST})

# Linking:
link_directories(${CAER_LIBRARY_DIR}/../lib)
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-lpthread -pthread -lcaer -caer -lstdc++ -stdc++")
target_link_libraries(calib-worker pthread caer)


# Exporting:
list(APPEND WORKERS_LIBS
    calib-worker
)


