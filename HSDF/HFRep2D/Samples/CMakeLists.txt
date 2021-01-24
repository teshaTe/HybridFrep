#add_executable(quadTreeSample quadTree_sample.cpp)
#target_link_libraries(quadTreeSample hfrep2D_lib)

#add_executable(HeartWordSample sample_heart_word.cpp)
#target_link_libraries(HeartWordSample hfrep2D_lib)

add_executable(ShapeSample sample_shape.cpp)
target_link_libraries(ShapeSample hfrep2D_lib)

#add_executable(ZoomingHFRepSample sample_zooming_hfrep.cpp)
#target_link_libraries(ZoomingHFRepSample hfrep2D_lib)

add_executable(FractalSample sample_fractals.cpp)
target_link_libraries(FractalSample hfrep2D_lib)

add_executable(ShowContinuity sample_continuity.cpp)
target_link_libraries(ShowContinuity hfrep2D_lib)

add_executable(robot2D robot2DSample.cpp)
target_link_libraries(robot2D hfrep2D_lib)

add_executable(playground2D sample_diff_Functions.cpp)
target_link_libraries(playground2D hfrep2D_lib)
