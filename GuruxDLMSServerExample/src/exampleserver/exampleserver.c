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
#include <Winsock.h> //Add support for sockets
#include <time.h>
#include <process.h>//Add support for threads
#include <Winsock.h> //Add support for sockets
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
#include "../../development/include/notify.h"
#include "../../development/include/gxset.h"
//DLMS settings.
dlmsServerSettings settings;

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

int socket1 = -1;

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

//Action schedule execution times.
typedef struct
{
    unsigned char count;
    gxtime values[10];
} GXExecutionTimes;

//Push communication window.
typedef struct
{
    unsigned char count;
    gxTimePair values[10];
} GXPushWindow;

//Push object list.
typedef struct
{
    uint16_t objectType;
    unsigned char logicalName[6];
    signed char attributeIndex;
    uint16_t dataIndex;
} GXTarget;

//Push object list.
typedef struct
{
    unsigned char count;
    GXTarget values[5];
} GXObjectCollection;

//Profile Generic settings.
typedef struct
{
    //How often load profile is captured to the EEPROM.
    //Load profile period.
    unsigned long period;
    //Amount of used rows in profile generic buffer.
    uint16_t entriesInUse;
    //Amount of total rows in profile generic buffer.
    uint16_t profileEntries;
    //Entry where profile generic row is added in ring buffer.
    uint16_t rowIndex;
    //Capture object list.
    GXObjectCollection objects;
} GXProfileGeneric;

//Push Setup.
typedef struct
{
    //Push communication window.
    GXPushWindow communicationWindow;
    //Push object list.
    GXObjectCollection objects;
} GXPushSetup;

//SAP assignment list.
typedef struct
{
    unsigned char count;
    gxSapItem values[5];
} GXSapList;

//Association settings (passwords).
typedef struct
{
    //Define low level password.
    unsigned char llsPasswordSize;
    char llsPassword[20];
    //Define high level password.
    unsigned char hlsPasswordSize;
    char hlsPassword[20];
}GXAssociation;

//Security settings
typedef struct
{
    unsigned char authenticationKey[16];
    unsigned char blockCipherKey[16];
    unsigned char securityPolicy;
    uint32_t invacationVector;
}GXSecuritySettings;


//Save serialized meter data here.
typedef struct {
    //Meter serial number.
    uint32_t SERIAL_NUMBER;
    //Define Logical Device Name.
    char LDN[17];
    //Association passwords.
    GXAssociation association;
    //Don't use clock as a name. Some compilers are using clock as reserved word.
    gxClock clock1;
    //Disconnect open execution times.
    GXExecutionTimes disconnectOpenExecutions;
    //Disconnect close execution times.
    GXExecutionTimes disconnectCloseExecutions;
    //IEC HDLC Setup.
    gxIecHdlcSetup hdlc;
    GXProfileGeneric loadProfile;
    GXProfileGeneric eventLog;
    GXPushSetup push;
    //Is meter in test mode.
    char testMode;
    //SAP assigment lists.
    GXSapList sapAssignmentList;
    //Master key, A.K.A KEK.
    unsigned char masterKey[16];
    GXSecuritySettings securitySettings;
    uint32_t InvocationCounter;
} GXSerializedMeterData;

GXSerializedMeterData meterData;

//Push objects are added here.
static gxTarget PUSH_OBJECTS[6];

//Capture objects for load profile.
static gxTarget LOAD_PROFILE_CAPTURE_OBJECT[10] = { 0 };
//Two capture objects for event log.
static gxTarget EVENT_LOG_CAPTURE_OBJECT[2] = { 0 };

static gxData ldn;
static gxData eventCode;
static gxData unixTime;
static gxData frameCounter;
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
                                   BASE(meterData.clock1), BASE(activePowerL1), BASE(pushSetup), BASE(scriptTableGlobalMeterReset), BASE(scriptTableDisconnectControl),
                                   BASE(scriptTableActivateTestMode), BASE(scriptTableActivateNormalMode), BASE(loadProfile), BASE(eventLog), BASE(meterData.hdlc),
                                   BASE(disconnectControl), BASE(actionScheduleDisconnectOpen), BASE(actionScheduleDisconnectClose), BASE(unixTime), BASE(frameCounter),
                                   BASE(imageTransfer), BASE(udpSetup), BASE(autoConnect), BASE(activityCalendar), BASE(localPortSetup), BASE(demandRegister),
                                   BASE(registerMonitor), BASE(autoAnswer), BASE(modemConfiguration), BASE(macAddressSetup), BASE(ip4Setup), BASE(pppSetup), BASE(gprsSetup),
                                   BASE(tarifficationScriptTable), BASE(registerActivation)
};


