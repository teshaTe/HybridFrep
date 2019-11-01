cmake_minimum_required(VERSION 3.1)

project(HFRep_2D)

add_executable( hfrep_quadTree_sample quadTree_sample.cpp )
target_link_libraries(hfrep_quadTree_sample hfrep2D_lib )

