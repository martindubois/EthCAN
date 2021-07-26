
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/Tool_Prog.scad

// Bottom();
Top();

// Private
/////////////////////////////////////////////////////////////////////////////

module Bottom()
{
    color( "grey" )
    {
        difference()
        {
            translate( [ 0, - HANDLE_Y, 0 ] )
                cube( [ SIZE_X, HANDLE_Y + SIZE_Y + 2, TICK_Z ] );

            translate( [ 8 - SPACE, - SPACE, - EPS ] )
                cube( [ 2 * SPACE + 6, SPACE + SIZE_Y + 2 + EPS, 2 * EPS + TICK_Z ] );

            translate( [ - EPS, SIZE_Y + 3, 0 ] )
            {
                rotate( [ 45, 0, 0 ] )
                    cube( [ 2 * EPS + SIZE_X, 3, 5 ] );
            }
        }

        translate( [ 0, - HANDLE_Y, TICK_Z ] )
            cube( [ SIZE_X, 2, 2 ] );
    }
}

module Top()
{
    translate( [ 4, 0, TICK_Z + SPACE ] )
        cube( [ SIZE_X - 4, TICK_Y, BOARD_Z + HEIGHT ] );

    translate( [ 8, 0, - SPACE ] )
        cube( [ 6, TICK_Y, 2 * SPACE + TICK_Z ] );

    translate( [ 0, 0, - SPACE - TICK_Z ] )
        cube( [ SIZE_X, SIZE_Y, TICK_Z ] );

    difference()
    {
        translate( [ 0, 0, TICK_Z + BOARD_Z + HEIGHT ] )
            cube( [ SIZE_X, 10, TICK_Z ] );

        translate( [ - EPS, HOLE_Y - 1.3, TICK_Z + BOARD_Z + HEIGHT - EPS ] )
            cube( [ SIZE_X - 2, 2.6, 2 + EPS ] );

        for ( x = [ 0, 13 ] )
        {
            translate( [ x + 3, HOLE_Y, TICK_Z + BOARD_Z + HEIGHT ] )
                cylinder( 4 + EPS, HOLE_R, HOLE_R, $fn = 20 );
        }

        translate( [ 3, HOLE_Y - 1, TICK_Z + BOARD_Z + HEIGHT ] )
            cube( [ 13, 2 * HOLE_R, 4 + EPS ] );
    }
}

BOARD_Z = 1.5;

EPS = 0.1;

HANDLE_Y = 6;

HEIGHT = 4;

HOLE_R = 1;
HOLE_Y = 7;

SIZE_X = 20;
SIZE_Y = 12;

SPACE = 0.1;

TICK_Y = 5;
TICK_Z = 3;
