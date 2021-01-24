#include "frep2D.h"
#include "hfrep2D.h"
#include "render2D.h"
#include <vector>
#include <algorithm>

using namespace std::placeholders;

int main(int argc, char *argv[])
{
    int resX = 512, resY = 512;
    glm::vec2 center(resX/2.0f, resY/2.0f);

    hfrep2D::FRepObj2D frep(resX, resY, 1.0);
    std::vector<float> frep_star = frep.getFRep2D(center, std::bind(&hfrep2D::FRepObj2D::suriken, frep, _1, _2));

    hfrep2D::HFRepObj2D hfrep(resX, resY);
    std::vector<float> hfrep0 = hfrep.calculateHFRep2D(&frep_star, nullptr, HYPERBOLIC_SIGMOID, 0.00001, true);
    std::vector<float> hfrep1 = hfrep.calculateHFRep2D(&frep_star, nullptr, HYPERBOLIC_SIGMOID, 0.1, true);

    std::vector<float> scaled_star = frep.scaleFunction(frep_star, 5);

    hfrep2D::render2D render;
    render.drawIsolines(&scaled_star, resX, resY, 0.005, "frep_star", 0.03);
    render.drawIsolines(&hfrep0, resX, resY, 0.05, "hfrep_0.00001");
    render.drawIsolines(&hfrep1, resX, resY, 0.005, "hfrep_0.1");

    return 0;
}
