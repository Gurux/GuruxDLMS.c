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
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include <zephyr.h>
#include <stdio.h>
#include <uart.h>
#include <string.h>

#include "dlms/include/dlmssettings.h"
#include "dlms/include/variant.h"
#include "dlms/include/cosem.h"
#include "dlms/include/server.h"

/////////////////////////////////////////////////////////////////////////////
//Serial port settings.
#define UART_NAME "UART_0"
static struct device *uart_dev;
static bool frame_rdy;
static int characters;

/////////////////////////////////////////////////////////////////////////////
//DLMS settings.
dlmsServerSettings settings;
//Meter serial number.
unsigned long SERIAL_NUMBER = 123456;

#define HDLC_BUFFER_SIZE 128
#define PDU_BUFFER_SIZE 256
static unsigned char pdu[PDU_BUFFER_SIZE];
static unsigned char frame[HDLC_BUFFER_SIZE];
gxByteBuffer reply;
static gxData ldn;
//Don't use clock as a name. Some compilers are using clock as reserved word.
static gxClock clock1;
static gxAssociationLogicalName association;
static gxRegister activePowerL1;
static long started;

const gxObject* ALL_OBJECTS[] = { &association.base, &ldn.base,  &activePowerL1.base, &clock1.base};

long time_current(void)
{
  //Get current time.
  //Because there is no clock, clock object keeps base time and uptime is added to that.
  return time_toUnixTime(&clock1.time.value) + (_impl_k_uptime_get() - started) / 1000;
}

long time_elapsed(void)
{
  return _impl_k_uptime_get();
}

void time_now(
gxtime* value)
{
  long offset = time_current();
  time_initUnix(value, offset);
}

///////////////////////////////////////////////////////////////////////
//This method adds example Logical Name Association object.
///////////////////////////////////////////////////////////////////////
int addAssociation()
{
  const unsigned char ln[6] = { 0, 0, 40, 0, 1, 255 };
  cosem_init2((gxObject*) &association.base, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
  //Only Logical Device Name is add to this Association View.
  //Use this if you  need to save heap.
  oa_attach(&association.objectList, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));
  bb_addString(&association.secret, "Gurux");
  association.authenticationMechanismName.mechanismId = DLMS_AUTHENTICATION_NONE;
  return 0;
}

///////////////////////////////////////////////////////////////////////
//Add Logical Device Name. 123456 is meter serial number.
///////////////////////////////////////////////////////////////////////
// COSEM Logical Device Name is defined as an octet-string of 16 octets.
// The first three octets uniquely identify the manufacturer of the device and it corresponds
// to the manufacturer's identification in IEC 62056-21.
// The following 13 octets are assigned by the manufacturer.
//The manufacturer is responsible for guaranteeing the uniqueness of these octets.
int addLogicalDeviceName() {
  int ret;
  char buff[17];
  sprintf(buff, "GRX%.13lu", SERIAL_NUMBER);
  const unsigned char ln[6] = { 0,0,42,0,0,255 };
  if ((ret = cosem_init2((gxObject*)&ldn.base, DLMS_OBJECT_TYPE_DATA, ln)) != 0)
  {
      return ret;
  }
  var_addBytes(&ldn.value, (unsigned char*)buff, 16);
  return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example clock object.
///////////////////////////////////////////////////////////////////////
int addClockObject()
{
  int ret;
  //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
  const unsigned char ln[6] = { 0,0,1,0,0,255 };
  if ((ret = cosem_init2(&clock1.base, DLMS_OBJECT_TYPE_CLOCK, ln)) != 0)
  {
    return ret;
  }
  time_init(&clock1.time, 2019, 1, 1, 0, 0, 0, 0, 0);
  time_init(&clock1.begin, -1, 9, 1, -1, -1, -1, -1, -1);
  time_init(&clock1.end, -1, 3, 1, -1, -1, -1, -1, -1);
  return 0;
}

///////////////////////////////////////////////////////////////////////
//This method adds example register object.
///////////////////////////////////////////////////////////////////////
int addRegisterObject()
{
  int ret;
  const unsigned char ln[6] = { 1,1,21,25,0,255 };
  if ((ret = cosem_init2((gxObject*)&activePowerL1, DLMS_OBJECT_TYPE_REGISTER, ln)) != 0)
  {
    return ret;
  }
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
  oa_attach(&settings->base.objects, ALL_OBJECTS, sizeof(ALL_OBJECTS) / sizeof(ALL_OBJECTS[0]));
  return 0;
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
      e->value.dateTime = (gxtime*) malloc(sizeof(gxtime));
      time_now(e->value.dateTime);     
      e->value.vt = DLMS_DATA_TYPE_DATETIME;
      e->handled = true;
     // time_now(&((gxClock*)e->target)->time);
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
  gxValueEventArg *e;
  int pos;
  for (pos = 0; pos != args->size; ++pos)
  {
    if (vec_getByIndex(args, pos, &e) != 0)
    {
      break;
    }  
    //Set new base time if user wants to set date and time.
    if (e->target == &clock1.base && e->index == 2)
    {
      started = _impl_k_uptime_get();
    }
  }
}

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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)//If Windows or Linux
    hlp_getLogicalNameToString(e->target->logicalName, str);
    printf("Action %s:%d\n", str, e->index);
#endif
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
  }
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
  return 0;
}

/**
* Client has try to made invalid connection. Password is incorrect.
*
* @param connectionInfo
*            Connection information.
*/
int svr_invalidConnection(
  dlmsServerSettings *settings)
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

static void isr(struct device *x)
{
  uart_irq_update(x);
  if (uart_irq_rx_ready(x)) {
    char ch;
    int ret = uart_fifo_read(x, &ch, 1);
    if (ret != 0)
    {
      if (svr_handleRequest3(&settings, ch, &reply) != 0)
      {
       bb_clear(&reply);
      }
      if (reply.size != 0)
      {
        //Send reply.
        for (size_t i = 0; i < reply.size; i++) {
          uart_poll_out(uart_dev, reply.data[i]);
        }
        bb_clear(&reply);
      }
    }
  }
}

static int gx_uart_init(char *uart_dev_name)
{
  int err;
  uart_dev = device_get_binding(uart_dev_name);
  if (uart_dev == NULL) {
    printf("Cannot bind %s\n", uart_dev_name);
    return EINVAL;
  }
  err = uart_err_check(uart_dev);
  if (err) {
    printf("UART check failed\n");
    return EINVAL;
  }
  uart_irq_rx_enable(uart_dev);
  uart_irq_callback_set(uart_dev, isr);
  return err;
}

void main(void)
{
  int ret;
  started = _impl_k_uptime_get();
  characters = 0;
  bb_init(&reply);
  //Start server using logical name referencing and HDLC framing.
  svr_init(&settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, frame, HDLC_BUFFER_SIZE, pdu, PDU_BUFFER_SIZE);
  //Add COSEM objects.
  svr_InitObjects(&settings);
  //Start server
  if ((ret = svr_initialize(&settings)) != 0)
  {
    printf("svr_initialize failed.\r\n");
    return;
  }
  printf("Gurux DLMS sample started.\n");    
  ret = gx_uart_init("UART_0");
  if (ret != 0) {
    printf("Gurux DLMS sample failed.\n");    
    return;
  }
  while (true) {
  }
}
