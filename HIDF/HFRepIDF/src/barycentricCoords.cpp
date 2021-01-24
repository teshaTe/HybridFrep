#include "include/barycentricCoords.h"

#include <Eigen/LU>

#include <cstdlib>
#include <ctime>
#include <iostream>

namespace idf {

Eigen::VectorXf baryCoords::meanValueCoords2D(const std::vector<Eigen::Vector2d> surfMeshPoints, const Eigen::Vector2d p)
{
    Eigen::VectorXf baryCoords;
    size_t size = surfMeshPoints.size();
    baryCoords.resize(size);
    baryCoords.setZero();

    //computing boundary coordinates
    if(computeBoundaryCoordinates2D(surfMeshPoints, p, baryCoords)) return baryCoords;

    //computing point in interior of the polygon
    eigMatrix_t s; s.resize(surfMeshPoints.size(), 2);
    eigVector_t ri; ri.resize(size);

    for(size_t i = 0; i < size; i++)
    {
        s.row(i) = (surfMeshPoints[i] - p).cast<float>();
        ri[i] = std::sqrt(s.row(i).squaredNorm());
    }

    //check boundary conditions according to the article:
    //"Mean Value Coordinates for Arbitrary Planar Polygons", Kai Hormann and Michael Floater;
    //algorithm in Figure 10, p. 12

    eigVector_t Ai; Ai.resize(size);
    eigVector_t Di; Di.resize(size);
    eigVector_t Ti; Ti.resize(size);

    for(size_t i = 0; i < size; i++)
    {
        const size_t iP = (i+1) % size;
        Ai[i] = 0.5f * (s(i, 0) * s(iP, 1) - s(iP, 0) * s(i, 1)); // square area of the triangle through vector multiplication and its determinant def
        Di[i] = s.row(i).dot(s.row(iP)); //dot product of two vectors

        //computing tan(a_i/2) = sin(a_i) / (1 + cos(a_i));
        //assert(std::abs(ri[i] * ri[iP] + Di[i]) > 0.0);
        Ti[i] = Ai[i] / (ri[i] * ri[iP] + Di[i]);
        assert(std::abs(Ai[i]) > 0.0f);
        //Ti[i] = (ri[i] * ri[iP] - Di[i]) / Ai[i];
    }

    eigVector_t w; w.resize(size);
    float W = 0.0f;

    //computing barycentric weights
    for(size_t i = 0; i < size; i++)
    {
        const size_t iM = (i + size - 1) % size;
        assert(std::abs(ri[i]) > 0.0f);
        w[i] = (Ti[iM] + Ti[i]) / ri[i];
        W += w[i];
    }

    //computing barycentric coordinates
    assert(std::abs(W) > 0.0f);
    const float invW = 1.0 / W;

    for(size_t i = 0; i < size; i++)
        baryCoords[i] = w[i] * invW; // equation 13 from the paper, p 10

    return baryCoords;
}

bool baryCoords::computeBoundaryCoordinates2D(const std::vector<Eigen::Vector2d> surfMeshPoints,
                                              const Eigen::Vector2d p, Eigen::VectorXf &baryCoords)
{
    size_t size = surfMeshPoints.size();
    baryCoords.resize(size);
    baryCoords.setZero();

    eigMatrix_t s; s.resize(size, 2);
    eigVector_t ri; ri.resize(size);

    for(size_t i = 0; i < size; i++)
    {
        s.row(i) = (surfMeshPoints[i] - p).cast<float>();
        ri[i] = std::sqrt(s.row(i).squaredNorm());
        if(std::abs(ri[i]) < eps)
        {
            baryCoords[i] = 1.0f;
            return true;
        }
    }

    eigVector_t Ai; Ai.resize(size);
    eigVector_t Di; Di.resize(size);

    for(size_t i = 0; i < size; i++)
    {
        const size_t iP   = (i + 1) % size;
        Ai[i] = 0.5f * (s(i, 0) * s(iP, 1) - s(i, 1) * s(iP, 0)); // square area of the triangle through vector multiplication and its determinant def
        Di[i] = s.row(i).dot(s.row(iP)); //s(iP, 0)*s(i, 0) + s(iP, 1)*s(i, 1); //dot product of two vectors

        if(std::abs(Ai[i]) < eps && Di[i] < 0.0f)
        {
            Eigen::Vector2f s1 = (p - surfMeshPoints[iP]).cast<float>();
            Eigen::Vector2f s2 = (surfMeshPoints[i] - surfMeshPoints[iP]).cast<float>();

            assert(std::abs(s2.squaredNorm()) > 0.0f);

            const float opScalar = s1.dot(s2);
            const float b1 = opScalar / s2.squaredNorm();

            //storing weighting coefficients for linear interpolation along polygon edges
            baryCoords[i] = b1;
            baryCoords[iP] = 1.0f - b1;
            return true;
        }
    }
    return false;
}

Eigen::VectorXf baryCoords::meanValueCoords3D(const std::vector<Eigen::Vector3d> surfMeshPoints,
                                              const Eigen::MatrixXi surfFaces, const Eigen::Vector3d p)
{
    size_t numSurfPts = surfMeshPoints.size();
    Eigen::VectorXf baryCoords(numSurfPts);
    baryCoords.setZero();

    Eigen::MatrixXd s(numSurfPts, 3);
    Eigen::VectorXd ri(numSurfPts);

    for(size_t i = 0; i < numSurfPts; i++)
    {
        s.row(i) = (surfMeshPoints[i] - p);
        ri[i] = s.row(i).norm();

        if(ri[i] < eps)
        {
             baryCoords[i] = 1.0;
             return baryCoords;
        }
        s.row(i) /= ri[i];
    }

    //cycle over all triangles to compute weights (p0,p2,p2 - points of the triangle)
    //1st: compute all angles
    for(int i = 0; i < surfFaces.rows(); i++)
    {
        int p0 = surfFaces.row(i).x(),
            p1 = surfFaces.row(i).y(),
            p2 = surfFaces.row(i).z();

        //unit vectors u0,u1,u2  obtained from matrix s
        Eigen::Vector3d edgeL = computeEdgesLength(s.row(p0), s.row(p1), s.row(p2));

        //computing angles
        double theta0 = 2.0 * std::asin(edgeL[0] / 2.0);
        double theta1 = 2.0 * std::asin(edgeL[1] / 2.0);
        double theta2 = 2.0 * std::asin(edgeL[2] / 2.0);
        double halfSum = (theta0 + theta1 + theta2) / 2.0;

        //2nd: compute border baryCoords
        if(M_PI - halfSum < eps)
        {
            // point p lies on triangle, use 2D barycentric coordinates
            baryCoords.setZero();
            baryCoords[p0] = std::sin(theta0) * ri[p1] * ri[p2];
            baryCoords[p1] = std::sin(theta1) * ri[p2] * ri[p0];
            baryCoords[p2] = std::sin(theta2) * ri[p0] * ri[p1];

            double sumW = baryCoords[p0] + baryCoords[p1] + baryCoords[p2];

            baryCoords[p0] /= sumW;
            baryCoords[p1] /= sumW;
            baryCoords[p2] /= sumW;
            return baryCoords;
        }

        //3rd: computing the rest
        // coefficient
        double sinHalfSum = std::sin(halfSum);
        double sinHalfSumSubTheta0 = std::sin(halfSum - theta0);
        double sinHalfSumSubTheta1 = std::sin(halfSum - theta1);
        double sinHalfSumSubTheta2 = std::sin(halfSum - theta2);
        double sinTheta0 = std::sin(theta0),
               sinTheta1 = std::sin(theta1),
               sinTheta2 = std::sin(theta2);

        double c0 = 2.0 * sinHalfSum * sinHalfSumSubTheta0 / (sinTheta1 * sinTheta2) - 1.0;
        double c1 = 2.0 * sinHalfSum * sinHalfSumSubTheta1 / (sinTheta2 * sinTheta0) - 1.0;
        double c2 = 2.0 * sinHalfSum * sinHalfSumSubTheta2 / (sinTheta0 * sinTheta1) - 1.0;

        if(std::abs(c0) > 1.0) c0 = c0 > 0.0 ? 1.0 : -1.0;
        if(std::abs(c1) > 1.0) c1 = c1 > 0.0 ? 1.0 : -1.0;
        if(std::abs(c2) > 1.0) c2 = c2 > 0.0 ? 1.0 : -1.0;

        //checking sign of the determinant of three unit vectors
        Eigen::Matrix3d uniMatr;
        uniMatr.row(0) = s.row(p0);
        uniMatr.row(1) = s.row(p1);
        uniMatr.row(2) = s.row(p2);
        double det = uniMatr.transpose().determinant();

        /*if(std::abs(det) < eps)
        {
            i++; continue;
        }*/

        double detSign = det > 0.0 ? 1.0 : -1.0;
        double sign0 = detSign * std::sqrt(1.0 - c0*c0);
        double sign1 = detSign * std::sqrt(1.0 - c1*c1);
        double sign2 = detSign * std::sqrt(1.0 - c2*c2);

        // if 'p' lies on the plane of current triangle but outside it, ignore the current triangle
        if(std::abs(sign0) < eps || std::abs(sign1) < eps || std::abs(sign2) < eps)
        {
            i++; continue;
        }

        // weight
        baryCoords[p0] += (theta0 - c1*theta2 - c2*theta1) / (ri[p0] * sinTheta1 * sign2); //sintheta1. sign2
        baryCoords[p1] += (theta1 - c2*theta0 - c0*theta2) / (ri[p1] * sinTheta2 * sign0); //sintheta2, sign0
        baryCoords[p2] += (theta2 - c0*theta1 - c1*theta0) / (ri[p2] * sinTheta0 * sign1); //sintheta0, sign1 */
    }

    // normalize weight
    double sumWeight = 0.0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+: sumWeight)
#endif
    for (size_t i = 0; i < numSurfPts; ++i)
        sumWeight += baryCoords[i];

    if(!sumWeight)
        std::wcerr << "WARNING: zero weights." << std::endl;

#ifdef _OPENMP
#pragma omp parallel for simd
#endif
    for (size_t i = 0; i < numSurfPts; ++i)
        baryCoords[i] /= sumWeight;

    return baryCoords;
}

Eigen::Vector3d baryCoords::computeEdgesLength(const Eigen::Vector3d u0, const Eigen::Vector3d u1, const Eigen::Vector3d u2)
{
    Eigen::Vector3d edgeL;
    //computing edge length
    edgeL[0] = (u1 - u2).norm();
    edgeL[1] = (u2 - u0).norm();
    edgeL[2] = (u0 - u1).norm();
    return edgeL;
}


}//namespace hfrep
