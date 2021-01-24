#include "device_funcs.cuh"
#include "helper_cuda_math.h"

#include <stdio.h>

#define EPS 0.01f

__host__ int divUp( int a, int b ) { return ( a + b - 1) / b; }

__device__ unsigned char clip( int n ) { return n > 255 ? 255 : ( n < 0 ? 0 : n ); }

__device__ int clipWithBounds( int n, int n_min, int n_max )
{
    return n > n_max ? n_max : ( n < n_min ? n_min : n );
}


__device__ float3 xRotate( float3 pos, float theta )
{
    const float c = cosf( theta ), s = sinf( theta );
    return make_float3( pos.x, c*pos.y - s*pos.z, s*pos.y + c*pos.z );
}

__device__ float3 yRotate( float3 pos, float theta )
{
    const float c = cosf( theta ), s = sinf( theta );
    return make_float3( c*pos.x + s*pos.z, pos.y, -s*pos.x + c*pos.z );
}

__device__ float3 zRotate( float3 pos, float theta )
{
    const float c = cosf( theta ), s = sinf( theta );
    return make_float3( c*pos.x - s*pos.y, s*pos.x + c*pos.y, pos.z );
}


/*__device__ float func( int col, int row, int stack, int id, int3 volSize, float4 params )
{
    const int3 pos0 = { volSize.x/2, volSize.y/2, volSize.z/2 };
    //const float dx = float(col - pos0.x)/float(volSize.x),
    //            dy = float(row - pos0.y)/float(volSize.y),
    //            dz = float(stack - pos0.z)/float(volSize.z);

    const float dx = float(col - pos0.x),
                dy = float(row - pos0.y),
                dz = float(stack - pos0.z);

    //sphere
    if( id == 0 )
    {
        return sqrtf( dx*dx + dy*dy + dz*dz ) - params.x ; //r = 0.36 if not params
    }
    //torus
    else if ( id == 1 )
    {
        const float r = sqrtf( dx*dx + dy*dy );
        return sqrtf( ( r - params.x )*( r - params.x ) + dz*dz ) - params.y;
    }
    //block
    else
    {
        float x = fabsf( dx ) - params.x, y = fabsf( dy ) - params.y, z = fabsf( dz ) - params.z;
        if( x <= 0 && y <= 0 && z <= 0 )
            return fmaxf( x, fmaxf( y, z ) );
        else
        {
            x = fmaxf( x, 0 ), y = fmaxf( y, 0 ), z = fmaxf( z, 0 );
            return sqrtf( x*x + y*y + z*z );
        }
    }
}*/

__device__ float3 srcIdxToPos( int col, int row, int w, int h, float zs )
{
    return make_float3( col - w/2, row - h/2, zs );
}

__device__ float3 paramRay( Ray r, float t ) { return r.o + t *(r.d); }

__device__ float planeSDF( float3 pos, float3 norm, float d )
{
    return dot( pos, normalize( norm ) ) - d;
}

__device__ bool rayPlaneIntersect( Ray myRay, float3 n, float dist, float *t )
{
    const float f0 = planeSDF( paramRay( myRay, 0.0f ), n, dist );
    const float f1 = planeSDF( paramRay( myRay, 1.0f ), n, dist );
    bool result = ( f0 * f1 < 0 );
    if( result ) *t = ( 0.0f - f0 ) / ( f1 - f0 );
    return result;
}

__device__ bool intersectBox( Ray r, float3 boxmin, float3 boxmax, float *tnear, float *tfar )
{
    //compute intersection of ray with all six bbox planes
    const float3 invR = make_float3( 1.0f ) / r.d;
    const float3 tbot = invR * ( boxmin - r.o ), ttop = invR * (boxmax - r.o );

    //Re-order intersections to find smallest and largest on each axiss
    const float3 tmin = fminf( ttop, tbot ), tmax = fmaxf( ttop, tbot );

    //find the largest tmin and the smallest tmax
    *tnear = fmaxf( fmaxf( tmin.x, tmin.y ), fmaxf( tmin.x, tmin.z ) );
    *tfar  = fminf( fminf( tmax.x, tmax.y ), fminf( tmax.x, tmax.z ) );
    return *tfar > *tnear;
}

__device__ int3 posToVolIndex( float3 pos, int3 volSize )
{
    return make_int3( pos.x + volSize.x/2, pos.y + volSize.y/2, pos.z + volSize.z/2 );
}

__device__ int flatten( int3 index, int3 volSize )
{
    return index.x + index.y * volSize.x + index.z * volSize.x * volSize.y;
}

__device__ float interpolate_in_volume( float *d_sdf, int3 sdfSize, float3 pos )
{
    int3 index = { pos.x, pos.y, pos.z };
    int i = index.x, j = index.y, k = index.z;

    const int w  = sdfSize.x, h = sdfSize.y, d = sdfSize.z;
    const float3 rem = fracf(pos);

    index = make_int3( clipWithBounds( i, 0, w - 2 ),
                       clipWithBounds( j, 0, h - 2 ),
                       clipWithBounds( k, 0, d - 2 ) );

    //directed incremenets for computing the gradient
    const int3 dx = { 1, 0, 0 }, dy = { 0, 1, 0 }, dz = { 0, 0, 1 };

    //directed increments for computing grid points
    const float dens000 = d_sdf[ flatten( index, sdfSize ) ];
    const float dens100 = d_sdf[ flatten( index + dx, sdfSize ) ];
    const float dens010 = d_sdf[ flatten( index + dy, sdfSize ) ];
    const float dens001 = d_sdf[ flatten( index + dz, sdfSize ) ];
    const float dens110 = d_sdf[ flatten( index + dx + dy, sdfSize ) ];
    const float dens101 = d_sdf[ flatten( index + dx + dz, sdfSize ) ];
    const float dens011 = d_sdf[ flatten( index + dy + dz, sdfSize ) ];
    const float dens111 = d_sdf[ flatten( index + dx + dy + dz, sdfSize ) ];

    //trilinear interpolation
    return ( 1 - rem.x ) * ( 1 - rem.y ) * ( 1 - rem.z ) * dens000 +
           rem.x         * ( 1 - rem.y ) * ( 1 - rem.z ) * dens100 +
           ( 1 - rem.x ) * rem.y         * ( 1 - rem.z ) * dens010 +
           ( 1 - rem.x ) * ( 1 - rem.y ) * rem.z         * dens001 +
           rem.x         * rem.y         * ( 1 - rem.z ) * dens110 +
           rem.x         * ( 1 - rem.y ) * rem.z         * dens101 +
           ( 1 - rem.x ) * rem.y         * rem.z         * dens011 +
            rem.x        * rem.y         * rem.z         * dens111;
}


