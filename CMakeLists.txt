cmake_minimum_required(VERSION 3.1)

project(hybridFRep)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS " -pthread -lstdc++ -lm ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

#################################_SET_UP_ADDITIONAL_LIBS_#################################

find_package( SFML REQUIRED COMPONENTS graphics window system )
find_package( Threads REQUIRED)
find_package( OpenCV  REQUIRED)
find_package( Boost REQUIRED )
find_package( GLEW    REQUIRED)
find_package( glfw3   REQUIRED)
find_package( OpenGL  REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HFRep2D/HFRep2D_lib/HelperFunctions/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HFRep2D/HFRep2D_lib/DF/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HFRep2D/HFRep2D_lib/DataStructures/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HFRep2D/HFRep2D_lib/HFRep/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HFRep3D/HFRep3D_lib/include")

#################################_SET_OPTIONS_#################################
option(USE_OPENMP     "Enable for CPU multithreading"                 OFF)
option(USE_CUDA       "Enable for GPU multithreading"                 OFF)
option(USE_DEBUG_MODE "To unbind the cursor while running the sample" OFF)
#################################_DEFINE_OPTIONS_PROPS_###################################

if(USE_DEBUG_MODE)
    add_definitions( -DUSE_DEBUG_MODE )
endif()

if(USE_OPENMP)
    find_package( OpenMP REQUIRED)
    add_definitions( -DENABLE_OPENMP)
    add_definitions(-DUSE_OPENMP)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "-fopenmp=libomp ${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "-fopenmp ${CMAKE_CXX_FLAGS}")
    endif()
endif()

if(USE_CUDA)
    find_package(CUDA REQUIRED)
    add_definitions( -DENABLE_CUDA)
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(HFRep2D)
add_subdirectory(HFRep3D)
