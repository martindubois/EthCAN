
// Author    KMS - Martin Dubois, P.Eng
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Hardware/ESP-32-POE-ISO.scad

ESP_32_POE_ISO();

// Public
/////////////////////////////////////////////////////////////////////////////

module ESP_32_POE_ISO()
{
    color( "red" )
    {
        cube( [ SIZE_X, SIZE_Y, 1 ] );

        // Eth
        translate( [ SIZE_X - 15, SIZE_Y - 16 - 2, 1 ] )
            cube( [ 21, 16, 14 ] );

        // ESP-32-WROOM
        translate( [ - 6, 5, 1 ] )
            cube( [ 25.5, 18, 3.1 ] );

        // USB
        translate( [ 60, 0, 1 ] )
            cube( [ 8, 6, 3 ] );

        // Power
        translate( [ SIZE_X - 6, 0, 1 ] )
            cube( [ 6, 6, 4 ] );
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

SIZE_X = 100;
SIZE_Y =  28;
