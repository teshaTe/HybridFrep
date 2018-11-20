cmake_minimum_required(VERSION 3.1)

project(HFRep_2D)

set (SRC_HEADERS  include/DDT.h
                  include/FRep.h
                  include/DrawField.h
    )

set (SRC_SOURCE   src/DDT.cpp
                  src/FRep.cpp
                  src/DrawField.cpp
                  src/hfrep_2D_sample.cpp
    )

add_executable( hfrep_2d src/hfrep_2D_sample.cpp ${SRC_SOURCE} ${SRC_HEADERS})
target_link_libraries(hfrep_2d opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core)
