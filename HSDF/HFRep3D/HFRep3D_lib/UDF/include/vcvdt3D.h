#ifndef H_DDT3D_CLASS
#define H_DDT3D_CLASS

#include <glm/glm.hpp>
#include <vector>
#include <cmath>

namespace hfrep3D {

class distanceTransform3D
{
public:
    distanceTransform3D(int gridW, int gridH, int gridD);
    ~distanceTransform3D() = default;

    void caclulateDiscreteDistanceTransform(std::vector<float> *inField);

    inline std::vector<float> getDDT() { return DDT3D; }
    inline std::vector<float> getSignedDDT() { return SDDT3D; }

private:
    int voxGrW, voxGrH, voxGrD;
    float INF;

    glm::vec3 INSIDE, OUTSIDE;
    std::vector<glm::vec3> vGrid1, vGrid2;
    std::vector<float> DDT3D, SDDT3D;

    void initializeVoxelGrid(std::vector<float> *field);
    void compareVoxelVectors(std::vector<glm::vec3> &voxGrid, glm::vec3 &curPoint,
                             int offsetX, int offsetY , int offsetZ ,
                             int x, int y, int z);

    glm::vec3 getOffsettedVoxelVal(std::vector<glm::vec3> &voxGrid, int offsetX, int offsetY, int offsetZ,
                                                                                      int x, int y, int z);
    glm::vec3 findMinVal(glm::vec3 *v1 , glm::vec3 *v2);

    void traverseVoxelGridForwardPass(std::vector<glm::vec3> &grid);
    void traverseVoxelGridBackwardPass(std::vector<glm::vec3> &grid);
    void mergeVoxGrids();

    inline int index(int x, int y, int z) { return z*voxGrW*voxGrH + y*voxGrW + x; }
    inline float euclideanDistSq(glm::vec3 point) { return point.x*point.x + point.y*point.y + point.z*point.z; }
    inline float calcINFval(){ return static_cast<float>(voxGrW+1); }

    inline glm::vec3 getVoxGridElement(std::vector<glm::vec3> &grid, int ind) { return grid[ind]; }
    inline void      fillVoxGridElement(std::vector<glm::vec3> &grid, glm::vec3 point, int ind) { grid[ind] = point; }
};

}// namespace hfrep3D
#endif
