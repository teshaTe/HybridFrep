#include "frep3D.h"
#include "HelperFunctions/timer.hpp"

#include <algorithm>
#include <omp.h>

namespace hfrep3D {

FRepObj3D::FRepObj3D(int resX, int resY, int resZ, float scaleF)
{
    resolutionX = resX;
    resolutionY = resY;
    resolutionZ = resZ;
    scale      = scaleF;
}

float FRepObj3D::sphere(glm::f32vec3 pos, glm::f32vec3 center, float R)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));

    glm::vec3 shXYZ = scale*glm::vec3(str - c0);
    float result = rad*rad - shXYZ.x*shXYZ.x - shXYZ.y*shXYZ.y - shXYZ.z*shXYZ.z;
    return result;
}

//TODO: check, something is not right here
float FRepObj3D::blobby(glm::f32vec3 pos, glm::f32vec3 center, float R)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));

    glm::vec3 shXYZ = scale*glm::vec3(str - c0);
    float result =  -(shXYZ.x*shXYZ.x + shXYZ.y*shXYZ.y +shXYZ.z*shXYZ.z + std::sin(4.0f*shXYZ.x) +
                      std::sin(4.0f*shXYZ.y) + std::sin(4.0f*shXYZ.z) - rad);
    return result;
}

float FRepObj3D::cylinder(glm::f32vec3 pos, glm::f32vec3 center, float R, float h)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));
    float H   = convertToSTR(scaleToNewRange(h));

    glm::vec3 shXYZ = scale*glm::vec3(str - c0);
    float result = -(std::abs(std::sqrt(shXYZ.x*shXYZ.x + shXYZ.y*shXYZ.y) - shXYZ.z/H) +
                     std::abs(std::sqrt(shXYZ.x*shXYZ.x + shXYZ.y*shXYZ.y) + shXYZ.z/H) - rad*rad);
    return result;
}

float FRepObj3D::ellipticCylinder(glm::f32vec3 pos, glm::f32vec3 center,float R, float h, float a, float b)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));
    float H   = convertToSTR(scaleToNewRange(h));

    glm::vec3 shXYZ = scale*glm::vec3(str - c0);
    float result = -(std::abs(std::sqrt(shXYZ.x*shXYZ.x/(a*a) + shXYZ.y*shXYZ.y/(b*b)) - shXYZ.z/H) +
                     std::abs(std::sqrt(shXYZ.x*shXYZ.x/(a*a) + shXYZ.y*shXYZ.y/(b*b)) + shXYZ.z/H) - rad*rad);;
    return result;
}

float FRepObj3D::cone(glm::f32vec3 pos, glm::f32vec3 center, float R, float h)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));
    float H   = convertToSTR(scaleToNewRange(h));

    glm::vec3 shXYZ = scale*glm::vec3(str - c0);
    float result = -(std::abs(std::sqrt(shXYZ.x*shXYZ.x + shXYZ.y*shXYZ.y) + shXYZ.z/H) +
                     std::sqrt(shXYZ.x*shXYZ.x + shXYZ.y*shXYZ.y) - rad);
    return result;
}

float FRepObj3D::ellipticCone(glm::f32vec3 pos, glm::f32vec3 center, float h, float R, float a, float b)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float H   = convertToSTR(scaleToNewRange(h));
    float rad = convertToSTR(scaleToNewRange(R));

    glm::vec3 shXYZ = scale*glm::vec3(str - c0);
    float result = -(std::abs(std::sqrt(shXYZ.x*shXYZ.x/(a*a) + shXYZ.y*shXYZ.y/(b*b)) + shXYZ.z/H) +
                     std::sqrt(shXYZ.x*shXYZ.x/(a*a) + shXYZ.y*shXYZ.y/(b*b)) - rad);
    return result;
}

