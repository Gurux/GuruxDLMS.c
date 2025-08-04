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

#ifndef SERVER_EVENTS_H
#define SERVER_EVENTS_H

#include "gxignore.h"
#if !defined(DLMS_IGNORE_SERVER) || (defined(DLMS_DEBUG) || defined(DLMS_NOTIFY_AUTHENTICATION_ERROR))
#include "dlmssettings.h"
#include "bytebuffer.h"

#ifdef  __cplusplus
extern "C" {
#endif
#ifndef DLMS_IGNORE_SERVER
    /**
  * Check is data sent to this server.
  *
  * @param serverAddress
  *            Server address.
  * @param clientAddress
  *            Client address.
  * @return True, if data is sent to this server.
  */
    extern unsigned char svr_isTarget(
        dlmsSettings* settings,
        uint32_t serverAddress,
        uint32_t clientAddress);

    /**
    * Get attribute access level.
    */
    extern DLMS_ACCESS_MODE svr_getAttributeAccess(
        dlmsSettings* settings,
        gxObject* obj,
        unsigned char index);

    /**
    * Get method access level.
    */
    extern DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
        dlmsSettings* settings,
        gxObject* obj,
        unsigned char index);

#ifdef DLMS_USE_ACCESS_SELECTOR
    /**
    * Get access selectors.
    */
    extern int svr_getAccessSelector(
        dlmsSettings* settings,
        gxObject* obj,
        unsigned char index,
        gxByteBuffer* data);
#endif //DLMS_USE_ACCESS_SELECTOR

    /**
    * called when client makes connection to the server.
    */
    extern int svr_connected(
        dlmsServerSettings* settings);

    /**
        * Client has try to made invalid connection. Password is incorrect.
        *
        * @param connectionInfo
        *            Connection information.
        */
    extern int svr_invalidConnection(
        dlmsServerSettings* settings);

    /**
    * called when client clses connection to the server.
    */
    extern int svr_disconnected(
        dlmsServerSettings* settings);

    extern void svr_preGet(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    extern void svr_postGet(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    /**
        * Read selected item(s).
        *
        * @param args
        *            Handled read requests.
        */
    extern void svr_preRead(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    /**
        * Write selected item(s).
        *
        * @param args
        *            Handled write requests.
        */
    extern void svr_preWrite(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    /**
         * Action is occurred.
         *
         * @param args
         *            Handled action requests.
         */
    extern void svr_preAction(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    /**
    * Read selected item(s).
    *
    * @param args
    *            Handled read requests.
    */
    extern void svr_postRead(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    /**
    * Write selected item(s).
    *
    * @param args
    *            Handled write requests.
    */
    extern void svr_postWrite(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    /**
    * Action is occurred.
    *
    * @param args
    *            Handled action requests.
    */
    extern void svr_postAction(
        dlmsSettings* settings,
        gxValueEventCollection* args);

    /**
        * Check whether the authentication and password are correct.
        *
        * @param authentication
        *            Authentication level.
        * @param password
        *            Password.
        * @return Source diagnostic.
        */
    extern DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
        dlmsServerSettings* settings,
        DLMS_AUTHENTICATION authentication,
        gxByteBuffer* password);

    /**
         * Find object.
         *
         * @param objectType
         *            Object type.
         * @param sn
         *            Short Name. In Logical name referencing this is not used.
         * @param ln
         *            Logical Name. In Short Name referencing this is not used.
         * @return Found object or NULL if object is not found.
         */
    extern int svr_findObject(
        dlmsSettings* settings,
        DLMS_OBJECT_TYPE objectType,
        int sn,
        unsigned char* ln,
        gxValueEventArg* e);

    /**
    * This is reserved for future use.
    *
    * @param args
    *            Handled data type requests.
    */
    extern void svr_getDataType(
        dlmsSettings* settings,
        gxValueEventCollection* args);
#endif //DLMS_IGNORE_SERVER
#ifdef DLMS_DEBUG
    /**
    * Trace that can be used in debugging.
    *
    * str: Method info.
    * Data: optional data.
    */
    extern void svr_trace(
        const char* str,
        const char* data);
#endif //DLMS_DEBUG

    //Server uses notify trace if DLMS_DEBUG is defined.
    void svr_notifyTrace(const char* str, int err);

    //Server uses notify trace if DLMS_DEBUG is defined.
    void svr_notifyTrace2(const char* str, const short ot, const unsigned char* ln, int err);

    //Server uses notify trace if DLMS_DEBUG is defined.
    void svr_notifyTrace3(const char* str, const char* value);

    //Server uses notify trace if DLMS_DEBUG is defined.
    void svr_notifyTrace4(const char* str, gxByteBuffer* value);

    //Server uses notify trace if DLMS_DEBUG is defined.
    void svr_notifyTrace5(const char* str, const unsigned char* value, uint16_t length);

#ifdef DLMS_INVOCATION_COUNTER_VALIDATOR
    //Some DLMS sub standard define that invocation counter must between given range.
    //Returns zero if the invocation counter is accepted.
    extern unsigned char svr_validateInvocationCounter(
        dlmsSettings* settings,
        uint32_t value);

    /* Code example:
    unsigned char svr_validateInvocationCounter(dlmsSettings* settings, uint32_t value)
    {
        if (value < *settings->expectedInvocationCounter - 3 &&
            value > *settings->expectedInvocationCounter + 3) {
            // If the IC value is out of the expected range
            return 1;
        }
        *settings->expectedInvocationCounter = value + 1;
        return 0;
    }
    */
#endif //DLMS_INVOCATION_COUNTER_VALIDATOR

#ifdef DLMS_NOTIFY_AUTHENTICATION_ERROR
    //Meter notifies from the authentication error.
    extern void svr_authenticationError();
#endif //DLMS_NOTIFY_AUTHENTICATION_ERROR

#ifdef DLMS_WRITE_MULTIPLE_DATABLOCKS
    /*The client is writing a value that does not fit into a single PDU, so the meter
    must temporarily store it in flash or RAM before updating the corresponding COSEM object.
    DLMS_ERROR_CODE_FALSE is returned if value is already updated.
    svr_preWrite and svr_postWrite are not called when custom write is used.
    */
    extern int svr_storePartialPDU(
        dlmsSettings* settings,
        gxObject* obj,
        unsigned char index,
        gxByteBuffer* value);

    /*Partical PDU is removed when the first PDU is received.*/
    extern int svr_clearPartialPDU(
        dlmsSettings* settings,
        gxObject* obj,
        unsigned char index);

    /*Server asks the complete data after all PDUs are received. This value is updated.
    DLMS_ERROR_CODE_FALSE is returned if value is already updated.
    */
    extern int svr_getCompletePDU(
        dlmsSettings* settings,
        gxObject* obj,
        unsigned char index,
        gxByteBuffer* value);
#endif //DLMS_WRITE_MULTIPLE_DATABLOCKS

#ifdef  __cplusplus
}
#endif
#endif //!defined(DLMS_IGNORE_SERVER) && (defined(DLMS_DEBUG) || defined(DLMS_NOTIFY_AUTHENTICATION_ERROR))
#endif //SERVER_EVENTS_H
