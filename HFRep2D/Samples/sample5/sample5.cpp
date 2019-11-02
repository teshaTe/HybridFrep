#include <iostream>
#include <chrono>
#include <random>
#include <fstream>

#include "frep2D.h"
#include "ddt2D.h"
//#include "hfrep2D.h"
#include "eikonalSolver2D.h"
#include "interpolation.h"
#include "render2D.h"

#include <boost/math/interpolators/cubic_b_spline.hpp>

int main(int argc, char** argv)
{
    int resX = 512, resY = 512;
    int x = 250, y = 250;
    float a = 30.0f, b = 30.0f, r = 50.0;

    hfrep2D::FRepObj2D frep( resX, resY, 1.0f );
    auto circle = std::bind(&hfrep2D::FRepObj2D::circle, frep, std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3);
    auto rec = std::bind(&hfrep2D::FRepObj2D::rectangle, frep, std::placeholders::_1,
                         std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    auto key = std::bind(&hfrep2D::FRepObj2D::trebleClef, frep, std::placeholders::_1 );
    auto bat = std::bind(&hfrep2D::FRepObj2D::bat, frep, std::placeholders::_1);

    std::vector<float> frep1 = frep.getFRep2D( hfrep2D::Point2Df( x, y ), r, circle );
    std::vector<float> frep2 = frep.getFRep2D( hfrep2D::Point2Df( x, y ), a, b, rec );
    std::vector<float> frep3 = frep.getFRep2D( hfrep2D::Point2Df( x, y ), key );
    std::vector<float> frep4 = frep.getFRep2D( hfrep2D::Point2Df( x, y ), bat );

    std::vector<hfrep2D::Point2Di> src; src.push_back( hfrep2D::Point2Di(x,y) );
    hfrep2D::FIMSolver fim(resX, resY);
    fim.setSources( &frep4, nullptr );
    fim.solveEikonalEquationParallel_CPU(-0.0019f, 0.0019f);
    std::vector<float> df = fim.getDistanceField();

    hfrep2D::DiscreteDistanceTransform ddt(resX, resY);
    ddt.caclulateDiscreteDistanceTransformVec( &frep4 );
    std::vector<float> ddt1 = ddt.get_DDT();

    /*hfrep2D::interpolate inter(resX, resY, 0);
    inter.smoothField( &ddt1, BICUBIC_INT );
    std::vector<float> smDDT1 = inter.getField();*/

    boost::math::cubic_b_spline<float> spline( df.data(), df.size(), 0, 1.0f );
    std::vector<float> smDDT1;
    for(int i = 0; i < ddt1.size(); i++)
        smDDT1.push_back( spline(i) );

    /*hfrep2D::HFRepObj2D hfrep(resX, resY, 0.0);
    std::vector<float> hfrep1 = hfrep.calculateHFRep2D( &frep4, &df,     HYPERBOLIC_SIGMOID, 0.00001f, true );
    std::vector<float> hfrep2 = hfrep.calculateHFRep2D( &frep4, nullptr, HYPERBOLIC_SIGMOID, 0.00001f, true );
*/

    hfrep2D::render2D render;
    render.drawIsolines( &frep4, resX, resY, 0.002f, "frep", 0.03f );
    render.drawIsolines( &df, resX, resY, 0.015f, "df" );
    render.drawIsolines( &ddt1, resX, resY, 0.03f, "ddt" );
    render.drawIsolines( &smDDT1, resX, resY, 0.03f, "smDDT" );
    //render.drawIsolines( &hfrep1, resX,resY, 0.017f, "hfrep_df" );
    //render.drawIsolines( &hfrep2, resX, resY, 0.03f, "hfrep_ddt" );

    /*std::ofstream file1, file2;
    file1.open("dfield.txt");
    file2.open("ddt.txt");

    for( size_t y = 0; y < resY; y++ )
    {
        for( size_t x = 0; x < resX; x++ )
        {
            file1 << df[x+y*resY] << "\t";
            file2 << ddt1[x+y*resY] << "\t";
        }
        file1 << std::endl;
        file2 << std::endl;
    }
    file1.close();
    file2.close();*/

    return 0;
}
