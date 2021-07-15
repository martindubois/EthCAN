
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/CAN-BUS.scad

use <C:/_VC/Base3D/Cylinder.scad>

// CAN_BUS();

CAN_BUS_Base();

// Public
/////////////////////////////////////////////////////////////////////////////

module CAN_BUS()
{
    color( "blue" )
    {
        cube( [ SIZE_X, SIZE_Y, TICK ] );

        translate( [ SIZE_X, CENTER_Y, 0 ] )
            Cylinder_Z( TICK, SCREW_R, 30 );

        for ( y = [ 0, SIZE_Y ] )
        {
            translate( [ SCREW_X, y, 0 ] )
                Cylinder_Z( TICK, SCREW_R, 30 );
        }

        // Groove
        translate( [ 33, CENTER_Y - 5, TICK ] )
            cube( [ 5, 10, 8 ] );

        // Connector
        translate( [ 3, CENTER_Y - 3.5, TICK ] )
            cube( [ 7, 7, 8 ] );
    }
}

module CAN_BUS_Base()
{
    translate( [ 0, 0, - CAN_BUS_Base_Z() ] )
    {
        difference()
        {
            translate( [ 0, - 2.5, 0 ] )
                cube( [ SIZE_X + 3, SIZE_Y + 5, CAN_BUS_Base_Z() ] );

            translate( [ 3, 1, - EPS ] )
                cube( [ SIZE_X - 4, SIZE_Y - 4, 2 * EPS + CAN_BUS_Base_Z() ] );

            translate( [ 10, 0, - EPS ] )
                cube( [ 17, EPS + 1, 2 * EPS + CAN_BUS_Base_Z() ] );

            for ( y = [ 0, SIZE_Y ] )
            {
                translate( [ 10, y, - EPS ] )
                    Cylinder_Z( 2 * EPS + CAN_BUS_Base_Z(), PILOT_R, 20 );
            }

            translate( [ SIZE_X, SIZE_Y / 2, - EPS ] )
                Cylinder_Z( 2 * EPS + CAN_BUS_Base_Z(), PILOT_R, 20 );
        }
    }
}

function CAN_BUS_Base_Z() = 3;

module CAN_BUS_Bottom()
{
    translate( [ - SPACE, - SPACE, 0 ] )
        cube( [ 2 * SPACE + SIZE_X, 2 * SPACE + SIZE_Y, 1 + EPS ] );

    // Connector
    translate( [ - 2 - EPS, CENTER_Y - 4.5, TICK ] )
        cube( [ 12 + EPS, 14, 8 ] );

    translate( [ 1, 1, - 3 ] )
        cube( [ SIZE_X - 2, SIZE_Y - 2, 3 + EPS ] );

    // Soldering
    translate( [ 11, 0, -3 ] )
        cube( [ 18, 1 + EPS, 3 + EPS ] );

    translate( [ SIZE_X, CENTER_Y, 0 ] )
        Cylinder_Z( 1 + EPS, SCREW_R + SPACE, 30 );

    for ( y = [ 0, SIZE_Y ] )
    {
        translate( [ SCREW_X, y, 0 ] )
            Cylinder_Z( 1 + EPS, SCREW_R + SPACE, 30 );
    }
}

module CAN_BUS_Top()
{
    translate( [ 13, 1, 0 ] )
        cube( [ SIZE_X - 14, SIZE_Y - 2, 17.5 ] );

    translate( [ 11 - EPS, CENTER_Y - 7.5, 0 ] )
        cube( [ 2 * EPS + 2, 16.5, 3 ] );

    // Connector
    translate( [ - 3 - EPS, CENTER_Y - 4.5, 0 ] )
        cube( [ 14 + EPS, 14, 20 + EPS ] );

    translate( [ - SPACE, - SPACE, 0 ] )
        cube( [ 2 * SPACE + SIZE_X, 2 * SPACE + SIZE_Y, TICK ] );

    translate( [ SIZE_X, CENTER_Y, 0 ] )
        Cylinder_Z( TICK, SCREW_R + SPACE, 30 );

    for ( y = [ 0, SIZE_Y ] )
    {
        translate( [ SCREW_X, y, 0 ] )
            Cylinder_Z( TICK, SCREW_R + SPACE, 30 );
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

EPS = 0.1;

SIZE_X = 41;
SIZE_Y = 20.5;

CENTER_Y = SIZE_Y / 2;

PILOT_R = 0.75;
SCREW_R =  2.2;
SCREW_X = 10;

SPACE = 0.1;

TICK = 1.5;
