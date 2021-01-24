project(sample3D_sfml)

find_package( GLEW   REQUIRED )
find_package( OpenGL REQUIRED )

SET( SRC_SHADERS shaders/VertSkyBoxShader.vert
                 shaders/FragSkyBoxShader.frag
                 shaders/VertHFRepShader.vert
                 shaders/FragHFRepShader.frag)

add_executable(hfrep3D_sample_sfml1 src/3d_sample_sfml1.cpp ${SRC_SHADERS} )
target_link_libraries(hfrep3D_sample_sfml1 ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} hfrep3D_lib)

add_executable(hfrep3D_sample_sfml2 src/3d_sample_sfml2.cpp ${SRC_SHADERS} )
target_link_libraries(hfrep3D_sample_sfml2 ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} hfrep3D_lib)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
