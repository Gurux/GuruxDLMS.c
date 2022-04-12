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
#ifndef DLMS_ARDUINO_SERVER
#define DLMS_ARDUINO_SERVER
#include <Arduino.h>
#include "include/gxignore.h"
#include "include/serverevents.h"
#include "include/gxvalueeventargs.h"
#include "include/dlmssettings.h"
#include "include/gxinvoke.h"
#include "include/dlms.h"
#include "include/gxObjects.h"
#include "include/notify.h"
#include "include/gxset.h"
#include "include/gxget.h"

#include "include/bytebuffer.h"
#include "include/variant.h"
#include "include/objectarray.h"
#include "include/cosem.h"
#include "include/gxserializer.h"

class GXDLMSServer
{
    dlmsServerSettings settings;
    gxValueEventArg events[10];
    //Space for client password.
    unsigned char PASSWORD[20];
    //Space for client challenge.
    unsigned char C2S_CHALLENGE[64];
    //Space for server challenge.
    unsigned char S2C_CHALLENGE[64];
  public:
    //Constructor.
    int init(
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
      unsigned short pduBufferSize);

    //Get authentication level.
    DLMS_AUTHENTICATION GetAuthentication();

    //Get push client address.
    unsigned short GetPushClientAddress();
    //Set push client address.
    void SetPushClientAddress(unsigned short value);

    //Add objects.
    int initObjects(gxObject** objects, unsigned short count);

    //List of COSEM objects that are removed from association view(s).
    //Objects can be used to save meter internal data.
    int internalObjects(gxObject** objects, unsigned short count);

    //Initialize server.
    int initialize();

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    //Set HDLC setup. This is used for inactivity timeout.
    void setHdlc(gxIecHdlcSetup* hdlc);

    // Get internal timeout for HDLC.
    // This defines how long is waited before meter expects that frame is compleated.
    uint16_t getFrameWaitTime();
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    //Set WRAPPER setup. This is used for inactivity timeout.
    void setTcpUdpSetup(gxTcpUdpSetup* wrapper);
#endif //DLMS_IGNORE_TCP_UDP_SETUP

    //Handle received message from the client.
    int handleRequest(unsigned char* data, unsigned short size, gxByteBuffer* reply);

    //Execute invokes.
    //start: current time.
    //executeTime: Next execute time.
    int run(uint32_t start, uint32_t* executeTime);

#ifndef DLMS_IGNORE_PUSH_SETUP
    //Generate push notification messages.
    int generatePushSetupMessages(gxPushSetup* push, message* messages);
#endif //DLMS_IGNORE_PUSH_SETUP

    //Return collection of objects.
    objectArray* getObjects();

    //Set default clock.
    void setDefaultClock(gxClock* clock);

#ifndef DLMS_IGNORE_SERIALIZER
    //Serialize objects to bytebuffer.
    int loadObjects(
      gxSerializerSettings* serializeSettings,
      gxObject** object,
      uint16_t count);

    //Serialize objects to bytebuffer.
    int saveObjects(
      gxSerializerSettings* serializeSettings,
      gxObject** object,
      uint16_t count);
#endif //DLMS_IGNORE_SERIALIZER
    //Check is client changing the settings with action.
    //This can be used to check is meter data changed.
    //Return value is saved atribute index or zero if nothing hasn't change.
    uint32_t isChangedWithAction(DLMS_OBJECT_TYPE objectType, unsigned char methodIndex);

    //Assign block cipher key.
    void assingBlockCipherKey(dlmsVARIANT* value);

    //Assign authentication key.
    void assingAuthenticationKey(dlmsVARIANT* value);

    //Assign KEK.
    void assingKek(dlmsVARIANT* value);

    //Assign aerver Invocation counter.
    void assignServerInvocationCounter(dlmsVARIANT* value);
};

static  GXDLMSServer Server;
#endif //DLMS_ARDUINO_SERVER
