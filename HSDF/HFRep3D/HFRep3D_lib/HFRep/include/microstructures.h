#ifndef H_MICROSTRUCTURES_CLASS
#define H_MICROSTRUCTURES_CLASS

#include "frep3D.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace hfrep3D {

enum class replicFunction
{
    TriangleWave1,
    TriangleWave2,
};

enum class frepCut
{
    X,
    Y,
    Z,
    NONE
};

class microStruct
{
public:
    microStruct(int resX, int resY, int resZ);
    ~microStruct() {}

    std::vector<float> computeInfSquareSlabsX(glm::vec3 q, glm::vec3 p, glm::vec3 l);
    std::vector<float> computeInfSquareSlabsY(glm::vec3 q, glm::vec3 p, glm::vec3 l);
    std::vector<float> computeInfSquareSlabsZ(glm::vec3 q, glm::vec3 p, glm::vec3 l);

    std::vector<float> computeInfCircleEllipseSlabsX(glm::vec3 q, glm::vec3 p, glm::vec3 d);
    std::vector<float> computeInfCircleEllipseSlabsY(glm::vec3 q, glm::vec3 p, glm::vec3 d);
    std::vector<float> computeInfCircleEllipseSlabsZ(glm::vec3 q, glm::vec3 p, glm::vec3 d);

    std::vector<float> computeSphereCellStructure(glm::vec3 iMin, glm::vec3 iMax, float r, replicFunction f);
    //std::vector<float> computeTorusCellStructure(float R, float r, frepPos frPos,replicFunction f, glm::vec3 pos, glm::vec3 *q = nullptr);
    //std::vector<float> computeEllipseCellStructure(float a, float b, frepPos frPos, replicFunction f, glm::vec3 pos, glm::vec3 *q = nullptr);

    std::vector<float> calcObjWithMStruct(std::vector<float> *frepObj, std::vector<float> *m_struct,
                                          float offset, frepCut cutM, glm::f32vec3 *cutP = nullptr);

private:
    void clamp( glm::vec3 *val );
    void clamp( float *v );

    inline float triangleWaveFunc1(float t) { return 1.0f + (2.0f/M_PI)*std::asin(std::sin(M_PI*(t - 0.5f))); }
    inline float triangleWaveFunc2(float t) { return 0.5f + (1.0f/M_PI)*std::asin(std::sin(M_PI*t/2.0f)); }
    inline float sinusFunc(float t, float p, float q) { return p * std::sin(q * t); }
    inline int index3d( int x, int y, int z )         { return z*res_x*res_y + y*res_x + x; }

private:
    int res_x, res_y, res_z;
    std::shared_ptr<FRepObj3D> frep;
};

} //namespace hfrep3D
#endif // H_MICROSTRUCTURES_CLASS
