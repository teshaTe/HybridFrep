#include "include/frep2D.h"
#include <iostream>
#include <cmath>

namespace frep2D {

FRepObj2D::FRepObj2D(int resX, int resY, float scaleF )
{
    resolutionX = resX;
    resolutionY = resY;
    scale       = scaleF;
}

float FRepObj2D::triangle(Point2D pos, Point2D cent, float a, float b, float c)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    a = convertToUV( a ); // right side
    b = convertToUV( b ); // left side
    c = convertToUV( c ); // the base of the triangle

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;

    float lb1 = shY + a - ( shX + b);
    float rb1 = shY + a - (-shX + b);
    return intersect_function( intersect_function( lb1,  rb1, 0.0f, 0.0f), -shY+c, 0.0f, 0.0f );
}

float FRepObj2D::triangle2(Point2D pos, Point2D cent, const float a, const float b)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    Point2D sides = convertToUV( Point2D(a, b) );

    float rec = rectangle( pos, cent, a, b );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    float line = shY - (sides.dy/sides.dx)*shX;

    return intersect_function( rec, line, 0.0f, 0.0f );
}

float FRepObj2D::circle(Point2D pos, Point2D cent, float R)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    R = convertToUV( R );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    return R*R - shX*shX - shY*shY;
}

float FRepObj2D::blobby2D(Point2D pos, Point2D cent, float R)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    R = convertToUV( R );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    return R*R/( shX*shX + shY*shY);
}

float FRepObj2D::ellipticCylZ2D(Point2D pos, Point2D cent, float a, float b)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    return 1.0f/resolutionX - std::pow( shX, 2.0f )/( a*a ) - std::pow( shY, 2.0f )/( b*b );
}

float FRepObj2D::ellipsoid2D(Point2D pos, Point2D cent, float a, float b)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    return 1.0f/resolutionX - std::pow( ( shX )/a, 2.0f ) - std::pow( ( shY )/b, 2.0f );
}

float FRepObj2D::torusY2D(Point2D pos, Point2D cent, float R, float rev)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    R = convertToUV( R );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    return R*R - std::pow( shY, 2.0f ) - std::pow( shY, 2.0f ) - rev*rev +
            2.0f*rev*std::sqrt( std::pow( shX, 2.0f ) + std::pow( shY, 2.0f ));
}

float FRepObj2D::torusZ2D(Point2D pos, Point2D cent, float R, float rev)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    R = convertToUV( R );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;

    return R*R - std::pow( shX, 2.0f ) - std::pow( shY, 2.0f ) -
            rev*rev + 2.0f*rev*std::sqrt( std::pow( shX, 2.0f ) + std::pow( shY, 2.0f ) );
}

float FRepObj2D::rectangle(Point2D pos, Point2D cent, float w, float h )
{
    Point2D uv  = convertToUV( pos );
    Point2D c0  = convertToUV( cent );
    Point2D rec = convertToUV( Point2D(w, h) );

    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    return intersect_function( rec.dx - std::abs(shX), rec.dy - std::abs(shY), 0.0f, 0.0f );
}

float FRepObj2D::heart2D(Point2D pos, Point2D cent)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;
    return - std::pow( scale*scale*shX*shX + scale*scale*shY*shY - 1.0f, 3.0f ) +
            scale*scale*shX*shX*std::pow( scale*shY, 3.0f );
}

float FRepObj2D::decocube2D(Point2D pos, Point2D cent)
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;

    return -( std::pow( scale*scale*shX*shX + scale*scale*shY*shY - 2.0f, 2.0f) + 1.0f ) *
           ( std::pow( scale*scale*shY*shY - 2.0f, 2.0f ) + std::pow(scale*scale*shX*shX-1.0f, 2.0f)) *
           ( std::pow( scale*scale*shX*shX - 2.0f, 2.0f ) + std::pow(scale*scale*shY*shY-1.0f, 2.0f)) + 50.0f;
}

