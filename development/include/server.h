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

#include "gxignore.h"
#ifndef DLMS_IGNORE_SERVER

#ifdef  __cplusplus
extern "C" {
#endif

#include "serverevents.h"
#include "dlms.h"

    typedef struct
    {
        /*Received data from the client.*/
        unsigned char* data;
        /*Data size.*/
        uint16_t dataSize;
        /*Server reply for the client.*/
        gxByteBuffer* reply;
        /*Is GBT streaming in progress.*/
        DLMS_DATA_REQUEST_TYPES moreData;
        /*GBT Message count to send.*/
        unsigned char gbtCount;
        /*HDLC window count to send.*/
        unsigned char hdlcWindowCount;
        /*Received command.*/
        DLMS_COMMAND command;
#ifndef DLMS_IGNORE_IEC
        /*Baudrate is changed when optical probe is used.*/
        uint16_t newBaudRate;
#endif //DLMS_IGNORE_IEC
    }gxServerReply;

    int sr_initialize(
        gxServerReply* sr,
        unsigned char* data,
        uint16_t dataSize,
        gxByteBuffer* reply);

    int svr_initialize(
        dlmsServerSettings* settings);

    /*Server handles received bytes from the client.*/
    int svr_handleRequest(
        dlmsServerSettings* settings,
        gxByteBuffer* data,
        gxByteBuffer* reply);

    /*Server handles received bytes from the client.*/
    int svr_handleRequest2(
        dlmsServerSettings* settings,
        unsigned char* buff,
        uint16_t size,
        gxByteBuffer* reply);

    /*Server handles received byte from the client.*/
    int svr_handleRequest3(
        dlmsServerSettings* settings,
        unsigned char data,
        gxByteBuffer* reply);

    /*Server handles received bytes from the client.*/
    int svr_handleRequest4(
        dlmsServerSettings* settings,
        gxServerReply* sr);

    void svr_reset(
        dlmsServerSettings* settings);

    /*
    Run the background processes.
    */
    int svr_run(
        dlmsServerSettings* settings,
        //Current EPOCH time.
        uint32_t time,
        //Next EPOCH execution time.
        uint32_t* next);

#ifndef DLMS_IGNORE_REGISTER_MONITOR
    /*
   The server verifies that value of monitored object don't cross the thresholds.
   Selected script is invoked if value is passed.
   */
    int svr_monitor(dlmsServerSettings* settings, gxRegisterMonitor* object);
    /*
   The server verifies that values of monitored objects don't cross the thresholds.
   Selected script is invoked if value is passed.
   */
    int svr_monitorAll(dlmsServerSettings* settings);
#endif //DLMS_IGNORE_REGISTER_MONITOR

#ifndef DLMS_IGNORE_LIMITER
    /*
   The server verifies that value of monitored object don't cross the thresholds.
   Selected script is invoked if value is passed.
   */
    int svr_limiter(dlmsServerSettings* settings, gxLimiter* object, uint32_t now);
    /*
   The server verifies that values of monitored objects don't cross the thresholds.
   Selected script is invoked if value is passed.
   */
    int svr_limiterAll(dlmsServerSettings* settings, uint32_t now);
#endif //DLMS_IGNORE_LIMITER

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    /**
    * Update short names.
    */
    int svr_updateShortNames(
        dlmsServerSettings* settings,
        unsigned char force);
#endif  //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#if !defined(DLMS_IGNORE_ACTIVITY_CALENDAR) && !defined(DLMS_IGNORE_OBJECT_POINTERS)
    int svr_handleActivityCalendar(
        dlmsServerSettings* settings,
        gxActivityCalendar* object,
        uint32_t time,
        uint32_t* next);
#endif //!defined(DLMS_IGNORE_ACTIVITY_CALENDAR) && !defined(DLMS_IGNORE_OBJECT_POINTERS)

#ifndef DLMS_IGNORE_SERIALIZER
    //Check is client changing the settings with action.
    //This can be used to check is meter data changed.
    //Return value is saved atribute index or zero if nothing hasn't change.
    uint32_t svr_isChangedWithAction(DLMS_OBJECT_TYPE objectType, unsigned char methodIndex);
#endif //DLMS_IGNORE_SERIALIZER

#ifdef DLMS_NOTIFY_CIPHERING_ERROR
    //Meter notifies from the ciphering error.
    extern void svr_cipheringError();
#endif //DLMS_NOTIFY_CIPHERING_ERROR

#ifdef DLMS_NOTIFY_AUTHENTICATION_ERROR
    //Meter notifies from the authentication error.
    extern void svr_authenticationError();
#endif //DLMS_NOTIFY_AUTHENTICATION_ERROR

#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_SERVER
#endif //SERVER_H
