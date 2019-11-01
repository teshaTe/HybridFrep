cmake_minimum_required(VERSION 3.1)
project(HFRep2D_lib)

set( SRC_HELPER_FUN_HEADERS HelperFunctions/include/timer.hpp )

set( SRC_DATA_STRUCTURES_HEADERS DataStructures/include/helperFunctions.hpp
                                 DataStructures/include/quadTree.h
                                 DataStructures/include/quadTreeRender.h
    )

set( SRC_DATA_STRUCTURES_SOURCES DataStructures/src/quadTree.cpp
                                 DataStructures/src/quadTreeRender.cpp
    )

set( SRC_DATA_STRUCTURES_HEADERS DataStructures/include/activeList.h
                                 DataStructures/include/helperFunctions.h
    )

set( SRC_SDF_HEADERS  DF/include/ddt2D.h
                      DF/include/diffusionSolver2D.h
                      DF/include/eikonalSolver2D.h
                      DF/include/interpolation2D.h
                      DF/include/interpolation.h
    )

set( SRC_HFREP_HEADERS  HFRep/include/frep2D.h
                        HFRep/include/hfrep2D.h
                        HFRep/include/render2D.h
                        HFRep/include/microstructures2D.h
   )

set( SRC_SDF_SOURCES  DF/src/ddt2D.cpp
                      DF/src/diffusionSolver2D.cpp
                      DF/src/eikonalSolver2D.cpp
                      DF/src/interpolation2D.cpp
                      DF/src/interpolation.cpp
    )

set( SRC_HFREP_SOURCES  HFRep/src/frep2D.cpp
                        HFRep/src/hfrep2D.cpp
                        HFRep/src/render2D.cpp
                        HFRep/src/microstructures2D.cpp
    )

add_library( hfrep2D_lib ${SRC_DATA_STRUCTURES_HEADERS} ${SRC_SDF_SOURCES} ${SRC_HFREP_SOURCES}
                         ${SRC_DATA_STRUCTURES_SOURCES} ${SRC_SDF_HEADERS} ${SRC_HFREP_HEADERS}
                         ${SRC_HELPER_FUN_HEADERS})

target_link_libraries(hfrep2D_lib opencv_imgcodecs opencv_highgui opencv_imgproc opencv_core sfml-graphics sfml-window sfml-system )

