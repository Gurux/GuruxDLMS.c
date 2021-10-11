//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL:  $
//
// Version:         $Revision:  $,
//                  $Date:  $
//                  $Author: $
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h> // malloc and free needs this or error is generated.

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#if _MSC_VER > 1400
#include <crtdbg.h>
#include <malloc.h>
#endif
#include <tchar.h>
#include <conio.h>
#include <time.h>
#include <process.h>//Add support for threads
#include <Winsock2.h> //Add support for sockets
#include "../include/connection.h"
#include "../../development/include/converters.h"
#include "../../development/include/helpers.h"
#include "../../development/include/date.h"
#endif
#if defined(__linux__) //Linux includes.
#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h> //Add support for sockets
#include <unistd.h> //Add support for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include "../include/connection.h"
#include "../../development/include/converters.h"
#include "../../development/include/helpers.h"
#include "../../development/include/date.h"
#endif

#include "../include/exampleserver.h"
#include "../../development/include/cosem.h"
#include "../../development/include/gxkey.h"
#include "../../development/include/gxobjects.h"
#include "../../development/include/gxget.h"
#include "../../development/include/gxset.h"
//Add this if you want to send notify messages.
#include "../../development/include/notify.h"
//Add support for serialization.
#include "../../development/include/gxserializer.h"

GX_TRACE_LEVEL trace = GX_TRACE_LEVEL_OFF;

const static char* FLAG_ID = "GRX";
//Serialization version is increased every time when structure of serialized data is changed.
const static uint16_t SERIALIZATION_VERSION = 2;

//Space for client challenge.
static unsigned char C2S_CHALLENGE[64];
//Space for server challenge.
static unsigned char S2C_CHALLENGE[64];
//Allocate space for read list.
static gxValueEventArg events[10];

unsigned char testMode = 1;
int socket1 = -1;
uint32_t SERIAL_NUMBER = 123456;

//TODO: Allocate space where profile generic row values are serialized.
#define PDU_MAX_PROFILE_GENERIC_COLUMN_SIZE 100
#define HDLC_HEADER_SIZE 17
#define HDLC_BUFFER_SIZE 128
#define PDU_BUFFER_SIZE 512
#define WRAPPER_BUFFER_SIZE 8 + PDU_BUFFER_SIZE
//Buffer where frames are saved.
static unsigned char frameBuff[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
//Buffer where PDUs are saved.
static unsigned char pduBuff[PDU_BUFFER_SIZE];
static unsigned char replyFrame[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
//Define server system title.
static unsigned char SERVER_SYSTEM_TITLE[8] = { 0 };
time_t imageActionStartTime = 0;
gxImageActivateInfo IMAGE_ACTIVATE_INFO[1];
static gxByteBuffer reply;

uint32_t time_current(void)
{
    //Get current time somewhere.
    return (uint32_t)time(NULL);
}

uint32_t time_elapsed(void)
{
    return (uint32_t)clock() / (CLOCKS_PER_SEC / 1000);
}

//In this example we wait 5 seconds before image is verified or activated.
time_t imageActionStartTime;

static gxClock clock1;
static gxIecHdlcSetup hdlc;
static gxData ldn, id1, id2, fw;
static gxData eventCode;
static gxData unixTime;
static gxData invocationCounter;
static gxAssociationLogicalName associationNone;
static gxAssociationLogicalName associationLow;
static gxAssociationLogicalName associationHigh;
static gxAssociationLogicalName associationHighGMac;
static gxRegister activePowerL1;
static gxScriptTable scriptTableGlobalMeterReset;
static gxScriptTable scriptTableDisconnectControl;
static gxScriptTable scriptTableActivateTestMode;
static gxScriptTable scriptTableActivateNormalMode;
static gxProfileGeneric eventLog;
static gxActionSchedule actionScheduleDisconnectOpen;
static gxActionSchedule actionScheduleDisconnectClose;
static gxPushSetup pushSetup;
static gxDisconnectControl disconnectControl;
static gxProfileGeneric loadProfile;
static gxSapAssignment sapAssignment;
//Security Setup High is for High authentication.
static gxSecuritySetup securitySetupHigh;
//Security Setup HighGMac is for GMac authentication.
static gxSecuritySetup securitySetupHighGMac;

gxImageTransfer imageTransfer;
gxAutoConnect autoConnect;
gxActivityCalendar activityCalendar;
gxLocalPortSetup localPortSetup;
gxDemandRegister demandRegister;
gxRegisterMonitor registerMonitor;
gxAutoAnswer autoAnswer;
gxModemConfiguration modemConfiguration;
gxMacAddressSetup macAddressSetup;
gxTcpUdpSetup udpSetup;
gxIp4Setup ip4Setup;
gxPppSetup pppSetup;
gxGPRSSetup gprsSetup;
gxScriptTable tarifficationScriptTable;
gxRegisterActivation registerActivation;
gxCompactData compactData;

//static gxObject* NONE_OBJECTS[] = { BASE(associationNone), BASE(ldn) };

static gxObject* ALL_OBJECTS[] = { BASE(associationNone), BASE(associationLow), BASE(associationHigh), BASE(associationHighGMac), BASE(securitySetupHigh), BASE(securitySetupHighGMac),
                                   BASE(ldn), BASE(sapAssignment), BASE(eventCode),
                                   BASE(clock1), BASE(activePowerL1), BASE(pushSetup), BASE(scriptTableGlobalMeterReset), BASE(scriptTableDisconnectControl),
                                   BASE(scriptTableActivateTestMode), BASE(scriptTableActivateNormalMode), BASE(loadProfile), BASE(eventLog), BASE(hdlc),
                                   BASE(disconnectControl), BASE(actionScheduleDisconnectOpen), BASE(actionScheduleDisconnectClose), BASE(unixTime), BASE(invocationCounter),
                                   BASE(imageTransfer), BASE(udpSetup), BASE(autoConnect), BASE(activityCalendar), BASE(localPortSetup), BASE(demandRegister),
                                   BASE(registerMonitor), BASE(autoAnswer), BASE(modemConfiguration), BASE(macAddressSetup), BASE(ip4Setup), BASE(pppSetup), BASE(gprsSetup),
                                   BASE(tarifficationScriptTable), BASE(registerActivation)
};

////////////////////////////////////////////////////
//Define what is serialized to decrease EEPROM usage.
gxSerializerIgnore NON_SERIALIZED_OBJECTS[] = {
    //Nothing is saved when authentication is not used.
    IGNORE_ATTRIBUTE(BASE(associationNone), GET_ATTRIBUTE_ALL()),
    //Only password is saved for low and high authentication.
    IGNORE_ATTRIBUTE(BASE(associationLow), GET_ATTRIBUTE_EXCEPT(7)),
    IGNORE_ATTRIBUTE(BASE(associationHigh), GET_ATTRIBUTE_EXCEPT(7)),
    //Only scaler and unit are saved for all register objects.
    IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_REGISTER, GET_ATTRIBUTE(2)) };

static uint32_t executeTime = 0;

static uint16_t activePowerL1Value = 0;

typedef enum
{
    //Meter is powered.
    GURUX_EVENT_CODES_POWER_UP = 0x1,
    //User has change the time.
    GURUX_EVENT_CODES_TIME_CHANGE = 0x2,
    //DST status is changed.
    GURUX_EVENT_CODES_DST = 0x4,
    //Push message is sent.
    GURUX_EVENT_CODES_PUSH = 0x8,
    //Meter makes auto connect.
    GURUX_EVENT_CODES_AUTO_CONNECT = 0x10,
    //User has change the password.
    GURUX_EVENT_CODES_PASSWORD_CHANGED = 0x20,
    //Wrong password tried 3 times.
    GURUX_EVENT_CODES_WRONG_PASSWORD = 0x40,
    //Disconnect control state is changed.
    GURUX_EVENT_CODES_OUTPUT_RELAY_STATE = 0x80,
    //User has reset the meter.
    GURUX_EVENT_CODES_GLOBAL_METER_RESET = 0x100
} GURUX_EVENT_CODES;


/////////////////////////////////////////////////////////////////////////////
// Save security settings to the EEPROM.
//
// Only updated value is saved. This is done because write to EEPROM is slow
// and there is a limit how many times value can be written to the EEPROM.
/////////////////////////////////////////////////////////////////////////////
int saveSecurity(
    dlmsSettings* settings)
{
    int ret = 0;
    const char* fileName = "security.raw";
    //Save keys to own block in EEPROM.
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, fileName, "wb");
#else
    FILE* f = fopen(fileName, "wb");
#endif
    gxByteBuffer bb;
    bb_init(&bb);
    bb_capacity(&bb, 256);
    if (f != NULL)
    {
        if ((ret = bb_set(&bb, settings->cipher.blockCipherKey.data, 16)) == 0 &&
            (ret = bb_set(&bb, settings->cipher.authenticationKey.data, 16)) == 0 &&
            (ret = bb_set(&bb, settings->kek.data, 16)) == 0 &&
            //Save server IV.
            (ret = bb_setUInt32(&bb, settings->cipher.invocationCounter)) == 0 &&
            //Save last client IV.
            (ret = bb_setUInt32(&bb, securitySetupHighGMac.minimumInvocationCounter)) == 0)
        {
            fwrite(bb.data, 1, bb.size, f);
        }
        bb_clear(&bb);
        fclose(f);
    }
    else
    {
        printf("%s\r\n", "Failed to open keys file.");
    }
    return ret;
}


/////////////////////////////////////////////////////////////////////////////
// Save data to the EEPROM.
//
// Only updated value is saved. This is done because write to EEPROM is slow
// and there is a limit how many times value can be written to the EEPROM.
/////////////////////////////////////////////////////////////////////////////
int saveSettings()
{
    int ret = 0;
    const char* fileName = "settings.raw";
    //Save keys to own block in EEPROM.
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, fileName, "wb");
#else
    FILE* f = fopen(fileName, "wb");
#endif
    gxByteBuffer bb;
    bb_init(&bb);
    bb_capacity(&bb, 256);
    if (f != NULL)
    {
        gxSerializerSettings serializerSettings;
        serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
        serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
        if ((ret = ser_saveObjects(&serializerSettings, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]), &bb)) == 0)
        {
            fwrite(bb.data, bb.size, 1, f);
        }
        bb_clear(&bb);
        fclose(f);
    }
    else
    {
        printf("%s\r\n", "Failed to open settings file.");
    }
    return ret;
}

//Allocate profile generic buffer.
void allocateProfileGenericBuffer(const char* fileName, uint32_t size)
{
    uint32_t pos;
    FILE* f = NULL;
#if _MSC_VER > 1400
    fopen_s(&f, fileName, "ab");
#else
    f = fopen(fileName, "ab");
#endif
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        if (ftell(f) == 0)
        {
            for (pos = 0; pos != size; ++pos)
            {
                if (fputc(0x00, f) != 0)
                {
                    printf("Error Writing to %s\n", fileName);
                    break;
                }
            }
        }
        fclose(f);
    }
}

int getProfileGenericFileName(gxProfileGeneric* pg, char* fileName)
{
    int ret = hlp_getLogicalNameToString(pg->base.logicalName, fileName);
#if defined(_WIN64)
    strcat(fileName, "64.raw");
#else // defined(_WIN32) || defined(__linux__)
    strcat(fileName, ".raw");
#endif //defined(_WIN32) || defined(__linux__)
    return ret;
}

//Returns profile generic buffer column sizes.
int getProfileGenericBufferColumnSizes(
    dlmsSettings* settings,
    gxProfileGeneric* pg,
    DLMS_DATA_TYPE* dataTypes,
    uint8_t* columnSizes,
    uint16_t* rowSize)
{
    int ret = 0;
    uint8_t pos;
    gxKey* it;
    gxValueEventArg e;
    ve_init(&e);
    *rowSize = 0;
    uint16_t size;
    unsigned char type;
    //Loop capture columns and get values.
    for (pos = 0; pos != pg->captureObjects.size; ++pos)
    {
        if ((ret = arr_getByIndex(&pg->captureObjects, (uint16_t)pos, (void**)&it)) != 0)
        {
            break;
        }
        //Date time is saved in EPOCH to save space.
        if (((gxObject*)it->key)->objectType == DLMS_OBJECT_TYPE_CLOCK && ((gxTarget*)it->value)->attributeIndex == 2)
        {
            type = DLMS_DATA_TYPE_UINT32;
            size = 4;
        }
        else
        {
            e.target = (gxObject*)it->key;
            e.index = ((gxTarget*)it->value)->attributeIndex;
            if ((ret = cosem_getValue(settings, &e)) != 0)
            {
                break;
            }
            if (bb_size(e.value.byteArr) != 0)
            {
                if ((ret = bb_getUInt8(e.value.byteArr, &type)) != 0)
                {
                    break;
                }
                size = bb_available(e.value.byteArr);
            }
            else
            {
                type = DLMS_DATA_TYPE_NONE;
                size = 0;
            }
        }
        if (dataTypes != NULL)
        {
            dataTypes[pos] = type;
        }
        if (columnSizes != NULL)
        {
            columnSizes[pos] = (uint8_t)size;
        }
        *rowSize += (uint16_t)size;
        ve_clear(&e);
    }
    ve_clear(&e);
    return ret;
}

