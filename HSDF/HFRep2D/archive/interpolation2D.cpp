#include "interpolation2D.h"

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

float ModifyField::get_bicubic_interpolated_val(const std::vector<float> *DT, glm::i32vec2 res, glm::f32vec2 indXY)
{
    int x = clipWithBounds( int(indXY.x), 0, res.x-3 );
    int y = clipWithBounds( int(indXY.y), 0, res.y-3 );

    float f00 = DT->at(x+y*res.x);
    float f10 = DT->at((x+1)+y*res.x);
    float f01 = DT->at(x+(y+1)*res.x);
    float f11 = DT->at((x+1)+(y+1)*res.x);

    float fx00 = ( DT->at((x+1)+y*res.x)     - DT->at(x+y*res.x) );
    float fx10 = ( DT->at((x+2)+y*res.x)     - DT->at((x+1)+y*res.x) );
    float fx01 = ( DT->at((x+1)+(y+1)*res.x) - DT->at(x+(y+1)*res.x) );
    float fx11 = ( DT->at((x+2)+(y+1)*res.x) - DT->at((x+1)+(y+1)*res.x) );

    float fy00 = ( DT->at(x+(y+1)*res.x)     - DT->at(x+y*res.x) );
    float fy10 = ( DT->at((x+1)+(y+1)*res.x) - DT->at((x+1)+y*res.x) );
    float fy01 = ( DT->at(x+(y+2)*res.x)     - DT->at(x+(y+1)*res.x) );
    float fy11 = ( DT->at((x+1)+(y+2)*res.x) - DT->at((x+1)+(y+1)*res.x) );

    float fxy00_1 = ( DT->at((x+1)+(y+1)*res.x) - DT->at((x+1)+y*res.x) );
    float fxy00_2 = ( DT->at(x+(y+1)*res.x)     - DT->at(x+y*res.x));
    float fxy00   = ( fxy00_1 - fxy00_2 );

    float fxy10_1 = ( DT->at((x+2)+(y+1)*res.x) - DT->at((x+2)+y*res.x) );
    float fxy10_2 = ( DT->at((x+1)+(y+1)*res.x) - DT->at((x+1)+y*res.x) );
    float fxy10   = ( fxy10_1 - fxy10_2 );

    float fxy01_1 = ( DT->at((x+1)+(y+2)*res.x) - DT->at((x+1)+(y+1)*res.x) );
    float fxy01_2 = ( DT->at(x+(y+2)*res.x)     - DT->at(x+(y+1)*res.x) );
    float fxy01   = ( fxy01_1 - fxy01_2 );

    float fxy11_1 = ( DT->at((x+2)+(y+2)*res.x) - DT->at((x+2)+(y+1)*res.x) );
    float fxy11_2 = ( DT->at((x+1)+(y+2)*res.x) - DT->at((x+1)+(y+1)*res.x) );
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

    float xN = indXY.x - std::floor(indXY.x);
    float yN = indXY.y - std::floor(indXY.y);

    float result = a00 + yN*a01 + yN*yN*a02 + std::pow(yN, 3.0f)*a03 + xN*a10 + xN*yN*a11 +
                   xN*yN*yN*a12 + xN*std::pow(yN, 3.0f)*a13 + xN*xN*a20 + xN*xN*yN*a21 +
                   std::pow(xN*yN, 2.0f)*a22 + yN*std::pow(xN*yN, 2.0f)*a23 + std::pow(xN, 3.0f)*a30 +
                   std::pow(xN, 3.0f)*yN*a31 + xN*std::pow(xN*yN, 2.0f)*a32 + std::pow(xN*yN, 3.0f)*a33 ;

    return result;
}

float ModifyField::get_bilinear_interpolated_val(const std::vector<float> *DT, glm::i32vec2 res, glm::f32vec2 indXY)
{
    float result;

    float k_x1 = static_cast<float>( std::floor(indXY.x + 1.0f) - indXY.x );
    float k_x2 = static_cast<float>( indXY.x - std::floor(indXY.x) );
    float k_y1 = static_cast<float>( std::floor(indXY.y + 1.0f) - indXY.y );
    float k_y2 = static_cast<float>( indXY.y - std::floor(indXY.y) );

    int x = clipWithBounds( int( indXY.x ), 0, res.x - 2);
    int y = clipWithBounds( int( indXY.y ), 0, res.y - 2);

    float fxy1 = k_x1 * DT->at(x+y*res.x)     + k_x2 * DT->at(x+1+y*res.x);
    float fxy2 = k_x1 * DT->at(x+(y+1)*res.x) + k_x2 * DT->at(x+1 + (y+1)*res.x);
    result = k_y1 * fxy1 + k_y2 * fxy2;

    return result;
}

