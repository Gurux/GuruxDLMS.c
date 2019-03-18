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

//If OS is not used get time.
//All compilers don't know or use time.
#if !defined(_WIN32) && !defined(_WIN64) && !defined(__linux__)
long time_current(void)
{
    //Get current time somewhere.
    return (long)time(NULL);
}

long time_elapsed(void)
{
    return (long)clock() * 1000;
}

void time_now(
    gxtime* value)
{
    //Get local time somewhere.
    time_t tm1 = time(NULL);
    struct tm dt = *localtime(&tm1);
    time_init2(value, &dt);
}
#endif
//Meter serial number.
unsigned long sn = 123456;

unsigned char useRingBuffer = 0;
/*Define all COSEM objects here so they are not use stack.*/
gxData ldn, id1, id2, fw;
gxAssociationLogicalName associationNone, highAssociation;

#ifndef DLMS_ITALIAN_STANDARD
//Association view for None and Low authentication if Logical name referencing is used.
gxAssociationLogicalName lowAssociation;
//Same Authentication view is used if short name referencing is used.
//This is not needed if logical name referencing is used.
#ifndef DLMS_INDIAN_STANDARD
gxAssociationShortName associationShortName;
#endif//DLMS_INDIAN_STANDARD
#endif//DLMS_ITALIAN_STANDARD

gxImageTransfer imageTransfer;
gxRegister activePowerL1;
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

int imageSize = 0;
#ifdef DLMS_INDIAN_STANDARD
/*Indian Standard examples begin.*/
gxProfileGeneric instantData;
gxRegister currentIR, currentIY, currentIB;
gxRegister voltageVRN, voltageVYN, voltageVBN, voltageVRY, voltageVBY, spfR, spfY, spfB, spfBF;
gxRegister frequency, apparentPower, signedActivePower, signedReactivePower;
gxRegister numberOfPowerFailures, cumulativePowerFailureDuration, cumulativeTamperCount, cumulativeBillingCount, cumulativeProgrammingCount;
gxRegister billingDate, cumulativeEnergy;
#endif //DLMS_INDIAN_STANDARD
/*Indian Standard examples end.*/

/*Italian Standard examples begin.*/
#ifdef DLMS_ITALIAN_STANDARD
gxData frameCounterThresholds, frameCounterOnLine, frameCounterOffLine, frameCounterGuarantor, frameCounterInstaller, frameCounterGateway, frameCounterBroadcast;
gxSecuritySetup securitySetup;
#endif // DLMS_ITALIAN_STANDARD

/*Italian Standard examples end.*/

//All objects.
const gxObject* ALL_OBJECTS[] = { &ldn.base, &id1.base, &id2.base, &fw.base, &imageTransfer.base, &activePowerL1.base, &clock1.base, &udpSetup.base, &profileGeneric.base,
    &autoConnect.base, &activityCalendar.base, &localPortSetup.base, &demandRegister.base, &registerMonitor.base,
    &actionSchedule.base, &sapAssignment.base, &autoAnswer.base, &modemConfiguration.base, &macAddressSetup.base,
    &disconnectControl.base, &ip4Setup.base, &pushSetup.base, &scriptTable.base, &iecHdlcSetup.base, &compactData.base,
    &associationNone.base, &highAssociation.base,
#if !defined(DLMS_ITALIAN_STANDARD)
    &lowAssociation.base,
#endif

#ifdef DLMS_INDIAN_STANDARD
    &instantData.base, &currentIR.base,
    &currentIY.base, &currentIB.base, &voltageVRN.base, &voltageVYN.base, &voltageVBN.base, &voltageVRY.base, &voltageVBY.base, &spfR.base,
    &spfY.base, &spfB.base, &spfBF.base, &frequency.base, &apparentPower.base, &signedActivePower.base, &signedReactivePower.base,
    &numberOfPowerFailures.base, &cumulativePowerFailureDuration.base, &cumulativeTamperCount.base, &cumulativeBillingCount.base,
    &cumulativeProgrammingCount.base, &billingDate.base, &cumulativeEnergy.base
#endif //DLMS_INDIAN_STANDARD
#ifdef DLMS_ITALIAN_STANDARD
    &frameCounterThresholds.base, &frameCounterOnLine.base, &frameCounterOffLine.base, &frameCounterGuarantor.base, &frameCounterInstaller.base, &frameCounterGateway.base, &frameCounterBroadcast.base,
    &securitySetup.base,
#endif // DLMS_ITALIAN_STANDARD
#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    &associationShortName.base,
#endif //!defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
};

//Objects what are shown on association view None.
const gxObject* NONE_OBJECTS[] = {
    &associationNone.base,
    &ldn.base, &clock1.base };

#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
//Objects what are shown on association view None.
const gxObject* SN_OBJECTS[] = { &associationShortName.base, &ldn.base, &id1.base, &id2.base, &fw.base, &imageTransfer.base,
&activePowerL1.base, &clock1.base, &udpSetup.base, &profileGeneric.base,
&autoConnect.base, &activityCalendar.base, &localPortSetup.base, &demandRegister.base, &registerMonitor.base,
&actionSchedule.base, &sapAssignment.base, &autoAnswer.base, &modemConfiguration.base, &macAddressSetup.base,
&disconnectControl.base, &ip4Setup.base, &pushSetup.base, &scriptTable.base, &iecHdlcSetup.base, &compactData.base };
#endif //!defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)

#if !defined(DLMS_ITALIAN_STANDARD)
//Objects what are shown on association view Low.
const gxObject* LOW_OBJECTS[] = { &lowAssociation.base, &ldn.base };
#endif //!defined(DLMS_ITALIAN_STANDARD)

