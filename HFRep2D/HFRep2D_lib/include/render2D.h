#ifndef DRAW_FIELD
#define DRAW_FIELD

#include "opencv2/core.hpp"

namespace hfrep2D {

class render2D
{
public:
    render2D() { }
    ~render2D() { }

    cv::Mat convert_field_to_image( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                                    int res_x, int res_y );

    void drawField         ( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                             int res_x, int res_y, std::string file_name="" );
    void drawGrey_isolines ( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                            int res_x, int res_y, std::string file_name="" );
    void drawRGB_isolines  ( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                            int res_x, int res_y, float thres, std::string file_name="" , bool sign = false);
};

} // namespace draw

#endif //define DRAW_FIELD
