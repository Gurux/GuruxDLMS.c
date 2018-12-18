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

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#if _MSC_VER > 1400
#include <stdlib.h>  
#include <crtdbg.h> 
#endif
#endif
#include "../include/gxmem.h"
#include "../include/server.h"
#include "../include/cosem.h"
#include "../include/enums.h"
#include "../include/gxset.h"
#include "../include/gxget.h"
#include "../include/gxinvoke.h"
#include "../include/serverevents.h"
#include "../include/gxignore.h"
#ifndef DLMS_IGNORE_SERVER

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

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
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
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

//Copy association view.
void svr_copyAssociationView(objectArray *target, objectArray* source)
{
    unsigned short cnt = 0, pos;
    oa_clear(target);
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

int svr_initialize(
    dlmsServerSettings* settings)
{
    unsigned short pos;
    int ret;
    gxObject* associationObject = NULL, *it;
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
        if (it->logicalName == NULL)
        {
            //Invalid Logical Name.
            return DLMS_ERROR_CODE_INVALID_LOGICAL_NAME;
        }
        if (it->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
#ifndef DLMS_IGNORE_PROFILE_GENERIC
            gxProfileGeneric* pg = (gxProfileGeneric*)it;
            if (pg->profileEntries < 1)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
#endif //DLMS_IGNORE_PROFILE_GENERIC
        }
        else if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME
            && !settings->base.useLogicalNameReferencing)
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            objectArray* list = &((gxAssociationShortName*)it)->objectList;
            if (list->size == 0)
            {
                svr_copyAssociationView(list, &settings->base.objects);
            }
            associationObject = it;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
        else if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME
            && settings->base.useLogicalNameReferencing)
        {
            gxAssociationLogicalName *ln = (gxAssociationLogicalName*)it;
            objectArray* list = &ln->objectList;
            if (list->size == 0)
            {
                svr_copyAssociationView(list, &settings->base.objects);
            }
            associationObject = it;
            ln->xDLMSContextInfo.maxReceivePduSize = ln->xDLMSContextInfo.maxSendPpuSize = settings->base.maxServerPDUSize;
            ln->xDLMSContextInfo.conformance = settings->base.proposedConformance;
        }
    }
    if (associationObject == NULL)
    {
        objectArray* list;
        if (settings->base.useLogicalNameReferencing)
        {
            gxAssociationLogicalName* ln;
            if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, (gxObject**)&ln)) != 0)
            {
                return ret;
            }
            ln->xDLMSContextInfo.maxReceivePduSize = ln->xDLMSContextInfo.maxSendPpuSize = settings->base.maxServerPDUSize;
            ln->xDLMSContextInfo.conformance = settings->base.proposedConformance;
            list = &((gxAssociationLogicalName*)ln)->objectList;
            oa_push(&settings->base.objects, (gxObject*)ln);
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
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
    }
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    // Arrange items by Short Name.
    if (!settings->base.useLogicalNameReferencing)
    {
        return svr_updateShortNames(settings, 0);
    }
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    return 0;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int svr_updateShortNames(
    dlmsServerSettings* settings,
    unsigned char force)
{
    gxObject *it;
    unsigned short sn = 0xA0;
    unsigned short pos;
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
                sn += (unsigned short)(8 * obj_attributeCount(it));
            }
        }
        else
        {
            sn = (unsigned short)(it->shortName + (8 * obj_attributeCount(it)));
        }
    }
    return 0;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME


void svr_setInitialize(dlmsServerSettings* settings)
{
    if (settings->base.protocolVersion != NULL)
    {
        gxfree(settings->base.protocolVersion);
        settings->base.protocolVersion = NULL;
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->base.cipher.dedicatedKey != NULL)
    {
        bb_clear(settings->base.cipher.dedicatedKey);
        settings->base.cipher.dedicatedKey = NULL;
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    trans_clear(&settings->transaction);
    settings->base.blockIndex = 1;
    settings->base.count = 0;
    settings->base.index = 0;
    settings->base.connected = DLMS_CONNECTION_STATE_NONE;
    settings->base.authentication = DLMS_AUTHENTICATION_NONE;
    settings->base.isAuthenticationRequired = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
    settings->base.cipher.invocationCounter = 1;
    settings->base.cipher.security = DLMS_SECURITY_NONE;
#endif //DLMS_IGNORE_HIGH_GMAC
    bb_clear(&settings->base.ctoSChallenge);
    bb_clear(&settings->base.stoCChallenge);
    bb_clear(&settings->base.sourceSystemTitle);
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
    int ret;
    gxByteBuffer error;
    DLMS_ASSOCIATION_RESULT result;
    DLMS_SOURCE_DIAGNOSTIC diagnostic;
    // Reset settings for wrapper.
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        svr_setInitialize(settings);
    }
    //If client is not called SNRM.
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC && (settings->base.connected & DLMS_CONNECTION_STATE_HDLC) == 0)
    {
        return DLMS_ERROR_CODE_REJECTED;
    }

    ret = apdu_parsePDU(&settings->base, data, &result, &diagnostic);
    bb_clear(data);
    bb_init(&error);
    if (ret == 0 && result == DLMS_ASSOCIATION_RESULT_ACCEPTED)
    {
        if (settings->base.dlmsVersionNumber < 6)
        {
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
            result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            diagnostic = DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
            bb_setUInt8(&error, 0xE);
            bb_setUInt8(&error, DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR);
            bb_setUInt8(&error, DLMS_SERVICE_ERROR_INITIATE);
            bb_setUInt8(&error, DLMS_INITIATE_INCOMPATIBLE_CONFORMANCE);
        }
        else if (diagnostic != DLMS_SOURCE_DIAGNOSTIC_NONE)
        {
            result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            diagnostic = DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
        }
        else
        {
            diagnostic = svr_validateAuthentication(
                settings,
                settings->base.authentication,
                &settings->base.password);
            if (diagnostic != DLMS_SOURCE_DIAGNOSTIC_NONE)
            {
                result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            }
            else if (settings->base.authentication > DLMS_AUTHENTICATION_LOW)
            {
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
                bb_clear(&error);
                return ret;
            }
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
                    it->associationStatus = DLMS_ASSOCIATION_STATUS_ASSOCIATION_PENDING;
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
        result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
        diagnostic = DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
         bb_setUInt8(&error, 0xE);
        bb_setUInt8(&error, DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR);
        bb_setUInt8(&error, DLMS_SERVICE_ERROR_INITIATE);
        bb_setUInt8(&error, DLMS_INITIATE_OTHER);
    }

    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        dlms_addLLCBytes(&settings->base, data);
    }
    ret = apdu_generateAARE(&settings->base, data, result, diagnostic, &error, NULL);
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
    unsigned short serverAddress, clientAddress;
    if ((ret = dlms_parseSnrmUaResponse(&settings->base, data)) != 0)
    {
        return ret;
    }
    bb_clear(data);
    serverAddress = settings->base.serverAddress;
    clientAddress = settings->base.clientAddress;
    svr_reset(settings);
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
    int len;
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        bb_setUInt8(data, 0x63);
        bb_setUInt8(data, 0x0);
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
        bb_setUInt8ByIndex(data, 2, (unsigned char)len);
    }
    return 0;
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
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_READ_REQUEST:
        cmd = DLMS_COMMAND_READ_RESPONSE;
        break;
    case DLMS_COMMAND_WRITE_REQUEST:
        cmd = DLMS_COMMAND_WRITE_RESPONSE;
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
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
        // Return HW error and close connection.
        cmd = DLMS_COMMAND_NONE;
        break;
    }
    bb_init(&data);
    if (settings->base.useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, &settings->base, 0, cmd, 1,
            NULL, NULL, error);
        ret = dlms_getLNPdu(&p, &data);
        if (ret != 0)
        {
            bb_clear(&data);
            return ret;
        }
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        gxSNParameters p;
        gxByteBuffer bb;
        bb_init(&bb);
        bb_setUInt8(&bb, error);
        params_initSN(&p, &settings->base, cmd, 1, 1, NULL, &bb);
        p.lastBlock = settings->base.index == settings->base.count;
        ret = dlms_getSNPdu(&p, &data);
        bb_clear(&bb);
        if (ret != 0)
        {
            bb_clear(&data);
            return ret;
        }
#else 
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        ret = dlms_getHdlcFrame(&settings->base, 0, &data, reply);
    }
#ifndef DLMS_IGNORE_WRAPPER
    else if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        ret = dlms_getWrapperFrame(&settings->base, &data, reply);
    }