float FRepObj2D::suriken( Point2D pos, Point2D cent )
{
    Point2D uv = convertToUV( pos );
    Point2D c0 = convertToUV( cent );
    float shX = uv.dx - c0.dx;
    float shY = uv.dy - c0.dy;

    float lb1 = shY + 0.7f - ( shX*2.0f + 0.3f);
    float rb1 = shY + 0.7f - (-shX*2.0f + 0.3f);
    float lb2 = shY - 0.1f - ( shX*2.0f + 0.3f);
    float rb2 = shY - 0.1f - (-shX*2.0f + 0.3f);
    float lb3 = shX + 0.7f - ( shY*2.0f + 0.3f);
    float rb3 = shX + 0.7f - (-shY*2.0f + 0.3f);
    float lb4 = shX - 0.1f - ( shY*2.0f + 0.3f);
    float rb4 = shX - 0.1f - (-shY*2.0f + 0.3f);

    float trian1 = intersect_function( intersect_function( lb1,  rb1, 0.0f, 0.0f), -shY+0.2f, 0.0f, 0.0f );
    float trian2 = intersect_function( intersect_function(-lb2, -rb2, 0.0f, 0.0f),  shY+0.2f, 0.0f, 0.0f );
    float trian3 = intersect_function( intersect_function( lb3,  rb3, 0.0f, 0.0f), -shX+0.2f, 0.0f, 0.0f );
    float trian4 = intersect_function( intersect_function(-lb4, -rb4, 0.0f, 0.0f),  shX+0.2f, 0.0f, 0.0f );

    return union_function( union_function( union_function( trian1, trian2, 0.0f, 0.0f ),
                                             trian3, 0.0f, 0.0f ), trian4, 0.0f, 0.0f );
}

