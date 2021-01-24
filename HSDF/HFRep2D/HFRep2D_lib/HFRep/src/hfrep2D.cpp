#include "hfrep2D.h"
#include "timer.hpp"

#include <iostream>

#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>

namespace hfrep2D {

HFRepObj2D::HFRepObj2D( int res_x, int res_y)
{
    resX = res_x;
    resY = res_y;

    DT    = std::make_shared<DiscreteDistanceTransform>( resX, resY );
    inter = std::make_shared<inter2D>();
}

std::vector<float> HFRepObj2D::calculateHFRep2D(std::vector<float> *frep, std::vector<float> *ddt, int stepfunc, float st_slope, bool checkHFRep)
{
    DDT.clear();
    SDDT.clear();
    SmDDT.clear();

    prof::timer Time;
    Time.Start();
    if( ddt == nullptr )
    {
        DT.get()->caclulateDiscreteDistanceTransformVec( frep );
        SDDT = DT.get()->get_signed_DDT();
        DDT  = DT.get()->get_DDT();
    }
    else {
        DDT = *ddt;
    }
    Time.End("UDF generation: ");

    Time.resetIter();
    Time.Start();
    SmDDT = inter.get()->geoToolsResample2D(&DDT, hfrep2D::InterType::GEO_BICUBIC,
                                            glm::ivec2(resX, resY), glm::ivec2(resX, resY),
                                            glm::vec2(1, 1), true);
    Time.End("Smoothing UDF: ");

    generateHFRepObject( frep, stepfunc, st_slope );

    if(checkHFRep)
        checkHFrep( &HFRep, frep, "initial check" );

    return HFRep;
}

float HFRepObj2D::getStepFunctionVal( float frep_val, int function, float st_slope )
{
    if( function == HYPERBOLIC_TANGENT )
    {
        float result = std::tanh( st_slope * frep_val ); //100000.0
        return result;
    }
    else if ( function == HYPERBOLIC_SIGMOID )
    {
        float result = 2.0f/(1.0f+std::exp(-2.0f*frep_val/st_slope)) - 1.0f;
        return result;
    }
    else if ( function == ALGEBRAIC )
    {
        float result = frep_val / std::sqrt( st_slope + frep_val * frep_val );
        return result;
    }
    else if ( function == GUDERMANIAN )
    {
        float result = 0.642129f*std::atan( std::sinh( st_slope * frep_val ) ); //1000
        return result;
    }
    else
    {
        std::cerr << "ERROR: unknown option! [ get_step_function_val(.., <?> ) ]" << std::endl;
        return -1;
    }
}

void HFRepObj2D::generateHFRepObject( std::vector<float> *FRep, int step_function, float st_slope )
{
    HFRep.clear();
    for(size_t i = 0; i < static_cast<size_t>( resX*resY ); i++)
    {
        float hfrep_val = getStepFunctionVal( FRep->at(i), step_function, st_slope ) * SmDDT[i];
        HFRep.push_back(hfrep_val);
    }
}

void HFRepObj2D::checkHFrep( std::vector<float> *hfrep, std::vector<float> *frep, std::string hfrep_check_name )
{
    int frep_zeros    = 0;
    int hfrep_zeros   = 0;
    int dist_tr_zeros = 0;

    for( int i = 0; i < resX*resY; i++)
    {
        if( frep->at(i) == 0.0f )
            frep_zeros++;
        if( hfrep->at(i) == 0.0f )
            hfrep_zeros++;
        if( SmDDT[i] == 0.0f )
            dist_tr_zeros++;
    }

    std::cout << "This is < " << hfrep_check_name << "> checking." << std::endl;
    if(frep_zeros == hfrep_zeros)
    {
        std::cout << "HFrep does not have additional zeros!" << std::endl;
        std::cout << "FRep zeros: " << frep_zeros << "   HFRep zeros: " << hfrep_zeros << std::endl;
    }
    else
    {
        std::cout << "HFRep has additional zeros: " << hfrep_zeros - frep_zeros << std::endl;
        std::cout << "FRep zeros: " << frep_zeros << "   HFRep zeros: " <<
                     hfrep_zeros << "   DT zeros: " << dist_tr_zeros << std::endl;
    }
}

}//namespace hfrep2D
