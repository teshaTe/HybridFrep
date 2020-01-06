#include "frep3D.h"

namespace hfrep3D {

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

float FRepObj3D::blobby(Point3D pos, Point3D center, float R)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return -( shX*shX + shY*shY +shZ*shZ + std::sin(4.0f*shX) + std::sin(4.0f*shY) + std::sin(4.0f*shZ) - rad );
}

float FRepObj3D::cylinderX(Point3D pos, Point3D center, float R)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return rad*rad - shY*shY - shZ*shZ;
}

float FRepObj3D::cylinderY(Point3D pos, Point3D center, float R)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shX = str.dx - c0.dx;
    float shZ = str.dz - c0.dz;

    return rad*rad - shX*shX - shZ*shZ;
}

float FRepObj3D::cylinderZ(Point3D pos, Point3D center, float R)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;

    return rad*rad - shX*shX - shY*shY;
}

float FRepObj3D::ellipticCylinderX(Point3D pos, Point3D center, float a, float b)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return 1.0f/resolutionX - shY*shY/(a*a) + shZ*shZ/(b*b);
}

float FRepObj3D::ellipticCylinderY(Point3D pos, Point3D center, float a, float b)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shZ = str.dz - c0.dz;

    return 1.0f/resolutionX - shX*shX/(a*a) - shZ*shZ/(b*b);
}

float FRepObj3D::ellipticCylinderZ(Point3D pos, Point3D center, float a, float b)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;

    return 1.0f/resolutionX - shX*shX/(a*a) - shY*shY/(b*b);
}

float FRepObj3D::coneX(Point3D pos, Point3D center, float R )
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return shX*shX - shY*shY/(rad*rad) - shZ*shZ/(rad*rad);
}

float FRepObj3D::coneY(Point3D pos, Point3D center, float R)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return shY*shY - shX*shX/(rad*rad) - shZ*shZ/(rad*rad);
}

float FRepObj3D::coneZ(Point3D pos, Point3D center, float R)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rad   = convertToSTR( R );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return shZ*shZ - shX*shX/(rad*rad) - shY*shY/(rad*rad);
}

float FRepObj3D::ellipticConeX(Point3D pos, Point3D center, float a, float b)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return shX*shX - shY*shY/(a*a) - shZ*shZ/(b*b);
}

float FRepObj3D::ellipticConeY(Point3D pos, Point3D center, float h, float a, float b)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return shY*shY - shX*shX/(a*a) - shZ*shZ/(b*b);
}

float FRepObj3D::ellipticConeZ(Point3D pos, Point3D center, float h, float a, float b)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return shZ*shZ - shX*shX/(a*a) - shY*shY/(b*b);
}

float FRepObj3D::torusX(Point3D pos, Point3D center, float R, float r)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rev   = convertToSTR( R );
    float rad   = convertToSTR( r );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return rad*rad - shX*shX - shY*shY - shZ*shZ - rev*rev + 2.0f*rev*std::sqrt( shZ*shZ + shY*shY );
}

float FRepObj3D::torusY(Point3D pos, Point3D center, float R, float r)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rev   = convertToSTR( R );
    float rad   = convertToSTR( r );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return rad*rad - shX*shX - shY*shY - shZ*shZ - rev*rev + 2.0f*rev*std::sqrt( shZ*shZ + shX*shX );
}

float FRepObj3D::torusZ(Point3D pos, Point3D center, float R, float r)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );
    float rev   = convertToSTR( R );
    float rad   = convertToSTR( r );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return rad*rad - shX*shX - shY*shY - shZ*shZ - rev*rev + 2.0f*rev*std::sqrt( shX*shX + shY*shY );
}

float FRepObj3D::box( Point3D center, float w, float h, float d)
{
    Point3D c0   = convertToSTR( center );
    float width  = convertToSTR( w );
    float height = convertToSTR( h );
    float depth  = convertToSTR( d );

    float planeXYfr  = c0.dz + depth / 2.0f;
    float planeXYbc  = c0.dz - depth / 2.0f;
    float planeXZbt  = c0.dx + height / 2.0f;
    float planeXZtop = c0.dx - height / 2.0f;
    float planeYZr   = c0.dy + width / 2.0f;
    float planeYZl   = c0.dy - width / 2.0f;

    float halfBox1 = intersect_function( planeXYfr, intersect_function( planeYZr, planeXZbt, 0.0, 0.0 ), 0.0f, 0.0f );
    float halfBox2 = intersect_function( planeXYbc, intersect_function( planeYZl, planeXZtop, 0.0, 0.0 ), 0.0f, 0.0f );
    return intersect_function( halfBox1, halfBox2, 0.0f, 0.0f );
}

