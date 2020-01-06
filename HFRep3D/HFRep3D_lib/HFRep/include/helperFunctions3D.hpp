#ifndef HELPER_3D_FUNCTIONS
#define HELPER_3D_FUNCTIONS
namespace hfrep3D {

struct Point3D
{
    Point3D( float x, float y, float z ) { dx = x; dy = y; dz = z; }
    float dx, dy, dz;
    float EuclideanDist(){ return dx*dx + dy*dy + dz*dz; }
};

}
#endif
