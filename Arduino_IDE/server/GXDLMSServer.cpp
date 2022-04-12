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
#include "include/dlms.h"
#include "include/gxObjects.h"
#include "include/dlmssettings.h"
#include "include/cosem.h"
#include "include/server.h"
#include "GXDLMSServer.h"

int GXDLMSServer::init(
  bool useLogicalNameReferencing,
  DLMS_INTERFACE_TYPE interfaceType,
  //Max frame size.
  unsigned short frameSize,
  //Max PDU size.
  unsigned short pduSize,
  //Buffer where received frames are saved.
  unsigned char* frameBuffer,
  //Size of frame buffer.
  unsigned short frameBufferSize,
  //PDU Buffer.
  unsigned char* pduBuffer,
  //Size of PDU buffer.
  unsigned short pduBufferSize)
{
  //Initialize DLMS settings.
  svr_init(&settings, useLogicalNameReferencing, interfaceType, frameSize, pduSize, frameBuffer, frameBufferSize, pduBuffer, pduBufferSize);
#ifdef DLMS_IGNORE_MALLOC
  //Allocate space for read list.
  vec_attach(&settings.transaction.targets, events, 0, sizeof(events) / sizeof(events[0]));
#endif //DLMS_IGNORE_MALLOC
  //Allocate space for client password.
  BB_ATTACH(settings.base.password, PASSWORD, 0);
  //Allocate space for client challenge.
  BB_ATTACH(settings.base.ctoSChallenge, C2S_CHALLENGE, 0);
  //Allocate space for server challenge.
  BB_ATTACH(settings.base.stoCChallenge, S2C_CHALLENGE, 0);
  //Set master key (KEK) to 1111111111111111.
  unsigned char KEK[16] = { 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31 };
#ifdef DLMS_IGNORE_MALLOC
  memcpy(settings.base.kek, KEK, sizeof(KEK));
  settings.base.serializedPdu = &settings.info.data;
#else
  bb_set(&settings.base.kek, KEK, sizeof(KEK));
#endif //DLMS_IGNORE_MALLOC
  return 0;
}

DLMS_AUTHENTICATION GXDLMSServer::GetAuthentication()
{
  return settings.base.authentication;
}

unsigned short GXDLMSServer::GetPushClientAddress()
{
  return settings.pushClientAddress;
}

void GXDLMSServer::SetPushClientAddress(unsigned short value)
{
  settings.pushClientAddress = value;
}

int GXDLMSServer::initObjects(gxObject** objects, unsigned short count)
{
  int ret;
  if (count == 0)
  {
    ret = DLMS_ERROR_CODE_INVALID_INVOKE_ID;
  }
  else
  {
    oa_attach(&settings.base.objects, objects, count);
    ret = oa_verify(&settings.base.objects);
  }
  return ret;
}

int GXDLMSServer::internalObjects(gxObject** objects, unsigned short count)
{
  oa_attach(&settings.base.internalObjects, objects, count);
  return 0;
}
    
//Initialize server.
int GXDLMSServer::initialize()
{
  return svr_initialize(&settings);
}

int GXDLMSServer::handleRequest(unsigned char* data, unsigned short size, gxByteBuffer* reply)
{
  return svr_handleRequest2(&settings, data, size, reply);
}

int GXDLMSServer::run(uint32_t start, uint32_t* executeTime)
{
  return svr_run(&settings, start, executeTime);
}

#ifndef DLMS_IGNORE_PUSH_SETUP
int GXDLMSServer::generatePushSetupMessages(gxPushSetup* push, message* messages)
{
  return notify_generatePushSetupMessages(&settings.base, 0, push, messages);
}
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
//Set HDLC setup. This is used for inactivity timeout.
void GXDLMSServer::setHdlc(gxIecHdlcSetup* hdlc)
{
  settings.hdlc = hdlc;
}

uint16_t GXDLMSServer::GXDLMSServer::getFrameWaitTime()
{
  if (settings.hdlc == NULL)
  {
    return 0;
  }
  return settings.hdlc->interCharachterTimeout;
}

#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
//Set WRAPPER setup. This is used for inactivity timeout.
void GXDLMSServer::setTcpUdpSetup(gxTcpUdpSetup* wrapper)
{
  settings.wrapper = wrapper;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP

//Return collection of objects.
objectArray* GXDLMSServer::getObjects()
{
  return &settings.base.objects;
}

void GXDLMSServer::setDefaultClock(gxClock* clock)
{
  settings.defaultClock = clock;
}

#ifndef DLMS_IGNORE_SERIALIZER

int GXDLMSServer::loadObjects(
  gxSerializerSettings* serializeSettings,
  gxObject** object,
  uint16_t count)
{
  return ser_loadObjects(&settings.base, serializeSettings, object, count);
}

int GXDLMSServer::saveObjects(
  gxSerializerSettings* serializeSettings,
  gxObject** object,
  uint16_t count)
{
  return ser_saveObjects(serializeSettings, object, count);
}

#endif //DLMS_IGNORE_SERIALIZER

//Check is client changing the settings with action.
//This can be used to check is meter data changed.
uint32_t GXDLMSServer::isChangedWithAction(DLMS_OBJECT_TYPE objectType, unsigned char methodIndex)
{
  return svr_isChangedWithAction(objectType, methodIndex);
}

void GXDLMSServer::assingBlockCipherKey(dlmsVARIANT* value)
{
  GX_OCTET_STRING(*value, settings.base.cipher.blockCipherKey, 16);
}

void GXDLMSServer::assingAuthenticationKey(dlmsVARIANT* value)
{
  GX_OCTET_STRING(*value, settings.base.cipher.authenticationKey, 16);
}

void GXDLMSServer::assingKek(dlmsVARIANT* value)
{
  GX_OCTET_STRING(*value, settings.base.kek, 16);
}

void GXDLMSServer::assignServerInvocationCounter(dlmsVARIANT* value)
{
  GX_UINT32_BYREF(*value, settings.base.cipher.invocationCounter);
}

//static GXDLMSServer Server;
