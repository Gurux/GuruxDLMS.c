//
// --------------------------------------------------------------------------
//  Gurux Ltd
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

#include "../include/gxignore.h"
#ifndef DLMS_IGNORE_SERVER

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#if _MSC_VER > 1400
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif
#include "../include/gxmem.h"
#include "../include/apdu.h"
#include "../include/server.h"
#include "../include/cosem.h"
#include "../include/enums.h"
#include "../include/gxset.h"
#include "../include/gxget.h"
#include "../include/gxinvoke.h"

typedef struct
{
    /**
     * Is attribute index or action index
     */
    unsigned char action;

    /**
     * Attribute index.
     */
    unsigned char index;

    /**
     * COSEM object.
     */
    gxObject* item;
} gxSNInfo;

#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
/**
 * Handle read request.
 */
int svr_handleReadRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data);

/**
* Declare a prototype for a svr_handleWriteRequest.
*/
int svr_handleWriteRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data);
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)

#if !defined(DLMS_IGNORE_MALLOC)
//Copy association view.
void svr_copyAssociationView(objectArray* target, objectArray* source)
{
    uint16_t cnt = 0, pos;
    oa_empty(target);
    oa_capacity(target, source->size);
    for (pos = 0; pos != source->size; ++pos)
    {
        if (source->data[pos]->objectType != DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME &&
            source->data[pos]->objectType != DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
        {
            target->data[cnt] = source->data[pos];
            ++cnt;
        }
    }
    target->size = cnt;
}
#endif //!defined(DLMS_IGNORE_MALLOC)

int svr_initialize(
    dlmsServerSettings* settings)
{
    uint16_t pos;
    int ret;
    gxObject* associationObject = NULL, * it;
    settings->initialized = 1;
    if (settings->base.maxPduSize < 64)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    for (pos = 0; pos != settings->base.objects.size; ++pos)
    {
        if ((ret = oa_getByIndex(&settings->base.objects, pos, &it)) != 0)
        {
            return ret;
        }
        if (memcmp(it->logicalName, EMPTY_LN, sizeof(EMPTY_LN)) == 0)
        {
            //Invalid Logical Name.
            return DLMS_ERROR_CODE_INVALID_LOGICAL_NAME;
        }
        if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME
            && !settings->base.useLogicalNameReferencing)
        {
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
            objectArray* list = &((gxAssociationShortName*)it)->objectList;
            if (list->size == 0)
            {
                svr_copyAssociationView(list, &settings->base.objects);
            }
            associationObject = it;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        }
        else if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME
            && settings->base.useLogicalNameReferencing)
        {
#if !defined(DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME) && !defined(DLMS_IGNORE_MALLOC)
            gxAssociationLogicalName* ln = (gxAssociationLogicalName*)it;
            objectArray* list = &ln->objectList;
            if (list->size == 0)
            {
                svr_copyAssociationView(list, &settings->base.objects);
            }
#endif //!defined(DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME) && !defined(DLMS_IGNORE_MALLOC)
            associationObject = it;
        }
    }
    if (associationObject == NULL)
    {
#ifndef DLMS_IGNORE_MALLOC
        objectArray* list;
        if (settings->base.useLogicalNameReferencing)
        {
            gxAssociationLogicalName* ln;
            if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, (gxObject**)&ln)) != 0)
            {
                return ret;
            }
            list = &((gxAssociationLogicalName*)ln)->objectList;
            oa_push(&settings->base.objects, (gxObject*)ln);
            //Add object to released objects list.
            ret = oa_push(&settings->base.releasedObjects, (gxObject*)ln);
            oa_copy(list, &settings->base.objects);
        }
        else
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            gxAssociationShortName* it2;
            if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME, (gxObject**)&it2)) != 0)
            {
                return ret;
            }
            list = &((gxAssociationShortName*)it2)->objectList;
            oa_push(&settings->base.objects, (gxObject*)it2);
            oa_copy(list, &settings->base.objects);
            //Add object to released objects list.
            ret = oa_push(&settings->base.releasedObjects, (gxObject*)it2);
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_MALLOC
    }
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    // Arrange items by Short Name.
    if (!settings->base.useLogicalNameReferencing)
    {
        return svr_updateShortNames(settings, 0);
    }
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    return 0;
}

#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
int svr_updateShortNames(
    dlmsServerSettings* settings,
    unsigned char force)
{
    gxObject* it;
    uint16_t sn = 0xA0;
    uint16_t pos;
    int ret;
    unsigned char offset, count;
    for (pos = 0; pos != settings->base.objects.size; ++pos)
    {
        if ((ret = oa_getByIndex(&settings->base.objects, pos, (gxObject**)&it)) != 0)
        {
            return ret;
        }
        if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME ||
            it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
        {
            continue;
        }
        // Generate Short Name if not given.
        if (force != 0 || it->shortName == 0)
        {
            it->shortName = sn;
            // Add method index addresses.
            if ((ret = dlms_getActionInfo((DLMS_OBJECT_TYPE)it->objectType, &offset, &count)) != 0)
            {
                return ret;
            }
            if (count != 0)
            {
                sn += offset + (8 * count);
            }
            else
            {
                // If there are no methods.
                // Add attribute index addresses.
                sn += (uint16_t)(8 * obj_attributeCount(it));
            }
        }
        else
        {
            sn = (uint16_t)(it->shortName + (8 * obj_attributeCount(it)));
        }
    }
    return 0;
}
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)

void svr_setInitialize(dlmsServerSettings* settings)
{
    settings->base.protocolVersion = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
#ifndef DLMS_IGNORE_MALLOC
    if (settings->base.cipher.dedicatedKey != NULL)
    {
        bb_clear(settings->base.cipher.dedicatedKey);
        gxfree(settings->base.cipher.dedicatedKey);
        settings->base.cipher.dedicatedKey = NULL;
    }
#else
    memset(settings->base.cipher.dedicatedKey, 0, 16);
#endif //DLMS_IGNORE_MALLOC
    memset(settings->base.sourceSystemTitle, 0, sizeof(settings->base.sourceSystemTitle));
#endif //DLMS_IGNORE_HIGH_GMAC
    trans_clear(&settings->transaction);
    settings->base.blockIndex = 1;
    settings->base.connected = DLMS_CONNECTION_STATE_NONE;
    settings->base.authentication = DLMS_AUTHENTICATION_NONE;
    settings->base.isAuthenticationRequired = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
    settings->base.cipher.invocationCounter = 1;
    settings->base.cipher.security = DLMS_SECURITY_NONE;
#endif //DLMS_IGNORE_HIGH_GMAC
    bb_clear(&settings->base.ctoSChallenge);
    bb_clear(&settings->base.stoCChallenge);
}

void svr_reset(
    dlmsServerSettings* settings)
{
    svr_setInitialize(settings);
    resetFrameSequence(&settings->base);
    reply_clear2(&settings->info, 1);
    settings->base.serverAddress = 0;
    settings->base.clientAddress = 0;
    settings->dataReceived = 0;
    settings->frameReceived = 0;
}

/**
    * Parse AARQ request that client send and returns AARE request.
    *
    * @return Reply to the client.
    */
int svr_HandleAarqRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    unsigned char command = 0;
    int ret;
    gxByteBuffer error;
    DLMS_ASSOCIATION_RESULT result;
    unsigned char diagnostic;
    // Reset settings for wrapper.
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        svr_setInitialize(settings);
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    else
    {
        settings->base.cipher.security = DLMS_SECURITY_NONE;
    }
#endif
    //If client is not called SNRM.
#ifndef DLMS_IGNORE_HDLC
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC && (settings->base.connected & DLMS_CONNECTION_STATE_HDLC) == 0)
    {
        return DLMS_ERROR_CODE_REJECTED;
    }
#endif //DLMS_IGNORE_HDLC
    ret = apdu_parsePDU(&settings->base, data, &result, &diagnostic, &command);
#ifdef DLMS_DEBUG
    svr_notifyTrace("parsePDU", ret);
#endif //DLMS_DEBUG
    bb_clear(data);
    bb_init(&error);
    if (ret == 0 && result == DLMS_ASSOCIATION_RESULT_ACCEPTED)
    {
        if (settings->base.dlmsVersionNumber < 6)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Invalid DLMS version number.", DLMS_INITIATE_DLMS_VERSION_TOO_LOW);
#endif //DLMS_DEBUG
            result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            diagnostic = DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
            bb_setUInt8(&error, 0xE);
            bb_setUInt8(&error, DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR);
            bb_setUInt8(&error, DLMS_SERVICE_ERROR_INITIATE);
            bb_setUInt8(&error, DLMS_INITIATE_DLMS_VERSION_TOO_LOW);
            settings->base.dlmsVersionNumber = 6;
        }
        else if (settings->base.maxPduSize < 64)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Max PDU size is too short.", DLMS_INITIATE_PDU_SIZE_TOOSHORT);
#endif //DLMS_DEBUG
            result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            diagnostic = DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
            bb_setUInt8(&error, 0xE);
            bb_setUInt8(&error, DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR);
            bb_setUInt8(&error, DLMS_SERVICE_ERROR_INITIATE);
            bb_setUInt8(&error, DLMS_INITIATE_PDU_SIZE_TOOSHORT);
            settings->base.maxPduSize = 64;
        }
        else if (settings->base.negotiatedConformance == 0)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Invalid negotiated conformance.", DLMS_INITIATE_INCOMPATIBLE_CONFORMANCE);
#endif //DLMS_DEBUG
            result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            diagnostic = DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
            bb_setUInt8(&error, 0xE);
            bb_setUInt8(&error, DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR);
            bb_setUInt8(&error, DLMS_SERVICE_ERROR_INITIATE);
            bb_setUInt8(&error, DLMS_INITIATE_INCOMPATIBLE_CONFORMANCE);
        }
        else if (diagnostic != DLMS_SOURCE_DIAGNOSTIC_NONE)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Connection rejected.", -1);
#endif //DLMS_DEBUG
            result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            diagnostic = DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
        }
        else
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("svr_validateAuthentication.", 0);
#endif //DLMS_DEBUG
            diagnostic = svr_validateAuthentication(
                settings,
                settings->base.authentication,
                &settings->base.password);
            if (diagnostic != DLMS_SOURCE_DIAGNOSTIC_NONE)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace("Connection rejected.", -1);
#endif //DLMS_DEBUG
                svr_invalidConnection(settings);
                result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            }
            else if (settings->base.authentication > DLMS_AUTHENTICATION_LOW)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace("High authentication is used.", 0);
