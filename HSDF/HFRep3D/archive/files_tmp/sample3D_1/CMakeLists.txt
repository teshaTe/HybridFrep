project(sample3D_1)

if(WITH_CUDA)
#####################################################################################
# Custom Build for CUDA files
#
file( GLOB PRJ_CUDA_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" kernels/*.cu kernels/*.cuh )
message ( STATUS "Located PRJ_CUDA kernels: ${PRJ_CUDA_FILES}" )

if ( NOT DEFINED CUDA_ARCH )
   SET( CUDA_ARCH "compute_30" CACHE STRING "CUDA Architecture target")
   SET( CUDA_CODE "sm_30" CACHE STRING "CUDA Code target")
endif ()

_COMPILEPTX ( SOURCES ${PRJ_CUDA_FILES} TARGET_PATH ${LIB_OUTPUT_PATH} GENERATED CUDA_PTX GENPATHS CUDA_PTX_PATHS INCLUDE
              "${CMAKE_CURRENT_SOURCE_DIR}/kernels" OPTIONS -DSIM_CODE -arch=${CUDA_ARCH} -code=${CUDA_CODE}
              --ptxas-options=-v -O3 --use_fast_math --maxrregcount=64 )

#####################################################################################
set( CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -O3 -gencode arch=${CUDA_ARCH},code=${CUDA_CODE} -rdc=true)

find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

SET( CUDA_SRC kernels/kernel.cu
              kernels/kernel.h
              kernels/device_funcs.cuh
              kernels/device_funcs.cu
              kernels/helper_cuda_math.h
   )

SET( SRC_SOURCE src/main.cpp )
SET( SRC_HEADER include/interactions.h )

cuda_add_executable(cuda_sample3D_1 src/main.cpp ${SRC_SOURCE} ${CUDA_SRC} ${SRC_HEADER})
target_link_libraries(cuda_sample3D_1 ${CUDA_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} )
set_property ( TARGET cuda_sample3D_1 APPEND PROPERTY DEPENDS ${PTX_SOURCES} )

endif(WITH_CUDA)

