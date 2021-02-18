#include "interiorDistanceField.h"
#include "frep2D.h"
#include "frep3D.h"
#include "timer.hpp"

#include <igl/readOFF.h>
#include <igl/readOBJ.h>
#include <igl/readMESH.h>
#include <igl/opengl/glfw/Viewer.h>

#include <iostream>
#include <functional>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gtsconfig.h>
#include <gts.h>

static void suriken(gdouble **f, GtsCartesianGrid g, guint k, gpointer data)
{
    gdouble x, y, z = g.z;
    guint i, j;
    hfrep2D::FRepObj2D frep0(g.nx, g.ny, 20);

   for (i = 0, x = g.x; i < g.nx; i++, x += g.dx)
      for (j = 0, y = g.y; j < g.ny; j++, y += g.dy)
        f[i][j] = -frep0.suriken(glm::f32vec2(x, y), glm::f32vec2(0, 0));
}

static void bat(gdouble **f, GtsCartesianGrid g, guint k, gpointer data)
{
    gdouble x, y, z = g.z;
    guint i, j;
    hfrep2D::FRepObj2D frep0(g.nx, g.ny, 20);

   for (i = 0, x = g.x; i < g.nx; i++, x += g.dx)
      for (j = 0, y = g.y; j < g.ny; j++, y += g.dy)
        f[i][j] = -frep0.bat(glm::f32vec2(x, y), glm::f32vec2(0, 0));
}

static void circle(gdouble **f, GtsCartesianGrid g, guint k, gpointer data)
{
    gdouble x, y, z = g.z;
    guint i, j;
    hfrep2D::FRepObj2D frep0(g.nx, g.ny, 6.4);

   for (i = 0, x = g.x; i < g.nx; i++, x += g.dx)
      for (j = 0, y = g.y; j < g.ny; j++, y += g.dy)
        f[i][j] = -frep0.circle(glm::f32vec2(x, y), glm::f32vec2(0,0), 1);
}

static void heart2d(gdouble **f, GtsCartesianGrid g, guint k, gpointer data)
{
    gdouble x, y, z = g.z;
    guint i, j;
    hfrep2D::FRepObj2D frep0(g.nx, g.ny, 6.4);

   for (i = 0, x = g.x; i < g.nx; i++, x += g.dx)
      for (j = 0, y = g.y; j < g.ny; j++, y += g.dy)
        f[i][j] = -frep0.heart2D(glm::f32vec2(x, y), glm::f32vec2(0,0));
}

static void heart3d(gdouble **f, GtsCartesianGrid g, guint k, gpointer data)
{
    gdouble x, y, z = g.z;
    guint i, j;
    hfrep3D::FRepObj3D frep(g.nx, g.ny, g.nz, 70);

   for (i = 0, x = g.x; i < g.nx; i++, x += g.dx)
      for (j = 0, y = g.y; j < g.ny; j++, y += g.dy)
        f[i][j] = -frep.heart3D(glm::f32vec3(x, y, z), glm::f32vec3(0, 0, 0));
}

static void sphere(gdouble ** f, GtsCartesianGrid g, guint k, gpointer data)
{
    gdouble x, y, z = g.z;
    guint i, j;
    hfrep3D::FRepObj3D frep(g.nx, g.ny, g.nz, 70);

    for (i = 0, x = g.x; i < g.nx; i++, x += g.dx)
        for (j = 0, y = g.y; j < g.ny; j++, y += g.dy)
            f[i][j] = -frep.sphere(glm::f32vec3(x, y, z), glm::f32vec3(0, 0, 0), 60);
}

