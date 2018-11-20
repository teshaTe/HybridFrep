#include <iostream>
#include <fstream>
#include <chrono>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "include/DDT.h"

namespace distance_transform
{

Point::Point(double dx, double dy) : m_dx(dx), m_dy(dy) { }
double Point::square_distance() { return (std::pow(m_dx, 2.0) + std::pow(m_dy, 2.0)); }

DistanceField::DistanceField( cv::Mat img, int bo_sh ): res_x(img.cols+bo_sh),
                                                        res_y(img.rows+bo_sh),
                                                        b_sh(bo_sh),
                                                        INF(calculate_inf()),
                                                        INSIDE(INF, INF),
                                                        EMPTY(0.0, 0.0),
                                                        grid_1(res_x * res_y, INSIDE),
                                                        grid_2(res_x * res_y, INSIDE),
                                                        DDT(res_x*res_y, 0.0)
{
    cv::Mat new_src;
    cv::copyMakeBorder( img, new_src, 0, b_sh, 0, b_sh, cv::BORDER_REPLICATE );
    create_grid( &new_src );
    generate_DF(grid_1);
    generate_DF(grid_2);
    merge_grids(DDT);
}

cv::Mat DistanceField::binarize_input( cv::Mat src)
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
void DistanceField::create_grid( cv::Mat *img )
{
    cv::Mat bin_dst = binarize_input(*img);
    unsigned char *img_buff = static_cast<unsigned char*>(bin_dst.data);

    for (int y = 0; y < res_y; y++)
    {
       for (int x = 0; x < res_x; x++)
       {

           if(img_buff[x+y*res_x] > 0)
          {
             fill_grid_element(grid_1, INSIDE, x, y);
             fill_grid_element(grid_2, EMPTY,  x, y);
          }
          else
          {
             fill_grid_element(grid_1, EMPTY,  x, y);
             fill_grid_element(grid_2, INSIDE, x, y);
          }
       }
    }
}

/*
 * compare_grid_points(...) - function for comparing values in DDT grid  (8 neighbours algorithm is made)
 */
void DistanceField::compare_grid_points(std::vector<Point> &grid, Point &point, int offsetx, int offsety, int x, int y)
{
    if(x + offsetx >= 0 && x + offsetx < res_x &&
       y + offsety >= 0 && y + offsety < res_y)
    {
        Point other_point = get_grid_element( grid, x + offsetx, y + offsety);
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
void DistanceField::generate_DF(std::vector<Point> &grid)
{
// Pass 0: firstly filtering grid in forward manner on both x & y, then on x in reverse manner
    for (int y1 = 0; y1 < res_y; y1++)
    {
        for (int x1 = 0; x1 < res_x; x1++)
        {
           Point point = get_grid_element( grid, x1, y1 );
           compare_grid_points( grid, point, -1,  0, x1, y1 );
           compare_grid_points( grid, point,  0, -1, x1, y1 );
           compare_grid_points( grid, point, -1, -1, x1, y1 );
           compare_grid_points( grid, point,  1, -1, x1, y1 );
           fill_grid_element( grid, point, x1, y1 );
         }

         for (int x2 = res_x - 1; x2 >= 0; x2--)
         {
            Point point = get_grid_element( grid, x2, y1);
            compare_grid_points( grid, point, 1, 0, x2, y1);
            fill_grid_element( grid, point, x2, y1);
         }
    }

// Pass 1: firstly filtering grid in reverse manner on both x & y, then on x in forward manner
    for (int y3 = res_y - 1; y3 >= 0; y3--)
    {
       for (int x3 = res_x - 1; x3 >= 0; x3--)
       {
          Point point = get_grid_element( grid, x3, y3);
          compare_grid_points( grid, point,  1,  0, x3, y3);
          compare_grid_points( grid, point,  0,  1, x3, y3);
          compare_grid_points( grid, point, -1,  1, x3, y3);
          compare_grid_points( grid, point,  1,  1, x3, y3);
          fill_grid_element( grid, point, x3, y3);
       }

       for (int x4 = 0; x4 < res_x; x4++)
       {
          Point point = get_grid_element( grid, x4, y3);
          compare_grid_points( grid, point, -1, 0, x4, y3);
          fill_grid_element( grid, point, x4, y3);
       }
    }
}

/*
 * merge_grids(...) - the 8-point algorithm generates 2 grids which are finally merged in one with sign defined
 */
void DistanceField::merge_grids(std::vector<double> &grid)
{
    int x, y;

     for( y = 0; y < res_y; y++)
    {
        for( x = 0; x < res_x; x++)
        {
            double dist1 = ( std::sqrt( (double)( get_grid_element( grid_1, x, y ).square_distance() )));
            double dist2 = ( std::sqrt( (double)( get_grid_element( grid_2, x, y ).square_distance() )));
            grid[x + y * res_x] = std::abs( (dist1 - dist2) * 10.0 / INF );
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

std::vector<double> DistanceField::smooth_field( const std::vector<double> *DT, int resX, int resY)
{
    std::vector<double> inter_result;
    for(int y = 0; y < resY-1; y++)
    {
        for(int x = 0; x < resX-1; x++)
        {
            double u = static_cast<float>(x)/resX;
            double v = static_cast<float>(y)/resY;
            double u_opp = 1.0 - u;
            double v_opp = 1.0 - v;

            double result = ( DT->at(x+    y*resX) * u_opp + DT->at((x+1)+    y*resX) * u ) * v_opp +
                            ( DT->at(x+(y+1)*resX) * u_opp + DT->at((x+1)+(y+1)*resX) * u ) * v;

            inter_result.push_back(result);
        }
    }
    return inter_result;
}

std::vector<double> DistanceField::generate_interpolated_field(const std::vector<double> *DT, int resX, int resY,
                                                               int shiftX, int shiftY, int interpolation_type)
{
    std::vector<double> fin_field = *DT;

    for( int y = 0; y < resY-shiftY; y+=shiftY )
    {
        for( int x = 0; x < resX-shiftX; x+=shiftX )
        {
            for( int y_in = 0; y_in <= shiftY; y_in++ )
            {
                for( int x_in = 0; x_in <= shiftX; x_in++ )
                {
                    if( DT->at(x+x_in+(y+y_in)*resX) == -10000.0f )
                    {
                        double k_x1 = static_cast<double>( shiftX - x_in ) / ( shiftX );
                        double k_x2 = static_cast<double>( x_in ) / ( shiftX );
                        double k_y1 = static_cast<double>( shiftY - y_in ) / ( shiftY );
                        double k_y2 = static_cast<double>( y_in ) / ( shiftY );

                        int ind0 = x+y*resX;
                        int ind1 = x+shiftX+y*resX;
                        int ind2 = x+(y+shiftY)*resX;
                        int ind3 = x+shiftX + (y+shiftY)*resX;

                        double fxy1 = k_x1 * DT->at(ind0) + k_x2 * DT->at(ind1);
                        double fxy2 = k_x1 * DT->at(ind2) + k_x2 * DT->at(ind3);
                        double result = k_y1 * fxy1 + k_y2 * fxy2;

                        fin_field[x+x_in + (y+y_in)*resX] = result;
                    }
                }
            }
        }
    }

    return fin_field;
}

std::vector<double> DistanceField::zoom_in_field(const std::vector<double> *field, const cv::Point2i start_p, const cv::Vec2i reg_s,
                                                 const cv::Vec2i fin_res, const int resX )
{
    std::vector<double> thinned_zoomed_field;
    if( (fin_res[0]-1)%reg_s[0] == 0 && (fin_res[1]-1)%reg_s[1] == 0 )
    {
        pix_xShift = fin_res[0] / reg_s[0];
        pix_yShift = fin_res[1] / reg_s[1];
    }
    else
    {
        std::cerr << "Uneven resolution attitude between zoomed res and region res is not supported yet! " << std::endl;
        return *field;
    }

    thinned_zoomed_field.resize( static_cast<size_t> (fin_res[0] * fin_res[1]) );
    std::fill(thinned_zoomed_field.begin(), thinned_zoomed_field.end(), -10000.0);

    int ind_x = 0;
    int ind_y = 0;

    for(int y = 0; y < reg_s[1]+1; y++)
    {
        for(int x = 0; x < reg_s[0]+1; x++)
        {
            thinned_zoomed_field[ind_x+ind_y*(fin_res[0])] = field->at(start_p.x+x+(start_p.y+y)*resX );
            ind_x += pix_xShift;
        }
        ind_x = 0;
        ind_y += pix_yShift;
    }

    std::vector<double> filled_field, result;
    filled_field = generate_interpolated_field( &thinned_zoomed_field, fin_res[0], fin_res[1], pix_xShift, pix_yShift);
    result = finalize_field( &filled_field, fin_res[0], fin_res[1] );

    return result;
}

std::vector<double> DistanceField::finalize_field( const std::vector<double> *field, int resX, int resY )
{
    std::vector<double> result;

    for(int y = 0; y < resY; y++)
    {
        for(int x = 0; x < resX; x++)
        {
            if( x < (resX-b_sh) )
            {
                if( y < (resY-b_sh))
                    result.push_back( field->at(x+y*resX) );
            }
        }
    }

    return result;
}

}//namespace distance_field


