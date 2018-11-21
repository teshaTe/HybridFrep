#ifndef FUNCTION_REPRESENTATION
#define FUNCTION_REPRESENTATION

#include "include/DDT.h"
#include "include/DrawField.h"
#include "include/ModifyField.h"

#include "opencv2/core.hpp"

#include <vector>

namespace function_rep {

enum class geometry
{
    BLOBBY,
    BORG,
    BUTTERFLY,
    CHAIR,
    CIRCLE,
    HEART,
    NONE
};

struct geometry_params
{
    double rad;
    cv::Point2d start_P ;
};

class HybrydFunctionRep {

public:
    HybrydFunctionRep(geometry geo, geometry_params params, int res_x, int res_y, int ddt_sh, int im_type);
    std::shared_ptr<draw::DrawField> drawF;
    std::shared_ptr<modified_field::ModifyField> modF;

    void generate_frep(geometry geo , std::string file_name="");
    void generate_hfrep(std::vector<double> *hfrep , const std::vector<double> frep,
                        const std::vector<double> *DistTr, std::string file_name="");
    void check_HFrep(std::vector<double> hfrep , std::string hfrep_check_name);

    cv::Mat get_FRep_im(const std::vector<double> *input, geometry geo, std::string file_name = "");

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

private:
    std::vector<double> dist_tr;
    std::vector<double> HFRep_vec;
    std::vector<double> FRep_vec;
    std::vector<double> ZoomedFRep_vec;
    std::vector<double> sm_dist_tr;

    cv::Mat FRep_im, HFRep_im;

    cv::Point2d start_p;

    double R, but_u;
    double bl_phi1, bl_phi2, bl_a, bl_b;

    int resolution_x, resolution_y;
    int cv_im_type;

};

} // namespace function_rep
#endif
