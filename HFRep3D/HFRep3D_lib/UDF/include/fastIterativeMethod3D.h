#ifndef H_FAST_ITERATIVE_METHOD_3D
#define H_FAST_ITERATIVE_METHOD_3D

#include "timer.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <memory>
#include <list>

namespace hfrep3D {

class FIMsolver3D
{
public:
    FIMsolver3D( const int gridResX, const int gridResY, const int gridResZ);
    ~FIMsolver3D(){}

    void solveEikonalParallel_CPU( std::vector<float> *srcField, float frep_zero_bot = -0.005f, float frep_zero_up = 0.005f );
    inline std::vector<float> getDistanceFeld() { normField(); return dField; }
    inline void setNewRes( const int resX, const int resY, const int resZ ) { res_x = resX; res_y = resY; res_z = resZ; }

private:
    void initialiseGrid(std::vector<float> *srcField, float frep_zero_bot, float frep_zero_up);
    void updateSolution_parallelCPU();

    float godunovSolver( std::vector<int> *indexes, int ind, float speedFun , float h = 1.0f);
    std::vector<glm::vec2> getNeighbourValues(std::vector<int> *indexes, int ind );

    bool isNodeInList( int ind )
    {
       auto it = std::find(aList1.begin(), aList1.end(), ind);
       return it != aList1.end();
    }

    inline float calculateINF() { return std::max(res_x, res_y) + 1; }
    inline int index3d( int x, int y, int z ) { return z*res_x*res_y + y*res_x + x; }
    inline float linearExtrapolation(float fx, float fy) { return (fx+fy)/ 2.0f; }
    inline void normField(){ std::transform( dField.begin(), dField.end(), dField.begin(),
                                             std::bind2nd(std::multiplies<float>(), 10.0f/Inf)); }
    inline void offsetField() { std::transform( dField.begin(), dField.end(), dField.begin(),
                                                std::bind2nd(std::plus<float>(), 1e-36)); }
private:
    int res_x, res_y, res_z;
    float Inf, spFun;

    std::vector<float> dField; // resulting distance field

    std::list<int> aList1;
    std::shared_ptr<prof::timer> profile;
};

} //namespace hfrep3D
#endif //H_FAST_ITERATIVE_METHOD_3D
