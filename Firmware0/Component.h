
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Component.h

#pragma once

// CONFIG _TRACE_
#define _TRACE_

// Macros
/////////////////////////////////////////////////////////////////////////////

#ifdef _TRACE_

    #define MSG_DEBUG(M)  Serial.print("DEBUG  ") Serial.println(M)

#else

    #define MSG_DEBUG(M)

#endif

#define MSG_ERROR(M)    Serial.print("ERROR  "  ); Serial.println(M)
#define MSG_INFO(M)     Serial.print("INFO  "   ); Serial.println(M)
#define MSG_WARNING(M)  Serial.print("WARNING  "); Serial.println(M)