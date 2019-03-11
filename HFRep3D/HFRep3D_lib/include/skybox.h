#ifndef H_SKYBOX_CLASS
#define H_SKYBOX_CLASS

#include <vector>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "mesh.h"

namespace hfrep3D {

class SkyBox{
public:
    SkyBox(std::vector<std::string> faceLocations, float resX, float resY, float resZ );
    ~SkyBox() { }

    void DrawSkybox( glm::mat4 viewMatrix, glm::mat4 projectionMatrix );

private:
    std::shared_ptr<mesh>   skyMesh;
    std::shared_ptr<shader> skyShader;

    GLuint textureID;
    GLuint uniformProjection, uniformView;
};

} // namespace hfrep3D
#endif
