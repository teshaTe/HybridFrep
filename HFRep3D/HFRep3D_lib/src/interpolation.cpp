#include "include/interpolation.h"

namespace interpolation_3d {

interpolation::interpolation(int gridW, int gridH, int gridD)
{
    voxGrW = gridW;
    voxGrH = gridH;
    voxGrD = gridD;
}

void interpolation::interpolateField(std::vector<float> *inField, int interType)
{
    finField.clear();

    if( interType == TRILINEAR )
        interpolateTrilinear( inField );
    else if ( interType == TRICUBIC )
        interpolateTricubic( inField );
    else {
        std::cerr << "ERROR: unknown interpolation type [interType]! " << std::endl;
        return;
    }
}

void interpolation::interpolateTrilinear(std::vector<float> *field)
{

}

void interpolation::interpolateTricubic(std::vector<float> *field)
{

}

}//namespace interpolation_3d
