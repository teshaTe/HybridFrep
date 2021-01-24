#include "include/frep3D.h"
#include <algorithm>

namespace frep {

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

    float shX = scale * (str.x - c0.x);
    float shY = scale * (str.y - c0.y);
    float shZ = scale * (str.z - c0.z);
    float result = rad*rad - shX*shX - shY*shY - shZ*shZ;
    return result;
}

//TODO: check, something is not right here
float FRepObj3D::blobby(glm::f32vec3 pos, glm::f32vec3 center, float R)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));

    float shX = scale*(str.x - c0.x);
    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);
    float result =  -(shX*shX + shY*shY +shZ*shZ + std::sin(4.0f*shX) +
                      std::sin(4.0f*shY) + std::sin(4.0f*shZ) - rad);
    return result;
}

float FRepObj3D::cylinder(glm::f32vec3 pos, glm::f32vec3 center, float R, float h)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));
    float H   = convertToSTR(scaleToNewRange(h));

    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);
    float infCylX = rad*rad - shY*shY - shZ*shZ;
    float result = intersect_function(infCylX, c0.y + H/2.0f);
    return result;
}

float FRepObj3D::ellipticCylinder(glm::f32vec3 pos, glm::f32vec3 center,float R, float h, float a, float b)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));
    float H   = convertToSTR(scaleToNewRange(h));

    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);
    float infCylX = rad - shY*shY/(a*a) - shZ*shZ/(b*b);
    float result = intersect_function(infCylX, c0.y + H/2.0f);
    return result;
}

float FRepObj3D::cone(glm::f32vec3 pos, glm::f32vec3 center, float R, float h)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));
    float H   = convertToSTR(scaleToNewRange(h));

    float shX = scale*(str.x - c0.x);
    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);
    float cone_x = shX*shX - (shY*shY + shZ*shZ)/(rad*rad);
    float result = intersect_function(cone_x, c0.x + H/2.0f);
    return result;
}

float FRepObj3D::ellipticCone(glm::f32vec3 pos, glm::f32vec3 center, float h, float a, float b)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float H = convertToSTR(scaleToNewRange(h));

    float shX = scale*(str.x - c0.x);
    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);
    float ellipConeX = shX*shX/(H*H) - shY*shY/(a*a) - shZ*shZ/(b*b);
    return ellipConeX;
}

float FRepObj3D::pyramid(glm::f32vec3 pos, glm::f32vec3 cen, glm::f32vec2 botRec, glm::f32vec2 topRec, float H)
{
    glm::f32vec3 rotPos = getRotatedCoordsX(pos, -90.0);
    float wB = scaleToNewRange(botRec.x);
    float hB = scaleToNewRange(botRec.y);
    float wT = scaleToNewRange(topRec.x);
    float hT = scaleToNewRange(topRec.y);
    float h = scaleToNewRange(H);

    glm::f32vec3 recB_p1 = glm::f32vec3(cen.x - 0.5f*wB, cen.y - 0.5f*hB, cen.z - h/2.0f);
    glm::f32vec3 recB_p2 = glm::f32vec3(recB_p1.x + wB,  recB_p1.y,       cen.z - h/2.0f);
    glm::f32vec3 recB_p3 = glm::f32vec3(recB_p1.x + wB,  recB_p1.y + hB,  cen.z - h/2.0f);
    glm::f32vec3 recB_p4 = glm::f32vec3(recB_p1.x,       recB_p1.y + hB,  cen.z - h/2.0f);

    glm::f32vec3 recT_p1 = glm::f32vec3(cen.x - 0.5f*wT, cen.y - 0.5f*hT, cen.z + h/2.0f);
    glm::f32vec3 recT_p2 = glm::f32vec3(recT_p1.x + wT,  recT_p1.y,       cen.z + h/2.0f);
    glm::f32vec3 recT_p3 = glm::f32vec3(recT_p1.x + wT,  recT_p1.y + hT,  cen.z + h/2.0f);
    glm::f32vec3 recT_p4 = glm::f32vec3(recT_p1.x,       recT_p1.y + hT,  cen.z + h/2.0f);

    float plane1 = plane(rotPos, recB_p1, recB_p2, recT_p2); //bp1, bp2, tp2
    float plane2 = plane(rotPos, recB_p2, recB_p3, recT_p3); //bp2, bp3, tp3
    float plane3 = plane(rotPos, recB_p3, recB_p4, recT_p4); //bp3, bp4, tp4
    float plane4 = plane(rotPos, recB_p4, recB_p1, recT_p1); //bp4, bp1, tp1

    float maxW = std::max(wB, wT);
    float maxH = std::max(hB, hT);
    float Box = box(rotPos, cen, maxW, maxH, h);

    float op1 = subtract_function(Box, plane1);
    float op2 = subtract_function(op1, plane2);
    float op3 = subtract_function(op2, plane3);
    float op4 = subtract_function(op3, plane4);
    return op4;
}

