
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/EthCan.scad

use <C:/_VC/Base3D/Cylinder.scad>
use <C:/_VC/Base3D/Metric/M3.scad>
use <C:/_VC/Base3D/SAE/Screw8.scad>

use <CAN-BUS.scad>
use <DCtoDC.scad>
use <ESP-32-POE-ISO.scad>
use <Mount_Ronin.scad>

CFG_BOARDS = 0;

// 1  Wall Mount
// 2  DIN Mount
// 3  RONIN Mount
CFG_MOUNT = 3;

if ( 0 != CFG_BOARDS )
{
    translate( [ CAN_X, CAN_Y, BOARD_Z ] ) CAN_BUS();
    translate( [ ESP_X, ESP_Y, BOARD_Z ] ) ESP_32_POE_ISO();
}

Bottom();

// Top();

// Public
/////////////////////////////////////////////////////////////////////////////

module Bottom()
{
    difference()
    {
        union()
        {
            difference()
            {
                union()
                {
                    if ( 1 == CFG_MOUNT )
                        Mount();

                    cube( [ SIZE_X, SIZE_Y, BOTTOM_Z ] );
                }

                translate( [ TICK, TICK, BOARD_Z + 1 ] )
                    cube( [ SIZE_X - 2 * TICK, SIZE_Y - 2 * TICK, BOTTOM_Z - TICK - 2 - 1 + EPS ] );

                translate( [ USB_X, - EPS, BOARD_Z - 1 ] )
                    cube( [ USB_SIZE_X, 2 * EPS + TICK + 0.5, 9 ] );

                translate( [ VOID_X, VOID_Y, 1 ] )
                    cube( [ VOID_SIZE_X, VOID_SIZE_Y + 3, BOTTOM_Z - 1 + EPS ] );

                translate( [ VOID_X, VOID_Y + VOID_SIZE_Y - EPS, 1 ] )
                    cube( [ VOID_SIZE_X - 7, EPS + 9, BOTTOM_Z - 1 + EPS ] );

                translate( [ CAN_X, CAN_Y, BOARD_Z ] )
                    CAN_BUS_Bottom();

                translate( [ ESP_X, ESP_Y, BOARD_Z ] )
                    ESP_32_POE_ISO_Bottom();
            }

            translate( [ CAN_X, CAN_Y, 1 + CAN_BUS_Base_Z() ] )
                CAN_BUS_Base();
        }

        Holes_Bottom();

        for ( y = [ 17, 22, 27, 32, 37, 42, 47, 52 ] )
        {
            translate( [ - EPS, y, 0 ] )
            {
                scale( [ 1, 0.5, 1 ] )
                    Cylinder_X( 2 * EPS + SIZE_X, 2 );
            }
        }

        if ( 1 == CFG_MOUNT ) Mount_Holes();
    }

    translate( [ 55, 35, 1 + DCtoDC_Base_Z() ] )
        DCtoDC_Base();

    for ( x = [ 20, 60 ] )
    {
        translate( [ x, 0, 1 ] )
            cube( [ 2, SIZE_Y, 0.5 ] );
    }

    if ( 3 == CFG_MOUNT )
    {
        translate( [ - Mount_Ronin_Size_X(), 31, 0 ] )
        {
            Mount_Ronin_Bottom();

            // Mount_Ronin_Cap();
        }
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
                translate( [ 0, 0, BOTTOM_Z ] )
                    cube( [ SIZE_X, SIZE_Y, TOP_Z ] );

                translate( [ TICK + SPACE, TICK + SPACE, BOTTOM_Z - 3 ] )
                    cube( [ SIZE_X - 2 * TICK - 2 * SPACE, SIZE_Y - 2 * TICK - 2 * SPACE, 3 ] );
            }

            translate( [ USB_X, - EPS, BOARD_Z ] )
                cube( [ USB_SIZE_X + 22, 2 * EPS + TICK, 7 ] );

            translate( [ VOID_X + 3, VOID_Y, 5 - EPS ] )
                cube( [ VOID_SIZE_X - 3, VOID_SIZE_Y, 17.5 ] );

            translate( [ VOID_X + 3, VOID_Y + VOID_SIZE_Y - EPS, 5 - EPS ] )
                cube( [ VOID_SIZE_X - 9, EPS + 9, 17.5 ] );

            translate( [ 9, SIZE_Y - 7, 5 - EPS ] )
                cube( [ 36, 5, 17.5 ] );

            translate( [ 14, 29, 5 - EPS ] )
                cube( [ 74, 12, 16.5 ] );

            translate( [ CAN_X, CAN_Y, BOARD_Z ] )
                CAN_BUS_Top();

            translate( [ ESP_X, ESP_Y, BOARD_Z ] )
                ESP_32_POE_ISO_Top();

            Holes_Top();

            if ( 2 == CFG_MOUNT )
            {
                translate( [ SIZE_X - 6 - EPS, SIZE_Y - 9, SIZE_Z / 2 + 1.5 ] )
                    M3_Shank_X( 2 * EPS + 6 );

                translate( [ SIZE_X - 9, SIZE_Y - 9, SIZE_Z / 2 + 1.5 ] )
                    M3_Insert_X();
            }
        }

        if ( 2 == CFG_MOUNT )
            Mount_DIN();
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

