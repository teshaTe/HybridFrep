#include "kernel.h"
#include "device_funcs.cuh"
#include "helper_cuda_math.h"

#include <stdio_ext.h>

#define TX_2D 32
#define TY_2D 32
#define TX 8
#define TY 8
#define TZ 8

#define NUMSTEPS 20

__global__ void renderKernel( uchar4 *d_out, float *d_vol, int w, int h, int3 volSize,
                              int method, float zs, float theta, float threshold, float dist )
{
    const int col = blockIdx.x*blockDim.x + threadIdx.x;
    const int row = blockIdx.y*blockDim.y + threadIdx.y;
    const int i   = col + row * w;

    if( ( col >= w ) || ( row >= h ) ) return; //checking if still within image bounds

    const uchar4 background = { 64, 0, 128, 0 };
    float3 source = { 0.0f, 0.0f, -zs };
    float3 pix    = srcIdxToPos( col, row, w, h, 2*volSize.z - zs );

    //apply viewing transformation: rotate around y-axis
    source = yRotate( source, theta );
    pix    = yRotate( pix, theta );

    //prepare inputs for ray-box intersection
    float t0, t1;
    const Ray pixRay = { source, pix - source };
    float3 center    = { volSize.x/2.0f, volSize.y/2.0f, volSize.z/2.0f };

    const float3 boxmin = -center;
    const float3 boxmax = { volSize.x - center.x, volSize.y - center.y, volSize.z - center.z };

    //perform ray-box intersection test
    const bool hitBox = intersectBox( pixRay, boxmin, boxmax, &t0, &t1 );
    uchar4 shade;
    if( !hitBox )
        shade = background;          // miss box -> background color
    else
    {
        if( t0 < 0.0f )  t0 = 0.0f;  // clamp to 0 to avoid looking backward

        //bounded by the points where the ray enters and leaves the box
        const Ray boxRay = { paramRay( pixRay, t0 ), paramRay( pixRay, t1 ) - paramRay( pixRay, t0 ) };

        if( method == 1 )
            shade = sliceShader( d_vol, volSize, boxRay, threshold, dist, source );
        else if ( method == 2 )
            shade = rayCastShader( d_vol, volSize, boxRay, threshold );
        else
            shade = volumeRenderShader( d_vol, volSize, boxRay, threshold, NUMSTEPS );
    }

    d_out[i] = shade;
}

/*__global__ void volumeKernel( float *d_vol, int3 volSize, int id, float4 params )
{
    const int w     = volSize.x, h = volSize.y, d = volSize.z;

    const int col   = blockIdx.x * blockDim.x + threadIdx.x;
    const int row   = blockIdx.y * blockDim.y + threadIdx.y;
    const int stack = blockIdx.z * blockDim.z + threadIdx.z;
    const int i     = col + row * w + stack * w * h;

    if ( ( col >= w ) || ( row >= h ) || ( stack >= d ) ) return;

    d_vol[i] = func( col, row, stack, id, volSize, params ); //computing and store the volume
}*/

__global__ void distanceTrkernel( float *d_frep, float *d_udf, int3 fieldSize )
{

}

__global__ void volumeKernel( float *d_sdf, float *d_vol, int3 volSize, int3 sdfSize )
{
    const int w = volSize.x, h = volSize.y, d = volSize.z;

    const int col   = blockIdx.x * blockDim.x + threadIdx.x;
    const int row   = blockIdx.y * blockDim.y + threadIdx.y;
    const int stack = blockIdx.z * blockDim.z + threadIdx.z;
    const int i     = col + row * w + stack * w * h;

    if ( ( col >= w ) || ( row >= h ) || ( stack >= d ) ) return;

    float3 k = make_float3( volSize.x / sdfSize.x, volSize.y / sdfSize.y, volSize.z / sdfSize.z );

    float3 pos = { float(col/k.x), float(row/k.y), float(stack/k.z)};
    d_vol[i] = interpolate_in_volume( d_sdf, sdfSize, pos ); //computing and store the volume
}

void kernelLauncher( uchar4 *d_out, float *d_vol, int w, int h, int3 volSize,
                     int method, int zs, float theta, float threshold, float dist )
{
    dim3 blockSize( TX_2D, TY_2D );
    dim3 gridSize( divUp( w, TX_2D ), divUp( h, TY_2D ) );
    renderKernel <<< gridSize, blockSize >>> ( d_out, d_vol, w, h, volSize, method, zs, theta, threshold, dist );
}

/*void volumeKernelLauncher( float *d_vol, int3 volSize, int id, float4 params )
{
    dim3 blockSize( TX, TY, TZ );
    dim3 gridSize( divUp( volSize.x, TX ), divUp( volSize.y, TY ), divUp( volSize.z, TZ ) );
    volumeKernel <<< gridSize, blockSize >>> ( d_vol, volSize, id, params );
}*/

void volumeKernelLauncher(float *h_sdf, float *d_sdf, float *d_vol, int3 volSize , int3 sdfSize)
{
    dim3 blockSize( TX, TY, TZ );
    dim3 gridSize( divUp( volSize.x, TX ), divUp( volSize.y, TY ), divUp( volSize.z, TZ ) );

    cudaMemcpy( d_sdf, h_sdf, float(sdfSize.x*sdfSize.y*sdfSize.z)*sizeof(float), cudaMemcpyHostToDevice );

    volumeKernel <<< gridSize, blockSize >>> ( d_sdf, d_vol, volSize, sdfSize );
}


void distanceTrKernelLauncher( float *h_frep, float *d_frep, float *d_udf, int3 fieldSize )
{
    dim3 blockSize ( TX, TY, TZ );
    dim3 gridSize( divUp( fieldSize.x, TX ), divUp( fieldSize.y, TY ), divUp( fieldSize.z, TZ ) );

    cudaMemcpy( d_frep, h_frep, float( fieldSize.x*fieldSize.y*fieldSize.z )*sizeof(float), cudaMemcpyHostToDevice );
    distanceTrkernel <<< gridSize, blockSize >>> ( d_frep, d_udf, fieldSize );
}

