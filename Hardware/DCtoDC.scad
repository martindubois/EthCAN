
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/DCtoDC.scad

use <C:/_VC/Base3D/Cylinder.scad>

DCtoDC();

DCtoDC_Base();

// Public
// //////////////////////////////////////////////////////////////////////////

module DCtoDC()
{
    color( "red" )
    {
        difference()
        {
            cube( [ SIZE_X, SIZE_Y, TICK ] );

            translate( [ SIZE_X - HOLE_X, HOLE_Y, - EPS ] )
                Cylinder_Z( 2 * EPS + TICK, HOLE_R, 20 );

            translate( [ HOLE_X, SIZE_Y - HOLE_Y, - EPS ] )
                Cylinder_Z( 2 * EPS + TICK, HOLE_R, 20 );
        }
    }
}

module DCtoDC_Base()
{
    translate( [ 0, 0, - DCtoDC_Base_Z() ] )
    {
        difference()
        {
            translate( [ 3, 0, 0 ] )
                cube( [ SIZE_X - 6, SIZE_Y, DCtoDC_Base_Z() ] );

            translate( [ 9, 2, - EPS ] )
                cube( [ SIZE_X - 18, SIZE_Y - 4, 2 * EPS + DCtoDC_Base_Z() ] );

            translate( [ SIZE_X - HOLE_X, HOLE_Y, - EPS ] )
                Cylinder_Z( 2 * EPS + DCtoDC_Base_Z(), PILOT_R, 20 );

            translate( [ HOLE_X, SIZE_Y - HOLE_Y, - EPS ] )
                Cylinder_Z( 2 * EPS + DCtoDC_Base_Z(), PILOT_R, 20 );
        }
    }
}

function DCtoDC_Base_Z() = 3;

// Private
// //////////////////////////////////////////////////////////////////////////

EPS = 0.1;

HOLE_R = 1.75;
HOLE_X = 6;
HOLE_Y = 2.5;

PILOT_R = 1.25;

SIZE_X = 43;
SIZE_Y = 21;

TICK = 1;
