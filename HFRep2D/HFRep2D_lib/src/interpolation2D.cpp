#include "include/interpolation2D.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace hfrep2D {

std::vector<float> ModifyField::smooth_field( const std::vector<float> *DT, int resX, int resY)
{
    std::vector<float> inter_result; inter_result.reserve( DT->size());
    for(int y = 0; y < resY; y++)
    {
        for(int x = 0; x < resX; x++)
        {
            float u = static_cast<float>(x)/resX;
            float v = static_cast<float>(y)/resY;
            float u_opp = 1.0f - u;
            float v_opp = 1.0f - v;

            int x1 = clipWithBounds(x, 0, resX - 2 );
            int y1 = clipWithBounds(y, 0, resX - 2 );

            float result = ( DT->at(x1+    y1*resX) * u_opp + DT->at((x1+1)+    y1*resX) * u ) * v_opp +
                           ( DT->at(x1+(y1+1)*resX) * u_opp + DT->at((x1+1)+(y1+1)*resX) * u ) * v;

            inter_result.push_back(result);
        }
    }
    return inter_result;
}

float ModifyField::get_bicubic_interpolated_val(const std::vector<float> *DT, Point2Di res, Point2Df indXY)
{
    int x = clipWithBounds( int(indXY.dx), 0, res.dx-3 );
    int y = clipWithBounds( int(indXY.dy), 0, res.dy-3 );

    float f00 = DT->at(x+y*res.dx);
    float f10 = DT->at((x+1)+y*res.dx);
    float f01 = DT->at(x+(y+1)*res.dx);
    float f11 = DT->at((x+1)+(y+1)*res.dx);

    float fx00 = ( DT->at((x+1)+y*res.dx)     - DT->at(x+y*res.dx) );
    float fx10 = ( DT->at((x+2)+y*res.dx)     - DT->at((x+1)+y*res.dx) );
    float fx01 = ( DT->at((x+1)+(y+1)*res.dx) - DT->at(x+(y+1)*res.dx) );
    float fx11 = ( DT->at((x+2)+(y+1)*res.dx) - DT->at((x+1)+(y+1)*res.dx) );

    float fy00 = ( DT->at(x+(y+1)*res.dx)     - DT->at(x+y*res.dx) );
    float fy10 = ( DT->at((x+1)+(y+1)*res.dx) - DT->at((x+1)+y*res.dx) );
    float fy01 = ( DT->at(x+(y+2)*res.dx)     - DT->at(x+(y+1)*res.dx) );
    float fy11 = ( DT->at((x+1)+(y+2)*res.dx) - DT->at((x+1)+(y+1)*res.dx) );

    float fxy00_1 = ( DT->at((x+1)+(y+1)*res.dx) - DT->at((x+1)+y*res.dx) );
    float fxy00_2 = ( DT->at(x+(y+1)*res.dx)     - DT->at(x+y*res.dx));
    float fxy00   = ( fxy00_1 - fxy00_2 );

    float fxy10_1 = ( DT->at((x+2)+(y+1)*res.dx) - DT->at((x+2)+y*res.dx) );
    float fxy10_2 = ( DT->at((x+1)+(y+1)*res.dx) - DT->at((x+1)+y*res.dx) );
    float fxy10   = ( fxy10_1 - fxy10_2 );

    float fxy01_1 = ( DT->at((x+1)+(y+2)*res.dx) - DT->at((x+1)+(y+1)*res.dx) );
    float fxy01_2 = ( DT->at(x+(y+2)*res.dx)     - DT->at(x+(y+1)*res.dx) );
    float fxy01   = ( fxy01_1 - fxy01_2 );

    float fxy11_1 = ( DT->at((x+2)+(y+2)*res.dx) - DT->at((x+2)+(y+1)*res.dx) );
    float fxy11_2 = ( DT->at((x+1)+(y+2)*res.dx) - DT->at((x+1)+(y+1)*res.dx) );
    float fxy11   = ( fxy11_1 - fxy11_2 );

    float a00 =  f00;
    float a10 =  fx00;
    float a01 =  fy00;
    float a11 =  fxy00;
    float a20 = -3.0f*f00   + 3.0f*f10   - 2.0f*fx00  - fx10;
    float a02 = -3.0f*f00   + 3.0f*f01   - 2.0f*fy00  - fy01;
    float a21 = -2.0f*fxy00 - fxy10      - 3.0f*fy00  + 3.0f*fy10;
    float a12 = -3.0f*fx00  + 3.0f*fx01  - 2.0f*fxy00 - fxy01;
    float a22 =  9.0f*f00   - 9.0f*f01   - 9.0f*f10   + 9.0f*f11 + 6.0f*fx00 - 6.0f*fx01 + 3.0f*fx10 - 3.0f*fx11 +
                 4.0f*fxy00 + 2.0f*fxy01 + 2.0f*fxy10 + fxy11   + 6.0f*fy00 + 3.0f*fy01 - 6.0f*fy10 - 3.0f*fy11;
    float a30 =  2.0f*f00   - 2.0f*f10   + fx00       + fx10;
    float a03 =  2.0f*f00   - 2.0f*f01   + fy00       + fy01;
    float a31 =  fxy00      + fxy10      + 2.0f*fy00  - 2.0f*fy10;
    float a13 =  2.0f*fx00  - 2.0f*fx01  + fxy00      + fxy01;
    float a23 = -6.0f*f00   + 6.0f*f01   + 6.0f*f10   - 6.0f*f11 - 4.0f*fx00 + 4.0f*fx01 - 2.0f*fx10 + 2.0f*fx11 -
                 2.0f*fxy00 - 2.0f*fxy01 - fxy10      - fxy11    - 3.0f*fy00 - 3.0f*fy01 + 3.0f*fy10 + 3.0f*fy11;
    float a32 = -6.0f*f00   + 6.0f*f01   + 6.0f*f10   - 6.0f*f11 - 3.0f*fx00 + 3.0f*fx01 - 3.0f*fx10 + 3.0f*fx11 -
                 2.0f*fxy00 - fxy01      - 2.0f*fxy10 - fxy11    - 4.0f*fy00 - 2.0f*fy01 + 4.0f*fy10 + 2.0f*fy11;
    float a33 =  4.0f*f00   - 4.0f*f01   - 4.0f*f10   + 4.0f*f11 + 2.0f*fx00 - 2.0f*fx01 + 2.0f*fx10 - 2.0f*fx11 +
                 fxy00      + fxy01      + fxy10      + fxy11    + 2.0f*fy00 + 2.0f*fy01 - 2.0f*fy10 - 2.0f*fy11;

    float xN = indXY.dx - std::floor(indXY.dx);
    float yN = indXY.dy - std::floor(indXY.dy);

    float result = a00 + yN*a01 + yN*yN*a02 + std::pow(yN, 3.0f)*a03 + xN*a10 + xN*yN*a11 +
                   xN*yN*yN*a12 + xN*std::pow(yN, 3.0f)*a13 + xN*xN*a20 + xN*xN*yN*a21 +
                   std::pow(xN*yN, 2.0f)*a22 + yN*std::pow(xN*yN, 2.0f)*a23 + std::pow(xN, 3.0f)*a30 +
                   std::pow(xN, 3.0f)*yN*a31 + xN*std::pow(xN*yN, 2.0f)*a32 + std::pow(xN*yN, 3.0f)*a33 ;

    return result;
}