float FRepObj3D::squarePyramid(glm::f32vec3 pos, glm::f32vec3 center, float a, float h)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    a = convertToSTR(scaleToNewRange(a));
    glm::f32vec3 shXYZ = scale * (str - c0);
    h = scaleToNewRange(h, 0, 3.0, 0, resolutionX);

    float result = a - (std::abs(std::abs(shXYZ.x - shXYZ.y) + std::abs(shXYZ.x + shXYZ.y) + h*shXYZ.z)+
                        std::abs(shXYZ.x - shXYZ.y) + std::abs(shXYZ.x + shXYZ.y));
    return result;
}

float FRepObj3D::truncPyramid(glm::f32vec3 pos, glm::f32vec3 center, float a, float b, float h)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    a = scaleToNewRange(a);
    b = scaleToNewRange(b);
    h = scaleToNewRange(h);

    glm::f32vec3 shXYZ = scale * (str - c0);

    float H = a*h/(a+b);
    float pyramid = squarePyramid(pos, center, a, H);
    float result = subtract_function(pyramid, shXYZ.z - convertToSTR(h));
    return result;
}

float FRepObj3D::octahedron(glm::f32vec3 pos, glm::f32vec3 cen, float a, float h)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(cen);
    a = convertToSTR(scaleToNewRange(a));
    h = scaleToNewRange(h, 0, 10, 0, resolutionX);

    glm::f32vec3 shXYZ = scale * (str - c0);
    float result = a -(std::abs(std::abs(shXYZ.x - shXYZ.y) + std::abs(shXYZ.x + shXYZ.y) + h*std::abs(shXYZ.z)) +
                       std::abs(shXYZ.x - shXYZ.y) + std::abs(shXYZ.x + shXYZ.y));
    return result;
}

float FRepObj3D::torus(glm::f32vec3 pos, glm::f32vec3 center, float R, float r)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rev   = convertToSTR(R);
    float rad   = convertToSTR(r);

    glm::f32vec3 shXYZ = scale * (str - c0);
    float result = rad*rad - shXYZ.x*shXYZ.x - shXYZ.y*shXYZ.y - shXYZ.z*shXYZ.z - rev*rev +
                   2.0f*rev*std::sqrt(shXYZ.z*shXYZ.z + shXYZ.y*shXYZ.y);
    return result;
}

float FRepObj3D::box(glm::f32vec3 pos, glm::f32vec3 center, float w, float h, float d)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0   = convertToSTR(center);
    float width  = convertToSTR(scaleToNewRange(w));
    float height = convertToSTR(scaleToNewRange(h));
    float depth  = convertToSTR(scaleToNewRange(d));

    glm::f32vec3 shXYZ = scale * (str - c0);
    float side   = intersect_function(width - std::abs(shXYZ.x), height - std::abs(shXYZ.y));
    float result = intersect_function(side, depth - std::abs(shXYZ.z));
    return result;
}

float FRepObj3D::ellipsoid(glm::f32vec3 pos, glm::f32vec3 center, float R, float a, float b, float c)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));

    glm::f32vec3 shXYZ = scale * (str - c0);
    float result = rad - shXYZ.x*shXYZ.x/(a*a) - shXYZ.y*shXYZ.y/(b*b) - shXYZ.z*shXYZ.z/(c*c);
    return result;
}

float FRepObj3D::superEllipsoid(glm::f32vec3 pos, glm::f32vec3 center, float R, float a, float b, float c, float s1, float s2)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));

    glm::f32vec3 shXYZ = scale * (str - c0);
    float val0 = shXYZ.x*shXYZ.x/(s2*a*a) + shXYZ.y*shXYZ.y/(s2*b*b);
    float result = rad - std::pow(val0, s2/s1) - std::pow(shXYZ.z/c, 2.0f/s1);
    return result;
}

float FRepObj3D::heart3D(glm::f32vec3 pos, glm::f32vec3 center)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);

    glm::f32vec3 shXYZ = scale * (str - c0);
    float result = - std::pow((2.0f*shXYZ.z*shXYZ.z + shXYZ.y*shXYZ.y + shXYZ.x*shXYZ.x - 1.0f), 3.0f) +
                     shXYZ.y*shXYZ.y * shXYZ.z*shXYZ.z*shXYZ.z/20.0f + shXYZ.x*shXYZ.x * shXYZ.y*shXYZ.y*shXYZ.y;
    return result;
}

