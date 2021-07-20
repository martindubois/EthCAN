
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/Tool_Prog.scad

difference()
{
    scale( [ 1.4, 1, 1 ] )
        cylinder( SIZE_Z, 12, 4.5, $fn = 30 );

    for ( x = [ - 3, 3 ] )
    {
        translate( [ x, 0, 1 ] )
            cylinder( SIZE_Z - 1 + EPS, 1.2, 1.2, $fn = 20 );
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

EPS = 0.1;

SIZE_Z = 20;