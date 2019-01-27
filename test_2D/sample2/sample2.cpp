#include "include/HFRep.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <iterator>
#include <fstream>

#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>

namespace  args
{
  static std::string input_pic;
  static std::string output_pic;
  static std::string geometry;
}

function_rep::geometry set_geometry(std::string input)
{
    if( input == "circle" )
        return function_rep::geometry::CIRCLE;
    else if( input == "blobby" )
        return function_rep::geometry::BLOBBY;
    else if( input == "butterfly" )
        return function_rep::geometry::BUTTERFLY;
    else if( input == "heart" )
        return  function_rep::geometry::HEART;
    else if( input == "chair")
        return function_rep::geometry::CHAIR;
    else if( input == "borg")
        return function_rep::geometry::BORG;
    else if( input == "elf" )
        return function_rep::geometry::ELF;
    else if( input == "quad" )
        return function_rep::geometry::QUAD;
    else if( input == "suriken" )
        return  function_rep::geometry::SURIKEN;
    else
    {
        std::cerr << "ERROR: unknown option! please specify with '-geo' [circle, quad, rectangle, triangle] \n " << std::endl;
        return function_rep::geometry::NONE;
    }
}

void print_help()
{
  std::cout << "\nPossible customly specified options: \n";
  std::cout << "-geo   - specify simple geometry [blobby, butterfly, chair, heart, borg] \n";
  std::cout << "-im    - specify first image [optional] \n";
  std::cout << "--save - save the output     [optional] \n";
  std::cout << "--help - output help\n" << std::endl;
}

bool parse_args(int argc, char** argv)
{
  for (int i = 1; i < argc; i += 2)
  {
    const char *cur = argv[i];
    if(!strcmp(cur, "-geo"))
      args::geometry = argv[i + 1];
    else if(!strcmp(cur, "-im"))
      args::input_pic = argv[i + 1];

    else if (!strcmp(cur, "--help"))
    {
      print_help();
      return false;
    }
    else
    {
      std::cerr << "ERROR: Unknown Option! exiting..." << std::endl;
      return false;
    }
  }

  if(args::geometry.empty() && args::input_pic.empty())
  {
      std::cerr << "ERROR: Please specify either input image [ -im ] or shape [ -geo ]\n" << std::endl;
      print_help();
      return false;
  }

  if(args::geometry.empty())
  {
      std::cerr << "ERROR: Please specify shape [ -geo ]\n" << std::endl;
      print_help();
      return false;
  }

  return true;
}


