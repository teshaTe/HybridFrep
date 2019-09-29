#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iterator>

#include "frep2D.h"
#include "render2D.h"
#include "interpolation2D.h"
#include "ddt2D.h"
#include "hfrep2D.h"

int main(int argc, char** argv)
{
    int step_function = HYPERBOLIC_SIGMOID;
    float slope = 0.0001f;

    float thres_viz_frep  = 0.009f;
    float thres_vis_ddt   = 0.04f;
    float thres_vis_hfrep = 0.04f;

    hfrep2D::FRepObj2D frep( 512, 512, 1.0f );
    hfrep2D::DiscreteDistanceTransform DT(512, 512);
    hfrep2D::HFRepObj2D  hfrep( 512, 512, 0 );
    hfrep2D::ModifyField modField;
    hfrep2D::render2D    drawField;

    // cv::Point2i(170, 170) is good for all shapes except heart; heart is cv::Point2i(150, 350)
    hfrep2D::Point2Di start_regP_heart( 185, 350);
    hfrep2D::Point2Di start_regP ( 170, 170 );

    //******************************************************************************************************
    //Zooming operation
    //******************************************************************************************************

    //getting distance transform, zoom in, smooth, draw it
    auto fun1 = std::bind(&hfrep2D::FRepObj2D::elf, frep, std::placeholders::_1 );
    std::vector<float> elf = frep.getFRep2D(fun1);

    drawField.drawRGB_isolines( &elf, 512, 512, 0.0001f, "elf_frep" );

    DT.caclulateDiscreteDistanceTransformVec( &elf );
    std::vector<float> ddt1  = DT.get_DDT();
    std::vector<float> sddt1 = DT.get_signed_DDT();
    std::vector<float> zoomed_ddt = modField.zoom_field( &ddt1, start_regP, hfrep2D::Point2Di(128, 128),
                                                          hfrep2D::Point2Di(512, 512));

    // zoom in frep field and draw it
    std::vector<float> zoomed_frep = modField.zoom_field( &elf, start_regP, hfrep2D::Point2Di(128, 128),
                                                          hfrep2D::Point2Di(512, 512));

    // generate zoomed in HFrep and draw it
    std::vector<float> ZM_hfrep_vec;
    ZM_hfrep_vec = hfrep.calculateHFRep2D( &zoomed_frep, &zoomed_ddt, step_function, slope, true );

    drawField.drawRGB_isolines( &ZM_hfrep_vec, 512, 512, 0.09f, "zoomed_hfrep" );

    //draw the difference between hfrep and ddt
    std::vector<float> sm_zoomed_ddt = modField.smooth_field(&zoomed_ddt, 512, 512 );
    std::vector<float> diff_field = modField.diff_fields( &ZM_hfrep_vec, &sm_zoomed_ddt, 100000.0f );
    drawField.drawRGB_isolines( &diff_field, 512, 512, 0.0, "zoomed_diff_field");

    //******************************************************************************************************
    //interpolation if the field is sparse
    //******************************************************************************************************

    //generating frep in full resolution
    //obtain sparse values for DDT
    hfrep.calculateHFRep2D( &elf, &ddt1, step_function, slope, true );

    auto start_sp_ddt = std::chrono::system_clock::now();
    std::vector<float> sp_ddt, sp_sddt;
    int stX = 512/128, stY= 512/128;
    for(int y = 0; y < 512; y+=stY )
    {
        for(int x = 0; x < 512; x+=stX)
        {
            sp_ddt.push_back( ddt1[x+y*512] );
            sp_sddt.push_back( sddt1[x+y*512] );
        }
    }
    auto end_sp_ddt = std::chrono::system_clock::now();
    std::chrono::duration<float> t_sp_ddt = end_sp_ddt - start_sp_ddt;

    //interpolate between sparse values for DDT
    auto start_int_ddt = std::chrono::system_clock::now();
    std::vector<float> inter_ddt  = modField.interpolate_field( &sp_ddt, hfrep2D::Point2Di(128,128),
                                                                hfrep2D::Point2Di(512,512), hfrep2D::BICUBIC );
    std::vector<float> inter_sddt = modField.interpolate_field( &sp_sddt, hfrep2D::Point2Di(128,128),
                                                                hfrep2D::Point2Di(512,512), hfrep2D::BICUBIC );
    auto end_int_ddt = std::chrono::system_clock::now();
    std::chrono::duration<float> t_int_ddt = end_int_ddt - start_int_ddt;

    drawField.drawRGB_isolines( &inter_sddt, 512, 512, thres_vis_ddt , "sddt_512x512" );
    drawField.drawRGB_isolines( &sp_ddt, 128, 128, thres_vis_ddt , "ddt_128x128" );

    auto start_hfrep = std::chrono::system_clock::now();
    std::vector<float> hfrep0 = hfrep.calculateHFRep2D( &elf, &inter_ddt, step_function, slope, true );
    auto end_hfrep   = std::chrono::system_clock::now();
    std::chrono::duration<float> t_hfrep = end_hfrep - start_hfrep;

    drawField.drawRGB_isolines( &hfrep0, 512, 512, thres_vis_hfrep , "hfrep_512x512");

    //calculate the difference between fields
    std::vector<float> sm_inter_ddt = modField.smooth_field(&inter_ddt, 512, 512);
    std::vector<float> diff_field_inter = modField.diff_fields( &hfrep0, &sm_inter_ddt, 100000.0 );
    drawField.drawRGB_isolines( &diff_field_inter, 512, 512, 0.0, "diff_hfrep_512x512" );

    //******************************************************************************************************
    //estimating error for interpolated case
    //******************************************************************************************************
    std::vector<float> dField_inter = modField.diff_fields( &hfrep0, &inter_ddt, 1.0f );

    std::vector<float> pos_val;
    std::copy_if( dField_inter.begin(), dField_inter.end(), std::back_inserter(pos_val),
                  std::bind(std::less<float>(), 0.0, std::placeholders::_1) );

    auto val = std::minmax_element( pos_val.begin(), pos_val.end() );
    auto min_inter   = pos_val[val.first  - pos_val.begin()];
    auto max_inter   = pos_val[val.second - pos_val.begin()];
    float aver_inter = std::accumulate(pos_val.begin(), pos_val.end(), 0.0) / float(pos_val.size());

    std::cout << "\nError after using bicubic interpolation: " << std::endl;
    std::cout << " min_inter = " << min_inter <<" ; max_inter = " << max_inter <<
                 " ; aver_inter = " << aver_inter << std::endl;

    std::cout << "Timings: " << std::endl;
    std::cout << "DDT_sparse ex. [sec.]  : " << t_sp_ddt.count()  << std::endl;
    std::cout << "DDT_interpol.ex.[sec.] : " << t_int_ddt.count() << std::endl;
    std::cout << "HFRep ex. [sec.]       : " << t_hfrep.count()   << std::endl;

    return 0;
}