std::vector<float> ModifyField::zoom_field( const std::vector<float> *field, const glm::i32vec2 start_p,
                                            const glm::i32vec2 reg_s, const glm::i32vec2 fin_res)
{
    std::vector<float> thinned_zoomed_field;
    if( (fin_res.x)%reg_s.x == 0 && (fin_res.y)%reg_s.y == 0 )
    {
        pix_xShift = fin_res.x / reg_s.x;
        pix_yShift = fin_res.y / reg_s.y;
    }
    else
    {
        pix_xShift = std::ceil(fin_res.x / reg_s.x);
        pix_yShift = std::ceil(fin_res.y / reg_s.y);
    }

    thinned_zoomed_field.resize(reg_s.x*reg_s.y);
    std::fill(thinned_zoomed_field.begin(), thinned_zoomed_field.end(), -10000.0f);

    for(int y = 0; y < reg_s.y; y++)
    {
        for(int x = 0; x < reg_s.x; x++)
        {
            thinned_zoomed_field[x+y*reg_s.x] = field->at(start_p.x+x+(start_p.y+y)*fin_res.x );
        }
    }

    std::vector<float> result;
    for( int y = 0; y < fin_res.y; ++y )
    {
        for( int x = 0; x < fin_res.x; ++x )
        {
            glm::f32vec2 indXY = { float(x)/pix_xShift, float(y)/pix_yShift };
            try {
                result.push_back( get_bicubic_interpolated_val( &thinned_zoomed_field, reg_s, indXY ) );
            } catch (const std::out_of_range& e) {
                std::cerr << "error: "  << e.what() << ",   index: " << x << ", " << y << "   " <<
                             " indXY: " << indXY.x << ", " << indXY.y << std::endl;
            }
        }
    }
    return  result;
}

std::vector<float> ModifyField::interpolate_field(const std::vector<float> *field, const glm::i32vec2 cur_res,
                                                   const glm::i32vec2 fin_res, interpolation inter )
{
    std::vector<float> result;
    if( (fin_res.x)%cur_res.x == 0 && (fin_res.y)%cur_res.y == 0 )
    {
        pix_xShift = fin_res.x / cur_res.x;
        pix_yShift = fin_res.y / cur_res.y;
    }
    else
    {
        pix_xShift = std::ceil(fin_res.x / cur_res.x);
        pix_yShift = std::ceil(fin_res.y / cur_res.y);
    }

    for( int y = 0; y < fin_res.y; ++y )
    {
        for( int x = 0; x < fin_res.x; ++x )
        {
            glm::f32vec2 indXY = { float(x)/pix_xShift, float(y)/pix_yShift };
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
                             " indXY: " << indXY.x << ", " << indXY.y << std::endl;
            }
        }
    }
    return  result;
}

std::vector<float> ModifyField::diff_fields(const std::vector<float> *field1,
                                            const std::vector<float> *field2, float multi )
{
    std::vector<float> result;
    for(size_t i = 0; i < field1->size(); i++)
         result.push_back( std::abs( std::abs(field1->at(i)) - std::abs(field2->at(i))) * multi); // multi = 10000.0

    return result;
}

float ModifyField::calcAverageFieldError(const std::vector<float> *field1, const std::vector<float> *field2, float *inside, float *outside)
{
    float averageError = 0.0, averageErrorIn = 0.0, averageErrorOut = 0.0;
    int in = 0, out = 0;

    for(size_t i = 0; i < field1->size(); i++)
    {
         averageError += std::abs( std::abs(field1->at(i)) - std::abs(field2->at(i)));
         if( field1->at(i) < 0.0f && field2->at(i) < 0.0f )
         {
             averageErrorOut += std::abs( std::abs(field1->at(i)) - std::abs(field2->at(i)));
             out++;
         }
         if( field1->at(i) >= 0.0f && field2->at(i) >= 0.0f )
         {
             averageErrorIn += std::abs( std::abs(field1->at(i)) - std::abs(field2->at(i)));
             in++;
         }
    }

    *inside  = averageErrorIn / in;
    *outside = averageErrorOut / out;

    return averageError / field1->size();
}

} //namespace hfrep2D
