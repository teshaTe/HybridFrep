#include "include/FRep.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

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
  std::cout << "-geo   - specify simple geometry [circle, quad, rectangle, triangle] \n";
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
    //function_rep::HybrydFunctionRep hfrep(set_geometry(args::geometry), geo, 512, 512, 1, CV_8UC1);
    function_rep::HybrydFunctionRep hfrep(set_geometry(args::geometry), geo, 512, 512, 1);

    // cv::Point2i(170, 170) is good for all shapes except heart; heart is cv::Point2i(150, 350)
    cv::Point2i start_regP;
    if(args::geometry == "heart" && !args::geometry.empty())
        start_regP = cv::Point2i(150, 350);
    else
        start_regP = cv::Point2i(170, 170);

    std::vector<double> dist_tr    = hfrep.get_DDT();
    std::vector<double> zoomed_ddt = hfrep.modF.get()->zoom_in_field( &dist_tr, start_regP, cv::Vec2i(128, 128),
                                                                      cv::Vec2i(512, 512), 1 );
    std::vector<double> smoothed_ddt = hfrep.modF.get()->smooth_field( &zoomed_ddt, 513, 513 );

    std::vector<uchar> img_field;
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &smoothed_ddt, 512, 512, "zoomed_ddt" );

    std::vector<double> frep_vec = hfrep.get_frep_vec();
    std::vector<double> zoomed_frep = hfrep.modF.get()->zoom_in_field( &frep_vec, start_regP, cv::Vec2i(128, 128),
                                                                      cv::Vec2i(512, 512), 1 );
    std::vector<double> fin_frep = hfrep.modF.get()->finalize_field( &zoomed_frep, 512, 512, 1 );

    img_field.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &fin_frep, 512, 512, "zoomed_frep" );

    std::vector<double> hfrep_vec;
    hfrep.generate_hfrep( &hfrep_vec, fin_frep, &smoothed_ddt, "zoomed_hfrep" );
    hfrep.check_HFrep( hfrep_vec, "zoomed" );

    img_field.clear();
    hfrep.drawF.get()->draw_grey_isolines( &img_field, &hfrep_vec, 512, 512, "zoomed_hfrep" );


    //TODO: zoomed_in_field check all the functions and how they work -> size of the final vector is smaller than it should be!!!!

    return 0;
}
