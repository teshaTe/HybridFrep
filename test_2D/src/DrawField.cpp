#include "include/DrawField.h"
#include "opencv2/highgui.hpp"

namespace draw {

cv::Mat DrawField::convert_field_to_image(std::vector<uchar> *uchar_field, std::vector<double> field, int res_x, int res_y)
{
    cv::Mat dst = cv::Mat( res_x, res_y, CV_8UC1 );
    uchar_field->resize(field.size());

    for( int i = 0; i < res_x*res_y; i++ )
    {
        double dist = std::abs(field[i]) * 20.0 + 128.0;
        if( dist < 0.0 )
            uchar_field->at(i) = 0;
        else if( dist > 255.0 )
            uchar_field->at(i) = 255;
        else
            uchar_field->at(i) = static_cast<uint>(dist);
    }

    memcpy(dst.data, uchar_field->data(), uchar_field->size()*sizeof (uchar));
    return dst;
}

void DrawField::draw_field( std::vector<uchar> uchar_field, std::vector<double> field, int res_x, int res_y, std::string file_name )
{
    cv::Mat dst = convert_field_to_image( &uchar_field, field, res_x, res_y);
    std::string ddt_file_name = "field_im_" + file_name + ".png";
    cv::imwrite(ddt_file_name, dst);
}

void DrawField::draw_grey_isolines(std::vector<uchar> uchar_field, std::vector<double> field, int res_x, int res_y, std::string file_name)
{
    convert_field_to_image(&uchar_field, field, res_x, res_y);
    std::vector<uchar> ISO_UDDT;
    ISO_UDDT.resize(field.size());

    for(int i = 0; i < res_y*res_x; i++)
    {
        if( 0 <= uchar_field[i] && uchar_field[i] < 5 )
            ISO_UDDT[i] = 5;
        else if( 5 <= uchar_field[i] && uchar_field[i] < 10 )
            ISO_UDDT[i] = 10;
        else if( 10 <= uchar_field[i] && uchar_field[i] < 15 )
            ISO_UDDT[i] = 15;
        else if( 15 <= uchar_field[i] && uchar_field[i] < 20 )
            ISO_UDDT[i] = 20;
        else if( 20 <= uchar_field[i] && uchar_field[i] < 25 )
            ISO_UDDT[i] = 25;
        else if( 25 <= uchar_field[i] && uchar_field[i] < 30 )
            ISO_UDDT[i] = 30;
        else if( 30 <= uchar_field[i] && uchar_field[i] < 35 )
            ISO_UDDT[i] = 35;
        else if( 35 <= uchar_field[i] && uchar_field[i] < 40 )
            ISO_UDDT[i] = 40;
        else if( 40 <= uchar_field[i] && uchar_field[i] < 45 )
            ISO_UDDT[i] = 45;
        else if( 45 <= uchar_field[i] && uchar_field[i] < 50 )
            ISO_UDDT[i] = 50;
        else if( 50 <= uchar_field[i] && uchar_field[i] < 55 )
            ISO_UDDT[i] = 55;
        else if( 55 <= uchar_field[i] && uchar_field[i] < 60 )
            ISO_UDDT[i] = 60;
        else if( 60 <= uchar_field[i] && uchar_field[i] < 65 )
            ISO_UDDT[i] = 65;
        else if( 65 <= uchar_field[i] && uchar_field[i] < 70 )
            ISO_UDDT[i] = 70;
        else if( 70 <= uchar_field[i] && uchar_field[i] < 75)
            ISO_UDDT[i] = 75;
        else if( 75 <= uchar_field[i] && uchar_field[i] < 80 )
            ISO_UDDT[i] = 80;
        else if( 80 <= uchar_field[i] && uchar_field[i] < 85 )
            ISO_UDDT[i] = 85;
        else if( 85 <= uchar_field[i] && uchar_field[i] < 90)
            ISO_UDDT[i] = 90;
        else if( 90 <= uchar_field[i] && uchar_field[i] < 95 )
            ISO_UDDT[i] = 95;
        else if( 95 <= uchar_field[i] && uchar_field[i] < 100 )
            ISO_UDDT[i] = 100;
        else if( 100 <= uchar_field[i] && uchar_field[i] < 105 )
            ISO_UDDT[i] = 105;
        else if( 105 <= uchar_field[i] && uchar_field[i] < 110 )
            ISO_UDDT[i] = 110;
        else if( 110 <= uchar_field[i] && uchar_field[i] < 115)
            ISO_UDDT[i] = 115;
        else if( 115 <= uchar_field[i] && uchar_field[i] < 120 )
            ISO_UDDT[i] = 120;
        else if( 120 <= uchar_field[i] && uchar_field[i] < 125)
            ISO_UDDT[i] = 125;
        else if( 125 <= uchar_field[i] && uchar_field[i] < 130)
            ISO_UDDT[i] = 130;
        else if( 130 <= uchar_field[i] && uchar_field[i] < 135)
            ISO_UDDT[i] = 135;
        else if( 135 <= uchar_field[i] && uchar_field[i] < 140)
            ISO_UDDT[i] = 140;
        else if( 140 <= uchar_field[i] && uchar_field[i] < 145)
            ISO_UDDT[i] = 145;
        else if( 145 <= uchar_field[i] && uchar_field[i] < 150)
            ISO_UDDT[i] = 150;
        else if( 150 <= uchar_field[i] && uchar_field[i] < 155)
            ISO_UDDT[i] = 155;
        else if( 155 <= uchar_field[i] && uchar_field[i] < 160)
            ISO_UDDT[i] = 160;
        else if( 160 <= uchar_field[i] && uchar_field[i] < 165)
            ISO_UDDT[i] = 165;
        else if( 165 <= uchar_field[i] && uchar_field[i] < 170)
            ISO_UDDT[i] = 170;
        else if( 170 <= uchar_field[i] && uchar_field[i] < 175)
            ISO_UDDT[i] = 175;
        else if( 175 <= uchar_field[i] && uchar_field[i] < 180)
            ISO_UDDT[i] = 180;
        else if( 180 <= uchar_field[i] && uchar_field[i] < 185)
            ISO_UDDT[i] = 185;
        else if( 185 <= uchar_field[i] && uchar_field[i] < 190)
            ISO_UDDT[i] = 190;
        else if( 190 <= uchar_field[i] && uchar_field[i] < 195)
            ISO_UDDT[i] = 195;
        else if( 195 <= uchar_field[i] && uchar_field[i] < 200)
            ISO_UDDT[i] = 200;
        else if( 200 <= uchar_field[i] && uchar_field[i] < 205)
            ISO_UDDT[i] = 205;
        else if( 205 <= uchar_field[i] && uchar_field[i] < 210)
            ISO_UDDT[i] = 210;
        else if( 210 <= uchar_field[i] && uchar_field[i] < 215)
            ISO_UDDT[i] = 215;
        else if( 215 <= uchar_field[i] && uchar_field[i] < 220)
            ISO_UDDT[i] = 220;
        else if( 220 <= uchar_field[i] && uchar_field[i] < 225 )
            ISO_UDDT[i] = 225;
        else if( 225 <= uchar_field[i] && uchar_field[i] < 230)
            ISO_UDDT[i] = 230;
        else if( 230 <= uchar_field[i] && uchar_field[i] < 235)
            ISO_UDDT[i] = 235;
        else if( 235 <= uchar_field[i] && uchar_field[i] < 240 )
            ISO_UDDT[i] = 240;
        else if( 240 <= uchar_field[i] && uchar_field[i] < 245)
            ISO_UDDT[i] = 245;
        else if( 245 <= uchar_field[i] && uchar_field[i] < 250)
            ISO_UDDT[i] = 250;
        else if( 250 <= uchar_field[i] && uchar_field[i] <= 255 )
            ISO_UDDT[i] = 255;

    }
    cv::Mat dst(res_x, res_y, CV_8UC1);
    memcpy(dst.data, ISO_UDDT.data(), ISO_UDDT.size()*sizeof (uchar));

    std::string ddt_file_name = "isoline_field_grey_" + file_name + ".png";
    cv::imwrite(ddt_file_name, dst);
}

void DrawField::draw_rgb_isolines(std::vector<uchar> uchar_field, std::vector<double> field, int res_x, int res_y, std::string file_name)
{
    convert_field_to_image(&uchar_field, field, res_x, res_y);
    cv::Mat dst(res_x, res_y, CV_8UC3);

    for(int y = 0; y < res_y; y++)
    {
        for(int x = 0; x < res_x; x++)
        {
            if( 0 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 5 )
            {

            }
            else if( 5 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 10 )
            {

            }
            else if( 10 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 15 )
            {

            }
            else if( 15 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 20 )
            {

            }
            else if( 20 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 25 )
            {

            }
            else if( 25 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 30 )
            {

            }
            else if( 30 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 35 )
            {

            }
            else if( 35 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 40 )
            {

            }
            else if( 40 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 45 )
            {

            }
            else if( 45 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 50 )
            {

            }
            else if( 50 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 55 )
            {

            }
            else if( 55 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 60 )
            {

            }
            else if( 60 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 65 )
            {

            }
            else if( 65 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 70 )
            {

            }
            else if( 70 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 75)
            {

            }
            else if( 75 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 80 )
            {

            }
            else if( 80 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 85 )
            {

            }
            else if( 85 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 90)
            {

            }
            else if( 90 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 95 )
            {

            }
            else if( 95 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 100 )
            {

            }
            else if( 100 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 105 )
            {

            }
            else if( 105 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 110 )
            {

            }
            else if( 110 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 115)
            {

            }
            else if( 115 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 120 )
            {

            }
            else if( 120 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 125)
            {

            }
            else if( 125 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 130)
            {

            }
            else if( 130 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 135)
            {

            }
            else if( 135 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 140)
            {

            }
            else if( 140 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 145)
            {

            }
            else if( 145 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 150)
            {

            }
            else if( 150 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 155)
            {

            }
            else if( 155 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 160)
            {

            }
            else if( 160 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 165)
            {

            }
            else if( 165 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 170)
            {

            }
            else if( 170 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 175)
            {

            }
            else if( 175 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 180)
            {

            }
            else if( 180 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 185)
            {

            }
            else if( 185 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 190)
            {

            }
            else if( 190 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 195)
            {

            }
            else if( 195 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 200)
            {

            }
            else if( 200 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 205)
            {

            }
            else if( 205 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 210)
            {

            }
            else if( 210 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 215)
            {

            }
            else if( 215 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 220)
            {

            }
            else if( 220 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 225 )
            {

            }
            else if( 225 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] <= 230)
            {

            }
            else if( 230 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 235)
            {

            }
            else if( 235 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 240 )
            {

            }
            else if( 240 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 245)
            {

            }
            else if( 245 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] < 250)
            {

            }
            else if( 250 <= uchar_field[x+y*res_x] && uchar_field[x+y*res_x] <= 255 )
            {

            }
        }
    }
}

} // namespace draw