float FRepObj2D::elf( Point2D pos )
{
    Point2D uv = convertToUV( pos );
    float shX  = resolutionX/2.0f,
          shY  = resolutionY/3.0f;
    //head
    Point2D headC = Point2D( (0.0f+shX), (4.5f+shY) );
    float head    = ellipsoid2D( pos, headC, 6.0f, 5.5f );

    //nose
    Point2D noseC = Point2D( 0.0f+shX, 25.5f+shY );
    float nose    = ellipsoid2D( pos, noseC, 0.5f, 0.5f );

    //face
    Point2D faceC1 = Point2D( 0.0f+shX, 4.5f+shY );
    float xFace1   = ellipsoid2D( pos, faceC1, 6.0f, 5.5f );
    Point2D faceC2 = Point2D( 0.0f+shX, 3.7f+shY );
    float xFace2   = ellipticCylZ2D( pos, faceC2, 4.7f, 4.5f );
    float face     = intersect_function( xFace1, xFace2, 0.0f, 0.0f );

    //mouth
    Point2D mouthC = Point2D( 0.0f+shX, 10.5f+shY );
    float mouth1   = ellipsoid2D( pos, mouthC, 3.0f, 2.3f );
    float mouth2   = -(60.5f+shY)/resolutionY + uv.dy;
    float mouth    = intersect_function( mouth1, mouth2, 0.0f, 0.0f );

    //eyes
    Point2D eye1C = Point2D( 0.0f+shX, 2.5f+shY );
    float eye1    = ellipsoid2D( pos, eye1C, 6.0f, 5.5f );

    Point2D eye2C = Point2D( 36.5f+shX, -10.5f+shY );
    float eye2    = ellipticCylZ2D( pos, eye2C, 1.5f, 1.4f );
    float eye3    = intersect_function( eye1, eye2, 0.0f, 0.0f );

    Point2D eye4C = Point2D( -36.5f+shX, -10.5f+shY );
    float eye4    = ellipticCylZ2D( pos, eye4C, 1.5f, 1.4f );
    float eye5    = intersect_function( eye1, eye4, 0.0f, 0.0f);

    Point2D eye6C = Point2D( 20.3f+shX, -10.5f+shY );
    float eye6    = ellipsoid2D( pos, eye6C, 0.3f, 0.3f );
    float eye7    = subtract_function( eye3, eye6, 0.0f, 0.0f );

    Point2D eye8C = Point2D( -20.3f+shX, -10.5f+shY );
    float eye8    = ellipsoid2D( pos, eye8C, 0.3f, 0.3f );
    float eye9    = subtract_function( eye5, eye8, 0.0f, 0.0f );

    //head final
    float fin_head0 = union_function( eye9, eye7, 0.0f, 0.0f );
    float fin_head1 = union_function( face, head, 0.0f, 0.0f );
    float fin_head2 = subtract_function( fin_head1, mouth, 0.0f, 0.0f );
    float fin_head3 = subtract_function( fin_head2, nose, 0.0f, 0.0f );
    float fin_head  = subtract_function( fin_head3, fin_head0, 0.0f, 0.0f );

    //body
    Point2D bodyC = Point2D( 0.0f+shX, 122.0f+shY );
    float body1   = ellipsoid2D( pos, bodyC, 4.5f, 6.0f );
    float body2   = intersect_function( intersect_function( body1, (126.0f+shY)/resolutionY+uv.dy, 0.0f, 0.0f ),
                                                                  -(121.0f+shY)/resolutionY+uv.dy, 0.0f, 0.0f );

    //neck
    Point2D neckC = Point2D( 0.0f+shX, 120.0f+shY );
    float neck    = ellipsoid2D( pos, neckC, 1.8f, 0.8f );

    //bell
    Point2D bellC = Point2D( 0.0f+shX, 180.0f+shY );
    float bell    = circle( pos, bellC, 10.8f);

    float body3 = union_function( body2, neck, 0.0f, 0.0f );

    //belly
    Point2D bellyC1 = Point2D( 0.0f+shX, 32.0f+shY );
    float belly1    = ellipsoid2D( pos, bellyC1, 4.5, 6.0 );

    Point2D bellyC2 = Point2D( 0.0f+shX, 200.0f+shY );
    float belly2    = ellipticCylZ2D( pos, bellyC2, 0.5f, 0.5f );
    float belly3    = intersect_function( belly1, belly2, 0.0f, 0.0f );

    Point2D bellyC4 = Point2D( 0.0f+shX, 138.0f+shY );
    float belly4    = ellipticCylZ2D( pos, bellyC4, 2.0f, 1.6f );

    Point2D bellyC5 = Point2D( 0.0f+shX, 200.0f+shY );
    float belly5    = ellipsoid2D( pos, bellyC5, 1.1f, 1.1f );
    float belly6    = -(103.0f+shY)/resolutionY + uv.dy;

    float fin_body0 = intersect_function( belly4, belly6, 0.0f, 0.0f);
    float fin_body1 = union_function( fin_body0, belly5, 0.0f, 0.0f );
    float fin_body2 = subtract_function( body3, fin_body1, 0.0f, 0.0f );
    float fin_body3 = union_function( fin_body2, belly3, 0.0f, 0.0f );
    float fin_body  = union_function( fin_body3, bell, 0.0f, 0.0f );

    //legs and feet
    Point2D l_legC = Point2D( 93.0f+shX, 247.5f+shY );
    float l_leg    = ellipsoid2D( pos, l_legC, 3.0f, 1.5f );
    Point2D r_legC = Point2D( -93.0f+shX, 247.5f+shY );
    float r_leg    = ellipsoid2D( pos, r_legC, 3.0, 1.5 );

    Point2D l_footC = Point2D( 92.0f+shX, 246.0f+shY );
    float l_foot    = ellipsoid2D( pos, l_footC, 1.0f, 1.0f );
    Point2D r_footC = Point2D(-92.0f+shX, 246.0f+shY);
    float r_foot    = ellipsoid2D( pos, r_footC, 1.0f, 1.0f );

    float legs0 = union_function( l_leg, l_foot, 0.0f, 0.0f);
    float legs1 = union_function( r_leg, r_foot, 0.0f, 0.0f );
    float legs  = union_function( legs0, legs1, 0.0f, 0.0f );

    //right mustache
    float rh = 0.15f;
    Point2D r_mustC1 = Point2D( 112.0f+shX, 100.0f+shY );
    float r_must1    = torusZ2D( pos, r_mustC1, 7.0f, rh );
    Point2D r_mustC2 = Point2D( 122.0f+shX, 102.0f+shY );
    float r_must2    = torusZ2D( pos, r_mustC2, 7.0f, rh );
    Point2D r_mustC3 = Point2D( 112.0f+shX, 104.0f+shY );
    float r_must3    = torusZ2D( pos, r_mustC3, 7.0f, rh );

    //left mustache
    Point2D l_mustC1 = Point2D( -112.0f+shX, 100.0f+shY );
    float l_must1    = torusZ2D( pos, l_mustC1, 7.0f, rh );
    Point2D l_mustC2 = Point2D( -112.0f+shX, 102.0f+shY );
    float l_must2    = torusZ2D( pos, l_mustC2, 7.0f, rh );
    Point2D l_mustC3 = Point2D( -112.0f+shX, 104.0f+shY );
    float l_must3    = torusZ2D( pos, l_mustC3, 7.0f, rh );

    float r_must11 = intersect_function( uv.dy, (1.4f*shY)/resolutionY - uv.dy, 0.0f, 0.0f );
    float r_must22 = union_function( union_function( r_must1, r_must2, 0.0f, 0.0f ), r_must3, 0.0f, 0.0f );
    float r_mustF  = intersect_function( intersect_function( r_must22, r_must11, 0.0f, 0.0f ), uv.dy, 0.0f, 0.0f );

    float l_must11 = intersect_function( -uv.dy + (1.4f*shY)/resolutionY, uv.dy, 0.0f, 0.0f );
    float l_must22 = union_function( union_function( l_must1, l_must2, 0.0f, 0.0f ), l_must3, 0.0f, 0.0f );
    float l_mustF  = intersect_function( intersect_function( l_must22, l_must11, 0.0f, 0.0f), uv.dy, 0.0f, 0.0f );
    float mustache = union_function( r_mustF, l_mustF, 0.0f, 0.0f );

    //hands
    Point2D r_handC = Point2D( 158.0f+shX, 152.5f+shY );
    float r_hand    = circle( pos, r_handC, 30.0f );
    Point2D l_handC = Point2D( -158.0f+shX, 152.5f+shY );
    float l_hand    = circle( pos, l_handC, 30.0f );

    float ude2   = uv.dy + (150.0f+shY)/resolutionY;
    float ude3   = (150.0f+shY)/resolutionY - uv.dy;
    Point2D udeC = Point2D( 0.0f+shX, 251.0f+shY );
    float ude1   = torusZ2D( pos, udeC, 30.0f, 0.3f );
    float te1    = intersect_function( intersect_function( ude1, ude2, 0.0f, 0.0f ), ude3, 0.0f, 0.0f );
    float te2    = union_function( union_function( te1, r_hand, 0.0f, 0.0f ), l_hand, 0.0f, 0.0f );

    //final model
    float elf0 = union_function( fin_head, fin_body, 0.0f, 0.0f );
    float elf1 = union_function( elf0, legs, 0.0f, 0.0f );
    float elf2 = union_function( elf1, te2, 0.0f, 0.0f );
    float elf  = union_function( elf2, mustache, 0.0f, 0.0f );

    return elf;
}

