#ifndef H_FRACTALS_CLASS
#define H_FRACTALS_CLASS

#include "frep2D.h"
#include <vector>
#include <cassert>
#include <complex>

namespace hfrep2D {

class fractals
{
public:
    fractals(int resX, int resY): res_x(resX), res_y(resY) {}
    ~fractals() {}

    std::vector<float> julia(const std::complex<float> c, const int maxDepth, const int r, const int detail);
    std::vector<float> mandelbrot(const int iterations);

private:
    friend class FRepObj2D;

    float juliaP(const float x, const float y, const int r, int depth,
                 const int nMax, std::complex<float> c, std::complex<float> z);
    float mandelbrotP(const int x, const int y, const int iterations);

private:
    int res_x, res_y;
    std::vector<float> field;

};

} //namespace frep2D
#endif //H_FRACTALS_CLASS
