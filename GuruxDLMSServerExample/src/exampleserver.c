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
//Buffer where frames are saved.
static unsigned char frameBuff[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
//Buffer where PDUs are saved.
static unsigned char pduBuff[PDU_BUFFER_SIZE];
static unsigned char replyFrame[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
//Define server system title.
static unsigned char SERVER_SYSTEM_TITLE[8] = { 0x47, 0x52, 0x58, 0x00, 0x00, 0x00, 0x00, 0x01 };
time_t imageActionStartTime = 0;
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
static gxAssociationLogicalName associationHighGMacPreEstablished;
static gxAssociationLogicalName associationHighEcdsa;
static gxRegister activePowerL1;
static gxScriptTable scriptTableGlobalMeterReset;
static gxScriptTable scriptTableDisconnectControl;
static gxScriptTable scriptTableActivateTestMode;
static gxScriptTable scriptTableActivateNormalMode;
static gxProfileGeneric eventLog;
static gxActionSchedule actionScheduleDisconnectOpen;
static gxActionSchedule actionScheduleDisconnectClose;
static gxPushSetup pushSetup;
static gxMbusDiagnostic mbusDiagnostic;
static gxMBusPortSetup mbusPortSetup;
static gxDisconnectControl disconnectControl;
static gxProfileGeneric loadProfile;
static gxSapAssignment sapAssignment;
//Security Setup High is for High authentication.
static gxSecuritySetup securitySetupHigh;
//Security Setup HighGMac is for GMac authentication.
static gxSecuritySetup securitySetupHighGMac;
//Security Setup HighGMac is for pre-established GMac authentication.
static gxSecuritySetup securitySetupHighGMacPreEstablished;
//Security Setup HighEdcsa is for ECDSA authentication.
static gxSecuritySetup securitySetupHighEcdsa;

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
gxLimiter limiter;
gxG3PlcMacLayerCounters g3plcMacLayerCounters;
gxG3PlcMacSetup g3PlcMacSetup;
gxG3Plc6LoWPAN g3Plc6LoWPAN;
gxArrayManager arrayManager;
gxLteMonitoring lteMonitoring;
gxAccount account;

#ifdef DLMS_ITALIAN_STANDARD

static gxTariffPlan activeTariffPlan;
static gxTariffPlan passiveTariffPlan;
static gxData currentDiagnostic;
static gxData snapshotReasonCode;
static gxData snapshotPeriodCounter;
static gxData spareObject;
static gxProfileGeneric hourlyProfileGeneric;
static gxProfileGeneric dailyProfileGeneric;
static gxProfileGeneric snapshotPeriodData;
static gxData hourlyDiagnostic;
//Compact Frame 6.
static gxCompactData cf6;
//Compact Frame 62.
static gxCompactData cf62;
//Compact Frame 63.
static gxCompactData cf63;
//Compact Frame 64.
static gxCompactData cf64;
static gxData ppNetworkStatus;
static gxData metrologicalEventCounter;
static gxData eventCounter;
static gxData dailyDiagnostic;
static gxRegister hourlyConvertedConsumption;
static gxRegister hourlyConvertedConsumptionUnderAlarm;
static gxRegister currentIndexOfConvertedVolume;
static gxRegister currentIndexOfConvertedVolumeF1Rate;
static gxRegister currentIndexOfConvertedVolumeF2Rate;
static gxRegister currentIndexOfConvertedVolumeF3Rate;
static gxData snapshotPeriodDiagnostic;
static gxRegister currentIndexOfConvertedVolumeUnderAlarm;
static gxExtendedRegister maximumConventionalConvertedGasFlow;
static gxData managementFrameCounterOnLine;
static gxData billingSnapshotPeriodCounter;
#endif //DLMS_ITALIAN_STANDARD

//static gxObject* NONE_OBJECTS[] = { BASE(associationNone), BASE(ldn) };

static gxObject* ALL_OBJECTS[] = {
    BASE(associationNone), BASE(associationLow), BASE(associationHigh), BASE(associationHighGMac),
    BASE(associationHighGMacPreEstablished),
    BASE(associationHighEcdsa),
    BASE(securitySetupHigh), BASE(securitySetupHighGMac),BASE(securitySetupHighGMacPreEstablished),
    BASE(securitySetupHighEcdsa),
    BASE(ldn), BASE(sapAssignment), BASE(eventCode),
    BASE(clock1), BASE(activePowerL1), BASE(pushSetup), BASE(scriptTableGlobalMeterReset), BASE(scriptTableDisconnectControl),
    BASE(scriptTableActivateTestMode), BASE(scriptTableActivateNormalMode), BASE(loadProfile), BASE(eventLog), BASE(hdlc),
    BASE(disconnectControl), BASE(actionScheduleDisconnectOpen), BASE(actionScheduleDisconnectClose), BASE(unixTime), BASE(invocationCounter),
    BASE(imageTransfer), BASE(udpSetup), BASE(autoConnect), BASE(activityCalendar), BASE(localPortSetup), BASE(demandRegister),
    BASE(registerMonitor), BASE(autoAnswer), BASE(modemConfiguration), BASE(macAddressSetup), BASE(ip4Setup), BASE(pppSetup), BASE(gprsSetup),
    BASE(tarifficationScriptTable), BASE(registerActivation), BASE(limiter),
    BASE(mbusDiagnostic), BASE(mbusPortSetup),
    BASE(g3plcMacLayerCounters), BASE(g3PlcMacSetup), BASE(g3Plc6LoWPAN), BASE(arrayManager),
    BASE(lteMonitoring),
    BASE(account),
    #ifdef DLMS_ITALIAN_STANDARD
    BASE(activeTariffPlan),
    BASE(passiveTariffPlan),
    BASE(spareObject),
    BASE(currentDiagnostic),
    BASE(snapshotReasonCode),
    BASE(snapshotPeriodCounter),
    BASE(cf6),
    BASE(cf62),
    BASE(cf63),
    BASE(cf64),
    BASE(hourlyProfileGeneric),
    BASE(dailyProfileGeneric),
    BASE(snapshotPeriodData),
    BASE(hourlyDiagnostic),
    BASE(hourlyConvertedConsumption),
    BASE(hourlyConvertedConsumptionUnderAlarm),
    BASE(ppNetworkStatus),
    BASE(metrologicalEventCounter),
    BASE(eventCounter),
    BASE(dailyDiagnostic),
    BASE(currentIndexOfConvertedVolume),
    BASE(currentIndexOfConvertedVolumeF1Rate),
    BASE(currentIndexOfConvertedVolumeF2Rate),
    BASE(currentIndexOfConvertedVolumeF3Rate),
    BASE(snapshotPeriodDiagnostic),
    BASE(currentIndexOfConvertedVolumeUnderAlarm),
    BASE(maximumConventionalConvertedGasFlow),
    BASE(managementFrameCounterOnLine),
    BASE(billingSnapshotPeriodCounter),
#endif //DLMS_ITALIAN_STANDARD
};

////////////////////////////////////////////////////
//Define what is serialized to decrease EEPROM usage.
gxSerializerIgnore NON_SERIALIZED_OBJECTS[] = {
    //Nothing is saved when authentication is not used.
    IGNORE_ATTRIBUTE(BASE(associationNone), GET_ATTRIBUTE_ALL()),
    IGNORE_ATTRIBUTE(BASE(securitySetupHighGMac), GET_ATTRIBUTE_ALL()),
    IGNORE_ATTRIBUTE(BASE(securitySetupHighGMacPreEstablished), GET_ATTRIBUTE_ALL()),
    IGNORE_ATTRIBUTE(BASE(securitySetupHighEcdsa), GET_ATTRIBUTE_ALL()),
    //Only password is saved for low and high authentication.
    IGNORE_ATTRIBUTE(BASE(associationLow), GET_ATTRIBUTE_EXCEPT(7)),
    IGNORE_ATTRIBUTE(BASE(associationHigh), GET_ATTRIBUTE_EXCEPT(7)),
    //Only scaler and unit are saved for all register objects.
    IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_REGISTER, GET_ATTRIBUTE(2)),
    //Objects are not load because they are created statically.
    IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, GET_ATTRIBUTE(2))
};

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
    if (f != NULL)
    {
        gxSerializerSettings serializerSettings;
        ser_init(&serializerSettings);
        serializerSettings.stream = f;
        serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
        serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
        ret = ser_saveObjects(&serializerSettings, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));
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
    uint16_t* rowSize,
    uint16_t* sortLocation)
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
        if (sortLocation != NULL && *sortLocation == 0 &&
            pg->sortObject == it->key)
        {
            //Update location of sort object.
            *sortLocation = *rowSize;
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
        getProfileGenericBufferColumnSizes(settings, pg, NULL, NULL, &rowSize, NULL);
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
        uint16_t dataSize = 0;
        uint8_t columnSizes[20];
        DLMS_DATA_TYPE dataTypes[20];
        if (sizeof(dataTypes) / sizeof(dataTypes[0]) < pg->captureObjects.size ||
            sizeof(columnSizes) / sizeof(columnSizes[0]) < pg->captureObjects.size)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
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
        getProfileGenericBufferColumnSizes(settings, pg, dataTypes, columnSizes, &dataSize, NULL);
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
                gxtime tm2;
                time_now(&tm2, 1);
                e.value.ulVal = time_toUnixTime2(&tm2);
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
#ifndef DLMS_ITALIAN_STANDARD
        associationHighGMac.xDLMSContextInfo.maxSendPduSize = associationHighGMac.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
#else
        associationHighGMac.xDLMSContextInfo.maxSendPduSize = associationHighGMac.xDLMSContextInfo.maxReceivePduSize = 504;
#endif //DLMS_ITALIAN_STANDARD
        associationHighGMac.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
            DLMS_CONFORMANCE_GENERAL_PROTECTION |
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
        memcpy(associationHighGMac.securitySetupReference, securitySetupHighGMac.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds pre-established example 
// Logical Name Association object for GMAC High authentication.
///////////////////////////////////////////////////////////////////////
int addAssociationHighGMacPreEstablished()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 5, 255 };
    if ((ret = INIT_OBJECT(associationHighGMacPreEstablished, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHighGMacPreEstablished.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH_GMAC;
        OA_ATTACH(associationHighGMac.objectList, ALL_OBJECTS);
        associationHighGMacPreEstablished.clientSAP = 0x2;
        associationHighGMacPreEstablished.xDLMSContextInfo.maxSendPduSize = associationHighGMacPreEstablished.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationHighGMacPreEstablished.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
            DLMS_CONFORMANCE_SET |
            DLMS_CONFORMANCE_SELECTIVE_ACCESS |
            DLMS_CONFORMANCE_ACTION |
            DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
            DLMS_CONFORMANCE_GET);
        //GMAC authentication don't need password.
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        associationHighGMacPreEstablished.securitySetup = &securitySetupHighGMacPreEstablished;
#else
        memcpy(associationHighGMacPreEstablished.securitySetupReference, securitySetupHighGMacPreEstablished.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for ECDSA High authentication.
///////////////////////////////////////////////////////////////////////
int addAssociationHighECDSA()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 6, 255 };
    if ((ret = INIT_OBJECT(associationHighEcdsa, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHighEcdsa.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH_ECDSA;
        OA_ATTACH(associationHighEcdsa.objectList, ALL_OBJECTS);
        associationHighEcdsa.clientSAP = 0x3;
        associationHighEcdsa.xDLMSContextInfo.maxSendPduSize = associationHighEcdsa.xDLMSContextInfo.maxReceivePduSize = PDU_BUFFER_SIZE;
        associationHighEcdsa.xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)
            (DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
                DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
                DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
                DLMS_CONFORMANCE_SET |
                DLMS_CONFORMANCE_SELECTIVE_ACCESS |
                DLMS_CONFORMANCE_ACTION |
                DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
                DLMS_CONFORMANCE_GET);
        //GMAC authentication don't need password.
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        associationHighEcdsa.securitySetup = &securitySetupHighEcdsa;
#else
        memcpy(associationHighEcdsa.securitySetupReference, securitySetupHighEcdsa.base.logicalName, 6);
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
#ifdef DLMS_ITALIAN_STANDARD
    // Client system title is not send with message and it must define for the 
    // meter. This example uses client system title: Gurux123
    static unsigned char CLIENT_SYSTEM_TITLE[8] = { 0x47, 0x75, 0x72, 0x75, 0x78, 0x31, 0x32, 0x33 };
#else
    static unsigned char CLIENT_SYSTEM_TITLE[8] = { 0 };
#endif
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
//This method adds security setup object for pre-established GMAC authentication.
///////////////////////////////////////////////////////////////////////
int addSecuritySetupPreEstablishedHighGMac()
{
    int ret;
    //Define client system title for the pre-established connection.
    //Pre-establiched system title is Gurux123
    static unsigned char CLIENT_SYSTEM_TITLE[8] = { 0x47, 0x75, 0x72, 0x75, 0x78, 0x31, 0x32, 0x33 };
    const unsigned char ln[6] = { 0, 0, 43, 0, 3, 255 };
    if ((ret = INIT_OBJECT(securitySetupHighGMacPreEstablished, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
    {
        BB_ATTACH(securitySetupHighGMacPreEstablished.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
        BB_ATTACH(securitySetupHighGMacPreEstablished.clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
        //Only Authenticated encrypted connections are allowed.
        securitySetupHighGMacPreEstablished.securityPolicy = DLMS_SECURITY_POLICY_AUTHENTICATED_ENCRYPTED;
        securitySetupHighGMacPreEstablished.securitySuite = DLMS_SECURITY_SUITE_V0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds security setup object for ECDSA authentication.
///////////////////////////////////////////////////////////////////////
int addSecuritySetupHighEcdsa()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 43, 0, 4, 255 };
    if ((ret = INIT_OBJECT(securitySetupHighEcdsa, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
    {
        BB_ATTACH(securitySetupHighEcdsa.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
        //Ciphering is not used.
        securitySetupHighEcdsa.securityPolicy = DLMS_SECURITY_POLICY_NOTHING;
        securitySetupHighEcdsa.securitySuite = DLMS_SECURITY_SUITE_V1;
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
    activePowerL1.unit = DLMS_UNIT_ACTIVE_ENERGY;
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
//Add limiter object.
///////////////////////////////////////////////////////////////////////
int addLimiter()
{
    int ret;
    const unsigned char ln[6] = { 0,0,17,0,0,255 };
    if ((ret = INIT_OBJECT(limiter, DLMS_OBJECT_TYPE_LIMITER, ln)) == 0)
    {
        limiter.monitoredValue = BASE(activePowerL1);
        limiter.selectedAttributeIndex = 2;
        //Add emergency profile group IDs.
        dlmsVARIANT* tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        var_init(tmp);
        var_setUInt16(tmp, 1);
        va_push(&limiter.emergencyProfileGroupIDs, tmp);
        tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        var_init(tmp);
        var_setUInt16(tmp, 2);
        va_push(&limiter.emergencyProfileGroupIDs, tmp);
        //Energy profile is active for 5 seconds.
        limiter.emergencyProfile.duration = 5;

        GX_UINT16(limiter.thresholdActive) = 1000;
        GX_UINT16(limiter.thresholdEmergency) = 2000;
        GX_UINT16(limiter.thresholdNormal) = 1000;
        limiter.minOverThresholdDuration = 60;
        limiter.minUnderThresholdDuration = 60;
        //////////////////////
        //Add under action. Turn LED OFF.
        limiter.actionUnderThreshold.script = &scriptTableDisconnectControl;
        limiter.actionUnderThreshold.scriptSelector = 1;
        //Add over action. Turn LED ON.
        limiter.actionOverThreshold.script = &scriptTableDisconnectControl;
        limiter.actionOverThreshold.scriptSelector = 2;
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
        //Add clock object.
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
        //Add clock object.
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
    bb_addString(&localPortSetup.deviceAddress, "12345678");
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
    imageTransfer.imageBlockSize = 450;
    imageTransfer.imageFirstNotTransferredBlockNumber = 0;
    //Enable image transfer.
    imageTransfer.imageTransferEnabled = 1;
    //There is only one image.
    gxImageActivateInfo* info;
    if (imageTransfer.imageActivateInfo.size == 0)
    {
        info = malloc(sizeof(gxImageActivateInfo));
        bb_init(&info->identification);
        bb_init(&info->signature);
        arr_push(&imageTransfer.imageActivateInfo, info);
    }
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
//Add Mbus diagnostic object.
///////////////////////////////////////////////////////////////////////
int addMbusDiagnostic()
{
    int ret;
    gxBroadcastFrameCounter* item;
    const unsigned char ln[6] = { 0,0,24,9,0,255 };
    if ((ret = INIT_OBJECT(mbusDiagnostic, DLMS_OBJECT_TYPE_MBUS_DIAGNOSTIC, ln)) == 0)
    {
        mbusDiagnostic.receivedSignalStrength = 50;
        mbusDiagnostic.channelId = 1;
        mbusDiagnostic.linkStatus = DLMS_MBUS_LINK_STATUS_NORMAL;
        item = (gxBroadcastFrameCounter*)malloc(sizeof(gxBroadcastFrameCounter));
        item->clientId = 1;
        item->counter = 1;
        time_now(&item->timeStamp, 1);
        arr_push(&mbusDiagnostic.broadcastFrames, item);
        item = (gxBroadcastFrameCounter*)malloc(sizeof(gxBroadcastFrameCounter));
        item->clientId = 1;
        item->counter = 2;
        time_now(&item->timeStamp, 1);
        arr_push(&mbusDiagnostic.broadcastFrames, item);
        mbusDiagnostic.transmissions = 1000;
        mbusDiagnostic.receivedFrames = 1000;
        mbusDiagnostic.failedReceivedFrames = 1;
        mbusDiagnostic.captureTime.attributeId = 2;
        time_now(&mbusDiagnostic.captureTime.timeStamp, 1);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Mbus port setup object.
///////////////////////////////////////////////////////////////////////
int addMbusPortSetup()
{
    int ret;
    const unsigned char ln[6] = { 0,0,24,8,0,255 };
    const unsigned char PROFILE_SELECTION[6] = { 0,0,24,0,0,255 };
    if ((ret = INIT_OBJECT(mbusPortSetup, DLMS_OBJECT_TYPE_MBUS_PORT_SETUP, ln)) == 0)
    {
        memcpy(mbusPortSetup.profileSelection, PROFILE_SELECTION, sizeof(PROFILE_SELECTION));
        mbusPortSetup.portCommunicationStatus = DLMS_MBUS_PORT_COMMUNICATION_STATE_LIMITED_ACCESS;
        mbusPortSetup.dataHeaderType = DLMS_MBUS_DATA_HEADER_TYPE_SHORT;
        mbusPortSetup.primaryAddress = 1;
        mbusPortSetup.identificationNumber = 2;
        mbusPortSetup.manufacturerId = 3;
        mbusPortSetup.mBusVersion = 1;
        mbusPortSetup.deviceType = DLMS_MBUS_METER_TYPE_ENERGY;
        mbusPortSetup.maxPduSize = 320;
        //Add listening window.
        gxtime* start, * end;
        start = (gxtime*)malloc(sizeof(gxtime));
        time_init(start, -1, -1, -1, 0, 0, 0, 0, -1);
        end = (gxtime*)malloc(sizeof(gxtime));
        time_init(end, -1, -1, -1, 12, 0, 0, 0, -1);
        arr_push(&mbusPortSetup.listeningWindow, key_init(start, end));
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add G3 PLC MAC layer counters object.
///////////////////////////////////////////////////////////////////////
int addG3PlcMacLayerCounters()
{
    int ret;
    const unsigned char ln[6] = { 0,0,29,0,0,255 };
    if ((ret = INIT_OBJECT(g3plcMacLayerCounters, DLMS_OBJECT_TYPE_G3_PLC_MAC_LAYER_COUNTERS, ln)) == 0)
    {
        g3plcMacLayerCounters.txDataPacketCount = 1;
        g3plcMacLayerCounters.rxDataPacketCount = 2;
        g3plcMacLayerCounters.txCmdPacketCount = 3;
        g3plcMacLayerCounters.rxCmdPacketCount = 4;
        g3plcMacLayerCounters.cSMAFailCount = 5;
        g3plcMacLayerCounters.cSMANoAckCount = 6;
        g3plcMacLayerCounters.badCrcCount = 7;
        g3plcMacLayerCounters.txDataBroadcastCount = 8;
        g3plcMacLayerCounters.rxDataBroadcastCount = 9;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add G3 PLC MAC setup object.
///////////////////////////////////////////////////////////////////////
int addG3PlcMacSetup()
{
    int ret;
    //CENELEC-A        
    static unsigned char TONE_MASK_CENELEC_A[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00 };
    static unsigned char TX_COEF_CENELEC_A[] = { 0xFF, 0xFF, 0xFF };
    static const unsigned char GMK_KEY[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    const unsigned char ln[6] = { 0,0,29,1,0,255 };
    if ((ret = INIT_OBJECT(g3PlcMacSetup, DLMS_OBJECT_TYPE_G3_PLC_MAC_SETUP, ln)) == 0)
    {
        g3PlcMacSetup.shortAddress = 1;
        g3PlcMacSetup.rcCoord = 2;
        g3PlcMacSetup.panId = 3;
        gxG3MacKeyTable* key = (gxG3MacKeyTable*)malloc(sizeof(gxG3MacKeyTable));
        key->id = 1;
        memcpy(key->key, GMK_KEY, sizeof(GMK_KEY));
        arr_push(&g3PlcMacSetup.keyTable, key);
        key = (gxG3MacKeyTable*)malloc(sizeof(gxG3MacKeyTable));
        key->id = 2;
        memcpy(key->key, GMK_KEY, sizeof(GMK_KEY));
        arr_push(&g3PlcMacSetup.keyTable, key);
        g3PlcMacSetup.frameCounter = 4;
        BIT_ATTACH(g3PlcMacSetup.toneMask, TONE_MASK_CENELEC_A, 8 * sizeof(TONE_MASK_CENELEC_A));
        g3PlcMacSetup.tmrTtl = 5;
        g3PlcMacSetup.maxFrameRetries = 6;
        g3PlcMacSetup.neighbourTableEntryTtl = 7;
        gxNeighbourTable* nt = (gxNeighbourTable*)malloc(sizeof(gxNeighbourTable));
        nt->shortAddress = 2;
        nt->payloadModulationScheme = 3;
        ba_init(&nt->toneMap);
        ba_set(&nt->toneMap, 1);
        ba_set(&nt->toneMap, 1);
        ba_set(&nt->toneMap, 1);
        ba_set(&nt->toneMap, 1);
        ba_set(&nt->toneMap, 1);
        ba_set(&nt->toneMap, 1);
        nt->modulation = DLMS_G3_PLC_MODULATION_QAM16;
        nt->txGain = 4;
        nt->txRes = 5;
        BIT_ATTACH(nt->txCoeff, TX_COEF_CENELEC_A, 8 * sizeof(TX_COEF_CENELEC_A));
        nt->lqi = 6;
        nt->phaseDifferential = 7;
        nt->tmrValidTime = 8;
        nt->noData = 9;
        arr_push(&g3PlcMacSetup.neighbourTable, nt);
        g3PlcMacSetup.highPriorityWindowSize = 10;
        g3PlcMacSetup.cscmFairnessLimit = 11;
        g3PlcMacSetup.beaconRandomizationWindowLength = 12;
        g3PlcMacSetup.macA = 13;
        g3PlcMacSetup.macK = 14;
        g3PlcMacSetup.minCwAttempts = 15;
        g3PlcMacSetup.cenelecLegacyMode = 16;
        g3PlcMacSetup.fccLegacyMode = 17;
        g3PlcMacSetup.maxBe = 18;
        g3PlcMacSetup.maxCsmaBackoffs = 19;
        g3PlcMacSetup.minBe = 20;
        g3PlcMacSetup.macBroadcastMaxCwEnabled = 1;
        g3PlcMacSetup.macTransmitAtten = 22;
        gxMacPosTable* mpt = (gxMacPosTable*)malloc(sizeof(gxMacPosTable));
        mpt->shortAddress = 1;
        mpt->lqi = 2;
        mpt->validTime = 3;
        arr_push(&g3PlcMacSetup.macPosTable, mpt);
        mpt = (gxMacPosTable*)malloc(sizeof(gxMacPosTable));
        mpt->shortAddress = 2;
        mpt->lqi = 3;
        mpt->validTime = 4;
        arr_push(&g3PlcMacSetup.macPosTable, mpt);
    }
    return ret;
}


#ifdef DLMS_ITALIAN_STANDARD
///////////////////////////////////////////////////////////////////////
// Add tariff plan object. Tariff plan is used only in Italy standard.
// 
// Active UNI/TS Tariff Plan (end-of-billing-period)
///////////////////////////////////////////////////////////////////////
int addActiveTariffPlan()
{
    int ret;
    static unsigned char CALENDAR_NAME[2] = { 0x0D, 0x01 };
    static unsigned char WEEKLY_ACTIVATION[2] = { 0x7, 0xFF };
    const unsigned char ln[6] = { 0, 0, 94, 39, 21, 101 };
    if ((ret = INIT_OBJECT(activeTariffPlan, DLMS_OBJECT_TYPE_TARIFF_PLAN, ln)) == 0)
    {
        BB_ATTACH(activeTariffPlan.calendarName, CALENDAR_NAME, 2);
        activeTariffPlan.enabled = 0;
        time_init(&activeTariffPlan.activationTime, 2015, 1, 1, 0, 0, 0, 0, 120);
        //General 
        activeTariffPlan.plan.defaultTariffBand = 3;
        BIT_ATTACH(activeTariffPlan.plan.weeklyActivation, WEEKLY_ACTIVATION, 2);

        //////////////
        //Get winter season.
        activeTariffPlan.plan.winterSeason.dayOfMonth = 1;
        activeTariffPlan.plan.winterSeason.month = 10;
        //Working day interval #1.
        activeTariffPlan.plan.winterSeason.workingDayIntervals[0].startHour = 21;
        activeTariffPlan.plan.winterSeason.workingDayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Working day interval #2.
        activeTariffPlan.plan.winterSeason.workingDayIntervals[1].startHour = 24;
        activeTariffPlan.plan.winterSeason.workingDayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #3.
        activeTariffPlan.plan.winterSeason.workingDayIntervals[2].startHour = 24;
        activeTariffPlan.plan.winterSeason.workingDayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #4.
        activeTariffPlan.plan.winterSeason.workingDayIntervals[3].startHour = 24;
        activeTariffPlan.plan.winterSeason.workingDayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #5.
        activeTariffPlan.plan.winterSeason.workingDayIntervals[4].startHour = 24;
        activeTariffPlan.plan.winterSeason.workingDayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Saturday interval #1.
        activeTariffPlan.plan.winterSeason.saturdayIntervals[0].startHour = 21;
        activeTariffPlan.plan.winterSeason.saturdayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Saturday interval #2.
        activeTariffPlan.plan.winterSeason.saturdayIntervals[1].startHour = 21;
        activeTariffPlan.plan.winterSeason.saturdayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #3.
        activeTariffPlan.plan.winterSeason.saturdayIntervals[2].startHour = 21;
        activeTariffPlan.plan.winterSeason.saturdayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #4.
        activeTariffPlan.plan.winterSeason.saturdayIntervals[3].startHour = 21;
        activeTariffPlan.plan.winterSeason.saturdayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #5.
        activeTariffPlan.plan.winterSeason.saturdayIntervals[4].startHour = 21;
        activeTariffPlan.plan.winterSeason.saturdayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Holiday interval #1.
        activeTariffPlan.plan.winterSeason.holidayIntervals[0].startHour = 21;
        activeTariffPlan.plan.winterSeason.holidayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Holiday interval #2.
        activeTariffPlan.plan.winterSeason.holidayIntervals[1].startHour = 21;
        activeTariffPlan.plan.winterSeason.holidayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #3.
        activeTariffPlan.plan.winterSeason.holidayIntervals[2].startHour = 21;
        activeTariffPlan.plan.winterSeason.holidayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #4.
        activeTariffPlan.plan.winterSeason.holidayIntervals[3].startHour = 21;
        activeTariffPlan.plan.winterSeason.holidayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #5.
        activeTariffPlan.plan.winterSeason.holidayIntervals[4].startHour = 21;
        activeTariffPlan.plan.winterSeason.holidayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //////////////
        // Get summer season.
        activeTariffPlan.plan.summerSeason.dayOfMonth = 1;
        activeTariffPlan.plan.summerSeason.month = 4;
        //Working day interval #1.
        activeTariffPlan.plan.summerSeason.workingDayIntervals[0].startHour = 21;
        activeTariffPlan.plan.summerSeason.workingDayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Working day interval #2.
        activeTariffPlan.plan.summerSeason.workingDayIntervals[1].startHour = 24;
        activeTariffPlan.plan.summerSeason.workingDayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #3.
        activeTariffPlan.plan.summerSeason.workingDayIntervals[2].startHour = 24;
        activeTariffPlan.plan.summerSeason.workingDayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #4.
        activeTariffPlan.plan.summerSeason.workingDayIntervals[3].startHour = 24;
        activeTariffPlan.plan.summerSeason.workingDayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #5.
        activeTariffPlan.plan.summerSeason.workingDayIntervals[4].startHour = 24;
        activeTariffPlan.plan.summerSeason.workingDayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Saturday interval #1.
        activeTariffPlan.plan.summerSeason.saturdayIntervals[0].startHour = 21;
        activeTariffPlan.plan.summerSeason.saturdayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Saturday interval #2.
        activeTariffPlan.plan.summerSeason.saturdayIntervals[1].startHour = 21;
        activeTariffPlan.plan.summerSeason.saturdayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #3.
        activeTariffPlan.plan.summerSeason.saturdayIntervals[2].startHour = 21;
        activeTariffPlan.plan.summerSeason.saturdayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #4.
        activeTariffPlan.plan.summerSeason.saturdayIntervals[3].startHour = 21;
        activeTariffPlan.plan.summerSeason.saturdayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #5.
        activeTariffPlan.plan.summerSeason.saturdayIntervals[4].startHour = 21;
        activeTariffPlan.plan.summerSeason.saturdayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Holiday interval #1.
        activeTariffPlan.plan.summerSeason.holidayIntervals[0].startHour = 21;
        activeTariffPlan.plan.summerSeason.holidayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Holiday interval #2.
        activeTariffPlan.plan.summerSeason.holidayIntervals[1].startHour = 21;
        activeTariffPlan.plan.summerSeason.holidayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #3.
        activeTariffPlan.plan.summerSeason.holidayIntervals[2].startHour = 21;
        activeTariffPlan.plan.summerSeason.holidayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #4.
        activeTariffPlan.plan.summerSeason.holidayIntervals[3].startHour = 21;
        activeTariffPlan.plan.summerSeason.holidayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #5.
        activeTariffPlan.plan.summerSeason.holidayIntervals[4].startHour = 21;
        activeTariffPlan.plan.summerSeason.holidayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        dlmsVARIANT* it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 1;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 2;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 3;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 4;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 5;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 6;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 7;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 8;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 9;
        va_push(&activeTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 10;
        va_push(&activeTariffPlan.plan.specialDays, it);
    }
    return ret;
}

int addPassiveTariffPlan()
{
    int ret;
    static unsigned char CALENDAR_NAME[2] = { 0x0D, 0x01 };
    static unsigned char WEEKLY_ACTIVATION[2] = { 0x7, 0xFF };
    const unsigned char ln[6] = { 0, 0, 94, 39, 22, 255 }; //0-0:94.39.22.255
    if ((ret = INIT_OBJECT(passiveTariffPlan, DLMS_OBJECT_TYPE_TARIFF_PLAN, ln)) == 0)
    {
        BB_ATTACH(passiveTariffPlan.calendarName, CALENDAR_NAME, 2);
        passiveTariffPlan.enabled = 0;
        time_init(&passiveTariffPlan.activationTime, 2024, 1, 1, 0, 0, 0, 0, 120);
        //General 
        passiveTariffPlan.plan.defaultTariffBand = 3;
        BIT_ATTACH(passiveTariffPlan.plan.weeklyActivation, WEEKLY_ACTIVATION, 2);

        //////////////
        //Get winter season.
        passiveTariffPlan.plan.winterSeason.dayOfMonth = 1;
        passiveTariffPlan.plan.winterSeason.month = 10;
        //Working day interval #1.
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[0].startHour = 21;
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Working day interval #2.
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[1].startHour = 24;
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #3.
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[2].startHour = 24;
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #4.
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[3].startHour = 24;
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #5.
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[4].startHour = 24;
        passiveTariffPlan.plan.winterSeason.workingDayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Saturday interval #1.
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[0].startHour = 21;
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Saturday interval #2.
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[1].startHour = 21;
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #3.
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[2].startHour = 21;
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #4.
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[3].startHour = 21;
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #5.
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[4].startHour = 21;
        passiveTariffPlan.plan.winterSeason.saturdayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Holiday interval #1.
        passiveTariffPlan.plan.winterSeason.holidayIntervals[0].startHour = 21;
        passiveTariffPlan.plan.winterSeason.holidayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Holiday interval #2.
        passiveTariffPlan.plan.winterSeason.holidayIntervals[1].startHour = 21;
        passiveTariffPlan.plan.winterSeason.holidayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #3.
        passiveTariffPlan.plan.winterSeason.holidayIntervals[2].startHour = 21;
        passiveTariffPlan.plan.winterSeason.holidayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #4.
        passiveTariffPlan.plan.winterSeason.holidayIntervals[3].startHour = 21;
        passiveTariffPlan.plan.winterSeason.holidayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #5.
        passiveTariffPlan.plan.winterSeason.holidayIntervals[4].startHour = 21;
        passiveTariffPlan.plan.winterSeason.holidayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //////////////
        // Get summer season.
        passiveTariffPlan.plan.summerSeason.dayOfMonth = 1;
        passiveTariffPlan.plan.summerSeason.month = 4;
        //Working day interval #1.
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[0].startHour = 21;
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Working day interval #2.
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[1].startHour = 24;
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #3.
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[2].startHour = 24;
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #4.
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[3].startHour = 24;
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Working day interval #5.
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[4].startHour = 24;
        passiveTariffPlan.plan.summerSeason.workingDayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Saturday interval #1.
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[0].startHour = 21;
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Saturday interval #2.
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[1].startHour = 21;
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #3.
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[2].startHour = 21;
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #4.
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[3].startHour = 21;
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Saturday interval #5.
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[4].startHour = 21;
        passiveTariffPlan.plan.summerSeason.saturdayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        //Holiday interval #1.
        passiveTariffPlan.plan.summerSeason.holidayIntervals[0].startHour = 21;
        passiveTariffPlan.plan.summerSeason.holidayIntervals[0].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2;
        //Holiday interval #2.
        passiveTariffPlan.plan.summerSeason.holidayIntervals[1].startHour = 21;
        passiveTariffPlan.plan.summerSeason.holidayIntervals[1].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #3.
        passiveTariffPlan.plan.summerSeason.holidayIntervals[2].startHour = 21;
        passiveTariffPlan.plan.summerSeason.holidayIntervals[2].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #4.
        passiveTariffPlan.plan.summerSeason.holidayIntervals[3].startHour = 21;
        passiveTariffPlan.plan.summerSeason.holidayIntervals[3].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;
        //Holiday interval #5.
        passiveTariffPlan.plan.summerSeason.holidayIntervals[4].startHour = 21;
        passiveTariffPlan.plan.summerSeason.holidayIntervals[4].intervalTariff = DLMS_DEFAULT_TARIFF_BAND_NONE;

        dlmsVARIANT* it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 1;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 2;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 3;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 4;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 5;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 6;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 7;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 8;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 9;
        va_push(&passiveTariffPlan.plan.specialDays, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 10;
        va_push(&passiveTariffPlan.plan.specialDays, it);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add spare object as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addSpareObject()
{
    int ret;
    unsigned char DATA[1] = { 0 };
    const unsigned char ln[6] = { 0, 0, 96, 39, 40, 255 };//0-0:94:39.40.255
    if ((ret = INIT_OBJECT(spareObject, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        var_addBytes(&spareObject.value, DATA, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add current diagnostic object as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addCurrentDiagnostic()
{
    int ret;
    unsigned char DATA[1] = { 0 };
    const unsigned char ln[6] = { 7, 0, 96, 5, 1, 255 };//7-0:96:5.1.255
    if ((ret = INIT_OBJECT(currentDiagnostic, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(currentDiagnostic.value) = 0;
    }
    return ret;
}


///////////////////////////////////////////////////////////////////////
//Add snapshot reason code object as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addSnapshotReasonCode()
{
    int ret;
    unsigned char DATA[1] = { 0 };
    const unsigned char ln[6] = { 0, 0, 96, 10, 2, 255 };//0-0:96:10.2.255
    if ((ret = INIT_OBJECT(snapshotReasonCode, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(snapshotReasonCode.value) = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add snapshot perios counter object as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addSnapshotPeriodCounter()
{
    int ret;
    unsigned char DATA[1] = { 0 };
    const unsigned char ln[6] = { 7, 0, 0, 1, 0, 255 };//7-0:0:1.0.255
    if ((ret = INIT_OBJECT(snapshotPeriodCounter, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        GX_UINT16(snapshotPeriodCounter.value) = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add management frame counter On-line object 
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addManagementFrameCounterOnLine()
{
    int ret;
    const unsigned char ln[6] = { 0,0,43,1,1,255 };
    if ((ret = INIT_OBJECT(managementFrameCounterOnLine, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        //Initial invocation counter value.
        GX_UINT32(managementFrameCounterOnLine.value) = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add billing snapshot/period counter object 
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addBillingSnapshotPeriodCounter()
{
    int ret;
    const unsigned char ln[6] = { 7,0,0,1,0,255 };
    if ((ret = INIT_OBJECT(billingSnapshotPeriodCounter, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
    {
        //Initial invocation counter value.
        GX_UINT32(billingSnapshotPeriodCounter.value) = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Compact frame 6 as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addCF6Plan(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 66, 0, 6, 255 }; // 0-0:66.0.6.255
    if ((ret = INIT_OBJECT(cf6, DLMS_OBJECT_TYPE_COMPACT_DATA, ln)) == 0)
    {
        cf6.templateId = 6;
        cf6.captureMethod = DLMS_CAPTURE_METHOD_IMPLICIT;
        //Add 6 capture objects.
        //CF6 attribute #4.
        gxTarget* capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 4;
        capture->dataIndex = 0;
        gxKey* k = key_init(BASE(cf6), capture);
        arr_push(&cf6.captureObjects, k);
        //Passive UNI/TS Tariff Plan attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(passiveTariffPlan), capture);
        arr_push(&cf6.captureObjects, k);
        //Passive UNI/TS Tariff Plan attribute #3.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 3;
        capture->dataIndex = 0;
        k = key_init(BASE(passiveTariffPlan), capture);
        arr_push(&cf6.captureObjects, k);
        //Passive UNI/TS Tariff Plan attribute #4.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 4;
        capture->dataIndex = 0;
        k = key_init(BASE(passiveTariffPlan), capture);
        arr_push(&cf6.captureObjects, k);
        //Passive UNI/TS Tariff Plan attribute #5.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 5;
        capture->dataIndex = 0;
        k = key_init(BASE(passiveTariffPlan), capture);
        arr_push(&cf6.captureObjects, k);
        //Spare Object.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(spareObject), capture);
        arr_push(&cf6.captureObjects, k);
        ret = compactData_updateTemplateDescription(settings, &cf6);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Compact frame 62 as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addCF62(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 66, 0, 62, 255 };//0-0:66.0.62.255
    if ((ret = INIT_OBJECT(cf62, DLMS_OBJECT_TYPE_COMPACT_DATA, ln)) == 0)
    {
        cf62.templateId = 62;
        cf62.captureMethod = DLMS_CAPTURE_METHOD_IMPLICIT;
        //Add 11 capture objects.
        //CF62 attribute #4.
        gxTarget* capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 4;
        capture->dataIndex = 0;
        gxKey* k = key_init(BASE(cf62), capture);
        arr_push(&cf62.captureObjects, k);

        //Unix time attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(unixTime), capture);
        arr_push(&cf62.captureObjects, k);

        //PP network status attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(ppNetworkStatus), capture);
        arr_push(&cf62.captureObjects, k);
        //LTE Monitoring attribute #3.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 3;
        capture->dataIndex = 0;
        k = key_init(BASE(lteMonitoring), capture);
        arr_push(&cf62.captureObjects, k);
        //Metrological event counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(metrologicalEventCounter), capture);
        arr_push(&cf62.captureObjects, k);
        //Event counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(eventCounter), capture);
        arr_push(&cf62.captureObjects, k);
        //Daily diagnostic attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(dailyDiagnostic), capture);
        arr_push(&cf62.captureObjects, k);

        //Current index of converted Volume attribute #3.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 3;
        capture->dataIndex = 0;
        k = key_init(BASE(currentIndexOfConvertedVolume), capture);
        arr_push(&cf62.captureObjects, k);

        //Daily load profile attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        //Last two day entries.
        capture->dataIndex = 0xE002;
        k = key_init(BASE(dailyProfileGeneric), capture);
        arr_push(&cf62.captureObjects, k);

        //Billing/Snapshot period counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(billingSnapshotPeriodCounter), capture);
        arr_push(&cf62.captureObjects, k);

        //Management frame counter - on-line attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(managementFrameCounterOnLine), capture);
        arr_push(&cf62.captureObjects, k);
        ret = compactData_updateTemplateDescription(settings, &cf62);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Compact frame 63 as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addCF63(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 66, 0, 63, 255 };//0-0:66.0.63.255
    if ((ret = INIT_OBJECT(cf63, DLMS_OBJECT_TYPE_COMPACT_DATA, ln)) == 0)
    {
        cf63.templateId = 63;
        cf63.captureMethod = DLMS_CAPTURE_METHOD_IMPLICIT;
        //Add 11 capture objects.
        //CF63 attribute #4.
        gxTarget* capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 4;
        capture->dataIndex = 0;
        gxKey* k = key_init(BASE(cf63), capture);
        arr_push(&cf63.captureObjects, k);

        //Unix time attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(unixTime), capture);
        arr_push(&cf63.captureObjects, k);

        //PP network status attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(ppNetworkStatus), capture);
        arr_push(&cf63.captureObjects, k);
        //LTE Monitoring attribute #3.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 3;
        capture->dataIndex = 0;
        k = key_init(BASE(lteMonitoring), capture);
        arr_push(&cf63.captureObjects, k);
        //Metrological event counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(metrologicalEventCounter), capture);
        arr_push(&cf63.captureObjects, k);
        //Event counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(eventCounter), capture);
        arr_push(&cf63.captureObjects, k);
        //Daily diagnostic attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(dailyDiagnostic), capture);
        arr_push(&cf63.captureObjects, k);

        //Current index of converted Volume attribute #3.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 3;
        capture->dataIndex = 0;
        k = key_init(BASE(currentIndexOfConvertedVolume), capture);
        arr_push(&cf63.captureObjects, k);

        //Daily load profile attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        //Last entry.
        capture->dataIndex = 0xE001;
        k = key_init(BASE(dailyProfileGeneric), capture);
        arr_push(&cf63.captureObjects, k);

        //Hourle load profile attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        //Last day entry.
        capture->dataIndex = 0xE201;
        k = key_init(BASE(hourlyProfileGeneric), capture);
        arr_push(&cf63.captureObjects, k);

        //Billing/Snapshot period counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(billingSnapshotPeriodCounter), capture);
        arr_push(&cf63.captureObjects, k);

        //Management frame counter - on-line attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(managementFrameCounterOnLine), capture);
        arr_push(&cf63.captureObjects, k);
        ret = compactData_updateTemplateDescription(settings, &cf63);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Compact frame 64 as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addCF64(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 66, 0, 64, 255 };//0-0:66.0.64.255
    if ((ret = INIT_OBJECT(cf64, DLMS_OBJECT_TYPE_COMPACT_DATA, ln)) == 0)
    {
        cf64.templateId = 64;
        cf64.captureMethod = DLMS_CAPTURE_METHOD_IMPLICIT;
        //Add 11 capture objects.
        //CF64 attribute #4.
        gxTarget* capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 4;
        capture->dataIndex = 0;
        gxKey* k = key_init(BASE(cf64), capture);
        arr_push(&cf64.captureObjects, k);

        //Unix time attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(unixTime), capture);
        arr_push(&cf64.captureObjects, k);

        //PP network status attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(ppNetworkStatus), capture);
        arr_push(&cf64.captureObjects, k);
        //LTE Monitoring attribute #3.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 3;
        capture->dataIndex = 0;
        k = key_init(BASE(lteMonitoring), capture);
        arr_push(&cf64.captureObjects, k);
        //Metrological event counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(metrologicalEventCounter), capture);
        arr_push(&cf64.captureObjects, k);
        //Event counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(eventCounter), capture);
        arr_push(&cf64.captureObjects, k);
        //Daily diagnostic attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(dailyDiagnostic), capture);
        arr_push(&cf64.captureObjects, k);

        //Current index of converted Volume attribute #3.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 3;
        capture->dataIndex = 0;
        k = key_init(BASE(currentIndexOfConvertedVolume), capture);
        arr_push(&cf64.captureObjects, k);

        //Daily load profile attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        //Last two entries.
        capture->dataIndex = 0xE002;
        k = key_init(BASE(dailyProfileGeneric), capture);
        arr_push(&cf64.captureObjects, k);

        //Hourle load profile attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        //Last two-day entries.
        capture->dataIndex = 0xE202;
        k = key_init(BASE(hourlyProfileGeneric), capture);
        arr_push(&cf64.captureObjects, k);

        //Billing/Snapshot period counter attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(billingSnapshotPeriodCounter), capture);
        arr_push(&cf64.captureObjects, k);

        //Management frame counter on-line attribute #2.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        k = key_init(BASE(managementFrameCounterOnLine), capture);
        arr_push(&cf64.captureObjects, k);
        ret = compactData_updateTemplateDescription(settings, &cf64);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
// Add hourly profile generic (historical data) object 
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addHourlyProfileGeneric(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 7, 0, 99, 99, 2, 255 };
    if ((ret = INIT_OBJECT(hourlyProfileGeneric, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        gxTarget* capture;
        //Set default values if load the first time.
        hourlyProfileGeneric.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add 4 columns.
        //Add hourly diagnostic.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&hourlyProfileGeneric.captureObjects, key_init(BASE(hourlyDiagnostic), capture));

        //Add hourly converted consumption.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&hourlyProfileGeneric.captureObjects, key_init(BASE(hourlyConvertedConsumption), capture));

        //Add hourly converted consumption under alarm.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&hourlyProfileGeneric.captureObjects, key_init(BASE(hourlyConvertedConsumptionUnderAlarm), capture));

        //Add unix time object.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&hourlyProfileGeneric.captureObjects, key_init(BASE(unixTime), capture));
        ///////////////////////////////////////////////////////////////////
        //Update amount of capture objects.
        //Set clock to sort object.
        hourlyProfileGeneric.sortObject = BASE(unixTime);
        hourlyProfileGeneric.sortObjectAttributeIndex = 2;
        hourlyProfileGeneric.profileEntries = getProfileGenericBufferMaxRowCount(settings, &hourlyProfileGeneric);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
// Add Daily profile generic (historical data) object 
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addDailyProfileGeneric(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 7, 0, 99, 99, 3, 255 };
    if ((ret = INIT_OBJECT(dailyProfileGeneric, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        gxTarget* capture;
        //Set default values if load the first time.
        dailyProfileGeneric.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add 6 columns.

        //Add unix time object.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&dailyProfileGeneric.captureObjects, key_init(BASE(unixTime), capture));

        //Add Daily diagnostic.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&dailyProfileGeneric.captureObjects, key_init(BASE(dailyDiagnostic), capture));

        //Add current index of converted volume.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&dailyProfileGeneric.captureObjects, key_init(BASE(currentIndexOfConvertedVolume), capture));

        //Add current index of converted volume under alarm.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&dailyProfileGeneric.captureObjects, key_init(BASE(currentIndexOfConvertedVolumeUnderAlarm), capture));

        //Add maximum conventional converted gas flow value.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&dailyProfileGeneric.captureObjects, key_init(BASE(maximumConventionalConvertedGasFlow), capture));

        //Add maximum conventional converted gas flow status.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 4;
        capture->dataIndex = 0;
        arr_push(&dailyProfileGeneric.captureObjects, key_init(BASE(maximumConventionalConvertedGasFlow), capture));

        ///////////////////////////////////////////////////////////////////
        //Update amount of capture objects.
        //Set clock to sort object.
        dailyProfileGeneric.sortObject = BASE(unixTime);
        dailyProfileGeneric.sortObjectAttributeIndex = 2;
        dailyProfileGeneric.profileEntries = getProfileGenericBufferMaxRowCount(settings, &dailyProfileGeneric);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
// Add snapshot period data profile generic (historical data) object 
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addSnapshotPeriodData(dlmsSettings* settings)
{
    int ret;
    const unsigned char ln[6] = { 7, 0, 98, 11, 0, 255 };
    if ((ret = INIT_OBJECT(snapshotPeriodData, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        gxTarget* capture;
        //Set default values if load the first time.
        snapshotPeriodData.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add 13 columns.

        //Add unix time object.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(unixTime), capture));

        //Add current diagnostic.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(currentDiagnostic), capture));

        //Add snapshot reason code.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(snapshotReasonCode), capture));

        //Add snapshot period counter.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(snapshotPeriodCounter), capture));

        //Add current index of converted volume.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(currentIndexOfConvertedVolume), capture));

        //Add current index of converted volume F1 rate.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(currentIndexOfConvertedVolumeF1Rate), capture));

        //Add current index of converted volume F2 rate.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(currentIndexOfConvertedVolumeF2Rate), capture));

        //Add current index of converted volume F3 rate.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(currentIndexOfConvertedVolumeF3Rate), capture));

        //Add current index of converted volume under alarm.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(currentIndexOfConvertedVolumeUnderAlarm), capture));

        //Add maximum conventional converted gas flow value.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(maximumConventionalConvertedGasFlow), capture));

        //Add maximum conventional converted gas flow status.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 4;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(maximumConventionalConvertedGasFlow), capture));

        //Add snapshot period diagnostic.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(snapshotPeriodDiagnostic), capture));
        //Add active UNI/TS tariff plan.
        capture = (gxTarget*)malloc(sizeof(gxTarget));
        capture->attributeIndex = 2;
        capture->dataIndex = 0;
        arr_push(&snapshotPeriodData.captureObjects, key_init(BASE(activeTariffPlan), capture));

        ///////////////////////////////////////////////////////////////////
        //Update amount of capture objects.
        //Set clock to sort object.
        snapshotPeriodData.sortObject = BASE(unixTime);
        snapshotPeriodData.sortObjectAttributeIndex = 2;
        snapshotPeriodData.profileEntries = getProfileGenericBufferMaxRowCount(settings, &snapshotPeriodData);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example hourly diagnostic data object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addHourlyDiagnostic()
{
    const unsigned char ln[6] = { 7,3,96,5,1,255 };
    INIT_OBJECT(hourlyDiagnostic, DLMS_OBJECT_TYPE_DATA, ln);
    GX_UINT16(hourlyDiagnostic.value) = 0;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example PP network status data object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addPpNetworkStatus()
{
    const unsigned char ln[6] = { 0,1,96,5,4,255 };
    INIT_OBJECT(ppNetworkStatus, DLMS_OBJECT_TYPE_DATA, ln);
    GX_UINT16(ppNetworkStatus.value) = 0;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example metrological event counter data object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addMetrologicalEventCounter()
{
    const unsigned char ln[6] = { 0,0,96,15,1,255 };
    INIT_OBJECT(metrologicalEventCounter, DLMS_OBJECT_TYPE_DATA, ln);
    GX_UINT16(metrologicalEventCounter.value) = 0;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example event counter data object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addEventCounter()
{
    const unsigned char ln[6] = { 0,0,96,15,2,255 };
    INIT_OBJECT(eventCounter, DLMS_OBJECT_TYPE_DATA, ln);
    GX_UINT16(eventCounter.value) = 0;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example daily diagnostic data object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addDailyDiagnostic()
{
    const unsigned char ln[6] = { 7,1,96,5,1,255 };
    INIT_OBJECT(dailyDiagnostic, DLMS_OBJECT_TYPE_DATA, ln);
    GX_UINT16(dailyDiagnostic.value) = 0;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example hourly diagnostic register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addHourlyConvertedConsumption()
{
    const unsigned char ln[6] = { 7,0,13,14,0,255 };
    INIT_OBJECT(hourlyConvertedConsumption, DLMS_OBJECT_TYPE_REGISTER, ln);
    //10 ^ -2 =  0,01
    GX_UINT16(hourlyConvertedConsumption.value) = 0;
    hourlyConvertedConsumption.scaler = -2;
    hourlyConvertedConsumption.unit = DLMS_UNIT_ACTIVE_ENERGY;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds hourly converted consumption under alarm register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addHourlyConvertedConsumptionUnderAlarm()
{
    const unsigned char ln[6] = { 7,0,12,14,0,255 };
    INIT_OBJECT(hourlyConvertedConsumptionUnderAlarm, DLMS_OBJECT_TYPE_REGISTER, ln);
    //10 ^ -2 =  0,01
    GX_UINT16(hourlyConvertedConsumptionUnderAlarm.value) = 1;
    hourlyConvertedConsumptionUnderAlarm.scaler = -2;
    hourlyConvertedConsumptionUnderAlarm.unit = DLMS_UNIT_ACTIVE_ENERGY;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds current index of converted volume register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addCurrentIndexOfConvertedVolume()
{
    const unsigned char ln[6] = { 7, 0, 13, 2, 0, 255 };
    INIT_OBJECT(currentIndexOfConvertedVolume, DLMS_OBJECT_TYPE_REGISTER, ln);
    GX_UINT16(currentIndexOfConvertedVolume.value) = 0;
    currentIndexOfConvertedVolume.scaler = 0;
    currentIndexOfConvertedVolume.unit = DLMS_UNIT_NONE;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds current index of converted volume F1 rate register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addcurrentIndexOfConvertedVolumeF1Rate()
{
    const unsigned char ln[6] = { 7, 0, 13, 2, 1, 255 };
    INIT_OBJECT(currentIndexOfConvertedVolumeF1Rate, DLMS_OBJECT_TYPE_REGISTER, ln);
    GX_UINT16(currentIndexOfConvertedVolumeF1Rate.value) = 0;
    currentIndexOfConvertedVolumeF1Rate.scaler = 0;
    currentIndexOfConvertedVolumeF1Rate.unit = DLMS_UNIT_NONE;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds current index of converted volume F1 rate register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addcurrentIndexOfConvertedVolumeF2Rate()
{
    const unsigned char ln[6] = { 7, 0, 13, 2, 2, 255 };
    INIT_OBJECT(currentIndexOfConvertedVolumeF2Rate, DLMS_OBJECT_TYPE_REGISTER, ln);
    GX_UINT16(currentIndexOfConvertedVolumeF2Rate.value) = 0;
    currentIndexOfConvertedVolumeF2Rate.scaler = 0;
    currentIndexOfConvertedVolumeF2Rate.unit = DLMS_UNIT_NONE;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds current index of converted volume F1 rate register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addcurrentIndexOfConvertedVolumeF3Rate()
{
    const unsigned char ln[6] = { 7, 0, 13, 2, 3, 255 };
    INIT_OBJECT(currentIndexOfConvertedVolumeF3Rate, DLMS_OBJECT_TYPE_REGISTER, ln);
    GX_UINT16(currentIndexOfConvertedVolumeF3Rate.value) = 0;
    currentIndexOfConvertedVolumeF3Rate.scaler = 0;
    currentIndexOfConvertedVolumeF3Rate.unit = DLMS_UNIT_NONE;
    return 0;
}

///////////////////////////////////////////////////////////////////////
// This method adds snapshot period diagnostic data object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addSnapshotPeriodDiagnostic()
{
    const unsigned char ln[6] = { 7, 2, 96, 5, 1, 255 };
    INIT_OBJECT(snapshotPeriodDiagnostic, DLMS_OBJECT_TYPE_DATA, ln);
    GX_UINT16(snapshotPeriodDiagnostic.value) = 0;
    return 0;
}

///////////////////////////////////////////////////////////////////////
// This method adds current index of converted volume under alarm register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addCurrentIndexOfConvertedVolumeUnderAlarm()
{
    const unsigned char ln[6] = { 7,0,12,2,0,255 };
    INIT_OBJECT(currentIndexOfConvertedVolumeUnderAlarm, DLMS_OBJECT_TYPE_REGISTER, ln);
    GX_UINT16(currentIndexOfConvertedVolumeUnderAlarm.value) = 0;
    currentIndexOfConvertedVolumeUnderAlarm.scaler = 0;
    currentIndexOfConvertedVolumeUnderAlarm.unit = DLMS_UNIT_NONE;
    return 0;
}


///////////////////////////////////////////////////////////////////////
//This method adds  maximum conventional converted gas flow value extended register object
// as defined in UNI TS 11291-13-2 specifications.
///////////////////////////////////////////////////////////////////////
int addMaximumConventionalConvertedGasFlow()
{
    const unsigned char ln[6] = { 7,0,43,45,0,255 };
    INIT_OBJECT(maximumConventionalConvertedGasFlow, DLMS_OBJECT_TYPE_EXTENDED_REGISTER, ln);
    //Set default value.
    GX_UINT16(maximumConventionalConvertedGasFlow.value) = 10;
    maximumConventionalConvertedGasFlow.scaler = 0;
    maximumConventionalConvertedGasFlow.unit = DLMS_UNIT_NONE;
    //Set default status.
    GX_UINT16(maximumConventionalConvertedGasFlow.status) = 0;
    return 0;
}

#endif //DLMS_ITALIAN_STANDARD

///////////////////////////////////////////////////////////////////////
//Add G3 PLC 6LoWPAN object.
///////////////////////////////////////////////////////////////////////
int addG3Plc6LoWPAN()
{
    int ret;
    //The maximum Context field length is 16 bytes (128 bits).
    static unsigned char PREFIX_TABLE[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    const unsigned char ln[6] = { 0,0,29,2,0,255 };
    if ((ret = INIT_OBJECT(g3Plc6LoWPAN, DLMS_OBJECT_TYPE_G3_PLC_6LO_WPAN, ln)) == 0)
    {
        g3Plc6LoWPAN.maxHops = 1;
        g3Plc6LoWPAN.weakLqiValue = 2;
        g3Plc6LoWPAN.securityLevel = 3;
        bb_set(&g3Plc6LoWPAN.prefixTable, PREFIX_TABLE, 16);
        gxRoutingConfiguration* rg = (gxRoutingConfiguration*)malloc(sizeof(gxRoutingConfiguration));
        rg->netTraversalTime = 1;
        rg->routingTableEntryTtl = 2;
        rg->kr = 3;
        rg->km = 4;
        rg->kc = 5;
        rg->kq = 6;
        rg->kh = 7;
        rg->krt = 8;
        rg->rReqRetries = 9;
        rg->rReqReqWait = 10;
        rg->blacklistTableEntryTtl = 11;
        rg->unicastRreqGenEnable = 1;
        rg->rlcEnabled = 1;
        rg->addRevLinkCost = 14;
        arr_push(&g3Plc6LoWPAN.routingConfiguration, rg);

        gxRoutingTable* rt = (gxRoutingTable*)malloc(sizeof(gxRoutingTable));
        rt->destinationAddress = 1;
        rt->nextHopAddress = 2;
        rt->routeCost = 3;
        rt->hopCount = 4;
        rt->weakLinkCount = 5;
        rt->validTime = 6;
        arr_push(&g3Plc6LoWPAN.routingTable, rt);

        gxContextInformationTable* cit = (gxContextInformationTable*)malloc(sizeof(gxContextInformationTable));
        cit->cid = 0xF;
        cit->context[0] = 0xFF;
        cit->context[1] = 0;
        cit->context[2] = 0xFF;
        cit->context[3] = 0;
        cit->contextLength = 4;
        cit->compression = 1;
        cit->validLifetime = 4;
        arr_push(&g3Plc6LoWPAN.contextInformationTable, cit);

        gxBlacklistTable* blt = (gxBlacklistTable*)malloc(sizeof(gxBlacklistTable));
        blt->neighbourAddress = 1;
        blt->validTime = 2;
        arr_push(&g3Plc6LoWPAN.blacklistTable, blt);
        blt = (gxBlacklistTable*)malloc(sizeof(gxBlacklistTable));
        blt->neighbourAddress = 3;
        blt->validTime = 4;
        arr_push(&g3Plc6LoWPAN.blacklistTable, blt);

        dlmsVARIANT* it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 1;
        va_push(&g3Plc6LoWPAN.groupTable, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 2;
        va_push(&g3Plc6LoWPAN.groupTable, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 3;
        va_push(&g3Plc6LoWPAN.groupTable, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 4;
        va_push(&g3Plc6LoWPAN.groupTable, it);

        g3Plc6LoWPAN.broadcastLogTableEntryTtl = 6;
        g3Plc6LoWPAN.maxJoinWaitTime = 12;
        g3Plc6LoWPAN.pathDiscoveryTime = 13;
        g3Plc6LoWPAN.activeKeyIndex = 14;
        g3Plc6LoWPAN.metricType = 15;
        g3Plc6LoWPAN.coordShortAddress = 16;
        g3Plc6LoWPAN.disableDefaultRouting = 1;
        g3Plc6LoWPAN.deviceType = DLMS_PAN_DEVICE_TYPE_NOT_DEFINED;
        g3Plc6LoWPAN.defaultCoordRouteEnabled = 1;

        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 1;
        va_push(&g3Plc6LoWPAN.destinationAddress, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 2;
        va_push(&g3Plc6LoWPAN.destinationAddress, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 3;
        va_push(&g3Plc6LoWPAN.destinationAddress, it);
        it = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
        GX_UINT16(*it) = 4;
        va_push(&g3Plc6LoWPAN.destinationAddress, it);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add array manager object.
///////////////////////////////////////////////////////////////////////
int addArrayManager()
{
    int ret;
    const unsigned char ln[6] = { 0,0,18,0,0,255 };
    if ((ret = INIT_OBJECT(arrayManager, DLMS_OBJECT_TYPE_ARRAY_MANAGER, ln)) == 0)
    {
        gxArrayManagerItem* it = (gxArrayManagerItem*)malloc(sizeof(gxArrayManagerItem));
        it->id = 1;
        it->element.target = BASE(loadProfile);
        it->element.attributeIndex = 2;
        arr_push(&arrayManager.elements, it);
        it = (gxArrayManagerItem*)malloc(sizeof(gxArrayManagerItem));
        it->id = 2;
        it->element.target = BASE(loadProfile);
        it->element.attributeIndex = 3;
        arr_push(&arrayManager.elements, it);
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

///////////////////////////////////////////////////////////////////////
//Add account object.
///////////////////////////////////////////////////////////////////////
int addAccount()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 19, 0, 0, 255 };
    if ((ret = INIT_OBJECT(account, DLMS_OBJECT_TYPE_ACCOUNT, ln)) == 0)
    {
        account.paymentMode = DLMS_ACCOUNT_PAYMENT_MODE_CREDIT;
        //Set default values.
        account.currentCreditStatus = DLMS_ACCOUNT_CREDIT_STATUS_OUT_OF_CREDIT;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add LTE monitoring object.
///////////////////////////////////////////////////////////////////////
int addLteMonitoring()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 25, 11, 0, 255 };
    if ((ret = INIT_OBJECT(lteMonitoring, DLMS_OBJECT_TYPE_LTE_MONITORING, ln)) == 0)
    {
        //Set default values.
        lteMonitoring.networkParameters.t3402 = 0;
        lteMonitoring.networkParameters.t3412 = 0;
        lteMonitoring.networkParameters.t3412ext2 = 0;
        lteMonitoring.networkParameters.t3324 = 0;
        lteMonitoring.networkParameters.teDRX = 0;
        lteMonitoring.networkParameters.tPTW = 0;
        lteMonitoring.networkParameters.qRxlevMin = 0;
        lteMonitoring.networkParameters.qRxlevMinCE = 0;
        lteMonitoring.networkParameters.qRxLevMinCE1 = 0;
        lteMonitoring.qualityOfService.signalQuality = 0;
        lteMonitoring.qualityOfService.signalLevel = 0;
        lteMonitoring.qualityOfService.signalToNoiseRatio = 0;
        lteMonitoring.qualityOfService.coverageEnhancement = 0;
    }
    return ret;
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
    int ret;
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
            gxSerializerSettings serializerSettings;
            ser_init(&serializerSettings);
            serializerSettings.stream = f;
            serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
            serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
            ret = ser_loadObjects(settings, &serializerSettings, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));
            fclose(f);
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
        (ret = addAssociationHighGMacPreEstablished()) != 0 ||
        (ret = addAssociationHighECDSA()) != 0 ||
        (ret = addSecuritySetupPreEstablishedHighGMac()) != 0 ||
        (ret = addSecuritySetupHighEcdsa()) != 0 ||
        (ret = addMbusDiagnostic()) != 0 ||
        (ret = addMbusPortSetup()) != 0 ||
        (ret = addG3PlcMacLayerCounters()) != 0 ||
        (ret = addG3PlcMacSetup()) != 0 ||
        (ret = addG3Plc6LoWPAN()) != 0 ||
        (ret = addArrayManager()) != 0 ||
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
        (ret = addLimiter()) != 0 ||
        (ret = addLteMonitoring()) != 0 ||
        (ret = addAccount()) != 0 ||
#ifdef DLMS_ITALIAN_STANDARD
        (ret = addCurrentDiagnostic()) != 0 ||
        (ret = addSnapshotReasonCode()) != 0 ||
        (ret = addSnapshotPeriodCounter()) != 0 ||
        (ret = addActiveTariffPlan()) != 0 ||
        (ret = addPassiveTariffPlan()) != 0 ||
        (ret = addSpareObject()) != 0 ||
        (ret = addHourlyDiagnostic()) != 0 ||
        (ret = addHourlyConvertedConsumption()) != 0 ||
        (ret = addHourlyConvertedConsumptionUnderAlarm()) != 0 ||
        (ret = addCurrentIndexOfConvertedVolume()) != 0 ||
        (ret = addcurrentIndexOfConvertedVolumeF1Rate()) != 0 ||
        (ret = addcurrentIndexOfConvertedVolumeF2Rate()) != 0 ||
        (ret = addcurrentIndexOfConvertedVolumeF3Rate()) != 0 ||
        (ret = addSnapshotPeriodDiagnostic()) != 0 ||
        (ret = addCurrentIndexOfConvertedVolumeUnderAlarm()) != 0 ||
        (ret = addMaximumConventionalConvertedGasFlow()) != 0 ||
        (ret = addPpNetworkStatus()) != 0 ||
        (ret = addMetrologicalEventCounter()) != 0 ||
        (ret = addEventCounter()) != 0 ||
        (ret = addDailyDiagnostic()) != 0 ||
        (ret = addManagementFrameCounterOnLine()) != 0 ||
        (ret = addBillingSnapshotPeriodCounter()) != 0 ||
        //Profile generic objects must add after capture objects are initialized.
        (ret = addHourlyProfileGeneric(&settings->base)) != 0 ||
        (ret = addDailyProfileGeneric(&settings->base)) != 0 ||
        (ret = addSnapshotPeriodData(&settings->base)) != 0 ||
        //Compact data object must add after capture objects are initialized.
        (ret = addCF6Plan(&settings->base)) != 0 ||
        (ret = addCF62(&settings->base)) != 0 ||
        (ret = addCF63(&settings->base)) != 0 ||
        (ret = addCF64(&settings->base)) != 0 ||
#endif //DLMS_ITALIAN_STANDARD
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
    else if (con->settings.base.interfaceType == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
    {
        con->settings.hdlc = &hdlc;
        con->settings.localPortSetup = &localPortSetup;
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
    gxValueEventArg* e,
    uint32_t startTime,
    uint32_t endTime)
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
    if (startTime == 0 && endTime == 0)
    {
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
    }
    else
    {
        s = startTime;
        l = endTime;
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
        uint8_t columnSizes[20];
        DLMS_DATA_TYPE dataTypes[20];
        if (sizeof(dataTypes) / sizeof(dataTypes[0]) < pg->captureObjects.size ||
            sizeof(columnSizes) / sizeof(columnSizes[0]) < pg->captureObjects.size)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        if (f != NULL)
        {
            //Find date time location. In Italy standard the date time can be the the last column.
            uint16_t location = 0;
            getProfileGenericBufferColumnSizes(settings, pg, dataTypes, columnSizes, &rowSize, &location);
            //Skip current index and total amount of the entries.
            fseek(f, 4, SEEK_SET);
            for (pos = 0; pos != pg->entriesInUse; ++pos)
            {
                //Load time from EEPROM.
                fseek(f, location, SEEK_CUR);
                fread(&t, sizeof(uint32_t), 1, f);
                fseek(f, -(location + sizeof(uint32_t)), SEEK_CUR);
                //seek to begin of next row.
                fseek(f, rowSize, SEEK_CUR);
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
                        if (t <= l)
                        {
                            ++e->transactionEndIndex;
                        }
                    }
                    last = t;
                }
                else if (t > l)
                {
                    //End value is found.
                    break;
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
    int ret;
    gxArray captureObjects;
    arr_init(&captureObjects);
    char fileName[30];
    ret = getProfileGenericFileName(pg, fileName);
    uint32_t startTime = 0, endTime = 0;
    unsigned char maxCount = 0;
    unsigned char selectedColumns = 0;
    DLMS_SELECTIVE_ACCESS_PARAMETER selective = DLMS_SELECTIVE_ACCESS_PARAMETER_NONE;
    if (e->action && e->dataIndex != 0)
    {
        //Data index tells rows count when compact data ask it.
        //Read by range.
        e->selector = 1;
        gxtime start, end;
        time_now(&end, 1);
        //Reset seconds.
        time_addSeconds(&end, -time_getSeconds(&end));
        end.millisecond = 0;
        maxCount = (unsigned char)(e->dataIndex & 0xFF);
        selective = (e->dataIndex >> 12);
        selectedColumns = (unsigned char)((e->dataIndex >> 8) & 0xF);
        switch (selective)
        {
            /*Last number of seconds.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_SECONDS:
            time_copy(&start, &end);
            time_addSeconds(&start, -maxCount);
            break;
            /*Last complete number of minutes.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_MINUTES:
            /*Last complete number of minutes including the current minute.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_MINUTES:
            if (selective == DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_MINUTES)
            {
                //Reset seconds.
                time_addSeconds(&end, -time_getSeconds(&end));
            }
            time_copy(&start, &end);
            time_addMinutes(&start, -maxCount);
            break;
            /*Last complete number of hours.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_HOURS:
            /*Last complete number of hours including the current hour.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_HOURS:
            if (selective == DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_HOURS)
            {
                //Reset seconds.
                time_addSeconds(&end, -time_getSeconds(&end));
                //Reset minutes.
                time_addMinutes(&end, -time_getMinutes(&end));
            }
            time_copy(&start, &end);
            time_addHours(&start, -maxCount);
            break;
            /*Last complete number of days.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_DAYS:
            /*Last complete number of days including the current day.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_DAYS:
            if (selective == DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_DAYS)
            {
                //Reset seconds.
                time_addSeconds(&end, -time_getSeconds(&end));
                //Reset minutes.
                time_addMinutes(&end, -time_getMinutes(&end));
                //Reset hours.
                time_addHours(&end, -time_getHours(&end));
            }
            time_copy(&start, &end);
            if (selective == DLMS_SELECTIVE_ACCESS_PARAMETER_DAYS)
            {
                //Reset seconds.
                time_addSeconds(&start, -time_getSeconds(&start));
                //Reset minutes.
                time_addMinutes(&start, -time_getMinutes(&start));
                //Reset hours.
                time_addHours(&start, -time_getHours(&start));
            }
            time_addDays(&start, -maxCount);
            break;
            /*Last complete number of months.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_MONTHS:
            /*Last complete number of months including the current month.*/
        case DLMS_SELECTIVE_ACCESS_PARAMETER_MONTHS:
            if (selective == DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_MONTHS)
            {
                //Reset seconds.
                time_addSeconds(&end, -time_getSeconds(&end));
                //Reset minutes.
                time_addMinutes(&end, -time_getMinutes(&end));
                //Reset hours.
                time_addHours(&end, -time_getHours(&end));
                //Reset days.
                time_addDays(&end, -(time_getDays(&end) - 1));
            }
            time_copy(&start, &end);
            if (selective == DLMS_SELECTIVE_ACCESS_PARAMETER_MONTHS)
            {
                //Reset seconds.
                time_addSeconds(&start, -time_getSeconds(&start));
                //Reset minutes.
                time_addMinutes(&start, -time_getMinutes(&start));
                //Reset hours.
                time_addHours(&start, -time_getHours(&start));
                //Reset days.
                time_addDays(&start, -(time_getDays(&start) - 1));
            }
            time_addMonths(&start, -maxCount);
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        };
        startTime = time_toUnixTime2(&start);
        endTime = time_toUnixTime2(&end);
        char s1[40];
        char e1[40];
        time_toString2(&start, s1, sizeof(s1));
        time_toString2(&end, e1, sizeof(e1));
        printf("Reading max %d rows between: %s - %s\r\n", maxCount, s1, e1);
    }
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
                if ((ret = getProfileGenericDataByRangeFromRingBuffer(settings, fileName, e, startTime, endTime)) != 0 ||
                    (ret = cosem_getColumns(&pg->captureObjects, e->selector, &e->parameters, &captureObjects)) != 0)
                {
                    e->transactionStartIndex = e->transactionEndIndex = 0;
                }
                else if (e->action && maxCount != 0 &&
                    e->transactionEndIndex >= e->transactionStartIndex + maxCount)
                {
                    //Set maximum count if profile generic selective access is used.
                    e->transactionEndIndex = e->transactionStartIndex + maxCount - 1;
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
        //Objects are saved to the buffer when action is used.
        if (ret == 0 && first && !e->action)
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
            FILE* f = NULL;
#if _MSC_VER > 1400
            if (fopen_s(&f, fileName, "rb") != 0)
            {
                printf("Failed to open %s.\r\n", fileName);
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
#else
            if ((f = fopen(fileName, "rb")) != 0)
            {
                printf("Failed to open %s.\r\n", fileName);
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
#endif
            uint16_t dataSize = 0;
            uint8_t columnSizes[20];
            DLMS_DATA_TYPE dataTypes[20];
            if (sizeof(dataTypes) / sizeof(dataTypes[0]) < pg->captureObjects.size ||
                sizeof(columnSizes) / sizeof(columnSizes[0]) < pg->captureObjects.size)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if (f != NULL)
            {
                ret = getProfileGenericBufferColumnSizes(settings, pg, dataTypes, columnSizes, &dataSize, NULL);
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
                //Objects are saved to the buffer when action is used.
                if (e->action)
                {
                    //Clear buffer.
                    arr_clear(&pg->buffer);
                }
                uint16_t pduSize = 0;
                for (pos = e->transactionStartIndex - 1; pos != e->transactionEndIndex; ++pos)
                {
                    //Objects are saved to the buffer when action is used.
                    if (!e->action)
                    {
                        if (dlms_isPduFull(settings, e->value.byteArr, NULL))
                        {
                            //Return last full row.
                            --e->transactionStartIndex;
                            e->value.byteArr->size = pduSize;
                            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                            break;
                        }
                        pduSize = e->value.byteArr->size;
                        if ((ret = cosem_setStructure(e->value.byteArr, pg->captureObjects.size)) != 0)
                        {
                            break;
                        }
                    }
                    uint8_t colIndex;
                    gxKey* it;
                    variantArray* va = NULL;
                    dlmsVARIANT* value;
                    //Objects are saved to the buffer when action is used.
                    if (e->action)
                    {
                        va = (variantArray*)malloc(sizeof(variantArray));
                        va_init(va);
                        arr_push(&pg->buffer, va);
                        //Allocate space for 20 bytes.
                        //The example expects that the max value size is 20 bytes.
                        bb_capacity(e->value.byteArr, 20);
                    }
                    //Loop capture columns and get values.
                    for (colIndex = 0; colIndex != pg->captureObjects.size; ++colIndex)
                    {
                        if (selectedColumns != 0 && colIndex == selectedColumns)
                        {
                            //If selected columns is used.
                            break;
                        }
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
                                //Objects are saved to the buffer when action is used.
                                if (va != NULL)
                                {
                                    value = malloc(sizeof(dlmsVARIANT));
                                    var_init(value);
                                    if (((gxObject*)it->key)->objectType == DLMS_OBJECT_TYPE_CLOCK)
                                    {
                                        //Add time from clock object.
                                        gxtime tmp;
                                        time_fromUnixTime(&tmp, &tm);
                                        var_setDateTime(value, &tmp);
                                    }
                                    else
                                    {
                                        //Add time from data object.
                                        var_setUInt32(value, time);
                                    }
                                    va_push(va, value);
                                }
                                else
                                {
                                    if (((gxObject*)it->key)->objectType == DLMS_OBJECT_TYPE_CLOCK)
                                    {
                                        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &tm);
                                        //Error is handled later.
                                    }
                                    else
                                    {
                                        ret = cosem_setUInt32(e->value.byteArr, time_toUnixTime(&tm));
                                        //Error is handled later.
                                    }
                                }
                            }
                            else
                            {
                                //Objects are saved to the buffer when action is used.
                                if (!e->action)
                                {
                                    //Append data type.
                                    if ((ret = bb_setUInt8(e->value.byteArr, dataTypes[colIndex])) != 0)
                                    {
                                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                                        break;
                                    }
                                    if (bb_getCapacity(e->value.byteArr) < bb_size(e->value.byteArr) + columnSizes[colIndex])
                                    {
                                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                                        break;
                                    }
                                }
                                //Read data.
                                fread(&e->value.byteArr->data[e->value.byteArr->size], columnSizes[colIndex], 1, f);
                                e->value.byteArr->size += columnSizes[colIndex];
                                if (e->action)
                                {
                                    gxDataInfo di;
                                    di_init(&di);
                                    di.type = dataTypes[colIndex];
                                    value = malloc(sizeof(dlmsVARIANT));
                                    var_init(value);
                                    va_push(va, value);
                                    ret = dlms_getData(e->value.byteArr, &di, value);
                                    bb_empty(e->value.byteArr);
                                    if (ret != 0)
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                        if (ret != 0)
                        {
                            break;
                        }
                    }
                    if (ret != 0)
                    {
                        break;
                    }
                    ++e->transactionStartIndex;
                }
                fclose(f);
            }
            else
            {
                printf("Failed to open %s.\r\n", fileName);
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
        }
    }
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        //Don't set error if PDU is full.
        ret = 0;
    }
    return ret;
}

int updateCompactData(dlmsSettings* settings, gxCompactData* cd)
{
    int ret;
    uint16_t pos;
    gxValueEventCollection args2;
    gxByteBuffer bb;
    bb_init(&bb);
    //Allocate space for 200 bytes.
    bb_capacity(&bb, 200);

    gxValueEventArg* e2 = malloc(sizeof(gxValueEventArg));
    ve_init(e2);
    e2->action = 1;
    vec_init(&args2);
    ret = vec_push(&args2, e2);
    gxKey* kv;
    //Loop capture columns and get values.
    for (pos = 0; pos != cd->captureObjects.size; ++pos)
    {
        if ((ret = arr_getByIndex(&cd->captureObjects, (uint16_t)pos, (void**)&kv)) != 0)
        {
            break;
        }
        bb_empty(&bb);
        e2->transactionStartIndex = e2->transactionEndIndex = 0;
        e2->value.byteArr = &bb;
        e2->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        e2->target = (gxObject*)kv->key;
        e2->index = ((gxTarget*)kv->value)->attributeIndex;
        e2->dataIndex = ((gxTarget*)kv->value)->dataIndex;
        //Read and update values. This example uses preRead.
        svr_preRead(settings, &args2);
        if (e2->error != 0)
        {
            ret = e2->error;
            break;
        }
    }
    //Update  template description.
    ret = compactData_updateTemplateDescription(settings, cd);
    //Byte buffer is clear separetly.
    e2->value.vt = DLMS_DATA_TYPE_NONE;
    vec_clear(&args2);
    bb_clear(&bb);
    return ret;
}

//Read profile generic captured values.
int updateProfileGenericValues(dlmsSettings* settings, gxProfileGeneric* pg)
{
    int ret;
    uint16_t pos;
    gxValueEventCollection args2;
    gxValueEventArg* e2 = malloc(sizeof(gxValueEventArg));
    ve_init(e2);
    e2->action = 1;
    vec_init(&args2);
    ret = vec_push(&args2, e2);
    gxKey* kv;
    //Loop capture columns and get values.
    for (pos = 0; pos != pg->captureObjects.size; ++pos)
    {
        if ((ret = arr_getByIndex(&pg->captureObjects, (uint16_t)pos, (void**)&kv)) != 0)
        {
            break;
        }
        ve_clear(e2);
        e2->target = (gxObject*)kv->key;
        e2->index = ((gxTarget*)kv->value)->attributeIndex;
        e2->dataIndex = ((gxTarget*)kv->value)->dataIndex;
        //Read and update values. This example uses preRead.
        svr_preRead(settings, &args2);
        if (e2->error != 0)
        {
            ret = e2->error;
            break;
        }
    }
    vec_clear(&args2);
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
        else if (e->target->objectType == DLMS_OBJECT_TYPE_REGISTER && e->index == 2)
        {
            //Update value by one every time when user reads register.
            GX_ADD(((gxRegister*)e->target)->value, 1);
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
        else if (e->target->objectType == DLMS_OBJECT_TYPE_COMPACT_DATA && e->index == 2 &&
            ((gxCompactData*)e->target)->captureMethod == DLMS_CAPTURE_METHOD_IMPLICIT)
        {
            //Compact data is updated in read.
            ret = updateCompactData(settings, (gxCompactData*)e->target);
        }
        else if (e->target->objectType == DLMS_OBJECT_TYPE_COMPACT_DATA && e->index == 5)
        {
            //Update template description.
            ret = compactData_updateTemplateDescription(settings, (gxCompactData*)e->target);
        }
        //Update Unix time.
        else if (e->target == BASE(unixTime) && e->index == 2)
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
        else if (e->target == &compactData.base && e->index == 2)
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
        //If client try to update low level password when high level authentication is established.
        //This is possible in Indian standard.
        else if (e->target == BASE(associationHigh) && e->index == 7)
        {
            ret = cosem_getOctetString(e->value.byteArr, &associationLow.secret);
            saveSettings();
            e->handled = 1;
        }
        //If client try to update low level password when high level authentication is established.
        //This is possible in Indian standard.
        else if (e->target == BASE(associationHigh) && e->index == 7)
        {
            ret = cosem_getOctetString(e->value.byteArr, &associationLow.secret);
            saveSettings();
            e->handled = 1;
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
        it->error = captureProfileGeneric(settings, ((gxProfileGeneric*)it->target));
    }
    if (it->error == 0)
    {
        it->error = saveSettings();
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
            //Read captured profile generic values.
            e->error = updateProfileGenericValues(settings, (gxProfileGeneric*)e->target);
            if (e->error == 0)
            {
                handleProfileGenericActions(settings, e);
            }
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
        else if (e->target->objectType == DLMS_OBJECT_TYPE_COMPACT_DATA && e->index == 2 &&
            ((gxCompactData*)e->target)->captureMethod == DLMS_CAPTURE_METHOD_INVOKE)
        {
            //Compact data is updated with invoke.
            ret = updateCompactData(settings, (gxCompactData*)e->target);
        }
        else if (e->target == BASE(imageTransfer))
        {
            e->handled = 1;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            FILE* f;
            dlmsVARIANT* identification, * pSize;
            gxImageTransfer* i = (gxImageTransfer*)e->target;
            const char* imageFile = "image.raw";
            //Image name and size to transfer
            if (e->index == 1)
            {
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
                //There is only one image.
                gxImageActivateInfo* info;
                if (e->parameters.vt != DLMS_DATA_TYPE_STRUCTURE ||
                    (e->error = arr_getByIndex(&imageTransfer.imageActivateInfo, 0, (void**)&info)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    return;
                }
                if ((ret = va_getByIndex(e->parameters.Arr, 0, &identification)) != 0 ||
                    identification->vt != DLMS_DATA_TYPE_OCTET_STRING ||
                    (ret = va_getByIndex(e->parameters.Arr, 1, &pSize)) != 0 ||
                    pSize->vt != DLMS_DATA_TYPE_UINT32)
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    return;
                }
                info->size = var_toInteger(pSize);
                bb_clear(&info->identification);
                bb_set(&info->identification, identification->byteArr->data, bb_size(identification->byteArr));
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                printf("Updating image %s Size: %d\r\n", imageFile, info->size);
#endif
                allocateImageTransfer(imageFile, info->size);
                ba_clear(&i->imageTransferredBlocksStatus);
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_INITIATED;
                //Reset old image. This is needed if old image is bigger then the new one.
#if _MSC_VER > 1400
                FILE* f = NULL;
                fopen_s(&f, imageFile, "wb");
#else
                FILE* f = fopen(imageFile, "wb");
#endif
                fclose(f);
            }
            //Transfers one block of the Image to the server
            else if (e->index == 2)
            {
                dlmsVARIANT* index, * block;
                if (e->parameters.vt != DLMS_DATA_TYPE_STRUCTURE ||
                    (ret = va_getByIndex(e->parameters.Arr, 0, &index)) != 0 ||
                    index->vt != DLMS_DATA_TYPE_UINT32 ||
                    (ret = va_getByIndex(e->parameters.Arr, 1, &block)) != 0 ||
                    block->vt != DLMS_DATA_TYPE_OCTET_STRING)
                {
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                if ((ret = ba_setByIndex(&i->imageTransferredBlocksStatus, (uint16_t)var_toInteger(index), 1)) == 0)
                {
                    i->imageFirstNotTransferredBlockNumber = (uint32_t)(var_toInteger(index) + 1);
                }
                f = fopen(imageFile, "a+b");
                if (!f)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Unable to open file %s\r\n", imageFile);
#endif
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                int ret = (int)fwrite(block->byteArr->data, 1, (int)bb_size(block->byteArr), f);
                fclose(f);
                if (ret != (int)bb_size(block->byteArr))
                {
                    e->error = DLMS_ERROR_CODE_UNMATCH_TYPE;
                }
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
                gxImageActivateInfo* info;
                if ((e->error = arr_getByIndex(&imageTransfer.imageActivateInfo, 0, (void**)&info)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    return;
                }
                if (size != info->size)
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
                it.target = e->target;
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
        if (svr_isChangedWithAction(e->target->objectType, e->index))
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
    pos = (int)(p - (char*)push->destination.data);
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
    uint32_t serverAddress,
    uint32_t clientAddress)
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
#ifndef DLMS_INVOCATION_COUNTER_VALIDATOR
                    //Set Invocation counter value.
                    settings->expectedInvocationCounter = NULL;
#endif //DLMS_INVOCATION_COUNTER_VALIDATOR
                    //Client can establish a ciphered connection only with Security Suite 1.
                    settings->expectedSecuritySuite = 0;
                    //Security policy is not defined by default. Client can connect using any security policy.
                    settings->expectedSecurityPolicy = 0xFF;
                    if (a->securitySetup != NULL)
                    {
                        //Set expected client system title. If this is set only client that is 
                        // using expected client system title can connect to the meter.
                        if (a->securitySetup->clientSystemTitle.size == 8)
                        {
                            settings->expectedClientSystemTitle = a->securitySetup->clientSystemTitle.data;
                            settings->preEstablishedSystemTitle = &a->securitySetup->clientSystemTitle;
                            settings->cipher.security = DLMS_SECURITY_AUTHENTICATION_ENCRYPTION;
                            //Authentication is updated.
                            settings->authentication = a->authenticationMechanismName.mechanismId;
                        }
                        else
                        {
                            settings->preEstablishedSystemTitle = NULL;
                        }
                        //GMac authentication uses innocation counter.
                        if (a->securitySetup == &securitySetupHighGMac)
                        {
#ifndef DLMS_INVOCATION_COUNTER_VALIDATOR
                            //Set invocation counter value. If this is set client's invocation counter must match with server IC.
                            settings->expectedInvocationCounter = &securitySetupHighGMac.minimumInvocationCounter;
#endif //DLMS_INVOCATION_COUNTER_VALIDATOR
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
        settings->cipher.broadcast = (serverAddress & 0x3FFF) == 0x3FFF || (serverAddress & 0x7F) == 0x7F;
        if (!(settings->cipher.broadcast ||
            (serverAddress & 0x3FFF) == SERIAL_NUMBER % 10000 + 1000))
        {
            ret = 0;
            // Find address from the SAP table.
            gxSapAssignment* sap;
            objects.size = 0;
            if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, &objects) == 0)
            {
                //If SAP assigment is not used.
                if (objects.size == 0)
                {
                    ret = 1;
                }
                gxSapItem* it;
                uint16_t sapIndex, pos;
                for (sapIndex = 0; sapIndex != objects.size; ++sapIndex)
                {
                    if (oa_getByIndex(&objects, sapIndex, (gxObject**)&sap) == 0)
                    {
                        //If SAP assigment list is empty.
                        if (objects.size == 1 && sap->sapAssignmentList.size == 0)
                        {
                            ret = 1;
                        }
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
                    if (ret != 0)
                    {
                        break;
                    }
                }
            }
            oa_empty(&objects);
        }
        //Set serial number as meter address if broadcast is used.
        if (settings->cipher.broadcast && settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
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

//Get attribute access level for activity calendar.
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

//Get attribute access level for array manager.
DLMS_ACCESS_MODE getArrayManagerAttributeAccess(
    dlmsSettings* settings,
    unsigned char index)
{
    if (settings->authentication > DLMS_AUTHENTICATION_LOW && index == 2)
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
    if (obj->objectType == DLMS_OBJECT_TYPE_ARRAY_MANAGER)
    {
        return getArrayManagerAttributeAccess(settings, index);
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