#include "eikonalSolver2D.h"

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
    profile = std::make_shared<timer>();
    // for now temp solution:
    spFun = 1.0f;
}

void FIMSolver::solveEikonalEquationParallel_CPU(float frep_zero_bot, float frep_zero_up)
{
    initialiseGrid(frep_zero_bot, frep_zero_up);
    updateSolution_parallelCPU();
}

void FIMSolver::solveEikonalEquationParallel_CPU_lockFree(float frep_zero_bot, float frep_zero_up)
{
    initialiseGrid(frep_zero_bot, frep_zero_up);
    updateSolution_parallelCPU_lockFree();
}

void FIMSolver::initialiseGrid(float frep_zero_bot, float frep_zero_up)
{
    if( srcField != nullptr && srcPoints == nullptr )
    {
        for( int i = 0; i < resX*resY; i++ )
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
    else if( srcPoints != nullptr && srcField == nullptr )
    {
        for( size_t i = 0; i < resX*resY; i++ )
        {
            dField[i] = Inf;
        }

        for( size_t i = 0; i < srcPoints->size(); i++)
        {
            int x = srcPoints->at(i).dx, y = srcPoints->at(i).dy;
            dField[x+y*resX] = 0.0f; //defining source node in the grid, zero level-set
            aList1.push_back(x+y*resX);
        }
    }
    else
    {
        std::cerr << "ERROR <FIM solver, init>: no sorces specified! " << std::endl;
        return;
    }
}

void FIMSolver::updateSolution_parallelCPU()
{
    profile.get()->Start();

    while ( !aList1.empty() )
    {
        float *distBuf = dField.data();
        float p1, q1, p2, q2;
        int i, j, k;

#ifdef ENABLE_OPENMP
    #pragma omp parallel //shared(distBuf, spFun) private(i,j,k,p1,p2,q1,q2)
    #pragma omp single
{
#endif
        for( auto node = aList1.begin(); node!=aList1.end(); ++node )
        {
#ifdef ENABLE_OPENMP
    #pragma omp task firstprivate(node)
#endif
            i  = *node;
            p1 = distBuf[i];
            q1 = godunovSolver(i, spFun);

            if(p1 > q1)
                distBuf[i] = q1;
            else
            {
                std::vector<int> neighbours = getNeighbours(i);
                for( j = 0; j < neighbours.size(); j++ )
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
        }
#ifdef ENABLE_OPENMP
#pragma omp taskwait
}
#endif
   }
   profile.get()->End("solver");
}

void FIMSolver::updateSolution_parallelCPU_lockFree()
{

}

float FIMSolver::godunovSolver(int ind, float speedFun, float h)
{
    float solution;
    Point2Df ab = getGodunovABvals( ind );

    if( std::abs( ab.dx - ab.dy ) <= h / speedFun )
        solution = 0.5f * ( ab.dx + ab.dy + std::sqrt( (2.0f*h*h / (speedFun*speedFun)) - (ab.dx - ab.dy)*(ab.dx - ab.dy)));
    else
        solution = std::min( ab.dx, ab.dy ) + h/speedFun;

    return solution;
}

Point2Df FIMSolver::getGodunovABvals(int ind)
{
    float a,b;
    int y = ind/resX, x = ind - y*resX;
    if( (x > 0 && x < resX-1) && (y > 0 && y < resY-1))
    {
        a = std::min( dField[ind-1],   dField[ind+1]);
        b = std::min( dField[ind-resX], dField[ind+resX]);
    }
    else if( y == 0 && ( x > 0 && x < resX-1 ))
    {
        a = std::min( dField[ind-1], dField[ind+1] );
        b = dField[ind+resX];
    }
    else if( x == 0 && ( y > 0 && y < resY-1 ))
    {
        a = dField[ind+1];
        b = std::min( dField[ind-resX], dField[ind+resX]);
    }
    else if( x == resX-1 && ( y > 0 && y < resY-1 ))
    {
        a = dField[ ind-1];
        b = std::min( dField[ind-resX ], dField[ind+resX] );
    }
    else if( y == resY-1 && ( x > 0 && x < resX -1 ) )
    {
        a = std::min( dField[ind-1], dField[ind+1] );
        b = dField[ind-resX];
    }
    else if( x == 0 && y == 0 )
    {
        a = dField[ind+1];
        b = dField[ind+resX];
    }
    else if( x == resX-1 && y == 0 )
    {
        a = dField[ind-1];
        b = dField[ind+resX];
    }
    else if( x == resX-1 && y == resY-1 )
    {
        a = dField[ind-1];
        b = dField[ind-resX];
    }
    else if( x == 0 && y == resY-1 )
    {
        a = dField[ind+1];
        b = dField[ind-resX];
    }
    else
    {
        std::cerr << "ERROR[FIM solver, choosing a & b]: something goes wrong with choosing 'a' and 'b'! " << std::endl;
        exit(-1);
    }
    return Point2Df(a, b);
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

} //namespace hfrep2D