//Objects what are shown on association view High.
const gxObject* HIGH_OBJECTS[] = {
    &highAssociation.base,
    &ldn.base, &id1.base, &id2.base, &fw.base, &imageTransfer.base, &activePowerL1.base, &clock1.base, &udpSetup.base, &profileGeneric.base,
&autoConnect.base, &activityCalendar.base, &localPortSetup.base, &demandRegister.base, &registerMonitor.base,
&actionSchedule.base, &sapAssignment.base, &autoAnswer.base, &modemConfiguration.base, &macAddressSetup.base,
&disconnectControl.base, &ip4Setup.base, &pushSetup.base, &scriptTable.base,
&iecHdlcSetup.base, &compactData.base,
#ifdef DLMS_INDIAN_STANDARD
&instantData.base, &currentIR.base, &currentIY.base, &currentIB.base, &voltageVRN.base, &voltageVYN.base, &voltageVBN.base, &voltageVRY.base, &voltageVBY.base, &spfR.base, &spfY.base, &spfB.base, &spfBF.base, &frequency.base, &apparentPower.base, &signedActivePower.base, &signedReactivePower.base, &numberOfPowerFailures.base, &cumulativePowerFailureDuration.base, &cumulativeTamperCount.base, &cumulativeBillingCount.base, &cumulativeProgrammingCount.base, &billingDate.base, &cumulativeEnergy.base
#endif //DLMS_INDIAN_STANDARD
#ifdef DLMS_ITALIAN_STANDARD
&frameCounterThresholds.base, &frameCounterOnLine.base, &frameCounterOffLine.base, &frameCounterGuarantor.base, &frameCounterInstaller.base, &frameCounterGateway.base, &frameCounterBroadcast.base,
&securitySetup.base,
#endif // DLMS_ITALIAN_STANDARD
};

#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
///////////////////////////////////////////////////////////////////////
//This method adds example Short Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociation()
{
    const unsigned char ln[6] = { 0, 0, 40, 0, 0, 255 };
    cosem_init2((gxObject*)&associationShortName, DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME, ln);
    associationShortName.base.shortName = 0xFA00;
    bb_addString(&associationShortName.secret, "Gurux");
    oa_attach(&associationShortName.objectList, SN_OBJECTS, sizeof(SN_OBJECTS) / sizeof(SN_OBJECTS[0]));
    //All objects are add for this Association View later.
    return 0;
}
#endif //#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)

