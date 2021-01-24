project(HFRep3D_lib)

set(SRC_WIN_CAM  WindowCamera/src/SFMLwindow.cpp
                 WindowCamera/src/camera.cpp

                 WindowCamera/include/SFMLwindow.h
                 WindowCamera/include/camera.h)

set(SRC_UDF      UDF/src/vcvdt3D.cpp
                 UDF/src/FIM3D.cpp
                 UDF/src/interpolation3D.cpp

                 UDF/include/vcvdt3D.h
                 UDF/include/FIM3D.h
                 UDF/include/interpolation3D.h)

set(SRC_HFREP     HFRep/src/frep3D.cpp
                  HFRep/src/hfrep3D.cpp
                  HFRep/src/microstructures.cpp
                  HFRep/src/volumetricObj.cpp

                  HFRep/include/frep3D.h
                  HFRep/include/hfrep3D.h
                  HFRep/include/microstructures.h
                  HFRep/include/volumetricObj.h)

set(SRC_ATTRIBUTES Attributes/src/proceduralNoise.cpp
                   Attributes/src/volumetricAttribute.cpp

                   Attributes/include/proceduralNoise.h
                   Attributes/include/volumetricAttribute.h)

set(SRC_OPENGL_API  OpenGL_api/src/mesh.cpp
                    OpenGL_api/src/texture.cpp
                    OpenGL_api/src/skybox.cpp
                    OpenGL_api/src/shader.cpp
                    OpenGL_api/src/light.cpp

                    OpenGL_api/include/mesh.h
                    OpenGL_api/include/skybox.h
                    OpenGL_api/include/texture.h
                    OpenGL_api/include/shader.h
                    OpenGL_api/include/light.h)

set(SRC_SHADERS     OpenGL_api/shaders/FragHFRepShader.frag
                    OpenGL_api/shaders/FragSkyBoxShader.frag
                    OpenGL_api/shaders/VertHFRepShader.vert
                    OpenGL_api/shaders/VertSkyBoxShader.vert)

set(HDR_HELPER_FUNCTIONS HelperFunctions/timer.hpp)

add_library(hfrep3D_lib ${SRC_WIN_CAM} ${SRC_UDF} ${SRC_HFREP} ${SRC_ATTRIBUTES} ${SRC_OPENGL_API}
                        ${SRC_SHADERS} ${HDR_HELPER_FUNCTIONS})
target_link_libraries(hfrep3D_lib ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} gtmathematicsgpu
                                               sfml-graphics sfml-window sfml-system DIP)

if(USE_OPENMP)
    target_link_libraries(hfrep3D_lib gomp omp omptarget)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_api/shaders
     DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/HFRep3D/sample3D_sfml)

