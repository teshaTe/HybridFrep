#include "include/ddt3D.h"

namespace distance_transform_3D {

distanceTransform3D::distanceTransform3D(int gridW, int gridH, int gridD): voxGrW( gridW ), voxGrH( gridH ), voxGrD( gridD ),
                                                                           INF(calcINFval()), INSIDE( 0.0f, 0.0f, 0.0f ),
                                                                           OUTSIDE( INF, INF, INF ),
                                                                           vGrid1( gridW*gridH*gridD, INSIDE),
                                                                           vGrid2( gridW*gridH*gridD, INSIDE),
                                                                           DDT3D(  gridW*gridH*gridD, 0.0f),
                                                                           SDDT3D( gridW*gridH*gridD, 0.0f)
{ }

void distanceTransform3D::caclulateDiscreteDistanceTransform(std::vector<float> *inField)
{
    initializeVoxelGrid( inField );

    traverseVoxelGridForwardPass( vGrid1 );
    traverseVoxelGridBackwardPass( vGrid1 );

    traverseVoxelGridForwardPass( vGrid2 );
    traverseVoxelGridBackwardPass( vGrid2 );

    mergeVoxGrids();
}

void distanceTransform3D::initializeVoxelGrid(std::vector<float> *field )
{
    for ( int z = 0; z < voxGrD ; z++ )
    {
        for ( int y = 0; y < voxGrH; y++ )
        {
            for ( int x = 0; x < voxGrW; x++ )
            {
                int ind = index(x,y,z);
                if( field->at(ind) >= 0.0f )
                {
                    fillVoxGridElement( vGrid1, INSIDE,  ind );
                    fillVoxGridElement( vGrid2, OUTSIDE, ind );
                }
                else
                {
                    fillVoxGridElement( vGrid1, OUTSIDE, ind );
                    fillVoxGridElement( vGrid2, INSIDE,  ind );
                }
            }
        }
    }
}

void distanceTransform3D::compareVoxelVectors( std::vector<Point3D> &voxGrid, Point3D &curPoint,
                                             int offsetX, int offsetY, int offsetZ,
                                             int x, int y, int z )
{
    if( x+offsetX >= 0 && x+offsetX < voxGrW &&
        y+offsetY >= 0 && y+offsetY < voxGrH &&
        z+offsetZ >= 0 && z+offsetZ < voxGrD )
    {
        int ind = index( x+offsetX, y+offsetY, z+offsetZ );
        Point3D otherPoint = getVoxGridElement( voxGrid, ind );
        otherPoint.dx += offsetX;
        otherPoint.dy += offsetY;
        otherPoint.dz += offsetZ;

        if( otherPoint.EuclideanDist() < curPoint.EuclideanDist() )
            curPoint = otherPoint;
    }
}

void distanceTransform3D::traverseVoxelGridForwardPass(std::vector<Point3D> &grid)
{
    for ( int z = 0; z < voxGrD; z++ )
    {
        for ( int y = 0; y < voxGrH; y++ )
        {
            for ( int x = 0; x < voxGrW; x++ )
            {
                //F - forward mask shifting + x,y,z - allong which axe
                int indFxyz   = index( x, y, z );
                Point3D point = getVoxGridElement( grid, indFxyz );
                compareVoxelVectors( grid, point, -1,  0,  0, x, y, z );
                compareVoxelVectors( grid, point,  0,  0, -1, x, y, z );
                compareVoxelVectors( grid, point,  0, -1,  0, x, y, z );
                fillVoxGridElement( grid, point, indFxyz );
            }

            for ( int x =  voxGrW-1; x >= 0; x--)
            {
                //F - forward mask shifting, B - backward mask shifting + x,y,z - allong which axe
                int indBxFyz  = index(x, y, z);
                Point3D point = getVoxGridElement( grid, indBxFyz );
                compareVoxelVectors( grid, point, 0, -1, 0, x, y, z );
                compareVoxelVectors( grid, point, 1,  0, 0, x, y, z );
                fillVoxGridElement( grid, point, indBxFyz );
            }
        }
    }

    for ( int z = 0; z < voxGrD; z++ )
    {
        for ( int y = voxGrH-1; y >= 0; y-- )
        {
            for ( int x =  voxGrW-1; x >= 0; x-- )
            {
                //F - forward mask shifting + x,y,z - allong which axe
                int indByFxz  = index( x, y, z );
                Point3D point = getVoxGridElement( grid, indByFxz );
                compareVoxelVectors( grid, point, 0, 1, 0, x, y, z );
                compareVoxelVectors( grid, point, 1, 0, 0, x, y, z );
                fillVoxGridElement( grid, point, indByFxz );
            }

            for ( int x = 0; x < voxGrW; x++ )
            {
                //F - forward mask shifting, B - backward mask shifting + x,y,z - allong which axe
                int indBxyFz  = index(x, y, z);
                Point3D point = getVoxGridElement( grid, indBxyFz );
                compareVoxelVectors( grid, point,  0, 1, 0, x, y, z );
                compareVoxelVectors( grid, point, -1, 0, 0, x, y, z );
                fillVoxGridElement( grid, point, indBxyFz );
            }
        }
    }
}

void distanceTransform3D::traverseVoxelGridBackwardPass(std::vector<Point3D> &grid)
{
    for ( int z = voxGrD-1; z >= 0; z-- )
    {
        for ( int y = voxGrH-1; y >= 0 ; y-- )
        {
            for ( int x = voxGrW-1; x >= 0 ; x-- )
            {
                //F - forward mask shifting + x,y,z - allong which axe
                int indBxyz   = index( x, y, z );
                Point3D point = getVoxGridElement( grid, indBxyz );
                compareVoxelVectors( grid, point, 0, 1, 0, x, y, z );
                compareVoxelVectors( grid, point, 1, 0, 0, x, y, z );
                compareVoxelVectors( grid, point, 0, 0, 1, x, y, z );
                fillVoxGridElement( grid, point, indBxyz );
            }

            for ( int x = 0; x < voxGrW; x++)
            {
                //F - forward mask shifting, B - backward mask shifting + x,y,z - allong which axe
                int indByzFx  = index(x, y, z);
                Point3D point = getVoxGridElement( grid, indByzFx );
                compareVoxelVectors( grid, point,  0, 1, 0, x, y, z );
                compareVoxelVectors( grid, point, -1, 0, 0, x, y, z );
                fillVoxGridElement( grid, point, indByzFx );
            }
        }
    }

    for ( int z = voxGrD-1; z >= 0; z-- )
    {
        for ( int y = 0; y < voxGrH; y++ )
        {
            for ( int x = 0; x < voxGrW; x++ )
            {
                //F - forward mask shifting + x,y,z - allong which axe
                int indBzFxy  = index( x, y, z );
                Point3D point = getVoxGridElement( grid, indBzFxy );
                compareVoxelVectors( grid, point, -1,  0, 0, x, y, z );
                compareVoxelVectors( grid, point,  0, -1, 0, x, y, z );
                fillVoxGridElement( grid, point, indBzFxy );
            }

            for ( int x = voxGrW-1; x >= 0 ; x--)
            {
                //F - forward mask shifting, B - backward mask shifting + x,y,z - allong which axe
                int indBxzFy  = index(x, y, z);
                Point3D point = getVoxGridElement( grid, indBxzFy );
                compareVoxelVectors( grid, point,  0, -1, 0, x, y, z );
                compareVoxelVectors( grid, point,  1,  0, 0, x, y, z );
                fillVoxGridElement( grid, point, indBxzFy );
            }
        }
    }
}

void distanceTransform3D::mergeVoxGrids()
{
    for ( int z = 0; z < voxGrD; z++ )
    {
        for ( int y = 0; y < voxGrH; y++ )
        {
            for ( int x = 0; x < voxGrW; x++ )
            {
                int ind     = index(x,y,z);
                float dist1 = std::sqrt( getVoxGridElement( vGrid1, ind ).EuclideanDist() );
                float dist2 = std::sqrt( getVoxGridElement( vGrid2, ind ).EuclideanDist() );
                float dist  = (dist2 - dist1)*10.0f/INF;
                DDT3D[ind]  = std::abs( dist );
                SDDT3D[ind] = dist;
            }
        }
    }
}

} //namespace distance_transform_3D
