project(HFRep3D_lib)

set( SRC_WIN_CAM  WindowCamera/src/SFMLwindow.cpp
                  WindowCamera/src/camera.cpp )

set( SRC_UDF      UDF/src/ddt3D.cpp
                  UDF/src/interpolation3D.cpp )

set( SRC_HFREP     HFRep/src/frep3D.cpp
                   HFRep/src/hfrep3D.cpp
                   HFRep/src/microstructures.cpp
                   HFRep/src/volumetricObj.cpp
                   HFRep/src/material.cpp)

set( SRC_OPENGL_API  OpenGL_api/src/mesh.cpp
                     OpenGL_api/src/texture.cpp
                     OpenGL_api/src/skybox.cpp
                     OpenGL_api/src/shader.cpp
                     OpenGL_api/src/light.cpp )


set( HDR_WIN_CAM  WindowCamera/include/SFMLwindow.h
                  WindowCamera/include/camera.h )

set( HDR_UDF  UDF/include/ddt3D.h
              UDF/include/interpolation3D.h)

set( HDR_HFREP   HFRep/include/frep3D.h
                 HFRep/include/hfrep3D.h
                 HFRep/include/microstructures.h
                 HFRep/include/volumetricObj.h
                 HFRep/include/material.h
                 HFRep/include/helperFunctions3D.hpp)

set( HDR_OPENGL_API  OpenGL_api/include/mesh.h
                     OpenGL_api/include/skybox.h
                     OpenGL_api/include/texture.h
                     OpenGL_api/include/shader.h
                     OpenGL_api/include/light.h)

add_library(hfrep3D_lib ${SRC_WIN_CAM} ${SRC_UDF} ${SRC_HFREP} ${SRC_OPENGL_API}
                        ${HDR_WIN_CAM} ${HDR_UDF} ${HDR_HFREP} ${HDR_OPENGL_API})
target_link_libraries(hfrep3D_lib ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} glfw sfml-graphics sfml-window sfml-system )
