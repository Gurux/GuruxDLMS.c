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

#ifndef CLIENT_H
#define CLIENT_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#if !(defined(DLMS_IGNORE_CLIENT) || defined(DLMS_IGNORE_MALLOC))
#ifdef DLMS_DEBUG
#include "serverevents.h"
#endif //DLMS_DEBUG

#include "dlms.h"

    int cl_getData(
        dlmsSettings* settings,
        gxByteBuffer* reply,
        gxReplyData* data);

    int cl_getData2(
        dlmsSettings* settings,
        gxByteBuffer* reply,
        gxReplyData* data,
        gxReplyData* notify,
        unsigned char* isNotify);

// Get size of the frame.
// size: Size of received bytes on the frame.
// Returns: Zero if succeeded, or rccurred error code.
    int cl_getFrameSize(dlmsSettings* settings, gxByteBuffer* data, uint32_t* size);

#ifndef DLMS_IGNORE_HDLC
    int cl_snrmRequest(
        dlmsSettings* settings,
        message* messages);

    int cl_parseUAResponse(
        dlmsSettings* settings,
        gxByteBuffer* data);

    // Returns the number of bytes to read before the frame is complete.
    // When WRAPPER is used this method can be used to check how many bytes we need to read.
    // data: Received data.
    // size: Size of received bytes on the frame.
    // Returns: Zero if succeeded, or rccurred error code.
    int cl_getRemainingFrameSize(dlmsSettings* settings, gxByteBuffer* data, uint32_t* size);
#endif //DLMS_IGNORE_HDLC

    int cl_aarqRequest(
        dlmsSettings* settings,
        message* messages);

    int cl_parseAAREResponse(
        dlmsSettings* settings,
        gxByteBuffer* data);

    int cl_getApplicationAssociationRequest(
        dlmsSettings* settings,
        message* messages);

    int cl_parseApplicationAssociationResponse(
        dlmsSettings* settings,
        gxByteBuffer* reply);

    /*Read association view. Association view is not available if malloc is not used.*/
    int cl_getObjectsRequest(
        dlmsSettings* settings,
        message* messages);

#ifndef DLMS_IGNORE_MALLOC
    /*Parse association view. Association view is not available if malloc is not used.*/
    int cl_parseObjects(
        dlmsSettings* settings,
        gxByteBuffer* data);
#endif //DLMS_IGNORE_MALLOC

    //Get objects count in association view.
    int cl_parseObjectCount(
        gxByteBuffer* data,
        uint16_t* count);

    /*Parse next association view object.
    This method can be used when malloc is not used or there is a limited amount of the memory in use.*/
    int cl_parseNextObject(
        dlmsSettings* settings,
        gxByteBuffer* data,
        gxObject* object);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    int cl_readSN(
        dlmsSettings* settings,
        uint16_t address,
        unsigned char attributeOrdinal,
        gxByteBuffer* data,
        message* messages);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    int cl_readLN(
        dlmsSettings* settings,
        const unsigned char* name,
        DLMS_OBJECT_TYPE interfaceClass,
        unsigned char attributeOrdinal,
        gxByteBuffer* data,
        message* messages);

    /*This method is used to read list of objects.*/
    int cl_readList(
        dlmsSettings* settings,
        gxArray* list,
        message* messages);

    int cl_read(
        dlmsSettings* settings,
        gxObject* object,
        unsigned char attributeOrdinal,
        message* messages);

    int cl_getKeepAlive(
        dlmsSettings* settings,
        message* messages);

#ifndef DLMS_IGNORE_PROFILE_GENERIC
    int cl_readRowsByEntry(
        dlmsSettings* settings,
        gxProfileGeneric* object,
        uint32_t index,
        uint32_t count,
        message* messages);

    int cl_readRowsByEntry2(
        dlmsSettings* settings,
        gxProfileGeneric* object,
        uint32_t index,
        uint32_t count,
        uint16_t colStart,
        uint16_t colEnd,
        message* messages);


#ifdef DLMS_USE_EPOCH_TIME
    int cl_readRowsByRange(
        dlmsSettings* settings,
        gxProfileGeneric* object,
        uint32_t start,
        uint32_t end,
        message* messages);
#else
    int cl_readRowsByRange(
        dlmsSettings* settings,
        gxProfileGeneric* object,
        struct tm* start,
        struct tm* end,
        message* messages);
#endif //DLMS_USE_EPOCH_TIME

    /////////////////////////////////////////////////////////////////////////
    //Read profile generic using start and end times.
    int cl_readRowsByRange2(
        dlmsSettings* settings,
        gxProfileGeneric* object,
        gxtime* start,
        gxtime* end,
        message* messages);

    /////////////////////////////////////////////////////////////////////////
    //Read profile generic using Indonesia standard.
#ifdef DLMS_INDONESIA_STANDARD
    int cl_readRowsByRange3(
        dlmsSettings* settings,
        gxProfileGeneric* object,
        gxtime* start,
        gxtime* end,
        unsigned char startRegister,
        unsigned char numberOfRegisters,
        message* messages)
#endif //DLMS_INDONESIA_STANDARD

