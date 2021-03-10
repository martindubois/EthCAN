
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/W_System_Impl.cpp

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Serial.h"

#include "System_Impl.h"

// Private
/////////////////////////////////////////////////////////////////////////////

// TODO USB.Windows
//      Do not try to communicate with device that are not present.

void System_Impl::Detect_USB()
{
    HKEY lKey0;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Enum\\USB\\VID_1A86&PID_7523", 0, KEY_READ, &lKey0))
    {
        unsigned int lCount = 0;
        char lId[32];
        DWORD lSize_byte = sizeof(lId);
        LSTATUS lStatus;

        while (ERROR_SUCCESS == RegEnumKeyEx(lKey0, lCount, lId, &lSize_byte, NULL, NULL, NULL, NULL))
        {
            assert(sizeof(lId) > lSize_byte);

            HKEY lKey1;

            if (ERROR_SUCCESS == RegOpenKeyEx(lKey0, lId, 0, KEY_READ, &lKey1))
            {
                HKEY lKey2;

                if (ERROR_SUCCESS == RegOpenKeyEx(lKey1, "Device Parameters", 0, KEY_READ, &lKey2))
                {
                    char lName[16];
                    DWORD lType;

                    lSize_byte = sizeof(lName);

                    if (   (ERROR_SUCCESS == RegQueryValueEx(lKey2, "PortName", NULL, &lType, reinterpret_cast<LPBYTE>(lName), &lSize_byte))
                        && (REG_SZ == lType))
                    {
                        assert(sizeof(lName) >= lSize_byte);

                        OnSerialLink(lName);
                    }

                    lStatus = RegCloseKey(lKey2);
                    assert(ERROR_SUCCESS == lStatus);
                }

                lStatus = RegCloseKey(lKey1);
                assert(ERROR_SUCCESS == lStatus);
            }

            lCount++;
        }

        lStatus = RegCloseKey(lKey0);
        assert(ERROR_SUCCESS == lStatus);
    }
}