#ifndef DLMS_ITALIAN_STANDARD
///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for None authentication.
// PC in Indian standard.
///////////////////////////////////////////////////////////////////////
int addNoneAssociation()
{
    const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
    cosem_init2((gxObject*)&associationNone, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
    //Only Logical Device Name is add to this Association View.
    //Use this if you  need to save heap.
    oa_attach(&associationNone.objectList, NONE_OBJECTS, sizeof(NONE_OBJECTS) / sizeof(NONE_OBJECTS[0]));
    bb_addString(&associationNone.secret, "Gurux");
    associationNone.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for Low authentication.
// MR in Indian standard.
///////////////////////////////////////////////////////////////////////
int addLowAssociation()
{
    const unsigned char ln[6] = { 0, 0, 40, 0, 2, 255 };
    cosem_init2((gxObject*)&lowAssociation, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
    bb_addString(&lowAssociation.secret, "Gurux");
    lowAssociation.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_LOW;
    //Use this if you  need to save heap.
    oa_attach(&lowAssociation.objectList, LOW_OBJECTS, sizeof(LOW_OBJECTS) / sizeof(LOW_OBJECTS[0]));
    //All objects are add for this Association View later.
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object for High authentication.
// UA in Indian standard.
///////////////////////////////////////////////////////////////////////
int addHighAssociation()
{
    const unsigned char ln[6] = { 0, 0, 40, 0, 2, 255 };
    cosem_init2((gxObject*)&highAssociation, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
    bb_addString(&highAssociation.secret, "Gurux");
    highAssociation.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH;
    //Use this if you  need to save heap.
    oa_attach(&highAssociation.objectList, HIGH_OBJECTS, sizeof(HIGH_OBJECTS) / sizeof(HIGH_OBJECTS[0]));
    //All objects are add for this Association View later.
    return 0;
}
#else
int addItalianAssociation(objectArray *objects)
{
#define PDU_BUFFER_SIZE 229
    {
        const unsigned char ln[6] = { 0,0,43,0,1,255 };
        cosem_init2(&securitySetup.base, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln);
    }
    {
        const unsigned char ln[6] = { 0, 0, 40, 0, 16, 255 };
        cosem_init2(&associationNone.base, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
        oa_attach(&associationNone.objectList, NONE_OBJECTS, sizeof(NONE_OBJECTS) / sizeof(NONE_OBJECTS[0]));
        //Max PDU size on italian standard is 229 bytes.
        associationNone.xDLMSContextInfo.maxReceivePduSize = associationNone.xDLMSContextInfo.maxReceivePduSize = 229;
    }

    {
        const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
        cosem_init2(&highAssociation.base, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
        oa_attach(&highAssociation.objectList, HIGH_OBJECTS, sizeof(HIGH_OBJECTS) / sizeof(HIGH_OBJECTS[0]));
        bb_addString(&highAssociation.secret, "Gurux");
        highAssociation.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_HIGH_GMAC;
        memcpy(highAssociation.securitySetupReference, securitySetup.base.logicalName, 6);
        //Max PDU size on italian standard is 229 bytes.
        highAssociation.xDLMSContextInfo.maxReceivePduSize = highAssociation.xDLMSContextInfo.maxReceivePduSize = 229;
    }
    return 0;
}

int addFrameCounters(objectArray *objects)
{
    {
        const unsigned char ln[6] = { 0, 0, 94, 39, 33, 255 };
        cosem_init2(&frameCounterThresholds.base, DLMS_OBJECT_TYPE_DATA, ln);
    }
    {
        const unsigned char ln[6] = { 0,0,43, 1,  1, 255 };
        cosem_init2(&frameCounterOnLine.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&frameCounterOnLine.value, 1);
    }
    {
        const unsigned char ln[6] = { 0, 1, 43,  1,  1, 255 };
        cosem_init2(&frameCounterOffLine.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&frameCounterOffLine.value, 1);
    }
    {
        const unsigned char ln[6] = { 0,0,43, 1,  48, 255 };
        cosem_init2(&frameCounterGuarantor.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&frameCounterGuarantor.value, 1);
    }
    {
        const unsigned char ln[6] = { 0,0,43, 1,  3, 255 };
        cosem_init2(&frameCounterInstaller.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&frameCounterInstaller.value, 1);
    }
    {
        const unsigned char ln[6] = { 0,0,43, 1,  64, 255 };
        cosem_init2(&frameCounterGateway.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&frameCounterGateway.value, 1);
    }
    {
        const unsigned char ln[6] = { 0,0,43, 1,  32, 255 };
        cosem_init2(&frameCounterBroadcast.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setUInt32(&frameCounterBroadcast.value, 1);
    }
    return 0;
}

#endif //DLMS_ITALIAN_STANDARD

///////////////////////////////////////////////////////////////////////
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject(
    objectArray *objects)
{
    const unsigned char ln[6] = { 1,1,21,25,0,255 };
    cosem_init2((gxObject*)&activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln);
    //10 ^ 3 =  1000
    activePowerL1.scaler = 3;
    activePowerL1.unit = 30;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject(
    objectArray *objects)
{
    //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
    const unsigned char ln[6] = { 0,0,1,0,0,255 };
    cosem_init2((gxObject*)&clock1, DLMS_OBJECT_TYPE_CLOCK, ln);
    time_init3(&clock1.begin, -1, 9, 1, -1, -1, -1, -1);
    time_init3(&clock1.end, -1, 3, 1, -1, -1, -1, -1);
    return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example TCP/UDP setup object.
///////////////////////////////////////////////////////////////////////
int addTcpUdpSetup(
    objectArray *objects)
{
    //Add Tcp/Udp setup. Default Logical Name is 0.0.25.0.0.255.
    const unsigned char ln[6] = { 0,0,25,0,0,255 };
    cosem_init2((gxObject*)&udpSetup, DLMS_OBJECT_TYPE_TCP_UDP_SETUP, ln);
    udpSetup.inactivityTimeout = 180;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addProfileGeneric(
    objectArray *objects)
{
    gxtime tm;
    gxKey *k;
    gxCaptureObject *capture;
    const unsigned char ln[6] = { 1,0,99,1,0,255 };
    cosem_init2((gxObject*)&profileGeneric, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln);
    //Set capture period to 60 second.
    profileGeneric.capturePeriod = 60;
    //Maximum row count.
    profileGeneric.profileEntries = 100;
    profileGeneric.sortMethod = DLMS_SORT_METHOD_FIFO;
    ///////////////////////////////////////////////////////////////////
    //Add colums.
    //Set saved attribute index.
    capture = (gxCaptureObject*)malloc(sizeof(gxCaptureObject));
    capture->attributeIndex = 2;
    capture->dataIndex = 0;
    k = key_init(&clock1, capture);
    arr_push(&profileGeneric.captureObjects, k);
    //Add register object.
    capture = (gxCaptureObject*)malloc(sizeof(gxCaptureObject));
    capture->attributeIndex = 2;
    capture->dataIndex = 0;

    k = key_init(&activePowerL1, capture);
    arr_push(&profileGeneric.captureObjects, k);

    // Create 10 000 rows for profile generic file.
    // In example profile generic we have two columns.
    // Date time and integer value.
    int rowCount = 10000;
    time_now(&tm);
    time_addMinutes(&tm, -tm.value.tm_min);
    time_addSeconds(&tm, -tm.value.tm_sec);
    time_addHours(&tm, -(rowCount - 1));
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, DATAFILE, "w");
#else
    FILE* f = fopen(DATAFILE, "w");
#endif
    for (int pos = 0; pos != rowCount; ++pos)
    {
        fprintf(f, "%d/%d/%d %d:%d:%d;%d\n", 1 + tm.value.tm_mon, tm.value.tm_mday, 1900 + tm.value.tm_year, tm.value.tm_hour, tm.value.tm_min, tm.value.tm_sec, pos + 1);
        time_addHours(&tm, 1);
    }
    fclose(f);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    //Maximum row count.
    profileGeneric.entriesInUse = rowCount;
    profileGeneric.profileEntries = rowCount;
    //Default value is 1. Change this for your needs.
    profileGeneric.maxRowCount = 1;
    return 0;
}

#ifdef DLMS_INDIAN_STANDARD
///////////////////////////////////////////////////////////////////////
//Add instant data Profile generic object.
// This is used in Indian standard.
///////////////////////////////////////////////////////////////////////
int addInstantDataProfileGeneric(
    objectArray *objects)
{
    {
        const unsigned char ln[6] = { 1,0,94,91,0,255 };
        cosem_init2((gxObject*)&instantData, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln);
    }
    //Set capture period to 60 second.
    instantData.capturePeriod = 60;
    //Maximum row count.
    instantData.profileEntries = 100;
    instantData.sortMethod = DLMS_SORT_METHOD_FIFO;
    ///////////////////////////////////////////////////////////////////
    //Add colums.
    //Set saved attribute index.
    //Add clock object.
    arr_push(&instantData.captureObjects, key_init(&clock1, co_init(2, 0)));

    //Add register objects.
    //Current, IR
    {
        const unsigned char ln[6] = { 1,0,31,7,0,255 };
        cosem_init2((gxObject*)&currentIR, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&currentIR, co_init(2, 0)));
    }

    //Current, IY
    {
        const unsigned char ln[6] = { 1,0,51,7,0,255 };
        cosem_init2((gxObject*)&currentIY, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&currentIY, co_init(2, 0)));
    }

    //Current, IB
    {
        const unsigned char ln[6] = { 1,0,71,7,0,255 };
        cosem_init2((gxObject*)&currentIB, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&currentIB, co_init(2, 0)));
    }

    //Voltage, VRN
    {
        const unsigned char ln[6] = { 1,0,32,7,0,255 };
        cosem_init2((gxObject*)&voltageVRN, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&voltageVRN, co_init(2, 0)));
    }

    //Voltage, VYN
    {
        const unsigned char ln[6] = { 1,0,52,7,0,255 };
        cosem_init2((gxObject*)&voltageVYN, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&voltageVYN, co_init(2, 0)));
    }

    //Voltage, VBN
    {
        const unsigned char ln[6] = { 1,0,72,7,0,255 };
        cosem_init2((gxObject*)&voltageVBN, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&voltageVBN, co_init(2, 0)));
    }

    //Voltage, VRY
    {
        const unsigned char ln[6] = { 1,0,32,7,0,255 };
        cosem_init2((gxObject*)&voltageVRY, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&voltageVRY, co_init(2, 0)));
    }

    //Voltage, VBY
    {
        const unsigned char ln[6] = { 1,0,52,7,0,255 };
        cosem_init2((gxObject*)&voltageVBY, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&voltageVBY, co_init(2, 0)));
    }

    //Signed power factor, R-phase
    {
        const unsigned char ln[6] = { 1,0,33,7,0,255 };
        cosem_init2((gxObject*)&spfR, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&spfR, co_init(2, 0)));
    }

    //Signed power factor, Y-phase
    {
        const unsigned char ln[6] = { 1,0,53,7,0,255 };
        cosem_init2((gxObject*)&spfY, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&spfY, co_init(2, 0)));
    }

    //Signed power factor, B-phase
    {
        const unsigned char ln[6] = { 1,0,73,7,0,255 };
        cosem_init2((gxObject*)&spfB, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&spfB, co_init(2, 0)));
    }

    //Three phase power facto, PF
    {
        const unsigned char ln[6] = { 1,0,13,7,0,255 };
        cosem_init2((gxObject*)&spfBF, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&spfBF, co_init(2, 0)));
    }

    //Frequency
    {
        const unsigned char ln[6] = { 1,0,14,7,0,255 };
        cosem_init2((gxObject*)&frequency, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&frequency, co_init(2, 0)));
    }
    //Apparent power, kVA
    {
        const unsigned char ln[6] = { 1,0,9,7,0,255 };
        cosem_init2((gxObject*)&apparentPower, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&apparentPower, co_init(2, 0)));
    }

    //Signed active power, kW (+ Forward; – Reverse)
    {
        const unsigned char ln[6] = { 1,0,1,7,0,255 };
        cosem_init2((gxObject*)&signedActivePower, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&signedActivePower, co_init(2, 0)));
    }

    //Signed reactive power, kvar (+ Lag; – Lead)
    {
        const unsigned char ln[6] = { 1,0,37,7,0,255 };
        cosem_init2((gxObject*)&signedReactivePower, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&signedReactivePower, co_init(2, 0)));
    }

    //Number of power — failures
    {
        const unsigned char ln[6] = { 0,0,96,7,0,255 };
        cosem_init2((gxObject*)&numberOfPowerFailures, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&numberOfPowerFailures, co_init(2, 0)));
    }
    //Cumulative power — failure duration.
    {
        const unsigned char ln[6] = { 0,0,94,91,8,255 };
        cosem_init2((gxObject*)&cumulativePowerFailureDuration, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&cumulativePowerFailureDuration, co_init(2, 0)));
    }

    //Cumulative tamper count.
    {
        const unsigned char ln[6] = { 0,0,94,91,0,255 };
        cosem_init2((gxObject*)&cumulativeTamperCount, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&cumulativeTamperCount, co_init(2, 0)));
    }

    //Cumulative billing count.
    {
        const unsigned char ln[6] = { 0,0,0,1,0,255 };
        cosem_init2((gxObject*)&cumulativeBillingCount, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&cumulativeBillingCount, co_init(2, 0)));
    }

    //Cumulative programming count.
    {
        const unsigned char ln[6] = { 0,0,96,2,0,255 };
        cosem_init2((gxObject*)&cumulativeProgrammingCount, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&cumulativeProgrammingCount, co_init(2, 0)));
    }

    //Billing date.
    {
        const unsigned char ln[6] = { 0,0,0,1,2,255 };
        cosem_init2((gxObject*)&billingDate, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&billingDate, co_init(2, 0)));
    }

    //Cumulative energy, kWh.
    {
        const unsigned char ln[6] = { 1,0,1,8,0,255 };
        cosem_init2((gxObject*)&cumulativeEnergy, DLMS_OBJECT_TYPE_REGISTER, ln);
        arr_push(&instantData.captureObjects, key_init(&cumulativeEnergy, co_init(2, 0)));
    }
    return 0;
}
#endif //DLMS_INDIAN_STANDARD

