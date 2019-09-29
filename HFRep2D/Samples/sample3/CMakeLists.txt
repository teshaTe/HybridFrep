cmake_minimum_required(VERSION 3.1)

project(HFRep_2D)

add_executable( hfrep_sample3 sample3.cpp )
target_link_libraries(hfrep_sample3 opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core hfrep2D_lib )

