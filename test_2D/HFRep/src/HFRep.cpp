#include "include/HFRep.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <fstream>

#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>

function_rep::HybrydFunctionRep::HybrydFunctionRep( int res_x, int res_y, int ddt_sh)
{
    resolution_x = res_x;
    resolution_y = res_y;
    dist_sh      = ddt_sh;

    DT        = std::make_shared<distance_transform::DiscreteDistanceTransform>( resolution_x, resolution_y );
    DT_custom = std::make_shared<distance_transform::DiscreteDistanceTransform>( resolution_x, resolution_y );
    modF      = std::make_shared<modified_field::ModifyField>();
    drawF     = std::make_shared<draw::DrawField>();
}

std::vector<double> function_rep::HybrydFunctionRep::precalcDDT(function_rep::geometry geo, function_rep::geometryParams p, bool sign)
{
    std::vector<double> frep  = generateFRep( geo, p );
    DT_custom.get()->caclulateDiscreteDistanceTransformVec( &frep );
    if(!sign)
        return DT_custom.get()->get_DDT();
    else
        return DT_custom.get()->get_signed_DDT();
}

void function_rep::HybrydFunctionRep::calculateHFRep(function_rep::geometry geo, function_rep::geometryParams p,
                                                      int stepfunc, function_rep::stepFuncParams stFunP,
                                                      std::vector<double> *frep, std::vector<double> *ddt,
                                                      std::string file_name )
{
    tangent_slope    = stFunP.tangent_slope;
    sigmoid_slope    = stFunP.sigmoid_slope;
    algebraic_slope  = stFunP.algebraic_slope;
    guardanian_slope = stFunP.guderanian_slope;

    setNewRes( resolution_x+dist_sh, resolution_y+dist_sh );
    dist_tr.clear();
    signDist_tr.clear();
    sm_dist_tr.clear();

    if( frep == nullptr )
        FRep_vec  = generateFRep( geo, p, file_name );
    else
        FRep_vec = *frep;

    if( ddt == nullptr )
    {
        DT.get()->caclulateDiscreteDistanceTransformVec( &FRep_vec );
        signDist_tr = DT.get()->get_signed_DDT();
        dist_tr     = DT.get()->get_DDT();
    }
    else {
        dist_tr = *ddt;
    }

    sm_dist_tr  = modF.get()->smooth_field( &dist_tr, resolution_x, resolution_y );
    generateHFRep( stepfunc );
}

double function_rep::HybrydFunctionRep::get_step_function_val( double frep_val, int function )
{
    if( function == 1 )
    {
        double result = std::tanh( tangent_slope * frep_val ); //100000.0
        return result;
    }
    else if ( function == 2 )
    {
        double result = 2.0/(1.0+std::exp(-2.0*frep_val/sigmoid_slope)) - 1.0;
        return result;
    }
    else if ( function == 3 )
    {
        double result = frep_val / std::sqrt( algebraic_slope + frep_val * frep_val );
        return result;
    }
    else if ( function == 4 )
    {
        double result = 0.642129*std::atan( std::sinh( guardanian_slope * frep_val ) ); //1000
        return result;
    }
    else
    {
        std::cerr << "ERROR: unknown option! [ get_step_function_val(.., <?> ) ]" << std::endl;
        exit(-1);
    }
}

void function_rep::HybrydFunctionRep::generateHFRep( int step_function , std::string file_name )
{
    HFRep_vec.clear();
    for(size_t i = 0; i < static_cast<size_t>( resolution_x*resolution_y ); i++)
    {
        double hfrep_val = get_step_function_val( FRep_vec[i], step_function ) * sm_dist_tr[i];
        HFRep_vec.push_back(hfrep_val);
    }

#ifdef USE_DEBUG_INFO_FREP
    std::ofstream file;
    std::string file_n = "HFREP_" + file_name + ".txt";
    file.open(file_n);
    file.precision(7);

    for (size_t i = 0; i < static_cast<size_t>( resolution_x*resolution_y ); i++)
        file << HFRep_vec[i] << std::endl;
#endif
}

