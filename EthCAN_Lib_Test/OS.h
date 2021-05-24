
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/OS.h

#pragma once

// Functions
/////////////////////////////////////////////////////////////////////////////

#if defined(_KMS_LINUX_) || defined(_KMS_OS_X_)
    #define OS_Sleep(T) usleep(1000 * (T))
#endif

#ifdef _KMS_WINDOWS_
    #define OS_Sleep(T) Sleep(T)
#endif
