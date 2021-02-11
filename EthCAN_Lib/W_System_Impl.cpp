
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

void System_Impl::Detect_USB()
{
    HKEY lKey0;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Enum\\USB\\VID_1A86&PID_7523", 0, KEY_READ, &lKey0))
    {
        unsigned int lCount = 0;
        char lId[32];
        DWORD lSize_byte = sizeof(lId);

        while (ERROR_SUCCESS == RegEnumKeyEx(lKey0, lCount, lId, &lSize_byte, NULL, NULL, NULL, NULL))
        {
            assert(sizeof(lId) > lSize_byte);

            HKEY lKey1;

            if (ERROR_SUCCESS == RegOpenKeyEx(lKey0, lId, 0, KEY_READ, &lKey1))
            {
                char lName[64];
                DWORD lType;

                lSize_byte = sizeof(lName);

                if (   (ERROR_SUCCESS == RegQueryValueEx(lKey1, "FriendlyName", NULL, &lType, reinterpret_cast<LPBYTE>(lName), &lSize_byte))
                    && (REG_SZ == lType))
                {
                    assert(sizeof(lName) >= lSize_byte);

                    char* lPtr = strchr(lName, '(');
                    if (NULL != lPtr)
                    {
                        char lLink[16];

                        strcpy_s(lLink, lPtr + 1);

                        lPtr = strchr(lLink, ')');
                        if (NULL != lPtr)
                        {
                            strcpy_s(lPtr SIZE_INFO(sizeof(lLink) - 6), ":");

                            OnSerialLink(lLink);
                        }
                    }
                }
            }

            lCount++;
        }
    }
}
