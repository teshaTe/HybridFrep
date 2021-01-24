#include <iostream>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

#include "dmaps/include/diffusion_map.h"
#include "dmaps/include/distance_matrix.h"
#include "dmaps/include/metrics.h"
#include <matplotlib-cpp/matplotlibcpp.h>

#include "timer.hpp"

#include <random>
#include <chrono>

using namespace std::placeholders;
namespace plt = matplotlibcpp;

std::vector<float> sdfSphere(glm::vec3 cent, float r, glm::vec3 res, int step)
{
    std::vector<float> sdf;
    for(int z =0; z < res.z; z += step)
        for(int y = 0; y < res.y; y += step)
            for(int x = 0; x <res.x; x += step)
            {
                float shX = (static_cast<float>(x)/res.x - cent.x/res.x);
                float shY = (static_cast<float>(y)/res.y - cent.y/res.y);
                float shZ = (static_cast<float>(z)/res.z - cent.z/res.z);
                sdf.push_back( r/res.x - std::sqrt(shX*shX + shY*shY + shZ*shZ));
            }
    return sdf;
}

int main(int argc, char *argv[])
{
    prof::timer profiler;
    profiler.Start();

    int resX = 256, resY = 256, resZ = 256;
    glm::vec3 cen(resX/2.0f, resY/2.0f, resZ/2.0f);
    float r = 100.0f;

    //std::vector<float> sdf = sdfSphere(cen, r, glm::vec3(resX, resY, resZ), 1);
    /*dmaps::matrix_t matr(resX, resY);
    for(int y = 0; y< resY; y++)
        for(int x =0; x < resX; x++)
            matr(x,y) = sdf[resX/2*resX*resY + y*resX + x];*/
    float length_phi = 12.0f;
    float length_z = 12.0f;
    float sigma = 0.1f;
    int seed = 5000;

    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);

    std::vector<float> X, Y, Z;
    X.resize(seed); Y.resize(seed);  Z.resize(seed);

    for (int i = 0; i < seed; i++)
    {
        float xi = unif(rng);
        float phi = length_phi*unif(rng);
        Z[i] = length_z * unif(rng);
        X[i] = (1.0f/6.0f) * (phi + sigma*xi)* std::sin(phi);
        Y[i] = (1.0f/6.0f) * (phi + sigma*xi)* std::cos(phi);
    }

    dmaps::matrix_t matr(3, seed), matrTr;
    for(int y = 0; y < seed; y++)
    {
        matr(0,y) = X[y];
        matr(1,y) = Y[y];
        matr(2,y) = Z[y];
    }
    matrTr = matr.transpose();

    #ifdef _OPENMP
        int num_threads = omp_get_thread_num();
    #else
        int num_threads = 1;
    #endif
    dmaps::distance_matrix distM(matrTr, num_threads);
    auto metrics = std::bind(&dmaps::euclidean, _1, _2, _3);
    distM.compute(metrics);
    dmaps::matrix_t dist = distM.get_distances();

    dmaps::vector_t weights;
    dmaps::diffusion_map difMap(dist, weights, num_threads);
    difMap.set_kernel_bandwidth(0.1);
    difMap.compute( 6, 1.0, 0.0);

    dmaps::matrix_t v = difMap.get_eigenvectors();
    dmaps::vector_t w = difMap.get_eigenvalues();

    std::vector<float> vecX, vecY;
    for(int y = 0; y < seed; y++)
    {
        vecX.push_back(v(y, 1) / v(y, 0));
        vecY.push_back(v(y, 2) / v(y, 0));
    }

    profiler.End("Total time:");
    /*std::vector<float> vecX, vecY;
    for(int x = 0; x < 5000; x++)
    {
        vecX.push_back(matrTr(x, 0));
        vecY.push_back(matrTr(x, 1));
    }*/

    plt::scatter(vecX, vecY);
    plt::show();

    return 0;
}