#endif //DLMS_DEBUG
                // If High authentication is used.
                result = DLMS_ASSOCIATION_RESULT_ACCEPTED;
                diagnostic = DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED;
            }
        }
        // Generate AARE packet.
        if (settings->base.authentication > DLMS_AUTHENTICATION_LOW)
        {
            // If High authentication is used.
            if ((ret = dlms_generateChallenge(&settings->base.stoCChallenge)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace("generateChallenge ", ret);
#endif //DLMS_DEBUG
                bb_clear(&error);
                return ret;
            }
            if (settings->base.useLogicalNameReferencing)
            {
                gxAssociationLogicalName* it;
                unsigned char ln[] = { 0, 0, 40, 0, 0, 255 };
                if ((ret = oa_findByLN(&settings->base.objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln, (gxObject**)&it)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace("oa_findByLN ", ret);
#endif //DLMS_DEBUG
                    return ret;
                }
                if (it == NULL)
                {
                    gxValueEventArg e;
                    ve_init(&e);
                    svr_findObject(&settings->base, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, 0, ln, &e);
                    it = (gxAssociationLogicalName*)e.target;
                }
                if (it != NULL)
                {
#ifndef DLMS_IGNORE_HIGH_GMAC
                    if (settings->base.cipher.security == DLMS_SECURITY_NONE)
#endif //DLMS_IGNORE_HIGH_GMAC
                    {
                        it->applicationContextName.contextId = DLMS_APPLICATION_CONTEXT_NAME_LOGICAL_NAME;
                    }
#ifndef DLMS_IGNORE_HIGH_GMAC
                    else
                    {
                        it->applicationContextName.contextId = DLMS_APPLICATION_CONTEXT_NAME_LOGICAL_NAME_WITH_CIPHERING;
                    }
#endif //DLMS_IGNORE_HIGH_GMAC
                    it->authenticationMechanismName.mechanismId = settings->base.authentication;
                    it->associationStatus = DLMS_ASSOCIATION_STATUS_ASSOCIATION_PENDING;
                }
                else
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace("Association logical name not found. ", -1);
#endif //DLMS_DEBUG
                }
            }
        }
        else
        {
            if (settings->base.useLogicalNameReferencing)
            {
                gxAssociationLogicalName* it;
                unsigned char ln[] = { 0, 0, 40, 0, 0, 255 };
                if ((ret = oa_findByLN(&settings->base.objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln, (gxObject**)&it)) != 0)
                {
                    return ret;
                }
                if (it == NULL)
                {
                    gxValueEventArg e;
                    ve_init(&e);
                    svr_findObject(&settings->base, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, 0, ln, &e);
                    it = (gxAssociationLogicalName*)e.target;
                }
                if (it != NULL)
                {
#ifndef DLMS_IGNORE_HIGH_GMAC
                    if (settings->base.cipher.security == DLMS_SECURITY_NONE)
#endif //DLMS_IGNORE_HIGH_GMAC
                    {
                        it->applicationContextName.contextId = DLMS_APPLICATION_CONTEXT_NAME_LOGICAL_NAME;
                    }
#ifndef DLMS_IGNORE_HIGH_GMAC
                    else
                    {
                        it->applicationContextName.contextId = DLMS_APPLICATION_CONTEXT_NAME_LOGICAL_NAME_WITH_CIPHERING;
                    }
#endif //DLMS_IGNORE_HIGH_GMAC
                    it->authenticationMechanismName.mechanismId = settings->base.authentication;
                    it->associationStatus = DLMS_ASSOCIATION_STATUS_ASSOCIATED;
                }
            }
        }
    }
    else if (diagnostic == DLMS_SOURCE_DIAGNOSTIC_NONE)
    {
#ifdef DLMS_DEBUG
        svr_notifyTrace("Permanent rejected.", -1);
#endif //DLMS_DEBUG
        result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
        diagnostic = DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
        bb_setUInt8(&error, 0xE);
        bb_setUInt8(&error, DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR);
        bb_setUInt8(&error, DLMS_SERVICE_ERROR_INITIATE);
        bb_setUInt8(&error, DLMS_INITIATE_OTHER);
    }

#ifndef DLMS_IGNORE_HDLC
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        dlms_addLLCBytes(&settings->base, data);
    }
#endif //DLMS_IGNORE_HDLC
    ret = apdu_generateAARE(&settings->base, data, result, diagnostic, &error, NULL, command);
#ifdef DLMS_DEBUG
    svr_notifyTrace("apdu_generateAARE.", ret);
#endif //DLMS_DEBUG
    bb_clear(&error);
    return ret;
}

/**
 * Parse SNRM Request. If server do not accept client empty byte array is
 * returned.
 *
 * @return Returns returned UA packet.
 */
int svr_handleSnrmRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    int ret;
    unsigned char len;
    uint16_t clientAddress;
    uint32_t serverAddress;
#ifndef DLMS_IGNORE_HIGH_GMAC
    DLMS_SECURITY security;
#endif
    if ((ret = dlms_parseSnrmUaResponse(&settings->base, data)) != 0)
    {
        return ret;
    }
    bb_clear(data);
#ifndef DLMS_IGNORE_HIGH_GMAC
    security = settings->base.cipher.security;
#endif
    serverAddress = settings->base.serverAddress;
    clientAddress = settings->base.clientAddress;
    svr_reset(settings);
#ifndef DLMS_IGNORE_HIGH_GMAC
    settings->base.cipher.security = security;
#endif
    settings->base.serverAddress = serverAddress;
    settings->base.clientAddress = clientAddress;
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    if (settings->hdlc != NULL)
    {
        //If client wants send larger HDLC frames what meter accepts.
        if (settings->hdlc->maximumInfoLengthReceive != 0 && settings->base.maxInfoTX > settings->hdlc->maximumInfoLengthReceive)
        {
            settings->base.maxInfoTX = settings->hdlc->maximumInfoLengthReceive;
        }
        //If client wants receive larger HDLC frames what meter accepts.
        if (settings->hdlc->maximumInfoLengthTransmit != 0 && settings->base.maxInfoRX > settings->hdlc->maximumInfoLengthTransmit)
        {
            settings->base.maxInfoRX = settings->hdlc->maximumInfoLengthTransmit;
        }
        //If client asks higher window size what meter accepts.
        if (settings->hdlc->windowSizeReceive != 0 && settings->base.windowSizeTX > settings->hdlc->windowSizeReceive)
        {
            settings->base.windowSizeTX = settings->hdlc->windowSizeReceive;
        }
        //If client asks higher window size what meter accepts.
        if (settings->hdlc->windowSizeTransmit != 0 && settings->base.windowSizeRX > settings->hdlc->windowSizeTransmit)
        {
            settings->base.windowSizeRX = settings->hdlc->windowSizeTransmit;
        }
    }
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
    bb_setUInt8(data, 0x81); // FromatID
    bb_setUInt8(data, 0x80); // GroupID
    bb_setUInt8(data, 0); // Length
    bb_setUInt8(data, HDLC_INFO_MAX_INFO_TX);
    dlms_appendHdlcParameter(data, settings->base.maxInfoTX);

    bb_setUInt8(data, HDLC_INFO_MAX_INFO_RX);
    dlms_appendHdlcParameter(data, settings->base.maxInfoRX);

    bb_setUInt8(data, HDLC_INFO_WINDOW_SIZE_TX);
    bb_setUInt8(data, 4);
    bb_setUInt32(data, settings->base.windowSizeTX);

    bb_setUInt8(data, HDLC_INFO_WINDOW_SIZE_RX);
    bb_setUInt8(data, 4);
    bb_setUInt32(data, settings->base.windowSizeRX);

    len = (unsigned char)(data->size - 3);
    // Update length.
    bb_setUInt8ByIndex(data, 2, len);
    return 0;
}

/**
 * Generates disconnect request.
 *
 * @return Disconnect request.
 */
int svr_generateDisconnectRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    int ret, len;
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        bb_setUInt8(data, 0x63);
        ret = bb_setUInt8(data, 0x0);
    }
    else
    {
        bb_setUInt8(data, 0x81); // FromatID
        bb_setUInt8(data, 0x80); // GroupID
        bb_setUInt8(data, 0); // Length

        bb_setUInt8(data, HDLC_INFO_MAX_INFO_TX);
        dlms_appendHdlcParameter(data, settings->base.maxInfoTX);

        bb_setUInt8(data, HDLC_INFO_MAX_INFO_RX);
        dlms_appendHdlcParameter(data, settings->base.maxInfoRX);

        bb_setUInt8(data, HDLC_INFO_WINDOW_SIZE_TX);
        bb_setUInt8(data, 4);
        bb_setUInt32(data, settings->base.windowSizeTX);

        bb_setUInt8(data, HDLC_INFO_WINDOW_SIZE_RX);
        bb_setUInt8(data, 4);
        bb_setUInt32(data, settings->base.windowSizeRX);

        len = data->size - 3;
        // Update length.
        ret = bb_setUInt8ByIndex(data, 2, (unsigned char)len);
    }
    return ret;
}

int svr_reportError(
    dlmsServerSettings* settings,
    DLMS_COMMAND command,
    DLMS_ERROR_CODE error,
    gxByteBuffer* reply)
{
    int ret;
    DLMS_COMMAND cmd;
    gxByteBuffer data;
    switch (command)
    {
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    case DLMS_COMMAND_READ_REQUEST:
        cmd = DLMS_COMMAND_READ_RESPONSE;
        break;
    case DLMS_COMMAND_WRITE_REQUEST:
        cmd = DLMS_COMMAND_WRITE_RESPONSE;
        break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    case DLMS_COMMAND_GET_REQUEST:
        cmd = DLMS_COMMAND_GET_RESPONSE;
        break;
    case DLMS_COMMAND_SET_REQUEST:
        cmd = DLMS_COMMAND_SET_RESPONSE;
        break;
    case DLMS_COMMAND_METHOD_REQUEST:
        cmd = DLMS_COMMAND_METHOD_RESPONSE;
        break;
    default:
        if (settings->info.encryptedCommand != 0)
        {
            //Do nothing if ciphered content is invalid.
            return 0;
        }
        else
        {
            // Return HW error and close connection.
            cmd = DLMS_COMMAND_NONE;
        }
        break;
    }
    bb_init(&data);
    if (settings->base.useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, &settings->base, 0, cmd, 1,
            NULL, NULL, error, DLMS_COMMAND_NONE, 0, 0);
        ret = dlms_getLNPdu(&p, &data);
        if (ret != 0)
        {
            bb_clear(&data);
            return ret;
        }
    }
    else
    {
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        gxSNParameters p;
        gxByteBuffer bb;
        bb_init(&bb);
        bb_setUInt8(&bb, error);
        params_initSN(&p, &settings->base, cmd, 1, 1, NULL, &bb, settings->info.encryptedCommand);
        p.lastBlock = 1;
        ret = dlms_getSNPdu(&p, &data);
        bb_clear(&bb);
        if (ret != 0)
        {
            bb_clear(&data);
            return ret;
        }
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    }
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
#ifndef DLMS_IGNORE_HDLC
        ret = dlms_getHdlcFrame(&settings->base, 0, &data, reply);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_HDLC
    }
#ifndef DLMS_IGNORE_WRAPPER
    else if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        ret = dlms_getWrapperFrame(&settings->base, command, &data, reply);
    }
#endif //DLMS_IGNORE_WRAPPER
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_clear(&data);
    return ret;
}

