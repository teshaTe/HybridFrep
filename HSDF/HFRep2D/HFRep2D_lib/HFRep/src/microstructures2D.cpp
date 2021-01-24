#include "microstructures2D.h"

#include <iostream>

namespace hfrep2D {

microstruct::microstruct(int res_x, int res_y)
{
    resX = res_x;
    resY = res_y;
    frep = std::make_shared<FRepObj2D>( res_x, res_y, 1.0f );
}

std::vector<float> microstruct::sinusoidalFunc(unitCellType uType, glm::vec2 q, glm::vec2 p, glm::vec2 l, float d_offset)
{
    std::vector<float> result;
    clamp( &l );
    float res;

    for ( int y = 0; y < resY; y++ )
    {
        for ( int x = 0; x < resX; x++)
        {
            float u = frep->convertToUV( x );
            float v = frep->convertToUV( y );

            float sX = std::sin( q.x * u + p.x ) - l.x;
            float sY = std::sin( q.y * v + p.y ) - l.y;

            if( uType == RODE_CELL )
                res  = -frep->intersect_function( sX, sY, 0.0f, 0.0f ) + d_offset;
            else if( uType == TORUS_CELL )
                res = frep->torusY2D( glm::vec2(sX, sY), glm::vec2(0.0f, 0.0f), 1.0f, 0.8f );

            result.push_back( res );
        }
    }

    return  result;
}

std::vector<float> microstruct::calcObjWithMStruct(std::vector<float> *frepObj, std::vector<float> *m_struct, float offset)
{
    std::vector<float> result;
    float uv_offset = frep->convertToUV( offset );

    for (size_t i = 0; i < resX*resY ; i++)
    {
        float offsetF  = frepObj->at(i) - uv_offset;
        float shell    = frep->subtract_function ( frepObj->at(i), offsetF, 0.0f, 0.0f );
        float interior = frep->intersect_function( m_struct->at(i), frepObj->at(i), 0.0f, 0.0f );
        float res = frep->union_function( shell, interior, 0.0f, 0.0f );
        result.push_back( res );
    }
    return result;
}

void microstruct::clamp(glm::vec2 *val)
{
    if( val->x > 1.0f )  val->x = 1.0f;
    if( val->y > 1.0f )  val->y = 1.0f;
    if( val->x < -1.0f ) val->x = -1.0f;
    if( val->y < -1.0f ) val->y = -1.0f;
}




} //namespace frep2D
