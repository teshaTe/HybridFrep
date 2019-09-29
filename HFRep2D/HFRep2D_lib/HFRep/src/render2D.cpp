#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "render2D.h"

#include "opencv2/highgui.hpp"

//#include <SOIL/SOIL.h>

#include <algorithm>

namespace hfrep2D {

cv::Mat render2D::convert_field_to_image( std::vector<uchar> *ufield, const std::vector<float> *field,
                                          const int res_x, const int res_y )
{
    ufield->clear();
    cv::Mat dst = cv::Mat( res_x, res_y, CV_8UC1 );
    ufield->resize(res_x*res_y);

    float dist;

    for( int i = 0; i < res_x*res_y; i++ )
    {
        if( std::abs(field->at(i)) < 0.0009f )
            dist = std::abs( field->at(i) ) * 20.0f*1000.0f + 128.0f;
        else
            dist = std::abs( field->at(i) ) * 20.0f + 128.0f;

        if( dist < 0.0f )
            ufield->at(i) = 0;
        else if( dist > 255.0f )
            ufield->at(i) = 255;
        else
            ufield->at(i) = static_cast<uchar>(dist);
    }

    memcpy(dst.data, ufield->data(), ufield->size()*sizeof (uchar));
    return dst;
}

void render2D::drawField( std::vector<uchar> *ufield, const std::vector<float> *field,
                          const int res_x, const int res_y, std::string file_name )
{
    cv::Mat dst = convert_field_to_image( ufield, field, res_x, res_y);
    std::string ddt_file_name = "field_im_" + file_name + ".png";

    cv::imwrite(ddt_file_name, dst);
}

void render2D::drawGrey_isolines( std::vector<uchar> *ufield, const std::vector<float> *field,
                                 const int res_x, const int res_y, std::string file_name )
{
    cv::Mat dst = convert_field_to_image( ufield, field, res_x, res_y );
    std::vector<uchar> ISO_UDDT;
    ISO_UDDT.resize(field->size());
    int col_st = 5;

    for(int i = 0; i < res_y*res_x; i++)
    {
        for(int j = 0; j <= 255-col_st; j+=col_st)
        {
            if( j <= ufield->at(i) && ufield->at(i) <= j+col_st )
            {
                ISO_UDDT[i] = j+col_st;
                break;
            }
        }
    }

    dst = cv::Mat( res_x, res_y, CV_8UC1 );
    memcpy(dst.data, ISO_UDDT.data(), ISO_UDDT.size()*sizeof (uchar));

    std::string ddt_file_name = "isoline_field_grey_" + file_name + ".png";
    cv::imwrite(ddt_file_name, dst);
}

void render2D::drawRGB_isolines( const std::vector<float> *field, const int res_x, const int res_y,
                                 float thres, std::string file_name, float thres_iso )
{
    cv::Mat dst = cv::Mat( res_x, res_y, CV_8UC3 );

    std::vector<uchar> dstImg, ufield; dstImg.resize(res_x * res_y * 3);

    //unsigned char *buff =  static_cast<unsigned char*>(dstImg.data());

    unsigned char *dst_buf = static_cast<unsigned char*>(dst.data);
    int d_ch      = dst.channels();
    size_t d_step = dst.step;
    int col_st    = 5;

    convert_field_to_image(&ufield, field, res_x, res_y);

    int col_ind = 0;

    for(int y = 0; y < res_y; y++)
    {
        for(int x = 0; x < res_x; x++)
        {
            for(int i = 0; i <= 255-col_st; i+=col_st)
            {
                if( field->at(x+y*res_x) < 0.0f )
                {
                    float val = field->at(x+y*res_x);
                    float indicator = std::fmod(val, static_cast<float>(1.0/col_st));

                    if( indicator < 0.0f && indicator > -thres_iso )
                    {
                        dst_buf[x*d_ch+y*d_step]   = 100;
                        dst_buf[x*d_ch+1+y*d_step] = 100;
                        dst_buf[x*d_ch+2+y*d_step] = 100;
                        break;
                    }

                    if( i <= ufield[x+y*res_x] && ufield[x+y*res_x] <= i+col_st )
                    {
                        dst_buf[x*d_ch+y*d_step]   = i+col_st;
                        dst_buf[x*d_ch+1+y*d_step] = i+col_st;
                        dst_buf[x*d_ch+2+y*d_step] = 0;
                        break;
                    }
                }
                else if( field->at(x+y*res_x) < thres && field->at(x+y*res_x) >= 0.0f )
                {
                    dst_buf[x*d_ch+y*d_step]   = 0;
                    dst_buf[x*d_ch+1+y*d_step] = 0;
                    dst_buf[x*d_ch+2+y*d_step] = 0;
                    break;
                }
                else
                {
                    float val = field->at(x+y*res_x);
                    float indicator = std::fmod(val, static_cast<float>(1.0/col_st));

                    if( indicator > 0.0f && indicator < thres_iso )
                    {
                        dst_buf[x*d_ch+y*d_step]   = 200;
                        dst_buf[x*d_ch+1+y*d_step] = 200;
                        dst_buf[x*d_ch+2+y*d_step] = 200;
                        break;
                    }

                    if( i <= ufield[x+y*res_x] && ufield[x+y*res_x] <= i+col_st )
                    {
                        dst_buf[x*d_ch+y*d_step]   = 0;
                        dst_buf[x*d_ch+1+y*d_step] = i+col_st;
                        dst_buf[x*d_ch+2+y*d_step] = i+col_st;
                        break;
                    }
                }
            }
        }
    }

    std::string field_file_name = "isoline_field_" + file_name + ".jpg";
    cv::imwrite(field_file_name, dst);
    dst.copyTo(res);

    //stbi_write_jpg(field_file_name.c_str(), res_x, res_y, 3, buff, -3*res_x );
}


} // namespace hfrep2D
