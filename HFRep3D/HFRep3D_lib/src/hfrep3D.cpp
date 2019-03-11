#include <include/hfrep3D.h>

#include <cmath>
#include <iostream>


namespace hfrep3D {

HFRep3D::HFRep3D(int gridW, int gridH, int gridD)
{
    voxGrW = gridW;
    voxGrH = gridH;
    voxGrD = gridD;

    DT3D          = std::make_shared<distance_transform_3D::distanceTransform3D>( gridW, gridH, gridD );
    interpolField = std::make_shared<interpolation::interpolate>( gridW, gridH, gridD );
}

void HFRep3D::calculateHFRep3D( std::vector<float> *frep3D, int stepFunc, float slope, bool checkRep )
{
    DDT.clear();
    SDDT.clear();
    SmoothDDT.clear();
    FRepObj.clear();

    FRepObj = *frep3D;
    DT3D.get()->caclulateDiscreteDistanceTransform( &FRepObj );
    DDT  = DT3D.get()->getDDT();
    SDDT = DT3D.get()->getSignedDDT();

    interpolField.get()->interpolateField( &DDT, TRICUBIC );
    SmoothDDT = interpolField.get()->getField();
    generateHFRepObject( stepFunc, slope );

    if( checkRep )
        checkHFrep();
}

float HFRep3D::getStepFunVal( float frep_val, int function, float slope )
{
    if( function == 1 )
    {
        float result = std::tanh( slope * frep_val ); //100000.0
        return result;
    }
    else if ( function == 2 )
    {
        float result = 2.0f/(1.0f+std::exp(-2.0f*frep_val/slope)) - 1.0f;
        return result;
    }
    else if ( function == 3 )
    {
        float result = frep_val / std::sqrt( slope + frep_val * frep_val );
        return result;
    }
    else if ( function == 4 )
    {
        float result = 0.642129f*std::atan( std::sinh( slope * frep_val ) ); //1000
        return result;
    }
    else
    {
        std::cerr << "ERROR: unknown option! [ get_step_function_val(.., <?> ) ]" << std::endl;
        return -1;
    }
}

void HFRep3D::generateHFRepObject(int stepFunction, float slope )
{
    HFRepObj.clear();

    for ( int z = 0; z < voxGrD; z++ )
    {
        for ( int y = 0; y < voxGrH; y++ )
        {
            for ( int x = 0; x < voxGrW; x++)
            {
                int i  = index( x, y, z );
                float val = getStepFunVal( FRepObj[i], stepFunction, slope ) * SmoothDDT[i]; //TODO: change back to smoothDDT after smoothing is implemented
                HFRepObj.push_back( val );
            }
        }
    }
}

bool HFRep3D::checkHFrep()
{
    int frep_zeros    = 0;
    int hfrep_zeros   = 0;
    int dist_tr_zeros = 0;

    for( int i = 0; i < static_cast<int>( voxGrW*voxGrH*voxGrD ); i++ )
    {
        if( FRepObj[i] == 0.0f )
            frep_zeros++;
        if( HFRepObj[i] == 0.0f )
            hfrep_zeros++;
        if( SDDT[i] == 0.0f )
            dist_tr_zeros++;
    }

    std::cout << "Checking HFRep for additional zeroes." << std::endl;
    if(frep_zeros == hfrep_zeros)
    {
        std::cout << "HFrep does not have additional zeros!" << std::endl;
        std::cout << "FRep zeros: " << frep_zeros << "   HFRep zeros: " << hfrep_zeros << std::endl;
        return true;
    }
    else
    {
        std::cout << "HFRep has additional zeros: " << hfrep_zeros - frep_zeros << std::endl;
        std::cout << "FRep zeros: "  << frep_zeros    << "   HFRep zeros: " << hfrep_zeros <<
                     "   DT zeros: " << dist_tr_zeros << std::endl;
        return false;
    }
}

}//namespace function_rep_3D
