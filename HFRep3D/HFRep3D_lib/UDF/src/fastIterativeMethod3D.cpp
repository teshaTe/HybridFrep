#include "fastIterativeMethod3D.h"
#include "timer.hpp"

#include <iostream>
#ifdef USE_OPENMP
    #include <omp.h>
#endif

namespace hfrep3D {

FIMsolver3D::FIMsolver3D(const int gridResX, const int gridResY, const int gridResZ): res_x(gridResX),
                                                                                      res_y(gridResY),
                                                                                      res_z(gridResZ),
                                                                                      Inf(calculateINF())
{
    dField.resize( static_cast<size_t>(gridResX * gridResY * gridResZ));
    profile = std::make_shared<prof::timer>();
    spFun = 1.0f;
}

void FIMsolver3D::solveEikonalParallel_CPU(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up)
{
    prof::timer time1;
    time1.Start();
    initialiseGrid( srcField, frep_zero_bot, frep_zero_up );
    time1.End("FIM3D, initialisation step: ");

    updateSolution_parallelCPU();
}

void FIMsolver3D::initialiseGrid(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up)
{
    if( srcField != nullptr )
    {
        for( int i = 0; i < res_x*res_y*res_z; i++ )
        {
            if( srcField->at(i) <= frep_zero_up && srcField->at(i) >= frep_zero_bot )
            {
                 dField[i] = 1e-36f; //defining source node in the grid, zero level-set
                 aList1.push_back(i);
            }
            else
                 dField[i] = Inf;
         }
    }
    else
    {
        std::cerr << "ERROR <FIM solver, init>: no sources specified! " << std::endl;
        return;
    }
    std::cout << "FIM3D: initial aLisit.size(): " << aList1.size() << std::endl;
}

void FIMsolver3D::updateSolution_parallelCPU()
{
    profile.get()->Start();

    while ( !aList1.empty() )
    {
        float *distBuf = dField.data();
        float p1, q1, p2, q2;
        int i, k;
        std::vector<int> nVals;

        for( auto node = aList1.begin(); node!=aList1.end(); ++node )
        {
            i  = *node;
            p1 = distBuf[i];
            q1 = godunovSolver(&nVals, i, spFun);

            if(p1 > q1)
                distBuf[i] = q1;
            else
            {
                for( size_t j = 0; j < nVals.size(); j++ )
                {
                    k = nVals[j];
                    if( distBuf[k] > distBuf[i] && !isNodeInList(k))
                    {
                        p2 = distBuf[k];
                        q2 = godunovSolver(nullptr, k, spFun);
                        if( p2 > q2 )
                        {
                            distBuf[k] = q2;
                            aList1.insert(node, k);
                        }
                    }
                }
                nVals.clear();
                node = aList1.erase(node);
                --node;
            }
            std::cout << "FIM3D: aList.size(): " << aList1.size() << std::endl;
            printf("\x1B[A");
        }
    }
    profile.get()->End("solver");
}

float FIMsolver3D::godunovSolver( std::vector<int> *indexes, int ind, float speedFun, float h)
{
    std::vector<glm::vec2> nVals = getNeighbourValues( indexes, ind );

    float a0 = std::min( nVals[0].x, nVals[0].y ); // x direction
    float b0 = std::min( nVals[1].x, nVals[1].y ); // y direction
    float c0 = std::min( nVals[2].x, nVals[2].y ); // z direction

    std::vector<float> abc; abc.resize(3);
    abc[0] = a0; abc[1] = b0; abc[2] = c0;
    std::sort(abc.begin(), abc.end());

    float c = abc[0];
    float b = abc[1];
    float a = abc[2];

    // Godunov discretisation scheme
    float u1 = c + h / speedFun;
    if( u1 <= b ) return u1;

    float u2 = 0.5f*(c + b + std::sqrt( 2.0f*h/(speedFun*speedFun) - (b - c)*(b - c)));
    if( u2 <= a ) return u2;

    float u3 = ( 2.0f*(a + b + c) + std::sqrt( 4.0f*(a + b + c)*(a + b + c) -
                                               12.0f * ( a*a + b*b + c*c - h/(speedFun*speedFun)))) / 6.0f;
    return u3;
}

std::vector<glm::vec2> FIMsolver3D::getNeighbourValues(std::vector<int> *indexes, int ind)
{
    int z = ind / (res_x*res_y), y = (ind-z*res_x*res_y)/res_x, x = (ind-z*res_x*res_y) - y*res_x;
    glm::vec2 n_x, n_y, n_z;

    if( (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == 0) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (y == 0) && (x > 0 && x < res_x-1) && (z > 0 && z < res_z-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = linearExtrapolation( dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (z == 0) && (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == 0) && (y = res_y-1) && (z > 0 && z < res_z-1) )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == res_x-1) && (y == 0) && (z > 0 && z < res_z-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        n_y.x = linearExtrapolation( dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == 0) && (y > 0 && y < res_y-1) && (z == res_z-1) )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( (x == res_x-1) && (y > 0 && y < res_y-1) && (z == 0) )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x > 0 && x < res_x-1) && (y == 0 ) && (z == res_z-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( (x > 0 && x < res_x-1) && (y == res_y-1) && (z == 0) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == res_x-1) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x-1, y, z)], dField[index3d(x,y,z)]);
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (y == res_y-1) && (x > 0 && x < res_x-1) && (z > 0 && z < res_z-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y-1, z)], dField[index3d(x, y, z)]);
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (z == res_z-1) && (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z-1)], dField[index3d(x,y,z)]);
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( (x == 0) && (y == 0) && ( z > 0 && z < res_z -1 ) )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == 0) && (z == 0) && ( y > 0 && y < res_y -1 ) )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (y == 0) && (z == 0) && ( x > 0 && x < res_x -1 ) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == res_x-1) && (y== res_y-1) && ( z > 0 && z < res_z -1 ) )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_z.x = dField[index3d(x, y, z-1)]; n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z-1)); indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( (x == res_x-1) && (z == res_z-1) && ( y > 0 && y < res_y -1 ) )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_y.x = dField[index3d(x, y-1, z)]; n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        if( indexes != nullptr )
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y-1, z)); indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( (y == res_y-1) && (z == res_z-1) && ( x > 0 && x < res_x -1 ) )
    {
        n_x.x = dField[index3d(x-1, y, z)]; n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        if( indexes != nullptr )
        {
            indexes->push_back(index3d(x-1, y, z)); indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( x == 0 && y == 0 && z == 0 )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr )
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( x == 0 && y == 0 && z == res_z - 1 )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
        if(indexes != nullptr )
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( x == 0 && y == res_y - 1 && z == 0 )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr )
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( x == res_x-1 && y == 0 && z == 0 )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        n_y.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr )
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( x == 0 && y == res_y-1 && z == res_z - 1 )
    {
        n_x.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        n_x.y = dField[index3d(x+1, y, z)];
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
        if(indexes != nullptr )
        {
            indexes->push_back(index3d(x+1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( x == res_x-1 && y == 0 && z == res_z - 1 )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        n_y.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        n_y.y = dField[index3d(x, y+1, z)];
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
        if(indexes != nullptr )
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y+1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else if( x == res_x-1 && y == res_y-1 && z == 0 )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        n_z.x = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
        n_z.y = dField[index3d(x, y, z+1)];
        if(indexes != nullptr )
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z+1));
        }
    }
    else if( x == res_x-1 && y == res_y - 1 && z == res_z -1 )
    {
        n_x.x = dField[index3d(x-1, y, z)];
        n_x.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        n_y.x = dField[index3d(x, y-1, z)];
        n_y.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        n_z.x = dField[index3d(x, y, z-1)];
        n_z.y = linearExtrapolation(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
        if(indexes != nullptr)
        {
            indexes->push_back(index3d(x-1, y, z));
            indexes->push_back(index3d(x, y-1, z));
            indexes->push_back(index3d(x, y, z-1));
        }
    }
    else
    {
        std::cerr << "ERROR <FIM3D>: cannot find neighbours! Soomething went wrong! " << std::endl;
        exit(-1);
    }

    std::vector<glm::vec2> result;
    result.push_back(n_x);
    result.push_back(n_y);
    result.push_back(n_z);
    return result;
}

}//namespace hfrep3D