///////////////////////////////////////////////////////////////////////
//Add Auto connect object.
///////////////////////////////////////////////////////////////////////
int addAutoConnect(
    objectArray *objects)
{
    gxByteBuffer *str;
    gxtime *start, *end;
    const unsigned char ln[6] = { 0,0,2,1,0,255 };
    cosem_init2((gxObject*)&autoConnect, DLMS_OBJECT_TYPE_AUTO_CONNECT, ln);
    autoConnect.mode = DLMS_AUTO_CONNECT_MODE_AUTO_DIALLING_ALLOWED_ANYTIME;
    autoConnect.repetitions = 10;
    autoConnect.repetitionDelay = 60;
    //Calling is allowed between 1am to 6am.
    start = (gxtime*)malloc(sizeof(gxtime));
    time_init3(start, -1, -1, -1, 1, 0, 0, -1);
    end = (gxtime*)malloc(sizeof(gxtime));
    time_init3(end, -1, -1, -1, 6, 0, 0, -1);
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
    objectArray *objects)
{
    gxDayProfile* dp;
    gxSeasonProfile * sp;
    gxWeekProfile * wp;
    gxDayProfileAction * act;

    const unsigned char ln[6] = { 0,0,13,0,0,255 };
    cosem_init2((gxObject*)&activityCalendar, DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR, ln);

    bb_addString(&activityCalendar.calendarNameActive, "Active");
    //Add season profile.
    sp = (gxSeasonProfile*)malloc(sizeof(gxSeasonProfile));
    bb_init(&sp->name);
    bb_addString(&sp->name, "Summer time");
    time_init3(&sp->start, -1, 3, 31, -1, -1, -1, -1);
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
    time_now(&act->startTime);
    memcpy(act->scriptLogicalName, actionSchedule.base.logicalName, 6);
    act->scriptSelector = 1;
    arr_push(&dp->daySchedules, act);
    arr_push(&activityCalendar.dayProfileTableActive, dp);
    bb_addString(&activityCalendar.calendarNamePassive, "Passive");

    sp = (gxSeasonProfile*)malloc(sizeof(gxSeasonProfile));
    bb_init(&sp->name);
    bb_addString(&sp->name, "Winter time");
    time_init3(&sp->start, -1, 10, 30, -1, -1, -1, -1);
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
    time_now(&act->startTime);
    memcpy(act->scriptLogicalName, actionSchedule.base.logicalName, 6);
    act->scriptSelector = 1;
    arr_push(&dp->daySchedules, act);
    arr_push(&activityCalendar.dayProfileTablePassive, dp);
    time_now(&activityCalendar.time);
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Optical Port Setup object.
///////////////////////////////////////////////////////////////////////
int addOpticalPortSetup(
    objectArray *objects)
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
    objectArray *objects)
{
    const unsigned char ln[6] = { 1,0,31,4,0,255 };
    cosem_init2((gxObject*)&demandRegister, DLMS_OBJECT_TYPE_DEMAND_REGISTER, ln);

    var_setUInt16(&demandRegister.currentAvarageValue, 10);
    var_setUInt16(&demandRegister.lastAvarageValue, 20);
    var_setUInt8(&demandRegister.status, 1);
    time_now(&demandRegister.startTimeCurrent);
    time_now(&demandRegister.captureTime);
    demandRegister.period = 10;
    demandRegister.numberOfPeriods = 1;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Register Monitor object.
///////////////////////////////////////////////////////////////////////
int addRegisterMonitor(
    objectArray *objects)
{
    int ret;
    gxActionSet* action;
    dlmsVARIANT* tmp;
    objectArray oa;

    const unsigned char ln[6] = { 0,0,16,1,0,255 };
    cosem_init2((gxObject*)&registerMonitor, DLMS_OBJECT_TYPE_REGISTER_MONITOR, ln);

    tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
    var_init(tmp);
    var_setString(tmp, "Gurux1", 6);
    va_push(&registerMonitor.thresholds, tmp);

    tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
    var_init(tmp);
    var_setString(tmp, "Gurux2", 6);
    va_push(&registerMonitor.thresholds, tmp);
    oa_init(&oa);
    //Find register object and add it to the profile generic capture columns.
    if ((ret = oa_getObjects(objects, DLMS_OBJECT_TYPE_REGISTER, &oa)) != 0)
    {
        oa_empty(&oa);
        return ret;
    }
    //If register object is not found.
    if (oa.size == 0)
    {
        oa_empty(&oa);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    registerMonitor.monitoredValue.attributeIndex = 2;
    registerMonitor.monitoredValue.objectType = oa.data[0]->objectType;
    memcpy(registerMonitor.monitoredValue.logicalName, oa.data[0]->logicalName, 6);

    action = (gxActionSet*)malloc(sizeof(gxActionSet));
    memcpy(action->actionDown.logicalName, registerMonitor.base.logicalName, 6);
    action->actionDown.scriptSelector = 1;
    memcpy(action->actionUp.logicalName, registerMonitor.base.logicalName, 6);
    action->actionUp.scriptSelector = 1;
    arr_push(&registerMonitor.actions, action);
    oa_empty(&oa);
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add action schedule object.
///////////////////////////////////////////////////////////////////////
int addActionSchedule(
    objectArray *objects)
{
    unsigned char target[6] = { 0, 1, 10, 1, 101,255 };
    gxtime* tm;
    const unsigned char ln[6] = { 0,0,15,0,0,255 };
    cosem_init2((gxObject*)&actionSchedule, DLMS_OBJECT_TYPE_ACTION_SCHEDULE, ln);
    memcpy(actionSchedule.executedScriptLogicalName, target, 6);
    actionSchedule.executedScriptSelector = 1;
    actionSchedule.type = DLMS_SINGLE_ACTION_SCHEDULE_TYPE1;
    tm = (gxtime*)malloc(sizeof(gxtime));
    time_now(tm);
    arr_push(&actionSchedule.executionTime, tm);
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add image transfer object.
///////////////////////////////////////////////////////////////////////
int addImageTransfer(
    objectArray *objects)
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
int addIecHdlcSetup(
    objectArray *objects)
{
    unsigned char ln[6] = { 0,0,22,0,0,255 };
    cosem_init2((gxObject*)&iecHdlcSetup, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln);
    iecHdlcSetup.communicationSpeed = DLMS_BAUD_RATE_9600;
    iecHdlcSetup.windowSizeReceive = iecHdlcSetup.windowSizeTransmit = 1;
    iecHdlcSetup.maximumInfoLengthTransmit = iecHdlcSetup.maximumInfoLengthReceive = 128;
    iecHdlcSetup.inactivityTimeout = 120;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add compact data object.
///////////////////////////////////////////////////////////////////////
int addCompactData(
    objectArray *objects)
{
    unsigned char ln[6] = { 0,0,66,0,1,255 };
    cosem_init2(&compactData.base, DLMS_OBJECT_TYPE_COMPACT_DATA, ln);
    compactData.templateId = 66;
    //Buffer is captured when invoke is called.
    compactData.captureMethod = DLMS_CAPTURE_METHOD_INVOKE;
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add SAP Assignment object.
///////////////////////////////////////////////////////////////////////
int addSapAssignment(
    objectArray *objects)
{
    gxSapItem *it;
    const unsigned char ln[6] = { 0,0,41,0,0,255 };
    cosem_init2((gxObject*)&sapAssignment, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, ln);

    it = (gxSapItem*)malloc(sizeof(gxSapItem));
    it->id = 1;
    bb_init(&it->name);
    char buff[17];
    sprintf(buff, "GRX%.13lu", sn);
    bb_addString(&it->name, buff);
    arr_push(&sapAssignment.sapAssignmentList, it);
    return 0;
}


///////////////////////////////////////////////////////////////////////
//Add Auto Answer object.
///////////////////////////////////////////////////////////////////////
int addAutoAnswer(
    objectArray *objects)
{
    gxtime *start, *end;
    const unsigned char ln[6] = { 0,0,2,2,0,255 };
    cosem_init2((gxObject*)&autoAnswer, DLMS_OBJECT_TYPE_AUTO_ANSWER, ln);

    start = (gxtime*)malloc(sizeof(gxtime));
    time_init3(start, -1, -1, -1, 6, -1, -1, -1);
    end = (gxtime*)malloc(sizeof(gxtime));
    time_init3(end, -1, -1, -1, 8, -1, -1, -1);

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
    objectArray *objects)
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
    objectArray *objects)
{
    const unsigned char ln[6] = { 0,0,25,2,0,255 };
    cosem_init2((gxObject*)&macAddressSetup, DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, ln);
    bb_addString(&macAddressSetup.macAddress, "00:11:22:33:44:55:66");
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Disconnect control object.
///////////////////////////////////////////////////////////////////////
int addDisconnectControl(
    objectArray *objects)
{
    const unsigned char ln[6] = { 0,0,96,3,10,255 };
    cosem_init2((gxObject*)&disconnectControl, DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln);
    return 0;
}

unsigned long getIpAddress()
{
    int ret = -1;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    struct hostent *phe;
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
    objectArray *objects)
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
    objectArray *objects)
{
    const char dest[] = "127.0.0.1:7000";

    gxCaptureObject *co;
    const unsigned char ln[6] = { 0,0,25,9,0,255 };
    cosem_init2((gxObject*)&pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln);
    pushSetup.destination = (char*)malloc(strlen(dest) + 1);
    strcpy(pushSetup.destination, dest);

    // Add push object itself. This is needed to tell structure of data to
    // the Push listener.
    co = (gxCaptureObject*)malloc(sizeof(gxCaptureObject));
    co->attributeIndex = 2;
    co->dataIndex = 0;
    arr_push(&pushSetup.pushObjectList, key_init(&pushSetup, co));
    // Add logical device name.
    co = (gxCaptureObject*)malloc(sizeof(gxCaptureObject));
    co->attributeIndex = 2;
    co->dataIndex = 0;
    arr_push(&pushSetup.pushObjectList, key_init(&ldn, co));
    // Add 0.0.25.1.0.255 Ch. 0 IPv4 setup IP address.
    co = (gxCaptureObject*)malloc(sizeof(gxCaptureObject));
    co->attributeIndex = 3;
    co->dataIndex = 0;
    arr_push(&pushSetup.pushObjectList, key_init(&ip4Setup, co));
    return 0;
}

///////////////////////////////////////////////////////////////////////
//Add script table object.
///////////////////////////////////////////////////////////////////////
int addScriptTable(
    objectArray *objects)
{
    gxScript* s = (gxScript*)malloc(sizeof(gxScript));
    gxScriptAction* a = (gxScriptAction*)malloc(sizeof(gxScriptAction));
    const unsigned char ln[6] = { 0,0,10,0,0,255 };
    cosem_init2((gxObject*)&scriptTable, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln);
    s->id = 1;
    arr_init(&s->actions);
    a->identifier = 2;//Write.

    //TODO: Set object type and logical name, attribute index to execute.
    //In this example it is a clock.
    a->objectType = DLMS_OBJECT_TYPE_CLOCK;
    memcpy(a->logicalName, clock1.base.logicalName, 6);
    a->index = 2;
    var_init(&a->parameter);
    //Add executed action to action list.
    arr_push(&s->actions, a);
    //Add executed script to script list.
    arr_push(&scriptTable.scripts, s);
    return 0;
}

int svr_InitObjects(
    dlmsServerSettings *settings)
{
    int ret;
    objectArray *objects = &settings->base.objects;
    char FW[] = "Gurux FW 1.0.1";
    char buff[17];
    oa_attach(objects, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));

#if defined(DLMS_ITALIAN_STANDARD)
    if ((ret = addItalianAssociation(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addFrameCounters(objects)) != 0)
    {
        return ret;
    }
#else
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
#endif //!defined(DLMS_ITALIAN_STANDARD)

#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    //Add short name referencing.
    if ((ret = addAssociation()) != 0)
    {
        return ret;
    }
#endif //!defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)

    ///////////////////////////////////////////////////////////////////////
    //Add Logical Device Name. 123456 is meter serial number.
    ///////////////////////////////////////////////////////////////////////
    // COSEM Logical Device Name is defined as an octet-string of 16 octets.
    // The first three octets uniquely identify the manufacturer of the device and it corresponds
    // to the manufacturer's identification in IEC 62056-21.
    // The following 13 octets are assigned by the manufacturer.
    //The manufacturer is responsible for guaranteeing the uniqueness of these octets.
    sprintf(buff, "GRX%.13lu", sn);
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
        var_setUInt32(&id2.value, sn);
    }

    //Firmware version.
    {
        const unsigned char ln[6] = { 1,0,0,2,0,255 };
        cosem_init2((gxObject*)&fw.base, DLMS_OBJECT_TYPE_DATA, ln);
        var_setString(&fw.value, FW, (unsigned short)strlen(FW));
    }
    if ((ret = addRegisterObject(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addClockObject(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addTcpUdpSetup(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addProfileGeneric(objects)) != 0)
    {
        return ret;
    }
#ifdef DLMS_INDIAN_STANDARD
    if ((ret = addInstantDataProfileGeneric(objects)) != 0)
    {
        return ret;
    }
#endif //DLMS_INDIAN_STANDARD

    if ((ret = addAutoConnect(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addActivityCalendar(objects)) != 0)
    {
        return ret;
    }

    if ((ret = addOpticalPortSetup(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addDemandRegister(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addRegisterMonitor(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addActionSchedule(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addSapAssignment(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addAutoAnswer(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addModemConfiguration(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addMacAddressSetup(objects)) != 0)
    {
        return ret;
    }

    if ((ret = addDisconnectControl(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addIP4Setup(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addPushSetup(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addScriptTable(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addImageTransfer(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addIecHdlcSetup(objects)) != 0)
    {
        return ret;
    }
    if ((ret = addCompactData(objects)) != 0)
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
    connection *con,
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
    gxValueEventArg *e)
{
#ifndef DLMS_ITALIAN_STANDARD
    if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        if (settings->authentication == DLMS_AUTHENTICATION_NONE)
        {
            e->target = &associationNone.base;
        }
        else if (settings->authentication == DLMS_AUTHENTICATION_LOW)
        {
            e->target = &lowAssociation.base;
        }
        else
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
    time_init3(&last, year, month, day, hour, minute, second, 0);
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
            time_init3(&tm, year, month, day, hour, minute, second, 0);
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
    dlmsVARIANT *tmp;
    variantArray *row;
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
                    row = (variantArray *)malloc(sizeof(variantArray));
                    va_init(row);
                    arr_push(&p->buffer, row);

                    //Add date time.
                    tmp = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
                    var_init(tmp);
                    tmp->dateTime = (gxtime*)malloc(sizeof(gxtime));
                    time_init3(tmp->dateTime, year, month, day, hour, minute, second, 0);
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
    dlmsVARIANT *it;
    gxtime tm, start, end, last;
    int ret;
    unsigned short pos = 0;
    dlmsVARIANT tmp;
    var_init(&tmp);
    time_init3(&last, year, month, day, hour, minute, second, 0);
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
                time_init3(&tm, year, month, day, hour, minute, second, 0);
                //If value is inside of start and end time.
                if (time_compare(&tm, &start) >= 0 && time_compare(&tm, &end) <= 0)
                {
                    if (last.value.tm_year == 71)
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
int getRestrictingObject(dlmsSettings* settings, gxValueEventArg *e, gxObject **obj, short *index)
{
    int ret;
    dlmsVARIANT *it, *it2;
    if ((ret = va_getByIndex(e->parameters.Arr, 0, &it)) != 0)
    {
        return ret;
    }
    DLMS_OBJECT_TYPE ot;
    unsigned char *ln;
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
    dlmsVARIANT *it;
    gxtime tm, start, end;
    int ret;
    dlmsVARIANT tmp;
    var_init(&tmp);
    gxObject *obj = NULL;
    short index;
    getRestrictingObject(settings, e, &obj, &index);
    if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) != 0)
    {
        return ret;
    }
    if (it->vt == DLMS_DATA_TYPE_UINT32)
    {
        time_init4(&start, (time_t)it->ulVal);
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
        time_init4(&end, (time_t)it->ulVal);
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
                time_init3(&tm, year, month, day, hour, minute, second, 0);
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
    gxValueEventArg *e;
    dlmsVARIANT* value;
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
                        dlmsVARIANT *it;
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
                unsigned short count = e->transactionEndIndex - e->transactionStartIndex;
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
                    unsigned short index = e->transactionStartIndex;
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
            //If value is update by write that value is returned.
            if (((gxClock*)e->target)->time.value.tm_year == 0)
            {
                time_now(&((gxClock*)e->target)->time);
            }
        }
        else if (type == DLMS_OBJECT_TYPE_REGISTER_MONITOR)
        {
            if (e->index == 2)
            {
                //Initialize random seed.
                srand((unsigned int)time(NULL));
                va_clear(&((gxRegisterMonitor*)e->target)->thresholds);
                value = (dlmsVARIANT*)malloc(sizeof(dlmsVARIANT));
                var_init(value);
                var_setUInt16(value, rand() % 100 + 1);
                va_push(&((gxRegisterMonitor*)e->target)->thresholds, value);
            }
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
    gxValueEventArg *e;
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

    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
}

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
int sendPush(gxPushSetup* push);
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
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
#ifndef GX_DLMS_MICROCONTROLLER
        hlp_getLogicalNameToString(e->target->logicalName, str);
        printf("Action %s:%d\n", str, e->index);
#endif
        //If disconnect control is called it must handle here:
        if (e->target->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL)
        {
            //Mark action as handled because there is nothing what framework can do with this.
            e->handled = 1;
        }
        if (e->target->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP && e->index == 1)
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            sendPush((gxPushSetup*)e->target);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            e->handled = 1;
        }
        if (e->target->objectType == DLMS_OBJECT_TYPE_IMAGE_TRANSFER)
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            FILE *f;
            gxImageTransfer* i = (gxImageTransfer*)e->target;
            //Image name and size to transfer
            if (e->index == 1)
            {
                i->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
                dlmsVARIANT *ImageUpdate, *size;
                if ((ret = va_getByIndex(e->parameters.Arr, 0, &ImageUpdate)) != 0 ||
                    (ret = va_getByIndex(e->parameters.Arr, 1, &size)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_UNMATCH_TYPE;
                    return;
                }
                imageSize = var_toInteger(size);
                char *p = strrchr(IMAGEFILE, '\\');
                ++p;
                *p = '\0';
                strncat(IMAGEFILE, (char*)ImageUpdate->byteArr->data, (int)ImageUpdate->byteArr->size);
                strcat(IMAGEFILE, ".bin");
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                printf("Updating image %s Size: %d\n", IMAGEFILE, imageSize);
#endif
                f = fopen(IMAGEFILE, "wb");
                if (!f)
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Unable to open file %s\n", IMAGEFILE);
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
                dlmsVARIANT *blockNo, *data;
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
                    printf("Unable to open file %s\n", IMAGEFILE);
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
                    printf("Unable to open file %s\n", IMAGEFILE);
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
                        printf("Image verification is on progress.\n");
#endif
                        e->error = DLMS_ERROR_CODE_TEMPORARY_FAILURE;
                    }
                    else
                    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                        printf("Image is verificated");
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
                    printf("Image activation is on progress.\n");
#endif
                    e->error = DLMS_ERROR_CODE_TEMPORARY_FAILURE;
                }
                else
                {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
                    printf("Image is activated.");
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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    gxValueEventArg *e;
    int ret, pos;
    char* buff;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) == 0)
        {
#ifndef GX_DLMS_MICROCONTROLLER
            if ((ret = obj_toString(e->target, &buff)) != 0)
            {
                printf("svr_postWrite::obj_toString failed %d.", ret);
                break;
            }
            printf(buff);
            free(buff);
#endif //GX_DLMS_MICROCONTROLLER
        }
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
}

void handleProfileGenericActions(
    gxValueEventArg* it)
{
    if (it->index == 1)
    {
        // Profile generic clear is called. Clear data.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
        FILE* f = NULL;
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
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
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
    const char *address,
    int port,
    int *s)
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
        struct hostent *Hostent = gethostbyname(address);
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
    gxPushSetup* push)
{
    char *p, *host;
    int ret, pos, port, s;
    dlmsSettings cl;
    message messages;
    gxByteBuffer *bb;

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
        cl_init(&cl, 1, 1, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_WRAPPER);
        if ((ret = notify_generatePushSetupMessages(&cl, NULL, push, &messages)) == 0)
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
        cl_clear(&cl);
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
    dlmsSettings *settings,
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
    //Check server address using serial number.
    if ((serverAddress & 0x3FFF) == sn % 10000 + 1000)
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
#if defined(DLMS_ITALIAN_STANDARD)
#else
    //Check used security level.
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->base.cipher.security == DLMS_SECURITY_NONE)
    {
        //Uncomment this if you want that only Get service is available with authentication level None.
        //settings->base.negotiatedConformance &= DLMS_CONFORMANCE_GET;
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
        else
        {
#ifdef DLMS_INDIAN_STANDARD
            return DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
#else
            expected = associationShortName.secret;
#endif
        }
        if (bb_compare(&expected, password->data, password->size) == 0)
        {
            return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
        }
    }
#endif //DLMS_ITALIAN_STANDARD
    // Other authentication levels are check on phase two.
    return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings *settings,
    gxObject *obj,
    unsigned char index)
{
    if (index == 1)
    {
        return DLMS_ACCESS_MODE_READ;
    }
    // Only read is allowed
    if (settings->authentication == DLMS_AUTHENTICATION_NONE)
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
    dlmsSettings *settings,
    gxObject *obj,
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
    dlmsServerSettings *settings)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    printf("Connected %d.\r\n", settings->base.connected);
#endif
#ifdef DLMS_ITALIAN_STANDARD
    if (settings->base.clientAddress == 1)
    {
        if (settings->base.connected == DLMS_CONNECTION_STATE_HDLC)
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
            bb_addString(settings->base.preEstablishedSystemTitle, "Gurux123");
            settings->base.cipher.security = DLMS_SECURITY_AUTHENTICATION_ENCRYPTION;
        }
        else
        {
            //Return error if client can connect only using pre-established connnection.
            return DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
    }
#endif //DLMS_ITALIAN_STANDARD
    return 0;
}

/**
    * Client has try to made invalid connection. Password is incorrect.
    *
    * @param connectionInfo
    *            Connection information.
    */
int svr_invalidConnection(dlmsServerSettings *settings)
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
    dlmsServerSettings *settings)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    printf("Disconnected %d.\r\n", settings->base.connected);
#endif
    return 0;
}

//Capture data to the ring buffer.
void captureToRingBuffer(gxProfileGeneric* pg)
{
    gxtime tm;
    int cnt = getProfileGenericDataCount() + 1;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, DATAFILE, "a");
#else
    FILE* f = fopen(DATAFILE, "a");
#endif
    obj_clearProfileGenericBuffer(&pg->buffer);
    // Update last average value.
    time_now(&tm);
    fprintf(f, "%d/%d/%d %d:%d:%d;%d\n", 1 + tm.value.tm_mon, tm.value.tm_mday, 1900 + tm.value.tm_year, tm.value.tm_hour, tm.value.tm_min, tm.value.tm_sec, cnt);
    fclose(f);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
}

void capture2File(gxProfileGeneric* pg)
{
    gxtime tm;
    int cnt = getProfileGenericDataCount() + 1;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
    FILE* f = NULL;
    fopen_s(&f, DATAFILE, "a");
#else
    FILE* f = fopen(DATAFILE, "a");
#endif
    obj_clearProfileGenericBuffer(&pg->buffer);
    // Update last average value.
    time_now(&tm);
    fprintf(f, "%d/%d/%d %d:%d:%d;%d\n", 1 + tm.value.tm_mon, tm.value.tm_mday, 1900 + tm.value.tm_year, tm.value.tm_hour, tm.value.tm_min, tm.value.tm_sec, cnt);
    fclose(f);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
}

void svr_preGet(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
    gxValueEventArg *e;
    int ret, pos;
    for (pos = 0; pos != args->size; ++pos)
    {
        if ((ret = vec_getByIndex(args, pos, &e)) != 0)
        {
            return;
        }
        //Save data to the file.
        if (e->target->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            gxProfileGeneric* pg = (gxProfileGeneric*)e->target;
            if (useRingBuffer)
            {
                captureToRingBuffer(pg);
            }
            else
            {
                capture2File(pg);
            }
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