float ModifyField::get_bilinear_interpolated_val(const std::vector<float> *DT, Point2Di res, Point2Df indXY)
{
    float result;

    float k_x1 = static_cast<float>( std::floor(indXY.dx + 1.0f) - indXY.dx );
    float k_x2 = static_cast<float>( indXY.dx - std::floor(indXY.dx) );
    float k_y1 = static_cast<float>( std::floor(indXY.dy + 1.0f) - indXY.dy );
    float k_y2 = static_cast<float>( indXY.dy - std::floor(indXY.dy) );

    int x = clipWithBounds( int( indXY.dx ), 0, res.dx - 2);
    int y = clipWithBounds( int( indXY.dy ), 0, res.dy - 2);

    float fxy1 = k_x1 * DT->at(x+y*res.dx)     + k_x2 * DT->at(x+1+y*res.dx);
    float fxy2 = k_x1 * DT->at(x+(y+1)*res.dx) + k_x2 * DT->at(x+1 + (y+1)*res.dx);
    result = k_y1 * fxy1 + k_y2 * fxy2;

    return result;
}

std::vector<float> ModifyField::zoom_field( const std::vector<float> *field, const Point2Di start_p,
                                            const Point2Di reg_s, const Point2Di fin_res)
{
    std::vector<float> thinned_zoomed_field;
    if( (fin_res.dx)%reg_s.dx == 0 && (fin_res.dy)%reg_s.dy == 0 )
    {
        pix_xShift = fin_res.dx / reg_s.dx;
        pix_yShift = fin_res.dy / reg_s.dy;
    }
    else
    {
        pix_xShift = std::ceil(fin_res.dx / reg_s.dx);
        pix_yShift = std::ceil(fin_res.dy / reg_s.dy);
    }

    thinned_zoomed_field.resize(reg_s.dx*reg_s.dy);
    std::fill(thinned_zoomed_field.begin(), thinned_zoomed_field.end(), -10000.0f);

    for(int y = 0; y < reg_s.dy; y++)
    {
        for(int x = 0; x < reg_s.dx; x++)
        {
            thinned_zoomed_field[x+y*reg_s.dx] = field->at(start_p.dx+x+(start_p.dy+y)*fin_res.dx );
        }
    }

    std::vector<float> result;
    for( int y = 0; y < fin_res.dy; ++y )
    {
        for( int x = 0; x < fin_res.dx; ++x )
        {
            Point2Df indXY = { float(x)/pix_xShift, float(y)/pix_yShift };
            try {
                result.push_back( get_bicubic_interpolated_val( &thinned_zoomed_field, reg_s, indXY ) );
            } catch (const std::out_of_range& e) {
                std::cerr << "error: "  << e.what() << ",   index: " << x << ", " << y << "   " <<
                             " indXY: " << indXY.dx << ", " << indXY.dy << std::endl;
            }
        }
    }
    return  result;
}

