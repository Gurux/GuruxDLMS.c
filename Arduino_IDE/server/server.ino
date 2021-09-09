//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// More information of Gurux products: http://www.gurux.org
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------
#include <EEPROM.h>
#include "GXDLMSServer.h"
//---------------------------------------------------------------------------
// Un-comment ignored objects from ArduinoIgnore.h to improve performance
// and memory usage.
//---------------------------------------------------------------------------

//Ignore time_now warning.
#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"


unsigned char hdlcChanged = 0;
const static char *FLAG_ID = "GRX";
const static uint16_t SERIALIZATION_VERSION = 1;

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

#define USE_TIME_INTERRUPT

static gxByteBuffer reply;

//Define profile generic buffer row data for Load profile.
typedef struct
{
  //Date-time value of the clock.
  uint32_t time;
  //Active power L1 value.
  uint16_t activePowerL1;
} gxLoadProfileData;

//Define profile generic buffer row data for event log.
typedef struct
{
  //Date-time value of the clock.
  uint32_t time;
  //Occurred event.
  uint16_t event;
} gxEventLogData;

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
} GXPushObjects;

//Profile Generic settings.
typedef struct
{
  //How often load profile is captured to the EEPROM.
  //Load profile period.
  unsigned long period;
  //Amount of rows in profile generic buffer.
  uint16_t entriesInUse;
  //Entry where profile generic row is added in ring buffer.
  uint16_t rowIndex;
} GXProfileGeneric;


//Push Setup.
typedef struct
{
  //Push communication window.
  GXPushWindow communicationWindow;
  //Push object list..
  GXPushObjects objects;
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
  unsigned char llsPassword[20];
  //Define high level password.
  unsigned char hlsPasswordSize;
  unsigned char hlsPassword[20];
} GXAssociation;

//Save serialized meter data here.
typedef struct {
  //Meter serial number.
  uint32_t SERIAL_NUMBER = 123456;
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
  char testMode = 1;
  //SAP assigment lists.
  GXSapList sapAssignmentList;
} GXSerializedMeterData;

GXSerializedMeterData meterData;
//Push objects are added here.
static gxTarget PUSH_OBJECTS[6];

static gxData ldn;
static gxData eventCode;
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
static gxProfileGeneric profileGeneric;
static gxSapAssignment sapAssignment;
static gxSecuritySetup securitySetupLow;
static gxSecuritySetup securitySetupHigh;

//static gxObject* NONE_OBJECTS[] = { BASE(associationNone), BASE(ldn), BASE(sapAssignment), BASE(meterData.clock1) };

static gxObject* ALL_OBJECTS[] = { BASE(associationNone), BASE(associationLow), BASE(associationHigh), BASE(associationHighGMac), BASE(securitySetupLow), BASE(securitySetupHigh),
                                   BASE(ldn), BASE(sapAssignment), BASE(eventCode),
                                   BASE(meterData.clock1), BASE(activePowerL1), BASE(pushSetup), BASE(scriptTableGlobalMeterReset), BASE(scriptTableDisconnectControl),
                                   BASE(scriptTableActivateTestMode), BASE(scriptTableActivateNormalMode), BASE(profileGeneric), BASE(eventLog), BASE(meterData.hdlc),
                                   BASE(disconnectControl), BASE(actionScheduleDisconnectOpen), BASE(actionScheduleDisconnectClose)
                                 };

#ifndef USE_TIME_INTERRUPT
static uint32_t started = 0;
#endif //USE_TIME_INTERRUPT

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

void captureEventLog(uint16_t value)
{
  gxEventLogData row;
  // Profile generic Capture is called. Save data to the buffer.
  // Ring buffer is used here.
  row.time = time_current();
  row.event = value;
  EEPROM.put(2048 + meterData.eventLog.rowIndex * sizeof(gxEventLogData), row);
  //Update how many entries is used until buffer is full.
  if (eventLog.entriesInUse != eventLog.profileEntries)
  {
    ++eventLog.entriesInUse;
    meterData.eventLog.entriesInUse = eventLog.entriesInUse;
  }
  //Update row index where next row is added.
  ++meterData.eventLog.rowIndex;
  meterData.eventLog.rowIndex = meterData.eventLog.rowIndex % eventLog.profileEntries;
  save(&meterData.eventLog, sizeof(GXProfileGeneric));
}

void updateState(uint16_t value)
{
  GX_UINT16(eventCode.value) = value;
  captureEventLog(value);
}

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
    byte c;
    Serial1.write("\t:", 2);
    while ((c = pgm_read_byte(str++)) != 0)
    {
      Serial1.write(c);
    }
    if (data != NULL)
    {
      Serial1.write(data, strlen(data));
    }
    Serial1.write("\0", 1);
    //Serial1.flush();
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
// Returns the number of milliseconds passed since the Arduino board began running the current program.
// This number will overflow (go back to zero), after approximately 50 days.
///////////////////////////////////////////////////////////////////////
uint32_t time_elapsed(void)
{
  return millis();
}

///////////////////////////////////////////////////////////////////////
//Get current time.
//Note! Real clock is missing from the Arduino.
//Because there is no clock, clock object keeps base time and uptime is added to that.
///////////////////////////////////////////////////////////////////////
uint32_t time_current(void)
{
#ifdef USE_TIME_INTERRUPT
  return time_toUnixTime2(&meterData.clock1.time);
#else
  uint32_t tmp = time_elapsed() - started;
  tmp /= 1000;
  tmp += time_toUnixTime2(&meterData.clock1.time);
  return tmp;
#endif //USE_TIME_INTERRUPT
}