#ifndef DLMS_IGNORE_SET
int svr_handleSetRequest2(
    dlmsServerSettings* settings,
    gxByteBuffer* data,
    unsigned char type,
    gxLNParameters* p)
{
    gxValueEventArg* e;
    gxValueEventCollection list;
    int ret;
    DLMS_OBJECT_TYPE ci;
    unsigned char ch;
    uint16_t tmp;
    unsigned char* ln = NULL;
#ifdef DLMS_IGNORE_MALLOC
    list = settings->transaction.targets;
    e = &list.data[0];
    ve_clear(e);
#else
    e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    vec_init(&list);
    vec_push(&list, e);
#endif //DLMS_IGNORE_MALLOC

    if ((ret = bb_getUInt16(data, &tmp)) != 0)
    {
        vec_clear(&list);
        return ret;
    }
    ci = (DLMS_OBJECT_TYPE)tmp;
    ln = data->data + data->position;
    data->position += 6;
    // Attribute index.
    if ((ret = bb_getUInt8(data, &e->index)) != 0)
    {
        vec_clear(&list);
        return ret;
    }
    // Get Access Selection.
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        vec_clear(&list);
        return ret;
    }
    if (type == DLMS_SET_COMMAND_TYPE_FIRST_DATABLOCK)
    {
        uint32_t blockNumber;
        uint16_t size;
        if ((ret = bb_getUInt8(data, &ch)) != 0)
        {
            vec_clear(&list);
            return ret;
        }
        p->multipleBlocks = ch == 0;
        ret = bb_getUInt32(data, &blockNumber);
        if (ret != 0)
        {
            vec_clear(&list);
            return ret;
        }
        if (blockNumber != settings->base.blockIndex)
        {
            vec_clear(&list);
            return DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
        }
        ++settings->base.blockIndex;
        if (hlp_getObjectCount2(data, &size) != 0)
        {
            vec_clear(&list);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        //If received date is not what expected.
        if (size != data->size - data->position)
        {
            vec_clear(&list);
            return DLMS_ERROR_CODE_BLOCK_UNAVAILABLE;
        }
    }
    if (!p->multipleBlocks)
    {
        gxDataInfo di;
        di_init(&di);
        resetBlockIndex(&settings->base);
        ret = dlms_getData(data, &di, &e->value);
    }
    if (ret == 0)
    {
        if ((ret = oa_findByLN(&settings->base.objects, ci, ln, &e->target)) == 0)
        {
            if (e->target == NULL)
            {
                ret = svr_findObject(&settings->base, ci, 0, ln, e);
            }
#ifndef DLMS_IGNORE_MALLOC
            bb_clear(data);
#endif //DLMS_IGNORE_MALLOC
            if (ret == 0)
            {
                // If target is unknown.
                if (e->target == NULL)
                {
                    // Device reports a undefined object.
                    ret = DLMS_ERROR_CODE_UNAVAILABLE_OBJECT;
                }
                else
                {
                    DLMS_ACCESS_MODE am = svr_getAttributeAccess(&settings->base, e->target, e->index);
                    // If write is denied.
                    if (am != DLMS_ACCESS_MODE_WRITE && am != DLMS_ACCESS_MODE_READ_WRITE)
                    {
                        //Read Write denied.
                        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                    }
                    else
                    {
                        svr_preWrite(&settings->base, &list);
                        if (p->multipleBlocks)
                        {
#ifdef DLMS_IGNORE_MALLOC
                            settings->transaction.targets.size = 1;
#else
                            bb_clear(&settings->transaction.data);
                            vec_clear(&settings->transaction.targets);
                            settings->transaction.targets = list;
                            var_clear(&e->value);
                            vec_init(&list);
                            if (!bb_isAttached(data))
                            {
                                ret = bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
                            }
#endif //DLMS_IGNORE_MALLOC
                            settings->transaction.command = DLMS_COMMAND_SET_REQUEST;
                        }
                        if (ret == 0)
                        {
                            if (e->error != 0)
                            {
                                ret = e->error;
                            }
                            else if (!e->handled && !p->multipleBlocks)
                            {
                                ret = cosem_setValue(&settings->base, e);
                                svr_postWrite(&settings->base, &list);
                                if (e->error != 0)
                                {
                                    ret = e->error;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (type == DLMS_SET_COMMAND_TYPE_NORMAL)
        {
            bb_clear(data);
        }
    }
#ifndef DLMS_IGNORE_MALLOC
    vec_clear(&list);
#endif //DLMS_IGNORE_MALLOC
    return ret;
}
#endif //DLMS_IGNORE_SET

#ifndef DLMS_IGNORE_MALLOC
int svr_hanleSetRequestWithDataBlock(
    dlmsServerSettings* settings,
    gxByteBuffer* data,
    gxLNParameters* p)
{
    int ret;
    uint32_t blockNumber;
    uint16_t size;
    unsigned char ch;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    p->multipleBlocks = ch == 0;
    if ((ret = bb_getUInt32(data, &blockNumber)) == 0)
    {
        if (blockNumber != settings->base.blockIndex)
        {
            ret = DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
        }
        else
        {
            ++settings->base.blockIndex;
            if (hlp_getObjectCount2(data, &size) != 0)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            else if (size != data->size - data->position)
            {
                ret = DLMS_ERROR_CODE_BLOCK_UNAVAILABLE;
            }
            if (ret == 0 && (ret = bb_set2(&settings->transaction.data, data, data->position, data->size - data->position)) == 0)
            {
                // If all data is received.
                if (!p->multipleBlocks)
                {
                    gxDataInfo di;
                    gxValueEventArg* e = settings->transaction.targets.data[0];
                    di_init(&di);
                    if ((ret != dlms_getData(&settings->transaction.data, &di, &e->value)) != 0)
                    {
                        return ret;
                    }
                    svr_preWrite(&settings->base, &settings->transaction.targets);
                    if (!e->handled && !p->multipleBlocks)
                    {
                        if ((ret = cosem_setValue(&settings->base, e)) != 0)
                        {
                            return ret;
                        }
                        svr_postWrite(&settings->base, &settings->transaction.targets);
                    }
                    trans_clear(&settings->transaction);
                    resetBlockIndex(&settings->base);
                }
            }
        }
    }
    p->multipleBlocks = 1;
    return ret;
}
#endif //DLMS_IGNORE_MALLOC

/**
    * Generate confirmed service error.
    *
    * @param service
    *            Confirmed service error.
    * @param type
    *            Service error.
    * @param code
    *            code
    * @return
    */
int svr_generateConfirmedServiceError(
    dlmsServerSettings* settings,
    DLMS_CONFIRMED_SERVICE_ERROR service,
    DLMS_SERVICE_ERROR type,
    unsigned char code,
    gxByteBuffer* data)
{
    bb_clear(data);
#ifndef DLMS_IGNORE_HDLC
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        dlms_addLLCBytes(&settings->base, data);
    }
#endif //DLMS_IGNORE_HDLC
    bb_setUInt8(data, DLMS_COMMAND_CONFIRMED_SERVICE_ERROR);
    bb_setUInt8(data, service);
    bb_setUInt8(data, type);
    bb_setUInt8(data, code);
    return 0;
}

#ifndef DLMS_IGNORE_SET
int svr_handleSetRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    gxLNParameters p;
    unsigned char invokeId, type;
    int ret;
    // Return error if connection is not established.
    if (!settings->info.preEstablished && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) == 0)
    {
        bb_clear(data);
        return svr_generateConfirmedServiceError(
            settings,
            DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR,
            DLMS_SERVICE_ERROR_SERVICE, DLMS_SERVICE_UNSUPPORTED,
            data);
    }
    // Get type.
    if ((ret = bb_getUInt8(data, &type)) != 0)
    {
        return ret;
    }
    // Get invoke ID and priority.
    if ((ret = bb_getUInt8(data, &invokeId)) != 0)
    {
        return ret;
    }
    updateInvokeId(settings, invokeId);
    params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_SET_RESPONSE, type, NULL, data, 0, settings->info.encryptedCommand, 0, 0);
    if (type == DLMS_SET_COMMAND_TYPE_NORMAL || type == DLMS_SET_COMMAND_TYPE_FIRST_DATABLOCK)
    {
        ret = svr_handleSetRequest2(settings, data, type, &p);
    }
    else if (type == DLMS_SET_COMMAND_TYPE_WITH_DATABLOCK)
    {
#ifdef DLMS_IGNORE_MALLOC
        //All data must fit to one PDU at the moment if malloc is not used.
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
#else
        // Set Request With Data Block
        ret = svr_hanleSetRequestWithDataBlock(settings, data, &p);
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        p.requestType = 1;
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    if (ret != 0)
    {
        // Access Error : Device reports Read-Write denied.
        bb_clear(data);
        resetBlockIndex(&settings->base);
        //If error is DLMS error code.
        if (ret > 0 && ret <= DLMS_ERROR_CODE_OTHER_REASON)
        {
            p.status = ret;
        }
        else
        {
            p.status = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        p.requestType = 0;
        p.multipleBlocks = 0;
        p.requestType = 1;
    }
    return dlms_getLNPdu(&p, data);
}

#endif //DLMS_IGNORE_SET

int svr_getRequestNormal(
    dlmsServerSettings* settings,
    gxByteBuffer* data,
    unsigned char invokeId)
{
    gxValueEventCollection* arr;
    uint16_t ci;
    gxValueEventArg* e;
    unsigned char index;
    int ret;
    unsigned char* ln;
    DLMS_ERROR_CODE status = DLMS_ERROR_CODE_OK;
    resetBlockIndex(&settings->base);
    // CI
    if ((ret = bb_getUInt16(data, &ci)) != 0)
    {
        return ret;
    }
    ln = data->data + data->position;
    data->position += 6;
    // Attribute Id
    if ((ret = bb_getUInt8(data, &index)) != 0)
    {
        return ret;
    }
#ifdef DLMS_IGNORE_MALLOC
    arr = &settings->transaction.targets;
    if (arr->capacity == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    e = &arr->data[0];
    ve_clear(e);
    arr->size = 1;
    e->value.byteArr = data;
    e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
#else
    gxValueEventCollection list;
    e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    e->value.byteArr = data;
    e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
    e->index = index;
    vec_init(&list);
    vec_push(&list, e);
    arr = &list;
#endif //DLMS_IGNORE_MALLOC
    e->index = index;
    // Access selection
    unsigned char selection;
    if ((ret = bb_getUInt8(data, &selection)) != 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        vec_clear(arr);
#endif //DLMS_IGNORE_MALLOC
        return ret;
    }
    if (selection != 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = bb_getUInt8(data, &e->selector)) != 0)
        {
            vec_clear(arr);
            return ret;
        }
        e->parameters.byteArr = data;
        e->parameters.vt = DLMS_DATA_TYPE_OCTET_STRING;
#else
        gxDataInfo di;
        di_init(&di);
        if ((ret = bb_getUInt8(data, &e->selector)) != 0 ||
            (ret = dlms_getData(data, &di, &e->parameters)) != 0)
        {
            vec_clear(arr);
            return ret;
        }
        bb_clear(data);
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        bb_clear(data);
    }
    if ((ret = oa_findByLN(&settings->base.objects, (DLMS_OBJECT_TYPE)ci, ln, &e->target)) != 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        vec_clear(arr);
#endif //DLMS_IGNORE_MALLOC
        return ret;
    }
    if (e->target == NULL)
    {
        ret = svr_findObject(&settings->base, (DLMS_OBJECT_TYPE)ci, 0, ln, e);
    }
    if (e->target == NULL)
    {
        // Access Error : Device reports a undefined object.
        status = DLMS_ERROR_CODE_UNDEFINED_OBJECT;
    }
    else
    {
        DLMS_ACCESS_MODE mode = svr_getAttributeAccess(&settings->base, e->target, index);
        if ((mode & DLMS_ACCESS_MODE_READ) == 0)
        {
            status = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        else
        {
            //Handle default Association LN read as a special case.
            static const unsigned char DEFAULT_ASSOCIATION[6] = { 0, 0, 40, 0, 0, 255 };
            if (ci == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME && index == 1 && memcmp(DEFAULT_ASSOCIATION, ln, 6) == 0)
            {
                e->byteArray = 1;
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = bb_setUInt8(data, 6)) != 0 ||
                    (ret = bb_set(data, DEFAULT_ASSOCIATION, 6)) != 0)
                {
#ifdef DLMS_IGNORE_MALLOC
                    vec_clear(arr);
#endif //DLMS_IGNORE_MALLOC
                    return ret;
                }
            }
            else
            {
                svr_preRead(&settings->base, arr);
                if (!e->handled)
                {
                    if ((ret = cosem_getValue(&settings->base, e)) != 0)
                    {
                        status = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    }
                    svr_postRead(&settings->base, arr);
                }
            }
            if (status == 0)
            {
                status = e->error;
            }
            if (status == 0)
            {
#ifdef DLMS_IGNORE_MALLOC
                if (e->value.vt != DLMS_DATA_TYPE_OCTET_STRING)
                {
                    if ((ret = dlms_setData(data, e->value.vt, &e->value)) != 0)
                    {
                        status = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    }
                }
#else
                if (!e->byteArray)
                {
                    if ((ret = dlms_setData(data, e->value.vt, &e->value)) != 0)
                    {
                        status = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    }
                }
                else if (!bb_isAttached(e->value.byteArr))
                {
                    if ((ret = bb_set2(data, e->value.byteArr, 0, e->value.byteArr->size)) != 0)
                    {
                        status = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    }
                    var_clear(&e->value);
                }
#endif //DLMS_IGNORE_MALLOC
            }
            else if (status != 0)
            {
                bb_clear(data);
            }
        }
    }
    //PDU is used for serialization. Set data type to none so size is not changed.
    if (e->byteArray || (e->value.vt == DLMS_DATA_TYPE_OCTET_STRING && bb_isAttached(e->value.byteArr)))
    {
        e->value.vt = DLMS_DATA_TYPE_NONE;
    }
    unsigned char moreData = e->transactionStartIndex != e->transactionEndIndex;
    gxLNParameters p;
    params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 1, NULL, data, status, settings->info.encryptedCommand, moreData, !moreData);
    ret = dlms_getLNPdu(&p, data);
    if (e->transactionStartIndex != e->transactionEndIndex || data->size > settings->base.maxPduSize)
    {
#ifndef DLMS_IGNORE_MALLOC
        trans_clear(&settings->transaction);
        settings->transaction.targets = list;
        vec_init(&list);
        if (!bb_isAttached(data))
        {
            ret = bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
        }
#endif //DLMS_IGNORE_MALLOC
        e->transaction = 1;
        settings->transaction.command = DLMS_COMMAND_GET_REQUEST;
    }
#ifndef DLMS_IGNORE_MALLOC
    vec_clear(arr);
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int svr_getRequestNextDataBlock(
    dlmsServerSettings* settings,
    gxByteBuffer* data,
    unsigned char invokeId)
{
    gxValueEventArg* e;
    gxLNParameters p;
    unsigned char moreData;
    int ret;
    uint32_t index, pos;
    // Get block index.
    if ((ret = bb_getUInt32(data, &index)) != 0)
    {
        return ret;
    }
    bb_clear(data);
    if (index != settings->base.blockIndex)
    {
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 2,
            NULL, NULL, DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID, settings->info.encryptedCommand, 0, 0);
        return dlms_getLNPdu(&p, data);
    }
    else
    {
        ++settings->base.blockIndex;
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 2, NULL, data, DLMS_ERROR_CODE_OK, settings->info.encryptedCommand, 1, 1);
        // If transaction is not in progress.
        if (settings->transaction.command == DLMS_COMMAND_NONE)
        {
            p.status = DLMS_ERROR_CODE_NO_LONG_GET_OR_READ_IN_PROGRESS;
        }
        else
        {
#ifndef DLMS_IGNORE_MALLOC
            bb_set2(data, &settings->transaction.data, 0, settings->transaction.data.size);
            bb_clear(&settings->transaction.data);
#endif //DLMS_IGNORE_MALLOC
            moreData = 1;
            // If there is multiple blocks on the buffer.
            // This might happen when Max PDU size is very small.
            if (data->size < settings->base.maxPduSize)
            {
                for (pos = 0; pos != settings->transaction.targets.size; ++pos)
                {
                    if ((ret = vec_getByIndex(&settings->transaction.targets, pos, &e)) != 0)
                    {
                        break;
                    }
                    e->value.byteArr = data;
                    e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
                    svr_preRead(&settings->base, &settings->transaction.targets);
                    if (!e->handled)
                    {
                        if ((ret = cosem_getValue(&settings->base, e)) != 0)
                        {
                            p.status = DLMS_ERROR_CODE_HARDWARE_FAULT;
                        }
                        svr_postRead(&settings->base, &settings->transaction.targets);
                    }
                    // Add data.
                    if (e->byteArray)
                    {
                        e->value.vt = DLMS_DATA_TYPE_NONE;
                    }
                    else
                    {
                        if ((ret = dlms_setData(data, e->value.vt, &e->value)) != 0)
                        {
                            break;
                        }
                        var_clear(&e->value);
                    }
                }
                p.lastBlock = !(e->transactionStartIndex < e->transactionEndIndex);
            }
            ret = dlms_getLNPdu(&p, data);
            if (moreData || data->size - data->position != 0)
            {
                //Append data to transaction data if buffer is not static.
#ifndef DLMS_IGNORE_MALLOC
                if (!bb_isAttached(data))
                {
                    bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
                }
#endif //DLMS_IGNORE_MALLOC
            }
            else
            {
                trans_clear(&settings->transaction);
                resetBlockIndex(&settings->base);
            }
        }
    }
    return ret;
}

int svr_getRequestWithList(
    dlmsServerSettings* settings,
    gxByteBuffer* data,
    unsigned char invokeId)
{
    int ret = 0;
    gxValueEventCollection* arr;
    gxValueEventArg* e;
    DLMS_OBJECT_TYPE ci;
    gxLNParameters p;
    unsigned char attributeIndex;
    uint16_t id;
    uint16_t pos, cnt;
    unsigned char* ln;
#ifdef DLMS_IGNORE_MALLOC
    if (hlp_getObjectCount2(data, &cnt) != 0 || cnt > settings->transaction.targets.capacity)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    arr = &settings->transaction.targets;
    arr->size = (unsigned char)cnt;
#else
    gxValueEventCollection list;
    if (hlp_getObjectCount2(data, &cnt) != 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    vec_init(&list);
    arr = &list;
#endif //DLMS_IGNORE_MALLOC

    for (pos = 0; pos != cnt; ++pos)
    {
        if ((ret = bb_getUInt16(data, &id)) != 0)
        {
            break;
        }
        ci = (DLMS_OBJECT_TYPE)id;
        ln = data->data + data->position;
        data->position += 6;
        if ((ret = bb_getUInt8(data, &attributeIndex)) != 0)
        {
            break;
        }
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = vec_getByIndex(arr, pos, &e)) != 0)
        {
            break;
        }
        ve_clear(e);
#else
        e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
        ve_init(e);
        vec_push(&list, e);
#endif //DLMS_IGNORE_MALLOC

        e->index = attributeIndex;
        if ((ret = oa_findByLN(&settings->base.objects, ci, ln, &e->target)) != 0)
        {
            break;
        }
        if (e->target == NULL)
        {
            ret = svr_findObject(&settings->base, ci, 0, ln, e);
        }
        if (e->target == NULL)
        {
            // Access Error : Device reports a undefined object.
            e->error = DLMS_ERROR_CODE_UNDEFINED_OBJECT;
        }
        else
        {
            if (svr_getAttributeAccess(&settings->base, e->target, attributeIndex) == DLMS_ACCESS_MODE_NONE)
            {
                // Read Write denied.
                e->error = DLMS_ERROR_CODE_READ_WRITE_DENIED;
            }
            else
            {
                // AccessSelection
                unsigned char selection;
                if ((ret = bb_getUInt8(data, &selection)) != 0)
                {
                    break;
                }
                if (selection != 0)
                {
                    gxDataInfo di;
                    di_init(&di);
                    if ((ret = bb_getUInt8(data, &e->selector)) != 0)
                    {
                        break;
                    }
                    if ((ret = dlms_getData(data, &di, &e->parameters)) != 0)
                    {
                        break;
                    }
                }
            }
        }
    }
    bb_clear(data);
    svr_preRead(&settings->base, arr);
    hlp_setObjectCount(cnt, data);
    unsigned char moreData = 0;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = vec_getByIndex(arr, pos, &e)) != 0)
        {
            break;
        }
