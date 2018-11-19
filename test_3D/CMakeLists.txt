cmake_minimum_required(VERSION 3.1)

project(HFRep_3D)

set (SRC_HEADERS  include/DDT.h
       
    )

set (SRC_SOURCE   src/DDT.cpp
				  src/hfrep_3D_sample.cpp       
    )

add_executable( hfrep_3d src/hfrep_3D_sample.cpp ${SRC_SOURCE} ${SRC_HEADERS})
target_link_libraries(hfrep_3d opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core)
