cmake_minimum_required(VERSION 3.1)

project(HFRep_2D)

set (SRC_HEADERS  include/DDT.h
                  include/HFRep.h
                  include/DrawField.h
                  include/ModifyField.h
    )

add_executable( hfrep_sample2 sample2.cpp ${SRC_HEADERS})

target_link_libraries( hfrep_sample2 opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core hfrep_2d )