int main(int argc, char *argv[])
{
    //*************************************************************************
    //I. First example dedicated to computing IDF for the polygon defined using
    //   vertices and edges;
    //*************************************************************************
    // Create the boundary of a square
    Eigen::MatrixXd V(46, 2);
    Eigen::MatrixXi E(46, 2);

    V << -2,-2,  -1.5,-2,  -1,-2,  -0.5,-2,  -0.5,-1.5,  -0.5,-1,  -0.25,-1,  0,-1,
          0.25,-1,  0.5,-1,  0.5,-1.5,  0.5,-2,  1,-2,  1.5,-2,  2,-2,
          2,-1.5,  2,-1,  2,-0.5,  2,0,  2,0.5,  2,1,  2,1.5,  2,2,
          1.5,2,  1,2,  0.5,2,  0.5,1.5,  0.5,1,  0.5,0.25,  0.25,0.25,  0,0.25,
          -0.25,0.25,  -0.5,0.25,  -0.5,1,  -0.5,1.5,  -0.5,2,
          -1.0,2,  -1.5,2,  -2,2,  -2,1.5,  -2,1,  -2,0.5, -2,0,  -2,-0.5,  -2,-1,  -2,-1.5;
    E << 0,1, 1,2, 2,3, 3,4,   4,5,   5,6,
         6,7, 7,8, 8,9, 9,10, 10,11, 11,12,
         12,13, 13,14, 14,15, 15,16, 16,17,
         17,18, 18,19, 19,20, 20,21, 21,22,
         22,23, 23,24, 24,25, 25,26, 26,27,
         27,28, 28,29, 29,30, 30,31, 31,32,
         32,33, 33,34, 34,35, 35,36, 36,37,
         37,38, 38,39, 39,40, 40,41, 41,42,
         42,43, 43,44, 44,45, 45,0;

    Eigen::MatrixXd V1;
    Eigen::MatrixXi E1;

    V1.resize(15, 2);
    E1.resize(15, 2);

    V1 << 0,2,  2,2,  2,0,  0,-2,  -2,0,  0,1,  1,1,  1,0,  0,0,
          0,0.5,  -1,0,  0, -1.5,  1.5,0,  1.5,1.5,  0, 1.5;
    E1 << 0,1,  1,2,  2,3,  3,4,  4,5,  5,6,  6,7,  7,8,
          8,9,  9,10,  10,11,  11,12,  12,13,  13,14,  14,0;

    //computing IDF
    idf::IDFdiffusion IDF;
    Eigen::Vector2d srcP0(0.5, 0.5);  // -1.2, 1
    //IDF.computeIDF_polygon2D(V1, E1, srcP0, 7, 0.0085);
    //IDF.plotDiffusionMap();
    //IDF.plotIDF2D(30);

    Eigen::MatrixXd Vm_in;
    Eigen::MatrixXi Fm_in, Tm_in;
    //igl::readOFF("fertility.off", Vm_in, Fm_in);
    //igl::readMESH("octopus-low.mesh", Vm_in, Tm_in, Fm_in);
    //igl::readOBJ("turtle.obj", Vm_in, Fm_in);
    Eigen::Vector3d srcP1(0, 0, 0.0); // 0, 2.5, 0.5

    //IDF.computeIDF_mesh3D(Vm_in/50.0, Fm_in, srcP1,7, 0.01); // 30 * Vm_in
    //IDF.plotDiffusionMap();
    //IDF.plotIDF3D(100);

    //IDF.setIsolinesNumber(50);
    //IDF.computeIDF_slice(Vm_in/50, Fm_in, srcP1, 7, 0.01); // 30 * Vm_in
    //******************************************************************************************
    //II. This example is dedicated to computing IDFs using function representation (FRep) in 2D;
    //******************************************************************************************

    Eigen::Vector2d srcP2(0.0, 0.0);
    Eigen::MatrixXd H;
    prof::timer Timer;
    Timer.Start();
    IDF.computeIDF_polygon2D(c_shape, Eigen::Vector3i(256, 256, 256), srcP2, 7, 0.1, H); //7, 0.1
    //Timer.End("2D IDF comp.: ");
    //IDF.plotDiffusionMap();
    IDF.plotIDF2D(30);

    //************************************************************************
    //III. This example is dedicated to computing of the IDFs using FRep in 3D
    //************************************************************************

    Eigen::Vector3d srcP3(0.0, 0.0, 0.0);
    //IDF.computeIDF_mesh3D(heart3d, srcP3, Eigen::Vector3i(128, 128, 128), 0.0, 7, 6); // 7, 0.1
    //IDF.plotDiffusionMap();
    //IDF.plotIDF3D(70);

    //IDF.setIsolinesNumber(30);
    //IDF.computeIDF_slice(heart3d, srcP3, Eigen::Vector3i(128, 128, 128), 0.0, 7, 0.1);

    return 0;
}
