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
#ifndef DLMS_IGNORE_CLIENT
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
#include "../include/cosem.h"

int cl_snrmRequest(dlmsSettings* settings, message* messages)
{
    int ret;
    gxByteBuffer bb;
    gxByteBuffer* reply;
    mes_clear(messages);
    settings->connected = DLMS_CONNECTION_STATE_NONE;
    settings->isAuthenticationRequired = 0;
    //SNRM request is not used in network connections.
    if (settings->interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        return DLMS_ERROR_CODE_OK;
    }
    resetFrameSequence(settings);
    reply = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
    bb_init(reply);
    bb_init(&bb);
    bb_capacity(&bb, 30);

    // FromatID
    bb_setUInt8(&bb, 0x81);
    // GroupID
    bb_setUInt8(&bb, 0x80);
    // Length is updated later.
    bb_setUInt8(&bb, 0);
    // If custom HDLC parameters are used.
    if (DEFAULT_MAX_INFO_TX != settings->maxInfoTX)
    {
        bb_setUInt8(&bb, HDLC_INFO_MAX_INFO_TX);
        dlms_appendHdlcParameter(&bb, settings->maxInfoTX);
    }
    if (DEFAULT_MAX_INFO_RX != settings->maxInfoRX)
    {
        bb_setUInt8(&bb, HDLC_INFO_MAX_INFO_RX);
        dlms_appendHdlcParameter(&bb, settings->maxInfoRX);
    }
    if (DEFAULT_MAX_WINDOW_SIZE_TX != settings->windowSizeTX)
    {
        bb_setUInt8(&bb, HDLC_INFO_WINDOW_SIZE_TX);
        bb_setUInt8(&bb, 4);
        bb_setUInt32(&bb, settings->windowSizeTX);
    }
    if (DEFAULT_MAX_WINDOW_SIZE_RX != settings->windowSizeRX)
    {
        bb_setUInt8(&bb, HDLC_INFO_WINDOW_SIZE_RX);
        bb_setUInt8(&bb, 4);
        bb_setUInt32(&bb, settings->windowSizeRX);
    }
    // If default HDLC parameters are not used.
    if (bb.size != 3)
    {
        // Length.
        bb_setUInt8ByIndex(&bb, 2, (unsigned char)(bb.size - 3));
    }
    else
    {
        bb_clear(&bb);
    }
    if ((ret = dlms_getHdlcFrame(settings, DLMS_COMMAND_SNRM, &bb, reply)) != 0)
    {
        bb_clear(&bb);
        bb_clear(reply);
        gxfree(reply);
        return ret;
    }
    bb_clear(&bb);
    mes_push(messages, reply);
    return ret;
}

int cl_parseUAResponse(dlmsSettings *settings, gxByteBuffer *data)
{
    int ret = dlms_parseSnrmUaResponse(settings, data);
    settings->connected = DLMS_CONNECTION_STATE_HDLC;
    return ret;
}

int cl_aarqRequest(
    dlmsSettings* settings,
    message* messages)
{
    int ret;
    gxByteBuffer buff;
    settings->connected &= ~DLMS_CONNECTION_STATE_DLMS;
    resetBlockIndex(settings);
    mes_clear(messages);
    ret = dlms_checkInit(settings);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    bb_clear(&settings->stoCChallenge);
    bb_clear(&settings->ctoSChallenge);
    // If authentication or ciphering is used.
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        if ((ret = dlms_generateChallenge(&settings->ctoSChallenge)) != 0)
        {
            return ret;
        }
    }
    bb_init(&buff);
    if ((ret = apdu_generateAarq(settings, &buff)) == 0)
    {
        if (settings->useLogicalNameReferencing)
        {
            gxLNParameters p;
            params_initLN(&p, settings, 0, DLMS_COMMAND_AARQ, 0, &buff, NULL, 0xFF, DLMS_COMMAND_NONE);
            ret = dlms_getLnMessages(&p, messages);
        }
        else
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            gxSNParameters p;
            params_initSN(&p, settings, DLMS_COMMAND_AARQ, 0, 0, NULL, &buff, DLMS_COMMAND_NONE);
            ret = dlms_getSnMessages(&p, messages);
#else
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        }
    }
    settings->connected &= ~DLMS_CONNECTION_STATE_DLMS;
    bb_clear(&buff);
    return ret;
}

