#include "include/FRep.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <fstream>
#include <cmath>

function_rep::HybrydFunctionRep::HybrydFunctionRep(geometry geo, geometry_params params,
                                                   int res_x, int res_y, int ddt_sh, int im_type): start_p (params.start_P.x/res_x, params.start_P.y/res_y),
                                                                                                   R       (params.rad/res_x),
                                                                                                   resolution_x (res_x),
                                                                                                   resolution_y (res_y),
                                                                                                   cv_im_type   (im_type)
{
    generate_frep( geo, res_x, res_y, 0, start_p );
    FRep_im = get_FRep_im( &FRep_vec, geo );
    DT      = std::make_shared<distance_transform::DistanceField>( FRep_im, ddt_sh );
    drawF   = std::make_shared<draw::DrawField>();
    modF    = std::make_shared<modified_field::ModifyField>();
    dist_tr = DT.get()->get_DDT();

    if(ddt_sh > 0)
        sm_dist_tr = modF.get()->smooth_field( &dist_tr, res_x+ddt_sh, res_y+ddt_sh );
    else
        sm_dist_tr = dist_tr;

    generate_hfrep( &HFRep_vec, FRep_vec, &sm_dist_tr, cv::Vec2i(res_x, res_y), "hfrep");

    std::vector<uchar> uchar_frep1, uchar_frep;
    HFRep_im    = drawF.get()->convert_field_to_image( &uchar_frep, &HFRep_vec, resolution_x, resolution_y);
    check_HFrep( HFRep_vec, "default");

#ifdef USE_DEBUG_INFO_FREP
    uchar_frep1.clear();
    drawF.get()->draw_grey_isolines( &uchar_frep1, &sm_dist_tr, resolution_x, resolution_y, "ddt1" );

    uchar_frep1.clear();
    drawF.get()->draw_grey_isolines( &uchar_frep1, &HFRep_vec, resolution_x, resolution_y, "hfrep1");
#endif
}

function_rep::HybrydFunctionRep::HybrydFunctionRep(function_rep::geometry geo, function_rep::geometry_params params,
                                                   int res_x, int res_y, int ddt_sh): start_p (params.start_P.x/res_x, params.start_P.y/res_y),
                                                                                      R       (params.rad/res_x),
                                                                                      resolution_x (res_x),
                                                                                      resolution_y (res_y)
{
    generate_frep( geo, res_x, res_y, 0, start_p );

    std::vector<double> inc_frep;
    if(ddt_sh > 0)
    {
        inc_frep = generate_frep( geo, res_x+ddt_sh, res_y+ddt_sh, 0, start_p );
        DT = std::make_shared<distance_transform::DistanceField>( inc_frep, res_x+ddt_sh, res_y+ddt_sh );
    }
    else
    {
        DT = std::make_shared<distance_transform::DistanceField>( FRep_vec, res_x, res_y );
    }

    drawF   = std::make_shared<draw::DrawField>();
    modF    = std::make_shared<modified_field::ModifyField>();
    dist_tr = DT.get()->get_DDT();

    if(ddt_sh > 0)
        sm_dist_tr = modF.get()->smooth_field( &dist_tr, res_x+ddt_sh, res_y+ddt_sh );
    else
        sm_dist_tr = dist_tr;

    generate_hfrep( &HFRep_vec, FRep_vec, &sm_dist_tr, cv::Vec2i(res_x, res_y), "hfrep");

    std::vector<uchar> uchar_frep1, uchar_frep;
    HFRep_im    = drawF.get()->convert_field_to_image( &uchar_frep, &HFRep_vec, resolution_x, resolution_y);
    check_HFrep( HFRep_vec, "default");

#ifdef USE_DEBUG_INFO_FREP
    uchar_frep1.clear();
    drawF.get()->draw_grey_isolines( &uchar_frep1, &sm_dist_tr, resolution_x, resolution_y, "ddt1" );

    uchar_frep1.clear();
    drawF.get()->draw_grey_isolines( &uchar_frep1, &HFRep_vec, resolution_x, resolution_y, "hfrep1");

    std::vector<double> diff_field; diff_field.resize( HFRep_vec.size() );
    diff_field = modF.get()->diff_fields( &HFRep_vec, &sm_dist_tr, 1000.0 );

    uchar_frep1.clear();
    drawF.get()->draw_grey_isolines( &uchar_frep1, &diff_field, 512, 512, "diff_field" );
#endif
}

