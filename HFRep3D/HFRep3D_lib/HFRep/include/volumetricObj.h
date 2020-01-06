#ifndef H_VOLUMETRIC_OBJECT_CLASS
#define H_VOLUMETRIC_OBJECT_CLASS

#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "shader.h"
#include "light.h"
#include "material.h"

namespace hfrep3D {

class volumeOBJ
{
public:
    volumeOBJ();
    ~volumeOBJ();

    inline void setRayMarchingParams( float minHitDist, float maxTraceDist, int stepNumber )
                                      { minHitD = minHitDist; maxTraceD = maxTraceDist; stepNum = stepNumber; }
    inline void setBoundingBox( float width, float height, float depth )
                                { aabb_w = width; aabb_h = height; aabb_d = depth; }

    inline void useDirectionalLight( light *dirLight ) { volShader->setDirectionalLight( dirLight ); }
    inline void useMaterial( material *mat ) { }
    inline void setSkyboxTextureID( GLuint texID ) { skyboxTexID = texID; }

    void createVolumetricObj(std::vector<float> field, float width, float height, float depth );
    void renderVolumetricObj(glm::mat4 viewMatrix, glm::mat4 projectionMatrix , glm::vec3 cameraPos);
    void clearVolumetricObj();

    void createGround( GLfloat width, GLfloat depth );
    void renderGround( glm::mat4 viewMatrix, glm::mat4 projectionMatrix , glm::vec3 cameraPos );
    void useSkyboxTex();

private:
    GLuint uniformProjection, uniformView, uniformEyePosition,
           uniformModel, uniformHFRep3D, uniformSkyboxSampler;

    GLuint textureFieldID, skyboxTexID;

    //variables for ray marching
    GLfloat minHitD, maxTraceD;
    GLint stepNum;

    //variables for axis aligned bounding box
    GLfloat aabb_w, aabb_h, aabb_d;

    GLfloat turnSpead, moveSpead;

    std::shared_ptr<mesh> volMesh, floorMesh;
    std::shared_ptr<shader> volShader;

    void checkError(int number);
    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max :( n < n_min ? n_min : n ); }
    inline int index3d( int x, int y, int z, int width, int height ) { return z*width*height + y*width + x; }
};

}//namespace volumetric_object
#endif
