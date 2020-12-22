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

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <tchar.h>
#include <conio.h>
#include <Winsock2.h> //Add support for sockets
#include <time.h>
#include <process.h>//Add support for threads
#include "../include/getopt.h"
#else //Linux includes.
#define closesocket close
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h> //Add support for sockets
#include <fcntl.h> //O_NONBLOCK needs this.
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#endif

#include "../dlms/include/dlmssettings.h"
#include "../dlms/include/variant.h"
#include "../dlms/include/cosem.h"
#include "../dlms/include/server.h"
//Add this if you want to send notify messages.
#include "../dlms/include/notify.h"
//Add support for serialization.
#include "../dlms/include/gxserializer.h"

//DLMS settings.
dlmsServerSettings settings;

GX_TRACE_LEVEL trace = GX_TRACE_LEVEL_OFF;

const static char* FLAG_ID = "GRX";
//Serialization version is increased every time when structure of serialized data is changed.
const static uint16_t SERIALIZATION_VERSION = 2;

//Space for client password.
static unsigned char PASSWORD[20];
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
//Buffer where PDUs are saved. Add 3 bytes for LLC bytes.
static unsigned char pduBuff[3 + PDU_BUFFER_SIZE];
static unsigned char replyFrame[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
//Define server system title.
static unsigned char SERVER_SYSTEM_TITLE[8] = { 0 };
time_t imageActionStartTime = 0;
gxImageActivateInfo IMAGE_ACTIVATE_INFO[1];
static gxIecTwistedPairSetup twistedPairSetup;

uint32_t time_current(void)
{
    //Get current time somewhere.
    return (uint32_t)time(NULL);
}

uint32_t time_elapsed(void)
{
    return (uint32_t)clock() / (CLOCKS_PER_SEC / 1000);
}

static gxByteBuffer reply;

static gxClock clock1;
static gxIecHdlcSetup hdlc;
static gxData ldn;
static gxData eventCode;
static gxData unixTime;
static gxData invocationCounter;
static gxAssociationLogicalName associationNone;
static gxAssociationLogicalName associationLow;
static gxAssociationLogicalName associationHigh;
static gxAssociationLogicalName associationHighGMac;
static gxRegister activePowerL1;
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
gxSpecialDaysTable specialDaysTable;
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
gxPrimeNbOfdmPlcMacFunctionalParameters primeNbOfdmPlcMacFunctionalParameters;
gxPrimeNbOfdmPlcMacNetworkAdministrationData primeNbOfdmPlcMacNetworkAdministrationData;

static gxScriptTable tarifficationScriptTable;
gxRegisterActivation registerActivation;
gxData currentlyActiveTariff;

//static gxObject* NONE_OBJECTS[] = { BASE(associationNone), BASE(ldn) };

//Append new COSEM object to the end so serialization will work correctly.
static gxObject* ALL_OBJECTS[] = { BASE(associationNone), BASE(associationLow), BASE(associationHigh), BASE(associationHighGMac), BASE(securitySetupHigh), BASE(securitySetupHighGMac),
                                   BASE(ldn), BASE(sapAssignment), BASE(eventCode),
                                   BASE(clock1), BASE(activePowerL1), BASE(pushSetup), BASE(scriptTableGlobalMeterReset), BASE(scriptTableDisconnectControl),
                                   BASE(scriptTableActivateTestMode), BASE(scriptTableActivateNormalMode), BASE(loadProfile), BASE(eventLog), BASE(hdlc),
                                   BASE(disconnectControl), BASE(actionScheduleDisconnectOpen), BASE(actionScheduleDisconnectClose), BASE(unixTime), BASE(invocationCounter),
                                   BASE(imageTransfer), BASE(udpSetup), BASE(autoConnect), BASE(activityCalendar), BASE(localPortSetup), BASE(demandRegister),
                                   BASE(registerMonitor), BASE(autoAnswer), BASE(modemConfiguration), BASE(macAddressSetup), BASE(ip4Setup), BASE(pppSetup), BASE(gprsSetup),
                                   BASE(tarifficationScriptTable), BASE(registerActivation), BASE(primeNbOfdmPlcMacFunctionalParameters), BASE(primeNbOfdmPlcMacNetworkAdministrationData),
                                   BASE(twistedPairSetup), BASE(specialDaysTable), BASE(currentlyActiveTariff)
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
int saveSecurity()
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
    unsigned char tmp[100];
    gxByteBuffer bb;
    BB_ATTACH(bb, tmp, 0);
    if (f != NULL)
    {
        if ((ret = bb_set(&bb, settings.base.cipher.blockCipherKey, 16)) == 0 &&
            (ret = bb_set(&bb, settings.base.cipher.authenticationKey, 16)) == 0 &&
            (ret = bb_set(&bb, settings.base.kek, 16)) == 0 &&
            //Save server IV.
            (ret = bb_setUInt32(&bb, settings.base.cipher.invocationCounter)) == 0 &&
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
    if (f != NULL)
    {
        unsigned char tmp[2048];
        gxByteBuffer bb;
        BB_ATTACH(bb, tmp, 0);
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
    gxProfileGeneric* pg,
    DLMS_DATA_TYPE* dataTypes,
    uint8_t* columnSizes,
    uint16_t* rowSize)
{
    int ret = 0;
    uint8_t pos;
    gxTarget* it;
    unsigned char pduBuff[PDU_MAX_PROFILE_GENERIC_COLUMN_SIZE];
    gxByteBuffer pdu;
    bb_attach(&pdu, pduBuff, 0, sizeof(pduBuff));
    gxValueEventArg e;
    ve_init(&e);
    *rowSize = 0;
    //Loop capture columns and get values.
    for (pos = 0; pos != pg->captureObjects.size; ++pos)
    {
        if ((ret = arr_getByIndex(&pg->captureObjects, (uint16_t)pos, (void**)&it, sizeof(gxTarget))) != 0)
        {
            break;
        }
        bb_clear(&pdu);
        ve_clear(&e);
        //Date time is saved in EPOCH to save space.
        if (it->target->objectType == DLMS_OBJECT_TYPE_CLOCK && it->attributeIndex == 2)
        {
            e.value.ulVal = time_current();
            e.value.vt = DLMS_DATA_TYPE_UINT32;
            pdu.size = 5;
        }
        else
        {
            e.target = it->target;
            e.index = it->attributeIndex;
            e.value.byteArr = &pdu;
            e.value.vt = DLMS_DATA_TYPE_OCTET_STRING;
            if ((ret = cosem_getValue(&settings.base, &e)) != 0)
            {
                break;
            }
            //Get data type.
            e.value.vt = pdu.data[0];
        }
        if (dataTypes != NULL)
        {
            dataTypes[pos] = e.value.vt;
        }
        if (columnSizes != NULL)
        {
            columnSizes[pos] = (uint8_t)pdu.size - 1;
        }
        *rowSize += pdu.size - 1;
    }
    return ret;
}

//Get max row count for allocated buffer.
uint16_t getProfileGenericBufferMaxRowCount(gxProfileGeneric* pg)
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
        getProfileGenericBufferColumnSizes(pg, NULL, NULL, &rowSize);
        if (rowSize != 0)
        {
            fseek(f, 0L, SEEK_END);
            count = (uint16_t)ftell(f);
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

int captureProfileGeneric(gxProfileGeneric* pg)
{
    unsigned char pos;
    gxTarget* it;
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
        bb_setUInt16(&pdu, (1 + index) % (pg->profileEntries));

        //Update how many entries is used until buffer is full.
        if (ret == 0 && pg->entriesInUse != pg->profileEntries)
        {
            //Total amount of the entries.
            ++pg->entriesInUse;
        }
        bb_setUInt16(&pdu, (uint16_t)pg->entriesInUse);
        //Update values to the EEPROM.
        fwrite(pdu.data, 1, 4, f);

        getProfileGenericBufferColumnSizes(pg, dataTypes, columnSizes, &dataSize);
        if (index != 0 && pg->profileEntries != 0)
        {
            fseek(f, 4 + ((index % pg->profileEntries) * dataSize), SEEK_SET);
        }
        //Loop capture columns and get values.
        for (pos = 0; pos != pg->captureObjects.size; ++pos)
        {
            if ((ret = arr_getByIndex(&pg->captureObjects, pos, (void**)&it, sizeof(gxTarget))) != 0)
            {
                break;
            }
            bb_clear(&pdu);
            //Date time is saved in EPOCH to save space.
            if ((it->target->objectType == DLMS_OBJECT_TYPE_CLOCK || it->target == BASE(unixTime)) && it->attributeIndex == 2)
            {
                e.value.ulVal = time_current();
                e.value.vt = DLMS_DATA_TYPE_UINT32;
                fwrite(&e.value.bVal, 4, 1, f);
            }
            else
            {
                e.target = it->target;
                e.index = it->attributeIndex;
                e.value.byteArr = &pdu;
                e.value.vt = DLMS_DATA_TYPE_OCTET_STRING;
                if ((ret = cosem_getValue(&settings.base, &e)) != 0)
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
    return ret;
}

void updateState(uint16_t value)
{
    GX_UINT16(eventCode.value) = value;
    captureProfileGeneric(&eventLog);
    saveSettings();
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

///////////////////////////////////////////////////////////////////////
//Print detailed information to the serial port.
void svr_trace(const char* str, const char* data)
{
    GXTRACE(str, data);
}
///////////////////////////////////////////////////////////////////////

void time_now(
    gxtime* value,
    unsigned char meterTime)
{
    uint32_t offset = time_current();
    time_initUnix(value, offset);
    //If date time is wanted in meter time, not in utc time.
    if (meterTime)
    {
        clock_utcToMeterTime(&clock1, value);
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
    static char SECRET[20];
    strcpy(SECRET, "Gurux");
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
        BB_ATTACH_STR(associationLow.secret, SECRET, (uint16_t)strlen(SECRET));
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
    //User list.
    static gxUser USER_LIST[10] = { 0 };
    static char SECRET[20];
    strcpy(SECRET, "Gurux");
    //Dedicated key.
    static unsigned char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 3, 255 };
    if ((ret = INIT_OBJECT(associationHigh, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHigh.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH;
        OA_ATTACH(associationHigh.objectList, ALL_OBJECTS);
        BB_ATTACH(associationHigh.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        //All objects are add for this Association View later.
        ARR_ATTACH(associationHigh.userList, USER_LIST, 0);
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
        BB_ATTACH_STR(associationHigh.secret, SECRET, (uint16_t)strlen(SECRET));
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
    //User list.
    static gxUser USER_LIST[10] = { 0 };
    //Dedicated key.
    static unsigned char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 4, 255 };
    if ((ret = INIT_OBJECT(associationHighGMac, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHighGMac.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH_GMAC;
        OA_ATTACH(associationHighGMac.objectList, ALL_OBJECTS);
        BB_ATTACH(associationHighGMac.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        //All objects are add for this Association View later.
        ARR_ATTACH(associationHighGMac.userList, USER_LIST, 0);
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
//Add SAP Assignment object.
///////////////////////////////////////////////////////////////////////
int addSapAssignment()
{
    int ret;
    static gxSapItem SAP_ITEMS[5];
    const unsigned char ln[6] = { 0, 0, 41, 0, 0, 255 };
    if ((ret = INIT_OBJECT(sapAssignment, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, ln)) == 0)
    {
        sprintf(SAP_ITEMS[0].name.value, "%s%.13lu", FLAG_ID, SERIAL_NUMBER);
        SAP_ITEMS[0].name.size = 16;
        SAP_ITEMS[0].id = 1;
        ARR_ATTACH(sapAssignment.sapAssignmentList, SAP_ITEMS, 1);
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
        //Define Logical Device Name.
        static char LDN[17];
        sprintf(LDN, "%s%.13lu", FLAG_ID, SERIAL_NUMBER);
        GX_OCTET_STRING(ldn.value, LDN, sizeof(LDN));
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

///////////////////////////////////////////////////////////////
//Add image transfer object.
///////////////////////////////////////////////////////////////////////
int addImageTransfer()
{
    int ret;
    unsigned char IMAGE_TRANSFERRED_BLOCKS_STATUS[1000];
    unsigned char ln[6] = { 0,0,44,0,0,255 };
    if ((ret = INIT_OBJECT(imageTransfer, DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln)) == 0)
    {
        imageTransfer.imageBlockSize = 100;
        imageTransfer.imageFirstNotTransferredBlockNumber = 0;
        //Enable image transfer.
        imageTransfer.imageTransferEnabled = 1;
        ARR_ATTACH(imageTransfer.imageActivateInfo, IMAGE_ACTIVATE_INFO, 0);
        BIT_ATTACH(imageTransfer.imageTransferredBlocksStatus, IMAGE_TRANSFERRED_BLOCKS_STATUS, 0);
    }
    return ret;
}


///////////////////////////////////////////////////////////////////////
//This method adds example TCP/UDP setup object.
///////////////////////////////////////////////////////////////////////
int addTcpUdpSetup()
{
    int ret;
    //Add Tcp/Udp setup. Default Logical Name is 0.0.25.0.0.255.
    const unsigned char ln[6] = { 0,0,25,0,0,255 };
    if ((ret = INIT_OBJECT(udpSetup, DLMS_OBJECT_TYPE_TCP_UDP_SETUP, ln)) == 0)
    {
        udpSetup.port = 4061;
        udpSetup.maximumSimultaneousConnections = 1;
        udpSetup.maximumSegmentSize = 40;
        udpSetup.inactivityTimeout = 180;
        udpSetup.ipSetup = BASE(ip4Setup);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Auto connect object.
///////////////////////////////////////////////////////////////////////
int addAutoConnect()
{
    int ret;
    static gxTimePair CALLING_WINDOW[10] = { 0 };
    static unsigned char DESTINATIONS[10][20] = { 0 };

    const unsigned char ln[6] = { 0,0,2,1,0,255 };
    if ((ret = INIT_OBJECT(autoConnect, DLMS_OBJECT_TYPE_AUTO_CONNECT, ln)) == 0)
    {
        autoConnect.mode = DLMS_AUTO_CONNECT_MODE_AUTO_DIALLING_ALLOWED_ANYTIME;
        autoConnect.repetitions = 10;
        autoConnect.repetitionDelay = 60;
        //Calling is allowed between 1am to 6am.
        time_init(&CALLING_WINDOW[0].first, -1, -1, -1, 1, 0, 0, -1, -clock1.timeZone);
        time_init(&CALLING_WINDOW[0].second, -1, -1, -1, 6, 0, 0, -1, -clock1.timeZone);
        ARR_ATTACH(autoConnect.callingWindow, CALLING_WINDOW, 1);
        //Add one destination.
        strcpy((char*)DESTINATIONS[0], "127.0.0.1:4059");
        ARR_ATTACH(autoConnect.destinations, DESTINATIONS, 1);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Activity Calendar object.
///////////////////////////////////////////////////////////////////////
int addActivityCalendar()
{
    int ret;
    gxWeekProfile* wp;
    //Define active calendar name.
    static unsigned char ACTIVE_CALENDAR_NAME[20] = { 0 };
    //Define passive calendar name.
    static unsigned char PASSIVE_CALENDAR_NAME[20] = { 0 };

    static gxSeasonProfile ACTIVE_SEASON_PROFILE[10];
    static gxSeasonProfile PASSIVE_SEASON_PROFILE[10];
    static gxWeekProfile ACTIVE_WEEK_PROFILE[10];
    static gxWeekProfile PASSIVE_WEEK_PROFILE[10];
    static gxDayProfile ACTIVE_DAY_PROFILE[3];
    static gxDayProfile PASSIVE_DAY_PROFILE[3];
    //Own day profile action for each day profile.
    static gxDayProfileAction ACTIVE_DAY_PROFILE_ACTIONS1[10];
    static gxDayProfileAction ACTIVE_DAY_PROFILE_ACTIONS2[10];
    static gxDayProfileAction ACTIVE_DAY_PROFILE_ACTIONS3[10];
    static gxDayProfileAction PASSIVE_DAY_PROFILE_ACTIONS1[10];
    static gxDayProfileAction PASSIVE_DAY_PROFILE_ACTIONS2[10];
    static gxDayProfileAction PASSIVE_DAY_PROFILE_ACTIONS3[10];

    const unsigned char ln[6] = { 0,0,13,0,0,255 };
    if ((ret = INIT_OBJECT(activityCalendar, DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR, ln)) == 0)
    {
        const char* activeSeasonName = "DST";
        const char* passiveSeasonName = "Normal";
        const char* WeekProfileName01 = "01";
        const char* WeekProfileName02 = "02";
        strcpy((char*)ACTIVE_CALENDAR_NAME, "DST");
        strcpy((char*)PASSIVE_CALENDAR_NAME, "Passive");

        BB_ATTACH(activityCalendar.calendarNameActive, ACTIVE_CALENDAR_NAME, (unsigned short)strlen((char*)ACTIVE_CALENDAR_NAME));

        ARR_ATTACH(activityCalendar.seasonProfileActive, ACTIVE_SEASON_PROFILE, 2);
        ARR_ATTACH(ACTIVE_DAY_PROFILE[0].daySchedules, ACTIVE_DAY_PROFILE_ACTIONS1, 3);
        ARR_ATTACH(ACTIVE_DAY_PROFILE[1].daySchedules, ACTIVE_DAY_PROFILE_ACTIONS2, 3);
        ARR_ATTACH(ACTIVE_DAY_PROFILE[2].daySchedules, ACTIVE_DAY_PROFILE_ACTIONS3, 0);

        time_init(&ACTIVE_DAY_PROFILE_ACTIONS1[0].startTime, -1, -1, -1, 0, 0, 0, 0, 0x8000);
        time_init(&ACTIVE_DAY_PROFILE_ACTIONS1[1].startTime, -1, -1, -1, 7, 0, 0, 0, 0x8000);
        time_init(&ACTIVE_DAY_PROFILE_ACTIONS1[2].startTime, -1, -1, -1, 22, 0, 0, 0, 0x8000);

        time_init(&ACTIVE_DAY_PROFILE_ACTIONS2[0].startTime, -1, -1, -1, 0, 0, 0, 0, 0x8000);
        time_init(&ACTIVE_DAY_PROFILE_ACTIONS2[1].startTime, -1, -1, -1, 7, 0, 0, 0, 0x8000);
        time_init(&ACTIVE_DAY_PROFILE_ACTIONS2[2].startTime, -1, -1, -1, 22, 0, 0, 0, 0x8000);
        /////////////////////////////////////////////////////////////////////////
        //Add active season profile when DST began.
        SET_OCTET_STRING(ACTIVE_SEASON_PROFILE[0].name, activeSeasonName, (unsigned short)strlen(activeSeasonName));
        SET_OCTET_STRING(ACTIVE_SEASON_PROFILE[0].weekName, WeekProfileName01, (unsigned short)strlen(WeekProfileName01));
        time_init(&ACTIVE_SEASON_PROFILE[0].start, -1, 4, 1, 0, 0, 0, -1, 0x8000);
        //Add week profile.
        ARR_ATTACH(activityCalendar.weekProfileTableActive, ACTIVE_WEEK_PROFILE, 2);
        SET_OCTET_STRING(ACTIVE_WEEK_PROFILE[0].name, WeekProfileName01, (unsigned short)strlen(WeekProfileName01));
        wp = &ACTIVE_WEEK_PROFILE[0];
        wp->monday = wp->tuesday = wp->wednesday = wp->thursday = wp->friday = wp->saturday = wp->sunday = 1;
        //Add active day profiles. There are max three day profiles where one is in use.
        ARR_ATTACH(activityCalendar.dayProfileTableActive, ACTIVE_DAY_PROFILE, 2);
        ACTIVE_DAY_PROFILE[0].dayId = 1;

        ACTIVE_DAY_PROFILE_ACTIONS1[0].script = BASE(tarifficationScriptTable);
        ACTIVE_DAY_PROFILE_ACTIONS1[0].scriptSelector = 1;
        ACTIVE_DAY_PROFILE_ACTIONS1[1].script = BASE(tarifficationScriptTable);
        ACTIVE_DAY_PROFILE_ACTIONS1[1].scriptSelector = 1;
        ACTIVE_DAY_PROFILE_ACTIONS1[2].script = BASE(tarifficationScriptTable);
        ACTIVE_DAY_PROFILE_ACTIONS1[2].scriptSelector = 1;

        ACTIVE_DAY_PROFILE_ACTIONS2[0].script = BASE(tarifficationScriptTable);
        ACTIVE_DAY_PROFILE_ACTIONS2[0].scriptSelector = 2;
        ACTIVE_DAY_PROFILE_ACTIONS2[1].script = BASE(tarifficationScriptTable);
        ACTIVE_DAY_PROFILE_ACTIONS2[1].scriptSelector = 2;
        ACTIVE_DAY_PROFILE_ACTIONS2[2].script = BASE(tarifficationScriptTable);
        ACTIVE_DAY_PROFILE_ACTIONS2[2].scriptSelector = 2;

        /////////////////////////////////////////////////////////////////////////
        //Add active season profile when normal times began.
        SET_OCTET_STRING(ACTIVE_SEASON_PROFILE[1].name, passiveSeasonName, (unsigned short)strlen(passiveSeasonName));
        SET_OCTET_STRING(ACTIVE_SEASON_PROFILE[1].weekName, WeekProfileName02, (unsigned short)strlen(WeekProfileName02));
        time_init(&ACTIVE_SEASON_PROFILE[1].start, -1, 11, 1, 0, 0, 0, -1, 0x8000);

        /////////////////////////////////////////////////////////////////////////
        //Add week profile.
        SET_OCTET_STRING(ACTIVE_WEEK_PROFILE[1].name, WeekProfileName02, (unsigned short)strlen(WeekProfileName02));
        wp = &ACTIVE_WEEK_PROFILE[1];
        wp->monday = wp->tuesday = wp->wednesday = wp->thursday = wp->friday = wp->saturday = wp->sunday = 2;

        /////////////////////////////////////////////////////////////////////////
        //Add passive day profiles. There are max three day profiles where one is in use.
        ACTIVE_DAY_PROFILE[1].dayId = 2;
        /////////////////////////////////////////////////////////////////////////
        //Add passive calendar.
        BB_ATTACH(activityCalendar.calendarNamePassive, PASSIVE_CALENDAR_NAME, (unsigned short)strlen((char*)PASSIVE_CALENDAR_NAME));
        ARR_ATTACH(activityCalendar.seasonProfilePassive, PASSIVE_SEASON_PROFILE, 0);
        ARR_ATTACH(activityCalendar.weekProfileTablePassive, PASSIVE_WEEK_PROFILE, 0);
        ARR_ATTACH(activityCalendar.dayProfileTablePassive, PASSIVE_DAY_PROFILE, 0);
        ARR_ATTACH(PASSIVE_DAY_PROFILE[0].daySchedules, PASSIVE_DAY_PROFILE_ACTIONS1, 0);
        ARR_ATTACH(PASSIVE_DAY_PROFILE[1].daySchedules, PASSIVE_DAY_PROFILE_ACTIONS2, 0);
        ARR_ATTACH(PASSIVE_DAY_PROFILE[2].daySchedules, PASSIVE_DAY_PROFILE_ACTIONS3, 0);
        time_init(&PASSIVE_DAY_PROFILE_ACTIONS1[0].startTime, -1, -1, -1, 0, 0, 0, 0, 0x8000);

        //Activate passive calendar is not called.
        time_init(&activityCalendar.time, -1, -1, -1, -1, -1, -1, -1, 0x8000);
    }
    return ret;
}


///////////////////////////////////////////////////////////////////////
//Add Optical Port Setup object.
///////////////////////////////////////////////////////////////////////
int addSpecialDaysTable()
{
    int ret;
    static gxSpecialDay DAYS[10];
    const unsigned char ln[6] = { 0, 0, 11, 0, 0, 255 };
    if ((ret = INIT_OBJECT(specialDaysTable, DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE, ln)) == 0)
    {
        //Add new year using Gregorian (Julian) calendar.
        DAYS[0].index = 0;
        time_init(&DAYS[0].date, -1, 1, 1, -1, -1, -1, -1, 0x8000);
        DAYS[0].dayId = 3;
        //Add special exact day.
        DAYS[1].index = 1;
        time_init(&DAYS[1].date, 2021, 6, 3, -1, -1, -1, -1, 0x8000);
        DAYS[1].dayId = 3;
        ARR_ATTACH(specialDaysTable.entries, DAYS, 2);
    }
    return ret;
}

//Add Currently active tariff object.
int addCurrentlyActiveTariff()
{
    int ret;
    static unsigned char TARIFF[10];
    const unsigned char ln[6] = { 0, 0, 96, 14, 0, 255 };
    if ((ret = INIT_OBJECT(currentlyActiveTariff, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_OCTET_STRING(currentlyActiveTariff.value, TARIFF, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Optical Port Setup object.
///////////////////////////////////////////////////////////////////////
int addOpticalPortSetup()
{
    int ret;
    static unsigned char DEVICE_ADDRESS[10];
    static unsigned char PASSWORD1[10];
    static unsigned char PASSWORD2[10];
    static unsigned char PASSWORD5[10];
    const unsigned char ln[6] = { 0,0,20,0,0,255 };
    if ((ret = INIT_OBJECT(localPortSetup, DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP, ln)) == 0)
    {
        localPortSetup.defaultMode = DLMS_OPTICAL_PROTOCOL_MODE_DEFAULT;
        localPortSetup.proposedBaudrate = DLMS_BAUD_RATE_9600;
        localPortSetup.defaultBaudrate = DLMS_BAUD_RATE_300;
        localPortSetup.responseTime = DLMS_LOCAL_PORT_RESPONSE_TIME_200_MS;
        BB_ATTACH(localPortSetup.deviceAddress, DEVICE_ADDRESS, 0);
        bb_addString(&localPortSetup.deviceAddress, "Gurux");
        BB_ATTACH(localPortSetup.password1, PASSWORD1, 0);
        bb_addString(&localPortSetup.password1, "Gurux1");
        BB_ATTACH(localPortSetup.password2, PASSWORD2, 0);
        bb_addString(&localPortSetup.password2, "Gurux2");
        BB_ATTACH(localPortSetup.password5, PASSWORD5, 0);
        ret = bb_addString(&localPortSetup.password5, "Gurux5");
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Demand Register object.
///////////////////////////////////////////////////////////////////////
int addDemandRegister()
{
    int ret;
    const unsigned char ln[6] = { 1,0,31,4,0,255 };
    if ((ret = INIT_OBJECT(demandRegister, DLMS_OBJECT_TYPE_DEMAND_REGISTER, ln)) == 0)
    {
        var_setUInt16(&demandRegister.currentAverageValue, 10);
        var_setUInt16(&demandRegister.lastAverageValue, 20);
        var_setUInt8(&demandRegister.status, 1);
        time_now(&demandRegister.startTimeCurrent, 1);
        time_now(&demandRegister.captureTime, 1);
        demandRegister.period = 10;
        demandRegister.numberOfPeriods = 1;
    }
    return ret;
}
///////////////////////////////////////////////////////////////////////
//Add Register Monitor object.
///////////////////////////////////////////////////////////////////////
int addRegisterMonitor()
{
    int ret;
    //Register Monitor threshold values.
    static dlmsVARIANT THRESHOLDS[10] = { 0 };
    static gxActionSet ACTIONS[10] = { 0 };
    const unsigned char ln[6] = { 0,0,16,1,0,255 };
    if ((ret = INIT_OBJECT(registerMonitor, DLMS_OBJECT_TYPE_REGISTER_MONITOR, ln)) == 0)
    {
        VA_ATTACH(registerMonitor.thresholds, THRESHOLDS, 2);
        ARR_ATTACH(registerMonitor.actions, ACTIONS, 2);

        //Add low value.
        GX_UINT32(THRESHOLDS[0]) = 10000;
        //Add high value.
        GX_UINT32(THRESHOLDS[1]) = 30000;
        registerMonitor.monitoredValue.attributeIndex = 2;
        registerMonitor.monitoredValue.target = BASE(activePowerL1);

        //Add actions. Call disconnect control.
        ACTIONS[0].actionDown.script = &scriptTableDisconnectControl;
        ACTIONS[0].actionDown.scriptSelector = 1;
        ACTIONS[0].actionUp.script = NULL;
        ACTIONS[0].actionUp.scriptSelector = 0;
        //Add high action.
        ACTIONS[1].actionDown.script = NULL;
        ACTIONS[1].actionDown.scriptSelector = 0;
        ACTIONS[1].actionUp.script = &scriptTableDisconnectControl;
        ACTIONS[1].actionUp.scriptSelector = 1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Auto Answer object.
///////////////////////////////////////////////////////////////////////
int addAutoAnswer()
{
    int ret;
    static gxTimePair LISTENING_WINDOW[10] = { 0 };
    const unsigned char ln[6] = { 0,0,2,2,0,255 };
    if ((ret = INIT_OBJECT(autoAnswer, DLMS_OBJECT_TYPE_AUTO_ANSWER, ln)) == 0)
    {
        ARR_ATTACH(autoAnswer.listeningWindow, LISTENING_WINDOW, 1);
        //Set start time.
        time_init(&LISTENING_WINDOW[0].first, -1, -1, -1, 6, -1, -1, -1, -clock1.timeZone);
        //Set end time.
        time_init(&LISTENING_WINDOW[0].second, -1, -1, -1, 8, -1, -1, -1, -clock1.timeZone);
        autoAnswer.mode = DLMS_AUTO_CONNECT_MODE_EMAIL_SENDING;
        autoAnswer.status = DLMS_AUTO_ANSWER_STATUS_INACTIVE;
        autoAnswer.numberOfCalls = 0;
        autoAnswer.numberOfRingsInListeningWindow = 1;
        autoAnswer.numberOfRingsOutListeningWindow = 2;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Modem Configuration object.
///////////////////////////////////////////////////////////////////////
int addModemConfiguration()
{
    int ret;
    static gxModemProfile PROFILES[10] = { 0 };
    static gxModemInitialisation INITIALISATIONS[20] = { 0 };
    const unsigned char ln[6] = { 0,0,2,0,0,255 };
    if ((ret = INIT_OBJECT(modemConfiguration, DLMS_OBJECT_TYPE_MODEM_CONFIGURATION, ln)) == 0)
    {
        modemConfiguration.communicationSpeed = DLMS_BAUD_RATE_38400;
        ARR_ATTACH(modemConfiguration.initialisationStrings, INITIALISATIONS, 1);
        SET_OCTET_STRING(INITIALISATIONS[0].request, "AT", 2);
        SET_OCTET_STRING(INITIALISATIONS[0].response, "OK", 2);
        INITIALISATIONS[0].delay = 0;
        ARR_ATTACH(modemConfiguration.modemProfile, PROFILES, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Mac Address Setup object.
///////////////////////////////////////////////////////////////////////
int addMacAddressSetup()
{
    int ret;
    static unsigned char MAC_ADDRESS[6] = { 00, 0x10,0x20,0x30,0x40,0x50 };
    const unsigned char ln[6] = { 0,0,25,2,0,255 };
    if ((ret = INIT_OBJECT(macAddressSetup, DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, ln)) == 0)
    {
        BB_ATTACH(macAddressSetup.macAddress, MAC_ADDRESS, 6);
    }
    return ret;
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
    static uint32_t MULTICAST_IP_ADDRESSES[10];
    const unsigned char ln[6] = { 0,0,25,1,0,255 };
    if ((ret = INIT_OBJECT(ip4Setup, DLMS_OBJECT_TYPE_IP4_SETUP, ln)) == 0)
    {
        ip4Setup.ipAddress = getIpAddress();
        ip4Setup.dataLinkLayer = BASE(pppSetup);
        MULTICAST_IP_ADDRESSES[0] = 0x0;
        MULTICAST_IP_ADDRESSES[1] = 0x0;
        ARR_ATTACH(ip4Setup.multicastIPAddress, MULTICAST_IP_ADDRESSES, 2);
        ip4Setup.subnetMask = 0xFFFFFFFF;
        ip4Setup.gatewayIPAddress = 0x0A000001;
        ip4Setup.primaryDNSAddress = 0x0A0B0C0D;
        ip4Setup.secondaryDNSAddress = 0x0C0D0E0F;
    }
    return ret;
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
//Add Prime NbOfdm Plc Mac Functional Parameters object.
///////////////////////////////////////////////////////////////////////
int addPrimeNbOfdmPlcMacFunctionalParameters()
{
    int ret;
    static unsigned char MAC_SNA[6] = { 0x10, 0x31, 0x46, 0x03, 0x39, 0x01 };
    const unsigned char ln[6] = { 0, 0, 28, 3, 0, 255 };
    if ((ret = INIT_OBJECT(primeNbOfdmPlcMacFunctionalParameters, DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS, ln)) == 0)
    {
        BB_ATTACH(primeNbOfdmPlcMacFunctionalParameters.sna, MAC_SNA, 6);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add PrimeNbOfdmPlcMacFunctionalParameters object.
///////////////////////////////////////////////////////////////////////
int addPrimeNbOfdmPlcMacNetworkAdministrationData()
{
    int ret;
    static gxMacPhyCommunication PHY_COMMUNICATION[2] = { 0 };
    static unsigned char EUI[6] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };

    static int16_t SWITCH_TABLE[3] = { 0 };
    SWITCH_TABLE[0] = 0x7FFF;
    SWITCH_TABLE[1] = -32767;
    const unsigned char ln[6] = { 0, 0, 28, 5, 0, 255 };
    if ((ret = INIT_OBJECT(primeNbOfdmPlcMacNetworkAdministrationData, DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA, ln)) == 0)
    {
        ARR_ATTACH(primeNbOfdmPlcMacNetworkAdministrationData.switchTable, SWITCH_TABLE, 2);
        ARR_ATTACH(primeNbOfdmPlcMacNetworkAdministrationData.communications, PHY_COMMUNICATION, 1);
        PHY_COMMUNICATION[0].txPower = 127;
        PHY_COMMUNICATION[0].txCoding = -128;
        memcpy(PHY_COMMUNICATION[0].eui, EUI, 6);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add twisted pair setup object.
///////////////////////////////////////////////////////////////////////
int addTwistedPairSetup()
{
    int ret;
    static unsigned char PRIMARY_ADDRESSES[5] = { 0 };
    static char TABIS[5] = { 0 };
    const unsigned char ln[6] = { 0, 0, 23, 0, 0, 255 };
    if ((ret = INIT_OBJECT(twistedPairSetup, DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP, ln)) == 0)
    {
        twistedPairSetup.mode = DLMS_IEC_TWISTED_PAIR_SETUP_MODE_ACTIVE;
        twistedPairSetup.speed = DLMS_BAUD_RATE_9600;
        PRIMARY_ADDRESSES[0] = 1;
        BB_ATTACH(twistedPairSetup.primaryAddresses, PRIMARY_ADDRESSES, 1);
        TABIS[0] = 1;
        BB_ATTACH(twistedPairSetup.tabis, TABIS, 1);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add push setup object. (On Connectivity)
///////////////////////////////////////////////////////////////////////
int addPushSetup()
{
    int ret;
    static gxTimePair COMMUNICATION_WINDOW[10];
    //Push objects are added here.
    static gxTarget PUSH_OBJECTS[6];
    const unsigned char ln[6] = { 0, 0, 25, 9, 0, 255 };
    if ((ret = INIT_OBJECT(pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln)) == 0)
    {
        pushSetup.service = DLMS_SERVICE_TYPE_HDLC;
        ARR_ATTACH(pushSetup.communicationWindow, COMMUNICATION_WINDOW, 2);
        //This push is sent every minute, but max 10 seconds over.
        time_init(&COMMUNICATION_WINDOW[0].first, -1, -1, -1, -1, -1, 0, 0, 0);
        time_init(&COMMUNICATION_WINDOW[0].second, -1, -1, -1, -1, -1, 10, 0, 0);
        //This push is sent every half minute, but max 40 seconds over.
        time_init(&COMMUNICATION_WINDOW[1].first, -1, -1, -1, -1, -1, 30, 0, 0);
        time_init(&COMMUNICATION_WINDOW[1].second, -1, -1, -1, -1, -1, 40, 0, 0);

        ARR_ATTACH(pushSetup.pushObjectList, PUSH_OBJECTS, 2);
        // Add logical device name.
        PUSH_OBJECTS[0].target = BASE(ldn);
        PUSH_OBJECTS[0].attributeIndex = 2;
        PUSH_OBJECTS[0].dataIndex = 0;
        // Add push object logical name. This is needed to tell structure of data to the Push listener.
        // Also capture object list can be used here.
        PUSH_OBJECTS[1].target = BASE(pushSetup);
        PUSH_OBJECTS[1].attributeIndex = 1;
        PUSH_OBJECTS[1].dataIndex = 0;
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
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
    int ret;
    const unsigned char ln[6] = { 1, 1, 21, 25, 0, 255 };
    if ((ret = INIT_OBJECT(activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        activePowerL1Value = 10;
        GX_UINT16_BYREF(activePowerL1.value, activePowerL1Value);
        //10 ^ 3 =  1000
        activePowerL1.scaler = 3;
        activePowerL1.unit = 30;
    }
    return ret;
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
    static gxScript SCRIPTS[2] = { 0 };
    static gxScriptAction ACTIONS1[2] = { 0 };
    static gxScriptAction ACTIONS2[2] = { 0 };
    const unsigned char ln[6] = { 0, 0, 10, 0, 100, 255 };
    static unsigned char MASK1[MAX_REGISTER_ACTIVATION_MASK_NAME_LENGTH];
    static unsigned char MASK2[MAX_REGISTER_ACTIVATION_MASK_NAME_LENGTH];
    if ((ret = INIT_OBJECT(tarifficationScriptTable, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        SCRIPTS[0].id = 1;
        SCRIPTS[1].id = 2;
        ARR_ATTACH(tarifficationScriptTable.scripts, SCRIPTS, 2);
        ARR_ATTACH(SCRIPTS[0].actions, ACTIONS1, 2);
        ACTIONS1[0].type = DLMS_SCRIPT_ACTION_TYPE_WRITE;
        ACTIONS1[0].target = BASE(registerActivation);
        ACTIONS1[0].index = 4;
        //Action data is register activation mask name (RATE1).
        strcat(MASK1, "RATE1");
        GX_OCTET_STRING(ACTIONS1[0].parameter, MASK1, 5);

        ACTIONS1[1].type = DLMS_SCRIPT_ACTION_TYPE_WRITE;
        ACTIONS1[1].target = BASE(currentlyActiveTariff);
        ACTIONS1[1].index = 2;
        //Action data is register activation mask name (RATE1).
        GX_OCTET_STRING(ACTIONS1[1].parameter, MASK1, strlen(MASK1));

        ARR_ATTACH(SCRIPTS[1].actions, ACTIONS2, 2);
        ACTIONS2[0].type = DLMS_SCRIPT_ACTION_TYPE_WRITE;
        ACTIONS2[0].target = BASE(registerActivation);
        ACTIONS2[0].index = 4;
        //Action data is register activation mask name (RATE2).
        strcat(MASK2, "RATE2");
        GX_OCTET_STRING(ACTIONS2[0].parameter, MASK2, 5);

        ACTIONS2[1].type = DLMS_SCRIPT_ACTION_TYPE_WRITE;
        ACTIONS2[1].target = BASE(currentlyActiveTariff);
        ACTIONS2[1].index = 2;
        //Action data is register activation mask name (RATE2).
        GX_OCTET_STRING(ACTIONS2[1].parameter, MASK2, strlen(MASK2));
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add register activation.
///////////////////////////////////////////////////////////////////////
int addRegisterActivation()
{
    int ret;
    static gxRegisterActivationMask MASK_LIST[5] = { 0 };
    static gxObject* REGISTER_ASSIGNMENT[10] = { 0 };
    static unsigned char ACTIVE_MASK[MAX_REGISTER_ACTIVATION_MASK_NAME_LENGTH] = { 0 };
    const unsigned char ln[6] = { 0, 0, 14, 0, 0, 255 };
    if ((ret = INIT_OBJECT(registerActivation, DLMS_OBJECT_TYPE_REGISTER_ACTIVATION, ln)) == 0)
    {
        BB_ATTACH(registerActivation.activeMask, ACTIVE_MASK, 0);
        bb_addString(&registerActivation.activeMask, "RATE1");
        REGISTER_ASSIGNMENT[0] = &activePowerL1.base;
        ARR_ATTACH(registerActivation.registerAssignment, REGISTER_ASSIGNMENT, 1);
        ARR_ATTACH(registerActivation.maskList, MASK_LIST, 2);
        strcpy((char*)MASK_LIST[0].name, "RATE1");
        MASK_LIST[0].length = (unsigned char)strlen(MASK_LIST[0].name);
        MASK_LIST[0].count = 2;
        MASK_LIST[0].indexes[0] = 1;
        MASK_LIST[0].indexes[1] = 2;
        strcpy((char*)MASK_LIST[1].name, "RATE2");
        MASK_LIST[1].length = (unsigned char)strlen(MASK_LIST[1].name);
        MASK_LIST[1].count = 2;
        MASK_LIST[1].indexes[0] = 1;
        MASK_LIST[1].indexes[1] = 2;
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
        SCRIPTS[0].id = 1;
        ARR_ATTACH(scriptTableGlobalMeterReset.scripts, SCRIPTS, 1);
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
    static gxScript SCRIPTS[2] = { 0 };
    static gxScriptAction ACTIONS1[1] = { 0 };
    static gxScriptAction ACTIONS2[1] = { 0 };
    const unsigned char ln[6] = { 0, 0, 10, 0, 106, 255 };
    if ((ret = INIT_OBJECT(scriptTableDisconnectControl, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        SCRIPTS[0].id = 1;
        SCRIPTS[1].id = 2;
        ARR_ATTACH(scriptTableDisconnectControl.scripts, SCRIPTS, 2);
        ARR_ATTACH(SCRIPTS[0].actions, ACTIONS1, 1);
        ACTIONS1[0].type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        ACTIONS1[0].target = BASE(disconnectControl);
        ACTIONS1[0].index = 1;
        var_init(&ACTIONS1[0].parameter);
        //Action data is Int8 zero.
        GX_INT8(ACTIONS1[0].parameter) = 0;

        ARR_ATTACH(SCRIPTS[1].actions, ACTIONS2, 1);
        ACTIONS2[0].type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        ACTIONS2[0].target = BASE(disconnectControl);
        ACTIONS2[0].index = 2;
        var_init(&ACTIONS2[0].parameter);
        //Action data is Int8 zero.
        GX_INT8(ACTIONS2[0].parameter) = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add script table object for test mode. In test mode meter is sending trace to the serial port.
///////////////////////////////////////////////////////////////////////
int addscriptTableActivateTestMode()
{
    int ret;
    static gxScript SCRIPTS[1] = { 0 };
    const unsigned char ln[6] = { 0, 0, 10, 0, 101, 255 };
    if ((ret = INIT_OBJECT(scriptTableActivateTestMode, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        SCRIPTS[0].id = 1;
        ARR_ATTACH(scriptTableActivateTestMode.scripts, SCRIPTS, 1);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add script table object for Normal mode. In normal mode meter is NOT sending trace to the serial port.
///////////////////////////////////////////////////////////////////////
int addscriptTableActivateNormalMode()
{
    int ret;
    static gxScript SCRIPTS[1] = { 0 };
    const unsigned char ln[6] = { 0, 0, 10, 0, 102, 255 };
    if ((ret = INIT_OBJECT(scriptTableActivateNormalMode, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        SCRIPTS[0].id = 1;
        ARR_ATTACH(scriptTableActivateNormalMode.scripts, SCRIPTS, 1);
    }
    return ret;
}

#define SET_CAPTURE_OBJECT(ARR, OBJECT, INDEX, DATA_INDEX) memcpy(ARR.logicalName, OBJECT.base.logicalName, 6); ARR.objectType = OBJECT.base.objectType; ARR.objectType = OBJECT.base.objectType; ARR.attributeIndex = INDEX; ARR.dataIndex = DATA_INDEX

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addLoadProfileProfileGeneric()
{
    int ret;
    static gxTarget CAPTURE_OBBJECTS[5];
    const unsigned char ln[6] = { 1, 0, 99, 1, 0, 255 };
    if ((ret = INIT_OBJECT(loadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        //We are using a ring buffer.
        loadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add 2 capture objects.
        ARR_ATTACH(loadProfile.captureObjects, CAPTURE_OBBJECTS, 2);
        //Add clock obect.
        CAPTURE_OBBJECTS[0].target = BASE(clock1);
        CAPTURE_OBBJECTS[0].attributeIndex = 2;
        CAPTURE_OBBJECTS[0].dataIndex = 0;
        //Add active power.
        CAPTURE_OBBJECTS[1].target = BASE(activePowerL1);
        CAPTURE_OBBJECTS[1].attributeIndex = 2;
        CAPTURE_OBBJECTS[1].dataIndex = 0;
        //Set clock to sort object.
        loadProfile.sortObject = BASE(clock1);
        loadProfile.sortObjectAttributeIndex = 2;
        //Count how many rows fit to the buffer.
        loadProfile.profileEntries = getProfileGenericBufferMaxRowCount(&loadProfile);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addEventLogProfileGeneric()
{
    int ret;
    static gxTarget CAPTURE_OBBJECTS[5];
    const unsigned char ln[6] = { 1, 0, 99, 98, 0, 255 };
    if ((ret = INIT_OBJECT(eventLog, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        //events are not captured.
        eventLog.capturePeriod = 0;
        //We are using a ring buffer.
        eventLog.sortMethod = DLMS_SORT_METHOD_FIFO;
        ARR_ATTACH(eventLog.captureObjects, CAPTURE_OBBJECTS, 2);
        ///////////////////////////////////////////////////////////////////
        //Add 2 columns as default.
        //Add clock obect.
        CAPTURE_OBBJECTS[0].target = BASE(clock1);
        CAPTURE_OBBJECTS[0].attributeIndex = 2;
        CAPTURE_OBBJECTS[0].dataIndex = 0;
        //Add event code.
        CAPTURE_OBBJECTS[1].target = BASE(eventCode);
        CAPTURE_OBBJECTS[1].attributeIndex = 2;
        CAPTURE_OBBJECTS[1].dataIndex = 0;
        //Set clock to sort object.
        eventLog.sortObject = BASE(clock1);
        eventLog.sortObjectAttributeIndex = 2;
        //Count how many rows fit to the buffer.
        eventLog.profileEntries = getProfileGenericBufferMaxRowCount(&eventLog);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add action schedule object for disconnect control to close the led.
///////////////////////////////////////////////////////////////////////
int addActionScheduleDisconnectClose()
{
    int ret;
    static gxtime EXECUTION_TIMES[10];
    const unsigned char ln[6] = { 0, 0, 15, 0, 1, 255 };
    if ((ret = INIT_OBJECT(actionScheduleDisconnectClose, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln)) == 0)
    {
        actionScheduleDisconnectClose.executedScript = &scriptTableDisconnectControl;
        actionScheduleDisconnectClose.executedScriptSelector = 1;
        actionScheduleDisconnectClose.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
        ARR_ATTACH(actionScheduleDisconnectClose.executionTime, EXECUTION_TIMES, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add action schedule object for disconnect control to open the led.
///////////////////////////////////////////////////////////////////////
int addActionScheduleDisconnectOpen()
{
    int ret;
    static gxtime EXECUTION_TIMES[10];
    const unsigned char ln[6] = { 0, 0, 15, 0, 3, 255 };
    //Action schedule execution times.
    if ((ret = INIT_OBJECT(actionScheduleDisconnectOpen, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln)) == 0)
    {
        actionScheduleDisconnectOpen.executedScript = &scriptTableDisconnectControl;
        actionScheduleDisconnectOpen.executedScriptSelector = 2;
        actionScheduleDisconnectOpen.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
        ARR_ATTACH(actionScheduleDisconnectOpen.executionTime, EXECUTION_TIMES, 0);
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
//Add IEC HDLC Setup object.
///////////////////////////////////////////////////////////////////////
int addIecHdlcSetup()
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
    settings.hdlc = &hdlc;
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Load security settings from the EEPROM.
/////////////////////////////////////////////////////////////////////////////
int loadSecurity()
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
        unsigned short size = (unsigned short)ftell(f);
        if (size != 0)
        {
            unsigned char tmp[100];
            fseek(f, 0L, SEEK_SET);
            gxByteBuffer bb;
            BB_ATTACH(bb, tmp, 0);
            bb.size += (unsigned short)fread(bb.data, 1, size, f);
            fclose(f);
            if ((ret = bb_get(&bb, settings.base.cipher.blockCipherKey, 16)) != 0 ||
                (ret = bb_get(&bb, settings.base.cipher.authenticationKey, 16)) != 0 ||
                (ret = bb_get(&bb, settings.base.kek, 16)) != 0 ||
                //load last server IC.
                (ret = bb_getUInt32(&bb, &settings.base.cipher.invocationCounter)) != 0 ||
                //load last client IC.
                (ret = bb_getUInt32(&bb, &securitySetupHighGMac.minimumInvocationCounter)) != 0)
            {
            }
            bb_clear(&bb);
            return ret;
        }
    }
    return saveSecurity();
}


/////////////////////////////////////////////////////////////////////////////
// Load data from the EEPROM.
// Returns serialization version or zero if data is not saved.
/////////////////////////////////////////////////////////////////////////////
int loadSettings()
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
        uint16_t size = (uint16_t)ftell(f);
        if (size != 0)
        {
            unsigned char tmp[2048];
            fseek(f, 0L, SEEK_SET);
            gxByteBuffer bb;
            BB_ATTACH(bb, tmp, 0);
            bb.size += (uint16_t)fread(bb.data, 1, size, f);
            fclose(f);
            gxSerializerSettings serializerSettings;
            serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
            serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
            ret = ser_loadObjects(&settings.base, &serializerSettings, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]), &bb);
            {
            }
            bb_clear(&bb);
            return ret;
        }
        fclose(f);
    }
    return saveSettings();
}

//Create objects and load values from EEPROM.
int createObjects()
{
    int ret;
    OA_ATTACH(settings.base.objects, ALL_OBJECTS);
    if ((ret = addLogicalDeviceName()) != 0 ||
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
        //Register activation must add before tariffication script table because TarifficationScriptTable is using Register activation.
        (ret = addRegisterActivation()) != 0 ||
        (ret = addTarifficationScriptTable()) != 0 ||
        (ret = addLoadProfileProfileGeneric()) != 0 ||
        (ret = addEventLogProfileGeneric()) != 0 ||
        (ret = addActionScheduleDisconnectOpen()) != 0 ||
        (ret = addActionScheduleDisconnectClose()) != 0 ||
        (ret = addDisconnectControl()) != 0 ||
        (ret = addIecHdlcSetup()) != 0 ||
        (ret = addImageTransfer()) != 0 ||
        (ret = addTcpUdpSetup()) != 0 ||
        (ret = addAutoConnect()) != 0 ||
        (ret = addActivityCalendar()) != 0 ||
        (ret = addSpecialDaysTable()) != 0 ||
        (ret = addCurrentlyActiveTariff()) != 0 ||
        (ret = addOpticalPortSetup()) != 0 ||
        (ret = addDemandRegister()) != 0 ||
        (ret = addRegisterMonitor()) != 0 ||
        (ret = addAutoAnswer()) != 0 ||
        (ret = addModemConfiguration()) != 0 ||
        (ret = addMacAddressSetup()) != 0 ||
        (ret = addIP4Setup()) != 0 ||
        (ret = addPppSetup()) != 0 ||
        (ret = addGprsSetup()) != 0 ||
        (ret = addPrimeNbOfdmPlcMacFunctionalParameters()) != 0 ||
        (ret = addPrimeNbOfdmPlcMacNetworkAdministrationData()) != 0 ||
        (ret = addTwistedPairSetup()) != 0 ||
        (ret = oa_verify(&settings.base.objects)) != 0 ||
        (ret = svr_initialize(&settings)) != 0)
    {
        GXTRACE_INT(("Failed to start the meter!"), ret);
        executeTime = 0;
        return ret;
    }
    if ((ret = loadSettings()) != 0)
    {
        GXTRACE_INT(("Failed to load settings!"), ret);
        executeTime = 0;
        return ret;
    }
    if ((ret = loadSecurity()) != 0)
    {
        GXTRACE_INT(("Failed to load security settings!"), ret);
        executeTime = 0;
        return ret;
    }
    updateState(GURUX_EVENT_CODES_POWER_UP);
    GXTRACE(("Meter started."), NULL);
    return 0;
}

//Get HDLC communication speed.
int32_t getCommunicationSpeed()
{
    int32_t ret = 9600;
    switch (hdlc.communicationSpeed)
    {
    case DLMS_BAUD_RATE_300:
        ret = 300;
        break;
    case DLMS_BAUD_RATE_600:
        ret = 600;
        break;
    case DLMS_BAUD_RATE_1200:
        ret = 1200;
        break;
    case DLMS_BAUD_RATE_2400:
        ret = 2400;
        break;
    case DLMS_BAUD_RATE_4800:
        ret = 4800;
        break;
    case DLMS_BAUD_RATE_9600:
        ret = 9600;
        break;
    case DLMS_BAUD_RATE_19200:
        ret = 19200;
        break;
    case DLMS_BAUD_RATE_38400:
        ret = 38400;
        break;
    case DLMS_BAUD_RATE_57600:
        ret = 57600;
        break;
    case DLMS_BAUD_RATE_115200:
        ret = 115200;
        break;
    }
    return ret;
}

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
                        a->authenticationMechanismName.mechanismId == settings->authentication)
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
    unsigned char ln[6];
    uint16_t ot;
    signed char aIndex;
    uint16_t dIndex;
    if ((ret = cosem_checkStructure(e->parameters.byteArr, 4)) == 0 &&
        (ret = cosem_checkStructure(e->parameters.byteArr, 4)) == 0 &&
        (ret = cosem_getUInt16(e->parameters.byteArr, &ot)) == 0 &&
        (ret = cosem_getOctetString2(e->parameters.byteArr, ln, 6, NULL)) == 0 &&
        (ret = cosem_getInt8(e->parameters.byteArr, &aIndex)) == 0 &&
        (ret = cosem_getUInt16(e->parameters.byteArr, &dIndex)) == 0)
    {
        uint16_t pos;
        gxTarget* it;
        for (pos = 0; pos != ((gxProfileGeneric*)e->target)->captureObjects.size; ++pos)
        {
            if ((ret = arr_getByIndex(&((gxProfileGeneric*)e->target)->captureObjects, pos, (void**)&it, sizeof(gxTarget))) != 0)
            {
                break;
            }
            if (it->target->objectType == ot && memcmp(it->target->logicalName, ln, 6) == 0)
            {
                *obj = it->target;
                break;
            }
        }
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
    int ret = 0;
    gxtime start;
    gxtime end;
    uint32_t pos;
    uint32_t last = 0;
    gxObject* obj = NULL;
    short index;
    if ((ret = getRestrictingObject(settings, e, &obj, &index)) != 0)
    {
        return ret;
    }
    //Get all data if there are no sort object.
    uint32_t s = 0;
    uint32_t l = 0xFFFFFFFF;
    //Check sort object
    if (e->parameters.byteArr->position < e->parameters.byteArr->size)
    {
        if (e->parameters.byteArr->data[e->parameters.byteArr->position] == DLMS_DATA_TYPE_OCTET_STRING)
        {
            if ((ret = cosem_getDateTimeFromOctetString(e->parameters.byteArr, &start)) != 0 ||
                (ret = cosem_getDateTimeFromOctetString(e->parameters.byteArr, &end)) != 0)
            {
                return ret;
            }
            s = time_toUnixTime2(&start);
            l = time_toUnixTime2(&end);
        }
        else if (e->parameters.byteArr->data[e->parameters.byteArr->position] == DLMS_DATA_TYPE_UINT32)
        {
            if ((ret = cosem_getUInt32(e->parameters.byteArr, &s)) != 0 ||
                (ret = cosem_getUInt32(e->parameters.byteArr, &l)) != 0)
            {
                return ret;
            }
        }
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
            getProfileGenericBufferColumnSizes(pg, dataTypes, columnSizes, &rowSize);
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
    gxTarget CAPTURE_OBJECT[10] = { 0 };
    ARR_ATTACH(captureObjects, CAPTURE_OBJECT, 0);
    char fileName[20];
    getProfileGenericFileName(pg, fileName);
    if (ret == DLMS_ERROR_CODE_OK)
    {
        e->byteArray = 1;
        e->handled = 1;
        // If reading first time.
        if (first)
        {
            uint16_t offset = getProfileGenericBufferEntriesInUse(pg);
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
                if ((ret = cosem_checkStructure(e->parameters.byteArr, 4)) != 0 ||
                    (ret = cosem_getUInt32(e->parameters.byteArr, &e->transactionStartIndex)) != 0 ||
                    (ret = cosem_getUInt32(e->parameters.byteArr, &e->transactionEndIndex)) != 0 ||
                    (ret = cosem_getColumns(&pg->captureObjects, e->selector, &e->parameters, &captureObjects)) != 0)
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
                    //Get index in ring buffer.
                }
            }
        }
        bb_clear(e->value.byteArr);
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
                getProfileGenericBufferColumnSizes(pg, dataTypes, columnSizes, &dataSize);
            }
            //Append data.
            if (ret == 0 && dataSize != 0)
            {
                if (fseek(f, 4 + ((e->transactionStartIndex - 1) * dataSize), SEEK_SET) != 0)
                {
                    printf("Failed to seek %s.\r\n", fileName);
                    return -1;
                }
                for (pos = e->transactionStartIndex - 1; pos != e->transactionEndIndex; ++pos)
                {
                    pduSize = e->value.byteArr->size;
                    if ((ret = cosem_setStructure(e->value.byteArr, pg->captureObjects.size)) != 0)
                    {
                        break;
                    }
                    uint8_t colIndex;
                    gxTarget* it;
                    //Loop capture columns and get values.
                    for (colIndex = 0; colIndex != pg->captureObjects.size; ++colIndex)
                    {
                        if ((ret = arr_getByIndex(&pg->captureObjects, colIndex, (void**)&it, sizeof(gxTarget))) == 0)
                        {
                            //Date time is saved in EPOCH to save space.
                            if ((it->target->objectType == DLMS_OBJECT_TYPE_CLOCK || it->target == BASE(unixTime)) && it->attributeIndex == 2)
                            {
                                uint32_t time;
                                fread(&time, 4, 1, f);
                                time_initUnix(&tm, time);
                                //Convert to meter time if UNIX time is not used.
                                if (it->target != BASE(unixTime))
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
                            break;
                        }
                        //If DPU is full.
                        if (e->value.byteArr->size > settings->maxPduSize)
                        {
                            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                            break;
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
                        break;
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
            e->error = cosem_setDateTimeAsOctetString(e->value.byteArr, &dt);
            e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
            e->handled = 1;
        }
        else if (e->target == BASE(loadProfile) && e->index == 2)
        {
            e->error = (DLMS_ERROR_CODE)readProfileGeneric(settings, &loadProfile, e);
        }
        else if (e->target == BASE(eventLog) && e->index == 2)
        {
            e->error = (DLMS_ERROR_CODE)readProfileGeneric(settings, &eventLog, e);
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
            updateState(GURUX_EVENT_CODES_TIME_CHANGE);
        }
        hlp_getLogicalNameToString(e->target->logicalName, str);
        printf("Writing %s\r\n", str);

    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
}

int sendPush(
    dlmsSettings* settings,
    gxPushSetup* push)
{
    GXTRACE_LN(("sendPush"), push->base.objectType, push->base.logicalName);
    int ret = 0, pos;
    if (socket1 != -1)
    {
        message messages;
        if (push->pushObjectList.size == 0)
        {
            GXTRACE(("sendPush Failed. No objects selected."), NULL);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        unsigned char pduBuff[PDU_BUFFER_SIZE];
        gxByteBuffer pdu;
        bb_attach(&pdu, pduBuff, 0, sizeof(pduBuff));
        settings->serializedPdu = &pdu;
        unsigned char data[PDU_BUFFER_SIZE];
        gxByteBuffer bb;
        bb_attach(&bb, data, 0, sizeof(data));
        gxByteBuffer* tmp[] = { &bb };
        mes_attach(&messages, tmp, 1);
        if ((ret = notify_generatePushSetupMessages(settings, 0, push, &messages)) == 0)
        {
            for (pos = 0; pos != messages.size; ++pos)
            {
                if (send(socket1, (const char*)messages.data[pos]->data, bb_available(messages.data[pos]), 0) == -1)
                {
                    break;
                }
                bb_clear(&reply);
            }
        }
        if (ret != 0)
        {
            GXTRACE(("generatePushSetupMessages Failed."), NULL);
        }
        mes_clear(&messages);
    }
    return ret;
}

int sendEventNotification(dlmsSettings* settings)
{
    GXTRACE_LN(("sendEventNotification"), eventLog.base.objectType, eventLog.base.logicalName);
    int ret = 0, pos;
    if (socket1 != -1)
    {
        gxListItem item;
        item.key = BASE(eventLog);
        item.value = 2;
        message messages;
        unsigned char pduBuff[PDU_BUFFER_SIZE];
        gxByteBuffer pdu;
        bb_attach(&pdu, pduBuff, 0, sizeof(pduBuff));
        settings->serializedPdu = &pdu;
        unsigned char data[PDU_BUFFER_SIZE];
        gxByteBuffer bb;
        bb_attach(&bb, data, 0, sizeof(data));
        gxByteBuffer* tmp[] = { &bb };
        mes_attach(&messages, tmp, 1);
        variantArray values;
        dlmsVARIANT valuesBuff[2];
        gxtime dt;
        time_now(&dt, 1);
        GX_DATETIME(valuesBuff[0]) = &dt;
        GX_UINT16(valuesBuff[1]) = activePowerL1Value;
        VA_ATTACH(values, valuesBuff, 2);
        if ((ret = notify_generateEventNotificationMessages(settings, 0, &item, &values, &pdu, &messages)) == 0)
        {
            for (pos = 0; pos != messages.size; ++pos)
            {
                if (send(socket1, (const char*)messages.data[pos]->data, bb_available(messages.data[pos]), 0) == -1)
                {
                    break;
                }
                bb_clear(&reply);
            }
        }
        if (ret != 0)
        {
            GXTRACE(("generatePushSetupMessages Failed."), NULL);
        }
        mes_clear(&messages);
    }
    return ret;
}

//This example sends data in the structure.
int sendEventNotification2(dlmsSettings* settings)
{
    GXTRACE_LN(("sendEventNotification2"), eventLog.base.objectType, eventLog.base.logicalName);
    int ret = 0, pos;
    if (socket1 != -1)
    {
        gxListItem item;
        item.key = BASE(eventLog);
        item.value = 2;
        message messages;
        unsigned char pduBuff[PDU_BUFFER_SIZE];
        gxByteBuffer pdu;
        bb_attach(&pdu, pduBuff, 0, sizeof(pduBuff));
        settings->serializedPdu = &pdu;
        unsigned char messageBuffer[PDU_BUFFER_SIZE];
        gxByteBuffer message;
        bb_attach(&message, messageBuffer, 0, sizeof(messageBuffer));
        gxByteBuffer* tmp[] = { &message };
        mes_attach(&messages, tmp, 1);
        unsigned char dataBuff[PDU_BUFFER_SIZE];
        gxByteBuffer data;
        bb_attach(&data, dataBuff, 0, sizeof(dataBuff));
        gxtime dt;
        time_now(&dt, 1);
        //Data is send in structure. Amount of the items in structure is 2.
        cosem_setStructure(&data, 2);
        cosem_setDateAsOctetString(&data, &dt);
        cosem_setUInt16(&data, activePowerL1Value);
        if ((ret = notify_generateEventNotificationMessages2(settings, 0, &item, &data, &pdu, &messages)) == 0)
        {
            for (pos = 0; pos != messages.size; ++pos)
            {
                if (send(socket1, (const char*)messages.data[pos]->data, bb_available(messages.data[pos]), 0) == -1)
                {
                    break;
                }
                bb_clear(&reply);
            }
        }
        if (ret != 0)
        {
            GXTRACE(("generatePushSetupMessages Failed."), NULL);
        }
        mes_clear(&messages);
    }
    return ret;
}

void handleProfileGenericActions(
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
            unsigned char tmp[4];
            gxByteBuffer pdu;
            BB_ATTACH(pdu, tmp, 0);
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
        captureProfileGeneric(((gxProfileGeneric*)it->target));
    }
    saveSettings();
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
#if defined(_WIN64)
    const char* fileName = "settings64.raw";
#else // defined(_WIN32) || defined(__linux__)
    const char* fileName = "settings.raw";
#endif //defined(_WIN32) || defined(__linux__)
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        GXTRACE_LN(("svr_preAction: "), e->target->objectType, e->target->logicalName);
        if (e->target == BASE(loadProfile))
        {
            handleProfileGenericActions(e);
            e->handled = 1;
        }
        else if (e->target == BASE(eventLog))
        {
            handleProfileGenericActions(e);
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
            updateState(GURUX_EVENT_CODES_PUSH);
            sendPush(settings, (gxPushSetup*)e->target);
            e->handled = 1;
        }
        //If client wants to clear EEPROM data using Global meter reset script.
        else if (e->target == BASE(scriptTableGlobalMeterReset) && e->index == 1)
        {
            //Initialize data size so default values are used on next connection.
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
            if ((ret = loadSettings()) != 0)
            {
                GXTRACE_INT(("Failed to load settings!"), ret);
                executeTime = 0;
                e->error = ret;
                break;
            }
            if ((ret = loadSecurity()) != 0)
            {
                GXTRACE_INT(("Failed to load security settings!"), ret);
                executeTime = 0;
                e->error = ret;
                break;
            }
            updateState(GURUX_EVENT_CODES_GLOBAL_METER_RESET);
            e->handled = 1;
        }
        else if (e->target == BASE(disconnectControl))
        {
            updateState(GURUX_EVENT_CODES_OUTPUT_RELAY_STATE);
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
        }
        else if (e->target == BASE(scriptTableActivateNormalMode))
        {
            //Activate normal mode.
            testMode = 0;
        }
        else if (e->target == BASE(tarifficationScriptTable))
        {
            printf("Tariffication script table invoked with selector %d\n.", e->index);
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
                if ((e->error = arr_getByIndex(&imageTransfer.imageActivateInfo, 0, (void**)&info, sizeof(gxImageActivateInfo))) != 0)
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    return;
                }
                if ((ret = cosem_checkStructure(e->parameters.byteArr, 2)) != 0 ||
                    (ret = cosem_getOctetString2(e->parameters.byteArr, info->identification.data, sizeof(info->identification.data), &info->identification.size)) != 0 ||
                    (ret = cosem_getUInt32(e->parameters.byteArr, &info->size)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    return;
                }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                printf("Updating image %s Size: %d\r\n", imageFile, info->size);
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
                handleProfileGenericActions(&it);
                //Count how many rows fit to the buffer.
                ((gxProfileGeneric*)e->target)->profileEntries = getProfileGenericBufferMaxRowCount(((gxProfileGeneric*)e->target));
                if (((gxProfileGeneric*)e->target)->captureObjects.size != 0)
                {
                    gxTarget* k = NULL;
                    arr_getByIndex(&((gxProfileGeneric*)e->target)->captureObjects, 0, (void**)&k, sizeof(gxTarget));
                    //Set 1st object to sort object.
                    ((gxProfileGeneric*)e->target)->sortObject = ((gxTarget*)k)->target;
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
                uint16_t maxCount = getProfileGenericBufferMaxRowCount(((gxProfileGeneric*)e->target));
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
            loadSettings();
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
                saveSecurity();
            }
            else
            {
                //Load default settings if there is an error.
                loadSecurity();
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
                loadSettings();
            }
        }
    }
}

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
                    settings->expectedClientSystemTitle = NULL;
                    //Set Invocation counter value.
                    settings->expectedInvocationCounter = NULL;
                    //Client can establish a ciphered connection only with Security Suite 1.
                    settings->expectedSecuritySuite = 0;
                    //Security policy is not defined by default. Client can connect using any security policy.
                    //settings->expectedSecurityPolicy = 0xFF;
                    //Client can't establish ciphered connection if there is no security setup.
                    settings->expectedSecurityPolicy = 0;
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
                            if (arr_getByIndex(&sap->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem)) == 0)
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

//Get attribute access level for register activation.
DLMS_ACCESS_MODE getRegisterActivationAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    //Register activation assigments and mask lit items are added with actions.
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
    if (obj->objectType == DLMS_OBJECT_TYPE_REGISTER_ACTIVATION)
    {
        return getRegisterActivationAttributeAccess(settings, index);
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
    if (settings->base.connected == DLMS_CONNECTION_STATE_HDLC)
    {
        GXTRACE(("svr_connected to HDLC level."), NULL);
    }
    else if ((settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
    {
        GXTRACE(("svr_connected DLMS level."), NULL);
    }
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
    GXTRACE(("svr_invalidConnection"), NULL);
    updateState(GURUX_EVENT_CODES_WRONG_PASSWORD);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Client has close the connection.
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
    dlmsServerSettings* settings)
{
    GXTRACE(("svr_disconnected"), NULL);
    if (settings->base.cipher.security != 0 && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
    {
        //Save Invocation counter value when connection is closed.
        saveSecurity();
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

#if defined(_WIN32) || defined(_WIN64)//If Windows
void serialPortThread(void* pVoid)
{
    int ret;
    unsigned char data;
    DWORD bytesHandled = 0;
    HANDLE comPort = *((HANDLE*)pVoid);
    OVERLAPPED		osWrite;
    OVERLAPPED		osReader;
    memset(&osReader, 0, sizeof(OVERLAPPED));
    memset(&osWrite, 0, sizeof(OVERLAPPED));
    osReader.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
    osWrite.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
    unsigned char first = 1;
    while (1)
    {
        if (!ReadFile(comPort, &data, 1, &bytesHandled, &osReader))
        {
            ret = GetLastError();
            if (ret != ERROR_IO_PENDING)
            {
                break;
            }
            //Wait until data is actually read
            if (WaitForSingleObject(osReader.hEvent, -1) != WAIT_OBJECT_0)
            {
                break;
            }
            if (!GetOverlappedResult(comPort, &osReader, &bytesHandled, TRUE))
            {
                break;
            }
        }
        if (trace > GX_TRACE_LEVEL_WARNING)
        {
            if (first)
            {
                printf("\nRX:\t");
                first = 0;
            }
            printf("%.2X ", data);
        }
        if (svr_handleRequest3(&settings, data, &reply) != 0)
        {
            first = 1;
            break;
        }
        if (reply.size != 0)
        {
            first = 1;
            if (trace > GX_TRACE_LEVEL_WARNING)
            {
                int pos;
                printf("\nTX\t");
                for (pos = 0; pos != reply.size; ++pos)
                {
                    printf("%.2X ", reply.data[pos]);
                }
                printf("\n");
            }
            if ((ret = WriteFile(comPort, reply.data, reply.size, &bytesHandled, &osWrite)) == 0)
            {
                int err = GetLastError();
                //If error occurs...
                if (err != ERROR_IO_PENDING)
                {
                    printf("WriteFile %d\r\n", err);
                    break;
                }
                //Wait until data is actually sent
                WaitForSingleObject(osWrite.hEvent, 5000);
            }
        }
    }
    CloseHandle(osReader.hEvent);
    CloseHandle(osWrite.hEvent);
}
#endif

void ListenerThread(void* pVoid)
{
#if defined(_WIN32) || defined(_WIN64)//If Windows
    int len;
#else //If Linux
    socklen_t len;
#endif
    unsigned char data;
    int ret;
    int ls = *((int*)pVoid);
    struct sockaddr_in client;
    while (1)
    {
        if ((ret = listen(ls, 1)) == -1)
        {
            //socket listen failed.
            return;
        }
        len = sizeof(client);
        uint16_t pos;
        unsigned char first = 1;
        socket1 = accept(ls, (struct sockaddr*)&client, &len);
        while (1)
        {
            //Read one char at the time.
            if ((ret = recv(socket1, (char*)&data, 1, 0)) == -1)
            {
#if defined(_WIN32) || defined(_WIN64)//If Windows
                closesocket(socket1);
                socket1 = INVALID_SOCKET;
#else //If Linux
                close(socket1);
                socket1 = -1;
#endif
                break;
            }
            //If client closes the connection.
            if (ret == 0)
            {
#if defined(_WIN32) || defined(_WIN64)//If Windows
                closesocket(socket1);
                socket1 = INVALID_SOCKET;
#else //If Linux
                close(socket1);
                socket1 = -1;
#endif
                break;
            }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            if (trace > GX_TRACE_LEVEL_WARNING)
            {
                if (first)
                {
                    printf("\nRX:\t");
                    first = 0;
                }
                printf("%.2X ", data);
            }
#endif //OS
            if (svr_handleRequest3(&settings, data, &reply) != 0)
            {
#if defined(_WIN32) || defined(_WIN64)//If Windows
                closesocket(socket1);
                socket1 = INVALID_SOCKET;
#else //If Linux
                close(socket1);
                socket1 = -1;
#endif
                break;
            }
            if (reply.size != 0)
            {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
                if (trace > GX_TRACE_LEVEL_WARNING)
                {
                    first = 1;
                    printf("\nTX %u:\t", (uint16_t)reply.size);
                    for (pos = 0; pos != reply.size; ++pos)
                    {
                        printf("%.2X ", reply.data[pos]);
                    }
                    printf("\n");
                }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
                if (send(socket1, (const char*)reply.data, reply.size - reply.position, 0) == -1)
                {
#if defined(_WIN32) || defined(_WIN64)//If Windows
                    closesocket(socket1);
                    socket1 = INVALID_SOCKET;
#else //If Linux
                    close(socket1);
                    socket1 = -1;
#endif
                    break;
                }
                bb_clear(&reply);
            }
        }
    }
}

//If Linux
#if defined(__linux__)
void* UnixSerialPortThread(void* pVoid)
{
    int ret;
    int comPort = *((int*)pVoid);
    unsigned char data;
    unsigned char first = 1;
    uint16_t pos;
    int bytesRead;
    while (1)
    {
        bytesRead = read(comPort, &data, 1);
        if (bytesRead < 1)
        {
            //If there is no data on the read buffer.
            if (errno != EAGAIN)
            {
                break;
            }
        }
        else
        {
            if (trace > GX_TRACE_LEVEL_WARNING)
            {
                if (first)
                {
                    printf("\nRX:\t");
                    first = 0;
                }
                printf("%.2X ", data);
            }
            if (svr_handleRequest3(&settings, data, &reply) != 0)
            {
                break;
            }
            if (reply.size != 0)
            {
                first = 1;
                if (trace > GX_TRACE_LEVEL_WARNING)
                {
                    printf("\nTX\t");
                    for (pos = 0; pos != reply.size; ++pos)
                    {
                        printf("%.2X ", reply.data[pos]);
                    }
                    printf("\n");
                }
                ret = write(comPort, reply.data, reply.size);
                if (ret != reply.size)
                {
                    printf("Write failed\n");
                }
            }
        }
    }
    return NULL;
}

void* UnixListenerThread(void* pVoid)
{
    ListenerThread(pVoid);
    return NULL;
}

char _getch()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    if (FD_ISSET(0, &fds))
    {
        return getchar();
    }
    return 0;
}
#endif

void showHelp()
{
    printf("Gurux DLMS example Server implements four DLMS/COSEM devices.\n");
    printf(" -t\t [Error, Warning, Info, Verbose] Trace messages.\n");
    printf(" -p\t Start port number. Default is 4060.\n");
    printf(" -S\t serial port.\n");
}

void println(char* desc, unsigned char* data, char size)
{
    if (data != NULL)
    {
        char str[100];
        hlp_bytesToHex2(data, size, str, sizeof(str));
        printf("%s: %s\r\n", desc, str);
    }
}

#if defined(_WIN32) || defined(_WIN64)
int com_setCommState(HANDLE hWnd, LPDCB DCB)
{
    if (!SetCommState(hWnd, DCB))
    {
        DWORD err = GetLastError(); //Save occured error.
        if (err == 995)
        {
            COMSTAT comstat;
            unsigned long RecieveErrors;
            if (!ClearCommError(hWnd, &RecieveErrors, &comstat))
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | GetLastError();
            }
            if (!SetCommState(hWnd, DCB))
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | GetLastError();
            }
        }
        else
        {
            //If USB to serial port converters do not implement this.
            if (err != ERROR_INVALID_FUNCTION)
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | err;
            }
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int com_initializeSerialPort(
    HANDLE* comPort,
    char* serialPort,
    unsigned char iec)
{
    int ret = 0;
    char buff[10];
#if _MSC_VER > 1000
    sprintf_s(buff, 10, "\\\\.\\%s", serialPort);
#else
    sprintf(buff, "\\\\.\\%s", serialPort);
#endif
    //Open serial port for read / write. Port can't share.
    * comPort = CreateFileA(buff,
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (*comPort == INVALID_HANDLE_VALUE)
    {
        ret = GetLastError();
        printf("Failed to open serial port: \"%s\"\n", buff);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    DCB dcb = { 0 };
    unsigned long sendSize = 0;
    if (*comPort == INVALID_HANDLE_VALUE)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(*comPort, &dcb))
    {
        ret = GetLastError();
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    dcb.fBinary = 1;
    dcb.fOutX = dcb.fInX = 0;
    //Abort all reads and writes on Error.
    dcb.fAbortOnError = 1;
    if (iec)
    {
        dcb.BaudRate = 300;
        dcb.ByteSize = 7;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = EVENPARITY;
    }
    else
    {
        dcb.BaudRate = 9600;
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = NOPARITY;
    }
    if ((ret = com_setCommState(*comPort, &dcb)) != 0)
    {
        return ret;
    }
    return 0;
}
#else //#if defined(__LINUX__)
int com_initializeSerialPort(
    int* comPort,
    char* serialPort,
    unsigned char iec)
{
    int ret;
    // read/write | not controlling term | don't wait for DCD line signal.
    *comPort = open(serialPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (*comPort == -1) // if open is unsuccessful.
    {
        ret = errno;
        printf("Failed to open serial port: %s\n", serialPort);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    if (!isatty(*comPort))
    {
        ret = errno;
        printf("Failed to Open port %s. This is not a serial port.\n", serialPort);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    struct termios options;
    memset(&options, 0, sizeof(options));
    options.c_iflag = 0;
    options.c_oflag = 0;
    if (iec)
    {
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS7;
        //Set Baud Rates
        cfsetospeed(&options, B300);
        cfsetispeed(&options, B300);
    }
    else
    {
        // 8n1, see termios.h for more information
        options.c_cflag = CS8 | CREAD | CLOCAL;
        /*
        options.c_cflag &= ~PARENB
        options.c_cflag &= ~CSTOPB
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        */
        //Set Baud Rates
        cfsetospeed(&options, B9600);
        cfsetispeed(&options, B9600);
    }
    options.c_lflag = 0;
    options.c_cc[VMIN] = 1;
    //How long we are waiting reply charachter from serial port.
    options.c_cc[VTIME] = 5;

    //hardware flow control is used as default.
    //options.c_cflag |= CRTSCTS;
    if (tcsetattr(*comPort, TCSAFLUSH, &options) != 0)
    {
        ret = errno;
        printf("Failed to Open port. tcsetattr failed.\r");
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    return 0;
}
#endif

#if defined(_WIN32) || defined(_WIN64)//Windows includes
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    int opt, port = 4061;
#if defined(_WIN32) || defined(_WIN64)//If Windows
    //Receiver thread handle.
    HANDLE receiverThread;
#else //If Linux.
    //Receiver thread handle.
    pthread_t receiverThread;
#endif

    //Serial port handlers.
#if defined(_WIN32) || defined(_WIN64)// If Windows
    HANDLE comPort = INVALID_HANDLE_VALUE;
#else //If Linux
    int comPort = -1;
#endif

    int ret, ls = 0;
    struct sockaddr_in add = { 0 };
    char* serialPort = NULL;
    while ((opt = getopt(argc, argv, "t:p:S:")) != -1)
    {
        switch (opt)
        {
        case 't':
            //Trace.
            if (strcmp("Error", optarg) == 0)
                trace = GX_TRACE_LEVEL_ERROR;
            else  if (strcmp("Warning", optarg) == 0)
                trace = GX_TRACE_LEVEL_WARNING;
            else  if (strcmp("Info", optarg) == 0)
                trace = GX_TRACE_LEVEL_INFO;
            else  if (strcmp("Verbose", optarg) == 0)
                trace = GX_TRACE_LEVEL_VERBOSE;
            else  if (strcmp("Off", optarg) == 0)
                trace = GX_TRACE_LEVEL_OFF;
            else
            {
                printf("Invalid trace option '%s'. (Error, Warning, Info, Verbose, Off)", optarg);
                return 1;
            }
            break;
        case 'p':
            //Port.
            port = atoi(optarg);
            break;
        case 'S':
            serialPort = optarg;
            break;
        case '?':
        {
            if (optarg[0] == 'p')
            {
                printf("Missing mandatory port option.\n");
            }
            else if (optarg[0] == 't')
            {
                printf("Missing mandatory trace option.\n");
            }
            else if (optarg[0] == 'S')
            {
                printf("Missing mandatory Serial port option.\n");
            }
            else
            {
                showHelp();
                return 1;
            }
        }
        break;
        default:
            showHelp();
            return 1;
        }
    }
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 1;
    }
#endif
    //Add FLAG ID.
    memcpy(SERVER_SYSTEM_TITLE, FLAG_ID, 3);
    //ADD serial number.
    memcpy(SERVER_SYSTEM_TITLE + 4, &SERIAL_NUMBER, 4);

    bb_attach(&reply, replyFrame, 0, sizeof(replyFrame));
    //Start server using logical name referencing and HDLC framing.
    svr_init(&settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, frameBuff, sizeof(frameBuff), pduBuff, sizeof(pduBuff));
    //Allocate space for read list.
    vec_attach(&settings.transaction.targets, events, 0, sizeof(events) / sizeof(events[0]));
    //Allocate space for client password.
    BB_ATTACH(settings.base.password, PASSWORD, 0);
    //Allocate space for client challenge.
    BB_ATTACH(settings.base.ctoSChallenge, C2S_CHALLENGE, 0);
    //Allocate space for server challenge.
    BB_ATTACH(settings.base.stoCChallenge, S2C_CHALLENGE, 0);
    //Set master key (KEK) to 1111111111111111.
    unsigned char KEK[16] = { 0x31,0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31 };
    memcpy(settings.base.kek, KEK, sizeof(KEK));
    //Add COSEM objects.
    if ((ret = createObjects()) != 0)
    {
        return ret;
    }

    //Set default clock.
    settings.defaultClock = &clock1;
    if (serialPort != NULL)
    {
        printf("Serial port server started in port: %s\n", serialPort);
        if ((ret = com_initializeSerialPort(
            &comPort,
            serialPort,
            0)) != 0)
        {
            return ret;
        }
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        receiverThread = (HANDLE)_beginthread(serialPortThread, 0, &comPort);
#else
        ret = pthread_create(&receiverThread, NULL, UnixSerialPortThread, &comPort);
#endif

    }
    else
    {
        printf("TCP/IP server started in port: %d\n", port);
        ls = socket(AF_INET, SOCK_STREAM, 0);
        add.sin_port = htons(port);
        add.sin_addr.s_addr = htonl(INADDR_ANY);
        add.sin_family = AF_INET;
        if ((ret = bind(ls, (struct sockaddr*)&add, sizeof(add))) == -1)
        {
#if defined(_WIN32) || defined(_WIN64)//Windows
            ret = GetLastError();
#else
            ret = errno;
#endif
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        }
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        receiverThread = (HANDLE)_beginthread(ListenerThread, 0, &ls);
#else
        ret = pthread_create(&receiverThread, NULL, UnixListenerThread, &ls);
#endif
    }
    printf("----------------------------------------------------------\n");
    printf("Authentication levels:\n");
    printf("None: Client address 16 (0x10)\n");
    printf("Low: Client address 17 (0x11)\n");
    printf("High: Client address 18 (0x12)\n");
    printf("HighGMac: Client address 1 (1)\n");
    printf("----------------------------------------------------------\n");
    println("System Title", settings.base.cipher.systemTitle, sizeof(settings.base.cipher.systemTitle));
    println("Authentication key", settings.base.cipher.authenticationKey, sizeof(settings.base.cipher.authenticationKey));
    println("Block cipher key", settings.base.cipher.blockCipherKey, sizeof(settings.base.cipher.blockCipherKey));
    println("Client System title", settings.base.preEstablishedSystemTitle, sizeof(settings.base.preEstablishedSystemTitle));
    println("Master key (KEK)", settings.base.kek, sizeof(settings.base.kek));
    printf("----------------------------------------------------------\n");
    printf("Press Enter to close application.\r\n");
    while (1)
    {
        uint32_t start = time_current();
        if (executeTime <= start)
        {
            svr_run(&settings, start, &executeTime);
            if (executeTime != -1)
            {
                time_t tmp = start;
                printf("%s", ctime(&tmp));
                tmp = executeTime;
                printf("%lu seconds before next invoke %s", executeTime - start, ctime(&tmp));
            }
        }
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        if (_kbhit()) {
            char c = _getch();
            if (c == '\r')
            {
                printf("Closing the server.\n");
                if (comPort != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(comPort);
                }
                else
                {
                    closesocket(ls);
                    WaitForSingleObject(receiverThread, 5000);
                }
                break;
            }
        }
        Sleep(1000);
#else
        char ch = _getch();
        if (ch == '\n')
        {
            printf("Closing the server.\n");
            if (comPort != -1)
            {
                close(comPort);
                void* res;
                pthread_join(receiverThread, (void**)&res);
                free(res);
            }
            else
            {
                shutdown(ls, SHUT_RDWR);
                closesocket(ls);
                void* res;
                pthread_join(receiverThread, (void**)&res);
                free(res);
            }
            break;
        }
        usleep(1000000);
#endif
    }

#if defined(_WIN32) || defined(_WIN64)//Windows
    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif

    return 0;
    }

