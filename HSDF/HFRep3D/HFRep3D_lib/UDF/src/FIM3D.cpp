#include "FIM3D.h"
#include "HelperFunctions/timer.hpp"

#include <iostream>
#ifdef _OPENMP
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
    std::cout << "\nStage FIM3D computation." << std::endl;
    prof::timer time1;
    time1.Start();
    initialiseGrid( srcField, frep_zero_bot, frep_zero_up );
    updateSolutionCPU();
    time1.End("\nFIM3D, total time: ");
    std::cout << "FIM3D stage complete." << std::endl;

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

void FIMsolver3D::updateSolutionCPU()
{
    profile.get()->Start();

    while ( !aList1.empty() )
    {
        float *distBuf = dField.data();
        float p1, q1, p2, q2;
        int i, k;
        std::vector<int> neighbours;

        for( auto node = aList1.begin(); node!=aList1.end(); ++node )
        {
            i  = *node;
            p1 = distBuf[i];
            q1 = godunovSolver(i, spFun);

            if(p1 > q1)
                distBuf[i] = q1;
            else
            {
                neighbours = getNeightboursIdexes(i);
                for( size_t j = 0; j < neighbours.size(); j++ )
                {
                    k = neighbours[j];
                    if( distBuf[k] > distBuf[i] && !isNodeInList(k))
                    {
                        p2 = distBuf[k];
                        q2 = godunovSolver(k, spFun);
                        if( p2 > q2 )
                        {
                            distBuf[k] = q2;
                            aList1.insert(node, k);
                        }
                    }
                }
                node = aList1.erase(node);
                --node;
            }
            std::cout << "FIM3D: aList.size(): " << aList1.size() << std::endl;
            printf("\x1B[A");
        }
    }
    profile.get()->End("solver");
}

float FIMsolver3D::godunovSolver(int curInd, float speedFun, float h)
{
    std::vector<float> vals = getMinValues(curInd);
    std::sort(vals.begin(), vals.end());

    float c = vals[0];
    float b = vals[1];
    float a = vals[2];

    // Godunov discretisation scheme
    float u1 = c + h / speedFun;
    if( u1 <= b ) return u1;

    float u2 = 0.5f*(c + b + std::sqrt( 2.0f*h*h/(speedFun*speedFun) - (b - c)*(b - c)));
    if( u2 <= a ) return u2;

    float u3 = ( 2.0f*(a + b + c) + std::sqrt( 4.0f * (a + b + c)*(a + b + c)/(h*h) -
                                               12.0f * ( (a*a + b*b + c*c)/(h*h) -
                                                         1.0f/(speedFun*speedFun)))) / 6.0f;
    return u3;
}