#endif //DLMS_IGNORE_WRAPPER
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_clear(&data);
    return ret;
}

int svr_handleSetRequest2(
    dlmsServerSettings* settings,
    gxByteBuffer *data,
    unsigned char type,
    gxLNParameters *p)
{
    gxValueEventArg* e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    gxValueEventCollection list;
    int ret;
    DLMS_OBJECT_TYPE ci;
    unsigned char ch;
    unsigned short tmp;
    unsigned char * ln = NULL;
    ve_init(e);
    vec_init(&list);
    vec_push(&list, e);
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
    if (type == 2)
    {
        unsigned long blockNumber;
        unsigned short size;
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
            p->status = DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
            return 0;
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
            p->status = DLMS_ERROR_CODE_BLOCK_UNAVAILABLE;
            return 0;
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
            bb_clear(data);
            if (ret == 0)
            {
                // If target is unknown.
                if (e->target == NULL)
                {
                    // Device reports a undefined object.
                    p->status = DLMS_ERROR_CODE_UNAVAILABLE_OBJECT;
                }
                else
                {
                    DLMS_ACCESS_MODE am = svr_getAttributeAccess(&settings->base, e->target, e->index);
                    // If write is denied.
                    if (am != DLMS_ACCESS_MODE_WRITE && am != DLMS_ACCESS_MODE_READ_WRITE)
                    {
                        //Read Write denied.
                        p->status = DLMS_ERROR_CODE_READ_WRITE_DENIED;
                    }
                    else
                    {
                        svr_preWrite(&settings->base, &list);
                        if (p->multipleBlocks)
                        {
                            bb_clear(&settings->transaction.data);
                            vec_clear(&settings->transaction.targets);
                            settings->transaction.targets = list;
                            var_clear(&e->value);
                            vec_init(&list);
                            settings->transaction.command = DLMS_COMMAND_GET_REQUEST;
                            if (!bb_isAttached(data))
                            {
                                bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
                            }
                        }
                        if (e->error != 0)
                        {
                            p->status = e->error;
                        }
                        else if (!e->handled && !p->multipleBlocks)
                        {
                            ret = cosem_setValue(&settings->base, e);
                            svr_postWrite(&settings->base, &list);
                            if (e->error != 0)
                            {
                                p->status = e->error;
                            }
                        }
                    }
                }
            }
        }
    }
    vec_clear(&list);
    return ret;
}

int svr_hanleSetRequestWithDataBlock(
    dlmsServerSettings* settings,
    gxByteBuffer *data,
    gxLNParameters *p)
{
    int ret;
    unsigned long blockNumber;
    unsigned short size;
    unsigned char ch;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    p->multipleBlocks = ch == 0;
    if ((ret = bb_getUInt32(data, &blockNumber)) != 0)
    {
        return ret;
    }
    if (blockNumber != settings->base.blockIndex)
    {
        p->status = DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
    }
    else
    {
        ++settings->base.blockIndex;
        if (hlp_getObjectCount2(data, &size) != 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if (size != data->size - data->position)
        {
            p->status = DLMS_ERROR_CODE_BLOCK_UNAVAILABLE;
        }
        bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
        // If all data is received.
        if (!p->multipleBlocks)
        {
            gxDataInfo di;
            gxValueEventArg *e = settings->transaction.targets.data[0];
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
    p->multipleBlocks = 1;
    return 0;
}

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
    gxByteBuffer * data)
{
    bb_clear(data);
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        dlms_addLLCBytes(&settings->base, data);
    }
    bb_setUInt8(data, DLMS_COMMAND_CONFIRMED_SERVICE_ERROR);
    bb_setUInt8(data, service);
    bb_setUInt8(data, type);
    bb_setUInt8(data, code);
    return 0;
}

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
    params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_SET_RESPONSE, type, NULL, data, 0);
    if (type == DLMS_SET_COMMAND_TYPE_NORMAL || type == DLMS_SET_COMMAND_TYPE_FIRST_DATABLOCK)
    {
        ret = svr_handleSetRequest2(settings, data, type, &p);
    }
    else if (type == DLMS_SET_COMMAND_TYPE_WITH_DATABLOCK)
    {
        // Set Request With Data Block
        ret = svr_hanleSetRequestWithDataBlock(settings, data, &p);
    }
    else
    {
        // Access Error : Device reports Read-Write denied.
        resetBlockIndex(&settings->base);
        p.status = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return dlms_getLNPdu(&p, data);
}


