#ifndef H_BARYCENTRIC_COORDS_CLASS
#define H_BARYCENTRIC_COORDS_CLASS

/*
 * This class relies on the following papers:
 *
 * for 2D case:
 * K. Hormann, M.S. Floater, Mean Value Coordinates for Arbitrary Planar Polygons, 2006,
 * ACM Trans. Graph., V. 25, N. 4, pp 1424 - 1441; https://doi.org/10.1145/1183287.1183295.
 *
 * for 3D case:
 * T. Ju, S. Schaefer, J. Warren. Mean value coordinates for closed triangular meshes. 2005.
 * In ACM SIGGRAPH 2005 Papers (SIGGRAPH ’05). Association for Computing Machinery,
 * DOI:https://doi.org/10.1145/1186822.1073229
 */


#include <Eigen/Core>
#include <vector>

 namespace idf
 {
 class baryCoords
 {
     using eigVector_t = Eigen::Matrix<float, Eigen::Dynamic, 1>;
     using eigMatrix_t = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;

 public:
     baryCoords():eps(1.0e-8) { }

     Eigen::VectorXf meanValueCoords2D(const std::vector<Eigen::Vector2d> surfMeshPoints, const Eigen::Vector2d p);
     Eigen::VectorXf meanValueCoords3D(const std::vector<Eigen::Vector3d> surfMeshPoints, const Eigen::MatrixXi surfFaces,
                                       const Eigen::Vector3d p);
    ~baryCoords() = default;

private:
    double eps;
    bool computeBoundaryCoordinates2D(const std::vector<Eigen::Vector2d> surfMeshPoints,
                                      const Eigen::Vector2d p, Eigen::VectorXf &baryCoords);
    Eigen::Vector3d computeEdgesLength(const Eigen::Vector3d u0, const Eigen::Vector3d u1, const Eigen::Vector3d u2);
 };

 } //namespace hfrep
 #endif