void function_rep::HybrydFunctionRep::checkHFrep( std::vector<double> *hfrep, std::string hfrep_check_name )
{
    int frep_zeros    = 0;
    int hfrep_zeros   = 0;
    int dist_tr_zeros = 0;

    for(int i = 0; i < resolution_x*resolution_y; i++)
    {
        if( hfrep->at(i) == 0.0 )
            frep_zeros++;
        if( hfrep->at(i) == 0.0 )
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

void function_rep::HybrydFunctionRep::getFieldImages( double thres_vis_frep, double thres_vis_ddt,
                                                      double thres_vis_hfrep, std::string file_name )
{
    std::vector<uchar> field;

    std::vector<double> SDDT = modF.get()->smooth_field( &signDist_tr, resolution_x, resolution_y );
    drawF.get()->draw_rgb_isolines( &field, &SDDT, resolution_x, resolution_y, thres_vis_ddt, "ddt", true );

    field.clear();
    drawF.get()->draw_rgb_isolines( &field, &HFRep_vec, resolution_x, resolution_y, thres_vis_hfrep, "hfrep", true);

    field.clear();
    drawF.get()->draw_rgb_isolines( &field, &FRep_vec, resolution_x, resolution_y, thres_vis_frep, "frep", true);

    std::vector<double> diff_field; diff_field.resize( HFRep_vec.size() );
    diff_field = modF.get()->diff_fields( &HFRep_vec, &sm_dist_tr, 100000.0 );

    field.clear();
    drawF.get()->draw_rgb_isolines( &field, &diff_field, 512, 512, 0.0,"diff_field" );
}

std::vector<double> function_rep::HybrydFunctionRep::generateFRep( geometry geo, geometryParams p, std::string file_name )
{
    FRep_vec.clear();

    p.start_P.x = p.start_P.x/resolution_x;
    p.start_P.y = p.start_P.y/resolution_y;
    p.rad       = p.rad / resolution_x;

    double zoom;

    if( geo == geometry::CIRCLE )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;
                double result = p.rad*p.rad - shX*shX - shY*shY;
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
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;
                double blob1 = p.rad*p.rad/( shX*shX + shY*shY);
                double blob2 = ( p.rad*p.rad ) * 0.25 /
                               ( (shX - 0.3)*(shX - 0.3) + (shY - 0.2)*(shY - 0.2) );
                double result = union_function(blob1, blob2);
                FRep_vec.push_back(result);            }
        }
    }
    else if ( geo == geometry::BUTTERFLY )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;

                if( p.zoom == 0.0 )
                    zoom = 4.0;
                else
                    zoom = p.zoom;

                double result = std::pow(shX*zoom, 6.0) + std::pow(shY*zoom, 6.0) - shX*shX*zoom*zoom;
                FRep_vec.push_back(result);            }
        }
    }
    else if ( geo == geometry::HEART )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;

                if( p.zoom == 0.0 )
                    zoom = 3.0;
                else
                    zoom = p.zoom;

                double result = -std::pow( zoom*zoom*shX*shX + zoom*zoom*shY*shY - 1.0, 3.0 ) +
                                           zoom*zoom*shX*shX*std::pow( zoom*shY, 3.0 );
                FRep_vec.push_back(result);
            }
        }
    }
    else if( geo == geometry::CHAIR)
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;
                double a = 0.8;
                double b = 0.6;
                double k = 2.0;

                if( p.zoom == 0.0 )
                    zoom = 6.0;
                else
                    zoom = p.zoom;

                double result = std::pow( zoom*zoom*shX*shX + zoom*shY*shY - a*k*k, 2.0) -
                                          b*(k*k - zoom*zoom*2.0*shX*shX)*(k*k - zoom*zoom*2.0*shY*shY);
                FRep_vec.push_back(result);            }
        }
    }
    else if( geo == geometry::BORG)
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;

                if( p.zoom == 0.0 )
                    zoom = 9.0;
                else
                    zoom = p.zoom;
                double result = std::sin(zoom*zoom*shX*shY);
                FRep_vec.push_back(result);
            }
        }
    }
    else if( geo == geometry::ELF )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double result = generate_elf_model( cv::Point2d(u, v), cv::Vec2i(resolution_x, resolution_y) );
                FRep_vec.push_back(result);
            }
        }
    }
    else if( geo == geometry::QUAD )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;
                double result = intersect_function( 170.0/resolution_x - std::abs(shX) , 170.0/resolution_y - std::abs(shY) );
                FRep_vec.push_back(result);
            }
        }
    }
    else if ( geo == geometry::SURIKEN )
    {
        for (int y = 0; y < resolution_y; y++)
        {
            for(int x = 0; x < resolution_x; x++)
            {
                double u   = static_cast<double>(x)/resolution_x;
                double v   = static_cast<double>(y)/resolution_y;
                double shX = u - p.start_P.x;
                double shY = v - p.start_P.y;
                double lb1 = shY+0.7 - (shX*2.0 + 0.3);
                double rb1 = shY+0.7 - (-shX*2.0 + 0.3);
                double lb2 = shY-0.1 - (shX*2.0 + 0.3);
                double rb2 = shY-0.1 - (-shX*2.0 + 0.3);
                double lb3 = shX+0.7 - (shY*2.0 + 0.3);
                double rb3 = shX+0.7 - (-shY*2.0 + 0.3);
                double lb4 = shX-0.1 - (shY*2.0 + 0.3);
                double rb4 = shX-0.1 - (-shY*2.0 + 0.3);

                double trian1 = intersect_function( intersect_function(lb1, rb1), -shY+0.2 );
                double trian2 = intersect_function( intersect_function(-lb2, -rb2), shY+0.2 );
                double trian3 = intersect_function( intersect_function(lb3, rb3), -shX+0.2 );
                double trian4 = intersect_function( intersect_function(-lb4, -rb4), shX+0.2 );

                //double result = trian2;
                double result = union_function( union_function( union_function( trian1, trian2 ), trian3 ), trian4 );
                FRep_vec.push_back(result);
            }
        }
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

    for (size_t i = 0; i < static_cast<size_t>(resolution_y*resolution_x); i++)
        file << FRep_vec[i] << std::endl;

    file.close();
#endif

    return FRep_vec;
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

double function_rep::HybrydFunctionRep::generate_elf_model( cv::Point2d pos, cv::Vec2i res )
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
