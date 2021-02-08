
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/CAN-BUS.scad

use <C:/_VC/Base3D/Cylinder.scad>

CAN_BUS();

// Public
/////////////////////////////////////////////////////////////////////////////

module CAN_BUS()
{
    color( "blue" )
    {
        cube( [ SIZE_X, SIZE_Y, 1 ] );

        translate( [ SIZE_X, SIZE_Y / 2, 0 ] )
            Cylinder_Z( 1, 2, 30 );

        for ( y = [ 0, SIZE_Y ] )
        {
            translate( [ 10, y, 0 ] )
                Cylinder_Z( 1, 2, 30 );
        }

        // Groove
        translate( [ SIZE_X - 5 - 2, SIZE_Y / 2 - 5, 1 ] )
            cube( [ 5, 10, 8 ] );

        // Connector
        translate( [ 2, SIZE_Y / 2 - 4, 1 ] )
            cube( [ 8, 8, 8 ] );
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

SIZE_X = 40;
SIZE_Y = 20;