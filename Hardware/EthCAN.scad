
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/EthCan.scad

use <C:/_VC/Base3D/Cylinder.scad>
use <C:/_VC/Base3D/Metric/M3.scad>
use <C:/_VC/Base3D/SAE/Screw8.scad>

use <CAN-BUS.scad>
use <ESP-32-POE-ISO.scad>

CFG_BOARDS = 1;
CFG_MOUNT  = 1;

if ( 0 != CFG_BOARDS )
{
    translate( [ 3, 7, 10 ] ) CAN_BUS();
    translate( [ 9, 3,  3 ] ) ESP_32_POE_ISO();
}

Bottom();

Top();

// Public
/////////////////////////////////////////////////////////////////////////////

module Bottom()
{
    difference()
    {
        union()
        {
            if ( 0 != CFG_MOUNT )
                Mount();

            cube( [ SIZE_X, SIZE_Y, TICK ] );

            translate( [ 0, 0, TICK ] )
            {
                // Sides
                for ( y = [ 0, SIZE_Y - 4 ] )
                    Cube( 0, y, SIZE_X, 4, 1 );

                Cube( SIZE_X - 7, 4, 7, SIZE_Y - 8, 1 ); // Back
                Cube( 0, 4, 10, SIZE_Y - 8, 1 ); // Front
            }

            translate( [ 0, 0, TICK + 1 ] )
            {
                // Sides
                cube( [ 68, 3 - SPACE, 1 ] );
                Cube( 78,          0        , SIZE_X - 78, 3 - SPACE, 1 );
                Cube(  0, SIZE_Y - 3 + SPACE, SIZE_X     , 3 - SPACE, 1 );

                Cube( SIZE_X - 6, 3 - SPACE, 6, SIZE_Y - 6 + 2 * SPACE, 1 ); // Back
                Cube(          0, 3 - SPACE, 9, SIZE_Y - 6 + 2 * SPACE, 1 ); // Front
            }

            translate( [ 0, 0, TICK + 2 ] )
            {
                // Sides
                cube( [ 68, TICK - SPACE, 3 ] );
                Cube( 78,                     0, SIZE_X - 78, TICK - SPACE, 3 );
                Cube(  0, SIZE_Y - TICK + SPACE, SIZE_X     , TICK - SPACE, 3 );

                // Back
                Wall_YZ( SIZE_X - TICK,          TICK     - SPACE, 11, 3 );
                Wall_YZ( SIZE_X - TICK, SIZE_Y - TICK - 3 + SPACE,  3, 3 );

                Wall_YZ( 0, TICK - SPACE, SIZE_Y - 2 * TICK + 2 * SPACE, 3 ); // Front
            }
        }

        Holes_Bottom();

        if ( 0 != CFG_MOUNT )
            Mount_Holes();
    }
}

