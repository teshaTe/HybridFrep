#ifndef H_MICROSTRUCTURES_2D_CLASS
#define H_MICROSTRUCTURES_2D_CLASS

#include <vector>
#include <memory>

#include "frep2D.h"
#include <glm/glm.hpp>

namespace hfrep2D {

enum unitCellType
{
    TORUS_CELL,
    CIRCLE_CELL,
    TRIANGLE_CELL,
    RODE_CELL
};

class microstruct
{
public:
    microstruct( int res_x, int res_y );
    ~microstruct() {  }

    std::vector<float> sinusoidalFunc  (unitCellType uType, glm::vec2 q, glm::vec2 p, glm::vec2 l, float d_offset );
    std::vector<float> irregularDistr  ( unitCellType uType, glm::vec2 q, glm::vec2 p );
    std::vector<float> sawtoothWaveFunc( unitCellType uType, int harmonNum, int period );
    std::vector<float> triangleWaveFunc( unitCellType uType,int period );

    std::vector<float> calcObjWithMStruct(std::vector<float> *frepObj, std::vector<float> *m_struct, float offset );

    inline void changeRes( int x, int y ) { resX = x; resY = y; }

private:
    void clamp(glm::vec2 *val );

    int resX, resY;

     std::shared_ptr<FRepObj2D> frep;
};

}//namespace frep2D
#endif //define H_MICROSTRUCTURES_2D_CLASS
