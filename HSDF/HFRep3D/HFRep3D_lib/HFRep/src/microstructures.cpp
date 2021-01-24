#include "microstructures.h"
#include "HelperFunctions/timer.hpp"

#include <iostream>
#include <functional>

using namespace std::placeholders;

namespace hfrep3D {

microStruct::microStruct(int resX, int resY, int resZ): res_x(resX), res_y(resY), res_z(resZ)
{
    frep = std::make_shared<FRepObj3D>(resX, resY, resZ, 1.0f);
}

std::vector<float> microStruct::computeInfSquareSlabsX(glm::vec3 q, glm::vec3 p, glm::vec3 l)
{
    std::vector<float> slabs; slabs.resize(res_x*res_y*res_z);
    glm::vec3 s;
    clamp(&l);
    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                s.y = (sin(q.y * static_cast<float>(y) + p.y) - l.y);
                s.z = (sin(q.z * static_cast<float>(z) + p.z) - l.z);
                slabs[index3d(x, y, z)] = frep.get()->intersect_function( s.y, s.z );
            }
    return slabs;
}

std::vector<float> microStruct::computeInfSquareSlabsY(glm::vec3 q, glm::vec3 p, glm::vec3 l)
{
    std::vector<float> slabs; slabs.resize(res_x*res_y*res_z);
    glm::vec3 s;
    clamp(&l);
    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                s.x = sin(q.x * static_cast<float>(x) + p.x) - l.x;
                s.z = sin(q.z * static_cast<float>(z) + p.z) - l.z;
                slabs[index3d(x, y, z)] = frep.get()->intersect_function( s.x, s.z);
            }
    return slabs;
}

std::vector<float> microStruct::computeInfSquareSlabsZ(glm::vec3 q, glm::vec3 p, glm::vec3 l)
{
    std::vector<float> slabs; slabs.resize(res_x*res_y*res_z);
    glm::vec3 s;
    clamp(&l);
    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                s.x = sin(q.x * static_cast<float>(x) + p.x) - l.x;
                s.y = sin(q.y * static_cast<float>(y) + p.y) - l.y;
                slabs[index3d(x, y, z)] = frep.get()->intersect_function( s.y, s.y );
            }
    return slabs;
}

std::vector<float> microStruct::computeInfCircleEllipseSlabsX(glm::vec3 q, glm::vec3 p, glm::vec3 d)
{
    std::vector<float> slabs; slabs.resize(res_x*res_y*res_z);
    glm::vec3 s;
    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                s.y = sin(q.y * static_cast<float>(y) + p.y) - 1.0f;
                s.z = sin(q.z * static_cast<float>(z) + p.z) - 1.0f;
                slabs[index3d(x, y, z)] = frep.get()->intersect_function( s.y, s.z) + d.x;
            }
    return slabs;
}

std::vector<float> microStruct::computeInfCircleEllipseSlabsY(glm::vec3 q, glm::vec3 p, glm::vec3 d)
{
    std::vector<float> slabs; slabs.resize(res_x*res_y*res_z);
    glm::vec3 s;
    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                s.x = sin(q.x * static_cast<float>(x) + p.x) - 1.0f;
                s.z = sin(q.z * static_cast<float>(z) + p.z) - 1.0f;
                slabs[index3d(x, y, z)] = frep.get()->intersect_function( s.x, s.z) + d.y;
            }
    return slabs;
}

std::vector<float> microStruct::computeInfCircleEllipseSlabsZ(glm::vec3 q, glm::vec3 p, glm::vec3 d)
{
    std::vector<float> slabs; slabs.resize(res_x*res_y*res_z);
    glm::vec3 s;
    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                s.x = sin(q.x * static_cast<float>(x) + p.x) - 1.0f;
                s.y = sin(q.y * static_cast<float>(y) + p.y) - 1.0f;
                slabs[index3d(x, y, z)] = frep.get()->intersect_function( s.x, s.y ) + d.z;
            }
    return slabs;
}