__device__ float density( float *d_vol, int3 volSize, float3 pos )
{
    int3 index = posToVolIndex( pos, volSize );
    int i = index.x, j = index.y, k = index.z;

    const int w  = volSize.x, h = volSize.y, d = volSize.z;
    const float3 rem = fracf( pos );

    index = make_int3( clipWithBounds( i, 0, w - 2 ),
                       clipWithBounds( j, 0, h - 2 ),
                       clipWithBounds( k, 0, d - 2 ) );

    //directed incremenets for computing the gradient
    const int3 dx = { 1, 0, 0 }, dy = { 0, 1, 0 }, dz = { 0, 0, 1 };

    //directed increments for computing grid points
    const float dens000 = d_vol[ flatten( index, volSize ) ];
    const float dens100 = d_vol[ flatten( index + dx, volSize ) ];
    const float dens010 = d_vol[ flatten( index + dy, volSize ) ];
    const float dens001 = d_vol[ flatten( index + dz, volSize ) ];
    const float dens110 = d_vol[ flatten( index + dx + dy, volSize ) ];
    const float dens101 = d_vol[ flatten( index + dx + dz, volSize ) ];
    const float dens011 = d_vol[ flatten( index + dy + dz, volSize ) ];
    const float dens111 = d_vol[ flatten( index + dx + dy + dz, volSize ) ];

    //trilinear interpolation
    return ( 1 - rem.x ) * ( 1 - rem.y ) * ( 1 - rem.z ) * dens000 +
           rem.x         * ( 1 - rem.y ) * ( 1 - rem.z ) * dens100 +
           ( 1 - rem.x ) * rem.y         * ( 1 - rem.z ) * dens010 +
           ( 1 - rem.x ) * ( 1 - rem.y ) * rem.z         * dens001 +
           rem.x         * rem.y         * ( 1 - rem.z ) * dens110 +
           rem.x         * ( 1 - rem.y ) * rem.z         * dens101 +
           ( 1 - rem.x ) * rem.y         * rem.z         * dens011 +
            rem.x        * rem.y         * rem.z         * dens111;
}

__device__ uchar4 sliceShader( float *d_vol, int3 volSize, Ray boxRay, float gain, float dist, float3 norm )
{
    float t;
    uchar4 shade = make_uchar4( 96, 0, 192, 0 );
    if( rayPlaneIntersect( boxRay, norm, dist, &t ) )
    {
        float sliceDens = density( d_vol, volSize, paramRay( boxRay, t ) );
        shade = make_uchar4( 48, clip(-10000*sliceDens ), 96, 255 );
    }
    return shade;
}

__device__ uchar4 volumeRenderShader( float *d_vol, int3 volSize, Ray boxRay, float threshold, int numSteps )
{
    uchar4 shade = make_uchar4( 96, 0, 192, 0 ); // defining background value
    const float dt  = 1.f / numSteps;
    const float len = length( boxRay.d ) / numSteps;

    float accum = 0.0f;
    float3 pos  = boxRay.o;
    float val   = density( d_vol, volSize, pos );

    for ( float t = 0.0f; t < 1.0f; t += dt )
    {
        if( val - threshold < 0.0f )
            accum += fabsf( val - threshold ) * len;

        pos = paramRay( boxRay, t );
        val = density( d_vol, volSize, pos );
    }

    if( clip( accum ) > 0.0f )
        shade.y = clip( accum );

    return shade;
}


__device__ uchar4 rayCastShader( float *d_vol, int3 volSize, Ray boxRay, float dist )
{
    uchar4 shade = make_uchar4( 96, 0, 192, 0 );
    float3 pos   = boxRay.o;

    float  len   = length( boxRay.d );
    float  t     = 0.0f;
    float  f     = density( d_vol, volSize, pos );
    while ( f > dist + EPS && t < 1.0f )
    {
        f   = density( d_vol, volSize, pos );
        t  += ( f - dist ) / len;
        pos = paramRay( boxRay, t );
        f   = density( d_vol, volSize, pos );
    }

    if( t < 1.0f )
    {
        const float3 ux = make_float3( 1, 0, 0 ), uy = make_float3( 0, 1, 0 ), uz = make_float3( 0, 0, 1 );

        float3 grad = { ( density( d_vol, volSize, pos + EPS*ux ) - density( d_vol, volSize, pos ) ) / EPS,
                        ( density( d_vol, volSize, pos + EPS*uy ) - density( d_vol, volSize, pos ) ) / EPS,
                        ( density( d_vol, volSize, pos + EPS*uz ) - density( d_vol, volSize, pos ) ) / EPS
                      };

        float intensity = -dot( normalize( boxRay.d ), normalize( grad ) );
        shade = make_uchar4( 255 * intensity, 0, 0, 255 );
    }

    return shade;
}
