
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Config.h

#include <Arduino.h>

#include <EEPROM.h>

#include "Component.h"

#include "CAN.h"
#include "Info.h"
#include "UDP.h"
#include "USB.h"

#include "Config.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define STORE_WHAT          (  0) //  1 bytes
                                  // 15 bytes
#define STORE_NAME          ( 16) // 16 bytes
#define STORE_CAN_FILTERS   ( 32) // 24 bytes
#define STORE_CAN_MASKS     ( 56) //  8 bytes
#define STORE_CAN_RATE      ( 64) //  1 bytes
                                  //  3 bytes
#define STORE_IPv4_ADDR     ( 68) //  4 bytes
#define STORE_IPv4_GATEWAY  ( 72) //  4 bytes
#define STORE_IPv4_MASK     ( 76) //  4 bytes
#define STORE_SERVER_IPv4   ( 80) //  4 bytes
#define STORE_SERVER_PORT   ( 84) //  2 bytes
#define STORE_SERVER_FLAGS  ( 86) //  1 bytes
                                  //  8 bytes
#define STORE_WIFI_FLAGS    ( 95) //  1 bytes
#define STORE_WIFI_NAME     ( 96) // 32 bytes
#define STORE_WIFI_PASSWORD (128) // 32 bytes

#define STORE_SIZE_byte (160)

// Global variables
/////////////////////////////////////////////////////////////////////////////

EthCAN_Config gConfig;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void Init();

static void Load(void * aOut, uint8_t aOffset, unsigned int aSize_byte);
static void Store(uint8_t aOffset, const void * aIn, unsigned int aSize_byte);

// Functions
/////////////////////////////////////////////////////////////////////////////

void Config_Load()
{
    // MSG_DEBUG("Config_Load()");

    Init();

    EEPROM.begin(STORE_SIZE_byte);

    uint8_t lWhat;

    Load(&lWhat, STORE_WHAT, sizeof(lWhat));
    if (0xff == lWhat)
    {
        MSG_WARNING("Config_Load - No configuration");
        return;
    }

    Load(gConfig.mName, STORE_NAME, sizeof(gConfig.mName));
    MSG_INFO("Name : ", gConfig.mName);

    if (0 != (lWhat & EthCAN_FLAG_STORE_CAN))
    {
        Load(&gConfig.mCAN_Filters, STORE_CAN_FILTERS, sizeof(gConfig.mCAN_Filters));
        Load(&gConfig.mCAN_Masks  , STORE_CAN_MASKS  , sizeof(gConfig.mCAN_Masks));
        Load(&gConfig.mCAN_Rate   , STORE_CAN_RATE   , sizeof(gConfig.mCAN_Rate));
        // TODO Firmware.Config.Load
        //      Validate filters
    }

    if (0 != (lWhat & EthCAN_FLAG_STORE_IPv4))
    {
        Load(&gConfig.mIPv4_Addr   , STORE_IPv4_ADDR   , sizeof(gConfig.mIPv4_Addr));
        Load(&gConfig.mIPv4_Gateway, STORE_IPv4_GATEWAY, sizeof(gConfig.mIPv4_Gateway));
        Load(&gConfig.mIPv4_Mask   , STORE_IPv4_MASK   , sizeof(gConfig.mIPv4_Mask));
        // TODO Firmware.Config.Load
        //      Validate IPv4
    }

    if (0 != (lWhat & EthCAN_FLAG_STORE_SERVER))
    {
        Load(&gConfig.mServer_Flags, STORE_SERVER_FLAGS, sizeof(gConfig.mServer_Flags));
        Load(&gConfig.mServer_IPv4 , STORE_SERVER_IPv4 , sizeof(gConfig.mServer_IPv4));
        Load(&gConfig.mServer_Port , STORE_SERVER_PORT , sizeof(gConfig.mServer_Port));
        // TODO Firmware.Config.Load
        //      Validate server IPv4 and port
    }

    if (0 != (lWhat & EthCAN_FLAG_STORE_WIFI))
    {
        Load(&gConfig.mWiFi_Flags   , STORE_WIFI_FLAGS   , sizeof(gConfig.mWiFi_Flags));
        Load(&gConfig.mWiFi_Name    , STORE_WIFI_NAME    , sizeof(gConfig.mWiFi_Name));
        Load(&gConfig.mWiFi_Password, STORE_WIFI_PASSWORD, sizeof(gConfig.mWiFi_Password));
        // TODO Firmware.Config.Load
        //      Validate WiFi
    }
}

void Config_OnFrame(const EthCAN_Frame & aFrame)
{
    MSG_DEBUG("Config_OnFrame(  )");

    EthCAN_Header lHeader;

    gInfo.mMessageId ++;

    lHeader.mId             = gInfo.mMessageId;
    lHeader.mCode           = EthCAN_REQUEST_SEND;
    lHeader.mDataSize_byte  = sizeof(aFrame);
    lHeader.mFlags          = EthCAN_FLAG_NO_RESPONSE;
    lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(aFrame);
    lHeader.mResult         = EthCAN_RESULT_REQUEST;
    lHeader.mSalt           = 0;
    lHeader.mSign           = 0;

    if (0 == (gConfig.mServer_Flags & EthCAN_FLAG_SERVER_USB))
    {
        USB_OnFrame(lHeader, aFrame);
    }
    else
    {
        if (0 != gConfig.mServer_Port)
        {
            UDP_OnFrame(lHeader, aFrame, gConfig.mServer_IPv4, gConfig.mServer_Port);
        }
    }
}

