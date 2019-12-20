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
#include "../dlms/include/converters.h"
#include "../dlms/include/helpers.h"
#include "../dlms/include/date.h"
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
#include "../dlms/include/converters.h"
#include "../dlms/include/helpers.h"
#include "../dlms/include/date.h"
#endif

#include "../include/exampleserver.h"
#include "../dlms/include/cosem.h"
#include "../dlms/include/gxkey.h"
#include "../dlms/include/gxobjects.h"
#include "../dlms/include/gxget.h"
#include "../dlms/include/notify.h"
#include "../dlms/include/gxset.h"
#include "../dlms/include/gxinvoke.h"

#define EOL '\r'

//Meter serial number.
unsigned long SERIAL_NUMBER = 123456;
//Define Logical Device Name.
char LDN[17] = "Gurux";

//Active power value. We are updating this.
unsigned long activePowerL1Value = 0;

/*Define all COSEM objects here so they are not use stack.*/
gxData ldn, id1, id2, fw;
gxAssociationLogicalName lowAssociation, associationNone, highAssociation;

gxImageTransfer imageTransfer;
gxRegister activePowerL1;
gxExtendedRegister extendedRegister;
//Don't use clock name. Some compilers are using clock as reserved word.
gxClock clock1;
gxTcpUdpSetup udpSetup;
gxProfileGeneric profileGeneric;
gxAutoConnect autoConnect;
gxActivityCalendar activityCalendar;
gxLocalPortSetup localPortSetup;
gxDemandRegister demandRegister;
gxRegisterMonitor registerMonitor;
gxActionSchedule actionSchedule;
gxSapAssignment sapAssignment;
gxAutoAnswer autoAnswer;
gxModemConfiguration modemConfiguration;
gxMacAddressSetup macAddressSetup;
gxDisconnectControl disconnectControl;
gxIp4Setup ip4Setup;
gxPushSetup pushSetup;
gxScriptTable scriptTable;
gxIecHdlcSetup iecHdlcSetup;
gxCompactData compactData;

gxAccount account;
gxCharge charge;
gxCredit credit;
gxGPRSSetup gprsModemSetup;
gxGsmDiagnostic gsmDiagnostic;
gxLimiter limiter;
gxMBusClient mbusClient;
gxMBusMasterPortSetup mBusMasterPortSetup;
gxMbusSlavePortSetup mbusSlavePortSetup;

gxParameterMonitor parameterMonitor;
gxPppSetup pppSetup;
gxTokenGateway tokenGateway;
gxRegisterActivation registerActivation;

int imageSize = 0;

gxSecuritySetup securitySetup;

gxSchedule schedule;
gxSpecialDaysTable specialDays;

gxArray captureObjects;


//All objects.
const gxObject* ALL_OBJECTS[] = { BASE(ldn), BASE(id1), BASE(id2), BASE(fw), BASE(imageTransfer), BASE(activePowerL1), BASE(clock1), BASE(udpSetup),
    BASE(autoConnect), BASE(activityCalendar), BASE(localPortSetup), BASE(demandRegister), BASE(registerMonitor),
    BASE(actionSchedule), BASE(sapAssignment), BASE(autoAnswer), BASE(modemConfiguration), BASE(macAddressSetup),
    BASE(disconnectControl), BASE(ip4Setup), BASE(pushSetup), BASE(scriptTable), BASE(iecHdlcSetup), BASE(compactData),
    BASE(associationNone), BASE(highAssociation),
    BASE(securitySetup),
    BASE(lowAssociation),
    BASE(account),
    BASE(charge),
    BASE(credit),
    BASE(extendedRegister),
    BASE(gprsModemSetup),
    BASE(gsmDiagnostic),
    BASE(limiter),
    BASE(mbusClient),
    BASE(mBusMasterPortSetup),
    BASE(mbusSlavePortSetup),
    BASE(parameterMonitor),
    BASE(pppSetup),
    BASE(tokenGateway),
    BASE(registerActivation),
    BASE(profileGeneric),
    BASE(schedule),
    BASE(specialDays),
};

