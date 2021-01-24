#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#include "kernels/kernel.h"

#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime_api.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include <vector_types.h>
#include <cmath>

#define W 600
#define H 600
#define DELTA 5
#define NX 128
#define NY 128
#define NZ 128

int id     = 1;  // 0 - sphere, 1 - torus, 2 - block
int method = 2;  // 0 - volumeRender; 1 - slice; 2 - raycast
int2 loc   = { W/2, H/2 };
bool dragMode = false;

const int3 volumeSize = { NX, NY, NZ }; // size of volumetric data grid
const float4 params   = { NX/4.0f, NY/6.f, NZ/16.f, 1.f };

float *d_vol;     // pointer to device array for storing volume data
float *d_sdf;

float zs   = NZ;  // distance from origin to source
float dist = 0.f;
float theta = 0.f;
float threshold = 0.f;

void mymenu( int value )
{
    switch ( value )
    {
        case 0: return;
        case 1: id = 0; break; // sphere
    }
    //volumeKernelLauncher( d_vol, volumeSize, id, params );
    glutPostRedisplay();
}

void createMenu()
{
    glutCreateMenu( mymenu );                 // object selection menu
    glutAddMenuEntry( "Object selector", 0 ); // menu title
    glutAddMenuEntry( "Sphere", 1 );          // id = 1, sphere
    glutAddMenuEntry( "Torus",  2 );          // id = 2, torus
    glutAddMenuEntry( "Bllock", 3 );          // id = 3, block
    glutAttachMenu( GLUT_RIGHT_BUTTON );      // right-click for menu
}

void keyboard( unsigned char key, int x, int y )
{
    if( key == 27  ) exit( 0 );     // exit the app
    if( key == '+' ) zs -= DELTA;   // move source closer  (zoom in)
    if( key == '-' ) zs += DELTA;   // move source farther (zoom out)
    if( key == 'd' ) --dist;        // decrease slice distance
    if( key == 'D' ) ++dist;        // increase slice distance
    if( key == 'z' ) zs = NZ, theta = 0.f, dist = 0.f; // reset values
    if( key == 'v' ) method = 0;    // volume render
    if( key == 's' ) method = 1;    // slice
    if( key == 'r' ) method = 2;    // raycast
    glutPostRedisplay();
}

void handleSpecialKeypress( int key, int x, int y)
{
    if( key == GLUT_KEY_LEFT )  theta -= 0.1f;     // rotate left
    if( key == GLUT_KEY_RIGHT ) theta += 0.1f;     // rotate right
    if( key == GLUT_KEY_UP )    threshold += 0.1f; // increase threshold (thickness)
    if( key == GLUT_KEY_DOWN )  threshold -= 0.1f; // decrease threshold (thinness)
    glutPostRedisplay();
}

void mouseDrag(int x, int y)
{
    if( !dragMode ) return;
    loc.x  = x;
    loc.y  = y;
    if( x > W/2 || y > H/2 )
        theta += 0.1f;
    else
        theta -= 0.1f;

    glutPostRedisplay();
}

void printInstructions()
{
    printf( "3D volume visualizer. \n" );
    printf( "COntrols: \n" );
    printf( "Volume render mode:        v\n" );
    printf( "Slice render mode:         s\n" );
    printf( "Raycast mode:              r\n" );
    printf( "Zoom out/in:               -/+\n" );
    printf( "Rotate view:               left/right\n" );
    printf( "Decr./Incr. offset \n "
            "(intensity in slice mode): down/up\n" );
    printf( "Decr./Incr. distance \n "
            "(only in slice mode):      d/D\n" );
    printf( "Reset paameters:           z\n" );
    printf( "Right-click for object selection menu \n" );
}

#endif
