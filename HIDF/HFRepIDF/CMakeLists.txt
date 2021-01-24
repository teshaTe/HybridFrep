project(HFRep_interior_df_lib)
cmake_minimum_required(VERSION 3.0)

set(DMAPS_SRC external/dmaps/src/diffusion_map.cpp
              external/dmaps/src/distance_matrix.cpp
              external/dmaps/src/metrics.cpp)

set(DMAPS_HRC external/dmaps/include/diffusion_map.h
              external/dmaps/include/distance_matrix.h
              external/dmaps/include/metrics.h
              external/dmaps/include/types.h)

set(HEADERS   include/interiorDistanceField.h
              include/barycentricCoords.h
              include/OperationsOverFields.h
              include/proceduralNoise.h
              include/frep2D.h
              include/frep3D.h
              include/timer.hpp )

set(SOURCES   src/interiorDistanceField.cpp
              src/barycentricCoords.cpp
              src/proceduralNoise.cpp
              src/OperationsOverFields.cpp
              src/frep2D.cpp
              src/frep3D.cpp)

add_library(hybridIDF ${DMAPS_SRC} ${DMAPS_HRC} ${HEADERS} ${SOURCES})
target_include_directories(hybridIDF PRIVATE ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS})
target_link_libraries(hybridIDF Eigen3::Eigen ${OpenGL_LIBRARIES} ${GTS_LIBRARIES} ${GLIB_LDFLAGS}
                                Python::Python Python::NumPy glad triangle tetgen glfw igl_tetgen igl igl_opengl
                                igl_opengl_glfw igl_opengl_glfw_imgui imgui igl_triangle)

if(USE_OPENMP)
    target_link_libraries(hybridIDF gomp omp omptarget)
endif()
