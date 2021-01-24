#ifndef H_HFREP_CLASS
#define H_HFREP_CLASS

#include <vector>
#include <memory>

#include "timer.hpp"
#include "vcvdt3D.h"
#include "FIM3D.h"
#include "interpolation3D.h"

namespace hfrep3D {

enum class stepFunction
{
    HYPERBOLIC_TANGENT,
    HYPERBOLIC_SIGMOID,
    ALGEBRAIC,
    GUDERMANIAN
};

enum class distMethood
{
    FIM3D,
    VCVDT3D,
    VDT3D_FAST,
    VDT3D_TIES,
    VDT3D_TRUE,
    VDT3D_BRUTEFORCE
};

class HFRep3D
{
public:
    HFRep3D( const int gridW, const int gridH, const int gridD);
    ~HFRep3D() { }

    void calculateHFRep3D(std::vector<float> *frep3D, stepFunction stepFunc, float slope,
                          distMethood method, glm::vec2 *surfLims=nullptr, bool checkRep = false );

    inline std::vector<float> getDDT3D()       { return UDF; }
    inline std::vector<float> getSmoothDDT3D() { return SmoothUDF; }
    inline std::vector<float> getSignedDDT()   { return SDF; }
    inline std::vector<float> getHFRepObj3D()  { return HFRepObj; }

    inline void setNewResolution( int voxWidth, int voxHeight, int voxDepth ) { voxGrW = voxWidth;
                                                                                voxGrH = voxHeight;
                                                                                voxGrD = voxDepth; }
private:
    int voxGrW, voxGrH, voxGrD;

    std::vector<float> FRepObj;
    std::vector<float> HFRepObj;
    std::vector<float> UDF, SDF, SmoothUDF;

    std::shared_ptr<distanceTransform3D> DT3D;
    std::shared_ptr<FIMsolver3D> FIM3D;
    std::shared_ptr<inter3D> interpolField;
    std::shared_ptr<prof::timer> profiler;

    std::vector<float> computeVDT(const glm::ivec3 res, std::vector<float> frep, distMethood method );
    float getStepFunVal(float frep_val, stepFunction stepFunc, float slope);
    void  generateHFRepObject( stepFunction stepFunc, float slope );
    void  smoothDistanceField();

    bool  checkHFrep(distMethood method);

    inline int index(int x, int y, int z) { return z*voxGrW*voxGrH + y*voxGrW + x; }
};

}//namespace function_rep_3D
#endif
