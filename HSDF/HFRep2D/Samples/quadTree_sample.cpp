#include "quadTree.h"
#include "quadTreeRender.h"
#include "frep2D.h"
#include "hfrep2D.h"
#include "seddt2D.h"
#include "hierarchicalFIM2D.h"
#include "FIM2D.h"
#include "timer.hpp"
#include "render2D.h"

#include <SFML/Graphics/Image.hpp>

#include <iostream>

using namespace std::placeholders;

std::vector<float> sdfSphere(glm::vec2 cent, float r, glm::vec2 res)
{
    std::vector<float> sdf;
    for(int y = 0; y < res.y; y++)
        for(int x = 0; x <res.x; x++)
        {
            float shX = (static_cast<float>(x)/res.x - cent.x/res.x);
            float shY = (static_cast<float>(y)/res.y - cent.y/res.y);
            sdf.push_back( r/res.x - std::sqrt(shX*shX + shY*shY) );
        }
    return sdf;
}

int main()
{
    int resX = 512, resY = 512;
    float rad = 100.0f, a = 6.0f;
    float c_x = resX/2.0f, c_y = resY/2.0f;
    size_t max_depth = 10, max_ext = 3, max_int =4, thres = 1;

    hfrep2D::FRepObj2D frep( resX, resY, 3.0 );
    auto key = std::bind(&hfrep2D::FRepObj2D::trebleClef, frep, _1 );
    auto circle = std::bind(&hfrep2D::FRepObj2D::circle,  frep, _1, _2, _3 );
    auto quad = std::bind(&hfrep2D::FRepObj2D::rectangle, frep, _1, _2, _3, _4 );
    auto heart = std::bind(&hfrep2D::FRepObj2D::heart2D, frep, _1, _2);
    auto bat = std::bind(&hfrep2D::FRepObj2D::bat, frep, _1);

    std::vector<float> func0 = frep.getFRep2D( glm::vec2( c_x, c_y ), key );
    std::vector<float> func1 = frep.getFRep2D( glm::vec2( c_x, c_y ), rad, circle );
    std::vector<float> func2 = frep.getFRep2D( glm::vec2( c_x, c_y ), a, a, quad );
    std::vector<float> func3 = frep.getFRep2D( glm::vec2( c_x, c_y ), heart);
    std::vector<float> func4 = frep.getFRep2D( glm::vec2( c_x, c_y ), bat);

    std::vector<float> f_to_pass = func0;

   /* hfrep2D::FIMSolver fim( resX, resY);
    glm::vec2 surfLim = frep.findZeroLevelSetInterval(f_to_pass, 1500);
    fim.solveEikonalEquationParallel_CPU( &f_to_pass, surfLim.x, surfLim.y );
    std::vector<float> udf = fim.getDistanceField();*/

    hfrep2D::DiscreteDistanceTransform ddt(resX, resY);
    ddt.caclulateDiscreteDistanceTransformVec( &f_to_pass );
    std::vector<float> uddt = ddt.get_DDT();

    hfrep2D::HFRepObj2D hfrep(resX, resY);
    std::vector<float> hfrep1 = hfrep.calculateHFRep2D(&f_to_pass, nullptr, HYPERBOLIC_SIGMOID, 0.0000001f, true);

    /*std::vector<float> sdf = sdfSphere(glm::vec2(c_x,c_y), rad, glm::vec2(resX,resY));

    float averErrFIM = 0, averErrDDT = 0, averErrHFRep = 0;
    for(size_t i = 0; i < udf.size(); i++)
    {
        averErrHFRep += std::abs(std::abs(hfrep1[i]) - std::abs(sdf[i]))/ static_cast<float>(uddt.size());
        averErrDDT += std::abs(uddt[i] - std::abs(sdf[i]))/ static_cast<float>(uddt.size());
        averErrFIM += std::abs(udf[i] - std::abs(sdf[i])) / static_cast<float>(udf.size());
    }
    std::cout << "averErrHFRep = " << averErrHFRep << std::endl;
    std::cout << "averErrDDT = " << averErrDDT << std::endl;
    std::cout << "averErrFIM = " << averErrFIM << std::endl;*/

    glm::vec2 surfLimQ = frep.findZeroLevelSetInterval(f_to_pass, 3000);

    hfrep2D::boundingBox box( glm::f32vec2(0,0), glm::f32vec2(resX, resY) );
    hfrep2D::quadTree tree( box, resX, resY, thres, max_depth, max_ext, max_int );
    tree.generateQuadTree( &f_to_pass, glm::vec2( surfLimQ.x, surfLimQ.y ));
    tree.treeCheck();

    prof::timer time;
    time.Start();
    std::vector<hfrep2D::quadTree::qNode*> leafs = tree.getLeafsWithFunVals( &f_to_pass );
    time.End( "Searching for leaves" );

    hfrep2D::quadTreeRender render;
    render.createWindow(resX, resY, "Test");
    render.displayTree( leafs, "qtree.jpg" );

    hfrep2D::FIMSparseSolver fim_sp( resX, resY );
    fim_sp.solveEikonalEquationParallel_CPU(tree, surfLimQ.x, surfLimQ.y, hfrep2D::InterType::GEO_BILINEAR);
    std::vector<float> adf_cubic = fim_sp.getDistanceField();

    //fim_sp.solveEikonalEquationParallel_CPU(tree, surfLimQ.x, surfLimQ.y, hfrep2D::InterType::GEO_AKIMA);
    //std::vector<float> adf_akima = fim_sp.getDistanceField();

    //fim_sp.solveEikonalEquationParallel_CPU(tree, surfLimQ.x, surfLimQ.y, hfrep2D::InterType::GEO_BILINEAR);
    //std::vector<float> adf_linear = fim_sp.getDistanceField();

    //std::vector<float> hfrep1 = hfrep.calculateHFRep2D(&func, &adf, HYPERBOLIC_SIGMOID, 0.0000001f, true);

    hfrep2D::render2D render_field;
    std::transform(f_to_pass.begin(), f_to_pass.end(), f_to_pass.begin(), std::bind2nd(std::multiplies<float>(), 50.0f));

    sf::Image imf0 = render_field.drawIsolines( &uddt, resX, resY, 0.05f, "uddt", 0.04 );
    //sf::Image imf1 = render_field.drawIsolines( &adf_linear, resX, resY, 0.05f, "adf_test_bilinear", 0.04 );
    //sf::Image imf2 = render_field.drawIsolines( &adf_cubic, resX, resY, 0.05f, "adf_test_bilinear", 0.04 );
    //sf::Image imf3 = render_field.drawIsolines( &adf_akima, resX, resY, 0.05f, "adf_test_akima", 0.04 );
    sf::Image imf2 = render_field.drawIsolines( &hfrep1, resX, resY, 0.05f, "Hfrep", 0.04 );
    sf::Image imf4 = render_field.drawIsolines( &f_to_pass, resX, resY, 0.0005f, "frep", 0.03 );

    render_field.displayImage(imf0);
    render_field.displayImage(imf2);
    render_field.displayImage(imf4);

    return 0;
}