///////////////////////////////////////////////////////////////////////
// Returns current time.
// Get current time.
// Because there is no clock, clock object keeps base time and uptime is added to that.
///////////////////////////////////////////////////////////////////////
void time_now(gxtime* value, unsigned char meterTime)
{
#ifdef USE_TIME_INTERRUPT
  value = &meterData.clock1.time;
#else
  uint32_t offset = time_current();
  time_initUnix(value, offset);
  //If date time is wanted in meter time, not in utc time.
  if (meterTime)
  {
    clock_utcToMeterTime(&meterData.clock1, value);
  }
#endif //USE_TIME_INTERRUPT
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
    associationNone.xDLMSContextInfo.conformance = DLMS_CONFORMANCE_GET;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociationLow(uint16_t serializationVersion)
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
    associationLow.xDLMSContextInfo.conformance =  (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
        DLMS_CONFORMANCE_SET |
        DLMS_CONFORMANCE_SELECTIVE_ACCESS |
        DLMS_CONFORMANCE_ACTION |
        DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
        DLMS_CONFORMANCE_GET);
    if (serializationVersion == 0)
    {
      memcpy(meterData.association.llsPassword, "Gurux", 5);
      meterData.association.llsPasswordSize = 5;
    }
    BB_ATTACH(associationLow.secret, meterData.association.llsPassword, meterData.association.llsPasswordSize);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    associationLow.securitySetup = &securitySetupLow;
#else
    memcpy(associationLow.securitySetupReference, securitySetupLow.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
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
  //User list.
  static gxUser USER_LIST[10] = { 0 };
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
    associationHigh.xDLMSContextInfo.conformance =  (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
        DLMS_CONFORMANCE_SET |
        DLMS_CONFORMANCE_SELECTIVE_ACCESS |
        DLMS_CONFORMANCE_ACTION |
        DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
        DLMS_CONFORMANCE_GET);
    if (serializationVersion == 0)
    {
      memcpy(meterData.association.hlsPassword, "Gurux", 5);
      meterData.association.hlsPasswordSize = 5;
    }
    BB_ATTACH(associationHigh.secret, meterData.association.hlsPassword, meterData.association.hlsPasswordSize);
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
  //User list.
  static gxUser USER_LIST[10] = { 0 };
  //If you are getting 'gxUser' does not name a type; did you mean 'gxKey'?
  //Uncomment #define DLMS_IGNORE_MALLOC from gxignore.h
  //This error is coming because this example is not using malloc
  //and there are no way to give compiler arguments in Arduino IDE.

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
    associationHighGMac.xDLMSContextInfo.conformance =  (DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
        DLMS_CONFORMANCE_SET |
        DLMS_CONFORMANCE_SELECTIVE_ACCESS |
        DLMS_CONFORMANCE_ACTION |
        DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
        DLMS_CONFORMANCE_GET);
    //GMAC authentication don't need password.
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    associationHighGMac.securitySetup = &securitySetupHigh;
#else
    memcpy(associationHighGMac.securitySetupReference, securitySetupHigh.base.logicalName, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////
//This method adds security setup object for Low authentication.
///////////////////////////////////////////////////////////////////////
int addSecuritySetupLow()
{
  int ret;
  //Define client system title.
  static unsigned char CLIENT_SYSTEM_TITLE[8] = { 0 };
  const unsigned char ln[6] = { 0, 0, 43, 0, 1, 255 };
  if ((ret = INIT_OBJECT(securitySetupLow, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln)) == 0)
  {
    BB_ATTACH(securitySetupLow.serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
    BB_ATTACH(securitySetupLow.clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
    securitySetupLow.securityPolicy = DLMS_SECURITY_POLICY_NOTHING;
    securitySetupLow.securitySuite = DLMS_SECURITY_SUITE_V0;
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
  const unsigned char ln[6] = { 0, 0, 43, 0, 2, 255 };
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
      sprintf((char*) meterData.sapAssignmentList.values[0].name.value, "%s%.13lu", FLAG_ID, meterData.SERIAL_NUMBER);
      meterData.sapAssignmentList.values[0].name.size = 16;
      meterData.sapAssignmentList.values[0].id = 1;
      meterData.sapAssignmentList.count = 1;
    }
    ARR_ATTACH(sapAssignment.sapAssignmentList, meterData.sapAssignmentList.values, meterData.sapAssignmentList.count);
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
    sprintf(meterData.LDN, "%s%.13lu", FLAG_ID, meterData.SERIAL_NUMBER);
    GX_OCTET_STRING(ldn.value, meterData.LDN, sizeof(meterData.LDN));
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

static int loadTargets(GXPushObjects* savedObjects, gxTarget* objects, uint16_t* count)
{
  uint16_t pos;
  int ret = 0;
  gxObject* it;
  *count = savedObjects->count;
  for (pos = 0; pos != savedObjects->count; ++pos)
  {
    if ((ret = oa_findByLN(Server.getObjects(), (DLMS_OBJECT_TYPE) savedObjects->values[pos].objectType, savedObjects->values[pos].logicalName, &it)) != 0)
    {
      break;
    }
    if (it == NULL)
    {
      *count = pos;
      ret = -1;
      break;
    }
    objects[pos].target = it;
    objects[pos].attributeIndex = savedObjects->values[pos].attributeIndex;
    objects[pos].dataIndex = savedObjects->values[pos].dataIndex;
  }
  return ret;
}

static int saveTargets(GXPushObjects* savedObjects, gxTarget* objects, uint16_t count)
{
  uint16_t pos;
  savedObjects->count = count;
  for (pos = 0; pos != count; ++pos)
  {
    memcpy(savedObjects->values[pos].logicalName, objects[pos].target->logicalName, 6);
    savedObjects->values[pos].objectType = objects[pos].target->objectType;
    savedObjects->values[pos].attributeIndex = objects[pos].attributeIndex;
    savedObjects->values[pos].dataIndex = objects[pos].dataIndex;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////
//Add push setup object. (On Connectivity)
///////////////////////////////////////////////////////////////////////
int addPushSetup(uint16_t serializationVersion)
{
  int ret;
  const unsigned char ln[6] = { 0, 0, 25, 9, 0, 255 };
  if ((ret = INIT_OBJECT(pushSetup, DLMS_OBJECT_TYPE_PUSH_SETUP, ln)) == 0)
  {
    pushSetup.service = DLMS_SERVICE_TYPE_HDLC;
    if (serializationVersion == 0)
    {
      //This push is sent every minute, but max 10 seconds over.
      time_init(&meterData.push.communicationWindow.values[0].first, -1, -1, -1, -1, -1, 0, 0, 0);
      time_init(&meterData.push.communicationWindow.values[0].second, -1, -1, -1, -1, -1, 10, 0, 0);
      //This push is sent every half minute, but max 40 seconds over.
      time_init(&meterData.push.communicationWindow.values[1].first, -1, -1, -1, -1, -1, 30, 0, 0);
      time_init(&meterData.push.communicationWindow.values[1].second, -1, -1, -1, -1, -1, 40, 0, 0);
      meterData.push.communicationWindow.count = 2;
      // Add logical device name.
      memcpy(meterData.push.objects.values[0].logicalName, BASE(ldn).logicalName, 6);
      meterData.push.objects.values[0].objectType = ldn.base.objectType;
      meterData.push.objects.values[0].attributeIndex = 2;
      meterData.push.objects.values[0].dataIndex = 0;
      // Add push object logical name. This is needed to tell structure of data to the Push listener.
      // Also capture object list can be used here.
      memcpy(meterData.push.objects.values[1].logicalName, BASE(pushSetup).logicalName, 6);
      meterData.push.objects.values[1].objectType = pushSetup.base.objectType;
      meterData.push.objects.values[1].attributeIndex = 1;
      meterData.push.objects.values[1].dataIndex = 0;
      //Update amount of push objects.
      meterData.push.objects.count = 2;
    }
    ARR_ATTACH(pushSetup.communicationWindow, meterData.push.communicationWindow.values, meterData.push.communicationWindow.count);
    ARR_ATTACH(pushSetup.pushObjectList, PUSH_OBJECTS, 0);
  }
  return ret;
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
  const unsigned char ln[6] = { 0, 0, 10, 0, 101, 255};
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
  const unsigned char ln[6] = { 0, 0, 10, 0, 102, 255};
  if ((ret = INIT_OBJECT(scriptTableActivateNormalMode, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln)) == 0)
  {
    SCRIPTS[0].id = 1;
    ARR_ATTACH(scriptTableActivateNormalMode.scripts, SCRIPTS, 1);
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addLoadProfileProfileGeneric()
{
  int ret;
  //Two capture objects.
  static gxTarget CAPTURE_OBJECT[2] = { 0 };
  const unsigned char ln[6] = { 1, 0, 99, 1, 0, 255 };
  if ((ret = INIT_OBJECT(profileGeneric, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
  {
    profileGeneric.capturePeriod = meterData.loadProfile.period;
    //Maximum row count. One row takes 6 bytes. Allocate 150 rows = 900B.
    profileGeneric.profileEntries = 150;
    profileGeneric.sortMethod = DLMS_SORT_METHOD_FIFO;
    //entries in use.
    profileGeneric.entriesInUse = meterData.loadProfile.entriesInUse;
    ///////////////////////////////////////////////////////////////////
    //Add 2 columns.
    ARR_ATTACH(profileGeneric.captureObjects, CAPTURE_OBJECT, 2);
    //Add clock obect.
    CAPTURE_OBJECT[0].target = BASE(meterData.clock1);
    CAPTURE_OBJECT[0].attributeIndex = 2;
    CAPTURE_OBJECT[0].dataIndex = 0;
    //Add active power.
    CAPTURE_OBJECT[1].target = BASE(activePowerL1);
    CAPTURE_OBJECT[1].attributeIndex = 2;
    CAPTURE_OBJECT[1].dataIndex = 0;
    //Set clock to sort object.
    profileGeneric.sortObject = BASE(meterData.clock1);
    profileGeneric.sortObjectAttributeIndex = 2;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////
//Add profile generic (historical data) object.
///////////////////////////////////////////////////////////////////////
int addEventLogProfileGeneric()
{
  int ret;
  //Two capture objects. Clock and event code
  static gxTarget CAPTURE_OBJECT[2] = { 0 };
  const unsigned char ln[6] = { 1, 0, 99, 98, 0, 255 };
  if ((ret = INIT_OBJECT(eventLog, DLMS_OBJECT_TYPE_PROFILE_GENERIC, ln)) == 0)
  {
    //events are not captured.
    eventLog.capturePeriod = 0;
    //Maximum row count. One row takes 6 bytes. Allocate 150 rows = 900B.
    eventLog.profileEntries = 150;
    eventLog.sortMethod = DLMS_SORT_METHOD_FIFO;
    //entries in use.
    eventLog.entriesInUse = meterData.eventLog.entriesInUse;
    ///////////////////////////////////////////////////////////////////
    //Add 2 columns.
    ARR_ATTACH(eventLog.captureObjects, CAPTURE_OBJECT, 2);
    //Add clock obect.
    CAPTURE_OBJECT[0].target = BASE(meterData.clock1);
    CAPTURE_OBJECT[0].attributeIndex = 2;
    CAPTURE_OBJECT[0].dataIndex = 0;
    //Add active power.
    CAPTURE_OBJECT[1].target = BASE(eventCode);
    CAPTURE_OBJECT[1].attributeIndex = 2;
    CAPTURE_OBJECT[1].dataIndex = 0;
    //Set clock to sort object.
    eventLog.sortObject = BASE(meterData.clock1);
    eventLog.sortObjectAttributeIndex = 2;
  }
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
//Add IEC HDLC Setup object.
///////////////////////////////////////////////////////////////////////
int addIecHdlcSetup(uint16_t serializationVersion)
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
  Server.setHdlc(&meterData.hdlc);
  return ret;
}

#ifdef USE_TIME_INTERRUPT

#ifndef ESP_PLATFORM
// Increase the time once per second.
ISR(TIMER1_COMPA_vect) {
  ++meterData.clock1.time.value;
}
#else
hw_timer_t * timer = NULL;
void IRAM_ATTR onTime() {
  ++meterData.clock1.time.value;
}
#endif //ESP_PLATFORM

/*
  Use interrupts to increase the time.
*/
void setTimer()
{
#ifndef ESP_PLATFORM
    //Stop interrupts.
    cli();
    //set timer1 interrupt at 1Hz
    TCCR1A = 0;
    TCCR1B = 0;
    //Initialize counter value to 0
    TCNT1 = 0;
    // set compare match register for 1hz increments
    OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be < 65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12 and CS10 bits for 1024 prescaler
    TCCR1B |= (1 << CS12) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    //Allow interrupts.
    sei();
#else
   timer = timerBegin(0, 80, true);                
   timerAttachInterrupt(timer, &onTime, true);    
   timerAlarmWrite(timer, 1000000, true);           
   timerAlarmEnable(timer);
#endif //ESP_PLATFORM
}

#endif //USE_TIME_INTERRUPT

/////////////////////////////////////////////////////////////////////////////
// Save data to the EEPROM.
//
// Only updated value is saved. This is done because write to EEPROM is slow
// and there is a limit how many times value can be written to the EEPROM.
/////////////////////////////////////////////////////////////////////////////
void save(void* data, uint16_t size)
{
  uint16_t offset = sizeof(uint16_t) + ((unsigned char*)data - (unsigned char*)&meterData);
  uint16_t pos;
  for (pos = 0; pos != size; ++pos)
  {
#ifdef ARDUINO_ARCH_ESP32
    EEPROM.write(offset, ((unsigned char*)data)[pos]);
#else
    EEPROM.update(offset, ((unsigned char*)data)[pos]);
#endif //ARDUINO_ARCH_ESP32
    ++offset;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Save meter data size to the EEPROM.
//
// Only updated value is saved. This is done because write to EEPROM is slow
// and there is a limit how many times value can be written to the EEPROM.
/////////////////////////////////////////////////////////////////////////////
void updateMeterDataSize()
{
  uint16_t data = sizeof(meterData);
  uint16_t pos;
  for (pos = 0; pos != sizeof(uint16_t); ++pos)
  {
#ifdef ARDUINO_ARCH_ESP32
    EEPROM.write(pos, ((unsigned char*)data)[pos]);
#else
    EEPROM.update(pos, ((unsigned char*)data)[pos]);
#endif //ARDUINO_ARCH_ESP32
  }
}

/////////////////////////////////////////////////////////////////////////////
// Load data from the EEPROM.
// Returns serialization version or zero if data is not saved.
/////////////////////////////////////////////////////////////////////////////
uint16_t load()
{
  uint16_t serializationVersion;
  EEPROM.get(0, serializationVersion);
  if (serializationVersion != 0xFFFF && serializationVersion != 0)
  {
    uint16_t pos;
    for (pos = 0; pos != sizeof(meterData); ++pos)
    {
      ((unsigned char*)&meterData)[pos] = EEPROM.read(sizeof(uint16_t) + pos);
    }
    return serializationVersion;
  }
  return 0;
}

//Create objects and load values from EEPROM.
void createObjects()
{
  int ret;
  uint16_t serializationVersion = load();
  if ((ret = addLogicalDeviceName()) != 0 ||
      (ret = addSapAssignment(serializationVersion)) != 0 ||
      (ret = addEventCode()) != 0 ||
      (ret = addClockObject(serializationVersion)) != 0 ||
      (ret = addRegisterObject()) != 0 ||
      (ret = addAssociationNone()) != 0 ||
      (ret = addAssociationLow(serializationVersion)) != 0 ||
      (ret = addAssociationHigh(serializationVersion)) != 0 ||
      (ret = addAssociationHighGMac(serializationVersion)) != 0 ||
      (ret = addSecuritySetupLow()) != 0 ||
      (ret = addSecuritySetupHigh()) != 0 ||
      (ret = addPushSetup(serializationVersion)) != 0 ||
      (ret = addscriptTableGlobalMeterReset()) != 0 ||
      (ret = addscriptTableDisconnectControl()) != 0 ||
      (ret = addscriptTableActivateTestMode()) != 0 ||
      (ret = addscriptTableActivateNormalMode()) != 0 ||
      (ret = addLoadProfileProfileGeneric()) != 0 ||
      (ret = addEventLogProfileGeneric()) != 0 ||
      (ret = addActionScheduleDisconnectOpen()) != 0 ||
      (ret = addActionScheduleDisconnectClose()) != 0 ||
      (ret = addDisconnectControl()) != 0 ||
      (ret = addIecHdlcSetup(serializationVersion)) != 0 ||
      (ret = Server.initObjects(ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]))) != 0 ||
      //Start server
      (ret = Server.initialize()) != 0)
  {
    GXTRACE_INT(PSTR("Failed to start the meter!"), ret);
    executeTime = 0;
    return;
  }
  //Load Push objects from EEPROM. This is done after all the objects are initialized.
  if ((ret = loadTargets(&meterData.push.objects, PUSH_OBJECTS, &pushSetup.pushObjectList.size)) != 0)
  {
    GXTRACE_INT(PSTR("Failed to load Push setup objects"), ret);
  }

  if (serializationVersion != SERIALIZATION_VERSION)
  {
    EEPROM.put(0, SERIALIZATION_VERSION);
    //Save default settings to the EEPROM.
    save(&meterData, sizeof(meterData));
  }
  GXTRACE(PSTR("Meter started."), NULL);
}

//Get HDLC communication speed.
int32_t getCommunicationSpeed()
{
  int32_t ret = 9600;
  switch (meterData.hdlc.communicationSpeed)
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

void setup() {
  //Add FLAG ID.
  memcpy(SERVER_SYSTEM_TITLE, FLAG_ID, 3);
  //ADD serial number.
  memcpy(SERVER_SYSTEM_TITLE + 4, &meterData.SERIAL_NUMBER, 4);

#ifndef ARDUINO_ARCH_ESP32
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
#endif //ARDUINO_ARCH_ESP32
  bb_attach(&reply, replyFrame, 0, sizeof(replyFrame));
  Server.init(true, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, frameBuff, sizeof(frameBuff), pduBuff, sizeof(pduBuff));
  Server.SetPushClientAddress(64);
  //Serial 1 is used to send trace.
  Serial1.begin(9600);
  createObjects();
  //Set default clock.
  Server.setDefaultClock(&meterData.clock1);
  char testMode = meterData.testMode;
  meterData.testMode = 1;
  //Communication speed is trace always because this is causing most of the problems.
  GXTRACE_INT(PSTR("Communication speed"), getCommunicationSpeed());
  if (testMode)
  {
    GXTRACE(PSTR("Test mode Is ON."), NULL);
  }
  else
  {
    GXTRACE(PSTR("Test mode Is OFF."), NULL);
  }
  GXTRACE(PSTR("LLS password"), (char*)associationLow.secret.data);
  GXTRACE(PSTR("HLS password"), (char*)meterData.association.hlsPassword);
  meterData.testMode = testMode;

  // start serial port with wanted communication speed:
  Serial.begin(getCommunicationSpeed());
  while (!Serial) {
    //  ; // wait for serial port to connect. Needed for native USB port only
  }
  updateState(GURUX_EVENT_CODES_POWER_UP);
  GXTRACE_INT(PSTR("EEPROM size"), EEPROM.length());

#ifdef USE_TIME_INTERRUPT
  setTimer();
#endif //USE_TIME_INTERRUPT
  executeTime = 0;
  GXTRACE(PSTR("Server started."), NULL);
}

void loop() {
  static unsigned char tmp[50];
  uint32_t start = time_current();
  int available;
  //Execute invokes when needed.
  if (executeTime <= start)
  {
    Server.run(start, &executeTime);
  }
  available = Serial.available();
  if (available > 0)
  {
    if (available > sizeof(tmp))
    {
      available = sizeof(tmp);
    }
    available = Serial.readBytes(tmp, available);
    if ((available = Server.handleRequest(tmp, available, &reply)) != 0)
    {
      GXTRACE_INT(PSTR("handleRequest failed:"), available);
      bb_clear(&reply);
    }
    if (reply.size != 0)
    {
      //Send reply.
      Serial.write(reply.data, reply.size);
      Serial.flush();
      bb_clear(&reply);
    }
  }
}


int svr_findObject(
  dlmsSettings* settings,
  DLMS_OBJECT_TYPE objectType,
  int sn,
  unsigned char* ln,
  gxValueEventArg* e)
{
  GXTRACE_LN(PSTR("findObject"), objectType, ln);
  if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
  {
    if (settings->authentication == DLMS_AUTHENTICATION_NONE)
    {
      e->target = BASE(associationNone);
    }
    else if (settings->authentication == DLMS_AUTHENTICATION_LOW)
    {
      e->target = BASE(associationLow);
    }
    else if (settings->authentication == DLMS_AUTHENTICATION_HIGH)
    {
      e->target = BASE(associationHigh);
    }
    else if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
    {
      e->target = BASE(associationHighGMac);
    }
  }
  return 0;
}

/**
  Find restricting object.
*/
int getRestrictingObject(dlmsSettings* settings, gxValueEventArg* e, gxObject** obj, short* index)
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
  unsigned char type,
  gxValueEventArg* e)
{
  gxtime start;
  gxtime end;
  uint32_t pos;
  uint32_t last = 0;
  int ret;
  gxObject* obj = NULL;
  short index;
  if ((ret = getRestrictingObject(settings, e, &obj, &index)) != 0 ||
      (ret = cosem_getDateTimeFromOctetString(e->parameters.byteArr, &start)) != 0 ||
      (ret = cosem_getDateTimeFromOctetString(e->parameters.byteArr, &end)) != 0)
  {
    return ret;
  }
  uint32_t s = time_toUnixTime2(&start);
  uint32_t l = time_toUnixTime2(&end);
  uint32_t t;
  gxLoadProfileData lp;
  gxEventLogData event1;
  gxProfileGeneric* p = (gxProfileGeneric*)e->target;
  for (pos = 0; pos != p->entriesInUse; ++pos)
  {
    //Load data from EEPROM.
    if (type == 0)
    {
      EEPROM.get(1024 + (pos * sizeof(gxLoadProfileData)), lp);
      t = lp.time;
    }
    else
    {
      EEPROM.get(2048 + (pos * sizeof(gxEventLogData)), event1);
      t = event1.time;
    }
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
          e->transactionEndIndex += p->entriesInUse - 1;
          e->transactionStartIndex = pos;
          break;
        }
      }
      last = t;
    }
  }
  return ret;
}

int readProfileGeneric(
  dlmsSettings* settings,
  gxProfileGeneric* pg,
  unsigned char type,
  gxValueEventArg* e)
{
  unsigned char first = e->transactionEndIndex == 0;
  int ret = 0;
  gxArray captureObjects;
  gxTarget CAPTURE_OBJECT[10] = { 0 };
  ARR_ATTACH(captureObjects, CAPTURE_OBJECT, 0);

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
      if ((ret = getProfileGenericDataByRangeFromRingBuffer(settings, type, e)) != 0 ||
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
    gxLoadProfileData lp;
    gxEventLogData event1;
    for (pos = e->transactionStartIndex - 1; pos != e->transactionEndIndex; ++pos)
    {
      pduSize = e->value.byteArr->size;
      //Load data from EEPROM.
      if (type == 0)
      {
        EEPROM.get(1024 + (pos * sizeof(gxLoadProfileData)), lp);
        time_initUnix(&tm, lp.time);
        if ((ret = cosem_setStructure(e->value.byteArr, 2)) != 0 ||
            (ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &tm)) != 0 ||
            (ret = cosem_setUInt16(e->value.byteArr, lp.activePowerL1)) != 0)
        {
          //Error is handled later.
        }
      }
      else
      {
        EEPROM.get(2048 + (pos * sizeof(gxEventLogData)), event1);
        time_initUnix(&tm, event1.time);
        if ((ret = cosem_setStructure(e->value.byteArr, 2)) != 0 ||
            (ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &tm)) != 0 ||
            (ret = cosem_setUInt16(e->value.byteArr, event1.event)) != 0)
        {
          //Error is handled later.
        }
      }
      if (ret != 0)
      {
        //Don't set error if PDU is full,
        if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
        {
          e->value.byteArr->size = pduSize;
          ret = 0;
        }
        else
        {
          break;
        }
        break;
      }
      ++e->transactionStartIndex;
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
    GXTRACE_LN(PSTR("svr_preRead: "), e->target->objectType, e->target->logicalName);
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
    if (e->target == &activePowerL1.base && e->index == 2)
    {
      readActivePowerValue();
    }
    //Get time if user want to read date and time.
    if (e->target == BASE(meterData.clock1) && e->index == 2)
    {
#ifndef USE_TIME_INTERRUPT
      gxtime dt;
      time_now(&dt, 1);
      e->error = cosem_setDateTimeAsOctetString(e->value.byteArr, &dt);
      e->value.vt = DLMS_DATA_TYPE_DATETIME;
      e->handled = 1;
#endif //USE_TIME_INTERRUPT
    }
    else if (e->target == BASE(profileGeneric) && e->index == 2)
    {
      e->error = (DLMS_ERROR_CODE)readProfileGeneric(settings, &profileGeneric, 0, e);
    }
    else if (e->target == BASE(eventLog) && e->index == 2)
    {
      e->error = (DLMS_ERROR_CODE)readProfileGeneric(settings, &eventLog, 1, e);
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
  gxValueEventArg* e;
  int ret, pos;
  for (pos = 0; pos != args->size; ++pos)
  {
    if ((ret = vec_getByIndex(args, pos, &e)) != 0)
    {
      return;
    }
    //Set new base time if user wants to set date and time.
    if (e->target == BASE(meterData.clock1) && e->index == 2)
    {
      updateState(GURUX_EVENT_CODES_TIME_CHANGE);
#ifndef USE_TIME_INTERRUPT
      started = time_elapsed();
#endif //USE_TIME_INTERRUPT
    }
    GXTRACE_LN(PSTR("svr_preWrite: "), e->target->objectType, e->target->logicalName);
  }
}

int sendPush(
  dlmsSettings* settings,
  gxPushSetup* push)
{
  GXTRACE_LN(PSTR("sendPush"), push->base.objectType, push->base.logicalName);
  int ret, pos;
  message messages;
  if (push->pushObjectList.size == 0)
  {
    GXTRACE(PSTR("sendPush Failed. No objects selected."), NULL);
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
  if ((ret = Server.generatePushSetupMessages(push, &messages)) == 0)
  {
    for (pos = 0; pos != messages.size; ++pos)
    {
      Serial.write(bb.data, bb.size);
      Serial.flush();
    }
  }
  if (ret != 0)
  {
    GXTRACE(PSTR("generatePushSetupMessages Failed."), NULL);
  }
  mes_clear(&messages);
  return ret;
}

void handleLoadProfileActions(
  gxValueEventArg* it)
{
  if (it->index == 1)
  {
    // Profile generic clear is called. Clear data.
    meterData.loadProfile.entriesInUse = meterData.loadProfile.rowIndex = profileGeneric.entriesInUse = 0;
  }
  else if (it->index == 2)
  {
    gxLoadProfileData row;
    // Profile generic Capture is called. Save data to the buffer.
    //We are using ring buffer here.
    row.time = time_current();
    row.activePowerL1 = readActivePowerValue();
    EEPROM.put(1024 + meterData.loadProfile.rowIndex * sizeof(gxLoadProfileData), row);
    //Update how many entries is used until buffer is full.
    if (profileGeneric.entriesInUse != profileGeneric.profileEntries)
    {
      ++profileGeneric.entriesInUse;
      meterData.loadProfile.entriesInUse = profileGeneric.entriesInUse;
    }
    //Update row index where next row is added.
    ++meterData.loadProfile.rowIndex;
    meterData.loadProfile.rowIndex = meterData.loadProfile.rowIndex % profileGeneric.profileEntries;
    save(&meterData.loadProfile, sizeof(GXProfileGeneric));
  }
}

void handleEventLogActions(
  gxValueEventArg* it)
{
  if (it->index == 1)
  {
    // Profile generic clear is called. Clear data.
    meterData.eventLog.entriesInUse = meterData.eventLog.rowIndex = 0;
    eventLog.entriesInUse = 0;
  }
  else if (it->index == 2)
  {
    captureEventLog(readEventCode());
  }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
  dlmsSettings * settings,
  gxValueEventCollection * args)
{
  gxValueEventArg* e;
  int ret, pos;
  for (pos = 0; pos != args->size; ++pos)
  {
    if ((ret = vec_getByIndex(args, pos, &e)) != 0)
    {
      return;
    }
    GXTRACE_LN(PSTR("svr_preAction: "), e->target->objectType, e->target->logicalName);
    if (e->target == BASE(profileGeneric))
    {
      handleLoadProfileActions(e);
      e->handled = 1;
    }
    else if (e->target == BASE(eventLog))
    {
      handleEventLogActions(e);
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
      EEPROM.put(0, (uint16_t) 0xFFFF);
      //Load objects again.
      createObjects();
      //Uncomment this if you want to clear all data from the EEPROM.
      //It's taking a long time.
      /*
        for (int i = 0; i < EEPROM.length(); i++)
        {
        EEPROM.write(i, 0xFF);
        }
      */
      updateState(GURUX_EVENT_CODES_GLOBAL_METER_RESET);
      e->handled = 1;
    }
    else if (e->target == BASE(disconnectControl))
    {
      updateState(GURUX_EVENT_CODES_OUTPUT_RELAY_STATE);
      //Disconnect. Turn led OFF.
      if (e->index == 1)
      {
        //If there is not buildin LED.
#ifndef ARDUINO_ARCH_ESP32
        digitalWrite(LED_BUILTIN, HIGH);
#endif //ARDUINO_ARCH_ESP32
      }
      else //Reconnnect. Turn LED ON.
      {
        //If there is not buildin LED.
#ifndef ARDUINO_ARCH_ESP32
        digitalWrite(LED_BUILTIN, HIGH);
#endif //ARDUINO_ARCH_ESP32
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
  }
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postRead(
  dlmsSettings * settings,
  gxValueEventCollection * args)
{
  gxValueEventArg* e;
  int ret, pos;
  for (pos = 0; pos != args->size; ++pos)
  {
    if ((ret = vec_getByIndex(args, pos, &e)) != 0)
    {
      return;
    }
    GXTRACE_LN(PSTR("svr_postRead: "), e->target->objectType, e->target->logicalName);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Only updated value is saved. This is done because write to EEPROM is slow
// and there is a limit how many times value can be written to the EEPROM.
/////////////////////////////////////////////////////////////////////////////
void svr_postWrite(
  dlmsSettings * settings,
  gxValueEventCollection * args)
{
  gxValueEventArg* e;
  int ret, pos;
  for (pos = 0; pos != args->size; ++pos)
  {
    if ((ret = vec_getByIndex(args, pos, &e)) != 0)
    {
      return;
    }
    GXTRACE_LN(PSTR("svr_postWrite: "), e->target->objectType, e->target->logicalName);
    if (e->target == BASE(meterData.clock1))
    {
      save(&meterData.clock1, sizeof(meterData.clock1));
    }
    else if (e->target == BASE(actionScheduleDisconnectOpen))
    {
      meterData.disconnectOpenExecutions.count = actionScheduleDisconnectOpen.executionTime.size;
      save(&meterData.disconnectOpenExecutions, sizeof(meterData.disconnectOpenExecutions));
    }
    else if (e->target == BASE(actionScheduleDisconnectClose))
    {
      meterData.disconnectCloseExecutions.count = actionScheduleDisconnectClose.executionTime.size;
      save(&meterData.disconnectCloseExecutions, sizeof(meterData.disconnectCloseExecutions));
    }
    else if (e->target == BASE(meterData.hdlc))
    {
      save(&meterData.hdlc, sizeof(meterData.hdlc));
      hdlcChanged = 1;
    }
    else if (e->target == BASE(profileGeneric))
    {
      meterData.loadProfile.period = profileGeneric.capturePeriod;
      save(&meterData.loadProfile, sizeof(meterData.loadProfile));
    }
    else if (e->target == BASE(pushSetup))
    {
      meterData.push.communicationWindow.count = pushSetup.communicationWindow.size;
      saveTargets(&meterData.push.objects, PUSH_OBJECTS, pushSetup.pushObjectList.size);
      save(&meterData.push, sizeof(meterData.push));
    }
    else if (e->target == BASE(associationLow))
    {
      //Save LLS password.
      meterData.association.llsPasswordSize = associationLow.secret.size;
      save(&meterData.association, sizeof(GXAssociation));
      GXTRACE(PSTR("Low level password: "), (char*) associationLow.secret.data);
    }
  }
  //Reset execute time to update execute time if user add new execute times or changes the time.
  executeTime = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postAction(
  dlmsSettings * settings,
  gxValueEventCollection * args)
{
  gxValueEventArg* e;
  int ret, pos;
  for (pos = 0; pos != args->size; ++pos)
  {
    if ((ret = vec_getByIndex(args, pos, &e)) != 0)
    {
      return;
    }
    GXTRACE_LN(PSTR("svr_postAction: "), e->target->objectType, e->target->logicalName);
    if (e->target == BASE(associationHigh) && e->index == 2)
    {
      //Save HLS passwords.
      meterData.association.hlsPasswordSize = associationHigh.secret.size;
      save(&meterData.association, sizeof(GXAssociation));
      GXTRACE(PSTR("High level password: "), (char*)associationHigh.secret.data);
    }
    else if (e->target == BASE(associationHighGMac) && e->index == 2)
    {
      //Save HLS passwords.
      meterData.association.hlsPasswordSize = associationHighGMac.secret.size;
      save(&meterData.association, sizeof(GXAssociation));
      GXTRACE(PSTR("High level password: "), (char*)associationHighGMac.secret.data);
    }
  }
}

unsigned char svr_isTarget(
  dlmsSettings * settings,
  uint32_t serverAddress,
  uint32_t clientAddress)
{
  GXTRACE(PSTR("svr_isTarget."), NULL);
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
              GXTRACE(PSTR("Connecting without authentication."), NULL);
              break;
            case DLMS_AUTHENTICATION_LOW:
              //Client connects using low authentication.
              GXTRACE(PSTR("Connecting using Low authentication."), NULL);
              break;
            default:
              //Client connects using High authentication.
              GXTRACE(PSTR("Connecting using High authentication."), NULL);
              break;
          }
          break;
        }
      }
    }
  }
  if (ret == 0)
  {
    GXTRACE_INT(PSTR("Invalid authentication level."), clientAddress);
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
    if (ret == 0)
    {
      GXTRACE_INT(PSTR("Invalid server address"), serverAddress);
    }
  }
  return ret;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
  dlmsServerSettings * settings,
  DLMS_AUTHENTICATION authentication,
  gxByteBuffer * password)
{
  GXTRACE(PSTR("svr_validateAuthentication"), NULL);
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
      GXTRACE(PSTR("Invalid low level password."), (char*) associationLow.secret.data);
      return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
    }
    GXTRACE(PSTR("Valid low level password."), (char*)password->data);
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
      case 4://capturePeriod
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

/**
  Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
  dlmsSettings * settings,
  gxObject * obj,
  unsigned char index)
{
  //GXTRACE("svr_getAttributeAccess", NULL);
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

  // Only read is allowed for Low authentication.
  if (settings->authentication == DLMS_AUTHENTICATION_LOW)
  {
    return DLMS_ACCESS_MODE_READ;
  }
  // All writes are allowed for High authentication.
  return DLMS_ACCESS_MODE_READ_WRITE;
}

/**
  Get method access level.
*/
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
  dlmsSettings * settings,
  gxObject * obj,
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
  dlmsServerSettings * settings)
{
  if (settings->base.connected == DLMS_CONNECTION_STATE_NONE)
  {
    GXTRACE(PSTR("svr_connected to HDLC level."), NULL);
  }
  else if ((settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
  {
    GXTRACE(PSTR("svr_connected DLMS level."), NULL);
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Client has try to made invalid connection. Password is incorrect.
/////////////////////////////////////////////////////////////////////////////
int svr_invalidConnection(dlmsServerSettings * settings)
{
  GXTRACE(PSTR("svr_invalidConnection"), NULL);
  updateState(GURUX_EVENT_CODES_WRONG_PASSWORD);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Client has close the connection.
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
  dlmsServerSettings * settings)
{
  GXTRACE(PSTR("svr_disconnected"), NULL);
  if (hdlcChanged)
  {
    hdlcChanged = 0;
    //Communication speed is trace always because this is causing most of the problems.
    GXTRACE_INT(PSTR("Communication speed"), getCommunicationSpeed());
    //Start to use new serial port settings.
    Serial.begin(getCommunicationSpeed());
    while (!Serial) {
      //  ; // wait for serial port to connect. Needed for native USB port only
    }
  }
  return 0;
}

void svr_preGet(
  dlmsSettings * settings,
  gxValueEventCollection * args)
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
  dlmsSettings * settings,
  gxValueEventCollection * args)
{

}

//Print detailed information to the serial port.
void svr_trace(const char* str, const char* data)
{
  GXTRACE(str, data);
}
