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

std::vector<double> ModifyField::generate_interpolated_field(const std::vector<double> *DT, int resX, int resY,
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

std::vector<double> ModifyField::zoom_in_field(const std::vector<double> *field, const cv::Point2i start_p,
                                               const cv::Vec2i reg_s, const cv::Vec2i fin_res, int b_sh )
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

    thinned_zoomed_field.resize( static_cast<size_t> ( (fin_res[0]+b_sh) * (fin_res[1]+b_sh) ) );
    std::fill(thinned_zoomed_field.begin(), thinned_zoomed_field.end(), -10000.0);

    int ind_x = 0;
    int ind_y = 0;

    for(int y = 0; y < reg_s[1]+1; y++)
    {
        for(int x = 0; x < reg_s[0]+1; x++)
        {
            thinned_zoomed_field[ind_x+ind_y*(fin_res[0]+b_sh)] = field->at(start_p.x+x+(start_p.y+y)*(fin_res[0]+b_sh) );
            ind_x += pix_xShift;
        }
        ind_x = 0;
        ind_y += pix_yShift;
    }

    std::vector<double> result = generate_interpolated_field( &thinned_zoomed_field, fin_res[0]+b_sh, fin_res[1]+b_sh, pix_xShift, pix_yShift);

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
