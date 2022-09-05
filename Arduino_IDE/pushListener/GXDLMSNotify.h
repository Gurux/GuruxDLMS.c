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
#ifndef DLMS_ARDUINO_NOTIFY
#define DLMS_ARDUINO_NOTIFY
#include <Arduino.h>
#include "include/gxignore.h"
#include "include/serverevents.h"
#include "include/dlmssettings.h"
#include "include/gxvalueeventargs.h"
#include "include/gxObjects.h"
#include "include/bytebuffer.h"
#include "include/variant.h"
#include "include/objectarray.h"
#include "include/cosem.h"
//obj_toString requires this
#include "include/converters.h"

class GXDLMSNotify
{
    dlmsSettings settings;
  public:
    //Constructor.
    int init(
      bool useLogicalNameReferencing,
      int clientAddress,
      int serverAddress,
      DLMS_AUTHENTICATION authentication,
      const char* password,
      DLMS_INTERFACE_TYPE interfaceType);

    //Handle received reply.
    int GetData(gxByteBuffer* reply, gxReplyData* data);


    int GetPushValues(
      gxPushSetup* pushSetup,
      variantArray* data,
      gxArray* items);

      #ifndef DLMS_IGNORE_HIGH_GMAC
    /////////////////////////////////////////////////////////////////////////
    //Get invocation counter.
    uint32_t GetInvocationCounter();

    /////////////////////////////////////////////////////////////////////////
    //Set invocation counter.
    void SetInvocationCounter(uint32_t value);

    /////////////////////////////////////////////////////////////////////////
    //Get security.
    DLMS_SECURITY GetSecurity();

    /////////////////////////////////////////////////////////////////////////
    //Set security level.
    int SetSecurity(DLMS_SECURITY value);

    /////////////////////////////////////////////////////////////////////////
    //Set block cipher key.
    int SetBlockCipherKey(const gxByteBuffer* blockCipherKey);

    /////////////////////////////////////////////////////////////////////////
    //Get block cipher key.
    int GetBlockCipherKey(gxByteBuffer* blockCipherKey);

    /////////////////////////////////////////////////////////////////////////
    //Get authentication key.
    int SetAuthenticationKey(const gxByteBuffer* authenticationKey);

    /////////////////////////////////////////////////////////////////////////
    //Get authentication key.
    int GetAuthenticationKey(gxByteBuffer* authenticationKey);

    /////////////////////////////////////////////////////////////////////////
    //Set system title.
    int SetSystemTitle(const gxByteBuffer* systemTitle);

    /////////////////////////////////////////////////////////////////////////
    //Get system title.
    int GetSystemTitle(gxByteBuffer* systemTitle);
#endif //DLMS_IGNORE_HIGH_GMAC
};

static GXDLMSNotify Notify;

#endif //DLMS_ARDUINO_NOTIFY
