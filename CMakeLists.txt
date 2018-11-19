cmake_minimum_required(VERSION 3.1)

project(hybridFRep)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -pthread ${CMAKE_CXX_FLAGS}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_lib/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_lib/lib")


option(USE_DEBUG_INFO_DF       "Use it for debugging the Distance Fields code"  OFF)
option(USE_DEBUG_INFO_FREP     "Use it for debugging the FRep code"             OFF)

if(USE_DEBUG_INFO_DF)
    add_definitions( -DUSE_DEBUG_INFO_DF )
endif()

if(USE_DEBUG_INFO_FREP)
    add_definitions( -DUSE_DEBUG_INFO_FREP )
endif()

add_subdirectory(test_2D)
add_subdirectory(test_3D)
