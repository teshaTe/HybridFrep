#ifndef H_FREP_CLASS
#define H_FREP_CLASS

#include "helperFunctions.hpp"
#include <cmath>
#include <vector>
#include <functional>

namespace hfrep2D {

class FRepObj2D
{
public:
    FRepObj2D(int resX, int resY, float scaleF );
    ~FRepObj2D(){ }

    //2D primitves
    float triangle(Point2Df pos, Point2Df cent, float a, float b, float c );
    float triangle2(Point2Df pos, Point2Df cent, float a, float b);

    float ellipticCylZ2D(Point2Df pos, Point2Df cent, float a, float b);
    float ellipsoid2D( Point2Df pos, Point2Df cent, float a, float b );
    float torusY2D(Point2Df pos, Point2Df cent, float R , float rev);
    float torusZ2D( Point2Df pos, Point2Df cent, float R, float rev );
    float rectangle(Point2Df pos, Point2Df cent, float w, float h );

    float circle( Point2Df pos, Point2Df cent, float R );
    float blobby2D( Point2Df pos, Point2Df cent, float R );

    float heart2D( Point2Df pos, Point2Df cent );
    float decocube2D( Point2Df pos, Point2Df cent );
    float suriken(Point2Df pos, Point2Df cent );
    float elf(Point2Df pos);
    float bat(Point2Df pos);
    float trebleClef(Point2Df pos);

    std::vector<float> getFRep2D( std::function<float( Point2Df )> fun);
    std::vector<float> getFRep2D( Point2Df cent, std::function<float( Point2Df, Point2Df )> fun);
    std::vector<float> getFRep2D( Point2Df cent, float R, std::function<float( Point2Df, Point2Df, float )> fun );
    std::vector<float> getFRep2D( Point2Df cent, float p1, float p2, std::function<float( Point2Df, Point2Df, float, float)> fun );
    std::vector<float> getFRep2D( Point2Df cent, float p1, float p2, float p3, std::function<float( Point2Df, Point2Df, float, float, float )> fun );
    std::vector<float> getFRep2D(std::vector<float> f1, std::vector<float> f2, float alpha, float m,
                                  std::function<float(float, float, float, float)> fun );
    std::vector<float> getFRep2D( std::vector<float> f1, std::vector<float> f2, float n,
                                  std::function<float(float, float, float)> fun );

    std::vector<float> getRotatedFrep2D( Point2Df cent, float w, float h,
                                         float angle, std::function<float (Point2Df, Point2Df, float, float )>);
    std::vector<float> getRotatedFrep2D(Point2Df cent, float a, float b, float c,
                                         float angle, std::function<float(Point2Df, Point2Df, float, float, float, float)>);
    //operations over 2D primitives
public:
    float bounded_blending(float f1, float f2, float a0, float a1, float a2, float a3 , float time, float alpha, float m);
    float constOffset(float f, float offset);
    float constRadiusOffset(float f, float fOffset, float R, float x0, float y0 );

    float intersect_function_R0(float f1, float f2, Point2Df gradf1, Point2Df gradf2, float n);
    float union_function_R0(float f1, float f2, Point2Df gradf1, Point2Df gradf2, float n);
    float union_function_R0(float f1, float f2, float n);

    std::vector<float> scaleFunction( const std::vector<float> field, const float factor )
    {
        std::vector<float> scaledField;
        std::transform( field.begin(), field.end(), std::back_inserter(scaledField), std::bind(std::multiplies<float>(), factor, std::placeholders::_1) );
        return scaledField;
    }

    inline float intersect_function(float f1, float f2, float alpha, float m)
    { return (1.0f/(1.0f+alpha))*( f1 + f2 - std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f)); }

    inline float union_function(float f1, float f2, float alpha, float m)
    { return (1.0f/(1.0f+alpha))*( f1 + f2 + std::sqrt(f1 * f1 + f2 * f2 - 2*alpha*f1*f2)*std::pow(f1*f1+f2*f2, m/2.0f)); }

    inline float subtract_function(float f1, float f2, float alpha, float m) { return intersect_function(f1, -f2, alpha, m); }

    inline void setNewRes( int resX, int resY )  { resolutionX = resX; resolutionY = resY; }
    inline void setScalingFactor( float scaleF ) { scale = scaleF; }

    inline float convertToUV( float val ) { return val / resolutionX; }
    inline Point2Df convertToUV( Point2Df val ) { return Point2Df( val.dx/resolutionX, val.dy/resolutionY ); }

private:
    Point2Df getRotatedCoords(Point2Df inCoords , const float angle);

    int resolutionX, resolutionY;
    float scale;

    std::vector<float> frep;

};

} //namespace frep2D_object
#endif
