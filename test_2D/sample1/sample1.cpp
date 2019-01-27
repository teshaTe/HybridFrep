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

    double thres_vis_frep;
    if(args::geometry == "elf" && !args::geometry.empty())
        thres_vis_frep = 0.0001;
    else
        thres_vis_frep = 0.009;

    hfrep.calculateHFRep( set_geometry("suriken"), geo, step_function, st_fun, nullptr, nullptr );

    std::vector<double> hfrep0  = hfrep.getHFRepVec();
    std::vector<double> sddt0   = hfrep.getSignedDDT();
    std::vector<double> sm_ddt0 = modField.smooth_field( &sddt0, 512, 512 );
    hfrep.checkHFrep( &hfrep0 );

    hfrep.calculateHFRep( set_geometry("quad"), geo, step_function, st_fun, nullptr, nullptr );

    std::vector<double> hfrep1  = hfrep.getHFRepVec();
    std::vector<double> sddt1   = hfrep.getSignedDDT();
    std::vector<double> sm_ddt1 = modField.smooth_field( &sddt1, 512, 512 );
    hfrep.checkHFrep( &hfrep1 );

    std::vector<double> combo_hfrep, combo_ddt;
    for (size_t i = 0; i < static_cast<size_t>(512*512); i++)
    {
        combo_hfrep.push_back( hfrep.union_function(hfrep0[i], hfrep1[i]) );
        combo_ddt.push_back(std::max(sm_ddt0[i], sm_ddt1[i]));
    }

    std::vector<uchar> uField;
    drawField.draw_rgb_isolines( &uField, &combo_hfrep, 512, 512, thres_vis_hfrep, "combo_hfrep", true );
    uField.clear();
    drawField.draw_rgb_isolines( &uField, &combo_ddt, 512, 512, thres_vis_ddt, "combo_ddt", true );

    return 0;
}
