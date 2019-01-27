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

struct geometryParams
{
    double rad;
    double zoom;
    cv::Point2d start_P ;
};

struct stepFuncParams
{
    double tangent_slope;     //
    double sigmoid_slope;     //
    double algebraic_slope;   //
    double guderanian_slope;  //
};

class HybrydFunctionRep
{

public:
    HybrydFunctionRep( int res_x, int res_y, int ddt_sh );

    std::vector<double> precalcDDT(geometry geo, geometryParams p , bool sign = false);

    void calculateHFRep(geometry geo, geometryParams p, int stepfunc, stepFuncParams stFunP, std::vector<double> *frep,
                                                                   std::vector<double> *ddt, std::string file_name = "" );
    void checkHFrep(std::vector<double> *hfrep , std::string hfrep_check_name = "" );

    // functions for getting calculated vectors
    void getFieldImages(double thres_vis_frep, double thres_vis_ddt, double thres_vis_hfrep, std::string file_name = "" );

    inline std::vector<double> getHFRepVec()    { return HFRep_vec; }
    inline std::vector<double> getFRepVec()     { return FRep_vec;  }
    inline std::vector<double> getDDT()         { return dist_tr; }
    inline std::vector<double> getSignedDDT()   { return signDist_tr; }
    inline std::vector<double> getSmoothedDDT() { return sm_dist_tr; }

    inline std::vector<double> precalcFRep(geometry geo, geometryParams p) { return generateFRep( geo, p ); }

    inline void setNewRes( int resX, int resY )  { resolution_x = resX; resolution_y = resY; }

private:
    std::shared_ptr<distance_transform::DiscreteDistanceTransform> DT, DT_custom;
    std::shared_ptr<draw::DrawField> drawF;
    std::shared_ptr<modified_field::ModifyField> modF;

    double get_step_function_val(double frep_val, int function);

    inline double intersect_function(double a, double b) { return a + b - std::sqrt(a * a + b * b); }
    inline double union_function(double a, double b)     { return a + b + std::sqrt(a * a + b * b); }
    inline double subtract_function(double a, double b)  { return intersect_function(a, -b); }

    std::vector<double> generateFRep(geometry geo, geometryParams p, std::string file_name = "" );
    void generateHFRep( int step_function, std::string file_name = "" );

    double generate_elf_model(cv::Point2d pos, cv::Vec2i res);
    double ellipsoid_2d (cv::Point2d pos, cv::Point2d cent, double a, double b , int resx);
    double ellipticCylZ (cv::Point2d pos, cv::Point2d cent, double a, double b , int resx);
    double torusY_2d( cv::Point2d pos, cv::Point2d cent, double r, double R );
    double torusZ_2d( cv::Point2d pos, cv::Point2d cent, double r, double R );
    double sphere_2d( cv::Point2d pos, cv::Point2d cent, double r );


private:
    std::vector<double> dist_tr;
    std::vector<double> sm_dist_tr;
    std::vector<double> signDist_tr;
    std::vector<double> HFRep_vec;
    std::vector<double> FRep_vec;

    cv::Point2d start_p;

    double R;
    double tangent_slope;
    double sigmoid_slope;
    double algebraic_slope;
    double guardanian_slope;

    int resolution_x, resolution_y;
    int cv_im_type;
    int dist_sh;

};

} // namespace function_rep
#endif
