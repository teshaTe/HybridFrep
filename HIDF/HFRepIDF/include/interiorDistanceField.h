#ifndef H_INTERIOR_DISTANCE_FIELDS_CLASS
#define H_INTERIOR_DISTANCE_FIELDS_CLASS

/*
 * This class is based on the following papers:
 * Rustamov, R.M., Lipman, Y. and Funkhouser, T. (2009), Interior Distance Using Barycentric Coordinates.
 * Computer Graphics Forum, 28: 1279-1288. doi:10.1111/j.1467-8659.2009.01505.x
 *
 * The external library - dmaps is based on the following paper:
 * Ronald R. Coifman, Stéphane Lafon, Diffusion maps, Applied and Computational Harmonic Analysis,
 * Volume 21, Issue 1, 2006, Pages 5-30, https://doi.org/10.1016/j.acha.2006.04.006.
 *
 * GTS library is used for extracting polygonised surface of the object defined by the function
 * for its further processing in dmaps to obtain diffusion map.
 *
 * libIGL library is used for cleaning the obtained mesh after triangulation/tetrahidralisation
 * and rendering the computed IDF.
 *
 */

#include "dmaps/include/diffusion_map.h"
#include "dmaps/include/distance_matrix.h"

#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Core>
#include <vector>

#include <gtsconfig.h>
#include <gts.h>

namespace idf {

class IDFdiffusion
{
public:
    IDFdiffusion():m_isolines(70)
    {
#ifdef _OPENMP
        Eigen::initParallel();
#endif
        m_slice_z = 0.5;
    }

    void computeIDF_polygon2D(const Eigen::MatrixXd &polyVerts, const Eigen::MatrixXi &meshEdges, const Eigen::Vector2d &srcP, const int eigVecNumber, double kernelBandW);
    void computeIDF_polygon2D(GtsIsoCartesianFunc func, const Eigen::Vector3i resGr, const Eigen::Vector2d &srcP, const int eigVecNumber, double kernelBandW, Eigen::MatrixXd H);

    void computeIDF_mesh3D(const Eigen::MatrixXd &meshVerts, const Eigen::MatrixXi &meshFaces, const Eigen::Vector3d &srcP, const int eigVecNumber, double kernelBandW);
    void computeIDF_mesh3D(GtsIsoCartesianFunc func, const Eigen::Vector3d &srcP, const Eigen::Vector3i gridRes, double iso,
                           const int eigVecNumber, double kernelBandW);

    void computeIDF_slice(const Eigen::MatrixXd &meshVerts, const Eigen::MatrixXi &meshFaces, const Eigen::Vector3d &srcP,
                          const int eigVecNumber, double kernelBandW);
    void computeIDF_slice(GtsIsoCartesianFunc func, const Eigen::Vector3d &srcP, const Eigen::Vector3i gridRes, double iso,
                          const int eigVecNumber, double kernelBandW);

    void plotIDF2D(int isoNum = 30);
    void plotIDF3D(int isoNum = 30);

    void getSurfaceComponents2D(GtsIsoCartesianFunc func, const Eigen::Vector3i gridRes,
                                double iso, Eigen::MatrixXd &V, Eigen::MatrixXi &E);
    void getSurfaceComponents3D(GtsIsoCartesianFunc func, const Eigen::Vector3i gridRes,
                                double iso, Eigen::MatrixXd &V, Eigen::MatrixXi &F);

    inline Eigen::VectorXf getIDF(){ return m_IDF; }
    inline void setIsolinesNumber(int isoNum) { m_isolines = isoNum; }

    ~IDFdiffusion() = default;

#ifdef USE_MATPLOTLIB
    void plotDiffusionMap();
#endif

private:    
    void computeInteriorDF2D(const Eigen::MatrixXd &surfMeshV, const Eigen::MatrixXd &inVerts, const Eigen::VectorXd &srcP);
    void computeInteriorDF3D(const Eigen::MatrixXd &surfMeshV, const Eigen::MatrixXd &inVerts, const Eigen::MatrixXi surfFaces, const Eigen::VectorXd &srcP);

    void iterateIDF_slice(const Eigen::MatrixXd &surfVertices, const Eigen::MatrixXi surfFaces, const Eigen::VectorXd &srcP );

    void computeDiffusionMap(const dmaps::matrix_t &inPoints, const int eigVecNum, double kernelBandWidth );
    Eigen::MatrixXf computePairwiseDist();

    void update_visualization(igl::opengl::glfw::Viewer &viewer);
    void setColormap(igl::opengl::glfw::Viewer & viewer, int isoNum);
    bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier);

    void findZeroLevelSet2D(const std::vector<float> func, int resX, int resY);
    void resetParams();

private:
    dmaps::matrix_t m_eigVecs;
    dmaps::vector_t m_eigVals;
    dmaps::matrix_t m_dist, m_kernelM;

    Eigen::MatrixXd m_V, m_Vm, m_TVm;
    Eigen::MatrixXi m_F, m_Fm, m_TFm, m_Tm, m_Em;
    Eigen::VectorXf m_IDF;

    //parameters for slicer
    Eigen::MatrixXd m_V_surf;
    Eigen::MatrixXi m_F_surf;
    Eigen::Vector3d m_sP;

    int m_isoNumb;
    int m_isolines;
    int m_eigVecNum;
    float m_kernelW;
    double m_slice_z;
};

} // namespace hfrep
#endif
