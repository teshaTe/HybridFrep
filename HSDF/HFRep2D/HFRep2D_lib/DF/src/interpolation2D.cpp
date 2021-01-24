#include "interpolation2D.h"

#include "diplib/geometry.h"
#include "diplib/multithreading.h"
#include "Mathematics/IntpAkimaUniform2.h"
#include "Mathematics/IntpBicubic2.h"
#include "Mathematics/IntpBilinear2.h"
#include "Mathematics/IntpBSplineUniform.h"
#include "Mathematics/IntpThinPlateSpline2.h"

#include <iostream>

namespace hfrep2D {

std::vector<float> inter2D::dipLibResample2D(std::vector<float> *field, float zoom, InterType type, glm::ivec2 res)
{
    int numThreads = dip::GetNumberOfThreads();
    dip::SetNumberOfThreads(numThreads);

    const unsigned int resX = res.x, resY = res.y;
    dip::Image in(dip::UnsignedArray{resX, resY}, 1, dip::DT_SFLOAT);
    for(int y = 0; y < resY; y++)
        for(int x = 0; x < resX; x++)
            in.At(x,y) = field->at(x+y*resX);

    dip::String method;
    switch (type) {
        case InterType::DIP_LINEAR:
            method = dip::S::LINEAR;
            break;
        case InterType::DIP_BSPLINE:
            method = dip::S::BSPLINE;
            break;
        case InterType::DIP_NEAREST:
            method = dip::S::NEAREST;
            break;
        case InterType::DIP_LANCZOS2:
            method = dip::S::LANCZOS2;
            break;
        case InterType::DIP_LANCZOS3:
            method = dip::S::LANCZOS3;
            break;
        case InterType::DIP_LANCZOS4:
            method = dip::S::LANCZOS4;
            break;
        case InterType::DIP_LANCZOS6:
            method = dip::S::LANCZOS6;
            break;
        case InterType::DIP_LANCZOS8:
            method = dip::S::LANCZOS8;
            break;
        case InterType::DIP_CUBIC_THIRD_ORDER:
            method = dip::S::CUBIC_ORDER_3;
            break;
        case InterType::DIP_CUBIC_FOURTH_ORDER:
            method = dip::S::CUBIC_ORDER_4;
            break;
        default:
        {
            std::wcerr << "ERROR <dipLibResample2D(), InterType>: Unknown option!" << std::endl;
            std::wcerr << "Default option: DIP_CUBIC_FOURTH_ORDER. " << std::endl;
            method = dip::S::CUBIC_ORDER_4;
        }
    }

    dip::Image out;
    Resampling(in, out, dip::FloatArray{zoom}, dip::FloatArray{0.0},
               method, dip::StringArray{dip::S::ZERO_ORDER});

    std::vector<float> result;
    for(int y = 0; y < resY; y++)
        for(int x = 0; x < resX; x++)
            result.push_back(out.At<float>(x,y));

    return result;
}

std::vector<float> inter2D::geoToolsResample2D(std::vector<float> *field, InterType type,
                                               glm::ivec2 inRes, glm::ivec2 outRes,
                                               glm::vec2 step, bool precise)
{
    glm::vec2 stP(0, 0);
    if(type == InterType::GEO_AKIMA)
    {
        std::vector<float> result;
        gte::IntpAkimaUniform2<float> akimaSp(inRes.x, inRes.y, stP.x, step.x,
                                              stP.y, step.y, &field->at(0));
        for(int y = stP.y; y < outRes.y; y++)
            for(int x = stP.x; x < outRes.x; x++)
                result.push_back(akimaSp(x, y));
        return result;
    }
    else if(type == InterType::GEO_BICUBIC)
    {
        std::vector<float> result;
        gte::IntpBicubic2<float> bicubicSp(inRes.x, inRes.y, stP.x, step.x,
                                           stP.y, step.y, &field->at(0), precise);
        for(int y = stP.y; y < outRes.y; y++)
            for(int x = stP.x; x < outRes.x; x++)
                result.push_back(bicubicSp(x, y));
        return result;
    }
    else if(type == InterType::GEO_BSPLINE)
    {
        /*std::vector<float> result;
        gte::IntpBSplineUniform<float, float> bicubicSp(inRes.x, inRes.y, 0, step.x, 0, step.y, &field->at(0), precise);
        for(int y = 0; y < outRes.y; y++)
            for(int x = 0; x < outRes.x; x++)
                result.push_back(bicubicSp(x, y));
        return result;*/
    }
    else if(type == InterType::GEO_THINPLATE)
    {
        std::vector<float> result, vecX, vecY;
        for(int i = 0; i < inRes.x; i++)
        {
            vecX.push_back(i);
            vecY.push_back(i);
        }

        gte::IntpThinPlateSpline2<float> thinPlateSp(field->size(), &vecX[0], &vecY[0], &field->at(0), 0.0, false);

        for(int y = stP.y; y < outRes.y; y++)
            for(int x = stP.x; x < outRes.x; x++)
                result.push_back(thinPlateSp(x, y));
        return result;
    }
    else if(type == InterType::GEO_BILINEAR)
    {
        std::vector<float> result;
        gte::IntpBilinear2<float> bilinearSp(inRes.x, inRes.y, stP.x, step.x,
                                             stP.y, step.y, &field->at(0));
        for(int y = stP.y; y < outRes.y; y++)
            for(int x = stP.x; x < outRes.x; x++)
                result.push_back(bilinearSp(x, y));
        return result;
    }
    else
    {
        std::wcerr << "Warning <GeoToolsResample2D(), InterType>: Unknown option!" << std::endl;
        std::wcerr << "Returing input field!" << std::endl;
        return *field;
    }
}

}//namespace frep2D

