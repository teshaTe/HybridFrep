#include <iostream>
#include <chrono>
#include <random>
#include <fstream>

#include "frep2D.h"
#include "ddt2D.h"
#include "eikonalSolver2D.h"
#include "render2D.h"

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

    std::vector<float> frep1 = frep.getFRep2D( hfrep2D::Point2Df( x, y ), r, circle );
    std::vector<float> frep2 = frep.getFRep2D( hfrep2D::Point2Df( x, y ), a, b, rec );
    std::vector<float> frep3 = frep.getFRep2D( hfrep2D::Point2Df( x, y ), key );

    std::vector<hfrep2D::Point2Di> src; src.push_back( hfrep2D::Point2Di(x,y) );
    hfrep2D::FIMSolver fim(resX, resY);
    fim.setSources( &frep3, nullptr );
    fim.solveEikonalEquationParallel_CPU();
    std::vector<float> df = fim.getDistanceField();

    hfrep2D::DiscreteDistanceTransform ddt(resX, resY);
    ddt.caclulateDiscreteDistanceTransformVec( &frep3 );
    std::vector<float> ddt1 = ddt.get_DDT();

    hfrep2D::render2D render;
    render.drawRGB_isolines( &frep1, resX, resY, 0.002f, "frep", 0.03f );
    render.drawRGB_isolines( &df, resX, resY, 0.02f, "df" );
    render.drawRGB_isolines( &ddt1, resX, resY, 0.05f, "ddt", 0.03f );

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
