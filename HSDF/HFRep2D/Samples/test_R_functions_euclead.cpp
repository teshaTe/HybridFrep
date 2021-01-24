#include "frep2D.h"
#include "seddt2D.h"
#include "hfrep2D.h"
#include "render2D.h"
#include "Mathematics/IntpBicubic2.h"

#include <functional>
#include <iostream>

float computeAlpha(float f1, float f2)
{
    float v0 = f1 * f2;
    float v1 = v0*f1*f1 + 2.0f*v0*v0 + v0*f2*f2;
    if(v1 < 0)
        std::cerr << "Error: v1 < 0; v1 = " << v1 << std::endl;

    return (-2.0f*v0 + std::sqrt(2.0f)*std::sqrt(v1))/v0;
}

float R_union_test3(float f1, float f2, float &alpha)
{
    alpha = computeAlpha(f1, f2);
    if(std::isnan(alpha) || alpha > 1.0f)
        alpha = 1.0f;

    if(alpha < -1.0f)
        alpha = -0.99f;

    std::cout << alpha << std::endl;

    float root = f1*f1 + f2*f2 - 2.0f*alpha*f1*f2;
    assert(root >= 0.0f);
    float result = (f1 + f2 + std::sqrt(root)) / (1.0f+alpha);
    return result;
}

sf::Image drawAlpha(std::vector<float> alphaV, int resX, int resY, std::string file)
{
    sf::Image alphaImg; alphaImg.create(resX, resY);
    for(int y = 0; y < resY; y++)
        for(int x = 0; x < resX; x++)
        {
            if(alphaV[x+y*resX] < 0.0f)
                alphaImg.setPixel(x, y, sf::Color(255, 255, 0));
            else if(alphaV[x+y*resX] >= 0.7f && alphaV[x+y*resX] <= 0.75f)
                alphaImg.setPixel(x, y, sf::Color(120, 0, 0));
            else if(alphaV[x+y*resX] > 0.75f && alphaV[x+y*resX] <= 0.8f)
                alphaImg.setPixel(x, y, sf::Color(200, 0, 0));
            else if(alphaV[x+y*resX] > 0.8f && alphaV[x+y*resX] <= 0.85f)
                alphaImg.setPixel(x, y, sf::Color(0, 120, 0));
            else if(alphaV[x+y*resX] > 0.85f && alphaV[x+y*resX] <= 0.9f)
                alphaImg.setPixel(x, y, sf::Color(0, 200, 0));
            else if(alphaV[x+y*resX] > 0.9f && alphaV[x+y*resX] <= 0.95f)
                alphaImg.setPixel(x, y, sf::Color(0, 0, 120));
            else
                alphaImg.setPixel(x, y, sf::Color(0, 0, 200));
        }
    alphaImg.saveToFile(file+".jpg");
    return alphaImg;
}

