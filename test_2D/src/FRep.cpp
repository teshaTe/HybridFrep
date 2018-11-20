#include "include/FRep.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <fstream>
#include <cmath>

function_rep::HybrydFunctionRep::HybrydFunctionRep(geometry geo, geometry_params params,
                                                   int res_x, int res_y, int ddt_sh, int im_type): start_p (params.start_P.x/res_x, params.start_P.y/res_y),
                                                                                                   R       (params.rad/res_x),
                                                                                                   but_u   (params.but_U),
                                                                                                   bl_phi1 (params.blob_phi1),
                                                                                                   bl_phi2 (params.blob_phi2),
                                                                                                   bl_a(params.blob_A),
                                                                                                   bl_b(params.blob_B),
                                                                                                   resolution_x (res_x),
                                                                                                   resolution_y (res_y),
                                                                                                   cv_im_type   (im_type)
{
    generate_frep( geo );
    FRep_im = get_FRep_im( &FRep_vec );
    DT      = std::make_shared<distance_transform::DistanceField>( FRep_im, ddt_sh );
    drawF   = std::make_shared<draw::DrawField>();
    dist_tr = DT.get()->get_DDT();

    if(ddt_sh > 0)
    {
        std::vector<double> tmp_field = DT.get()->smooth_field(&dist_tr, res_x, res_y);
        sm_dist_tr = DT.get()->finalize_field(&tmp_field, res_x,res_y);
    }
    else
        sm_dist_tr = dist_tr;

    generate_hfrep( &HFRep_vec, FRep_vec, &sm_dist_tr, "hfrep");

    std::vector<uchar> uchar_frep1, uchar_frep;
    HFRep_im    = drawF.get()->convert_field_to_image( &uchar_frep, &HFRep_vec, resolution_x, resolution_y);
    check_HFrep( HFRep_vec );

#ifdef USE_DEBUG_INFO_FREP
    uchar_frep1.clear();
    drawF.get()->draw_field( &uchar_frep1, &sm_dist_tr, resolution_x, resolution_y, "ddt1" );
    uchar_frep1.clear();
    drawF.get()->draw_grey_isolines( &uchar_frep1, &sm_dist_tr, resolution_x, resolution_y, "ddt1" );

    uchar_frep1.clear();
    drawF.get()->draw_field( &uchar_frep1, &HFRep_vec, resolution_x, resolution_y, "hfrep1");
    uchar_frep1.clear();
    drawF.get()->draw_grey_isolines( &uchar_frep1, &HFRep_vec, resolution_x, resolution_y, "hfrep1");
#endif
}

void function_rep::HybrydFunctionRep::generate_frep( geometry geo, std::string file_name )
{
    if( geo == geometry::CIRCLE )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double result = R*R - ( u - start_p.x )*( u - start_p.x ) - ( v - start_p.y )*( v - start_p.y );
                FRep_vec.push_back(result);
            }
        }
    }
    else if ( geo == geometry::BLOBBY )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double blob1 = R*R/( (u - bl_a)*(u - bl_a) + (v - bl_b)*(v - bl_b));
                double blob2 = R*R*0.25/((u - bl_a-0.3)*(u - bl_a-0.3) + (v - bl_b-0.2)*(v - bl_b-0.2));
                double result = union_function(blob1, blob2);
                FRep_vec.push_back(result);
            }
        }
    }
    else if ( geo == geometry::BUTTERFLY )
    {

    }
    else
    {
        std::cerr << "ERROR: no input pre-defined geometry is specified!" << std::endl;
        FRep_vec.clear();
    }

#ifdef USE_DEBUG_INFO_FREP
    std::ofstream file;
    std::string file_n = "FREP_" + file_name + ".txt";
    file.open(file_n);
    file.precision(7);

    for (int i = 0; i < resolution_y*resolution_x; i++)
        file << FRep_vec[i] << std::endl;
#endif
}

cv::Mat function_rep::HybrydFunctionRep::get_FRep_im( const std::vector<double> *input, std::string file_name )
{
    cv::Mat result(resolution_x, resolution_y, cv_im_type);
    std::vector<uchar> frep_im;
    frep_im.resize(input->size());

    for( int i = 0; i < resolution_y*resolution_x; i++ )
    {
       if( input->at(i) < 0.0 )
           frep_im[i] = 0;
       else if ( input->at(i) > 255 )
           frep_im[i] = 255;
       else
           frep_im[i] = input->at(i)*10000;
    }

    memcpy(result.data, frep_im.data(), frep_im.size()*sizeof(uchar));

#ifdef USE_DEBUG_INFO_FREP
    std::string fin_file_name = "frep_im_" + file_name + ".png";
    cv::imwrite(fin_file_name, result);
#endif
    return result;
}

double function_rep::HybrydFunctionRep::get_step_function_val(double frep_val, int function )
{
    if( function == 1 )
    {
        double result = std::tanh( frep_val );
        return result;
    }
    else if ( function == 2 )
    {
        double result = 2.0/(1+std::exp(-2.0*frep_val/0.0001))-1.0;
        return result;
    }
    else if ( function == 3 )
    {
        double result = frep_val / std::sqrt( 1.0 + frep_val * frep_val );
        return result;
    }
    else if ( function == 4 )
    {
        double result = (4.0/M_PI) * std::tanh( std::atan( (M_PI/4.0)* frep_val ) );
        return result;
    }
    else
    {
        std::cerr << "ERROR: unknown option! [ get_step_function_val(.., <?> ) ]" << std::endl;
        exit(-1);
    }

}

void function_rep::HybrydFunctionRep::generate_hfrep(std::vector<double> *hfrep, const std::vector<double> frep,
                                                     const std::vector<double> *DistTr, std::string file_name )
{
    for(int i = 0; i < resolution_y*resolution_x; i++)
    {
        double hfrep_val = get_step_function_val( frep[i], 2 ) * DistTr->at(i);
        hfrep->push_back(hfrep_val);
    }

#ifdef USE_DEBUG_INFO_FREP
    std::ofstream file;
    std::string file_n = "HFREP_" + file_name + ".txt";
    file.open(file_n);
    file.precision(7);

    for (int i = 0; i < resolution_y*resolution_x; i++)
        file << hfrep->at(i) << std::endl;
#endif
}

void function_rep::HybrydFunctionRep::check_HFrep( std::vector<double> hfrep )
{
    int frep_zeros  = 0;
    int hfrep_zeros = 0;
    int dist_tr_zeros = 0;
    for(int i = 0; i < resolution_x*resolution_y; i++)
    {
        if( hfrep[i] == 0.0 )
            frep_zeros++;
        if( hfrep[i] == 0.0 )
            hfrep_zeros++;
        if( dist_tr[i] == 0.0 )
            dist_tr_zeros++;
    }

    if(frep_zeros == hfrep_zeros)
    {
        std::cout << "HFrep does not have additional zeros!" << std::endl;
        std::cout << "FRep zeros: " << frep_zeros << "   HFRep zeros: " << hfrep_zeros << std::endl;
    }
    else
    {
        std::cout << "HFRep has additional zeros: " << hfrep_zeros - frep_zeros << std::endl;
        std::cout << "FRep zeros: " << frep_zeros << "   HFRep zeros: " <<
                     hfrep_zeros << "   DT zeros: " << dist_tr_zeros << std::endl;
    }
}