float FRepObj2D::bounded_blending(float f1, float f2, float a0, float a1, float a2, float a3,
                                  float time, float alpha, float m)
{
      f1 = intersect_function( f1, -time, alpha, m);
      f2 = intersect_function( f2, time - 1.0f, alpha, m);
      float f3 = intersect_function(time + 10.0f, 10.0f - time, alpha, m);

      float r1 = (f1/a1)*(f1/a1) + (f2/a2)*(f2/a2);
      float r2 = 0.0f;

      if( f3 > 0.0f )
      {
        r2 = (f3/a3) * (f3/a3);
      }

      float rr = 0.0f;
      if( r1 > 0.0f )
      {
        rr = r1 / (r1 + r2);
      }

      float d = 0.0f;
      if( rr < 1.0f )
      {
        d = a0 * (1.0f - rr)*(1.0f - rr)*(1.0f - rr) / (1.0f + rr);
      }

      return union_function(f1, f2, alpha, m) + d;
}

float FRepObj2D::intersect_function_R0(float f1, float f2, Point2D gradf1, Point2D gradf2, float n)
{
    float result = 0.0f;

    float k = 2.0f * ( gradf1.dx * gradf2.dx + gradf1.dy * gradf1.dy );

    float f12  = f1*f1;
    float f22  = f2*f2;
    float f13  = f12*f1;
    float f23  = f22*f2;

    float znam  = f1*f1 + f2*f2;
    float znam2 = znam  * znam;
    float znam3 = znam2 * znam;

    float A1 = ( f13*f1*f22 + f12*f23*f2 + f1*f2*2.0f ) / znam3;
    float B1 = ( - f12*f22*4.0f - f13*f2*2.0f - f1*f23*2.0f ) / znam2;
    float C1 = ( f12 + f22 + f1*f2*2.0f ) / znam;

    float alpha1 = 1.0f/std::sqrt( A1+B1+C1 );
    float alpha2 = 1.0f/std::sqrt( (f12+f22+f1*f2*2.0f) / znam );

    if( f1 > 0.0f && f2 > 0.0f)
       result = alpha1 * f1*f2 * std::pow( std::pow(f1, n) + std::pow(f2, n), -1.0f/n );
    else if( f1 <= 0.0f && f2 >= 0.0f )
       result = f1;
    else if( f1 >= 0.0f && f2 <= 0.0f )
       result = f2;
    else if( f1 < 0.0f && f2 < 0.0f )
       result = alpha2 * std::pow( -1.0f, n+1.0f ) * std::pow( std::pow(f1, n) + std::pow(f2, n), 1.0f/n );

    return result;
}


