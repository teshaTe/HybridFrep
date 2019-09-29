#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"

#include "render2D.h"
#include "frep2D.h"
#include "hfrep2D.h"

#include <iostream>
#include <iterator>
#include <fstream>

#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>
#include <functional>

int main(int argc, char** argv)
{
    hfrep2D::FRepObj2D frep(512, 512, 4.0f);
    std::vector<float> frep0 = frep.getFRep2D( hfrep2D::Point2D(200.0f, 250.0f), 50.0f, std::bind(&hfrep2D::FRepObj2D::circle, frep,
                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ));
    std::vector<float> frep1 = frep.getFRep2D( hfrep2D::Point2D(250.0f, 250.0f), 60.0f, std::bind(&hfrep2D::FRepObj2D::circle, frep,
                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ));
    std::vector<float> frep2 = frep.getFRep2D( hfrep2D::Point2D(300.0f, 250.0f), 40.0f, std::bind(&hfrep2D::FRepObj2D::circle, frep,
                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ));
    std::vector<float> frep3 = frep.getFRep2D( hfrep2D::Point2D(250.0f, 350.0f), 100.0, 50.0f, std::bind(&hfrep2D::FRepObj2D::rectangle, frep,
                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 ));
    std::vector<float> frep4 = frep.getFRep2D( hfrep2D::Point2D(250.0f, 100.0f), 100.0f, 100.0f, 100.0f, std::bind(&hfrep2D::FRepObj2D::triangle, frep,
                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5 ));

    float step_f_sl = 0.00001f;
    hfrep2D::HFRepObj2D hfrep(512, 512, 0.0);
    std::vector<float> hfrep0 = hfrep.calculateHFRep2D( &frep0, nullptr, HYPERBOLIC_SIGMOID, step_f_sl, true );
    std::vector<float> hfrep1 = hfrep.calculateHFRep2D( &frep1, nullptr, HYPERBOLIC_SIGMOID, step_f_sl, true );
    std::vector<float> hfrep2 = hfrep.calculateHFRep2D( &frep2, nullptr, HYPERBOLIC_SIGMOID, step_f_sl, true );
    std::vector<float> hfrep3 = hfrep.calculateHFRep2D( &frep3, nullptr, HYPERBOLIC_SIGMOID, step_f_sl, true );
    std::vector<float> hfrep4 = hfrep.calculateHFRep2D( &frep4, nullptr, HYPERBOLIC_SIGMOID, step_f_sl, true );


    auto start1 = std::chrono::system_clock::now();
    std::vector<float> union1, union2;
    for (size_t i = 0; i < 512*512 ; i++)
    {
        float hfrep_u1 = frep.union_function_R0( hfrep0[i], hfrep1[i], 2.0f );
        float hfrep_u2 = frep.union_function_R0( hfrep2[i], hfrep3[i], 2.0f );
        union1.push_back( frep.union_function_R0( hfrep_u1, hfrep_u2, 2.0f ));
    }


    for (size_t y = 0; y < 512 ; y++)
    {
        for (size_t x = 0; x < 512; x++)
        {
            hfrep2D::Point2D grF1_1 = hfrep2D::Point2D( union1[x+1+y*512]   - union1[x+y*512],
                                                      union1[x+(y+1)*512] - union1[x+y*512] );
            hfrep2D::Point2D grF2_1 = hfrep2D::Point2D( hfrep4[x+1+y*512]   - hfrep4[x+y*512],
                                                      hfrep4[x+(y+1)*512] - hfrep4[x+y*512] );
            union2.push_back( frep.union_function_R0( union1[x+y*512], hfrep4[x+y*512], grF1_1, grF2_1, 2.0f ));
        }
    }

    auto end1 = std::chrono::system_clock::now();
    std::chrono::duration<float> t_hfrep1 = end1 - start1;
    std::cout << "time for R0_union: " << t_hfrep1.count() << std::endl;

    auto start2 = std::chrono::system_clock::now();
    std::vector<float> union_R, union_hfrep;
    for (size_t i = 0; i < 512*512; i++)
    {
        float union_R1 = frep.union_function( hfrep0[i], hfrep1[i], 0.0f, 0.0f );
        float union_R2 = frep.union_function( hfrep2[i], hfrep3[i], 0.0f, 0.0f );
        float union_R3 = frep.union_function(  union_R1,  union_R2, 0.0f, 0.0f );
        union_R.push_back( frep.union_function( union_R3, hfrep4[i], 0.0f, 0.0f ));
    }

    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<float> t_hfrep2 = end2 - start2;
    std::cout << "time for R0_union: " << t_hfrep2.count() << std::endl;

    std::vector<float> union_max;
    for (size_t i = 0; i < 512*512; i++)
    {
        float union_max1 = std::max( hfrep0[i], hfrep1[i] );
        float union_max2 = std::max( hfrep2[i], hfrep3[i] );
        float union_max3 = std::max( union_max1,  union_max2 );
        union_max.push_back( std::max( union_max3, hfrep4[i] ));
    }

    hfrep2D::render2D draw;
    draw.drawRGB_isolines( &union2, 512, 512, 0.05f, "hfrep_uR0" );
    draw.drawRGB_isolines( &union_R, 512, 512, 0.001f, "frep_uR" );
    draw.drawRGB_isolines( &union_max, 512, 512, 0.05f, "hfrep_max" );

    frep.setScalingFactor( 3.0f );
    std::vector<float> heartFrep = frep.getFRep2D( hfrep2D::Point2D(250.0f, 250.0f),
                                                   std::bind(&hfrep2D::FRepObj2D::heart2D, frep, std::placeholders::_1, std::placeholders::_2 ));
    std::vector<float> heartHFRep = hfrep.calculateHFRep2D( &heartFrep, nullptr, HYPERBOLIC_SIGMOID, 0.000001f, false );
    std::vector<float> sddt = hfrep.getDDT();

    draw.drawRGB_isolines( &sddt, 512, 512, 0.04f, "ddt4");

    return 0;
}
