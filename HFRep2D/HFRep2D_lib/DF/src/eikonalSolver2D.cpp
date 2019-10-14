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

void FIMSolver::solveEikonalEquationParallel_CPU()
{
    initialiseGrid();
    updateSolution();
}

void FIMSolver::initialiseGrid()
{
    if( srcField != nullptr && srcPoints == nullptr )
    {
        for( int i = 0; i < resX*resY; i++ )
        {
            if( srcField->at(i) <= 0.00025f && srcField->at(i) >= -0.00025f )
            {
                 dField[i] = 0; //defining source node in the grid, zero level-set
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

void FIMSolver::updateSolution( )
{
    while ( !aList1.empty() )
    {
#ifdef ENABLE_OPENMP
#pragma omp parallel shared() private()
{
#pragma omp for simd schedule(static,5)
#endif
        for( std::list<int>::iterator node = aList1.begin(); node!=aList1.end(); ++node )
        {
            profile.get()->Start();

            int i = *node;
            float p1 = dField[i];
            float q1 = godunovSolver(i, spFun);

            if(p1 > q1)
                dField[i] = q1;
            else
            {
                std::vector<int> neighbours = getNeighbours(i);
                for( size_t j = 0; j < neighbours.size(); j++ )
                {
                    int k = neighbours[j];
                    if( dField[k] > dField[i] && !isNodeInList(k))
                    {
                        float p2 = dField[k];
                        float q2 = godunovSolver(k, spFun);
                        if( p2 > q2 )
                        {
                            dField[k] = q2;
                            aList1.insert(node, k);
                        }
                    }
                }
                node = aList1.erase(node);
                --node;
            }
            profile.get()->End("solver");
        }
        profile.get()->resetIter();
    }
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