int svr_getRequestNormal(
    dlmsServerSettings* settings,
    gxByteBuffer *data,
    unsigned char invokeId)
{
    gxValueEventCollection list;
    unsigned short ci;
    gxValueEventArg *e;
    unsigned char index;
    int ret;
    unsigned char *ln;
    DLMS_ERROR_CODE status = DLMS_ERROR_CODE_OK;
    settings->base.count = 0;
    settings->base.index = 0;
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

    e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    e->value.byteArr = data;
    e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
    e->index = index;
    vec_init(&list);
    vec_push(&list, e);
    // Access selection
    unsigned char selection;
    if ((ret = bb_getUInt8(data, &selection)) != 0)
    {
        vec_init(&list);
        return ret;
    }
    if (selection != 0)
    {
        gxDataInfo di;
        di_init(&di);
        if ((ret = bb_getUInt8(data, &e->selector)) != 0 ||
            (ret = dlms_getData(data, &di, &e->parameters)) != 0)
        {
            vec_init(&list);
            return ret;
        }
    }
    bb_clear(data);
    if ((ret = oa_findByLN(&settings->base.objects, (DLMS_OBJECT_TYPE)ci, ln, &e->target)) != 0)
    {
        vec_clear(&list);
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
                    return ret;
                }
            }
            else
            {
                svr_preRead(&settings->base, &list);
                if (!e->handled)
                {
                    settings->base.count = e->transactionEndIndex - e->transactionStartIndex;
                    if ((ret = cosem_getValue(&settings->base, e)) != 0)
                    {
                        status = DLMS_ERROR_CODE_HARDWARE_FAULT;
                    }
                    svr_postRead(&settings->base, &list);
                }
            }
            if (status == 0)
            {
                status = e->error;
            }
            if (status == 0)
            {
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
    gxLNParameters p;
    params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 1, NULL, data, status);
    ret = dlms_getLNPdu(&p, data);
    if (settings->base.count != settings->base.index || data->size > settings->base.maxPduSize)
    {
        e->transaction = 1;
        trans_clear(&settings->transaction);
        settings->transaction.targets = list;
        vec_init(&list);
        settings->transaction.command = DLMS_COMMAND_GET_REQUEST;
        if (!bb_isAttached(data))
        {
            bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
        }
    }
    vec_clear(&list);
    return ret;
}

