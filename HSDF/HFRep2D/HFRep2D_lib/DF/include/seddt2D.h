/*
 * The algorithm used for computing the Signed Discrete Distance Transform is based on the article
 * "Fast raster scan distance propagation on the discrete rectangular lattice", F. Leymarie and M.D. Levine
 * ( https://www.sciencedirect.com/science/article/pii/104996609290008Q?via%3Dihub )
 */

#ifndef DISTANCE_FIELD_H
#define DISTANCE_FIELD_H

#include <vector>
#include <cmath>
#include <glm/glm.hpp>

namespace hfrep2D
{

class DiscreteDistanceTransform
{

public:
    DiscreteDistanceTransform( int grWidth, int grHeight );
    ~DiscreteDistanceTransform(){ }

    void caclulateDiscreteDistanceTransformVec( std::vector<float> *inField );
    void calculateSparseDiscreteDistanceTransformVec( std::vector<float> *inField, int inFResX, int inFResY, float stX, float stY );
    void caclulateDiscreteDistanceTransformImg( );

    inline void setNewResolution( int resX, int resY ) { res_x = resX; res_y = resY; }
    inline std::vector<float> get_DDT() { return DDT; }
    inline std::vector<float> get_signed_DDT() { return SDDT; }
    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max : ( n < n_min ? n_min : n );}

    //functions for generating distance transform using binarized input image
private:
    inline float euclideanDistSq(glm::vec2 point) { return point.x*point.x + point.y*point.y; }
    inline float  calculate_inf() { return std::max(res_x, res_y) + 1; }
    inline glm::vec2 get_grid_element(std::vector<glm::vec2> &grid, int ind ) { return grid[ind]; }
    inline void   fill_grid_element(std::vector<glm::vec2> &grid, glm::vec2 point, int ind) { grid[ind] = point; }

    inline int index(int x, int y) { return x+y*res_x; }

    void generate_DF(std::vector<glm::vec2> &grid);
    void create_grid( std::vector<float> *field );
    void compare_grid_points(std::vector<glm::vec2> &grid, glm::vec2 &point, int offsetx, int offsety, int x, int y);
    void merge_grids(std::vector<float> &grid);

private:
    int res_x, res_y, b_sh;

    float INF;
    glm::vec2 INSIDE;
    glm::vec2 EMPTY;

    std::vector<glm::vec2> grid_1;
    std::vector<glm::vec2> grid_2;

    std::vector<float> DDT;
    std::vector<float> SDDT;
};

} // namespace distance_field

#endif // DISTANCE_FIELD_H
