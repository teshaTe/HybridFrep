#include "FIM2D.h"

#include <iostream>

#ifdef USE_OPENMP
    #include <omp.h>
#endif

namespace hfrep2D {

FIMSolver::FIMSolver(int gridResX, int gridResY): resX(gridResX),
                                                  resY(gridResY),
                                                  Inf(calculateINF())
{
    dField.resize( static_cast<size_t>(resX*resY) );
    profile = std::make_shared<prof::timer>();
    spFun = 1.0f;
}

void FIMSolver::solveEikonalEquationCPU(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up)
{
    initialiseGrid( srcField, frep_zero_bot, frep_zero_up);
    updateSolutionCPU();
}

void FIMSolver::solveEikonalEquationParallel_CPU(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up)
{
    initialiseGrid( srcField, frep_zero_bot, frep_zero_up);
    updateSolution_parallelCPU();
}

void FIMSolver::solveEikonalEquationParallel_CPU_lockFree(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up)
{
    initialiseGrid(srcField, frep_zero_bot, frep_zero_up);
    updateSolution_parallelCPU_lockFree();
}

void FIMSolver::initialiseGrid(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up)
{
    if( srcField != nullptr )
    {
        float *dF = dField.data();
        float *sF = srcField->data();
        int res = resX*resY;

        for( int i = 0; i < res; i++ )
        {
            if( sF[i] <= frep_zero_up && sF[i] >= frep_zero_bot )
            {
                 //dField[i] = 1e-36f; //defining source node in the grid, zero level-set
                 dF[i] = 1e-36f;
                 aList1.push_back(i);
            }
            else
                 dF[i] = Inf;
         }
    }
    else
    {
        std::cerr << "ERROR <FIM solver, init>: no sources specified! " << std::endl;
        return;
    }
}

void FIMSolver::updateSolutionCPU()
{
    profile.get()->Start();
    std::cout << "FIM2D: initial aList.size(): " << aList1.size() << std::endl;

    while ( !aList1.empty() )
    {
        float *distBuf = dField.data();
        float p1, q1, p2, q2;
        int i, j, k;
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
                neighbours = getNeighbours(i);
                for( j = 0; j < neighbours.size(); j++ )
                {
                    k = neighbours[j];
                    if( distBuf[k] > p1 && !isNodeInList(k))
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
            std::cout << "FIM2D: aList.size(): " << aList1.size() << std::endl;
            printf("\x1B[A");
        }
   }
   profile.get()->End("solver");
}

void FIMSolver::updateSolution_parallelCPU()
{
    profile.get()->Start();
    std::cout << "FIM2D: initial aList.size(): " << aList1.size() << std::endl;

    while ( !aList1.empty() )
    {
        float *distBuf = dField.data();
        float p1, q1, p2, q2;
        int i, j, k;
        std::vector<int> neighbours;
        std::list<int>::iterator node;

#ifdef USE_OPENMP
    omp_lock_t lck;
    omp_init_lock(&lck);
    #pragma omp parallel shared(aList1, distBuf, lck)
    #pragma omp single
#endif
        for( node = aList1.begin(); node!=aList1.end(); ++node )
        {
#ifdef USE_OPENMP
    #pragma omp task firstprivate(node)
#endif
            i  = *node;
            p1 = distBuf[i];
            q1 = godunovSolver(i, spFun);

            if(p1 > q1)
            {
//                omp_set_lock(&lck);
                distBuf[i] = q1;
//                omp_unset_lock(&lck);
            }
            else
            {
//                omp_set_lock(&lck);
                neighbours = getNeighbours(i);
//                omp_unset_lock(&lck);
#ifdef USE_OPENMP
    //#pragma omp parallel for
#endif
//                omp_set_lock(&lck);
                for( j = 0; j < neighbours.size(); j++ )
                {
                    k = neighbours[j];

                    if( distBuf[k] > p1 && !isNodeInList(k))
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
//                omp_unset_lock(&lck);
            }
            std::cout << "FIM2D: aList.size(): " << aList1.size() << std::endl;
            printf("\x1B[A");
        }
   }
   profile.get()->End("solver");
}

void FIMSolver::updateSolution_parallelCPU_lockFree()
{

}

float FIMSolver::godunovSolver(int ind, float speedFun, float h)
{
    float solution;
    std::vector<float> vals = getMinValues(ind);
    float a = vals[0], b = vals[1];

    if( std::abs( a - b ) <= h / speedFun )
        solution = 0.5f * ( a + b + std::sqrt((2.0f*h*h / (speedFun*speedFun)) - (a - b)*(a - b)));
    else
        solution = std::min(a, b) + h/speedFun;

    return solution;
}

std::vector<int> FIMSolver::getNeighbours(int ind)
{
    std::vector<int> list;
    int y = ind / resX, x = ind - y*resX;
    int xM = x-1, xP = x+1;
    int yM = y-1, yP = y+1;

    if( (x > 0 && y > 0) && ( x < resX-1 && y < resY -1 ))
    {
            list.push_back( xM+y*resX );
            list.push_back( xP+y*resX );
            list.push_back( x+yM*resX );
            list.push_back( x+yP*resX );
    }
    else if( x == 0 && (y > 0 && y < resY-1))
    {
            list.push_back( xP );
            list.push_back( x+yM*resX );
            list.push_back( x+yP*resX );
    }
    else if( y == 0 && (x > 0 && x < resX -1))
    {
            list.push_back( xM+y*resX );
            list.push_back( xP+y*resX );
            list.push_back( x+yP*resX );
    }
    else if( x == resX-1 && (y > 0 && y < resY -1))
    {
            list.push_back( xM+y*resX );
            list.push_back( x+yM*resX );
            list.push_back( x+yM*resX );
    }
    else if( y == resY-1 && (x > 0 && x < resX -1))
    {
            list.push_back( xM+y*resX );
            list.push_back( xP+y*resX );
            list.push_back( x+yM*resX );
    }
    else if( x == 0 && y == 0 )
    {
            list.push_back( 1 );
            list.push_back( resX );
    }
    else if( x == resX-1 && y == 0 )
    {
            list.push_back( resX-2 );
            list.push_back( 2*resX-1 );
    }
    else if( x == resX-1 && y == resY -1 )
    {
            list.push_back( resX-1 + (resY-2)*resX );
            list.push_back( resX-2 + (resY-1)*resX );
    }
    else if( x == 0 && y == resY - 1 )
    {
            list.push_back( (resY-2)*resX );
            list.push_back( 1+(resY-1)*resX );
    }
    else
    {
        std::cerr << "ERROR <FIM solver, adjacent nodes choosing>: no neighbours for the point was found! " << std::endl;
        exit(-1);
    }

    return list;
}

std::vector<float> FIMSolver::getMinValues(int ind)
{
    float a, b;
    int y = ind / resX, x = ind - y*resX;

#ifdef USE_OPENMP
    #pragma omp parallel shared(dField, a, b, x, y, resX)
    {
#endif

    if( (x > 0 && y > 0) && ( x < resX-1 && y < resY -1 ))
    {
        a = std::min(dField[x-1 + y*resX], dField[x+1 + y*resX]);
        b = std::min(dField[x+(y-1)*resX], dField[x+(y+1)*resX]);
    }
    else if( x == 0 && (y > 0 && y < resY-1))
    {
        a = dField[x+1 + y*resX];
        b = std::min(dField[x+(y-1)*resX], dField[x+(y+1)*resX]);
    }
    else if( y == 0 && (x > 0 && x < resX -1))
    {
        a = std::min(dField[x-1 + y*resX], dField[x+1 + y*resX]);
        b = dField[x+(y+1)*resX];
    }
    else if( x == resX-1 && (y > 0 && y < resY -1))
    {
        a = dField[x-1 + y*resX];
        b = std::min(dField[x+(y-1)*resX], dField[x+(y+1)*resX]);
    }
    else if( y == resY-1 && (x > 0 && x < resX -1))
    {
        a = std::min(dField[x-1 + y*resX], dField[x+1 + y*resX]);
        b = dField[x+(y-1)*resX];
    }
    else if( x == 0 && y == 0 )
    {
        a = dField[x+1 + y*resX];
        b = dField[x+(y+1)*resX];
    }
    else if( x == resX-1 && y == 0 )
    {
        a = dField[x-1 + y*resX];
        b = dField[x+(y+1)*resX];
    }
    else if( x == resX-1 && y == resY -1 )
    {
        a = dField[x-1 + y*resX];
        b = dField[x+(y-1)*resX];
    }
    else if( x == 0 && y == resY - 1 )
    {
        a = dField[x+1 + y*resX];
        b = dField[x+(y-1)*resX];
    }
    else
    {
        std::cerr << "ERROR <FIM solver, adjacent nodes choosing>: no neighbours for the point was found! " << std::endl;
        exit(-1);
    }
#ifdef USE_OPENMP
    }
#endif
    std::vector<float> list;
    list.push_back(a);
    list.push_back(b);
    return list;
}

} //namespace hfrep2D
