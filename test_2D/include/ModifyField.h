#ifndef H_MODIFY_FIELD
#define H_MODIFY_FIELD

#include "opencv2/core.hpp"

#include <vector>

namespace modified_field {

enum interpolation
{
    BILINEAR,
    BICUBIC,
    BSPLINE
};

class ModifyField
{

public:
    ModifyField()  {}
    ~ModifyField() {}

    std::vector<double> smooth_field ( const std::vector<double> *DDT,   int resX, int resY);
    std::vector<double> zoom_field   ( const std::vector<double> *field, const cv::Point2i start_p,
                                       const cv::Vec2i reg_s, const cv::Vec2i fin_res);
    std::vector<double> interpolate_field( const std::vector<double> *field, const cv::Vec2i cur_res, const cv::Vec2i fin_res);
    double get_bilinear_interpolated_val (const std::vector<double> *DT, cv::Vec2i res, cv::Vec2f indXY );

    std::vector<double> diff_fields(const std::vector<double> *field1, const std::vector<double> *field2 , double multi);

private:
    //std::vector<double> generate_b_spline_interpolated_field   ( const std::vector<double> *DT, int resX, int resY, int shiftX, int shiftY );
    std::vector<double> generate_bicubic_interpolated_field    ( const std::vector<double> *DT, int resX, int resY, int shiftX, int shiftY );
    std::vector<double> generate_bilinearly_interpolated_field ( const std::vector<double> *DT, int resX, int resY, int shiftX, int shiftY );
    std::vector<double> generate_extrapolated_field( const std::vector<double> DDT, int interpolation_type = 0 );

    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max : ( n < n_min ? n_min : n );}

private:
    int pix_xShift, pix_yShift;

};

}//namespace modified_field

#endif
