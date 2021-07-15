
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/Tool_ProgCap.scad

difference()
{
    union()
    {
        cube( [ SIZE_X, SIZE_Y, SIZE_Z ] );
    }

    translate( [ TICK, TICK, - EPS ] )
        cube( [ SIZE_X - 2 * TICK, SIZE_Y - 2 * TICK, EPS + INSIDE_Z ] );

    translate( [ TICK, TICK, INSIDE_Z - EPS ] )
    {
        translate( [ 2.4, 2.8, 0 ] )
            cube( [ 4 * 2.54 + 0.6, 2.54 + 0.4, 2 * EPS + SIZE_Z ] );

        translate( [ 12, 3.1, 0 ] )
            cube( [ 6, 2, 5 ] );
    }

    translate( [ - EPS, TICK + 7, SIZE_Z ] )
    {
        rotate( [ - 60, 0, 0 ] )
            cube( [ 2 * EPS + SIZE_X, SIZE_Y, 10 ] );
    }

    translate( [ - EPS, - EPS, 11 ] )
    {
        rotate( [ 72, 0, 0 ] )
            cube( [ 2 * EPS + SIZE_X, 12, 4 ] );
    }

    translate( [ TICK + 17, - EPS, 18 ] )
    {
        cube( [ 3, SIZE_Y, 2 ] );
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

EPS = 0.1;

INSIDE_Z = 8;

SIZE_X = 25.4;
SIZE_Y = 15.4;
SIZE_Z = 22;

TICK = 2;
