#include "frep2D.h"
#include "hfrep2D.h"
#include "render2D.h"

#include <vector>
#include <glm/glm.hpp>

using namespace std::placeholders;

int main(int argc, char *argv[])
{
    int resX = 512, resY = 512;
    glm::vec2 c(250, 250);

    hfrep2D::FRepObj2D frep(resX, resY, 1.0);
    auto bat  = std::bind(&hfrep2D::FRepObj2D::bat, frep, _1, _2);
    auto clef = std::bind(&hfrep2D::FRepObj2D::trebleClef, frep, _1, _2);

    std::vector<float> bat_frep = frep.getFRep2D(c, bat);
    std::vector<float> clef_frep = frep.getFRep2D(c, clef);

    hfrep2D::HFRepObj2D hfrep(resX, resY);
    std::vector<float> bat_hfrep = hfrep.calculateHFRep2D(&bat_frep, nullptr, HYPERBOLIC_SIGMOID, 0.000001);
    std::vector<float> bat_udf = hfrep.getDDT();
    std::vector<float> bat_sdf = hfrep.getSignedDDT();

    std::vector<float> clef_hfrep = hfrep.calculateHFRep2D(&clef_frep, nullptr, HYPERBOLIC_SIGMOID, 0.000001);
    std::vector<float> clef_udf = hfrep.getDDT();
    std::vector<float> clef_sdf = hfrep.getSignedDDT();


    std::vector<float> scaled_bat = frep.scaleFunction(bat_frep, 10);
    std::vector<float> scaled_clef = frep.scaleFunction(clef_frep, 60);

    hfrep2D::render2D render;
    render.drawIsolines(&scaled_bat, resX, resY, 0.005, "bat_frep", 0.03);
    render.drawIsolines(&bat_hfrep, resX, resY, 0.05, "bat_hfrep");
    render.drawIsolines(&bat_udf, resX, resY, 0.05, "bat_udf");
    render.drawIsolines(&bat_sdf, resX, resY, 0.05, "bat_sdf");

    render.drawIsolines(&scaled_clef, resX, resY, 0.005, "clef_frep", 0.04);
    render.drawIsolines(&clef_hfrep, resX, resY, 0.05, "clef_hfrep");
    render.drawIsolines(&clef_udf, resX, resY, 0.05, "clef_udf");
    render.drawIsolines(&clef_sdf, resX, resY, 0.05, "clef_sdf");

    return 0;
}
