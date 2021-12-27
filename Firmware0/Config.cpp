
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Config.h

// CODE REVIEW 2021-03-23 KMS - Martin Dubois, P.Eng.

#include <Arduino.h>

#include <EEPROM.h>

#include "Component.h"

// ===== Firmware0 ==========================================================
#include "CAN.h"
#include "Info.h"
#include "TCP.h"
#include "UDP.h"
#include "USB.h"

#include "Config.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define DEFAULT_CAN_FILTER (0)
#define DEFAULT_CAN_MASK   (0)

#define STORE_WHAT          (  0) //  1 bytes
                                  // 15 bytes
#define STORE_NAME          ( 16) // 16 bytes
#define STORE_CAN_FILTERS   ( 32) // 24 bytes
#define STORE_CAN_MASKS     ( 56) //  8 bytes
#define STORE_CAN_RATE      ( 64) //  1 bytes
#define STORE_CAN_FLAGS     ( 65) //  1 bytes
                                  //  2 bytes
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

// Static variable
/////////////////////////////////////////////////////////////////////////////

static bool sCAN = false;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void Load(void * aOut, uint8_t aOffset, unsigned int aSize_byte);
static bool Store(uint8_t aOffset, const void * aIn, unsigned int aSize_byte);

static bool Validate(const EthCAN_Config & aIn);
static bool Validate_Id(uint32_t aIn, bool aAdvanced);
static bool Validate_String(const char * aIn, unsigned int aMin_byte, unsigned int aSize_byte);

// Functions
/////////////////////////////////////////////////////////////////////////////

void Config_Erase()
{
    uint8_t lWhat;

    Load(&lWhat, STORE_WHAT, sizeof(lWhat));
    if (0xff != lWhat)
    {
        lWhat = 0xff;

        Store(STORE_WHAT, &lWhat, sizeof(lWhat));

        EEPROM.commit();
    }
}

void Config_Load()
{
    EthCAN_CONFIG_DEFAULT(&gConfig);

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
        Load(&gConfig.mCAN_Flags  , STORE_CAN_FLAGS  , sizeof(gConfig.mCAN_Flags));
        Load(&gConfig.mCAN_Masks  , STORE_CAN_MASKS  , sizeof(gConfig.mCAN_Masks));
        Load(&gConfig.mCAN_Rate   , STORE_CAN_RATE   , sizeof(gConfig.mCAN_Rate));
    }

    if (0 != (lWhat & EthCAN_FLAG_STORE_IPv4))
    {
        Load(&gConfig.mIPv4_Address, STORE_IPv4_ADDR   , sizeof(gConfig.mIPv4_Address));
        Load(&gConfig.mIPv4_Gateway, STORE_IPv4_GATEWAY, sizeof(gConfig.mIPv4_Gateway));
        Load(&gConfig.mIPv4_NetMask, STORE_IPv4_MASK   , sizeof(gConfig.mIPv4_NetMask));
    }

    if (0 != (lWhat & EthCAN_FLAG_STORE_SERVER))
    {
        Load(&gConfig.mServer_Flags, STORE_SERVER_FLAGS, sizeof(gConfig.mServer_Flags));
        Load(&gConfig.mServer_IPv4 , STORE_SERVER_IPv4 , sizeof(gConfig.mServer_IPv4));
        Load(&gConfig.mServer_Port , STORE_SERVER_PORT , sizeof(gConfig.mServer_Port));
    }

    if (0 != (lWhat & EthCAN_FLAG_STORE_WIFI))
    {
        Load(&gConfig.mWiFi_Flags   , STORE_WIFI_FLAGS   , sizeof(gConfig.mWiFi_Flags));
        Load(&gConfig.mWiFi_Name    , STORE_WIFI_NAME    , sizeof(gConfig.mWiFi_Name));
        Load(&gConfig.mWiFi_Password, STORE_WIFI_PASSWORD, sizeof(gConfig.mWiFi_Password));
    }

    if (!Validate(gConfig))
    {
        MSG_ERROR("Config_Load - Corrupted configuration", "");
        EthCAN_CONFIG_DEFAULT(&gConfig);
    }
}

void Config_Loop()
{
    if (sCAN)
    {
        CAN_Config();
        sCAN = false;
    }
}

// Critical path
void Config_OnFrame(const EthCAN_Frame & aFrame)
{
    EthCAN_Header lHeader;

    lHeader.mId             = Info_Get_MessageId();
    lHeader.mCode           = EthCAN_REQUEST_SEND;
    lHeader.mDataSize_byte  = sizeof(aFrame);
    lHeader.mFlags          = EthCAN_FLAG_NO_RESPONSE;
    lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(aFrame);
    lHeader.mResult         = EthCAN_RESULT_REQUEST;

    if (EthCAN_FLAG_SERVER_USB == (gConfig.mServer_Flags & EthCAN_FLAG_SERVER_USB))
    {
        USB_OnFrame(lHeader, aFrame);
        Info_Count_Fx_Frame(aFrame.mDataSize_byte);
    }
    else if (EthCAN_FLAG_SERVER_TCP == (gConfig.mServer_Flags & EthCAN_FLAG_SERVER_TCP))
    {
        if (TCP_OnFrame(lHeader, aFrame))
        {
            Info_Count_Fx_Frame(aFrame.mDataSize_byte);
        }
    }
    else
    {
        if (0 != gConfig.mServer_Port)
        {
            UDP_OnFrame(lHeader, aFrame, gConfig.mServer_IPv4, gConfig.mServer_Port);
            Info_Count_Fx_Frame(aFrame.mDataSize_byte);
        }
    }
}

