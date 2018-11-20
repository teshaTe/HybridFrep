#ifndef DRAW_FIELD
#define DRAW_FIELD

#include "opencv2/core.hpp"

#include "include/DDT.h"

namespace draw {

class DrawField
{
public:
    DrawField() { }
    ~DrawField() { }

    cv::Mat convert_field_to_image(std::vector<uchar> *uchar_field, const std::vector<double> *field,  int res_x,  int res_y );

    void draw_field         ( std::vector<uchar> *uchar_field, const std::vector<double> *field, int res_x, int res_y, std::string file_name="" );
    void draw_grey_isolines ( std::vector<uchar> *uchar_field, const std::vector<double> *field, int res_x, int res_y, std::string file_name="" );
    void draw_rgb_isolines  ( std::vector<uchar> *uchar_field, const std::vector<double> *field, int res_x, int res_y, std::string file_name="" );
};

} // namespace draw

#endif //define DRAW_FIELD
