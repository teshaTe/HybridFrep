cmake_minimum_required(VERSION 3.1)
project(HFRep2D_lib)

set(SRC_HELPER_FUN_HEADERS  HelperFunctions/timer.hpp)

set(SRC_MATERIALS_SOURCES  Materials/proceduralNoise.h
                           Materials/proceduralNoise.cpp)

set( SRC_SDF_HEADERS  DF/include/seddt2D.h
                      DF/include/FIM2D.h
                      DF/include/interpolation2D.h
    )

set( SRC_SDF_SOURCES  DF/src/seddt2D.cpp
                      DF/src/FIM2D.cpp
                      DF/src/interpolation2D.cpp
    )

set( SRC_HFREP_HEADERS  HFRep/include/frep2D.h
                        HFRep/include/hfrep2D.h
                        HFRep/include/fractals2D.h
                        HFRep/include/render2D.h
                        HFRep/include/microstructures2D.h
   )

set( SRC_HFREP_SOURCES  HFRep/src/frep2D.cpp
                        HFRep/src/hfrep2D.cpp
                        HFRep/src/fractals2D.cpp
                        HFRep/src/render2D.cpp
                        HFRep/src/microstructures2D.cpp
    )

add_library( hfrep2D_lib ${SRC_SDF_SOURCES} ${SRC_HFREP_SOURCES}
                         ${SRC_SDF_HEADERS} ${SRC_HFREP_HEADERS}
                         ${SRC_HELPER_FUN_HEADERS} ${SRC_MATERIALS_SOURCES})

target_link_libraries(hfrep2D_lib sfml-graphics sfml-window sfml-system DIP Eigen3::Eigen )