module Top()
{
    color( "gray" )
    {
        difference()
        {
            union()
            {
                translate( [ 0, 0, SIZE_Z - TICK ] ) cube( [ SIZE_X, SIZE_Y, TICK ] );

                translate( [ 0, 0, 5 ] )
                {
                    // Sides
                    Wall_XZ( TICK + SPACE,              TICK,     68 -     TICK      -     SPACE, SIZE_Z - TICK - 5 );
                    Wall_XZ(           78,              TICK, SIZE_X -     TICK - 78 -     SPACE, SIZE_Z - TICK - 5 );
                    Wall_XZ( TICK + SPACE, SIZE_Y - 2 * TICK, SIZE_X - 2 * TICK      - 2 * SPACE, SIZE_Z - TICK - 5 );
                }

                translate( [ 0, 0, 7 ] )
                {
                    // Sides
                    for ( y = [ 0, SIZE_Y - TICK ] )
                        Wall_XZ( 0, y, SIZE_X, SIZE_Z - 7 - TICK );

                    // Back
                    Wall_YZ( SIZE_X - TICK,          TICK            , 11 - SPACE, 11 );
                    Wall_YZ( SIZE_X - TICK, SIZE_Y - TICK - 3 + SPACE,  3 - SPACE, 11 );

                    // Front
                    Wall_YZ( 0, TICK, SIZE_Y - 2 * TICK, 4 );
                }

                translate( [ 0, 0, 11 ] )
                {
                    // Front
                    Cube( 0, TICK + 10, TICK + 2, SIZE_Y - 2 * TICK - 20, TICK );
                    for ( y = [ 10, 22 ] )
                        Wall_XZ( 0, y, 14, SIZE_Z - TICK - 11 );

                    for ( y = [ TICK, 24 ] )
                        Cube( 0, y, 8, 8, SIZE_Z - TICK - 11 );

                    // Columns
                    translate( [ 43, SIZE_Y / 2, 0 ] ) Cylinder_Z( 12, 2, 30 );

                    for ( y = [ 7, 27 ] )
                        translate( [ 13, y, 0 ] ) Cylinder_Z( 12, 2, 30 );
                }

                translate( [ 0, 0, 18 ] )
                {
                    Cube( SIZE_X - 8, TICK, 8, SIZE_Y - 2 * TICK, 5 ); // Back
                    Wall_YZ( 14, 10, 12, 5 ); // Front
                }
            }

            translate( [ - EPS, 12, SIZE_Z - TICK - EPS ] ) cube( [ 14, 10, 2 * EPS + TICK ] );

            Holes_Top();
        }
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

module Cube( aX, aY, aSizeX, aSizeY, aSizeZ )
{
    translate( [ aX, aY, 0 ] )
        cube( [ aSizeX, aSizeY, aSizeZ ] );
}

module Holes_Bottom()
{
    for ( y = [ 4, SIZE_Y - 4 ] )
    {
        translate( [ 4, y, - EPS ] )
        {
            M3_Shank_Z( 2 * EPS + 7 );
            M3_Insert_ZR();
        }
    }

    translate( [ SIZE_X - 4, 10, - EPS ] )
    {
        M3_Shank_Z( 2 * EPS + 7 );
        M3_Insert_ZR();
    }
}

module Holes_Top()
{
    for ( y = [ 4, SIZE_Y - 4 ] )
    {
        translate( [ 4, y, 5 - EPS    ] ) M3_Shank_Z( 2 * EPS + SIZE_Z - 5 );
        translate( [ 4, y, SIZE_Z - 4 ] ) M3_Head_Z( 4 + EPS );
    }

    translate( [ SIZE_X - 4, 10, 7 - EPS    ] ) M3_Shank_Z( 2 * EPS + SIZE_Z - 7 );
    translate( [ SIZE_X - 4, 10, SIZE_Z - 4 ] ) M3_Head_Z( 4 + EPS );
}

module Mount()
{
    Wall_XY( 20, - 15, 30, 15 );

    for ( x = [ 20, 50, SIZE_X - 20 ] )
    {
        translate( [ x, 0, 0 ] )
            Cylinder_Z( TICK, 15, 50 );
    }

    Wall_XY( 20, SIZE_Y, SIZE_X - 40, 15 );
    
    for ( x = [ 20, SIZE_X - 20 ] )
    {
        translate( [ x, SIZE_Y, 0 ] )
            Cylinder_Z( TICK, 15, 50 );
    }
}

module Mount_Holes()
{
    for ( x = [ 20, SIZE_X - 20 ] )
    {
        for ( y = [ - 7, SIZE_Y + 7 ] )
        {
            translate( [ x - 5, y, - EPS ] )
                Screw8_ShankSlot_ZX( 2 * EPS + TICK, 10 );
        }
    }

    for ( y = [ - 7, SIZE_Y + 7 ] )
    {
        translate( [ SIZE_X / 2 - 18, y, - EPS ] )
            Screw8_ShankSlot_ZX( 2 * EPS + TICK, 16 );

        translate( [ SIZE_X / 2 - 10, y, - EPS ] )
            Screw8_Head_Z( 2 * EPS + TICK );
    }
}

module Wall_XY( aX, aY, aSizeX, aSizeY )
{
    translate( [ aX, aY, 0 ] )
        cube( [ aSizeX, aSizeY, TICK ] );
}

module Wall_XZ( aX, aY, aSizeX, aSizeZ )
{
    translate( [ aX, aY, 0 ] )
        cube( [ aSizeX, TICK, aSizeZ ] );
}

module Wall_YZ( aX, aY, aSizeY, aSizeZ )
{
    translate( [ aX, aY, 0 ] )
        cube( [ TICK, aSizeY, aSizeZ ] );
}

EPS = 0.1;

SIZE_X = 115;
SIZE_Y =  34;
SIZE_Z =  25;

SPACE = 0.1;

TICK = 2;
