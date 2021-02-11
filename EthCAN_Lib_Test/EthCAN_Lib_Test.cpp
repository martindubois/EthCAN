
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/EthCAN_Lib_Test.cpp

#include "Component.h"

// Tests
/////////////////////////////////////////////////////////////////////////////

// Setup-A An EthCAN is connected to the network
// Setup-B An EthCAN is connected to the test computer using a USB cable
// Setup-C Two EthCAN are connected to the network
// Setup-D Two EthCAN are connected to the network and are connected to the
//         sam CAN bus
KMS_TEST_GROUP_LIST_BEGIN
    KMS_TEST_GROUP_LIST_ENTRY("Base")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-A")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-B")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-C")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-D")
KMS_TEST_GROUP_LIST_END

extern int Device_Base();
extern int Device_SetupA();
extern int Device_SetupB();
extern int System_Base();
extern int System_SetupA();

KMS_TEST_LIST_BEGIN
    KMS_TEST_LIST_ENTRY(Device_Base  , "Device - Base"   , 0, 0)
    KMS_TEST_LIST_ENTRY(Device_SetupA, "System - Setup A", 1, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(Device_SetupB, "System - Setup B", 2, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(System_Base  , "System - Base"   , 0, 0)
    KMS_TEST_LIST_ENTRY(System_SetupA, "System - Setup A", 1, KMS_TEST_FLAG_INTERACTION_NEEDED)
KMS_TEST_LIST_END

KMS_TEST_MAIN