int main(int argc, char** argv)
{
    if (!parse_args(argc, argv))
         exit(EXIT_SUCCESS);

    cv::Mat input, output;

    if(!args::input_pic.empty())
    {
        input = cv::imread(args::input_pic, cv::IMREAD_UNCHANGED);
        if( input.empty())
        {
            std::cerr << "ERROR: can not load the input image [-im]! Abort!" << std::endl;
            return -1;
        }
    }

    function_rep::geometryParams geo;
    geo.start_P.x      = 250.0;
    geo.start_P.y      = 250.0;
    geo.rad            = 60.0;
    geo.zoom           = 0.0;

    function_rep::stepFuncParams st_fun;
    st_fun.sigmoid_slope    = 0.0001;
    st_fun.tangent_slope    = 100000.0;
    st_fun.algebraic_slope  = 0.0001;
    st_fun.guderanian_slope = 1000.0;
    int step_function = HYPERBOLIC_SIGMOID;

    double thres_vis_ddt   = 0.04;
    double thres_vis_hfrep = 0.05;

    function_rep::HybrydFunctionRep hfrep( 512, 512, 0 );
    modified_field::ModifyField modField;
    draw::DrawField drawField;

    // cv::Point2i(170, 170) is good for all shapes except heart; heart is cv::Point2i(150, 350)
    cv::Point2i start_regP;
    if(args::geometry == "heart" && !args::geometry.empty())
        start_regP = cv::Point2i(185, 350);
    else
        start_regP = cv::Point2i(170, 170);

    double thres_viz_frep;
    if(args::geometry == "elf" && !args::geometry.empty())
        thres_viz_frep = 0.0001;
    else
        thres_viz_frep = 0.009;

    //******************************************************************************************************
    //Zooming operation
    //******************************************************************************************************

    //getting distance transform, zoom in, smooth, draw it
    std::vector<double> dist_tr    = hfrep.precalcDDT( set_geometry(args::geometry), geo );
    std::vector<double> zoomed_ddt = modField.zoom_field( &dist_tr, start_regP, cv::Vec2i(128, 128), cv::Vec2i(512, 512));

    std::vector<uchar> img_field;

    // get frep field and draw it
    std::vector<double> frep_vec = hfrep.precalcFRep( set_geometry(args::geometry), geo );

    // zoom in frep field and draw it
    std::vector<double> zoomed_frep = modField.zoom_field( &frep_vec, start_regP, cv::Vec2i(128, 128), cv::Vec2i(512, 512));

    // generate zoomed in HFrep and draw it
    std::vector<double> ZM_hfrep_vec;
    hfrep.calculateHFRep( set_geometry(args::geometry), geo, step_function, st_fun, &zoomed_frep, &zoomed_ddt );
    ZM_hfrep_vec = hfrep.getHFRepVec();

    hfrep.checkHFrep( &ZM_hfrep_vec, "zoomed" );

    img_field.clear();
    drawField.draw_rgb_isolines( &img_field, &ZM_hfrep_vec, 512, 512, 0.09, "zoomed_hfrep", true );

    //draw the difference between hfrep and ddt
    img_field.clear();
    std::vector<double> sm_zoomed_ddt = modField.smooth_field(&zoomed_ddt, 512, 512 );
    std::vector<double> diff_field = modField.diff_fields( &ZM_hfrep_vec, &sm_zoomed_ddt, 100000.0 );
    drawField.draw_rgb_isolines(&img_field, &diff_field, 512, 512, 0.0, "zoomed_diff_field");

    //******************************************************************************************************
    //interpolation if the field is sparse
    //******************************************************************************************************

    //generating frep in full resolution
    //obtain sparse values for DDT
    hfrep.calculateHFRep(set_geometry(args::geometry), geo, step_function, st_fun, nullptr, nullptr );
    hfrep.getFieldImages( thres_viz_frep, thres_vis_ddt, thres_vis_hfrep);

    std::vector<double> ddt  = hfrep.precalcDDT( set_geometry(args::geometry), geo);
    std::vector<double> sddt = hfrep.precalcDDT( set_geometry(args::geometry), geo, true );

    auto start_sp_ddt = std::chrono::system_clock::now();
    std::vector<double> sp_ddt, sp_sddt;
    int stX = 512/128, stY= 512/128;
    for(int y = 0; y < 512; y+=stY )
    {
        for(int x = 0; x < 512; x+=stX)
        {
            sp_ddt.push_back( ddt[x+y*512] );
            sp_sddt.push_back( sddt[x+y*512] );
        }
    }
    auto end_sp_ddt = std::chrono::system_clock::now();
    std::chrono::duration<double> t_sp_ddt = end_sp_ddt - start_sp_ddt;

    //interpolate between sparse values for DDT
    auto start_int_ddt = std::chrono::system_clock::now();
    std::vector<double> inter_ddt = modField.interpolate_field( &sp_ddt, cv::Vec2i(128,128),
                                                                         cv::Vec2i(512,512), modified_field::BICUBIC );
    std::vector<double> inter_sddt = modField.interpolate_field( &sp_sddt, cv::Vec2i(128,128),
                                                                           cv::Vec2i(512,512), modified_field::BICUBIC );
    auto end_int_ddt = std::chrono::system_clock::now();
    std::chrono::duration<double> t_int_ddt = end_int_ddt - start_int_ddt;

    std::vector<uchar> img_field0;

    drawField.draw_rgb_isolines( &img_field0, &inter_sddt, 512, 512, thres_vis_ddt , "sddt_512x512", true );
    img_field0.clear();
    drawField.draw_rgb_isolines( &img_field0, &sp_sddt, 128, 128, thres_vis_ddt , "sddt_128x128", true );

    auto start_hfrep = std::chrono::system_clock::now();
    hfrep.calculateHFRep( set_geometry(args::geometry), geo, step_function, st_fun, nullptr, &inter_ddt );
    std::vector<double> hfrep0 = hfrep.getHFRepVec();
    auto end_hfrep   = std::chrono::system_clock::now();
    std::chrono::duration<double> t_hfrep = end_hfrep - start_hfrep;

    hfrep.checkHFrep(&hfrep0, "hfrep0_checking");

    drawField.draw_rgb_isolines( &img_field0, &hfrep0, 512, 512, thres_vis_hfrep , "hfrep_512x512", true);

    //calculate the difference between fields
    std::vector<double> sm_inter_ddt = modField.smooth_field(&inter_ddt, 512, 512);
    std::vector<double> diff_field_inter = modField.diff_fields( &hfrep0, &sm_inter_ddt, 100000.0 );
    img_field0.clear();
    drawField.draw_rgb_isolines( &img_field0, &diff_field_inter, 512, 512, 0.0, "diff_hfrep_512x512" );

    //******************************************************************************************************
    //estimating error for interpolated case
    //******************************************************************************************************
    std::vector<double> dField_inter = modField.diff_fields( &hfrep0, &inter_ddt, 1.0 );

    std::vector<double> pos_val;
    std::copy_if( dField_inter.begin(), dField_inter.end(), std::back_inserter(pos_val),
                  std::bind(std::less<double>(), 0.0, std::placeholders::_1) );

    auto val = std::minmax_element( pos_val.begin(), pos_val.end() );
    auto min_inter = pos_val[val.first  - pos_val.begin()];
    auto max_inter = pos_val[val.second - pos_val.begin()];
    double aver_inter = std::accumulate(pos_val.begin(), pos_val.end(), 0.0) / double(pos_val.size());

    std::cout << "\nError after using bicubic interpolation: " << std::endl;
    std::cout << " min_inter = " << min_inter <<" ; max_inter = " << max_inter <<
                 " ; aver_inter = " << aver_inter << std::endl;

    std::cout << "Timings: " << std::endl;
    std::cout << "DDT_sparse ex. [sec.]  : " << t_sp_ddt.count()  << std::endl;
    std::cout << "DDT_interpol.ex.[sec.] : " << t_int_ddt.count() << std::endl;
    std::cout << "HFRep ex. [sec.]       : " << t_hfrep.count()   << std::endl;

    return 0;
}