//Get max row count for allocated buffer.
uint16_t getProfileGenericBufferMaxRowCount(
    dlmsSettings* settings,
    gxProfileGeneric* pg)
{
    uint16_t count = 0;
    char fileName[30];
    //Allocate space for load profile buffer.
    getProfileGenericFileName(pg, fileName);
    uint16_t rowSize = 0;
    FILE* f = NULL;
#if _MSC_VER > 1400
    fopen_s(&f, fileName, "r+b");
#else
    f = fopen(fileName, "r+b");
#endif
    if (f == NULL)
    {
        //Allocate space for the profile generic buffer.
        allocateProfileGenericBuffer(fileName, 1024);
#if _MSC_VER > 1400
        fopen_s(&f, fileName, "r+b");
#else
        f = fopen(fileName, "r+b");
#endif
    }
    if (f != NULL)
    {
        getProfileGenericBufferColumnSizes(settings, pg, NULL, NULL, &rowSize);
        if (rowSize != 0)
        {
            fseek(f, 0L, SEEK_END);
            count = (uint16_t)ftell(f);
            //Decrease current index and total amount of the entries.
            count -= 4;
            count /= rowSize;
        }
        fclose(f);
    }
    return count;
}

//Get current row count for allocated buffer.
uint16_t getProfileGenericBufferEntriesInUse(gxProfileGeneric* pg)
{
    uint16_t index = 0;
    int ret = 0;
    char fileName[30];
    getProfileGenericFileName(pg, fileName);
    FILE* f = NULL;
#if _MSC_VER > 1400
    fopen_s(&f, fileName, "r+b");
#else
    f = fopen(fileName, "r+b");
#endif
    if (f != NULL)
    {
        uint16_t dataSize = 0;
        //Load current entry index from the begin of the data.
        unsigned char pduBuff[2];
        gxByteBuffer pdu;
        bb_attach(&pdu, pduBuff, 0, sizeof(pduBuff));
        if (fread(pdu.data, 1, 2, f) == 2)
        {
            pdu.size = 2;
            bb_getUInt16(&pdu, &index);
            fseek(f, 0, SEEK_SET);
            bb_empty(&pdu);
        }
        fclose(f);
    }
    return index;
}

int captureProfileGeneric(
    dlmsSettings* settings,
    gxProfileGeneric* pg)
{
    unsigned char pos;
    gxKey* it;
    int ret = 0;
    char fileName[30];
    getProfileGenericFileName(pg, fileName);
    unsigned char pduBuff[PDU_MAX_PROFILE_GENERIC_COLUMN_SIZE];
    gxByteBuffer pdu;
    bb_attach(&pdu, pduBuff, 0, sizeof(pduBuff));
    gxValueEventArg e;
    ve_init(&e);
    FILE* f = NULL;
#if _MSC_VER > 1400
    fopen_s(&f, fileName, "r+b");
#else
    f = fopen(fileName, "r+b");
#endif
    if (f != NULL)
    {
        uint16_t dataSize = 0;
        uint8_t columnSizes[10];
        DLMS_DATA_TYPE dataTypes[10];
        //Load current entry index from the begin of the data.
        uint16_t index = 0;
        if (fread(pdu.data, 1, 2, f) == 2)
        {
            pdu.size = 2;
            bb_getUInt16(&pdu, &index);
            fseek(f, 0, SEEK_SET);
            bb_empty(&pdu);
        }
        //Current index in ring buffer.
        if (pg->profileEntries != 0)
        {
            bb_setUInt16(&pdu, (1 + index) % (pg->profileEntries));
        }

        //Update how many entries is used until buffer is full.
        if (ret == 0 && pg->entriesInUse != pg->profileEntries)
        {
            //Total amount of the entries.
            ++pg->entriesInUse;
        }
        bb_setUInt16(&pdu, (uint16_t)pg->entriesInUse);
        //Update values to the EEPROM.
        fwrite(pdu.data, 1, 4, f);
        getProfileGenericBufferColumnSizes(settings, pg, dataTypes, columnSizes, &dataSize);
        if (index != 0 && pg->profileEntries != 0)
        {
            fseek(f, 4 + ((index % pg->profileEntries) * dataSize), SEEK_SET);
        }
        //Loop capture columns and get values.
        for (pos = 0; pos != pg->captureObjects.size; ++pos)
        {
            if ((ret = arr_getByIndex(&pg->captureObjects, pos, (void**)&it)) != 0)
            {
                break;
            }
            bb_clear(&pdu);
            //Date time is saved in EPOCH to save space.
            if ((((gxObject*)it->key)->objectType == DLMS_OBJECT_TYPE_CLOCK || ((gxObject*)it->key) == BASE(unixTime)) &&
                ((gxTarget*)it->value)->attributeIndex == 2)
            {
                e.value.ulVal = time_current();
                e.value.vt = DLMS_DATA_TYPE_UINT32;
                fwrite(&e.value.bVal, 4, 1, f);
            }
            else
            {
                e.target = (gxObject*)it->key;
                e.index = ((gxTarget*)it->value)->attributeIndex;
                e.value.byteArr = &pdu;
                e.value.vt = DLMS_DATA_TYPE_OCTET_STRING;
                if ((ret = cosem_getValue(settings, &e)) != 0)
                {
                    break;
                }
                //Data type is not serialized. For that reason first byte is ignored.
                fwrite(&e.value.byteArr->data[1], e.value.byteArr->size - 1, 1, f);
            }
        }
        fclose(f);
        if (ret != 0)
        {
            //Total amount of the entries.
            --pg->entriesInUse;
        }
    }
    //Append data.
    return ret;
}