#ifdef DLMS_IGNORE_MALLOC
        e->value.byteArr = data;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
#endif //DLMS_IGNORE_MALLOC
        if (e->error == 0 && !e->handled)
        {
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
            uint32_t pos2 = data->size;
#else
            uint16_t pos2 = data->size;
#endif
            bb_setUInt8(data, 0);
            if ((ret = cosem_getValue(&settings->base, e)) != 0)
            {
                e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
            }
            bb_setUInt8ByIndex(data, pos2, (unsigned char)e->error);
        }
        if (e->error == 0)
        {
            if (!e->byteArray)
            {
                if ((ret = dlms_setData(data, e->value.vt, &e->value)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                }
            }
            else if (!bb_isAttached(e->value.byteArr))
            {
                if ((ret = bb_set2(data, e->value.byteArr, 0, e->value.byteArr->size)) != 0)
                {
                    e->error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                }
                var_clear(&e->value);
            }
        }
        else
        {
            bb_setUInt8(data, 1);
            bb_setUInt8(data, (unsigned char)e->error);
        }
        //PDU is used for serialization. Set data type to none so size is not changed.
        if (e->byteArray || (e->value.vt == DLMS_DATA_TYPE_OCTET_STRING && bb_isAttached(e->value.byteArr)))
        {
            e->value.vt = DLMS_DATA_TYPE_NONE;
        }
        if (e->transactionStartIndex != e->transactionEndIndex)
        {
            e->transaction = 1;
            settings->transaction.command = DLMS_COMMAND_GET_REQUEST;
            moreData = 1;
#ifndef DLMS_IGNORE_MALLOC
            trans_clear(&settings->transaction);
            settings->transaction.targets = list;
            var_clear(&e->value);
            vec_init(&list);
            if (!bb_isAttached(data))
            {
                bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    svr_postRead(&settings->base, arr);
    params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 3, NULL, data, 0xFF, settings->info.encryptedCommand, moreData, !moreData);
    ret = dlms_getLNPdu(&p, data);
#ifndef DLMS_IGNORE_MALLOC
    vec_clear(&list);
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int svr_handleGetRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    int ret;
    DLMS_GET_COMMAND_TYPE type;
    unsigned char invokeId, ch;
    // Return error if connection is not established.
    if (!settings->info.preEstablished && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) == 0)
    {
        return svr_generateConfirmedServiceError(
            settings,
            DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR,
            DLMS_SERVICE_ERROR_SERVICE, DLMS_SERVICE_UNSUPPORTED,
            data);
    }
    // Get type.
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    type = (DLMS_GET_COMMAND_TYPE)ch;
    // Get invoke ID and priority.
    if ((ret = bb_getUInt8(data, &invokeId)) != 0)
    {
        return ret;
    }
    updateInvokeId(settings, invokeId);
    // GetRequest normal
    if (type == DLMS_GET_COMMAND_TYPE_NORMAL)
    {
        ret = svr_getRequestNormal(settings, data, invokeId);
    }
    else if (type == DLMS_GET_COMMAND_TYPE_NEXT_DATA_BLOCK)
    {
        // Get request for next data block
        ret = svr_getRequestNextDataBlock(settings, data, invokeId);
    }
    else if (type == DLMS_GET_COMMAND_TYPE_WITH_LIST)
    {
        // Get request with a list.
        ret = svr_getRequestWithList(settings, data, invokeId);
    }
    else
    {
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        type = DLMS_GET_COMMAND_TYPE_NORMAL;
    }
    if (ret != 0)
    {
        //If error is not DLMS error code.
        if (!(ret > 0 && ret <= DLMS_ERROR_CODE_OTHER_REASON))
        {
            // Access Error : Device reports Read-Write denied.
            ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        gxLNParameters p;
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, type, NULL, data, ret, settings->info.encryptedCommand, 0, 0);
        bb_clear(data);
        ret = dlms_getLNPdu(&p, data);
    }
    return ret;
}

#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
/**
 * Find Short Name object.
 *
 * @param sn
 */
