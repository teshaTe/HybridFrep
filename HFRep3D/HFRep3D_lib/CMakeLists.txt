project(HFRep3D_lib)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

SET( SRC_SOURCE  src/window.cpp
                 src/frep3D.cpp
                 src/hfrep3D.cpp
                 src/microstructures.cpp
                 src/ddt3D.cpp
                 src/mesh.cpp
                 src/interpolation.cpp
                 src/texture.cpp
                 src/material.cpp
                 src/skybox.cpp
                 src/shader.cpp
                 src/light.cpp
                 src/camera.cpp
                 src/arcBallCamera.cpp
                 src/volumetricObj.cpp)

SET( SRC_HEADER  include/stb_image.h
                 include/window.h
                 include/frep3D.h
                 include/hfrep3D.h
                 include/microstructures.h
                 include/ddt3D.h
                 include/mesh.h
                 include/interpolation.h
                 include/skybox.h
                 include/texture.h
                 include/material.h
                 include/shader.h
                 include/light.h
                 include/volumetricObj.h
                 include/camera.h
                 include/arcBallCamera.h)

add_library(hfrep3D_lib ${SRC_SOURCE} ${SRC_HEADER})
target_link_libraries(hfrep3D_lib ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} glfw )