std::vector<float> microStruct::computeSphereCellStructure(glm::vec3 iMin, glm::vec3 iMax, float r, replicFunction f)
{
    std::vector<float> sphere; sphere.resize(res_x*res_y*res_z);
    glm::f32vec3 xyzN;

    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                if( f == replicFunction::TriangleWave1 )
                {
                    xyzN.x = iMin.x + triangleWaveFunc1(static_cast<float>(x)/res_x)*(iMax.x - iMin.x);
                    xyzN.y = iMin.y + triangleWaveFunc1(static_cast<float>(y)/res_y)*(iMax.y - iMin.y);
                    xyzN.z = iMin.z + triangleWaveFunc1(static_cast<float>(z)/res_z)*(iMax.z - iMin.z);
                }
                else
                {
                    xyzN.x = iMin.x + triangleWaveFunc2(static_cast<float>(x))*(iMax.x - iMin.x);
                    xyzN.y = iMin.y + triangleWaveFunc2(static_cast<float>(y))*(iMax.y - iMin.y);
                    xyzN.z = iMin.z + triangleWaveFunc2(static_cast<float>(z))*(iMax.z - iMin.z);
                }

                sphere[index3d(x,y,z)] = frep.get()->sphere(xyzN, glm::vec3(0,0,0), r);
            }
    return sphere;
}

std::vector<float> microStruct::calcObjWithMStruct(std::vector<float> *frepObj, std::vector<float> *m_struct,
                                                   float offset, frepCut cutM, glm::f32vec3 *cutP)
{
    std::vector<float> result; result.resize(res_x*res_y*res_z);
    float p, cutFrep, cutOffsetFrep;

    for(int z = 0; z < res_z; z++)
        for(int y = 0; y < res_y; y++)
            for(int x = 0; x < res_x; x++)
            {
                float frep0 = frepObj->at(index3d(x,y,z));
                float offsetFrep = frep0 - offset/static_cast<float>(res_x);
                float shelledFrep = frep.get()->subtract_function( frep0, offsetFrep);

                if(cutM == frepCut::NONE)
                    cutFrep = shelledFrep;
                else if(cutM == frepCut::X && cutP != nullptr)
                {
                    p = static_cast<float>(x)/res_x - cutP->x/res_x;
                    cutFrep = frep.get()->intersect_function(shelledFrep, -p);
                    cutOffsetFrep = frep.get()->intersect_function(offsetFrep, -p);
                }
                else if(cutM == frepCut::Y && cutP != nullptr)
                {
                    p = static_cast<float>(y)/res_y - cutP->y/res_y;
                    cutFrep = frep.get()->intersect_function(shelledFrep, -p);
                    cutOffsetFrep = frep.get()->intersect_function(offsetFrep, -p);
                }
                else if(cutM == frepCut::Z && cutP != nullptr)
                {
                    p = static_cast<float>(z)/res_z - cutP->z/res_z;
                    cutFrep = frep.get()->intersect_function(shelledFrep, -p);
                    cutOffsetFrep = frep.get()->intersect_function(offsetFrep, -p);
                }
                else
                {
                    std::cerr << "ERROR<calcObjWithMStruct()>: option for the cutting plane is not specified!" << std::endl;
                    exit(-1);
                }
                //computing the microstructure that will fit in interior of the object
                float microStrObj = frep.get()->intersect_function( m_struct->at(index3d(x, y, z)), cutOffsetFrep);
                //obtaining the resulting field
                float obj = frep.get()->union_function( cutFrep, microStrObj );
                result[index3d(x, y, z)] = obj;
            }
    return result;
}

void microStruct::clamp(glm::vec3 *val)
{
    if( val->x > 1.0f )  val->x = 1.0f;
    if( val->y > 1.0f )  val->y = 1.0f;
    if( val->z > 1.0f )  val->z = 1.0f;

    if( val->x < -1.0f ) val->x = -1.0f;
    if( val->y < -1.0f ) val->y = -1.0f;
    if( val->z < -1.0f ) val->z = -1.0f;
}

void microStruct::clamp(float *v)
{
    if( *v > 1.0f ) *v = 1.0f;
    if( *v < 0 )    *v = 0.0f;
}


} // namespace hfrep3D
