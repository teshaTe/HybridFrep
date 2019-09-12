cmake_minimum_required(VERSION 3.1)

project(hybridFRep)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS " -pthread ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

#################################_SET_UP_ADDITIONAL_LIBS_#################################

find_package( Threads )
find_package( OpenCV )

find_package( GLEW )
find_package( glfw3 )
find_package( OpenGL )

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HFRep2D/HFRep2D_lib/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HFRep3D/HFRep3D_lib/include")

#################################_SET_OPTIONS_#################################
option(USE_OPENMP     "Use it for speeding up calc of DT in 3D"       OFF)
option(USE_DEBUG_MODE "To unbind the cursor while running the sample" OFF)

#################################_DEFINE_OPTIONS_PROPS_###################################

if(USE_DEBUG_MODE)
    add_definitions( -DUSE_DEBUG_MODE )
endif()

if(USE_OPENMP)
    find_package( OpenMP )
    add_definitions( -DUSE_OPENMP)
    set(CMAKE_CXX_FLAGS " -fopenmp ${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(HFRep2D)
add_subdirectory(HFRep3D)