int cl_parseAAREResponse(dlmsSettings* settings, gxByteBuffer* reply)
{
    int ret;
    unsigned char sd;
    DLMS_ASSOCIATION_RESULT result;
    if ((ret = apdu_parsePDU(settings, reply, &result, &sd)) != 0)
    {
        return ret;
    }
    settings->isAuthenticationRequired = result == DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED && sd == DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED;
    if (!settings->isAuthenticationRequired)
    {
        settings->connected |= DLMS_CONNECTION_STATE_DLMS;
    }
    if (settings->dlmsVersionNumber != 6)
    {
        //Invalid DLMS version number.
        return DLMS_ERROR_CODE_INVALID_VERSION_NUMBER;
    }
    if (settings->isAuthenticationRequired != 0)
    {
        settings->connected |= DLMS_CONNECTION_STATE_DLMS;
    }
    return 0;
}

int cl_getApplicationAssociationRequest(
    dlmsSettings* settings,
    message* messages)
{
    int ret;
    gxByteBuffer challenge;
    gxByteBuffer *pw;
    dlmsVARIANT data;
    if (settings->authentication != DLMS_AUTHENTICATION_HIGH_ECDSA &&
        settings->authentication != DLMS_AUTHENTICATION_HIGH_GMAC &&
        settings->password.size == 0)
    {
        //Password is invalid.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_init(&challenge);
    resetBlockIndex(settings);
    if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
    {
#ifndef DLMS_IGNORE_HIGH_GMAC
        pw = &settings->cipher.systemTitle;
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_HIGH_GMAC
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
        &settings->stoCChallenge,
        pw,
        &challenge);
    if (ret == 0)
    {
        var_init(&data);
        data.byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        bb_init(data.byteArr);
        data.vt = DLMS_DATA_TYPE_OCTET_STRING;
        bb_setUInt8(data.byteArr, DLMS_DATA_TYPE_OCTET_STRING);
        hlp_setObjectCount(challenge.size, data.byteArr);
        bb_set2(data.byteArr, &challenge, 0, challenge.size);
        bb_clear(&challenge);
        if (settings->useLogicalNameReferencing)
        {
            unsigned char LN[6] = { 0, 0, 40, 0, 0, 255 };
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
        var_clear(&data);
    }
    return ret;
}

int cl_parseApplicationAssociationResponse(
    dlmsSettings* settings,
    gxByteBuffer* reply)
{
    gxDataInfo info;
    unsigned char ch;
    unsigned char equals = 0;
    gxByteBuffer *secret;
    gxByteBuffer challenge, bb;
    int ret;
    unsigned long ic = 0;
    dlmsVARIANT value;
    var_init(&value);
    di_init(&info);
    if ((ret = dlms_getData(reply,
        &info,
        &value)) != 0)
    {
        return ret;
    }
    bb_init(&challenge);
    bb_init(&bb);
    if (value.vt != DLMS_DATA_TYPE_NONE)
    {
        if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
        {
            secret = &settings->sourceSystemTitle;
            bb_set(&bb, value.byteArr->data, value.byteArr->size);
            if ((ret = bb_getUInt8(&bb, &ch)) != 0 ||
                (ret = bb_getUInt32(&bb, &ic)) != 0)
            {
                var_clear(&value);
                bb_clear(&challenge);
                bb_clear(&bb);
                return ret;
            }
        }
        else
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
            var_clear(&value);
            bb_clear(&challenge);
            bb_clear(&bb);
            return ret;
        }
        equals = bb_compare(
            &challenge,
            value.byteArr->data,
            value.byteArr->size);
        var_clear(&value);
    }
    else
    {
        // Server did not accept CtoS.
    }
    bb_clear(&challenge);
    bb_clear(&bb);
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
        static unsigned char ln[] = { 0, 0, 40, 0, 0, 0xFF };
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

// LN referencing
int cl_parseLNObjects(gxByteBuffer* data, objectArray* objects)
{
    gxDataInfo info;
    DLMS_OBJECT_TYPE class_id;
    unsigned char version;
    gxObject* object;
    int ret;
    unsigned short pos, count;
    dlmsVARIANT value;
    dlmsVARIANT *it1 = NULL, *it2 = NULL, *it3 = NULL, *ln = NULL;
    unsigned char size;
    oa_clear(objects);
    var_init(&value);
    //Get array tag.
    ret = bb_getUInt8(data, &size);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return 0;
    }
    //Check that data is in the array
    if (size != DLMS_DATA_TYPE_ARRAY)
    {
        return DLMS_ERROR_CODE_INVALID_RESPONSE;
    }
    //get object count
    if (hlp_getObjectCount2(data, &count) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    oa_capacity(objects, (unsigned short)count);
    for (pos = 0; pos != count; ++pos)
    {
        var_clear(&value);
        object = NULL;
        di_init(&info);
        if ((ret = dlms_getData(data, &info, &value)) != 0)
        {
            //This fix Iskraemeco (MT-880) bug.
            if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
            {
                break;
            }
            return ret;
        }
        if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr->size != 4)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = va_get(value.Arr, &it1)) != 0 ||
            (ret = va_get(value.Arr, &it2)) != 0 ||
            (ret = va_get(value.Arr, &ln)) != 0 ||
            (ret = va_get(value.Arr, &it3)) != 0)
        {
            return ret;
        }
        if (it1->vt != DLMS_DATA_TYPE_UINT16 ||
            it2->vt != DLMS_DATA_TYPE_UINT8 ||
            ln->vt != DLMS_DATA_TYPE_OCTET_STRING ||
            it3->vt != DLMS_DATA_TYPE_STRUCTURE ||
            it3->Arr->size != 2)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        class_id = (DLMS_OBJECT_TYPE)var_toInteger(it1);
        version = (unsigned char)var_toInteger(it2);
        ret = cosem_createObject(class_id, &object);
        //If known object.
        if (ret != DLMS_ERROR_CODE_INVALID_PARAMETER)
        {
            object->version = version;
            ret = obj_updateAttributeAccessModes(object, it3->Arr);
            if (ret != 0)
            {
                return ret;
            }
            cosem_setLogicalName(object, ln->byteArr->data);
            oa_push(objects, object);
        }
    }
    var_clear(&value);
    return 0;
}


