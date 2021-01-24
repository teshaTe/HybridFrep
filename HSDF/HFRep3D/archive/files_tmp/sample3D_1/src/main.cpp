#include "include/interactions.h"
#include "kernels/kernel.h"

#include <stdlib.h>
#include <stdio.h>
#include <cmath>

#ifdef __APPE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#endif

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#define TITLE_STRING "Volume rendering"

//texture and pixel objects
GLuint pbo = 0;    // OpenGL pixel buffer object
GLuint tex = 0;    // OpenGL texture object
struct cudaGraphicsResource *cuda_pbo_resource;

void render()
{
    uchar4 *d_out = 0;
    cudaGraphicsMapResources( 1, &cuda_pbo_resource, 0 );
    cudaGraphicsResourceGetMappedPointer( (void **)&d_out, NULL, cuda_pbo_resource );
    kernelLauncher( d_out, d_vol, W, H, volumeSize, method, zs, theta, threshold, dist);
    cudaGraphicsUnmapResources( 1, &cuda_pbo_resource, 0 );
    char title[128];
    sprintf( title, "Volume Visualizer: objId = %d, method = %d, dist = %.1f, theta = %.1f", id, method, dist, theta );
    glutSetWindowTitle( title );
}

void drawTexture()
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0, 0 );
    glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, H );
    glTexCoord2f( 1.0f, 1.0f ); glVertex2f( W, H );
    glTexCoord2f( 1.0f, 0.0f ); glVertex2f( W, 0 );
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void display()
{
    render();
    drawTexture();
    glutSwapBuffers();
}

void InitGLUT( int *argc, char **argv )
{
    glutInit( argc, argv );                         //initialization of the library
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE ); // setting up display mode
    glutInitWindowSize( W, H );                     // setting up window size
    glutCreateWindow( TITLE_STRING );               // setting up the name of the window
#ifndef __APPLE__
    glewInit();
#endif
}

void initPixelBuffer()
{
    glGenBuffers( 1, &pbo );                                              //setting up pixel buffer object
    glBindBuffer( GL_PIXEL_UNPACK_BUFFER, pbo );
    glBufferData( GL_PIXEL_UNPACK_BUFFER, 4*W*H*sizeof (GLubyte), 0, GL_STREAM_DRAW ); //filling buffer with init data
    glGenTextures( 1, &tex );                                             //setting up texture
    glBindTexture( GL_TEXTURE_2D, tex);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );  //setting up texture paameters
    cudaGraphicsGLRegisterBuffer( &cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard ); //register resource on gpu
}

void exitfunc()
{
    if(pbo)
    {
        cudaGraphicsUnregisterResource( cuda_pbo_resource );
        glDeleteBuffers  ( 1, &pbo );
        glDeleteTextures ( 1, &tex );
    }
    cudaFree( d_vol );
    cudaFree( d_sdf );
}

void function(float *vol, int3 volSize, int3 pos0, int id )
{
    switch(id)
    {
        case 0: return;
        case 1: // heart
        {
            for( int i = 0; i < volSize.x; i++ )
            {
                for( int j = 0; j < volSize.y; j++ )
                {
                    for( int k = 0; k < volSize.z; k++ )
                    {
                        float x = ( i - pos0.x ) / float( volSize.x ),
                              y = ( j - pos0.y ) / float( volSize.y ),
                              z = ( k - pos0.z ) / float( volSize.z );
                        float result = std::pow( 2.0f*x*x + y*y + z*z - 1.0f, 3.0f ) -
                                       x*x*z*z*z / 10.0f - y*y*z*z*z;
                        vol[i + j*volSize.x + k*volSize.y*volSize.z] = result;
                    }
                }
            }
        } break;
        case 2: //sphere
        {
        for( int i = 0; i < volSize.x; i++ )
        {
            for( int j = 0; j < volSize.y; j++ )
            {
                for( int k = 0; k < volSize.z; k++ )
                {
                    float x = ( i - pos0.x ) / float( volSize.x ),
                          y = ( j - pos0.y ) / float( volSize.y ),
                          z = ( k - pos0.z ) / float( volSize.z );
                    vol[i + j*volSize.x + k*volSize.y*volSize.z] = sqrtf( x*x + y*y + z*z ) - 0.36f;
                }
            }
        }
        }
    }
}

int main(int argc, char** argv)
{
    cudaMalloc( &d_vol, NX*NY*NZ*sizeof(float) ); //3D volume data
    cudaMalloc( &d_sdf, 64*64*64*sizeof(float) );
    float *h_sdf = static_cast<float*> (calloc( 64*64*64, sizeof (float) ));

    int3 sdfSize = { 64, 64, 64};
    const int3 pos0 = { sdfSize.x/2, sdfSize.y/2, sdfSize.z/2 };

    function( h_sdf, sdfSize, pos0, 2);

    volumeKernelLauncher( h_sdf, d_sdf, d_vol, volumeSize, sdfSize );

    printInstructions();
    InitGLUT( &argc, argv );
    createMenu();
    gluOrtho2D( 0, W, H, 0 );                 // viewing transform, orthographic projection

    glutKeyboardFunc( keyboard );
    glutSpecialFunc( handleSpecialKeypress );
    glutMotionFunc( mouseDrag );

    glutDisplayFunc( display );
    initPixelBuffer();
    glutMainLoop();

    atexit( exitfunc );
    free(h_sdf);

    return 0;
}
