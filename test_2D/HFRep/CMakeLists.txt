cmake_minimum_required(VERSION 3.1)

project(HFRep2D_lib)

set (SRC_HEADERS  include/ddt2D.h
                  include/HFRep.h
                  include/DrawField.h
                  include/ModifyField.h
    )

set (SRC_SOURCE   src/ddt2D.cpp
                  src/HFRep.cpp
                  src/DrawField.cpp
                  src/ModifyField.cpp
    )

add_library( hfrep2D_lib ${SRC_SOURCE} ${SRC_HEADERS} )

target_link_libraries(hfrep2D_lib opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core )

