#include "interpolation3D.h"

#include "diplib/geometry.h"
#include "diplib/multithreading.h"
#include "Mathematics/IntpAkimaUniform3.h"
#include "Mathematics/IntpTricubic3.h"
#include "Mathematics/IntpTrilinear3.h"
#include "Mathematics/IntpBSplineUniform.h"

#include <iostream>

namespace hfrep3D {

std::vector<float> inter3D::dipLibResample3D(std::vector<float> *field, float zoom, InterType type, glm::ivec3 res)
{
    int numThreads = dip::GetNumberOfThreads();
    dip::SetNumberOfThreads(numThreads);

    const unsigned int resX = res.x, resY = res.y, resZ = res.z;
    dip::Image in(dip::UnsignedArray{resX, resY, resZ}, 1, dip::DT_SFLOAT);
    for(int z = 0; z < resZ; z++)
        for(int y = 0; y < resY; y++)
            for(int x = 0; x < resX; x++)
                in.At(x,y,z) = field->at(index3d(x,y,z, res));

    dip::Image out;
    dip::String method;
    switch (type) {
        case InterType::DIP_LINEAR:
            method = dip::S::LINEAR;
        case InterType::DIP_BSPLINE:
            method = dip::S::BSPLINE;
        case InterType::DIP_NEAREST:
            method = dip::S::NEAREST;
        case InterType::DIP_LANCZOS2:
            method = dip::S::LANCZOS2;
        case InterType::DIP_LANCZOS3:
            method = dip::S::LANCZOS3;
        case InterType::DIP_LANCZOS4:
            method = dip::S::LANCZOS4;
        case InterType::DIP_LANCZOS6:
            method = dip::S::LANCZOS6;
        case InterType::DIP_LANCZOS8:
            method = dip::S::LANCZOS8;
        case InterType::DIP_CUBIC_THIRD_ORDER:
            method = dip::S::CUBIC_ORDER_3;
        case InterType::DIP_CUBIC_FOURTH_ORDER:
            method = dip::S::CUBIC_ORDER_4;
        default:
        {
            std::wcerr << "ERROR <dipLibResample2D(), InterType>: Unknown option!" << std::endl;
            std::wcerr << "Default option: DIP_CUBIC_FOURTH_ORDER. " << std::endl;
            method = dip::S::CUBIC_ORDER_4;
        }
    }

    dip::Resampling(in, out, dip::FloatArray{zoom}, dip::FloatArray{0.0}, method, dip::StringArray{dip::S::ZERO_ORDER});

    std::vector<float> result;
    for(int z = 0; z < resZ; z++)
        for(int y = 0; y < resY; y++)
            for(int x = 0; x < resX; x++)
                result.push_back(out.At<float>(x,y,z));

    return result;
}

std::vector<float> inter3D::geoToolsResample3D(std::vector<float> *field, InterType type, glm::ivec3 inRes,
                                               glm::ivec3 outRes, glm::vec3 step, bool precise)
{
    if(type == InterType::GEO_AKIMA)
    {
        std::vector<float> result;
        gte::IntpAkimaUniform3<float> akimaSp(inRes.x, inRes.y, inRes.z,
                                              0, step.x, 0, step.y, 0, step.z,
                                              &field->at(0));
        for(int z = 0; z < outRes.z; z++)
            for(int y = 0; y < outRes.y; y++)
                for(int x = 0; x < outRes.x; x++)
                    result.push_back(akimaSp(x, y, z));
        return result;
    }
    else if(type == InterType::GEO_TRICUBIC)
    {
        std::vector<float> result;
        gte::IntpTricubic3<float> tricubicSp(inRes.x, inRes.y, inRes.z,
                                              0, step.x, 0, step.y, 0, step.z,
                                              &field->at(0), precise);
        for(int z = 0; z < outRes.z; z++)
            for(int y = 0; y < outRes.y; y++)
                for(int x = 0; x < outRes.x; x++)
                    result.push_back(tricubicSp(x, y, z));
        return result;
    }
    else if(type == InterType::GEO_TRILINEAR)
    {
        std::vector<float> result;
        gte::IntpTrilinear3<float> triLinear(inRes.x, inRes.y, inRes.z,
                                              0, step.x, 0, step.y, 0, step.z,
                                              &field->at(0));
        for(int z = 0; z < outRes.z; z++)
            for(int y = 0; y < outRes.y; y++)
                for(int x = 0; x < outRes.x; x++)
                    result.push_back(triLinear(x, y, z));
        return result;
    }
    else if(type == InterType::GEO_BSPLINE)
    {

    }
    else
    {
        std::wcerr << "Warning <GeoToolsResample2D(), InterType>: Unknown option!" << std::endl;
        std::wcerr << "Returing input field!" << std::endl;
        return *field;
    }
}


}//namespace frep2D

