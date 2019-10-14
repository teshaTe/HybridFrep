#ifndef H_MODIFY_FIELD
#define H_MODIFY_FIELD

#include "helperFunctions.hpp"
#include <vector>
#include <cmath>

namespace hfrep2D {

enum interpolation
{
    BILINEAR,
    BICUBIC
};

class ModifyField
{

public:
    ModifyField()  {}
    ~ModifyField() {}

    std::vector<float> smooth_field ( const std::vector<float> *DDT, int resX, int resY);
    std::vector<float> zoom_field   ( const std::vector<float> *field, const Point2Di start_p,
                                      const Point2Di reg_s, const Point2Di fin_res);
    std::vector<float> interpolate_field( const std::vector<float> *field, const Point2Di cur_res,
                                          const Point2Di fin_res, interpolation inter );

    float get_bilinear_interpolated_val ( const std::vector<float> *DT, Point2Di res, Point2Df indXY );
    float get_bicubic_interpolated_val  ( const std::vector<float> *DT, Point2Di res, Point2Df indXY );

    std::vector<float> diff_fields(const std::vector<float> *field1, const std::vector<float> *field2 , float multi);
    float calcAverageFieldError( const std::vector<float> *field1, const std::vector<float> *field2, float *inside, float *outside );

private:
    float get_cubic_interpolated_val(const std::vector<float> *DT, Point2Di res, Point2Df indXY , float var, int shY);

    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max : ( n < n_min ? n_min : n );}

private:
    int pix_xShift, pix_yShift;

};

}//namespace hfrep2d

#endif
