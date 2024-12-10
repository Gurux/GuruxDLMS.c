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
#include "include/gxobjects.h"
#include "include/bytebuffer.h"
#include "include/variant.h"
#include "include/objectarray.h"
#include "include/cosem.h"
#include "include/helpers.h"
//obj_toString requires this
#include "include/converters.h"
#ifndef DLMS_IGNORE_SERIALIZER
#include "include/gxserializer.h"
#endif //DLMS_IGNORE_SERIALIZER

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

    void SetAuthentication(DLMS_AUTHENTICATION value);

    //Get password.
    gxByteBuffer* GetPassword();

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


#ifndef DLMS_IGNORE_HDLC
    int SnrmRequest(
      message* messages);

    int ParseUAResponse(
      gxByteBuffer* data);

#endif //DLMS_IGNORE_HDLC
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
    //Get client address.
    uint16_t GetClientAddress();

    //Set client address.
    void SetClientAddress(uint16_t value);

    //Get server address.
    uint32_t GetServerAddress();

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

#ifndef DLMS_IGNORE_SERIALIZER
    /////////////////////////////////////////////////////////////////////////
    //Serialize objects from EEPROM.
    int LoadObjects(gxSerializerSettings* serializeSettings, gxObject** objects, uint16_t count);

    /////////////////////////////////////////////////////////////////////////
    //Serialize objects to EEPROM.
    int SaveObjects(gxSerializerSettings* serializeSettings, gxObject** objects, uint16_t count);

    /////////////////////////////////////////////////////////////////////////
    //Serialize objects to EEPROM.
    int SaveObjects2(gxSerializerSettings* serializeSettings);
#endif //DLMS_IGNORE_SERIALIZER 


    /////////////////////////////////////////////////////////////////////////
    //Get objects count in association view.
    int ParseObjectCount(gxByteBuffer* data, uint16_t* count);

    /////////////////////////////////////////////////////////////////////////
    //Serialize objects to EEPROM.
    //Parse next association view object.
    // This method can be used when malloc is not used or
    // there is a limited amount of the memory in use.
    int ParseNextObject(
      gxByteBuffer* data,
      gxObject* object);

    /////////////////////////////////////////////////////////////////////////
    //Is Logical Name referencing used.
    bool UseLogicalNameReferencing();

    /////////////////////////////////////////////////////////////////////////
    //Get HDLC settings.
    void GetHDLCSettings(uint16_t& maxInfoTX, uint16_t& maxInfoRX, unsigned char& windowSizeTX, unsigned char& windowSizeRX);

    /////////////////////////////////////////////////////////////////////////
    //Set HDLC settings.
    void SetHDLCSettings(uint16_t maxInfoTX, uint16_t maxInfoRX, unsigned char windowSizeTX, unsigned char windowSizeRX);

    /////////////////////////////////////////////////////////////////////////
    //Returns used interface type.
    DLMS_INTERFACE_TYPE GetInterfaceType();

};

static GXDLMSClient Client;

#endif //DLMS_ARDUINO_CLIENT
