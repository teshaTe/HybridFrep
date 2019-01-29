#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "include/ddt2D.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

namespace distance_transform
{

DiscreteDistanceTransform::DiscreteDistanceTransform( cv::Mat img, int b_sh ) : res_x( img.cols ),
                                                                                res_y( img.rows ),
                                                                                INF(calculate_inf()),
                                                                                INSIDE( INF, INF ),
                                                                                EMPTY( 0.0, 0.0 ),
                                                                                grid_1(res_x*res_y, INSIDE),
                                                                                grid_2(res_x*res_y, INSIDE),
                                                                                DDT( res_x*res_y, 0.0 )
{
    cv::copyMakeBorder( img, newSrc, 0, b_sh, 0, b_sh, cv::BORDER_REPLICATE );
}

DiscreteDistanceTransform::DiscreteDistanceTransform( int grWidth, int grHeight ): res_x( grWidth ),
                                                                                   res_y( grHeight ),
                                                                                   INF(calculate_inf()),
                                                                                   INSIDE( INF, INF ),
                                                                                   EMPTY( 0.0, 0.0 ),
                                                                                   grid_1(res_x*res_y, INSIDE),
                                                                                   grid_2(res_x*res_y, INSIDE),
                                                                                   DDT( res_x*res_y, 0.0 )
{ }

void DiscreteDistanceTransform::caclulateDiscreteDistanceTransformVec(std::vector<double> *inField)
{
    create_grid(nullptr, inField);
    generate_DF(grid_1);
    generate_DF(grid_2);
    merge_grids(DDT);
}

void DiscreteDistanceTransform::caclulateDiscreteDistanceTransformImg()
{
    create_grid( &newSrc, nullptr);
    generate_DF(grid_1);
    generate_DF(grid_2);
    merge_grids(DDT);
}

cv::Mat DiscreteDistanceTransform::binarize_input( cv::Mat src)
{
    cv::Mat dst_gray, dst_bw;
    if(src.channels() > 1)
    {
        cv::cvtColor( src, dst_gray, cv::COLOR_BGR2GRAY );
        cv::blur( dst_gray, dst_gray, cv::Size( 3, 3 ) );
        dst_bw = dst_gray > 60;
    }
    else
    {
        cv::blur( src, src, cv::Size( 3, 3 ) );
        dst_bw = src > 60;
    }

#ifdef USE_DEBUG_INFO_DF
    cv::imwrite("binary_image.png", dst_bw);
#endif

    return dst_bw;
}

/*
 * create_grid() - function which preinitialize DDT grid for further calculations
 */
void DiscreteDistanceTransform::create_grid( cv::Mat *img, std::vector<double> *field )
{
    if(img != nullptr)
    {
        cv::Mat bin_dst = binarize_input(*img);
        unsigned char *img_buff = static_cast<unsigned char*>(bin_dst.data);

        for (int y = 0; y < static_cast<int>(res_y); y++)
        {
           for (int x = 0; x < static_cast<int>(res_x); x++)
           {
               int ind = index( x, y);

               if(img_buff[ind] > 0)
               {
                  fill_grid_element( grid_1, INSIDE, ind );
                  fill_grid_element( grid_2, EMPTY,  ind );
               }
               else
               {
                  fill_grid_element( grid_1, EMPTY,  ind );
                  fill_grid_element( grid_2, INSIDE, ind );
               }
           }
        }
    }
    else
    {
        for (int y = 0; y < static_cast<int>(res_y); y++)
        {
           for (int x = 0; x < static_cast<int>(res_x); x++)
           {
               int ind = index( x, y);

               if(field->at(x+y*res_x) >= 0)
               {
                   fill_grid_element( grid_1, INSIDE, ind );
                   fill_grid_element( grid_2, EMPTY,  ind );
               }
               else
               {
                   fill_grid_element( grid_1, EMPTY,  ind );
                   fill_grid_element( grid_2, INSIDE, ind );
               }
           }
        }

    }
}

/*
 * compare_grid_points(...) - function for comparing values in DDT grid  (8 neighbours algorithm is made)
 */
void DiscreteDistanceTransform::compare_grid_points(std::vector<Point2D> &grid, Point2D &point,
                                                    int offsetx, int offsety, int x, int y)
{
    if(x + offsetx >= 0 && x + offsetx < res_x &&
       y + offsety >= 0 && y + offsety < res_y)
    {
        int ind = index(x + offsetx, y + offsety);
        Point2D other_point = get_grid_element( grid, ind );
        other_point.m_dx += offsetx;
        other_point.m_dy += offsety;

        if(other_point.square_distance() < point.square_distance())
            point = other_point;
    }
}

/*
 * generate_DF(...) - function which allows one to calculate distance field using 8-point algorithm.
 *                    the central point is chosen and comparing algorithm is called looking in 8 neighbours around
 */
void DiscreteDistanceTransform::generate_DF(std::vector<Point2D> &grid)
{
// Pass 0: firstly filtering grid in forward manner on both x & y, then on x in reverse manner
    for (int y = 0; y < static_cast<int>(res_y); y++)
    {
        for (int x = 0; x < static_cast<int>(res_x); x++)
        {
           int ind = index( x, y );
           Point2D point = get_grid_element( grid, ind );
           compare_grid_points( grid, point, -1,  0, x, y );
           compare_grid_points( grid, point,  0, -1, x, y );
           compare_grid_points( grid, point, -1, -1, x, y );
           compare_grid_points( grid, point,  1, -1, x, y );
           fill_grid_element( grid, point, ind );
         }

         for (int x = static_cast<int>(res_x-1); x >= 0; x--)
         {
            int ind = index( x, y );
            Point2D point = get_grid_element( grid, ind);
            compare_grid_points( grid, point, 1, 0, x, y);
            fill_grid_element( grid, point, ind);
         }
    }

// Pass 1: firstly filtering grid in reverse manner on both x & y, then on x in forward manner
    for (int y = static_cast<int>(res_y-1); y >= 0; y--)
    {
       for (int x = static_cast<int>(res_x-1); x >= 0; x--)
       {
           int ind = index( x, y );
           Point2D point = get_grid_element( grid, ind);
           compare_grid_points( grid, point,  1,  0, x, y);
           compare_grid_points( grid, point,  0,  1, x, y);
           compare_grid_points( grid, point, -1,  1, x, y);
           compare_grid_points( grid, point,  1,  1, x, y);
           fill_grid_element( grid, point, ind);
       }

       for (int x = 0; x < static_cast<int>(res_x); x++)
       {
           int ind = index( x, y );
           Point2D point = get_grid_element( grid, ind);
           compare_grid_points( grid, point, -1, 0, x, y);
           fill_grid_element( grid, point, ind);
       }
    }
}

/*
 * merge_grids(...) - the 8-point algorithm generates 2 grids which are finally merged in one with sign defined
 */
void DiscreteDistanceTransform::merge_grids(std::vector<double> &grid)
{
    SDDT.resize( grid.size());

    for( int y = 0; y < static_cast<int>(res_y); y++)
    {
       for( int x = 0; x < static_cast<int>(res_x); x++)
       {
           int ind = index( x, y );
           double dist1 = ( std::sqrt( get_grid_element( grid_1, ind).square_distance() ));
           double dist2 = ( std::sqrt( get_grid_element( grid_2, ind).square_distance() ));
           DDT[ind]  = std::abs( (dist1 - dist2) * 10.0 / INF );
           SDDT[ind] = (dist1 - dist2) * 10.0 / INF ;
       }
    }

#ifdef USE_DEBUG_INFO_DF
    std::ofstream file;
    file.open("DDT2.txt");
    file.precision(7);


    for(int i = 0; i < res_y*res_x; x++)
       file << DDT[i] << std::endl;

#endif
}

}//namespace distance_field


