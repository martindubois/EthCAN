
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/OS.h

#pragma once

// Functions
/////////////////////////////////////////////////////////////////////////////

#ifdef _KMS_LINUX_

    #define OS_Sleep(T) usleep(1000 * (T))

    extern uint64_t OS_GetTickCount();

#endif

#ifdef _KMS_WINDOWS_

    #define OS_GetTickCount() GetTickCount64()
    #define OS_Sleep(T)       Sleep(T)

#endif