std::vector<double> function_rep::HybrydFunctionRep::generate_frep(geometry geo, int res_x, int res_y,
                                                                    double z, cv::Point2d init_pos, std::string file_name )
{
    FRep_vec.clear();
    double zoom;

    if( geo == geometry::CIRCLE )
    {
        for (int y = 0; y < res_y; y++)
        {
            for(int x = 0; x < res_x; x++)
            {
                double u   = static_cast<double>(x)/res_x;
                double v   = static_cast<double>(y)/res_y;
                double shX = u - init_pos.x;
                double shY = v - init_pos.y;
                double result = R*R - shX*shX - shY*shY;
                FRep_vec.push_back(result);
            }
        }
    }
    else if ( geo == geometry::BLOBBY )
    {
        for (int y = 0; y < res_y; y++)
        {
            for(int x = 0; x < res_x; x++)
            {
                double u   = static_cast<double>(x)/res_x;
                double v   = static_cast<double>(y)/res_y;
                double shX = u - init_pos.x;
                double shY = v - init_pos.y;
                double blob1 = R*R/( shX*shX + shY*shY);
                double blob2 = R*R*0.25/((shX - 0.3)*(shX - 0.3) + (shY - 0.2)*(shY - 0.2));
                double result = union_function(blob1, blob2);
                FRep_vec.push_back(result);            }
        }
    }
    else if ( geo == geometry::BUTTERFLY )
    {
        for (int y = 0; y < res_y; y++)
        {
            for(int x = 0; x < res_x; x++)
            {
                double u   = static_cast<double>(x)/res_x;
                double v   = static_cast<double>(y)/res_y;
                double shX = u - init_pos.x;
                double shY = v - init_pos.y;

                if( z == 0.0 )
                    zoom = 4.0;
                else
                    zoom = z;

                double result = std::pow(shX*zoom, 6.0) + std::pow(shY*zoom, 6.0) - shX*shX*zoom*zoom;
                FRep_vec.push_back(result);            }
        }
    }
    else if ( geo == geometry::HEART )
    {
        for (int y = 0; y < res_y; y++)
        {
            for(int x = 0; x < res_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - init_pos.x;
                double shY = v - init_pos.y;

                if( z == 0.0 )
                    zoom = 3.0;
                else
                    zoom = z;

                double result = std::pow( zoom*zoom*shX*shX + zoom*zoom*shY*shY - 1.0, 3.0 ) -
                                          zoom*zoom*shX*shX*std::pow( zoom*shY, 3.0 );
                FRep_vec.push_back(result);            }
        }
    }
    else if( geo == geometry::CHAIR)
    {
        for (int y = 0; y < res_y; y++)
        {
            for(int x = 0; x < res_x; x++)
            {
                double u   = static_cast<double>(x)/res_x;
                double v   = static_cast<double>(y)/res_y;
                double shX = u - init_pos.x;
                double shY = v - init_pos.y;
                double a = 0.8;
                double b = 0.6;
                double k = 2.0;

                if( z == 0.0 )
                    zoom = 6.0;
                else
                    zoom = z;

                double result = std::pow( zoom*zoom*shX*shX + zoom*shY*shY - a*k*k, 2.0) -
                                          b*(k*k - zoom*zoom*2.0*shX*shX)*(k*k - zoom*zoom*2.0*shY*shY);
                FRep_vec.push_back(result);            }
        }
    }
    else if( geo == geometry::BORG)
    {
        for (int y = 0; y < res_y; y++)
        {
            for(int x = 0; x < res_x; x++)
            {
                double u   = static_cast<double>(x)/res_x;
                double v   = static_cast<double>(y)/res_y;
                double shX = u - init_pos.x;
                double shY = v - init_pos.y;

                if( z == 0.0 )
                    zoom = 9.0;
                else
                    zoom = z;
                double result = std::sin(zoom*zoom*shX*shY);
                FRep_vec.push_back(result);
            }
        }
    }
    else if( geo == geometry::CAT )
    {
        for (int y = 0; y < res_y; y++)
        {
            for(int x = 0; x < res_x; x++)
            {
                double u   = static_cast<double>(x)/res_x;
                double v   = static_cast<double>(y)/res_y;
                double result = generate_cat_model( cv::Point2d(u, v), cv::Vec2i(res_x, res_y) );
                FRep_vec.push_back(result);
            }
        }
    }
    else
    {
        std::cerr << "ERROR: no input pre-defined geometry is specified!" << std::endl;
        FRep_vec.clear();
    }

    return FRep_vec;

#ifdef USE_DEBUG_INFO_FREP
    std::ofstream file;
    std::string file_n = "FREP_" + file_name + ".txt";
    file.open(file_n);
    file.precision(7);

    for (int i = 0; i < resolution_y*resolution_x; i++)
        file << FRep_vec[i] << std::endl;
#endif
}