std::vector<float> ModifyField::interpolate_field( const std::vector<float> *field, const Point2Di cur_res,
                                                   const Point2Di fin_res, interpolation inter )
{
    std::vector<float> result;
    if( (fin_res.dx)%cur_res.dx == 0 && (fin_res.dy)%cur_res.dy == 0 )
    {
        pix_xShift = fin_res.dx / cur_res.dx;
        pix_yShift = fin_res.dy / cur_res.dy;
    }
    else
    {
        pix_xShift = std::ceil(fin_res.dx / cur_res.dx);
        pix_yShift = std::ceil(fin_res.dy / cur_res.dy);
    }

    for( int y = 0; y < fin_res.dy; ++y )
    {
        for( int x = 0; x < fin_res.dx; ++x )
        {
            Point2Df indXY = { float(x)/pix_xShift, float(y)/pix_yShift };
            try {
                if( inter == interpolation::BILINEAR )
                    result.push_back( get_bilinear_interpolated_val( field, cur_res, indXY ) );
                else if( inter == interpolation::BICUBIC )
                    result.push_back( get_bicubic_interpolated_val( field, cur_res, indXY ) );
                else
                {
                    std::cerr << "Error: <interpolation inter> Unknown option passsed! " << std::endl;
                    result.clear();
                    return result;
                }
            } catch (const std::out_of_range& e) {
                std::cerr << "error: "  << e.what() << ",   index: " << x << ", " << y << "   " <<
                             " indXY: " << indXY.dx << ", " << indXY.dy << std::endl;
            }
        }
    }
    return  result;
}

std::vector<float> ModifyField::diff_fields(const std::vector<float> *field1,
                                            const std::vector<float> *field2, float multi )
{
    std::vector<float> result;
    for(int i = 0; i < field1->size(); i++)
         result.push_back( std::abs( std::abs(field1->at(i)) - field2->at(i) ) * multi); // multi = 10000.0

    return result;
}

} //namespace hfrep2D
