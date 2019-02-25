#ifndef H_FREP_CLASS
#define H_FREP_CLASS

#include <cmath>
#include <vector>
#include <functional>

namespace frep2D {

struct Point2D
{
    Point2D(float x, float y) { dx = x; dy = y; }
    float dx, dy;
};

class FRepObj2D
{
public:
    FRepObj2D(int resX, int resY, float scaleF );
    ~FRepObj2D(){ }

    //2D primitves
    float triangle( Point2D pos, Point2D cent, float a, float b, float c );

    float ellipticCylZ2D(Point2D pos, Point2D cent, float a, float b);
    float ellipsoid2D( Point2D pos, Point2D cent, float a, float b );
    float torusY2D(Point2D pos, Point2D cent, float R , float rev);
    float torusZ2D( Point2D pos, Point2D cent, float R, float rev );
    float rectangle( Point2D pos, Point2D cent, float w, float h );

    float circle( Point2D pos, Point2D cent, float R );
    float blobby2D( Point2D pos, Point2D cent, float R );

    float heart2D( Point2D pos, Point2D cent );
    float suriken( Point2D pos, Point2D cent );
    float elf(Point2D pos);

    std::vector<float> getFRep2D( std::function<float( Point2D )> fun);
    std::vector<float> getFRep2D( Point2D cent, std::function<float( Point2D, Point2D )> fun);
    std::vector<float> getFRep2D( Point2D cent, float R, std::function<float( Point2D, Point2D, float )> fun );
    std::vector<float> getFRep2D( Point2D cent, float p1, float p2, std::function<float( Point2D, Point2D, float, float)> fun );
    std::vector<float> getFRep2D( Point2D cent, float p1, float p2, float p3, std::function<float( Point2D, Point2D, float, float, float )> fun );

    std::vector<float> getRotatedFrep2D( Point2D cent, float w, float h,
                                         float angle, std::function<float (Point2D, Point2D, float, float )>);
    //operations over 2D primitives
public:
    float bounded_blending(float f1, float f2, float a0, float a1, float a2, float a3 , float time);
    float constOffset(float f, float offset);
    float constRadiusOffset(float f, float fOffset, float R, float x0, float y0 );

    inline float intersect_function(float f1, float f2) { return f1 + f2 - std::sqrt(f1 * f1 + f2 * f2); }
    inline float union_function(float f1, float f2)     { return f1 + f2 + std::sqrt(f1 * f1 + f2 * f2); }
    inline float subtract_function(float f1, float f2)  { return intersect_function(f1, -f2); }

    inline void setNewRes( int resX, int resY )  { resolutionX = resX; resolutionY = resY; }
    inline void setScalingFactor( float scaleF ) { scale = scaleF; }

private:
    int resolutionX, resolutionY;
    float scale;

    std::vector<float> frep;

    inline float convertToUV( float val ) { return val / resolutionX; }
    inline Point2D convertToUV( Point2D val ) { return Point2D( val.dx/resolutionX, val.dy/resolutionY ); }

};

} //namespace frep2D_object
#endif
