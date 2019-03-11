#include "include/interpolation.h"
#include <iostream>

namespace interpolation {

interpolate::interpolate(int gridW, int gridH, int gridD)
{
    voxGrW = gridW;
    voxGrH = gridH;
    voxGrD = gridD;
}

void interpolate::interpolateField(std::vector<float> *inField, int interType)
{
    finField.clear();
    if( interType == BICUBIC )
    {
        finField.resize(inField->size());
        for ( int y = 0; y < voxGrH; y++)
        {
            for ( int x = 0; x < voxGrW; x++)
            {
                int x0 = clipWithBounds( x, 0, voxGrW-4 );
                int y0 = clipWithBounds( y, 0, voxGrH-4 );

                finField[index2d(x, y)] = interpolateBicubic( inField, x0, y0 );
            }
        }

    }
    else if( interType == TRICUBIC )
    {
        finField.resize(inField->size());
        for (int z = 0; z < voxGrD ; z++)
        {
            for (int y = 0; y < voxGrH; y++)
            {
                for (int x = 0; x < voxGrW; x++)
                {
                    int x0 = clipWithBounds( x, 0, voxGrW-4 );
                    int y0 = clipWithBounds( y, 0, voxGrH-4 );
                    int z0 = clipWithBounds( z, 0, voxGrD-4 );
                    finField[index3d(x, y, z)] = interpolateTricubic( inField, x0, y0, z0 );
                }

            }
        }
    }

}

float interpolate::interpolateCubic( float point[4], float x )
{
    return point[1] + 0.5f * x*(point[2] - point[0] + x*(2.0f*point[0] - 5.0f*point[1] + 4.0f*point[2] -
           point[3] + x*(3.0f*(point[1] - point[2]) + point[3] - point[0])));
}

float interpolate::interpolateBicubic(std::vector<float> *field, int x, int y)
{
    float val[4];
    float u = x / voxGrW, v = y / voxGrH;

    val[0] = interpolateCubic( &field->at(index2d(x, y)),   v );
    val[1] = interpolateCubic( &field->at(index2d(x, y+1)), v );
    val[2] = interpolateCubic( &field->at(index2d(x, y+2)), v );
    val[3] = interpolateCubic( &field->at(index2d(x, y+3)), v );
    return interpolateCubic( val, u );
}

float interpolate::interpolateTricubic(std::vector<float> *field, int x, int y, int z)
{
     float val[4], val0[4], val1[4], val2[4], val3[4];
     float u = x / voxGrW, v = y / voxGrH, w = z / voxGrD;

     val0[0] = interpolateCubic( &field->at(index3d(x, y, z)),   w );
     val0[1] = interpolateCubic( &field->at(index3d(x, y, z+1)), w );
     val0[2] = interpolateCubic( &field->at(index3d(x, y, z+2)), w );
     val0[3] = interpolateCubic( &field->at(index3d(x, y, z+3)), w );
     val[0] = interpolateCubic( val0, v );

     val1[0] = interpolateCubic( &field->at(index3d(x+1, y, z)),   w );
     val1[1] = interpolateCubic( &field->at(index3d(x+1, y, z+1)), w );
     val1[2] = interpolateCubic( &field->at(index3d(x+1, y, z+2)), w );
     val1[3] = interpolateCubic( &field->at(index3d(x+1, y, z+3)), w );
     val[1] = interpolateCubic( val1, v );

     val2[0] = interpolateCubic( &field->at(index3d(x+2, y, z)),   w );
     val2[1] = interpolateCubic( &field->at(index3d(x+2, y, z+1)), w );
     val2[2] = interpolateCubic( &field->at(index3d(x+2, y, z+2)), w );
     val2[3] = interpolateCubic( &field->at(index3d(x+2, y, z+3)), w );
     val[2] = interpolateCubic( val2, v );

     val3[0] = interpolateCubic( &field->at(index3d(x+3, y, z)),   w );
     val3[1] = interpolateCubic( &field->at(index3d(x+3, y, z+1)), w );
     val3[2] = interpolateCubic( &field->at(index3d(x+3, y, z+2)), w );
     val3[3] = interpolateCubic( &field->at(index3d(x+3, y, z+3)), w );
     val[3] = interpolateCubic( val3, v );

     return interpolateCubic( val, u );
}

}//namespace interpolation
