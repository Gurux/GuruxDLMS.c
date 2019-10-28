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

//Define ignored functionality.
#define DLMS_IGNORE_WRAPPER
#define DLMS_IGNORE_CLIENT
#define GX_DLMS_MICROCONTROLLER
#define DLMS_IGNORE_HIGH_SHA256
#define DLMS_IGNORE_HIGH_SHA1
#define DLMS_IGNORE_HIGH_MD5
#define DLMS_IGNORE_AES
#define DLMS_IGNORE_HIGH_GMAC
// #define DLMS_IGNORE_DATA
// #define DLMS_IGNORE_REGISTER
#define DLMS_IGNORE_EXTENDED_REGISTER
#define DLMS_IGNORE_DEMAND_REGISTER
#define DLMS_IGNORE_REGISTER_ACTIVATION
#define DLMS_IGNORE_PROFILE_GENERIC
// #define DLMS_IGNORE_CLOCK
#define DLMS_IGNORE_SCRIPT_TABLE
#define DLMS_IGNORE_SCHEDULE
#define DLMS_IGNORE_SPECIAL_DAYS_TABLE
#define DLMS_IGNORE_ASSOCIATION_SHORT_NAME
// #define DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#define DLMS_IGNORE_SAP_ASSIGNMENT
#define DLMS_IGNORE_IMAGE_TRANSFER
#define DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#define DLMS_IGNORE_ACTIVITY_CALENDAR
#define DLMS_IGNORE_REGISTER_MONITOR
#define DLMS_IGNORE_ACTION_SCHEDULE
#define DLMS_IGNORE_IEC_HDLC_SETUP
#define DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#define DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#define DLMS_IGNORE_UTILITY_TABLES
#define DLMS_IGNORE_MODEM_CONFIGURATION
#define DLMS_IGNORE_AUTO_ANSWER
#define DLMS_IGNORE_AUTO_CONNECT
#define DLMS_IGNORE_TCP_UDP_SETUP
#define DLMS_IGNORE_IP4_SETUP
#define DLMS_IGNORE_MAC_ADDRESS_SETUP
#define DLMS_IGNORE_PPP_SETUP
#define DLMS_IGNORE_GPRS_SETUP
#define DLMS_IGNORE_SMTP_SETUP
#define DLMS_IGNORE_GSM_DIAGNOSTIC
#define DLMS_IGNORE_REGISTER_TABLE
#define DLMS_IGNORE_STATUS_MAPPING
#define DLMS_IGNORE_SECURITY_SETUP
#define DLMS_IGNORE_DISCONNECT_CONTROL
#define DLMS_IGNORE_LIMITER
#define DLMS_IGNORE_MBUS_CLIENT
#define DLMS_IGNORE_PUSH_SETUP
#define DLMS_IGNORE_MESSAGE_HANDLER
#define DLMS_IGNORE_PARAMETER_MONITOR
#define DLMS_IGNORE_WIRELESS_MODE_Q_CHANNEL
#define DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#define DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#define DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#define DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#define DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#define DLMS_IGNORE_DATA_PROTECTION
#define DLMS_IGNORE_ACCOUNT
#define DLMS_IGNORE_CREDIT
#define DLMS_IGNORE_CHARGE
#define DLMS_IGNORE_TOKEN_GATEWAY
#define DLMS_IGNORE_COMPACT_DATA

#define DLMS_IGNORE_MALLOC
//#define DLMS_IGNORE_FLOAT32
//#define DLMS_IGNORE_FLOAT64

//Use EPOCH time. This can be used to improve memory usage.
#define DLMS_USE_EPOCH_TIME

//Use UTC time zone. Read more: https://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSClock
// #define DLMS_USE_UTC_TIME_ZONE


#include "include/dlmssettings.h"
#include "include/variant.h"
#include "include/cosem.h"
#include "include/server.h"

//DLMS settings.
static dlmsServerSettings settings;

#define HDLC_BUFFER_SIZE 128
#define PDU_BUFFER_SIZE 512
static unsigned char pdu[PDU_BUFFER_SIZE];
static unsigned char frame[11 + HDLC_BUFFER_SIZE];
static unsigned char replyFrame[11 + HDLC_BUFFER_SIZE];
gxByteBuffer reply;
gxData ldn;
//Don't use clock as a name. Some compilers are using clock as reserved word.
gxClock clock1;
gxAssociationLogicalName association;
gxRegister activePowerL1;

gxObject* ALL_OBJECTS[] = { BASE(association), BASE(ldn), BASE(clock1), BASE(activePowerL1) };
//Define low level password.
char LLS_PASSWORD[20] = "Gurux";
//Meter serial number.
unsigned long SERIAL_NUMBER = 123456;
//Define Logical Device Name.
char LDN[17] = "Gurux";

gxObject* ALL_OBJECTS2[] = {BASE(association), BASE(ldn), BASE(clock1), BASE(activePowerL1) };

gxtime time1;
///////////////////////////////////////////////////////////////////////
//Returns the approximate processor time in ms.
///////////////////////////////////////////////////////////////////////
long time_elapsed(void)
{
  return (long)millis();
}

