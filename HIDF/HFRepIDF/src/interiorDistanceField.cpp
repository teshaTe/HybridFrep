#include "include/interiorDistanceField.h"
#include "include/barycentricCoords.h"
#include "include/timer.hpp"

#include "dmaps/include/metrics.h"

#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/remove_duplicates.h>
#include <igl/triangle/triangulate.h>
#include <igl/isolines_map.h>
#include <igl/barycenter.h>
#include <igl/marching_tets.h>
#include <igl/parula.h>
#include <igl/boundary_loop.h>
#include <igl/mat_max.h>

#include <gts.h>
#include <iostream>
#include <functional>
#include <omp.h>

#ifdef USE_MATPLOTLIB
    #include <matplotlib-cpp/matplotlibcpp.h>
#endif

namespace idf {

void IDFdiffusion::computeIDF_polygon2D(const Eigen::MatrixXd &polyVerts, const Eigen::MatrixXi &meshEdges,
                                        const Eigen::Vector2d &srcP, const int eigVecNumber, double kernelBandW)
{
    assert(polyVerts.size() > 0);
    assert(meshEdges.size() > 0);

    Eigen::MatrixXd H, Vmb;
    Eigen::VectorXi Ib;
    resetParams();

    igl::triangle::triangulate(polyVerts, meshEdges, H, "a0.005q", m_Vm, m_Fm);
    igl::boundary_loop(m_Fm, Ib);

    Vmb.resize(Ib.size(), 2);
    for(int i = 0; i < Ib.size(); i++)
        Vmb.row(i) = m_Vm.row(Ib[i]);

    computeDiffusionMap(Vmb.cast<float>(), eigVecNumber, kernelBandW);
    computeInteriorDF2D(Vmb, m_Vm, srcP);
}

void IDFdiffusion::computeIDF_polygon2D(GtsIsoCartesianFunc func, const Eigen::Vector3i resGr,
                                        const Eigen::Vector2d &srcP, const int eigVecNumber, double kernelBandW,
                                        Eigen::MatrixXd H)
{
    resetParams();
    Eigen::MatrixXd Vm0, cleanVm0, Vmb;
    Eigen::MatrixXi Fm0, Em0, cleanEm0;
    Eigen::VectorXi I, Ib;

    prof::timer Timer;
    Timer.Start();
    getSurfaceComponents2D(func, resGr, 0, Vm0, Em0);
    igl::remove_duplicates(Vm0, Em0, cleanVm0, cleanEm0, I);

    igl::triangle::triangulate(cleanVm0, cleanEm0, H, "a0.005q", m_Vm, m_Fm);
    igl::boundary_loop(m_Fm, Ib);
    Timer.End("FRep triangulation: ");

    Vmb.resize(Ib.size(), 2);
    for(int i = 0; i < Ib.size(); i++)
        Vmb.row(i) = m_Vm.row(Ib[i]);

    computeDiffusionMap(Vmb.cast<float>(), eigVecNumber, kernelBandW);
    computeInteriorDF2D(Vmb, m_Vm, srcP);
}

void IDFdiffusion::findZeroLevelSet2D(const std::vector<float> func, int resX, int resY)
{
    float eps = 1000.0f * std::numeric_limits<float>::epsilon();
    std::vector<Eigen::Vector2d> pointsB, pointsIN;
    for(int y = 0; y < resY; y++)
        for(int x = 0; x< resX; x++)
        {
            if(func[x+y*resX] >= 0.0f && func[x+y*resX] <= eps)
            {
                Eigen::Vector2d pB(x*16.0f/(float)resX, y*16.0f/(float)resY);
                pointsB.push_back(pB);
            }
            else if(func[x+y*resX] >= 0.0f)
            {
                Eigen::Vector2d pIN(x*16.0f/(float)resX, y*16.0f/(float)resY);
                pointsIN.push_back(pIN);
            }
        }

    m_V.resize(pointsB.size(), 2);
    for(int i = 0; i < pointsB.size(); i++)
        m_V.row(i) = pointsB[i];

    m_Vm.resize(pointsIN.size(), 2);
    for(int i = 0; i < pointsIN.size(); i++)
        m_Vm.row(i) = pointsIN[i];
}

void IDFdiffusion::computeIDF_mesh3D(const Eigen::MatrixXd &meshVerts, const Eigen::MatrixXi &meshFaces,
                                     const Eigen::Vector3d &srcP, const int eigVecNumber, double kernelBandW)
{
    resetParams();
    igl::copyleft::tetgen::tetrahedralize(meshVerts, meshFaces, "pq1.414a0.001Y", m_TVm, m_Tm, m_TFm);
    computeDiffusionMap(meshVerts.cast<float>(), eigVecNumber, kernelBandW);
    computeInteriorDF3D(meshVerts, m_TVm, meshFaces, srcP);
}

void IDFdiffusion::computeIDF_mesh3D(GtsIsoCartesianFunc func, const Eigen::Vector3d &srcP, const Eigen::Vector3i gridRes,
                                     double iso, const int eigVecNumber, double kernelBandW)
{
    resetParams();
    Eigen::VectorXd maxV;
    Eigen::VectorXi I;

    prof::timer Time;
    Time.Start();
    getSurfaceComponents3D(func, gridRes, iso, m_V, m_F);
    igl::remove_duplicates(m_V, m_F, m_Vm, m_Fm, I);
    igl::copyleft::tetgen::tetrahedralize(m_Vm, m_Fm, "pq1.414a0.001Y", m_TVm, m_Tm, m_TFm);
    Time.End("FRep triangulation & tetrahedralisation: ");

    Time.resetIter();
    Time.Start();
    computeDiffusionMap(m_Vm.cast<float>(), eigVecNumber, kernelBandW);
    Time.End("Diffusion map comp.: ");

    Time.resetIter();
    Time.Start();
    computeInteriorDF3D(m_Vm, m_TVm, m_Fm, srcP);
    Time.End("IDF computaiton: ");
}

void IDFdiffusion::computeIDF_slice(const Eigen::MatrixXd &meshVerts, const Eigen::MatrixXi &meshFaces,
                                    const Eigen::Vector3d &srcP, const int eigVecNumber, double kernelBandW)
{
    resetParams();
    computeDiffusionMap(meshVerts.cast<float>(), eigVecNumber, kernelBandW);
    igl::copyleft::tetgen::tetrahedralize(meshVerts, meshFaces, "pq1.414a0.001Y", m_TVm, m_Tm, m_TFm);
    iterateIDF_slice(meshVerts, meshFaces, srcP);
}

void IDFdiffusion::computeIDF_slice(GtsIsoCartesianFunc func, const Eigen::Vector3d &srcP,
                                    const Eigen::Vector3i gridRes, double iso, const int eigVecNumber,
                                    double kernelBandW)
{
    resetParams();
    Eigen::VectorXi I;

    getSurfaceComponents3D(func, gridRes, iso, m_V, m_F);
    igl::remove_duplicates(m_V, m_F, m_Vm, m_Fm, I);

    computeDiffusionMap(m_Vm.cast<float>(), eigVecNumber, kernelBandW);
    igl::copyleft::tetgen::tetrahedralize(m_Vm, m_Fm, "pq1.414a0.001Y", m_TVm, m_Tm, m_TFm);

    iterateIDF_slice(m_Vm, m_Fm, srcP);
}

void IDFdiffusion::iterateIDF_slice(const Eigen::MatrixXd &surfVertices, const Eigen::MatrixXi surfFaces,
                                    const Eigen::VectorXd &srcP)
{
    m_V_surf = surfVertices;
    m_F_surf = surfFaces;
    m_sP = srcP;

    igl::opengl::glfw::Viewer viewer;
    update_visualization(viewer);
    viewer.callback_key_down = std::bind(&IDFdiffusion::key_down, this, std::placeholders::_1,
                                                 std::placeholders::_2, std::placeholders::_3);
    viewer.launch();
}

void IDFdiffusion::resetParams()
{
    m_dist.resize(0, 0);    m_dist.setZero();
    m_eigVals.resize(0);    m_eigVals.setZero();
    m_eigVecs.resize(0, 0); m_eigVecs.setZero();
    m_IDF.resize(0);        m_IDF.setZero();
    m_Vm.resize(0, 0);      m_Vm.setZero();
    m_Fm.resize(0, 0);      m_Fm.setZero();
    m_Tm.resize(0, 0);      m_Tm.setZero();
}

static void pick_first_face (GtsFace * f, GtsFace ** first)
{
  if (*first == NULL)
    *first = f;
}

//This function uses GTS for triangulating the functionally defined object with chossen level-set
void IDFdiffusion::getSurfaceComponents2D(GtsIsoCartesianFunc func, const Eigen::Vector3i gridRes,
                                               double iso, Eigen::MatrixXd &V, Eigen::MatrixXi &E)
{
    GtsCartesianGrid g;
    GtsSurface *surface;

    g.nx = gridRes.x();
    g.ny = gridRes.y();
    g.nz = gridRes.z();

    //here we specify the computational grid
    /* interval is [-10:10][-10:10][-10:10] */
    g.x = -10.0; g.dx = 20./(gdouble) (g.nx - 1);
    g.y = -10.0; g.dy = 20./(gdouble) (g.ny - 1);
    g.z = -10.0; g.dz = 20./(gdouble) (g.nz - 1);

    //then we define the new surface, in gts it is a tree structure with components: vertexes, edges, faces
    surface = gts_surface_new (gts_surface_class (),
                               gts_face_class (),
                               gts_edge_class (),
                               gts_vertex_class ());
    gts_isosurface_cartesian (surface, g, func, NULL, iso);
    gts_surface_print_stats (surface, stderr);

    //extract boundary from the 2D function
    GSList *edgesB = gts_surface_boundary(surface);

    int i = 0;
    std::vector<Eigen::Vector2i> edges;
    std::vector<Eigen::Vector2d> verts;

    //process obtained edges to get vertexes that form them
    while(edgesB)
    {
        GtsEdge *e = GTS_EDGE(edgesB->data);
        verts.push_back(Eigen::Vector2d(GTS_SEGMENT(e)->v1->p.x, GTS_SEGMENT(e)->v1->p.y));
        verts.push_back(Eigen::Vector2d(GTS_SEGMENT(e)->v2->p.x, GTS_SEGMENT(e)->v2->p.y));

        Eigen::Vector2i edge(i, i+1);
        edges.push_back(edge);
        edgesB = edgesB->next;
        i+=2;
    }

    //store vertexes and edges for further processing
    V.resize(verts.size(), 2);
    for(int i = 0; i < verts.size(); i++)
        V.row(i) = verts[i];

    E.resize(edges.size(), 2);
    for(int i = 0; i < edges.size(); i++)
        E.row(i) = edges[i];
}

void IDFdiffusion::getSurfaceComponents3D(GtsIsoCartesianFunc func, const Eigen::Vector3i gridRes,
                                               double iso, Eigen::MatrixXd &V, Eigen::MatrixXi &F)
{
    GtsCartesianGrid g;
    GtsSurface *surface;

    g.nx = gridRes.x();
    g.ny = gridRes.y();
    g.nz = gridRes.z();

    //here we specify the computational grid
    /* interval is [-10:10][-10:10][-10:10] */
    g.x = -10.0; g.dx = 20./(gdouble) (g.nx - 1);
    g.y = -10.0; g.dy = 20./(gdouble) (g.ny - 1);
    g.z = -10.0; g.dz = 20./(gdouble) (g.nz - 1);

    //then we define the new surface, in gts it is a tree structure with components: vertexes, edges, faces
    surface = gts_surface_new (gts_surface_class (),
                               gts_face_class (),
                               gts_edge_class (),
                               gts_vertex_class ());
    gts_isosurface_cartesian (surface, g, func, NULL, iso);
    gts_surface_print_stats (surface, stderr);

    //set up function for parsing the faces of the triangulated mesh
    //here we need to specify a manual static function 'pick_first_face' to pick the first face
    GtsFace *first = NULL;
    gts_surface_foreach_face (surface, (GtsFunc) pick_first_face, &first);
    GtsRange depth_range; gts_range_init (&depth_range);
    int j = 0;

    std::vector<Eigen::Vector3d> verts;
    std::vector<Eigen::Vector3i> faces;

    if (first)
    {
        //preparing to traverse the surface tree
        GtsSurfaceTraverse * t = gts_surface_traverse_new(surface, first);
        GtsFace * f;
        guint level;

        //actual surface straversing
        while ((f = gts_surface_traverse_next (t, &level)))
        {
            GtsVertex *v1, *v2, *v3;
            gts_triangle_vertices(GTS_TRIANGLE(f), &v1, &v2, &v3);

            verts.push_back(Eigen::Vector3d(v1->p.x, v1->p.y, v1->p.z));
            verts.push_back(Eigen::Vector3d(v2->p.x, v2->p.y, v2->p.z));
            verts.push_back(Eigen::Vector3d(v3->p.x, v3->p.y, v3->p.z));

            faces.push_back(Eigen::Vector3i(j, j+1, j+2));
            j += 3;
            gts_range_add_value (&depth_range, level);
        }
        gts_surface_traverse_destroy (t);
    }

    //storing vertexes and faces for further computations
    V.resize(verts.size(), 3);
    for(int i = 0; i < verts.size(); i++)
        V.row(i) = Eigen::Vector3d(verts[i].x(), verts[i].y(), verts[i].z());

    F.resize(faces.size(), 3);
    for(int i = 0; i < faces.size(); i++)
        F.row(i) = faces[i];
}

void IDFdiffusion::computeInteriorDF2D(const Eigen::MatrixXd &surfMeshV, const Eigen::MatrixXd &inVerts, const Eigen::VectorXd &srcP)
{
    std::cout << "Stage: starting computing IDF." << std::endl;

    Eigen::MatrixXf D_ij;
    /*here we compute pair-wise distances according to Rustamov et. al.
    * Interior distance using barycentric coordinates, section 4, p. 4
    * equation for diffusion distance d^2(v_i,v_j)=sum(exp(-2*l_k*t)*(phi_k(v_i) - phi_k(v_j))^2); l_k - eigen values
    * eigVals and eigVecs are obtained as a result of the diffusion map computation on the boundary of the mesh
    */
    D_ij = computePairwiseDist();

    std::vector<Eigen::Vector2d> surfMeshPoints;
    for(int i = 0; i < surfMeshV.rows(); i++)
        surfMeshPoints.push_back(surfMeshV.row(i));

    /* Computing mean-value coordinates and barycentric interpolation
     * to extend boundary distances to interior of the mesh;
     * 1st: compute them for the source point srcP;
     */
    idf::baryCoords mvc;
    Eigen::VectorXf baryW1 = mvc.meanValueCoords2D(surfMeshPoints, srcP);

    std::cout << "\nStage: starting computing mean value interpoaltion." << std::endl;
    std::cout << "Total points to process: " << inVerts.rows() << std::endl;
    prof::timer time;
    Eigen::VectorXf baryW2;
    float dSum1 = 0.0f, dSum2 = 0.0f;
    m_IDF.resize(inVerts.rows());

    /* 2nd: computing mean value coords for the rest interior points and points along the boundary
     * Here we use equation from Rustamov et. al. Interior distance using barycentric coordinates,
     * section 5, equation (5), p. 5
     */
    for(int l = 0; l < inVerts.rows(); l++)
    {
        time.Start();
        baryW2 = mvc.meanValueCoords2D(surfMeshPoints, inVerts.row(l));

#ifdef _OPENMP
#pragma omp parallel for reduction(+:dSum1, dSum2) shared(baryW1, D_ij, baryW2) schedule(static)
#endif
        for(int i = 0; i < D_ij.rows(); i++)
            for(int j = 0; j < D_ij.cols(); j++)
            {
                dSum1 += D_ij(i, j) * baryW1[i] * baryW2[j];
                dSum2 += D_ij(i, j) * (baryW1[i] * baryW1[j] + baryW2[i] * baryW2[j]);
            }
        m_IDF[l] = std::sqrt(dSum1 - 0.5f * dSum2);
        dSum1 = dSum2 = 0.0f;
        time.End("1 point: ");
    }
    std::cout << "\nStage: finished.\n" << std::endl;
}

void IDFdiffusion::computeInteriorDF3D(const Eigen::MatrixXd &surfMeshV, const Eigen::MatrixXd &inVerts,
                                       const Eigen::MatrixXi surfFaces,  const Eigen::VectorXd &srcP)
{
    std::cout << "Stage: starting computing IDF." << std::endl;

    Eigen::MatrixXf D_ij;
    /*here we compute pair-wise distances according to Rustamov et. al.
    * Interior distance using barycentric coordinates, section 4, p. 4
    * equation for diffusion distance d^2(v_i,v_j)=sum(exp(-2*l_k*t)*(phi_k(v_i) - phi_k(v_j))^2); l_k - eigen values
    * eigVals and eigVecs are obtained as a result of the diffusion map computation on the boundary of the mesh
    */
    D_ij = computePairwiseDist();

    std::vector<Eigen::Vector3d> surfMeshPoints;
    for(int i = 0; i < surfMeshV.rows(); i++)
        surfMeshPoints.push_back(surfMeshV.row(i));

    m_IDF.resize(inVerts.rows());

    /* Computing mean-value coordinates and barycentric interpolation
     * to extend boundary distances to interior of the mesh;
     * 1st: compute them for the source point srcP;
     */
    idf::baryCoords mvc;
    Eigen::VectorXf baryW1 = mvc.meanValueCoords3D(surfMeshPoints, surfFaces, srcP);

    std::cout << "\nStage: starting computing mean value interpoaltion." << std::endl;
    std::cout << "Total points to process: " << inVerts.rows() << std::endl;

    prof::timer time;
    Eigen::VectorXf baryW2;
    float dSum1 = 0.0f, dSum2 = 0.0f;

    /* 2nd: computing mean value coords for the rest interior points and points along the boundary
     * Here we use equation from Rustamov et. al. Interior distance using barycentric coordinates,
     * section 5, equation (5), p. 5
     */
    for(int l = 0; l < inVerts.rows(); l++)
    {
        time.Start();
        baryW2 = mvc.meanValueCoords3D(surfMeshPoints, surfFaces, inVerts.row(l));
#ifdef _OPENMP
#pragma omp parallel for reduction(+:dSum1, dSum2) shared(baryW1, D_ij, baryW2) schedule(static)
#endif
        for(int i = 0; i < D_ij.rows(); i++)
            for(int j = 0; j < D_ij.cols(); j++)
            {
                dSum1 += D_ij(i, j) * baryW1[i] * baryW2[j];
                dSum2 += D_ij(i, j) * (baryW1[i] * baryW1[j] + baryW2[i] * baryW2[j]);
            }
        m_IDF[l] = std::sqrt(dSum1 - 0.5f * dSum2);
        dSum1 = dSum2 = 0.0f;

        time.End("1 point: ");
    }
    std::cout << "\nStage: finished. \n" << std::endl;
}

void IDFdiffusion::computeDiffusionMap(const dmaps::matrix_t &inPoints, const int eigVecNum, double kernelBandWidth)
{
    std::cout << "Stage: starting computing diffusion map." << std::endl;

    //computing distances
#ifdef _OPENMP
    int num_threads = omp_get_num_threads();
#else
    int num_threads = 1;
#endif

    dmaps::distance_matrix dMatr(inPoints, num_threads);
    auto metrics = std::bind(&dmaps::euclidean, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    dMatr.compute(metrics);
    m_dist = dMatr.get_distances();

    //computing diffusion map: eigen values and eigen functions of the Laplace-Beltrammi operator
    dmaps::vector_t weights;
    dmaps::diffusion_map diffMap(m_dist, weights, num_threads);
    diffMap.set_kernel_bandwidth(kernelBandWidth);
    diffMap.compute(eigVecNum, 1.0, 0.0);

    //change the order of the stored values to non-decreasing for both eigen vectors and eigen values
    dmaps::matrix_t eigVecsTmp = diffMap.get_eigenvectors();
    m_eigVals = diffMap.get_eigenvalues().reverse().eval();
    m_eigVecs = eigVecsTmp.rowwise().reverse().eval();

    std::cout << "Stage: finished.\n" << std::endl;
}

Eigen::MatrixXf IDFdiffusion::computePairwiseDist()
{
    //computing distances on the boundary of the mesh/polygon using diffusion map;
    Eigen::MatrixXf D_ij; D_ij.resize(m_eigVecs.rows(), m_eigVecs.rows());

    float t =1.0f/ (8.0f * m_eigVals[1]);
    float dSq = 0.0f;
    float eps = 1e-6;

    /*here we compute pair-wise distances according to Rustamov et. al.
    * Interior distance using barycentric coordinates, section 4, p. 4
    * equation for diffusion distance d^2(v_i,v_j)=sum(exp(-2*l_k*t)*(phi_k(v_i) - phi_k(v_j))^2); l_k - eigen values
    * eigVals and eigVecs are obtained as a result of the diffusion map computation on the boundary of the mesh
    */
    std::cout << "Stage: starting computing diffusion distances on the boundary." << std::endl;
    for(int i = 0; i < m_eigVecs.rows(); i++)
        for(int j = 0; j < m_eigVecs.rows(); j++)
        {
            for(int k = 0; k < m_eigVecs.cols(); k++)
            {
                if(std::exp(-m_eigVals[k]*t) > eps)
                {
                    float eigDiff = m_eigVecs(i, k) - m_eigVecs(j, k);
                    dSq += std::exp(-2.0f * t * m_eigVals[k]) * eigDiff * eigDiff;
                }
            }
            D_ij(i, j) = dSq;
            dSq = 0.0f;
        }
    std::cout << "Stage: finished.\n" << std::endl;
    return D_ij;
}

void IDFdiffusion::plotIDF2D(int isoNum)
{
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(m_Vm, m_Fm);
    viewer.data().set_data(m_IDF.cast<double>());

    //m_isolines = isoNum;
    //setColormap(viewer, m_isolines);
    Eigen::MatrixXd CM;
    igl::parula(Eigen::VectorXd::LinSpaced(isoNum, 0, 1).eval(), false, CM);
    igl::isolines_map(Eigen::MatrixXd(CM),CM);
    viewer.data().set_colormap(CM);
    viewer.data().show_lines = false;
    viewer.launch();
}

void IDFdiffusion::plotIDF3D(int isoNum)
{
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(m_TVm, m_TFm);
    viewer.data().set_data(m_IDF.cast<double>());
    Eigen::MatrixXd CM;
    igl::parula(Eigen::VectorXd::LinSpaced(isoNum, 0, 1).eval(), false, CM);
    igl::isolines_map(Eigen::MatrixXd(CM),CM);
    viewer.data().set_colormap(CM);
    viewer.data().show_faces = true;
    viewer.launch();
}

#ifdef USE_MATPLOTLIB
void IDFdiffusion::plotDiffusionMap()
{
    std::vector<float> vecXn, vecYn;
    int iCol = m_eigVecs.cols()-1;
    for(int i = 0; i < m_eigVecs.rows(); i++)
    {
        vecXn.push_back(m_eigVecs(i, iCol-1) / m_eigVecs(i, iCol));
        vecYn.push_back(m_eigVecs(i, iCol-2) / m_eigVecs(i, iCol));
    }
    matplotlibcpp::scatter(vecXn, vecYn);
    matplotlibcpp::show();
}
#endif

void IDFdiffusion::update_visualization(igl::opengl::glfw::Viewer &viewer)
{
    Eigen::Vector4d plane(0, 0, 1,-((1 - m_slice_z) * m_TVm.col(2).minCoeff() +
                                         m_slice_z  * m_TVm.col(2).maxCoeff()));
    Eigen::MatrixXd V_vis;
    Eigen::MatrixXi F_vis;

    Eigen::VectorXi J;
    {
        Eigen::SparseMatrix<double> bary;
        const Eigen::VectorXd IV = (m_TVm.col(0)*plane(0) +
                                    m_TVm.col(1)*plane(1) +
                                    m_TVm.col(2)*plane(2)).array() + plane(3);

        igl::marching_tets(m_TVm, m_Tm, IV, V_vis, F_vis, J, bary);
    }
    computeInteriorDF3D(m_V_surf, V_vis, m_F_surf, m_sP);

    viewer.data().clear();
    viewer.data().set_mesh(V_vis, F_vis);
    viewer.data().set_data(m_IDF.cast<double>());

    Eigen::MatrixXd CM;
    igl::parula(Eigen::VectorXd::LinSpaced(m_isolines, 0, 1).eval(), false, CM);
    igl::isolines_map(Eigen::MatrixXd(CM), CM);
    viewer.data().set_colormap(CM);
    viewer.data().set_face_based(true);
}

bool IDFdiffusion::key_down(igl::opengl::glfw::Viewer &viewer, unsigned char key, int modifier)
{
    switch(key)
      {
        default:
          return false;
        case '.':
          m_slice_z = std::min(m_slice_z + 0.01, 0.99);
        break;
        case ',':
          m_slice_z = std::max(m_slice_z - 0.01, 0.01);
        break;
      }
      update_visualization(viewer);
      return true;
}

} // namespace hfrep
