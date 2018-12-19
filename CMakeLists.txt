cmake_minimum_required(VERSION 3.1)

project(hybridFRep)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(Helpers)
include(Packages)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -pthread ${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

#################################_SET_UP_ADDITIONAL_LIBS_#################################

find_package(Threads REQUIRED)
_add_package_CUDA()

enable_language(CUDA)
set(CUDA_SEPARABLE_COMPILATION ON)
set(CUDA_PROPAGATE_HOST_FLAGS OFF)

if (CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CUDA_64_BIT_DEVICE_CODE_DEFAULT ON)
endif()


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_lib/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_lib/lib")

#################################_SET_UP_ADDITIONAL_LIBS_#################################



#################################_SET_OPTIONS_#################################
option(USE_DEBUG_INFO_DF       "Use it for debugging the Distance Fields code"  OFF)
option(USE_DEBUG_INFO_FREP     "Use it for debugging the FRep code"             OFF)

option(WITH_CUDA       "Enable CUDA building system"       ON)
option(USE_DEBUG_PTX   "Enable CUDA debugging with NSight" OFF)
option(CUDA_compute_30 "computing capability of videocard" OFF)
option(CUDA_compute_35 "computing capability of videocard" OFF)
option(CUDA_compute_50 "computing capability of videocard" ON)

#################################_DEFINE_OPTIONS_PROPS_#################################

if(CUDA_compute_30)
    SET( CUDA_ARCH "compute_30" CACHE STRING "CUDA Architecture target")
    SET( CUDA_CODE "sm_30" CACHE STRING "CUDA Code target")
endif()

if(CUDA_compute_35)
    SET( CUDA_ARCH "compute_35" CACHE STRING "CUDA Architecture target")
    SET( CUDA_CODE "sm_35" CACHE STRING "CUDA Code target")
endif()

if(CUDA_compute_50)
    SET( CUDA_ARCH "compute_50" CACHE STRING "CUDA Architecture target")
    SET( CUDA_CODE "sm_50" CACHE STRING "CUDA Code target")
endif()

if(USE_DEBUG_INFO_DF)
    add_definitions( -DUSE_DEBUG_INFO_DF )
endif()

if(USE_DEBUG_INFO_FREP)
    add_definitions( -DUSE_DEBUG_INFO_FREP )
endif()

add_subdirectory(test_2D)
add_subdirectory(test_3D)
