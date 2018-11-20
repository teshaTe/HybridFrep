#include "include/DDT.h"
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

   /* distance_transform::DistanceTransform dist(input, 0);
    std::vector<double> dist_f = dist.get_DDT();
    std::vector<double> dist_sf = dist.get_smooth_DDT();
    std::vector<uchar> uchar_vec;
    dist.draw_isolines_for_field(uchar_vec, dist_f,  input.cols, input.rows ,"unsmoothed");
    dist.draw_isolines_for_field(uchar_vec, dist_sf, input.cols, input.rows , "smoothed");*/

    function_rep::geometry_params geo;
    geo.start_P.x = 216.0;
    geo.start_P.y = 216.0;
    geo.rad       = 50.0;
    geo.blob_A    = 0.5;
    geo.blob_B    = 0.3;
    function_rep::HybrydFunctionRep hfrep(set_geometry(args::geometry), geo, 512, 512, 0, CV_8UC1);


    //ISDDT = zoom_in_field(&SDDT, cv::Point2i(170, 170), cv::Vec2i(128, 128), cv::Vec2i(new_src.cols, new_src.rows), res_x);

    return 0;
}
