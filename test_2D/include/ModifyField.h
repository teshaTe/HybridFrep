#ifndef H_MODIFY_FIELD
#define H_MODIFY_FIELD

#include "opencv2/core.hpp"

#include <vector>

namespace modified_field {

class ModifyField
{

public:
    ModifyField()  {}
    ~ModifyField() {}

    std::vector<double> zoom_in_field  (const std::vector<double> *field, const cv::Point2i start_p,
                                        const cv::Vec2i reg_s, const cv::Vec2i fin_res, int b_sh);
    std::vector<double> finalize_field (const std::vector<double> *field, int resX, int resY, int b_sh);
    std::vector<double> smooth_field   (const std::vector<double> *DDT,   int resX, int resY);

private:
    std::vector<double> generate_interpolated_field(const std::vector<double> *DT, int resX, int resY,
                                                    int shiftX, int shiftY, int interpolation_type = 0 );
    std::vector<double> generate_extrapolated_field( const std::vector<double> DDT, int interpolation_type = 0 );

private:
    int pix_xShift, pix_yShift;

};

}//namespace modified_field

#endif
