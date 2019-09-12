#ifndef H_HFREP_CLASS
#define H_HFREP_CLASS

#include "ddt2D.h"
#include "interpolation2D.h"

#include <vector>
#include <memory>

namespace hfrep2D {

#define HYPERBOLIC_TANGENT 1
#define HYPERBOLIC_SIGMOID 2
#define ALGEBRAIC          3
#define GUDERMANIAN        4

class HFRepObj2D
{

public:
    HFRepObj2D(int res_x, int res_y, int ddt_sh);

    std::vector<float> calculateHFRep2D(std::vector<float> *frep, std::vector<float> *ddt, int stepfunc, float st_slope , bool checkHFRep);
    void checkHFrep(std::vector<float> *hfrep , std::vector<float> *frep, std::string hfrep_check_name = "" );
    float get_step_function_val(float frep_val, int function, float st_slope);

    inline std::vector<float> getDDT()         { return DDT; }
    inline std::vector<float> getSignedDDT()   { return SDDT; }
    inline std::vector<float> getSmoothedDDT() { return SmDDT; }

    inline void setNewRes( int resX, int resY )  { resolution_x = resX; resolution_y = resY; }

private:
    std::shared_ptr<DiscreteDistanceTransform> DT;
    std::shared_ptr<ModifyField> modF;

    void generateHFRepObject(std::vector<float> *FRep, int step_function, float st_slope);

private:
    std::vector<float> DDT;
    std::vector<float> SmDDT;
    std::vector<float> SDDT;
    std::vector<float> HFRep;

    int resolution_x, resolution_y;
    int dist_sh;
};

} // namespace function_rep
#endif