///////////////////////////////////////////////////////////////////////
// Returns current time.
// Get current time.
// Because there is no clock, clock object keeps base time and uptime is added to that.
///////////////////////////////////////////////////////////////////////
void time_now(gxtime* value, unsigned char meterTime)
{
    *value = clock1.time;
    time_addSeconds(value, time_elapsed() / 1000);
    //If date time is wanted in meter time.
    if (meterTime)
    {
        clock_utcToMeterTime(&clock1, value);
    }
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociation()
{
    int ret;
    const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
    if ((ret = INIT_OBJECT(association, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln)) == 0)
    {
        //Only Logical Device Name is add to this Association View.
        //Use this if you  need to save heap.
        OA_ATTACH(association.objectList, ALL_OBJECTS);
        association.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
        association.clientSAP = 0x10;
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
void addLogicalDeviceName() {
  sprintf(LDN, "GRX%.13lu", SERIAL_NUMBER);
  const unsigned char ln[6] = { 0, 0, 42, 0, 0, 255 };
  INIT_OBJECT(ldn, DLMS_OBJECT_TYPE_DATA, ln);
  GX_OCTECT_STRING(ldn.value, LDN, sizeof(LDN));
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject()
{
  int ret;
  //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
  const unsigned char ln[6] = { 0, 0, 1, 0, 0, 255 };
  if ((ret = INIT_OBJECT(clock1, DLMS_OBJECT_TYPE_CLOCK, ln)) == 0)
  {
    time_init(&clock1.begin, -1, 9, 1, -1, -1, -1, -1, 0x8000);
        time_init(&clock1.end, -1, 3, 1, -1, -1, -1, -1, 0x8000);
        //Meter is using UTC time zone.
        clock1.timeZone = 0;
        //Deviation is 60 minutes.
        clock1.deviation = 60;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
  const unsigned char ln[6] = { 1, 1, 21, 25, 0, 255 };
  cosem_init2((gxObject*)&activePowerL1.base, DLMS_OBJECT_TYPE_REGISTER, ln);
  GX_UINT16(activePowerL1.value) = 20;
  //10 ^ 3 =  1000
  activePowerL1.scaler = 3;
  activePowerL1.unit = 30;
  return 0;
}

int svr_InitObjects(
  dlmsServerSettings *settings)
{
  addLogicalDeviceName();
  addClockObject();
  addRegisterObject();
  addAssociation();
  OA_ATTACH(settings->base.objects, ALL_OBJECTS);
  return oa_verify(&settings->base.objects);
}

int svr_findObject(
  dlmsSettings* settings,
  DLMS_OBJECT_TYPE objectType,
  int sn,
  unsigned char* ln,
  gxValueEventArg *e)
{
  if (objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
  {
    e->target = &association.base;
  }
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
    //Update value by one every time when user reads register.
    if (e->target == &activePowerL1.base && e->index == 2)
    {
      var_setUInt16(&activePowerL1.value, activePowerL1.value.uiVal + 1);
    }
    //Get time if user want to read date and time.
    if (e->target == &clock1.base && e->index == 2)
    {
       GX_DATETIME(e->value) = &time1;
       time_now(&time1, 1);
       e->handled = true;
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
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preAction(
  dlmsSettings* settings,
  gxValueEventCollection* args)
{
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
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postAction(
  dlmsSettings* settings,
  gxValueEventCollection* args)
{
}

unsigned char svr_isTarget(
  dlmsSettings *settings,
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
  if (authentication == DLMS_AUTHENTICATION_NONE)
  {
    //Uncomment this if authentication is always required.
    //return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
    return DLMS_SOURCE_DIAGNOSTIC_NONE;
  }
  //Uncomment this if only authentication None is supported.
  //return DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
  return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

/**
  Get attribute access level.
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
  Get method access level.
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
  return 0;
}

/**
      Client has try to made invalid connection. Password is incorrect.

      @param connectionInfo
                 Connection information.
*/
int svr_invalidConnection(dlmsServerSettings *settings)
{
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Client has close the connection.
/////////////////////////////////////////////////////////////////////////////
int svr_disconnected(
  dlmsServerSettings *settings)
{
  return 0;
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
  }
}

void svr_postGet(
  dlmsSettings* settings,
  gxValueEventCollection* args)
{

}

/**
  This is reserved for future use. Do not use it.

  @param args
             Handled data type requests.
*/
void svr_getDataType(
  dlmsSettings* settings,
  gxValueEventCollection* args)
{

}

void setup() {
  int ret;
  time_clear(&time1);
  bb_init(&reply);
  //Start server using logical name referencing and HDLC framing.
  svr_init(&settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, frame, HDLC_BUFFER_SIZE, pdu, PDU_BUFFER_SIZE);
  //Add COSEM objects.
  svr_InitObjects(&settings);
  //Start server
  if ((ret = svr_initialize(&settings)) != 0)
  {
    //TODO: Show error.
    return;
  }
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  unsigned char tmp[HDLC_BUFFER_SIZE];
  unsigned short available;
  available = Serial.available();
  if (available > 0) {
    Serial.readBytes(tmp, available);
    if (svr_handleRequest2(&settings, tmp, available, &reply) != 0)
    {
      bb_clear(&reply);
    }
    if (reply.size != 0)
    {
      //Send reply.
      Serial.write(reply.data, reply.size);
      bb_clear(&reply);
    }
  }
}