int svr_findSNObject(dlmsServerSettings* settings, int sn, gxSNInfo* i)
{
    uint16_t pos;
    int ret;
    gxObject* it;
    unsigned char offset, count;
    i->action = 0;
    i->item = NULL;
    i->index = 0;

    for (pos = 0; pos != settings->base.objects.size; ++pos)
    {
        if ((ret = oa_getByIndex(&settings->base.objects, pos, &it)) != 0)
        {
            return ret;
        }
        if (sn >= it->shortName)
        {
            // If attribute is accessed.
            if (sn < it->shortName + obj_attributeCount(it) * 8)
            {
                i->action = 0;
                i->item = it;
                i->index = (unsigned char)(((sn - it->shortName) / 8) + 1);
                break;
            }
            else
            {
                // If method is accessed.
                dlms_getActionInfo((DLMS_OBJECT_TYPE)it->objectType, &offset, &count);
                if (sn < it->shortName + offset + (8 * count))
                {
                    i->item = it;
                    i->action = 1;
                    i->index = (unsigned char)((sn - it->shortName - offset) / 8 + 1);
                    break;
                }
            }
        }
    }
    if (i->item == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return 0;
}

int svr_handleRead(
    dlmsServerSettings* settings,
    DLMS_VARIABLE_ACCESS_SPECIFICATION type,
    gxByteBuffer* data,
    gxValueEventCollection* list,
    gxValueEventCollection* reads,
    gxValueEventCollection* actions)
{
    gxSNInfo info;
    gxValueEventArg* e;
    int ret;
    gxDataInfo di;
    uint16_t sn;
    di_init(&di);
    if ((ret = bb_getUInt16(data, &sn)) != 0)
    {
        return ret;
    }
    if ((ret = svr_findSNObject(settings, sn, &info)) != 0)
    {
        return ret;
    }

#ifdef DLMS_IGNORE_MALLOC
    if (settings->transaction.targets.capacity < 1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    list = &settings->transaction.targets;
    list->size = 1;
    e = &list->data[0];
    ve_clear(e);
#else
    e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    vec_push(list, e);
#endif //DLMS_IGNORE_MALLOC
    e->target = info.item;
    e->index = info.index;
    e->action = info.action;
    if (type == DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS)
    {
        if ((ret = bb_getUInt8(data, &e->selector)) != 0)
        {
            return ret;
        }
        if ((ret = dlms_getData(data, &di, &e->parameters)) != 0)
        {
            return ret;
        }
    }
    // Return error if connection is not established.
    if (!settings->info.preEstablished && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) == 0
        && (!e->action || e->target->shortName != 0xFA00 || e->index != 8))
    {
        return svr_generateConfirmedServiceError(
            settings,
            DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR, DLMS_SERVICE_ERROR_SERVICE,
            DLMS_SERVICE_UNSUPPORTED, data);
    }
    if (svr_getAttributeAccess(&settings->base, info.item, info.index) == DLMS_ACCESS_MODE_NONE)
    {
        e->error = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        e->handled = 1;
    }
    else
    {
        if (e->action)
        {
            vec_push(actions, e);
        }
        else
        {
            vec_push(reads, e);
        }
    }
    return ret;
}
#endif //#ifdef !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)

/**
* Get data for Read command.
*
* @param settings
*            DLMS settings.
* @param list
*            received objects.
* @param data
*            Data as byte array.
* @param type
*            Response type.
*/
int svr_getReadData(
    dlmsServerSettings* settings,
    gxValueEventCollection* list,
    gxByteBuffer* data,
    DLMS_SINGLE_READ_RESPONSE* type,
    unsigned char* multipleBlocks)
{
    int ret;
    gxValueEventArg* e;
    uint32_t pos;
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    uint32_t statusindex;
#else
    uint16_t statusindex;
#endif
    unsigned char first = 1;
    *type = DLMS_SINGLE_READ_RESPONSE_DATA;
    bb_clear(data);
    for (pos = 0; pos != list->size; ++pos)
    {
        if ((ret = vec_getByIndex(list, pos, &e)) != 0)
        {
            break;
        }
        e->value.byteArr = data;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        statusindex = data->size;
        if (!e->handled)
        {
            if (e->action)
            {
                if ((ret = cosem_invoke(settings, e)) != 0)
                {
                    e->error = (DLMS_ERROR_CODE)ret;
                }
            }
            else
            {
                if ((ret = cosem_getValue(&settings->base, e)) != 0)
                {
                    e->error = (DLMS_ERROR_CODE)ret;
                }
            }
        }
        if (e->error == DLMS_ERROR_CODE_OK)
        {
            if (!first && list->size != 1)
            {
                bb_insertUInt8(data, statusindex, DLMS_SINGLE_READ_RESPONSE_DATA);
            }
            if (e->byteArray)
            {
                e->value.vt = DLMS_DATA_TYPE_NONE;
            }
            else
            {
                if ((ret = dlms_setData(data, e->value.vt, &e->value)) != 0)
                {
                    var_clear(&e->value);
                    break;
                }
            }
        }
        else
        {
            if (!first && list->size != 1)
            {
                bb_insertUInt8(data, statusindex, DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR);
            }
            bb_insertUInt8(data, statusindex + 1, e->error);
            *type = DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR;
        }
        var_clear(&e->value);
        first = 0;
        *multipleBlocks |= e->transactionEndIndex != e->transactionStartIndex;
        e->transaction = *multipleBlocks;
    }
    return 0;
}
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
int svr_handleReadBlockNumberAccess(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    gxSNParameters p;
    gxValueEventArg* e;
    uint16_t pos, blockNumber;
    unsigned char lastBlock = 1;
    int ret;
    unsigned char multipleBlocks;
    if ((ret = bb_getUInt16(data, &blockNumber)) != 0)
    {
        return ret;
    }
    if (blockNumber != settings->base.blockIndex)
    {
        gxByteBuffer bb;
        bb_init(&bb);
        bb_setUInt8(&bb, DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID);
        params_initSN(&p, &settings->base,
            DLMS_COMMAND_READ_RESPONSE, 1,
            DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL, settings->info.encryptedCommand);
        ret = dlms_getSNPdu(&p, data);
        resetBlockIndex(&settings->base);
        bb_clear(&bb);
        return ret;
    }
    gxValueEventCollection reads;
    gxValueEventCollection actions;
    vec_init(&reads);
    vec_init(&actions);
    for (pos = 0; pos != settings->transaction.targets.size; ++pos)
    {
        if ((ret = vec_getByIndex(&settings->transaction.targets, pos, &e)) != 0)
        {
            break;
        }
        if (e->action)
        {
            vec_push(&actions, e);
        }
        else
        {
            vec_push(&reads, e);
        }
    }
    if (ret == 0)
    {
        DLMS_SINGLE_READ_RESPONSE requestType;
        if (reads.size != 0)
        {
            svr_preRead(&settings->base, &reads);
        }

        if (actions.size != 0)
        {
            svr_preAction(&settings->base, &actions);
        }
        ret = svr_getReadData(settings, &settings->transaction.targets,
            data, &requestType, &multipleBlocks);
        if (reads.size != 0)
        {
            svr_postRead(&settings->base, &reads);
        }
        if (actions.size != 0)
        {
            svr_postAction(&settings->base, &actions);
        }
        if (lastBlock)
        {
            lastBlock = e->transactionStartIndex == e->transactionEndIndex;
        }
    }
    vec_empty(&reads);
    vec_empty(&actions);
    if (ret == 0)
    {
        ++settings->base.blockIndex;
        params_initSN(&p, &settings->base, DLMS_COMMAND_READ_RESPONSE, 1,
            DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT, NULL, data, settings->info.encryptedCommand);
        p.multipleBlocks = 1;
        p.lastBlock = lastBlock;
        ret = dlms_getSNPdu(&p, data);
        // If all data is sent.
        if (p.lastBlock && settings->transaction.data.size == settings->transaction.data.position)
        {
            trans_clear(&settings->transaction);
            resetBlockIndex(&settings->base);
        }
        else
        {
            bb_trim(&settings->transaction.data);
        }
    }
    return ret;
}

int svr_handleReadDataBlockAccess(
    dlmsServerSettings* settings,
    DLMS_COMMAND command,
    gxByteBuffer* data,
    int cnt)
{
    gxSNParameters p;
    int ret;
    uint16_t size;
    uint16_t blockNumber;
    unsigned char isLast, ch;
    unsigned char count = 1, type = DLMS_DATA_TYPE_OCTET_STRING;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    isLast = ch != 0;
    if ((ret = bb_getUInt16(data, &blockNumber)) != 0)
    {
        return ret;
    }
    if (blockNumber != settings->base.blockIndex)
    {
        gxByteBuffer bb;
        bb_init(&bb);
        bb_setUInt8(&bb, DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID);
        params_initSN(&p, &settings->base, command, 1, DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL, settings->info.encryptedCommand);
        ret = dlms_getSNPdu(&p, data);
        resetBlockIndex(&settings->base);
        bb_clear(&bb);
        return ret;
    }
    if (command == DLMS_COMMAND_WRITE_RESPONSE)
    {
        if ((ret = bb_getUInt8(data, &count)) != 0 ||
            (ret = bb_getUInt8(data, &type)) != 0)
        {
            return ret;
        }
    }
    if (hlp_getObjectCount2(data, &size) != 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (count != 1 || type != DLMS_DATA_TYPE_OCTET_STRING ||
        size != data->size - data->position)
    {
        gxByteBuffer bb;
        bb_init(&bb);
        bb_setUInt8(&bb, DLMS_ERROR_CODE_BLOCK_UNAVAILABLE);
        params_initSN(&p, &settings->base, command, cnt,
            DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL, settings->info.encryptedCommand);
        ret = dlms_getSNPdu(&p, data);
        bb_clear(&bb);
        resetBlockIndex(&settings->base);
        return ret;
    }
    settings->transaction.command = command;
    bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
    if (!isLast)
    {
        gxByteBuffer bb;
        bb_init(&bb);
        bb_setUInt16(&bb, blockNumber);
        ++settings->base.blockIndex;
        if (command == DLMS_COMMAND_READ_RESPONSE)
        {
            type = DLMS_SINGLE_READ_RESPONSE_BLOCK_NUMBER;
        }
        else
        {
            type = DLMS_SINGLE_WRITE_RESPONSE_BLOCK_NUMBER;
        }
        params_initSN(&p, &settings->base, command, cnt, type, NULL, &bb, settings->info.encryptedCommand);
        ret = dlms_getSNPdu(&p, data);
        bb_clear(&bb);
    }
    else
    {
        data->position = data->size = 0;
        bb_set2(data, &settings->transaction.data,
            settings->transaction.data.position,
            settings->transaction.data.size - settings->transaction.data.position);
        trans_clear(&settings->transaction);
        if (command == DLMS_COMMAND_READ_RESPONSE)
        {
            ret = svr_handleReadRequest(settings, data);
        }
        else
        {
            ret = svr_handleWriteRequest(settings, data);
        }
        resetBlockIndex(&settings->base);
    }
    return ret;
}

int svr_returnSNError(
    dlmsServerSettings* settings,
    DLMS_COMMAND cmd,
    DLMS_ERROR_CODE error,
    gxByteBuffer* data)
{
    int ret;
    gxByteBuffer bb;
    gxSNParameters p;
    bb_init(&bb);
    bb_setUInt8(&bb, error);
    params_initSN(&p, &settings->base, cmd, 1,
        DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL, settings->info.encryptedCommand);
    ret = dlms_getSNPdu(&p, data);
    resetBlockIndex(&settings->base);
    return ret;
}

/**
 * Handle read request.
 */
int svr_handleReadRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    DLMS_SINGLE_READ_RESPONSE requestType;
    gxSNParameters p;
    int ret = 0;
    unsigned char ch, multipleBlocks = 0;
    uint16_t pos, cnt = 0;
    DLMS_VARIABLE_ACCESS_SPECIFICATION type;
    gxValueEventCollection list;
    gxValueEventCollection reads;
    gxValueEventCollection actions;
    if (settings->base.useLogicalNameReferencing)
    {
        return svr_generateConfirmedServiceError(
            settings,
            DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR,
            DLMS_SERVICE_ERROR_SERVICE, DLMS_SERVICE_UNSUPPORTED,
            data);
    }
    vec_init(&reads);
    vec_init(&actions);
    // If get next frame.
    if (data->size == 0)
    {
        if (settings->transaction.command != DLMS_COMMAND_NONE)
        {
            return 0;
        }
        list = settings->transaction.targets;
    }
    else
    {
        vec_init(&list);
        if (hlp_getObjectCount2(data, &cnt) != 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        for (pos = 0; pos != cnt; ++pos)
        {
            if ((ret = bb_getUInt8(data, &ch)) != 0)
            {
                break;
            }
            type = (DLMS_VARIABLE_ACCESS_SPECIFICATION)ch;
            switch (type)
            {
            case DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME:
            case DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS:
                ret = svr_handleRead(settings, type, data, &list, &reads, &actions);
                break;
            case DLMS_VARIABLE_ACCESS_SPECIFICATION_BLOCK_NUMBER_ACCESS:
                vec_clear(&list);
                vec_clear(&reads);
                vec_clear(&actions);
                return svr_handleReadBlockNumberAccess(settings, data);
            case DLMS_VARIABLE_ACCESS_SPECIFICATION_READ_DATA_BLOCK_ACCESS:
                vec_clear(&list);
                vec_clear(&reads);
                vec_clear(&actions);
                return svr_handleReadDataBlockAccess(settings, DLMS_COMMAND_READ_RESPONSE, data, cnt);
            default:
                vec_clear(&list);
                vec_clear(&reads);
                vec_clear(&actions);
                return svr_returnSNError(settings, DLMS_COMMAND_READ_RESPONSE, DLMS_ERROR_CODE_READ_WRITE_DENIED, data);
            }
        }
        if (ret == 0)
        {
            if (reads.size != 0)
            {
                svr_preRead(&settings->base, &reads);
            }
            if (actions.size != 0)
            {
                svr_preAction(&settings->base, &actions);
            }
        }
    }
    if (ret == 0)
    {
        ret = svr_getReadData(settings, &list, data, &requestType, &multipleBlocks);
        if (ret == 0)
        {
            if (reads.size != 0)
            {
                svr_postRead(&settings->base, &reads);
            }
            if (actions.size != 0)
            {
                svr_postAction(&settings->base, &actions);
            }

            params_initSN(&p, &settings->base, DLMS_COMMAND_READ_RESPONSE, cnt,
                requestType, NULL, data, settings->info.encryptedCommand);
            p.multipleBlocks = multipleBlocks;
            p.lastBlock = list.data[0]->transactionStartIndex == list.data[0]->transactionEndIndex;
            ret = dlms_getSNPdu(&p, data);
            if (ret == 0)
            {
                if (list.data[0]->transactionStartIndex != list.data[0]->transactionEndIndex)
                {
                    settings->transaction.targets = list;
                    vec_init(&list);
                    settings->transaction.command = DLMS_COMMAND_READ_REQUEST;
                    if (!bb_isAttached(data))
                    {
                        bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
                    }
                }
            }
        }
    }
    vec_empty(&reads);
    vec_empty(&actions);
    vec_clear(&list);
    return ret;
}

int svr_handleWriteRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    gxSNParameters p;
    int ret = 0;
    gxValueEventArg* e;
    unsigned char ch;
    uint16_t sn;
    uint16_t cnt, pos;
    gxDataInfo di;
    DLMS_ACCESS_MODE am;
    gxSNInfo i;
    gxValueEventCollection arr;
    DLMS_VARIABLE_ACCESS_SPECIFICATION type;
    gxByteBuffer results;
    if (settings->base.useLogicalNameReferencing)
    {
        return svr_generateConfirmedServiceError(
            settings,
            DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR,
            DLMS_SERVICE_ERROR_SERVICE, DLMS_SERVICE_UNSUPPORTED,
            data);
    }
    // Return error if connection is not established.
    if (!settings->info.preEstablished && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) == 0)
    {
        return svr_generateConfirmedServiceError(
            settings,
            DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR,
            DLMS_SERVICE_ERROR_SERVICE,
            DLMS_SERVICE_UNSUPPORTED, data);
    }
    // Get object count.
    if (hlp_getObjectCount2(data, &cnt) != 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_init(&results);
    bb_capacity(&results, cnt);
    vec_init(&arr);
    for (pos = 0; pos != cnt; ++pos)
    {
        if ((ret = bb_getUInt8(data, &ch)) != 0)
        {
            break;
        }
        type = (DLMS_VARIABLE_ACCESS_SPECIFICATION)ch;
        switch (type)
        {
        case DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME:
            if ((ret = bb_getUInt16(data, &sn)) != 0)
            {
                break;
            }
            if ((ret = svr_findSNObject(settings, sn, &i)) != 0)
            {
                break;
            }
            e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
            ve_init(e);
            e->target = i.item;
            e->index = i.index;
            vec_push(&arr, e);
            // If target is unknown.
            if (i.item == NULL)
            {
                // Device reports a undefined object.
                bb_setUInt8(&results, DLMS_ERROR_CODE_UNDEFINED_OBJECT);
            }
            else
            {
                bb_setUInt8(&results, DLMS_ERROR_CODE_OK);
            }
            break;
        case DLMS_VARIABLE_ACCESS_SPECIFICATION_WRITE_DATA_BLOCK_ACCESS:
            bb_clear(&results);
            return svr_handleReadDataBlockAccess(settings, DLMS_COMMAND_WRITE_RESPONSE, data, cnt);
        default:
            // Device reports a HW error.
            bb_setUInt8(&results, DLMS_ERROR_CODE_HARDWARE_FAULT);
        }
    }
    // Get data count.
    if (hlp_getObjectCount2(data, &cnt) != 0)
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        cnt = 0;
    }
    di_init(&di);
    for (pos = 0; pos != cnt; ++pos)
    {
        if ((ret = bb_getUInt8ByIndex(&results, pos, &ch)) != 0)
        {
            break;
        }
        if (ch == 0)
        {
            // If object has found.
            if ((ret = vec_getByIndex(&arr, pos, &e)) != 0)
            {
                break;
            }
            if ((ret = dlms_getData(data, &di, &e->value)) != 0)
            {
                break;
            }
            di_init(&di);
            am = svr_getAttributeAccess(&settings->base, e->target, e->index);
            // If write is denied.
            if (am != DLMS_ACCESS_MODE_WRITE && am != DLMS_ACCESS_MODE_READ_WRITE)
            {
                bb_setUInt8ByIndex(&results, pos, DLMS_ERROR_CODE_READ_WRITE_DENIED);
            }
            else
            {
                svr_preWrite(&settings->base, &arr);
                if (e->error != 0)
                {
                    bb_setUInt8ByIndex(&results, pos, e->error);
                }
                else if (!e->handled)
                {
                    if ((ret = cosem_setValue(&settings->base, e)) != 0)
                    {
                        break;
                    }
                    svr_postWrite(&settings->base, &arr);
                    if (e->error != 0)
                    {
                        bb_setUInt8ByIndex(&results, pos, e->error);
                    }
                }
            }
        }
    }
    if (ret == 0)
    {
        gxByteBuffer bb;
        bb_init(&bb);
        bb_capacity(&bb, 2 * cnt);
        for (pos = 0; pos != cnt; ++pos)
        {
            if ((ret = bb_getUInt8(&results, &ch)) != 0)
            {
                break;
            }
            // If meter returns error.
            if (ch != 0)
            {
                bb_setUInt8(&bb, 1);
            }
            bb_setUInt8(&bb, ch);
        }
        params_initSN(&p, &settings->base, DLMS_COMMAND_WRITE_RESPONSE, cnt, 0xFF, NULL, &bb, settings->info.encryptedCommand);
        p.lastBlock = e->transactionStartIndex == e->transactionEndIndex;
        ret = dlms_getSNPdu(&p, data);
        bb_clear(&bb);
    }
    // If all data is transfered.
    bb_clear(&results);
    vec_clear(&arr);
    return ret;
}
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)