float FRepObj3D::BarthDecic(glm::f32vec3 pos, glm::f32vec3 center, float c)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);

    glm::f32vec3 shXYZ = scale * (str - c0);

    float p = (std::sqrt(5.0f) + 1.0f) / 2.0f;
    float a = 1.0f;

    float result = (c*(shXYZ.x*shXYZ.x - std::pow(p, 4)*shXYZ.y*shXYZ.y) * (shXYZ.y*shXYZ.y - std::pow(p, 4)*shXYZ.z*shXYZ.z) *
                    (shXYZ.z*shXYZ.z - std::pow(p,4)*shXYZ.x*shXYZ.x) * (std::pow(shXYZ.x, 4) + std::pow(shXYZ.y, 4) +
                    std::pow(shXYZ.z, 4) - 2*shXYZ.x*shXYZ.x*shXYZ.y*shXYZ.y - 2*shXYZ.x*shXYZ.x*shXYZ.z*shXYZ.z -
                    2*shXYZ.y*shXYZ.y*shXYZ.z*shXYZ.z) + a*(3.0 + 5.0*p)*std::pow(shXYZ.x*shXYZ.x + shXYZ.y*shXYZ.y +
                    shXYZ.z*shXYZ.z - a, 2) * std::pow(shXYZ.x*shXYZ.x + shXYZ.y*shXYZ.y + shXYZ.z*shXYZ.z - (2.0 - p)*a, 2));
    return result;
}

float FRepObj3D::flower(glm::f32vec3 pos, glm::f32vec3 center)
{
    scale = 70;
    float BarthDecicS = BarthDecic(pos, center, 1);
    scale = 1.0;
    float core = sphere(pos, center, 60);
    float result = intersect_function(BarthDecicS, core);
    return BarthDecicS;
}

std::vector<float> FRepObj3D::getShelledObj(std::vector<float> *frepObj, float offset, cut cutM, glm::f32vec3 *cutP)
{
    std::vector<float> result; result.resize(resolutionX*resolutionY*resolutionZ);
    float p, cutFrep, cutOffsetFrep;

    for(int z = 0; z < resolutionZ; z++)
        for(int y = 0; y < resolutionY; y++)
            for(int x = 0; x < resolutionX; x++)
            {
                float frep0 = frepObj->at(index3d(x,y,z));
                float offsetFrep = frep0 - offset/static_cast<float>(resolutionX);
                float shelledFrep = subtract_function( frep0, offsetFrep);

                if(cutM == cut::NONE)
                {
                    cutFrep = shelledFrep;
                    result.push_back(cutFrep);
                }
                else if(cutM == cut::X && cutP != nullptr)
                {
                    p = static_cast<float>(x)/resolutionX - cutP->x/resolutionX;
                    cutFrep = intersect_function(shelledFrep, -p);
                    cutOffsetFrep = intersect_function(offsetFrep, -p);
                    result.push_back(cutOffsetFrep);
                }
                else if(cutM == cut::Y && cutP != nullptr)
                {
                    p = static_cast<float>(y)/resolutionY - cutP->y/resolutionY;
                    cutFrep = intersect_function(shelledFrep, -p);
                    cutOffsetFrep = intersect_function(offsetFrep, -p);
                    result.push_back(cutOffsetFrep);
                }
                else if(cutM == cut::Z && cutP != nullptr)
                {
                    p = static_cast<float>(z)/resolutionZ - cutP->z/resolutionZ;
                    cutFrep = intersect_function(shelledFrep, -p);
                    cutOffsetFrep = intersect_function(offsetFrep, -p);
                    result.push_back(cutOffsetFrep);
                }
                else
                {
                    std::cerr << "ERROR<calcObjWithMStruct()>: option for the cutting plane is not specified!" << std::endl;
                    exit(-1);
                }
            }

    return result;
}

