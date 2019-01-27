cmake_minimum_required(VERSION 3.1)

project(HFRep_2D)

set (SRC_HEADERS  include/DDT.h
                  include/HFRep.h
                  include/DrawField.h
                  include/ModifyField.h
    )

set (SRC_SOURCE   src/DDT.cpp
                  src/HFRep.cpp
                  src/DrawField.cpp
                  src/ModifyField.cpp
    )

add_library( hfrep_2d ${SRC_SOURCE} ${SRC_HEADERS} )

target_link_libraries(hfrep_2d opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core )

