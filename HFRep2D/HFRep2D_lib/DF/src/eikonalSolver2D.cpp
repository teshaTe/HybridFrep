#include "eikonalSolver2D.h"

#ifdef USE_OPENMP
    #include <omp.h>
#endif

namespace hfrep2D {

FIMSolver::FIMSolver(int gridResX, int gridResY):resX(gridResX), resY(gridResY),
                                                 Inf(calculateINF())
{
    grid.resize( static_cast<size_t>(resX * resY) );
    aList1 = std::shared_ptr<activeList>();
}

void FIMSolver::solveEikonalEquationParallel_CPU(const std::vector<float> *inField, const float error, const float convNodeThres)
{
    initialiseGrid( inField );
}

void FIMSolver::initialiseGrid(const std::vector<float> *field)
{

}





} //namespace hfrep2D
