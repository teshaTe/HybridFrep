/*
 * Implementation of the FIM algorithm for solving Eikonal equaiton in three versions:
 * 1) basic FIM introduced in:         https://doi.org/10.1137/060670298
 * 2) lock-free FIM introduced in:     https://doi.org/10.1109/TPDS.2016.2567397
 * 3) Group-Ordered FIM introduced in: https://doi.org/10.1109/TPDS.2016.2567397
 *
 * @Copyright TeSha 2019
 */

#ifndef FIM_SOLVER_CLASS
#define FIM_SOLVER_CLASS

#include "activeList.h"
#include "helperFunctions.h"

#include <vector>
#include <memory>

namespace hfrep2D {

class FIMSolver
{
public:
    FIMSolver(int gridResX, int gridResY);
    ~FIMSolver(){}

    /**
     * @brief solveEikonalEquationParallel_CPU - this function implements basic FIM algorithm
     * @param inField
     */
    void solveEikonalEquationParallel_CPU(const std::vector<float> *inField, const float error, const float convNodeThres);

    /**
     * @brief solveEikonalEquationParallel_CPU_lockFree - this function implements modified lock-free version of FIM algorithm;
     * @param inField
     */
    void solveEikonalEquationParallel_CPU_lockFree( std::vector<float> inField, float error, float convNodeThres );

    /**
     * @brief solveEikonalEquationParallel_GPU - this function implements Group-Ordered FIM algorithm
     * @param inField
     */
    void solveEikonalEquationParallel_GPU( std::vector<float> inField );

    //HELPER FUNCTIONS
    inline std::vector<float> getDistanceField() { return dField; }
    inline void setNewRes(int grX, int grY ) { resX = grX; resY = grY; }

private:
    void initialiseGrid(const std::vector<float> *field );

    float godunovSolver();
    float getEdge( aNode node1, aNode node2 );

    bool isNodesAdjacent( aNode node1, aNode node2 );
    bool isNodeUpwind( aNode node1, aNode node2 );

    //compute the infinite value
    inline float calculateINF() { return std::max(resX, resY) + 1; }

private:
    int resX, resY;
    float Inf;
    std::vector<Point2D> grid;        // grid for computations
    std::vector<float> dField;        // resulting field
    std::shared_ptr<activeList> aList1; // reference tp the Active List class

};

} //namespace hfrep2D
#endif //define FIM_SOLVER_CLASS
