
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/EthCAN_Lib_Test.cpp

#include "Component.h"

// Tests
/////////////////////////////////////////////////////////////////////////////

// Setup-A An EthCAN is connected to the computer
// Setup-B An EthCAN is connected to the network
// Setup-C An EthCAN is connected to the computer using a USB cable
// Setup-D Two EthCANs are connected to the computer and both EthCAN are
//         connected to the same CAN bus
// Setup-E An EthCAN connected to the computer using a USB cable and
//         connected to the network
KMS_TEST_GROUP_LIST_BEGIN
    KMS_TEST_GROUP_LIST_ENTRY("Base")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-A")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-B")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-C")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-D")
    KMS_TEST_GROUP_LIST_ENTRY("Setup-E")
KMS_TEST_GROUP_LIST_END

extern int Device_SetupA();
extern int Device_SetupB();
extern int Device_SetupC();
extern int Device_SetupD();
extern int Device_SetupE();
extern int Display_Base();
extern int File_Base();
extern int System_Base();
extern int System_SetupA();
extern int System_SetupB();
extern int System_SetupC();

KMS_TEST_LIST_BEGIN
    KMS_TEST_LIST_ENTRY(Device_SetupA, "Device - Setup A", 1, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(Device_SetupB, "Device - Setup B", 2, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(Device_SetupC, "Device - Setup C", 3, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(Device_SetupD, "Device - Setup D", 4, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(Device_SetupE, "Device - Setup E", 5, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(Display_Base , "Display - Base"  , 0, 0)
    KMS_TEST_LIST_ENTRY(File_Base    , "File - Base"     , 0, 0)
    KMS_TEST_LIST_ENTRY(System_Base  , "System - Base"   , 0, 0)
    KMS_TEST_LIST_ENTRY(System_SetupA, "System - Setup A", 1, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(System_SetupB, "System - Setup B", 2, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_ENTRY(System_SetupC, "System - Setup C", 3, KMS_TEST_FLAG_INTERACTION_NEEDED)
    KMS_TEST_LIST_END

KMS_TEST_MAIN
