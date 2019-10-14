#ifndef H_HELPER_FUNCTIONS
#define H_HELPER_FUNCTIONS

#include <cmath>

namespace hfrep2D {

struct node2D
{
    node2D(float x, float y): dx(x), dy(y) { }
    float dx, dy;
    float dist;
    float EuclideanDist() { return (std::pow(dx, 2.0f) + std::pow(dy, 2.0f)); }
};

struct Point2Df
{
    Point2Df(float x, float y): dx(x), dy(y) { }
    float dx, dy;
};

struct Point2Di
{
    Point2Di(int x, int y): dx(x), dy(y) { }
    int dx, dy;
};

} //namespace hfrep2D

#endif //#define H_HELPER_FUNCTIONS
