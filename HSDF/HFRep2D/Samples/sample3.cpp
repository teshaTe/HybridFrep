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

inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max : ( n < n_min ? n_min : n );}
inline float convertToUV( float val ) { return val / 512.0f; }
inline glm::vec2 convertToUV( glm::vec2 val ) { return glm::vec2( val.x/512.0f, val.y/512.0f ); }

float find_Y_coordinate( glm::vec2 cent1, glm::vec2 cent2, float R1, float R2 )
{
    float X, Y;
    float A    = cent1.x*cent1.x + cent1.y*cent1.y - R1*R1;
    float B    = cent2.x*cent2.x + cent2.y*cent2.y - R2*R2;
    float dx   = cent1.x - cent2.x;
    float dy   = cent1.y - cent2.y;
    float znam1 = 8.0f*(dx*dx + dy*dy);

    float a1 =  4.0f*(A - B)*dx + 8.0f*(cent2.x*cent1.y - cent1.x*cent2.y)*dy;
    float a2 = -16.0f*( dx*dx + dy*dy );
    float a3 =  (A-B)*(A-B) + 4.0f*(B*cent1.y - A*cent2.y)*dy;
    float a4 =  4.0f *dx*( B - A ) - 8.0f*( cent1.x*cent2.y - cent2.x*cent1.y )*dy;

    X = (1.0f/znam1) * ( a1 - std::sqrt( a2 * a3 + a4*a4 ));

    float znam2 = znam1 / 2.0f;
    float cx12  = cent1.x*cent1.x;
    float cx22  = cent2.x*cent2.x;
    float cy12  = cent1.x*cent1.y;
    float cy22  = cent2.x*cent2.y;

    float b1 = 4.0f*dx*dx*( A - B ) + 8.0f * cent1.x * cent2.x * ( cy12 - cy22 ) +
               8.0f*cent1.y*cent2.y * ( cx22 - cx12 ) + 8.0f * ( cx12*cy22 - cx22*cy12 );

    Y = ( 1.0f / (-2.0f*dy) ) * ( B - A + ( b1 - cent1.x * std::sqrt( a2 * a3 + a4*a4 ) +
                                                 cent2.x * std::sqrt( a2 * a3 + a4*a4 )) / znam2 );

    return Y;
}

