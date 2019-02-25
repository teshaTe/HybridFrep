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
    frep2D::FRepObj2D frep( 512, 512, 4.0f );
    auto fun = std::bind(&frep2D::FRepObj2D::heart2D, frep, std::placeholders::_1, std::placeholders::_2);
    std::vector<float> heart = frep.getFRep2D( frep2D::Point2D( 250.0f, 150.0f ), fun );

    cv::Mat heart_pic = cv::Mat(512, 512, CV_8UC1);
    unsigned char* heart_buf = heart_pic.data;

    for (size_t i =0; i < 512*512; i++)
    {
        if( heart[i] < 0.0f )
            heart_buf[i] = 0;
        else {
            heart_buf[i] = 255;
        }
    }

    cv::imwrite("heart.jpg", heart_pic);

    auto fun1 = std::bind(&frep2D::FRepObj2D::rectangle, frep, std::placeholders::_1, std::placeholders::_2,
                                                               std::placeholders::_3, std::placeholders::_4);
    std::vector<float> frep1H = frep.getFRep2D( frep2D::Point2D( 100.0f, 350.0f ),
                                                  10.0f, 40.0f, fun1 );
    std::vector<float> frep2H = frep.getFRep2D( frep2D::Point2D( 140.0f, 350.0f ),
                                                  10.0f, 40.0f, fun1 );
    std::vector<float> frep3H = frep.getFRep2D( frep2D::Point2D( 125.0f, 350.0f ),
                                                  20.0f, 10.0f, fun1 );

    std::vector<float> frep1E = frep.getFRep2D( frep2D::Point2D( 170.0f, 350.0f ),
                                                  10.0f, 40.0f, fun1 );
    std::vector<float> frep2E = frep.getFRep2D( frep2D::Point2D( 195.0f, 320.0f ),
                                                  20.0f, 10.0f, fun1 );
    std::vector<float> frep3E = frep.getFRep2D( frep2D::Point2D( 195.0f, 350.0f ),
                                                  20.0f, 10.0f, fun1 );
    std::vector<float> frep4E = frep.getFRep2D( frep2D::Point2D( 195.0f, 380.0f ),
                                                  20.0f, 10.0f, fun1 );

    std::vector<float> frep1A = frep.getFRep2D( frep2D::Point2D( 235.0f, 350.0f ),
                                                  10.0f, 40.0f, fun1 );
    std::vector<float> frep2A = frep.getFRep2D( frep2D::Point2D( 260.0f, 320.0f ),
                                                  30.0f, 10.0f, fun1 );
    std::vector<float> frep3A = frep.getFRep2D( frep2D::Point2D( 285.0f, 350.0f ),
                                                  10.0f, 40.0f, fun1 );
    std::vector<float> frep4A = frep.getFRep2D( frep2D::Point2D( 260.0f, 350.0f ),
                                                  30.0f, 10.0f, fun1 );

    std::vector<float> frep1R = frep.getFRep2D( frep2D::Point2D( 320.0f, 350.0f ),
                                                  10.0f, 40.0f, fun1 );
    std::vector<float> frep2R = frep.getFRep2D( frep2D::Point2D( 365.0f, 330.0f ),
                                                  10.0f, 20.0f, fun1 );
    std::vector<float> frep3R = frep.getFRep2D( frep2D::Point2D( 345.0f, 320.0f ),
                                                  30.0f, 10.0f, fun1 );
    std::vector<float> frep4R = frep.getFRep2D( frep2D::Point2D( 345.0f, 350.0f ),
                                                  30.0f, 10.0f, fun1 );
    std::vector<float> frep5R = frep.getRotatedFrep2D(frep2D::Point2D( 510.0f, 10.0f ),
                                                  25.0f, 10.0f, -45.0f*3.14f/180.0f, fun1 );

    std::vector<float> frep1T = frep.getFRep2D( frep2D::Point2D( 415.0f, 350.0f ),
                                                  10.0f, 40.0f, fun1 );
    std::vector<float> frep2T = frep.getFRep2D( frep2D::Point2D( 415.0f, 320.0f ),
                                                  30.0f, 10.0f, fun1 );


    std::vector<float> frepH, frepT, frepA, frepR, frepE, frepHEART;

    for( size_t i = 0; i < 512*512; i++)
    {
        frepH.push_back( frep.union_function(frep1H[i], frep.union_function( frep2H[i], frep3H[i] )));
        frepT.push_back( frep.union_function( frep1T[i], frep2T[i] ) );
        frepA.push_back( frep.union_function(frep1A[i], frep.union_function(frep2A[i],
                                                        frep.union_function(frep3A[i], frep4A[i]))));
        frepR.push_back( frep.union_function(frep1R[i], frep.union_function(frep2R[i],
                                                         frep.union_function(frep3R[i],
                                                         frep.union_function(frep4R[i], frep5R[i])))) );
        frepE.push_back( frep.union_function(frep1E[i], frep.union_function(frep2E[i], frep.union_function(frep3E[i], frep4E[i]))) );
        frepHEART.push_back( frep.union_function( frepH[i], frep.union_function(frepE[i],
                             frep.union_function(frepA[i], frep.union_function(frepR[i],frepT[i] )) )) );
    }

    cv::Mat h_pic = cv::Mat(512, 512, CV_8UC1);
    cv::Mat t_pic = cv::Mat(512, 512, CV_8UC1);
    cv::Mat a_pic = cv::Mat(512, 512, CV_8UC1);
    cv::Mat r_pic = cv::Mat(512, 512, CV_8UC1);
    cv::Mat e_pic = cv::Mat(512, 512, CV_8UC1);
    cv::Mat heartFR_pic = cv::Mat(512, 512, CV_8UC1);

    unsigned char* h_buf = h_pic.data;
    unsigned char* t_buf = t_pic.data;
    unsigned char* a_buf = a_pic.data;
    unsigned char* r_buf = r_pic.data;
    unsigned char* e_buf = e_pic.data;
    unsigned char* heartFR_buf = heartFR_pic.data;

    for (size_t i =0; i < 512*512; i++)
    {
        if( frepH[i] < 0.0f )
            h_buf[i] = 0;
        else {
            h_buf[i] = 255;
        }
        if( frepT[i] < 0.0f )
            t_buf[i] = 0;
        else {
            t_buf[i] = 255;
        }
        if( frepA[i] < 0.0f )
            a_buf[i] = 0;
        else {
            a_buf[i] = 255;
        }
        if( frepR[i] < 0.0f )
            r_buf[i] = 0;
        else {
            r_buf[i] = 255;
        }
        if( frepE[i] < 0.0f )
            e_buf[i] = 0;
        else {
            e_buf[i] = 255;
        }
        if( frepHEART[i] < 0.0f )
            heartFR_buf[i] = 0;
        else {
            heartFR_buf[i] = 255;
        }
    }

    cv::imwrite("h.jpg", h_pic);
    cv::imwrite("t.jpg", t_pic);
    cv::imwrite("a.jpg", a_pic);
    cv::imwrite("r.jpg", r_pic);
    cv::imwrite("e.jpg", e_pic);
    cv::imwrite("heartFR.jpg", heartFR_pic);

    return 0;
}