module Holes_Bottom()
{
    for ( y = [ 5, SIZE_Y - 5 ] )
    {
        translate( [ 5, y, - EPS ] )
        {
            M3_Shank_Z( 2 * EPS + 7 );
            M3_Hex_ZR();
        }
    }

    for ( y = [ 10, SIZE_Y - 5 ] )
    {
        translate( [ SIZE_X - 5, y, - EPS ] )
        {
            M3_Shank_Z( 2 * EPS + 7 );
            M3_Hex_ZR();
        }
    }
}

module Holes_Top()
{
    for ( y = [ 5, SIZE_Y - 5 ] )
    {
        translate( [ 5, y, 5 - EPS    ] ) M3_Shank_Z( 2 * EPS + SIZE_Z - 5 );
        translate( [ 5, y, SIZE_Z - 4 ] ) M3_Head_Z( 4 + EPS );
    }

    for ( y = [ 10, SIZE_Y - 5 ] )
    {
        translate( [ SIZE_X - 5, y, 5 - EPS    ] ) M3_Shank_Z( 2 * EPS + SIZE_Z - 7 );
        translate( [ SIZE_X - 5, y, SIZE_Z - 4 ] ) M3_Head_Z( 4 + EPS );
    }
}

module Mount()
{
    for ( y = [ - 14, SIZE_Y ] )
        Wall_XY( 20, y, SIZE_X - 40, 14 );

    for ( y = [ 0, SIZE_Y ] )
    {    
        for ( x = [ 20, SIZE_X - 20 ] )
        {
            translate( [ x, y, 0 ] )
                Cylinder_Z( TICK, 14, 50 );
        }
    }
}

module Mount_DIN()
{
    difference()
    {
        union()
        {
            translate( [ SIZE_X, 35, BOTTOM_Z ] )
                cube( [ 1, 6, TOP_Z ] );

            translate( [ SIZE_X + 1, 35, 3 ] )
                cube( [ 3, SIZE_Y - 35 + DIN_Y, SIZE_Z - 3 ] );

            translate( [ SIZE_X - 1, SIZE_Y + DIN_T, 3 ] )
                cube( [ 2, DIN_Y - DIN_T, SIZE_Z - 3 ] );

            translate( [ SIZE_X - 44, SIZE_Y, BOTTOM_Z ] )
                cube( [ 10, DIN_Y, TOP_Z ] );

            translate( [ SIZE_X - 44, SIZE_Y + DIN_T, 3 ] )
                cube( [ 12, DIN_Y - DIN_T, SIZE_Z - 3 ] );
        }

        union()
        {
            translate( [ SIZE_X - 1 - 3, SIZE_Y + DIN_T + 2, 3 - EPS ] )
            {
                rotate( [ 0, 0, - 40 ] )
                    cube( [ 3, 10, 2 * EPS + SIZE_Z - 3 ] );
            }

            translate( [ SIZE_X - 33.5, SIZE_Y + 0.1, 3 - EPS ] )
            {
                rotate( [ 0, 0, 20 ] )
                    cube( [ 3, DIN_T, 2 * EPS + SIZE_Z - 3 ] );
            }

            translate( [ SIZE_X + 1 - EPS, SIZE_Y - 9, SIZE_Z / 2 + 1.5 ] )
                M3_Shank_X( 2 * EPS + 3 );
        }

    }
}

module Mount_Holes()
{
    for ( x = [ 20, 44 ] )
    {
        for ( y = [ - 6, SIZE_Y + 6 ] )
        {
            translate( [ x - 5, y, - EPS ] )
                Screw8_ShankSlot_ZX( 2 * EPS + TICK, 10 );
        }
    }

    for ( y = [ - 6, SIZE_Y + 6 ] )
    {
        translate( [ SIZE_X - 33, y, - EPS ] )
            Screw8_ShankSlot_ZX( 2 * EPS + TICK, 16 );

        translate( [ SIZE_X - 25, y, - EPS ] )
            Screw8_Head_Z( 2 * EPS + TICK );
    }
}

module Wall_XY( aX, aY, aSizeX, aSizeY )
{
    translate( [ aX, aY, 0 ] )
        cube( [ aSizeX, aSizeY, TICK ] );
}

EPS = 0.1;

BOARD_Z = 4;

BOTTOM_Z = 8;

DIN_T = 1.5;
DIN_Y = 6;

TOP_Z = 15;

CAN_X =  2;
CAN_Y = 35;

ESP_X = 9;
ESP_Y = 3;

SIZE_X = 110.5;
SIZE_Y =  64;
SIZE_Z =  BOTTOM_Z + TOP_Z;

USB_SIZE_X = 13;

USB_X = 60;

SPACE = 0.1;

TICK = 2;

VOID_SIZE_X = 96;
VOID_SIZE_Y = 17;

VOID_X = 11;
VOID_Y = 34;