void Config_Reset()
{
    MSG_DEBUG("Config_Reset()");

    Init();

    CAN_Config();
}

EthCAN_Result Config_Set(const EthCAN_Header * aIn)
{
    MSG_DEBUG("Config_Set(  )");

    if (sizeof(EthCAN_Config) > aIn->mDataSize_byte)
    {
        MSG_ERROR("Config_Set - EthCAN_ERROR_INVALID_DATA_SIZE");
        return EthCAN_ERROR_INVALID_DATA_SIZE;
    }

    const EthCAN_Config * lConfig = reinterpret_cast<const EthCAN_Config *>(aIn + 1);

    // TODO Firmware.Config.Validate

    gConfig = *lConfig;

    strcpy(gInfo.mName, gConfig.mName);

    CAN_Config();
  
    return EthCAN_OK;
}

EthCAN_Result Config_Store(const EthCAN_Header * aIn)
{
    MSG_DEBUG("Config_Store(  )");

    uint8_t lIn = aIn->mFlags;
    uint8_t lWhat;
    
    Load(&lWhat, STORE_WHAT, sizeof(lWhat));
    if (0xff == lWhat)
    {
        MSG_DEBUG("Config_Store - New configuration");
        lWhat = 0;
    }

    Store(STORE_NAME, &gConfig.mName, sizeof(gConfig.mName));

    if (0 != (lIn & EthCAN_FLAG_STORE_CAN))
    {
        Store(STORE_CAN_FILTERS, &gConfig.mCAN_Filters, sizeof(gConfig.mCAN_Filters));
        Store(STORE_CAN_MASKS  , &gConfig.mCAN_Masks  , sizeof(gConfig.mCAN_Masks));
        Store(STORE_CAN_RATE   , &gConfig.mCAN_Rate   , sizeof(gConfig.mCAN_Rate));
    }

    if (0 != (lIn & EthCAN_FLAG_STORE_IPv4))
    {
        Store(STORE_IPv4_ADDR   , &gConfig.mIPv4_Addr   , sizeof(gConfig.mIPv4_Addr));
        Store(STORE_IPv4_GATEWAY, &gConfig.mIPv4_Gateway, sizeof(gConfig.mIPv4_Gateway));
        Store(STORE_IPv4_MASK   , &gConfig.mIPv4_Mask   , sizeof(gConfig.mIPv4_Mask));
    }

    if (0 != (lIn & EthCAN_FLAG_STORE_SERVER))
    {
        Store(STORE_SERVER_FLAGS, &gConfig.mServer_Flags, sizeof(gConfig.mServer_Flags));
        Store(STORE_SERVER_IPv4 , &gConfig.mServer_IPv4 , sizeof(gConfig.mServer_IPv4));
        Store(STORE_SERVER_PORT , &gConfig.mServer_Port , sizeof(gConfig.mServer_Port));
    }

    if (0 != (lIn & EthCAN_FLAG_STORE_WIFI))
    {
        Store(STORE_WIFI_FLAGS   , &gConfig.mWiFi_Flags   , sizeof(gConfig.mWiFi_Flags));
        Store(STORE_WIFI_NAME    , &gConfig.mWiFi_Name    , sizeof(gConfig.mWiFi_Name));
        Store(STORE_WIFI_PASSWORD, &gConfig.mWiFi_Password, sizeof(gConfig.mWiFi_Password));
    }
    
    lWhat |= lIn;
    Store(STORE_WHAT, &lWhat, sizeof(lWhat));

    // TODO Firmware0.Config.Store
    //      Do not commit if nothing changed.

    EEPROM.commit();
    
    return EthCAN_OK;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void Init()
{
    // MSG_DEBUG("Init()");

    unsigned int i;

    for (i = 0; i < 6; i ++)
    {
        gConfig.mCAN_Filters[i] = 0;
    }

    for (i = 0; i < 2; i ++)
    {
        gConfig.mCAN_Masks[i] = 0;
    }

    gConfig.mCAN_Rate = EthCAN_RATE_1_Mb;

    strcpy(gConfig.mName, "EthCAN");

    gConfig.mIPv4_Addr    = 0;
    gConfig.mIPv4_Gateway = 0;
    gConfig.mIPv4_Mask    = 0;

    gConfig.mServer_Flags = 0;
    gConfig.mServer_IPv4  = 0;
    gConfig.mServer_Port  = 0;

    gConfig.mWiFi_Flags       = 0;
    gConfig.mWiFi_Name    [0] = '\0';
    gConfig.mWiFi_Password[0] = '\0';
}

void Load(void * aOut, uint8_t aOffset, unsigned int aSize_byte)
{
    uint8_t * lOut = reinterpret_cast<uint8_t *>(aOut);

    for (unsigned int i = 0; i < aSize_byte; i ++)
    {
        lOut[i] = EEPROM.read(aOffset + i);
    }
}

void Store(uint8_t aOffset, const void * aIn, unsigned int aSize_byte)
{
    const uint8_t * lIn = reinterpret_cast<const uint8_t *>(aIn);

    for (unsigned int i = 0; i < aSize_byte; i ++)
    {
        EEPROM.write(aOffset + i, lIn[i]);
    }
}
