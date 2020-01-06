#ifndef H_HFREP_CLASS
#define H_HFREP_CLASS

#include <vector>
#include <memory>

#include "ddt3D.h"
#include "interpolation3D.h"

namespace hfrep3D {

#define HYPERBOLIC_TANGENT 1
#define HYPERBOLIC_SIGMOID 2
#define ALGEBRAIC          3
#define GUDERMANIAN        4

class HFRep3D
{
public:
    HFRep3D( int gridW, int gridH, int gridD);
    ~HFRep3D() { }

    void calculateHFRep3D(std::vector<float> *frep3D, int stepFunc, float slope, bool checkRep = false );

    inline std::vector<float> getDDT3D()       { return DDT; }
    inline std::vector<float> getSmoothDDT3D() { return SmoothDDT; }
    inline std::vector<float> getSignedDDT()   { return SDDT; }
    inline std::vector<float> getHFRepObj3D()  { return HFRepObj; }

    inline void setNewResolution( int voxWidth, int voxHeight, int voxDepth ) { voxGrW = voxWidth;
                                                                                voxGrH = voxHeight;
                                                                                voxGrD = voxDepth; }
private:
    int voxGrW, voxGrH, voxGrD;

    std::vector<float> FRepObj;
    std::vector<float> HFRepObj;
    std::vector<float> DDT, SDDT, SmoothDDT;

    std::shared_ptr<distanceTransform3D> DT3D;
    std::shared_ptr<interpolate> interpolField;

    float getStepFunVal(float frep_val, int function, float slope);
    void  generateHFRepObject( int stepFunction, float slope );

    bool  checkHFrep();

    inline int index(int x, int y, int z) { return z*voxGrW*voxGrH + y*voxGrW + x; }
};

}//namespace function_rep_3D
#endif