#ifndef DLMS_IGNORE_ACTION
/**
* Handle action request.
*
* @param reply
*            Received data from the client.
* @return Reply.
*/
int svr_handleMethodRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data)
{
    DLMS_OBJECT_TYPE ci;
    gxValueEventArg* e;
    unsigned char* ln;
    int error = DLMS_ERROR_CODE_OK;
    int ret;
    unsigned char invokeId, ch, id;
    uint16_t tmp;
    gxValueEventCollection* list;
#ifndef DLMS_IGNORE_MALLOC
    gxValueEventCollection arr;
#endif //DLMS_IGNORE_MALLOC
    // Get type.
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    // Get invoke ID and priority.
    if ((ret = bb_getUInt8(data, &invokeId)) != 0)
    {
        return ret;
    }
    updateInvokeId(settings, invokeId);
    // CI
    if ((ret = bb_getUInt16(data, &tmp)) != 0)
    {
        return ret;
    }
    ci = (DLMS_OBJECT_TYPE)tmp;
    ln = data->data + data->position;
    data->position += 6;
    // Attribute
    if ((ret = bb_getUInt8(data, &id)) != 0)
    {
        return ret;
    }
    // Get parameters.
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
#ifdef DLMS_IGNORE_MALLOC
    if (settings->transaction.targets.capacity < 1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    list = &settings->transaction.targets;
    list->size = 1;
    e = &list->data[0];
    ve_clear(e);
#else
    list = &arr;
    vec_init(list);
    e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    vec_push(list, e);
#endif //DLMS_IGNORE_MALLOC
    e->index = id;
    if (ch != 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        e->parameters.byteArr = data;
        e->parameters.vt = DLMS_DATA_TYPE_OCTET_STRING;
#endif //DLMS_IGNORE_MALLOC
        gxDataInfo di;
        di_init(&di);
        if ((ret = dlms_getData(data, &di, &e->parameters)) != 0)
        {
#ifndef DLMS_IGNORE_MALLOC
            bb_clear(data);
            vec_clear(&arr);
#endif //DLMS_IGNORE_MALLOC
            return ret;
        }
    }
    if (ci == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
        e->target = NULL;
    }
    else
    {
        if ((ret = oa_findByLN(&settings->base.objects, ci, ln, &e->target)) != 0)
        {
#ifndef DLMS_IGNORE_MALLOC
            bb_clear(data);
            vec_clear(&arr);
#endif //DLMS_IGNORE_MALLOC
            return ret;
        }
    }
    if (e->target == NULL)
    {
        ret = svr_findObject(&settings->base, ci, 0, ln, e);
    }
#ifndef DLMS_IGNORE_MALLOC
    bb_clear(data);
#endif //DLMS_IGNORE_MALLOC
    if (e->target == NULL)
    {
        error = DLMS_ERROR_CODE_UNDEFINED_OBJECT;
    }
    else
    {
#if !defined(DLMS_ITALIAN_STANDARD)
        //In Italian standard reply_to_HLS_authentication can be called without access rights.
        if (!(ci == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME && id == 1) &&
            svr_getMethodAccess(&settings->base, e->target, id) == DLMS_METHOD_ACCESS_MODE_NONE)
#else
        if (svr_getMethodAccess(&settings->base, e->target, id) == DLMS_METHOD_ACCESS_MODE_NONE)
#endif //defined(DLMS_ITALIAN_STANDARD)
        {
            e->error = DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        else
        {
            svr_preAction(&settings->base, list);
            if (!e->handled)
            {
                if ((ret = cosem_invoke(settings, e)) != 0)
                {
                    e->error = (DLMS_ERROR_CODE)ret;
                    ret = 0;
                }
                svr_postAction(&settings->base, list);
            }
        }
#ifdef DLMS_IGNORE_MALLOC
        if (data->position == data->size)
        {
            bb_clear(data);
        }
#endif //DLMS_IGNORE_MALLOC

        // Set default action reply if not given.
        if (e->error == DLMS_ERROR_CODE_OK)
        {
            if (// Add return parameters
                (ret = bb_insertUInt8(data, 0, 1)) != 0 ||
                //Add parameters error code.
                (ret = bb_insertUInt8(data, 1, 0)) != 0)
            {
                error = DLMS_ERROR_CODE_HARDWARE_FAULT;
            }
            if (e->byteArray)
            {
                e->value.vt = DLMS_DATA_TYPE_NONE;
            }
            else
            {
                if ((ret = dlms_setData(data, e->value.vt, &e->value)) != 0)
                {
                    error = DLMS_ERROR_CODE_HARDWARE_FAULT;
                }
            }
        }
        else
        {
            // Add parameters error code.
            if (e->error > 0 && e->error < DLMS_ERROR_CODE_OTHER_REASON + 1)
            {
                error = e->error;
            }
            else
            {
                error = DLMS_ERROR_CODE_READ_WRITE_DENIED;
            }
            bb_clear(data);
            bb_setUInt8(data, 0);
        }
    }
    if (ret == 0)
    {
        gxLNParameters p;
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_METHOD_RESPONSE, 1, NULL, data, error, settings->info.encryptedCommand, 0, 0);
        ret = dlms_getLNPdu(&p, data);
        // If High level authentication fails.
        if (e->target != NULL && e->target->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME && id == 1)
        {
            if (((gxAssociationLogicalName*)e->target)->associationStatus == DLMS_ASSOCIATION_STATUS_ASSOCIATED)
            {
                settings->base.connected |= DLMS_CONNECTION_STATE_DLMS;
                svr_connected(settings);
            }
            else
            {
                svr_invalidConnection(settings);
                settings->base.connected &= ~DLMS_CONNECTION_STATE_DLMS;
            }
        }
    }
#ifndef DLMS_IGNORE_MALLOC
    vec_clear(&arr);
#endif //DLMS_IGNORE_MALLOC
    return ret;
}
#endif //DLMS_IGNORE_ACTION

/**
* Handles release request.
*
* @param settings
*            DLMS settings.
* @param data
*            Received data.
*/
int svr_handleReleaseRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data) {
    int ret;
    unsigned char ch, len;
    gxByteBuffer tmp;
    //Get len.
    if ((ret = bb_getUInt8(data, &len)) != 0 ||
        (ret = bb_getUInt8(data, &ch)) != 0 ||
        (ret = bb_getUInt8(data, &ch)) != 0 ||
        //Get reason.
        (ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    unsigned char userInfo = len != 3;
    bb_clear(data);
#ifdef DLMS_IGNORE_MALLOC
    unsigned char offset = settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC ? 12 : 9;
    bb_attach(&tmp, data->data + offset, 0, data->capacity - offset);
#else
    bb_init(&tmp);
#endif //DLMS_IGNORE_MALLOC
    if (userInfo && (ret = apdu_getUserInformation(&settings->base, &tmp, 0)) != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_HDLC
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        dlms_addLLCBytes(&settings->base, data);
    }
#endif //DLMS_IGNORE_HDLC
    if ((ret = bb_setUInt8(data, 0x63)) == 0 &&
        //Len.
        (ret = bb_setUInt8(data, (unsigned char)(tmp.size + 3))) == 0 &&
        (ret = bb_setUInt8(data, 0x80)) == 0 &&
        (ret = bb_setUInt8(data, 0x01)) == 0 &&
        //Reason
        (ret = bb_setUInt8(data, ch)) == 0)
    {
        if (tmp.size != 0)
        {
            if ((ret = bb_setUInt8(data, 0xBE)) == 0 &&
                (ret = bb_setUInt8(data, (unsigned char)(tmp.size + 1))) == 0 &&
                (ret = bb_setUInt8(data, 4)) == 0 &&
                (ret = bb_setUInt8(data, (unsigned char)tmp.size)) == 0)
            {
#ifndef DLMS_IGNORE_MALLOC
                ret = bb_set(data, tmp.data, tmp.size);
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return 0;
}

int svr_handleCommand(
    dlmsServerSettings* settings,
    DLMS_COMMAND cmd,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    int ret = 0;
    unsigned char frame = 0;
    switch (cmd)
    {
#ifndef DLMS_IGNORE_SET
    case DLMS_COMMAND_SET_REQUEST:
        ret = svr_handleSetRequest(settings, data);
        break;
#endif //DLMS_IGNORE_SET
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    case DLMS_COMMAND_WRITE_REQUEST:
        ret = svr_handleWriteRequest(settings, data);
        break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    case DLMS_COMMAND_GET_REQUEST:
        //Check is client reading frames.
        if (settings->transaction.command != DLMS_COMMAND_NONE &&
            data->position == data->size)
        {
            data->position = 0;
        }
        else
        {
            ret = svr_handleGetRequest(settings, data);
        }
        break;
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    case DLMS_COMMAND_READ_REQUEST:
        if (settings->transaction.command != DLMS_COMMAND_NONE &&
            data->position == data->size)
        {
            data->position = 0;
        }
        else
        {
            ret = svr_handleReadRequest(settings, data);
        }
        break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
#ifndef DLMS_IGNORE_ACTION
    case DLMS_COMMAND_METHOD_REQUEST:
        ret = svr_handleMethodRequest(settings, data);
        if (ret != 0)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("handleMethodRequest failed. ", ret);
#endif //DLMS_DEBUG
        }
        break;
#endif //DLMS_IGNORE_ACTION
    case DLMS_COMMAND_SNRM:
        if ((ret = svr_handleSnrmRequest(settings, data)) == 0)
        {
            settings->base.connected = DLMS_CONNECTION_STATE_HDLC;
            frame = DLMS_COMMAND_UA;
        }
        else
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("handleSnrmRequest failed. ", ret);
#endif //DLMS_DEBUG
        }
        break;
    case DLMS_COMMAND_AARQ:
        ret = svr_HandleAarqRequest(settings, data);
        if (ret == 0 && settings->base.authentication < DLMS_AUTHENTICATION_HIGH)
        {
            settings->base.connected |= DLMS_CONNECTION_STATE_DLMS;
            svr_connected(settings);
        }
        else
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("HandleAarqRequest failed. ", ret);
#endif //DLMS_DEBUG
        }
        break;
    case DLMS_COMMAND_RELEASE_REQUEST:
        ret = svr_handleReleaseRequest(settings, data);
        svr_disconnected(settings);
        settings->base.connected &= ~DLMS_CONNECTION_STATE_DLMS;
        break;
    case DLMS_COMMAND_DISC:
        ret = svr_generateDisconnectRequest(settings, data);
        if (settings->base.connected != DLMS_CONNECTION_STATE_NONE)
        {
            svr_disconnected(settings);
            settings->base.connected = DLMS_CONNECTION_STATE_NONE;
        }
        frame = DLMS_COMMAND_UA;
        break;
    case DLMS_COMMAND_NONE:
        //Get next frame.
        data->position = 0;
        frame = getNextSend(&settings->base, 0);
        break;
    default:
        //Invalid command.
#ifdef DLMS_DEBUG
        svr_notifyTrace("Unknown command. ", cmd);
#endif //DLMS_DEBUG
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    if (ret != 0)
    {
        return ret;
    }
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
#ifndef DLMS_IGNORE_HDLC
        if ((ret = dlms_getHdlcFrame(&settings->base, frame, data, reply)) != 0)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("getHdlcFrame failed. ", ret);
#endif //DLMS_DEBUG
        }
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_HDLC
    }
#ifndef DLMS_IGNORE_WRAPPER
    else if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        if ((ret = dlms_getWrapperFrame(&settings->base, cmd, data, reply)) != 0)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("getHdlcFrame failed. ", ret);
#endif //DLMS_DEBUG
        }
    }
