#ifndef FUNCTION_REPRESENTATION
#define FUNCTION_REPRESENTATION

#include "include/DDT.h"
#include "include/DrawField.h"
#include "include/ModifyField.h"

#include "opencv2/core.hpp"

#include <vector>

namespace function_rep {
#define HYPERBOLIC_TANGENT 1
#define HYPERBOLIC_SIGMOID 2
#define ALGEBRAIC          3
#define GUDERMANIAN        4

enum class geometry
{
    BLOBBY,
    BORG,
    BUTTERFLY,
    CHAIR,
    CIRCLE,
    HEART,
    ELF,
    SURIKEN,
    QUAD,
    NONE
};

struct geometry_params
{
    double rad;
    double zoom;
    double thres_vis_hfrep;
    double thres_vis_ddt;
    cv::Point2d start_P ;
};

struct step_function_params
{
    double tangent_slope;     //
    double sigmoid_slope;     //
    double algebraic_slope;   //
    double guderanian_slope;  //
};

class HybrydFunctionRep {

public:
    HybrydFunctionRep( geometry geo, geometry_params params, int step_function,
                       step_function_params st_fun, int res_x, int res_y, int ddt_sh, int im_type );
    HybrydFunctionRep( geometry geo, geometry_params params, int step_function,
                       step_function_params st_fun, int res_x, int res_y, int ddt_sh );
    std::shared_ptr<draw::DrawField> drawF;
    std::shared_ptr<modified_field::ModifyField> modF;

    std::vector<double> generate_frep(geometry geo, int res_x, int res_y, geometry_params p, std::string file_name = "" );
    void generate_hfrep(std::vector<double> *hfrep , const std::vector<double> frep,
                         const std::vector<double> *DistTr, cv::Vec2i res, int step_function, std::string file_name = "" );
    void check_HFrep( std::vector<double> hfrep , std::string hfrep_check_name );

    cv::Mat get_FRep_im( const std::vector<double> *input, geometry geo, std::string file_name = "" );

// functions for getting calculated vectors
public:
    inline std::vector<double> get_hfrep_vec()    { return HFRep_vec; }
    inline std::vector<double> get_frep_vec()     { return FRep_vec;  }
    inline std::vector<double> get_DDT()          { return dist_tr; }
    inline std::vector<double> get_smoothed_DDT() { return sm_dist_tr; }

private:
    std::shared_ptr<distance_transform::DistanceField> DT;
    double get_step_function_val(double frep_val, int function);

    inline double intersect_function(double a, double b) { return a + b - std::sqrt(a * a + b * b); }
    inline double union_function(double a, double b)     { return a + b + std::sqrt(a * a + b * b); }
    inline double subtract_function(double a, double b)  { return intersect_function(a, -b); }

    double generate_elf_model(cv::Point2d pos, cv::Vec2i res);
    double ellipsoid_2d (cv::Point2d pos, cv::Point2d cent, double a, double b , int resx);
    double ellipticCylZ (cv::Point2d pos, cv::Point2d cent, double a, double b , int resx);
    double torusY_2d( cv::Point2d pos, cv::Point2d cent, double r, double R );
    double torusZ_2d( cv::Point2d pos, cv::Point2d cent, double r, double R );
    double sphere_2d( cv::Point2d pos, cv::Point2d cent, double r );


private:
    std::vector<double> dist_tr;
    std::vector<double> HFRep_vec;
    std::vector<double> FRep_vec;
    std::vector<double> ZoomedFRep_vec;
    std::vector<double> sm_dist_tr;

    cv::Mat FRep_im, HFRep_im;

    cv::Point2d start_p;

    double R;
    double tangent_slope;
    double sigmoid_slope;
    double algebraic_slope;
    double guardanian_slope;

    int resolution_x, resolution_y;
    int cv_im_type;

};

} // namespace function_rep
#endif
