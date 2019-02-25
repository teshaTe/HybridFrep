#include "include/ddt2D.h"

#include <chrono>
#include <ctime>

namespace hfrep2D
{

DiscreteDistanceTransform::DiscreteDistanceTransform( int grWidth, int grHeight ): res_x( grWidth ),
                                                                                   res_y( grHeight ),
                                                                                   INF(calculate_inf()),
                                                                                   INSIDE( INF, INF ),
                                                                                   EMPTY( 0.0, 0.0 ),
                                                                                   grid_1(res_x*res_y, INSIDE),
                                                                                   grid_2(res_x*res_y, INSIDE),
                                                                                   DDT( res_x*res_y, 0.0 )
{ }

void DiscreteDistanceTransform::caclulateDiscreteDistanceTransformVec(std::vector<float> *inField)
{
    create_grid( inField );
    generate_DF( grid_1 );
    generate_DF( grid_2 );
    merge_grids( DDT );
}

/*
 * create_grid() - function which preinitialize DDT grid for further calculations
 */
void DiscreteDistanceTransform::create_grid(std::vector<float> *field )
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
        other_point.dx += offsetx;
        other_point.dy += offsety;

        if(other_point.EuclideanDist() < point.EuclideanDist())
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
void DiscreteDistanceTransform::merge_grids(std::vector<float> &grid)
{
    SDDT.resize( grid.size());

    for( int y = 0; y < static_cast<int>(res_y); y++)
    {
       for( int x = 0; x < static_cast<int>(res_x); x++)
       {
           int ind = index( x, y );
           float dist1 = ( std::sqrt( get_grid_element( grid_1, ind).EuclideanDist() ));
           float dist2 = ( std::sqrt( get_grid_element( grid_2, ind).EuclideanDist() ));
           DDT[ind]  = std::abs( (dist1 - dist2) * 10.0f / INF );
           SDDT[ind] = (dist1 - dist2) * 10.0f / INF ;
       }
    }
}

}//namespace distance_field


