#ifndef FUNCTION_REPRESENTATION
#define FUNCTION_REPRESENTATION

#include "include/DDT.h"
#include "include/DrawField.h"

#include "opencv2/core.hpp"

#include <vector>

namespace function_rep {

enum class geometry
{
    CIRCLE,
    BLOBBY,
    HEART,
    BUTTERFLY,
    NONE
};

struct geometry_params
{
    double rad;
    double blob_phi1;
    double blob_phi2;
    double blob_A;
    double blob_B;
    double but_U;
    cv::Point2d start_P ;
};

class HybrydFunctionRep {

public:
    HybrydFunctionRep(geometry geo, geometry_params params, int res_x, int res_y, int ddt_sh, int im_type);
    std::shared_ptr<distance_transform::DistanceField> DT;
    std::shared_ptr<draw::DrawField> drawF;

    void generate_frep(geometry geo , std::string file_name="");
    void generate_hfrep(std::vector<double> DistTr, std::vector<double> *hfrep , std::string file_name="");
    void check_HFrep(std::vector<double> hfrep );

    inline std::vector<double> get_hfrep_vec() { return HFRep_vec; }
    inline std::vector<double> get_frep_vec()  { return FRep_vec;  }

    cv::Mat get_FRep_im(std::vector<double> input, std::string file_name = "");
    cv::Mat get_HFRep_im(std::vector<double> input, std::string file_name = "");

private:
    double get_step_function_val(double frep_val, int function);

    inline double intersect_function(double a, double b) { return a + b - std::sqrt(a * a + b * b); }
    inline double union_function(double a, double b)     { return a + b + std::sqrt(a * a + b * b); }
    inline double subtract_function(double a, double b)  { return intersect_function(a, -b); }

private:
    std::vector<double> dist_tr;
    std::vector<double> HFRep_vec;
    std::vector<double> FRep_vec;

    cv::Mat FRep_im, HFRep_im;

    cv::Point2d start_p;

    double R, but_u;
    double bl_phi1, bl_phi2, bl_a, bl_b;

    int resolution_x, resolution_y;
    int cv_im_type;

};

} // namespace function_rep
#endif
