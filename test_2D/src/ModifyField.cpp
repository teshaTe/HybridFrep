#include "include/ModifyField.h"

#include <iostream>

namespace modified_field {

std::vector<double> ModifyField::smooth_field( const std::vector<double> *DT, int resX, int resY)
{
    std::vector<double> inter_result;
    for(int y = 0; y < resY-1; y++)
    {
        for(int x = 0; x < resX-1; x++)
        {
            double u = static_cast<double>(x)/resX;
            double v = static_cast<double>(y)/resY;
            double u_opp = 1.0 - u;
            double v_opp = 1.0 - v;

            double result = ( DT->at(x+    y*resX) * u_opp + DT->at((x+1)+    y*resX) * u ) * v_opp +
                            ( DT->at(x+(y+1)*resX) * u_opp + DT->at((x+1)+(y+1)*resX) * u ) * v;

            inter_result.push_back(result);
        }
    }
    return inter_result;
}

// Value of k-th B-Spline basic function at t.
double ModifyField::Bspline(size_t k, double t) {
    assert(0 <= t && t < 1);
    assert(k < 4);

    switch (k) {
        case 0:
            return (t * (t * (-t + 3) - 3) + 1) / 6;
        case 1:
            return (t * t * (3 * t - 6) + 4) / 6;
        case 2:
            return (t * (t * (-3 * t + 3) + 3) + 1) / 6;
        case 3:
            return t * t * t / 6;
        default:
            return 0;
    }
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

std::vector<double> ModifyField::generate_bilinearly_interpolated_field(const std::vector<double> *DT, int resX, int resY, int shiftX, int shiftY)
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

                        double fxy1 = k_x1 * DT->at(x+y*resX) + k_x2 * DT->at(x+shiftX+y*resX);
                        double fxy2 = k_x1 * DT->at(x+(y+shiftY)*resX) + k_x2 * DT->at(x+shiftX + (y+shiftY)*resX);
                        double result = k_y1 * fxy1 + k_y2 * fxy2;

                        fin_field[x+x_in + (y+y_in)*resX] = result;
                    }
                }
            }
        }
    }

    return fin_field;
}

std::vector<double> ModifyField::zoom_in_field(const std::vector<double> *field, const cv::Point2i start_p,
                                               const cv::Vec2i reg_s, const cv::Vec2i fin_res, int b_sh, interpolation inter)
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

    int new_width, new_height, new_reg_w, new_reg_h;

    if( inter == interpolation::BILINEAR )
    {
        new_width  = fin_res[0]+b_sh;
        new_height = fin_res[1]+b_sh;
        new_reg_w  = reg_s[0]+b_sh;
        new_reg_h  = reg_s[1]+b_sh;
        thinned_zoomed_field.resize( static_cast<size_t> ( new_width * new_height ) );
    }
    else if ( inter == interpolation::BICUBIC )
    {
        new_width  = fin_res[0]+pix_xShift+b_sh;
        new_height = fin_res[1]+pix_yShift+b_sh;
        new_reg_w  = reg_s[0]+2*b_sh;
        new_reg_h  = reg_s[1]+2*b_sh;
        thinned_zoomed_field.resize( static_cast<size_t> ( new_width * new_height ) );
    }
    else
    {
        std::cerr << "ERROR: Unknown interpolation type! <interpolation inter> option. input field will be returned." << std::endl;
        return *field;
    }

    std::fill(thinned_zoomed_field.begin(), thinned_zoomed_field.end(), -10000.0);

    int ind_x = 0;
    int ind_y = 0;

    for(int y = 0; y < new_reg_h; y++)
    {
        for(int x = 0; x < new_reg_w; x++)
        {
            thinned_zoomed_field[ind_x+ind_y*new_width] = field->at(start_p.x+x+(start_p.y+y)*(fin_res[0]+b_sh) );
            ind_x += pix_xShift;
        }
        ind_x = 0;
        ind_y += pix_yShift;
    }

    std::vector<double> result;
    if( inter == interpolation::BILINEAR )
        result = generate_bilinearly_interpolated_field( &thinned_zoomed_field, new_width, new_height, pix_xShift, pix_yShift );
    //else if(inter == interpolation::BSPLINE )
        //result = generate_b_spline_interpolated_field( &thinned_zoomed_field, new_width, new_height, pix_xShift, pix_yShift );
    else
        result = generate_bicubic_interpolated_field( &thinned_zoomed_field, new_width, new_height, pix_xShift, pix_yShift );

    return result;
}

std::vector<double> ModifyField::finalize_field( const std::vector<double> *field, int resX, int resY, int b_sh )
{
    std::vector<double> result;

    for(int y = 0; y < resY+b_sh; y++)
    {
        for(int x = 0; x < resX+b_sh; x++)
        {
            if( x < resX )
            {
                if( y < resY)
                    result.push_back( field->at(x+y*(resX+b_sh) ));
            }
        }
    }

    return result;
}

} //namespace modifed_field
