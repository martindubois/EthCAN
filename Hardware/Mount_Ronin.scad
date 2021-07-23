
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/Mount_Ronin.scad

use <C:/_VC/Base3D/Metric/M4.scad>

Mount_Ronin_Bottom();

// Mount_Ronin_Cap();

// Public
// //////////////////////////////////////////////////////////////////////////

module Mount_Ronin_Bottom()
{
    difference()
    {
        cube( [ Mount_Ronin_Size_X(), SIZE_Y, SIZE_Z ] );

        for ( x = [ HOLE_FIRST, HOLE_FIRST + HOLE_DIST ] )
        {
            translate( [ x, SIZE_Y / 2, RAIL_SIZE_Z - EPS ] )
                M4_Shank_Z( 2 * EPS + SIZE_Z - RAIL_SIZE_Z );
        }

        translate( [ TICK, ( SIZE_Y - RAIL_SIZE_Y ) / 2, - EPS ] )
            cube( [ EPS + Mount_Ronin_Size_X() - TICK, RAIL_SIZE_Y, EPS + RAIL_SIZE_Z ] );

        lConX = CON_CENTER_X - CON_SIZE_X / 2;
        lConY = ( SIZE_Y - CON_SIZE_Y ) / 2;

        translate( [ lConX, lConY, RAIL_SIZE_Z - EPS ] )
            cube( [ CON_SIZE_X, CON_SIZE_Y, EPS + CON_SIZE_Z ] );

        translate( [ lConX + 0.5, lConY + 0.5, RAIL_SIZE_Z + CON_SIZE_Z - EPS ] )
            cube( [ CON_SIZE_X - 1, CON_SIZE_Y - 1, 2 * EPS + SIZE_Z - RAIL_SIZE_Z - CON_SIZE_Z ] );

        translate( [ lConX - 1, lConY + 1, RAIL_SIZE_Z + CON_SIZE_Z - EPS ] )
            cube( [ CON_SIZE_X + 2, CON_SIZE_Y - 2, 2 * EPS + SIZE_Z - RAIL_SIZE_Z - CON_SIZE_Z ] );

        for ( y = [ TICK, SIZE_Y - TICK - CANAL_SIZE_Y ] )
        {
            translate( [ TICK, y, SIZE_Z - CANAL_SIZE_Z ] )
                cube( [ EPS + Mount_Ronin_Size_X() - TICK, CANAL_SIZE_Y, EPS + CANAL_SIZE_Z ] );
        }

        translate( [ Mount_Ronin_Size_X() - 5, TICK, RAIL_SIZE_Z + TICK ] )
            cube( [ EPS + 5, SIZE_Y - 2 * TICK, EPS + SIZE_Z - RAIL_SIZE_Z - TICK ] );
    }
}

module Mount_Ronin_Cap()
{
    color( "red" )
    {
        translate( [ 0, 0, SIZE_Z ] )
        {
            difference()
            {
                cube( [ Mount_Ronin_Size_X(), SIZE_Y, CAP_SIZE_Z ] );

                for ( x = [ HOLE_FIRST, HOLE_FIRST + HOLE_DIST ] )
                {
                    translate( [ x, SIZE_Y / 2, - EPS ] )
                        M4_Shank_Z( 2 * EPS + CAP_SIZE_Z );

                    translate( [ x, SIZE_Y / 2, CAP_SIZE_Z / 2 ] )
                        M4_Head_Z( EPS + CAP_SIZE_Z / 2 );
                }
            }

            translate( [ Mount_Ronin_Size_X(), 4.8, CAP_SIZE_Z - TICK ] )
                cube( [ 12.9, 13.9, TICK ] );
        }
    }
}

function Mount_Ronin_Size_X() = 32;

// Private
// //////////////////////////////////////////////////////////////////////////

CANAL_SIZE_Y = 6;
CANAL_SIZE_Z = 4;

// Connector
CON_CENTER_X = 16.5;
CON_SIZE_X   = 2 * 2.54;
CON_SIZE_Y   = 4 * 2.54;
CON_SIZE_Z   = 3.5;

CAP_SIZE_Z = 8;

EPS = 0.1;

HOLE_FIRST =  6.5;
HOLE_DIST  = 20;

RAIL_SIZE_Y = 20.6;
RAIL_SIZE_Z =  5;

SIZE_Y = 25;
SIZE_Z = 15;

TICK = 3;