int svr_getRequestNextDataBlock(
    dlmsServerSettings* settings,
    gxByteBuffer *data,
    unsigned char invokeId)
{
    gxValueEventArg* e;
    gxLNParameters p;
    unsigned char moreData;
    int ret;
    unsigned long index, pos;
    // Get block index.
    if ((ret = bb_getUInt32(data, &index)) != 0)
    {
        return ret;
    }
    bb_clear(data);
    if (index != settings->base.blockIndex)
    {
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 2,
            NULL, NULL, DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID);
        return dlms_getLNPdu(&p, data);
    }
    else
    {
        ++settings->base.blockIndex;
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 2, NULL, data, DLMS_ERROR_CODE_OK);
        // If transaction is not in progress.
        if (settings->transaction.command == DLMS_COMMAND_NONE)
        {
            p.status = DLMS_ERROR_CODE_NO_LONG_GET_OR_READ_IN_PROGRESS;
        }
        else
        {
            bb_set2(data, &settings->transaction.data, 0, settings->transaction.data.size);
            bb_clear(&settings->transaction.data);
            moreData = settings->base.index != settings->base.count;
            if (moreData)
            {
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
                    moreData = settings->base.index != settings->base.count;
                }
            }
            p.multipleBlocks = 1;
            ret = dlms_getLNPdu(&p, data);
            if (moreData || data->size - data->position != 0)
            {
                //Append data to transaction data if buffer is not static.
                if (!bb_isAttached(data))
                {
                    bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
                }
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
    gxByteBuffer *data,
    unsigned char invokeId)
{
    gxValueEventArg* e;
    DLMS_OBJECT_TYPE ci;
    gxLNParameters p;
    gxValueEventCollection list;
    int ret;
    unsigned char attributeIndex;
    unsigned short id;
    unsigned short pos, cnt;
    unsigned char *ln;
    if (hlp_getObjectCount2(data, &cnt) != 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    vec_init(&list);
    for (pos = 0; pos != cnt; ++pos)
    {
        if ((ret = bb_getUInt16(data, &id)) != 0)
        {
            return ret;
        }
        ci = (DLMS_OBJECT_TYPE)id;
        ln = data->data + data->position;
        data->position += 6;
        if ((ret = bb_getUInt8(data, &attributeIndex)) != 0)
        {
            return ret;
        }
        e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
        ve_init(e);
        e->index = attributeIndex;
        vec_push(&list, e);
        if ((ret = oa_findByLN(&settings->base.objects, ci, ln, &e->target)) != 0)
        {
            vec_clear(&list);
            return ret;
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
                    return ret;
                }
                if (selection != 0)
                {
                    gxDataInfo di;
                    di_init(&di);
                    if ((ret = bb_getUInt8(data, &e->selector)) != 0)
                    {
                        return ret;
                    }
                    if ((ret = dlms_getData(data, &di, &e->parameters)) != 0)
                    {
                        return ret;
                    }
                }
            }
        }
    }
    bb_clear(data);
    svr_preRead(&settings->base, &list);
    hlp_setObjectCount(cnt, data);
    for (pos = 0; pos != list.size; ++pos)
    {
        if ((ret = vec_getByIndex(&list, pos, &e)) != 0)
        {
            break;
        }
        if (!e->handled)
        {
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
            unsigned long pos2 = data->size;
#else 
            unsigned short pos2 = data->size;
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
        /*
        if ((ret = dlms_setData(data, e->value.vt, &e->value)) != 0)
        {
            break;
        }
        */
        if (settings->base.index != settings->base.count)
        {
            trans_clear(&settings->transaction);
            settings->transaction.targets = list;
            var_clear(&e->value);
            vec_init(&list);
            settings->transaction.command = DLMS_COMMAND_GET_REQUEST;
            if (!bb_isAttached(data))
            {
                bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
            }
        }
    }
    svr_postRead(&settings->base, &list);
    params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 3, NULL, data, 0xFF);
    ret = dlms_getLNPdu(&p, data);
    vec_clear(&list);
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
        // Access Error : Device reports Read-Write denied.
        gxLNParameters p;
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_GET_RESPONSE, 1, NULL, data, DLMS_ERROR_CODE_READ_WRITE_DENIED);
        bb_clear(data);
        ret = dlms_getLNPdu(&p, data);
    }
    if (ret != 0)
    {
        return svr_generateConfirmedServiceError(
            settings,
            2,
            DLMS_SERVICE_ERROR_SERVICE, DLMS_SERVICE_UNSUPPORTED,
            data);
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME

/**
 * Find Short Name object.
 *
 * @param sn
 */
int svr_findSNObject(dlmsServerSettings* settings, int sn, gxSNInfo* i)
{
    unsigned short pos;
    int ret;
    gxObject *it;
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
    gxValueEventCollection *list,
    gxValueEventCollection *reads,
    gxValueEventCollection *actions)
{
    gxSNInfo info;
    gxValueEventArg *e;
    int ret;
    gxDataInfo di;
    unsigned short sn;
    di_init(&di);
    if ((ret = bb_getUInt16(data, &sn)) != 0)
    {
        return ret;
    }
    if ((ret = svr_findSNObject(settings, sn, &info)) != 0)
    {
        return ret;
    }
    e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    e->target = info.item;
    e->index = info.index;
    e->action = info.action;
    vec_push(list, e);
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
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

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
    gxValueEventCollection *list,
    gxByteBuffer *data,
    DLMS_SINGLE_READ_RESPONSE *type,
    unsigned char* multipleBlocks)
{
    int ret;
    gxValueEventArg *e;
    unsigned long pos;
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long statusindex;
#else 
    unsigned short statusindex;
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
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME

int svr_handleReadBlockNumberAccess(
    dlmsServerSettings* settings,
    gxByteBuffer *data)
{
    gxSNParameters p;
    gxValueEventArg *e;
    unsigned short pos, blockNumber;
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
            DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL);
        ret = dlms_getSNPdu(&p, data);
        resetBlockIndex(&settings->base);
        bb_clear(&bb);
        return ret;
    }
    if (settings->base.index != settings->base.count
        && settings->transaction.data.size < settings->base.maxPduSize)
    {
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
        }
        vec_empty(&reads);
        vec_empty(&actions);
    }
    if (ret == 0)
    {
        ++settings->base.blockIndex;
        params_initSN(&p, &settings->base, DLMS_COMMAND_READ_RESPONSE, 1,
            DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT, NULL, data);
        p.multipleBlocks = 1;
        p.lastBlock = settings->base.index == settings->base.count;
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
    gxByteBuffer *data,
    int cnt)
{
    gxSNParameters p;
    int ret;
    unsigned short size;
    unsigned short blockNumber;
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
        params_initSN(&p, &settings->base, command, 1, DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL);
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
            DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL);
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
        params_initSN(&p, &settings->base, command, cnt, type, NULL, &bb);
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
        DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR, &bb, NULL);
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
    unsigned short pos, cnt = 0;
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
                requestType, NULL, data);
            p.multipleBlocks = multipleBlocks;
            p.lastBlock = settings->base.index == settings->base.count;
            ret = dlms_getSNPdu(&p, data);
            if (ret == 0)
            {
                if (settings->base.count != settings->base.index)
                {
                    settings->transaction.targets = list;
                    vec_init(&list);
                    settings->transaction.command = DLMS_COMMAND_READ_REQUEST;
                    if (!bb_isAttached(data))
                    {
                        bb_set2(&settings->transaction.data, data, data->position, data->size - data->position);
                    }
                }
                else
                {
                    // If all data is transfered.
                    settings->base.index = 0;
                    settings->base.count = 0;
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
    gxValueEventArg *e;
    unsigned char ch;
    unsigned short sn;
    unsigned short cnt, pos;
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
        params_initSN(&p, &settings->base, DLMS_COMMAND_WRITE_RESPONSE, cnt, 0xFF, NULL, &bb);
        p.lastBlock = settings->base.index == settings->base.count;
        ret = dlms_getSNPdu(&p, data);
        bb_clear(&bb);
    }
    // If all data is transfered.
    settings->base.index = 0;
    settings->base.count = 0;
    bb_clear(&results);
    vec_clear(&arr);
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

/**
* Handle action request.
*
* @param reply
*            Received data from the client.
* @return Reply.
*/
int svr_handleMethodRequest(
    dlmsServerSettings* settings,
    gxByteBuffer *data)
{
    DLMS_OBJECT_TYPE ci;
    gxValueEventArg *e;
    unsigned char * ln;
    DLMS_ERROR_CODE error = DLMS_ERROR_CODE_OK;
    int ret;
    unsigned char invokeId, ch, id;
    unsigned short tmp;
    gxValueEventCollection arr;

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

    vec_init(&arr);
    e = (gxValueEventArg*)gxmalloc(sizeof(gxValueEventArg));
    ve_init(e);
    e->index = id;
    vec_push(&arr, e);
    if (ch != 0)
    {
        gxDataInfo di;
        di_init(&di);
        if ((ret = dlms_getData(data, &di, &e->parameters)) != 0)
        {
            vec_clear(&arr);
            return ret;
        }
    }

    if ((ret = oa_findByLN(&settings->base.objects, ci, ln, &e->target)) != 0)
    {
        vec_clear(&arr);
        return ret;
    }
    if (e->target == NULL)
    {
        ret = svr_findObject(&settings->base, ci, 0, ln, e);
    }
    bb_clear(data);
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
            svr_preAction(&settings->base, &arr);
            if (!e->handled)
            {
                if ((ret = cosem_invoke(settings, e)) != 0)
                {
                    e->error = (DLMS_ERROR_CODE)ret;
                    ret = 0;
                }
                svr_postAction(&settings->base, &arr);
            }
        }
        // Set default action reply if not given.
        if (e->error == DLMS_ERROR_CODE_OK)
        {
            // Add return parameters
            bb_insertUInt8(data, 0, 1);
            //Add parameters error code.
            bb_insertUInt8(data, 1, 0);
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
            error = e->error;
            bb_clear(data);
            bb_setUInt8(data, 0);
        }
    }
    if (ret == 0)
    {
        gxLNParameters p;
        params_initLN(&p, &settings->base, invokeId, DLMS_COMMAND_METHOD_RESPONSE, 1, NULL, data, error);
        ret = dlms_getLNPdu(&p, data);
        // If High level authentication fails.
        if (e->target->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME && id == 1)
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
    vec_clear(&arr);
    return ret;
}