float FRepObj2D::union_function_R0(float f1, float f2, float n)
{
    float result = 0.0f;

    if( f1 > 0.0f && f2 > 0.0f)
       result = std::pow( std::pow(f1, n) + std::pow(f2, n), 1.0f/n );
    else if( f1 <= 0.0f && f2 >= 0.0f )
       result = f2;
    else if( f1 >= 0.0f && f2 <= 0.0f )
       result = f1;
    else if( f1 < 0.0f && f2 < 0.0f )
       result = std::pow( -1.0f, n+1.0f ) * f1*f2 * std::pow( std::pow(f1, n) + std::pow(f2, n), -1.0f/n );

    return result;
}

float FRepObj2D::union_function_R0(float f1, float f2, frep2D::Point2D gradf1, frep2D::Point2D gradf2, float n)
{
    float result = 0.0f;

    float k = 2.0f * ( gradf1.dx * gradf2.dx + gradf1.dy * gradf2.dy );

    float f12  = f1*f1;
    float f22  = f2*f2;
    float f13  = f12*f1;
    float f23  = f22*f2;

    float znam  = f12 + f22;
    float znam2 = znam  * znam;
    float znam3 = znam2 * znam;

    float A1 = (  f13*f1*f22   + f12*f23*f2 + k*f13*f23 ) / znam3;
    float B1 = ( -4.0f*f12*f22 - k*f13*f2   - k*f1*f23  ) / znam2;
    float C1 = ( f12 + f22 + k*f1*f2 ) / znam;

    float alpha2 = 1.0f / std::sqrt( A1 + B1 + C1 );

    if( f1 > 0.0f && f2 > 0.0f)
    {
       result = std::pow( std::pow(f1, n) + std::pow(f2, n), 1.0f/n );
    }
    else if( f1 <= 0.0f && f2 >= 0.0f )
       result = f2;
    else if( f1 >= 0.0f && f2 <= 0.0f )
       result = f1;
    else if( f1 < 0.0f && f2 < 0.0f )
    {
        if( !std::isnan( alpha2 ) && !std::isinf( alpha2 ))
            result = alpha2 * std::pow( -1.0f, n+1.0f ) * f1*f2 * std::pow( std::pow(f1, n) + std::pow(f2, n), -1.0f/n );
        else
            result = std::pow( -1.0f, n+1.0f ) * f1*f2 * std::pow( std::pow(f1, n) + std::pow(f2, n), -1.0f/n );
    }

    return result;
}

