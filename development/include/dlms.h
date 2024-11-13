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

#ifndef DLMS_H
#define DLMS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "errorcodes.h"
#include "bytebuffer.h"
#include "message.h"
#include "helpers.h"
#include "dlmssettings.h"
#include "apdu.h"
#include "variant.h"
#include "objectarray.h"
#include "replydata.h"
#include "datainfo.h"
#include "parameters.h"
    //Makes sure that the basic settings are set.
    int dlms_checkInit(
        dlmsSettings* settings);

#ifndef DLMS_IGNORE_HDLC
    //Is HDLC framing used.
    unsigned char dlms_useHdlc(DLMS_INTERFACE_TYPE type);

    /**
    * Get PDU as HDLC frame.
    */
    int dlms_getHdlcFrame(
        dlmsSettings* settings,
        int frame,
        gxByteBuffer* data,
        gxByteBuffer* reply);

#endif //DLMS_IGNORE_HDLC

#ifndef DLMS_IGNORE_PLC
    int dlms_getMacHdlcFrame(
        dlmsSettings* settings,
        unsigned char frame,
        unsigned char creditFields,
        gxByteBuffer* data,
        gxByteBuffer* reply);
#endif //DLMS_IGNORE_HDLC

#ifndef DLMS_IGNORE_WRAPPER
    /**
         * Split DLMS PDU to wrapper frames.
         *
         * @param settings
         *            DLMS settings.
         * @param data
         *            Wrapped data.
         * @return Wrapper frames.
    */
    int dlms_getWrapperFrame(
        dlmsSettings* settings,
        DLMS_COMMAND command,
        gxByteBuffer* data,
        gxByteBuffer* reply);
#endif //DLMS_IGNORE_WRAPPER

#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char dlms_useDedicatedKey(dlmsSettings* settings);
#endif //DLMS_IGNORE_HIGH_GMAC

    //Set data from DLMS Varuant to DLMS byte stream.
    int dlms_setData(
        gxByteBuffer* data,
        DLMS_DATA_TYPE type,
        dlmsVARIANT* value);

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    //Set data from DLMS Varuant to DLMS byte stream.
    int dlms_setData2(
        unsigned char* buff,
        uint32_t length,
        DLMS_DATA_TYPE type,
        dlmsVARIANT* value);
#else
    //Set data from DLMS Varuant to DLMS byte stream.
    int dlms_setData2(
        unsigned char* buff,
        uint16_t length,
        DLMS_DATA_TYPE type,
        dlmsVARIANT* value);
#endif

    int dlms_receiverReady(
        dlmsSettings* settings,
        DLMS_DATA_REQUEST_TYPES type,
        gxByteBuffer* reply);

    /**
    * Get next logical name PDU.
    *
    * @param p
    *            LN parameters.
    * @param reply
    *            Generated message.
    */
    int dlms_getLNPdu(
        gxLNParameters* p,
        gxByteBuffer* reply);

    //Get value from DLMS byte stream.
    int dlms_getData(
        gxByteBuffer* data,
        gxDataInfo* info,
        dlmsVARIANT* value);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    /**
    * Get next short name PDU.
    * @param p SN parameters.
    * @param reply  Generated message.
    */
    int dlms_getSNPdu(
        gxSNParameters* p,
        gxByteBuffer* reply);

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_HDLC
    //Return DLMS_ERROR_CODE_FALSE if LLC bytes are not included.
    int dlms_checkLLCBytes(
        dlmsSettings* settings,
        gxByteBuffer* data);

    int dlms_getHdlcData(
        unsigned char server,
        dlmsSettings* settings,
        gxByteBuffer* reply,
        gxReplyData* data,
        unsigned char* frame,
        unsigned char preEstablished,
        unsigned char first);
#endif //DLMS_IGNORE_HDLC

#ifndef DLMS_IGNORE_WRAPPER
    int dlms_getTcpData(
        dlmsSettings* settings,
        gxByteBuffer* buff,
        gxReplyData* data,
        gxReplyData* notify,
        unsigned char* isNotify);
#endif //DLMS_IGNORE_WRAPPER

    int dlms_changeType2(
        dlmsVARIANT* value,
        DLMS_DATA_TYPE type,
        dlmsVARIANT* newValue);

    int dlms_changeType(
        gxByteBuffer* value,
        DLMS_DATA_TYPE type,
        dlmsVARIANT* newValue);


    int dlms_getPdu(
        dlmsSettings* settings,
        gxReplyData* data,
        unsigned char first);

    int dlms_getData2(
        dlmsSettings* settings,
        gxByteBuffer* reply,
        gxReplyData* data,
        unsigned char first);

    int dlms_getData3(
        dlmsSettings* settings,
        gxByteBuffer* reply,
        gxReplyData* data,
        gxReplyData* notify,
        unsigned char first,
        unsigned char* isNotify);

    /**
         * Get all Logical name messages. Client uses this to generate messages.
         *
         * @param p
         *            LN settings.
         * @param reply
         *            Generated messages.
         * @return    Status code.
         */
    int dlms_getLnMessages(
        gxLNParameters* p,
        message* reply);

#ifndef DLMS_IGNORE_HDLC
    /**
    * Add LLC bytes to generated message.
    *
    * @param settings
    *            DLMS settings.
    * @param data
    *            Data where bytes are added.
    */
    int dlms_addLLCBytes(
        dlmsSettings* settings,
        gxByteBuffer* data);
#endif //DLMS_IGNORE_HDLC

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    /**
    * Get all Short Name messages. Client uses this to generate messages.
    *
    * @param p
    *            DLMS SN parameters.
    * @param reply
    *            Generated messages.
    * @return    Status code.
    */
    int dlms_getSnMessages(
        gxSNParameters* p,
        message* reply);

    int dlms_getActionInfo(
        DLMS_OBJECT_TYPE objectType,
        unsigned char* value,
        unsigned char* count);
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    int dlms_generateChallenge(
        gxByteBuffer* challenge);

    /**
        * Chipher text.
        *
        * @param auth
        *            Authentication level.
        * @param data
        *            Text to chipher.
        * @param secret
        *            Secret.
        * @return Chiphered text.
        */
    int dlms_secure(
        dlmsSettings* settings,
        int32_t ic,
        gxByteBuffer* data,
        gxByteBuffer* secret,
        gxByteBuffer* reply);

    int dlms_parseSnrmUaResponse(
        dlmsSettings* settings,
        gxByteBuffer* data);

    // Add HDLC parameter.
    int dlms_appendHdlcParameter(
        gxByteBuffer* data,
        uint16_t value);

    //Is it possible to add more data to the PDU before it's full.
    int dlms_isPduFull(
        dlmsSettings* settings,
        gxByteBuffer* data,
        uint16_t* size);

    //Thís method returns the amount of availabe bytes in PDU.
    int dlms_pduAvailable(
        dlmsSettings* settings,
        gxByteBuffer* data,
        uint16_t* size);

    int dlms_getMacFrame(
        dlmsSettings* settings,
        unsigned char frame,
        unsigned char creditFields,
        gxByteBuffer* data,
        gxByteBuffer* reply);

    /**
    * This function returns true, if pre-established connection is used.
    */
    unsigned char dlms_usePreEstablishedConnection(dlmsSettings* settings);


#ifdef  __cplusplus
}
#endif

#endif //DLMS_H
