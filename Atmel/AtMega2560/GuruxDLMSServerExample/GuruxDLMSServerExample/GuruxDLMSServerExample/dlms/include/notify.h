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

#ifndef NOTIFY_H
#define NOTIFY_H
#include "gxignore.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include "dlms.h"
#include "gxget.h"
#include "gxkey.h"

/**
     * Removes the HDLC frame from the packet, and returns COSEM data only.
     *
     * @param reply
     *            The received data from the device.
     * @param data
     *            Information from the received data.
     * @return Is frame complete.
     */
int notify_getData(
    dlmsSettings* settings,
    gxByteBuffer* reply,
    gxReplyData *data);

/**
 * Add value of COSEM object to byte buffer. AddData method can be used with
 * GetDataNotificationMessage -method. DLMS specification do not specify the
 * structure of Data-Notification body. So each manufacture can sent
 * different data.
 *
 * @param obj
 *            COSEM object.
 * @param index
 *            Attribute index.
 * @param buff
 *            Byte buffer.
 */
int notify_addData(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index,
    gxByteBuffer* buff);

/**
 * Generates data notification message.
 *
 * @param date
 *            Date time. Set to null or Date(0) if not used
 * @param data
 *            Notification body.
 * @return Generated data notification message(s).
 */
int notify_generateDataNotificationMessages2(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t time,
#else
    struct tm* time,
#endif //DLMS_USE_EPOCH_TIME
    gxByteBuffer* data,
    message* messages);

/**
 * Generates data notification message.
 *
 * @param date
 *            Date time. Set To Min or Max if not added.
 * @param objects
 *            List of objects and attribute indexes to notify.
 * @return Generated data notification message(s).
 */
int notify_generateDataNotificationMessages(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t date,
#else
    struct tm* date,
#endif //DLMS_USE_EPOCH_TIME
    gxArray* objects,
    message* messages);

#ifndef DLMS_IGNORE_PUSH_SETUP
/**
 * Generates push setup message.
 *
 * @param date
 *            Date time. Set to null or Date(0) if not used.
 * @param push
 *            Target Push object.
 * @return Generated data notification message(s).
 */
int notify_generatePushSetupMessages(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t date,
#else
    struct tm* date,
#endif //DLMS_USE_EPOCH_TIME
    gxPushSetup* push,
    message* messages);

/**
   * Returns collection of push objects. If this method is used Push object
   * must be set for first object on push object list.
   *
   * @param data
   *            Received value.
   * @return Array of objects and called indexes.
   */
int notify_parsePush(
    dlmsSettings* settings,
    variantArray* data,
    gxArray* items);

#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
int notify_getPushValues(
    dlmsSettings* settings,
    gxPushSetup* pushSetup,
    variantArray* data,
    gxArray* items);
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)

//Sends Event Notification Request.
int notify_generateEventNotificationMessages2(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t time,
#else
    struct tm* time,
#endif //DLMS_USE_EPOCH_TIME
    gxListItem* item,
    gxByteBuffer* data,
    gxByteBuffer* pdu,
    message* messages);

//Sends Event Notification Request.
int notify_generateEventNotificationMessages(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t time,
#else
    struct tm* time,
#endif //DLMS_USE_EPOCH_TIME
    gxListItem* item,
    variantArray* data,
    gxByteBuffer* pdu,
    message* messages);

#endif //DLMS_IGNORE_PUSH_SETUP

#ifdef  __cplusplus
}
#endif
#endif //NOTIFY_H
