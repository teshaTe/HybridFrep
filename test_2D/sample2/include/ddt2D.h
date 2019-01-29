#ifndef DISTANCE_FIELD_H
#define DISTANCE_FIELD_H

#include <opencv2/core.hpp>

#include <vector>
#include <cmath>

namespace distance_transform
{

struct Point2D
{
    Point2D(double dx, double dy) : m_dx(dx), m_dy(dy) { }
    double m_dx, m_dy;
    double square_distance() { return (std::pow(m_dx, 2.0) + std::pow(m_dy, 2.0)); }
};

class DiscreteDistanceTransform
{

public:
    DiscreteDistanceTransform( cv::Mat img, int b_sh );
    DiscreteDistanceTransform( int grWidth, int grHeight );
    ~DiscreteDistanceTransform(){ }

    void caclulateDiscreteDistanceTransformVec( std::vector<double> *inField );
    void caclulateDiscreteDistanceTransformImg( );

    inline std::vector<double> get_DDT() { return DDT; }
    inline std::vector<double> get_signed_DDT() { return SDDT; }

    //functions for generating distance transform using binarized input image
private:
    inline double   calculate_inf() { return std::max(res_x, res_y) + 1; }
    inline Point2D get_grid_element(std::vector<Point2D> &grid, int ind ) { return grid[ind]; }
    inline void  fill_grid_element(std::vector<Point2D> &grid, Point2D point, int ind) { grid[ind] = point; }

    inline int index(int x, int y) { return x+y*static_cast<int>(res_x); }

    cv::Mat binarize_input(cv::Mat src);
    void generate_DF(std::vector<Point2D> &grid);
    void create_grid(cv::Mat *img, std::vector<double> *field=nullptr);
    void compare_grid_points(std::vector<Point2D> &grid, Point2D &point, int offsetx, int offsety, int x, int y);
    void merge_grids(std::vector<double> &grid);

private:
    int res_x, res_y, b_sh;
    cv::Mat newSrc;

    double INF;
    Point2D INSIDE;
    Point2D EMPTY;

    std::vector<Point2D> grid_1;
    std::vector<Point2D> grid_2;

    std::vector<double> DDT;
    std::vector<double> SDDT;
};

} // namespace distance_field

#endif // DISTANCE_FIELD_H
