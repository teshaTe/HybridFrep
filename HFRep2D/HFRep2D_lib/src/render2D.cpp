#include "include/render2D.h"
#include "opencv2/highgui.hpp"

#include <algorithm>

namespace hfrep2D {

cv::Mat render2D::convert_field_to_image( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                                          const int res_x, const int res_y )
{
    uchar_field->clear();
    cv::Mat dst = cv::Mat( res_x, res_y, CV_8UC1 );
    uchar_field->resize(res_x*res_y);

    float dist;

    for( int i = 0; i < res_x*res_y; i++ )
    {
        if( std::abs(field->at(i)) < 0.0009f )
            dist = std::abs( field->at(i) ) * 20.0f*1000.0f + 128.0f;
        else
            dist = std::abs( field->at(i) ) * 20.0f + 128.0f;

        if( dist < 0.0f )
            uchar_field->at(i) = 0;
        else if( dist > 255.0f )
            uchar_field->at(i) = 255;
        else
            uchar_field->at(i) = static_cast<uchar>(dist);
    }

    memcpy(dst.data, uchar_field->data(), uchar_field->size()*sizeof (uchar));
    return dst;
}

void render2D::drawField( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                          const int res_x, const int res_y, std::string file_name )
{
    cv::Mat dst = convert_field_to_image( uchar_field, field, res_x, res_y);
    std::string ddt_file_name = "field_im_" + file_name + ".png";
    cv::imwrite(ddt_file_name, dst);
}

void render2D::drawGrey_isolines( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                                 const int res_x, const int res_y, std::string file_name )
{
    convert_field_to_image( uchar_field, field, res_x, res_y );
    std::vector<uchar> ISO_UDDT;
    ISO_UDDT.resize(field->size());
    int col_st = 5;

    for(int i = 0; i < res_y*res_x; i++)
    {
        for(int j = 0; j <= 255-col_st; j+=col_st)
        {
            if( j <= uchar_field->at(i) && uchar_field->at(i) <= j+col_st )
            {
                ISO_UDDT[i] = j+col_st;
                break;
            }

        }
    }

    cv::Mat dst(res_x, res_y, CV_8UC1);
    memcpy(dst.data, ISO_UDDT.data(), ISO_UDDT.size()*sizeof (uchar));

    std::string ddt_file_name = "isoline_field_grey_" + file_name + ".png";
    cv::imwrite(ddt_file_name, dst);
}

void render2D::drawRGB_isolines( std::vector<uchar> *uchar_field, const std::vector<float> *field,
                                const int res_x, const int res_y, float thres, std::string file_name, bool sign )
{
    cv::Mat dst = cv::Mat( res_x, res_y, CV_8UC3 );
    unsigned char *dst_buf = static_cast<unsigned char*>(dst.data);
    int d_ch      = dst.channels();
    size_t d_step = dst.step;
    int col_st    = 5;

    convert_field_to_image(uchar_field, field, res_x, res_y);

    for(int y = 0; y < dst.rows; y++)
    {
        for(int x = 0; x < dst.cols; x++)
        {
            if( sign )
            {
                for(int i = 0; i <= 255-col_st; i+=col_st)
                {
                    if( field->at(x+y*res_x) < 0.0f )
                    {
                        if( i <= uchar_field->at(x+y*res_x) && uchar_field->at(x+y*res_x) <= i+col_st )
                        {
                            dst_buf[x*d_ch+y*d_step]   = i+col_st;
                            dst_buf[x*d_ch+1+y*d_step] = i+col_st;
                            dst_buf[x*d_ch+2+y*d_step] = 0;
                            break;
                        }
                    }
                    else if( field->at(x+y*res_x) < thres && field->at(x+y*res_x) > 0.0f )
                    {
                        dst_buf[x*d_ch+y*d_step]   = 0;
                        dst_buf[x*d_ch+1+y*d_step] = 0;
                        dst_buf[x*d_ch+2+y*d_step] = 0;
                        break;
                    }
                    else if( field->at(x+y*res_x) > 0.0 )
                    {
                        if( i <= uchar_field->at(x+y*res_x) && uchar_field->at(x+y*res_x) <= i+col_st )
                        {
                            dst_buf[x*d_ch+y*d_step]   = 0;
                            dst_buf[x*d_ch+1+y*d_step] = i+col_st;
                            dst_buf[x*d_ch+2+y*d_step] = i+col_st;
                            break;
                        }
                    }
                }
            }
            else
            {
                for(int i = 0; i <= 255-col_st; i+=col_st)
                {
                    if( i <= uchar_field->at(x+y*res_x) && uchar_field->at(x+y*res_x) <= i+col_st )
                    {
                        if( i%2 == 0 )
                        {
                            dst_buf[x*d_ch+y*d_step]   = 9;
                            dst_buf[x*d_ch+1+y*d_step] = i+col_st;
                            dst_buf[x*d_ch+2+y*d_step] = 0;
                            break;
                        }
                        else if( i%3 == 0)
                        {
                            dst_buf[x*d_ch+y*d_step]   = 0;
                            dst_buf[x*d_ch+1+y*d_step] = 214;
                            dst_buf[x*d_ch+2+y*d_step] = i+col_st;
                            break;
                        }
                        else
                        {
                            dst_buf[x*d_ch+y*d_step]   = 0;
                            dst_buf[x*d_ch+1+y*d_step] = 0;
                            dst_buf[x*d_ch+2+y*d_step] = i+col_st;
                            break;
                        }
                    }

                    if( field->at(x+y*res_x) < thres && field->at(x+y*res_x) > 0.0f )
                    {
                        dst_buf[x*d_ch+y*d_step]   = 0;
                        dst_buf[x*d_ch+1+y*d_step] = 0;
                        dst_buf[x*d_ch+2+y*d_step] = 0;
                        break;
                    }
                }
            }
        }
    }

    std::string field_file_name = "isoline_field_" + file_name + ".jpeg";
    cv::imwrite(field_file_name, dst);
}


} // namespace hfrep2D
