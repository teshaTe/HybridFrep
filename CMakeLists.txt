cmake_minimum_required(VERSION 3.1)

project(hybridFRep)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS " -pthread -lstdc++ -lm ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

#################################_SET_OPTIONS_#################################
option(BUILD_2D_HFREP_SDF    "Enable to build 2D HFRep SDF lib"        ON)
option(BUILD_3D_HFREP_SDF    "Enable to build 3D HFRep SDF lib"        ON)
option(BUILD_2D_3D_HFREP_IDF "Enable to build 2D/3D HFRep IDF lib"     ON)
option(USE_OPENMP            "Enable for CPU multithreading"          OFF)
option(USE_MATPLOT_LIB_C++   "Enable Matplotlib"                      OFF)
#################################_DEFINE_OPTIONS_PROPS_###################################

if(USE_MATPLOT_LIB_C++)
    add_definitions(-DUSE_MATPLOTLIB)
endif()

if(USE_OPENMP)
    find_package(OpenMP REQUIRED)
    add_definitions(-DUSE_OPENMP)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "-fopenmp=libomp ${CMAKE_CXX_FLAGS}")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        set(CMAKE_CXX_FLAGS "-qopenmp ${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "-fopenmp ${CMAKE_CXX_FLAGS}")
    endif()
endif(USE_OPENMP)

#################################_SET_UP_ADDITIONAL_LIBS_#################################

find_package(SFML REQUIRED COMPONENTS graphics window system )
find_package(Threads      REQUIRED)
find_package(GLEW         REQUIRED)
find_package(OpenGL       REQUIRED)
find_package(Eigen3 3.3.9 REQUIRED)
find_package(OpenGL)

if(BUILD_2D_3D_HFREP_IDF)
    find_package(GTS)
    find_package(Python COMPONENTS Development NumPy)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB REQUIRED glib-2.0)
    include_directories(${GLIB_INCLUDE_DIRS})
endif(BUILD_2D_3D_HFRep_IDF)

#################################_SET_UP_INCLUDE_DIRS_####################################

#################################_SET_UP_INCLUDE_DIRS_####################################

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/ExternalLibs/DipLib/include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/ExternalLibs/DipLib/lib")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/ExternalLibs/GeoTools")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/ExternalLibs/GeoTools/lib")

if(BUILD_2D_HFREP_SDF)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep2D/HFRep2D_lib/HelperFunctions")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep2D/HFRep2D_lib/DF/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep2D/HFRep2D_lib/DataStructures/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep2D/HFRep2D_lib/HFRep/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep2D/HFRep2D_lib/Materials")
endif()

if(BUILD_3D_HFREP_SDF)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep3D/HFRep3D_lib/UDF/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep3D/HFRep3D_lib/HFRep/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep3D/HFRep3D_lib/WindowCamera/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep3D/HFRep3D_lib/Attributes/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep3D/HFRep3D_lib/OpenGL_api/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep3D/HFRep3D_lib/OpenGL_api/shaders")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HSDF/HFRep3D/HFRep3D_lib/HelperFunctions")
endif()

if(BUILD_2D_3D_HFREP_IDF)
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external/libIgl/lib")
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external/glad/lib")

    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external/libIgl/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external/imgui")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external/imgui/backends")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external/imgui/misc")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external/glad/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/external")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/HIDF/HFRepIDF/include")
    add_subdirectory(HIDF)
endif()

if(BUILD_2D_HFREP_SDF OR BUILD_3D_HFREP_SDF)
    add_subdirectory(HSDF)
endif()