float FRepObj3D::ellipsoid(Point3D pos, Point3D center, float a, float b, float c)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return 1.0f/resolutionX - shX*shX/(a*a) - shY*shY/(b*b) - shZ*shZ/(c*c);
}

float FRepObj3D::superEllipsoid(Point3D pos, Point3D center, float a, float b, float c, float s1, float s2)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    float val0 = shX*shX/( s2*a*a ) + shY*shY/( s2*b*b );
    return 1.0f/resolutionX - std::pow( val0, s2/s1 ) - std::pow( shZ/c, 2.0f/s1 );
}

float FRepObj3D::heart3D(Point3D pos, Point3D center)
{
    Point3D str = convertToSTR( pos );
    Point3D c0  = convertToSTR( center );

    float shX = str.dx - c0.dx;
    float shY = str.dy - c0.dy;
    float shZ = str.dz - c0.dz;

    return - std::pow( ( 2.0f*scale*scale*shZ*shZ + scale*scale*shY*shY + scale*scale*shX*shX - 1.0f ), 3.0f ) +
             scale*scale*shY*shY * scale*scale*scale*shZ*shZ*shZ/20.0f +
             scale*scale*shX*shX * scale*scale*scale*shY*shY*shY;
}

float FRepObj3D::elf3D(Point3D pos)
{
    Point3D uvw = convertToSTR( pos );
    float shX  = resolutionX/2.0f,
          shY  = resolutionY/3.0f,
          shZ  = resolutionZ/2.0f;
    //head
    Point3D headC = Point3D( 0.0f+shX, 4.5f+shY, 0.0f+shZ );
    float head    = ellipsoid( pos, headC, 6.0f, 5.5f, 6.0f );

    //nose
    Point3D noseC = Point3D( 0.0f+shX, 25.5f+shY, 0.3f + shZ );
    float nose    = ellipsoid( pos, noseC, 0.5f, 0.5f, 0.5f );

    //face
    Point3D faceC1 = Point3D( 0.0f+shX, 4.5f+shY, 0.3f+shZ );
    float xFace1   = ellipsoid( pos, faceC1, 6.0f, 5.5f, 6.0f );
    Point3D faceC2 = Point3D( 0.0f+shX, 3.7f+shY, 0.0f+shZ );
    float xFace2   = ellipticCylinderZ( pos, faceC2, 4.7f, 4.5f );
    float face     = intersect_function( xFace1, xFace2, 0.0f, 0.0f );

    //mouth
    Point3D mouthC = Point3D( 0.0f+shX, 10.5f+shY, 6.0f+shZ );
    float mouth1   = ellipsoid( pos, mouthC, 3.0f, 2.3f, 2.3f );
    float mouth2   = -(60.5f+shY)/resolutionY + uvw.dy;
    float mouth    = intersect_function( mouth1, mouth2, 0.0f, 0.0f );

    //eyes
    Point3D eye1C = Point3D( 0.0f+shX, 2.5f+shY, 0.4f+shZ );
    float eye1    = ellipsoid( pos, eye1C, 6.0f, 5.5f, 6.0f );

    Point3D eye2C = Point3D( 36.5f+shX, -10.5f+shY, 0.0f+shZ );
    float eye2    = ellipticCylinderZ( pos, eye2C, 1.5f, 1.4f );
    float eye3    = intersect_function( eye1, eye2, 0.0f, 0.0f );

    Point3D eye4C = Point3D( -36.5f+shX, -10.5f+shY, 0.0f+shZ );
    float eye4    = ellipticCylinderZ( pos, eye4C, 1.5f, 1.4f );
    float eye5    = intersect_function( eye1, eye4, 0.0f, 0.0f);

    Point3D eye6C = Point3D( 20.3f+shX, -10.5f+shY, 5.5f+shZ );
    float eye6    = ellipsoid( pos, eye6C, 0.3f, 0.3f, 0.3f );
    float eye7    = subtract_function( eye3, eye6, 0.0f, 0.0f );

    Point3D eye8C = Point3D( -20.3f+shX, -10.5f+shY, 5.5f+shZ );
    float eye8    = ellipsoid( pos, eye8C, 0.3f, 0.3f, 0.3f );
    float eye9    = subtract_function( eye5, eye8, 0.0f, 0.0f );

    //head final
    float fin_head0 = union_function( eye9, eye7, 0.0f, 0.0f );
    float fin_head1 = union_function( face, head, 0.0f, 0.0f );
    float fin_head2 = subtract_function( fin_head1, mouth, 0.0f, 0.0f );
    float fin_head3 = subtract_function( fin_head2, nose, 0.0f, 0.0f );
    float fin_head  = subtract_function( fin_head3, fin_head0, 0.0f, 0.0f );

    //body
    Point3D bodyC = Point3D( 0.0f+shX, 122.0f+shY, 0.0f+shZ );
    float body1   = ellipsoid( pos, bodyC, 4.5f, 6.0f, 4.5f );
    float body2   = intersect_function( intersect_function( body1, (126.0f+shY)/resolutionY+uvw.dy, 0.0f, 0.0f ),
                                                                  -(121.0f+shY)/resolutionY+uvw.dy, 0.0f, 0.0f );

    //neck
    Point3D neckC = Point3D( 0.0f+shX, 120.0f+shY, 0.0f+shZ );
    float neck    = torusY( pos, neckC, 4.0f, 0.5f );

    //bell
    Point3D bellC = Point3D( 0.0f+shX, 180.0f+shY, 4.5f+shZ );
    float bell    = sphere( pos, bellC, 10.8f);

    float body3 = union_function( body2, neck, 0.0f, 0.0f );

    //belly
    Point3D bellyC1 = Point3D( 0.0f+shX, 32.0f+shY, 0.3f+shZ );
    float belly1    = ellipsoid( pos, bellyC1, 4.5, 6.0, 4.5f );

    Point3D bellyC2 = Point3D( 0.0f+shX, 200.0f+shY, 0.0f+shZ );
    float belly2    = ellipticCylinderZ( pos, bellyC2, 0.5f, 0.5f );
    float belly3    = intersect_function( belly1, belly2, 0.0f, 0.0f );

    Point3D bellyC4 = Point3D( 0.0f+shX, 138.0f+shY, 0.6f+shZ );
    float belly4    = ellipticCylinderZ( pos, bellyC4, 2.0f, 1.6f );

    Point3D bellyC5 = Point3D( 0.0f+shX, 200.0f+shY, 0.6f+shZ );
    float belly5    = ellipsoid( pos, bellyC5, 1.1f, 1.1f, 1.1f );
    float belly6    = -(103.0f+shY)/resolutionY + uvw.dy;

    float fin_body0 = intersect_function( belly4, belly6, 0.0f, 0.0f);
    float fin_body1 = union_function( fin_body0, belly5, 0.0f, 0.0f );
    float fin_body2 = subtract_function( body3, fin_body1, 0.0f, 0.0f );
    float fin_body3 = union_function( fin_body2, belly3, 0.0f, 0.0f );
    float fin_body  = union_function( fin_body3, bell, 0.0f, 0.0f );

    //legs and feet
    Point3D l_legC = Point3D( 93.0f+shX, 247.5f+shY, 0.0f+shZ );
    float l_leg    = ellipsoid( pos, l_legC, 3.0f, 1.5f, 4.0f );
    Point3D r_legC = Point3D( -93.0f+shX, 247.5f+shY, 0.0f+shZ );
    float r_leg    = ellipsoid( pos, r_legC, 3.0, 1.5, 4.0f );

    Point3D l_footC = Point3D( 92.0f+shX, 246.0f+shY, 0.0f+shZ );
    float l_foot    = ellipsoid( pos, l_footC, 1.0f, 1.0f, 2.0f );
    Point3D r_footC = Point3D(-92.0f+shX, 246.0f+shY, 0.0f+shZ);
    float r_foot    = ellipsoid( pos, r_footC, 1.0f, 1.0f, 2.0f );

    float legs0 = union_function( l_leg, l_foot, 0.0f, 0.0f);
    float legs1 = union_function( r_leg, r_foot, 0.0f, 0.0f );
    float legs  = union_function( legs0, legs1, 0.0f, 0.0f );

    //right mustache
    float rh = 0.15f;
    Point3D r_mustC1 = Point3D( 112.0f+shX, 100.0f+shY, 4.5f+shZ );
    float r_must1    = torusZ( pos, r_mustC1, 7.0f, rh );
    Point3D r_mustC2 = Point3D( 122.0f+shX, 102.0f+shY, 4.5f+shZ );
    float r_must2    = torusZ( pos, r_mustC2, 7.0f, rh );
    Point3D r_mustC3 = Point3D( 112.0f+shX, 104.0f+shY, 4.5f+shZ );
    float r_must3    = torusZ( pos, r_mustC3, 7.0f, rh );

    //left mustache
    Point3D l_mustC1 = Point3D( -112.0f+shX, 100.0f+shY, 4.5f+shZ );
    float l_must1    = torusZ( pos, l_mustC1, 7.0f, rh );
    Point3D l_mustC2 = Point3D( -112.0f+shX, 102.0f+shY, 4.5f+shZ );
    float l_must2    = torusZ( pos, l_mustC2, 7.0f, rh );
    Point3D l_mustC3 = Point3D( -112.0f+shX, 104.0f+shY, 4.5f+shZ );
    float l_must3    = torusZ( pos, l_mustC3, 7.0f, rh );

    float r_must11 = intersect_function( uvw.dx, (1.4f*shX)/resolutionX - uvw.dx, 0.0f, 0.0f );
    float r_must22 = union_function( union_function( r_must1, r_must2, 0.0f, 0.0f ), r_must3, 0.0f, 0.0f );
    float r_mustF  = intersect_function( intersect_function( r_must22, r_must11, 0.0f, 0.0f ), uvw.dy, 0.0f, 0.0f );

    float l_must11 = intersect_function( -uvw.dx + (1.4f*shX)/resolutionX, uvw.dx, 0.0f, 0.0f );
    float l_must22 = union_function( union_function( l_must1, l_must2, 0.0f, 0.0f ), l_must3, 0.0f, 0.0f );
    float l_mustF  = intersect_function( intersect_function( l_must22, l_must11, 0.0f, 0.0f), uvw.dy, 0.0f, 0.0f );
    float mustache = union_function( r_mustF, l_mustF, 0.0f, 0.0f );

    //hands
    Point3D r_handC = Point3D( 158.0f+shX, 152.5f+shY, 0.0f+shZ );
    float r_hand    = sphere( pos, r_handC, 30.0f );
    Point3D l_handC = Point3D( -158.0f+shX, 152.5f+shY, 0.0f+shZ );
    float l_hand    = sphere( pos, l_handC, 30.0f );

    float ude2   = uvw.dx + (150.0f+shX)/resolutionY;
    float ude3   = (150.0f+shX)/resolutionY - uvw.dx;
    Point3D udeC = Point3D( 0.0f+shX, 251.0f+shY, 0.0f+shZ );
    float ude1   = torusZ( pos, udeC, 30.0f, 0.3f );
    float te1    = intersect_function( intersect_function( ude1, ude2, 0.0f, 0.0f ), ude3, 0.0f, 0.0f );
    float te2    = union_function( union_function( te1, r_hand, 0.0f, 0.0f ), l_hand, 0.0f, 0.0f );

    //final model
    float elf0 = union_function( fin_head, fin_body, 0.0f, 0.0f );
    float elf1 = union_function( elf0, legs, 0.0f, 0.0f );
    float elf2 = union_function( elf1, te2, 0.0f, 0.0f );
    float elf  = union_function( elf2, mustache, 0.0f, 0.0f );

    return head;
}

std::vector<float> FRepObj3D::getFRep3D(std::function<float (Point3D)> fun)
{
    frep.clear();
    for (int z = 0; z < resolutionZ; z++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            for (int x = 0; x < resolutionX; x++)
            {
                frep.push_back(fun(Point3D( x, y, z )));
            }
        }
    }
    return frep;
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