float FRepObj3D::blending_union(float f1, float f2, float a0, float a1, float a2)
{
    float f1f2  = union_function(f1, f2);
    float blFun = a0 / (1.0f + (f1/a1)*(f1/a1) + (f2/a2)*(f1/a1));
    float result = f1f2 + blFun;
    return result;
}

float FRepObj3D::blending_intersection(float f1, float f2, float a0, float a1, float a2)
{
    float f1f2  = intersect_function(f1, f2);
    float blFun = a0 / (1.0f + (f1/a1)*(f1/a1) + (f2/a2)*(f1/a1));
    float result = f1f2 + blFun;
    return result;
}

float FRepObj3D::blending_subtraction(float f1, float f2, float a0, float a1, float a2)
{
    float f1f2  = subtract_function(f1, f2);
    float blFun = a0 / (1.0f + (f1/a1)*(f1/a1) + (f2/a2)*(f1/a1));
    float result = f1f2 + blFun;
    return result;
}

glm::vec2 FRepObj3D::findZeroLevelSetInterval(std::vector<float> field, int numElemToAverage)
{
    std::vector<float> posVals, negVals;

    for(size_t i = 0; i < field.size(); i++)
    {
        if(field[i] >= 0)
            posVals.push_back(field[i]);
    }

    std::vector<float> minPosV;
    for(int j = 0; j < numElemToAverage; j++)
    {
        std::vector<float>::iterator minPos = std::min_element(posVals.begin(), posVals.end());
        minPosV.push_back(*minPos);
        posVals.erase(std::remove(posVals.begin(), posVals.end(), *minPos), posVals.end());
    }

    float minPos, minPsum = 0;
    for(size_t i = 0; i < minPosV.size(); i++)
        minPsum += minPosV[i];

    minPos = minPsum / static_cast<float>(minPosV.size());

    glm::vec2 result = glm::vec2(0.0f, minPos);
    return result;
}

glm::f32vec3 FRepObj3D::getRotatedCoordsZ(glm::f32vec3 inCoords, const float angle)
{
    float angle0 = static_cast<float>((angle/180.0f) * M_PI);

    float rotX = inCoords.x*cosf(angle0) + inCoords.y*sinf(angle0);
    float rotY = -inCoords.x*sinf(angle0) + inCoords.y*cosf(angle0);
    float rotZ = inCoords.z;
    return glm::f32vec3(rotX, rotY, rotZ);
}

glm::f32vec3 FRepObj3D::getRotatedCoordsY(glm::f32vec3 inCoords, const float angle)
{
    float angle0 = static_cast<float>((angle/180.0f) * M_PI);

    float rotX = inCoords.x*cosf(angle0) - inCoords.z*sinf(angle0);
    float rotY = inCoords.y;
    float rotZ = inCoords.x*sinf(angle0) + inCoords.z*cosf(angle0);
    return glm::f32vec3(rotX, rotY, rotZ);
}

glm::f32vec3 FRepObj3D::getRotatedCoordsX(glm::f32vec3 inCoords, const float angle)
{
    float angle0 = static_cast<float>((angle/180.0f) * M_PI);

    float rotX = inCoords.x;
    float rotY = inCoords.y*cosf(angle0) + inCoords.z*sinf(angle0);
    float rotZ = -inCoords.y*sinf(angle0) + inCoords.z*cosf(angle0);
    return glm::f32vec3(rotX, rotY, rotZ);
}

float FRepObj3D::scaleToNewRange(float v, float newMin, float newMax, float oldMin, float oldMax)
{
    if(newMax == 0)
        newMax = resolutionX;
    return ((v - oldMin)*(newMax-newMin)/(oldMax - oldMin))+newMin;
}

glm::f32vec2 FRepObj3D::scaleToNewRange(glm::f32vec2 v, float newMin, float oldMin, float oldMax)
{
    return glm::f32vec2(scaleToNewRange(v.x), scaleToNewRange(v.y));
}