#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
// SN referencing
int cl_parseSNObjects(gxByteBuffer* data, objectArray* objects)
{
    gxDataInfo info;
    short sn;
    DLMS_OBJECT_TYPE class_id;
    dlmsVARIANT value;
    dlmsVARIANT *it1 = NULL, *it2 = NULL, *it3 = NULL, *ln = NULL;
    gxObject* object;
    int ret;
    unsigned short count, pos;
    unsigned char size, version;
    oa_clear(objects);
    var_init(&value);
    //Get array tag.
    ret = bb_getUInt8(data, &size);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return 0;
    }
    //Check that data is in the array
    if (size != 0x01)
    {
        return DLMS_ERROR_CODE_INVALID_RESPONSE;
    }
    //get object count
    if ((hlp_getObjectCount2(data, &count)) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    oa_capacity(objects, (unsigned short)count);
    for (pos = 0; pos != count; ++pos)
    {
        var_clear(&value);
        object = NULL;
        di_init(&info);
        if ((ret = dlms_getData(data, &info, &value)) != 0)
        {
            var_clear(&value);
            return ret;
        }
        if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr->size != 4)
        {
            var_clear(&value);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = va_get(value.Arr, &it1)) != 0 ||
            (ret = va_get(value.Arr, &it2)) != 0 ||
            (ret = va_get(value.Arr, &it3)) != 0 ||
            (ret = va_get(value.Arr, &ln)) != 0)
        {
            var_clear(&value);
            return ret;
        }
        if (it1->vt != DLMS_DATA_TYPE_INT16 ||
            it2->vt != DLMS_DATA_TYPE_UINT16 ||
            it3->vt != DLMS_DATA_TYPE_UINT8 ||
            ln->vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            var_clear(&value);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        sn = (short)var_toInteger(it1);
        class_id = (DLMS_OBJECT_TYPE)var_toInteger(it2);
        version = (unsigned char)var_toInteger(it3);
        ret = cosem_createObject(class_id, &object);
        //If known object.
        if (ret != DLMS_ERROR_CODE_INVALID_PARAMETER)
        {
            object->shortName = sn;
            object->version = version;
            cosem_setLogicalName(object, ln->byteArr->data);
            oa_push(objects, object);
        }
    }
    var_clear(&value);
    return 0;
}

