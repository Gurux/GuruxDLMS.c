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
#if !(defined(DLMS_IGNORE_CLIENT) || defined(DLMS_IGNORE_MALLOC))
#include "../include/gxmem.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif

#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include "../include/apdu.h"
#include "../include/client.h"
#include "../include/gxkey.h"
#include "../include/gxset.h"
#include "../include/gxget.h"
#include "../include/dlms.h"
#include "../include/cosem.h"
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#include "../include/gxsetignoremalloc.h"
#else
#include "../include/gxsetmalloc.h"
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_HDLC
int cl_snrmRequest(dlmsSettings* settings, message* messages)
{
    int ret;
    gxByteBuffer* reply;
    gxByteBuffer* pData;
    mes_clear(messages);
    //Save default values.
    settings->initializeMaxInfoTX = settings->maxInfoTX;
    settings->initializeMaxInfoRX = settings->maxInfoRX;
    settings->initializeWindowSizeTX = settings->windowSizeTX;
    settings->initializeWindowSizeRX = settings->windowSizeRX;
    settings->connected = DLMS_CONNECTION_STATE_NONE;
    settings->isAuthenticationRequired = 0;
#ifndef DLMS_IGNORE_PLC
    // SNRM request is not used for all communication channels.
    if (settings->interfaceType == DLMS_INTERFACE_TYPE_PLC_HDLC)
    {
        reply = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(reply);
        ret = dlms_getMacHdlcFrame(settings, DLMS_COMMAND_SNRM, 0, NULL, reply);
        if (ret == 0)
        {
            ret = mes_push(messages, reply);
        }
        return ret;
    }
#endif //DLMS_IGNORE_PLC
    if (settings->interfaceType != DLMS_INTERFACE_TYPE_HDLC && settings->interfaceType != DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
    {
        return 0;
    }
    if (settings->interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        return DLMS_ERROR_CODE_OK;
    }
    resetFrameSequence(settings);
#ifdef DLMS_IGNORE_MALLOC
    if (!(messages->capacity > messages->size))
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    reply = messages->data[messages->size];
    ++messages->size;
    bb_clear(reply);
    pData = settings->serializedPdu;
    if (pData == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_clear(pData);
#else
    reply = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
    BYTE_BUFFER_INIT(reply);
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_capacity(&bb, 30)) != 0)
    {
        return ret;
    }
    pData = &bb;
#endif //DLMS_IGNORE_MALLOC

    // FromatID
    if ((ret = bb_setUInt8(pData, 0x81)) == 0 &&
        // GroupID
        (ret = bb_setUInt8(pData, 0x80)) == 0 &&
        // Length is updated later.
        (ret = bb_setUInt8(pData, 0)) == 0)
    {
        // If custom HDLC parameters are used.
        if (ret == 0 &&
            (DEFAULT_MAX_INFO_TX != settings->maxInfoTX ||
                DEFAULT_MAX_INFO_RX != settings->maxInfoRX ||
                DEFAULT_MAX_WINDOW_SIZE_TX != settings->windowSizeTX ||
                DEFAULT_MAX_WINDOW_SIZE_RX != settings->windowSizeRX))
        {
            if ((ret = bb_setUInt8(pData, HDLC_INFO_MAX_INFO_TX)) != 0 ||
                (ret = dlms_appendHdlcParameter(pData, settings->maxInfoTX)) != 0 ||
                (ret = bb_setUInt8(pData, HDLC_INFO_MAX_INFO_RX)) != 0 ||
                (ret = dlms_appendHdlcParameter(pData, settings->maxInfoRX)) != 0 ||
                (ret = bb_setUInt8(pData, HDLC_INFO_WINDOW_SIZE_TX)) != 0 ||
                (ret = bb_setUInt8(pData, 4)) != 0 ||
                (ret = bb_setUInt32(pData, settings->windowSizeTX)) != 0 ||
                (ret = bb_setUInt8(pData, HDLC_INFO_WINDOW_SIZE_RX)) != 0 ||
                (ret = bb_setUInt8(pData, 4)) != 0 ||
                (ret = bb_setUInt32(pData, settings->windowSizeRX)) != 0)
            {
                //Error is returned in the end of this method.
            }
        }
        // If default HDLC parameters are not used.
        if (ret == 0)
        {
            if (pData->size != 3)
            {
                // Length.
                ret = bb_setUInt8ByIndex(pData, 2, (unsigned char)(pData->size - 3));
            }
            else
            {
                bb_clear(pData);
            }
        }
        if (ret == 0 && (ret = dlms_getHdlcFrame(settings, DLMS_COMMAND_SNRM, pData, reply)) != 0)
        {
            bb_clear(pData);
            bb_clear(reply);
#ifndef DLMS_IGNORE_MALLOC
            gxfree(reply);
#endif //DLMS_IGNORE_MALLOC
            return ret;
        }
    }
    bb_clear(pData);
#ifndef DLMS_IGNORE_MALLOC
    ret = mes_push(messages, reply);
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int cl_parseUAResponse(dlmsSettings* settings, gxByteBuffer* data)
{
    int ret = dlms_parseSnrmUaResponse(settings, data);
    if (ret == 0 && bb_size(data) != 0)
    {
        settings->connected = DLMS_CONNECTION_STATE_HDLC;
    }
    return ret;
}

int cl_getFrameSize(dlmsSettings* settings, gxByteBuffer* data, uint32_t* size)
{
    static const unsigned char HDLC_FRAME_START_END = 0x7E;
    int ret = 0;
    uint16_t value;
    unsigned char ch;
    *size = 1;
    switch (settings->interfaceType)
    {
    case DLMS_INTERFACE_TYPE_HDLC:
    case DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E:
    {
        uint32_t pos, index = data->position;
        // If whole frame is not received yet.
        if (bb_available(data) > 8)
        {
            // Find start of HDLC frame.
            for (pos = data->position; pos < data->size; ++pos)
            {
                ret = bb_getUInt8(data, &ch);
                if (ret != 0 || ch == HDLC_FRAME_START_END)
                {
                    break;
                }
            }
            if ((ret = bb_getUInt8(data, &ch)) == 0)
            {
                // Check frame length.
                if ((ch & 0x7) != 0)
                {
                    *size = ((ch & 0x7) << 8);
                }
                if ((ret = bb_getUInt8(data, &ch)) == 0)
                {
                    *size += 1 + ch;
                }
            }
        }
        data->position = index;
    }
    break;
    case DLMS_INTERFACE_TYPE_WRAPPER:
        if (bb_available(data) < 8 ||
            (ret = bb_getUInt16ByIndex(data, data->position, &value)) != 0 ||
            value != 1)
        {
            *size = 8;
        }
        else
        {
            if ((ret = bb_getUInt16ByIndex(data, data->position + 6, &value)) == 0)
            {
                *size = 8 + value;
            }
        }
        break;
    case DLMS_INTERFACE_TYPE_PLC:
        if (bb_available(data) < 2 ||
            (ret = bb_getUInt8ByIndex(data, data->position, &ch)) != 0 ||
            ch != 2)
        {
            *size = 2;
        }
        else
        {
            if ((ret = bb_getUInt8ByIndex(data, data->position + 1, &ch)) == 0)
            {
                *size = 2 + ch;
            }
        }
        break;
    default:
        *size = 1;
        break;
    }
    if (*size < 1)
    {
        *size = 1;
    }
    return ret;
}

int cl_getRemainingFrameSize(dlmsSettings* settings, gxByteBuffer* data, uint32_t* size)
{
    int ret = cl_getFrameSize(settings, data, size);
    if (ret == 0)
    {
        *size -= bb_available(data);
    }
    return ret;
}

#endif //DLMS_IGNORE_HDLC

int cl_aarqRequest(
    dlmsSettings* settings,
    message* messages)
{
    if (settings->proposedConformance == 0)
    {
        //Invalid conformance.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (dlms_usePreEstablishedConnection(settings))
    {
        //Invalid conformance.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    //Save default values.
    settings->initializePduSize = settings->maxPduSize;
    int ret;
    gxByteBuffer* pdu;
#ifdef DLMS_IGNORE_MALLOC
    if (!(messages->size < messages->capacity))
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pdu = settings->serializedPdu;
    if (pdu == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_clear(pdu);
#else
    gxByteBuffer buff;
#ifdef GX_DLMS_MICROCONTROLLER
    static unsigned char GX_AARQ_PDU[130];
    if ((ret = bb_attach(&buff, GX_AARQ_PDU, 0, sizeof(GX_AARQ_PDU))) != 0)
    {
        return ret;
    }
    pdu = &buff;
#else
    BYTE_BUFFER_INIT(&buff);
    if ((ret = bb_capacity(&buff, 100)) != 0)
    {
        return ret;
    }
    pdu = &buff;
#endif
#endif //DLMS_IGNORE_MALLOC

    settings->connected &= ~DLMS_CONNECTION_STATE_DLMS;
    resetBlockIndex(settings);
    mes_clear(messages);
    ret = dlms_checkInit(settings);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    bb_clear(&settings->stoCChallenge);
    if (settings->autoIncreaseInvokeID)
    {
        settings->invokeID = 0;
    }
    else
    {
        settings->invokeID = 1;
    }
    // If authentication or ciphering is used.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW && settings->customChallenges == 0)
    {
        if ((ret = dlms_generateChallenge(&settings->ctoSChallenge)) != 0)
        {
            return ret;
        }
    }
    if ((ret = apdu_generateAarq(settings, pdu)) == 0)
    {
        if (settings->useLogicalNameReferencing)
        {
            gxLNParameters p;
            params_initLN(&p, settings, 0, DLMS_COMMAND_AARQ, 0, pdu, NULL, 0xFF, DLMS_COMMAND_NONE, 0, 0);
            ret = dlms_getLnMessages(&p, messages);
        }
        else
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            gxSNParameters p;
            params_initSN(&p, settings, DLMS_COMMAND_AARQ, 0, 0, NULL, pdu, DLMS_COMMAND_NONE);
            ret = dlms_getSnMessages(&p, messages);
#else
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
    }
    settings->connected &= ~DLMS_CONNECTION_STATE_DLMS;
    bb_clear(pdu);
    return ret;
}

int cl_parseAAREResponse(dlmsSettings* settings, gxByteBuffer* reply)
{
    int ret;
    unsigned char sd;
    DLMS_ASSOCIATION_RESULT result;
    unsigned char command = 0;
    if ((ret = apdu_parsePDU(settings, reply, &result, &sd, &command)) != 0)
    {
        return ret;
    }
    if (result != DLMS_ASSOCIATION_RESULT_ACCEPTED)
    {
        if (result == DLMS_ASSOCIATION_RESULT_TRANSIENT_REJECTED)
        {
            return DLMS_ERROR_CODE_REJECTED_TRANSIENT;
        }
        return DLMS_ERROR_CODE_REJECTED_PERMAMENT;
    }
    settings->isAuthenticationRequired = sd == DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED;
    if (!settings->isAuthenticationRequired)
    {
        settings->connected |= DLMS_CONNECTION_STATE_DLMS;
    }
    if (settings->dlmsVersionNumber != 6)
    {
        //Invalid DLMS version number.
        return DLMS_ERROR_CODE_INVALID_VERSION_NUMBER;
    }
    return 0;
}

int cl_getApplicationAssociationRequest(
    dlmsSettings* settings,
    message* messages)
{
    int ret;
    gxByteBuffer challenge;
    gxByteBuffer* pw;
    dlmsVARIANT data;
#if !defined(DLMS_IGNORE_HIGH_GMAC) || !defined(DLMS_IGNORE_HIGH_SHA256)
    gxByteBuffer pw2;
    BYTE_BUFFER_INIT(&pw2);
#endif //DLMS_IGNORE_HIGH_GMAC
#ifndef GX_DLMS_MICROCONTROLLER
    unsigned char APPLICATION_ASSOCIATION_REQUEST[64];
#else
    static unsigned char APPLICATION_ASSOCIATION_REQUEST[64];
#endif //DLMS_IGNORE_HIGH_GMAC
    bb_attach(&challenge, APPLICATION_ASSOCIATION_REQUEST, 0, sizeof(APPLICATION_ASSOCIATION_REQUEST));
    if (settings->authentication != DLMS_AUTHENTICATION_HIGH_ECDSA &&
#ifndef DLMS_IGNORE_HIGH_GMAC
        settings->authentication != DLMS_AUTHENTICATION_HIGH_GMAC &&
#endif // DLMS_IGNORE_HIGH_GMAC
        settings->password.size == 0)
    {
        //Password is invalid.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    resetBlockIndex(settings);
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
    {
#ifndef DLMS_IGNORE_MALLOC
        pw = &settings->cipher.systemTitle;
#else
        bb_attach(&pw2, settings->cipher.systemTitle, 8, 8);
        pw = &pw2;
#endif //DLMS_IGNORE_MALLOC
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    else if (settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA256)
    {
#ifndef DLMS_IGNORE_HIGH_SHA256
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = bb_set(&pw2, settings->password.data, settings->password.size)) != 0 ||
            (ret = bb_set(&pw2, settings->cipher.systemTitle, 8)) != 0 ||
            (ret = bb_set(&pw2, settings->sourceSystemTitle, 8)) != 0)
        {
            return ret;
        }
#else
        if ((ret = bb_set(&pw2, settings->password.data, settings->password.size)) != 0 ||
            (ret = bb_set(&pw2, settings->cipher.systemTitle.data, settings->cipher.systemTitle.size)) != 0 ||
            (ret = bb_set(&pw2, settings->sourceSystemTitle, 8)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_MALLOC
        if ((ret = bb_set(&pw2, settings->stoCChallenge.data, settings->stoCChallenge.size)) != 0 ||
            (ret = bb_set(&pw2, settings->ctoSChallenge.data, settings->ctoSChallenge.size)) != 0)
        {
            return ret;
        }
        pw = &pw2;
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_HIGH_SHA256
    }
    else
    {
        pw = &settings->password;
    }
    ret = dlms_secure(settings,
#ifndef DLMS_IGNORE_HIGH_GMAC
        settings->cipher.invocationCounter,
#else
        0,
#endif //DLMS_IGNORE_HIGH_GMAC
        & settings->stoCChallenge,
        pw,
        &challenge);
#if !defined(DLMS_IGNORE_HIGH_GMAC) || !defined(DLMS_IGNORE_HIGH_SHA256)
    bb_clear(&pw2);
#endif //!defined(DLMS_IGNORE_HIGH_GMAC) || !defined(DLMS_IGNORE_HIGH_SHA256)
    if (ret == 0)
    {
        var_init(&data);
        data.vt = DLMS_DATA_TYPE_OCTET_STRING;
        data.byteArr = &challenge;
        {
            if (settings->useLogicalNameReferencing)
            {
                static const unsigned char LN[6] = { 0, 0, 40, 0, 0, 255 };
                ret = cl_methodLN(settings, LN, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME,
                    1, &data, messages);
            }
            else
            {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
                ret = cl_methodSN(settings, 0xFA00, DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME, 8, &data,
                    messages);
#else
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            }
        }
#ifndef DLMS_IGNORE_MALLOC
        var_clear(&data);
        bb_clear(&challenge);
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int cl_parseApplicationAssociationResponse(
    dlmsSettings* settings,
    gxByteBuffer* reply)
{
    unsigned char empty, equals = 0;
    gxByteBuffer* secret;
    gxByteBuffer challenge;
#ifndef DLMS_IGNORE_HIGH_GMAC
    gxByteBuffer bb2;
#endif //DLMS_IGNORE_HIGH_GMAC
    int ret;
    uint32_t ic = 0;
    gxByteBuffer value;
    static unsigned char tmp[MAX_CHALLENGE_SIZE];
    static unsigned char CHALLENGE_BUFF[MAX_CHALLENGE_SIZE];
    bb_attach(&value, tmp, 0, sizeof(tmp));
    bb_attach(&challenge, CHALLENGE_BUFF, 0, sizeof(CHALLENGE_BUFF));
    if ((ret = cosem_getOctetString(reply, &value)) != 0)
    {
        settings->connected &= ~DLMS_CONNECTION_STATE_DLMS;
        //ParseApplicationAssociationResponse failed. Server to Client do not match.
        return DLMS_ERROR_CODE_AUTHENTICATION_FAILURE;
    }
    empty = value.size == 0;
    if (!empty)
    {
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
        {
            unsigned char ch;
            bb_attach(&bb2, settings->sourceSystemTitle, sizeof(settings->sourceSystemTitle), sizeof(settings->sourceSystemTitle));
            secret = &bb2;
            if ((ret = bb_set(&challenge, value.data, value.size)) != 0 ||
                (ret = bb_getUInt8(&challenge, &ch)) != 0 ||
                (ret = bb_getUInt32(&challenge, &ic)) != 0)
            {
                return ret;
            }
            bb_clear(&challenge);
        }
        else
#endif //DLMS_IGNORE_HIGH_GMAC
#ifndef DLMS_IGNORE_HIGH_SHA256
        if (settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA256)
        {
            bb_attach(&bb2, CHALLENGE_BUFF, 0, sizeof(CHALLENGE_BUFF));
            secret = &bb2;
            if ((ret = bb_set(secret, settings->password.data, settings->password.size)) != 0 ||
                (ret = bb_set(secret, settings->sourceSystemTitle, 8)) != 0 ||
                (ret = bb_set(secret, settings->cipher.systemTitle.data, settings->cipher.systemTitle.size)) != 0 ||
                (ret = bb_set(secret, settings->ctoSChallenge.data, settings->ctoSChallenge.size)) != 0 ||
                (ret = bb_set(secret, settings->stoCChallenge.data, settings->stoCChallenge.size)) != 0)
            {
                return ret;
            }
        }
        else
#endif //DLMS_IGNORE_HIGH_SHA256        
        {
            secret = &settings->password;
        }
        if ((ret = dlms_secure(
            settings,
            ic,
            &settings->ctoSChallenge,
            secret,
            &challenge)) != 0)
        {
            return ret;
        }
        equals = bb_compare(
            &challenge,
            value.data,
            value.size);
    }
    else
    {
        // Server did not accept CtoS.
    }
    if (!equals)
    {
        settings->connected &= ~DLMS_CONNECTION_STATE_DLMS;
        //ParseApplicationAssociationResponse failed. Server to Client do not match.
        return DLMS_ERROR_CODE_AUTHENTICATION_FAILURE;
    }
    settings->connected |= DLMS_CONNECTION_STATE_DLMS;
    return 0;
}

int cl_getObjectsRequest(dlmsSettings* settings, message* messages)
{
    int ret;
    if (settings->useLogicalNameReferencing)
    {
        static const unsigned char ln[] = { 0, 0, 40, 0, 0, 0xFF };
        ret = cl_readLN(settings, ln, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, 2, NULL, messages);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cl_readSN(settings, 0xFA00, 2, NULL, messages);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}

#ifndef DLMS_IGNORE_MALLOC
int cl_parseObjects(dlmsSettings* settings, gxByteBuffer* data)
{
    int ret;
    oa_clear(&settings->objects, 0);
    oa_clear(&settings->releasedObjects, 1);
    if (settings->useLogicalNameReferencing)
    {
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
        ret = cosem_parseLNObjects(settings, data, &settings->objects);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif // DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cosem_parseSNObjects(settings, data, &settings->objects);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}
#endif //DLMS_IGNORE_MALLOC

int cl_parseObjectCount(
    gxByteBuffer* data,
    uint16_t* count)
{
    unsigned char ch;
    int ret;
    //Get array tag.
    ret = bb_getUInt8(data, &ch);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    //Check that data is in the array
    if (ch != DLMS_DATA_TYPE_ARRAY)
    {
        return DLMS_ERROR_CODE_INVALID_RESPONSE;
    }
    //get object count
    if (hlp_getObjectCount2(data, count) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return ret;
}

int cl_parseNextObject(
    dlmsSettings* settings,
    gxByteBuffer* data,
    gxObject* object)
{
    int ret = 0;
    uint16_t size, capacity = sizeof(object->logicalName);
    if (settings->useLogicalNameReferencing)
    {
        uint16_t pos;
        signed char id;
        unsigned char mode;
        dlmsVARIANT selector;
        if ((ret = cosem_checkStructure(data, 4)) == 0 &&
            (ret = cosem_getUInt16(data, &object->objectType)) == 0 &&
            (ret = cosem_getUInt8(data, &object->version)) == 0 &&
            (ret = cosem_getOctetString2(data, object->logicalName, capacity, &size)) == 0 &&
            (ret = cosem_checkStructure(data, 2)) == 0)
        {
            size = 0xFFFF;
            if ((ret = cosem_checkArray(data, &size)) == 0)
            {
                var_init(&selector);
                //Get access modes.
                for (pos = 0; pos != size; ++pos)
                {
                    if ((ret = cosem_checkStructure(data, 3)) != 0 ||
                        (ret = cosem_getInt8(data, &id)) != 0 ||
                        (ret = cosem_getEnum(data, &mode)) != 0 ||
                        (ret = cosem_getVariant(data, &selector)) != 0)
                    {
                        break;
                    }
                    //Save access mode if user wants to know it.
                    if (object->access != NULL)
                    {
                        if ((ret = bb_setUInt8ByIndex(&object->access->attributeAccessModes, pos, mode)) != 0)
                        {
                            break;
                        }
                    }
                }
                //Get action access modes.
                if (ret == 0)
                {
                    size = 0xFFFF;
                    if ((ret = cosem_checkArray(data, &size)) == 0)
                    {
                        for (pos = 0; pos != size; ++pos)
                        {
                            if ((ret = cosem_checkStructure(data, 2)) != 0 ||
                                (ret = cosem_getInt8(data, &id)) != 0)
                            {
                                break;
                            }
                            if ((ret = bb_getUInt8(data, &mode)) != 0)
                            {
                                return ret;
                            }
                            //Version 0 uses boolean and other versions uses enum.
                            if (mode != DLMS_DATA_TYPE_ENUM && mode != DLMS_DATA_TYPE_BOOLEAN)
                            {
                                return DLMS_ERROR_CODE_INVALID_PARAMETER;
                            }
                            if ((ret = bb_getUInt8(data, &mode)) != 0)
                            {
                                return ret;
                            }
                            //Save action access mode if user wants to know it.
                            if (object->access != NULL)
                            {
                                if ((ret = bb_setUInt8ByIndex(&object->access->methodAccessModes, pos, mode)) != 0)
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        if ((ret = cosem_checkStructure(data, 4)) == 0 &&
            (ret = cosem_getInt16(data, (int16_t*)&object->shortName)) == 0 &&
            (ret = cosem_getUInt16(data, &object->objectType)) == 0 &&
            (ret = cosem_getUInt8(data, &object->version)) == 0 &&
            (ret = cosem_getOctetString2(data, object->logicalName, capacity, &size)) == 0)
        {

        }
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int cl_readSN(
    dlmsSettings* settings,
    uint16_t address,
    unsigned char attributeOrdinal,
    gxByteBuffer* data,
    message* messages)
{
    int ret;
    DLMS_VARIABLE_ACCESS_SPECIFICATION requestType;
    gxSNParameters p;
    gxByteBuffer attributeDescriptor;
    if ((attributeOrdinal < 1))
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    BYTE_BUFFER_INIT(&attributeDescriptor);
    resetBlockIndex(settings);
    address += (attributeOrdinal - 1) * 8;
    bb_setUInt16(&attributeDescriptor, address);
    // Add Selector.
    if (data != NULL && data->size != 0)
    {
        requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS;
    }
    else
    {
        requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME;
    }
    params_initSN(&p, settings, DLMS_COMMAND_READ_REQUEST, 1,
        requestType, &attributeDescriptor, data, DLMS_COMMAND_NONE);
    ret = dlms_getSnMessages(&p, messages);
    bb_clear(&attributeDescriptor);
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

int cl_readLN(
    dlmsSettings* settings,
    const unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
    unsigned char attributeOrdinal,
    gxByteBuffer* data,
    message* messages)
{
    int ret;
    gxLNParameters p;
    gxByteBuffer* pdu;
    if ((attributeOrdinal < 1))
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifdef DLMS_IGNORE_MALLOC
    if (settings->serializedPdu == NULL)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pdu = settings->serializedPdu;
    bb_clear(pdu);
#else
    gxByteBuffer attributeDescriptor;
    BYTE_BUFFER_INIT(&attributeDescriptor);
    pdu = &attributeDescriptor;
#endif //DLMS_IGNORE_MALLOC
    resetBlockIndex(settings);
    // CI
    if ((ret = bb_setUInt16(pdu, objectType)) == 0 &&
        // Add LN
        (ret = bb_set(pdu, name, 6)) == 0 &&
        // Attribute ID.
        (ret = bb_setUInt8(pdu, attributeOrdinal)) == 0)
    {
        if (data == NULL || data->size == 0)
        {
            // Access selection is not used.
            ret = bb_setUInt8(pdu, 0);
        }
        else
        {
            // Access selection is used.
            if ((ret = bb_setUInt8(pdu, 1)) == 0)
            {
                // Add data.
                ret = bb_set2(pdu, data, 0, data->size);
            }
        }
    }
    if (ret == 0)
    {
        params_initLN(&p, settings, 0,
            DLMS_COMMAND_GET_REQUEST, DLMS_GET_COMMAND_TYPE_NORMAL,
            pdu, data, 0xFF, DLMS_COMMAND_NONE, 0, 0);
        ret = dlms_getLnMessages(&p, messages);
    }
    bb_clear(pdu);
    return ret;
}

int cl_readList(
    dlmsSettings* settings,
    gxArray* list,
    message* reply)
{
    gxListItem* it;
    gxObject* obj;
    uint16_t pos = 0, count;
    int ret;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    uint16_t sn;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    gxByteBuffer bb;
    if (list->size == 0)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    BYTE_BUFFER_INIT(&bb);
    resetBlockIndex(settings);
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, DLMS_COMMAND_GET_REQUEST, DLMS_GET_COMMAND_TYPE_WITH_LIST,
            &bb, NULL, 0xff, DLMS_COMMAND_NONE, 0, 0);
        //Request service primitive shall always fit in a single APDU.
        count = (settings->maxPduSize - 12) / 10;
        if (list->size < count)
        {
            count = list->size;
        }
        //All meters can handle 10 items.
        if (count > 10)
        {
            count = 10;
        }
        // Add length.
        hlp_setObjectCount(count, &bb);
        for (pos = 0; pos != list->size; )
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxObject*))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            obj = (gxObject*)it->key;
            // CI.
            bb_setUInt16(&bb, obj->objectType);
            bb_set(&bb, obj->logicalName, 6);
            // Attribute ID.
            bb_setUInt8(&bb, it->value);
            // Attribute selector is not used.
            bb_setUInt8(&bb, 0);
            ++pos;
            if (pos % count == 0 && list->size != pos)
            {
                if ((ret = dlms_getLnMessages(&p, reply)) != 0)
                {
                    return ret;
                }
                bb_clear(&bb);
                if (list->size - pos < count)
                {
                    hlp_setObjectCount(list->size - pos, &bb);
                }
                else
                {
                    hlp_setObjectCount(count, &bb);
                }
            }
        }
        ret = dlms_getLnMessages(&p, reply);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        gxSNParameters p;
        for (pos = 0; pos != list->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxListItem))) != 0)
            {
                return ret;
            }
#else
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
            {
                return ret;
            }
#endif //DLMS_IGNORE_MALLOC

            // Add variable type.
            bb_setUInt8(&bb, 2);
            sn = ((gxObject*)it->key)->shortName;
            sn += ((uint16_t)it->value - 1) * 8;
            bb_setUInt16(&bb, sn);
        }
        params_initSN(&p, settings, DLMS_COMMAND_READ_REQUEST, list->size, 0xFF, &bb, NULL, DLMS_COMMAND_NONE);
        ret = dlms_getSnMessages(&p, reply);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    bb_clear(&bb);
    return ret;
}

int cl_read(
    dlmsSettings* settings,
    gxObject* object,
    unsigned char attributeOrdinal,
    message* messages)
{
    int ret;
    if (object == NULL)
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    else if (settings->useLogicalNameReferencing)
    {
        ret = cl_readLN(settings, object->logicalName, object->objectType, attributeOrdinal, NULL, messages);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cl_readSN(settings, object->shortName, attributeOrdinal, NULL, messages);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}

int cl_getKeepAlive(
    dlmsSettings* settings,
    message* messages)
{
    int ret;
    if (settings->useLogicalNameReferencing)
    {
        static const unsigned char ln[6] = { 0, 0, 40, 0, 0, 255 };
        ret = cl_readLN(settings, ln, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, 1, NULL, messages);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cl_readSN(settings, 0xFA00, 1, NULL, messages);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}

#ifndef DLMS_IGNORE_PROFILE_GENERIC
int cl_readRowsByEntry(dlmsSettings* settings, gxProfileGeneric* object, uint32_t index, uint32_t count, message* messages)
{
    return cl_readRowsByEntry2(settings, object, index, count, 1, 0, messages);
}

int cl_readRowsByEntry2(dlmsSettings* settings, gxProfileGeneric* object, uint32_t index, uint32_t count, uint16_t colStart, uint16_t colEnd, message* messages)
{
    int ret;
    gxByteBuffer data;
    if (object == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifdef DLMS_IGNORE_MALLOC
    unsigned char buff[20];
    bb_attach(&data, buff, 0, sizeof(buff));
#else
    BYTE_BUFFER_INIT(&data);
#endif //DLMS_IGNORE_MALLOC
    //Add AccessSelector
    if ((ret = bb_setUInt8(&data, 2)) == 0 &&
        //Add structure tag.
        (ret = cosem_setStructure(&data, 4)) == 0 &&
        //Add start index
        (ret = cosem_setUInt32(&data, index)) == 0 &&
        //Add Count
        (ret = cosem_setUInt32(&data, count == 0 ? 0 : index + count - 1)) == 0 &&
        //Add columns.
        (ret = cosem_setUInt16(&data, colStart)) == 0 &&
        (ret = cosem_setUInt16(&data, colEnd)) == 0)
    {
        if (settings->useLogicalNameReferencing)
        {
            ret = cl_readLN(settings, object->base.logicalName, object->base.objectType, 2, &data, messages);
        }
        else
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            ret = cl_readSN(settings, object->base.shortName, 2, &data, messages);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
    }
    bb_clear(&data);
    return ret;
}

int cl_readRowsByRange2(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    gxtime* start,
    gxtime* end,
    message* messages)
{
    unsigned char unixTime = 0;
    static unsigned char LN[] = { 0, 0, 1, 0, 0, 255 };
    DLMS_OBJECT_TYPE type = DLMS_OBJECT_TYPE_CLOCK;
    unsigned char* ln = LN;
    int ret;
    gxByteBuffer data;
    if (object == NULL || start == NULL || end == NULL || messages == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (object->captureObjects.size != 0)
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxTarget* kv;
        ret = arr_getByIndex2(&object->captureObjects, 0, (void**)&kv, sizeof(gxTarget));
        if (ret != 0)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        type = kv->target->objectType;
        ln = kv->target->logicalName;
#else
        type = kv->objectType;
        ln = kv->logicalName;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
        gxKey* kv;
        ret = arr_getByIndex(&object->captureObjects, 0, (void**)&kv);
        if (ret != 0)
        {
            return ret;
        }
        type = ((gxObject*)kv->key)->objectType;
        ln = ((gxObject*)kv->key)->logicalName;
#endif //DLMS_IGNORE_MALLOC
        unixTime = type == DLMS_OBJECT_TYPE_DATA;
    }
#ifndef DLMS_IGNORE_MALLOC
    if ((ret = obj_clearProfileGenericBuffer(&object->buffer)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
#ifdef DLMS_IGNORE_MALLOC
    unsigned char buff[100];
    bb_attach(&data, buff, 0, sizeof(buff));
#else
    BYTE_BUFFER_INIT(&data);
#endif //DLMS_IGNORE_MALLOC
    //Add AccessSelector
    if ((ret = bb_setUInt8(&data, 1)) == 0 &&
        //Add structure tag.
        (ret = cosem_setStructure(&data, 4)) == 0 &&
        //Add structure tag.
        (ret = cosem_setStructure(&data, 4)) == 0 &&
        // Add class_id
        (ret = cosem_setUInt16(&data, type)) == 0 &&
        // Add parameter Logical name
        (ret = cosem_setOctetString2(&data, ln, 6)) == 0 &&
        //Add attribute index.
        (ret = cosem_setInt8(&data, 2)) == 0 &&
        //Add data index.
        (ret = cosem_setUInt16(&data, 0)) == 0)
    {
        //Add start time
        if (unixTime)
        {
            ret = cosem_setUInt32(&data, time_toUnixTime2(start));
        }
        else
        {
            if ((ret = cosem_setDateTimeAsOctetString(&data, start)) != 0)
            {
                bb_clear(&data);
                return ret;
            }
        }
        //Add end time
        if (unixTime)
        {
            ret = cosem_setUInt32(&data, time_toUnixTime2(end));
        }
        else
        {
            if ((ret = cosem_setDateTimeAsOctetString(&data, end)) != 0)
            {
                bb_clear(&data);
                return ret;
            }
        }
        //Add array of read columns. Read All...
        bb_setUInt8(&data, 0x01);
        //Add item count
        bb_setUInt8(&data, 0x00);
        if (settings->useLogicalNameReferencing)
        {
            ret = cl_readLN(settings, object->base.logicalName, object->base.objectType, 2, &data, messages);
        }
        else
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            ret = cl_readSN(settings, object->base.shortName, 2, &data, messages);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
    }
    bb_clear(&data);
    return ret;
}

#ifdef DLMS_INDONESIA_STANDARD
int cl_readRowsByRange3(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    gxtime* start,
    gxtime* end,
    unsigned char startRegister,
    unsigned char numberOfRegisters,
    message* messages)
{
    int ret;
    gxByteBuffer data;
    if (start == NULL || end == NULL || messages == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifdef DLMS_IGNORE_MALLOC
    unsigned char buff[100];
    bb_attach(&data, buff, 0, sizeof(buff));
#else
    BYTE_BUFFER_INIT(&data);
#endif //DLMS_IGNORE_MALLOC
    //Add AccessSelector
    if ((ret = bb_setUInt8(&data, 1)) == 0 &&
        //Add structure tag.
        (ret = cosem_setStructure(&data, 4)) == 0)
    {
        //Add start time
        if ((ret = cosem_setDateTimeAsOctetString(&data, start)) != 0)
        {
            bb_clear(&data);
            return ret;
        }
        //Add end time
        if ((ret = cosem_setDateTimeAsOctetString(&data, end)) != 0)
        {
            bb_clear(&data);
            return ret;
        }
        //Add start register
        bb_setUInt8(&data, startRegister);
        //Add number of registers.
        bb_setUInt8(&data, numberOfRegisters);
        if (settings->useLogicalNameReferencing)
        {
            ret = cl_readLN(settings, object->base.logicalName, object->base.objectType, 2, &data, messages);
        }
        else
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            ret = cl_readSN(settings, object->base.shortName, 2, &data, messages);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
    }
    bb_clear(&data);
    return ret;
}
#endif //DLMS_INDONESIA_STANDARD

#ifdef DLMS_USE_EPOCH_TIME
int cl_readRowsByRange(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    uint32_t start,
    uint32_t end,
    message* messages)
#else
int cl_readRowsByRange(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    struct tm* start,
    struct tm* end,
    message* messages)
#endif //DLMS_USE_EPOCH_TIME
{
    gxtime s, e;
#ifdef DLMS_USE_EPOCH_TIME
    time_initUnix(&s, start);
    time_initUnix(&e, end);
#else
    time_initUnix(&s, time_toUnixTime(start));
    time_initUnix(&e, time_toUnixTime(end));
#endif //DLMS_USE_EPOCH_TIME
    return cl_readRowsByRange2(settings, object, &s, &e, messages);
}
#endif // DLMS_IGNORE_PROFILE_GENERIC

int cl_getData(dlmsSettings* settings, gxByteBuffer* reply, gxReplyData* data)
{
    return dlms_getData2(settings, reply, data, 0);
}

int cl_getData2(
    dlmsSettings* settings,
    gxByteBuffer* reply,
    gxReplyData* data,
    gxReplyData* notify,
    unsigned char* isNotify)
{
    return dlms_getData3(settings, reply, data, notify, 0, isNotify);
}

int cl_changeType(
    gxByteBuffer* value,
    DLMS_DATA_TYPE type,
    dlmsVARIANT* newValue)
{
    return dlms_changeType(value, type, newValue);
}

int cl_updateValue(
    dlmsSettings* settings,
    gxObject* target,
    unsigned char attributeIndex,
    dlmsVARIANT* value)
{
    gxValueEventArg e;
    if (target == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    e.target = target;
    e.index = attributeIndex;
    e.value = *value;
    return cosem_setValue(settings, &e);
}

int cl_updateValues(
    dlmsSettings* settings,
    gxArray* list,
    gxByteBuffer* data)
{
    uint16_t count;
    int ret = 0;
    uint16_t pos;
    gxListItem* it;
    gxDataInfo info;
    unsigned char ch;
    gxValueEventArg e;
    ve_init(&e);
    if ((ret = hlp_getObjectCount2(data, &count)) != 0)
    {
        return ret;
    }
    if (count != list->size)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    for (pos = 0; pos != list->size; ++pos)
    {
        di_init(&info);
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxListItem))) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
        ret = bb_getUInt8(data, &ch);
        if (ret != 0)
        {
            break;
        }
        if (ch == 0)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            e.value.vt = DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_OCTET_STRING;
            e.value.byteArr = data;
#else
            if ((ret = dlms_getData(data, &info, &e.value)) != 0)
            {
                break;
            }
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            e.target = (gxObject*)it->key;
            e.index = it->value;
            ret = cosem_setValue(settings, &e);
            ve_clear(&e);
            if (ret != 0)
            {
                break;
            }
        }
        else
        {
            ret = ch;
            break;
        }
    }
    ve_clear(&e);
    return ret;
}

int cl_receiverReady(dlmsSettings* settings, DLMS_DATA_REQUEST_TYPES type, gxByteBuffer* reply)
{
    return dlms_receiverReady(settings, type, reply);
}

/**
* Generates a release request.
*
* @return Release request, as byte array.
*/
int cl_releaseRequest(dlmsSettings* settings, message* packets)
{
    return cl_releaseRequest2(settings, packets, 0);
}

/**
* Generates a release request.
*
* @return Release request, as byte array.
*/
int cl_releaseRequest2(dlmsSettings* settings, message* packets, unsigned char useProtectedRelease)
{
    int ret;
    gxByteBuffer bb;
    mes_clear(packets);
    // If connection is not established, there is no need to send
    // DisconnectRequest.
    if ((settings->connected & DLMS_CONNECTION_STATE_DLMS) == 0)
    {
        return 0;
    }
    settings->connected &= ~DLMS_CONNECTION_STATE_DLMS;
    BYTE_BUFFER_INIT(&bb);
    if (!useProtectedRelease)
    {
        if ((ret = bb_setUInt8(&bb, 0x3)) != 0 ||
            (ret = bb_setUInt8(&bb, 0x80)) != 0 ||
            (ret = bb_setUInt8(&bb, 0x01)) != 0 ||
            (ret = bb_setUInt8(&bb, 0x0)) != 0)
        {
            return ret;
        }
    }
    else
    {
        // Length.
        if ((ret = bb_setUInt8(&bb, 0x0)) != 0 ||
            (ret = bb_setUInt8(&bb, 0x80)) != 0 ||
            (ret = bb_setUInt8(&bb, 0x01)) != 0 ||
            (ret = bb_setUInt8(&bb, 0x0)) != 0)
        {
            return ret;
        }
        //Restore default values.
        settings->maxPduSize = settings->initializePduSize;
        apdu_generateUserInformation(settings, &bb);
        bb.data[0] = (unsigned char)(bb.size - 1);
    }
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0,
            DLMS_COMMAND_RELEASE_REQUEST, DLMS_SET_COMMAND_TYPE_NORMAL,
            &bb, NULL, 0xff, DLMS_COMMAND_NONE, 0, 0);
        ret = dlms_getLnMessages(&p, packets);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        gxSNParameters p;
        params_initSN(&p, settings,
            DLMS_COMMAND_RELEASE_REQUEST, 1,
            DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME,
            NULL, &bb, DLMS_COMMAND_NONE);
        ret = dlms_getSnMessages(&p, packets);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    bb_clear(&bb);
    //Restore default values.
    settings->maxPduSize = settings->initializePduSize;
    return ret;
}

int cl_disconnectRequest(dlmsSettings* settings, message* packets)
{
    int ret = 0;
#ifndef DLMS_IGNORE_WRAPPER
    gxByteBuffer bb;
#endif //DLMS_IGNORE_WRAPPER
    gxByteBuffer* reply = NULL;
    mes_clear(packets);
    settings->maxPduSize = 0xFFFF;
    // If connection is not established, there is no need to send DisconnectRequest.
    if ((settings->connected & DLMS_CONNECTION_STATE_HDLC) == 0)
    {
        return ret;
    }
    settings->connected &= ~DLMS_CONNECTION_STATE_HDLC;
#ifdef DLMS_IGNORE_MALLOC
    reply = packets->data[0];
    ++packets->size;
    bb_clear(reply);
#else
    reply = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
    BYTE_BUFFER_INIT(reply);
#endif //DLMS_IGNORE_MALLOC
    switch (settings->interfaceType)
    {
#ifndef DLMS_IGNORE_HDLC
    case DLMS_INTERFACE_TYPE_HDLC:
    case DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E:
    {
        ret = dlms_getHdlcFrame(settings, DLMS_COMMAND_DISC, NULL, reply);
#ifndef DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            ret = mes_push(packets, reply);
        }
        else
        {
            gxfree(reply);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    break;
#endif //DLMS_IGNORE_HDLC
#ifndef DLMS_IGNORE_PLC
    case DLMS_INTERFACE_TYPE_PLC:
#ifndef DLMS_IGNORE_MALLOC
        gxfree(reply);
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_INTERFACE_TYPE_PLC_HDLC:
    {
        ret = dlms_getMacHdlcFrame(settings, DLMS_COMMAND_DISC, 0, NULL, reply);
#ifndef DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            ret = mes_push(packets, reply);
        }
        else
        {
            gxfree(reply);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    break;
#endif //DLMS_IGNORE_PLC
#ifndef DLMS_IGNORE_WRAPPER
    case DLMS_INTERFACE_TYPE_WRAPPER:
    {
        BYTE_BUFFER_INIT(&bb);
        bb_setUInt8(&bb, DLMS_COMMAND_RELEASE_REQUEST);
        bb_setUInt8(&bb, 0x0);
        ret = dlms_getWrapperFrame(settings, DLMS_COMMAND_NONE, &bb, reply);
#ifndef DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            ret = mes_push(packets, reply);
        }
        else
        {
            gxfree(reply);
        }
#endif //DLMS_IGNORE_MALLOC
        bb_clear(&bb);
    }
    break;
#endif //DLMS_IGNORE_WRAPPER
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
#ifndef DLMS_IGNORE_HDLC
    if (dlms_useHdlc(settings->interfaceType))
    {
        //Restore default HDLC values.
        settings->maxInfoTX = settings->initializeMaxInfoTX;
        settings->maxInfoRX = settings->initializeMaxInfoRX;
        settings->windowSizeTX = settings->initializeWindowSizeTX;
        settings->windowSizeRX = settings->initializeWindowSizeRX;
    }
#endif //DLMS_IGNORE_HDLC
    //Restore default values.
    settings->maxPduSize = settings->initializePduSize;
    resetFrameSequence(settings);
    return ret;
}

int cl_write(
    dlmsSettings* settings,
    gxObject* object,
    unsigned char index,
    message* messages)
{
    unsigned int ret;
    gxValueEventArg e;
    ve_init(&e);
    e.target = object;
    e.index = index;
    if ((ret = cosem_getValue(settings, &e)) != 0)
    {
        return ret;
    }
    if (settings->useLogicalNameReferencing)
    {
        ret = cl_writeLN(
            settings,
            object->logicalName,
            object->objectType,
            index,
            &e.value,
            e.byteArray,
            messages);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cl_writeSN(
            settings,
            object->shortName,
            index,
            &e.value,
            messages);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    ve_clear(&e);
    return ret;
}

int cl_writeList(
    dlmsSettings* settings,
    gxArray* list,
    message* reply)
{
    gxListItem* it;
    uint16_t pos;
    gxValueEventArg e;
    gxByteBuffer data;
    ve_init(&e);
    gxByteBuffer* pdu;
    int ret;
#ifdef DLMS_IGNORE_MALLOC
    if (settings->serializedPdu == NULL)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pdu = settings->serializedPdu;
    //Use same buffer for header and data. Header size is 10 bytes.
    BYTE_BUFFER_INIT(&data);
    bb_clear(pdu);
#else
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    pdu = &bb;
    BYTE_BUFFER_INIT(&data);
#endif //DLMS_IGNORE_MALLOC
    if (list->size == 0)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    resetBlockIndex(settings);
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, DLMS_COMMAND_SET_REQUEST, DLMS_SET_COMMAND_TYPE_WITH_LIST,
            pdu, &data, 0xff, DLMS_COMMAND_NONE, 0, 0);
        // Add length.
        hlp_setObjectCount(list->size, pdu);
        for (pos = 0; pos != list->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxListItem*))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            // CI.
            bb_setUInt16(pdu, it->key->objectType);
            bb_set(pdu, it->key->logicalName, 6);
            // Attribute ID.
            bb_setUInt8(pdu, it->value);
            // Attribute selector is not used.
            bb_setUInt8(pdu, 0);
        }
        // Add length.
        if (ret == 0)
        {
            hlp_setObjectCount(list->size, pdu);
            for (pos = 0; pos != list->size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxListItem*))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                e.target = it->key;
                e.index = it->value;
                if ((ret = cosem_getValue(settings, &e)) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if (e.byteArray != 0)
                {
                    bb_set2(pdu, e.value.byteArr, 0, bb_size(e.value.byteArr));
                }
                else
                {
                    if ((ret = dlms_setData(pdu, e.value.vt, &e.value)) != 0)
                    {
                        return ret;
                    }
                }
#else
                if (e.byteArray != 0)
                {
                    bb_set2(&data, e.value.byteArr, 0, e.value.byteArr->size);
                }
                else
                {
                    if ((ret = dlms_setData(&data, e.value.vt, &e.value)) != 0)
                    {
                        return ret;
                    }
                }
#endif //DLMS_IGNORE_MALLOC
            }
            if (ret == 0)
            {
                ret = dlms_getLnMessages(&p, reply);
            }
        }
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        uint16_t sn;
        gxSNParameters p;
        for (pos = 0; pos != list->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxListItem))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            // Add variable type.
            bb_setUInt8(pdu, 2);
            sn = ((gxObject*)it->key)->shortName;
            sn += ((uint16_t)it->value - 1) * 8;
            bb_setUInt16(pdu, sn);
        }
        if (ret == 0)
        {
            // Add length.
            hlp_setObjectCount(list->size, pdu);
            for (pos = 0; pos != list->size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxListItem*))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                e.target = it->key;
                e.index = it->value;
                if ((ret = cosem_getValue(settings, &e)) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if (e.byteArray != 0)
                {
                    bb_set2(pdu, value->byteArr, 0, value->byteArr->size);
                }
                else
                {
                    if ((ret = dlms_setData(pdu, value->vt, value)) != 0)
                    {
                        break;
                    }
                }
#else
                if (e.byteArray != 0)
                {
                    bb_set2(&data, e.value.byteArr, 0, e.value.byteArr->size);
                }
                else
                {
                    if ((ret = dlms_setData(&data, e.value.vt, &e.value)) != 0)
                    {
                        break;
                    }
                }
            }
#endif //DLMS_IGNORE_MALLOC
            if (ret == 0)
            {
                params_initSN(&p, settings, DLMS_COMMAND_WRITE_REQUEST, list->size, 0xFF, pdu, &data, DLMS_COMMAND_NONE);
                ret = dlms_getSnMessages(&p, reply);
            }
        }
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    ve_clear(&e);
    bb_clear(pdu);
    bb_clear(&data);
    return ret;
}

int cl_writeLN(
    dlmsSettings* settings,
    const unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
    unsigned char index,
    dlmsVARIANT* value,
    unsigned char byteArray,
    message* messages)
{
    int ret;
    gxLNParameters p;
    gxByteBuffer* pdu;
    gxByteBuffer data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifdef DLMS_IGNORE_MALLOC
    if (settings->serializedPdu == NULL)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pdu = settings->serializedPdu;
    //Use same buffer for header and data. Header size is 10 bytes.
    BYTE_BUFFER_INIT(&data);
    bb_clear(pdu);
#else
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    pdu = &bb;
    BYTE_BUFFER_INIT(&data);
#endif //DLMS_IGNORE_MALLOC
    resetBlockIndex(settings);
    // Add CI.
    bb_setUInt16(pdu, objectType);
    // Add LN
    bb_set(pdu, name, 6);
    // Attribute ID.
    bb_setUInt8(pdu, index);
    // Access selection is not used.
    bb_setUInt8(pdu, 0);
#ifdef DLMS_IGNORE_MALLOC
    if (byteArray != 0)
    {
        bb_set2(pdu, value->byteArr, 0, value->byteArr->size);
    }
    else
    {
        if ((ret = dlms_setData(pdu, value->vt, value)) != 0)
        {
            return ret;
        }
    }
#else
    if (byteArray != 0)
    {
        bb_set2(&data, value->byteArr, 0, value->byteArr->size);
    }
    else
    {
        if ((ret = dlms_setData(&data, value->vt, value)) != 0)
        {
            return ret;
        }
    }
#endif //DLMS_IGNORE_MALLOC

    params_initLN(&p, settings, 0,
        DLMS_COMMAND_SET_REQUEST, DLMS_SET_COMMAND_TYPE_NORMAL,
        pdu, &data, 0xff, DLMS_COMMAND_NONE, 0, 0);
    ret = dlms_getLnMessages(&p, messages);
    bb_clear(&data);
    bb_clear(pdu);
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int cl_writeSN(
    dlmsSettings* settings,
    uint16_t address,
    int index,
    dlmsVARIANT* value,
    message* messages)
{
    int ret;
    gxSNParameters p;
    gxByteBuffer bb, data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    resetBlockIndex(settings);
    BYTE_BUFFER_INIT(&bb);
    BYTE_BUFFER_INIT(&data);
    if ((ret = dlms_setData(&data, value->vt, value)) != 0)
    {
        return ret;
    }

    // Add name.
    address += (uint16_t)((index - 1) * 8);
    bb_setUInt16(&bb, address);
    // Add data count.
    bb_setUInt8(&bb, 1);
    params_initSN(&p, settings,
        DLMS_COMMAND_WRITE_REQUEST, 1,
        DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME,
        &bb, &data, DLMS_COMMAND_NONE);
    ret = dlms_getSnMessages(&p, messages);
    bb_clear(&data);
    bb_clear(&bb);
    return ret;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

int cl_method(
    dlmsSettings* settings,
    gxObject* object,
    unsigned char index,
    dlmsVARIANT* data,
    message* messages)
{
    int ret;
    if (settings->useLogicalNameReferencing)
    {
        ret = cl_methodLN(settings, object->logicalName, object->objectType, index, data, messages);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cl_methodSN(settings, object->shortName, object->objectType, index, data, messages);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}

int cl_method2(
    dlmsSettings* settings,
    gxObject* object,
    unsigned char index,
    unsigned char* value,
    uint32_t length,
    message* messages)
{
    int ret;
    if (settings->useLogicalNameReferencing)
    {
        ret = cl_methodLN2(settings, object->logicalName, object->objectType, index, value, length, messages);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cl_methodSN2(settings, object->shortName, object->objectType, index, value, length, messages);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}

int cl_methodLN(
    dlmsSettings* settings,
    const unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
    unsigned char index,
    dlmsVARIANT* value,
    message* messages)
{
    int ret = 0;
    gxLNParameters p;
    gxByteBuffer* pdu;
    gxByteBuffer data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifdef DLMS_IGNORE_MALLOC
    if (settings->serializedPdu == NULL)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pdu = settings->serializedPdu;
    //Use same buffer for header and data. Header size is 10 bytes.
    BYTE_BUFFER_INIT(&data);
    bb_clear(pdu);
#else
    gxByteBuffer bb;
    unsigned char GX_METHOD_PDU[10];
    bb_attach(&bb, GX_METHOD_PDU, 0, sizeof(GX_METHOD_PDU));
    pdu = &bb;
    BYTE_BUFFER_INIT(&data);
#endif //DLMS_IGNORE_MALLOC
    resetBlockIndex(settings);
    // CI
    if ((ret = bb_setUInt16(pdu, objectType)) == 0 &&
        // Add LN
        (ret = bb_set(pdu, name, 6)) == 0 &&
        // Attribute ID.
        (ret = bb_setUInt8(pdu, index)) == 0 &&
        // Is Method Invocation Parameters used.
        (ret = bb_setUInt8(pdu, value == NULL || value->vt == DLMS_DATA_TYPE_NONE ? 0 : 1)) == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        if (value != NULL && value->vt != DLMS_DATA_TYPE_NONE)
        {
            if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
            {
                ret = bb_set(pdu, value->byteArr->data, value->byteArr->size);
            }
            else
            {
                ret = dlms_setData(pdu, value->vt, value);
            }
        }
#else
        if (value != NULL && value->vt != DLMS_DATA_TYPE_NONE)
        {
            if ((value->vt == DLMS_DATA_TYPE_ARRAY || value->vt == DLMS_DATA_TYPE_STRUCTURE) &&
                value->vt == DLMS_DATA_TYPE_OCTET_STRING)
            {
                ret = bb_set(&data, value->byteArr->data, value->byteArr->size);
            }
            else
            {
                if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
                {
                    //Space is allocated for type and size
                    ret = bb_capacity(&data, 5 + bb_size(value->byteArr));
                }
                if (ret == 0)
                {
                    ret = dlms_setData(&data, value->vt, value);
                }
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0)
    {
        params_initLN(&p, settings, 0,
            DLMS_COMMAND_METHOD_REQUEST, DLMS_ACTION_COMMAND_TYPE_NORMAL,
            pdu, &data, 0xff, DLMS_COMMAND_NONE, 0, 0);
        ret = dlms_getLnMessages(&p, messages);
    }
    bb_clear(&data);
    bb_clear(pdu);
    return ret;
}

int cl_methodLN2(
    dlmsSettings* settings,
    unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
    unsigned char index,
    unsigned char* value,
    uint32_t length,
    message* messages)
{
    int ret = 0;
    gxLNParameters p;
    gxByteBuffer* pdu;
    gxByteBuffer data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifdef DLMS_IGNORE_MALLOC
    if (settings->serializedPdu == NULL)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pdu = settings->serializedPdu;
    //Use same buffer for header and data. Header size is 10 bytes.
    BYTE_BUFFER_INIT(&data);
    bb_clear(pdu);
#else
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    pdu = &bb;
    BYTE_BUFFER_INIT(&data);
#endif //DLMS_IGNORE_MALLOC
    resetBlockIndex(settings);
    // CI
    if ((ret = bb_setUInt16(pdu, objectType)) == 0 &&
        // Add LN
        (ret = bb_set(pdu, name, 6)) == 0 &&
        // Attribute ID.
        (ret = bb_setUInt8(pdu, index)) == 0 &&
        // Is Method Invocation Parameters used.
        (ret = bb_setUInt8(pdu, 1)) == 0)
    {
        ret = bb_set(pdu, value, length);
    }
    if (ret == 0)
    {
        params_initLN(&p, settings, 0,
            DLMS_COMMAND_METHOD_REQUEST, DLMS_ACTION_COMMAND_TYPE_NORMAL,
            pdu, &data, 0xff, DLMS_COMMAND_NONE, 0, 0);
        ret = dlms_getLnMessages(&p, messages);
    }
    bb_clear(&data);
    bb_clear(pdu);
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int cl_methodSN(
    dlmsSettings* settings,
    uint16_t address,
    DLMS_OBJECT_TYPE objectType,
    int index,
    dlmsVARIANT* value,
    message* messages)
{
    int ret;
    unsigned char v, count;
    gxSNParameters p;
    unsigned char requestType;
    gxByteBuffer bb, data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    resetBlockIndex(settings);
    BYTE_BUFFER_INIT(&data);
    if (value != NULL && value->vt != DLMS_DATA_TYPE_NONE)
    {
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            bb_set(&data, value->byteArr->data, value->byteArr->size);
        }
        else
        {
            dlms_setData(&data, value->vt, value);
        }
    }
    BYTE_BUFFER_INIT(&bb);
    if (value == NULL || value->vt == DLMS_DATA_TYPE_NONE)
    {
        requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME;
    }
    else
    {
        requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS;
    }
    if ((ret = dlms_getActionInfo(objectType, &v, &count)) != 0)
    {
        return ret;
    }
    if (index > count)
    {
        //Invalid parameter
        bb_clear(&data);
        bb_clear(&bb);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    index = (v + (index - 1) * 0x8);
    address += (uint16_t)index;
    // Add SN count.
    bb_setUInt8(&bb, 1);
    // Add name length.
    bb_setUInt8(&bb, 4);
    // Add name.
    bb_setUInt16(&bb, address);
    // Method Invocation Parameters is not used.
    if (value == NULL || value->vt == DLMS_DATA_TYPE_NONE)
    {
        bb_setUInt8(&bb, 0);
    }
    else
    {
        bb_setUInt8(&bb, 1);
    }
    params_initSN(&p, settings, DLMS_COMMAND_READ_REQUEST, 1,
        requestType, &bb, &data, DLMS_COMMAND_NONE);
    ret = dlms_getSnMessages(&p, messages);
    bb_clear(&data);
    bb_clear(&bb);
    return ret;
}

int cl_methodSN2(
    dlmsSettings* settings,
    uint16_t address,
    DLMS_OBJECT_TYPE objectType,
    int index,
    unsigned char* value,
    uint32_t length,
    message* messages)
{
    int ret;
    unsigned char v, count;
    gxSNParameters p;
    gxByteBuffer bb, data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    resetBlockIndex(settings);
    BYTE_BUFFER_INIT(&data);
    ret = bb_set(&data, value, length);
    BYTE_BUFFER_INIT(&bb);
    if ((ret = dlms_getActionInfo(objectType, &v, &count)) != 0)
    {
        return ret;
    }
    if (index > count)
    {
        //Invalid parameter
        bb_clear(&data);
        bb_clear(&bb);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    index = (v + (index - 1) * 0x8);
    address += (uint16_t)index;
    // Add SN count.
    bb_setUInt8(&bb, 1);
    // Add name length.
    bb_setUInt8(&bb, 4);
    // Add name.
    bb_setUInt16(&bb, address);
    bb_setUInt8(&bb, 1);
    params_initSN(&p, settings, DLMS_COMMAND_READ_REQUEST, 1,
        DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS, &bb, &data, DLMS_COMMAND_NONE);
    ret = dlms_getSnMessages(&p, messages);
    bb_clear(&data);
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

uint16_t cl_getServerAddress(uint16_t logicalAddress, uint16_t physicalAddress, unsigned char addressSize)
{
    uint16_t value;
    if (addressSize < 4 && physicalAddress < 0x80 && logicalAddress < 0x80)
    {
        value = (uint16_t)(logicalAddress << 7 | physicalAddress);
    }
    else if (physicalAddress < 0x4000 && logicalAddress < 0x4000)
    {
        value = (uint16_t)(logicalAddress << 14 | physicalAddress);
    }
    else
    {
        value = 0;
    }
    return value;
}
#endif //!(defined(DLMS_IGNORE_CLIENT) || defined(DLMS_IGNORE_MALLOC))
