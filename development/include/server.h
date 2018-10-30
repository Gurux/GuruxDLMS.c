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

#ifndef SERVER_H
#define SERVER_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#include "dlms.h"

    int svr_initialize(
        dlmsServerSettings* settings);

    int svr_handleRequest(
        dlmsServerSettings* settings,
        gxByteBuffer* data,
        gxByteBuffer* reply);

    int svr_handleRequest2(
        dlmsServerSettings* settings,
        unsigned char* buff,
        unsigned short size,
        gxByteBuffer* reply);

    //Handle only one received byte.
    int svr_handleRequest3(
        dlmsServerSettings* settings,
        unsigned char data,
        gxByteBuffer* reply);

    void svr_reset(
        dlmsServerSettings* settings);

    /*
    Run the background processes.
    */
    int svr_run(
        dlmsServerSettings* settings);

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
        dlmsSettings *settings, 
        unsigned long serverAddress,
        unsigned long clientAddress);

    /**
    * Get attribute access level.
    */
    extern DLMS_ACCESS_MODE svr_getAttributeAccess(
        dlmsSettings *settings,
        gxObject *obj,
        unsigned char index);

    /**
    * Get method access level.
    */
    extern DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
        dlmsSettings *settings,
        gxObject *obj,
        unsigned char index);

    /**
    * called when client makes connection to the server.
    */
    extern int svr_connected(
        dlmsServerSettings *settings);

    /**
        * Client has try to made invalid connection. Password is incorrect.
        *
        * @param connectionInfo
        *            Connection information.
        */
    extern int svr_invalidConnection(
        dlmsServerSettings *settings);

    /**
    * called when client clses connection to the server.
    */
    extern int svr_disconnected(
        dlmsServerSettings *settings);

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
        gxValueEventArg *e);

    /**
    * This is reserved for future use.
    *
    * @param args
    *            Handled data type requests.
    */
    extern void svr_getDataType(
        dlmsSettings* settings,
        gxValueEventCollection* args);


#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    /**
    * Update short names.
    */
    int svr_updateShortNames(
        dlmsServerSettings* settings,
        unsigned char force);
#endif  //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifdef  __cplusplus
}
#endif

#endif //SERVER_H
