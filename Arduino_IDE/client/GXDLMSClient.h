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
//#define DLMS_IGNORE_NOTIFY
//#define DLMS_IGNORE_SERVER
#ifndef DLMS_ARDUINO_CLIENT
#define DLMS_ARDUINO_CLIENT
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

class GXDLMSClient
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

    //Return collection of objects.
    objectArray* GetObjects();

    DLMS_AUTHENTICATION GetAuthentication();


    //Handle received reply.
    int GetData(gxByteBuffer* reply, gxReplyData* data);

    DLMS_CONFORMANCE GetNegotiatedConformance();


    int ReadRowsByRange(
      gxProfileGeneric* object,
      gxtime* start,
      gxtime* end,
      message* messages);


    int UpdateValue(
      gxObject* object,
      unsigned char attributeOrdinal,
      dlmsVARIANT* value);


    int ReadRowsByEntry(
      gxProfileGeneric* object,
      uint32_t index,
      uint32_t count,
      message* messages);

    int ReadList(
      gxArray* list,
      message* messages);

    /**
        Update list of values.

        @param list
                   read objects.
        @param data
                   Received reply from the meter.
    */
    int UpdateValues(
      gxArray* list,
      gxByteBuffer* data);

    int ReceiverReady(
      DLMS_DATA_REQUEST_TYPES type,
      gxByteBuffer* message);

    /*Read association view.*/
    int GetObjectsRequest(
      message* messages);

    /*Parse association view.*/
    int ParseObjects(
      gxByteBuffer* data);

    int Read(
      gxObject* object,
      unsigned char index,
      message* messages);

    /**
          Generate Method (Action) request.

          @param object
                     Method object.
          @param index
                     Method index.
          @param data
                     Method data.
          @param type
                     Data type.
          Returns DLMS action message(s).
    */
    int Method(
      gxObject* object,
      unsigned char index,
      dlmsVARIANT* data,
      message* messages);

    int Write(
      gxObject* object,
      unsigned char index,
      message* messages);

    int GetApplicationAssociationRequest(
      message* messages);

    int ParseApplicationAssociationResponse(
      gxByteBuffer* reply);

    int SnrmRequest(
      message* messages);

    int ParseUAResponse(
      gxByteBuffer* data);

    int AarqRequest(
      message* messages);

    int ParseAAREResponse(
      gxByteBuffer* data);

    /////////////////////////////////////////////////////////////////////////
    // Generates a release request.
    // returns Release request, as byte array.
    int ReleaseRequest(
      bool useProtectedRelease,
      message* packets);

    int DisconnectRequest(message* messages);

    /////////////////////////////////////////////////////////////////////////
    //Release dynamically allocated objects.
    void ReleaseObjects();

    /////////////////////////////////////////////////////////////////////////
    // Convert physical address and logical address to server address.
    // logicalAddress: Server logical address.
    // physicalAddress: Server physical address.
    // addressSize: Address size in bytes.
    // Returns Server address.
    static uint16_t GetServerAddress(uint16_t logicalAddress, uint16_t physicalAddress, unsigned char addressSize);

    /////////////////////////////////////////////////////////////////////////
    //Set system title.
    int SetSystemTitle(const gxByteBuffer* systemTitle);

    /////////////////////////////////////////////////////////////////////////
    //Get system title.
    int GetSystemTitle(gxByteBuffer* systemTitle);


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

};

static GXDLMSClient Client;

#endif //DLMS_ARDUINO_CLIENT
