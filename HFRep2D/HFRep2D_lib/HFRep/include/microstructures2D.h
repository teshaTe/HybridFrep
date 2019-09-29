#ifndef H_MICROSTRUCTURES_2D_CLASS
#define H_MICROSTRUCTURES_2D_CLASS

#include <vector>
#include <memory>

#include "frep2D.h"

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

    std::vector<float> sinusoidalFunc  (unitCellType uType, Point2D q, Point2D p, Point2D l, float d_offset );
    std::vector<float> irregularDistr  ( unitCellType uType, Point2D q, Point2D p );
    std::vector<float> sawtoothWaveFunc( unitCellType uType, int harmonNum, int period );
    std::vector<float> triangleWaveFunc( unitCellType uType,int period );

    std::vector<float> calcObjWithMStruct(std::vector<float> *frepObj, std::vector<float> *m_struct, float offset );

    inline void changeRes( int x, int y ) { resX = x; resY = y; }

private:
    void clamp(Point2D *val );

    int resX, resY;

     std::shared_ptr<FRepObj2D> frep;
};

}//namespace frep2D
#endif //define H_MICROSTRUCTURES_2D_CLASS
