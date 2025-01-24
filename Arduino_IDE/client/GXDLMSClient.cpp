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

void GXDLMSClient::SetAuthentication(DLMS_AUTHENTICATION value)
{
  settings.authentication = value;
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

#ifndef DLMS_IGNORE_HDLC
int GXDLMSClient::SnrmRequest(  message* messages)
{
  return cl_snrmRequest(&settings, messages);
}

int GXDLMSClient::ParseUAResponse(gxByteBuffer* data)
{
  return cl_parseUAResponse(&settings, data);
}

#endif //DLMS_IGNORE_HDLC
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

uint16_t GXDLMSClient::GetServerAddress(uint16_t logicalAddress, uint16_t physicalAddress, unsigned char addressSize)
{
  return cl_getServerAddress(logicalAddress, physicalAddress, addressSize);
}

#ifndef DLMS_IGNORE_HIGH_GMAC

int GXDLMSClient::SetSystemTitle(const gxByteBuffer* systemTitle)
{
  if (systemTitle->size != 8)
  {
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
  }
  bb_empty(&settings.cipher.systemTitle);
  return bb_set(&settings.cipher.systemTitle, systemTitle->data, 8);
}

int GXDLMSClient::GetSystemTitle(gxByteBuffer* systemTitle)
{
  return bb_set(systemTitle, settings.cipher.systemTitle.data, settings.cipher.systemTitle.size);
}

int GXDLMSClient::SetBlockCipherKey(const gxByteBuffer* blockCipherKey)
{
  if (blockCipherKey->size != 16)
  {
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
  }
  bb_empty(&settings.cipher.blockCipherKey);
  return bb_set(&settings.cipher.blockCipherKey, blockCipherKey->data, blockCipherKey->size);
}

int GXDLMSClient::GetBlockCipherKey(gxByteBuffer* blockCipherKey)
{
  return bb_set(blockCipherKey, settings.cipher.blockCipherKey.data, settings.cipher.blockCipherKey.size);
}

bool GXDLMSClient::UseLogicalNameReferencing()
{
  return settings.useLogicalNameReferencing;
}

int GXDLMSClient::SetAuthenticationKey(const gxByteBuffer* authenticationKey)
{
  if (authenticationKey->size != 16)
  {
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
  }
  bb_clear(&settings.cipher.authenticationKey);
  return bb_set(&settings.cipher.authenticationKey, authenticationKey->data, authenticationKey->size);
}

int GXDLMSClient::GetAuthenticationKey(gxByteBuffer* authenticationKey)
{
  return bb_set(authenticationKey, settings.cipher.authenticationKey.data, settings.cipher.authenticationKey.size);
}

uint32_t GXDLMSClient::GetInvocationCounter()
{
  return settings.cipher.invocationCounter;
}

void GXDLMSClient::SetInvocationCounter(uint32_t value)
{
  settings.cipher.invocationCounter = value;
}

DLMS_SECURITY GXDLMSClient::GetSecurity()
{
  return settings.cipher.security;
}

int GXDLMSClient::SetSecurity(DLMS_SECURITY value)
{
  settings.cipher.security = value;
  return 0;
}
#endif //DLMS_IGNORE_HIGH_GMAC

gxByteBuffer* GXDLMSClient::GetPassword()
{
  settings.password.position = 0;
  return &settings.password;
}

uint16_t GXDLMSClient::GetClientAddress()
{
  return settings.clientAddress;
}


void GXDLMSClient::SetClientAddress(uint16_t value)
{
  settings.clientAddress = value;
}

uint32_t GXDLMSClient::GetServerAddress()
{
  return settings.serverAddress;
}

DLMS_INTERFACE_TYPE GXDLMSClient::GetInterfaceType()
{
  return settings.interfaceType;
}

#ifndef DLMS_IGNORE_SERIALIZER
int GXDLMSClient::LoadObjects(gxSerializerSettings* serializeSettings, gxObject** objects, uint16_t count)
{
  return ser_loadObjects(&settings, serializeSettings, objects, count);
}

int GXDLMSClient::SaveObjects(gxSerializerSettings* serializeSettings, gxObject** objects, uint16_t count)
{
  return ser_saveObjects(serializeSettings, objects, count);
}

int GXDLMSClient::SaveObjects2(gxSerializerSettings* serializeSettings)
{
  return ser_saveObjects2(serializeSettings, GetObjects());
}
#endif //DLMS_IGNORE_SERIALIZER 

int GXDLMSClient::ParseObjectCount(gxByteBuffer* data, uint16_t* count)
{
  return cl_parseObjectCount(data, count);
}

int GXDLMSClient::ParseNextObject(gxByteBuffer* data, gxObject* object)
{
  return cl_parseNextObject(&settings, data, object);
}

void GXDLMSClient::GetHDLCSettings(uint16_t& maxInfoTX, uint16_t& maxInfoRX, unsigned char& windowSizeTX, unsigned char& windowSizeRX)
{
  maxInfoTX = settings.maxInfoTX;
  maxInfoRX = settings.maxInfoRX;
  windowSizeTX = settings.windowSizeTX;
  windowSizeRX = settings.windowSizeRX;
}

void GXDLMSClient::SetHDLCSettings(uint16_t maxInfoTX, uint16_t maxInfoRX, unsigned char windowSizeTX, unsigned char windowSizeRX)
{
  settings.maxInfoTX = maxInfoTX;
  settings.maxInfoRX = maxInfoRX;
  settings.windowSizeTX = windowSizeTX;
  settings.windowSizeRX = windowSizeRX;
}

//static GXDLMSClient Client;
