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

//#include "activeList.h"
#include "helperFunctions.hpp"
#include "timer.hpp"

#include <vector>
#include <memory>
#include <cmath>
#include <list>
#include <iterator>
#include <algorithm>

namespace hfrep2D {

class FIMSolver
{
public:
    FIMSolver(int gridResX, int gridResY);
    ~FIMSolver(){}

    /**
     * @brief solveEikonalEquationParallel_CPU - this function implements basic FIM algorithm
     * @param inField       -
     * @param error         -
     * @param convNodeThres -
     */
    void solveEikonalEquationParallel_CPU();

    /**
     * @brief solveEikonalEquationParallel_CPU_lockFree - this function implements modified lock-free version of FIM algorithm;
     * @param inField       -
     * @param error         -
     * @param convNodeThres -
     */
    void solveEikonalEquationParallel_CPU_lockFree(const std::vector<float> inField, const float convNodeThres );

    /**
     * @brief solveEikonalEquationParallel_GPU - this function implements Group-Ordered FIM algorithm
     * @param inField
     */
    void solveEikonalEquationParallel_GPU( std::vector<float> inField );

    inline std::vector<float> getDistanceField(){ normField(); return dField; }
    inline void setNewRes(const int grX, const int grY ) { resX = grX; resY = grY; }
    inline void setSources( std::vector<float> *inField=nullptr, std::vector<Point2Di> *inPoints=nullptr )
    { srcField = inField; srcPoints = inPoints; }

private:
    //basic functions for computing FIM
    void initialiseGrid();
    void updateSolution();
    float godunovSolver(int ind, float speedFun , float h = 1.0f);
    Point2Df getGodunovABvals(int ind);

    std::vector<int> getNeighbours(int ind);

    //compute the infinite value
    inline float calculateINF() { return std::max(resX, resY) + 1; }
    inline bool isNodeConverged(float p, float q, float thres ) { return std::abs(p - q) <= thres; }

    bool isNodeInList( int ind )
    {
       auto it = std::find(aList1.begin(), aList1.end(), ind);
       return it != aList1.end();
    }

    inline void normField(){ std::transform(dField.begin(), dField.end(), dField.begin(),
                                           std::bind2nd(std::multiplies<float>(), 10.0f/Inf)); }

private:
    int resX, resY;
    float Inf, spFun;

    std::vector<float> dField; // resulting distance field
    std::vector<Point2Di> *srcPoints;
    std::vector<float> *srcField;

    //activeList aList1;
    std::list<int> aList1;
    std::shared_ptr<timer> profile;
};

} //namespace hfrep2D
#endif //define FIM_SOLVER_CLASS
