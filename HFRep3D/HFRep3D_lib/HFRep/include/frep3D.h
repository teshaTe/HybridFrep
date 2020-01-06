#ifndef H_FREP_3D_CLASS
#define H_FREP_3D_CLASS

#include "helperFunctions3D.hpp"

#include <cmath>
#include <vector>
#include <functional>

namespace hfrep3D {

/*struct Point3D
{
    Point3D( float x, float y, float z ) { dx = x; dy = y; dz = z; }
    float dx, dy, dz;
    float EuclideanDist(){ return dx*dx + dy*dy + dz*dz; }
};*/

class FRepObj3D
{
//primitives
public:
    FRepObj3D(int resX, int resY, int resZ, float scaleF);
    ~FRepObj3D(){}

    float heart3D ( Point3D pos, Point3D center );

    float sphere ( Point3D pos, Point3D center, float R );
    float blobby ( Point3D pos, Point3D center, float R );

    float cylinderX( Point3D pos, Point3D center, float R );
    float cylinderY( Point3D pos, Point3D center, float R );
    float cylinderZ( Point3D pos, Point3D center, float R );

    float ellipticCylinderX( Point3D pos, Point3D center, float a, float b );
    float ellipticCylinderY( Point3D pos, Point3D center, float a, float b );
    float ellipticCylinderZ( Point3D pos, Point3D center, float a, float b );

    float coneX ( Point3D pos, Point3D center, float R );
    float coneY ( Point3D pos, Point3D center, float R );
    float coneZ ( Point3D pos, Point3D center, float R );

    float ellipticConeX(Point3D pos, Point3D center, float a, float b );
    float ellipticConeY( Point3D pos, Point3D center, float h, float a, float b );
    float ellipticConeZ( Point3D pos, Point3D center, float h, float a, float b );

    float torusX ( Point3D pos, Point3D center, float R, float r );
    float torusY ( Point3D pos, Point3D center, float R, float r );
    float torusZ ( Point3D pos, Point3D center, float R, float r );

    float box       ( Point3D center, float w, float h, float d );
    float ellipsoid ( Point3D pos, Point3D center, float a, float b, float c );

    float superEllipsoid( Point3D pos, Point3D center, float a, float b, float c, float s1, float s2 );

    float elf3D( Point3D pos );

//operations over primitives and some other operations
public:
    std::vector<float> getFRep3D( std::function<float(Point3D)> fun );
    std::vector<float> getFRep3D( Point3D cent, std::function<float( Point3D, Point3D )> fun);
    std::vector<float> getFRep3D( Point3D cent, float R, std::function<float( Point3D, Point3D, float )> fun );
    std::vector<float> getFRep3D( Point3D cent, float a, float b, std::function<float( Point3D, Point3D, float, float )> fun );
    std::vector<float> getFRep3D( Point3D cent, float a, float b, float c, std::function<float( Point3D, float, float, float )> fun );
    std::vector<float> getFRep3D( Point3D cent, float a, float b, float c, std::function<float( Point3D, Point3D, float, float, float )> fun );
    std::vector<float> getFRep3D( Point3D cent, float a, float b, float c, float d, std::function<float( Point3D, Point3D, float, float, float, float )> fun );

    inline void setNewResolution( int resX, int resY, int resZ ) { resolutionX = resX;
                                                                   resolutionY = resY;
                                                                   resolutionZ = resZ; }
    inline void setNewScale( float scaleF) { scale = scaleF; }

    inline float intersect_function(float f1, float f2, float alpha, float m)
    { return (1.0f/(1.0f+alpha))*( f1 + f2 - std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f)); }

    inline float union_function(float f1, float f2, float alpha, float m)
    { return (1.0f/(1.0f+alpha))*( f1 + f2 + std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f)); }

    inline float subtract_function(float f1, float f2, float alpha, float m) { return intersect_function(f1, -f2, alpha, m); }

    std::vector<float> rotateObj( std::vector<float> obj, float angle );
    std::vector<float> scaleObj ( std::vector<float> obj, float scale );

private:
    int resolutionX, resolutionY, resolutionZ;
    float scale;

    std::vector<float> frep;

    inline float convertToSTR( float val ) { return val / resolutionX; }
    inline Point3D convertToSTR( Point3D val ) { return Point3D( val.dx/resolutionX, val.dy/resolutionY, val.dz/resolutionZ ); }

};

} //namespace frep3D_object
#endif
