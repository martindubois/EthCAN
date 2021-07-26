
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/EthCan_v2.scad

use <C:/_VC/Base3D/Metric/M3.scad>

use <CAN-BUS.scad>
use <ESP-32-POE-ISO.scad>

CFG_BOARDS = 0;
CFG_MOUNT  = 0;

if ( 0 != CFG_BOARDS )
{
    translate( [ CAN_X, CAN_Y, CAN_Z ] )
    {
        rotate( [ 0, 0, - 90 ] )
            CAN_BUS();
    }

    translate( [ ESP_X, ESP_Y, ESP_Z ] ) ESP_32_POE_ISO();
}

Bottom();

// Top();

// Public
/////////////////////////////////////////////////////////////////////////////

module Bottom()
{
    color( "gray" )
    {
        difference()
        {
            cube( [ SIZE_X, SIZE_Y, BOTTOM_Z ] );

            translate( [ ESP_X, ESP_Y, ESP_Z ] )
                ESP_32_POE_ISO_Bottom();

            translate( [ CAN_X, CAN_Y, CAN_Z ] )
            {
                rotate( [ 0, 0, - 90 ] )
                    CAN_BUS_Bottom();
            }

            Holes_Bottom();

            translate( [ USB_X, - EPS, ESP_Z + 1 ] )
                cube( [ 8, 8, 4 ] );

            translate( [ USB_X - 3, - EPS, ESP_Z - 1 ] )
                cube( [ 14, 6, 8 ] );

            translate( [ USB_X - 3 + 14 - 1, - EPS, ESP_Z - 1 ] )
                cube( [ 21, 5, 8 ] );

            for ( x = [ 71, 77, 83, 89 ] )
            {
                translate( [ x, 4, ESP_Z + 1 ] )
                    cube( [ 2, 3, 2 ] );
            }

            translate( [ 0, 0, ESP_Z + 1 ] )
            {
                translate( [ CORNER, TICK, 0 ] )
                    cube( [ 34, SIZE_Y - 2 * TICK, BOTTOM_Z ] );

                translate( [ 73, TICK, 8 ] )
                    cube( [ SIZE_X - 73 - CORNER, CORNER, BOTTOM_Z ] );

                translate( [ 73, SIZE_Y - TICK - CORNER, 0 ] )
                    cube( [ SIZE_X - 73 - CORNER, CORNER, BOTTOM_Z ] );

                for ( x = [ TICK, SIZE_X - TICK - CORNER ] )
                {
                    translate( [ x, CORNER, 0 ] )
                        cube( [ CORNER, SIZE_Y - 2 * CORNER, BOTTOM_Z ] );
                }
            }

            translate( [ 0, 0, 1 ] )
            {
                translate( [ CORNER, TICK, 0 ] )
                    cube( [ 34, 3, BOTTOM_Z ] );

                translate( [ CORNER, SIZE_Y - TICK - 8, 0 ] )
                    cube( [ 34, 8, BOTTOM_Z ] );

                translate( [ 49, 5, 0 ] )
                    cube( [ 8, 4, BOTTOM_Z ] );

                translate( [ 49, SIZE_Y - 3 - 10, 0 ] )
                    cube( [ 19, 10, BOTTOM_Z ] );

                translate( [ 73, SIZE_Y - TICK - 8, 0 ] )
                    cube( [ SIZE_X - 73 - CORNER, 8, BOTTOM_Z ] );
            }
        }
    }
}

module Top()
{
    translate( [ 0, 0, BOTTOM_Z ] )
    {
        cube( [ SIZE_X, SIZE_Y, TOP_Z ] );
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

module Holes_Bottom()
{
    for ( x = [ 5, SIZE_X - 5 ] )
    {
        for ( y = [ 5, SIZE_Y - 5 ] )
        {
            translate( [ x, y, - EPS ] )
            {
                M3_Shank_Z( 2 * EPS + BOTTOM_Z );
                M3_Hex_ZR();
            }
        }
    }
}

BOTTOM_Z = 18;

CAN_X = 48;
CAN_Y = 45;
CAN_Z = 17;

CORNER = 10;

EPS = 0.1;

ESP_X = 9;
ESP_Y = 7;
ESP_Z = 4;

SIZE_X = 110.5;
SIZE_Y =  47;

TICK = 2;

TOP_Z = 12;

USB_X = 62;
