#include "fractals2D.h"
#include <iostream>
#include <complex>

namespace hfrep2D {

std::vector<float> fractals::julia(const std::complex<float> c, const int maxDepth, const int r, const int detail)
{
    field.clear(); field.reserve(res_x*res_y);
    for(float x = res_x/2 - res_y/2; x < res_x/2 + res_y/2; x+= detail)
        for(float y = 0; y < res_y; y+=detail)
        {
            std::complex<float> z(2.0f * r * (x - res_x/2.0f)/res_y, (2.0f*r*(y - res_y/2.0f)/res_y));
            field.push_back(juliaP(x, y, r, maxDepth, maxDepth, c, z));
        }

    return field;
}

float fractals::juliaP(const float x, const float y, const int r, int depth, const int nMax,
                       std::complex<float> c, std::complex<float> z)
{
    if(std::abs(z) > r)
    {
        depth = 0;
        return -std::norm(z);
    }
    if(std::sqrt(std::pow(x - res_x/2.0f, 2) + std::pow(y - res_y/2.0f, 2)) > res_y / 2.0f)
        return std::norm(z);
    if(depth < nMax/4)
        return std::norm(z);

    return juliaP(x, y, r, depth-1, nMax, c, std::pow(z, 2) + c);
}

std::vector<float> fractals::mandelbrot(const int iterations)
{
    field.clear(); field.resize(res_x*res_y);

    for(int i = 0; i < res_y; i++)
        for(int j = 0; j < res_x; j++)
            field[j+i*res_x] = mandelbrotP(i, j, iterations);

    return field;
}

float fractals::mandelbrotP(const int x, const int y, const int iterations)
{
    std::complex<float> point(2.4*(float)x/res_x - 1.7, 2.4*(float)y/res_y - 1.2);
    std::complex<float> z(0, 0);
    int nb_iter = 0;
    while (abs (z) < 2 && nb_iter <= iterations)
    {
        z = z * z + point; //Mandelbrot complex function
        nb_iter++;
    }
    if (nb_iter < iterations)
       return -std::abs(z);
    else
       return std::abs(z);
}

} //namespace hfrep2D
