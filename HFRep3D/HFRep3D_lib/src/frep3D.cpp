#include "include/frep3D.h"

namespace frep3D {

FRepObj3D::FRepObj3D(int resX, int resY, int resZ, float scaleF)
{
    resolutionX = resX;
    resolutionY = resY;
    resolutionZ = resZ;
    scale      = scaleF;
}

float FRepObj3D::sphere(Point3D pos, Point3D center, float R)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return rad*rad - shX*shX - shY*shY - shZ*shZ;
}

float FRepObj3D::heart3D(Point3D pos, Point3D center)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return - std::pow( ( 2.0f*scale*scale*shZ*shZ + scale*scale*shY*shY + scale*scale*shX*shX - 1.0f ), 3.0f ) +
             scale*scale*shY*shY * scale*scale*scale*shZ*shZ*shZ/10.0f +
             scale*scale*shX*shX * scale*scale*scale*shY*shY*shY;
}

std::vector<float> FRepObj3D::getFRep3D(Point3D cent, std::function<float (Point3D, Point3D)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            for (int x = 0; x < resolutionX; x++)
            {
                frep.push_back(fun(cent, Point3D( x, y, z )));
            }
        }
    }
    return frep;
}

std::vector<float> FRepObj3D::getFRep3D(Point3D cent, float R, std::function<float (Point3D, Point3D, float)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            for (int x = 0; x < resolutionX; x++)
            {
                frep.push_back(fun(cent, Point3D( x, y, z ), R ));
            }
        }
    }
    return frep;
}


} //namespace frep3D_object
