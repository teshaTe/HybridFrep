#include "proceduralNoise.h"

#include <iostream>
#include <random>
#include <functional>

namespace material {

procNoise::procNoise(const int resX, const int resY, const int seed, const unsigned permTableSize): res_x(resX),
                                                                                                    res_y(resY),
                                                                                                    randSeed(seed),
                                                                                                    tableSize(permTableSize)
{
    maxTableSizeMask = tableSize - 1;
    permutationTable.resize(tableSize * 2);
    r.resize(tableSize);
    valueNoiseInit();
}

void procNoise::procWhiteNoise(const std::vector<float> fun, HObj_single_mat &obj)
{
    std::mt19937 gen(randSeed);
    std::uniform_real_distribution<float> distr;
    auto dice = std::bind(distr, gen);
    obj.second.clear();
    obj.second.resize(fun.size());
    obj.first = fun;

    for( unsigned j = 0; j < res_y; j++ )
        for( unsigned i = 0; i< res_x; i++)
        {
            if(fun[i+j*res_x] >= 0)
                obj.second[i+j*res_x] = dice();
            else
                obj.second[i+j*res_x] = 0.0f;
        }
}

void procNoise::procValueNoise(const std::vector<float> fun, float frequency, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    obj.first = fun;

    for( unsigned j = 0; j < res_y; j++ )
        for( unsigned i = 0; i< res_x; i++)
        {
            glm::vec2 p(i, j);
            if(fun[i+j*res_x] >= 0)
            {
                p *= frequency;
                obj.second[i+j*res_x] = eval(p);
            }
            else
                obj.second[i+j*res_x] = 0.0f;
        }
}

void procNoise::procWood(const std::vector<float> fun, float frequency, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    obj.first = fun;

    for( unsigned j = 0; j < res_y; j++ )
        for( unsigned i = 0; i< res_x; i++)
        {
            glm::vec2 p(i, j);
            if(fun[i+j*res_x] >= 0)
            {
                p *= frequency/10;
                float g = eval(p) * 10.0f;
                obj.second[i+j*res_x] = (g - static_cast<int>(g))*fun[i+j*res_x]*20;
            }
            else
            {
                obj.second[i+j*res_x] = 0;
            }
        }
}

void procNoise::procMarble(const std::vector<float> fun, float frequency, float freqMult,
                           float ampMult, float numLayers, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    std::fill(obj.second.begin(), obj.second.end(), 0.0f);
    obj.first = fun;

    for( unsigned j = 0; j < res_y; j++ )
        for( unsigned i = 0; i< res_x; i++)
        {
            glm::vec2 p = glm::vec2(i, j) * frequency;
            float amplitude = 1.0f;
            float noiseV = 0.0f;

            for(unsigned l = 0; l < numLayers; l++)
            {
                 noiseV += eval(p)*amplitude;
                 p *= freqMult;
                 amplitude *= ampMult;
            }

            if(fun[i+j*res_x] > 0.0f)
                obj.second[i+j*res_x] = (std::sin((i + noiseV * 100.0f) * 2.0f * M_PI / 200.0f) + 1.0f) / 2.0f;
        }
}

void procNoise::procFractalNoise(const std::vector<float> fun, float frequency, float freqMult,
                                 float ampMult, float numLayers, float maxNoiseV, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    std::fill(obj.second.begin(), obj.second.end(), 0.0f);
    obj.first = fun;

    for( unsigned j = 0; j < res_y; j++ )
        for( unsigned i = 0; i< res_x; i++)
        {
            glm::vec2 p = glm::vec2(i, j) * frequency;
            float amplitude = 1.0f;

            for(unsigned l = 0; l < numLayers; l++)
            {
                if(fun[i+j*res_x] >= 0)
                {
                    obj.second[i+j*res_x] += eval(p)*amplitude;
                    p *= freqMult;
                    amplitude *= ampMult;
                }
                else
                    obj.second[i+j*res_x] = 0.0f;
            }

            if(obj.second[i+j*res_x] > maxNoiseV)
                maxNoiseV = obj.second[i+j*res_x];
        }

    for(unsigned i = 0; i < res_x * res_y; i++)
        obj.second[i] /= maxNoiseV;
}

void procNoise::procTurbulence(const std::vector<float> fun, float frequency, float freqMult,
                               float ampMult, float numLayers, float maxNoiseV, HObj_single_mat &obj)
{
    obj.first = fun;

    for( unsigned j = 0; j < res_y; j++ )
        for( unsigned i = 0; i < res_x; i++)
        {
            glm::vec2 p = glm::vec2(i, j) * frequency;
            float amplitude = 1.0f;

            for(unsigned l = 0; l < numLayers; l++)
            {
                if(fun[i+j*res_x] >= 0)
                {
                    obj.second[i+j*res_x] += std::abs(2.0f * eval(p) - 1) * amplitude;
                    p *= freqMult;
                    amplitude *= ampMult;
                }
                else
                    obj.second[i+j*res_x] = 0.0f;
            }

            if(obj.second[i+j*res_x] > maxNoiseV)
                maxNoiseV = obj.second[i+j*res_x];
        }

    for(unsigned i = 0; i < res_x * res_y; i++)
        obj.second[i] /= maxNoiseV;
}

sf::Image procNoise::getObjectImage(const HObj_single_mat obj, std::vector<colRegion> regions)
{
    sf::Image dst; dst.create(res_x, res_y);

    auto it = std::minmax_element(obj.first.begin(), obj.first.end());

    for(int y = 0; y < res_y; y++)
        for(int x = 0; x < res_x; x++)
        {
            if(obj.first[x+y*res_x] < 0.0f)
                dst.setPixel(x, y, sf::Color(0, 0, 0));
            else
            {
                for(size_t i = 0; i < regions.size(); i++)
                {

                    if((x>= regions[i].region.x && x < regions[i].region.y) &&
                        y >= regions[i].region.z && y < regions[i].region.w)
                    {
                        glm::vec3 col;
                        if(regions[i].activeNoise.r)
                            col.r = obj.second[x+y*res_x]*255;
                        else if(regions[i].activeDistFunc.r)
                            col.r = scaleToNewRange(obj.first[x+y*res_x], *it.first, *it.second)*255;
                        else
                            col.r = regions[i].manualCol.r;

                        if(regions[i].activeNoise.g)
                            col.g = obj.second[x+y*res_x]*255;
                        else if(regions[i].activeDistFunc.g)
                            col.g = scaleToNewRange(obj.first[x+y*res_x], *it.first, *it.second)*255;
                        else
                            col.g = regions[i].manualCol.g;

                        if(regions[i].activeNoise.b)
                            col.b = obj.second[x+y*res_x]*255;
                        else if(regions[i].activeDistFunc.b)
                            col.b = scaleToNewRange(obj.first[x+y*res_x], *it.first, *it.second)*255;
                        else
                            col.b = regions[i].manualCol.b;

                        dst.setPixel(x, y, sf::Color((sf::Uint8)(col.r), (sf::Uint8)(col.g), (sf::Uint8)(col.b)));
                    }
                }
            }
        }

    return dst;
}

float procNoise::eval(glm::vec2 &p)
{
    int xInt = std::floor(p.x);
    int yInt = std::floor(p.y);

    // tx - x coord of the point in which we computing a noise value
    // ty - y coord of the point in which we computing a noise value
    float tX = p.x - xInt;
    float tY = p.y - yInt;

    int rX0 = xInt & maxTableSizeMask;
    int rY0 = yInt & maxTableSizeMask;
    int rX1 = (rX0 + 1) & maxTableSizeMask;
    int rY1 = (rY0 + 1) & maxTableSizeMask;

    //computing random values at the corners of the cell using permutation table
    const float &c00 = r[permutationTable[permutationTable[rX0] + rY0]];
    const float &c10 = r[permutationTable[permutationTable[rX1] + rY0]];
    const float &c01 = r[permutationTable[permutationTable[rX0] + rY1]];
    const float &c11 = r[permutationTable[permutationTable[rX1] + rY1]];

    //remapping of tX and tY to the interval [0, 1]
    float sX = smothstep(tX);
    float sY = smothstep(tY);

    //linearly interpolate values along the X axis, points belong to the cell border top and bottom borders
    float nX0 = lerp(c00, c10, sX);
    float nX1 = lerp(c01, c11, sX);

    //linearly interpolate the nX0/nX1 along the Y axis
    return lerp(nX0, nX1, sY);
}

void procNoise::valueNoiseInit()
{
    std::mt19937 gen(randSeed);
    std::uniform_real_distribution<float> distrF;
    auto randDistrF = std::bind(distrF, gen);

    //initialize permutaiton table
    for(unsigned i = 0; i < tableSize; i++)
    {
        r[i] = randDistrF();
        permutationTable[i] = i;
    }

    std::uniform_int_distribution<unsigned> distrU;
    auto randDistrU = std::bind(distrU, gen);
    for(int i = 0; i < tableSize; i++)
    {
        unsigned j = randDistrU() & maxTableSizeMask;
        std::swap(permutationTable[i], permutationTable[j]);
        permutationTable[i+tableSize] = permutationTable[i];
    }
}

} //namespace material
