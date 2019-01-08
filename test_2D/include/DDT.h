#ifndef DISTANCE_FIELD_H
#define DISTANCE_FIELD_H

#include <opencv2/core.hpp>

#include <vector>
#include <cmath>

namespace distance_transform
{

struct Point
{
  Point(double dx, double dy);
  double m_dx, m_dy;
  double square_distance();
};

class DistanceField
{

public:
    DistanceField( cv::Mat img, int b_sh );
    DistanceField(std::vector<double> field, int f_width, int f_height );
    ~DistanceField(){ }

    inline std::vector<double> get_DDT() { return DDT; }
    inline std::vector<double> get_signed_DDT() { return SDDT; }

    //functions for generating distance transform using binarized input image
private:
    inline int   calculate_inf() { return std::max(res_x, res_y) + 1; }
    inline Point get_grid_element(std::vector<Point> &grid, int x, int y) { return grid[x + y * res_x]; }
    inline void  fill_grid_element(std::vector<Point> &grid, Point point, int x, int y) { grid[x + y * res_x] = point; }

    cv::Mat binarize_input(cv::Mat src);
    void generate_DF(std::vector<Point> &grid);
    void create_grid(cv::Mat *img, std::vector<double> *field=nullptr);
    void compare_grid_points(std::vector<Point> &grid, Point &point, int offsetx, int offsety, int x, int y);
    void merge_grids(std::vector<double> &grid);

private:
    int res_x, res_y, b_sh;

    const int INF;
    const Point INSIDE;
    const Point EMPTY;

    std::vector<Point> grid_1;
    std::vector<Point> grid_2;

    std::vector<double> DDT;
    std::vector<double> SDDT;
};

} // namespace distance_field

#endif // DISTANCE_FIELD_H
