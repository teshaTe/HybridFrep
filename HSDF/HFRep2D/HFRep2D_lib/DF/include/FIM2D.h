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

#include "timer.hpp"

#include <glm/glm.hpp>

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
     * @brief solveEikonalEquationCPU()/solveEikonalEquationParallel_CPU() - function for slving eikonal equaiton on regular grid
     * @param frep_zero_bot - paramter defining the smallest value of the inteval in which the zero level set of the object is
     * @param frep_zero_up  - paramter defining the biggest value of the inteval in which the zero level set of the object is
     */
    void solveEikonalEquationCPU(std::vector<float> *srcField, float frep_zero_bot=-0.005f, float frep_zero_up=0.005f);
    void solveEikonalEquationParallel_CPU(std::vector<float> *srcField, float frep_zero_bot=-0.005f, float frep_zero_up=0.005f);

    /**
     * @brief solveEikonalEquationParallel_CPU_lockFree - this function implements modified lock-free version of FIM algorithm;
     * @param inField       -
     * @param error         -
     * @param convNodeThres -
     */
    void solveEikonalEquationParallel_CPU_lockFree(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up);

    inline std::vector<float> getDistanceField(){ normField(); return dField; }
    inline void setNewRes(const int grX, const int grY ) { resX = grX; resY = grY; }

private:
    //basic functions for computing FIM
    void initialiseGrid(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up);
    void updateSolutionCPU();
    void updateSolution_parallelCPU();
    void updateSolution_parallelCPU_lockFree();

    float godunovSolver(int ind, float speedFun , float h = 1.0f);
    std::vector<int> getNeighbours(int ind);

    std::vector<float> getMinValues(int ind );

    //compute the infinite value
    inline float calculateINF() { return std::max(resX, resY) + 1; }

    bool isNodeInList( int ind )
    {
       auto it = std::find(aList1.begin(), aList1.end(), ind);
       return it != aList1.end();
    }

    inline void normField(){ std::transform( dField.begin(), dField.end(), dField.begin(),
                                             std::bind2nd(std::multiplies<float>(), 10.0f/Inf)); }
    inline void offsetField() { std::transform( dField.begin(), dField.end(), dField.begin(),
                                                std::bind2nd(std::plus<float>(), 1e-36)); }
    inline int index2D(int x, int y) { return x+y*resX; }

private:
    int resX, resY;
    float Inf, spFun;

    std::vector<float> dField; // resulting distance field

    std::list<int> aList1;
    std::shared_ptr<prof::timer> profile;
};

} //namespace hfrep2D
#endif //define FIM_SOLVER_CLASS