#endif //DLMS_IGNORE_PROFILE_GENERIC

    /*This method is used to write object.*/
    int cl_write(
        dlmsSettings* settings,
        gxObject* object,
        unsigned char index,
        message* messages);

    /*This method is used to write list of objects.*/
    int cl_writeList(
        dlmsSettings* settings,
        gxArray* list,
        message* messages);

    int cl_writeLN(
        dlmsSettings* settings,
        const unsigned char* name,
        DLMS_OBJECT_TYPE interfaceClass,
        unsigned char index,
        dlmsVARIANT* data,
        unsigned char byteArray,
        message* messages);

    int cl_writeSN(
        dlmsSettings* settings,
        uint16_t address,
        int index,
        dlmsVARIANT* data,
        message* messages);

    int cl_changeType(
        gxByteBuffer* value,
        DLMS_DATA_TYPE type,
        dlmsVARIANT* newValue);

    int cl_updateValue(
        dlmsSettings* settings,
        gxObject* object,
        unsigned char attributeOrdinal,
        dlmsVARIANT* value);

    /**
         * Update list of values.
         *
         * @param list
         *            read objects.
         * @param data
         *            Received reply from the meter.
         */
    int cl_updateValues(
        dlmsSettings* settings,
        gxArray* list,
        gxByteBuffer* data);

    int cl_receiverReady(
        dlmsSettings* settings,
        DLMS_DATA_REQUEST_TYPES type,
        gxByteBuffer* message);


    /**
    * Generates a release request.
    *
    * @return Release request, as byte array.
    */
    int cl_releaseRequest(
        dlmsSettings* settings,
        message* packets);

    /**
* Generates a release request.
*
* @return Release request, as byte array.
*/
    int cl_releaseRequest2(
        dlmsSettings* settings,
        message* packets,
        unsigned char useProtectedRelease);


    int cl_disconnectRequest(
        dlmsSettings* settings,
        message* messages);

    /**
        * Generate Method (Action) request.
        *
        * @param object
        *            Method object.
        * @param index
        *            Method index.
        * @param data
        *            Method data.
        * @param messages DLMS action messages.
        */
    int cl_method(
        dlmsSettings* settings,
        gxObject* object,
        unsigned char index,
        dlmsVARIANT* data,
        message* messages);

    /**
    * Generate Method (Action) request.
    *
    * @param object
    *            Method object.
    * @param index
    *            Method index.
    * @param data
    *            Method data.
    * @param bytearray
    *            Is data bytearray.
    * @param messages DLMS action messages.
    */
    int cl_method2(
        dlmsSettings* settings,
        gxObject* object,
        unsigned char index,
        unsigned char* value,
        uint32_t length,
        message* messages);

    /**
        * Generate Method (Action) request..
        *
        * @param name
        *            Method object short name or Logical Name.
        * @param objectType
        *            Object type.
        * @param index
        *            Method index.
        * @param value
        *            Method data.
        * @param dataType
        *            Data type.
        * @return DLMS action message.
        */
    int cl_methodLN(
        dlmsSettings* settings,
        const unsigned char* name,
        DLMS_OBJECT_TYPE objectType,
        unsigned char index,
        dlmsVARIANT* data,
        message* messages);

    /**
        * Generate Method (Action) request..
        *
        * @param name
        *            Method object short name or Logical Name.
        * @param objectType
        *            Object type.
        * @param index
        *            Method index.
        * @param value
        *            byte array.
        * @param length
        *            Length of byte array.
        * @return DLMS action message.
        */
    int cl_methodLN2(
        dlmsSettings* settings,
        unsigned char* name,
        DLMS_OBJECT_TYPE objectType,
        unsigned char index,
        unsigned char* value,
        uint32_t length,
        message* messages);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    /**
        * Generate Method (Action) request.
        *
        * @param name
        *            Method object short name or Logical Name.
        * @param objectType
        *            Object type.
        * @param index
        *            Method index.
        * @param value
        *            Method data.
        * @param dataType
        *            Data type.
        * @return DLMS action message.
        */
    int cl_methodSN(
        dlmsSettings* settings,
        uint16_t address,
        DLMS_OBJECT_TYPE objectType,
        int index,
        dlmsVARIANT* data,
        message* messages);


    /**
        * Generate Method (Action) request.
        *
        * @param name
        *            Method object short name or Logical Name.
        * @param objectType
        *            Object type.
        * @param index
        *            Method index.
        * @param value
        *            byte array.
        * @param lenght
        *            Length of the byte array.
        * @return DLMS action message.
        */
    int cl_methodSN2(
        dlmsSettings* settings,
        uint16_t address,
        DLMS_OBJECT_TYPE objectType,
        int index,
        unsigned char* value,
        uint32_t length,
        message* messages);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    /////////////////////////////////////////////////////////////////////////
    // Convert physical address and logical address to server address.
    // logicalAddress: Server logical address.
    // physicalAddress: Server physical address.
    // addressSize: Address size in bytes.
    // Returns Server address.
    uint16_t cl_getServerAddress(
        uint16_t logicalAddress,
        uint16_t physicalAddress,
        unsigned char addressSize);
#ifdef  __cplusplus
}
#endif
#endif //!(defined(DLMS_IGNORE_CLIENT) || defined(DLMS_IGNORE_MALLOC))
#endif //CLIENT_H
