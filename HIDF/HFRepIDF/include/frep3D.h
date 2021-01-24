#ifndef H_FREP_3D_CLASS
#define H_FREP_3D_CLASS

#include <cmath>
#include <vector>
#include <functional>

#include <glm/glm.hpp>

namespace frep {

class FRepObj3D
{
public:
    //TODO: templates of two var types
    struct heterObj3D
    {
        std::vector<float> geom;
        std::pair<std::vector<std::vector<float>>, std::vector<std::vector<float>>> attr;
    };

    FRepObj3D(int resX, int resY, int resZ, float scaleF);
    ~FRepObj3D(){}

    //*****************************************************
    //primitives
    //*****************************************************
    float halfSphere(glm::f32vec3 pos, glm::f32vec3 center, float R);
    float plane(glm::f32vec3 pos, glm::f32vec3 p1, glm::f32vec3 p2, glm::f32vec3 p3);
    float sphere (glm::f32vec3 pos, glm::f32vec3 center, float R);
    float blobby (glm::f32vec3 pos, glm::f32vec3 center, float R);
    float torus(glm::f32vec3 pos, glm::f32vec3 center, float R, float r);
    float ellipsoid(glm::f32vec3 pos, glm::f32vec3 center, float R, float a, float b, float c);
    float superEllipsoid(glm::f32vec3 pos, glm::f32vec3 center, float R, float a, float b, float c, float s1, float s2);

    float cylinder(glm::f32vec3 pos, glm::f32vec3 center, float R, float h);
    float ellipticCylinder(glm::f32vec3 pos, glm::f32vec3 center, float R, float h, float a, float b);
    float cone(glm::f32vec3 pos, glm::f32vec3 center, float R, float h);
    float ellipticCone(glm::f32vec3 pos, glm::f32vec3 center, float h, float a, float b);
    float pyramid(glm::f32vec3 pos, glm::f32vec3 cen, glm::f32vec2 botRec, glm::f32vec2 topRec, float H);
    float trianglePryramid(glm::f32vec3 pos, glm::f32vec3 cen, float w, float h, float H);

    float box(glm::f32vec3 pos, glm::f32vec3 center, float w, float h, float d);
    float heart3D(glm::f32vec3 pos, glm::f32vec3 center);

    float fox(glm::f32vec3 pos, glm::f32vec3 center);

    //*****************************************************
    //operations over primitives and some other operations
    //*****************************************************

    std::vector<float> getFRep3D(glm::f32vec3 cent, std::function<float(glm::f32vec3, glm::f32vec3)> fun);
    std::vector<float> getFRep3D(glm::f32vec3 cent, float a, std::function<float(glm::f32vec3, glm::f32vec3, float)> fun);
    std::vector<float> getFRep3D(glm::f32vec3 cent, float a, float b, std::function<float(glm::f32vec3, glm::f32vec3, float, float)> fun);
    std::vector<float> getFRep3D(glm::f32vec3 cent, float a, float b, float c, std::function<float(glm::f32vec3, glm::f32vec3, float, float, float)> fun);
    std::vector<float> getFRep3D(glm::f32vec3 cent, float a, float b, float c, float d, std::function<float(glm::f32vec3, glm::f32vec3, float, float, float, float)> fun);
    std::vector<float> getFRep3D(glm::f32vec3 cent, float a, float b, float c, float d, float e, std::function<float(glm::f32vec3, glm::f32vec3, float, float, float, float, float)> fun);
    std::vector<float> getFRep3D(std::vector<float> f1, std::vector<float> f2, float alpha, float m,
                                  std::function<float(float, float, float, float)> fun);

    float intersect_function(float f1, float f2, float alpha = 0.0f, float m = 0.0f);
    float union_function(float f1, float f2, float alpha = 0.0f, float m = 0.0f);
    float subtract_function(float f1, float f2, float alpha = 0.0f, float m = 0.0f);

    float blending_union(float f1, float f2, float a0, float a1, float a2);
    float blending_intersection(float f1, float f2, float a0, float a1, float a2);
    float blending_subtraction(float f1, float f2, float a0, float a1, float a2);

    glm::vec2 findZeroLevelSetInterval(std::vector<float> field , int numElemToAverage = 20);
    glm::f32vec3 getRotatedCoordsX(glm::f32vec3 inCoords , const float angle);
    glm::f32vec3 getRotatedCoordsY(glm::f32vec3 inCoords , const float angle);
    glm::f32vec3 getRotatedCoordsZ(glm::f32vec3 inCoords , const float angle);

    inline void setNewResolution(int resX, int resY, int resZ) { resolutionX = resX;
                                                                   resolutionY = resY;
                                                                   resolutionZ = resZ; }
    inline void setNewScale(float scaleF) { scale = scaleF; }

private:    
    float scaleToNewRange(float v, float newMin=0, float oldMin = 0, float oldMax = 128);
    glm::f32vec2 scaleToNewRange(glm::f32vec2 v, float newMin=0, float oldMin = 0, float oldMax = 128);

    inline float convertToSTR(float val) { return val / resolutionX; }
    inline glm::f32vec2 convertToSTR(glm::f32vec2 val) { return glm::f32vec2(val.x/resolutionX, val.y/resolutionY); }
    inline glm::f32vec3 convertToSTR(glm::f32vec3 val) { return glm::f32vec3(val.x/resolutionX, val.y/resolutionY, val.z/resolutionZ); }

private:
    int resolutionX, resolutionY, resolutionZ;
    float scale;
    std::vector<float> frep;
};

} //namespace frep3D_object
#endif