uint8_t Config_Reset()
{
    sCAN = (0 != gConfig.mCAN_Flags) || (EthCAN_RATE_DEFAULT != gConfig.mCAN_Rate);
    if (!sCAN)
    {
        unsigned int i = 0;
        for (i = 0; i < EthCAN_FILTER_QTY; i ++)
        {
            if (DEFAULT_CAN_FILTER != gConfig.mCAN_Filters[i])
            {
                sCAN = true;
                break;
            }
        }

        if (!sCAN)
        {
            for (i = 0; i < EthCAN_MASK_QTY; i ++)
            {
                if (DEFAULT_CAN_MASK != gConfig.mCAN_Masks[i])
                {
                    sCAN = true;
                    break;
                }
            }
        }
    }

    EthCAN_CONFIG_DEFAULT(&gConfig);

    return sCAN ? EthCAN_FLAG_BUSY : 0;
}

EthCAN_Result Config_Set(const EthCAN_Header * aIn, uint8_t * aFlags)
{
    if (sizeof(EthCAN_Config) > aIn->mDataSize_byte)
    {
        return Info_Count_Error(__LINE__, EthCAN_ERROR_DATA_SIZE);
    }

    const EthCAN_Config * lConfig = reinterpret_cast<const EthCAN_Config *>(aIn + 1);

    if (!Validate(*lConfig))
    {
        return Info_Count_Error(__LINE__, EthCAN_ERROR_CONFIG);
    }

    sCAN = (gConfig.mCAN_Flags != lConfig->mCAN_Flags) || (gConfig.mCAN_Rate != lConfig->mCAN_Rate);
    if (!sCAN)
    {
        unsigned int i;

        for (i = 0; i < EthCAN_FILTER_QTY; i ++)
        {
            if (gConfig.mCAN_Filters[i] != lConfig->mCAN_Filters[i])
            {
                sCAN = true;
                break;
            }
        }

        if (!sCAN)
        {
            for (i = 0; i < EthCAN_MASK_QTY; i ++)
            {
                if (gConfig.mCAN_Masks[i] != lConfig->mCAN_Masks[i])
                {
                    sCAN = true;
                    break;
                }
            }
        }
    }

    gConfig = *lConfig;

    Info_Set_Name(gConfig.mName);

    *aFlags = sCAN ? EthCAN_FLAG_BUSY : 0;

    return EthCAN_OK;
}