/**
* Handles release request.
*
* @param settings
*            DLMS settings.
* @param data
*            Received data.
*/
int svr_handleReleaseRequest(dlmsServerSettings* settings, gxByteBuffer* data) {
    bb_clear(data);
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        dlms_addLLCBytes(&settings->base, data);
    }
    int ret;
    gxByteBuffer tmp;
    if ((ret = bb_init(&tmp)) != 0)
    {
        return ret;
    }
    if ((ret = apdu_getUserInformation(&settings->base, &tmp)) != 0)
    {
        bb_clear(&tmp);
        return ret;
    }
    bb_setUInt8(data, 0x63);
    //Len.
    bb_setUInt8(data, (unsigned char)(tmp.size + 3));
    bb_setUInt8(data, 0x80);
    bb_setUInt8(data, 0x01);
    bb_setUInt8(data, 0x00);
    bb_setUInt8(data, 0xBE);
    bb_setUInt8(data, (unsigned char)(tmp.size + 1));
    bb_setUInt8(data, 4);
    bb_setUInt8(data, (unsigned char)tmp.size);
    bb_set(data, tmp.data, tmp.size);
    bb_clear(&tmp);
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
    case DLMS_COMMAND_SET_REQUEST:
        ret = svr_handleSetRequest(settings, data);
        break;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_WRITE_REQUEST:
        ret = svr_handleWriteRequest(settings, data);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
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
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
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
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_METHOD_REQUEST:
        ret = svr_handleMethodRequest(settings, data);
        break;
    case DLMS_COMMAND_SNRM:
        ret = svr_handleSnrmRequest(settings, data);
        settings->base.connected = DLMS_CONNECTION_STATE_HDLC;
        frame = DLMS_COMMAND_UA;
        break;
    case DLMS_COMMAND_AARQ:
        ret = svr_HandleAarqRequest(settings, data);
        if (ret == 0 && settings->base.authentication < DLMS_AUTHENTICATION_HIGH)
        {
            settings->base.connected |= DLMS_CONNECTION_STATE_DLMS;
            svr_connected(settings);
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
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    if (ret != 0)
    {
        return ret;
    }
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        ret = dlms_getHdlcFrame(&settings->base, frame, data, reply);
    }