cv::Mat function_rep::HybrydFunctionRep::get_FRep_im( const std::vector<double> *input, geometry geo, std::string file_name )
{
    cv::Mat result(resolution_x, resolution_y, cv_im_type);
    std::vector<uchar> frep_im;
    frep_im.resize(input->size());

    double val = 0;
    for( int i = 0; i < resolution_y*resolution_x; i++ )
    {
       if(geo == geometry::CIRCLE )
           val = input->at(i)*10000.0;
       else if(geo == geometry::BLOBBY )
           val = input->at(i)*100.0;
       else if(geo == geometry::BUTTERFLY )
           val = input->at(i)*100000.0;
       else if (geo == geometry::HEART )
           val = input->at(i)*10000000.0;
       else if (geo == geometry::CHAIR )
           val = input->at(i)*1000000.0;
       else if ( geo == geometry::BORG )
           val = input->at(i)*10000.0;
       else if ( geo == geometry::CAT )
           val = input->at(i)*10.0;

       if( val < 0.0 )
           frep_im[i] = 0;
       else if ( val > 255 )
           frep_im[i] = 255;
       else
           frep_im[i] = val;
    }

    memcpy(result.data, frep_im.data(), frep_im.size()*sizeof(uchar));

#ifdef USE_DEBUG_INFO_FREP
    std::string fin_file_name = "frep_im_" + file_name + ".png";
    cv::imwrite(fin_file_name, result);
#endif
    return result;
}

double function_rep::HybrydFunctionRep::ellipsoid_2d( cv::Point2d pos, cv::Point2d cent, double a, double b, int resx )
{
    return 1.0/resx - std::pow( ( pos.x - cent.x )/a, 2.0 ) - std::pow( ( pos.y - cent.y )/b, 2.0 );
}

double function_rep::HybrydFunctionRep::ellipticCylZ( cv::Point2d pos, cv::Point2d cent, double a, double b, int resx )
{
    return 1.0/resx - std::pow( pos.x - cent.x, 2.0 )/( a*a ) - std::pow( pos.y - cent.y, 2.0 )/( b*b );
}

double function_rep::HybrydFunctionRep::torusY_2d(cv::Point2d pos, cv::Point2d cent, double r, double R)
{
    return r*r - std::pow( pos.y - cent.y, 2.0 ) - std::pow( pos.y - cent.y, 2.0 )- R*R +
           2.0*R*std::sqrt( std::pow( pos.x - cent.x, 2.0 ) + std::pow( pos.y - cent.y, 2.0 ));
}

double function_rep::HybrydFunctionRep::torusZ_2d(cv::Point2d pos, cv::Point2d cent, double r, double R)
{
    return r*r - std::pow( pos.x - cent.x, 2.0 ) - std::pow( pos.y - cent.y, 2.0 ) -
           R*R + 2.0*R*std::sqrt( std::pow( pos.x - cent.x, 2.0 ) + std::pow( pos.y - cent.y, 2.0 ) );
}

double function_rep::HybrydFunctionRep::sphere_2d(cv::Point2d pos, cv::Point2d cent, double r)
{
    return r*r - std::pow( pos.x - cent.x, 2.0 ) - std::pow( pos.y - cent.y, 2.0 );
}

