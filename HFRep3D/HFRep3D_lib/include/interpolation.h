#ifndef H_INTERPOLATION_CLASS
#define H_INTERPOLATION_CLASS

#include <iostream>
#include <vector>

namespace interpolation_3d {

#define TRILINEAR 0
#define TRICUBIC  1

class interpolation
{
public:
    interpolation( int gridW, int gridH, int gridD );
    ~interpolation() { }

    void interpolateField( std::vector<float> *inField, int interType );

    inline std::vector<float> getField() { return finField; }

private:
    int voxGrW, voxGrH, voxGrD;

    std::vector<float> finField;

    void interpolateTrilinear( std::vector<float> *field );
    void interpolateTricubic( std::vector<float> *field );

    inline int index( int x, int y, int z ) { return z*voxGrW*voxGrH + y*voxGrW + x; }
    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max :( n < n_min ? n_min : n ); }

};

} //namespace myMesh
#endif