EthCAN_Result Config_Store(const EthCAN_Header * aIn)
{
    uint8_t lIn = aIn->mFlags;
    uint8_t lWhat;
    
    Load(&lWhat, STORE_WHAT, sizeof(lWhat));
    if (0xff == lWhat)
    {
        MSG_INFO("Config_Store - New configuration", "");
        lWhat = 0;
    }

    bool lCommit = Store(STORE_NAME, &gConfig.mName, sizeof(gConfig.mName));

    if (0 != (lIn & EthCAN_FLAG_STORE_CAN))
    {
        lCommit |= Store(STORE_CAN_FILTERS, &gConfig.mCAN_Filters, sizeof(gConfig.mCAN_Filters));
        lCommit |= Store(STORE_CAN_FLAGS  , &gConfig.mCAN_Flags  , sizeof(gConfig.mCAN_Flags));
        lCommit |= Store(STORE_CAN_MASKS  , &gConfig.mCAN_Masks  , sizeof(gConfig.mCAN_Masks));
        lCommit |= Store(STORE_CAN_RATE   , &gConfig.mCAN_Rate   , sizeof(gConfig.mCAN_Rate));
    }

    if (0 != (lIn & EthCAN_FLAG_STORE_IPv4))
    {
        lCommit |= Store(STORE_IPv4_ADDR   , &gConfig.mIPv4_Address, sizeof(gConfig.mIPv4_Address));
        lCommit |= Store(STORE_IPv4_GATEWAY, &gConfig.mIPv4_Gateway, sizeof(gConfig.mIPv4_Gateway));
        lCommit |= Store(STORE_IPv4_MASK   , &gConfig.mIPv4_NetMask, sizeof(gConfig.mIPv4_NetMask));
    }

    if (0 != (lIn & EthCAN_FLAG_STORE_SERVER))
    {
        lCommit |= Store(STORE_SERVER_FLAGS, &gConfig.mServer_Flags, sizeof(gConfig.mServer_Flags));
        lCommit |= Store(STORE_SERVER_IPv4 , &gConfig.mServer_IPv4 , sizeof(gConfig.mServer_IPv4));
        lCommit |= Store(STORE_SERVER_PORT , &gConfig.mServer_Port , sizeof(gConfig.mServer_Port));
    }

    if (0 != (lIn & EthCAN_FLAG_STORE_WIFI))
    {
        lCommit |= Store(STORE_WIFI_FLAGS   , &gConfig.mWiFi_Flags   , sizeof(gConfig.mWiFi_Flags));
        lCommit |= Store(STORE_WIFI_NAME    , &gConfig.mWiFi_Name    , sizeof(gConfig.mWiFi_Name));
        lCommit |= Store(STORE_WIFI_PASSWORD, &gConfig.mWiFi_Password, sizeof(gConfig.mWiFi_Password));
    }
    
    lWhat |= lIn;
    lCommit |= Store(STORE_WHAT, &lWhat, sizeof(lWhat));

    if (lCommit)
    {
        EEPROM.commit();
    }
    
    return EthCAN_OK;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void Load(void * aOut, uint8_t aOffset, unsigned int aSize_byte)
{
    uint8_t * lOut = reinterpret_cast<uint8_t *>(aOut);

    for (unsigned int i = 0; i < aSize_byte; i ++)
    {
        lOut[i] = EEPROM.read(aOffset + i);
    }
}

bool Store(uint8_t aOffset, const void * aIn, unsigned int aSize_byte)
{
    const uint8_t * lIn = reinterpret_cast<const uint8_t *>(aIn);
    bool lResult = false;

    for (unsigned int i = 0; i < aSize_byte; i ++)
    {
        uint8_t lByte = EEPROM.read(aOffset + i);
        if (lIn[i] != lByte)
        {
            EEPROM.write(aOffset + i, lIn[i]);
            lResult = true;
        }
    }

    return lResult;
}

bool Validate(const EthCAN_Config & aIn)
{
    if (!Validate_String(aIn.mName, 1, EthCAN_NAME_SIZE_byte)) { return false; }

    unsigned int i;

    for (i = 0; i < EthCAN_FILTER_QTY; i++)
    {
        if (!Validate_Id(aIn.mCAN_Filters[i], aIn.mCAN_Flags & EthCAN_FLAG_CAN_ADVANCED)) { return false; }
    }

    for (i = 0; i < EthCAN_MASK_QTY; i++)
    {
        if (!Validate_Id(aIn.mCAN_Masks[i], aIn.mCAN_Flags & EthCAN_FLAG_CAN_ADVANCED)) { return false; }
    }

    if (EthCAN_RATE_QTY <= aIn.mCAN_Rate) { return false; }

    if (0 != aIn.mIPv4_Address)
    {
        if (0 == aIn.mIPv4_NetMask) { return false; }

        if (0 == (aIn.mIPv4_Address &    aIn.mIPv4_NetMask )) { return false; }
        if (0 == (aIn.mIPv4_Address & (~ aIn.mIPv4_NetMask))) { return false; }

        if (0 != aIn.mIPv4_Gateway)
        {
            if ((aIn.mIPv4_Address & aIn.mIPv4_NetMask) != (aIn.mIPv4_Gateway & aIn.mIPv4_NetMask)) { return false; }
        }
    }

    unsigned int lMin_byte = 0;

    if (EthCAN_FLAG_WIFI_AP == (aIn.mWiFi_Flags & EthCAN_FLAG_WIFI_AP))
    {
        if (0 == aIn.mIPv4_Address) { return false; }

        lMin_byte = 1;
    }

    if (!Validate_String(aIn.mWiFi_Name    , lMin_byte, EthCAN_WIFI_NAME_SIZE_byte    )) { return false; }
    if (!Validate_String(aIn.mWiFi_Password, lMin_byte, EthCAN_WIFI_PASSWORD_SIZE_byte)) { return false; }

    return true;
}

bool Validate_Id(uint32_t aIn, bool aAdvanced)
{
    uint32_t lMask;

    if (aAdvanced)
    {
        lMask = (0 != (aIn & 0x00000800)) ? 0x00001400 : 0x00001700;
    }
    else
    {
        lMask = (EthCAN_ID_EXTENDED == (aIn & EthCAN_ID_EXTENDED)) ? 0x60000000 : 0x7ffff800;
    }

    return 0 == (aIn & lMask);
}

bool Validate_String(const char * aIn, unsigned int aMin_byte, unsigned int aSize_byte)
{
    for (unsigned int i = 0; i < aSize_byte; i ++)
    {
        if ('\0' == aIn[i])
        {
            MSG_ERROR("Validate_String - Too short - ", aMin_byte);
            return (aMin_byte <= i);
        }
    }

    MSG_ERROR("Validate_String - Too long - ", aSize_byte)
    return false;
}
