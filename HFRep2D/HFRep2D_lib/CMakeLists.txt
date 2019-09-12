cmake_minimum_required(VERSION 3.1)

project(HFRep2D_lib)

set (SRC_HEADERS  include/ddt2D.h
                  include/frep2D.h
                  include/hfrep2D.h
                  include/render2D.h
                  include/interpolation2D.h
                  include/interpolation.h
                  include/microstructures2D.h
    )

set (SRC_SOURCE   src/ddt2D.cpp
                  src/frep2D.cpp
                  src/hfrep2D.cpp
                  src/render2D.cpp
                  src/interpolation2D.cpp
                  src/interpolation.cpp
                  src/microstructures2D.cpp
    )

add_library( hfrep2D_lib ${SRC_SOURCE} ${SRC_HEADERS} )

target_link_libraries(hfrep2D_lib opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core )

