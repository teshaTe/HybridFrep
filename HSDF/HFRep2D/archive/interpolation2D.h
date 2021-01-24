#ifndef H_MODIFY_FIELD
#define H_MODIFY_FIELD

#include <vector>
#include <cmath>
#include <glm/glm.hpp>

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
    std::vector<float> zoom_field   ( const std::vector<float> *field, const glm::i32vec2 start_p,
                                      const glm::i32vec2 reg_s, const glm::i32vec2 fin_res);
    std::vector<float> interpolate_field( const std::vector<float> *field, const glm::i32vec2 cur_res,
                                          const glm::i32vec2 fin_res, interpolation inter );

    float get_bilinear_interpolated_val ( const std::vector<float> *DT, glm::i32vec2 res, glm::f32vec2 indXY );
    float get_bicubic_interpolated_val  ( const std::vector<float> *DT, glm::i32vec2 res, glm::f32vec2 indXY );
    float get_bicubic_val_in_quad_cell( const std::vector<float> *dField, glm::i32vec2 res );

    std::vector<float> diff_fields(const std::vector<float> *field1, const std::vector<float> *field2 , float multi);
    float calcAverageFieldError( const std::vector<float> *field1, const std::vector<float> *field2, float *inside, float *outside );

private:
    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max : ( n < n_min ? n_min : n );}

private:
    int pix_xShift, pix_yShift;

};

}//namespace hfrep2d

#endif
