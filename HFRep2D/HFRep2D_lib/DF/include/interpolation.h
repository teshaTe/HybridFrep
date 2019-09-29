#ifndef H_INTERPOLATION_CLASS
#define H_INTERPOLATION_CLASS

#include <vector>

namespace interpolation {

#define BICUBIC_INT  1
#define TRICUBIC_INT 2

class interpolate
{
public:
    interpolate( int gridW, int gridH, int gridD );
    ~interpolate() { }

    void smoothField( std::vector<float> *inField, int interType );
    void interpolateField( std::vector<float> *inField, int curGridW, int curGridH, int curGridD,int interType );

    inline std::vector<float> getField() { return finField; }

private:
    int voxGrW, voxGrH, voxGrD;

    std::vector<float> finField;

    float interpolateCubic(float point[] , float x);
    float interpolateBicubic(std::vector<float> *field, int curGridW, int curGridH, int x, int y);
    float interpolateTricubic(std::vector<float> *field , int x, int y, int z);

    inline int index2d( int x, int y, int curW ) { return x + y*curW; }
    inline int index3d( int x, int y, int z, int curW, int curH ) { return z*curW*curH + y*curW + x; }
    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max :( n < n_min ? n_min : n ); }

};

} //namespace interpolation
#endif
