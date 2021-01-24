#ifndef KERNEL_H
#define KERNEL_H
#include <cuda_runtime.h>

void kernelLauncher(uchar4 *dout, float *d_vol, int w, int h, int3 volSize,
                     int method, int zs, float theta, float threshold, float dist );

void volumeKernelLauncher( float *d_vol, int3 volSize, int id, float4 params );
void volumeKernelLauncher( float *h_sdf, float *d_sdf, float *d_vol, int3 volSize, int3 sdfSize );

void distanceTrKernelLauncher( float *h_frep, float *d_frep, float *d_udf, int3 fieldSize );

#endif
