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
#include "include/client.h"
#include "GXDLMSClient.h"
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


unsigned char svr_isTarget(
  dlmsSettings * /*settings*/,
  uint32_t /*serverAddress*/,
  uint32_t /*clientAddress*/)
{
  return 0;
}


/////////////////////////////////////////////////////////////////////////////
//Client has made connection to the server.
/////////////////////////////////////////////////////////////////////////////
int svr_connected(
  dlmsServerSettings * /*settings*/)
{
  return 0;
}

int GXDLMSClient::init(
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

//Return collection of objects.
objectArray* GXDLMSClient::GetObjects()
{
  return &settings.objects;
}

DLMS_AUTHENTICATION GXDLMSClient::GetAuthentication()
{
  return settings.authentication;
}

int GXDLMSClient::GetData(gxByteBuffer* reply, gxReplyData* data)
{
  return dlms_getData2(&settings, reply, data, 0);
}

DLMS_CONFORMANCE GXDLMSClient::GetNegotiatedConformance()
{
  return settings.negotiatedConformance;
}

int GXDLMSClient::ReadRowsByRange(gxProfileGeneric* object, gxtime* start, gxtime* end, message* messages)
{
  return cl_readRowsByRange2(&settings, object, start, end, messages);
}

int GXDLMSClient::UpdateValue(gxObject* object, unsigned char attributeOrdinal, dlmsVARIANT* value)
{
  return  cl_updateValue(&settings, object, attributeOrdinal, value);
}

int GXDLMSClient::ReadRowsByEntry(gxProfileGeneric* object, uint32_t index, uint32_t count, message* messages)
{
  return cl_readRowsByEntry(&settings, object, index, count, messages);
}

int GXDLMSClient::ReadList(gxArray* list, message* messages)
{
  return cl_readList(&settings, list, messages);
}

int GXDLMSClient::UpdateValues(gxArray* list, gxByteBuffer* data)
{
  return cl_updateValues(&settings, list, data);
}

int GXDLMSClient::ReceiverReady(DLMS_DATA_REQUEST_TYPES type, gxByteBuffer* message)
{
  return cl_receiverReady(&settings, type, message);
}

int GXDLMSClient::Method(gxObject* object, unsigned char index, dlmsVARIANT* data, message* messages)
{
  return cl_method(&settings, object, index, data, messages);
}

int GXDLMSClient::Write(gxObject* object, unsigned char index, message* messages)
{
  return cl_write(&settings, object, index, messages);
}

int GXDLMSClient::Read(gxObject* object, unsigned char index, message* messages)
{
  return cl_read(&settings, object, index, messages);
}

int GXDLMSClient::GetObjectsRequest(message* messages)
{
  return cl_getObjectsRequest(&settings, messages);
}

int GXDLMSClient::ParseObjects(gxByteBuffer* data)
{
  return cl_parseObjects(&settings, data);
}

int GXDLMSClient::GetApplicationAssociationRequest(message* messages)
{
  return cl_getApplicationAssociationRequest(&settings, messages);
}

int GXDLMSClient::ParseApplicationAssociationResponse(gxByteBuffer* reply)
{
  return cl_parseApplicationAssociationResponse(&settings, reply);
}

int GXDLMSClient::SnrmRequest(  message* messages)
{
  return cl_snrmRequest(&settings, messages);
}

int GXDLMSClient::ParseUAResponse(gxByteBuffer* data)
{
  return cl_parseUAResponse(&settings, data);
}

int GXDLMSClient::AarqRequest(message* messages)
{
  return cl_aarqRequest(&settings, messages);
}

int GXDLMSClient::ParseAAREResponse(gxByteBuffer* data)
{
  return cl_parseAAREResponse(&settings, data);
}

int GXDLMSClient::ReleaseRequest(bool useProtectedRelease, message* packets)
{
  return cl_releaseRequest2(&settings, packets, useProtectedRelease);
}

int GXDLMSClient::DisconnectRequest(message* messages)
{
  return cl_disconnectRequest(&settings, messages);
}

void GXDLMSClient::ReleaseObjects()
{
  oa_clear(&settings.releasedObjects, 1);
}

//static GXDLMSClient Client;