std::vector<float> FRepObj2D::getFRep2D(std::function<float(Point2D)> fun)
{
    frep.clear();
    for( int y = 0; y < resolutionY; y++ )
    {
        for( int x = 0; x < resolutionX; x++ )
        {
            frep.push_back( fun( Point2D( x, y )) );
        }
    }
    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(Point2D cent, std::function<float( Point2D, Point2D )> fun)
{
    frep.clear();
    for( int y = 0; y < resolutionY; y++ )
    {
        for( int x = 0; x < resolutionX; x++ )
        {
            frep.push_back( fun( Point2D( x, y ), cent ));
        }
    }
    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(Point2D cent, float R,
                                        std::function<float(Point2D, Point2D, float)> fun )
{
    frep.clear();
    for( int y = 0; y < resolutionY; y++ )
    {
        for( int x = 0; x < resolutionX; x++ )
        {
            frep.push_back( fun( Point2D( x, y ), cent, R ));
        }
    }
    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(Point2D cent, float p1, float p2,
                                        std::function<float(Point2D, Point2D, float, float )> fun )
{
    frep.clear();
    for( int y = 0; y < resolutionY; y++ )
    {
        for( int x = 0; x < resolutionX; x++ )
        {
            frep.push_back( fun( Point2D( x, y ), cent, p1, p2 ));
        }
    }
    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(Point2D cent, float p1, float p2, float p3,
                                        std::function<float(Point2D, Point2D, float, float, float)> fun)
{
    frep.clear();
    for( int y = 0; y < resolutionY; y++ )
    {
        for( int x = 0; x < resolutionX; x++ )
        {
            frep.push_back( fun( Point2D( x, y ), cent, p1, p2, p3 ));
        }
    }
    return frep;
}

std::vector<float> FRepObj2D::getFRep2D( std::vector<float> f1, std::vector<float> f2, float alpha, float m,
                                         std::function<float (float, float, float, float)> fun)
{
    frep.clear();
    for(int i = 0; i < resolutionX*resolutionY; i++)
    {
        frep.push_back( fun( f1[i], f2[i], alpha, m ));
    }

    return frep;
}

std::vector<float> FRepObj2D::getFRep2D(std::vector<float> f1, std::vector<float> f2, float n, std::function<float (float, float, float)> fun)
{
    frep.clear();
    for(int i = 0; i < resolutionX*resolutionY; i++)
    {
        frep.push_back( fun( f1[i], f2[i], n ));
    }

    return frep;
}

Point2D FRepObj2D::getRotatedCoords(Point2D inCoords, const float angle )
{
    float angle0 = static_cast<float>( (angle/180.0f) * M_PI );

    float rotX = inCoords.dx*cosf( angle0 ) - inCoords.dy*sinf( angle0);
    float rotY = inCoords.dx*sinf( angle0 ) + inCoords.dy*cosf( angle0 );
    return Point2D( rotX, rotY );
}

std::vector<float> FRepObj2D::getRotatedFrep2D( Point2D cent, float w, float h,
                                                float angle, std::function<float (Point2D, Point2D, float, float)> fun)
{
    frep.clear();
    Point2D c0 = getRotatedCoords( cent, angle );

    for( int y = 0; y < resolutionY; y++ )
    {
        for( int x = 0; x < resolutionX; x++ )
        {
            frep.push_back( fun( getRotatedCoords( Point2D(x,y), angle ), c0, w, h ));
        }
    }
    return frep;
}

std::vector<float> FRepObj2D::getRotatedFrep2D( Point2D cent, float a, float b, float c, float angle,
                                                std::function<float (Point2D, Point2D, float, float, float, float)> fun )
{
    frep.clear();
    Point2D c0 = getRotatedCoords( cent, angle );

    for( int y = 0; y < resolutionY; y++ )
    {
        for( int x = 0; x < resolutionX; x++ )
        {
            frep.push_back( fun( getRotatedCoords( Point2D(x, y), angle ), c0, a, b, c, angle ));
        }
    }
    return frep;
}

} //namespace frep2D_object
