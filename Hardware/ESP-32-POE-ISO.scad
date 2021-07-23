
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
        cube( [ SIZE_X, SIZE_Y, TICK ] );

        translate( [ 0, 0, TICK ] )
        {
            translate( [ ETH_X, ETH_Y, 0 ] )
                cube( [ ETH_SIZE_X, ETH_SIZE_Y, ETH_SIZE_Z ] );

            translate( [ ESP_X, 5, 0 ] )
                cube( [ 25.5, 18, 3.1 ] );

            translate( [ USB_X, 0, 0 ] )
                cube( [ 8, 7, 3 ] );

            translate( [ POWER_X, 0, 0 ] )
                cube( [ 6, 6, 4 ] );

            translate( [ BOX_X, 9, 0 ] )
                cube( [ 7, 19, 10 ] );

            // Connector
            translate( [ 24, 4, 0 ] )
                cube( [ 9, 20, 10 ] );
        }
    }
}

module ESP_32_POE_ISO_Bottom()
{
    translate( [ - SPACE, - SPACE, 0 ] )
        cube( [ 2 * SPACE + SIZE_X, 2 * SPACE + SIZE_Y, 19 ] ); // 1 + EPS ] );

    translate( [ 1, 1, - 3 ] )
        cube( [ SIZE_X - 2, SIZE_Y - 2, 19 ] ); // 3 + EPS ] );

    translate( [ ETH_X, ETH_Y - SPACE, TICK - SPACE ] )
        cube( [ ETH_SIZE_X + EPS, 2 * SPACE + ETH_SIZE_Y, 19 ] ); // 2 * SPACE + ETH_SIZE_Z ] );

    // Back components

    translate( [ 0, 0, - 3 ] )
    {
        translate( [ 5, 0, 0 ] )
        cube( [ 5, 1 + EPS, 3 + EPS ] );

        translate( [ SIZE_X - 8, 0, 0 ] )
            cube( [ 4, 1 + EPS, 3 + EPS ] );

        translate( [ 58, SIZE_Y - 1 - EPS, 0 ] )
            cube( [ SIZE_X - 58 - 3, 1 + EPS, 3 + EPS ] );
    }
}

module ESP_32_POE_ISO_Top()
{
    translate( [ ESP_X, - SPACE, 0 ] )
        cube( [ 6 + SIZE_X + 4, 2 * SPACE + SIZE_Y, TICK ] );

    translate( [ ESP_X, 1, TICK - EPS ] )
        cube( [ 6 + EPS, SIZE_Y - 2, 12 ] );

    translate( [ 0, 1, TICK - EPS ] )
        cube( [ SIZE_X - 1, SIZE_Y - 2, 17 ] );

    // translate( [ ESP_X - 1, 4, 0 ] )
    //    cube( [ 25.5, 20, TICK + 9 ] );

    translate( [ ETH_X, ETH_Y - SPACE, 0 ] )
        cube( [ ETH_SIZE_X + EPS, 2 * SPACE + ETH_SIZE_Y, TICK + ETH_SIZE_Z + SPACE ] );

    translate( [ USB_X - 1, - 1, 0 ] )
        cube( [ 36, 9, TICK + 4 ] );

    translate( [ POWER_X - 1, - 1, 0 ] )
        cube( [ 8, 12, TICK + 12 ] );

    // translate( [ BOX_X - 1, 8, 0 ] )
    //    cube( [ 28, 21, TICK + 17 ] );

    for ( y = [ 0, SIZE_Y - 1 - EPS ] )
    {
        translate( [ 3, y, TICK - EPS ] )
            cube( [ 10, 1 + EPS, 10 ] );
    }

}

// Private
/////////////////////////////////////////////////////////////////////////////

EPS = 0.1;

SIZE_X = 94;
SIZE_Y = 28.2;

BOX_X = 52;

ESP_X = - 6;

ETH_SIZE_X = 21.5;
ETH_SIZE_Y = 17;
ETH_SIZE_Z = 14;

ETH_X = SIZE_X - 14;
ETH_Y =          10.3;

POWER_X = SIZE_X - 6;

USB_X = 53;

SPACE = 0.1;

TICK = 1.5;
