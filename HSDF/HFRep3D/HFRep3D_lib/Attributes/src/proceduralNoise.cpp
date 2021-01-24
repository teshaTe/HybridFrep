#include "Attributes/include/proceduralNoise.h"

#include <iostream>
#include <random>
#include <functional>

namespace material {

procNoise::procNoise(const int resX, const int resY, const int resZ, const int seed, const unsigned permTableSize):
                     res_x(resX), res_y(resY), res_z(resZ), randSeed(seed), tableSize(permTableSize)
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

    for(unsigned z = 0; z < res_z; z++ )
        for(unsigned y = 0; y< res_y; y++)
            for(unsigned x = 0; x< res_x; x++)
            {
                if(fun[index3d(x, y, z)] >= 0)
                    obj.second[index3d(x, y, z)] = dice();
                else
                    obj.second[index3d(x, y, z)] = 0.0f;
            }
}

void procNoise::procValueNoise(const std::vector<float> fun, float frequency, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    obj.first = fun;

    for( unsigned z = 0; z < res_z; z++ )
        for( unsigned y = 0; y < res_y; y++)
            for( unsigned x = 0; x < res_x; x++)
            {
                glm::vec3 p(x, y, z);
                if(fun[index3d(x, y, z)] >= 0)
                {
                    p *= frequency;
                    obj.second[index3d(x, y, z)] = eval(p);
                }
                else
                    obj.second[index3d(x, y, z)] = 0.0f;
            }
}

void procNoise::procWood(const std::vector<float> fun, float frequency, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    obj.first = fun;

    for( unsigned z = 0; z < res_z; z++ )
        for( unsigned y = 0; y < res_y; y++)
            for( unsigned x = 0; x < res_x; x++)
            {
                glm::vec3 p(x, y, z);
                if(fun[index3d(x, y, z)] >= 0)
                {
                    p *= frequency;
                    float g = eval(p) * 10.0f;
                    obj.second[index3d(x, y ,z)] = g - static_cast<int>(g);
                }
                else
                    obj.second[index3d(x, y, z)] = 0;
        }
}

void procNoise::procMarble(const std::vector<float> fun, float frequency, float freqMult,
                           float ampMult, float numLayers, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    std::fill(obj.second.begin(), obj.second.end(), 0.0f);
    obj.first = fun;

    for(unsigned z = 0; z < res_z; z++ )
        for(unsigned y = 0; y < res_y; y++)
            for(unsigned x = 0; x < res_x; x++)
            {
                glm::vec3 p = glm::vec3(x, y, z) * frequency;
                float amplitude = 1.0f;
                float noiseV = 0.0f;

                for(unsigned l = 0; l < numLayers; l++)
                {
                     noiseV += eval(p)*amplitude;
                     p *= freqMult;
                     amplitude *= ampMult;
                }

                if(fun[index3d(x, y, z)] > 0.0f)
                    obj.second[index3d(x, y, z)] = (std::sin((x + noiseV * 100.0f) * 2.0f * M_PI / 200.0f) + 1.0f) / 2.0f;
            }
}

void procNoise::procFractalNoise(const std::vector<float> fun, float frequency, float freqMult,
                                 float ampMult, float numLayers, float maxNoiseV, HObj_single_mat &obj)
{
    obj.second.clear();
    obj.second.resize(fun.size());
    std::fill(obj.second.begin(), obj.second.end(), 0.0f);
    obj.first = fun;

    for(unsigned z = 0; z < res_z; z++ )
        for(unsigned y = 0; y < res_y; y++)
            for(unsigned x = 0; x < res_x; x++)
            {
                glm::vec3 p = glm::vec3(x, y, z) * frequency;
                float amplitude = 1.0f;

                for(unsigned l = 0; l < numLayers; l++)
                {
                    if(fun[index3d(x, y, z)] >= 0)
                    {
                        obj.second[index3d(x, y, z)] += eval(p)*amplitude;
                        p *= freqMult;
                        amplitude *= ampMult;
                    }
                    else
                        obj.second[index3d(x, y, z)] = 0.0f;
                }

                if(obj.second[index3d(x, y, z)] > maxNoiseV)
                    maxNoiseV = obj.second[index3d(x, y, z)];
            }

    for(unsigned i = 0; i < res_x * res_y * res_z; i++)
        obj.second[i] /= maxNoiseV;
}

void procNoise::procTurbulence(const std::vector<float> fun, float frequency, float freqMult,
                               float ampMult, float numLayers, float maxNoiseV, HObj_single_mat &obj)
{
    obj.first = fun;

    for(unsigned z = 0; z < res_z; z++ )
        for(unsigned y = 0; y < res_y; y++)
            for(unsigned x = 0; x < res_x; x++)
            {
                glm::vec3 p = glm::vec3(x, y, z) * frequency;
                float amplitude = 1.0f;

                for(unsigned l = 0; l < numLayers; l++)
                {
                    if(fun[index3d(x, y, z)] >= 0)
                    {
                        obj.second[index3d(x, y, z)] += std::abs(2.0f * eval(p) - 1) * amplitude;
                        p *= freqMult;
                        amplitude *= ampMult;
                    }
                    else
                        obj.second[index3d(x, y, z)] = 0.0f;
                }

                if(obj.second[index3d(x, y, z)] > maxNoiseV)
                    maxNoiseV = obj.second[index3d(x, y, z)];
            }

    for(unsigned i = 0; i < res_x * res_y * res_z; i++)
        obj.second[i] /= maxNoiseV;
}

float procNoise::eval(glm::vec3 &p)
{
    int xInt = std::floor(p.x);
    int yInt = std::floor(p.y);
    int zInt = std::floor(p.z);

    // tx - x coord of the point in which we computing a noise value
    // ty - y coord of the point in which we computing a noise value
    float tX = p.x - xInt;
    float tY = p.y - yInt;
    float tZ = p.z - zInt;

    int rX0 = xInt & maxTableSizeMask;
    int rY0 = yInt & maxTableSizeMask;
    int rZ0 = zInt & maxTableSizeMask;

    int rX1 = (rX0 + 1) & maxTableSizeMask;
    int rY1 = (rY0 + 1) & maxTableSizeMask;
    int rZ1 = (rZ0 + 1) & maxTableSizeMask;

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
