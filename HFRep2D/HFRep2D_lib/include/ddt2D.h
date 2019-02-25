#ifndef DISTANCE_FIELD_H
#define DISTANCE_FIELD_H

#include <vector>
#include <cmath>

namespace hfrep2D
{

struct Point2D
{
    Point2D(float x, float y) : dx(x), dy(y) { }
    float dx, dy;
    float EuclideanDist() { return (std::pow(dx, 2.0f) + std::pow(dy, 2.0f)); }
};

class DiscreteDistanceTransform
{

public:
    DiscreteDistanceTransform( int grWidth, int grHeight );
    ~DiscreteDistanceTransform(){ }

    void caclulateDiscreteDistanceTransformVec( std::vector<float> *inField );
    void caclulateDiscreteDistanceTransformImg( );

    inline std::vector<float> get_DDT() { return DDT; }
    inline std::vector<float> get_signed_DDT() { return SDDT; }
    inline int clipWithBounds( int n, int n_min, int n_max ) { return n > n_max ? n_max : ( n < n_min ? n_min : n );}

    //functions for generating distance transform using binarized input image
private:
    inline float   calculate_inf() { return std::max(res_x, res_y) + 1; }
    inline Point2D get_grid_element(std::vector<Point2D> &grid, int ind ) { return grid[ind]; }
    inline void    fill_grid_element(std::vector<Point2D> &grid, Point2D point, int ind) { grid[ind] = point; }

    inline int index(int x, int y) { return x+y*static_cast<int>(res_x); }

    void generate_DF(std::vector<Point2D> &grid);
    void create_grid( std::vector<float> *field );
    void compare_grid_points(std::vector<Point2D> &grid, Point2D &point, int offsetx, int offsety, int x, int y);
    void merge_grids(std::vector<float> &grid);

private:
    int res_x, res_y, b_sh;

    float INF;
    Point2D INSIDE;
    Point2D EMPTY;

    std::vector<Point2D> grid_1;
    std::vector<Point2D> grid_2;

    std::vector<float> DDT;
    std::vector<float> SDDT;
};

} // namespace distance_field

#endif // DISTANCE_FIELD_H