#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME

int cl_parseObjects(dlmsSettings* settings, gxByteBuffer* data)
{
    int ret;
    if (settings->useLogicalNameReferencing)
    {
        ret = cl_parseLNObjects(data, &settings->objects);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = cl_parseSNObjects(data, &settings->objects);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int cl_readSN(
    dlmsSettings* settings,
    unsigned short address,
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
    bb_init(&attributeDescriptor);
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
    unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
    unsigned char attributeOrdinal,
    gxByteBuffer* data,
    message* messages)
{
    int ret;
    gxLNParameters p;
    gxByteBuffer attributeDescriptor;
    if ((attributeOrdinal < 1))
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_init(&attributeDescriptor);
    resetBlockIndex(settings);
    // CI
    bb_setUInt16(&attributeDescriptor, objectType);
    // Add LN
    bb_set(&attributeDescriptor, name, 6);
    // Attribute ID.
    bb_setUInt8(&attributeDescriptor, attributeOrdinal);
    if (data == NULL || data->size == 0)
    {
        // Access selection is not used.
        bb_setUInt8(&attributeDescriptor, 0);
    }
    else
    {
        // Access selection is used.
        bb_setUInt8(&attributeDescriptor, 1);
        // Add data.
        bb_set2(&attributeDescriptor, data, 0, data->size);
    }
    params_initLN(&p, settings, 0,
        DLMS_COMMAND_GET_REQUEST, DLMS_GET_COMMAND_TYPE_NORMAL,
        &attributeDescriptor, data, 0xFF, DLMS_COMMAND_NONE);
    ret = dlms_getLnMessages(&p, messages);
    bb_clear(&attributeDescriptor);
    return ret;
}

int cl_readList(
    dlmsSettings* settings,
    gxArray* list,
    message* reply)
{
    gxListItem *it;
    gxObject *obj;
    unsigned short pos = 0, count;
    int ret;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    unsigned short sn;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    gxByteBuffer bb;
    if (list->size == 0)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_init(&bb);
    resetBlockIndex(settings);
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, DLMS_COMMAND_GET_REQUEST, DLMS_GET_COMMAND_TYPE_WITH_LIST,
            &bb, NULL, 0xff, DLMS_COMMAND_NONE);
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
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
            {
                return ret;
            }
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
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
            {
                return ret;
            }
            // Add variable type.
            bb_setUInt8(&bb, 2);
            sn = ((gxObject*)it->key)->shortName;
            sn += ((unsigned short)it->value - 1) * 8;
            bb_setUInt16(&bb, sn);
        }
        params_initSN(&p, settings, DLMS_COMMAND_READ_REQUEST, list->size, 0xFF, &bb, NULL, DLMS_COMMAND_NONE);
        ret = dlms_getSnMessages(&p, reply);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER
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

#ifndef DLMS_IGNORE_PROFILE_GENERIC
int cl_readRowsByEntry(dlmsSettings* settings, gxProfileGeneric* object, unsigned long index, unsigned long count, message* messages)
{
    return cl_readRowsByEntry2(settings, object, index, count, 1, 0, messages);
}

int cl_readRowsByEntry2(dlmsSettings* settings, gxProfileGeneric* object, unsigned long index, unsigned long count, unsigned short colStart, unsigned short colEnd, message* messages)
{
    dlmsVARIANT tmp;
    int ret;
    gxByteBuffer data;
    if (object == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    bb_init(&data);
    bb_setUInt8(&data, 2);  //Add AccessSelector
    bb_setUInt8(&data, DLMS_DATA_TYPE_STRUCTURE); //Add enum tag.
    bb_setUInt8(&data, 0x04); //Add item count
    var_init(&tmp);
    //Add start index
    var_setUInt32(&tmp, index);
    if ((ret = dlms_setData(&data, tmp.vt, &tmp)) != 0)
    {
        var_clear(&tmp);
        bb_clear(&data);
        return ret;
    }
    //Add Count
    if (count == 0)
    {
        var_setUInt32(&tmp, count);
    }
    else
    {
        var_setUInt32(&tmp, index + count - 1);
    }
    if ((ret = dlms_setData(&data, tmp.vt, &tmp)) != 0)
    {
        var_clear(&tmp);
        bb_clear(&data);
        return ret;
    }
    //Add columns.
    var_setUInt16(&tmp, colStart);
    if ((ret = dlms_setData(&data, tmp.vt, &tmp)) != 0)
    {
        var_clear(&tmp);
        bb_clear(&data);
        return ret;
    }
    var_setUInt16(&tmp, colEnd);
    if ((ret = dlms_setData(&data, tmp.vt, &tmp)) != 0)
    {
        var_clear(&tmp);
        bb_clear(&data);
        return ret;
    }
    var_clear(&tmp);
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
    bb_clear(&data);
    return ret;
}

int cl_readRowsByRange(dlmsSettings* settings, gxProfileGeneric* object, struct tm* start, struct tm* end, message* messages)
{
    unsigned char unixTime = 0;
    static unsigned char LN[] = { 0, 0, 1, 0, 0, 255 };
    DLMS_OBJECT_TYPE type = DLMS_OBJECT_TYPE_CLOCK;
    unsigned char* ln = LN;
    gxtime t;
    dlmsVARIANT tmp;
    int ret;
    gxByteBuffer data;
    if (object == NULL || start == NULL || end == NULL || messages == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (object->captureObjects.size != 0)
    {
        gxKey *kv;
        ret = arr_getByIndex(&object->captureObjects, 0, (void**)&kv);
        if (ret != 0)
        {
            return ret;
        }
        type = ((gxObject*)kv->key)->objectType;
        ln = ((gxObject*)kv->key)->logicalName;
        unixTime = type == DLMS_OBJECT_TYPE_DATA;
    }
    if ((ret = obj_clearProfileGenericBuffer(&object->buffer)) != 0)
    {
        return ret;
    }
    time_clear(&t);
    bb_init(&data);
    bb_setUInt8(&data, 1);  //Add AccessSelector
    bb_setUInt8(&data, DLMS_DATA_TYPE_STRUCTURE); //Add enum tag.
    bb_setUInt8(&data, 0x04); //Add item count
    bb_setUInt8(&data, DLMS_DATA_TYPE_STRUCTURE); //Add enum tag.
    bb_setUInt8(&data, 0x04); //Add item count
    // Add class_id
    var_init(&tmp);
    var_setUInt16(&tmp, type);
    if ((ret = dlms_setData(&data, tmp.vt, &tmp)) != 0)
    {
        var_clear(&tmp);
        bb_clear(&data);
        return ret;
    }
    // Add parameter Logical name
    bb_setUInt8(&data, DLMS_DATA_TYPE_OCTET_STRING); //Add enum tag.
    bb_setUInt8(&data, 6); //Add item count
    bb_set(&data, ln, 6);
    //Add attribute index.
    var_setInt8(&tmp, 2);
    if ((ret = dlms_setData(&data, tmp.vt, &tmp)) != 0)
    {
        var_clear(&tmp);
        bb_clear(&data);
        return ret;
    }
    //Add version.
    var_setUInt16(&tmp, 0);
    if ((ret = dlms_setData(&data, tmp.vt, &tmp)) != 0)
    {
        var_clear(&tmp);
        bb_clear(&data);
        return ret;
    }
    //Add start time
    if (unixTime)
    {
        var_setUInt32(&tmp, (unsigned long) time_toUnixTime(start));
        if ((ret = dlms_setData(&data, DLMS_DATA_TYPE_UINT32, &tmp)) != 0)
        {
            var_clear(&tmp);
            bb_clear(&data);
            return ret;
        }
    }
    else
    {
        time_init2(&t, start);
        var_setDateTime(&tmp, &t);
        if ((ret = dlms_setData(&data, DLMS_DATA_TYPE_OCTET_STRING, &tmp)) != 0)
        {
            var_clear(&tmp);
            bb_clear(&data);
            return ret;
        }
    }
    //Add end time
    if (unixTime)
    {
        var_setUInt32(&tmp, (unsigned long) time_toUnixTime(end));
        if ((ret = dlms_setData(&data, DLMS_DATA_TYPE_UINT32, &tmp)) != 0)
        {
            var_clear(&tmp);
            bb_clear(&data);
            return ret;
        }
    }
    else
    {
        time_init2(&t, end);
        var_setDateTime(&tmp, &t);
        if ((ret = dlms_setData(&data, DLMS_DATA_TYPE_OCTET_STRING, &tmp)) != 0)
        {
            var_clear(&tmp);
            bb_clear(&data);
            return ret;
        }
    }
    var_clear(&tmp);
    //Add array of read columns. Read All...
    //Add array type
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
    bb_clear(&data);
    return ret;
}
#endif // DLMS_IGNORE_PROFILE_GENERIC

int cl_getData(dlmsSettings *settings, gxByteBuffer *reply, gxReplyData *data)
{
    return dlms_getData2(settings, reply, data, 0);
}

int cl_changeType(
    gxByteBuffer* value,
    DLMS_DATA_TYPE type,
    dlmsVARIANT *newValue)
{
    return dlms_changeType(value, type, newValue);
}

int cl_updateValue(
    dlmsSettings* settings,
    gxObject* target,
    unsigned char attributeIndex,
    dlmsVARIANT *value)
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
    int pos, ret = 0;
    gxListItem *it;
    gxDataInfo info;
    unsigned char ch;
    gxValueEventArg e;
    ve_init(&e);
    for (pos = 0; pos != list->size; ++pos)
    {
        di_init(&info);
        if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
        {
            break;
        }
        ret = bb_getUInt8(data, &ch);
        if (ret != 0)
        {
            break;
        }
        if (ch == 0)
        {
            if ((ret = dlms_getData(data, &info, &e.value)) != 0)
            {
                break;
            }
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
    bb_init(&bb);
    // Length.
    bb_setUInt8(&bb, 0x0);
    bb_setUInt8(&bb, 0x80);
    bb_setUInt8(&bb, 0x01);
    bb_setUInt8(&bb, 0x0);
    apdu_generateUserInformation(settings, &bb);
    bb.data[0] = (unsigned char)(bb.size - 1);
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0,
            DLMS_COMMAND_RELEASE_REQUEST, DLMS_SET_COMMAND_TYPE_NORMAL,
            &bb, NULL, 0xff, DLMS_COMMAND_NONE);
        ret = dlms_getLnMessages(&p, packets);
    }
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    else
    {
        gxSNParameters p;
        params_initSN(&p, settings,
            DLMS_COMMAND_RELEASE_REQUEST, 1,
            DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME,
            NULL, &bb, DLMS_COMMAND_NONE);
        ret = dlms_getSnMessages(&p, packets);
    }
#else
    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    bb_clear(&bb);
    return ret;
}

int cl_disconnectRequest(dlmsSettings* settings, message* packets)
{
    int ret;
#ifndef DLMS_IGNORE_WRAPPER
    gxByteBuffer bb;
#endif //DLMS_IGNORE_WRAPPER
    gxByteBuffer *reply = NULL;
    mes_clear(packets);
    settings->maxPduSize = 0xFFFF;
    // If connection is not established, there is no need to send DisconnectRequest.
    if ((settings->connected & DLMS_CONNECTION_STATE_HDLC) == 0)
    {
        return 0;
    }
    settings->connected &= ~DLMS_CONNECTION_STATE_HDLC;
    reply = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
    bb_init(reply);
    if (settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        ret = dlms_getHdlcFrame(settings, DLMS_COMMAND_DISC, NULL, reply);
        if (ret == 0)
        {
            mes_push(packets, reply);
        }
        else
        {
            gxfree(reply);
        }
        return ret;
    }
#ifndef DLMS_IGNORE_WRAPPER
    bb_init(&bb);
    bb_setUInt8(&bb, DLMS_COMMAND_RELEASE_REQUEST);
    bb_setUInt8(&bb, 0x0);
    ret = dlms_getWrapperFrame(settings, &bb, reply);
    if (ret == 0)
    {
        mes_push(packets, reply);
    }
    else
    {
        gxfree(reply);
    }
    bb_clear(&bb);
#endif //DLMS_IGNORE_WRAPPER
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

int cl_writeLN(
    dlmsSettings* settings,
    unsigned char* name,
    DLMS_OBJECT_TYPE objectType,
    unsigned char index,
    dlmsVARIANT* value,
    unsigned char byteArray,
    message* messages)
{
    int ret;
    gxLNParameters p;
    gxByteBuffer bb, data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    resetBlockIndex(settings);
    bb_init(&bb);
    bb_init(&data);
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
    // Add CI.
    bb_setUInt16(&bb, objectType);
    // Add LN
    bb_set(&bb, name, 6);
    // Attribute ID.
    bb_setUInt8(&bb, index);
    // Access selection is not used.
    bb_setUInt8(&bb, 0);
    params_initLN(&p, settings, 0,
        DLMS_COMMAND_SET_REQUEST, DLMS_SET_COMMAND_TYPE_NORMAL,
        &bb, &data, 0xff, DLMS_COMMAND_NONE);
    ret = dlms_getLnMessages(&p, messages);
    bb_clear(&data);
    bb_clear(&bb);
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int cl_writeSN(
    dlmsSettings* settings,
    unsigned short address,
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
    bb_init(&bb);
    bb_init(&data);
    if ((ret = dlms_setData(&data, value->vt, value)) != 0)
    {
        return ret;
    }

    // Add name.
    address += (unsigned short)((index - 1) * 8);
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

int cl_methodLN(
    dlmsSettings* settings,
    unsigned char *name,
    DLMS_OBJECT_TYPE objectType,
    unsigned char index,
    dlmsVARIANT* value,
    message* messages)
{
    int ret;
    gxLNParameters p;
    gxByteBuffer bb, data;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    resetBlockIndex(settings);
    bb_init(&data);
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
    bb_init(&bb);
    // CI
    bb_setUInt16(&bb, objectType);
    // Add LN
    bb_set(&bb, name, 6);
    // Attribute ID.
    bb_setUInt8(&bb, index);
    // Method Invocation Parameters is not used.
    if (value == NULL || value->vt == DLMS_DATA_TYPE_NONE)
    {
        bb_setUInt8(&bb, 0);
    }
    else
    {
        bb_setUInt8(&bb, 1);
    }
    params_initLN(&p, settings, 0,
        DLMS_COMMAND_METHOD_REQUEST, DLMS_ACTION_COMMAND_TYPE_NORMAL,
        &bb, &data, 0xff, DLMS_COMMAND_NONE);
    ret = dlms_getLnMessages(&p, messages);
    bb_clear(&data);
    bb_clear(&bb);
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int cl_methodSN(
    dlmsSettings* settings,
    unsigned short address,
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
    bb_init(&data);
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
    bb_init(&bb);
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
    address += (unsigned short)index;
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

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#endif //DLMS_IGNORE_CLIENT
