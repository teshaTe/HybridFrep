#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"

#include "render2D.h"
#include "frep2D.h"
#include "microstructures2D.h"
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
    frep2D::FRepObj2D frep(512, 512, 3.0f);
    frep2D::microstruct mStruct( 512, 512 );
    hfrep2D::HFRepObj2D hfrep( 512, 512, 0 );

    auto rect = std::bind( &frep2D::FRepObj2D::rectangle, frep, std::placeholders::_1, std::placeholders::_2,
                                                                std::placeholders::_3, std::placeholders::_4 );
    auto triangle = std::bind( &frep2D::FRepObj2D::triangle2, frep, std::placeholders::_1, std::placeholders::_2,
                                            std::placeholders::_3, std::placeholders::_4 );
    auto union_f = std::bind( &frep2D::FRepObj2D::union_function, frep, std::placeholders::_1, std::placeholders::_2,
                                                                        std::placeholders::_3, std::placeholders::_4 );

    auto star = std::bind( &frep2D::FRepObj2D::suriken, frep, std::placeholders::_1, std::placeholders::_2 );

    std::vector<float> rec0 = frep.getFRep2D( frep2D::Point2D(250.0f, 250.0f), 50.0f, 30.0f, rect );
    std::vector<float> rec1 = frep.getFRep2D( frep2D::Point2D(330.0f, 245.0f), 40.0f, 35.0f, rect );
    std::vector<float> rec2 = frep.getFRep2D( frep2D::Point2D(170.0f, 245.0f), 40.0f, 35.0f, rect );
    std::vector<float> rec3 = frep.getFRep2D( frep2D::Point2D(335.0f, 204.0f), 35.0f, 9.0f, rect );
    std::vector<float> rec4 = frep.getFRep2D( frep2D::Point2D(165.0f, 204.0f), 35.0f, 9.0f, rect );
    std::vector<float> rec5 = frep.getFRep2D( frep2D::Point2D(250.0f, 215.0f), 12.0f, 6.0f, rect );

    std::vector<float> tri0 = frep.getRotatedFrep2D(frep2D::Point2D( 250.0f, 270.0f ), 40.0f, 40.0f,  45.0f, triangle );
    std::vector<float> tri1 = frep.getRotatedFrep2D(frep2D::Point2D( 216.1f, 216.0f ),  7.0f,  7.0f,   0.0f, triangle );
    std::vector<float> tri2 = frep.getRotatedFrep2D(frep2D::Point2D( 283.1f, 216.0f ),  7.0f,  7.0f,  90.0f, triangle );
    std::vector<float> tri3 = frep.getRotatedFrep2D(frep2D::Point2D( 204.7f, 202.08f ), 5.0f,  8.0f,   0.0f, triangle );
    std::vector<float> tri4 = frep.getRotatedFrep2D(frep2D::Point2D( 295.4f, 202.08f ), 8.0f,  5.0f,  90.0f, triangle );
    std::vector<float> tri5 = frep.getRotatedFrep2D(frep2D::Point2D( 258.5f, 199.3f ),  4.0f, 10.0f,   0.0f, triangle );
    std::vector<float> tri6 = frep.getRotatedFrep2D(frep2D::Point2D( 241.0f, 199.3f ), 10.0f,  4.0f,  90.0f, triangle );
    std::vector<float> tri7 = frep.getRotatedFrep2D(frep2D::Point2D(  98.0f, 230.0f ), 40.0f, 35.0f, 180.0f, triangle );
    std::vector<float> tri8 = frep.getRotatedFrep2D(frep2D::Point2D( 400.0f, 230.0f ), 35.0f, 40.0f, 270.0f, triangle );

    std::vector<float> union0 = frep.getFRep2D( rec0, rec1, 0.0f, 0.0f, union_f );
    std::vector<float> union1 = frep.getFRep2D( union0, rec2, 0.0f, 0.0f, union_f );
    std::vector<float> union2 = frep.getFRep2D( union1, rec3, 0.0f, 0.0f, union_f );
    std::vector<float> union3 = frep.getFRep2D( union2, rec4, 0.0f, 0.0f, union_f );
    std::vector<float> union4 = frep.getFRep2D( union3, rec5, 0.0f, 0.0f, union_f );
    std::vector<float> union5 = frep.getFRep2D( union4, tri0, 0.0f, 0.0f, union_f );
    std::vector<float> union6 = frep.getFRep2D( union5, tri1, 0.0f, 0.0f, union_f );
    std::vector<float> union7 = frep.getFRep2D( union6, tri2, 0.0f, 0.0f, union_f );
    std::vector<float> union8 = frep.getFRep2D( union7, tri3, 0.0f, 0.0f, union_f );
    std::vector<float> union9 = frep.getFRep2D( union8, tri4, 0.0f, 0.0f, union_f );
    std::vector<float> union10 = frep.getFRep2D( union9, tri5, 0.0f, 0.0f, union_f );
    std::vector<float> union11 = frep.getFRep2D( union10, tri6, 0.0f, 0.0f, union_f );
    std::vector<float> union12 = frep.getFRep2D( union11, tri7, 0.0f, 0.0f, union_f );
    std::vector<float> union13 = frep.getFRep2D( union12, tri8, 0.0f, 0.0f, union_f );

    std::vector<float> suriken = frep.getFRep2D( frep2D::Point2D(250.0f, 215.0f), star );

    std::vector<float> hfrep0 = hfrep.calculateHFRep2D( &union13, nullptr, HYPERBOLIC_SIGMOID, 0.0000001f, true );
    std::vector<float> hfrep1 = hfrep.calculateHFRep2D( &union13, nullptr, HYPERBOLIC_SIGMOID, 0.02f, true );
    std::vector<float> hfrep2 = hfrep.calculateHFRep2D( &suriken, nullptr, HYPERBOLIC_SIGMOID, 0.0000001f, true );

    std::vector<float> sddt2 = hfrep.getSignedDDT();

    hfrep2D::render2D draw;
    draw.drawRGB_isolines( &hfrep0, 512, 512, 0.03f, "hfrep" );
    cv::imshow( "pic1", draw.getResultingImg() );

    draw.drawRGB_isolines( &hfrep1, 512, 512, 0.005f, "hfrep_C1" );
    cv::imshow( "pic2", draw.getResultingImg() );

    draw.drawRGB_isolines( &hfrep2, 512, 512, 0.005f, "hfrep_star" );
    cv::imshow( "pic3", draw.getResultingImg() );

    draw.drawRGB_isolines( &sddt2, 512, 512, 0.005f, "sddt_star" );
    cv::imshow( "pic4", draw.getResultingImg() );

    cv::waitKey(0);

    return 0;
}
