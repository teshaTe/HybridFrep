#ifndef H_HFREP_CLASS
#define H_HFREP_CLASS

#include "seddt2D.h"
#include "interpolation2D.h"

#include <vector>
#include <memory>
#include <string>

namespace hfrep2D {

#define HYPERBOLIC_TANGENT 1
#define HYPERBOLIC_SIGMOID 2
#define ALGEBRAIC          3
#define GUDERMANIAN        4

class HFRepObj2D
{

public:
    HFRepObj2D(int res_x, int res_y);
    ~HFRepObj2D() = default;

    std::vector<float> calculateHFRep2D(std::vector<float> *frep, std::vector<float> *ddt,
                                        int stepfunc, float st_slope , bool checkHFRep = false);

    void checkHFrep(std::vector<float> *hfrep , std::vector<float> *frep, std::string hfrep_check_name = "" );

    float getStepFunctionVal(float frep_val, int function, float st_slope);

    inline std::vector<float> getDDT()         { return DDT; }
    inline std::vector<float> getSignedDDT()   { return SDDT; }
    inline std::vector<float> getSmoothedDDT() { return SmDDT; }

    inline void setNewRes( int res_x, int res_y )  { resX = res_x; resY = res_y; }

private:
    std::shared_ptr<DiscreteDistanceTransform> DT;
    std::shared_ptr<inter2D> inter;

    void generateHFRepObject(std::vector<float> *FRep, int step_function, float st_slope);

private:
    std::vector<float> DDT;
    std::vector<float> SmDDT;
    std::vector<float> SDDT;
    std::vector<float> HFRep;

    int resX, resY;
    int dist_sh;
};

} // namespace function_rep
#endif
