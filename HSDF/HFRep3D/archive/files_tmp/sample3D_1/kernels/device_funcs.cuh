#ifndef DEVICEFUNCS_CUH
#define DEVICEFUNCS_CUH

#include <cuda_runtime.h>

typedef struct {
    float3 o, d;
} Ray;

__host__   int    divUp( int a, int b);

__device__ float3 xRotate( float3 pos, float theta );
__device__ float3 yRotate( float3 pos, float theta );
__device__ float3 zRotate( float3 pos, float theta );

//__device__ float  func( int col, int row, int stack, int id, int3 volSize, float4 params );
__device__ float interpolate_in_volume(float *d_sdf, int3 sdfSize , float3 pos);

__device__ float3 paramRay( Ray r, float t );
__device__ float3 srcIdxToPos( int col, int row, int w, int h, float sz);

__device__ bool intersectBox( Ray R, float3 boxmin, float3 boxmax, float *tnear, float *tfar );

__device__ uchar4 sliceShader( float *d_vol, int3 volsize, Ray boxray, float threshold, float dist, float3 norm );
__device__ uchar4 volumeRenderShader( float *d_vol, int3 volSize, Ray boxray, float dist, int numSteps );
__device__ uchar4 rayCastShader( float *d_vol, int3 volSize, Ray boxRay, float dist );

#endif
