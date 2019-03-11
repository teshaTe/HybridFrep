#ifndef H_FREP_3D_CLASS
#define H_FREP_3D_CLASS

#include <cmath>
#include <vector>
#include <functional>

namespace frep3D {

struct Point3D
{
    Point3D( float x, float y, float z ) { dx = x; dy = y; dz = z; }
    float dx, dy, dz;
    float EuclideanDist(){ return dx*dx + dy*dy + dz*dz; }
};

class FRepObj3D {
public:
    FRepObj3D(int resX, int resY, int resZ, float scaleF);
    ~FRepObj3D(){}

    float sphere(Point3D pos, Point3D center, float R );
    float heart3D( Point3D pos, Point3D center );

    std::vector<float> getFRep3D( Point3D cent, std::function<float( Point3D, Point3D )> fun);
    std::vector<float> getFRep3D( Point3D cent, float R, std::function<float( Point3D, Point3D, float )> fun );

    inline void setNewResolution( int resX, int resY, int resZ ) { resolutionX = resX;
                                                                   resolutionY = resY;
                                                                   resolutionZ = resZ; }
    inline void setNewScale( float scaleF) { scale = scaleF; }

    inline float intersect_function(float a, float b) { return a + b - std::sqrt(a * a + b * b); }
    inline float union_function(float a, float b)     { return a + b + std::sqrt(a * a + b * b); }
    inline float subtract_function(float a, float b)  { return intersect_function(a, -b); }

private:
    int resolutionX, resolutionY, resolutionZ;
    float scale;

    std::vector<float> frep;

    inline float convertToSTR( float val ) { return val / resolutionX; }
    inline Point3D convertToSTR( Point3D val ) { return Point3D( val.dx/resolutionX, val.dy/resolutionY, val.dz/resolutionZ ); }

};

} //namespace frep3D_object
#endif
