#include "include/hfrep2D.h"

#include <iostream>

#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>

namespace hfrep2D {

HFRepObj2D::HFRepObj2D( int res_x, int res_y, int ddt_sh)
{
    resolution_x = res_x;
    resolution_y = res_y;
    dist_sh      = ddt_sh;

    DT   = std::make_shared<DiscreteDistanceTransform>( resolution_x, resolution_y );
    modF = std::make_shared<ModifyField>();
}

std::vector<float> HFRepObj2D::calculateHFRep2D(std::vector<float> *frep, std::vector<float> *ddt, int stepfunc, float st_slope, bool checkHFRep)
{
    setNewRes( resolution_x+dist_sh, resolution_y+dist_sh );
    DDT.clear();
    SDDT.clear();
    SmDDT.clear();

    if( ddt == nullptr )
    {
        DT.get()->caclulateDiscreteDistanceTransformVec( frep );
        SDDT = DT.get()->get_signed_DDT();
        DDT  = DT.get()->get_DDT();
    }
    else {
        DDT = *ddt;
    }

    SmDDT = modF.get()->smooth_field( &DDT, resolution_x, resolution_y );
    generateHFRepObject( frep, stepfunc, st_slope );

    if(checkHFRep)
        checkHFrep( &HFRep, frep, "initial check" );

    return HFRep;
}

float HFRepObj2D::get_step_function_val( float frep_val, int function, float st_slope )
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
    for(size_t i = 0; i < static_cast<size_t>( resolution_x*resolution_y ); i++)
    {
        float hfrep_val = get_step_function_val( FRep->at(i), step_function, st_slope ) * SmDDT[i];
        HFRep.push_back(hfrep_val);
    }
}

void HFRepObj2D::checkHFrep( std::vector<float> *hfrep, std::vector<float> *frep, std::string hfrep_check_name )
{
    int frep_zeros    = 0;
    int hfrep_zeros   = 0;
    int dist_tr_zeros = 0;

    for( int i = 0; i < resolution_x*resolution_y; i++)
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