float FRepObj3D::trianglePryramid(glm::f32vec3 pos, glm::f32vec3 cen, float w, float h, float H)
{
    float baseW = scaleToNewRange(w);
    float baseH = scaleToNewRange(h);
    float height = scaleToNewRange(H);

    glm::f32vec3 npos = getRotatedCoordsY(getRotatedCoordsX(getRotatedCoordsZ(pos, 180), 90), 90);

    float Box = box(npos, cen, w, h, H);
    glm::f32vec3 baseP1(cen.x - baseW/2.0f, cen.y - baseH/2.0f, cen.z-height/2.0f);
    glm::f32vec3 baseP2(baseP1.x + baseW, baseP1.y, baseP1.z);
    glm::f32vec3 baseP3(baseP1.x + baseW, baseP1.y + baseH, baseP1.z);

    glm::f32vec3 baseTP1 = glm::f32vec3(cen.x - 0.5f*baseW, cen.y - 0.5f*baseH, cen.z + height/2.0f);
    glm::f32vec3 baseTP3 = glm::f32vec3(baseTP1.x, baseTP1.y + baseH, baseTP1.z);

    float cutPlane = plane(npos, baseP2, baseTP1, baseTP3);

    float result = intersect_function(Box, cutPlane);
    return result;
}

float FRepObj3D::torus(glm::f32vec3 pos, glm::f32vec3 center, float R, float r)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rev   = convertToSTR(R);
    float rad   = convertToSTR(r);

    float shX = scale*(str.x - c0.x);
    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);
    float torus_x = rad*rad - shX*shX - shY*shY - shZ*shZ - rev*rev + 2.0f*rev*std::sqrt(shZ*shZ + shY*shY);
    return torus_x;
}

float FRepObj3D::box(glm::f32vec3 pos, glm::f32vec3 center, float w, float h, float d)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0   = convertToSTR(center);
    float width  = convertToSTR(scaleToNewRange(w));
    float height = convertToSTR(scaleToNewRange(h));
    float depth  = convertToSTR(scaleToNewRange(d));

    glm::f32vec3 shXYZ = scale * (str - c0);

    float side1 = intersect_function(width - std::abs(shXYZ.x), height - std::abs(shXYZ.y));
    float side2 = intersect_function(width - std::abs(shXYZ.x), depth - std::abs(shXYZ.z));
    float result = intersect_function(side1, side2);
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

float FRepObj3D::plane(glm::f32vec3 pos, glm::f32vec3 p1, glm::f32vec3 p2, glm::f32vec3 p3)
{
    glm::f32vec3 str = convertToSTR(pos)*scale;
    glm::f32vec3 P1 = convertToSTR(p1);
    glm::f32vec3 P2 = convertToSTR(p2);
    glm::f32vec3 P3 = convertToSTR(p3);

    glm::f32vec3 r1 = P2 - P1;
    glm::f32vec3 r2 = P3 - P1;
    glm::f32vec3 norm = glm::cross(r1, r2);

    float result = (norm.x * (str.x - P1.x) + norm.y * (str.y - P1.y) + norm.z * (str.z - P1.z));
    return result;
}

float FRepObj3D::superEllipsoid(glm::f32vec3 pos, glm::f32vec3 center, float R, float a, float b, float c, float s1, float s2)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);
    float rad = convertToSTR(scaleToNewRange(R));

    float shX = scale*(str.x - c0.x);
    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);

    float val0 = shX*shX/(s2*a*a) + shY*shY/(s2*b*b);
    float result = rad - std::pow(val0, s2/s1) - std::pow(shZ/c, 2.0f/s1);
    return result;
}

float FRepObj3D::heart3D(glm::f32vec3 pos, glm::f32vec3 center)
{
    glm::f32vec3 str = convertToSTR(pos);
    glm::f32vec3 c0  = convertToSTR(center);

    float shX = scale*(str.x - c0.x);
    float shY = scale*(str.y - c0.y);
    float shZ = scale*(str.z - c0.z);
    float result = - std::pow((2.0f*shZ*shZ + shY*shY + shX*shX - 1.0f), 3.0f) +
                            shY*shY * shZ*shZ*shZ/20.0f + shX*shX * shY*shY*shY;
    return result;
}

float FRepObj3D::fox(glm::f32vec3 pos, glm::f32vec3 center)
{
    float body0 = pyramid(getRotatedCoordsZ(pos, -95), center, glm::f32vec2(35, 25), glm::f32vec2(30, 20), 20.0f);
    float body1 = trianglePryramid(getRotatedCoordsZ(pos, 0), center+glm::f32vec3(-10/2
                                                                                  , 0, 0), 20, 10, 10);
    float body2 = union_function(body0, body1);
    return body1;
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

float FRepObj3D::scaleToNewRange(float v, float newMin, float oldMin, float oldMax)
{
    return ((v - oldMin)*(resolutionX-newMin)/(oldMax - oldMin))+newMin;
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
