#ifndef H_FREP_CLASS
#define H_FREP_CLASS

#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <functional>
#include <algorithm>

namespace hfrep2D {

class FRepObj2D
{
public:
    FRepObj2D(int resX, int resY, float scaleF);
    ~FRepObj2D(){ }

    //2D primitves
    float triangle(glm::vec2 pos, glm::vec2 cent, float a, float b, float c);
    float triangle2(glm::vec2 pos, glm::vec2 cent, float a, float b);

    float ellipticCylZ2D(glm::vec2 pos, glm::vec2 cent, float R, float a, float b);
    float ellipsoid2D(glm::vec2 pos, glm::vec2 cent, float R, float a, float b);
    float torusY2D(glm::vec2 pos, glm::vec2 cent, float R , float rev);
    float torusZ2D(glm::vec2 pos, glm::vec2 cent, float R, float rev);
    float rectangle(glm::vec2 pos, glm::vec2 cent, float w, float h);

    float circle(glm::vec2 pos, glm::vec2 cent, float R);
    float blobby2D(glm::vec2 pos, glm::vec2 cent, float R);

    float heart2D(glm::vec2 pos, glm::vec2 cent);
    float decocube2D(glm::vec2 pos, glm::vec2 cent);
    float suriken(glm::vec2 pos, glm::vec2 cent);
    float elf(glm::vec2 pos, glm::vec2 center);
    float bat(glm::vec2 pos, glm::vec2 center);
    float trebleClef(glm::vec2 pos, glm::vec2 center);

    std::vector<float> getFRep2D(std::function<float(glm::vec2)> fun);
    std::vector<float> getFRep2D(glm::vec2 cent, std::function<float(glm::vec2, glm::vec2)> fun);
    std::vector<float> getFRep2D(glm::vec2 cent, float R, std::function<float(glm::vec2, glm::vec2, float)> fun);
    std::vector<float> getFRep2D(glm::vec2 cent, float p1, float p2, std::function<float(glm::vec2, glm::vec2, float, float)> fun);
    std::vector<float> getFRep2D(glm::vec2 cent, float p1, float p2, float p3, std::function<float(glm::vec2, glm::vec2, float, float, float)> fun);
    std::vector<float> getFRep2D(std::vector<float> f1, std::vector<float> f2, float alpha, float m,
                                  std::function<float(float, float, float, float)> fun);
    std::vector<float> getFRep2D(std::vector<float> f1, std::vector<float> f2, float n,
                                  std::function<float(float, float, float)> fun);

    std::vector<float> getRotatedFrep2D(glm::vec2 cent, float w, float h,
                                         float angle, std::function<float (glm::vec2, glm::vec2, float, float)>);
    std::vector<float> getRotatedFrep2D(glm::vec2 cent, float a, float b, float c,
                                         float angle, std::function<float(glm::vec2, glm::vec2, float, float, float, float)>);
    //operations over 2D primitives
public:
    float bounded_blending(float f1, float f2, float a0, float a1, float a2, float a3 , float time, float alpha, float m);
    float blending_union(float f1, float f2, float a1, float a2, float alpha, float m);
    float blending_intersection(float f1, float f2, float a1, float a2, float alpha, float m);
    float blending_subtraction(float f1, float f2, float a1, float a2, float alpha, float m);

    float constOffset(float f, float offset);
    float constRadiusOffset(float f, float fOffset, float R, float x0, float y0);
    float union_function_R0(float f1, float f2, float n);

    glm::vec2 findZeroLevelSetInterval(std::vector<float> field , int numElemToAverage = 15);

    std::vector<float> scaleFunction(const std::vector<float> field, const float factor);

    float intersect_function(float f1, float f2, float alpha = 0, float m = 0);
    float union_function(float f1, float f2, float alpha = 0, float m = 0);
    float subtract_function(float f1, float f2, float alpha = 0, float m = 0);

    inline void setNewRes(int resX, int resY)  { resolutionX = resX; resolutionY = resY; }
    inline void setScalingFactor(float scaleF) { scale = scaleF; }

    inline float convertToUV(float val) { return val / resolutionX; }
    inline glm::vec2 convertToUV(glm::vec2 val) { return glm::vec2(val.x/resolutionX, val.y/resolutionY); }
    inline float scaleToNewRange(float v, float newMin=0, float oldMin = 0, float oldMax = 512)
                                { return ((v - oldMin)*(resolutionX-newMin)/(oldMax - oldMin))+newMin; }
private:
    glm::vec2 getRotatedCoords(glm::vec2 inCoords , const float angle);
    float blending(float f1, float f2, float a1, float a2);
    float blending(float f1, float f2, float a0, float a1, float a2, float a3, float time);

    int resolutionX, resolutionY;
    float scale;

    std::vector<float> frep;

};

} //namespace frep2D_object
#endif