int main(int argc, char** argv)
{
    hfrep2D::FRepObj2D frep( 512, 512, 4.0f );

    auto fun1 = std::bind(&hfrep2D::FRepObj2D::circle, frep, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    std::vector<float> circle0 = frep.getFRep2D( glm::vec2( 250.0f, 150.0f ), 60.0f, fun1 );
    std::vector<float> circle1 = frep.getFRep2D( glm::vec2( 250.0f, 230.0f ), 90.0f, fun1 );

    hfrep2D::HFRepObj2D hfrep( 512, 512, 0 );

    std::vector<float> hfrep_circle0 = hfrep.calculateHFRep2D( &circle0, nullptr, HYPERBOLIC_SIGMOID, 0.00001f, true );
    std::vector<float> sddt_circle0  = hfrep.getSignedDDT();

    std::vector<float> hfrep_circle1 = hfrep.calculateHFRep2D( &circle1, nullptr, HYPERBOLIC_SIGMOID, 0.00001f, true );
    std::vector<float> sddt_circle1  = hfrep.getSignedDDT();

    //draw.drawIsolines( &field, &hfrep_fin, 512, 512, 0.04f, "hfrep_combo", true );
    auto fun2 = std::bind(&hfrep2D::FRepObj2D::rectangle, frep, std::placeholders::_1, std::placeholders::_2,
                                                               std::placeholders::_3, std::placeholders::_4);
    std::vector<float> rectangle = frep.getFRep2D( glm::vec2(250.0f, 230.0f), 110.0f, 50.0f, fun2 );

    auto fun3 = std::bind( &hfrep2D::FRepObj2D::union_function, frep, std::placeholders::_1, std::placeholders::_2,
                                                                     std::placeholders::_3, std::placeholders::_4 );
    std::vector<float> frep_union = frep.getFRep2D( rectangle, circle0, 0.0f, 0.0f, fun3 );
    std::vector<float> frep_union_cir = frep.getFRep2D( circle0, circle1, 0.0f, 0.0f, fun3 );

    std::vector<float> hfrep_rec = hfrep.calculateHFRep2D( &rectangle, nullptr, HYPERBOLIC_SIGMOID, 0.00001f, true );
    std::vector<float> sddt_rec  = hfrep.getSignedDDT();

    std::vector<float> hfrep_union = hfrep.calculateHFRep2D( &frep_union, nullptr, HYPERBOLIC_SIGMOID, 0.1f, true );

    std::vector<float> sddt_union, sddt_union_alt;
    for ( size_t i = 0; i < 512*512; i++)
    {
        sddt_union.push_back( std::max( sddt_circle0[i], sddt_rec[i] ));
    }

    std::vector<float> sddt_circle;
    for (int y = 0; y < 512 ; y++)
    {
        for (int x = 0; x < 512; x++)
        {
            glm::vec2 grF1 = glm::vec2( sddt_circle0[x+1+y*512]   - sddt_circle0[x+y*512],
                                        sddt_circle0[x+(y+1)*512] - sddt_circle0[x+y*512] );
            glm::vec2 grF2 = glm::vec2( sddt_rec[x+1+y*512]   - sddt_rec[x+y*512],
                                        sddt_rec[x+(y+1)*512] - sddt_rec[x+y*512] );
            sddt_circle.push_back( frep.union_function_R0( sddt_circle0[x+y*512], sddt_rec[x+y*512], grF1, grF2, 2.0f ));

            sddt_union_alt.push_back( hfrep.get_step_function_val( frep_union[x+y*512], HYPERBOLIC_SIGMOID, 0.01f )
                                      * std::abs(sddt_union[x+y*512]));
        }
    }

    //euclidean distance
    std::vector<float> true_dist0, true_dist1, true_dist2, dist_test;
    glm::vec2 cent1 = convertToUV( glm::vec2( 250.0f, 150.0f ) );
    glm::vec2 cent2 = convertToUV( glm::vec2( 250.0f, 230.0f ) );

    std::vector<float> inside0, outside0, inside1, outside1;
    for (int y = 0; y < 512; y++)
    {
        for (int x = 0; x < 512; x++)
        {
            float u = x / 512.0f;
            float v = y / 512.0f;

            float dist1 = ( - std::sqrt((u - cent1.x)*(u - cent1.x) +
                              (v - cent1.y )*(v - cent1.y )) + 60.0f / 512.0f ) * 10.0f;
            float dist2 = ( - std::sqrt((u - cent2.x)*(u - cent2.x) +
                              (v - cent2.y )*(v - cent2.y )) + 90.0f / 512.0f ) * 10.0f;

            true_dist0.push_back( std::max( dist1, dist2 ) );

            glm::vec2 grF1 = glm::vec2( -(u - cent1.x)/
                                   std::sqrt((u - cent1.x)*(u - cent1.x) + (v - cent1.y)*(v - cent1.y)),
                                                    -(v - cent1.y)/
                                   std::sqrt((u - cent1.x)*(u - cent1.x) + (v - cent1.y)*(v - cent1.y)));

            glm::vec2 grF2 = glm::vec2( -(u - cent2.x)/
                                   std::sqrt((u - cent2.x)*(u - cent2.x) + (v - cent2.y)*(v - cent2.y)),
                                                    -(v - cent2.y)/
                                   std::sqrt((u - cent2.x)*(u - cent2.x) + (v - cent2.y)*(v - cent2.y)));

            true_dist1.push_back( frep.union_function_R0( dist1, dist2, grF1, grF2, 2.0f ) );
           // true_dist1.push_back( hfrep.get_step_function_val( frep_union_cir[x+y*512], HYPERBOLIC_SIGMOID, 0.00001f ) * std::abs(frep.union_function_R0( dist1, dist2, grF1, grF2, 2.0f )));

            if( true_dist0[x+y*512] >= 0.001f )
                inside0.push_back( true_dist0[x+y*512] );
            else
                outside0.push_back( true_dist0[x+y*512] );

            if( true_dist1[x+y*512] >= 0.001f )
                inside1.push_back( true_dist1[x+y*512] );
            else
                outside1.push_back( true_dist1[x+y*512] );

        }
    }

    hfrep2D::ModifyField modF;
    std::vector<float> dist_diff    = modF.diff_fields( &true_dist0 , &true_dist1, 1.0f );
    std::vector<float> inside_diff  = modF.diff_fields( &inside0,     &inside1,    1.0f );
    std::vector<float> outside_diff = modF.diff_fields( &outside0,    &outside1,   1.0f );

    float aver_error     = std::accumulate( dist_diff.begin(),    dist_diff.end(),    0.0f );
    float aver_error_in  = std::accumulate( inside_diff.begin(),  inside_diff.end(),  0.0f );
    float aver_error_out = std::accumulate( outside_diff.begin(), outside_diff.end(), 0.0f );

    std::cout << "average error in: "  << aver_error_in  / static_cast<float>(inside_diff.size())  << std::endl;
    std::cout << "average error out: " << aver_error_out / static_cast<float>(outside_diff.size()) << std::endl;
    std::cout << "average error: "     << aver_error     / static_cast<float>(dist_diff.size())    << std::endl;

    hfrep2D::render2D draw;
    draw.drawIsolines( &sddt_union,     512, 512, 0.04f,  "sddt_max" );
    draw.drawIsolines( &sddt_union_alt, 512, 512, 0.001f, "sddt_alt_max" );
    draw.drawIsolines( &hfrep_union,    512, 512, 0.001f, "hfrep_union" );

    draw.drawIsolines( &sddt_circle, 512, 512, 0.04f, "sddt_R0_max" );
    draw.drawIsolines( &true_dist0,  512, 512, 0.02f, "true_dist_max" );
    draw.drawIsolines( &true_dist1,  512, 512, 0.02f, "true_dist_R0" );

    draw.drawIsolines( &frep_union_cir, 512, 512, 0.001f, "frep_union" );
    return 0;
}