void updateState(
    dlmsSettings* settings,
    uint16_t value)
{
    GX_UINT16(eventCode.value) = value;
    captureProfileGeneric(settings, &eventLog);
}
///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char* str, const char* data)
{
    //Send trace to the serial port in test mode.
    if (testMode)
    {
        if (data == NULL)
        {
            printf("%s\r\n", str);
        }
        else
        {
            printf("%s %s\r\n", str, data);
        }
    }
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_INT(const char* str, int32_t value)
{
    char data[10];
    sprintf(data, " %ld", value);
    GXTRACE(str, data);
}

///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE_LN(const char* str, uint16_t type, unsigned char* ln)
{
    char buff[30];
    sprintf(buff, "%d %d.%d.%d.%d.%d.%d", type, ln[0], ln[1], ln[2], ln[3], ln[4], ln[5]);
    GXTRACE(str, buff);
}

//Returns current time.
//If you are not using operating system you have to implement this by yourself.
//Reason for this is that all compilers's or HWs don't support time at all.
void time_now(
    gxtime* value, unsigned char meterTime)
{
    time_initUnix(value, (unsigned long)time(NULL));
    //If date time is wanted in meter time.
    if (meterTime)
    {
        clock_utcToMeterTime(&clock1, value);
    }
}

void println(char* desc, gxByteBuffer* data)
{
    if (data != NULL)
    {
        char* str = bb_toHexString(data);
        printf("%s: %s\r\n", desc, str);
        free(str);
    }
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociationNone()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
    if ((ret = INIT_OBJECT(associationNone, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        //All objects are shown also without authentication.
        OA_ATTACH(associationNone.objectList, ALL_OBJECTS);
        //Uncomment this if you want to show only part of the objects without authentication.
        //OA_ATTACH(associationNone.objectList, NONE_OBJECTS);
        associationNone.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        associationNone.clientSAP = 0x10;
        //Max PDU is half of PDU size. This is for demonstration purposes only.
        associationNone.xDLMSContextInfo.maxSendPduSize = associationNone.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE / 2;
        associationNone.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_GET | DLMS_CONFORMANCE_SET);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociationLow()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 2, 255 };
    if ((ret = INIT_OBJECT(associationLow, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        //Only Logical Device Name is add to this Association View.
        OA_ATTACH(associationLow.objectList, ALL_OBJECTS);
        associationLow.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_LOW;
        associationLow.clientSAP = 0x11;
        associationLow.xDLMSContextInfo.maxSendPduSize = associationLow.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationLow.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
            DLMS_CONFORMANCE_SET |
            DLMS_CONFORMANCE_SELECTIVE_ACCESS |
            DLMS_CONFORMANCE_ACTION |
            DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
            DLMS_CONFORMANCE_GET);
        bb_addString(&associationLow.secret, "Gurux");
        associationLow.securitySetup = NULL;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for High authentication.
// UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addAssociationHigh()
{
    int ret;
    //Dedicated key.
    static unsigned char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 3, 255 };
    if ((ret = INIT_OBJECT(associationHigh, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHigh.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH;
        OA_ATTACH(associationHigh.objectList, ALL_OBJECTS);
        BB_ATTACH(associationHigh.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        associationHigh.clientSAP = 0x12;
        associationHigh.xDLMSContextInfo.maxSendPduSize = associationHigh.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationHigh.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
            DLMS_CONFORMANCE_SET |
            DLMS_CONFORMANCE_SELECTIVE_ACCESS |
            DLMS_CONFORMANCE_ACTION |
            DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
            DLMS_CONFORMANCE_GET);
        bb_addString(&associationHigh.secret, "Gurux");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        associationHigh.securitySetup = &securitySetupHigh;
#else
        memcpy(associationHigh.securitySetupReference, securitySetupHigh.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for GMAC High authentication.
// UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addAssociationHighGMac()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 4, 255 };
    if ((ret = INIT_OBJECT(associationHighGMac, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHighGMac.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH_GMAC;
        OA_ATTACH(associationHighGMac.objectList, ALL_OBJECTS);
        associationHighGMac.clientSAP = 0x1;
        associationHighGMac.xDLMSContextInfo.maxSendPduSize = associationHighGMac.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationHighGMac.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
            DLMS_CONFORMANCE_SET |
            DLMS_CONFORMANCE_SELECTIVE_ACCESS |
            DLMS_CONFORMANCE_ACTION |
            DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
            DLMS_CONFORMANCE_GET);
        //GMAC authentication don't need password.
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        associationHighGMac.securitySetup = &securitySetupHighGMac;
#else
        memcpy(associationHighGMac.securitySetupReference, securitySetupHigh.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds security setup object for High authentication.
///////////////////////////////////////////////////////////////////////
int addSecuritySetupHigh()
{
    int ret;
    //Define client system title.
    static unsigned char CLIENT_SYSTEM_TITLE[8] = { 0 };
    const unsigned char ln[6] = { 0, 0, 43, 0, 1, 255 };
    if ((ret = INIT_OBJECT(securitySetupHigh, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
    {
        BB_ATTACH(securitySetupHigh.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
        BB_ATTACH(securitySetupHigh.clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
        securitySetupHigh.securityPolicy = DLMS_SECURITY_POLICY_NOTHING;
        securitySetupHigh.securitySuite = DLMS_SECURITY_SUITE_V0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds security setup object for High GMAC authentication.
///////////////////////////////////////////////////////////////////////
int addSecuritySetupHighGMac()
{
    int ret;
    //Define client system title.
    static unsigned char CLIENT_SYSTEM_TITLE[8] = { 0 };
    const unsigned char ln[6] = { 0, 0, 43, 0, 2, 255 };
    if ((ret = INIT_OBJECT(securitySetupHighGMac, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
    {
        BB_ATTACH(securitySetupHighGMac.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
        BB_ATTACH(securitySetupHighGMac.clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
        //Only Authenticated encrypted connections are allowed.
        securitySetupHighGMac.securityPolicy = DLMS_SECURITY_POLICY_AUTHENTICATED_ENCRYPTED;
        securitySetupHighGMac.securitySuite = DLMS_SECURITY_SUITE_V0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
    const unsigned char ln[6] = { 1,1,21,25,0,255 };
    INIT_OBJECT(activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln);
    //10 ^ 3 =  1000
    GX_UINT16_BYREF(activePowerL1.value, activePowerL1Value);
    activePowerL1.scaler = -2;
    activePowerL1.unit = 30;
    return 0;
}

uint16_t readActivePowerValue()
{
    return ++activePowerL1Value;
}

uint16_t readEventCode()
{
    return eventCode.value.uiVal;
}



///////////////////////////////////////////////////////////////////////
//Add tariffication script table object.
///////////////////////////////////////////////////////////////////////
int addTarifficationScriptTable()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 10, 0, 100, 255 };
    if ((ret = INIT_OBJECT(tarifficationScriptTable, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript* s1 = (gxScript*)malloc(sizeof(gxScript));
        gxScript* s2 = (gxScript*)malloc(sizeof(gxScript));
        s1->id = 1;
        arr_init(&s1->actions);
        s2->id = 2;
        arr_init(&s2->actions);
        arr_push(&tarifficationScriptTable.scripts, s1);
        arr_push(&tarifficationScriptTable.scripts, s2);
        gxScriptAction* a = (gxScriptAction*)malloc(sizeof(gxScriptAction));
        arr_push(&s1->actions, a);
        a->type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        a->target = BASE(registerActivation);
        a->index = 1;
        var_init(&a->parameter);
        //Action data is Int8 zero.
        GX_INT8(a->parameter) = 0;

        a = (gxScriptAction*)malloc(sizeof(gxScriptAction));
        arr_push(&s2->actions, a);
        a->type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        a->target = BASE(registerActivation);
        a->index = 1;
        var_init(&a->parameter);
        //Action data is Int8 zero.
        GX_INT8(a->parameter) = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add register activation.
///////////////////////////////////////////////////////////////////////
int addRegisterActivation(dlmsServerSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 14, 0, 1, 255 };
    if ((ret = INIT_OBJECT(registerActivation, DLMS_OBJECT_TYPE_REGISTER_ACTIVATION, ln)) == 0)
    {
        bb_init(&registerActivation.activeMask);
        bb_addString(&registerActivation.activeMask, "RATE1");
        oa_init(&registerActivation.registerAssignment);
        oa_push(&registerActivation.registerAssignment, BASE(activePowerL1));

        arr_init(&registerActivation.maskList);
        gxByteBuffer* name = malloc(sizeof(gxByteBuffer));
        bb_init(name);
        bb_addString(name, "RATE1");
        gxByteBuffer* indexes = malloc(sizeof(gxByteBuffer));
        bb_init(indexes);
        bb_setUInt8(indexes, 1);
        arr_push(&registerActivation.maskList, key_init(name, indexes));
        name = malloc(sizeof(gxByteBuffer));
        bb_init(name);
        bb_addString(name, "RATE2");
        indexes = malloc(sizeof(gxByteBuffer));
        bb_init(indexes);
        bb_setUInt8(indexes, 1);
        bb_setUInt8(indexes, 2);
        arr_push(&registerActivation.maskList, key_init(name, indexes));
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add script table object for meter reset. This will erase the EEPROM.
///////////////////////////////////////////////////////////////////////
int addscriptTableGlobalMeterReset()
{
    int ret;
    static gxScript SCRIPTS[1] = { 0 };
    const unsigned char ln[6] = { 0, 0, 10, 0, 0, 255 };
    if ((ret = INIT_OBJECT(scriptTableGlobalMeterReset, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript* s = (gxScript*)malloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        //Add executed script to script list.
        arr_push(&scriptTableGlobalMeterReset.scripts, s);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////
//Add script table object for disconnect control.
//Action 1 calls remote_disconnect #1 (close).
//Action 2 calls remote_connect #2(open).
///////////////////////////////////////////////////////////////////////
int addscriptTableDisconnectControl()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 10, 0, 106, 255 };
    if ((ret = INIT_OBJECT(scriptTableDisconnectControl, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript* s = (gxScript*)malloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        gxScriptAction* a = (gxScriptAction*)malloc(sizeof(gxScriptAction));

        a->type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        a->target = BASE(disconnectControl);
        a->index = 1;
        var_init(&a->parameter);
        //Action data is Int8 zero.
        GX_INT8(a->parameter) = 0;
        arr_push(&s->actions, a);
        //Add executed script to script list.
        arr_push(&scriptTableDisconnectControl.scripts, s);

        s = (gxScript*)malloc(sizeof(gxScript));
        s->id = 2;
        arr_init(&s->actions);
        a = (gxScriptAction*)malloc(sizeof(gxScriptAction));
        a->type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        a->target = BASE(disconnectControl);
        a->index = 2;
        var_init(&a->parameter);
        //Action data is Int8 zero.
        GX_INT8(a->parameter) = 0;
        arr_push(&s->actions, a);
        //Add executed script to script list.
        arr_push(&scriptTableDisconnectControl.scripts, s);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add script table object for test mode. In test mode meter is sending trace to the serial port.
///////////////////////////////////////////////////////////////////////
int addscriptTableActivateTestMode()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 10, 0, 101, 255 };
    if ((ret = INIT_OBJECT(scriptTableActivateTestMode, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript* s = (gxScript*)malloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        //Add executed script to script list.
        arr_push(&scriptTableActivateTestMode.scripts, s);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add script table object for Normal mode. In normal mode meter is NOT sending trace to the serial port.
///////////////////////////////////////////////////////////////////////
int addscriptTableActivateNormalMode()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 10, 0, 102, 255 };
    if ((ret = INIT_OBJECT(scriptTableActivateNormalMode, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        gxScript* s = (gxScript*)malloc(sizeof(gxScript));
        s->id = 1;
        arr_init(&s->actions);
        //Add executed script to script list.
        arr_push(&scriptTableActivateNormalMode.scripts, s);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject()
{
    int ret = 0;
    //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
    const unsigned char ln[6] = { 0, 0, 1, 0, 0, 255 };
    if ((ret = INIT_OBJECT(clock1, DLMS_OBJECT_TYPE_CLOCK, ln)) == 0)
    {
        //Set default values.
        time_init(&clock1.begin, -1, 3, -1, 2, 0, 0, 0, 0);
        clock1.begin.extraInfo = DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY;
        time_init(&clock1.end, -1, 10, -1, 3, 0, 0, 0, 0);
        clock1.end.extraInfo = DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY;
        //Meter is using UTC time zone.
        clock1.timeZone = 0;
        //Deviation is 60 minutes.
        clock1.deviation = 60;
        clock1.clockBase = DLMS_CLOCK_BASE_FREQUENCY_50;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example TCP/UDP setup object.
///////////////////////////////////////////////////////////////////////
int addTcpUdpSetup()
{
    //Add Tcp/Udp setup. Default Logical Name is 0.0.25.0.0.255.
    const unsigned char ln[6] = { 0,0,25,0,0,255 };
    INIT_OBJECT(udpSetup, DLMS_OBJECT_TYPE_TCP_UDP_SETUP, ln);
    udpSetup.maximumSimultaneousConnections = 1;
    udpSetup.maximumSegmentSize = 40;
    udpSetup.inactivityTimeout = 180;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addLoadProfileProfileGeneric(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 99, 1, 0, 255 };
    if ((ret = INIT_OBJECT(loadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        gxTarget* capture;
        //Set default values if load the first time.
        loadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add 2 columns.
        //Add clock obect.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&loadProfile.captureObjects, key_init(&clock1, capture));
        //Add active power.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&loadProfile.captureObjects, key_init(&activePowerL1, capture));
        ///////////////////////////////////////////////////////////////////
        //Update amount of capture objects.
        //Set clock to sort object.
        loadProfile.sortObject = BASE(clock1);
        loadProfile.sortObjectAttributeIndex = 2;
        loadProfile.profileEntries = getProfileGenericBufferMaxRowCount(settings, &loadProfile);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addEventLogProfileGeneric(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 1, 0, 99, 98, 0, 255 };
    if ((ret = INIT_OBJECT(eventLog, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        eventLog.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add 2 columns as default.
        gxTarget* capture;
        //Add clock obect.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&eventLog.captureObjects, key_init(&clock1, capture));

        //Add event code.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&eventLog.captureObjects, key_init(&eventCode, capture));
        //Set clock to sort object.
        eventLog.sortObject = BASE(clock1);
        eventLog.sortObjectAttributeIndex = 2;
        eventLog.profileEntries = getProfileGenericBufferMaxRowCount(settings, &eventLog);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Auto connect object.
///////////////////////////////////////////////////////////////////////
int addAutoConnect()
{
    gxByteBuffer* str;
    gxtime* start, * end;
    const unsigned char ln[6] = { 0,0,2,1,0,255 };
    INIT_OBJECT(autoConnect, DLMS_OBJECT_TYPE_AUTO_CONNECT, ln);
    autoConnect.mode = DLMS_AUTO_CONNECT_MODE_AUTO_DIALLING_ALLOWED_ANYTIME;
    autoConnect.repetitions = 10;
    autoConnect.repetitionDelay = 60;
    //Calling is allowed between 1am to 6am.
    start = (gxtime*)malloc(sizeof(gxtime));
    time_init(start, -1, -1, -1, 1, 0, 0, -1, -1);
    end = (gxtime*)malloc(sizeof(gxtime));
    time_init(end, -1, -1, -1, 6, 0, 0, -1, -1);
    arr_push(&autoConnect.callingWindow, key_init(start, end));
    str = (gxByteBuffer*)malloc(sizeof(gxByteBuffer));
    bb_init(str);
    bb_addString(str, "www.gurux.org");
    arr_push(&autoConnect.destinations, str);
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Activity Calendar object.
///////////////////////////////////////////////////////////////////////
int addActivityCalendar()
{
    gxDayProfile* dp;
    gxSeasonProfile* sp;
    gxWeekProfile* wp;
    gxDayProfileAction* act;

    const unsigned char ln[6] = { 0,0,13,0,0,255 };
    INIT_OBJECT(activityCalendar, DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR, ln);

    bb_addString(&activityCalendar.calendarNameActive, "Active");
    //Add season profile.
    sp = (gxSeasonProfile*)malloc(sizeof(gxSeasonProfile));
    bb_init(&sp->name);
    bb_addString(&sp->name, "Summer time");
    time_init(&sp->start, -1, 3, 31, -1, -1, -1, -1, -clock1.timeZone);
    bb_init(&sp->weekName);
    arr_push(&activityCalendar.seasonProfileActive, sp);
    //Add week profile.
    wp = (gxWeekProfile*)malloc(sizeof(gxWeekProfile));
    bb_init(&wp->name);
    bb_addString(&wp->name, "Monday");
    wp->monday = wp->tuesday = wp->wednesday = wp->thursday = wp->friday = wp->saturday = wp->sunday = 1;
    arr_push(&activityCalendar.weekProfileTableActive, wp);

    //Add day profile.
    dp = (gxDayProfile*)malloc(sizeof(gxDayProfile));
    arr_init(&dp->daySchedules);

    dp->dayId = 1;
    act = (gxDayProfileAction*)malloc(sizeof(gxDayProfileAction));
    time_init(&act->startTime, -1, -1, -1, 0, 0, 0, 0, 0x8000);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    act->script = BASE(tarifficationScriptTable);
#else
    memcpy(act->scriptLogicalName, tarifficationScriptTable.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS

    act->scriptSelector = 1;
    arr_push(&dp->daySchedules, act);
    arr_push(&activityCalendar.dayProfileTableActive, dp);
    bb_addString(&activityCalendar.calendarNamePassive, "Passive");

    sp = (gxSeasonProfile*)malloc(sizeof(gxSeasonProfile));
    bb_init(&sp->name);
    bb_addString(&sp->name, "Winter time");
    time_init(&sp->start, -1, 10, 30, -1, -1, -1, -1, 0x8000);
    bb_init(&sp->weekName);
    arr_push(&activityCalendar.seasonProfilePassive, sp);
    //Add week profile.
    wp = (gxWeekProfile*)malloc(sizeof(gxWeekProfile));
    bb_init(&wp->name);
    bb_addString(&wp->name, "Tuesday");
    wp->monday = wp->tuesday = wp->wednesday = wp->thursday = wp->friday = wp->saturday = wp->sunday = 1;
    arr_push(&activityCalendar.weekProfileTablePassive, wp);

    //Add day profile.
    dp = (gxDayProfile*)malloc(sizeof(gxDayProfile));
    arr_init(&dp->daySchedules);
    dp->dayId = 1;
    act = (gxDayProfileAction*)malloc(sizeof(gxDayProfileAction));
    time_init(&act->startTime, -1, -1, -1, 0, 0, 0, 0, 0x8000);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    act->script = BASE(tarifficationScriptTable);
#else
    memcpy(act->scriptLogicalName, tarifficationScriptTable.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    act->scriptSelector = 1;
    arr_push(&dp->daySchedules, act);
    arr_push(&activityCalendar.dayProfileTablePassive, dp);
    //Activate passive calendar is not called.
    time_init(&activityCalendar.time, -1, -1, -1, -1, -1, -1, -1, 0x8000);
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Optical Port Setup object.
///////////////////////////////////////////////////////////////////////
int addOpticalPortSetup()
{
    const unsigned char ln[6] = { 0,0,20,0,0,255 };
    INIT_OBJECT(localPortSetup, DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP, ln);
    localPortSetup.defaultMode = DLMS_OPTICAL_PROTOCOL_MODE_DEFAULT;
    localPortSetup.proposedBaudrate = DLMS_BAUD_RATE_9600;
    localPortSetup.defaultBaudrate = DLMS_BAUD_RATE_300;
    localPortSetup.responseTime = DLMS_LOCAL_PORT_RESPONSE_TIME_200_MS;
    bb_addString(&localPortSetup.deviceAddress, "Gurux");
    bb_addString(&localPortSetup.password1, "Gurux1");
    bb_addString(&localPortSetup.password2, "Gurux2");
    bb_addString(&localPortSetup.password5, "Gurux5");
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Demand Register object.
///////////////////////////////////////////////////////////////////////
int addDemandRegister()
{
    const unsigned char ln[6] = { 1,0,31,4,0,255 };
    INIT_OBJECT(demandRegister, DLMS_OBJECT_TYPE_DEMAND_REGISTER, ln);

    var_setUInt16(&demandRegister.currentAverageValue, 10);
    var_setUInt16(&demandRegister.lastAverageValue, 20);
    var_setUInt8(&demandRegister.status, 1);
    time_now(&demandRegister.startTimeCurrent, 1);
    time_now(&demandRegister.captureTime, 1);
    demandRegister.period = 10;
    demandRegister.numberOfPeriods = 1;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Register Monitor object.
///////////////////////////////////////////////////////////////////////
int addRegisterMonitor()
{
    gxActionSet* action;
    dlmsVARIANT* tmp;

    const unsigned char ln[6] = { 0,0,16,1,0,255 };
    INIT_OBJECT(registerMonitor, DLMS_OBJECT_TYPE_REGISTER_MONITOR, ln);

    //Add low value.
    tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
    var_init(tmp);
    var_setUInt32(tmp, 10000);
    va_push(&registerMonitor.thresholds, tmp);
    //Add high value.
    tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
    var_init(tmp);
    var_setUInt32(tmp, 30000);
    va_push(&registerMonitor.thresholds, tmp);
    //Add last values so script is not invoke multiple times.
    dlmsVARIANT empty;
    var_init(&empty);
    va_addValue(&registerMonitor.lastValues, &empty, 2);
    registerMonitor.monitoredValue.attributeIndex = 2;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    registerMonitor.monitoredValue.target = BASE(activePowerL1);
#else
    registerMonitor.monitoredValue.objectType = activePowerL1.base.objectType;
    memcpy(registerMonitor.monitoredValue.logicalName, activePowerL1.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS

    //////////////////////
    //Add low action. Turn LED OFF.
    action = (gxActionSet*)malloc(sizeof(gxActionSet));
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionDown.script = &scriptTableDisconnectControl;
#else
    memcpy(action->actionDown.logicalName, scriptTableDisconnectControl.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS

    action->actionDown.scriptSelector = 1;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionUp.script = NULL;
#else
    memset(action->actionUp.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    action->actionUp.scriptSelector = 2;
    arr_push(&registerMonitor.actions, action);
    //////////////////////
    //Add high action. Turn LED ON.
    action = (gxActionSet*)malloc(sizeof(gxActionSet));
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionDown.script = NULL;
#else
    memset(action->actionDown.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    action->actionDown.scriptSelector = 1;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionUp.script = &scriptTableDisconnectControl;
#else
    memset(action->actionUp.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    action->actionUp.scriptSelector = 1;
    arr_push(&registerMonitor.actions, action);
    return 0;
}


///////////////////////////////////////////////////////////////////////
//Add action schedule object for disconnect control to close the led.
///////////////////////////////////////////////////////////////////////
int addActionScheduleDisconnectClose()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 15, 0, 1, 255 };
    if ((ret = INIT_OBJECT(actionScheduleDisconnectClose, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln)) == 0)
    {
        actionScheduleDisconnectClose.executedScript = &scriptTableDisconnectControl;
        actionScheduleDisconnectClose.executedScriptSelector = 1;
        actionScheduleDisconnectClose.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add action schedule object for disconnect control to open the led.
///////////////////////////////////////////////////////////////////////
int addActionScheduleDisconnectOpen()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 15, 0, 3, 255 };
    //Action schedule execution times.
    if ((ret = INIT_OBJECT(actionScheduleDisconnectOpen, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln)) == 0)
    {
        actionScheduleDisconnectOpen.executedScript = &scriptTableDisconnectControl;
        actionScheduleDisconnectOpen.executedScriptSelector = 2;
        actionScheduleDisconnectOpen.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Disconnect control object.
///////////////////////////////////////////////////////////////////////
int addDisconnectControl()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 3, 10, 255 };
    if ((ret = INIT_OBJECT(disconnectControl, DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln)) == 0)
    {
    }
    return ret;
}


///////////////////////////////////////////////////////////////////////
//Add image transfer object.
///////////////////////////////////////////////////////////////////////
int addImageTransfer()
{
    unsigned char ln[6] = { 0,0,44,0,0,255 };
    INIT_OBJECT(imageTransfer, DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln);
    imageTransfer.imageBlockSize = 100;
    imageTransfer.imageFirstNotTransferredBlockNumber = 0;
    //Enable image transfer.
    imageTransfer.imageTransferEnabled = 1;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add IEC HDLC Setup object.
///////////////////////////////////////////////////////////////////////
int addIecHdlcSetup(dlmsServerSettings* settings)
{
    int ret = 0;
    unsigned char ln[6] = { 0, 0, 22, 0, 0, 255 };
    if ((ret = INIT_OBJECT(hdlc, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln)) == 0)
    {
        hdlc.communicationSpeed = DLMS_BAUD_RATE_9600;
        hdlc.windowSizeReceive = hdlc.windowSizeTransmit = 1;
        hdlc.maximumInfoLengthTransmit = hdlc.maximumInfoLengthReceive = 128;
        hdlc.inactivityTimeout = 120;
        hdlc.deviceAddress = 0x10;
    }
    settings->hdlc = &hdlc;
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add compact data object.
///////////////////////////////////////////////////////////////////////
int addCompactData(
    dlmsSettings* settings,
    objectArray* objects)
{
    gxTarget* capture;
    gxKey* k;
    unsigned char ln[6] = { 0,0,66,0,1,255 };
    INIT_OBJECT(compactData, DLMS_OBJECT_TYPE_COMPACT_DATA, ln);
    compactData.templateId = 66;
#ifdef DLMS_ITALIAN_STANDARD
    //Some Italy meters require that there is a array count in some compact buffer.
    //This is against compact data structure defined in DLMS standard.
    compactData.appendAA = 1;
#endif //DLMS_ITALIAN_STANDARD
    //Buffer is captured when invoke is called.
    compactData.captureMethod = DLMS_CAPTURE_METHOD_INVOKE;
    ////////////////////////////////////////
    //Add capture objects.
    //Add compact data template ID as first object.
    capture = (gxTarget*)malloc(sizeof(gxTarget));
    capture->attributeIndex = 4;
    capture->dataIndex = 0;
    k = key_init(&compactData, capture);
    arr_push(&compactData.captureObjects, k);

    capture = (gxTarget*)malloc(sizeof(gxTarget));
    capture->attributeIndex = 4;
    capture->dataIndex = 0;
    k = key_init(&actionScheduleDisconnectOpen, capture);
    arr_push(&compactData.captureObjects, k);
    return compactData_updateTemplateDescription(settings, &compactData);
}

///////////////////////////////////////////////////////////////////////
//Add SAP Assignment object.
///////////////////////////////////////////////////////////////////////
int addSapAssignment()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 41, 0, 0, 255 };
    if ((ret = INIT_OBJECT(sapAssignment, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, ln)) == 0)
    {
        char tmp[17];
        gxSapItem* it = (gxSapItem*)malloc(sizeof(gxSapItem));
        bb_init(&it->name);
        ret = sprintf(tmp, "%s%.13lu", FLAG_ID, SERIAL_NUMBER);
        bb_addString(&it->name, tmp);
        it->id = 1;
        ret = arr_push(&sapAssignment.sapAssignmentList, it);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Logical Device Name. 123456 is meter serial number.
///////////////////////////////////////////////////////////////////////
// COSEM Logical Device Name is defined as an octet-string of 16 octets.
// The first three octets uniquely identify the manufacturer of the device and it corresponds
// to the manufacturer's identification in IEC 62056-21.
// The following 13 octets are assigned by the manufacturer.
//The manufacturer is responsible for guaranteeing the uniqueness of these octets.
int addLogicalDeviceName()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 42, 0, 0, 255 };
    if ((ret = INIT_OBJECT(ldn, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        char tmp[17];
        sprintf(tmp, "%s%.13lu", FLAG_ID, SERIAL_NUMBER);
        var_addBytes(&ldn.value, (unsigned char*)tmp, 16);
    }
    return ret;
}

//Add event code object.
int addEventCode()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 96, 11, 0, 255 };
    if ((ret = INIT_OBJECT(eventCode, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(eventCode.value) = 0;
    }
    return ret;
}

//Add unix time object.
int addUnixTime()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 1, 1, 0, 255 };
    if ((ret = INIT_OBJECT(unixTime, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        //Set initial value.
        GX_UINT32(unixTime.value) = 0;
    }
    return ret;
}

//Add invocation counter object.
int addInvocationCounter()
{
    int ret;
    const unsigned char ln[6] = { 0,0,43,1,0,255 };
    if ((ret = INIT_OBJECT(invocationCounter, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        //Initial invocation counter value.
        GX_UINT32_BYREF(invocationCounter.value, securitySetupHighGMac.minimumInvocationCounter);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Auto Answer object.
///////////////////////////////////////////////////////////////////////
int addAutoAnswer()
{
    gxtime* start, * end;
    const unsigned char ln[6] = { 0,0,2,2,0,255 };
    INIT_OBJECT(autoAnswer, DLMS_OBJECT_TYPE_AUTO_ANSWER, ln);

    start = (gxtime*)malloc(sizeof(gxtime));
    time_init(start, -1, -1, -1, 6, -1, -1, -1, -1);
    end = (gxtime*)malloc(sizeof(gxtime));
    time_init(end, -1, -1, -1, 8, -1, -1, -1, -1);

    autoAnswer.mode = DLMS_AUTO_CONNECT_MODE_EMAIL_SENDING;
    arr_push(&autoAnswer.listeningWindow, key_init(start, end));
    autoAnswer.status = DLMS_AUTO_ANSWER_STATUS_INACTIVE;
    autoAnswer.numberOfCalls = 0;
    autoAnswer.numberOfRingsInListeningWindow = 1;
    autoAnswer.numberOfRingsOutListeningWindow = 2;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Modem Configuration object.
///////////////////////////////////////////////////////////////////////
int addModemConfiguration()
{
    gxModemInitialisation* init;
    const unsigned char ln[6] = { 0,0,2,0,0,255 };
    INIT_OBJECT(modemConfiguration, DLMS_OBJECT_TYPE_MODEM_CONFIGURATION, ln);

    modemConfiguration.communicationSpeed = DLMS_BAUD_RATE_38400;
    init = (gxModemInitialisation*)malloc(sizeof(gxModemInitialisation));
    bb_init(&init->request);
    bb_init(&init->response);
    bb_addString(&init->request, "AT");
    bb_addString(&init->response, "OK");
    init->delay = 0;
    arr_push(&modemConfiguration.initialisationStrings, init);
    return 0;
}


///////////////////////////////////////////////////////////////////////
//Add Mac Address Setup object.
///////////////////////////////////////////////////////////////////////
int addMacAddressSetup()
{
    const unsigned char ln[6] = { 0,0,25,2,0,255 };
    unsigned char address[] = { 00, 11, 22, 33, 44, 55, 66, 77 };
    INIT_OBJECT(macAddressSetup, DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, ln);
    bb_set(&macAddressSetup.macAddress, address, sizeof(address));
    return 0;
}

unsigned long getIpAddress()
{
    int ret = -1;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    struct hostent* phe;
    char ac[80];
    if ((ret = gethostname(ac, sizeof(ac))) != -1)
    {
        phe = gethostbyname(ac);
        if (phe == 0)
        {
            ret = 0;
        }
        else
        {
            struct in_addr* addr = (struct in_addr*)phe->h_addr_list[0];
#if defined(_WIN32) || defined(_WIN64)//If Windows
            return addr->S_un.S_addr;
#else //or Linux
            return addr->s_addr;
#endif
        }
    }
#else
    //If no OS get IP.
#endif
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add IP4 Setup object.
///////////////////////////////////////////////////////////////////////
int addIP4Setup()
{
    int ret;
    dlmsVARIANT* it;
    const unsigned char ln[6] = { 0,0,25,1,0,255 };
    if ((ret = INIT_OBJECT(ip4Setup, DLMS_OBJECT_TYPE_IP4_SETUP, ln)) == 0)
    {
        ip4Setup.ipAddress = getIpAddress();
        ip4Setup.dataLinkLayer = BASE(pppSetup);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT32(*it) = 0;
        va_push(&ip4Setup.multicastIPAddress, it);
        ip4Setup.subnetMask = 0xFFFFFFFF;
        ip4Setup.gatewayIPAddress = 0x0A000001;
        ip4Setup.primaryDNSAddress = 0x0A0B0C0D;
        ip4Setup.secondaryDNSAddress = 0x0C0D0E0F;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add PPP Setup object.
///////////////////////////////////////////////////////////////////////
int addPppSetup()
{
    int ret;
    const unsigned char ln[6] = { 0,0,25,3,0,255 };
    if ((ret = INIT_OBJECT(pppSetup, DLMS_OBJECT_TYPE_PPP_SETUP, ln)) == 0)
    {
        pppSetup.phy = BASE(gprsSetup);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add GPRS Setup object.
///////////////////////////////////////////////////////////////////////
int addGprsSetup()
{
    int ret;
    static unsigned char APN[15];
    const unsigned char ln[6] = { 0,0,25,4,0,255 };
    if ((ret = INIT_OBJECT(gprsSetup, DLMS_OBJECT_TYPE_GPRS_SETUP, ln)) == 0)
    {
        BB_ATTACH(gprsSetup.apn, APN, 0);
        ret = bb_addString(&gprsSetup.apn, "vpn.gurux.fi");
    }
    return ret;
}
///////////////////////////////////////////////////////////////////////
//Add push setup object. (On Connectivity)
///////////////////////////////////////////////////////////////////////
int addPushSetup()
{
    static unsigned char DESTINATION[20] = { 0 };

    const char dest[] = "127.0.0.1:7000";
    gxTarget* co;
    const unsigned char ln[6] = { 0,0,25,9,0,255 };
    INIT_OBJECT(pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln);
    bb_addString(&pushSetup.destination, "127.0.0.1:7000");
    // Add push object itself. This is needed to tell structure of data to
    // the Push listener.
    co = (gxTarget*)malloc(sizeof(gxTarget));
    co->attributeIndex = 2;
    co->dataIndex = 0;
    arr_push(&pushSetup.pushObjectList, key_init(&pushSetup, co));
    // Add logical device name.
    co = (gxTarget*)malloc(sizeof(gxTarget));
    co->attributeIndex = 2;
    co->dataIndex = 0;
    arr_push(&pushSetup.pushObjectList, key_init(&ldn, co));
    // Add 0.0.25.1.0.255 Ch. 0 IPv4 setup IP address.
    co = (gxTarget*)malloc(sizeof(gxTarget));
    co->attributeIndex = 3;
    co->dataIndex = 0;
    arr_push(&pushSetup.pushObjectList, key_init(&ip4Setup, co));
    pushSetup.randomisationStartInterval = 0;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Load security settings from the EEPROM.
/////////////////////////////////////////////////////////////////////////////
int loadSecurity(dlmsSettings* settings)
{
    const char* fileName = "security.raw";
    int ret = 0;
    //Update keys.
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, fileName, "rb");
#else
    FILE* f = fopen(fileName, "rb");
#endif
    if (f != NULL)
    {
        //Check that file is not empty.
        fseek(f, 0L, SEEK_END);
        long size = ftell(f);
        if (size != 0)
        {
            fseek(f, 0L, SEEK_SET);
            gxByteBuffer bb;
            bb_init(&bb);
            bb_capacity(&bb, size);
            bb.size += fread(bb.data, 1, size, f);
            fclose(f);
            if ((ret = bb_clear(&settings->cipher.blockCipherKey)) != 0 ||
                (ret = bb_clear(&settings->cipher.authenticationKey)) != 0 ||
                (ret = bb_clear(&settings->kek)) != 0 ||
                (ret = bb_set2(&settings->cipher.blockCipherKey, &bb, 0, 16)) != 0 ||
                (ret = bb_set2(&settings->cipher.authenticationKey, &bb, bb.position, 16)) != 0 ||
                (ret = bb_set2(&settings->kek, &bb, bb.position, 16)) != 0 ||
                //load last server IC.
                (ret = bb_getUInt32(&bb, &settings->cipher.invocationCounter)) != 0 ||
                //load last client IC.
                (ret = bb_getUInt32(&bb, &securitySetupHighGMac.minimumInvocationCounter)) != 0)
            {
            }
            bb_clear(&bb);
            return ret;
        }
    }
    return saveSecurity(settings);
}


/////////////////////////////////////////////////////////////////////////////
// Load data from the EEPROM.
// Returns serialization version or zero if data is not saved.
/////////////////////////////////////////////////////////////////////////////
int loadSettings(dlmsSettings* settings)
{
    const char* fileName = "settings.raw";
    int ret = 0;
    //Update keys.
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, fileName, "rb");
#else
    FILE* f = fopen(fileName, "rb");
#endif
    if (f != NULL)
    {
        //Check that file is not empty.
        fseek(f, 0L, SEEK_END);
        long size = ftell(f);
        if (size != 0)
        {
            fseek(f, 0L, SEEK_SET);
            gxByteBuffer bb;
            bb_init(&bb);
            bb_capacity(&bb, size);
            bb.size += fread(bb.data, 1, size, f);
            fclose(f);
            gxSerializerSettings serializerSettings;
            serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
            serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
            ret = ser_loadObjects(settings, &serializerSettings, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]), &bb);
            bb_clear(&bb);
            return ret;
        }
        fclose(f);
    }
    return saveSettings();
}

///////////////////////////////////////////////////////////////////////
//Add Firmware version.
///////////////////////////////////////////////////////////////////////
int AddFirmwareversion()
{
    int ret;
    const unsigned char ln[6] = { 1,0,0,2,0,255 };
    if ((ret = INIT_OBJECT(fw, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        const char FW[] = "Gurux FW 1.0.1";
        var_setString(&fw.value, FW, (unsigned short)strlen(FW));
    }
    return ret;
}

int svr_InitObjects(
    dlmsServerSettings* settings)
{
    char buff[17];
    int ret;
    OA_ATTACH(settings->base.objects, ALL_OBJECTS);
    ///////////////////////////////////////////////////////////////////////
   //Add Logical Device Name. 123456 is meter serial number.
   ///////////////////////////////////////////////////////////////////////
   // COSEM Logical Device Name is defined as an octet-string of 16 octets.
   // The first three octets uniquely identify the manufacturer of the device and it corresponds
   // to the manufacturer's identification in IEC 62056-21.
   // The following 13 octets are assigned by the manufacturer.
   //The manufacturer is responsible for guaranteeing the uniqueness of these octets.
    sprintf(buff, "GRX%.13lu", SERIAL_NUMBER);
    {
        const unsigned char ln[6] = { 0,0,42,0,0,255 };
        INIT_OBJECT(ldn, DLMS_OBJECT_TYPE_DATA, ln);
        var_addBytes(&ldn.value, (unsigned char*)buff, 16);
    }
    //Electricity ID 1
    {
        const unsigned char ln[6] = { 1,1,0,0,0,255 };
        INIT_OBJECT(id1, DLMS_OBJECT_TYPE_DATA, ln);
        var_setString(&id1.value, buff, 16);
    }

    //Electricity ID 2.
    {
        const unsigned char ln[6] = { 1,1,0,0,1,255 };
        INIT_OBJECT(id2, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&id2.value, SERIAL_NUMBER);
    }

    if ((ret = AddFirmwareversion()) != 0 ||
        (ret = addLogicalDeviceName()) != 0 ||
        (ret = addSapAssignment()) != 0 ||
        (ret = addEventCode()) != 0 ||
        (ret = addUnixTime()) != 0 ||
        (ret = addInvocationCounter()) != 0 ||
        (ret = addClockObject()) != 0 ||
        (ret = addRegisterObject()) != 0 ||
        (ret = addAssociationNone()) != 0 ||
        (ret = addAssociationLow()) != 0 ||
        (ret = addAssociationHigh()) != 0 ||
        (ret = addAssociationHighGMac()) != 0 ||
        (ret = addSecuritySetupHigh()) != 0 ||
        (ret = addSecuritySetupHighGMac()) != 0 ||
        (ret = addPushSetup()) != 0 ||
        (ret = addscriptTableGlobalMeterReset()) != 0 ||
        (ret = addscriptTableDisconnectControl()) != 0 ||
        (ret = addscriptTableActivateTestMode()) != 0 ||
        (ret = addscriptTableActivateNormalMode()) != 0 ||
        (ret = addTarifficationScriptTable()) != 0 ||
        (ret = addRegisterActivation(settings)) != 0 ||
        (ret = addLoadProfileProfileGeneric(&settings->base)) != 0 ||
        (ret = addEventLogProfileGeneric(&settings->base)) != 0 ||
        (ret = addActionScheduleDisconnectOpen()) != 0 ||
        (ret = addActionScheduleDisconnectClose()) != 0 ||
        (ret = addDisconnectControl()) != 0 ||
        (ret = addIecHdlcSetup(settings)) != 0 ||
        (ret = addTcpUdpSetup()) != 0 ||
        (ret = addAutoConnect()) != 0 ||
        (ret = addActivityCalendar()) != 0 ||
        (ret = addOpticalPortSetup()) != 0 ||
        (ret = addDemandRegister()) != 0 ||
        (ret = addRegisterMonitor()) != 0 ||
        (ret = addAutoAnswer()) != 0 ||
        (ret = addModemConfiguration()) != 0 ||
        (ret = addMacAddressSetup()) != 0 ||
        (ret = addIP4Setup()) != 0 ||
        (ret = addPppSetup()) != 0 ||
        (ret = addGprsSetup()) != 0 ||
        (ret = addImageTransfer()) != 0 ||
        (ret = addCompactData(&settings->base, &settings->base.objects)) != 0 ||
        (ret = oa_verify(&settings->base.objects)) != 0 ||
        (ret = svr_initialize(settings)) != 0)
    {
        GXTRACE_INT(("Failed to start the meter!"), ret);
        executeTime = 0;
        return ret;
    }
    if ((ret = loadSettings(&settings->base)) != 0)
    {
        GXTRACE_INT(("Failed to load settings!"), ret);
        executeTime = 0;
        return ret;
    }
    if ((ret = loadSecurity(&settings->base)) != 0)
    {
        GXTRACE_INT(("Failed to load security settings!"), ret);
        executeTime = 0;
        return ret;
    }
    updateState(&settings->base, GURUX_EVENT_CODES_POWER_UP);
    GXTRACE(("Meter started."), NULL);
    return 0;
}

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
/**
* Start server.
*/
int svr_start(
    connection* con,
    unsigned short port)
{
    int ret;
    con->settings.pushClientAddress = 64;
    if ((ret = svr_listen(con, port)) != 0)
    {
        return ret;
    }
    if (con->settings.base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        con->settings.hdlc = &hdlc;
    }
    else if (con->settings.base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        con->settings.wrapper = &udpSetup;
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    ///////////////////////////////////////////////////////////////////////
    //Server must initialize after all objects are added.
    ret = svr_initialize(&con->settings);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    bb_addString(&con->settings.base.kek, "1111111111111111");
    return DLMS_ERROR_CODE_OK;
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)


int svr_findObject(
    dlmsSettings* settings,
    DLMS_OBJECT_TYPE objectType,
    int sn,
    unsigned char* ln,
    gxValueEventArg* e)
{
    GXTRACE_LN(("findObject"), objectType, ln);
    if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        uint16_t pos;
        objectArray objects;
        gxObject* tmp[6];
        oa_attach(&objects, tmp, sizeof(tmp) / sizeof(tmp[0]));
        objects.size = 0;
        if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, &objects) == 0)
        {
            gxAssociationLogicalName* a;
            for (pos = 0; pos != objects.size; ++pos)
            {
                if (oa_getByIndex(&objects, pos, (gxObject**)&a) == 0)
                {
                    if (a->clientSAP == settings->clientAddress &&
                        a->authenticationMechanismName.mechanismId == settings->authentication &&
                        (memcmp(ln, DEFAULT_ASSOCIATION, sizeof(DEFAULT_ASSOCIATION)) == 0 || memcmp(a->base.logicalName, ln, 6) == 0))
                    {
                        e->target = (gxObject*)a;
                        break;
                    }
                }
            }
        }
    }
    if (e->target == NULL)
    {
        GXTRACE_LN(("Unknown object"), objectType, ln);
    }
    return 0;
}


/**
  Find restricting object.
*/
int getRestrictingObject(dlmsSettings* settings,
    gxValueEventArg* e,
    gxObject** obj,
    short* index)
{
    int ret;
    dlmsVARIANT* it, * it2;
    if ((ret = va_getByIndex(e->parameters.Arr, 0, &it)) != 0)
    {
        return ret;
    }
    DLMS_OBJECT_TYPE ot;
    unsigned char* ln;
    if ((ret = va_getByIndex(it->Arr, 0, &it2)) != 0)
    {
        return ret;
    }
    ot = it2->iVal;
    if ((ret = va_getByIndex(it->Arr, 1, &it2)) != 0)
    {
        return ret;
    }
    ln = it2->byteArr->data;
    if ((ret = va_getByIndex(it->Arr, 3, &it2)) != 0)
    {
        return ret;
    }
    *index = it2->iVal;
    if ((ret = oa_findByLN(&settings->objects, ot, ln, obj)) != 0)
    {
        return ret;
    }
    return ret;
}

/**
  Find start index and row count using start and end date time.

  settings: DLMS settings.
             Start time.
  type: Profile Generic type.
  e: Parameters
*/
int getProfileGenericDataByRangeFromRingBuffer(
    dlmsSettings* settings,
    const char* fileName,
    gxValueEventArg* e)
{
    //Get all data if there are no sort object.
    uint32_t s = 0;
    uint32_t l = 0xFFFFFFFF;
    dlmsVARIANT tmp;
    int ret = 0;
    dlmsVARIANT* it;
    uint32_t pos;
    uint32_t last = 0;
    gxObject* obj = NULL;
    short index;
    if ((ret = getRestrictingObject(settings, e, &obj, &index)) != 0)
    {
        return ret;
    }
    var_init(&tmp);
    //Check sort object
    if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) != 0)
    {
        return ret;
    }
    if (it->vt == DLMS_DATA_TYPE_UINT32)
    {
        s = it->ulVal;
    }
    else
    {
        if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp)) != 0)
        {
            var_clear(&tmp);
            return ret;
        }
        //Start time.
        s = time_toUnixTime2(tmp.dateTime);
        var_clear(&tmp);
    }
    if ((ret = va_getByIndex(e->parameters.Arr, 2, &it)) != 0)
    {
        return ret;
    }
    if (it->vt == DLMS_DATA_TYPE_UINT32)
    {
        l = it->ulVal;
    }
    else
    {
        if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp)) != 0)
        {
            var_clear(&tmp);
            return ret;
        }
        l = time_toUnixTime2(tmp.dateTime);
        var_clear(&tmp);
    }

    uint32_t t;
    gxProfileGeneric* pg = (gxProfileGeneric*)e->target;
    if (pg->entriesInUse != 0)
    {
#if _MSC_VER > 1400
        FILE* f = NULL;
        fopen_s(&f, fileName, "rb");
#else
        FILE* f = fopen(fileName, "rb");
#endif
        uint16_t rowSize = 0;
        uint8_t columnSizes[10];
        DLMS_DATA_TYPE dataTypes[10];
        if (f != NULL)
        {
            getProfileGenericBufferColumnSizes(settings, pg, dataTypes, columnSizes, &rowSize);
            //Skip current index and total amount of the entries.
            fseek(f, 4, SEEK_SET);
            for (pos = 0; pos != pg->entriesInUse; ++pos)
            {
                //Load time from EEPROM.
                fread(&t, sizeof(uint32_t), 1, f);
                //seek to begin of next row.
                fseek(f, rowSize - sizeof(uint32_t), SEEK_CUR);
                //If value is inside of start and end time.
                if (t >= s && t <= l)
                {
                    if (last == 0)
                    {
                        //Save end position if we have only one row.
                        e->transactionEndIndex = e->transactionStartIndex = 1 + pos;
                    }
                    else
                    {
                        if (last <= t)
                        {
                            e->transactionEndIndex = pos + 1;
                        }
                        else
                        {
                            //Index is one based, not zero.
                            if (e->transactionEndIndex == 0)
                            {
                                ++e->transactionEndIndex;
                            }
                            e->transactionEndIndex += pg->entriesInUse - 1;
                            e->transactionStartIndex = pos;
                            break;
                        }
                    }
                    last = t;
                }
            }
            fclose(f);
        }
    }
    return ret;
}


int readProfileGeneric(
    dlmsSettings* settings,
    gxProfileGeneric* pg,
    gxValueEventArg* e)
{
    unsigned char first = e->transactionEndIndex == 0;
    int ret = 0;
    gxArray captureObjects;
    arr_init(&captureObjects);
    char fileName[20];
    getProfileGenericFileName(pg, fileName);
    if (ret == DLMS_ERROR_CODE_OK)
    {
        e->byteArray = 1;
        e->handled = 1;
        // If reading first time.
        if (first)
        {
            //Read all.
            if (e->selector == 0)
            {
                e->transactionStartIndex = 1;
                e->transactionEndIndex = pg->entriesInUse;
            }
            else if (e->selector == 1)
            {
                //Read by entry. Find start and end index from the ring buffer.
                if ((ret = getProfileGenericDataByRangeFromRingBuffer(settings, fileName, e)) != 0 ||
                    (ret = cosem_getColumns(&pg->captureObjects, e->selector, &e->parameters, &captureObjects)) != 0)
                {
                    e->transactionStartIndex = e->transactionEndIndex = 0;
                }
            }
            else if (e->selector == 2)
            {
                dlmsVARIANT* it;
                if ((ret = va_getByIndex(e->parameters.Arr, 0, &it)) == 0)
                {
                    e->transactionStartIndex = var_toInteger(it);
                    if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) == 0)
                    {
                        e->transactionEndIndex = var_toInteger(it);
                    }
                }
                if (ret != 0)
                {
                    e->transactionStartIndex = e->transactionEndIndex = 0;
                }
                else
                {
                    //If start index is too high.
                    if (e->transactionStartIndex > pg->entriesInUse)
                    {
                        e->transactionStartIndex = e->transactionEndIndex = 0;
                    }
                    //If end index is too high.
                    if (e->transactionEndIndex > pg->entriesInUse)
                    {
                        e->transactionEndIndex = pg->entriesInUse;
                    }
                }
            }
        }
        bb_clear(e->value.byteArr);
        arr_clear(&captureObjects);
        if (ret == 0 && first)
        {
            if (e->transactionEndIndex == 0)
            {
                ret = cosem_setArray(e->value.byteArr, 0);
            }
            else
            {
                ret = cosem_setArray(e->value.byteArr, (uint16_t)(e->transactionEndIndex - e->transactionStartIndex + 1));
            }
        }
        if (ret == 0 && e->transactionEndIndex != 0)
        {
            //Loop items.
            uint32_t pos;
            gxtime tm;
            uint16_t pduSize;
            FILE* f = NULL;
#if _MSC_VER > 1400
            if (fopen_s(&f, fileName, "rb") != 0)
            {
                printf("Failed to open %s.\r\n", fileName);
                return -1;
            }
#else
            if ((f = fopen(fileName, "rb")) != 0)
            {
                printf("Failed to open %s.\r\n", fileName);
                return -1;
            }
#endif
            uint16_t dataSize = 0;
            uint8_t columnSizes[10];
            DLMS_DATA_TYPE dataTypes[10];
            if (f != NULL)
            {
                getProfileGenericBufferColumnSizes(settings, pg, dataTypes, columnSizes, &dataSize);
            }
            //Append data.
            if (ret == 0 && dataSize != 0)
            {
                //Skip current index and total amount of the entries (+4 bytes).
                if (fseek(f, 4 + ((e->transactionStartIndex - 1) * dataSize), SEEK_SET) != 0)
                {
                    printf("Failed to seek %s.\r\n", fileName);
                    return -1;
                }
                for (pos = e->transactionStartIndex - 1; pos != e->transactionEndIndex; ++pos)
                {
                    pduSize = (uint16_t)e->value.byteArr->size;
                    if ((ret = cosem_setStructure(e->value.byteArr, pg->captureObjects.size)) != 0)
                    {
                        break;
                    }
                    uint8_t colIndex;
                    gxKey* it;
                    //Loop capture columns and get values.
                    for (colIndex = 0; colIndex != pg->captureObjects.size; ++colIndex)
                    {
                        if ((ret = arr_getByIndex(&pg->captureObjects, colIndex, (void**)&it)) == 0)
                        {
                            //Date time is saved in EPOCH to save space.
                            if ((((gxObject*)it->key)->objectType == DLMS_OBJECT_TYPE_CLOCK || (gxObject*)it->key == BASE(unixTime))
                                && ((gxTarget*)it->value)->attributeIndex == 2)
                            {
                                uint32_t time;
                                fread(&time, 4, 1, f);
                                time_initUnix(&tm, time);
                                //Convert to meter time if UNIX time is not used.
                                if (((gxObject*)it->key) != BASE(unixTime))
                                {
                                    clock_utcToMeterTime(&clock1, &tm);
                                }
                                if ((ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &tm)) != 0)
                                {
                                    //Error is handled later.
                                }
                            }
                            else
                            {
                                //Append data type.
                                e->value.byteArr->data[e->value.byteArr->size] = dataTypes[colIndex];
                                ++e->value.byteArr->size;
                                //Read data.
                                fread(&e->value.byteArr->data[e->value.byteArr->size], columnSizes[colIndex], 1, f);
                                e->value.byteArr->size += columnSizes[colIndex];
                            }
                        }
                        if (ret != 0)
                        {
                            //Don't set error if PDU is full.
                            if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
                            {
                                --e->transactionStartIndex;
                                e->value.byteArr->size = pduSize;
                                ret = 0;
                            }
                            else
                            {
                                break;
                            }
                            break;
                        }
                    }
                    ++e->transactionStartIndex;
                }
                fclose(f);
            }
            else
            {
                printf("Failed to open %s.\r\n", fileName);
                return -1;
            }
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preRead(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    DLMS_OBJECT_TYPE type;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_preRead: "), e->target->objectType, e->target->logicalName);
        //Let framework handle Logical Name read.
        if (e->index == 1)
        {
            continue;
        }

        //Get target type.
        type = (DLMS_OBJECT_TYPE)e->target->objectType;
        //Let Framework will handle Association objects and profile generic automatically.
        if (type == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME ||
            type == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
        {
            continue;
        }
        //Update value by one every time when user reads register.
        if (e->target == BASE(activePowerL1) && e->index == 2)
        {
            readActivePowerValue();
        }
        //Get time if user want to read date and time.
        if (e->target == BASE(clock1) && e->index == 2)
        {
            gxtime dt;
            time_now(&dt, 1);
            if (e->value.byteArr == NULL)
            {
                e->value.byteArr = (gxByteBuffer*)malloc(sizeof(gxByteBuffer));
                bb_init(e->value.byteArr);
            }
            e->error = cosem_setDateTimeAsOctetString(e->value.byteArr, &dt);
            e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
            e->handled = 1;
        }
        else if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC && e->index == 2)
        {
            e->error = (DLMS_ERROR_CODE)readProfileGeneric(settings, (gxProfileGeneric*)e->target, e);
        }
        //Update Unix time.
        if (e->target == BASE(unixTime) && e->index == 2)
        {
            gxtime dt;
            time_now(&dt, 0);
            e->value.ulVal = time_toUnixTime2(&dt);
            e->value.vt = DLMS_DATA_TYPE_UINT32;
            e->handled = 1;
        }
    }
}

#ifndef GX_DLMS_MICROCONTROLLER
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
int printValues(variantArray* values)
{
    int pos;
    dlmsVARIANT* it;
    gxByteBuffer bb;
    bb_init(&bb);
    for (pos = 0; pos != values->size; ++pos)
    {
        if (va_getByIndex(values, pos, &it) != 0 ||
            var_toString(it, &bb) != 0)
        {
            return DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        char* tmp = bb_toString(&bb);
        printf("Writing %s\r\n", tmp);
        free(tmp);
        bb_clear(&bb);
    }
    return 0;
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#endif //GX_DLMS_MICROCONTROLLER

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preWrite(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    char str[25];
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        if (e->target == BASE(clock1) && e->index == 2)
        {
            updateState(settings, GURUX_EVENT_CODES_TIME_CHANGE);
        }
        //Loop buffer elements in write.
        if (e->target == &compactData.base && e->index == 2)
        {
#ifndef GX_DLMS_MICROCONTROLLER
            variantArray values;
            va_init(&values);
            if ((compactData_getValues(settings, &compactData.templateDescription, e->value.byteArr, &values)) != 0 ||
                printValues(&values) != 0)
            {
                e->error = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                break;
            }
            va_clear(&values);
#endif //GX_DLMS_MICROCONTROLLER
            break;
        }
        hlp_getLogicalNameToString(e->target->logicalName, str);
        printf("Writing %s\r\n", str);
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
}

int sendPush(dlmsSettings* settings, gxPushSetup* push);

void handleProfileGenericActions(
    dlmsSettings* settings,
    gxValueEventArg* it)
{
    if (it->index == 1)
    {
        char fileName[30];
        getProfileGenericFileName((gxProfileGeneric*)it->target, fileName);
        // Profile generic clear is called. Clear data.
        ((gxProfileGeneric*)it->target)->entriesInUse = 0;
        FILE* f = NULL;
#if _MSC_VER > 1400
        fopen_s(&f, fileName, "r+b");
#else
        f = fopen(fileName, "r+b");
#endif
        if (f != NULL)
        {
            gxByteBuffer pdu;
            bb_init(&pdu);
            //Current index in ring buffer.
            bb_setUInt16(&pdu, 0);
            bb_setUInt16(&pdu, 0);
            //Update values to the EEPROM.
            fwrite(pdu.data, 1, 4, f);
            fclose(f);
        }
    }
    else if (it->index == 2)
    {
        //Increase power value before each load profile read to increase the value.
        //This is needed for demo purpose only.
        if (it->target == BASE(loadProfile))
        {
            readActivePowerValue();
        }
        captureProfileGeneric(settings, ((gxProfileGeneric*)it->target));
    }
    saveSettings();
}


//Allocate space for image tranfer.
void allocateImageTransfer(const char* fileName, uint32_t size)
{
    uint32_t pos;
    FILE* f = NULL;
#if _MSC_VER > 1400
    fopen_s(&f, fileName, "ab");
#else
    f = fopen(fileName, "ab");
#endif
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        if ((uint32_t)ftell(f) < size)
        {
            for (pos = (uint32_t)ftell(f); pos != size; ++pos)
            {
                if (fputc(0x00, f) != 0)
                {
                    printf("Error Writing to %s\n", fileName);
                    break;
                }
            }
        }
        fclose(f);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_preAction: "), e->target->objectType, e->target->logicalName);
        if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            handleProfileGenericActions(settings, e);
            e->handled = 1;
        }
        else if (e->target == BASE(activePowerL1))
        {
            //Set default value for active power.
            activePowerL1Value = 0;
            e->handled = 1;
        }
        else if (e->target == BASE(pushSetup) && e->index == 1)
        {
            updateState(settings, GURUX_EVENT_CODES_PUSH);
            sendPush(settings, (gxPushSetup*)e->target);
            e->handled = 1;
        }
        //If client wants to clear EEPROM data using Global meter reset script.
        else if (e->target == BASE(scriptTableGlobalMeterReset) && e->index == 1)
        {
            //Initialize data size so default values are used on next connection.
            const char* fileName = "settings.raw";
#if _MSC_VER > 1400
            FILE* f = NULL;
            fopen_s(&f, fileName, "wb");
#else
            FILE* f = fopen(fileName, "wb");
#endif
            if (f != NULL)
            {
                fclose(f);
            }
            //Load objects again.
            if ((ret = loadSettings(settings)) != 0)
            {
                GXTRACE_INT(("Failed to load settings!"), ret);
                executeTime = 0;
                break;
            }
            if ((ret = loadSecurity(settings)) != 0)
            {
                GXTRACE_INT(("Failed to load security settings!"), ret);
                executeTime = 0;
                break;
            }
            updateState(settings, GURUX_EVENT_CODES_GLOBAL_METER_RESET);
            e->handled = 1;
        }
        else if (e->target == BASE(disconnectControl))
        {
            updateState(settings, GURUX_EVENT_CODES_OUTPUT_RELAY_STATE);
            //Disconnect. Turn led OFF.
            if (e->index == 1)
            {
                printf("%s\r\n", "Led is OFF.");
            }
            else //Reconnnect. Turn LED ON.
            {
                printf("%s\r\n", "Led is ON.");
            }
        }
        else if (e->target == BASE(scriptTableActivateTestMode))
        {
            //Activate test mode.
            testMode = 1;
            saveSettings();
        }
        else if (e->target == BASE(scriptTableActivateNormalMode))
        {
            //Activate normal mode.
            testMode = 0;
            saveSettings();
        }
        if (e->target == BASE(imageTransfer))
        {
            e->handled = 1;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            FILE* f;
            gxImageTransfer* i = (gxImageTransfer*)e->target;
            const char* imageFile = "image.raw";
            //Image name and size to transfer
            if (e->index == 1)
            {
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
                //There is only one image.
                gxImageActivateInfo* info;
                imageTransfer.imageActivateInfo.size = 1;
                if ((e->error = arr_getByIndex(&imageTransfer.imageActivateInfo, 0, (void**)&info)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    return;
                }
                uint16_t size;
                if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
                    (ret = cosem_getOctetString2(e->parameters.byteArr, info->identification.data, sizeof(info->identification.data), &size)) != 0 ||
                    (ret = cosem_getUInt32(e->parameters.byteArr, &info->size)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    return;
                }
                info->identification.size = size;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                printf("Updating image %s Size: %ld\r\n", imageFile, info->size);
#endif
                allocateImageTransfer(imageFile, info->size);
                ba_clear(&i->imageTransferredBlocksStatus);
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_INITIATED;
            }
            //Transfers one block of the Image to the server
            else if (e->index == 2)
            {
                uint32_t index;
                uint16_t blockSize;
                if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt32(e->parameters.byteArr, &index)) != 0 ||
                    (ret = hlp_getObjectCount2(e->parameters.byteArr, &blockSize)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                if ((ret = ba_setByIndex(&i->imageTransferredBlocksStatus, (uint16_t)index, 1)) == 0)
                {
                    i->imageFirstNotTransferredBlockNumber = index + 1;
                }
                f = fopen(imageFile, "r+b");
                if (!f)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Unable to open file %s\r\n", imageFile);
#endif
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                int ret = (int)fwrite(e->parameters.byteArr->data + e->parameters.byteArr->position, 1, (int)blockSize, f);
                fclose(f);
                if (ret != (int)blockSize)
                {
                    e->error = DLMS_ERROR_CODE_UNMATCH_TYPE;
                }
                bb_clear(e->parameters.byteArr);
                imageActionStartTime = time(NULL);
                return;
            }
            //Verifies the integrity of the Image before activation.
            else if (e->index == 3)
            {
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_INITIATED;
                f = fopen(imageFile, "rb");
                if (!f)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Unable to open file %s\r\n", imageFile);
#endif
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                fseek(f, 0L, SEEK_END);
                long size = ftell(f);
                fclose(f);
                if (size != IMAGE_ACTIVATE_INFO[0].size)
                {
                    i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED;
                    e->error = DLMS_ERROR_CODE_OTHER_REASON;
                }
                else
                {
                    //Wait 5 seconds before image is verified.  This is for example only.
                    if (time(NULL) - imageActionStartTime < 5)
                    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                        printf("Image verification is on progress.\r\n");
#endif
                        e->error = DLMS_ERROR_CODE_TEMPORARY_FAILURE;
                    }
                    else
                    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                        printf("Image is verificated.\r\n");
#endif
                        i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL;
                        imageActionStartTime = time(NULL);
                    }
                }
            }
            //Activates the Image.
            else if (e->index == 4)
            {
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_INITIATED;
                //Wait 5 seconds before image is activated. This is for example only.
                if (time(NULL) - imageActionStartTime < 5)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Image activation is on progress.\r\n");
#endif
                    e->error = DLMS_ERROR_CODE_TEMPORARY_FAILURE;
                }
                else
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Image is activated.\r\n");
#endif
                    i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_SUCCESSFUL;
                    imageActionStartTime = time(NULL);
                }
            }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postRead(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_postRead: "), e->target->objectType, e->target->logicalName);
    }
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postWrite(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_postWrite: "), e->target->objectType, e->target->logicalName);
        if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            //Use want to change capture objects.
            if (e->index == 3)
            {
                saveSettings();
                //Clear buffer if user changes captured objects.
                gxValueEventArg it;
                ve_init(&it);
                it.index = 1;
                handleProfileGenericActions(settings, &it);
                //Count how many rows fit to the buffer.
                ((gxProfileGeneric*)e->target)->profileEntries = getProfileGenericBufferMaxRowCount(settings, ((gxProfileGeneric*)e->target));
                if (((gxProfileGeneric*)e->target)->captureObjects.size != 0)
                {
                    gxKey* k = NULL;
                    arr_getByIndex(&((gxProfileGeneric*)e->target)->captureObjects, 0, (void**)&k);
                    //Set 1st object to sort object.
                    ((gxProfileGeneric*)e->target)->sortObject = (gxObject*)k->key;
                }
                else
                {
                    ((gxProfileGeneric*)e->target)->sortObject = NULL;
                }
            }
            //Use want to change max amount of profile entries.
            else if (e->index == 8)
            {
                //Count how many rows fit to the buffer.
                uint16_t maxCount = getProfileGenericBufferMaxRowCount(settings, ((gxProfileGeneric*)e->target));
                //If use try to set max profileEntries bigger than can fit to allocated memory.
                if (maxCount < ((gxProfileGeneric*)e->target)->profileEntries)
                {
                    ((gxProfileGeneric*)e->target)->profileEntries = maxCount;
                }
            }
        }
        if (e->error == 0)
        {
            //Save settings to EEPROM.
            saveSettings();
        }
        else
        {
            //Reject changes loading previous settings if there is an error.
            loadSettings(settings);
        }
    }
    //Reset execute time to update execute time if user add new execute times or changes the time.
    executeTime = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postAction(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_postAction: "), e->target->objectType, e->target->logicalName);
        if (e->target == BASE(securitySetupHigh) ||
            e->target == BASE(securitySetupHighGMac))
        {
            //Update block cipher key authentication key or broadcast key.
            //Save settings to EEPROM.
            if (e->error == 0)
            {
                saveSecurity(settings);
            }
            else
            {
                //Load default settings if there is an error.
                loadSecurity(settings);
            }
        }
        //Check is client changing the settings with action.
        else if (svr_isChangedWithAction(e->target->objectType, e->index))
        {
            //Save settings to EEPROM.
            if (e->error == 0)
            {
                saveSettings();
            }
            else
            {
                //Load default settings if there is an error.
                loadSettings(settings);
            }
        }
    }
}

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)

/**
* Connect to Push listener.
*/
int connectServer(
    const char* address,
    int port,
    int* s)
{
    int ret;
    struct sockaddr_in add;
    //create socket.
    *s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (*s == -1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    add.sin_port = htons(port);
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = inet_addr(address);
    //If address is give as name
    if (add.sin_addr.s_addr == INADDR_NONE)
    {
        struct hostent* Hostent = gethostbyname(address);
        if (Hostent == NULL)
        {
#if defined(_WIN32) || defined(_WIN64)//If Windows
            int err = WSAGetLastError();
#else
            int err = errno;
#endif
#if defined(_WIN32) || defined(_WIN64)//Windows includes
            closesocket(*s);
#else
            close(*s);
#endif
            return err;
        };
        add.sin_addr = *(struct in_addr*)(void*)Hostent->h_addr_list[0];
    };

    //Connect to the meter.
    ret = connect(*s, (struct sockaddr*)&add, sizeof(struct sockaddr_in));
    if (ret == -1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    };
    return DLMS_ERROR_CODE_OK;
}

int sendPush(
    dlmsSettings* settings,
    gxPushSetup* push)
{
    char* p, * host;
    int ret, pos, port, s;
    message messages;
    gxByteBuffer* bb;
    p = strchr((char*)push->destination.data, ':');
    if (p == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pos = (int)(p - (char *)push->destination.data);
    host = (char*)malloc(pos + 1);
    memcpy(host, push->destination.data, pos);
    host[pos] = '\0';
    sscanf(++p, "%d", &port);
    mes_init(&messages);
    if ((ret = connectServer(host, port, &s)) == 0)
    {
        if ((ret = notify_generatePushSetupMessages(settings, 0, push, &messages)) == 0)
        {
            for (pos = 0; pos != messages.size; ++pos)
            {
                bb = messages.data[pos];
                if ((ret = send(s, (char*)bb->data, bb->size, 0)) == -1)
                {
                    mes_clear(&messages);
                    break;
                }
            }
        }
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        closesocket(s);
#else
        close(s);
#endif
    }
    mes_clear(&messages);
    free(host);
    return 0;
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

unsigned char svr_isTarget(
    dlmsSettings* settings,
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    GXTRACE(("svr_isTarget."), NULL);
    objectArray objects;
    oa_init(&objects);
    unsigned char ret = 0;
    uint16_t pos;
    gxObject* tmp[6];
    oa_attach(&objects, tmp, sizeof(tmp) / sizeof(tmp[0]));
    objects.size = 0;
    if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, &objects) == 0)
    {
        gxAssociationLogicalName* a;
        for (pos = 0; pos != objects.size; ++pos)
        {
            if (oa_getByIndex(&objects, pos, (gxObject**)&a) == 0)
            {
                if (a->clientSAP == clientAddress)
                {
                    ret = 1;
                    switch (a->authenticationMechanismName.mechanismId)
                    {
                    case DLMS_AUTHENTICATION_NONE:
                        //Client connects without authentication.
                        GXTRACE(("Connecting without authentication."), NULL);
                        break;
                    case DLMS_AUTHENTICATION_LOW:
                        //Client connects using low authentication.
                        GXTRACE(("Connecting using Low authentication."), NULL);
                        break;
                    default:
                        //Client connects using High authentication.
                        GXTRACE(("Connecting using High authentication."), NULL);
                        break;
                    }
                    settings->proposedConformance = a->xDLMSContextInfo.conformance;
                    settings->expectedClientSystemTitle = NULL;
                    //Set Invocation counter value.
                    settings->expectedInvocationCounter = NULL;
                    //Client can establish a ciphered connection only with Security Suite 1.
                    settings->expectedSecuritySuite = 0;
                    //Security policy is not defined by default. Client can connect using any security policy.
                    settings->expectedSecurityPolicy = 0xFF;
                    if (a->securitySetup != NULL)
                    {
                        //Set expected client system title. If this is set only client that is using expected client system title can connect to the meter.
                        if (a->securitySetup->clientSystemTitle.size == 8)
                        {
                            settings->expectedClientSystemTitle = a->securitySetup->clientSystemTitle.data;
                        }
                        //GMac authentication uses innocation counter.
                        if (a->securitySetup == &securitySetupHighGMac)
                        {
                            //Set invocation counter value. If this is set client's invocation counter must match with server IC.
                            settings->expectedInvocationCounter = &securitySetupHighGMac.minimumInvocationCounter;
                        }

                        //Set security suite that client must use.
                        settings->expectedSecuritySuite = a->securitySetup->securitySuite;
                        //Set security policy that client must use if it is set.
                        if (a->securitySetup->securityPolicy != 0)
                        {
                            settings->expectedSecurityPolicy = a->securitySetup->securityPolicy;
                        }
                    }
                    break;
                }
            }
        }
    }
    if (ret == 0)
    {
        GXTRACE_INT(("Invalid authentication level."), clientAddress);
        //Authentication is now allowed. Meter is quiet and doesn't return an error.
    }
    else
    {
        // If address is not broadcast or serial number.
        //Remove logical address from the server address.
        unsigned char broadcast = (serverAddress & 0x3FFF) == 0x3FFF || (serverAddress & 0x7F) == 0x7F;
        if (!(broadcast ||
            (serverAddress & 0x3FFF) == SERIAL_NUMBER % 10000 + 1000))
        {
            ret = 0;
            // Find address from the SAP table.
            gxSapAssignment* sap;
            objects.size = 0;
            if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, &objects) == 0)
            {
                gxSapItem* it;
                uint16_t sapIndex, pos;
                for (sapIndex = 0; sapIndex != objects.size; ++sapIndex)
                {
                    if (oa_getByIndex(&objects, sapIndex, (gxObject**)&sap) == 0)
                    {
                        for (pos = 0; pos != sap->sapAssignmentList.size; ++pos)
                        {
                            if (arr_getByIndex(&sap->sapAssignmentList, pos, (void**)&it) == 0)
                            {
                                //Check server address with one byte.
                                if (((serverAddress & 0xFFFFFF00) == 0 && (serverAddress & 0x7F) == it->id) ||
                                    //Check server address with two bytes.
                                    ((serverAddress & 0xFFFF0000) == 0 && (serverAddress & 0x7FFF) == it->id))
                                {
                                    ret = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            oa_empty(&objects);
        }
        //Set serial number as meter address if broadcast is used.
        if (broadcast)
        {
            settings->serverAddress = SERIAL_NUMBER % 10000 + 1000;
        }
        if (ret == 0)
        {
            GXTRACE_INT(("Invalid server address"), serverAddress);
        }
    }
    return ret;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
    dlmsServerSettings* settings,
    DLMS_AUTHENTICATION authentication,
    gxByteBuffer* password)
{
    GXTRACE(("svr_validateAuthentication"), NULL);
    if (authentication == DLMS_AUTHENTICATION_NONE)
    {
        //Uncomment this if authentication is always required.
        //return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
        return DLMS_SOURCE_DIAGNOSTIC_NONE;
    }
    //Check Low Level security..
    if (authentication == DLMS_AUTHENTICATION_LOW)
    {
        if (bb_compare(password, associationLow.secret.data, associationLow.secret.size) == 0)
        {
            GXTRACE(("Invalid low level password."), (const char*)associationLow.secret.data);
            return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
        }
    }
    // Hith authentication levels are check on phase two.
    return DLMS_SOURCE_DIAGNOSTIC_NONE;
}


//Get attribute access level for profile generic.
DLMS_ACCESS_MODE getProfileGenericAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    //Only read is allowed for event log.
    if (obj == BASE(eventLog))
    {
        return DLMS_ACCESS_MODE_READ;
    }
    //Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 3://captureObjects.
            return DLMS_ACCESS_MODE_READ_WRITE;
        case 4://capturePeriod
            return DLMS_ACCESS_MODE_READ_WRITE;
        case 8://Profile entries.
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}


//Get attribute access level for Push Setup.
DLMS_ACCESS_MODE getPushSetupAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    //Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 2://pushObjectList
        case 4://communicationWindow
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}

//Get attribute access level for Disconnect Control.
DLMS_ACCESS_MODE getDisconnectControlAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

//Get attribute access level for register schedule.
DLMS_ACCESS_MODE getActionSchduleAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    //Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 4://Execution time.
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}

//Get attribute access level for register.
DLMS_ACCESS_MODE getRegisterAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

//Get attribute access level for data objects.
DLMS_ACCESS_MODE getDataAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

//Get attribute access level for script table.
DLMS_ACCESS_MODE getScriptTableAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ;
}

//Get attribute access level for IEC HDLS setup.
DLMS_ACCESS_MODE getHdlcSetupAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    //Write is allowed only for High authentication.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        switch (index)
        {
        case 2: //Communication speed.
        case 7:
        case 8:
            return DLMS_ACCESS_MODE_READ_WRITE;
        default:
            break;
        }
    }
    return DLMS_ACCESS_MODE_READ;
}


//Get attribute access level for association LN.
DLMS_ACCESS_MODE getAssociationAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    //If secret
    if (settings->authentication == DLMS_AUTHENTICATION_LOW && index == 7)
    {
        return DLMS_ACCESS_MODE_READ_WRITE;
    }
    return DLMS_ACCESS_MODE_READ;
}

//Get attribute access level for security setup.
DLMS_ACCESS_MODE getSecuritySetupAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    //Only client system title is writable.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW && index == 4)
    {
        return DLMS_ACCESS_MODE_READ_WRITE;
    }
    return DLMS_ACCESS_MODE_READ;
}


//Get attribute access level for security setup.
DLMS_ACCESS_MODE getActivityCalendarAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    //Only Activate passive calendar date-time and passive calendar settings are writeble.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW && index > 5)
    {
        return DLMS_ACCESS_MODE_READ_WRITE;
    }
    return DLMS_ACCESS_MODE_READ;
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    GXTRACE("svr_getAttributeAccess", NULL);
    // Only read is allowed if authentication is not used.
    if (index == 1 || settings->authentication == DLMS_AUTHENTICATION_NONE)
    {
        return DLMS_ACCESS_MODE_READ;
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        return getAssociationAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
    {
        return getProfileGenericAttributeAccess(settings, obj, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP)
    {
        return getPushSetupAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL)
    {
        return getDisconnectControlAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL)
    {
        return getDisconnectControlAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_ACTION_SCHEDULE)
    {
        return getActionSchduleAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_SCRIPT_TABLE)
    {
        return getScriptTableAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_REGISTER)
    {
        return getRegisterAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_DATA)
    {
        return getDataAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_IEC_HDLC_SETUP)
    {
        return getHdlcSetupAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_SECURITY_SETUP)
    {
        return getSecuritySetupAttributeAccess(settings, index);
    }
    if (obj->objectType == DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR)
    {
        return getActivityCalendarAttributeAccess(settings, index);
    }
    // Only clock write is allowed.
    if (settings->authentication == DLMS_AUTHENTICATION_LOW)
    {
        if (obj->objectType == DLMS_OBJECT_TYPE_CLOCK)
        {
            return DLMS_ACCESS_MODE_READ_WRITE;
        }
        return DLMS_ACCESS_MODE_READ;
    }
    // All writes are allowed.
    return DLMS_ACCESS_MODE_READ_WRITE;
}

/**
* Get method access level.
*/
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    // Methods are not allowed.
    if (settings->authentication == DLMS_AUTHENTICATION_NONE)
    {
        return DLMS_METHOD_ACCESS_MODE_NONE;
    }
    // Only clock methods are allowed.
    if (settings->authentication == DLMS_AUTHENTICATION_LOW)
    {
        if (obj->objectType == DLMS_OBJECT_TYPE_CLOCK)
        {
            return DLMS_METHOD_ACCESS_MODE_ACCESS;
        }
        return DLMS_METHOD_ACCESS_MODE_NONE;
    }
    return DLMS_METHOD_ACCESS_MODE_ACCESS;
}

/////////////////////////////////////////////////////////////////////////////
//Client has made connection to the server.
/////////////////////////////////////////////////////////////////////////////
int svr_connected(
    dlmsServerSettings* settings)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    printf("Connected %d.\r\n", settings->base.connected);
#endif
#ifdef DLMS_ITALIAN_STANDARD
    if (settings->base.clientAddress == 1)
    {
        if (settings->base.connected != DLMS_CONNECTION_STATE_DLMS)
        {
            if (settings->base.preEstablishedSystemTitle != NULL)
            {
                bb_clear(settings->base.preEstablishedSystemTitle);
            }
            else
            {
                settings->base.preEstablishedSystemTitle = (gxByteBuffer*)malloc(sizeof(gxByteBuffer));
                bb_init(settings->base.preEstablishedSystemTitle);
            }
            bb_addString(settings->base.preEstablishedSystemTitle, "ABCDEFGH");
            settings->base.cipher.security = DLMS_SECURITY_AUTHENTICATION_ENCRYPTION;
        }
        else
        {
            //Return error if client can connect only using pre-established connnection.
            return DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
}
#else
#endif //DLMS_ITALIAN_STANDARD
    return 0;
}

/**
    * Client has try to made invalid connection. Password is incorrect.
    *
    * @param connectionInfo
    *            Connection information.
    */
int svr_invalidConnection(dlmsServerSettings* settings)
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
    dlmsServerSettings* settings)
{
    printf("Disconnected %d.\r\n", settings->base.connected);
    if (settings->base.cipher.security != 0 && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
    {
        //Save Invocation counter value when connection is closed.
        saveSecurity(&settings->base);
    }
    return 0;
}

void svr_preGet(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
    }
}

void svr_postGet(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{

}

/**
* This is reserved for future use. Do not use it.
*
* @param args
*            Handled data type requests.
*/
void svr_getDataType(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{

}