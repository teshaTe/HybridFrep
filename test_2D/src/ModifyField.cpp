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

/*std::vector<double> ModifyField::generate_b_spline_interpolated_field(const std::vector<double> *DT, int resX, int resY, int shiftX, int shiftY)
{




    return ;
}*/

std::vector<double> ModifyField::generate_bicubic_interpolated_field(const std::vector<double> *DT, int resX, int resY, int shiftX, int shiftY)
{
    std::vector<double> fin_field;
    fin_field.resize( (resX - shiftX) * (resY - shiftY) );

    for( int y = 0; y < resY-2*shiftY; y+=shiftY )
    {
        for( int x = 0; x < resX-2*shiftX; x+=shiftX )
        {
            double f00 = DT->at(x+y*resX);
            double f10 = DT->at((x+shiftX)+y*resX);
            double f01 = DT->at(x+(y+shiftY)*resX);
            double f11 = DT->at((x+shiftX)+(y+shiftY)*resX);

            double fx00 = ( DT->at((x+shiftX)+y*resX)            - DT->at(x+y*resX) ) / shiftX;
            double fx10 = ( DT->at((x+2*shiftX)+y*resX)          - DT->at((x+shiftX)+y*resX) ) / shiftX;
            double fx01 = ( DT->at((x+shiftX)+(y+shiftY)*resX)   - DT->at(x+(y+shiftY)*resX) ) / shiftX;
            double fx11 = ( DT->at((x+2*shiftX)+(y+shiftY)*resX) - DT->at((x+shiftX)+(y+shiftY)*resX) ) / shiftX;

            double fy00 = ( DT->at(x+(y+shiftY)*resX)            - DT->at(x+y*resX) ) / shiftY;
            double fy10 = ( DT->at((x+shiftX)+(y+shiftY)*resX)   - DT->at((x+shiftX)+y*resX) ) / shiftY;
            double fy01 = ( DT->at(x+(y+2*shiftY)*resX)          - DT->at(x+(y+shiftY)*resX) ) / shiftY;
            double fy11 = ( DT->at((x+shiftX)+(y+2*shiftY)*resX) - DT->at((x+shiftX)+(y+shiftY)*resX) ) / shiftY;

            double fxy00_1 = ( DT->at((x+shiftX)+(y+shiftY)*resX)  - DT->at((x+shiftX)+y*resX) ) / shiftY;
            double fxy00_2 = ( DT->at(x+(y+shiftY)*resX)           - DT->at(x+y*resX)) / shiftY;
            double fxy00   = ( 1.0/shiftX ) * ( fxy00_1 - fxy00_2 );

            double fxy10_1 = ( DT->at((x+2*shiftX)+(y+shiftY)*resX) - DT->at((x+2*shiftX)+y*resX) ) / shiftY;
            double fxy10_2 = ( DT->at((x+shiftX)+(y+shiftY)*resX)   - DT->at((x+shiftX)+y*resX) ) / shiftY;
            double fxy10   = ( 1.0/shiftX ) * ( fxy10_1 - fxy10_2 );

            double fxy01_1 = ( DT->at((x+shiftX)+(y+2*shiftY)*resX) - DT->at((x+shiftX)+(y+shiftY)*resX) ) / shiftY;
            double fxy01_2 = ( DT->at(x+(y+2*shiftY)*resX)          - DT->at(x+(y+shiftY)*resX) ) / shiftY;
            double fxy01   = ( 1.0/shiftX ) * ( fxy01_1 - fxy01_2 );

            double fxy11_1 = ( DT->at((x+2*shiftX)+(y+2*shiftY)*resX) - DT->at((x+2*shiftX)+(y+shiftY)*resX) ) / shiftY;
            double fxy11_2 = ( DT->at((x+shiftX)+(y+2*shiftY)*resX)   - DT->at((x+shiftX)+(y+shiftY)*resX) ) / shiftY;
            double fxy11   = ( 1.0/shiftX ) * ( fxy11_1 - fxy11_2 );

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

            for( int y_in = 0; y_in <= shiftY; y_in++ )
            {
                for( int x_in = 0; x_in <= shiftX; x_in++ )
                {
                    if( DT->at(x+x_in+(y+y_in)*resX) == -10000.0f )
                    {

                        double result = a00 + y_in*a01 + y_in*y_in*a02 + std::pow(y_in, 3.0)*a03 + x_in*a10 + x_in*y_in*a11 +
                                        x_in*y_in*y_in*a12 + x_in*std::pow(y_in, 3.0)*a13 + x_in*x_in*a20 + x_in*x_in*y_in*a21 +
                                        std::pow(x_in*y_in, 2.0)*a22 + y_in*std::pow(x_in*y_in, 2.0)*a23 + std::pow(x_in, 3.0)*a30 +
                                        std::pow(x_in, 3.0)*y_in*a31 + x_in*std::pow(x_in*y_in, 2.0)*a32 + std::pow(x_in*y_in, 3.0)*a33 ;

                        fin_field[x+x_in + (y+y_in)*(resX-shiftX)] = std::abs(result);
                        if(result < 0)
                            int stop = 0;
                    }
                    else
                        fin_field[x+x_in + (y+y_in)*(resX-shiftX)] = DT->at(x+x_in+(y+y_in)*resX);
                }
            }
        }
    }

    return fin_field;
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
        std::cerr << "Uneven resolution attitude between zoomed res and region res is not supported yet! " << std::endl;
        return *field;
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
                result.push_back( get_bilinear_interpolated_val( &thinned_zoomed_field, reg_s, indXY ) );
            } catch (const std::out_of_range& e) {
                std::cerr << "error: " << e.what() << ",   index: " << x << ", " << y << "   " <<
                             " indXY: " << indXY[0] << ", " << indXY[1] << std::endl;
            }
        }
    }
    return  result;
}

std::vector<double> ModifyField::interpolate_field(const std::vector<double> *field, const cv::Vec2i cur_res, const cv::Vec2i fin_res)
{
    std::vector<double> result;
    if( (fin_res[0])%cur_res[0] == 0 && (fin_res[1])%cur_res[1] == 0 )
    {
        pix_xShift = fin_res[0] / cur_res[0];
        pix_yShift = fin_res[1] / cur_res[1];
    }
    else
    {
        std::cerr << "Uneven resolution attitude between zoomed res and region res is not supported yet! " << std::endl;
        return *field;
    }

    for( int y = 0; y < fin_res[1]; ++y )
    {
        for( int x = 0; x < fin_res[0]; ++x )
        {
            cv::Vec2f indXY = { float(x)/pix_xShift, float(y)/pix_yShift };
            try {
                result.push_back( get_bilinear_interpolated_val( field, cur_res, indXY ) );
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