double function_rep::HybrydFunctionRep::generate_cat_model( cv::Point2d pos, cv::Vec2i res )
{
    double shX = res[0]/2.0, shY = res[1]/3.0;
    //head
    cv::Point2d headC = { (0.0+shX)/res[0], (4.5+shY)/res[1] };
    double head = ellipsoid_2d( pos, headC, 6.0, 5.5, res[0] );

    //nose
    cv::Point2d noseC = { (0.0+shX)/res[0], (25.5+shY)/res[1] };
    double nose = ellipsoid_2d( pos, noseC, 0.5, 0.5, res[0] );

    //face
    cv::Point2d faceC1 = { (0.0+shX)/res[0], (4.5+shY)/res[1] };
    double xFace1 = ellipsoid_2d( pos, faceC1, 6.0, 5.5, res[0] );
    cv::Point2d faceC2 = { (0.0+shX)/res[0], (3.7+shY)/res[1] };
    double xFace2 = ellipticCylZ( pos, faceC2, 4.7, 4.5, res[0] );
    double face   = intersect_function( xFace1, xFace2 );

    //mouth
    cv::Point2d mouthC = { (0.0+shX)/res[0], (10.5+shY)/res[1] };
    double mouth1 = ellipsoid_2d( pos, mouthC, 3.0, 2.3, res[0] );
    double mouth2 = -(60.5+shY)/res[1] + pos.y;
    double mouth  = intersect_function( mouth1, mouth2 );

    //eyes
    cv::Point2d eye1C = { (0.0+shX)/res[0], (2.5+shY)/res[1] };
    double eye1 = ellipsoid_2d( pos, eye1C, 6.0, 5.5, res[0] );

    cv::Point2d eye2C = { (36.5+shX)/res[0], (-10.5+shY)/res[1] };
    double eye2 = ellipticCylZ( pos, eye2C, 1.5, 1.4, res[0] );
    double eye3 = intersect_function( eye1, eye2 );

    cv::Point2d eye4C = { (-36.5+shX)/res[0], (-10.5+shY)/res[1] };
    double eye4 = ellipticCylZ( pos, eye4C, 1.5, 1.4, res[0] );
    double eye5 = intersect_function( eye1, eye4 );

    cv::Point2d eye6C = { (20.3+shX)/res[0], (-10.5+shY)/res[1]};
    double eye6 = ellipsoid_2d( pos, eye6C, 0.3, 0.3, res[0] );
    double eye7 = subtract_function( eye3, eye6 );

    cv::Point2d eye8C = { (-20.3+shX)/res[0], (-10.5+shY)/res[1] };
    double eye8 = ellipsoid_2d( pos, eye8C, 0.3, 0.3, res[0] );
    double eye9 = subtract_function( eye5, eye8 );

    //head final
    double fin_head0 = union_function( eye9, eye7 );
    double fin_head1 = union_function( face, head );
    double fin_head2 = subtract_function( fin_head1, mouth );
    double fin_head3 = subtract_function( fin_head2, nose );
    double fin_head  = subtract_function( fin_head3, fin_head0);

    //body
    cv::Point2d bodyC = { (0.0+shX)/res[0], (122.0+shY)/res[1] };
    double body1 = ellipsoid_2d( pos, bodyC, 4.5, 6.0, res[0] );
    double body2 = intersect_function( intersect_function( body1, (126.0+shY)/res[1]+pos.y ), -(121.0+shY)/res[1]+pos.y );

    //neck
    cv::Point2d neckC = { (0.0+shX)/res[0], (120.0+shY)/res[1] };
    double neck = ellipsoid_2d( pos, neckC, 1.8, 0.8, res[0] );

    //bell
    cv::Point2d bellC = { (0.0+shX)/res[0], (180.0+shY)/res[1] };
    double bell  = sphere_2d( pos, bellC, 10.8/res[0] );

    double body3 = union_function( body2, neck );

    //belly
    cv::Point2d bellyC1 = { (0.0+shX)/res[0], ( 32.0+shY)/res[1] };
    double belly1 = ellipsoid_2d( pos, bellyC1, 4.5, 6.0, res[0] );

    cv::Point2d bellyC2 = { (0.0+shX)/res[0], ( 200.0+shY)/res[1] };
    double belly2 = ellipticCylZ( pos, bellyC2, 0.5, 0.5, res[0] );
    double belly3 = intersect_function( belly1, belly2 );

    cv::Point2d bellyC4 = { (0.0+shX)/res[0], ( 138.0+shY)/res[1] };
    double belly4 = ellipticCylZ( pos, bellyC4, 2.0, 1.6, res[0] );

    cv::Point2d bellyC5 = { (0.0+shX)/res[0], ( 200.0+shY)/res[1] };
    double belly5 = ellipsoid_2d( pos, bellyC5, 1.1, 1.1, res[0] );
    double belly6 = -(103.0+shY)/res[1] + pos.y;

    double fin_body0 = intersect_function( belly4, belly6 );
    double fin_body1 = union_function( fin_body0, belly5 );
    double fin_body2 = subtract_function( body3, fin_body1 );
    double fin_body3 = union_function( fin_body2, belly3 );
    double fin_body  = union_function( fin_body3, bell );

    //legs and feet
    cv::Point2d l_legC = { (93.0+shX)/res[0], (247.5+shY)/res[1] };
    double l_leg = ellipsoid_2d( pos, l_legC, 3.0, 1.5, res[0] );
    cv::Point2d r_legC = { (-93.0+shX)/res[0], (247.5+shY)/res[1] };
    double r_leg = ellipsoid_2d( pos, r_legC, 3.0, 1.5, res[0] );

    cv::Point2d l_footC = { (92.0+shX)/res[0], (246.0+shY)/res[1] };
    double l_foot = ellipsoid_2d( pos, l_footC, 1.0, 1.0, res[0] );
    cv::Point2d r_footC = { (-92.0+shX)/res[0], (246.0+shY)/res[1] };
    double r_foot = ellipsoid_2d( pos, r_footC, 1.0, 1.0, res[0] );

    double legs0 = union_function( l_leg, l_foot );
    double legs1 = union_function( r_leg, r_foot );
    double legs  = union_function( legs0, legs1 );

    //right mustache
    double rh = 0.15;
    cv::Point2d r_mustC1 = { (112.0+shX)/res[0], ( 100.0+shY)/res[1] };
    double r_must1 = torusZ_2d( pos, r_mustC1, 7.0/res[0], rh );
    cv::Point2d r_mustC2 = { (122.0+shX)/res[0], ( 102.0+shY)/res[1] };
    double r_must2 = torusZ_2d( pos, r_mustC2, 7.0/res[0], rh );
    cv::Point2d r_mustC3 = { (112.0+shX)/res[0], ( 104.0+shY)/res[1] };
    double r_must3 = torusZ_2d( pos, r_mustC3, 7.0/res[0], rh );

    //left mustache
    cv::Point2d l_mustC1 = { (-112.0+shX)/res[0], (100.0+shY)/res[1] };
    double l_must1 = torusZ_2d( pos, l_mustC1, 7.0/res[0], rh );
    cv::Point2d l_mustC2 = { (-112.0+shX)/res[0], (102.0+shY)/res[1] };
    double l_must2 = torusZ_2d( pos, l_mustC2, 7.0/res[0], rh );
    cv::Point2d l_mustC3 = { (-112.0+shX)/res[0], (104.0+shY)/res[1] };
    double l_must3 = torusZ_2d( pos, l_mustC3, 7.0/res[0], rh );

    double r_must11 = intersect_function( pos.y, (1.4*shY)/res[1] - pos.y );
    double r_must22 = union_function( union_function( r_must1, r_must2 ), r_must3 );
    double r_mustF  = intersect_function( intersect_function( r_must22, r_must11 ), pos.y );

    double l_must11 = intersect_function( -pos.y + (1.4*shY)/res[1], pos.y );
    double l_must22 = union_function( union_function( l_must1, l_must2 ), l_must3 );
    double l_mustF  = intersect_function( intersect_function( l_must22, l_must11 ), pos.y );
    double mustache = union_function( r_mustF, l_mustF );

    //hands
    cv::Point2d r_handC = { (158.0+shX)/res[0], (152.5+shY)/res[1] };
    double r_hand = sphere_2d( pos, r_handC, 30.0/res[0] );
    cv::Point2d l_handC = { (-158.0+shX)/res[0], (152.5+shY)/res[1] };
    double l_hand = sphere_2d( pos, l_handC, 30.0/res[0] );

    double ude2 = pos.y + (150.0+shY)/res[1];
    double ude3 = (150.0+shY)/res[1] - pos.y;
    cv::Point2d udeC = { (0.0+shX)/res[0], (251.0+shY)/res[1] };
    double ude1 = torusZ_2d( pos, udeC, 30.0/res[0], 0.3 );
    double te1  = intersect_function( intersect_function( ude1, ude2 ), ude3 );
    double te2  = union_function( union_function( te1, r_hand ), l_hand );

    //final model
    double cat0 = union_function( fin_head, fin_body );
    double cat1 = union_function( cat0, legs );
    double cat2 = union_function( cat1, te2 );
    double cat  = union_function( cat2, mustache );

    return cat;
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
                                                     const std::vector<double> *DistTr, cv::Vec2i res, std::string file_name )
{
    for(int i = 0; i < res[0]*res[1]; i++)
    {
        double hfrep_val = get_step_function_val( frep[i], 2 ) * DistTr->at(i);
        hfrep->push_back(hfrep_val);
    }

#ifdef USE_DEBUG_INFO_FREP
    std::ofstream file;
    std::string file_n = "HFREP_" + file_name + ".txt";
    file.open(file_n);
    file.precision(7);

    for (int i = 0; i < res[0]*res[1]; i++)
        file << hfrep->at(i) << std::endl;
#endif
}

void function_rep::HybrydFunctionRep::check_HFrep( std::vector<double> hfrep, std::string hfrep_check_name )
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

    std::cout << "This is < " << hfrep_check_name << "> checking." << std::endl;
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