int main(int argc, char *argv[])
{
    int resX = 512, resY = 512;
    float r1 = 80, r2 = 50;
    int step = 0;
    glm::vec2 c1(200, 250), c2(270, 250);

    hfrep2D::FRepObj2D frep(resX+step, resY+step, 1.0f);
    auto circle = std::bind(&hfrep2D::FRepObj2D::circle, frep, std::placeholders::_1, std::placeholders::_2,
                                                                                      std::placeholders::_3);
    auto rectangle = std::bind(&hfrep2D::FRepObj2D::rectangle, frep, std::placeholders::_1, std::placeholders::_2,
                                                                     std::placeholders::_3, std::placeholders::_4);

    std::vector<float> frep1 = frep.getFRep2D(c1, r1, circle);
    std::vector<float> frep2 = frep.getFRep2D(c2, r2, circle);
    std::vector<float> frep3 = frep.getFRep2D(c1, r1, r1, rectangle);
    std::vector<float> frep4 = frep.getFRep2D(c2, r2-10, r2-10, rectangle);

    hfrep2D::DiscreteDistanceTransform dt(resX+step, resY+step);
    dt.caclulateDiscreteDistanceTransformVec(&frep1);
    std::vector<float> dt1 = dt.get_signed_DDT();
    dt.caclulateDiscreteDistanceTransformVec(&frep2);
    std::vector<float> dt2 = dt.get_signed_DDT();
    dt.caclulateDiscreteDistanceTransformVec(&frep3);
    std::vector<float> dt3 = dt.get_signed_DDT();
    dt.caclulateDiscreteDistanceTransformVec(&frep4);
    std::vector<float> dt4 = dt.get_signed_DDT();

    /*hfrep2D::HFRepObj2D hfrep(resX, resY);
    std::vector<float> hfrep1 = hfrep.calculateHFRep2D(&frep1, nullptr, HYPERBOLIC_SIGMOID, 0.00001f);
    std::vector<float> hfrep2 = hfrep.calculateHFRep2D(&frep2, nullptr, HYPERBOLIC_SIGMOID, 0.00001f);
    */
    std::vector<float> union1;
    std::vector<float> union2;
    std::vector<float> union3;
    std::vector<float> union4;

    std::vector<float> alphaV1;
    std::vector<float> alphaV2;
    std::vector<float> alpha_new1, alpha_old1, alpha_new2, alpha_old2;

    /*for(int i = 0; i < resX*resY; i++)
    {
        alpha_old1.push_back(computeAlpha(dt1[i], dt2[i]));
        alpha_old2.push_back(computeAlpha(dt3[i], dt4[i]));
    }

    float min_a1 = *std::min_element(alpha_old1.begin(), alpha_old1.end());
    float max_a1 = *std::max_element(alpha_old1.begin(), alpha_old1.end());

    float min_a2 = *std::min_element(alpha_old2.begin(), alpha_old2.end());
    float max_a2 = *std::max_element(alpha_old2.begin(), alpha_old2.end());

    for(int i = 0; i < resX*resY;i++)
    {
        float norm_a1 = 2.0f*(alpha_old1[i] - min_a1) /(max_a1-min_a1);
        float norm_a2 = 2.0f*(alpha_old2[i] - min_a2) /(max_a2-min_a2);

        alpha_new1.push_back(norm_a1);
        alpha_new2.push_back(norm_a2);
        //std::cout << norm_a1 << "; \t " << norm_a2 << std::endl;
    }*/

    for(int i = 0; i < resX*resY; i++)
    {
        float alpha1, alpha2;
        union1.push_back(R_union_test3(dt1[i], dt2[i], alpha1));
        union2.push_back(R_union_test3(dt3[i], dt4[i], alpha2));
        union3.push_back(std::max(dt1[i], dt2[i]));
        union4.push_back(std::max(dt3[i], dt4[i]));
        alpha_new1.push_back(alpha1);
        alpha_new2.push_back(alpha2);
    }

    float errIn0 = 0, errOut0 = 0;
    float errIn1 = 0, errOut1 = 0;
    int in0=0, in1=0, out0=0, out1=0;

    for(int i = 0; i < resX*resY; i++)
    {
        if(union3[i] > 0)
        {
            errIn0 += std::abs(union1[i] - union3[i]);
            in0++;
        }else {
            errOut0 += std::abs(union1[i]) - std::abs(union3[i]);
            out0++;
        }

        if(union4[i] > 0)
        {
            errIn1 += std::abs(union2[i] - union4[i]);
            in1++;
        }else{
            errOut1 += std::abs(union2[i]) - std::abs(union4[i]);
            out1++;
        }
    }

    std::cout <<"Average ERROR, circles <out, in>: " << errOut0/(float)(out0) << " ; " << errIn0/(float)(in0) << std::endl;
    std::cout <<"Average ERROR, rectangles <out, in>: " << errOut1/(float)(out1) << " ; " << errIn1/(float)(in1) << std::endl;

    hfrep2D::render2D render;
    sf::Image img1 = render.drawIsolines(&union1, resX, resY, 0.005, "union1");
    sf::Image img2 = render.drawIsolines(&union2, resX, resY, 0.005, "union2");
    sf::Image img3 = render.drawIsolines(&union3, resX, resY, 0.005, "union3");
    sf::Image img4 = render.drawIsolines(&union4, resX, resY, 0.005, "union4");
    //sf::Image img5 = render.drawIsolines(&subtr1, resX-st, resY-st, 0.005, "subtr2");
    //sf::Image img6 = render.drawIsolines(&subtr2, resX-st, resY-st, 0.005, "subtr2");

    sf::Image alphaImg1 = drawAlpha(alpha_new1, resX, resY, "alpha1");
    sf::Image alphaImg2 = drawAlpha(alpha_new2, resX, resY, "alpha2");
    //sf::Image alphaImg3 = drawAlpha(alphaV3, resX-st, resY-st, "alpha3");

    render.displayImage(img1);
    render.displayImage(img2);
    render.displayImage(img3);
    render.displayImage(img4);
    //render.displayImage(img5);
    //render.displayImage(img6);

    render.displayImage(alphaImg1);
    render.displayImage(alphaImg2);
    //render.displayImage(alphaImg3);

    return 0;
}
