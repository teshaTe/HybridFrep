#include "include/proceduralNoise.h"

#include <iostream>
#include <random>
#include <functional>

namespace material {

procNoise::procNoise(const int seed, const unsigned permTableSize): randSeed(seed),
                                                                    tableSize(permTableSize)
{
    maxTableSizeMask = tableSize - 1;
    permutationTable.resize(tableSize * 2);
    r.resize(tableSize);
    valueNoiseInit();
}

void procNoise::procWood(const Eigen::VectorXd field, const Eigen::MatrixXd V, float frequency, Eigen::MatrixXd &texture)
{
    texture.resize(field.rows(), 3);
    for(int i = 0; i < field.size(); i++)
    {
         Eigen::Vector2d p = V.row(i)*100;
         p *= frequency*field[i];
         float g = eval(p) * 10.0f*field[i];
         texture.row(i) = Eigen::Vector3d((g - static_cast<int>(g)), field[i], field[i]*0.5);
    }
}

void procNoise::procMarble(const Eigen::VectorXd field, const Eigen::MatrixXd V, float frequency, float freqMult,
                           float ampMult, float numLayers, Eigen::MatrixXd &texture)
{
    texture.resize(field.rows(), 3);
    for(int i = 0; i < field.size(); i++)
    {
        Eigen::Vector2d p = V.row(i) * frequency;
        double amplitude = 1.0;
        double noiseV = 0.0;
        for(unsigned l = 0; l < numLayers; l++)
        {
            noiseV += eval(p) * amplitude * field[i];
            p *= freqMult;
            amplitude *= ampMult * field[i];
        }
        double marble = (std::sin((i + noiseV * 100.0) * 2.0 * M_PI / 200.0) + 1.0) / 2.0;
        texture.row(i) = Eigen::Vector3d(marble, field[i], (marble + field[i]));
    }
}

float procNoise::eval(Eigen::Vector2d &p)
{
    int xInt = std::floor(p.x());
    int yInt = std::floor(p.y());

    // tx - x coord of the point in which we computing a noise value
    // ty - y coord of the point in which we computing a noise value
    float tX = p.x() - xInt;
    float tY = p.y() - yInt;

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