std::vector<float> FRepObj3D::getFRep3D(glm::f32vec3 cent, std::function<float (glm::f32vec3, glm::f32vec3)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
        for (int y = 0; y < resolutionY; y++)
            for (int x = 0; x < resolutionX; x++)
                frep.push_back(fun(glm::f32vec3(x, y, z), cent));

    return frep;
}

std::vector<float> FRepObj3D::getFRep3D(glm::f32vec3 cent, float a, std::function<float (glm::f32vec3, glm::f32vec3, float)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
        for (int y = 0; y < resolutionY; y++)
            for (int x = 0; x < resolutionX; x++)
                frep.push_back(fun(glm::f32vec3(x, y, z), cent, a));

    return frep;
}

std::vector<float> FRepObj3D::getFRep3D(glm::f32vec3 cent, float a, float b, std::function<float (glm::f32vec3, glm::f32vec3, float, float)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
        for (int y = 0; y < resolutionY; y++)
            for (int x = 0; x < resolutionX; x++)
                frep.push_back(fun(glm::f32vec3(x, y, z), cent, a, b));
    return frep;
}

std::vector<float> FRepObj3D::getFRep3D(glm::f32vec3 cent, float a, float b, float c, std::function<float(glm::f32vec3, glm::f32vec3, float, float, float)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
        for (int y = 0; y < resolutionY; y++)
            for (int x = 0; x < resolutionX; x++)
                frep.push_back(fun(glm::f32vec3(x, y, z), cent, a, b, c));
    return frep;
}

std::vector<float> FRepObj3D::getFRep3D(glm::f32vec3 cent, float a, float b, float c, float d, std::function<float (glm::f32vec3, glm::f32vec3, float, float, float, float)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
        for (int y = 0; y < resolutionY; y++)
            for (int x = 0; x < resolutionX; x++)
                frep.push_back(fun(glm::f32vec3(x, y, z), cent, a, b, c, d));
    return frep;

}

std::vector<float> FRepObj3D::getFRep3D(glm::f32vec3 cent, float a, float b, float c, float d, float e, std::function<float (glm::f32vec3, glm::f32vec3, float, float, float, float, float)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
        for (int y = 0; y < resolutionY; y++)
            for (int x = 0; x < resolutionX; x++)
                frep.push_back(fun(glm::f32vec3(x, y, z), cent, a, b, c, d, e));
    return frep;
}

std::vector<float> FRepObj3D::getFRep3D(std::vector<float> f1, std::vector<float> f2, float alpha, float m, std::function<float (float, float, float, float)> fun)
{
    assert(f1.size() == f2.size());
    frep.clear();
    for (size_t i = 0; i < f1.size(); i++)
        frep.push_back(fun(f1[i], f2[i], alpha, m));
    return frep;
}

std::vector<float> FRepObj3D::getFRep3D(glm::f32vec3 cent, glm::f32vec2 a, glm::f32vec2 b, float c, std::function<float (glm::f32vec3, glm::f32vec3, glm::f32vec2, glm::f32vec2, float)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
        for (int y = 0; y < resolutionY; y++)
            for (int x = 0; x < resolutionX; x++)
                frep.push_back(fun(glm::f32vec3(x, y, z), cent, a, b, c));
    return frep;

}

float FRepObj3D::intersect_function(float f1, float f2, float alpha, float m)
{
    return (1.0f/(1.0f+alpha))*(f1 + f2 - std::sqrt(f1 * f1 + f2 * f2 - 2.0f*alpha*f1*f2)*
                                                         std::pow(f1*f1+f2*f2, m/2.0f));
}

float FRepObj3D::union_function(float f1, float f2, float alpha, float m)
{
    return (1.0f/(1.0f+alpha))*(f1 + f2 + std::sqrt(f1 * f1 + f2 * f2 - 2.0f*alpha*f1*f2)*
                                 std::pow(f1*f1+f2*f2, m/2.0f));
}

float FRepObj3D::subtract_function(float f1, float f2, float alpha, float m)
{
    return intersect_function(f1, -f2, alpha, m);
}

} //namespace frep3D_object
