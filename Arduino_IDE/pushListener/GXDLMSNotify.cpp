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
#include <Arduino.h>
#include "include/gxignore.h"
//#include "include/serverevents.h"
#include "include/gxObjects.h"
#include "include/cosem.h"
#include "include/notify.h"
#include "GXDLMSNotify.h"
#include "include/dlms.h"
#include "include/converters.h"


DLMS_ACCESS_MODE svr_getAttributeAccess(
  dlmsSettings * /*settings*/,
  gxObject * /*obj*/,
  unsigned char /*index*/)
{
  //Client don't need this.
  return DLMS_ACCESS_MODE_NONE;
}

DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
  dlmsSettings * /*settings*/,
  gxObject * /*obj*/,
  unsigned char /*index*/)
{
  //Client don't need this.
  return DLMS_METHOD_ACCESS_MODE_NONE;
}

/////////////////////////////////////////////////////////////////////////////
//Client has made connection to the server.
/////////////////////////////////////////////////////////////////////////////
int svr_connected(
  dlmsServerSettings* settings)
{
  //Empty skeleton. This is added because server implementation needs this.
  return 0;
}

unsigned char svr_isTarget(
  dlmsSettings * /*settings*/,
  uint32_t /*serverAddress*/,
  uint32_t /*clientAddress*/)
{
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preRead(
  dlmsSettings* settings,
  gxValueEventCollection* args)
{
  //Empty skeleton. This is added because server implementation needs this.
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postRead(
  dlmsSettings* settings,
  gxValueEventCollection* args)
{
  //Empty skeleton. This is added because server implementation needs this.
}

int GXDLMSNotify::init(
  bool useLogicalNameReferencing,
  int clientAddress,
  int serverAddress,
  DLMS_AUTHENTICATION authentication,
  const char* password,
  DLMS_INTERFACE_TYPE interfaceType)
{
  //Initialize DLMS settings.
  cl_init(&settings, useLogicalNameReferencing, clientAddress, serverAddress, authentication, password, interfaceType);
  return 0;
}

int GXDLMSNotify::GetPushValues(
  gxPushSetup* pushSetup,
  variantArray* data,
  gxArray* items)
{
  return notify_getPushValues(&settings, pushSetup, data, items);
}

int GXDLMSNotify::GetData(gxByteBuffer* reply, gxReplyData* data)
{
  return notify_getData(&settings, reply, data);
}

//static GXDLMSNotify Notify;
