#include "include/ModifyField.h"

#include <iostream>
#include <stdexcept>

namespace modified_field {

std::vector<double> ModifyField::smooth_field( const std::vector<double> *DT, int resX, int resY)
{
    std::vector<double> inter_result; inter_result.reserve( DT->size());
    for(int y = 0; y < resY; y++)
    {
        for(int x = 0; x < resX; x++)
        {
            double u = static_cast<double>(x)/resX;
            double v = static_cast<double>(y)/resY;
            double u_opp = 1.0 - u;
            double v_opp = 1.0 - v;

            int x1 = clipWithBounds(x, 0, resX -2 );
            int y1 = clipWithBounds(y, 0, resX -2 );

            double result = ( DT->at(x1+    y1*resX) * u_opp + DT->at((x1+1)+    y1*resX) * u ) * v_opp +
                            ( DT->at(x1+(y1+1)*resX) * u_opp + DT->at((x1+1)+(y1+1)*resX) * u ) * v;

            inter_result.push_back(result);
        }
    }
    return inter_result;
}

double ModifyField::get_bicubic_interpolated_val(const std::vector<double> *DT, cv::Vec2i res, cv::Vec2f indXY)
{
    int x = clipWithBounds( int(indXY[0]), 0, res[0]-3 );
    int y = clipWithBounds( int(indXY[1]), 0, res[1]-3 );

    double f00 = DT->at(x+y*res[0]);
    double f10 = DT->at((x+1)+y*res[0]);
    double f01 = DT->at(x+(y+1)*res[0]);
    double f11 = DT->at((x+1)+(y+1)*res[0]);

    double fx00 = ( DT->at((x+1)+y*res[0])     - DT->at(x+y*res[0]) );
    double fx10 = ( DT->at((x+2)+y*res[0])     - DT->at((x+1)+y*res[0]) );
    double fx01 = ( DT->at((x+1)+(y+1)*res[0]) - DT->at(x+(y+1)*res[0]) );
    double fx11 = ( DT->at((x+2)+(y+1)*res[0]) - DT->at((x+1)+(y+1)*res[0]) );

    double fy00 = ( DT->at(x+(y+1)*res[0])     - DT->at(x+y*res[0]) );
    double fy10 = ( DT->at((x+1)+(y+1)*res[0]) - DT->at((x+1)+y*res[0]) );
    double fy01 = ( DT->at(x+(y+2)*res[0])     - DT->at(x+(y+1)*res[0]) );
    double fy11 = ( DT->at((x+1)+(y+2)*res[0]) - DT->at((x+1)+(y+1)*res[0]) );

    double fxy00_1 = ( DT->at((x+1)+(y+1)*res[0]) - DT->at((x+1)+y*res[0]) );
    double fxy00_2 = ( DT->at(x+(y+1)*res[0])     - DT->at(x+y*res[0]));
    double fxy00   = ( fxy00_1 - fxy00_2 );

    double fxy10_1 = ( DT->at((x+2)+(y+1)*res[0]) - DT->at((x+2)+y*res[0]) );
    double fxy10_2 = ( DT->at((x+1)+(y+1)*res[0]) - DT->at((x+1)+y*res[0]) );
    double fxy10   = ( fxy10_1 - fxy10_2 );

    double fxy01_1 = ( DT->at((x+1)+(y+2)*res[0]) - DT->at((x+1)+(y+1)*res[0]) );
    double fxy01_2 = ( DT->at(x+(y+2)*res[0])     - DT->at(x+(y+1)*res[0]) );
    double fxy01   = ( fxy01_1 - fxy01_2 );

    double fxy11_1 = ( DT->at((x+2)+(y+2)*res[0]) - DT->at((x+2)+(y+1)*res[0]) );
    double fxy11_2 = ( DT->at((x+1)+(y+2)*res[0]) - DT->at((x+1)+(y+1)*res[0]) );
    double fxy11   = ( fxy11_1 - fxy11_2 );

    double a00 =  f00;
    double a10 =  fx00;
    double a01 =  fy00;
    double a11 =  fxy00;
    double a20 = -3.0*f00   + 3.0*f10   - 2.0*fx00  - fx10;
    double a02 = -3.0*f00   + 3.0*f01   - 2.0*fy00  - fy01;
    double a21 = -2.0*fxy00 - fxy10     - 3.0*fy00  + 3.0*fy10;
    double a12 = -3.0*fx00  + 3.0*fx01  - 2.0*fxy00 - fxy01;
    double a22 =  9.0*f00   - 9.0*f01   - 9.0*f10   + 9.0*f11 + 6.0*fx00 - 6.0*fx01 + 3.0*fx10 - 3.0*fx11 +
                  4.0*fxy00 + 2.0*fxy01 + 2.0*fxy10 + fxy11   + 6.0*fy00 + 3.0*fy01 - 6.0*fy10 - 3.0*fy11;
    double a30 =  2.0*f00   - 2.0*f10   + fx00      + fx10;
    double a03 =  2.0*f00   - 2.0*f01   + fy00      + fy01;
    double a31 =  fxy00     + fxy10     + 2.0*fy00  - 2.0*fy10;
    double a13 =  2.0*fx00  - 2.0*fx01  + fxy00     + fxy01;
    double a23 = -6.0*f00   + 6.0*f01   + 6.0*f10   - 6.0*f11 - 4.0*fx00 + 4.0*fx01 - 2.0*fx10 + 2.0*fx11 -
                  2.0*fxy00 - 2.0*fxy01 - fxy10     - fxy11   - 3.0*fy00 - 3.0*fy01 + 3.0*fy10 + 3.0*fy11;
    double a32 = -6.0*f00   + 6.0*f01   + 6.0*f10   - 6.0*f11 - 3.0*fx00 + 3.0*fx01 - 3.0*fx10 + 3.0*fx11 -
                  2.0*fxy00 - fxy01     - 2.0*fxy10 - fxy11   - 4.0*fy00 - 2.0*fy01 + 4.0*fy10 + 2.0*fy11;
    double a33 =  4.0*f00   - 4.0*f01   - 4.0*f10   + 4.0*f11 + 2.0*fx00 - 2.0*fx01 + 2.0*fx10 - 2.0*fx11 +
                  fxy00     + fxy01     + fxy10     + fxy11   + 2.0*fy00 + 2.0*fy01 - 2.0*fy10 - 2.0*fy11;

    double xN = indXY[0] - std::floor(indXY[0]);
    double yN = indXY[1] - std::floor(indXY[1]);

    double result = a00 + yN*a01 + yN*yN*a02 + std::pow(yN, 3.0)*a03 + xN*a10 + xN*yN*a11 +
                    xN*yN*yN*a12 + xN*std::pow(yN, 3.0)*a13 + xN*xN*a20 + xN*xN*yN*a21 +
                    std::pow(xN*yN, 2.0)*a22 + yN*std::pow(xN*yN, 2.0)*a23 + std::pow(xN, 3.0)*a30 +
                    std::pow(xN, 3.0)*yN*a31 + xN*std::pow(xN*yN, 2.0)*a32 + std::pow(xN*yN, 3.0)*a33 ;

    return result;
}


double ModifyField::get_bilinear_interpolated_val(const std::vector<double> *DT, cv::Vec2i res, cv::Vec2f indXY)
{
    double result;

    double k_x1 = static_cast<double>( std::floor(indXY[0] + 1.0f) - indXY[0] );
    double k_x2 = static_cast<double>( indXY[0] - std::floor(indXY[0]) );
    double k_y1 = static_cast<double>( std::floor(indXY[1] + 1.0f) - indXY[1] );
    double k_y2 = static_cast<double>( indXY[1] - std::floor(indXY[1]) );

    int x = clipWithBounds( int( indXY[0] ), 0, res[0] - 2);
    int y = clipWithBounds( int( indXY[1] ), 0, res[1] - 2);

    double fxy1 = k_x1 * DT->at(x+y*res[0])     + k_x2 * DT->at(x+1+y*res[0]);
    double fxy2 = k_x1 * DT->at(x+(y+1)*res[0]) + k_x2 * DT->at(x+1 + (y+1)*res[0]);
    result = k_y1 * fxy1 + k_y2 * fxy2;

    return result;
}

std::vector<double> ModifyField::zoom_field( const std::vector<double> *field, const cv::Point2i start_p,
                                             const cv::Vec2i reg_s, const cv::Vec2i fin_res)
{
    std::vector<double> thinned_zoomed_field;
    if( (fin_res[0])%reg_s[0] == 0 && (fin_res[1])%reg_s[1] == 0 )
    {
        pix_xShift = fin_res[0] / reg_s[0];
        pix_yShift = fin_res[1] / reg_s[1];
    }
    else
    {
        pix_xShift = std::ceil(fin_res[0] / reg_s[0]);
        pix_yShift = std::ceil(fin_res[1] / reg_s[1]);
        //std::cerr << "Uneven resolution attitude between zoomed res and region res is not supported yet! " << std::endl;
        //return *field;
    }

    thinned_zoomed_field.resize(reg_s[0]*reg_s[1]);
    std::fill(thinned_zoomed_field.begin(), thinned_zoomed_field.end(), -10000.0);

    for(int y = 0; y < reg_s[1]; y++)
    {
        for(int x = 0; x < reg_s[0]; x++)
        {
            thinned_zoomed_field[x+y*reg_s[0]] = field->at(start_p.x+x+(start_p.y+y)*fin_res[0] );
        }
    }

    std::vector<double> result;
    for( int y = 0; y < fin_res[1]; ++y )
    {
        for( int x = 0; x < fin_res[0]; ++x )
        {
            cv::Vec2f indXY = { float(x)/pix_xShift, float(y)/pix_yShift };
            try {
                result.push_back( get_bicubic_interpolated_val( &thinned_zoomed_field, reg_s, indXY ) );
            } catch (const std::out_of_range& e) {
                std::cerr << "error: " << e.what() << ",   index: " << x << ", " << y << "   " <<
                             " indXY: " << indXY[0] << ", " << indXY[1] << std::endl;
            }
        }
    }
    return  result;
}

std::vector<double> ModifyField::interpolate_field(const std::vector<double> *field, const cv::Vec2i cur_res,
                                                   const cv::Vec2i fin_res, interpolation inter )
{
    std::vector<double> result;
    if( (fin_res[0])%cur_res[0] == 0 && (fin_res[1])%cur_res[1] == 0 )
    {
        pix_xShift = fin_res[0] / cur_res[0];
        pix_yShift = fin_res[1] / cur_res[1];
    }
    else
    {
        pix_xShift = std::ceil(fin_res[0] / cur_res[0]);
        pix_yShift = std::ceil(fin_res[1] / cur_res[1]);
        //std::cerr << "Uneven resolution attitude between zoomed res and region res is not supported yet! " << std::endl;
        //return *field;
    }

    for( int y = 0; y < fin_res[1]; ++y )
    {
        for( int x = 0; x < fin_res[0]; ++x )
        {
            cv::Vec2f indXY = { float(x)/pix_xShift, float(y)/pix_yShift };
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
                std::cerr << "error: " << e.what() << ",   index: " << x << ", " << y << "   " <<
                             " indXY: " << indXY[0] << ", " << indXY[1] << std::endl;
            }
        }
    }
    return  result;
}

std::vector<double> ModifyField::diff_fields(const std::vector<double> *field1, const std::vector<double> *field2, double multi )
{
    std::vector<double> result;
    for(int i = 0; i < field1->size(); i++)
         result.push_back( std::abs( std::abs(field1->at(i)) - field2->at(i) ) * multi); // multi = 10000.0

    return result;
}

} //namespace modifed_field
