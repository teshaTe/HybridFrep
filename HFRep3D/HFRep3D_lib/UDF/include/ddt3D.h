#ifndef H_DDT3D_CLASS
#define H_DDT3D_CLASS

#include "helperFunctions3D.hpp"

#include <vector>
#include <cmath>

namespace hfrep3D {

/*struct Point3D
{
    Point3D( float x, float y, float z ) { dx = x; dy = y; dz = z; }
    float dx, dy, dz;
    float EuclideanDist(){ return dx*dx + dy*dy + dz*dz; }
};*/

class distanceTransform3D
{
public:
    distanceTransform3D( int gridW, int gridH, int gridD );
    ~distanceTransform3D() { }

    void caclulateDiscreteDistanceTransform( std::vector<float> *inField );

    inline std::vector<float> getDDT() { return DDT3D; }
    inline std::vector<float> getSignedDDT() { return SDDT3D; }

private:
    int voxGrW, voxGrH, voxGrD;
    float INF;

    Point3D INSIDE, OUTSIDE;
    std::vector<Point3D> vGrid1, vGrid2;
    std::vector<float> DDT3D, SDDT3D;

    void initializeVoxelGrid(std::vector<float> *field);
    void compareVoxelVectors(std::vector<Point3D> &voxGrid, Point3D &curPoint,
                             int offsetX, int offsetY , int offsetZ ,
                             int x, int y, int z);
    void traverseVoxelGridForwardPass(std::vector<Point3D> &grid);
    void traverseVoxelGridBackwardPass(std::vector<Point3D> &grid);
    void mergeVoxGrids();

    inline int index(int x, int y, int z) { return z*static_cast<int>(voxGrW*voxGrH) +
                                                   y*static_cast<int>(voxGrW) + x; }
    inline float calcINFval(){ return static_cast<float>(std::max(voxGrW, std::max(voxGrH, voxGrD))+1); }

    inline Point3D getVoxGridElement(std::vector<Point3D> &grid, int ind) { return grid[ind]; }
    inline void    fillVoxGridElement(std::vector<Point3D> &grid, Point3D point, int ind) { grid[ind] = point; }
};

}// namespace hfrep3D
#endif
