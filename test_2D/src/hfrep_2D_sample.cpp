#include "include/FRep.h"

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

    function_rep::geometry_params geo;
    geo.start_P.x = 250.0;
    geo.start_P.y = 250.0;
    geo.rad       = 60.0;
    geo.zoom      = 0.0;

    function_rep::HybrydFunctionRep hfrep(set_geometry(args::geometry), geo, 512, 512, 0);

    // cv::Point2i(170, 170) is good for all shapes except heart; heart is cv::Point2i(150, 350)
    cv::Point2i start_regP;
    if(args::geometry == "heart" && !args::geometry.empty())
        start_regP = cv::Point2i(185, 350);
    else
        start_regP = cv::Point2i(170, 170);

    double frep_thres;
    if(args::geometry == "elf" && !args::geometry.empty())
        frep_thres = 0.00009;
    else
        frep_thres = 0.009;

    //******************************************************************************************************
    //Zooming operation
    //******************************************************************************************************

    //getting distance transform, zoom in, smooth, draw it
    std::vector<double> dist_tr    = hfrep.get_DDT();
    std::vector<double> zoomed_ddt = hfrep.modF.get()->zoom_field( &dist_tr, start_regP, cv::Vec2i(128, 128),
                                                                    cv::Vec2i(512, 512));
    std::vector<double> smZoom_ddt = hfrep.modF.get()->smooth_field( &zoomed_ddt, 512, 512 );

    std::vector<uchar> img_field;
    hfrep.drawF.get()->draw_rgb_isolines( &img_field, &smZoom_ddt, 512, 512, 0.09,"zoomed_ddt" );

    // get frep field and draw it
    img_field.clear();
    std::vector<double> frep_vec = hfrep.get_frep_vec();
    hfrep.drawF.get()->draw_rgb_isolines( &img_field, &frep_vec, 512, 512, frep_thres, "frep_field", true );

    // zoom in frep field and draw it
    std::vector<double> zoomed_frep = hfrep.modF.get()->zoom_field( &frep_vec, start_regP, cv::Vec2i(128, 128),
                                                                    cv::Vec2i(512, 512));
    img_field.clear();
    hfrep.drawF.get()->draw_rgb_isolines( &img_field, &zoomed_frep, 512, 512, 0.009,"zoomed_frep" );

    // generate zoomed in HFrep and draw it
    std::vector<double> ZM_hfrep_vec;
    hfrep.generate_hfrep( &ZM_hfrep_vec, zoomed_frep, &smZoom_ddt, cv::Vec2i(512, 512), "zoomed_hfrep" );
    hfrep.check_HFrep( ZM_hfrep_vec, "zoomed" );

    img_field.clear();
    hfrep.drawF.get()->draw_rgb_isolines( &img_field, &ZM_hfrep_vec, 512, 512, 0.09,"zoomed_hfrep" );

    //draw the difference between hfrep and ddt
    img_field.clear();
    std::vector<double> diff_field = hfrep.modF.get()->diff_fields( &ZM_hfrep_vec, &smZoom_ddt, 1000000.0 );
    hfrep.drawF.get()->draw_rgb_isolines( &img_field, &diff_field, 512, 512, 0.0, "zoomed_diff_field" );

    //******************************************************************************************************
    //interpolation if the field is sparse
    //******************************************************************************************************

    //generating frep in full resolution
    std::vector<double> frep0;
    std::vector<uchar> img_field0;

    auto start_frep = std::chrono::system_clock::now();
    frep0 = hfrep.generate_frep( set_geometry(args::geometry), 512, 512, geo );
    auto end_frep   = std::chrono::system_clock::now();
    std::chrono::duration<double> t_frep = end_frep - start_frep;

    //generating ddt in full resolution
    auto start_ddt = std::chrono::system_clock::now();
    distance_transform::DistanceField dt( frep0, 512, 512 );
    std::vector<double> ddt0   = dt.get_DDT();
    std::vector<double> sm_ddt = hfrep.modF->smooth_field( &ddt0, 512, 512 );
    auto end_ddt = std::chrono::system_clock::now();
    std::chrono::duration<double> t_ddt = end_ddt - start_ddt;

    //obtain sparse values for DDT
    auto start_sp_ddt = std::chrono::system_clock::now();
    std::vector<double> sp_ddt;
    int stX = 512/128, stY= 512/128;
    for(int y = 0; y < 512; y+=stY )
    {
        for(int x = 0; x < 512; x+=stX)
        {
            sp_ddt.push_back( ddt0[x+y*512] );
        }
    }
    auto end_sp_ddt = std::chrono::system_clock::now();
    std::chrono::duration<double> t_sp_ddt = end_sp_ddt - start_sp_ddt;

    //interpolate between sparse values for DDT
    auto start_int_ddt = std::chrono::system_clock::now();
    std::vector<double> inter_ddt = hfrep.modF.get()->interpolate_field( &sp_ddt, cv::Vec2i(128,128),
                                                                         cv::Vec2i(512,512), modified_field::BICUBIC );
    auto end_int_ddt = std::chrono::system_clock::now();
    std::chrono::duration<double> t_int_ddt = end_int_ddt - start_int_ddt;

    img_field0.clear();
    hfrep.drawF.get()->draw_rgb_isolines( &img_field0, &inter_ddt, 512, 512, 0.04, "ddt_512x512" );

    //finally generate hfrep in full resolution
    auto start_hfrep =  std::chrono::system_clock::now();
    std::vector<double> hfrep0;
    hfrep.generate_hfrep(&hfrep0, frep0, &inter_ddt, cv::Vec2i(512, 512) );
    auto end_hfrep =  std::chrono::system_clock::now();
    std::chrono::duration<double> t_hfrep = end_hfrep - start_hfrep;

    img_field0.clear();
    hfrep.drawF.get()->draw_rgb_isolines( &img_field0, &hfrep0, 512, 512, 0.04, "hfrep_512x512", true);

    //calculate the difference between fields
    std::vector<double> diff_field_inter = hfrep.modF.get()->diff_fields( &hfrep0, &inter_ddt, 1000000.0 );
    img_field0.clear();
    hfrep.drawF.get()->draw_rgb_isolines( &img_field0, &diff_field_inter, 512, 512, 0.0,"diff_hfrep_512x512" );

    //******************************************************************************************************
    //estimating error for interpolated case
    //******************************************************************************************************
    std::vector<double> dField_inter = hfrep.modF.get()->diff_fields( &hfrep0, &inter_ddt, 1.0 );

    std::vector<double> pos_val;
    std::copy_if( dField_inter.begin(), dField_inter.end(), std::back_inserter(pos_val),
                  std::bind(std::less<double>(), 0.0, std::placeholders::_1) );

    auto val = std::minmax_element( pos_val.begin(), pos_val.end() );
    auto min_inter = pos_val[val.first  - pos_val.begin()];
    auto max_inter = pos_val[val.second - pos_val.begin()];
    double aver_inter = std::accumulate(pos_val.begin(), pos_val.end(), 0.0) / double(pos_val.size());

    std::cout << "\nError after using bicubic interpolation: " << std::endl;
    std::cout << " min_inter = " << min_inter <<" ; max_inter = " << max_inter << " ; aver_inter = " << aver_inter << std::endl;

    std::cout << "Timings: " << std::endl;
    std::cout << "FRep ex. [sec.]        : " << t_frep.count()    << std::endl;
    std::cout << "DDT  ex. [sec.]        : " << t_ddt.count()     << std::endl;
    std::cout << "DDT_sparse ex. [sec.]  : " << t_sp_ddt.count()  << std::endl;
    std::cout << "DDT_interpol.ex.[sec.] : " << t_int_ddt.count() << std::endl;
    std::cout << "HFRep ex. [sec.]       : " << t_hfrep.count()   << std::endl;

    return 0;
}