//Returns the approximate processor time in ms.
long time_elapsed(void)
{
    return (long)clock() * 1000;
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

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for None authentication.
// PC in Indian standard.
///////////////////////////////////////////////////////////////////////
int addNoneAssociation()
{
    int ret;
    //User list.
    static gxUser USER_LIST[10] = { 0 };
    //Dedicated key.
    static char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
    if ((ret = INIT_OBJECT(associationNone, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        //Only Logical Device Name is add to this Association View.
        //Use this if you  need to save heap.
        OA_ATTACH(associationNone.objectList, ALL_OBJECTS);
        associationNone.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        ARR_ATTACH(associationNone.userList, USER_LIST, 0);
        BB_ATTACH(associationNone.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        associationNone.clientSAP = 0x10;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for Low authentication.
// MR in Indian standard.
///////////////////////////////////////////////////////////////////////
int addLowAssociation()
{
    int ret;
    //User list.
    static gxUser USER_LIST[10] = { 0 };
    //Define low level password.
    static char LLS_PASSWORD[20] = "Gurux";
    //Dedicated key.
    static char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 2, 255 };
    if ((ret = INIT_OBJECT(lowAssociation, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        BB_ATTACH(lowAssociation.secret, LLS_PASSWORD, (unsigned short)strlen(LLS_PASSWORD));
        lowAssociation.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_LOW;
        OA_ATTACH(lowAssociation.objectList, ALL_OBJECTS);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        lowAssociation.securitySetup = &securitySetup;
#else
        memcpy(lowAssociation.securitySetupReference, securitySetup.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        BB_ATTACH(lowAssociation.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        ARR_ATTACH(lowAssociation.userList, USER_LIST, 0);
        lowAssociation.clientSAP = 0x20;
        BB_ATTACH(lowAssociation.secret, LLS_PASSWORD, (unsigned short)strlen(LLS_PASSWORD));
        //All objects are add for this Association View later.
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for High authentication.
// UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addHighAssociation()
{
    int ret;
    //User list.
    static gxUser USER_LIST[10] = { 0 };
    //Dedicated key.
    static char CYPHERING_INFO[20] = { 0 };
    const unsigned char ln[6] = { 0, 0, 40, 0, 3, 255 };
    if ((ret = INIT_OBJECT(highAssociation, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        highAssociation.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH;
        OA_ATTACH(highAssociation.objectList, ALL_OBJECTS);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        highAssociation.securitySetup = &securitySetup;
#else
        memcpy(highAssociation.securitySetupReference, securitySetup.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        BB_ATTACH(highAssociation.xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);
        //All objects are add for this Association View later.
        ARR_ATTACH(highAssociation.userList, USER_LIST, 0);
        highAssociation.clientSAP = 0x30;
        static char HLS_PASSWORD[20] = "Gurux";
        BB_ATTACH(highAssociation.secret, HLS_PASSWORD, (unsigned short)strlen(HLS_PASSWORD));
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example security setup object.
///////////////////////////////////////////////////////////////////////
int addSecuritySetup()
{
    int ret;
    //Define server system title.
    static unsigned char SERVER_SYSTEM_TITLE[8] = { 0 };
    //Define client system title.
    static unsigned char CLIENT_SYSTEM_TITLE[8] = { 0 };
    const unsigned char ln[6] = { 0,0,43,0,1,255 };
    if ((ret = INIT_OBJECT(securitySetup, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
    {
        //Add FLAG ID.
        memcpy(SERVER_SYSTEM_TITLE, "GRX", 3);
        //ADD serial number.
        memcpy(SERVER_SYSTEM_TITLE + 4, &SERIAL_NUMBER, 4);
        BB_ATTACH(securitySetup.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
        BB_ATTACH(securitySetup.clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
        securitySetup.securityPolicy = DLMS_SECURITY_POLICY_NOTHING;
        securitySetup.securitySuite = DLMS_SECURITY_SUITE_AES_GCM_128;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds account object.
///////////////////////////////////////////////////////////////////////
int addAccount()
{
    static unsigned char CREDIT_REFERENCES[10][20] = { 0 };
    static unsigned char CHARGE_REFERENCES[10][20] = { 0 };
    static gxCreditChargeConfiguration CREDIT_CHARGE_CONFIGURATIONS[10] = { 0 };
    static gxTokenGatewayConfiguration TOKEN_GATEWAY_CONFIGURATIONS[10] = { 0 };
    int ret;
    const unsigned char ln[6] = { 0,0,19,0,0, 255 };
    if ((ret = INIT_OBJECT(account, DLMS_OBJECT_TYPE_ACCOUNT, ln)) == 0)
    {
        ARR_ATTACH(account.creditReferences, CREDIT_REFERENCES, 0);
        ARR_ATTACH(account.chargeReferences, CHARGE_REFERENCES, 0);
        ARR_ATTACH(account.creditChargeConfigurations, CREDIT_CHARGE_CONFIGURATIONS, 0);
        ARR_ATTACH(account.tokenGatewayConfigurations, TOKEN_GATEWAY_CONFIGURATIONS, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds account object.
///////////////////////////////////////////////////////////////////////
int addRegisterActivation()
{
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    static gxObjectDefinition REGISTER_ASSIGNMENTS[10] = { 0 };
#else
    static gxObject* REGISTER_ASSIGNMENTS[10] = { 0 };
#endif //DLMS_IGNORE_OBJECT_POINTERS
    int ret;
    const unsigned char ln[6] = { 0,0,14,0,1,255 };
    if ((ret = INIT_OBJECT(registerActivation, DLMS_OBJECT_TYPE_REGISTER_ACTIVATION, ln)) == 0)
    {
        ARR_ATTACH(registerActivation.registerAssignment, REGISTER_ASSIGNMENTS, 0);
    }
    return ret;
}


///////////////////////////////////////////////////////////////////////
//This method adds charge object.
///////////////////////////////////////////////////////////////////////
int addCharge()
{
    int ret;
    static unsigned char CHARGE_CONFIGURATION[20] = { 0 };
    static gxChargeTable ACTIVE_CHARGE_TABLES[10] = { 0 };
    static gxChargeTable PASSIVE_CHARGE_TABLES[10] = { 0 };
    const unsigned char ln[6] = { 0, 0, 19, 20, 0, 255 };
    if ((ret = INIT_OBJECT(charge, DLMS_OBJECT_TYPE_CHARGE, ln)) == 0)
    {
        BIT_ATTACH(charge.chargeConfiguration, CHARGE_CONFIGURATION, 0);
        ARR_ATTACH(charge.unitChargeActive.chargeTables, ACTIVE_CHARGE_TABLES, 0);
        ARR_ATTACH(charge.unitChargePassive.chargeTables, PASSIVE_CHARGE_TABLES, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds credit object.
///////////////////////////////////////////////////////////////////////
int addCredit()
{
    static unsigned char CREDIT_CONFIGURATION[20] = { 0 };
    int ret;
    const unsigned char ln[6] = { 0, 0, 19, 10, 0, 255 };
    if ((ret = INIT_OBJECT(credit, DLMS_OBJECT_TYPE_CREDIT, ln)) == 0)
    {
        BIT_ATTACH(credit.creditConfiguration, CREDIT_CONFIGURATION, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds GPRS Modem Setup object.
///////////////////////////////////////////////////////////////////////
int addGprsModemSetup()
{
    int ret;
    //APN
    static unsigned char APN[20] = "APN-Gurux";
    const unsigned char ln[6] = { 0, 0, 25, 4, 0, 255 };
    if ((ret = INIT_OBJECT(gprsModemSetup, DLMS_OBJECT_TYPE_GPRS_SETUP, ln)) == 0)
    {
        BB_ATTACH(gprsModemSetup.apn, APN, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds GSM Diagnostic object.
///////////////////////////////////////////////////////////////////////
int addGsmDiagnostic()
{
    int ret;
    static gxAdjacentCell ADJACENT_CELLS[10] = { 0 };
    static char OPERATOR_NAME[40] = { 0 };
    const unsigned char ln[6] = { 0, 0, 25, 6, 0, 255 };
    if ((ret = INIT_OBJECT(gsmDiagnostic, DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC, ln)) == 0)
    {
        BB_ATTACH(gsmDiagnostic.operatorName, (unsigned char*)OPERATOR_NAME, 0);
        strcpy(OPERATOR_NAME, "Gurux Ltd");
        ARR_ATTACH(gsmDiagnostic.adjacentCells, ADJACENT_CELLS, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds limiter object.
///////////////////////////////////////////////////////////////////////
int addLimiter()
{
    int ret;
    static unsigned short EMERGENCY_PROFILE_GROUP_IDS[10];
    const unsigned char ln[6] = { 0, 0, 17, 0, 0, 255 };
    if ((ret = INIT_OBJECT(limiter, DLMS_OBJECT_TYPE_LIMITER, ln)) == 0)
    {
        ARR_ATTACH(limiter.emergencyProfileGroupIDs, EMERGENCY_PROFILE_GROUP_IDS, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds M-BUS client object.
///////////////////////////////////////////////////////////////////////
int addMbusClient()
{
    static gxCaptureDefinition CAPTURE_DEFINITION[10] = { 0 };
    int ret;
    const unsigned char ln[6] = { 0, 2, 24, 1, 0, 255 };
    if ((ret = INIT_OBJECT(mbusClient, DLMS_OBJECT_TYPE_MBUS_CLIENT, ln)) == 0)
    {
        ARR_ATTACH(mbusClient.captureDefinition, CAPTURE_DEFINITION, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds M-BUS master port setup object.
///////////////////////////////////////////////////////////////////////
int addMBusMasterPortSetup()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 24, 6, 0, 255 };
    if ((ret = INIT_OBJECT(mBusMasterPortSetup, DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP, ln)) == 0)
    {
        mBusMasterPortSetup.commSpeed = DLMS_BAUD_RATE_2400;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds M-BUS slave port setup object.
///////////////////////////////////////////////////////////////////////
int addMBusSlavePortSetup()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 24, 0, 0,255 };
    if ((ret = INIT_OBJECT(mbusSlavePortSetup, DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP, ln)) == 0)
    {
        mbusSlavePortSetup.defaultBaud = DLMS_BAUD_RATE_2400;
        mbusSlavePortSetup.availableBaud = DLMS_BAUD_RATE_2400;
        mbusSlavePortSetup.addressState = DLMS_ADDRESS_STATE_NONE;
        mbusSlavePortSetup.busAddress = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds parameter monitor object.
///////////////////////////////////////////////////////////////////////
int addParameterMonitor()
{
    int ret;
    static gxChangedParameter PARAMETERS[10] = { 0 };
    const unsigned char ln[6] = { 0, 0, 16 ,2, 0, 255 };
    if ((ret = INIT_OBJECT(parameterMonitor, DLMS_OBJECT_TYPE_PARAMETER_MONITOR, ln)) == 0)
    {
        ARR_ATTACH(parameterMonitor.parameters, PARAMETERS, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds PPP Setup object.
///////////////////////////////////////////////////////////////////////
int addPppSetup()
{
    unsigned char USER_NAME[20] = { 0 };
    unsigned char PASSWORD[20] = { 0 };
    static gxpppSetupLcpOption OPTIONS[10] = { 0 };
    int ret;
    const unsigned char ln[6] = { 0, 0, 25, 3, 0, 255 };
    if ((ret = INIT_OBJECT(pppSetup, DLMS_OBJECT_TYPE_PPP_SETUP, ln)) == 0)
    {
        ARR_ATTACH(pppSetup.lcpOptions, OPTIONS, 0);
        BB_ATTACH(pppSetup.userName, USER_NAME, 0);
        BB_ATTACH(pppSetup.password, PASSWORD, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds token gateway object.
///////////////////////////////////////////////////////////////////////
int addTokenGateway()
{
    int ret;
    static unsigned char TOKEN[32] = { 0 };
    static gxTokenGatewayDescription  DESCRIPTIONS[5] = { 0 };
    const unsigned char ln[6] = { 0, 0, 19, 40, 0, 255 };
    static unsigned char dataValue[10] = { 0 };
    if ((ret = INIT_OBJECT(tokenGateway, DLMS_OBJECT_TYPE_TOKEN_GATEWAY, ln)) == 0)
    {
        BB_ATTACH(tokenGateway.token, TOKEN, 0);
        ARR_ATTACH(tokenGateway.descriptions, DESCRIPTIONS, 0);
        BIT_ATTACH(tokenGateway.dataValue, dataValue, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
    int ret;
    const unsigned char ln[6] = { 1,1,21,25,0,255 };
    if ((ret = INIT_OBJECT(activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln)) == 0)
    {
        GX_UINT32_BYREF(activePowerL1.value) = &activePowerL1Value;
        //10 ^ 3 =  1000
        activePowerL1.scaler = -2;
        activePowerL1.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds extended register object.
///////////////////////////////////////////////////////////////////////
int addExtendedRegisterObject()
{
    int ret;
    const unsigned char ln[6] = { 1,0,1,5,0,255 };
    if ((ret = INIT_OBJECT(extendedRegister, DLMS_OBJECT_TYPE_EXTENDED_REGISTER, ln)) == 0)
    {
        GX_UINT16(extendedRegister.value) = 25400;
        //10 ^ 3 =  1000
        extendedRegister.scaler = -2;
        extendedRegister.unit = DLMS_UNIT_ACTIVE_ENERGY;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject()
{
    int ret;
    //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
    const unsigned char ln[6] = { 0,0,1,0,0,255 };
    if ((ret = INIT_OBJECT(clock1, DLMS_OBJECT_TYPE_CLOCK, ln)) == 0)
    {
        time_init(&clock1.begin, -1, 9, 1, -1, -1, -1, -1, 0x8000);
        time_init(&clock1.end, -1, 3, 1, -1, -1, -1, -1, 0x8000);
        //Meter is using UTC time zone.
        clock1.timeZone = 0;
        //Deviation is 60 minutes.
        clock1.deviation = 60;
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
        udpSetup.maximumSimultaneousConnections = 1;
        udpSetup.maximumSegmentSize = 40;
        udpSetup.inactivityTimeout = 180;
        udpSetup.ipSetup = BASE(ip4Setup);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addProfileGeneric(dlmsServerSettings* settings)
{
    int ret;
    gxtime tm;
    //Two capture objects.
    static gxTarget CAPTURE_OBJECT[2] = { 0 };
    const unsigned char ln[6] = { 1,0,99,1,0,255 };
    if ((ret = INIT_OBJECT(profileGeneric, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
    {
        //Set capture period to 60 second.
        profileGeneric.capturePeriod = 60;
        //Maximum row count.
        profileGeneric.profileEntries = 100;
        profileGeneric.sortMethod = DLMS_SORT_METHOD_FIFO;
        ///////////////////////////////////////////////////////////////////
        //Add 2 columns.
        ARR_ATTACH(profileGeneric.captureObjects, CAPTURE_OBJECT, 2);
        //Add clock obect.
        CAPTURE_OBJECT[0].target = &clock1.base;
        CAPTURE_OBJECT[0].attributeIndex = 2;
        CAPTURE_OBJECT[0].dataIndex = 0;
        //Add active power.
        CAPTURE_OBJECT[1].target = &activePowerL1.base;
        CAPTURE_OBJECT[1].attributeIndex = 2;
        CAPTURE_OBJECT[1].dataIndex = 0;

        // Update date times.
        // Save values in UTC format.
        // It's important to save values in UTC format because the client might change the time zone afterward.
        unsigned short rowCount = 10000;
        time_initUnix(&tm, (unsigned long)time(NULL));
        time_clearMinutes(&tm);
        time_clearSeconds(&tm);
        time_addHours(&tm, -(rowCount - 1));
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
        FILE * f = NULL;
        fopen_s(&f, DATAFILE, "w");
#else
        FILE* f = fopen(DATAFILE, "w");
#endif
        for (int pos = 0; pos != rowCount; ++pos)
        {
            fprintf(f, "%d/%d/%d %d:%d:%d;%d\r", time_getMonths(&tm), time_getDays(&tm), time_getYears(&tm), time_getHours(&tm), time_getMinutes(&tm), time_getSeconds(&tm), pos + 1);
            time_addHours(&tm, 1);
        }
        fclose(f);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        //Maximum row count.
        profileGeneric.entriesInUse = rowCount;
        profileGeneric.profileEntries = rowCount;

        gxTarget CAPTURE_OBJECT[10] = { 0 };
        ARR_ATTACH(captureObjects, CAPTURE_OBJECT, 0);
    }
    return 0;
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
        strcpy((char*)DESTINATIONS[0], "www.gurux.org");
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
        const char* activeSeasonName = "Summer time";
        const char* passiveSeasonName = "Winter time";
        const char* activeWeekProfileName = "Monday";
        const char* passiveWeekProfileName = "Tuesday";
        strcpy((char*)ACTIVE_CALENDAR_NAME, "Active");
        strcpy((char*)PASSIVE_CALENDAR_NAME, "Passive");

        BB_ATTACH(activityCalendar.calendarNameActive, ACTIVE_CALENDAR_NAME, (unsigned short)strlen(ACTIVE_CALENDAR_NAME));

        /////////////////////////////////////////////////////////////////////////
        //Add active season profile.
        ARR_ATTACH(activityCalendar.seasonProfileActive, ACTIVE_SEASON_PROFILE, 1);
        SET_OCTECT_STRING(ACTIVE_SEASON_PROFILE[0].name, activeSeasonName, (unsigned short)strlen(activeSeasonName));
        time_init(&ACTIVE_SEASON_PROFILE[0].start, -1, 3, 31, -1, -1, -1, -1, -clock1.timeZone);
        /////////////////////////////////////////////////////////////////////////
        //Add week profile.
        ARR_ATTACH(activityCalendar.weekProfileTableActive, ACTIVE_WEEK_PROFILE, 1);
        SET_OCTECT_STRING(ACTIVE_WEEK_PROFILE[0].name, activeWeekProfileName, (unsigned short)strlen(activeWeekProfileName));
        wp = &ACTIVE_WEEK_PROFILE[0];
        wp->monday = wp->tuesday = wp->wednesday = wp->thursday = wp->friday = wp->saturday = wp->sunday = 1;
        /////////////////////////////////////////////////////////////////////////
        //Add active day profiles. There are max three day profiles where one is in use.
        ARR_ATTACH(activityCalendar.dayProfileTableActive, ACTIVE_DAY_PROFILE, 1);
        ACTIVE_DAY_PROFILE[0].dayId = 1;
        ARR_ATTACH(ACTIVE_DAY_PROFILE[0].daySchedules, ACTIVE_DAY_PROFILE_ACTIONS1, 1);
        ARR_ATTACH(ACTIVE_DAY_PROFILE[1].daySchedules, ACTIVE_DAY_PROFILE_ACTIONS2, 0);
        ARR_ATTACH(ACTIVE_DAY_PROFILE[2].daySchedules, ACTIVE_DAY_PROFILE_ACTIONS3, 0);

        time_now(&ACTIVE_DAY_PROFILE_ACTIONS1[0].startTime, 0);
        ACTIVE_DAY_PROFILE_ACTIONS1[0].script = BASE(actionSchedule);
        ACTIVE_DAY_PROFILE_ACTIONS2[0].scriptSelector = 1;

        /////////////////////////////////////////////////////////////////////////
        //Add passive season profile.
        BB_ATTACH(activityCalendar.calendarNamePassive, PASSIVE_CALENDAR_NAME, (unsigned short)strlen((char*)PASSIVE_CALENDAR_NAME));
        ARR_ATTACH(activityCalendar.seasonProfilePassive, PASSIVE_SEASON_PROFILE, 1);
        SET_OCTECT_STRING(PASSIVE_SEASON_PROFILE[0].name, passiveSeasonName, (unsigned short)strlen(passiveSeasonName));
        time_init(&PASSIVE_SEASON_PROFILE[0].start, -1, 10, 30, -1, -1, -1, -1, -clock1.timeZone);

        /////////////////////////////////////////////////////////////////////////
        //Add week profile.
        ARR_ATTACH(activityCalendar.weekProfileTablePassive, PASSIVE_WEEK_PROFILE, 1);
        SET_OCTECT_STRING(PASSIVE_WEEK_PROFILE[0].name, passiveWeekProfileName, (unsigned short)strlen(passiveWeekProfileName));
        wp = &PASSIVE_WEEK_PROFILE[0];
        wp->monday = wp->tuesday = wp->wednesday = wp->thursday = wp->friday = wp->saturday = wp->sunday = 1;

        /////////////////////////////////////////////////////////////////////////
        //Add passive day profiles. There are max three day profiles where one is in use.
        ARR_ATTACH(activityCalendar.dayProfileTablePassive, PASSIVE_DAY_PROFILE, 1);
        PASSIVE_DAY_PROFILE[0].dayId = 1;
        ARR_ATTACH(PASSIVE_DAY_PROFILE[0].daySchedules, PASSIVE_DAY_PROFILE_ACTIONS1, 1);
        ARR_ATTACH(PASSIVE_DAY_PROFILE[1].daySchedules, PASSIVE_DAY_PROFILE_ACTIONS2, 0);
        ARR_ATTACH(PASSIVE_DAY_PROFILE[2].daySchedules, PASSIVE_DAY_PROFILE_ACTIONS3, 0);
        time_now(&PASSIVE_DAY_PROFILE_ACTIONS1[0].startTime, 0);
        PASSIVE_DAY_PROFILE_ACTIONS1[0].script = BASE(actionSchedule);
        PASSIVE_DAY_PROFILE_ACTIONS2[0].scriptSelector = 1;
        time_now(&activityCalendar.time, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Optical Port Setup object.
///////////////////////////////////////////////////////////////////////
int addOpticalPortSetup()
{
    int ret;
    static unsigned char DEVICE_ADDRESS[20];
    static unsigned char PASSWORD1[20];
    static unsigned char PASSWORD2[20];
    static unsigned char PASSWORD5[20];
    strcpy((char*)DEVICE_ADDRESS, "Gurux");
    strcpy((char*)PASSWORD1, "Gurux1");
    strcpy((char*)PASSWORD2, "Gurux2");
    strcpy((char*)PASSWORD5, "Gurux5");

    const unsigned char ln[6] = { 0,0,20,0,0,255 };
    if ((ret = INIT_OBJECT(localPortSetup, DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP, ln)) == 0)
    {
        localPortSetup.defaultMode = DLMS_OPTICAL_PROTOCOL_MODE_DEFAULT;
        localPortSetup.proposedBaudrate = DLMS_BAUD_RATE_9600;
        localPortSetup.defaultBaudrate = DLMS_BAUD_RATE_300;
        localPortSetup.responseTime = DLMS_LOCAL_PORT_RESPONSE_TIME_200_MS;
        BB_ATTACH(localPortSetup.deviceAddress, DEVICE_ADDRESS, (unsigned short)strlen((char*)DEVICE_ADDRESS));
        BB_ATTACH(localPortSetup.password1, PASSWORD1, (unsigned short)strlen((char*)PASSWORD1));
        BB_ATTACH(localPortSetup.password2, PASSWORD2, (unsigned short)strlen((char*)PASSWORD2));
        BB_ATTACH(localPortSetup.password5, PASSWORD5, (unsigned short)strlen((char*)PASSWORD5));
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
        GX_UINT16(demandRegister.currentAvarageValue) = 10;
        GX_UINT16(demandRegister.lastAvarageValue) = 20;
        GX_UINT8(demandRegister.status) = 1;
        time_now(&demandRegister.startTimeCurrent, 0);
        time_now(&demandRegister.captureTime, 0);
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

        //Add actions.
        ACTIONS[0].actionDown.script = &scriptTable;
        ACTIONS[0].actionDown.scriptSelector = 1;
        ACTIONS[0].actionUp.script = NULL;
        ACTIONS[0].actionUp.scriptSelector = 0;
        //Add high action.
        ACTIONS[1].actionDown.script = NULL;
        ACTIONS[1].actionDown.scriptSelector = 0;
        ACTIONS[1].actionUp.script = &scriptTable;
        ACTIONS[1].actionUp.scriptSelector = 1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add action schedule object.
///////////////////////////////////////////////////////////////////////
int addActionSchedule()
{
    int ret;
    //Action schedule execution times.
    static gxtime EXECUTION_TIMES[10] = { 0 };
    const unsigned char ln[6] = { 0,0,15,0,0,255 };
    if ((ret = INIT_OBJECT(actionSchedule, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln)) == 0)
    {
        actionSchedule.executedScript = &scriptTable;
        actionSchedule.executedScriptSelector = 1;
        actionSchedule.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
        //Add one execution time.
        time_now(&EXECUTION_TIMES[0], 0);
        ARR_ATTACH(actionSchedule.executionTime, EXECUTION_TIMES, 1);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add image transfer object.
///////////////////////////////////////////////////////////////////////
int addImageTransfer()
{
    int ret;
    static unsigned char IMAGE_TRANSFERRED_BLOCKS_STATUS[100] = { 0 };
    unsigned char ln[6] = { 0,0,44,0,0,255 };
    if ((ret = INIT_OBJECT(imageTransfer, DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln)) == 0)
    {
        imageTransfer.imageBlockSize = 100;
        imageTransfer.imageFirstNotTransferredBlockNumber = 0;
        //Enable image transfer.
        imageTransfer.imageTransferEnabled = 1;
        BIT_ATTACH(imageTransfer.imageTransferredBlocksStatus, IMAGE_TRANSFERRED_BLOCKS_STATUS, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add IEC HDLC Setup object.
///////////////////////////////////////////////////////////////////////
int addIecHdlcSetup()
{
    int ret;
    unsigned char ln[6] = { 0,0,22,0,0,255 };
    if ((ret = INIT_OBJECT(iecHdlcSetup, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln)) == 0)
    {
        iecHdlcSetup.communicationSpeed = DLMS_BAUD_RATE_9600;
        iecHdlcSetup.windowSizeReceive = iecHdlcSetup.windowSizeTransmit = 1;
        iecHdlcSetup.maximumInfoLengthTransmit = iecHdlcSetup.maximumInfoLengthReceive = 128;
        iecHdlcSetup.inactivityTimeout = 120;
        iecHdlcSetup.deviceAddress = 0x10;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add compact data object.
///////////////////////////////////////////////////////////////////////
int addCompactData(dlmsServerSettings* settings)
{
    int ret;
    static unsigned char BUFFER[200];
    static unsigned char TEMPLATE_DESCRIPTION[100];
    static gxTarget CAPTURE_OBJECTS[10] = { 0 };
    unsigned char ln[6] = { 0,0,66,0,1,255 };
    if ((ret = INIT_OBJECT(compactData, DLMS_OBJECT_TYPE_COMPACT_DATA, ln)) == 0)
    {
        compactData.templateId = 66;
        BB_ATTACH(compactData.buffer, BUFFER, 0);
        //Buffer is captured when invoke is called.
        compactData.captureMethod = DLMS_CAPTURE_METHOD_INVOKE;
        ////////////////////////////////////////
        ARR_ATTACH(compactData.captureObjects, CAPTURE_OBJECTS, 2);
        //Add capture objects.
        //Add compact data template ID as first object.
        CAPTURE_OBJECTS[0].attributeIndex = 4;
        CAPTURE_OBJECTS[0].dataIndex = 0;
        CAPTURE_OBJECTS[0].target = &compactData.base;

        CAPTURE_OBJECTS[1].attributeIndex = 4;
        CAPTURE_OBJECTS[1].dataIndex = 0;
        CAPTURE_OBJECTS[1].target = &actionSchedule.base;
        BB_ATTACH(compactData.templateDescription, TEMPLATE_DESCRIPTION, 0);
        ret = compactData_updateTemplateDescription(&settings->base, &compactData);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add gxSchedule object.
///////////////////////////////////////////////////////////////////////
int addSchedule()
{
    int ret;
    static gxScheduleEntry ENTRIES[10] = { 0 };
    unsigned char ln[6] = { 0,0,12,0,0,255 };
    if ((ret = INIT_OBJECT(schedule, DLMS_OBJECT_TYPE_SCHEDULE, ln)) == 0)
    {
        ARR_ATTACH(schedule.entries, ENTRIES, 1);
        ENTRIES[0].index = 0;
        ENTRIES[0].enable = 1;
        ENTRIES[0].scriptTable = &scriptTable;
        ENTRIES[0].scriptSelector = 1;
        //Time is 00:00:00*
        time_init(&ENTRIES[0].switchTime, -1, -1, -1, 0, 0, 0, -1, -1);
        ENTRIES[0].validityWindow = 0xFFFF;
        //Exec week days are Monday and Wednesday. 1010000
        ENTRIES[0].execWeekdays = 0x50;
        //ENTRIES[0].execSpecDays
        //Date is 1/1/*
        time_init(&ENTRIES[0].beginDate, -1, 1, 1, -1, -1, -1, -1, -1);
        //Date is 1/1/*
        time_init(&ENTRIES[0].endDate, -1, 1, 1, -1, -1, -1, -1, -1);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add special days table object.
///////////////////////////////////////////////////////////////////////
int addSpecialDaysTable()
{
    int ret;
    static gxSpecialDay SPECIAL_DAYS[10] = { 0 };
    unsigned char ln[6] = { 0,0,11,0,0,255 };
    if ((ret = INIT_OBJECT(specialDays, DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE, ln)) == 0)
    {
        ARR_ATTACH(specialDays.entries, SPECIAL_DAYS, 1);
        SPECIAL_DAYS[0].index = 0;
        //Date is 1/1/*
        time_init(&SPECIAL_DAYS[0].date, -1, 1, 1, -1, -1, -1, -1, -1);
        SPECIAL_DAYS[0].dayId = 1;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add SAP Assignment object.
///////////////////////////////////////////////////////////////////////
int addSapAssignment()
{
    int ret;
    static gxSapItem SAP_ASSIGNMENT_LIST[10] = { 0 };
    const unsigned char ln[6] = { 0,0,41,0,0,255 };
    if ((ret = INIT_OBJECT(sapAssignment, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, ln)) == 0)
    {
        ARR_ATTACH(sapAssignment.sapAssignmentList, SAP_ASSIGNMENT_LIST, 1);
        char buff[17];
        sprintf(buff, "GRX%.13lu", SERIAL_NUMBER);
        SAP_ASSIGNMENT_LIST[0].id = 1;
        SET_OCTECT_STRING(SAP_ASSIGNMENT_LIST[0].name, (unsigned char*)buff, (unsigned short)strlen(buff));
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
        ARR_ATTACH(modemConfiguration.initialisationStrings, INITIALISATIONS, 2);
        SET_OCTECT_STRING(INITIALISATIONS[0].request, "AT", 2);
        SET_OCTECT_STRING(INITIALISATIONS[0].response, "OK", 2);
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
    static unsigned char MAC_ADDRESS[16] = { 0 };
    const unsigned char ln[6] = { 0,0,25,2,0,255 };
    if ((ret = INIT_OBJECT(macAddressSetup, DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, ln)) == 0)
    {
        BB_ATTACH(macAddressSetup.macAddress, MAC_ADDRESS, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add Disconnect control object.
///////////////////////////////////////////////////////////////////////
int addDisconnectControl()
{
    int ret;
    const unsigned char ln[6] = { 0,0,96,3,10,255 };
    if ((ret = INIT_OBJECT(disconnectControl, DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln)) == 0)
    {
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
    static unsigned long MULTICAST_IP_ADDRESS[10] = { 0 };
    const unsigned char ln[6] = { 0,0,25,1,0,255 };
    if ((ret = INIT_OBJECT(ip4Setup, DLMS_OBJECT_TYPE_IP4_SETUP, ln)) == 0)
    {
        ip4Setup.ipAddress = getIpAddress();
        ARR_ATTACH(ip4Setup.multicastIPAddress, MULTICAST_IP_ADDRESS, 0);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add push setup object. (On Connectivity)
///////////////////////////////////////////////////////////////////////
int addPushSetup()
{
    int ret;
    const char* DEST = "127.0.0.1:7000";
    static gxTarget CAPTURE_OBJECTS[10] = { 0 };

    const unsigned char ln[6] = { 0,0,25,9,0,255 };
    if ((ret = INIT_OBJECT(pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln)) == 0)
    {
        SET_OCTECT_STRING(pushSetup.destination, DEST, (unsigned short)strlen(DEST));

        ARR_ATTACH(pushSetup.pushObjectList, CAPTURE_OBJECTS, 2);
        // Add push object itself. This is needed to tell structure of data to
        // the Push listener.
        CAPTURE_OBJECTS[0].target = &pushSetup.base;
        CAPTURE_OBJECTS[0].attributeIndex = 2;
        CAPTURE_OBJECTS[0].dataIndex = 0;

        // Add logical device name.
        CAPTURE_OBJECTS[1].target = &ldn.base;
        CAPTURE_OBJECTS[1].attributeIndex = 2;
        CAPTURE_OBJECTS[1].dataIndex = 0;

        // Add 0.0.25.1.0.255 Ch. 0 IPv4 setup IP address.
        CAPTURE_OBJECTS[2].target = &ip4Setup.base;
        CAPTURE_OBJECTS[2].attributeIndex = 3;
        CAPTURE_OBJECTS[2].dataIndex = 0;
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add script table object.
///////////////////////////////////////////////////////////////////////
int addScriptTable()
{
    int ret;
    static gxScript SCRIPTS[2] = { 0 };
    static gxScriptAction ACTIONS1[2] = { 0 };
    static gxScriptAction ACTIONS2[2] = { 0 };

    const unsigned char ln[6] = { 0,0,10,0,0,255 };
    if ((ret = INIT_OBJECT(scriptTable, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
    {
        SCRIPTS[0].id = 1;
        ARR_ATTACH(scriptTable.scripts, SCRIPTS, 1);
        ARR_ATTACH(SCRIPTS[0].actions, ACTIONS1, 1);
        ARR_ATTACH(SCRIPTS[1].actions, ACTIONS2, 0);
        //TODO: Set object type and logical name, attribute index to execute.
        //In this example it is a clock.
        ACTIONS1[0].type = DLMS_SCRIPT_ACTION_TYPE_EXECUTE;
        ACTIONS1[0].target = BASE(clock1);
        ACTIONS1[0].index = 2;
        var_init(&ACTIONS1[0].parameter);
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////
//Add firmware version object.
///////////////////////////////////////////////////////////////////////
int addFirmwareVersion()
{
    int ret;
    static char FW[] = "Gurux FW 1.0.1";
    //Firmware version.
    {
        const unsigned char ln[6] = { 1,0,0,2,0,255 };
        if ((ret = INIT_OBJECT(fw, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_OCTECT_STRING(fw.value, FW, (unsigned short)strlen(FW));
        }
    }
    return ret;
}

int svr_InitObjects(
    dlmsServerSettings* settings)
{
    int ret;
    sprintf(LDN, "GRX%.13lu", SERIAL_NUMBER);
    OA_ATTACH(settings->base.objects, ALL_OBJECTS);

    addSecuritySetup();
    //Add Logical Name Association for Authentication None.
    if ((ret = addNoneAssociation()) != 0)
    {
        return ret;
    }
    //Add Logical Name Association for Authentication Low.
    if ((ret = addLowAssociation()) != 0)
    {
        return ret;
    }
    //Add Logical Name Association for Authentication High.
    if ((ret = addHighAssociation()) != 0)
    {
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
    {
        const unsigned char ln[6] = { 0,0,42,0,0,255 };
        if ((ret = INIT_OBJECT(ldn, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_OCTECT_STRING(ldn.value, LDN, sizeof(LDN));
        }
    }
    //Electricity ID 1
    {
        const unsigned char ln[6] = { 1,1,0,0,0,255 };
        if ((ret = INIT_OBJECT(id1, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_OCTECT_STRING(id1.value, LDN, sizeof(LDN));
        }
    }

    //Electricity ID 2.
    {
        const unsigned char ln[6] = { 1,1,0,0,1,255 };
        if ((ret = INIT_OBJECT(id2, DLMS_OBJECT_TYPE_DATA, ln)) == 0)
        {
            GX_UINT32(id2.value) = SERIAL_NUMBER;
        }
    }
    if ((ret = addFirmwareVersion()) != 0)
    {
        return ret;
    }
    if ((ret = addAccount()) != 0)
    {
        return ret;
    }
    if ((ret = addRegisterActivation()) != 0)
    {
        return ret;
    }
    if ((ret = addCharge()) != 0)
    {
        return ret;
    }
    if ((ret = addCredit()) != 0)
    {
        return ret;
    }
    if ((ret = addGprsModemSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addGsmDiagnostic()) != 0)
    {
        return ret;
    }
    if ((ret = addLimiter()) != 0)
    {
        return ret;
    }
    if ((ret = addMbusClient()) != 0)
    {
        return ret;
    }
    if ((ret = addMBusMasterPortSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addMBusSlavePortSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addParameterMonitor()) != 0)
    {
        return ret;
    }
    if ((ret = addPppSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addTokenGateway()) != 0)
    {
        return ret;
    }

    if ((ret = addRegisterObject()) != 0)
    {
        return ret;
    }
    if ((ret = addExtendedRegisterObject()) != 0)
    {
        return ret;
    }
    if ((ret = addClockObject()) != 0)
    {
        return ret;
    }
    if ((ret = addTcpUdpSetup()) != 0)
    {
        return ret;
    }
    if ((ret = addProfileGeneric(settings)) != 0)
    {
        return ret;
    }
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
    if ((ret = addCompactData(settings)) != 0)
    {
        return ret;
    }

    if ((ret = addSpecialDaysTable()) != 0)
    {
        return ret;
    }
    if ((ret = addSchedule()) != 0)
    {
        return ret;
    }
    return oa_verify(&settings->base.objects);
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
    if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        if (settings->authentication == DLMS_AUTHENTICATION_NONE)
        {
            e->target = &associationNone.base;
            return 0;
        }
        if (settings->authentication == DLMS_AUTHENTICATION_LOW)
        {
            e->target = &lowAssociation.base;
            return 0;
        }
        if (settings->authentication == DLMS_AUTHENTICATION_HIGH ||
            settings->authentication == DLMS_AUTHENTICATION_HIGH_MD5 ||
            settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA1 ||
            settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC ||
            settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA256)
        {
            e->target = &highAssociation.base;
            return 0;
        }
        e->target = NULL;
        return 0;
    }
    int ret, pos;
    gxObject* obj;
    objectArray* objects = &settings->objects;
    for (pos = 0; pos != objects->size; ++pos)
    {
        ret = oa_getByIndex(objects, pos, &obj);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj->objectType == objectType)
        {
            memcpy(obj->logicalName, ln, 6);
            e->target = obj;
            return 0;
        }
    }
    return 0;
}

/**
* Find restricting object.
*/
int getRestrictingObject(dlmsSettings* settings, gxValueEventArg* e, gxObject** obj, short* index)
{
    int ret;
    unsigned char ln[6];
    unsigned short ot;
    signed char aIndex;
    unsigned short dIndex;
    if ((ret = cosem_checkStructure(e->parameters.byteArr, 4)) == 0 &&
        (ret = cosem_checkStructure(e->parameters.byteArr, 4)) == 0 &&
        (ret = cosem_getUInt16(e->parameters.byteArr, &ot)) == 0 &&
        (ret = cosem_getOctectString2(e->parameters.byteArr, ln, 6, NULL)) == 0 &&
        (ret = cosem_getInt8(e->parameters.byteArr, &aIndex)) == 0 &&
        (ret = cosem_getUInt16(e->parameters.byteArr, &dIndex)) == 0)
    {

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
    gxtime tm, start, end;
    int ret;
    dlmsVARIANT tmp;
    var_init(&tmp);
    gxObject* obj = NULL;
    short index;
    if ((ret = getRestrictingObject(settings, e, &obj, &index)) != 0 ||
        (ret = cosem_getDateTimeFromOctectString(e->parameters.byteArr, &start)) != 0 ||
        (ret = cosem_getDateTimeFromOctectString(e->parameters.byteArr, &end)) != 0)
    {
        return ret;
    }
#if defined(_DEBUG) && !defined(GX_DLMS_MICROCONTROLLER)

    char str1[40];
    char str2[40];
    time_toString2(&start, str1, sizeof(str1));
    time_toString2(&end, str2, sizeof(str2));
    printf("Reading rows %s-%s\r\n", str1, str2);
#endif
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)

#if _MSC_VER > 1400
    FILE * f = NULL;
    fopen_s(&f, DATAFILE, "r");
#else
    FILE* f = fopen(DATAFILE, "r");
#endif
    if (f != NULL)
    {
        e->transactionStartIndex = 1;
        while ((len = fscanf(f, "%d/%d/%d %d:%d:%d;%d", &month, &day, &year, &hour, &minute, &second, &value)) != -1)
        {
            //Skip empty lines.
            if (len == 7)
            {
                time_init(&tm, year, month, day, hour, minute, second, 0, 0);
                if (time_compare(&tm, &end) > 0)
                {
                    // If all data is read.
                    break;
                }
                if (time_compare(&tm, &start) < 0)
                {
                    // If we have not found the first item.
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

/**
* Get row count.
*/
unsigned long getProfileGenericDataCount() {
    unsigned long rows = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    int ch;
#if _MSC_VER > 1400
    FILE * f = NULL;
    fopen_s(&f, DATAFILE, "r");
#else
    FILE* f = fopen(DATAFILE, "r");
#endif
    if (f != NULL)
    {
        while ((ch = fgetc(f)) != EOF)
        {
            if (ch == EOL)
            {
                ++rows;
            }
        }
        fclose(f);
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    return rows;
}

//Check is column selected.
unsigned char isSelected(gxObject* target)
{
    int ret, pos;
    gxTarget* it;
    //If all columns are read.
    if (captureObjects.size == 0)
    {
        return 1;
    }
    for (pos = 0; pos != captureObjects.size; ++pos)
    {
        if ((ret = arr_getByIndex(&captureObjects, pos, (void**)& it, sizeof(gxTarget))) != 0)
        {
            break;
        }
        if (it->target == target)
        {
            return 1;
        }
    }
    return 0;
}

/**
* Return data using start and end indexes.
*
* @param p
*            ProfileGeneric
* @param index start index.
* @param count Amount of the rows.
*/
int getProfileGenericDataByEntry(dlmsSettings* settings, gxProfileGeneric* p, gxValueEventArg* e)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    gxByteBuffer * data = e->value.byteArr;
    int ret = 0, len, month = 0, day = 0, year = 0, hour = 0, minute = 0, second = 0, value = 0;
    gxtime tm;
    unsigned short rowSize = 0;
    unsigned long index = 1;
    if (!(e->transactionEndIndex < e->transactionStartIndex))
    {
#if _MSC_VER > 1400
        FILE * f = NULL;
        fopen_s(&f, DATAFILE, "r");
#else
        FILE* f = fopen(DATAFILE, "r");
#endif
        if (f != NULL)
        {
            while ((len = fscanf(f, "%d/%d/%d %d:%d:%d;%d", &month, &day, &year, &hour, &minute, &second, &value)) != -1)
            {
                // Skip row
                if (!(index < e->transactionStartIndex) && len == 7)
                {
                    time_init(&tm, year, month, day, hour, minute, second, 0, 0);
                    //Add time zone and deviation.
                    clock_utcToMeterTime(&clock1, &tm);
                    //Set amount of columns.
                    if ((ret = cosem_setStructure(data, captureObjects.size)) != 0)
                    {
                        break;
                    }
                    //Add date time.
                    if (isSelected(BASE(clock1)))
                    {
                        if ((ret = cosem_setDateTimeAsOctectString(data, &tm)) != 0)
                        {
                            break;
                        }
                    }
                    //Add register value as UInt32.
                    if (isSelected(BASE(activePowerL1)))
                    {
                        if ((ret = cosem_setUInt32(data, value)) != 0)
                        {
                            break;
                        }
                    }

                    if (index == e->transactionEndIndex)
                    {
                        //If we have read all the items.
                        break;
                    }
                    ++e->transactionStartIndex;
                    if (dlms_isPduFull(settings, data, &rowSize))
                    {
                        //If PDU is full.
                        break;
                    }
                }
                ++index;
            }
            fclose(f);
        }
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
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

        //If user wan't to read profile generic buffer.
        if (e->target == BASE(profileGeneric))
        {
            // If buffer is read and we want to save memory.
            if (e->index == 7)
            {
                // If client wants to know EntriesInUse.
                profileGeneric.entriesInUse = getProfileGenericDataCount();
            }
            else if (e->index == 2)
            {
                // Buffer read is done here.
                // Read rows from file.
                // If reading first time.
                if (e->transactionEndIndex == 0)
                {
                    if (e->selector == 0)
                    {
                        //Read all data.
                        e->transactionEndIndex = (unsigned short)getProfileGenericDataCount();
                        //Index is One-based value.
                        e->transactionStartIndex = 1;
                    }
                    else if (e->selector == 1)
                    {
                        //Read by entry (start and end time).
                        if ((ret = getProfileGenericDataByRange(settings, e)) != 0 ||
                            (ret = cosem_getColumns(&profileGeneric.captureObjects, e->selector, &e->parameters, &captureObjects)) != 0)
                        {
                            e->error = ret;
                            break;
                        }
                    }
                    else if (e->selector == 2)
                    {
                        //Read by range (start and end index).
                        if ((ret = cosem_checkStructure(e->parameters.byteArr, 4)) != 0 ||
                            (ret = cosem_getUInt32(e->parameters.byteArr, &e->transactionStartIndex)) != 0 ||
                            (ret = cosem_getUInt32(e->parameters.byteArr, &e->transactionEndIndex)) != 0 ||
                            (ret = cosem_getColumns(&profileGeneric.captureObjects, e->selector, &e->parameters, &captureObjects)) != 0)
                        {
                            e->error = ret;
                            break;
                        }
                        //Index is One-based value.
                        if (e->transactionStartIndex == 0)
                        {
                            e->error = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                            break;
                        }
                        // If client wants to read more data what we have.
                        unsigned long cnt = getProfileGenericDataCount();
                        if (e->transactionEndIndex == 0 || e->transactionEndIndex > cnt)
                        {
                            e->transactionEndIndex = cnt;
                        }
                    }
                    //Add array. This is done only once when new read starts.
                    unsigned short count = (unsigned short)(e->transactionEndIndex - e->transactionStartIndex + 1);
                    if ((ret = bb_clear(e->value.byteArr)) != 0 ||
                        (ret = cosem_setArray(e->value.byteArr, count)) != 0)
                    {
                        e->error = ret;
                        break;
                    }
                }
                else
                {
                    if ((ret = bb_clear(e->value.byteArr)) != 0)
                    {
                        e->error = ret;
                        break;
                    }
                }
                getProfileGenericDataByEntry(settings, &profileGeneric, e);
            }
            continue;
        }

        //Update date and time of clock object.
        if (e->target == BASE(clock1) && e->index == 2)
        {
            time_now(&((gxClock*)e->target)->time, 1);
        }
        //Increase value every time when it's read.
        if (e->target == BASE(activePowerL1) && e->index == 2)
        {
            ++activePowerL1Value;
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
#ifndef GX_DLMS_MICROCONTROLLER
    char str[25];
    gxValueEventArg* e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        hlp_getLogicalNameToString(e->target->logicalName, str);
        printf("Writing %s\r\n", str);
    }
#endif //GX_DLMS_MICROCONTROLLER
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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    char str[25];
#endif
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
        printf("Action %s:%d\r\n", str, e->index);
#endif
        //If disconnect control is called it must handle here:
        if (e->target == BASE(disconnectControl))
        {
            //Mark action as handled because there is nothing what framework can do with this.
            e->handled = 1;

        }
        if (e->target->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP && e->index == 1)
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            sendPush(settings, (gxPushSetup*)e->target);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            e->handled = 1;
        }
        if (e->target->objectType == DLMS_OBJECT_TYPE_IMAGE_TRANSFER)
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            FILE * f;
            gxImageTransfer* i = (gxImageTransfer*)e->target;
            //Image name and size to transfer
            if (e->index == 1)
            {
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
                dlmsVARIANT* ImageUpdate, * size;
                if ((ret = va_getByIndex(e->parameters.Arr, 0, &ImageUpdate)) != 0 ||
                    (ret = va_getByIndex(e->parameters.Arr, 1, &size)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_UNMATCH_TYPE;
                    return;
                }
                imageSize = var_toInteger(size);
                char* p = strrchr(IMAGEFILE, '\\');
                ++p;
                *p = '\0';
                strncat(IMAGEFILE, (char*)ImageUpdate->byteArr->data, (int)ImageUpdate->byteArr->size);
                strcat(IMAGEFILE, ".bin");
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                printf("Updating image %s Size: %d\r\n", IMAGEFILE, imageSize);
#endif
                f = fopen(IMAGEFILE, "wb");
                if (!f)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Unable to open file %s\r\n", IMAGEFILE);
#endif
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                fclose(f);
                ba_clear(&i->imageTransferredBlocksStatus);
            }
            //Transfers one block of the Image to the server
            else if (e->index == 2)
            {
                dlmsVARIANT* blockNo, * data;
                if ((ret = va_getByIndex(e->parameters.Arr, 0, &blockNo)) != 0 ||
                    (ret = va_getByIndex(e->parameters.Arr, 1, &data)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_INITIATED;
                f = fopen(IMAGEFILE, "ab");
                if (!f)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Unable to open file %s\r\n", IMAGEFILE);
#endif
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                int ret = (int)fwrite(data->byteArr->data, 1, (int)data->byteArr->size, f);
                fclose(f);
                if (ret != (int)data->byteArr->size)
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
                f = fopen(IMAGEFILE, "rb");
                if (!f)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Unable to open file %s\r\n", IMAGEFILE);
#endif
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    return;
                }
                fseek(f, 0L, SEEK_END);
                int size = (int)ftell(f);
                fclose(f);
                if (size != imageSize)
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
}
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postWrite(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    //Show updated values.
#if !(defined(DLMS_IGNORE_MALLOC) ||defined(GX_DLMS_MICROCONTROLLER))
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    gxValueEventArg * e;
    int ret, pos;
    char* buff;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) == 0)
        {
            if ((ret = obj_toString(e->target, &buff)) != 0)
            {
                printf("svr_postWrite::obj_toString failed %d.", ret);
                break;
            }
            printf(buff);
            free(buff);
        }
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#endif //!(defined(GX_DLMS_MICROCONTROLLER) ||defined(GX_DLMS_MICROCONTROLLER))
}

void handleProfileGenericActions(
    gxValueEventArg* it)
{
    if (it->index == 1)
    {
        // Profile generic clear is called. Clear data.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
        FILE * f = NULL;
        fopen_s(&f, DATAFILE, "w");
#else
        FILE* f = fopen(DATAFILE, "w");
#endif
        fclose(f);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    }
    else if (it->index == 2)
    {
        // Profile generic Capture is called.
        // In this example data is saved to the file on svr_preGet so we do nothing here.
    }
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
            break;
        }
        if (e->target == BASE(profileGeneric))
        {
            handleProfileGenericActions(e);
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
    ret = connect(*s, (struct sockaddr*) & add, sizeof(struct sockaddr_in));
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
    char host[20];
    char* p;
    int ret, pos, port, s;
    message messages;
    if (push->destination.size == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    p = strchr((char*)push->destination.value, ':');
    if (p == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pos = (int)(p - (char*)push->destination.value);
    strncpy(host, (char*)push->destination.value, pos);
    host[pos] = 0;
    sscanf(++p, "%d", &port);
    unsigned char data[PDU_BUFFER_SIZE];
    gxByteBuffer bb;
    bb_attach(&bb, data, 0, sizeof(data));
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
    if ((ret = connectServer(host, port, &s)) == 0)
    {
        if ((ret = notify_generatePushSetupMessages(settings, 0, push, &messages)) == 0)
        {
            for (pos = 0; pos != messages.size; ++pos)
            {
                if ((ret = send(s, (char*)bb.data, bb.size, 0)) == -1)
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
    return 0;
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

unsigned char svr_isTarget(
    dlmsSettings* settings,
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    //Check server address using serial number.
    if ((serverAddress & 0x3FFF) == SERIAL_NUMBER % 10000 + 1000)
    {
        return 1;
    }
    //Check server address with two bytes.
    if ((serverAddress & 0xFFFF0000) == 0 && (serverAddress & 0x7FFF) == 1)
    {
        return 1;
    }
    //Check server address with one byte.
    if ((serverAddress & 0xFFFFFF00) == 0 && (serverAddress & 0x7F) == 1)
    {
        return 1;
    }
    return 0;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
    dlmsServerSettings* settings,
    DLMS_AUTHENTICATION authentication,
    gxByteBuffer* password)
{
    //Check used security level.
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->base.cipher.security == DLMS_SECURITY_NONE)
    {
        //Uncomment this if you want that only Get service is available with authentication level None.
        //settings->base.negotiatedConformance = DLMS_CONFORMANCE_GET;
         //Uncomment this if secured (ciphered) connection is always required.
         //return DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
    }
#endif //DLMS_IGNORE_HIGH_GMAC

    if (authentication == DLMS_AUTHENTICATION_NONE)
    {
        //Uncomment this if authentication is always required.
        //return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
    }
    //Check Low Level security..
    if (authentication == DLMS_AUTHENTICATION_LOW)
    {
        gxByteBuffer expected;
        if (settings->base.useLogicalNameReferencing)
        {
            expected = lowAssociation.secret;
        }
        if (bb_compare(&expected, password->data, password->size) == 0)
        {
            return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
        }
    }
    // Other authentication levels are check on phase two.
    return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    if (index == 1)
    {
        return DLMS_ACCESS_MODE_READ;
    }
    //Security Setup client_system_title and server_system_title access is not allowed.
    if (obj->objectType == DLMS_OBJECT_TYPE_SECURITY_SETUP && (index == 4 || index == 5))
    {
        return DLMS_ACCESS_MODE_READ;
    }
    //Compact data buffer is read-only.
    if (obj->objectType == DLMS_OBJECT_TYPE_COMPACT_DATA && index == 2)
    {
        return DLMS_ACCESS_MODE_READ;
    }

    // Only read is allowed
    if (settings->authentication == DLMS_AUTHENTICATION_NONE)
    {
        return DLMS_ACCESS_MODE_READ;
    }
    //Only read is allowed for followed tests.
    if (obj->objectType == DLMS_OBJECT_TYPE_REGISTER ||
        obj->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME ||
        obj->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
    {
        return DLMS_ACCESS_MODE_READ;
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
    //Example server don't allow security setup changes.
    if (obj->objectType == DLMS_OBJECT_TYPE_SECURITY_SETUP)
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

/////////////////////////////////////////////////////////////////////////////
// Remove zero-based row from the file.
/////////////////////////////////////////////////////////////////////////////
int removeLine(char* fileName, unsigned long index)
{
    --index;
    int ret = 0;
    unsigned long row = 0;
    unsigned char skipRow = index == 0 ? 1 : 0;
    char tmp[256];
    strcpy(tmp, fileName);
    strcat(tmp, ".tmp");
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    int ch;
#if _MSC_VER > 1400
    FILE * fr = NULL;
    fopen_s(&fr, fileName, "r");
    FILE* fw = NULL;
    fopen_s(&fw, tmp, "w");
#else
    FILE* fr = fopen(fileName, "r");
    FILE* fw = fopen(tmp, "w");
#endif
    if (fr != NULL && fw != NULL)
    {
        while ((ch = fgetc(fr)) != EOF)
        {
            if (ch == EOL)
            {
                ++row;
                //Remove row.
                if (row == index && skipRow == 0)
                {
                    skipRow = 1;
                }
                else if (skipRow)
                {
                    skipRow = 0;
                    if (index == 0)
                    {
                        continue;
                    }
                }
            }
            if (!skipRow)
            {
                if (putc(ch, fw) == EOF)
                {
                    break;
                }
            }
        }
        fclose(fr);
        fflush(fw);
        fclose(fw);
        // remove the original file
        remove(fileName);
        // rename the temporary file to original name
        rename(tmp, fileName);
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    return ret;
}

void capture2File(gxProfileGeneric* pg)
{
    gxtime tm;
    unsigned long cnt = getProfileGenericDataCount() + 1;
    //Remove first item if buffer is full.
    if (cnt > profileGeneric.entriesInUse)
    {
        removeLine(DATAFILE, 0);
    }
    else
    {
        profileGeneric.entriesInUse = cnt;
    }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
    FILE * f = NULL;
    fopen_s(&f, DATAFILE, "a");
#else
    FILE* f = fopen(DATAFILE, "a");
#endif
    // Update last average value.
    time_now(&tm, 0);
    fprintf(f, "%d/%d/%d %d:%d:%d;%d\r", time_getMonths(&tm), time_getDays(&tm), time_getYears(&tm),
        time_getHours(&tm), time_getMinutes(&tm), time_getSeconds(&tm), activePowerL1Value);
    fclose(f);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
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
            break;
        }
        //Save data to the file.
        if (e->target == BASE(profileGeneric))
        {
            capture2File(&profileGeneric);
            e->handled = 1;
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