#ifndef DLMS_IGNORE_WRAPPER
    else if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        ret = dlms_getWrapperFrame(&settings->base, data, reply);
    }
#endif //DLMS_IGNORE_WRAPPER
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (cmd == DLMS_COMMAND_DISC ||
        (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER && cmd == DLMS_COMMAND_RELEASE_REQUEST))
    {
        svr_reset(settings);
    }
    return ret;
}

int svr_handleRequest(
    dlmsServerSettings* settings,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    return svr_handleRequest2(settings, data->data, (unsigned short)(data->size - data->position), reply);
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
    unsigned short size,
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
        //Server not Initialized.
        return DLMS_ERROR_CODE_NOT_INITIALIZED;
    }
    //Check frame using inter Charachter Timeout.
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC && settings->hdlc != NULL && settings->hdlc->interCharachterTimeout != 0)
    {
        long now = time_elapsed();
        unsigned short elapsed = (unsigned short)(now - settings->frameReceived);
        //If frame shoud be fully received.
        if (elapsed >= settings->hdlc->interCharachterTimeout)
        {
            settings->receivedData.position = settings->receivedData.size = 0;
        }
        settings->frameReceived = now;
    }
#endif //DLMS_IGNORE_IEC_HDLC_SETUP

    if (bb_isAttached(&settings->receivedData) && settings->receivedData.size + size > bb_getCapacity(&settings->receivedData))
    {
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
        return ret;
    }
    bb_set(&settings->receivedData, buff, size);
    first = settings->base.serverAddress == 0
        && settings->base.clientAddress == 0;
    if ((ret = dlms_getData2(&settings->base, &settings->receivedData, &settings->info, first)) != 0)
    {
        if (ret == DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS)
        {
            return 0;
        }
        else if (ret == DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS)
        {
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
            else if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
                (settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
            {
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
                settings->receivedData.position = settings->receivedData.size = 0;
                return ret;
            }
            settings->receivedData.position = settings->receivedData.size = 0;
            reply_clear2(&settings->info, 1);
            return 0;
        }
        else if (ret == DLMS_ERROR_CODE_INVALID_FRAME_NUMBER)
        {
            if ((settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
            {
                settings->dataReceived = time_current();
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
                settings->receivedData.position = settings->receivedData.size = 0;
                return ret;
            }
            settings->receivedData.position = settings->receivedData.size = 0;
            reply_clear2(&settings->info, 1);
            return 0;
        }
        else
        {
            if (ret != DLMS_ERROR_CODE_WRONG_CRC && settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
                (settings->base.connected & DLMS_CONNECTION_STATE_HDLC) != 0)
            {
                ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_REJECTED, NULL, reply);
                settings->receivedData.position = settings->receivedData.size = 0;
                return ret;
            }
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
        ret = dlms_getHdlcFrame(&settings->base, DLMS_COMMAND_DISCONNECT_MODE, NULL, reply);
        reply_clear2(&settings->info, 1);
        return ret;
    }

    if (first || settings->info.command == DLMS_COMMAND_SNRM ||
        (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER && settings->info.command == DLMS_COMMAND_AARQ))
    {
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC && settings->info.preEstablished)
        {
            svr_disconnected(settings);
        }
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
        if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC)
        {
            settings->base.connected |= DLMS_CONNECTION_STATE_HDLC;
            svr_connected(settings);
        }
    }
    // If client want next frame.
    if ((settings->info.moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == DLMS_DATA_REQUEST_TYPES_FRAME)
    {
        settings->dataReceived = time_current();
        return dlms_getHdlcFrame(&settings->base, getReceiverReady(&settings->base), NULL, reply);
    }
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
                //Return rest of frame.
                return dlms_getHdlcFrame(&settings->base, getNextSend(&settings->base, 0), &settings->info.data, reply);
            }
        }
        else if (settings->info.data.size == 0)
        {
            settings->dataReceived = time_current();
            return dlms_getHdlcFrame(&settings->base, getKeepAlive(&settings->base), NULL, reply);
        }
    }
    //Check inactivity timeout.
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
        settings->hdlc != NULL && settings->hdlc->inactivityTimeout != 0)
    {
        if (settings->info.command != DLMS_COMMAND_SNRM)
        {
            int elapsed = (int)(time_current() - settings->dataReceived);
            //If inactivity time out is elapsed.
            if (elapsed >= settings->hdlc->inactivityTimeout)
            {
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
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER &&
        settings->wrapper != NULL && settings->wrapper->inactivityTimeout != 0)
    {
        if (settings->info.command != DLMS_COMMAND_AARQ)
        {
            int elapsed = (int)(time_current() - settings->dataReceived);
            //If inactivity time out is elapsed.
            if (elapsed >= settings->wrapper->inactivityTimeout)
            {
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
    ret = svr_handleCommand(settings, settings->info.command, &settings->info.data, reply);
    if (ret != 0)
    {
        bb_clear(reply);
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
        {
            ret = svr_reportError(settings, settings->info.command, DLMS_ERROR_CODE_OTHER_REASON, reply);
        }
    }
    reply_clear2(&settings->info, 0);
    settings->dataReceived = time_current();
    return ret;
}

int handleInactivityTimeout(dlmsServerSettings* settings)
{
    //Check inactivity timeout.
    unsigned char inactivity = 0;
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_HDLC &&
        settings->hdlc != NULL && settings->hdlc->inactivityTimeout != 0)
    {
        inactivity = (time_current() - settings->dataReceived) >= settings->hdlc->inactivityTimeout;
    }
#endif // DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    if (settings->base.interfaceType == DLMS_INTERFACE_TYPE_WRAPPER &&
        settings->wrapper != NULL && settings->wrapper->inactivityTimeout != 0)
    {
        inactivity = (time_current() - settings->dataReceived) >= settings->wrapper->inactivityTimeout;
    }
#endif // DLMS_IGNORE_TCP_UDP_SETUP
    //If inactivity time out is elapsed.
    if (inactivity && (settings->info.preEstablished || settings->base.connected != DLMS_CONNECTION_STATE_NONE))
    {
        svr_disconnected(settings);
        svr_reset(settings);
    }
    return 0;
}

int svr_run(
    dlmsServerSettings* settings)
{
    return handleInactivityTimeout(settings);
}
#endif //DLMS_IGNORE_SERVER