std::vector<float> FIMsolver3D::getMinValues(int curInd)
{
    int z = curInd / (res_x*res_y), y = (curInd-z*res_x*res_y)/res_x, x = (curInd-z*res_x*res_y) - y*res_x;
    float a,b,c;

    if( (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == 0) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (y == 0) && (x > 0 && x < res_x-1) && (z > 0 && z < res_z-1) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (z == 0) && (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == 0) && (y == res_y-1) && (z > 0 && z < res_z-1) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == res_x-1) && (y == 0) && (z > 0 && z < res_z-1) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == 0) && (y > 0 && y < res_y-1) && (z == res_z-1) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( (x == res_x-1) && (y > 0 && y < res_y-1) && (z == 0) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( (x > 0 && x < res_x-1) && (y == 0 ) && (z == res_z-1) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( (x > 0 && x < res_x-1) && (y == res_y-1) && (z == 0) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == res_x-1) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (y == res_y-1) && (x > 0 && x < res_x-1) && (z > 0 && z < res_z-1) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (z == res_z-1) && (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( (x == 0) && (y == 0) && ( z > 0 && z < res_z -1 ) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == 0) && (z == 0) && ( y > 0 && y < res_y -1 ) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( (y == 0) && (z == 0) && ( x > 0 && x < res_x -1 ) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == res_x-1) && (y == res_y-1) && ( z > 0 && z < res_z -1 ) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z-1)], dField[index3d(x, y, z+1)]);
    }
    else if( (x == res_x-1) && (z == res_z-1) && ( y > 0 && y < res_y -1 ) )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y-1, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( (y == res_y-1) && (z == res_z-1) && ( x > 0 && x < res_x -1 ) )
    {
        a = std::min(dField[index3d(x-1, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( x == 0 && y == 0 && z == 0 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( x == 0 && y == 0 && z == res_z - 1 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( x == 0 && y == res_y - 1 && z == 0 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( x == res_x-1 && y == 0 && z == 0 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( x == 0 && y == res_y-1 && z == res_z - 1 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x+1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( x == res_x-1 && y == 0 && z == res_z - 1 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y+1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else if( x == res_x-1 && y == res_y-1 && z == 0 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z+1)]);
    }
    else if( x == res_x-1 && y == res_y - 1 && z == res_z -1 )
    {
        a = std::min(dField[index3d(x, y, z)], dField[index3d(x-1, y, z)]);
        b = std::min(dField[index3d(x, y, z)], dField[index3d(x, y-1, z)]);
        c = std::min(dField[index3d(x, y, z)], dField[index3d(x, y, z-1)]);
    }
    else
    {
        std::cerr << "ERROR <FIM3D>: cannot find neighbours! Soomething went wrong! " << std::endl;
        exit(-1);
    }

    std::vector<float> result; result.resize(3);
    result[0] = a;
    result[1] = b;
    result[2] = c;
    return result;
}

std::vector<int> FIMsolver3D::getNeightboursIdexes(int curInd)
{
    int z = curInd / (res_x*res_y), y = (curInd-z*res_x*res_y)/res_x, x = (curInd-z*res_x*res_y) - y*res_x;
    std::vector<int> indexes;
    if( (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        indexes.resize(6);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y-1, z)); indexes[3] = (index3d(x, y+1, z));
        indexes[4] = (index3d(x, y, z-1)); indexes[5] = (index3d(x, y, z+1));
    }
    else if( (x == 0) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        indexes.resize(5);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y-1, z)); indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z-1)); indexes[4] = (index3d(x, y, z+1));
    }
    else if( (y == 0) && (x > 0 && x < res_x-1) && (z > 0 && z < res_z-1) )
    {
        indexes.resize(5);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z-1)); indexes[4] = (index3d(x, y, z+1));
    }
    else if( (z == 0) && (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) )
    {
        indexes.resize(5);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y-1, z)); indexes[3] = (index3d(x, y+1, z));
        indexes[4] = (index3d(x, y, z+1));
    }
    else if( (x == 0) && (y == res_y-1) && (z > 0 && z < res_z-1) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y-1, z));
        indexes[2] = (index3d(x, y, z-1)); indexes[3] = (index3d(x, y, z+1));
    }
    else if( (x == res_x-1) && (y == 0) && (z > 0 && z < res_z-1) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y+1, z));
        indexes[2] = (index3d(x, y, z-1)); indexes[3] = (index3d(x, y, z+1));
    }
    else if( (x == 0) && (y > 0 && y < res_y-1) && (z == res_z-1) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y-1, z)); indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z-1));
    }
    else if( (x == res_x-1) && (y > 0 && y < res_y-1) && (z == 0) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y-1, z)); indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z+1));
    }
    else if( (x > 0 && x < res_x-1) && (y == 0 ) && (z == res_z-1) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z-1));
    }
    else if( (x > 0 && x < res_x-1) && (y == res_y-1) && (z == 0) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y-1, z));
        indexes[3] = (index3d(x, y, z+1));
    }
    else if( (x == res_x-1) && (y > 0 && y < res_y-1) && (z > 0 && z < res_z-1) )
    {
        indexes.resize(5);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y-1, z)); indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z-1)); indexes[4] = (index3d(x, y, z+1));
    }
    else if( (y == res_y-1) && (x > 0 && x < res_x-1) && (z > 0 && z < res_z-1) )
    {
        indexes.resize(5);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y-1, z));
        indexes[3] = (index3d(x, y, z-1)); indexes[4] = (index3d(x, y, z+1));
    }
    else if( (z == res_z-1) && (x > 0 && x < res_x-1) && (y > 0 && y < res_y-1) )
    {
        indexes.resize(5);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y-1, z)); indexes[3] = (index3d(x, y+1, z));
        indexes[4] = (index3d(x, y, z-1));
    }
    else if( (x == 0) && (y == 0) && ( z > 0 && z < res_z -1 ) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y+1, z));
        indexes[2] = (index3d(x, y, z-1)); indexes[3] = (index3d(x, y, z+1));
    }
    else if( (x == 0) && (z == 0) && ( y > 0 && y < res_y -1 ) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y-1, z)); indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z+1));
    }
    else if( (y == 0) && (z == 0) && ( x > 0 && x < res_x -1 ) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z+1));
    }
    else if( (x == res_x-1) && (y == res_y-1) && ( z > 0 && z < res_z -1 ) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y-1, z));
        indexes[2] = (index3d(x, y, z-1)); indexes[3] = (index3d(x, y, z+1));
    }
    else if( (x == res_x-1) && (z == res_z-1) && ( y > 0 && y < res_y -1 ) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y-1, z)); indexes[2] = (index3d(x, y+1, z));
        indexes[3] = (index3d(x, y, z-1));
    }
    else if( (y == res_y-1) && (z == res_z-1) && ( x > 0 && x < res_x -1 ) )
    {
        indexes.resize(4);
        indexes[0] = (index3d(x-1, y, z)); indexes[1] = (index3d(x+1, y, z));
        indexes[2] = (index3d(x, y-1, z));
        indexes[3] = (index3d(x, y, z-1));
    }
    else if( x == 0 && y == 0 && z == 0 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y+1, z));
        indexes[2] = (index3d(x, y, z+1));
    }
    else if( x == 0 && y == 0 && z == res_z - 1 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y+1, z));
        indexes[2] = (index3d(x, y, z-1));
    }
    else if( x == 0 && y == res_y - 1 && z == 0 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y-1, z));
        indexes[2] = (index3d(x, y, z+1));
    }
    else if( x == res_x-1 && y == 0 && z == 0 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y+1, z));
        indexes[2] = (index3d(x, y, z+1));
    }
    else if( x == 0 && y == res_y-1 && z == res_z - 1 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x+1, y, z));
        indexes[1] = (index3d(x, y-1, z));
        indexes[2] = (index3d(x, y, z-1));
    }
    else if( x == res_x-1 && y == 0 && z == res_z - 1 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y+1, z));
        indexes[2] = (index3d(x, y, z-1));
    }
    else if( x == res_x-1 && y == res_y-1 && z == 0 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y-1, z));
        indexes[2] = (index3d(x, y, z+1));
    }
    else if( x == res_x-1 && y == res_y - 1 && z == res_z -1 )
    {
        indexes.resize(3);
        indexes[0] = (index3d(x-1, y, z));
        indexes[1] = (index3d(x, y-1, z));
        indexes[2] = (index3d(x, y, z-1));
    }
    else
    {
        std::cerr << "ERROR <FIM3D>: cannot find neighbours! Soomething went wrong! " << std::endl;
        exit(-1);
    }

    return indexes;
}

}//namespace hfrep3D
