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
    DistanceField(cv::Mat img, int b_sh);
    ~DistanceField(){ }

    inline std::vector<double> get_DDT()               { return DDT; }

    std::vector<double> zoom_in_field (const std::vector<double> *field, const cv::Point2i start_p, const cv::Vec2i reg_s, const cv::Vec2i fin_res , const int resX);
    std::vector<double> finalize_field(const std::vector<double> *field, int resX, int resY);

    std::vector<double> smooth_field(const std::vector<double> *DDT, int resX, int resY);
    std::vector<double> generate_interpolated_field(const std::vector<double> *DT, int resX, int resY,
                                                    int shiftX, int shiftY, int interpolation_type = 0 );
    std::vector<double> generate_extrapolated_field( const std::vector<double> DDT, int interpolation_type = 0 );

    //functions for generating distance transform using binarized input image
private:
    inline int   calculate_inf() { return std::max(res_x, res_y) + 1; }
    inline Point get_grid_element(std::vector<Point> &grid, int x, int y) { return grid[x + y * res_x]; }
    inline void  fill_grid_element(std::vector<Point> &grid, Point point, int x, int y) { grid[x + y * res_x] = point; }

    cv::Mat binarize_input(cv::Mat src);
    void generate_DF(std::vector<Point> &grid);
    void create_grid(cv::Mat *img);
    void compare_grid_points(std::vector<Point> &grid, Point &point, int offsetx, int offsety, int x, int y);
    void merge_grids(std::vector<double> &grid);

private:
    int res_x, res_y, b_sh;
    int pix_xShift, pix_yShift;

    const int INF;
    const Point INSIDE;
    const Point EMPTY;

    std::vector<Point> grid_1;
    std::vector<Point> grid_2;

    std::vector<double> DDT;
    std::vector<double> SDDT;
    std::vector<double> ISDDT;
};

} // namespace distance_field

#endif // DISTANCE_FIELD_H
