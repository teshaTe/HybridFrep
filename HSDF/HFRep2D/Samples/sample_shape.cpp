#include "frep2D.h"
#include "hfrep2D.h"
#include "render2D.h"
#include "proceduralNoise.h"

#include <vector>
#include <random>
#include <functional>
#include <iostream>
#include <cmath>

#include <glm/glm.hpp>

float lerp(const float &lo, const float &hi, const float &t)
{
    return lo * (1.0f - t) + hi * t;
}

float smoothstep(const float &t)
{
    return t * t * ( 3.0f - 2.0f* t);
}

class ValueNoise
{
public:
    ValueNoise(unsigned seed = 2016)
    {
        std::mt19937 gen(seed);
        std::uniform_real_distribution<float> distrFloat;
        auto randFloat = std::bind(distrFloat, gen);

        // create an array of random values and initialize permutation table
        for (unsigned k = 0; k < kMaxTableSize; ++k) {
            r[k] = randFloat();
            permutationTable[k] = k;
        }

        // shuffle values of the permutation table
        std::uniform_int_distribution<unsigned> distrUInt;
        auto randUInt = std::bind(distrUInt, gen);
        for (unsigned k = 0; k < kMaxTableSize; ++k) {
            unsigned i = randUInt() & kMaxTableSizeMask;
            std::swap(permutationTable[k], permutationTable[i]);
            permutationTable[k + kMaxTableSize] = permutationTable[k];
        }
    }

    float eval(glm::vec2 &p)
    {
        int xi = std::floor(p.x);
        int yi = std::floor(p.y);

        float tx = p.x - xi;
        float ty = p.y - yi;

        int rx0 = xi & kMaxTableSizeMask;
        int rx1 = (rx0 + 1) & kMaxTableSizeMask;
        int ry0 = yi & kMaxTableSizeMask;
        int ry1 = (ry0 + 1) & kMaxTableSizeMask;

        // random values at the corners of the cell using permutation table
        const float & c00 = r[permutationTable[permutationTable[rx0] + ry0]];
        const float & c10 = r[permutationTable[permutationTable[rx1] + ry0]];
        const float & c01 = r[permutationTable[permutationTable[rx0] + ry1]];
        const float & c11 = r[permutationTable[permutationTable[rx1] + ry1]];

        // remapping of tx and ty using the Smoothstep function
        float sx = smoothstep(tx);
        float sy = smoothstep(ty);

        // linearly interpolate values along the x axis
        float nx0 = lerp(c00, c10, sx);
        float nx1 = lerp(c01, c11, sx);

        // linearly interpolate the nx0/nx1 along they y axis
        return lerp(nx0, nx1, sy);
    }

    static const unsigned kMaxTableSize = 256;
    static const unsigned kMaxTableSizeMask = kMaxTableSize - 1;
    float r[kMaxTableSize];
    unsigned permutationTable[kMaxTableSize * 2];
};

