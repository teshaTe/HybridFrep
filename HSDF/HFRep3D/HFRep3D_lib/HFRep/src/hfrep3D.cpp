#include "hfrep3D.h"
#include "timer.hpp"

#include "diplib.h"
#include "diplib/multithreading.h"
#include "diplib/distance.h"
#include "diplib/geometry.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>


namespace hfrep3D {

HFRep3D::HFRep3D(int gridW, int gridH, int gridD)
{
    voxGrW = gridW;
    voxGrH = gridH;
    voxGrD = gridD;

    DT3D  = std::make_shared<distanceTransform3D>( gridW, gridH, gridD );
    FIM3D = std::make_shared<FIMsolver3D>( gridW, gridH, gridD );
    profiler = std::make_shared<prof::timer>();
    interpolField = std::make_shared<inter3D>();
}

void HFRep3D::calculateHFRep3D(std::vector<float> *frep3D, stepFunction stepFunc, float slope,
                               distMethood method, glm::vec2 *surfLims, bool checkRep )
{
    std::cout << "Stage HFRep generation." << std::endl;
    prof::timer time;
    time.Start();

    UDF.clear();
    SDF.clear();
    SmoothUDF.clear();
    FRepObj.clear();
    FRepObj = *frep3D;

    profiler.get()->Start();
    if( method == distMethood::VCVDT3D )
    {
        DT3D.get()->caclulateDiscreteDistanceTransform( &FRepObj );
        UDF  = DT3D.get()->getDDT();
        SDF = DT3D.get()->getSignedDDT();
    }
    else if( method == distMethood::VDT3D_FAST ||
             method == distMethood::VDT3D_TIES ||
             method == distMethood::VDT3D_BRUTEFORCE )
    {
        SDF = computeVDT(glm::ivec3(voxGrW, voxGrH, voxGrD), *frep3D, method);
        UDF.resize(SDF.size());
        std::transform(SDF.begin(),SDF.end(), UDF.begin(), static_cast<float(*)(float)>(&std::abs));
    }
    else
    {
        if(surfLims == nullptr)
        {
            std::cerr << "ERROR, <calculateHFRep3D()>: surfLims are not specidied! ABORT!" << std::endl;
            exit(-1);
        }
        FIM3D.get()->solveEikonalParallel_CPU( &FRepObj, surfLims->x, surfLims->y );
        UDF = FIM3D.get()->getDistanceFeld();
        std::cerr << "WARNING: FIM3D solver do not provide sign! SDF returned by HFRep3D will be empty!" << std::endl;
    }
    profiler.get()->End("HFRep3D: computing UDF: ");

    profiler.get()->resetIter();
    profiler.get()->Start();
    smoothDistanceField();
    profiler.get()->End("HFRep3D: smoothing UDF: ");

    profiler.get()->resetIter();
    profiler.get()->Start();
    generateHFRepObject( stepFunc, slope );
    profiler.get()->End("HFRep3D: object: ");

    if( checkRep )
        checkHFrep( method );

    time.End("HFRep stage took");
    std::cout << "\n HFRep stage is complete." << std::endl;
}

std::vector<float> HFRep3D::computeVDT(const glm::ivec3 res, std::vector<float> frep, distMethood method)
{
    const unsigned int resX = res.x, resY = res.y, resZ = res.z;
    dip::String methodDT;
    if(method == distMethood::VDT3D_FAST)
        methodDT = dip::S::FAST;
    else if(method == distMethood::VDT3D_TIES)
        methodDT = dip::S::TIES;
    else if(method == distMethood::VDT3D_TRUE)
    {
        std::wcerr << "WARNING: true option is not working with spherical objects! [bug in diplib]!" << std::endl;
        methodDT = dip::S::TRUE;
    }
    else
        methodDT = dip::S::BRUTE_FORCE;

    int numThr = dip::GetNumberOfThreads();
    dip::SetNumberOfThreads(numThr);

    dip::Image in1(dip::UnsignedArray{resX, resY, resZ}, 1, dip::DT_BIN);
    dip::Image in2(dip::UnsignedArray{resX, resY, resZ}, 1, dip::DT_BIN);
    in1.ResetPixelSize();
    in2.ResetPixelSize();

    for(int z = 0; z< resZ; z++)
        for(int y = 0; y < resY; y++)
            for(int x = 0; x < resX; x++)
            {
                if( frep[z*resX*resY + y*resX + x] >= 0.0f )
                {
                    in1.At(x,y,z) = 0;
                    in2.At(x,y,z) = 1;
                }
                else
                {
                    in1.At(x,y,z) = 1;
                    in2.At(x,y,z) = 0;
                }
            }

    dip::Image out1, out2;

    dip::VectorDistanceTransform(in1, out1, dip::S::OBJECT, methodDT);
    dip::VectorDistanceTransform(in2, out2, dip::S::OBJECT, methodDT);

    std::vector<float> ddt;
    for(int z = 0; z< resZ; z++)
        for(int y = 0; y < resY; y++)
            for(int x = 0; x < resX; x++)
            {
                float px1 = out1.At<float>(x,y,z)[0];
                float py1 = out1.At<float>(x,y,z)[1];
                float pz1 = out1.At<float>(x,y,z)[2];
                float px2 = out2.At<float>(x,y,z)[0];
                float py2 = out2.At<float>(x,y,z)[1];
                float pz2 = out2.At<float>(x,y,z)[2];
                float d1 = std::sqrt(px1*px1 + py1*py1 + pz1*pz1);
                float d2 = std::sqrt(px2*px2 + py2*py2 + pz2*pz2);
                ddt.push_back((d2-d1) / static_cast<float>(resX));
            }

    return ddt;
}

float HFRep3D::getStepFunVal(float frep_val, stepFunction stepFunc, float slope )
{
    switch(stepFunc)
    {
        case stepFunction::HYPERBOLIC_TANGENT:
            return std::tanh( slope * frep_val ); //100000.0
        case stepFunction::HYPERBOLIC_SIGMOID:
            return 2.0f/(1.0f+std::exp(-2.0f*frep_val/slope)) - 1.0f;
        case stepFunction::ALGEBRAIC:
            return frep_val / std::sqrt( slope + frep_val * frep_val );
        case stepFunction::GUDERMANIAN:
            return 0.642129f*std::atan( std::sinh( slope * frep_val ) ); //1000
        default:
        {
            std::cerr << "ERROR: unknown option! [ get_step_function_val(.., <?> ) ]" << std::endl;
            return -1;
        }
    }
}

void HFRep3D::generateHFRepObject(stepFunction stepFunc, float slope )
{
    HFRepObj.clear();

    for ( int z = 0; z < voxGrD; z++ )
    {
        for ( int y = 0; y < voxGrH; y++ )
        {
            for ( int x = 0; x < voxGrW; x++)
            {
                int i  = index( x, y, z );
                float val = getStepFunVal( FRepObj[i], stepFunc, slope ) * SmoothUDF[i]; //TODO: change back to smoothDDT after smoothing is implemented
                HFRepObj.push_back( val );
            }
        }
    }
}

void HFRep3D::smoothDistanceField()
{
    int numThr = dip::GetNumberOfThreads();
    dip::SetNumberOfThreads(numThr);

    const unsigned int resX = voxGrW, resY = voxGrH, resZ = voxGrD;
    dip::Image in(dip::UnsignedArray{resX, resY, resZ}, 1, dip::DT_SFLOAT);
    for(int z = 0; z < voxGrD; z++)
        for(int y = 0; y < voxGrH; y++)
            for(int x = 0; x < voxGrW; x++)
                in.At(x,y,z) = UDF[index(x,y,z)];

    dip::Image out;
    dip::Resampling(in, out, dip::FloatArray{1.0001f}, dip::FloatArray{0.0},
                    (dip::String)dip::S::CUBIC_ORDER_4, dip::StringArray{dip::S::ZERO_ORDER});

    for(int z = 0; z < voxGrD; z++)
        for(int y = 0; y < voxGrH; y++)
            for(int x = 0; x < voxGrW; x++)
                SmoothUDF.push_back(out.At<float>(x,y,z));
}

bool HFRep3D::checkHFrep( distMethood method )
{
    int frep_zeros    = 0;
    int hfrep_zeros   = 0;
    int dist_tr_zeros = 0;

    for( int i = 0; i < static_cast<int>( voxGrW*voxGrH*voxGrD ); i++ )
    {
        if( FRepObj[i] == 0.0f )
            frep_zeros++;
        if( HFRepObj[i] == 0.0f )
            hfrep_zeros++;
        if( method != distMethood::FIM3D )
        {
            if( SDF[i] == 0.0f )
                dist_tr_zeros++;
        }
    }

    std::cout << "Checking HFRep for additional zeroes." << std::endl;
    if(frep_zeros == hfrep_zeros)
    {
        std::cout << "HFrep does not have additional zeros!" << std::endl;
        std::cout << "FRep zeros: " << frep_zeros << "   HFRep zeros: " << hfrep_zeros << std::endl;
        return true;
    }
    else
    {
        std::cout << "HFRep has additional zeros: " << hfrep_zeros - frep_zeros << std::endl;
        std::cout << "FRep zeros: "  << frep_zeros    << "   HFRep zeros: " << hfrep_zeros <<
                     "   DT zeros: " << dist_tr_zeros << std::endl;
        return false;
    }
}

}//namespace function_rep_3D
