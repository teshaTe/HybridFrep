project(sample3D_2)

find_package( GLEW   REQUIRED )
find_package( glfw3  REQUIRED )
find_package( OpenGL REQUIRED )

SET( SRC_SHADERS shaders/VertSkyBoxShader.vert
                 shaders/FragSkyBoxShader.frag
                 shaders/VertHFRepShader.vert
                 shaders/FragHFRepShader.frag)

add_executable(hfrep3D_sample1 src/3d_sample.cpp ${SRC_SHADERS} )
target_link_libraries(hfrep3D_sample1 ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} glfw hfrep3D_lib)

file( COPY ${CMAKE_SOURCE_DIR}/HFRep3D/sample3D_1/shaders  DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file( COPY ${CMAKE_SOURCE_DIR}/HFRep3D/sample3D_1/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