#endif //DLMS_IGNORE_WRAPPER
    else
    {
#ifdef DLMS_DEBUG
        svr_notifyTrace("Unknown frame type. ", -1);
#endif //DLMS_DEBUG
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (cmd == DLMS_COMMAND_DISC ||
        (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER && cmd == DLMS_COMMAND_RELEASE_REQUEST))
    {
        svr_reset(settings);
    }
#ifdef DLMS_DEBUG
    if (ret != 0)
    {
        svr_notifyTrace("svr_handleCommand", ret);
    }
#endif //DLMS_DEBUG
    return ret;
}

int svr_handleRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    if (data == NULL || data->data == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return svr_handleRequest2(settings, data->data, bb_available(data), reply);
}

int svr_handleRequest3(
    dlmsServerSettings* settings,
    unsigned char data,
    gxByteBuffer* reply)
{
    return svr_handleRequest2(settings, &data, 1, reply);
}

int svr_handleRequest2(
    dlmsServerSettings* settings,
    unsigned char* buff,
    uint16_t size,
    gxByteBuffer* reply)
{
    unsigned char first;
    int ret;
    bb_clear(reply);
    if (buff == NULL || size == 0)
    {
        return 0;
    }
    if (!settings->initialized)
    {
#ifdef DLMS_DEBUG
        svr_notifyTrace("Server not Initialized.", -1);
#endif //DLMS_DEBUG
        //Server not Initialized.
        return DLMS_ERROR_CODE_NOT_INITIALIZED;
    }
    //Check frame using inter Charachter Timeout.
#ifndef DLMS_IGNORE_HDLC
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC && settings->hdlc != NULL && settings->hdlc->interCharachterTimeout != 0)
    {
        uint32_t now = time_elapsed();
        uint16_t elapsed = (uint16_t)(now - settings->frameReceived) / 1000;
        //If frame shoud be fully received.
        if (elapsed >= settings->hdlc->interCharachterTimeout)
        {
            settings->receivedData.position = settings->receivedData.size = 0;
        }
        settings->frameReceived = now;
    }
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#endif //DLMS_IGNORE_HDLC
    if (bb_isAttached(&settings->receivedData) && settings->receivedData.size + size > bb_getCapacity(&settings->receivedData))
    {
#ifdef DLMS_DEBUG
        svr_notifyTrace("svr_handleRequest2 bb_isAttached failed. ", -1);
#endif //DLMS_DEBUG
#ifndef DLMS_IGNORE_HDLC
        //Send U-Frame Frame Reject if we have received more data that can fit to one HDLC frame.
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
        {
            ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
            settings->receivedData.position = settings->receivedData.size = 0;
            reply_clear2(&settings->info, 1);
        }
        else
        {
            ret = 0;
        }
#else
        ret = 0;
#endif //DLMS_IGNORE_HDLC
        return ret;
    }
    bb_set(&settings->receivedData, buff, size);
    first = settings->base.serverAddress == 0
        && settings->base.clientAddress == 0;
    if ((ret = dlms_getData2(&settings->base, &settings->receivedData, &settings->info, first)) != 0)
    {
#ifdef DLMS_DEBUG
        svr_notifyTrace("svr_handleRequest2 dlms_getData2 failed. ", ret);
#endif //DLMS_DEBUG
        if (ret == DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Invalid server address. ", -1);
#endif //DLMS_DEBUG
            return 0;
        }
        else if (ret == DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Invalid client address. ", -1);
#endif //DLMS_DEBUG
            if (settings->info.preEstablished)
            {
                svr_disconnected(settings);
                svr_reset(settings);
                first = 1;
                settings->receivedData.position = 0;
                if ((ret = dlms_getData2(&settings->base, &settings->receivedData, &settings->info, first)) != 0)
                {
                    settings->receivedData.position = settings->receivedData.size = 0;
                    return ret;
                }
            }
#ifndef DLMS_IGNORE_HDLC
            else if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
                (settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
            {
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
                settings->receivedData.position = settings->receivedData.size = 0;
                return ret;
            }
#endif //DLMS_IGNORE_HDLC
            settings->receivedData.position = settings->receivedData.size = 0;
            reply_clear2(&settings->info, 1);
            return 0;
        }
        else if (ret == DLMS_ERROR_CODE_INVALID_FRAME_NUMBER)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Invalid frame number. ", -1);
#endif //DLMS_DEBUG
#ifndef DLMS_IGNORE_HDLC
            if ((settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
            {
                settings->dataReceived = time_elapsed();
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
                settings->receivedData.position = settings->receivedData.size = 0;
                return ret;
            }
#endif //DLMS_IGNORE_HDLC
            settings->receivedData.position = settings->receivedData.size = 0;
            reply_clear2(&settings->info, 1);
            return 0;
        }
        else
        {
#ifndef DLMS_IGNORE_HDLC
            if (ret != DLMS_ERROR_CODE_WRONG_CRC && settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
                (settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
            {
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
                settings->receivedData.position = settings->receivedData.size = 0;
                return ret;
            }
#endif //DLMS_IGNORE_HDLC
            settings->receivedData.position = settings->receivedData.size = 0;
            reply_clear2(&settings->info, 1);
            return 0;
        }
    }
    // If all data is not received yet.
    if (!settings->info.complete)
    {
        return 0;
    }
    bb_clear(&settings->receivedData);
    if (settings->info.command == DLMS_COMMAND_DISC && (settings->base.connected & DLMS_CONNECTION_STATE_HDLC) == 0)
    {
#ifdef DLMS_DEBUG
        svr_notifyTrace("Disconnecting from the meter. ", -1);
#endif //DLMS_DEBUG
#ifndef DLMS_IGNORE_HDLC
        ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_DISCONNECT_MODE, NULL, reply);
#endif //DLMS_IGNORE_HDLC
        reply_clear2(&settings->info, 1);
        return ret;
    }

    if (first || settings->info.command == DLMS_COMMAND_SNRM ||
        (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER && settings->info.command == DLMS_COMMAND_AARQ))
    {
#ifndef DLMS_IGNORE_HDLC
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC && settings->info.preEstablished)
        {
            svr_disconnected(settings);
        }
#endif //DLMS_IGNORE_HDLC
        // Check is data send to this server.
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER && settings->info.command == DLMS_COMMAND_AARQ)
        {
            if (!svr_isTarget(&settings->base, settings->base.serverAddress, settings->base.clientAddress))
            {
                if ((settings->base.connected & DLMS_CONNECTION_STATE_DLMS) == 0)
                {
                    settings->base.serverAddress = settings->base.clientAddress = 0;
                }
                reply_clear2(&settings->info, 1);
                return 0;
            }
        }
#ifndef DLMS_IGNORE_HDLC
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
        {
            settings->base.connected |= DLMS_CONNECTION_STATE_HDLC;
            svr_connected(settings);
        }
#endif //DLMS_IGNORE_HDLC
    }
    // If client want next frame.
#ifndef DLMS_IGNORE_HDLC
    if ((settings->info.moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == DLMS_DATA_REQUEST_TYPES_FRAME)
    {
        settings->dataReceived = time_elapsed();
        return dlms_getHdlcFrame(&settings->base, getReceiverReady(&settings->base), NULL, reply);
    }
#endif //DLMS_IGNORE_HDLC
    // Update command if transaction and next frame is asked.
    if (settings->info.command == DLMS_COMMAND_NONE)
    {
        if (settings->transaction.command != DLMS_COMMAND_NONE)
        {
            //If client wants next PDU.
            if (settings->info.data.size == 0)
            {
                settings->info.command = settings->transaction.command;
            }
            else
            {
                settings->info.data.position = 0;
#ifndef DLMS_IGNORE_HDLC
                //Return rest of frame.
                return dlms_getHdlcFrame(&settings->base, getNextSend(&settings->base, 0), &settings->info.data, reply);
#endif //DLMS_IGNORE_HDLC
            }
        }
        else if (settings->info.data.size == 0)
        {
            settings->dataReceived = time_elapsed();
#ifndef DLMS_IGNORE_HDLC
            return dlms_getHdlcFrame(&settings->base, getKeepAlive(&settings->base), NULL, reply);
#endif //DLMS_IGNORE_HDLC
        }
    }
    //Check inactivity timeout.
#ifndef DLMS_IGNORE_HDLC
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
        settings->hdlc != NULL && settings->hdlc->inactivityTimeout != 0)
    {
        if (settings->info.command != DLMS_COMMAND_SNRM)
        {
            uint32_t elapsed = time_elapsed() - settings->dataReceived;
            elapsed /= 1000;
            //If inactivity time out is elapsed.
            if (elapsed >= settings->hdlc->inactivityTimeout)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace("Inactivity timeout. ", 0);
#endif //DLMS_DEBUG
                if (!settings->info.preEstablished)
                {
                    if (settings->info.command == DLMS_COMMAND_DISC)
                    {
                        dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_DISCONNECT_MODE, NULL, reply);
                    }
                    svr_disconnected(settings);
                    svr_reset(settings);
                    return 0;
                }
                if ((settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
                {
                    svr_disconnected(settings);
                }
            }
        }
    }
#endif // DLMS_IGNORE_IEC_HDLC_SETUP
#endif //DLMS_IGNORE_HDLC
#ifndef DLMS_IGNORE_WRAPPER
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER &&
        settings->wrapper != NULL && settings->wrapper->inactivityTimeout != 0)
    {
        if (settings->info.command != DLMS_COMMAND_AARQ)
        {
            uint32_t elapsed = time_elapsed() - settings->dataReceived;
            elapsed /= 1000;
            //If inactivity time out is elapsed.
            if (elapsed >= settings->wrapper->inactivityTimeout)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace("Inactivity timeout. ", 0);
#endif //DLMS_DEBUG
                if (!settings->info.preEstablished)
                {
                    svr_disconnected(settings);
                    svr_reset(settings);
                    return 0;
                }
                if ((settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
                {
                    svr_disconnected(settings);
                }
            }
        }
    }
#endif // DLMS_IGNORE_TCP_UDP_SETUP
#endif //DLMS_IGNORE_WRAPPER
    ret = svr_handleCommand(settings, settings->info.command, &settings->info.data, reply);
    if (ret != 0)
    {
        bb_clear(reply);
#ifndef DLMS_IGNORE_HDLC
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
        {
            if (ret == DLMS_ERROR_CODE_REJECTED)
            {
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_DISCONNECT_MODE, NULL, reply);
            }
            else
            {
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
            }
            settings->receivedData.position = settings->receivedData.size = 0;
        }
        else
#endif //DLMS_IGNORE_HDLC
        {
            ret = svr_reportError(settings, settings->info.command, DLMS_ERROR_CODE_OTHER_REASON, reply);
        }
    }
    reply_clear2(&settings->info, 0);
    settings->dataReceived = time_elapsed();
    return ret;
}

