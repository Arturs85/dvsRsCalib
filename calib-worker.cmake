#      ______     ______   __        __         _             
#     |  _ \ \   / / ___|  \ \      / /__  _ __| | _____ _ __ 
#     | | | \ \ / /\___ \   \ \ /\ / / _ \| '__| |/ / _ \ '__|
#     | |_| |\ V /  ___) |   \ V  V / (_) | |  |   <  __/ |   
#     |____/  \_/  |____/     \_/\_/ \___/|_|  |_|\_\___|_|   
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
    ${WORKER_DEFS_DIR}/iworker.cpp
    ${WORKER_DEFS_DIR}/iworkerthread.cpp
)


# Compiling:
add_library(calib-worker STATIC ${SRC_LIST})

# Linking:
link_directories(${CAER_LIBRARY_DIR}/../lib)
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-lpthread -pthread -lcaer -caer")
target_link_libraries(calib-worker pthread caer)


# Exporting:
list(APPEND WORKERS_LIBS
    calib-worker
)


