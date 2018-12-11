#include "include/FRep.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>

#include<fstream>

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
    else if (input == "borg")
        return function_rep::geometry::BORG;
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

  if(args::output_pic.empty())
    std::cout << "\n WARNING: Output file is not specified, no data will be saved! \n" << std::endl;

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
    geo.start_P.x = 216.0;
    geo.start_P.y = 216.0;
    geo.rad       = 60.0;
    function_rep::HybrydFunctionRep hfrep(set_geometry(args::geometry), geo, 512, 512, 0);

    // cv::Point2i(170, 170) is good for all shapes except heart; heart is cv::Point2i(150, 350)
    cv::Point2i start_regP;
    if(args::geometry == "heart" && !args::geometry.empty())
        start_regP = cv::Point2i(150, 350);
    else
        start_regP = cv::Point2i(170, 170);

    //getting distance transform, zoom in, smooth, draw it
    std::vector<double> dist_tr    = hfrep.get_DDT();
    std::vector<double> zoomed_ddt = hfrep.modF.get()->zoom_field( &dist_tr, start_regP, cv::Vec2i(128, 128),
                                                                    cv::Vec2i(512, 512));
    std::vector<double> smZoom_ddt = hfrep.modF.get()->smooth_field( &zoomed_ddt, 512, 512 );

    std::vector<uchar> img_field;
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &smZoom_ddt, 512, 512, "zoomed_ddt" );

    // get frep field and draw it
    img_field.clear();
    std::vector<double> frep_vec = hfrep.get_frep_vec();
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &frep_vec, 512, 512, "frep_field" );

    // zoom in frep field and draw it
    std::vector<double> zoomed_frep = hfrep.modF.get()->zoom_field( &frep_vec, start_regP, cv::Vec2i(128, 128),
                                                                    cv::Vec2i(512, 512));

    //std::vector<double> fin_frep = hfrep.modF.get()->finalize_field( &zoomed_frep, 512, 512, 1 );

    img_field.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &zoomed_frep, 512, 512, "zoomed_frep" );

    // generate zoomed in HFrep and draw it
    std::vector<double> ZM_hfrep_vec;
    hfrep.generate_hfrep( &ZM_hfrep_vec, zoomed_frep, &smZoom_ddt, cv::Vec2i(512, 512), "zoomed_hfrep" );
    hfrep.check_HFrep( ZM_hfrep_vec, "zoomed" );

    img_field.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &ZM_hfrep_vec, 512, 512, "zoomed_hfrep" );

    //draw the difference between hfrep and ddt
    img_field.clear();
    std::vector<double> diff_field = hfrep.modF.get()->diff_fields( &ZM_hfrep_vec, &smZoom_ddt, 10000.0 );
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &diff_field, 512, 512, "zoomed_diff_field" );

    //interpolation if the field is sparse
    std::vector<double> inter_frep, frep0;
    std::vector<uchar> img_field0;
    cv::Point2d sh_center = {15.0/128.0, 15.0/128.0};
    frep0 = hfrep.generate_frep( function_rep::geometry::HEART, 128, 128, 15.0, sh_center);
    hfrep.drawF.get()->draw_grey_isolines( &img_field0, &frep0, 128, 128, "frep_128x128" );

    img_field0.clear();
    inter_frep = hfrep.modF.get()->interpolate_field( &frep0, cv::Vec2i(128, 128), cv::Vec2i(512, 512) );
    hfrep.drawF.get()->draw_grey_isolines( &img_field0, &inter_frep, 512, 512, "inter_frep_512x512" );

    distance_transform::DistanceField dt( frep0, 128, 128 );
    std::vector<double> ddt0 = dt.get_DDT();
    hfrep.modF->smooth_field(&ddt0, 128, 128);
    img_field0.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field0, &ddt0, 128, 128, "ddt_128x128" );

    std::vector<double> inter_ddt = hfrep.modF.get()->interpolate_field( &ddt0, cv::Vec2i(128,128), cv::Vec2i(512,512) );
    img_field0.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field0, &inter_ddt, 512, 512, "ddt_512x512" );

    std::vector<double> hfrep0;
    hfrep.generate_hfrep(&hfrep0, frep0, &ddt0, cv::Vec2i(128, 128) );
    img_field0.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field0, &hfrep0, 128, 128, "hfrep_128x128" );

    std::vector<double> inter_hfrep = hfrep.modF.get()->interpolate_field( &hfrep0, cv::Vec2i(128,128), cv::Vec2i(512,512) );
    img_field0.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field0, &inter_hfrep, 512, 512, "hfrep_512x512" );

    std::vector<double> diff_field_inter = hfrep.modF.get()->diff_fields( &inter_hfrep, &inter_ddt, 1000.0 );
    img_field0.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field0, &diff_field_inter, 512, 512, "diff_hfrep_512x512" );

    return 0;
}
