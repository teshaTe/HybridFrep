#ifndef H_INTERPOLATION_CLASS
#define H_INTERPOLATION_CLASS

#include <vector>

namespace hfrep3D {

#define BICUBIC  1
#define TRICUBIC 2

class interpolate
{
public:
    interpolate( int gridW, int gridH, int gridD );
    ~interpolate() { }

    void interpolateField( std::vector<float> *inField, int interType );

    inline std::vector<float> getField() { return finField; }

private:
    int voxGrW, voxGrH, voxGrD;

    std::vector<float> finField;

    float interpolateCubic(float point[] , float x);
    float interpolateBicubic(std::vector<float> *field , int x, int y);
    float interpolateTricubic(std::vector<float> *field , int x, int y, int z);

    inline int index2d( int x, int y ) { return x + y*voxGrW; }
    inline int index3d( int x, int y, int z ) { return z*voxGrW*voxGrH + y*voxGrW + x; }
    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max :( n < n_min ? n_min : n ); }

};

} //namespace interpolation
#endif