///////////////////////////////////////////////////////////////////////
// Write trace to the serial port.
//
// This can be used for debugging.
///////////////////////////////////////////////////////////////////////
void GXTRACE(const char* str, const char* data)
{
    //Send trace to the serial port in test mode.
    if (meterData.testMode)
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
        clock_utcToMeterTime(&meterData.clock1, value);
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
int addAssociationLow(uint16_t serializationVersion)
{
    int ret;
    if (serializationVersion == 0)
    {
        memcpy(meterData.association.llsPassword, "Gurux", 5);
        meterData.association.llsPasswordSize = 5;
    }
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
        BB_ATTACH_STR(associationLow.secret, meterData.association.llsPassword, meterData.association.llsPasswordSize);
        associationLow.securitySetup = NULL;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for High authentication.
// UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addAssociationHigh(uint16_t serializationVersion)
{
    int ret;
    if (serializationVersion == 0)
    {
        memcpy(meterData.association.hlsPassword, "Gurux", 5);
        meterData.association.hlsPasswordSize = 5;
    }
    //Dedicated key.
    static unsigned char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 3, 255 };
    if ((ret = INIT_OBJECT(associationHigh, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHigh.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH;
        OA_ATTACH(associationHigh.objectList, ALL_OBJECTS);
        BB_ATTACH(associationHigh.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        //All objects are add for this Association View later.
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
        BB_ATTACH_STR(associationHigh.secret, meterData.association.hlsPassword, meterData.association.hlsPasswordSize);
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
int addAssociationHighGMac(uint16_t serializationVersion)
{
    int ret;
    //Dedicated key.
    static unsigned char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 4, 255 };
    if ((ret = INIT_OBJECT(associationHighGMac, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        associationHighGMac.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH_GMAC;
        OA_ATTACH(associationHighGMac.objectList, ALL_OBJECTS);
        BB_ATTACH(associationHighGMac.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
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
int addRegisterObject(
    objectArray* objects)
{
    const unsigned char ln[6] = { 1,1,21,25,0,255 };
    cosem_init2((gxObject*)&activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln);
    //10 ^ 3 =  1000
    var_setUInt32(&activePowerL1.value, 25400);
    activePowerL1.scaler = -2;
    activePowerL1.unit = 30;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject(uint16_t serializationVersion)
{
    int ret = 0;
    //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
    if (serializationVersion == 0)
    {
        const unsigned char ln[6] = { 0, 0, 1, 0, 0, 255 };
        if ((ret = INIT_OBJECT(meterData.clock1, DLMS_OBJECT_TYPE_CLOCK, ln)) == 0)
        {
            //Set default values.
            time_init(&meterData.clock1.begin, -1, 3, 0, 0, 0, 0, 0, 0);
            time_init(&meterData.clock1.end, -1, 9, 0, 0, 0, 0, 0, 0);
            //Meter is using UTC time zone.
            meterData.clock1.timeZone = 0;
            //Deviation is 60 minutes.
            meterData.clock1.deviation = 60;
            meterData.clock1.clockBase = DLMS_CLOCK_BASE_FREQUENCY_50;
        }
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
    cosem_init2((gxObject*)&udpSetup, DLMS_OBJECT_TYPE_TCP_UDP_SETUP, ln);
    udpSetup.maximumSimultaneousConnections = 1;
    udpSetup.maximumSegmentSize = 40;
    udpSetup.inactivityTimeout = 180;
    return 0;
}

#define SET_CAPTURE_OBJECT(ARR, OBJECT, INDEX, DATA_INDEX) memcpy(ARR.logicalName, OBJECT.base.logicalName, 6); ARR.objectType = OBJECT.base.objectType; ARR.objectType = OBJECT.base.objectType; ARR.attributeIndex = INDEX; ARR.dataIndex = DATA_INDEX

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addLoadProfileProfileGeneric(uint16_t serializationVersion)
{
    //Set default values if load the first time.
    if (serializationVersion == 0)
    {
        meterData.loadProfile.entriesInUse = 0;
        meterData.loadProfile.period = 0;
        meterData.loadProfile.rowIndex = 0;
        meterData.loadProfile.objects.count = 0;
    }
    int ret;
    const unsigned char ln[6] = { 1, 0, 99, 1, 0, 255 };
    if ((ret = INIT_OBJECT(loadProfile, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        loadProfile.capturePeriod = meterData.loadProfile.period;
        loadProfile.sortMethod = DLMS_SORT_METHOD_FIFO;
        //entries in use.
        loadProfile.entriesInUse = meterData.loadProfile.entriesInUse;
        ///////////////////////////////////////////////////////////////////
        ARR_ATTACH(loadProfile.captureObjects, LOAD_PROFILE_CAPTURE_OBJECT, 0);
        //Add 2 columns.
        if (serializationVersion == 0)
        {
            //Add clock obect.
            SET_CAPTURE_OBJECT(meterData.loadProfile.objects.values[0], meterData.clock1, 2, 0);
            //Add active power.
            SET_CAPTURE_OBJECT(meterData.loadProfile.objects.values[1], activePowerL1, 2, 0);
            //Update amount of capture objects.
            loadProfile.captureObjects.size = meterData.loadProfile.objects.count = 2;
        }
        loadProfile.profileEntries = meterData.loadProfile.profileEntries;
        //Set clock to sort object.
        loadProfile.sortObject = BASE(meterData.clock1);
        loadProfile.sortObjectAttributeIndex = 2;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addEventLogProfileGeneric(uint16_t serializationVersion)
{
    //Set default values if load the first time.
    if (serializationVersion == 0)
    {
        meterData.eventLog.entriesInUse = 0;
        meterData.eventLog.period = 0;
        meterData.eventLog.rowIndex = 0;
        meterData.eventLog.objects.count = 0;
    }
    int ret;
    const unsigned char ln[6] = { 1, 0, 99, 98, 0, 255 };
    if ((ret = INIT_OBJECT(eventLog, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        //events are not captured.
        eventLog.capturePeriod = 0;
        eventLog.sortMethod = DLMS_SORT_METHOD_FIFO;
        //entries in use.
        eventLog.entriesInUse = meterData.eventLog.entriesInUse;
        ARR_ATTACH(eventLog.captureObjects, EVENT_LOG_CAPTURE_OBJECT, 0);
        ///////////////////////////////////////////////////////////////////
        //Add 2 columns as default.
        if (serializationVersion == 0)
        {
            //Add clock obect.
            SET_CAPTURE_OBJECT(meterData.eventLog.objects.values[0], meterData.clock1, 2, 0);
            //Add event code.
            SET_CAPTURE_OBJECT(meterData.eventLog.objects.values[1], eventCode, 2, 0);
            //Update amount of capture objects.
            eventLog.captureObjects.size = meterData.eventLog.objects.count = 2;
        }
        eventLog.profileEntries = meterData.eventLog.profileEntries;
        //Set clock to sort object.
        eventLog.sortObject = BASE(meterData.clock1);
        eventLog.sortObjectAttributeIndex = 2;
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
    cosem_init2((gxObject*)&autoConnect, DLMS_OBJECT_TYPE_AUTO_CONNECT, ln);
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
int addActivityCalendar(
    objectArray* objects)
{
    gxDayProfile* dp;
    gxSeasonProfile* sp;
    gxWeekProfile* wp;
    gxDayProfileAction* act;

    const unsigned char ln[6] = { 0,0,13,0,0,255 };
    cosem_init2((gxObject*)&activityCalendar, DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR, ln);

    bb_addString(&activityCalendar.calendarNameActive, "Active");
    //Add season profile.
    sp = (gxSeasonProfile*)malloc(sizeof(gxSeasonProfile));
    bb_init(&sp->name);
    bb_addString(&sp->name, "Summer time");
    time_init(&sp->start, -1, 3, 31, -1, -1, -1, -1, -1);
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
    time_now(&act->startTime, 0);
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
    time_init(&sp->start, -1, 10, 30, -1, -1, -1, -1, -1);
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
    time_now(&act->startTime, 0);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    act->script = BASE(tarifficationScriptTable);
#else
    memcpy(act->scriptLogicalName, tarifficationScriptTable.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    act->scriptSelector = 1;
    arr_push(&dp->daySchedules, act);
    arr_push(&activityCalendar.dayProfileTablePassive, dp);
    time_now(&activityCalendar.time, 0);
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Optical Port Setup object.
///////////////////////////////////////////////////////////////////////
int addOpticalPortSetup(
    objectArray* objects)
{
    const unsigned char ln[6] = { 0,0,20,0,0,255 };
    cosem_init2((gxObject*)&localPortSetup, DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP, ln);
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
int addDemandRegister(
    objectArray* objects)
{
    const unsigned char ln[6] = { 1,0,31,4,0,255 };
    cosem_init2((gxObject*)&demandRegister, DLMS_OBJECT_TYPE_DEMAND_REGISTER, ln);

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
int addRegisterMonitor(
    objectArray* objects)
{
    gxActionSet* action;
    dlmsVARIANT* tmp;

    const unsigned char ln[6] = { 0,0,16,1,0,255 };
    cosem_init2((gxObject*)&registerMonitor, DLMS_OBJECT_TYPE_REGISTER_MONITOR, ln);

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

    registerMonitor.monitoredValue.attributeIndex = 2;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    registerMonitor.monitoredValue.target = BASE(activePowerL1);
#else
    registerMonitor.monitoredValue.objectType = activePowerL1.base.objectType;
    memcpy(registerMonitor.monitoredValue.logicalName, activePowerL1.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS


    //Add low action.
    action = (gxActionSet*)malloc(sizeof(gxActionSet));
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionDown.script = NULL;
#else
    memset(action->actionUp.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS

    action->actionDown.scriptSelector = 1;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionUp.script = NULL;
#else
    memset(action->actionUp.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    action->actionUp.scriptSelector = 1;
    arr_push(&registerMonitor.actions, action);
    //Add high action.
    action = (gxActionSet*)malloc(sizeof(gxActionSet));
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionDown.script = NULL;
#else
    memset(action->actionDown.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    action->actionDown.scriptSelector = 1;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    action->actionUp.script = NULL;
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
        ARR_ATTACH(actionScheduleDisconnectClose.executionTime, meterData.disconnectCloseExecutions.values, meterData.disconnectCloseExecutions.count);
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
        ARR_ATTACH(actionScheduleDisconnectOpen.executionTime, meterData.disconnectOpenExecutions.values, meterData.disconnectOpenExecutions.count);
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
int addImageTransfer(
    objectArray* objects)
{
    unsigned char ln[6] = { 0,0,44,0,0,255 };
    cosem_init2((gxObject*)&imageTransfer, DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln);
    imageTransfer.imageBlockSize = 100;
    imageTransfer.imageFirstNotTransferredBlockNumber = 0;
    //Enable image transfer.
    imageTransfer.imageTransferEnabled = 1;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add IEC HDLC Setup object.
///////////////////////////////////////////////////////////////////////
int addIecHdlcSetup(dlmsServerSettings* settings, uint16_t serializationVersion)
{
    int ret = 0;
    if (serializationVersion == 0)
    {
        unsigned char ln[6] = { 0, 0, 22, 0, 0, 255 };
        if ((ret = INIT_OBJECT(meterData.hdlc, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln)) == 0)
        {
            meterData.hdlc.communicationSpeed = DLMS_BAUD_RATE_9600;
            meterData.hdlc.windowSizeReceive = meterData.hdlc.windowSizeTransmit = 1;
            meterData.hdlc.maximumInfoLengthTransmit = meterData.hdlc.maximumInfoLengthReceive = 128;
            meterData.hdlc.inactivityTimeout = 120;
            meterData.hdlc.deviceAddress = 0x10;
        }
    }
    settings->hdlc = &meterData.hdlc;
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add compact data object.
///////////////////////////////////////////////////////////////////////
int addCompactData(
    dlmsServerSettings* settings,
    objectArray* objects)
{
    gxTarget* capture;
    gxKey* k;
    unsigned char ln[6] = { 0,0,66,0,1,255 };
    cosem_init2(&compactData.base, DLMS_OBJECT_TYPE_COMPACT_DATA, ln);
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
   //MIKKO k = key_init(&actionSchedule, capture);
   //MIKKO arr_push(&compactData.captureObjects, k);
    return compactData_updateTemplateDescription(&settings->base, &compactData);
}

///////////////////////////////////////////////////////////////////////
//Add SAP Assignment object.
///////////////////////////////////////////////////////////////////////
int addSapAssignment(uint16_t serializationVersion)
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 41, 0, 0, 255 };
    if ((ret = INIT_OBJECT(sapAssignment, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, ln)) == 0)
    {
        if (serializationVersion == 0)
        {
            char tmp[16];
            sprintf(tmp, "%s%.13lu", FLAG_ID, meterData.SERIAL_NUMBER);
            bb_addString(&meterData.sapAssignmentList.values[0].name, tmp);
            meterData.sapAssignmentList.values[0].id = 1;
            meterData.sapAssignmentList.count = 1;
        }
        ARR_ATTACH(sapAssignment.sapAssignmentList, meterData.sapAssignmentList.values, meterData.sapAssignmentList.count);
    }
    return ret;
}


///////////////////////////////////////////////////////////////////////
//Add Auto Answer object.
///////////////////////////////////////////////////////////////////////
int addAutoAnswer(
    objectArray* objects)
{
    gxtime* start, * end;
    const unsigned char ln[6] = { 0,0,2,2,0,255 };
    cosem_init2((gxObject*)&autoAnswer, DLMS_OBJECT_TYPE_AUTO_ANSWER, ln);

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
int addModemConfiguration(
    objectArray* objects)
{
    gxModemInitialisation* init;
    const unsigned char ln[6] = { 0,0,2,0,0,255 };
    cosem_init2((gxObject*)&modemConfiguration, DLMS_OBJECT_TYPE_MODEM_CONFIGURATION, ln);

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
int addMacAddressSetup(
    objectArray* objects)
{
    const unsigned char ln[6] = { 0,0,25,2,0,255 };
    cosem_init2((gxObject*)&macAddressSetup, DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, ln);
    bb_addString(&macAddressSetup.macAddress, "00:11:22:33:44:55:66");
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
int addIP4Setup(
    objectArray* objects)
{
    const unsigned char ln[6] = { 0,0,25,1,0,255 };
    cosem_init2((gxObject*)&ip4Setup, DLMS_OBJECT_TYPE_IP4_SETUP, ln);
    ip4Setup.ipAddress = getIpAddress();
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add push setup object. (On Connectivity)
///////////////////////////////////////////////////////////////////////
int addPushSetup(
    objectArray* objects)
{
    const char dest[] = "127.0.0.1:7000";

    gxTarget* co;
    const unsigned char ln[6] = { 0,0,25,9,0,255 };
    cosem_init2((gxObject*)&pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln);
    pushSetup.destination = (char*)malloc(strlen(dest) + 1);
    strcpy(pushSetup.destination, dest);

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
    return 0;
}

int svr_InitObjects(
    dlmsServerSettings* settings)
{
    int ret;
    objectArray* objects = &settings->base.objects;
    char FW[] = "Gurux FW 1.0.1";
    char buff[17];
    oa_attach(objects, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));

    ///////////////////////////////////////////////////////////////////////
    //Add Logical Device Name. 123456 is meter serial number.
    ///////////////////////////////////////////////////////////////////////
    // COSEM Logical Device Name is defined as an octet-string of 16 octets.
    // The first three octets uniquely identify the manufacturer of the device and it corresponds
    // to the manufacturer's identification in IEC 62056-21.
    // The following 13 octets are assigned by the manufacturer.
    //The manufacturer is responsible for guaranteeing the uniqueness of these octets.
    sprintf(buff, "GRX%.13lu", meterData.SERIAL_NUMBER);
    {
        const unsigned char ln[6] = { 0,0,42,0,0,255 };
        cosem_init2((gxObject*)&ldn.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_addBytes(&ldn.value, (unsigned char*)buff, 16);
    }
    //Electricity ID 1
    {
        const unsigned char ln[6] = { 1,1,0,0,0,255 };
        cosem_init2((gxObject*)&id1.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setString(&id1.value, buff, 16);
    }

    //Electricity ID 2.
    {
        const unsigned char ln[6] = { 1,1,0,0,1,255 };
        cosem_init2((gxObject*)&id2.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&id2.value, meterData.SERIAL_NUMBER);
    }

    //Firmware version.
    {
        const unsigned char ln[6] = { 1,0,0,2,0,255 };
        cosem_init2((gxObject*)&fw.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setString(&fw.value, FW, (unsigned short)strlen(FW));
    }
    if ((ret = addRegisterObject()) != 0)
    {
        return ret;
    }
    if ((ret = addClockObject(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addTcpUdpSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addProfileGeneric()) != 0)
    {
        return ret;
    }
#ifdef DLMS_INDIAN_STANDARD
    if ((ret = addInstantDataProfileGeneric()) != 0)
    {
        return ret;
    }
#endif //DLMS_INDIAN_STANDARD
    if ((ret = addAutoConnect()) != 0)
    {
        return ret;
    }
    if ((ret = addActivityCalendar()) != 0)
    {
        return ret;
    }

    if ((ret = addOpticalPortSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addDemandRegister()) != 0)
    {
        return ret;
    }
    if ((ret = addScriptTable()) != 0)
    {
        return ret;
    }
    if ((ret = addRegisterMonitor()) != 0)
    {
        return ret;
    }
    if ((ret = addActionSchedule()) != 0)
    {
        return ret;
    }
    if ((ret = addSapAssignment()) != 0)
    {
        return ret;
    }
    if ((ret = addAutoAnswer()) != 0)
    {
        return ret;
    }
    if ((ret = addModemConfiguration()) != 0)
    {
        return ret;
    }
    if ((ret = addMacAddressSetup()) != 0)
    {
        return ret;
    }

    if ((ret = addDisconnectControl()) != 0)
    {
        return ret;
    }
    if ((ret = addIP4Setup()) != 0)
    {
        return ret;
    }
    if ((ret = addPushSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addImageTransfer()) != 0)
    {
        return ret;
    }
    if ((ret = addIecHdlcSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addCompactData(settings, objects)) != 0)
    {
        return ret;
    }
    return oa_verify(objects);
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
        con->settings.hdlc = &iecHdlcSetup;
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
#ifndef DLMS_ITALIAN_STANDARD
    if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        const unsigned char CURRENT_ASSOCIATION[6] = { 0, 0, 40, 0, 0, 255 };
        if (settings->authentication == DLMS_AUTHENTICATION_NONE &&
            (memcmp(CURRENT_ASSOCIATION, ln, 6) == 0 || memcmp(ln, associationNone.base.logicalName, 6) == 0))
        {
            e->target = &associationNone.base;
        }
        else if (settings->authentication == DLMS_AUTHENTICATION_LOW &&
            (memcmp(CURRENT_ASSOCIATION, ln, 6) == 0 || memcmp(ln, lowAssociation.base.logicalName, 6) == 0))
        {
            e->target = &lowAssociation.base;
        }
        else if (memcmp(CURRENT_ASSOCIATION, ln, 6) == 0 || memcmp(ln, highAssociation.base.logicalName, 6) == 0)
        {
            e->target = &highAssociation.base;
        }
    }
    else
    {
        if (settings->authentication == DLMS_AUTHENTICATION_NONE)
        {
            return oa_findByLN(&associationNone.objectList, objectType, ln, &e->target);
        }
        else if (settings->authentication == DLMS_AUTHENTICATION_LOW)
        {
            return oa_findByLN(&lowAssociation.objectList, objectType, ln, &e->target);
        }
        else
        {
            return oa_findByLN(&highAssociation.objectList, objectType, ln, &e->target);
        }
    }
#else
    if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        if (settings->authentication == DLMS_AUTHENTICATION_NONE)
        {
            e->target = &associationNone.base;
        }
        else if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
        {
            e->target = &highAssociation.base;
        }

    }
#endif //DLMS_ITALIAN_STANDARD
    return 0;
}

/**
*  Get head position where next new item is inserted.
*
* This is used with ring buffer.
*
* @return Position where next item is inserted.
*/
unsigned short getHead() {
    unsigned short head = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    gxtime tm, last;
    int len, month = 0, day = 0, year = 1971, hour = 0, minute = 0, second = 0, value = 0;
    time_init(&last, year, month, day, hour, minute, second, 0, -1);
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, DATAFILE, "r");
#else
    FILE* f = fopen(DATAFILE, "r");
#endif
    if (f != NULL)
    {
        while ((len = fscanf(f, "%d/%d/%d %d:%d:%d;%d", &month, &day, &year, &hour, &minute, &second, &value)) != -1)
        {
            time_init(&tm, year, month, day, hour, minute, second, 0, -1);
            if (time_compare(&last, &tm) > 0)
            {
                break;
            }
            ++head;
            last = tm;
        }
        fclose(f);
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    return head;
}


/**
* Return data using start and end indexes.
*
* @param p
*            ProfileGeneric
* @param index start index.
* @param count Amount of the rows.
*/
void getProfileGenericDataByEntry(gxProfileGeneric* p, long index, long count)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    dlmsVARIANT* tmp;
    variantArray* row;
    int len, month = 0, day = 0, year = 0, hour = 0, minute = 0, second = 0, value = 0;
    if (count != 0)
    {

#if _MSC_VER > 1400
        FILE* f = NULL;
        fopen_s(&f, DATAFILE, "r");
#else
        FILE* f = fopen(DATAFILE, "r");
#endif
        if (f != NULL)
        {
            while ((len = fscanf(f, "%d/%d/%d %d:%d:%d;%d", &month, &day, &year, &hour, &minute, &second, &value)) != -1)
            {
                // Skip row
                if (index > 0)
                {
                    --index;
                }
                else if (len == 7)
                {
                    if (p->buffer.size == count)
                    {
                        break;
                    }
                    row = (variantArray*)malloc(sizeof(variantArray));
                    va_init(row);
                    arr_push(&p->buffer, row);

                    //Add date time.
                    tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
                    var_init(tmp);
                    tmp->dateTime = (gxtime*)malloc(sizeof(gxtime));
                    time_init(tmp->dateTime, year, month, day, hour, minute, second, 0, -1);
                    tmp->vt = DLMS_DATA_TYPE_DATETIME;
                    va_push(row, tmp);

                    //Add register value.
                    tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
                    var_init(tmp);
                    var_setInt32(tmp, value);
                    va_push(row, tmp);
                }
                if (p->buffer.size == count)
                {
                    break;
                }
            }
            fclose(f);
        }
        //Read values from the begin if ring buffer is used.
        if (p->buffer.size != count)
        {
            getProfileGenericDataByEntry(p, index, count);
        }
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
}

/**
* Get row count.
*

* @return
*/
int getProfileGenericDataCount() {
    int rows = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    int ch;
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, DATAFILE, "r");
#else
    FILE* f = fopen(DATAFILE, "r");
#endif
    if (f != NULL)
    {
        while ((ch = fgetc(f)) != EOF)
        {
            if (ch == '\n')
            {
                ++rows;
            }
        }
        fclose(f);
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    return rows;
}


/**
* Find start index and row count using start and end date time.
*
* @param start
*            Start time.
* @param end
*            End time
* @param index
*            Start index.
* @param count
*            Item count.
*/
int GetProfileGenericDataByRangeFromRingBuffer(gxValueEventArg* e)
{
    int len, month = 0, day = 0, year = 1971, hour = 0, minute = 0, second = 0, value = 0;
    dlmsVARIANT* it;
    gxtime tm, start, end, last;
    int ret;
    unsigned short pos = 0;
    dlmsVARIANT tmp;
    var_init(&tmp);
    time_init(&last, year, month, day, hour, minute, second, 0, -1);
    if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) != 0)
    {
        return ret;
    }
    if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp)) != 0)
    {
        var_clear(&tmp);
        return ret;
    }
    //Start time.
    start = *tmp.dateTime;
    var_clear(&tmp);
    if ((ret = va_getByIndex(e->parameters.Arr, 2, &it)) != 0)
    {
        return ret;
    }
    if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp)) != 0)
    {
        var_clear(&tmp);
        return ret;
    }
    end = *tmp.dateTime;
    var_clear(&tmp);

#if defined(_DEBUG) && !defined(GX_DLMS_MICROCONTROLLER)
    gxByteBuffer bb;
    bb_init(&bb);
    time_toString(&start, &bb);
    time_toString(&end, &bb);
    printf("Reading rows %s\n", bb.data);
    bb_clear(&bb);
#endif
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)

#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, DATAFILE, "r");
#else
    FILE* f = fopen(DATAFILE, "r");
#endif
    if (f != NULL)
    {
        while ((len = fscanf(f, "%d/%d/%d %d:%d:%d;%d", &month, &day, &year, &hour, &minute, &second, &value)) != -1)
        {
            //Skip emmpty lines.
            if (len == 7)
            {
                time_init(&tm, year, month, day, hour, minute, second, 0, -1);
                //If value is inside of start and end time.
                if (time_compare(&tm, &start) >= 0 && time_compare(&tm, &end) <= 0)
                {
                    if (time_getYears(&last) == 1971)
                    {
                        e->transactionStartIndex = pos;
                        //Save end position if we have only one row.
                        e->transactionEndIndex = pos + 1;
                    }
                    else
                    {
                        if (time_compare(&tm, &last) > 0)
                        {
                            e->transactionEndIndex = pos + 1;
                        }
                        else
                        {
                            gxProfileGeneric* p = (gxProfileGeneric*)e->target;
                            if (e->transactionEndIndex == 0)
                            {
                                ++e->transactionEndIndex;
                            }
                            e->transactionEndIndex += getProfileGenericDataCount(p);
                            e->transactionStartIndex = pos;
                            break;
                        }
                    }
                    time_copy(&last, &tm);
                }
                ++pos;
            }
            else
            {
                break;
            }
        }
        fclose(f);
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    return 0;
}

/**
* Find restricting object.
*/
int getRestrictingObject(dlmsSettings* settings, gxValueEventArg* e, gxObject** obj, short* index)
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
* Find start index and row count using start and end date time.
*
* @param e
*            Start and end time are get from the parameters.
*/
int getProfileGenericDataByRange(dlmsSettings* settings, gxValueEventArg* e)
{
    int len, month = 0, day = 0, year = 0, hour = 0, minute = 0, second = 0, value = 0;
    dlmsVARIANT* it;
    gxtime tm, start, end;
    int ret;
    dlmsVARIANT tmp;
    var_init(&tmp);
    gxObject* obj = NULL;
    short index;
    getRestrictingObject(settings, e, &obj, &index);
    if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) != 0)
    {
        return ret;
    }
    if (it->vt == DLMS_DATA_TYPE_UINT32)
    {
        time_initUnix(&start, (time_t)it->ulVal);
    }
    else
    {
        if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp)) != 0)
        {
            var_clear(&tmp);
            return ret;
        }
        //Start time.
        start = *tmp.dateTime;
        var_clear(&tmp);
    }
    if ((ret = va_getByIndex(e->parameters.Arr, 2, &it)) != 0)
    {
        return ret;
    }
    if (it->vt == DLMS_DATA_TYPE_UINT32)
    {
        time_initUnix(&end, (time_t)it->ulVal);
    }
    else
    {
        if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp)) != 0)
        {
            var_clear(&tmp);
            return ret;
        }
        end = *tmp.dateTime;
        var_clear(&tmp);
    }
#if defined(_DEBUG) && !defined(GX_DLMS_MICROCONTROLLER)
    gxByteBuffer bb;
    bb_init(&bb);
    time_toString(&start, &bb);
    time_toString(&end, &bb);
    printf("Reading rows %s\n", bb.data);
    bb_clear(&bb);
#endif
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)

#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, DATAFILE, "r");
#else
    FILE* f = fopen(DATAFILE, "r");
#endif
    if (f != NULL)
    {
        while ((len = fscanf(f, "%d/%d/%d %d:%d:%d;%d", &month, &day, &year, &hour, &minute, &second, &value)) != -1)
        {
            //Skip emmpty lines.
            if (len == 7)
            {
                time_init(&tm, year, month, day, hour, minute, second, 0, -1);
                if (time_compare(&tm, &end) > 0)
                {
                    // If all data is read.
                    break;
                }
                if (time_compare(&tm, &start) < 0)
                {
                    // If we have not find first item.
                    ++e->transactionStartIndex;
                }
                ++e->transactionEndIndex;
            }
            else
            {
                break;
            }
        }
        fclose(f);
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    return 0;
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

        if (e->target == &profileGeneric.base)
        {
            gxProfileGeneric* p = (gxProfileGeneric*)e->target;
            // If buffer is read and we want to save memory.
            if (e->index == 7)
            {
                // If client wants to know EntriesInUse.
                p->entriesInUse = getProfileGenericDataCount();
            }
            else if (e->index == 2)
            {
                // Read rows from file.
                // If reading first time.
                if (e->transactionEndIndex == 0)
                {
                    if (e->selector == 0)
                    {
                        e->transactionEndIndex = getProfileGenericDataCount();
                    }
                    else if (e->selector == 1)
                    {
                        //Read by entry.
                        if (useRingBuffer)
                        {
                            GetProfileGenericDataByRangeFromRingBuffer(e);
                        }
                        else
                        {
                            getProfileGenericDataByRange(settings, e);
                        }
                    }
                    else if (e->selector == 2)
                    {
                        dlmsVARIANT* it;
                        if ((ret = va_getByIndex(e->parameters.Arr, 0, &it)) != 0)
                        {
                            continue;
                        }
                        unsigned int begin = var_toInteger(it);
                        if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) != 0)
                        {
                            continue;
                        }
                        e->transactionStartIndex = begin;
                        e->transactionEndIndex = begin + var_toInteger(it);
                        // If client wants to read more data what we have.
                        int cnt = getProfileGenericDataCount();
                        if (e->transactionEndIndex - e->transactionStartIndex > cnt - e->transactionStartIndex)
                        {
                            if (useRingBuffer)
                            {
                                e->transactionEndIndex = cnt;
                            }
                            else
                            {
                                e->transactionEndIndex = cnt - e->transactionStartIndex;
                            }
                            if (e->transactionEndIndex < 0)
                            {
                                e->transactionEndIndex = 0;
                            }
                        }
                    }
                }
                unsigned long count = e->transactionEndIndex - e->transactionStartIndex;
                // Read only rows that can fit to one PDU.
                if (e->transactionEndIndex - e->transactionStartIndex > p->maxRowCount)
                {
                    /**
                    * Max row count is used with Profile Generic to tell how many rows are read
                    * to one PDU. Default value is 1. Change this for your needs.
                    */
                    count = p->maxRowCount;
                }
                // Clear old data. It's already serialized.
                obj_clearProfileGenericBuffer(&p->buffer);

                if (e->selector == 1)
                {
                    getProfileGenericDataByEntry(p, e->transactionStartIndex, count);
                }
                else
                {
                    //Index where to start.
                    unsigned long index = e->transactionStartIndex;
                    if (useRingBuffer)
                    {
                        index += getHead();
                    }
                    getProfileGenericDataByEntry(p, index, count);
                }
            }
            continue;
        }

        //Update date and time of clock object.
        if (e->target == (gxObject*)&clock1 && e->index == 2)
        {
            time_now(&((gxClock*)e->target)->time, 1);
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
#ifndef GX_DLMS_MICROCONTROLLER
        hlp_getLogicalNameToString(e->target->logicalName, str);
        printf("Writing %s\r\n", str);
#endif //GX_DLMS_MICROCONTROLLER
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
#ifndef GX_DLMS_MICROCONTROLLER
        //Loop excution times in write.
        if (e->target == &actionSchedule.base && e->index == 4)
        {
            int pos;
            dlmsVARIANT* it;
            gxByteBuffer bb;
            bb_init(&bb);
            for (pos = 0; pos != e->value.Arr->size; ++pos)
            {
                if (va_getByIndex(e->value.Arr, pos, &it) != 0 ||
                    var_toString(it, &bb) != 0)
                {
                    e->error = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                    break;
                }
                char* tmp = bb_toString(&bb);
                printf("Writing %s\r\n", tmp);
                free(tmp);
                bb_clear(&bb);
            }
            break;
        }
#endif //GX_DLMS_MICROCONTROLLER
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
}

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
int sendPush(dlmsSettings* settings, gxPushSetup* push);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

//In this example we wait 5 seconds before image is verified or activated.
time_t imageActionStartTime;
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
            handleLoadProfileActions(e);
            save(&meterData.loadProfile, sizeof(meterData.loadProfile));
            e->handled = 1;
        }
        else if (e->target == BASE(eventLog))
        {
            handleEventLogActions(e);
            save(&meterData.eventLog, sizeof(meterData.eventLog));
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
            createObjects();
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
            meterData.testMode = 1;
            save(&meterData.testMode, sizeof(meterData.testMode));
        }
        else if (e->target == BASE(scriptTableActivateNormalMode))
        {
            //Activate normal mode.
            meterData.testMode = 0;
            save(&meterData.testMode, sizeof(meterData.testMode));
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
                    (ret = cosem_getOctectString2(e->parameters.byteArr, info->identification.data, sizeof(info->identification.data), &info->identification.size)) != 0 ||
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
        if (e->target == BASE(meterData.clock1))
        {
            save(&meterData.clock1, sizeof(meterData.clock1));
        }
        else if (e->target == BASE(actionScheduleDisconnectOpen))
        {
            meterData.disconnectOpenExecutions.count = (unsigned char)actionScheduleDisconnectOpen.executionTime.size;
            save(&meterData.disconnectOpenExecutions, sizeof(meterData.disconnectOpenExecutions));
        }
        else if (e->target == BASE(actionScheduleDisconnectClose))
        {
            meterData.disconnectCloseExecutions.count = (unsigned char)actionScheduleDisconnectClose.executionTime.size;
            save(&meterData.disconnectCloseExecutions, sizeof(meterData.disconnectCloseExecutions));
        }
        else if (e->target == BASE(meterData.hdlc))
        {
            save(&meterData.hdlc, sizeof(meterData.hdlc));
        }
        else if (e->target == BASE(loadProfile))
        {
            //Use want to change capture objects.
            if (e->index == 3)
            {
                saveTargets(&meterData.loadProfile.objects, LOAD_PROFILE_CAPTURE_OBJECT, loadProfile.captureObjects.size);
                //Clear buffer if user changes captured objects.
                gxValueEventArg it;
                ve_init(&it);
                it.index = 1;
                handleLoadProfileActions(&it);
                //Count how many rows fit to the buffer.
                loadProfile.profileEntries = meterData.loadProfile.profileEntries = getProfileGenericBufferMaxRowCount(&loadProfile);
                if (loadProfile.captureObjects.size != 0 &&
                    (LOAD_PROFILE_CAPTURE_OBJECT[0].target == BASE(meterData.clock1) ||
                        LOAD_PROFILE_CAPTURE_OBJECT[0].target == BASE(unixTime)))
                {
                    //Set 1st object to sort object.
                    loadProfile.sortObject = LOAD_PROFILE_CAPTURE_OBJECT[0].target;
                }
                else
                {
                    loadProfile.sortObject = NULL;
                }
            }
            //User wants to change capture period.
            if (e->index == 4)
            {
                meterData.loadProfile.period = loadProfile.capturePeriod;
            }
            //Use want to change max amount of profile entries.
            if (e->index == 8)
            {
                //Count how many rows fit to the buffer.
                uint16_t maxCount = getProfileGenericBufferMaxRowCount(&loadProfile);
                //If use try to set max profileEntries bigger than can fit to EEPROM.
                if (maxCount < loadProfile.profileEntries)
                {
                    loadProfile.profileEntries = maxCount;
                }
                meterData.loadProfile.profileEntries = (unsigned short)loadProfile.profileEntries;
            }
            save(&meterData.loadProfile, sizeof(meterData.loadProfile));
        }
        else if (e->target == BASE(pushSetup))
        {
            meterData.push.communicationWindow.count = (unsigned char)pushSetup.communicationWindow.size;
            saveTargets(&meterData.push.objects, PUSH_OBJECTS, pushSetup.pushObjectList.size);
            save(&meterData.push, sizeof(meterData.push));
        }
        else if (e->target == BASE(associationLow))
        {
            //Save LLS password.
            meterData.association.llsPasswordSize = (char)associationLow.secret.size;
            save(&meterData.association, sizeof(GXAssociation));
        }
    }
    //Reset execute time to update execute time if user add new execute times or changes the time.
   //MIKKO executeTime = 0;
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
        if (e->target == BASE(associationHigh) && e->index == 2)
        {
            //Save HLS passwords.
            meterData.association.hlsPasswordSize = (char)associationHigh.secret.size;
            save(&meterData.association, sizeof(GXAssociation));
        }
        else if (e->target == BASE(associationHighGMac) && e->index == 2)
        {
            //Save HLS passwords.
            meterData.association.hlsPasswordSize = (char)associationHighGMac.secret.size;
            save(&meterData.association, sizeof(GXAssociation));
        }
        else if (e->target == BASE(securitySetupHigh) ||
            e->target == BASE(securitySetupHighGMac))
        {
            //Update block cipher key.
            //MIKKO memcpy(meterData.securitySettings.blockCipherKey, settings->cipher.blockCipherKey, sizeof(meterData.securitySettings.blockCipherKey));
            //Update authentication key.
            //MIKKO memcpy(meterData.securitySettings.authenticationKey, settings->cipher.authenticationKey, sizeof(meterData.securitySettings.authenticationKey));
            save(&meterData.securitySettings, sizeof(GXSecuritySettings));
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
    p = strchr(push->destination, ':');
    if (p == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pos = (int)(p - push->destination);
    host = (char*)malloc(pos + 1);
    memcpy(host, push->destination, pos);
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
#ifdef DLMS_ITALIAN_STANDARD
    if (clientAddress == 1 || clientAddress == 16 || clientAddress == 3 || clientAddress == 48)
    {
        const unsigned char ss[6] = { 0, 0, 43, 0, (unsigned char)clientAddress, 255 };
        memcpy(securitySetup.base.logicalName, ss, 6);
        const unsigned char ln[6] = { 0, 0, 40, 0, (unsigned char)clientAddress, 255 };
        memcpy(highAssociation.base.logicalName, ln, 6);
        memcpy(highAssociation.securitySetupReference, securitySetup.base.logicalName, 6);
    }
    else
    {
        printf("Invalid HDLC address %d.\r\n", clientAddress);
        return 0;
    }
    //Update invocation counter.
    if (clientAddress == 1)
    {
        settings->cipher.invocationCounter = frameCounterOnLine.value.lVal;
    }
    else  if (clientAddress == 3)
    {
        settings->cipher.invocationCounter = frameCounterInstaller.value.lVal;
    }
    else  if (clientAddress == 48)
    {
        settings->cipher.invocationCounter = frameCounterGuarantor.value.lVal;
    }
#endif //DLMS_ITALIAN_STANDARD

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
                        break;
                    case DLMS_AUTHENTICATION_LOW:
                        //Client connects using low authentication.
                        break;
                    default:
                        //Client connects using High authentication.
                        break;
                    }
                    break;
                }
            }
        }
    }
    if (ret == 0)
    {
        //Authentication is now allowed. Meter is quiet and doesn't return an error.
    }
    else
    {
        // If address is not broadcast or serial number.
        if (!(serverAddress == 0x3FFF || serverAddress == 0x7F ||
            (serverAddress & 0x3FFF) == meterData.SERIAL_NUMBER % 10000 + 1000))
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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    printf("Disconnected %d.\r\n", settings->base.connected);
#endif
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