int main(int argc, char *argv[])
{
    int resX = 512, resY = 512;

    hfrep2D::FRepObj2D frep(resX, resY, 1.0f);
    auto box = std::bind(&hfrep2D::FRepObj2D::rectangle, frep, std::placeholders::_1, std::placeholders::_2,
                                                               std::placeholders::_3, std::placeholders::_4);
    auto circle = std::bind(&hfrep2D::FRepObj2D::circle, frep, std::placeholders::_1, std::placeholders::_2,
                                                                                      std::placeholders::_3);
    auto unionOp = std::bind(&hfrep2D::FRepObj2D::union_function, frep, std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3, std::placeholders::_4 );
    auto subtrOp = std::bind(&hfrep2D::FRepObj2D::subtract_function, frep, std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3, std::placeholders::_4 );
    auto clef = std::bind(&hfrep2D::FRepObj2D::trebleClef, frep, std::placeholders::_1, std::placeholders::_2);

    glm::vec2 offset(20, 0);

    std::vector<float> box1 = frep.getFRep2D(glm::vec2(250, 280), 50, 90, box);
    std::vector<float> box2 = frep.getFRep2D(glm::vec2(250, 280), 20, 60, box);
    std::vector<float> i_base = frep.getFRep2D(box1, box2, 0.0, 0.0, subtrOp);

    std::vector<float> circle1 = frep.getFRep2D(glm::vec2(250, 130), 50, circle);
    std::vector<float> circle2 = frep.getFRep2D(glm::vec2(240, 100), 20, circle);
    std::vector<float> circle3 = frep.getFRep2D(glm::vec2(260, 140), 10, circle);

    std::vector<float> i_top0 = frep.getFRep2D(circle1, circle2, 0.0, 0.0, subtrOp);
    std::vector<float> i_top = frep.getFRep2D(i_top0, circle3, 0.0, 0.0, subtrOp);
    std::vector<float> i = frep.getFRep2D(i_base, i_top, 0.0, 0.0, unionOp);

    std::vector<float> in_base = frep.getFRep2D(glm::vec2(280, 220)-offset, 150-30, circle);
    std::vector<float> circle4 = frep.getFRep2D(glm::vec2(280, 120)-offset, 80, circle);
    std::vector<float> circle5 = frep.getFRep2D(glm::vec2(230, 260)-offset, 30, circle);
    std::vector<float> circle6 = frep.getFRep2D(glm::vec2(330, 260)-offset, 30, circle);

    std::vector<float> hals = frep.getFRep2D(circle5, circle6, 0.0, 0.0, unionOp);
    std::vector<float> inShape0 = frep.getFRep2D(in_base, circle4, 0.0, 0.0, subtrOp);
    std::vector<float> inShape = frep.getFRep2D(inShape0, hals, 0.0, 0.0, subtrOp);

    hfrep2D::HFRepObj2D hfrep(resX, resY);
    std::vector<float> hfrep0 = hfrep.calculateHFRep2D(&i, nullptr, HYPERBOLIC_SIGMOID, 0.00001f);
    std::vector<float> hfrep1 = hfrep.calculateHFRep2D(&inShape, nullptr, HYPERBOLIC_SIGMOID, 0.00001f);

    std::vector<float> _i = frep.scaleFunction(i, 100);
    std::vector<float> _inShape = frep.scaleFunction(inShape, 100);

    std::vector<float> frep_clef = frep.getFRep2D(glm::vec2(250, 250), clef);
    std::vector<float> hfrep_clef = hfrep.calculateHFRep2D(&frep_clef, nullptr, HYPERBOLIC_SIGMOID, 0.00001f);

    frep.setScalingFactor(4);
    auto heart = std::bind(&hfrep2D::FRepObj2D::heart2D, frep, std::placeholders::_1, std::placeholders::_2);
    std::vector<float> frep_heart = frep.getFRep2D(glm::vec2(250,250), heart);
    std::vector<float> hfrep_heart = hfrep.calculateHFRep2D(&frep_heart, nullptr, HYPERBOLIC_SIGMOID, 0.00001f);
    std::vector<float> frep_heart_v = frep.scaleFunction(frep_heart, 2);

    hfrep2D::render2D render;
    sf::Image i_baseIM = render.drawIsolines(&_i, resX, resY, 0.005, "i_base", 0.04);
    sf::Image inIM   = render.drawIsolines(&_inShape, resX, resY, 0.005, "in'", 0.04);
    sf::Image hfrIm1 = render.drawIsolines(&hfrep0, resX, resY, 0.005, "hfrep0");
    sf::Image hfrIm2 = render.drawIsolines(&hfrep1, resX, resY, 0.005, "hfrep1");
    render.drawIsolines(&hfrep_heart, resX, resY, 0.05, "heart");

    //render.displayImage(inIM);
    render.displayImage(hfrIm1);
    render.displayImage(hfrIm2);

    material::procNoise noise(resX, resY, 2016, 256);

    float frequency     = 0.02f;
    float frequencyMult = 1.8;
    float amplitudeMult = 0.35;
    unsigned numLayers  = 5;
    float maxNoiseVal   = 0;

    material::HObj_single_mat obj1, obj2;
    noise.procWood(hfrep_clef, frequency, obj1);

    material::colRegion cr1[4], cr2[4];
    std::vector<material::colRegion> regions1, regions2;
    regions1.reserve(4);

    cr1[0].activeNoise = glm::bvec3(false, true, true);
    cr1[0].activeDistFunc = glm::bvec3(true, false, false);
    cr1[0].region = glm::vec4(0, resX/2, 0, resY/2);

    cr1[1].activeNoise = glm::bvec3(true, true, false);
    cr1[1].activeDistFunc = glm::bvec3(false, false, true);
    cr1[1].region = glm::vec4(resX/2, resX, 0, resY/2);

    cr1[2].activeNoise = glm::bvec3(true, true, false);
    cr1[2].activeDistFunc = glm::bvec3(false, false, true);
    cr1[2].region = glm::vec4(0, resX/2, resY/2, resY);

    cr1[3].activeNoise = glm::bvec3(false, true, true);
    cr1[3].activeDistFunc = glm::bvec3(true, false, false);
    cr1[3].region = glm::vec4(resX/2, resX, resY/2, resY);

    regions1.insert(regions1.end(), cr1, cr1+4);

    sf::Image im1 = noise.getObjectImage(obj1, regions1);
    render.displayImage(im1);
    im1.saveToFile("test1.png");

    regions2.reserve(4);

    cr2[0].activeNoise = glm::bvec3(true, false, false);
    cr2[0].activeDistFunc = glm::bvec3(false, false, false);
    cr2[0].region = glm::vec4(0, resX/2, 0, resY/2);
    cr2[0].manualCol = glm::vec3(0, 150, 200);

    cr2[1].activeNoise = glm::bvec3(true, false, false);
    cr2[1].activeDistFunc = glm::bvec3(false, false, false);
    cr2[1].region = glm::vec4(resX/2, resX, 0, resY/2);
    cr2[1].manualCol = glm::vec3(0, 150, 200);

    cr2[2].activeNoise = glm::bvec3(false, true, false);
    cr2[2].activeDistFunc = glm::bvec3(false, false, true);
    cr2[2].region = glm::vec4(0, resX/2, resY/2, resY);

    cr2[3].activeNoise = glm::bvec3(false, true, false);
    cr2[3].activeDistFunc = glm::bvec3(false, false, true);
    cr2[3].region = glm::vec4(resX/2, resX, resY/2, resY);

    regions2.insert(regions2.end(), cr2, cr2+4);

    noise.procWood(hfrep1, 0.05, obj2);
    sf::Image im2 = noise.getObjectImage(obj2, regions2);
    render.displayImage(im2);
    im2.saveToFile("test2.png");

    return 0;
}
