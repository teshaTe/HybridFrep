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

    inline void useDirectionalLight( light *dirLight ) { volShader.get()->setDirectionalLight( dirLight ); }
    inline void useMaterial( material *mat ) { mat->useSimpleMaterial( volShader.get()->getSpecularIntesityLocation(),
                                                                       volShader.get()->getSpecularPowerLocation(),
                                                                       volShader.get()->getFresnelCoeffLocation()); }

    void createVolumetricObj(std::vector<float> field, float width, float height, float depth );
    void renderVolumetricObj(glm::mat4 viewMatrix, glm::mat4 projectionMatrix , glm::vec3 cameraPos, light *dirLight);
    void clearVolumetricObj();

    void genGradientNormals(std::vector<float> *field, float gradSt, int width, int height, int depth );
    void useGradientNormals();

    void sliceVolumetricObj();

private:
    GLuint uniformProjection, uniformView, uniformEyePosition,
           uniformModel, uniformSampler3D, uniformHFRepMinMax;
    GLuint textureFieldID;

    float HFRepMin, HFRepMax;

    float minHitD, maxTraceD;
    int stepNum;

    float aabb_w, aabb_h, aabb_d;

    std::shared_ptr<mesh>   volMesh;
    std::shared_ptr<shader> volShader;

    void checkError(int number);
};

}//namespace volumetric_object
#endif