int svr_handleInactivityTimeout(
    dlmsServerSettings* settings,
    uint32_t time,
    uint32_t* next)
{
    //If connection is established.
    if (settings->info.preEstablished || (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
    {
        //Check inactivity timeout.
        unsigned char inactivity = 0;
        //Delay in seconds from last message.
        uint32_t elapsed = (time_elapsed() - settings->dataReceived);
        elapsed /= 1000;
        uint32_t timeout = 0xFFFFFFFF;
#ifndef DLMS_IGNORE_HDLC
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
            settings->hdlc != NULL && settings->hdlc->inactivityTimeout != 0)
        {
            inactivity = !(elapsed < settings->hdlc->inactivityTimeout);
            timeout = settings->hdlc->inactivityTimeout - elapsed;
        }
#endif // DLMS_IGNORE_IEC_HDLC_SETUP
#endif //DLMS_IGNORE_HDLC
#ifndef DLMS_IGNORE_WRAPPER
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER &&
            settings->wrapper != NULL && settings->wrapper->inactivityTimeout != 0)
        {
            inactivity = !(elapsed < settings->wrapper->inactivityTimeout);
            timeout = settings->wrapper->inactivityTimeout - elapsed;
        }
#endif // DLMS_IGNORE_TCP_UDP_SETUP
#endif //DLMS_IGNORE_WRAPPER
        //If inactivity timeout is elapsed.
        if (inactivity)
        {
            svr_disconnected(settings);
            svr_reset(settings);
        }
        else if (timeout != 0xFFFFFFFF)
        {
            if (*next > time + timeout)
            {
                *next = time + timeout;
            }
        }
    }
    return 0;
}

int svr_invoke(
    dlmsServerSettings* settings,
    gxObject* target,
    unsigned char index,
    uint32_t time,
    gxtime* start,
    gxtime* end,
    uint32_t* executed,
    uint32_t* next)
{
    unsigned char exec = start == NULL;
    if (!exec)
    {
        //Execute in exact time if end time is not given.
        if (end == NULL)
        {
            //Ignore deviation and status for single action script.
            exec = *executed < time && time_compareWithDiff(start, time, 0) == 0;
        }
        else if (*executed < time)
        {
            exec = time_compare2(start, time) != 1 && time_compare2(end, time) != -1;
        }
    }
    if (exec)
    {
        *executed = time;
        gxValueEventCollection args;
        gxValueEventArg* e;
#ifdef DLMS_IGNORE_MALLOC
        gxValueEventArg tmp[1];
        ve_init(&tmp[0]);
        vec_attach(&args, tmp, 1, 1);
        e = &tmp[0];
#else
        e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
        ve_init(e);
        vec_init(&args);
        vec_push(&args, e);
#endif //DLMS_IGNORE_MALLOC
        e->target = target;
        e->index = index;
        svr_preAction(&settings->base, &args);
        if (!e->handled)
        {
            cosem_invoke(settings, e);
            svr_postAction(&settings->base, &args);
        }
        vec_empty(&args);
        //Increase time by one second so next scheduled date is retreaved.
        ++time;
    }
    if (start != NULL)
    {
        uint32_t tmp = time_getNextScheduledDate(time, start);
        if (tmp < *next)
        {
            *next = tmp;
        }
    }
    return 0;
}

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int svr_handleProfileGeneric(
    dlmsServerSettings* settings,
    gxProfileGeneric* object,
    uint32_t time,
    uint32_t* next)
{
    int ret = 0;
    if (object->capturePeriod != 0)
    {
        //Get seconds.
        uint32_t tm = time % 60L;
        //Time where seconds part is zero.
        uint32_t tm2 = time - tm;
        tm = time % object->capturePeriod;
        if (tm == 0)
        {
            if (*next == -1 || *next > time + object->capturePeriod)
            {
                *next = time + object->capturePeriod;
            }
            ret = svr_invoke(
                settings,
                (gxObject*)object,
                2,
                time,
                NULL,
                NULL,
                &object->executedTime,
                next);
        }
        else if (tm2 + object->capturePeriod < *next)
        {
            tm = time - tm2;
            tm %= object->capturePeriod;
            uint32_t offset = object->capturePeriod - tm;
            *next = time + offset;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC


#if !defined(DLMS_IGNORE_ACTION_SCHEDULE) && !defined(DLMS_IGNORE_OBJECT_POINTERS)

int svr_handleSingleActionSchedule(
    dlmsServerSettings* settings,
    gxActionSchedule* object,
    uint32_t time,
    uint32_t* next)
{
    gxtime* s;
    int ret = 0;
    int pos;
    for (pos = 0; pos != object->executionTime.size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&s)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&s, sizeof(gxtime))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
        if (object->executedScript != NULL)
        {
            gxScript* it;
            gxScriptAction* a;
            int posS;
            if (settings->defaultClock != NULL)
            {
               s->deviation = settings->defaultClock->timeZone;
               s->status = settings->defaultClock->status;
            }
            for (posS = 0; posS != object->executedScript->scripts.size; ++posS)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->executedScript->scripts, posS, (void**)&it, sizeof(gxScript))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->executedScript->scripts, posS, (void**)&it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if (it->id == object->executedScriptSelector)
                {
                    int posA;
                    for (posA = 0; posA != it->actions.size; ++posA)
                    {
#ifdef DLMS_IGNORE_MALLOC
                        if ((ret = arr_getByIndex(&it->actions, posA, (void**)&a, sizeof(gxScriptAction))) != 0)
                        {
                            break;
                        }
#else
                        if ((ret = arr_getByIndex(&it->actions, posA, (void**)&a)) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_MALLOC
                        if ((ret = svr_invoke(
                            settings,
                            (gxObject*)a->target,
                            a->index,
                            time,
                            s,
                            NULL,
                            &object->executedTime,
                            next)) != 0)
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    return ret;
}
#endif //!defined(DLMS_IGNORE_ACTION_SCHEDULE) && !defined(DLMS_IGNORE_OBJECT_POINTERS)

#ifndef DLMS_IGNORE_PUSH_SETUP

int svr_handlePushSetup(
    dlmsServerSettings* settings,
    gxPushSetup* object,
    uint32_t time,
    uint32_t* next)
{
    gxtime* s, * e;
#ifndef DLMS_IGNORE_MALLOC
    gxKey* k;
#else
    gxTimePair* k;
#endif //DLMS_IGNORE_MALLOC
    int ret = 0;
    int pos;
    for (pos = 0; pos != object->communicationWindow.size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&k)) != 0)
        {
            break;
        }
        s = (gxtime*)k->key;
        e = (gxtime*)k->value;
#else
        if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&k, sizeof(gxTimePair))) != 0)
        {
            break;
        }
        s = &k->first;
        e = &k->second;
#endif //DLMS_IGNORE_MALLOC
        if ((ret = svr_invoke(
            settings,
            (gxObject*)object,
            1,
            time,
            s,
            e,
            &object->executedTime,
            next)) != 0)
        {
            //Save infor that invoke failed.
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_AUTO_CONNECT
int svr_handleAutoConnect(
    dlmsServerSettings* settings,
    gxAutoConnect* object,
    uint32_t time,
    uint32_t* next)
{
    gxtime* s, * e;
#ifndef DLMS_IGNORE_MALLOC
    gxKey* k;
#else
    gxTimePair* k;
#endif //DLMS_IGNORE_MALLOC
    int ret = 0;
    int pos;
    for (pos = 0; pos != object->callingWindow.size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k)) != 0)
        {
            break;
        }
        s = (gxtime*)k->key;
        e = (gxtime*)k->value;
#else
        if ((ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k, sizeof(gxTimePair))) != 0)
        {
            break;
        }
        s = &k->first;
        e = &k->second;
#endif //DLMS_IGNORE_MALLOC
        if ((ret = svr_invoke(
            settings,
            (gxObject*)object,
            1,
            time,
            s,
            e,
            &object->executedTime,
            next)) != 0)
        {
            break;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_CONNECT

int svr_run(
    dlmsServerSettings* settings,
    uint32_t time,
    uint32_t* next)
{
    uint16_t pos;
    int ret = 0;
    gxObject* obj;
    *next = -1;
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    //Push objects.
    for (pos = 0; pos != settings->base.objects.size; ++pos)
    {
        if ((ret = oa_getByIndex(&settings->base.objects, pos, &obj)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            svr_handleProfileGeneric(settings, (gxProfileGeneric*)obj, time, next);
        }
    }
#endif //DLMS_IGNORE_PROFILE_GENERIC

#if !defined(DLMS_IGNORE_ACTION_SCHEDULE) && !defined(DLMS_IGNORE_OBJECT_POINTERS)
    //Single action schedules.
    for (pos = 0; pos != settings->base.objects.size; ++pos)
    {
        if ((ret = oa_getByIndex(&settings->base.objects, pos, &obj)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj->objectType == DLMS_OBJECT_TYPE_ACTION_SCHEDULE)
        {
            svr_handleSingleActionSchedule(settings, (gxActionSchedule*)obj, time, next);
        }
    }
#endif //!defined(DLMS_IGNORE_ACTION_SCHEDULE) && !defined(DLMS_IGNORE_OBJECT_POINTERS)

#ifndef DLMS_IGNORE_PUSH_SETUP
    //Push objects.
    for (pos = 0; pos != settings->base.objects.size; ++pos)
    {
        if ((ret = oa_getByIndex(&settings->base.objects, pos, &obj)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP)
        {
            svr_handlePushSetup(settings, (gxPushSetup*)obj, time, next);
        }
    }
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_AUTO_CONNECT
    //Get auto connect objects.
    for (pos = 0; pos != settings->base.objects.size; ++pos)
    {
        if ((ret = oa_getByIndex(&settings->base.objects, pos, &obj)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj->objectType == DLMS_OBJECT_TYPE_AUTO_CONNECT)
        {
            //Handle calling window execution times. Ignore errors.
            svr_handleAutoConnect(settings, (gxAutoConnect*)obj, time, next);
        }
    }
#endif //DLMS_IGNORE_AUTO_CONNECT
    return svr_handleInactivityTimeout(settings, time, next);
}
#endif //DLMS_IGNORE_SERVER
