
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware1/Component.h

#pragma once

// CONFIG _TEST_
// #define _TEST_

// Macros
/////////////////////////////////////////////////////////////////////////////

#ifdef _TEST_

    #define MSG_DEBUG(M)   Serial.print("DEBUG  "  ); Serial.println(M)
    #define MSG_ERROR(M,D) Serial.print("ERROR  "  ); Serial.print(M); Serial.println(D);
    #define MSG_INFO(M,D)  Serial.print("INFO  "   ); Serial.print(M); Serial.println(D);
    #define MSG_WARNING(M) Serial.print("WARNING  "); Serial.println(M)

#else

    #define MSG_DEBUG(M)
    #define MSG_ERROR(M,D)
    #define MSG_INFO(M,D)
    #define MSG_WARNING(M)

#endif
