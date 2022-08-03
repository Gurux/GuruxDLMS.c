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
#include "../include/serverevents.h"
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif
#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h> /* memset */
#include "../include/enums.h"
#include "../include/dlms.h"
#ifndef DLMS_IGNORE_HIGH_GMAC
#include "../include/ciphering.h"
#endif //DLMS_IGNORE_HIGH_GMAC
#include "../include/crc.h"
#include "../include/cosem.h"
#include "../include/gxobjects.h"
#include "../include/date.h"
#ifndef DLMS_IGNORE_HIGH_MD5
#include "../include/gxmd5.h"
#endif //DLMS_IGNORE_HIGH_MD5
#ifndef DLMS_IGNORE_HIGH_SHA256
#include "../include/gxsha256.h"
#endif  //DLMS_IGNORE_HIGH_SHA256
#ifndef DLMS_IGNORE_HIGH_SHA1
#include "../include/gxsha1.h"
#endif  //DLMS_IGNORE_HIGH_SHA1
#ifndef DLMS_IGNORE_HIGH_AES
#include "../include/gxaes.h"
#endif  //DLMS_IGNORE_HIGH_AES

unsigned char pduAttributes[PDU_MAX_HEADER_SIZE];
static const unsigned char LLC_SEND_BYTES[3] = { 0xE6, 0xE6, 0x00 };
static const unsigned char LLC_REPLY_BYTES[3] = { 0xE6, 0xE7, 0x00 };
static const unsigned char HDLC_FRAME_START_END = 0x7E;

unsigned char dlms_useHdlc(DLMS_INTERFACE_TYPE type)
{
#ifndef DLMS_IGNORE_HDLC
    return type == DLMS_INTERFACE_TYPE_HDLC ||
        type == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E ||
        type == DLMS_INTERFACE_TYPE_PLC_HDLC;
#else
    return 0;
#endif //DLMS_IGNORE_HDLC
}

//Makes sure that the basic settings are set.
int dlms_checkInit(dlmsSettings* settings)
{
    if (settings->clientAddress == 0)
    {
        return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
    }
    if (settings->serverAddress == 0)
    {
        return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
    }
    if (settings->maxPduSize < 64)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

#ifndef DLMS_IGNORE_HIGH_GMAC
unsigned char dlms_useDedicatedKey(dlmsSettings* settings)
{
#ifndef DLMS_IGNORE_MALLOC
    return settings->cipher.dedicatedKey != NULL;
#else
    return memcmp(settings->cipher.dedicatedKey, EMPTY_KEY, 16) != 0;
#endif //DLMS_IGNORE_MALLOC
}

unsigned char dlms_usePreEstablishedSystemTitle(dlmsSettings* settings)
{
#ifndef DLMS_IGNORE_MALLOC
    return settings->preEstablishedSystemTitle != NULL;
#else
    return memcmp(settings->preEstablishedSystemTitle, EMPTY_SYSTEM_TITLE, 8) != 0;
#endif //DLMS_IGNORE_MALLOC
}

#ifndef DLMS_IGNORE_HIGH_GMAC
/**
* Get used glo message.
*
* @param command
*            Executed DLMS command.
* @return Integer value of glo message.
*/
unsigned char dlms_getGloMessage(dlmsSettings* settings, DLMS_COMMAND command, DLMS_COMMAND encryptedCommand)
{
    unsigned char cmd;
    unsigned glo = settings->negotiatedConformance & DLMS_CONFORMANCE_GENERAL_PROTECTION;
    unsigned ded = dlms_useDedicatedKey(settings) && (settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0;
    if (encryptedCommand == DLMS_COMMAND_GENERAL_GLO_CIPHERING ||
        encryptedCommand == DLMS_COMMAND_GENERAL_DED_CIPHERING)
    {
        cmd = encryptedCommand;
    }
    else if (glo && encryptedCommand == DLMS_COMMAND_NONE)
    {
        if (ded)
        {
            cmd = DLMS_COMMAND_GENERAL_DED_CIPHERING;
        }
        else
        {
            cmd = DLMS_COMMAND_GENERAL_GLO_CIPHERING;
        }
    }
    else
    {
        switch (command)
        {
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_READ_REQUEST:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_READ_REQUEST;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_READ_REQUEST;
            }
            break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_GET_REQUEST:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_GET_REQUEST;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_GET_REQUEST;
            }
            break;
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_WRITE_REQUEST:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_WRITE_REQUEST;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_WRITE_REQUEST;
            }
            break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_SET_REQUEST:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_SET_REQUEST;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_SET_REQUEST;
            }
            break;
        case DLMS_COMMAND_METHOD_REQUEST:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_METHOD_REQUEST;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_METHOD_REQUEST;
            }
            break;
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_READ_RESPONSE:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_READ_RESPONSE;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_READ_RESPONSE;
            }
            break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_GET_RESPONSE:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_GET_RESPONSE;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_GET_RESPONSE;
            }
            break;
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_WRITE_RESPONSE:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_WRITE_RESPONSE;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_WRITE_RESPONSE;
            }
            break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_SET_RESPONSE:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_SET_RESPONSE;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_SET_RESPONSE;
            }
            break;
        case DLMS_COMMAND_METHOD_RESPONSE:
            if (ded)
            {
                cmd = DLMS_COMMAND_DED_METHOD_RESPONSE;
            }
            else
            {
                cmd = DLMS_COMMAND_GLO_METHOD_RESPONSE;
            }
            break;
        case DLMS_COMMAND_DATA_NOTIFICATION:
            if (ded)
            {
                cmd = DLMS_COMMAND_GENERAL_DED_CIPHERING;
            }
            else
            {
                cmd = DLMS_COMMAND_GENERAL_GLO_CIPHERING;
            }
            break;
        case DLMS_COMMAND_RELEASE_REQUEST:
            cmd = DLMS_COMMAND_RELEASE_REQUEST;
            break;
        case DLMS_COMMAND_RELEASE_RESPONSE:
            cmd = DLMS_COMMAND_RELEASE_RESPONSE;
            break;
        case DLMS_COMMAND_GENERAL_DED_CIPHERING:
            cmd = DLMS_COMMAND_GENERAL_DED_CIPHERING;
            break;
        case DLMS_COMMAND_GENERAL_GLO_CIPHERING:
            cmd = DLMS_COMMAND_GENERAL_GLO_CIPHERING;
            break;
        default:
            cmd = DLMS_COMMAND_NONE;
        }
    }
    return cmd;
}
#endif //DLMS_IGNORE_HIGH_GMAC
#endif //DLMS_IGNORE_HIGH_GMAC

unsigned char dlms_getInvokeIDPriority(dlmsSettings* settings, unsigned char increase)
{
    unsigned char value = 0;
    if (settings->priority == DLMS_PRIORITY_HIGH)
    {
        value |= 0x80;
    }
    if (settings->serviceClass == DLMS_SERVICE_CLASS_CONFIRMED)
    {
        value |= 0x40;
    }
    if (increase)
    {
        settings->invokeID = (unsigned char)((1 + settings->invokeID) & 0xF);
    }
    value |= settings->invokeID;
    return value;
}

/**
* Generates Invoke ID and priority.
*
* @param settings
*            DLMS settings->
* @return Invoke ID and priority.
*/
int32_t dlms_getLongInvokeIDPriority(dlmsSettings* settings)
{
    int32_t value = 0;
    if (settings->priority == DLMS_PRIORITY_HIGH)
    {
        value = 0x80000000;
    }
    if (settings->serviceClass == DLMS_SERVICE_CLASS_CONFIRMED)
    {
        value |= 0x40000000;
    }
    value |= (settings->longInvokeID & 0xFFFFFF);
    ++settings->longInvokeID;
    return value;
}

int dlms_getMaxPduSize(
    dlmsSettings* settings,
    gxByteBuffer* data,
    gxByteBuffer* bb)
{
    int offset = 0;
    int size = data->size - data->position;
    if (bb != NULL)
    {
        offset = bb->size;
    }
    if (size + offset > settings->maxPduSize)
    {
        size = settings->maxPduSize - offset;
        size -= hlp_getObjectCountSizeInBytes(size);
    }
    else if (size + hlp_getObjectCountSizeInBytes(size) > settings->maxPduSize)
    {
        size = (size - hlp_getObjectCountSizeInBytes(size));
    }
    return size;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int dlms_setData2(
    unsigned char* buff,
    uint32_t length,
    DLMS_DATA_TYPE type,
    dlmsVARIANT* value)
#else
int dlms_setData2(
    unsigned char* buff,
    uint16_t length,
    DLMS_DATA_TYPE type,
    dlmsVARIANT* value)
#endif
{
    gxByteBuffer bb;
    bb_attach(&bb, buff, length, length);
    return dlms_setData(&bb, type, value);
}

int dlms_setData(gxByteBuffer* buff, DLMS_DATA_TYPE type, dlmsVARIANT* value)
{
#ifndef DLMS_IGNORE_MALLOC
    int ret;
    ret = var_changeType(value, type);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
    return var_getBytes2(value, type, buff);
}

#ifndef DLMS_IGNORE_MALLOC
int getCount(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    if (hlp_getObjectCount2(buff, &info->count) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    value->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
    if (value->Arr == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    va_init(value->Arr);
    va_capacity(value->Arr, info->count);
    value->vt = DLMS_DATA_TYPE_ARRAY;
    return 0;
}

/**
* Get array from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* index
*            starting index.
* Returns  CGXDLMSVariant array.
*/
int getArray(gxByteBuffer* buff, gxDataInfo* info, uint16_t index, dlmsVARIANT* value)
{
    dlmsVARIANT* tmp;
    gxDataInfo info2;
    uint32_t size;
    uint16_t pos, startIndex;
    int ret;
    if (info->count == 0)
    {
        if ((ret = getCount(buff, info, value)) != 0)
        {
            return ret;
        }
    }
    size = buff->size - buff->position;
    if (info->count != 0 && size < 1)
    {
        info->complete = 0;
        return 0;
    }
    startIndex = index;
    // Position where last row was found. Cache uses this info.
    for (pos = info->index; pos != info->count; ++pos)
    {
        tmp = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
        if (tmp == NULL)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        var_init(tmp);
        di_init(&info2);
        if ((ret = dlms_getData(buff, &info2, tmp)) != 0)
        {
            var_clear(tmp);
            gxfree(tmp);
            return ret;
        }
        if (!info2.complete)
        {
            var_clear(tmp);
            gxfree(tmp);
            buff->position = startIndex;
            info->complete = 0;
            break;
        }
        else
        {
            if (info2.count == info2.index)
            {
                startIndex = (uint16_t)buff->position;
                va_push(value->Arr, tmp);
            }
        }
    }
    info->index = pos;
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MALLOC

/**
* Get UInt32 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed UInt32 value.
*/
int getUInt32(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 4)
    {
        info->complete = 0;
        return 0;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_UINT32;
    }
    if ((ret = bb_getUInt32(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->ulVal : value->pulVal)) != 0)
    {
        return ret;
    }
    return DLMS_ERROR_CODE_OK;
}

/**
* Get Int32 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed Int32 value.
*/
int getInt32(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 4)
    {
        info->complete = 0;
        return 0;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_INT32;
    }
    if ((ret = bb_getInt32(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->lVal : value->plVal)) != 0)
    {
        return ret;
    }
    return DLMS_ERROR_CODE_OK;
}

/**
* Get bit std::string value from DLMS data.
*
* buff : Received DLMS data.
* info : Data info.
* Returns parsed bit std::string value.
*/
static int getBitString(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    uint16_t cnt = 0;
#ifndef DLMS_IGNORE_MALLOC
    int ret;
#endif //DLMS_IGNORE_MALLOC
    if (hlp_getObjectCount2(buff, &cnt) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    uint16_t byteCnt = ba_getByteCount(cnt);
    // If there is not enough data available.
    if (buff->size - buff->position < byteCnt)
    {
        info->complete = 0;
        return 0;
    }
#ifdef DLMS_IGNORE_MALLOC
    if (value->vt != (DLMS_DATA_TYPE_BIT_STRING | DLMS_DATA_TYPE_BYREF))
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (value->capacity < cnt)
    {
        return DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
    }
    value->size = cnt;
    memcpy(value->pVal, buff->data + buff->position, byteCnt);
    buff->position += byteCnt;
#else
    value->bitArr = (bitArray*)gxmalloc(sizeof(bitArray));
    ba_init(value->bitArr);
    if ((ret = hlp_add(value->bitArr, buff, cnt)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

static int getBool(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    unsigned char ch;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_BOOLEAN;
        value->boolVal = ch != 0;
    }
    else
    {
        *value->pboolVal = ch != 0;
    }
    return 0;
}

/**
* Get string value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed std::string value.
*/
int getString(gxByteBuffer* buff, gxDataInfo* info, unsigned char knownType, dlmsVARIANT* value)
{
    int ret = 0;
    uint16_t len = 0;
    if (knownType)
    {
        len = (uint16_t)buff->size;
    }
    else
    {
        if (hlp_getObjectCount2(buff, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        // If there is not enough data available.
        if (buff->size - buff->position < (uint16_t)len)
        {
            info->complete = 0;
            return 0;
        }
    }
#ifdef DLMS_IGNORE_MALLOC
    if (value->vt != (DLMS_DATA_TYPE_STRING | DLMS_DATA_TYPE_BYREF))
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (value->capacity < len)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    value->size = len;
    memcpy(value->pVal, buff->data + buff->position, len);
    buff->position += len;
#else
    value->vt = DLMS_DATA_TYPE_STRING;
    if (len > 0)
    {
        value->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(value->strVal);
        ret = bb_set2(value->strVal, buff, buff->position, len);
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

#ifndef DLMS_IGNORE_MALLOC
/**
* Get UTF std::string value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed UTF std::string value.
*/
int getUtfString(
    gxByteBuffer* buff,
    gxDataInfo* info,
    unsigned char knownType,
    dlmsVARIANT* value)
{
    int ret = 0;
    uint16_t len = 0;
    var_clear(value);
    if (knownType)
    {
        len = (uint16_t)buff->size;
    }
    else
    {
        if (hlp_getObjectCount2(buff, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        // If there is not enough data available.
        if (buff->size - buff->position < (uint16_t)(2 * len))
        {
            info->complete = 0;
            return 0;
        }
    }
    if (len > 0)
    {
        value->strUtfVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(value->strUtfVal);
        ret = bb_set2(value->strUtfVal, buff, buff->position, len);
        value->vt = DLMS_DATA_TYPE_STRING_UTF8;
    }
    else
    {
        value->strUtfVal = NULL;
    }
    return ret;
}
#endif //DLMS_IGNORE_MALLOC

/**
* Get octet string value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed octet string value.
*/
int getOctetString(gxByteBuffer* buff, gxDataInfo* info, unsigned char knownType, dlmsVARIANT* value)
{
    uint16_t len;
    int ret = 0;
    if (knownType)
    {
        len = (uint16_t)buff->size;
    }
    else
    {
        if (hlp_getObjectCount2(buff, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        // If there is not enough data available.
        if (buff->size - buff->position < len)
        {
            info->complete = 0;
            return 0;
        }
    }
#ifdef DLMS_IGNORE_MALLOC
    if (value->vt != (DLMS_DATA_TYPE_OCTET_STRING | DLMS_DATA_TYPE_BYREF))
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (value->capacity < len)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    value->size = len;
    memcpy(value->pVal, buff->data + buff->position, len);
    buff->position += len;
#else
    if (len == 0)
    {
        var_clear(value);
    }
    else
    {
        ret = var_addBytes(value, buff->data + buff->position, len);
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
        buff->position += (uint32_t)len;
#else
        buff->position += (uint16_t)len;
#endif
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

/**
* Get BCD value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed BCD value.
*/
int getBcd(gxByteBuffer* buff, gxDataInfo* info, unsigned char knownType, dlmsVARIANT* value)
{
#ifndef DLMS_IGNORE_MALLOC
    unsigned char idHigh, idLow;
#endif //DLMS_IGNORE_MALLOC
    static const char hexArray[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    int ret = 0, a;
    uint16_t len;
    unsigned char ch;
    if (knownType)
    {
        len = (uint16_t)buff->size;
    }
    else
    {
        if (hlp_getObjectCount2(buff, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        // If there is not enough data available.
        if ((buff->size - buff->position) < (uint16_t)(2 * len))
        {
            info->complete = 0;
            return 0;
        }
    }
#ifdef DLMS_IGNORE_MALLOC
    value->size = 2 * len;
    if (value->vt != (DLMS_DATA_TYPE_OCTET_STRING | DLMS_DATA_TYPE_BYREF))
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    else if (value->capacity < len)
    {
        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    else
    {
        unsigned char* p = (unsigned char*)value->pVal;
        for (a = 0; a != len; ++a)
        {
            if ((ret = bb_getUInt8(buff, &ch)) != 0)
            {
                break;
            }
            *p = hexArray[ch >> 4];
            p++;
            *p = hexArray[ch & 0x0F];
            p++;
        }
    }
#else
    value->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
    BYTE_BUFFER_INIT(value->strVal);
    value->vt = DLMS_DATA_TYPE_STRING;
    bb_capacity(value->strVal, (uint16_t)(len * 2));
    for (a = 0; a != len; ++a)
    {
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            break;
        }
        idHigh = ch >> 4;
        idLow = ch & 0x0F;
        bb_setInt8(value->strVal, hexArray[idHigh]);
        bb_setInt8(value->strVal, hexArray[idLow]);
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}


/**
* Get UInt8 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed UInt8 value.
*/
int getUInt8(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getUInt8(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->bVal : value->pbVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_UINT8;
    }
    return 0;
}

/**
* Get Int16 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed Int16 value.
*/
int getInt16(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 2)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getInt16(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->iVal : value->piVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_INT16;
    }
    return 0;
}

/**
* Get Int8 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed Int8 value.
*/
int getInt8(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getInt8(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->cVal : value->pcVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_INT8;
    }
    return 0;
}


/**
* Get UInt16 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed UInt16 value.
*/
int getUInt16(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 2)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getUInt16(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->uiVal : value->puiVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_UINT16;
    }
    return 0;
}


/**
* Get Int64 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed Int64 value.
*/
int getInt64(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 8)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getInt64(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->llVal : value->pllVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_INT64;
    }
    return 0;
}


/**
* Get UInt64 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed UInt64 value.
*/
int getUInt64(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 8)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getUInt64(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->ullVal : value->pullVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_UINT64;
    }
    return 0;
}


/**
* Get enumeration value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed enumeration value.
*/
int getEnum(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getUInt8(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->bVal : value->pbVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_ENUM;
    }
    return 0;
}

#ifndef DLMS_IGNORE_FLOAT64
/**
* Get double value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed double value.
*/
int getDouble(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 8)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getDouble(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->dblVal : value->pdblVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_FLOAT64;
    }
    return 0;
}
#endif //#endif //DLMS_IGNORE_FLOAT32

#ifndef DLMS_IGNORE_FLOAT32
/**
* Get float value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed float value.
*/
int getFloat(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 4)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getFloat(buff, (value->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &value->fltVal : value->pfltVal)) != 0)
    {
        return ret;
    }
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = DLMS_DATA_TYPE_FLOAT32;
    }
    return 0;
}

#endif //DLMS_IGNORE_FLOAT32

/**
* Get time from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed time.
*/
int getTime(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    unsigned char ch, hour, minute, second;
    uint16_t ms = 0xFFFF;
    if (buff->size - buff->position < 4)
    {
        // If there is not enough data available.
        info->complete = 0;
        return 0;
    }
    // Get time.
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    hour = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    minute = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    second = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0xFF)
    {
        ms = ch * 10;
    }
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
    }
    time_init(value->dateTime, (uint16_t)-1, 0xFF, 0xFF, hour, minute, second, ms, 0x8000);
    value->vt = DLMS_DATA_TYPE_TIME;
#else
    time_init((gxtime*)value->pVal, -1, -1, -1, hour, minute, second, ms, 0x8000);
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

/**
* Get date from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed date.
*/
int getDate(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    int ret;
    unsigned char month, day;
    uint16_t year;
    unsigned char ch;
    if (buff->size - buff->position < 5)
    {
        // If there is not enough data available.
        info->complete = 0;
        return 0;
    }
    // Get year.
    if ((ret = bb_getUInt16(buff, &year)) != 0)
    {
        return ret;
    }
    // Get month
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    month = ch;
    // Get day.
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    day = ch;
    // Skip week day
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }

#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    if ((value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
    }
    time_init(value->dateTime, year, month, day, 0xFF, 0xFF, 0xFF, 0xFF, 0x8000);
    if (ch > 7)
    {
        value->dateTime->skip |= DATETIME_SKIPS_DAYOFWEEK;
    }
    value->vt = DLMS_DATA_TYPE_DATE;
#else
    time_init((gxtime*)value->pVal, year, month, day, -1, -1, -1, -1, 0x8000);
    if (ch > 7)
    {
        ((gxtime*)value->pVal)->skip |= DATETIME_SKIPS_DAYOFWEEK;
    }
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

/**
* Get date and time from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed date and time.
*/
int getDateTime(gxByteBuffer* buff, gxDataInfo* info, dlmsVARIANT* value)
{
    uint16_t year;
#ifdef DLMS_USE_EPOCH_TIME
    int ret, status;
#else
    int ret, ms, status;
#endif // DLMS_USE_EPOCH_TIME
    unsigned char ch;
    // If there is not enough data available.
    if (buff->size - buff->position < 12)
    {
        info->complete = 0;
        return 0;
    }
    // Get year.
    if ((ret = bb_getUInt16(buff, &year)) != 0)
    {
        return ret;
    }
    gxtime* t;
#if defined(DLMS_IGNORE_MALLOC)
    t = value->pVal;
#else
    if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
    {
        t = value->pVal;
    }
    else
    {
        t = value->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
        value->vt = DLMS_DATA_TYPE_DATETIME;
    }
#endif //DLMS_IGNORE_MALLOC
#ifdef DLMS_USE_EPOCH_TIME
    short deviation;
    unsigned char mon = 0, day = 0, hour = 0, min = 0, sec = 0, wday = 0, skip = 0;
    // Get month
    if ((ret = bb_getUInt8(buff, &mon)) != 0)
    {
        return ret;
    }
    // Get day
    if ((ret = bb_getUInt8(buff, &day)) != 0)
    {
        return ret;
    }
    // Skip week day
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    // Get time.
    if ((ret = bb_getUInt8(buff, &hour)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &min)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &sec)) != 0)
    {
        return ret;
    }
    //Get ms.
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getInt16(buff, &deviation)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    status = ch;
    t->status = (DLMS_CLOCK_STATUS)status;
    skip = DATETIME_SKIPS_MS;
    if (year < 1 || year == 0xFFFF)
    {
        skip |= DATETIME_SKIPS_YEAR;
        year = 1970;
    }
    if (wday > 7)
    {
        wday = 0;
        skip |= DATETIME_SKIPS_DAYOFWEEK;
    }
    if ((mon < 1 || mon > 12) && mon != 0xFE && mon != 0xFD)
    {
        skip |= DATETIME_SKIPS_MONTH;
        mon = 1;
    }
    if ((day < 1 || day > 31) && day != 0xFE && day != 0xFD)
    {
        skip |= DATETIME_SKIPS_DAY;
        day = 1;
    }
    if (hour > 24)
    {
        skip |= DATETIME_SKIPS_HOUR;
        hour = 0;
    }
    if (min > 60)
    {
        skip |= DATETIME_SKIPS_MINUTE;
        min = 0;
    }
    if (sec > 60)
    {
        skip |= DATETIME_SKIPS_SECOND;
        sec = 0;
    }
    time_init(t, year, mon, day, hour, min, sec, 0, deviation);
    t->skip = skip;
    t->status = status;
    if (status == 0xFF)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_STATUS);
        t->status = 0;
    }
#else
    t->value.tm_yday = 0;
    t->value.tm_year = year;
    // Get month
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    t->value.tm_mon = ch;
    // Get day
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    t->value.tm_mday = ch;
    // Skip week day
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    t->value.tm_wday = ch;
    // Get time.
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    t->value.tm_hour = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    t->value.tm_min = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    t->value.tm_sec = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    t->extraInfo = DLMS_DATE_TIME_EXTRA_INFO_NONE;
    t->skip = DATETIME_SKIPS_NONE;
    ms = ch;
    if (ms != 0xFF)
    {
        ms *= 10;
    }
    else
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_MS);
    }
    if ((ret = bb_getInt16(buff, &t->deviation)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch == 0xFF)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_STATUS);
        ch = 0;
    }
    status = ch;
    t->status = (DLMS_CLOCK_STATUS)status;
    if (year < 1 || year == 0xFFFF)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_YEAR);
        t->value.tm_year = 1;
    }
    else
    {
        t->value.tm_year -= 1900;
    }
    if (t->value.tm_wday < 0 || t->value.tm_wday > 7)
    {
        t->value.tm_wday = 0;
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_DAYOFWEEK);
    }
    if (t->value.tm_mon == 0xFE)
    {
        t->extraInfo |= DLMS_DATE_TIME_EXTRA_INFO_DST_BEGIN;
        t->value.tm_mon = 0;
    }
    else if (t->value.tm_mon == 0xFD)
    {
        t->extraInfo |= DLMS_DATE_TIME_EXTRA_INFO_DST_END;
        t->value.tm_mon = 0;
    }
    else if (t->value.tm_mon < 1 || t->value.tm_mon > 12)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_MONTH);
        t->value.tm_mon = 0;
    }
    else
    {
        t->value.tm_mon -= 1;
    }
    if (t->value.tm_mday == 0xFD)
    {
        // 2nd last day of month.
        t->value.tm_mday = 1;
        t->extraInfo |= DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY2;
    }
    else if (t->value.tm_mday == 0xFE)
    {
        //Last day of month
        t->value.tm_mday = 1;
        t->extraInfo |= DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY;
    }
    else if (t->value.tm_mday == -1 || t->value.tm_mday == 0 || t->value.tm_mday > 31)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_DAY);
        t->value.tm_mday = 1;
    }
    if (t->value.tm_hour < 0 || t->value.tm_hour > 24)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_HOUR);
        t->value.tm_hour = 0;
    }
    if (t->value.tm_min < 0 || t->value.tm_min > 60)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_MINUTE);
        t->value.tm_min = 0;
    }
    if (t->value.tm_sec < 0 || t->value.tm_sec > 60)
    {
        t->skip = (DATETIME_SKIPS)(t->skip | DATETIME_SKIPS_SECOND);
        t->value.tm_sec = 0;
    }
    t->value.tm_isdst = (status & DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE);
    mktime(&t->value);
#endif //DLMS_USE_EPOCH_TIME
    return 0;
}

#ifndef DLMS_IGNORE_MALLOC

int getCompactArrayItem(
    gxByteBuffer* buff,
    DLMS_DATA_TYPE dt,
    variantArray* list,
    uint32_t len)
{
    int ret;
    gxDataInfo tmp;
    di_init(&tmp);
    tmp.type = dt;
    uint32_t start = buff->position;
    dlmsVARIANT* value = gxmalloc(sizeof(dlmsVARIANT));
    if (value == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    var_init(value);
    if (dt == DLMS_DATA_TYPE_STRING)
    {
        while (buff->position - start < len)
        {
            var_clear(value);
            di_init(&tmp);
            tmp.type = dt;
            if ((ret = getString(buff, &tmp, 0, value)) != 0)
            {
                return ret;
            }
            va_push(list, value);
            if (!tmp.complete)
            {
                break;
            }
        }
    }
    else if (dt == DLMS_DATA_TYPE_OCTET_STRING)
    {
        while (buff->position - start < len)
        {
            var_clear(value);
            di_init(&tmp);
            tmp.type = dt;
            if ((ret = getOctetString(buff, &tmp, 0, value)) != 0)
            {
                return ret;
            }
            va_push(list, value);
            if (!tmp.complete)
            {
                break;
            }
        }
    }
    else
    {
        while (buff->position - start < len)
        {
            var_clear(value);
            di_init(&tmp);
            tmp.type = dt;
            if ((ret = dlms_getData(buff, &tmp, value)) != 0)
            {
                return ret;
            }
            va_push(list, value);
            if (!tmp.complete)
            {
                break;
            }
        }
    }
    return 0;
}

int getCompactArrayItem2(
    gxByteBuffer* buff,
    variantArray* dt,
    variantArray* list,
    int len)
{
    int ret, pos;
    dlmsVARIANT* tmp = gxmalloc(sizeof(dlmsVARIANT));
    if (tmp == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    dlmsVARIANT* it;
    var_init(tmp);
    tmp->vt = DLMS_DATA_TYPE_ARRAY;
    tmp->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
    if (tmp->Arr == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    va_init(tmp->Arr);
    for (pos = 0; pos != dt->size; ++pos)
    {
        if ((ret = va_getByIndex(dt, pos, &it)) != 0)
        {
            var_clear(tmp);
            gxfree(tmp);
            return ret;
        }
        if (it->vt == DLMS_DATA_TYPE_ARRAY || it->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            if ((ret = getCompactArrayItem2(buff, it->Arr, tmp->Arr, 1)) != 0)
            {
                var_clear(tmp);
                gxfree(tmp);
                return ret;
            }
        }
        else
        {
            if ((ret = getCompactArrayItem(buff, (DLMS_DATA_TYPE)it->bVal, tmp->Arr, 1)) != 0)
            {
                var_clear(tmp);
                gxfree(tmp);
                return ret;
            }
        }
    }
    va_push(list, tmp);
    return 0;
}

int getDataTypes(
    gxByteBuffer* buff,
    variantArray* cols,
    int len)
{
    int ret;
    unsigned char ch;
    uint16_t cnt;
    DLMS_DATA_TYPE dt;
    if (cols->size == 0)
    {
        va_capacity(cols, (uint16_t)len);
    }
    for (int pos = 0; pos != len; ++pos)
    {
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        dt = (DLMS_DATA_TYPE)ch;
        if (dt == DLMS_DATA_TYPE_ARRAY)
        {
            if ((ret = bb_getUInt16(buff, &cnt)) != 0)
            {
                return ret;
            }
            dlmsVARIANT tmp;
            dlmsVARIANT* it;
            var_init(&tmp);
            tmp.Arr = (variantArray*)gxmalloc(sizeof(variantArray));
            tmp.vt = DLMS_DATA_TYPE_ARRAY;
            if (tmp.Arr == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            va_init(tmp.Arr);
            getDataTypes(buff, tmp.Arr, 1);
            if ((ret = va_getByIndex(tmp.Arr, 0, &it)) != 0)
            {
                va_clear(cols);
                return ret;
            }
            dlmsVARIANT* tmp2 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
            if (tmp2 == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            var_init(tmp2);
            tmp2->vt = DLMS_DATA_TYPE_ARRAY;
            tmp2->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
            if (tmp2->Arr == NULL)
            {
                var_clear(tmp2);
                gxfree(tmp2);
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            va_init(tmp2->Arr);
            for (int i = 0; i != cnt; ++i)
            {
                dlmsVARIANT* tmp3 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                if (tmp3 == NULL)
                {
                    var_clear(tmp2);
                    gxfree(tmp2);
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                var_init(tmp3);
                if ((ret = var_copy(tmp3, it)) != 0)
                {
                    var_clear(tmp3);
                    gxfree(tmp3);
                    var_clear(tmp2);
                    gxfree(tmp2);
                    return ret;
                }
                va_push(tmp2->Arr, tmp3);
            }
            var_clear(&tmp);
            va_push(cols, tmp2);
        }
        else if (dt == DLMS_DATA_TYPE_STRUCTURE)
        {
            dlmsVARIANT* tmp = gxmalloc(sizeof(dlmsVARIANT));
            if (tmp == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            var_init(tmp);
            if ((ret = bb_getUInt8(buff, &ch)) != 0)
            {
                va_clear(cols);
                var_clear(tmp);
                gxfree(tmp);
                return ret;
            }
            tmp->vt = DLMS_DATA_TYPE_STRUCTURE;
            tmp->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
            if (tmp->Arr == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            va_init(tmp->Arr);
            getDataTypes(buff, tmp->Arr, ch);
            va_push(cols, tmp);
        }
        else
        {
            dlmsVARIANT* tmp = gxmalloc(sizeof(dlmsVARIANT));
            if (tmp == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            var_init(tmp);
            if (cols->size == 0)
            {
                va_capacity(cols, 1);
            }
            var_setUInt8(tmp, dt);
            va_push(cols, tmp);
        }
    }
    return 0;
}

//Get compact array value from DLMS data.
int getCompactArray(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    gxDataInfo* info,
    dlmsVARIANT* value,
    unsigned char onlyDataTypes)
{
    int ret, pos;
    uint16_t len;
    unsigned char ch;
    var_clear(value);
    // If there is not enough data available.
    if (buff->size - buff->position < 2)
    {
        info->complete = 0;
        return 0;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    DLMS_DATA_TYPE dt = (DLMS_DATA_TYPE)ch;
    if (dt == DLMS_DATA_TYPE_ARRAY)
    {
        //Invalid compact array data.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = hlp_getObjectCount2(buff, &len)) != 0)
    {
        return ret;
    }
    value->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
    value->vt = DLMS_DATA_TYPE_ARRAY;
    if (value->Arr == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    va_init(value->Arr);
    if (dt == DLMS_DATA_TYPE_STRUCTURE)
    {
        // Get data types.
        variantArray cols;
        va_init(&cols);
        if ((ret = getDataTypes(buff, &cols, len)) != 0)
        {
            va_clear(&cols);
            return ret;
        }
        if (onlyDataTypes)
        {
            va_attach2(value->Arr, &cols);
            return 0;
        }
        if (buff->position == buff->size)
        {
            len = 0;
        }
        else
        {
            if ((ret = hlp_getObjectCount2(buff, &len)) != 0)
            {
                va_clear(&cols);
                return ret;
            }
        }
        int start = buff->position;
        while (buff->position - start < len)
        {
            dlmsVARIANT* it, * it2, * it3, * it4;
            variantArray* row = (variantArray*)gxmalloc(sizeof(variantArray));
            if (row == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            va_init(row);
            for (pos = 0; pos != cols.size; ++pos)
            {
                if ((ret = va_getByIndex(&cols, pos, &it)) != 0)
                {
                    va_clear(&cols);
                    va_clear(row);
                    gxfree(row);
                    return ret;
                }
                if (it->vt == DLMS_DATA_TYPE_STRUCTURE)
                {
                    if ((ret = getCompactArrayItem2(buff, it->Arr, row, 1)) != 0)
                    {
                        va_clear(&cols);
                        va_clear(row);
                        gxfree(row);
                        return ret;
                    }
                }
                else if (it->vt == DLMS_DATA_TYPE_ARRAY)
                {
                    int pos1;
                    variantArray tmp2;
                    va_init(&tmp2);
#ifdef DLMS_ITALIAN_STANDARD
                    //Some Italy meters require that there is a array count in data.
                    if (info->appendAA)
                    {
                        if ((ret = hlp_getObjectCount2(buff, &len)) != 0)
                        {
                            va_clear(&cols);
                            return ret;
                        }
                        if (it->Arr->size != len)
                        {
                            return DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                    }
#endif //DLMS_ITALIAN_STANDARD

                    if ((ret = getCompactArrayItem2(buff, it->Arr, &tmp2, 1)) != 0 ||
                        (ret = va_getByIndex(&tmp2, 0, &it2)) != 0)
                    {
                        va_clear(&tmp2);
                        va_clear(&cols);
                        va_clear(row);
                        gxfree(row);
                        return ret;
                    }
                    for (pos1 = 0; pos1 != it2->Arr->size; ++pos1)
                    {
                        if ((ret = va_getByIndex(it2->Arr, pos1, &it3)) != 0)
                        {
                            va_clear(&tmp2);
                            va_clear(&cols);
                            va_clear(row);
                            gxfree(row);
                            return ret;
                        }
                        it4 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                        if (it4 == NULL)
                        {
                            return DLMS_ERROR_CODE_OUTOFMEMORY;
                        }
                        var_init(it4);
                        if ((ret = var_copy(it4, it3)) != 0)
                        {
                            return ret;
                        }
                        va_push(row, it4);
                    }
                    va_clear(&tmp2);
                }
                else
                {
                    if ((ret = getCompactArrayItem(buff, (DLMS_DATA_TYPE)it->bVal, row, 1)) != 0)
                    {
                        va_clear(&cols);
                        va_clear(row);
                        gxfree(row);
                        return ret;
                    }
                }
                if (buff->position == buff->size)
                {
                    break;
                }
            }
            //If all columns are read.
            if (row->size >= cols.size)
            {
                dlmsVARIANT* tmp = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                var_clear(tmp);
                tmp->Arr = row;
                tmp->vt = DLMS_DATA_TYPE_ARRAY;
                va_push(value->Arr, tmp);
            }
            else
            {
                break;
            }
        }
        va_clear(&cols);
        return 0;
    }
    return getCompactArrayItem(buff, dt, value->Arr, len);
}
#endif //DLMS_IGNORE_MALLOC

int dlms_getData(gxByteBuffer* data, gxDataInfo* info, dlmsVARIANT* value)
{
    unsigned char ch, knownType;
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    uint32_t startIndex = data->position;
    var_clear(value);
#endif //DLMS_IGNORE_MALLOC
    if (data->position == data->size)
    {
        info->complete = 0;
        return 0;
    }
    info->complete = 1;
    knownType = info->type != DLMS_DATA_TYPE_NONE;
    if (!knownType)
    {
        ret = bb_getUInt8(data, &ch);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        info->type = (DLMS_DATA_TYPE)ch;
    }
    if (info->type == DLMS_DATA_TYPE_NONE)
    {
        //Do nothing.
        return DLMS_ERROR_CODE_OK;
    }
    if (data->position == data->size)
    {
        info->complete = 0;
        return 0;
    }
    switch (info->type & ~DLMS_DATA_TYPE_BYREF)
    {
    case DLMS_DATA_TYPE_ARRAY:
    case DLMS_DATA_TYPE_STRUCTURE:
    {
#if !defined(DLMS_IGNORE_MALLOC)
        ret = getArray(data, info, (uint16_t)startIndex, value);
        value->vt = info->type;
#else
        if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            uint16_t count, pos;
            if ((ret = hlp_getObjectCount2(data, &count)) != 0)
            {
                return ret;
            }
            //If user try to write too many items.
            if (va_getCapacity(value->Arr) < count)
            {
                return DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
            }
            value->Arr->size = count;
            for (pos = 0; pos != count; ++pos)
            {
                gxDataInfo info2;
                di_init(&info2);
                dlmsVARIANT* value2;
                if ((ret = va_getByIndex(value->Arr, pos, &value2)) != 0 ||
                    (ret = dlms_getData(data, &info2, value2)) != 0)
                {
                    return ret;
                }
            }
        }
        else
        {
            value->vt = DLMS_DATA_TYPE_OCTET_STRING;
            --data->position;
            value->byteArr = data;
        }
        return 0;
#endif //#!defined(DLMS_IGNORE_MALLOC)
        break;
    }
    case DLMS_DATA_TYPE_BOOLEAN:
    {
        ret = getBool(data, info, value);
        break;
    }
    case DLMS_DATA_TYPE_BIT_STRING:
#if !defined(DLMS_IGNORE_MALLOC)
        ret = getBitString(data, info, value);
#else
        if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = getBitString(data, info, value);
        }
        else
        {
            value->vt = DLMS_DATA_TYPE_OCTET_STRING;
            --data->position;
            value->byteArr = data;
            return 0;
        }
#endif //!defined(DLMS_IGNORE_MALLOC)
        break;
    case DLMS_DATA_TYPE_INT32:
        ret = getInt32(data, info, value);
        break;
    case DLMS_DATA_TYPE_UINT32:
        ret = getUInt32(data, info, value);
        break;
    case DLMS_DATA_TYPE_STRING:
#if !defined(DLMS_IGNORE_MALLOC)
        ret = getString(data, info, knownType, value);
#else
        if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = getString(data, info, 0, value);
        }
        else
        {
            value->vt = DLMS_DATA_TYPE_OCTET_STRING;
            --data->position;
            value->byteArr = data;
            ret = 0;
        }
#endif //!defined(DLMS_IGNORE_MALLOC)
        break;
    case DLMS_DATA_TYPE_STRING_UTF8:
#if !defined(DLMS_IGNORE_MALLOC)
        ret = getUtfString(data, info, knownType, value);
#else
        value->vt = DLMS_DATA_TYPE_OCTET_STRING;
        --data->position;
        value->byteArr = data;
        return 0;
#endif //!defined(DLMS_IGNORE_MALLOC)
        break;
    case DLMS_DATA_TYPE_OCTET_STRING:
#if !defined(DLMS_IGNORE_MALLOC)
        ret = getOctetString(data, info, knownType, value);
#else
        if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = getOctetString(data, info, 0, value);
        }
        else
        {
            value->vt = DLMS_DATA_TYPE_OCTET_STRING;
            --data->position;
            value->byteArr = data;
            ret = 0;
        }
#endif // DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
        ret = getBcd(data, info, knownType, value);
        break;
    case DLMS_DATA_TYPE_INT8:
        ret = getInt8(data, info, value);
        break;
    case DLMS_DATA_TYPE_INT16:
        ret = getInt16(data, info, value);
        break;
    case DLMS_DATA_TYPE_UINT8:
        ret = getUInt8(data, info, value);
        break;
    case DLMS_DATA_TYPE_UINT16:
        ret = getUInt16(data, info, value);
        break;
    case DLMS_DATA_TYPE_COMPACT_ARRAY:
#ifndef DLMS_IGNORE_MALLOC
        ret = getCompactArray(NULL, data, info, value, 0);
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_INT64:
        ret = getInt64(data, info, value);
        break;
    case DLMS_DATA_TYPE_UINT64:
        ret = getUInt64(data, info, value);
        break;
    case DLMS_DATA_TYPE_ENUM:
        ret = getEnum(data, info, value);
        break;
#ifndef DLMS_IGNORE_FLOAT32
    case DLMS_DATA_TYPE_FLOAT32:
        ret = getFloat(data, info, value);
        break;
#endif //DLMS_IGNORE_FLOAT32
#ifndef DLMS_IGNORE_FLOAT64
    case DLMS_DATA_TYPE_FLOAT64:
        ret = getDouble(data, info, value);
        break;
#endif //DLMS_IGNORE_FLOAT64
    case DLMS_DATA_TYPE_DATETIME:
#if defined(DLMS_IGNORE_MALLOC)
        if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = getDateTime(data, info, value);
        }
        else
        {
            value->vt = DLMS_DATA_TYPE_OCTET_STRING;
            --data->position;
            value->byteArr = data;
            ret = 0;
        }
#else
        ret = getDateTime(data, info, value);
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_DATE:
#if defined(DLMS_IGNORE_MALLOC)
        if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = getDate(data, info, value);
        }
        else
        {
            value->vt = DLMS_DATA_TYPE_OCTET_STRING;
            --data->position;
            value->byteArr = data;
            ret = 0;
        }
#else
        ret = getDate(data, info, value);
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_TIME:
#if defined(DLMS_IGNORE_MALLOC)
        if ((value->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = getTime(data, info, value);
        }
        else
        {
            value->vt = DLMS_DATA_TYPE_OCTET_STRING;
            --data->position;
            value->byteArr = data;
            ret = 0;
        }
#else
        ret = getTime(data, info, value);
#endif //DLMS_IGNORE_MALLOC
        break;
    default:
#ifdef _DEBUG
        //Assert in debug version.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
#endif
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifdef DLMS_IGNORE_MALLOC
    if (ret == 0 && (value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = info->type;
    }
#else
    if (ret == 0 && (value->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        value->vt = info->type;
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

#ifndef DLMS_IGNORE_HDLC

//Return DLMS_ERROR_CODE_FALSE if LLC bytes are not included.
int dlms_checkLLCBytes(dlmsSettings* settings, gxByteBuffer* data)
{
    if (settings->server)
    {
        //Check LLC bytes.
        if (memcmp(data->data + data->position, LLC_SEND_BYTES, 3) != 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else
    {
        //Check LLC bytes.
        if (memcmp(data->data + data->position, LLC_REPLY_BYTES, 3) != 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    data->position += 3;
    return DLMS_ERROR_CODE_OK;
}

int dlms_getHDLCAddress(
    gxByteBuffer* buff,
    uint32_t* address,
    unsigned char checkClientAddress)
{
    unsigned char ch;
    uint16_t s, pos;
    uint32_t l;
    int ret, size = 0;
    for (pos = (uint16_t)buff->position; pos != (uint16_t)buff->size; ++pos)
    {
        ++size;
        if ((ret = bb_getUInt8ByIndex(buff, pos, &ch)) != 0)
        {
            return ret;
        }
        if ((ch & 0x1) == 1)
        {
            break;
        }
    }
    //DLMS CCT test requires that client size is one byte.
    if (checkClientAddress && size != 1)
    {
        return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
    }

    if (size == 1)
    {
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        *address = ((ch & 0xFE) >> 1);
    }
    else if (size == 2)
    {
        if ((ret = bb_getUInt16(buff, &s)) != 0)
        {
            return ret;
        }
        *address = ((s & 0xFE) >> 1) | ((s & 0xFE00) >> 2);
    }
    else if (size == 4)
    {
        if ((ret = bb_getUInt32(buff, &l)) != 0)
        {
            return ret;
        }
        *address = ((l & 0xFE) >> 1) | ((l & 0xFE00) >> 2)
            | ((l & 0xFE0000) >> 3) | ((l & 0xFE000000) >> 4);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

/**
* Check that client and server address match.
*
* @param server
*            Is server.
* @param settings
*            DLMS settings.
* @param reply
*            Received data.
* @param index
*            Position.
* @return True, if client and server address match.
*/
int dlms_checkHdlcAddress(
    unsigned char server,
    dlmsSettings* settings,
    gxByteBuffer* reply,
    uint16_t index)
{
    unsigned char ch;
    int ret;
    uint32_t source, target;
    // Get destination and source addresses.
    if ((ret = dlms_getHDLCAddress(reply, &target, 0)) != 0)
    {
        return ret;
    }
    if ((ret = dlms_getHDLCAddress(reply, &source, server)) != 0)
    {
        return ret;
    }
    if (server)
    {
        // Check that server addresses match.
        if (settings->serverAddress != 0 && settings->serverAddress != target)
        {
            // Get frame command.
            if (bb_getUInt8ByIndex(reply, reply->position, &ch) != 0)
            {
                return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
            }
            return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
        }
        else
        {
            settings->serverAddress = target;
        }

        // Check that client addresses match.
        if (settings->clientAddress != 0 && settings->clientAddress != source)
        {
            // Get frame command.
            if (bb_getUInt8ByIndex(reply, reply->position, &ch) != 0)
            {
                return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
            }
            //If SNRM and client has not call disconnect and changes client ID.
            if (ch == DLMS_COMMAND_SNRM)
            {
                settings->clientAddress = (uint16_t)source;
            }
            else
            {
                return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
            }
        }
        else
        {
            settings->clientAddress = (uint16_t)source;
        }
    }
    else
    {
        // Check that client addresses match.
        if (settings->clientAddress != target)
        {
            // If echo.
            if (settings->clientAddress == source && settings->serverAddress == target)
            {
                reply->position = index + 1;
            }
            return DLMS_ERROR_CODE_FALSE;
        }
        // Check that server addresses match.
        if (settings->serverAddress != source &&
            // If All-station (Broadcast).
            (settings->serverAddress & 0x7F) != 0x7F && (settings->serverAddress & 0x3FFF) != 0x3FFF)
        {
            return DLMS_ERROR_CODE_FALSE;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int dlms_getAddress(int32_t value, uint32_t* address, int* size)
{
    if (value < 0x80)
    {
        *address = (unsigned char)(value << 1 | 1);
        *size = 1;
        return 0;
    }
    else if (value < 0x4000)
    {
        *address = (uint16_t)((value & 0x3F80) << 2 | (value & 0x7F) << 1 | 1);
        *size = 2;
    }
    else if (value < 0x10000000)
    {
        *address = (uint32_t)((value & 0xFE00000) << 4 | (value & 0x1FC000) << 3
            | (value & 0x3F80) << 2 | (value & 0x7F) << 1 | 1);
        *size = 4;
    }
    else
    {
        //Invalid address
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

/**
* Get HDLC address as bytes.
*/
int dlms_getAddressBytes(
    uint32_t value,
    gxByteBuffer* bytes)
{
    int ret, size;
    uint32_t address;
    if ((ret = dlms_getAddress(value, &address, &size)) != 0)
    {
        return ret;
    }
    if (size == 1)
    {
        bb_setUInt8(bytes, (unsigned char)address);
    }
    else if (size == 2)
    {
        bb_setUInt16(bytes, (uint16_t)address);
    }
    else if (size == 4)
    {
        bb_setUInt32(bytes, address);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

int dlms_getHdlcFrame(
    dlmsSettings* settings,
    int frame,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    unsigned char tmp[4], tmp2[4];
    uint16_t crc;
    int ret;
    uint16_t frameSize;
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t len = 0;
#else
    uint16_t len = 0;
#endif
    gxByteBuffer primaryAddress, secondaryAddress;
    bb_clear(reply);
    bb_attach(&primaryAddress, tmp, 0, 4);
    bb_attach(&secondaryAddress, tmp2, 0, 4);
    if (settings->server)
    {
        if ((ret = bb_capacity(&primaryAddress, 1)) == 0 &&
            (ret = bb_capacity(&secondaryAddress, 4)) == 0)
        {
            if ((frame == 0x13 || frame == 0x3) && ((dlmsServerSettings*)settings)->pushClientAddress != 0)
            {
                ret = dlms_getAddressBytes(((dlmsServerSettings*)settings)->pushClientAddress, &primaryAddress);
            }
            else
            {
                ret = dlms_getAddressBytes(settings->clientAddress, &primaryAddress);
            }
            if (ret == 0)
            {
                ret = dlms_getAddressBytes(settings->serverAddress, &secondaryAddress);
            }
        }
    }
    else
    {
        ret = bb_capacity(&primaryAddress, 4);
        if (ret == 0)
        {
            ret = bb_capacity(&secondaryAddress, 1);
        }
        if (ret == 0 && (ret = dlms_getAddressBytes(settings->serverAddress, &primaryAddress)) == 0)
        {
            ret = dlms_getAddressBytes(settings->clientAddress, &secondaryAddress);
        }
    }

    // Add BOP
    if (ret == 0 && (ret = bb_capacity(reply, 8)) == 0)
    {
        ret = bb_setUInt8(reply, HDLC_FRAME_START_END);
    }

    frameSize = settings->maxInfoTX;
    if (data != NULL && data->position == 0)
    {
        frameSize -= 3;
    }
    // If no data
    if (ret == 0 && (data == NULL || data->size == 0))
    {
        len = 0;
        ret = bb_setUInt8(reply, 0xA0);
    }
    else if (ret == 0 && data->size - data->position <= frameSize)
    {
        // Is last packet.
        len = bb_available(data);
        ret = bb_setUInt8(reply, (unsigned char)(0xA0 | (((7 + primaryAddress.size + secondaryAddress.size + len) >> 8) & 0x7)));
    }
    else if (ret == 0)
    {
        // More data to left.
        len = frameSize;
        ret = bb_setUInt8(reply, (unsigned char)(0xA8 | (((7 + primaryAddress.size + secondaryAddress.size + len) >> 8) & 0x7)));
    }
    // Frame len.
    if (ret == 0 && len == 0)
    {
        ret = bb_setUInt8(reply, (unsigned char)(5 + primaryAddress.size + secondaryAddress.size + len));
    }
    else if (ret == 0)
    {
        if ((ret = bb_capacity(reply, (uint16_t)(11 + len))) == 0)
        {
            ret = bb_setUInt8(reply, (unsigned char)(7 + primaryAddress.size + secondaryAddress.size + len));
        }
    }
    // Add primary address.
    if (ret == 0 && (ret = bb_set2(reply, &primaryAddress, 0, primaryAddress.size)) == 0)
    {
        // Add secondary address.
        ret = bb_set2(reply, &secondaryAddress, 0, secondaryAddress.size);
    }

    // Add frame ID.
    if (ret == 0 && frame == 0)
    {
        ret = bb_setUInt8(reply, getNextSend(settings, 1));
    }
    else if (ret == 0)
    {
        ret = bb_setUInt8(reply, (unsigned char)frame);
    }
    if (ret == 0)
    {
        // Add header CRC.
        crc = countCRC(reply, 1, (reply->size - 1));
        ret = bb_setUInt16(reply, crc);
    }
    if (ret == 0 && len != 0)
    {
        // Add data.
        if ((ret = bb_set2(reply, data, data->position, len)) == 0)
        {
            // Add data CRC.
            crc = countCRC(reply, 1, (reply->size - 1));
            ret = bb_setUInt16(reply, crc);
        }
    }
    // Add EOP
    if (ret == 0)
    {
        ret = bb_setUInt8(reply, HDLC_FRAME_START_END);
    }
    // Remove sent data in server side.
    if (ret == 0 && settings->server)
    {
        if (data != NULL)
        {
            //If all data is sent.
            if (data->size == data->position)
            {
                ret = bb_clear(data);
            }
            else
            {
                //Remove sent data.
                ret = bb_move(data, data->position, 0, data->size - data->position);
                data->position = 0;
            }
        }
    }
    bb_clear(&primaryAddress);
    bb_clear(&secondaryAddress);
    return ret;
}
#endif //DLMS_IGNORE_HDLC

#ifndef DLMS_IGNORE_PLC
int dlms_getPlcFrame(
    dlmsSettings* settings,
    unsigned char creditFields,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    int frameSize = bb_available(data);
    //Max frame size is 124 bytes.
    if (frameSize > 134)
    {
        frameSize = 134;
    }
    //PAD Length.
    unsigned char padLen = (unsigned char)((36 - ((11 + frameSize) % 36)) % 36);
    bb_capacity(reply, 15 + frameSize + padLen);
    //Add STX
    bb_setUInt8(reply, 2);
    //Length.
    bb_setUInt8(reply, (unsigned char)(11 + frameSize));
    //Length.
    bb_setUInt8(reply, 0x50);
    //Add  Credit fields.
    bb_setUInt8(reply, creditFields);
    //Add source and target MAC addresses.
    bb_setUInt8(reply, (unsigned char)(settings->plcSettings.macSourceAddress >> 4));
    int val = settings->plcSettings.macSourceAddress << 12;
    val |= settings->plcSettings.macDestinationAddress & 0xFFF;
    bb_setUInt16(reply, (uint16_t)val);
    bb_setUInt8(reply, padLen);
    //Control byte.
    bb_setUInt8(reply, DLMS_PLC_DATA_LINK_DATA_REQUEST);
    bb_setUInt8(reply, (unsigned char)settings->serverAddress);
    bb_setUInt8(reply, (unsigned char)settings->clientAddress);
    bb_set(reply, data->data + data->position, frameSize);
    data->position += frameSize;
    //Add padding.
    while (padLen != 0)
    {
        bb_setUInt8(reply, 0);
        --padLen;
    }
    //Checksum.
    uint16_t crc = countCRC(reply, 0, reply->size);
    bb_setUInt16(reply, crc);
    //Remove sent data in server side.
    if (settings->server)
    {
        if (data->size == data->position)
        {
            bb_clear(data);
        }
        else
        {
            bb_move(data, data->position, 0, data->size - data->position);
            data->position = 0;
        }
    }
    return 0;
}

// Reserved for internal use.
const uint32_t CRCPOLY = 0xD3B6BA00;
uint32_t dlms_countFCS24(unsigned char* buff, int index, int count)
{
    unsigned char i, j;
    uint32_t crcreg = 0;
    for (j = 0; j < count; ++j)
    {
        unsigned char b = buff[index + j];
        for (i = 0; i < 8; ++i)
        {
            crcreg >>= 1;
            if ((b & 0x80) != 0)
            {
                crcreg |= 0x80000000;
            }
            if ((crcreg & 0x80) != 0)
            {
                crcreg = crcreg ^ CRCPOLY;
            }
            b <<= 1;
        }
    }
    return crcreg >> 8;
}

int dlms_getMacHdlcFrame(
    dlmsSettings* settings,
    unsigned char frame,
    unsigned char creditFields,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    if (settings->maxInfoTX > 126)
    {
        settings->maxInfoTX = 86;
    };
    int ret;
    gxByteBuffer tmp;
    BYTE_BUFFER_INIT(&tmp);
    //Lenght is updated last.
    bb_setUInt16(reply, 0);
    //Add  Credit fields.
    bb_setUInt8(reply, creditFields);
    //Add source and target MAC addresses.
    bb_setUInt8(reply, (unsigned char)(settings->plcSettings.macSourceAddress >> 4));
    int val = settings->plcSettings.macSourceAddress << 12;
    val |= settings->plcSettings.macDestinationAddress & 0xFFF;
    bb_setUInt16(reply, (uint16_t)val);
    if ((ret = dlms_getHdlcFrame(settings, frame, data, &tmp)) == 0)
    {
        unsigned char padLen = (unsigned char)((36 - ((10 + tmp.size) % 36)) % 36);
        bb_setUInt8(reply, padLen);
        bb_set(reply, tmp.data, tmp.size);
        //Add padding.
        while (padLen != 0)
        {
            bb_setUInt8(reply, 0);
            --padLen;
        }
        //Checksum.
        uint32_t crc = dlms_countFCS24(reply->data, 2, reply->size - 2 - padLen);
        bb_setUInt8(reply, (unsigned char)(crc >> 16));
        bb_setUInt16(reply, (uint16_t)crc);
        //Add NC
        val = reply->size / 36;
        if (reply->size % 36 != 0)
        {
            ++val;
        }
        if (val == 1)
        {
            val = DLMS_PLC_MAC_SUB_FRAMES_ONE;
        }
        else if (val == 2)
        {
            val = DLMS_PLC_MAC_SUB_FRAMES_TWO;
        }
        else if (val == 3)
        {
            val = DLMS_PLC_MAC_SUB_FRAMES_THREE;
        }
        else if (val == 4)
        {
            val = DLMS_PLC_MAC_SUB_FRAMES_FOUR;
        }
        else if (val == 5)
        {
            val = DLMS_PLC_MAC_SUB_FRAMES_FIVE;
        }
        else if (val == 6)
        {
            val = DLMS_PLC_MAC_SUB_FRAMES_SIX;
        }
        else if (val == 7)
        {
            val = DLMS_PLC_MAC_SUB_FRAMES_SEVEN;
        }
        else
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        ret = bb_setUInt16ByIndex(reply, 0, (uint16_t)val);
    }
    return ret;
}

int dlms_getMacFrame(
    dlmsSettings* settings,
    unsigned char frame,
    unsigned char creditFields,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    if (settings->interfaceType == DLMS_INTERFACE_TYPE_PLC)
    {
        return dlms_getPlcFrame(settings, creditFields, data, reply);
    }
    return dlms_getMacHdlcFrame(settings, frame, creditFields, data, reply);
}

#endif //DLMS_IGNORE_PLC

int dlms_getDataFromFrame(
    gxByteBuffer* reply,
    gxReplyData* data,
    unsigned char hdlc)
{
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t offset = data->data.size;
    uint32_t cnt;
#else
    uint16_t offset = data->data.size;
    uint16_t cnt;
#endif
    if (data->packetLength < reply->position)
    {
        cnt = 0;
    }
    else
    {
        cnt = data->packetLength - reply->position;
    }
    if (cnt != 0)
    {
        int ret;
        if ((ret = bb_capacity(&data->data, offset + cnt)) != 0 ||
            (ret = bb_set2(&data->data, reply, reply->position, cnt)) != 0)
        {
            return ret;
        }
        if (hdlc)
        {
            reply->position += 3;
        }
    }
    // Set position to begin of new data.
    data->data.position = offset;
    return 0;
}

#ifndef DLMS_IGNORE_HDLC

int dlms_getHdlcData(
    unsigned char server,
    dlmsSettings* settings,
    gxByteBuffer* reply,
    gxReplyData* data,
    unsigned char* frame,
    unsigned char preEstablished,
    unsigned char first)
{
    int ret;
    unsigned char ch;
    uint16_t eopPos;
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t pos, frameLen = 0;
    uint32_t packetStartID = reply->position;
#else
    uint16_t pos, frameLen = 0;
    uint16_t packetStartID = (uint16_t)reply->position;
#endif
    uint16_t crc, crcRead;
    // If whole frame is not received yet.
    if (reply->size - reply->position < 9)
    {
        data->complete = 0;
        return 0;
    }
    data->complete = 1;
    // Find start of HDLC frame.
    for (pos = (uint16_t)reply->position; pos < reply->size; ++pos)
    {
        if ((ret = bb_getUInt8(reply, &ch)) != 0)
        {
            return ret;
        }
        if (ch == HDLC_FRAME_START_END)
        {
            packetStartID = pos;
            break;
        }
    }
    // Not a HDLC frame.
    // Sometimes meters can send some strange data between DLMS frames.
    if (reply->position == reply->size)
    {
        data->complete = 0;
        // Not enough data to parse;
        return 0;
    }
    if ((ret = bb_getUInt8(reply, frame)) != 0)
    {
        return ret;
    }
    if ((*frame & 0xF0) != 0xA0)
    {
        --reply->position;
        // If same data.
        return dlms_getHdlcData(server, settings, reply, data, frame, preEstablished, first);
    }
    // Check frame length.
    if ((*frame & 0x7) != 0)
    {
        frameLen = ((*frame & 0x7) << 8);
    }
    if ((ret = bb_getUInt8(reply, &ch)) != 0)
    {
        return ret;
    }
    // If not enough data.
    frameLen += ch;
    if ((reply->size - reply->position + 1) < frameLen)
    {
        data->complete = 0;
        reply->position = packetStartID;
        // Not enough data to parse;
        return 0;
    }
    eopPos = (uint16_t)(frameLen + packetStartID + 1);
    if ((ret = bb_getUInt8ByIndex(reply, eopPos, &ch)) != 0)
    {
        return ret;
    }
    if (ch != HDLC_FRAME_START_END)
    {
        reply->position -= 2;
        return dlms_getHdlcData(server, settings, reply, data, frame, preEstablished, first);
    }

    // Check addresses.
    ret = dlms_checkHdlcAddress(server, settings, reply, eopPos);
    if (ret != 0)
    {
        //If pre-established client address has change.
        if (ret == DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS)
        {
            return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
        }
        else
        {
            if (ret == DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS &&
                reply->position + 4 == reply->size)
            {
                data->packetLength = 0;
                bb_clear(reply);
                return ret;
            }
            if (ret == DLMS_ERROR_CODE_FALSE)
            {
                // If echo or reply to other meter.
                return dlms_getHdlcData(server, settings, reply, data, frame, preEstablished, first);
            }
            reply->position = packetStartID + 1;
            ret = dlms_getHdlcData(server, settings, reply, data, frame, preEstablished, first);
            return ret;
        }
    }
    // Is there more data available.
    unsigned char moreData = (*frame & 0x8) != 0;
    // Get frame type.
    if ((ret = bb_getUInt8(reply, frame)) != 0)
    {
        return ret;
    }

    // Is there more data available.
    if (moreData)
    {
        data->moreData |= DLMS_DATA_REQUEST_TYPES_FRAME;
    }
    else
    {
        data->moreData = ((DLMS_DATA_REQUEST_TYPES)(data->moreData & ~DLMS_DATA_REQUEST_TYPES_FRAME));
    }

    if (!preEstablished && !checkFrame(settings, *frame))
    {
        reply->position = eopPos + 1;
        if (settings->server)
        {
            return DLMS_ERROR_CODE_INVALID_FRAME_NUMBER;
        }
        return dlms_getHdlcData(server, settings, reply, data, frame, preEstablished, first);
    }
    // Check that header CRC is correct.
    crc = countCRC(reply, packetStartID + 1,
        reply->position - packetStartID - 1);

    if ((ret = bb_getUInt16(reply, &crcRead)) != 0)
    {
        return ret;
    }

    if (crc != crcRead)
    {
        if (reply->size - reply->position > 8)
        {
            return dlms_getHdlcData(server, settings, reply, data, frame, preEstablished, first);
        }
#ifdef DLMS_DEBUG
        svr_notifyTrace("Invalid CRC. ", -1);
#endif //DLMS_DEBUG
        return DLMS_ERROR_CODE_WRONG_CRC;
    }
    // Check that packet CRC match only if there is a data part.
    if (reply->position != packetStartID + frameLen + 1)
    {
        crc = countCRC(reply, packetStartID + 1, frameLen - 2);
        if ((ret = bb_getUInt16ByIndex(reply, packetStartID + frameLen - 1, &crcRead)) != 0)
        {
            return ret;
        }
        if (crc != crcRead)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace("Invalid CRC. ", -1);
#endif //DLMS_DEBUG
            return DLMS_ERROR_CODE_WRONG_CRC;
        }
        // Remove CRC and EOP from packet length.
        data->packetLength = eopPos - 2;
    }
    else
    {
        data->packetLength = eopPos - 2;
    }


    if (*frame != 0x13 && *frame != 0x3 && (*frame & HDLC_FRAME_TYPE_U_FRAME) == HDLC_FRAME_TYPE_U_FRAME)
    {
        // Get Eop if there is no data.
        if (reply->position == packetStartID + frameLen + 1)
        {
            // Get EOP.
            if ((ret = bb_getUInt8(reply, &ch)) != 0)
            {
                return ret;
            }
        }
        data->command = (DLMS_COMMAND)*frame;
        switch (data->command)
        {
        case DLMS_COMMAND_SNRM:
        case DLMS_COMMAND_UA:
        case DLMS_COMMAND_DISCONNECT_MODE:
        case DLMS_COMMAND_REJECTED:
        case DLMS_COMMAND_DISC:
            break;
        default:
            //Unknown command.
            return DLMS_ERROR_CODE_REJECTED;
        }
    }
    else if (*frame != 0x13 && *frame != 0x3 && (*frame & HDLC_FRAME_TYPE_S_FRAME) == HDLC_FRAME_TYPE_S_FRAME)
    {
        // If S-frame
        int tmp = (*frame >> 2) & 0x3;
        // If frame is rejected.
        if (tmp == HDLC_CONTROL_FRAME_REJECT)
        {
            return DLMS_ERROR_CODE_REJECTED;
        }
        else if (tmp == HDLC_CONTROL_FRAME_RECEIVE_NOT_READY)
        {
            return DLMS_ERROR_CODE_REJECTED;
        }
        else if (tmp == HDLC_CONTROL_FRAME_RECEIVE_READY)
        {
        }
        // Get Eop if there is no data.
        if (reply->position == packetStartID + frameLen + 1)
        {
            // Get EOP.
            if ((ret = bb_getUInt8(reply, &ch)) != 0)
            {
                return ret;
            }
        }
    }
    else
    {
        // I-frame
        // Get Eop if there is no data.
        if (reply->position == packetStartID + frameLen + 1)
        {
            // Get EOP.
            if ((ret = bb_getUInt8(reply, &ch)) != 0)
            {
                return ret;
            }
            if ((*frame & 0x1) == 0x1)
            {
                data->moreData = DLMS_DATA_REQUEST_TYPES_FRAME;
            }
        }
        else
        {
            dlms_checkLLCBytes(settings, reply);
        }
    }
    if (settings->server && (first || data->command == DLMS_COMMAND_SNRM))
    {
#ifndef DLMS_IGNORE_SERVER
        // Check is data send to this server.
        if (!svr_isTarget(settings, settings->serverAddress, settings->clientAddress))
        {
            settings->serverAddress = 0;
            settings->clientAddress = 0;
            if (reply->size - reply->position > 8)
            {
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
                uint32_t pos = reply->position;
#else
                uint16_t pos = reply->position;
#endif //!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
                ret = dlms_getHdlcData(server, settings, reply, data, frame, preEstablished, first);
                if (settings->serverAddress != 0 && settings->clientAddress != 0)
                {
                    reply->position = pos;
                }
                return ret;
            }
            return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
        }
#endif //DLMS_IGNORE_SERVER
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_HDLC

#ifndef DLMS_IGNORE_WRAPPER
int dlms_checkWrapperAddress(dlmsSettings* settings,
    gxByteBuffer* buff,
    gxReplyData* data,
    gxReplyData* notify,
    unsigned char* isNotify)
{
    int ret;
    uint16_t value;
    *isNotify = 0;
    if (settings->server)
    {
        if ((ret = bb_getUInt16(buff, &value)) != 0)
        {
            return ret;
        }
        // Check that client addresses match.
        if (settings->clientAddress != 0 && settings->clientAddress != value)
        {
            return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
        }
        else
        {
            settings->clientAddress = value;
        }

        if ((ret = bb_getUInt16(buff, &value)) != 0)
        {
            return ret;
        }
        // Check that server addresses match.
        if (settings->serverAddress != 0 && settings->serverAddress != value)
        {
            return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
        }
        else
        {
            settings->serverAddress = value;
        }
    }
    else
    {
        if ((ret = bb_getUInt16(buff, &value)) != 0)
        {
            return ret;
        }
        // Check that server addresses match.
        if (settings->serverAddress != 0 && settings->serverAddress != value)
        {
            if (notify == NULL)
            {
                return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
            }
            notify->serverAddress = value;
            *isNotify = 1;
        }
        else
        {
            settings->serverAddress = value;
        }

        if ((ret = bb_getUInt16(buff, &value)) != 0)
        {
            return ret;
        }
        // Check that client addresses match.
        if (settings->clientAddress != 0 && settings->clientAddress != value)
        {
            if (notify == NULL)
            {
                return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
            }
            notify->clientAddress = value;
            *isNotify = 1;
        }
        else
        {
            settings->clientAddress = value;
        }
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_WRAPPER

#ifndef DLMS_IGNORE_WRAPPER
int dlms_getTcpData(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    gxReplyData* data,
    gxReplyData* notify,
    unsigned char* isNotify)
{
    int ret, pos;
    uint16_t value;
    *isNotify = 0;
    // If whole frame is not received yet.
    if (buff->size - buff->position < 8)
    {
        data->complete = 0;
        return DLMS_ERROR_CODE_OK;
    }
    pos = buff->position;
    data->complete = 0;
    if (notify != NULL)
    {
        notify->complete = 0;
    }
    while (buff->position != buff->size)
    {
        // Get version
        if ((ret = bb_getUInt16(buff, &value)) != 0)
        {
            return ret;
        }
        if (value == 1)
        {
            // Check TCP/IP addresses.
            if ((ret = dlms_checkWrapperAddress(settings, buff, data, notify, isNotify)) != 0)
            {
                return ret;
            }
            //If notify.
            if (notify != NULL && *isNotify != 0)
            {
                data = notify;
            }
            // Get length.
            if ((ret = bb_getUInt16(buff, &value)) != 0)
            {
                return ret;
            }
            data->complete = !((buff->size - buff->position) < value);
            if (!data->complete)
            {
                buff->position = pos;
            }
            else
            {
                data->packetLength = buff->position + value;
            }
            break;
        }
        else
        {
            --buff->position;
        }
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_WRAPPER

#ifndef DLMS_IGNORE_WIRELESS_MBUS
int dlms_getMBusData(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    gxReplyData* data)
{
    int ret;
    unsigned char len, ch;
    //L-field.
    if ((ret = bb_getUInt8(buff, &len)) != 0)
    {
        return ret;
    }
    //Some meters are counting length to frame size.
    if (buff->size < (unsigned char)(len - 1))
    {
        data->complete = 0;
        buff->position = buff->position - 1;
    }
    else
    {
        //Some meters are counting length to frame size.
        if (buff->size < len)
        {
            --len;
        }
        data->packetLength = len;
        data->complete = 1;
        //C-field.
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        // DLMS_MBUS_COMMAND cmd = (DLMS_MBUS_COMMAND)ch;
        //M-Field.
        uint16_t manufacturerID;
        if ((ret = bb_getUInt16(buff, &manufacturerID)) != 0)
        {
            return ret;
        }
        //A-Field.
        uint32_t id;
        if ((ret = bb_getUInt32(buff, &id)) != 0)
        {
            return ret;
        }
        unsigned char meterVersion;
        if ((ret = bb_getUInt8(buff, &meterVersion)) != 0 ||
            (ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        // DLMS_MBUS_METER_TYPE type = (DLMS_MBUS_METER_TYPE)ch;
        // CI-Field
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        // DLMS_MBUS_CONTROL_INFO ci = (DLMS_MBUS_CONTROL_INFO)ch;
        //Access number.
        unsigned char frameId;
        if ((ret = bb_getUInt8(buff, &frameId)) != 0)
        {
            return ret;
        }
        //State of the meter
        unsigned char state;
        if ((ret = bb_getUInt8(buff, &state)) != 0)
        {
            return ret;
        }
        //Configuration word.
        uint16_t configurationWord;
        if ((ret = bb_getUInt16(buff, &configurationWord)) != 0)
        {
            return ret;
        }
        //unsigned char encryptedBlocks = (unsigned char)(configurationWord >> 12);
        // DLMS_MBUS_ENCRYPTION_MODE encryption = (DLMS_MBUS_ENCRYPTION_MODE)(configurationWord & 7);
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        settings->clientAddress = ch;
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        settings->serverAddress = ch;
    }
    return ret;
}

#endif //DLMS_IGNORE_WIRELESS_MBUS

#ifndef DLMS_IGNORE_PLC

int dlms_getPlcData(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    gxReplyData* data)
{
    if (bb_available(buff) < 9)
    {
        data->complete = 0;
        return 0;
    }
    unsigned char ch;
    int ret;
    unsigned short pos;
    int packetStartID = buff->position;
    // Find STX.
    unsigned char stx;
    for (pos = (unsigned short)buff->position; pos < buff->size; ++pos)
    {
        if ((ret = bb_getUInt8(buff, &stx)) != 0)
        {
            return ret;
        }
        if (stx == 2)
        {
            packetStartID = pos;
            break;
        }
    }
    // Not a PLC frame.
    if (buff->position == buff->size)
    {
        // Not enough data to parse;
        data->complete = 0;
        buff->position = packetStartID;
        return 0;
    }
    unsigned char len;
    if ((ret = bb_getUInt8(buff, &len)) != 0)
    {
        return ret;
    }
    int index = buff->position;
    if (bb_available(buff) < len)
    {
        data->complete = 0;
        buff->position = buff->position - 2;
    }
    else
    {
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        //Credit fields.  IC, CC, DC
        unsigned char credit;
        if ((ret = bb_getUInt8(buff, &credit)) != 0)
        {
            return ret;
        }
        //MAC Addresses.
        uint32_t mac;
        if ((ret = bb_getUInt24(buff, &mac)) != 0)
        {
            return ret;
        }
        //SA.
        short macSa = (short)(mac >> 12);
        //DA.
        short macDa = (short)(mac & 0xFFF);
        //PAD length.
        unsigned char padLen;
        if ((ret = bb_getUInt8(buff, &padLen)) != 0)
        {
            return ret;
        }

        if (buff->size < (unsigned short)(len + padLen + 2))
        {
            data->complete = 0;
            buff->position = buff->position - index - 6;
        }
        else
        {
            //DL.Data.request
            if ((ret = bb_getUInt8(buff, &ch)) != 0)
            {
                return ret;
            }
            if (ch != DLMS_PLC_DATA_LINK_DATA_REQUEST)
            {
                //Parsing MAC LLC data failed. Invalid DataLink data request.
                return DLMS_ERROR_CODE_INVALID_COMMAND;
            }
            unsigned char da, sa;
            if ((ret = bb_getUInt8(buff, &da)) != 0 ||
                (ret = bb_getUInt8(buff, &sa)) != 0)
            {
                return ret;
            }
            if (settings->server)
            {
                data->complete =
                    (macDa == DLMS_PLC_DESTINATION_ADDRESS_ALL_PHYSICAL || macDa == settings->plcSettings.macSourceAddress) &&
                    (macSa == DLMS_PLC_SOURCE_ADDRESS_INITIATOR || macSa == settings->plcSettings.macDestinationAddress);
                data->serverAddress = macDa;
                data->clientAddress = macSa;
            }
            else
            {
                data->complete =
                    macDa == DLMS_PLC_DESTINATION_ADDRESS_ALL_PHYSICAL ||
                    macDa == DLMS_PLC_SOURCE_ADDRESS_INITIATOR ||
                    macDa == settings->plcSettings.macDestinationAddress;
                data->clientAddress = macDa;
                data->serverAddress = macSa;
            }
            //Skip padding.
            if (data->complete)
            {
                uint16_t crcCount, crc;
                crcCount = countCRC(buff, 0, len + padLen);
                if ((ret = bb_getUInt16ByIndex(buff, len + padLen, &crc)) != 0)
                {
                    return ret;
                }
                //Check CRC.
                if (crc != crcCount)
                {
                    //Invalid data checksum.
                    return DLMS_ERROR_CODE_WRONG_CRC;
                }
                data->packetLength = len;
            }
        }
    }
    return ret;
}

int dlms_getPlcHdlcData(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    gxReplyData* data,
    unsigned char* frame)
{
    if (bb_available(buff) < 2)
    {
        data->complete = 0;
        return 0;
    }
    int ret;
    *frame = 0;
    unsigned char frameLen;
    //SN field.
    uint16_t ns;
    if ((ret = bb_getUInt16(buff, &ns)) != 0)
    {
        return ret;
    }
    switch (ns)
    {
    case DLMS_PLC_MAC_SUB_FRAMES_ONE:
        frameLen = 36;
        break;
    case DLMS_PLC_MAC_SUB_FRAMES_TWO:
        frameLen = 2 * 36;
        break;
    case DLMS_PLC_MAC_SUB_FRAMES_THREE:
        frameLen = 3 * 36;
        break;
    case DLMS_PLC_MAC_SUB_FRAMES_FOUR:
        frameLen = 4 * 36;
        break;
    case DLMS_PLC_MAC_SUB_FRAMES_FIVE:
        frameLen = 5 * 36;
        break;
    case DLMS_PLC_MAC_SUB_FRAMES_SIX:
        frameLen = 6 * 36;
        break;
    case DLMS_PLC_MAC_SUB_FRAMES_SEVEN:
        frameLen = 7 * 36;
        break;
    default:
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (bb_available(buff) < (unsigned char)(frameLen - 2))
    {
        data->complete = 0;
    }
    else
    {
        unsigned long index = buff->position;
        //Credit fields.  IC, CC, DC
        unsigned char credit;
        if ((ret = bb_getUInt8(buff, &credit)) != 0)
        {
            return ret;
        }
        //MAC Addresses.
        uint32_t mac;
        if ((ret = bb_getUInt24(buff, &mac)) != 0)
        {
            return ret;
        }
        //SA.
        unsigned short sa = (unsigned short)(mac >> 12);
        //DA.
        unsigned short da = (unsigned short)(mac & 0xFFF);
        if (settings->server)
        {
            data->complete = (da == DLMS_PLC_DESTINATION_ADDRESS_ALL_PHYSICAL || da == settings->plcSettings.macSourceAddress) &&
                (sa == DLMS_PLC_HDLC_SOURCE_ADDRESS_INITIATOR || sa == settings->plcSettings.macDestinationAddress);
            data->serverAddress = da;
            data->clientAddress = sa;
        }
        else
        {
            data->complete = da == DLMS_PLC_HDLC_SOURCE_ADDRESS_INITIATOR || da == settings->plcSettings.macDestinationAddress;
            data->serverAddress = da;
            data->clientAddress = sa;
        }
        if (data->complete)
        {
            //PAD length.
            unsigned char padLen;
            if ((ret = bb_getUInt8(buff, &padLen)) != 0)
            {
                return ret;
            }
            if ((ret = dlms_getHdlcData(settings->server, settings, buff, data, frame, 0, 1)) != 0)
            {
                return ret;
            }
            dlms_getDataFromFrame(buff, data, dlms_useHdlc(settings->interfaceType));
            buff->position = buff->position + padLen;
            uint32_t crcCount = dlms_countFCS24(buff->data, index, buff->position - index);
            uint32_t crc;
            if ((ret = bb_getUInt24ByIndex(buff, buff->position, &crc)) != 0)
            {
                return ret;
            }
            //Check CRC.
            if (crc != crcCount)
            {
                //Invalid data checksum.
                return DLMS_ERROR_CODE_WRONG_CRC;
            }
            data->packetLength = (uint16_t)(2 + buff->position - index);
        }
        else
        {
            buff->position = (uint16_t)(buff->position + frameLen - index - 4);
        }
    }
    return ret;
}

// Check is this PLC S-FSK message.
// buff: Received data.
// Returns True, if this is PLC message.
unsigned char dlms_isPlcSfskData(gxByteBuffer* buff)
{
    if (bb_available(buff) < 2)
    {
        return 0;
    }
    int ret;
    uint16_t len;
    if ((ret = bb_getUInt16ByIndex(buff, buff->position, &len)) != 0)
    {
        return (unsigned char)ret;
    }
    switch (len)
    {
    case DLMS_PLC_MAC_SUB_FRAMES_ONE:
    case DLMS_PLC_MAC_SUB_FRAMES_TWO:
    case DLMS_PLC_MAC_SUB_FRAMES_THREE:
    case DLMS_PLC_MAC_SUB_FRAMES_FOUR:
    case DLMS_PLC_MAC_SUB_FRAMES_FIVE:
    case DLMS_PLC_MAC_SUB_FRAMES_SIX:
    case DLMS_PLC_MAC_SUB_FRAMES_SEVEN:
        return 1;
    default:
        return 0;
    }
}
#endif //DLMS_IGNORE_PLC

int dlms_getDataFromBlock(gxByteBuffer* data, uint16_t index)
{
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t pos, len = data->position - index;
#else
    uint16_t pos, len = data->position - index;
#endif
    if (data->size == data->position)
    {
        bb_clear(data);
        return 0;
    }
    pos = data->position;
    bb_move(data, data->position, data->position - len, data->size - data->position);
    data->position = pos - len;
    return 0;
}

int dlms_receiverReady(
    dlmsSettings* settings,
    DLMS_DATA_REQUEST_TYPES type,
    gxByteBuffer* reply)
{
    int ret;
    DLMS_COMMAND cmd;
    message tmp;
    gxByteBuffer bb;
    bb_clear(reply);
    if (type == DLMS_DATA_REQUEST_TYPES_NONE)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifndef DLMS_IGNORE_HDLC
    // Get next frame.
    if ((type & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {
        unsigned char id = getReceiverReady(settings);
        switch (settings->interfaceType)
        {
#ifndef DLMS_IGNORE_PLC
        case DLMS_INTERFACE_TYPE_PLC_HDLC:
            ret = dlms_getMacHdlcFrame(settings, id, 0, NULL, reply);
            break;
#endif //DLMS_IGNORE_PLC
#ifndef DLMS_IGNORE_HDLC
        case DLMS_INTERFACE_TYPE_HDLC:
            ret = dlms_getHdlcFrame(settings, id, NULL, reply);
            break;
#endif //DLMS_IGNORE_HDLC
        default:
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        return ret;
    }
#endif //DLMS_IGNORE_HDLC
    // Get next block.
    if (settings->useLogicalNameReferencing)
    {
        if (settings->server)
        {
            cmd = DLMS_COMMAND_GET_RESPONSE;
        }
        else
        {
            cmd = DLMS_COMMAND_GET_REQUEST;
        }
    }
    else
    {
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        if (settings->server)
        {
            cmd = DLMS_COMMAND_READ_RESPONSE;
        }
        else
        {
            cmd = DLMS_COMMAND_READ_REQUEST;
        }
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    }
#ifdef DLMS_IGNORE_MALLOC
    unsigned char buff[40];
    bb_attach(&bb, buff, 0, sizeof(buff));
#else
    BYTE_BUFFER_INIT(&bb);
#endif //DLMS_IGNORE_MALLOC

    if (settings->useLogicalNameReferencing)
    {
        bb_setUInt32(&bb, settings->blockIndex);
    }
    else
    {
        bb_setUInt16(&bb, (uint16_t)settings->blockIndex);
    }
    ++settings->blockIndex;
#ifdef DLMS_IGNORE_MALLOC
    gxByteBuffer* p[] = { reply };
    mes_attach(&tmp, p, 1);
#else
    mes_init(&tmp);
#endif //DLMS_IGNORE_MALLOC
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, cmd, DLMS_GET_COMMAND_TYPE_NEXT_DATA_BLOCK, &bb, NULL, 0xFF, DLMS_COMMAND_NONE, 0, 0);
#ifdef DLMS_IGNORE_MALLOC
        p.serializedPdu = &bb;
#endif //DLMS_IGNORE_MALLOC
        ret = dlms_getLnMessages(&p, &tmp);
    }
    else
    {
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        gxSNParameters p;
        params_initSN(&p, settings, cmd, 1, DLMS_VARIABLE_ACCESS_SPECIFICATION_BLOCK_NUMBER_ACCESS, &bb, NULL, DLMS_COMMAND_NONE);
        ret = dlms_getSnMessages(&p, &tmp);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    }
#ifndef DLMS_IGNORE_MALLOC
    if (ret == 0)
    {
        ret = bb_set2(reply, (gxByteBuffer*)tmp.data[0], 0, tmp.data[0]->size);
    }
    bb_clear(&bb);
    mes_clear(&tmp);
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

#ifndef DLMS_IGNORE_WRAPPER
int dlms_getWrapperFrame(
    dlmsSettings* settings,
    DLMS_COMMAND command,
    gxByteBuffer* data,
    gxByteBuffer* reply)
{
    int ret;
    if ((ret = bb_clear(reply)) == 0 &&
        (ret = bb_capacity(reply, 8 + data->size - data->position)) == 0 &&
        // Add version.
        (ret = bb_setUInt16(reply, 1)) == 0)
    {
        if (settings->server)
        {
            if ((ret = bb_setUInt16(reply, (unsigned short)settings->serverAddress)) == 0)
            {
                if (((dlmsServerSettings*)settings)->pushClientAddress != 0 && (command == DLMS_COMMAND_DATA_NOTIFICATION || command == DLMS_COMMAND_EVENT_NOTIFICATION))
                {
                    ret = bb_setUInt16(reply, ((dlmsServerSettings*)settings)->pushClientAddress);
                }
                else
                {
                    ret = bb_setUInt16(reply, settings->clientAddress);
                }
            }
        }
        else
        {
            if ((ret = bb_setUInt16(reply, settings->clientAddress)) == 0)
            {
                ret = bb_setUInt16(reply, (unsigned short)settings->serverAddress);
            }
        }
        // Data length.
        if (ret == 0 && (ret = bb_setUInt16(reply, (uint16_t)data->size)) == 0)
        {
            // Data
            ret = bb_set2(reply, data, data->position, data->size - data->position);
        }
        // Remove sent data in server side.
        if (ret == 0 && settings->server)
        {
            if (data->size == data->position)
            {
                ret = bb_clear(data);
            }
            else
            {
                ret = bb_move(data, data->position, 0, data->size - data->position);
                data->position = 0;
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_WRAPPER

int dlms_verifyInvokeId(dlmsSettings* settings, gxReplyData* reply)
{
    if (settings->autoIncreaseInvokeID && reply->invokeId != dlms_getInvokeIDPriority(settings, 0))
    {
        //Invalid invoke ID.
        return DLMS_ERROR_CODE_INVALID_INVOKE_ID;
    }
    return 0;
}

int dlms_handleGetResponse(
    dlmsSettings* settings,
    gxReplyData* reply,
    uint16_t index)
{
    int ret;
    uint16_t count;
    unsigned char ch;
    uint32_t number;
    short type;
    // Get type.
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    type = ch;
    // Get invoke ID and priority.
    if ((ret = bb_getUInt8(&reply->data, &reply->invokeId)) != 0)
    {
        return ret;
    }
    if ((ret = dlms_verifyInvokeId(settings, reply)) != 0)
    {
        return ret;
    }
    // Response normal
    if (type == 1)
    {
        // Result
        if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
            {
                return ret;
            }
            return ch;
        }
        ret = dlms_getDataFromBlock(&reply->data, 0);
    }
    else if (type == 2)
    {
        // GetResponsewithDataBlock
        // Is Last block.
        if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
        {
            return ret;
        }
        if (ch == 0)
        {
            reply->moreData = (DLMS_DATA_REQUEST_TYPES)(reply->moreData | DLMS_DATA_REQUEST_TYPES_BLOCK);
        }
        else
        {
            reply->moreData =
                (DLMS_DATA_REQUEST_TYPES)(reply->moreData & ~DLMS_DATA_REQUEST_TYPES_BLOCK);
        }
        // Get Block number.
        if ((ret = bb_getUInt32(&reply->data, &number)) != 0)
        {
            return ret;
        }
        // If meter's block index is zero based or Actaris is read.
        // Actaris SL7000 might return wrong block index sometimes.
        // It's not reseted to 1.
        if (number != 1 && settings->blockIndex == 1)
        {
            settings->blockIndex = number;
        }
        else if (number != settings->blockIndex)
        {
            return DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
        }
        // Get status.
        if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
            {
                return ret;
            }
            return ch;
        }
        else
        {
            // Get data size.
            if ((ret = hlp_getObjectCount2(&reply->data, &count)) != 0)
            {
                return ret;
            }
            // if whole block is read.
            if ((reply->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
            {
                // Check Block length.
                if (count > (uint16_t)(bb_available(&reply->data)))
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                reply->command = DLMS_COMMAND_NONE;
            }
            if (count == 0)
            {
                // If meter sends empty data block.
                reply->data.size = index;
            }
            else
            {
                if ((ret = dlms_getDataFromBlock(&reply->data, index)) != 0)
                {
                    return ret;
                }
            }
            // If last packet and data is not try to peek.
            if (reply->moreData == DLMS_DATA_REQUEST_TYPES_NONE)
            {
                if (!reply->peek)
                {
                    reply->data.position = 0;
                    resetBlockIndex(settings);
                }
            }
        }
    }
    else if (type == 3)
    {
        return DLMS_ERROR_CODE_FALSE;
    }
    else
    {
        //Invalid Get response.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

int handleWriteResponse(gxReplyData* data)
{
    unsigned char ch;
    int ret;
    uint16_t count, pos;
    if (hlp_getObjectCount2(&data->data, &count) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    for (pos = 0; pos != count; ++pos)
    {
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
            {
                return ret;
            }
            return ch;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int dlms_handleWriteResponse(
    gxReplyData* data)
{
    unsigned char ch;
    int ret;
    uint16_t pos, count;
    if (hlp_getObjectCount2(&data->data, &count) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    for (pos = 0; pos != count; ++pos)
    {
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
            {
                return ret;
            }
            return ch;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int dlms_getValueFromData(dlmsSettings* settings,
    gxReplyData* reply)
{
    uint16_t index;
    int ret;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    int pos;
    dlmsVARIANT_PTR tmp;
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    dlmsVARIANT value;
    gxDataInfo info;
    di_init(&info);
    var_init(&value);
    if (reply->dataValue.vt == DLMS_DATA_TYPE_ARRAY)
    {
        info.type = DLMS_DATA_TYPE_ARRAY;
        info.count = (uint16_t)reply->totalCount;
        info.index = (uint16_t)reply->data.size;
    }
    index = (uint16_t)(reply->data.position);
    reply->data.position = reply->readPosition;
    if ((ret = dlms_getData(&reply->data, &info, &value)) != 0)
    {
        var_clear(&value);
        return ret;
    }
    // If new data.
    if (value.vt != DLMS_DATA_TYPE_NONE)
    {
        if (value.vt != DLMS_DATA_TYPE_ARRAY && value.vt != DLMS_DATA_TYPE_STRUCTURE)
        {
            reply->dataType = info.type;
            reply->dataValue = value;
            reply->totalCount = 0;
            if (reply->command == DLMS_COMMAND_DATA_NOTIFICATION)
            {
                reply->readPosition = reply->data.position;
            }
        }
        else
        {
            if (reply->dataValue.vt == DLMS_DATA_TYPE_NONE)
            {
                reply->dataValue = value;
            }
            else
            {
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
                for (pos = 0; pos != value.Arr->size; ++pos)
                {
                    if ((ret = va_getByIndex(value.Arr, pos, &tmp)) != 0)
                    {
                        return ret;
                    }
                    va_push(reply->dataValue.Arr, tmp);
                }
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
            }
        }
        reply->readPosition = reply->data.position;
        // Element count.
        reply->totalCount = info.count;
    }
    else if (info.complete
        && reply->command == DLMS_COMMAND_DATA_NOTIFICATION)
    {
        // If last item is null. This is a special case.
        reply->readPosition = reply->data.position;
    }
    reply->data.position = index;

    // If last data frame of the data block is read.
    if (reply->command != DLMS_COMMAND_DATA_NOTIFICATION
        && info.complete && reply->moreData == DLMS_DATA_REQUEST_TYPES_NONE)
    {
        // If all blocks are read.
        resetBlockIndex(settings);
        reply->data.position = 0;
    }
    return 0;
}

int dlms_readResponseDataBlockResult(
    dlmsSettings* settings,
    gxReplyData* reply,
    uint16_t index)
{
    int ret;
    uint16_t number;
    uint16_t blockLength;
    unsigned char lastBlock;
    if ((ret = bb_getUInt8(&reply->data, &lastBlock)) != 0)
    {
        return ret;
    }
    // Get Block number.
    if ((ret = bb_getUInt16(&reply->data, &number)) != 0)
    {
        return ret;
    }
    if (hlp_getObjectCount2(&reply->data, &blockLength) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    // Is Last block.
    if (!lastBlock)
    {
        reply->moreData |= DLMS_DATA_REQUEST_TYPES_BLOCK;
    }
    else
    {
        reply->moreData &= ~DLMS_DATA_REQUEST_TYPES_BLOCK;
    }
    // If meter's block index is zero based.
    if (number != 1 && settings->blockIndex == 1)
    {
        settings->blockIndex = number;
    }
    if (number != settings->blockIndex)
    {
        //Invalid Block number
        return DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
    }
    // If whole block is not read.
    if ((reply->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {
        dlms_getDataFromBlock(&reply->data, index);
        return DLMS_ERROR_CODE_FALSE;
    }
    if (blockLength != bb_available(&reply->data))
    {
        //Invalid block length.
        return DLMS_ERROR_CODE_DATA_BLOCK_UNAVAILABLE;
    }
    reply->command = DLMS_COMMAND_NONE;

    dlms_getDataFromBlock(&reply->data, index);
    reply->totalCount = 0;
    // If last packet and data is not try to peek.
    if (reply->moreData == DLMS_DATA_REQUEST_TYPES_NONE)
    {
        resetBlockIndex(settings);
    }
    return ret;
}
int dlms_handleReadResponse(
    dlmsSettings* settings,
    gxReplyData* reply,
    uint16_t index)
{
    int ret;
    unsigned char ch;
    uint16_t number;
    uint16_t pos, cnt = reply->totalCount;
    DLMS_SINGLE_READ_RESPONSE type;
    variantArray values;

    // If we are reading value first time or block is handed.
    unsigned char first = reply->totalCount == 0 || reply->commandType == DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT;
    if (first)
    {
        if (hlp_getObjectCount2(&reply->data, &cnt) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        reply->totalCount = cnt;
    }
    if (cnt != 1)
    {
        //Parse data after all data is received when readlist is used.
        if (reply->moreData != DLMS_DATA_REQUEST_TYPES_NONE)
        {
            if ((ret = dlms_getDataFromBlock(&reply->data, 0)) == 0)
            {
                ret = DLMS_ERROR_CODE_FALSE;
            }
            return ret;
        }
        if (!first)
        {
            reply->data.position = 0;
            first = 1;
        }
    }
    va_init(&values);
    for (pos = 0; pos != cnt; ++pos)
    {
        // Get status code. Status code is begin of each PDU.
        if (first)
        {
            if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
            {
                return ret;
            }
            reply->commandType = ch;
            type = (DLMS_SINGLE_READ_RESPONSE)ch;
        }
        else
        {
            type = (DLMS_SINGLE_READ_RESPONSE)reply->commandType;
        }
        switch (type)
        {
        case DLMS_SINGLE_READ_RESPONSE_DATA:
            if (cnt == 1)
            {
                ret = dlms_getDataFromBlock(&reply->data, 0);
            }
            else
            {
                // If read multiple items.
                reply->readPosition = reply->data.position;
                dlms_getValueFromData(settings, reply);
                reply->data.position = reply->readPosition;
                va_push(&values, &reply->dataValue);
                var_clear(&reply->dataValue);
            }
            break;
        case DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR:
            // Get error code.
            if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
            {
                return ret;
            }
            return ch;
        case DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT:
            if ((ret = dlms_readResponseDataBlockResult(settings, reply, index)) != 0)
            {
                return ret;
            }
            break;
        case DLMS_SINGLE_READ_RESPONSE_BLOCK_NUMBER:
            // Get Block number.
            if ((ret = bb_getUInt16(&reply->data, &number)) != 0)
            {
                return ret;
            }
            if (number != settings->blockIndex)
            {
                //Invalid Block number
                return DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
            }
            ++settings->blockIndex;
            reply->moreData |= DLMS_DATA_REQUEST_TYPES_BLOCK;
            break;
        default:
            //HandleReadResponse failed. Invalid tag.
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
    }
    if (values.size != 0)
    {
#ifndef DLMS_IGNORE_MALLOC
        reply->dataValue.vt = DLMS_DATA_TYPE_ARRAY;
        reply->dataValue.Arr = gxmalloc(sizeof(variantArray));
        va_init(reply->dataValue.Arr);
        reply->dataValue.Arr->capacity = values.capacity;
        va_copyArray(reply->dataValue.Arr, &values);
        gxfree(values.data);
#endif //DLMS_IGNORE_MALLOC
    }
    if (cnt != 1)
    {
        return DLMS_ERROR_CODE_FALSE;
    }
    return 0;
}

int dlms_handleMethodResponse(
    dlmsSettings* settings,
    gxReplyData* data)
{
    int ret;
    unsigned char ch, type;
    // Get type.
    if ((ret = bb_getUInt8(&data->data, &type)) != 0)
    {
        return ret;
    }
    // Get invoke ID and priority.
    if ((ret = bb_getUInt8(&data->data, &data->invokeId)) != 0)
    {
        return ret;
    }
    if ((ret = dlms_verifyInvokeId(settings, data)) != 0)
    {
        return ret;
    }
    //Action-Response-Normal
    if (type == 1)
    {
        //Get Action-Result
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            return ch;
        }
        resetBlockIndex(settings);
        // Get data if exists. Some meters do not return here anything.
        if (data->data.position < data->data.size)
        {
            //Get-Data-Result.
            if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
            {
                return ret;
            }
            //If data.
            if (ch == 0)
            {
                return dlms_getDataFromBlock(&data->data, 0);
            }
            else if (ch == 1) //Data-Access-Result
            {
                //Get Data-Access-Result
                if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
                {
                    return ret;
                }
                if (ch != 0)
                {
                    if (ch == 9)
                    {
                        //Handle Texas Instrument missing byte here.
                        if ((ret = bb_getUInt8ByIndex(&data->data, data->data.position, &ch)) != 0)
                        {
                            return ret;
                        }
                        if (ch == 16)
                        {
                            --data->data.position;
                            return dlms_getDataFromBlock(&data->data, 0);
                        }
                    }
                    if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
                    {
                        return ret;
                    }
                    return ch;
                }
                return dlms_getDataFromBlock(&data->data, 0);
            }
            else
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
        }
    }
    //Action-Response-With-Pblock
    else if (type == 2)
    {
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    // Action-Response-With-List.
    else if (type == 3)
    {
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    //Action-Response-Next-Pblock
    else if (type == 4)
    {
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    return DLMS_ERROR_CODE_OK;
}

int dlms_handlePush(gxReplyData* reply)
{
    unsigned char ch;
    uint32_t ul;
    int ret;
    uint16_t index = (uint16_t)(reply->data.position - 1);
    // Is last block
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }

    if ((ch & 0x80) == 0)
    {
        reply->moreData = (DLMS_DATA_REQUEST_TYPES)(reply->moreData | DLMS_DATA_REQUEST_TYPES_BLOCK);
    }
    else
    {
        reply->moreData = (DLMS_DATA_REQUEST_TYPES)(reply->moreData & ~DLMS_DATA_REQUEST_TYPES_BLOCK);
    }
    // Get block number sent.
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    // Get block number acknowledged
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    // Get APU tag.
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    // Addl fields
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    // Data-Notification
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    if ((ch & 0x0F) == 0)
    {
        //Invalid data.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    // Long-Invoke-Id-And-Priority
    if ((ret = bb_getUInt32(&reply->data, &ul)) != 0)
    {
        return ret;
    }
    // Get date time and skip it if used.
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0)
    {
        reply->data.position = reply->data.position + ch;
    }
    return dlms_getDataFromBlock(&reply->data, index);
}

int dlms_handleSetResponse(
    dlmsSettings* settings,
    gxReplyData* data)
{
    unsigned char ch, type;
    int ret;
    if ((ret = bb_getUInt8(&data->data, &type)) != 0)
    {
        return ret;
    }

    //Invoke ID and priority.
    if ((ret = bb_getUInt8(&data->data, &data->invokeId)) != 0)
    {
        return ret;
    }
    if ((ret = dlms_verifyInvokeId(settings, data)) != 0)
    {
        return ret;
    }
    // SetResponseNormal
    if (type == DLMS_SET_RESPONSE_TYPE_NORMAL)
    {
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            return ch;
        }
    }
    else if (type == DLMS_SET_RESPONSE_TYPE_DATA_BLOCK || type == DLMS_SET_RESPONSE_TYPE_LAST_DATA_BLOCK)
    {
        uint32_t  tmp;
        if ((ret = bb_getUInt32(&data->data, &tmp)) != 0)
        {
            return ret;
        }
    }
    else
    {
        //Invalid data type.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}


int dlms_changeType2(
    dlmsVARIANT* value,
    DLMS_DATA_TYPE type,
    dlmsVARIANT* newValue)
{
    gxByteBuffer bb;
    if (value->byteArr != NULL)
    {
        bb_attach(&bb, value->byteArr->data, value->byteArr->size, value->byteArr->size);
        bb.position = value->byteArr->position;
    }
    return dlms_changeType(&bb, type, newValue);

}

int dlms_changeType(
    gxByteBuffer* value,
    DLMS_DATA_TYPE type,
    dlmsVARIANT* newValue)
{
    int ret;
    gxDataInfo info;
    di_init(&info);
#ifndef DLMS_IGNORE_MALLOC
    var_clear(newValue);
#endif //DLMS_IGNORE_MALLOC
    if (value->size == 0)
    {
        if (type == DLMS_DATA_TYPE_STRING || type == DLMS_DATA_TYPE_STRING_UTF8)
        {
            newValue->vt = type;
        }
        return DLMS_ERROR_CODE_OK;
    }
    if (type == DLMS_DATA_TYPE_NONE)
    {
#if !defined(GX_DLMS_MICROCONTROLLER) && !defined(DLMS_IGNORE_MALLOC)
        char* tmp = bb_toHexString(value);
        newValue->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(newValue->strVal);
        bb_addString(newValue->strVal, tmp);
        gxfree(tmp);
        newValue->vt = DLMS_DATA_TYPE_STRING;
        return DLMS_ERROR_CODE_OK;
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //!defined(GX_DLMS_MICROCONTROLLER) && !defined(DLMS_IGNORE_MALLOC)
    }
    info.type = type;
    if ((ret = dlms_getData(value, &info, newValue)) != 0)
    {
        return ret;
    }
    value->position = 0;
    if (!info.complete)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    if (type == DLMS_DATA_TYPE_OCTET_STRING && newValue->vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
#if !defined(GX_DLMS_MICROCONTROLLER) && !defined(DLMS_IGNORE_MALLOC)
        char* tmp = bb_toHexString(value);
        newValue->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(newValue->strVal);
        bb_addString(newValue->strVal, tmp);
        gxfree(tmp);
        newValue->vt = DLMS_DATA_TYPE_STRING;
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //!defined(GX_DLMS_MICROCONTROLLER) && !defined(DLMS_IGNORE_MALLOC)
    }
    return 0;
}

/**
* Handle data notification get data from block and/or update error status.
*
* @param reply
*            Received data from the client.
*/
int dlms_handleDataNotification(
    dlmsSettings* settings,
    gxReplyData* reply)
{
    uint32_t id;
    int ret;
    unsigned char len;
    gxByteBuffer tmp;
    dlmsVARIANT t;
    uint16_t start = (uint16_t)(reply->data.position - 1);
    // Get invoke id.
    if ((ret = bb_getUInt32(&reply->data, &id)) != 0)
    {
        return ret;
    }
    // Get date time.
#ifdef DLMS_USE_EPOCH_TIME
    reply->time = 0;
#else
    memset(&reply->time, 0, sizeof(struct tm));
#endif //DLMS_USE_EPOCH_TIME
    if ((ret = bb_getUInt8(&reply->data, &len)) != 0)
    {
        return ret;
    }
    if (len != 0)
    {
#ifndef DLMS_IGNORE_MALLOC
        var_init(&t);
#else
        gxtime tm;
        GX_DATETIME(t) = &tm;
#endif //DLMS_IGNORE_MALLOC
        unsigned char buff[12];
        bb_attach(&tmp, buff, 0, sizeof(buff));
        if ((ret = bb_set2(&tmp, &reply->data, reply->data.position, len)) != 0 ||
            (ret = dlms_changeType(&tmp, DLMS_DATA_TYPE_DATETIME, &t)) != 0)
        {
            return ret;
        }
#ifdef DLMS_USE_EPOCH_TIME
#ifndef DLMS_IGNORE_MALLOC
        reply->time = t.dateTime->value;
#else
        reply->time = tm.value;
#endif //DLMS_IGNORE_MALLOC
#else
#ifndef DLMS_IGNORE_MALLOC
        reply->time = t.dateTime->value;
#else
        reply->time = ((gxtime*)t.pVal)->value;
#endif //DLMS_IGNORE_MALLOC
#endif // DLMS_USE_EPOCH_TIME
    }
    if ((ret = dlms_getDataFromBlock(&reply->data, start)) != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_MALLOC
    return dlms_getValueFromData(settings, reply);
#else
    //Client app must do the data parsing if malloc is not used.
    return 0;
#endif //DLMS_IGNORE_MALLOC
}

/**
* Handle General block transfer message.
*
* @param settings
*            DLMS settings.
* @param data
*            received data.
*/
int dlms_handleGbt(
    dlmsSettings* settings,
    gxReplyData* data)
{
    int ret;
    unsigned char bc;
    uint16_t bn, bna;
    uint16_t index = (uint16_t)(data->data.position - 1);
    if ((ret = bb_getUInt8(&data->data, &bc)) != 0)
    {
        return ret;
    }
    // Is streaming active.
    data->streaming = (bc & 0x40) != 0;
    data->windowSize = (bc & 0x3F);
    // Block number.
    if ((ret = bb_getUInt16(&data->data, &bn)) != 0)
    {
        return ret;
    }
    // Block number acknowledged.
    if ((ret = bb_getUInt16(&data->data, &bna)) != 0)
    {
        return ret;
    }
    // Remove existing data when first block is received.
    if (bn == 1)
    {
        index = 0;
    }
    else if (bna != settings->blockIndex - 1)
    {
        // If this block is already received.
        data->data.size = index;
        data->command = DLMS_COMMAND_NONE;
        return 0;
    }

    data->blockNumber = bn;
    // Block number acknowledged.
    data->blockNumberAck = bna;
    data->command = DLMS_COMMAND_NONE;
    uint16_t len;
    if (hlp_getObjectCount2(&data->data, &len) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    if (len != (data->data.size - data->data.position))
    {
        data->complete = 0;
        return 0;
    }
    if ((ret = dlms_getDataFromBlock(&data->data, index)) != 0)
    {
        return ret;
    }

    // Is Last block.
    if ((bc & 0x80) == 0) {
        data->moreData |= DLMS_DATA_REQUEST_TYPES_GBT;
    }
    else
    {
        data->moreData &= ~DLMS_DATA_REQUEST_TYPES_GBT;
        if (data->data.size != 0)
        {
            data->data.position = 0;
            if ((ret = dlms_getPdu(settings, data, 0)) != 0)
            {
                return ret;
            }
            // Get data if all data is read or we want to peek data.
            if (data->data.position != data->data.size
                && (
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
                    data->command == DLMS_COMMAND_READ_RESPONSE ||
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
                    data->command == DLMS_COMMAND_GET_RESPONSE)
                && (data->moreData == DLMS_DATA_REQUEST_TYPES_NONE || data->peek))
            {
                data->data.position = 0;
                ret = dlms_getValueFromData(settings, data);
            }
        }
    }
    return ret;
}

#if !defined(DLMS_IGNORE_SERVER)
int dlms_handleGloDedRequest(dlmsSettings* settings,
    gxReplyData* data)
{
    int ret = 0;
#ifdef DLMS_IGNORE_HIGH_GMAC
    ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
    DLMS_SECURITY_SUITE suite;
    uint64_t invocationCounter;
    // If all frames are read.
    if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        unsigned char ch;
        DLMS_SECURITY security;
        --data->data.position;
        unsigned char emptySourceSystemTile;
        emptySourceSystemTile = memcmp(settings->sourceSystemTitle, EMPTY_SYSTEM_TITLE, 8) == 0;
        if (dlms_useDedicatedKey(settings) && (settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0)
        {
            if ((ret = cip_decrypt(&settings->cipher,
                settings->sourceSystemTitle,
                settings->cipher.dedicatedKey,
                &data->data,
                &security,
                &suite,
                &invocationCounter)) != 0)
            {
                return ret;
            }
        }
        //If pre-set connection is made.
        else if (dlms_usePreEstablishedSystemTitle(settings) && emptySourceSystemTile)
        {
#ifndef DLMS_IGNORE_SERVER
            if (settings->server && settings->connected == DLMS_CONNECTION_STATE_NONE && !data->preEstablished)
            {
                // Check is data send to this server.
                if (!svr_isTarget(settings, settings->serverAddress, settings->clientAddress))
                {
                    if ((settings->connected & DLMS_CONNECTION_STATE_DLMS) == 0)
                    {
                        settings->serverAddress = settings->clientAddress = 0;
                    }
                    return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
                }
                if ((ret = svr_connected((dlmsServerSettings*)settings)) != 0)
                {
                    return ret;
                }
            }
#endif //DLMS_IGNORE_SERVER
            if ((ret = cip_decrypt(&settings->cipher,
#ifndef DLMS_IGNORE_MALLOC
                settings->preEstablishedSystemTitle->data,
                &settings->cipher.blockCipherKey,
#else
                settings->preEstablishedSystemTitle,
                settings->cipher.blockCipherKey,
#endif //DLMS_IGNORE_MALLOC
                & data->data,
                &security,
                &suite,
                &invocationCounter)) != 0)
            {
                return ret;
            }
            if (data->preEstablished == 0)
            {
                data->preEstablished = 1;
            }
        }
        else
        {
            if ((ret = cip_decrypt(&settings->cipher,
#ifndef DLMS_IGNORE_MALLOC
                settings->sourceSystemTitle,
                &settings->cipher.blockCipherKey,
#else
                settings->sourceSystemTitle,
                settings->cipher.blockCipherKey,
#endif //DLMS_IGNORE_MALLOC
                & data->data,
                &security,
                &suite,
                &invocationCounter)) != 0)
            {
                return ret;
            }
        }
        //If IC value is wrong.
        if (settings->expectedInvocationCounter != NULL)
        {
            if (invocationCounter < *settings->expectedInvocationCounter)
            {
                return DLMS_ERROR_CODE_INVOCATION_COUNTER_TOO_SMALL;
            }
            //Update IC.
#ifdef DLMS_COSEM_INVOCATION_COUNTER_SIZE64
            * settings->expectedInvocationCounter = (1 + invocationCounter);
#else
            * settings->expectedInvocationCounter = (uint32_t)(1 + invocationCounter);
#endif //DLMS_COSEM_INVOCATION_COUNTER_SIZE64
        }
        // Get command.
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        data->encryptedCommand = data->command;
        data->command = (DLMS_COMMAND)ch;
    }
    else
    {
        data->data.position -= 1;
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return ret;
}
#endif // !defined(DLMS_IGNORE_SERVER)

#if !defined(DLMS_IGNORE_CLIENT)
int dlms_handleGloDedResponse(dlmsSettings* settings,
    gxReplyData* data, uint32_t index)
{
#ifdef DLMS_IGNORE_HIGH_GMAC
    return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
    int ret = 0;
    DLMS_SECURITY_SUITE suite;
    uint64_t invocationCounter;
    if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        DLMS_SECURITY security;
        --data->data.position;
        data->data.position = index;
        gxByteBuffer bb;
        bb_attach(&bb, data->data.data + index, bb_available(&data->data), bb_getCapacity(&data->data));
        if ((settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0 && dlms_useDedicatedKey(settings))
        {
            if ((ret = cip_decrypt(&settings->cipher,
                settings->sourceSystemTitle,
                settings->cipher.dedicatedKey,
                &bb,
                &security,
                &suite,
                &invocationCounter)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = cip_decrypt(&settings->cipher,
                settings->sourceSystemTitle,
#ifndef DLMS_IGNORE_MALLOC
                & settings->cipher.blockCipherKey,
#else
                settings->cipher.blockCipherKey,
#endif //DLMS_IGNORE_MALLOC
                & bb,
                &security,
                &suite,
                &invocationCounter)) != 0)
            {
                return ret;
            }
        }
        data->data.size = bb.size + index;
        data->data.position += bb.position;
        //If target is sending data ciphered using different security policy.
        if (settings->cipher.security != security)
        {
            return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
        }
        if (settings->expectedInvocationCounter != NULL)
        {
            //If data is ciphered using invalid invocation counter value.
            if (invocationCounter != *settings->expectedInvocationCounter)
            {
                return DLMS_ERROR_CODE_INVOCATION_COUNTER_TOO_SMALL;
            }
#ifdef DLMS_COSEM_INVOCATION_COUNTER_SIZE64
            * settings->expectedInvocationCounter = (1 + invocationCounter);
#else
            * settings->expectedInvocationCounter = (uint32_t)(1 + invocationCounter);
#endif //DLMS_COSEM_INVOCATION_COUNTER_SIZE64
        }
        data->command = DLMS_COMMAND_NONE;
        ret = dlms_getPdu(settings, data, 0);
        data->cipherIndex = (uint16_t)data->data.size;
    }
    return ret;
#endif //DLMS_IGNORE_HIGH_GMAC
}
#endif //!defined(DLMS_IGNORE_CLIENT)

#if !defined(DLMS_IGNORE_GENERAL_CIPHERING) && !defined(DLMS_IGNORE_HIGH_GMAC)
int dlms_handleGeneralCiphering(
    dlmsSettings* settings,
    gxReplyData* data)
{
#ifdef DLMS_IGNORE_HIGH_GMAC
    return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
    unsigned char ch;
    int ret;
    // If all frames are read.
    if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        --data->data.position;
        DLMS_SECURITY security;
        DLMS_SECURITY_SUITE suite;
        uint64_t invocationCounter;
        if ((ret = cip_decrypt(&settings->cipher,
            settings->sourceSystemTitle,
#ifndef DLMS_IGNORE_MALLOC
            & settings->cipher.blockCipherKey,
#else
            settings->cipher.blockCipherKey,
#endif //DLMS_IGNORE_MALLOC
            & data->data,
            &security,
            &suite,
            &invocationCounter)) != 0)
        {
            return ret;
        }
        // Get command
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        data->command = DLMS_COMMAND_NONE;
        if (security != DLMS_SECURITY_NONE)
        {
            if ((ret = dlms_getPdu(settings, data, 0)) != 0)
            {
                return ret;
            }
        }
    }
    return 0;
#endif //DLMS_IGNORE_HIGH_GMAC
}
#endif //!defined(DLMS_IGNORE_GENERAL_CIPHERING) && !defined(DLMS_IGNORE_HIGH_GMAC)

#if !defined(DLMS_IGNORE_SERVER)
int32_t dlms_handleConfirmedServiceError(gxByteBuffer* data)
{
    int32_t ret;
    unsigned char ch;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    DLMS_CONFIRMED_SERVICE_ERROR service = (DLMS_CONFIRMED_SERVICE_ERROR)ch;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    DLMS_SERVICE_ERROR type = (DLMS_SERVICE_ERROR)ch;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    ret = service;
    ret <<= 16;
    ret |= DLMS_ERROR_TYPE_CONFIRMED_SERVICE_ERROR;
    ret |= type << 8;
    ret |= ch;
    return ret;
}

int dlms_handleExceptionResponse(gxByteBuffer* data)
{
    int ret;
    unsigned char ch;
    // DLMS_EXCEPTION_STATE_ERROR state;
    DLMS_EXCEPTION_SERVICE_ERROR error;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    // state = (DLMS_EXCEPTION_STATE_ERROR)ch;
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    error = (DLMS_EXCEPTION_SERVICE_ERROR)ch;
    uint32_t value = 0;
    if (error == DLMS_EXCEPTION_SERVICE_ERROR_INVOCATION_COUNTER_ERROR && bb_available(data) > 3)
    {
        bb_getUInt32(data, &value);
    }
    return DLMS_ERROR_TYPE_EXCEPTION_RESPONSE | value << 8 | error;
}
#endif //!defined(DLMS_IGNORE_SERVER)


int dlms_getPdu(
    dlmsSettings* settings,
    gxReplyData* data,
    unsigned char first)
{
    int ret = DLMS_ERROR_CODE_OK;
    uint32_t index;
    unsigned char ch;
    DLMS_COMMAND cmd = data->command;
    // If header is not read yet or GBT message.
    if (cmd == DLMS_COMMAND_NONE)
    {
        // If PDU is missing.
        if (bb_available(&data->data) == 0)
        {
            // Invalid PDU.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        index = data->data.position;
        // Get command.
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        cmd = (DLMS_COMMAND)ch;
        data->command = cmd;
        switch (cmd)
        {
#if !defined(DLMS_IGNORE_CLIENT)
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_READ_RESPONSE:
            if ((ret = dlms_handleReadResponse(settings, data, (uint16_t) index)) != 0)
            {
                if (ret == DLMS_ERROR_CODE_FALSE)
                {
                    return DLMS_ERROR_CODE_OK;
                }
                return ret;
            }
            break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_GET_RESPONSE:
            if ((ret = dlms_handleGetResponse(settings, data, (uint16_t) index)) != 0)
            {
                if (ret == DLMS_ERROR_CODE_FALSE)
                {
                    return DLMS_ERROR_CODE_OK;
                }
                return ret;
            }
            break;
        case DLMS_COMMAND_SET_RESPONSE:
            ret = dlms_handleSetResponse(settings, data);
            break;
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_WRITE_RESPONSE:
            ret = dlms_handleWriteResponse(data);
            break;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_METHOD_RESPONSE:
            ret = dlms_handleMethodResponse(settings, data);
            break;
        case DLMS_COMMAND_GENERAL_BLOCK_TRANSFER:
            ret = dlms_handleGbt(settings, data);
            break;
#endif //!defined(DLMS_IGNORE_CLIENT)
        case DLMS_COMMAND_AARQ:
        case DLMS_COMMAND_AARE:
            // This is parsed later.
            data->data.position -= 1;
            break;
        case DLMS_COMMAND_RELEASE_RESPONSE:
            break;
#if !defined(DLMS_IGNORE_SERVER)
        case DLMS_COMMAND_CONFIRMED_SERVICE_ERROR:
            ret = dlms_handleConfirmedServiceError(&data->data);
            break;
        case DLMS_COMMAND_EXCEPTION_RESPONSE:
            ret = dlms_handleExceptionResponse(&data->data);
            break;
        case DLMS_COMMAND_GET_REQUEST:
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_READ_REQUEST:
        case DLMS_COMMAND_WRITE_REQUEST:
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        case DLMS_COMMAND_SET_REQUEST:
        case DLMS_COMMAND_METHOD_REQUEST:
        case DLMS_COMMAND_RELEASE_REQUEST:
            // Server handles this.
            if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
            {
                break;
            }
            break;
#endif //!defined(DLMS_IGNORE_SERVER)
#ifndef DLMS_IGNORE_HIGH_GMAC
#if !defined(DLMS_IGNORE_SERVER)
        case DLMS_COMMAND_GLO_READ_REQUEST:
        case DLMS_COMMAND_GLO_WRITE_REQUEST:
        case DLMS_COMMAND_GLO_GET_REQUEST:
        case DLMS_COMMAND_GLO_SET_REQUEST:
        case DLMS_COMMAND_GLO_METHOD_REQUEST:
        case DLMS_COMMAND_DED_GET_REQUEST:
        case DLMS_COMMAND_DED_SET_REQUEST:
        case DLMS_COMMAND_DED_METHOD_REQUEST:
            ret = dlms_handleGloDedRequest(settings, data);
            // Server handles this.
            break;
#endif //!defined(DLMS_IGNORE_SERVER)
#if !defined(DLMS_IGNORE_CLIENT)
        case DLMS_COMMAND_GLO_READ_RESPONSE:
        case DLMS_COMMAND_GLO_WRITE_RESPONSE:
        case DLMS_COMMAND_GLO_GET_RESPONSE:
        case DLMS_COMMAND_GLO_SET_RESPONSE:
        case DLMS_COMMAND_GLO_METHOD_RESPONSE:
        case DLMS_COMMAND_DED_GET_RESPONSE:
        case DLMS_COMMAND_DED_SET_RESPONSE:
        case DLMS_COMMAND_DED_EVENT_NOTIFICATION:
        case DLMS_COMMAND_DED_METHOD_RESPONSE:
            // If all frames are read.
            ret = dlms_handleGloDedResponse(settings, data, index);
            break;
#endif // !defined(DLMS_IGNORE_CLIENT)
        case DLMS_COMMAND_GENERAL_GLO_CIPHERING:
        case DLMS_COMMAND_GENERAL_DED_CIPHERING:
#if !defined(DLMS_IGNORE_SERVER)
            if (settings->server)
            {
                if ((settings->connected & DLMS_CONNECTION_STATE_DLMS) == 0)
                {
                    return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
                }
                ret = dlms_handleGloDedRequest(settings, data);
            }
#endif// !defined(DLMS_IGNORE_CLIENT)
#if !defined(DLMS_IGNORE_CLIENT)
#if !defined(DLMS_IGNORE_SERVER)
            else
#endif // !defined(DLMS_IGNORE_SERVER)
            {
                ret = dlms_handleGloDedResponse(settings, data, index);
            }
#endif //!defined(DLMS_IGNORE_CLIENT)
            break;
#if !defined(DLMS_IGNORE_GENERAL_CIPHERING) && !defined(DLMS_IGNORE_HIGH_GMAC)
        case DLMS_COMMAND_GENERAL_CIPHERING:
            ret = dlms_handleGeneralCiphering(settings, data);
            break;
#endif //!defined(DLMS_IGNORE_GENERAL_CIPHERING) && !defined(DLMS_IGNORE_HIGH_GMAC)
#endif //DLMS_IGNORE_HIGH_GMAC
        case DLMS_COMMAND_DATA_NOTIFICATION:
            ret = dlms_handleDataNotification(settings, data);
            // Client handles this.
            break;
        case DLMS_COMMAND_EVENT_NOTIFICATION:
            // Client handles this.
            break;
        case DLMS_COMMAND_INFORMATION_REPORT:
            // Client handles this.
            break;
        default:
            // Invalid command.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        // Is whole block is read and if last packet and data is not try to
        // peek.
        if (!data->peek && data->moreData == DLMS_DATA_REQUEST_TYPES_NONE)
        {
            if (!settings->server || data->command == DLMS_COMMAND_AARE || data->command == DLMS_COMMAND_AARQ)
            {
                data->data.position = 0;
            }
            else
            {
                data->data.position = 1;
            }
        }
        if (cmd == DLMS_COMMAND_GENERAL_BLOCK_TRANSFER)
        {
            data->data.position = data->cipherIndex + 1;
            ret = dlms_handleGbt(settings, data);
            data->cipherIndex = (uint16_t)data->data.size;
            data->command = DLMS_COMMAND_NONE;
        }
        // Get command if operating as a server.
#ifndef DLMS_IGNORE_SERVER
        if (settings->server)
        {
#ifndef DLMS_IGNORE_HIGH_GMAC
            // Ciphered messages are handled after whole PDU is received.
            switch (cmd)
            {
            case DLMS_COMMAND_GLO_READ_REQUEST:
            case DLMS_COMMAND_GLO_WRITE_REQUEST:
            case DLMS_COMMAND_GLO_GET_REQUEST:
            case DLMS_COMMAND_GLO_SET_REQUEST:
            case DLMS_COMMAND_GLO_METHOD_REQUEST:
                data->command = DLMS_COMMAND_NONE;
                data->data.position = (data->cipherIndex);
                ret = dlms_getPdu(settings, data, 0);
                break;
            default:
                break;
            }
#endif //DLMS_IGNORE_HIGH_GMAC
        }
        else
#endif //DLMS_IGNORE_SERVER
        {
            // Client do not need a command any more.
            data->command = DLMS_COMMAND_NONE;
#ifndef DLMS_IGNORE_HIGH_GMAC
            // Ciphered messages are handled after whole PDU is received.
            switch (cmd)
            {
            case DLMS_COMMAND_GLO_READ_RESPONSE:
            case DLMS_COMMAND_GLO_WRITE_RESPONSE:
            case DLMS_COMMAND_GLO_GET_RESPONSE:
            case DLMS_COMMAND_GLO_SET_RESPONSE:
            case DLMS_COMMAND_GLO_METHOD_RESPONSE:
            case DLMS_COMMAND_DED_GET_RESPONSE:
            case DLMS_COMMAND_DED_SET_RESPONSE:
            case DLMS_COMMAND_DED_METHOD_RESPONSE:
            case DLMS_COMMAND_GENERAL_GLO_CIPHERING:
            case DLMS_COMMAND_GENERAL_DED_CIPHERING:
                data->data.position = data->cipherIndex;
                ret = dlms_getPdu(settings, data, 0);
                break;
            default:
                break;
            }
#endif //DLMS_IGNORE_HIGH_GMAC
        }
    }

#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME)
    // Get data only blocks if SN is used. This is faster.
    if (ret == 0 && cmd == DLMS_COMMAND_READ_RESPONSE
        && data->commandType == DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT
        && (data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {
        return 0;
    }
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME)
    // Get data if all data is read or we want to peek data.
    if (ret == 0 && !data->ignoreValue && data->data.position != data->data.size
        && (
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME)
            cmd == DLMS_COMMAND_READ_RESPONSE ||
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME)
            cmd == DLMS_COMMAND_GET_RESPONSE)
        && (data->moreData == DLMS_DATA_REQUEST_TYPES_NONE
            || data->peek))
    {
        ret = dlms_getValueFromData(settings, data);
    }
#else
    data->dataValue.byteArr = &data->data;
    data->dataValue.vt = DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_OCTET_STRING;
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    return ret;
}

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
    gxByteBuffer* data)
{
    int ret;
    if (settings->server)
    {
        ret = bb_insert(LLC_REPLY_BYTES, 3, data, 0);
    }
    else
    {
        ret = bb_insert(LLC_SEND_BYTES, 3, data, 0);
    }
    return ret;
}
#endif //DLMS_IGNORE_HDLC

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int dlms_appendMultipleSNBlocks(
    gxSNParameters* p,
    gxByteBuffer* reply)
{
    uint32_t maxSize;
#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char ciphering = p->settings->cipher.security != DLMS_SECURITY_NONE;
#else
    unsigned char ciphering = 0;
#endif //DLMS_IGNORE_HIGH_GMAC
    uint32_t hSize = reply->size + 3;
    // Add LLC bytes.
    if (p->command == DLMS_COMMAND_WRITE_REQUEST
        || p->command == DLMS_COMMAND_READ_REQUEST)
    {
        hSize += 1 + hlp_getObjectCountSizeInBytes(p->count);
    }
    maxSize = p->settings->maxPduSize - hSize;
    if (ciphering)
    {
        maxSize -= CIPHERING_HEADER_SIZE;
#ifndef DLMS_IGNORE_HDLC
        if (dlms_useHdlc(p->settings->interfaceType))
        {
            maxSize -= 3;
        }
#endif //DLMS_IGNORE_HDLC
    }
    maxSize -= hlp_getObjectCountSizeInBytes(maxSize);
    if ((uint16_t)(p->data->size - p->data->position) > maxSize)
    {
        // More blocks.
        bb_setUInt8(reply, 0);
    }
    else
    {
        // Last block.
        bb_setUInt8(reply, p->lastBlock);
        maxSize = p->data->size - p->data->position;
    }
    // Add block index.
    bb_setUInt16(reply, p->blockIndex);
    if (p->command == DLMS_COMMAND_WRITE_REQUEST)
    {
        ++p->blockIndex;
        hlp_setObjectCount(p->count, reply);
        bb_setUInt8(reply, DLMS_DATA_TYPE_OCTET_STRING);
    }
    else if (p->command == DLMS_COMMAND_READ_REQUEST)
    {
        ++p->blockIndex;
    }

    hlp_setObjectCount(maxSize, reply);
    return maxSize;
}

int dlms_getSNPdu(
    gxSNParameters* p,
    gxByteBuffer* reply)
{
    int ret = 0, cnt = 0;
    unsigned char cipherSize = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char ciphering = p->command != DLMS_COMMAND_AARQ && p->command != DLMS_COMMAND_AARE && p->settings->cipher.security != DLMS_SECURITY_NONE;
#else
    unsigned char ciphering = 0;
#endif //DLMS_IGNORE_HIGH_GMAC
    gxByteBuffer* h;
    if (p->settings->server)
    {
        gxByteBuffer header;
        bb_attach(&header, pduAttributes, 0, sizeof(pduAttributes));
        h = &header;
    }
    else
    {
        h = reply;
    }
    if (ciphering)
    {
        cipherSize = CIPHERING_HEADER_SIZE;
    }
    if (p->data != NULL)
    {
        cnt = p->data->size - p->data->position;
    }
    // Add command.
    if (p->command == DLMS_COMMAND_INFORMATION_REPORT)
    {
        bb_setUInt8(h, (unsigned char)p->command);
        // Add date time.
#ifdef DLMS_USE_EPOCH_TIME
        if (p->time == 0)
#else
        if (p->time == NULL)
#endif // DLMS_USE_EPOCH_TIME
        {
            bb_setUInt8(h, (unsigned char)DLMS_DATA_TYPE_NONE);
        }
        else
        {
            // Data is send in octet string. Remove data type.
            int pos = reply->size;
            dlmsVARIANT tmp;
            gxtime t;

#ifndef DLMS_IGNORE_MALLOC
            tmp.dateTime = &t;
            tmp.vt = DLMS_DATA_TYPE_DATETIME;
#else
            GX_DATETIME(tmp) = &t;
#endif // DLMS_IGNORE_MALLOC
#ifdef DLMS_USE_EPOCH_TIME
            t.value = p->time;
#else
            t.value = *p->time;
#endif //DLMS_USE_EPOCH_TIME
            if ((ret = dlms_setData(reply, DLMS_DATA_TYPE_OCTET_STRING, &tmp)) != 0)
            {
                return ret;
            }
            //Remove data type.
            bb_move(reply, pos + 1, pos, reply->size - pos - 1);
        }
        hlp_setObjectCount(p->count, reply);
        bb_set2(reply, p->attributeDescriptor, 0, p->attributeDescriptor->size);
    }
    else if (p->command != DLMS_COMMAND_AARQ && p->command != DLMS_COMMAND_AARE)
    {
        bb_setUInt8(h, (unsigned char)p->command);
        if (p->count != 0xFF)
        {
            hlp_setObjectCount(p->count, h);
        }
        if (!p->multipleBlocks)
        {
            if (p->requestType != 0xFF)
            {
                bb_setUInt8(h, p->requestType);
            }
            if (p->attributeDescriptor != NULL)
            {
                bb_set2(h, p->attributeDescriptor, 0, p->attributeDescriptor->size);
            }
            p->multipleBlocks = h->size + cipherSize + cnt > p->settings->maxPduSize;
            // If reply data is not fit to one PDU.
            if (p->multipleBlocks)
            {
                reply->size = 0;
                if (p->command == DLMS_COMMAND_WRITE_REQUEST)
                {
                    p->requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_WRITE_DATA_BLOCK_ACCESS;
                }
                else if (p->command == DLMS_COMMAND_READ_REQUEST)
                {
                    p->requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_READ_DATA_BLOCK_ACCESS;
                }
                else if (p->command == DLMS_COMMAND_READ_RESPONSE)
                {
                    p->requestType = DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT;
                }
                else
                {
                    //Invalid command.
                    return DLMS_ERROR_CODE_INVALID_COMMAND;
                }
                bb_setUInt8(reply, (unsigned char)p->command);
                // Set object count.
                bb_setUInt8(reply, 1);
                if (p->requestType != 0xFF)
                {
                    bb_setUInt8(reply, p->requestType);
                }
                cnt = dlms_appendMultipleSNBlocks(p, h);
            }
        }
        else
        {
            if (p->command == DLMS_COMMAND_WRITE_REQUEST)
            {
                p->requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_WRITE_DATA_BLOCK_ACCESS;
            }
            else if (p->command == DLMS_COMMAND_READ_REQUEST)
            {
                p->requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_READ_DATA_BLOCK_ACCESS;
            }
            else if (p->command == DLMS_COMMAND_READ_RESPONSE)
            {
                p->requestType = DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT;
            }
            else
            {
                //Invalid command.
                return DLMS_ERROR_CODE_INVALID_COMMAND;
            }
            if (p->requestType != 0xFF)
            {
                bb_setUInt8(h, p->requestType);
            }
            if (p->attributeDescriptor != NULL)
            {
                bb_set2(h, p->attributeDescriptor, 0, p->attributeDescriptor->size);
            }
            cnt = dlms_appendMultipleSNBlocks(p, h);
        }
    }
    // Add data.
    if (p->settings->server)
    {
        bb_insert(h->data, h->size, reply, 0);
    }
    else if (p->data != NULL)
    {
        bb_set2(reply, p->data, p->data->position, cnt);
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    // If Ciphering is used.
    if (ciphering && p->command != DLMS_COMMAND_AARQ
        && p->command != DLMS_COMMAND_AARE)
    {
        ret = cip_encrypt(
            &p->settings->cipher,
            p->settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            p->settings->cipher.invocationCounter + 1,
            dlms_getGloMessage(p->settings, p->command, p->encryptedCommand),
#ifndef DLMS_IGNORE_MALLOC
            p->settings->cipher.systemTitle.data,
            &p->settings->cipher.blockCipherKey,
#else
            p->settings->cipher.systemTitle,
            p->settings->cipher.blockCipherKey,
#endif //DLMS_IGNORE_MALLOC
            reply);
        if (ret != 0)
        {
            return ret;
        }
        reply->position = reply->size = 0;
    }
#endif //DLMS_IGNORE_HIGH_GMAC
#ifndef DLMS_IGNORE_HDLC
    if (ret == 0 && dlms_useHdlc(p->settings->interfaceType))
    {
        ret = dlms_addLLCBytes(p->settings, reply);
    }
#endif //DLMS_IGNORE_HDLC
    return 0;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME


/**
* Check is all data fit to one data block.
*
* @param p
*            LN parameters.
* @param reply
*            Generated reply.
*/
void dlms_multipleBlocks(
    gxLNParameters* p,
    gxByteBuffer* reply,
    unsigned char ciphering)
{
    // Check is all data fit to one message if data is given.
    int len = bb_available(p->data);
    if (p->attributeDescriptor != NULL)
    {
        len += p->attributeDescriptor->size;
    }
    if (ciphering)
    {
        len += CIPHERING_HEADER_SIZE;
    }
    if (!p->multipleBlocks)
    {
        // Add command type and invoke and priority.
        p->multipleBlocks = 2 + reply->size + len > p->settings->maxPduSize;
    }
    if (p->lastBlock)
    {
        // Add command type and invoke and priority.
        p->lastBlock = !(8 + reply->size + len > p->settings->maxPduSize);
    }
}

int dlms_getLNPdu(
    gxLNParameters* p,
    gxByteBuffer* reply)
{
    int ret = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char ciphering = (p->command != DLMS_COMMAND_AARQ && p->command != DLMS_COMMAND_AARE &&
        p->settings->cipher.security != DLMS_SECURITY_NONE) || p->encryptedCommand != DLMS_COMMAND_NONE;
#else
    unsigned char ciphering = 0;
#endif //DLMS_IGNORE_HIGH_GMAC
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t len = 0;
#else
    uint16_t len = 0;
#endif
    if (p->command == DLMS_COMMAND_AARQ)
    {
        //Data is already added to reply when malloc is not used.
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = bb_set2(reply, p->attributeDescriptor, 0, p->attributeDescriptor->size)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        gxByteBuffer header;
        gxByteBuffer* h;
        if (p->settings->server)
        {
            bb_attach(&header, pduAttributes, 0, sizeof(pduAttributes));
            h = &header;
        }
        else
        {
#ifdef DLMS_IGNORE_MALLOC
            bb_attach(&header, pduAttributes, 0, sizeof(pduAttributes));
            h = &header;
#else
            h = reply;
#endif //DLMS_IGNORE_MALLOC
        }
        // Add command.
        if (p->command != DLMS_COMMAND_GENERAL_BLOCK_TRANSFER)
        {
            ret = bb_setUInt8(h, (unsigned char)p->command);
        }
        if (p->command == DLMS_COMMAND_EVENT_NOTIFICATION ||
            p->command == DLMS_COMMAND_DATA_NOTIFICATION ||
            p->command == DLMS_COMMAND_ACCESS_REQUEST ||
            p->command == DLMS_COMMAND_ACCESS_RESPONSE)
        {
            // Add Long-Invoke-Id-And-Priority
            if (p->command != DLMS_COMMAND_EVENT_NOTIFICATION)
            {
                if (p->invokeId != 0)
                {
                    ret = bb_setUInt32(h, p->invokeId);
                }
                else
                {
                    ret = bb_setUInt32(h, dlms_getLongInvokeIDPriority(p->settings));
                }
            }
            // Add date time.
#ifdef DLMS_USE_EPOCH_TIME
            if (p->time == 0)
#else
            if (p->time == NULL)
#endif // DLMS_USE_EPOCH_TIME
            {
                ret = bb_setUInt8(h, DLMS_DATA_TYPE_NONE);
            }
            else
            {
                // Data is send in octet string. Remove data type.
                uint16_t pos = (uint16_t)h->size;
                dlmsVARIANT tmp;
                gxtime t;
#ifndef DLMS_IGNORE_MALLOC
                tmp.dateTime = &t;
                tmp.vt = DLMS_DATA_TYPE_DATETIME;
#else
                GX_DATETIME(tmp) = &t;
#endif // DLMS_IGNORE_MALLOC
#ifdef DLMS_USE_EPOCH_TIME
                t.value = p->time;
#else
                t.value = *p->time;
#endif // DLMS_USE_EPOCH_TIME
                if ((ret = dlms_setData(h, DLMS_DATA_TYPE_OCTET_STRING, &tmp)) != 0)
                {
                    return ret;
                }
                //Remove data type.
                if (p->command != DLMS_COMMAND_EVENT_NOTIFICATION)
                {
                    bb_move(h, pos + 1, pos, reply->size - pos - 1);
                }
            }
        }
        else if (p->command != DLMS_COMMAND_RELEASE_REQUEST)
        {
            // Get request size can be bigger than PDU size.
            if (p->command != DLMS_COMMAND_GET_REQUEST && p->data != NULL
                && p->data->size != 0)
            {
                dlms_multipleBlocks(p, h, ciphering);
            }
            // Change Request type if Set request and multiple blocks is
            // needed.
            if (p->command == DLMS_COMMAND_SET_REQUEST)
            {
                if (p->multipleBlocks)
                {
                    if (p->requestType == 1)
                    {
                        p->requestType = 2;
                    }
                    else if (p->requestType == 2)
                    {
                        p->requestType = 3;
                    }
                }
            }
            // Change request type If get response and multiple blocks is
            // needed.
            if (p->command == DLMS_COMMAND_GET_RESPONSE)
            {
                if (p->multipleBlocks)
                {
                    if (p->requestType == 1)
                    {
                        p->requestType = 2;
                    }
                }
            }
            ret = bb_setUInt8(h, p->requestType);
            // Add Invoke Id And Priority.
            if (p->invokeId != 0)
            {
                ret = bb_setUInt8(h, p->invokeId);
            }
            else
            {
                ret = bb_setUInt8(h, dlms_getInvokeIDPriority(p->settings, p->settings->autoIncreaseInvokeID));
            }
        }
#ifndef DLMS_IGNORE_MALLOC
        // Add attribute descriptor.
        if (ret == 0 && p->attributeDescriptor != NULL)
        {
            ret = bb_set(reply, p->attributeDescriptor->data, p->attributeDescriptor->size);
        }
#endif //DLMS_IGNORE_MALLOC
        if (ret == 0 &&
            p->command != DLMS_COMMAND_EVENT_NOTIFICATION &&
            p->command != DLMS_COMMAND_DATA_NOTIFICATION &&
            (p->settings->negotiatedConformance & DLMS_CONFORMANCE_GENERAL_BLOCK_TRANSFER) == 0)
        {
            int totalLength;
            // If multiple blocks.
            if (p->multipleBlocks)
            {
                // Is last block.
                if (p->lastBlock)
                {
                    ret = bb_setUInt8(h, 1);
                }
                else
                {
                    ret = bb_setUInt8(h, 0);
                }
                // Block index.
                ret = bb_setUInt32(h, p->blockIndex);
                p->blockIndex = p->blockIndex + 1;
                // Add status if reply.
                if (p->status != 0xFF)
                {
                    if (p->status != 0 && p->command == DLMS_COMMAND_GET_RESPONSE)
                    {
                        ret = bb_setUInt8(h, 1);
                    }
                    ret = bb_setUInt8(h, p->status);
                }
                // Block size.
                if (p->data != NULL)
                {
                    len = (uint16_t)(p->data->size - p->data->position);
                }
                else
                {
                    len = 0;
                }
                totalLength = len + h->size;
#ifndef DLMS_IGNORE_HIGH_GMAC
                if (ciphering)
                {
                    totalLength += CIPHERING_HEADER_SIZE;
                }
#endif //DLMS_IGNORE_HIGH_GMAC
                if (totalLength > p->settings->maxPduSize)
                {
                    len = (uint16_t)(p->settings->maxPduSize - h->size);
#ifndef DLMS_IGNORE_HIGH_GMAC
                    if (ciphering)
                    {
                        len -= CIPHERING_HEADER_SIZE;
                    }
#endif //DLMS_IGNORE_HIGH_GMAC
                    len -= hlp_getObjectCountSizeInBytes(len);
                }
                ret = hlp_setObjectCount(len, h);
                if (ret == 0)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = bb_insert(h->data, h->size, reply, 0)) != 0)
                    {
                        //If this fails PDU buffer size must be bigger.
                        return ret;
                    }
#else
                    if (p->settings->server)
                    {
                        if ((ret = bb_insert(h->data, h->size, reply, 0)) != 0)
                        {
                            //If this fails PDU buffer size must be bigger.
                            return ret;
                        }
                    }
                    else if (p->data != NULL)
                    {
                        ret = bb_set2(reply, p->data, p->data->position, len);
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
        // Add data that fits to one block.
        if (ret == 0 && len == 0)
        {
            // Add status if reply.
            if (p->status != 0xFF)
            {
                if (p->status != 0
                    && (p->command == DLMS_COMMAND_GET_RESPONSE))
                {
                    ret = bb_setUInt8(h, 1);
                }
                ret = bb_setUInt8(h, p->status);
            }
            if (ret == 0 && p->data != NULL && p->data->size != 0)
            {
                len = bb_available(p->data);
                if (len + reply->size > p->settings->maxPduSize)
                {
                    len = (uint16_t)(p->settings->maxPduSize - h->size - p->data->size - p->data->position);
                }
#ifdef DLMS_IGNORE_MALLOC
                ret = bb_insert(h->data, h->size, reply, 0);
                if (!p->settings->server)
                {
                    p->data->position += len;
                }
#else
                if (p->settings->server)
                {
                    ret = bb_insert(h->data, h->size, reply, 0);
                }
                else
                {
                    ret = bb_set2(reply, p->data, p->data->position, len);
                }
#endif //DLMS_IGNORE_MALLOC
            }
            else
            {
#ifdef DLMS_IGNORE_MALLOC
                ret = bb_insert(h->data, h->size, reply, 0);
#else
                if (p->settings->server)
                {
                    ret = bb_insert(h->data, h->size, reply, 0);
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (ret == 0 && ciphering && reply->size != 0 && p->command != DLMS_COMMAND_RELEASE_REQUEST)
        {
#ifndef DLMS_IGNORE_MALLOC
            gxByteBuffer* key;
#else
            unsigned char* key;
#endif //DLMS_IGNORE_MALLOC
            if (dlms_useDedicatedKey(p->settings) && (p->settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0)
            {
                key = p->settings->cipher.dedicatedKey;
            }
            else
            {
#ifndef DLMS_IGNORE_MALLOC
                key = &p->settings->cipher.blockCipherKey;
#else
                key = p->settings->cipher.blockCipherKey;
#endif //DLMS_IGNORE_MALLOC
            }
            ret = cip_encrypt(
                &p->settings->cipher,
                p->settings->cipher.security,
                DLMS_COUNT_TYPE_PACKET,
                p->settings->cipher.invocationCounter,
                dlms_getGloMessage(p->settings, p->command, p->encryptedCommand),
#ifndef DLMS_IGNORE_MALLOC
                p->settings->cipher.systemTitle.data,
#else
                p->settings->cipher.systemTitle,
#endif //DLMS_IGNORE_MALLOC
                key,
                reply);
            ++p->settings->cipher.invocationCounter;
        }
#endif //DLMS_IGNORE_HIGH_GMAC1

        if (p->command == DLMS_COMMAND_GENERAL_BLOCK_TRANSFER ||
            (p->multipleBlocks && (p->settings->negotiatedConformance & DLMS_CONFORMANCE_GENERAL_BLOCK_TRANSFER) != 0))
        {
            bb_setUInt8(h, DLMS_COMMAND_GENERAL_BLOCK_TRANSFER);
            dlms_multipleBlocks(p, h, ciphering);
            // Is last block
            if (!p->lastBlock)
            {
                bb_setUInt8(h, 0);
            }
            else
            {
                bb_setUInt8(h, 0x80);
            }
            // Set block number sent.
            bb_setUInt8(h, 0);
            // Set block number acknowledged
            bb_setUInt8(h, (unsigned char)p->blockIndex);
            p->blockIndex = p->blockIndex + 1;
            // Add APU tag.
            bb_setUInt8(h, 0);
            // Add Addl fields
            bb_setUInt8(h, 0);
        }
    }
#ifndef DLMS_IGNORE_HDLC
    if (ret == 0 && dlms_useHdlc(p->settings->interfaceType))
    {
        ret = dlms_addLLCBytes(p->settings, reply);
    }
#endif //DLMS_IGNORE_HDLC
    return ret;
}

int dlms_getLnMessages(
    gxLNParameters* p,
    message* messages)
{
    int ret;
    gxByteBuffer* pdu;
    gxByteBuffer* it;
#ifndef DLMS_IGNORE_HDLC
    unsigned char frame = 0;
    if (p->command == DLMS_COMMAND_DATA_NOTIFICATION ||
        p->command == DLMS_COMMAND_EVENT_NOTIFICATION)
    {
        if ((p->settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0)
        {
            //If connection is established.
            frame = 0x13;
        }
        else
        {
            frame = 0x3;
        }
    }
#endif //DLMS_IGNORE_HDLC
#ifdef DLMS_IGNORE_MALLOC
    pdu = p->serializedPdu;
#else
    gxByteBuffer reply;
    BYTE_BUFFER_INIT(&reply);
    pdu = &reply;
#endif //DLMS_IGNORE_MALLOC
    do
    {
        if ((ret = dlms_getLNPdu(p, pdu)) == 0)
        {
            p->lastBlock = 1;
            if (p->attributeDescriptor == NULL)
            {
                ++p->settings->blockIndex;
            }
        } while (ret == 0 && pdu->position != pdu->size)
        {
#ifdef DLMS_IGNORE_MALLOC
            if (!(messages->size < messages->capacity))
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                break;
            }
            it = messages->data[messages->size];
            ++messages->size;
            bb_clear(it);
#else
            it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            BYTE_BUFFER_INIT(it);
#endif //DLMS_IGNORE_MALLOC
            switch (p->settings->interfaceType)
            {
#ifndef DLMS_IGNORE_WRAPPER
            case DLMS_INTERFACE_TYPE_WRAPPER:
                ret = dlms_getWrapperFrame(p->settings, p->command, pdu, it);
                break;
#endif //DLMS_IGNORE_WRAPPER
#ifndef DLMS_IGNORE_HDLC
            case DLMS_INTERFACE_TYPE_HDLC:
            case DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E:
                ret = dlms_getHdlcFrame(p->settings, frame, pdu, it);
                if (ret == 0 && pdu->position != pdu->size)
                {
                    frame = getNextSend(p->settings, 0);
                }
                break;
#endif //DLMS_IGNORE_HDLC
            case DLMS_INTERFACE_TYPE_PDU:
                ret = bb_set2(it, pdu, 0, pdu->size);
                break;
#ifndef DLMS_IGNORE_PLC
            case DLMS_INTERFACE_TYPE_PLC:
                ret = dlms_getPlcFrame(p->settings, 0x90, pdu, it);
                break;
            case DLMS_INTERFACE_TYPE_PLC_HDLC:
                ret = dlms_getMacHdlcFrame(p->settings, frame, 0, pdu, it);
                break;
#endif //DLMS_IGNORE_PLC
            default:
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            if (ret != 0)
            {
                break;
            }
#ifndef DLMS_IGNORE_MALLOC
            mes_push(messages, it);
#endif //DLMS_IGNORE_MALLOC
        }
        bb_clear(pdu);
#ifndef DLMS_IGNORE_HDLC
        frame = 0;
#endif //DLMS_IGNORE_HDLC
    } while (ret == 0 && p->data != NULL && p->data->position != p->data->size);
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int dlms_getSnMessages(
    gxSNParameters* p,
    message* messages)
{
    int ret;
    gxByteBuffer data;
    gxByteBuffer* it;
    unsigned char frame = 0x0;
    if (p->command == DLMS_COMMAND_INFORMATION_REPORT)
    {
        if ((p->settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0)
        {
            //If connection is established.
            frame = 0x13;
        }
        else
        {
            frame = 0x3;
        }
    }
    else if (p->command == DLMS_COMMAND_NONE)
    {
        frame = getNextSend(p->settings, 1);
    }
    BYTE_BUFFER_INIT(&data);
    mes_clear(messages);
    do
    {
        ret = dlms_getSNPdu(p, &data);
        // Command is not add to next PDUs.
        while (ret == 0 && data.position != data.size)
        {
#ifndef DLMS_IGNORE_MALLOC
            it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
#else
            if (!(messages->size < messages->capacity))
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            it = messages->data[messages->size];
            ++messages->size;
#endif //DLMS_IGNORE_MALLOC
            BYTE_BUFFER_INIT(it);
            if (p->settings->interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
            {
#ifndef DLMS_IGNORE_WRAPPER
                ret = dlms_getWrapperFrame(p->settings, p->command, &data, it);
#else
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_WRAPPER
            }
            else
            {
#ifndef DLMS_IGNORE_HDLC
                ret = dlms_getHdlcFrame(p->settings, frame, &data, it);
                if (data.position != data.size)
                {
                    frame = getNextSend(p->settings, 0);
                }
#else
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_HDLC
            }
            if (ret != 0)
            {
                break;
            }
#ifndef DLMS_IGNORE_MALLOC
            mes_push(messages, it);
#endif //DLMS_IGNORE_MALLOC
        }
        bb_clear(&data);
        frame = 0;
    } while (ret == 0 && p->data != NULL && p->data->position != p->data->size);
    return 0;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

int dlms_getData2(
    dlmsSettings* settings,
    gxByteBuffer* reply,
    gxReplyData* data,
    unsigned char first)
{
    unsigned char isNotify;
    return dlms_getData3(settings, reply, data, NULL, first, &isNotify);
}

int dlms_getData3(
    dlmsSettings* settings,
    gxByteBuffer* reply,
    gxReplyData* data,
    gxReplyData* notify,
    unsigned char first,
    unsigned char* isNotify)
{
    int ret;
    unsigned char frame = 0;
    if (isNotify != NULL)
    {
        *isNotify = 0;
    }
    switch (settings->interfaceType)
    {
#ifndef DLMS_IGNORE_HDLC
    case DLMS_INTERFACE_TYPE_HDLC:
    case DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E:
        ret = dlms_getHdlcData(settings->server, settings, reply, data, &frame, data->preEstablished, first);
        break;
#endif //DLMS_IGNORE_HDLC
#ifndef DLMS_IGNORE_WRAPPER
    case DLMS_INTERFACE_TYPE_WRAPPER:
        ret = dlms_getTcpData(settings, reply, data, notify, isNotify);
        break;
#endif //DLMS_IGNORE_WRAPPER
#ifndef DLMS_IGNORE_WIRELESS_MBUS
    case DLMS_INTERFACE_TYPE_WIRELESS_MBUS:
        ret = dlms_getMBusData(settings, reply, data);
        break;
#endif //DLMS_IGNORE_WIRELESS_MBUS
    case DLMS_INTERFACE_TYPE_PDU:
        data->packetLength = reply->size;
        data->complete = reply->size != 0;
        ret = 0;
        break;
#ifndef DLMS_IGNORE_PLC
    case DLMS_INTERFACE_TYPE_PLC:
        ret = dlms_getPlcData(settings, reply, data);
        break;
    case DLMS_INTERFACE_TYPE_PLC_HDLC:
        ret = dlms_getPlcHdlcData(settings, reply, data, &frame);
#endif //DLMS_IGNORE_PLC
        break;
    default:
        // Invalid Interface type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    if (ret != 0)
    {
        return ret;
    }
    if (*isNotify && notify != NULL)
    {
        if (!notify->complete)
        {
            // If all data is not read yet.
            return 0;
        }
        data = notify;
    }
    else if (!data->complete)
    {
        // If all data is not read yet.
        return 0;
    }
    if (settings->interfaceType != DLMS_INTERFACE_TYPE_PLC_HDLC)
    {
        if ((ret = dlms_getDataFromFrame(reply, data, dlms_useHdlc(settings->interfaceType))) != 0)
        {
            return ret;
        }
    }
    // If keepalive or get next frame request.
    if (((frame != 0x13 && frame != 0x3) || data->moreData != DLMS_DATA_REQUEST_TYPES_NONE) && (frame & 0x1) != 0)
    {
        if (dlms_useHdlc(settings->interfaceType) && data->data.size != 0)
        {
            if (reply->position != reply->size)
            {
                reply->position += 3;
            }
        }
        if (data->command == DLMS_COMMAND_REJECTED)
        {
            return DLMS_ERROR_CODE_REJECTED;
        }
        return DLMS_ERROR_CODE_OK;
    }
    ret = dlms_getPdu(settings, data, first);
    if (ret == 0 && notify != NULL && !isNotify)
    {
        //Check command to make sure it's not notify message.
        switch (data->command)
        {
        case DLMS_COMMAND_DATA_NOTIFICATION:
        case DLMS_COMMAND_GLO_EVENT_NOTIFICATION_REQUEST:
        case DLMS_COMMAND_INFORMATION_REPORT:
        case DLMS_COMMAND_EVENT_NOTIFICATION:
        case DLMS_COMMAND_DED_EVENT_NOTIFICATION:
            *isNotify = 1;
            notify->complete = data->complete;
            notify->moreData = data->moreData;
            notify->command = data->command;
            data->command = DLMS_COMMAND_NONE;
            notify->time = data->time;
#ifdef DLMS_USE_EPOCH_TIME
            data->time = 0;
#else
            memset(&data->time, 0, sizeof(data->time));
#endif // DLMS_USE_EPOCH_TIME
            bb_set2(&notify->data, &data->data, data->data.position, bb_available(&data->data));
            bb_clear(&data->data);
            notify->dataValue = data->dataValue;
            data->dataValue.vt = DLMS_DATA_TYPE_NONE;
            break;
        default:
            break;
        }
    }
    return ret;
}

int dlms_generateChallenge(
    gxByteBuffer* challenge)
{
    // Random challenge is 8 to 64 bytes.
    // Texas Instruments accepts only 16 byte int32_t challenge.
    // For this reason challenge size is 16 bytes at the moment.
    int ret = 0, pos, len = 16;//hlp_rand() % 58 + 8;
    bb_clear(challenge);
    for (pos = 0; pos != len; ++pos)
    {
        if ((ret = bb_setUInt8(challenge, hlp_rand())) != 0)
        {
            break;
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int dlms_getActionInfo(
    DLMS_OBJECT_TYPE objectType,
    unsigned char* value,
    unsigned char* count)
{
    switch (objectType)
    {
    case DLMS_OBJECT_TYPE_DATA:
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
    case DLMS_OBJECT_TYPE_PPP_SETUP:
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
    case DLMS_OBJECT_TYPE_SCHEDULE:
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        *value = 00;
        *count = 0;
        break;
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        *value = 0x40;
        *count = 4;
        break;
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        *value = 0x50;
        *count = 1;
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        *value = 0x60;
        *count = 4;
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        *value = 0x20;
        *count = 8;
        break;
    case DLMS_OBJECT_TYPE_CLOCK:
        *value = 0x60;
        *count = 6;
        break;
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        *value = 0x48;
        *count = 2;
        break;
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        *value = 0x38;
        *count = 1;
        break;
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        *value = 0x60;
        *count = 3;
        break;
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        *value = 0x60;
        *count = 8;
        break;
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        *value = 0x58;
        *count = 4;
        break;
    case DLMS_OBJECT_TYPE_REGISTER:
        *value = 0x28;
        *count = 1;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        *value = 0x30;
        *count = 3;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        *value = 0x28;
        *count = 2;
        break;
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        *value = 0x20;
        *count = 1;
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        *value = 0x10;
        *count = 2;
        break;
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        *value = 0x20;
        *count = 2;
        break;
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        *value = 0x38;
        *count = 1;
        break;
    default:
        *count = *value = 0;
        break;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

int dlms_secure(
    dlmsSettings* settings,
    int32_t ic,
    gxByteBuffer* data,
    gxByteBuffer* secret,
    gxByteBuffer* reply)
{
    int ret = 0;
    gxByteBuffer challenge;
    bb_clear(reply);
#ifdef DLMS_IGNORE_MALLOC
    bb_attach(&challenge, pduAttributes, 0, sizeof(pduAttributes));
#else
    BYTE_BUFFER_INIT(&challenge);
#endif //DLMS_IGNORE_MALLOC
    if (settings->authentication == DLMS_AUTHENTICATION_HIGH)
    {
#ifndef DLMS_IGNORE_AES
        unsigned char tmp[16];
        gxByteBuffer s;
        uint16_t len = (uint16_t)data->size;
        bb_attach(&s, tmp, 0, sizeof(tmp));
        if (len % 16 != 0)
        {
            len += (16 - (data->size % 16));
        }
        if (secret->size > data->size)
        {
            len = (uint16_t)secret->size;
            if (len % 16 != 0)
            {
                len += (16 - (secret->size % 16));
            }
        }
        if ((ret = bb_set(&s, secret->data, secret->size)) == 0 &&
            (ret = bb_zero(&s, s.size, len - s.size)) == 0 &&
            (ret = bb_set(&challenge, data->data, data->size)) == 0 &&
            (ret = bb_zero(&challenge, challenge.size, len - challenge.size)) == 0 &&
            (ret = bb_capacity(reply, challenge.size)) == 0)
        {
            gxaes_ecb_encrypt(challenge.data, s.data, reply->data, s.size);
        }
        reply->size = s.size;
#ifndef DLMS_IGNORE_MALLOC
        bb_clear(&s);
        bb_clear(&challenge);
#endif //DLMS_IGNORE_MALLOC
        return ret;
#else
        return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#endif //DLMS_IGNORE_AES
    }
    // Get server Challenge.
    // Get shared secret
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->authentication != DLMS_AUTHENTICATION_HIGH_GMAC)
#endif //DLMS_IGNORE_HIGH_GMAC
    {
        if (settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA256)
        {
            //If SHA256 is not used.
#ifdef DLMS_IGNORE_HIGH_SHA256
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
#ifndef DLMS_IGNORE_HIGH_GMAC
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = bb_set(&challenge, secret->data, secret->size)) != 0 ||
                (ret = bb_set(&challenge, settings->cipher.systemTitle, 8)) != 0 ||
                (ret = bb_set(&challenge, settings->sourceSystemTitle, 8)) != 0)
            {
                return ret;
            }
#else
            if ((ret = bb_set(&challenge, secret->data, secret->size)) != 0 ||
                (ret = bb_set(&challenge, settings->cipher.systemTitle.data, settings->cipher.systemTitle.size)) != 0 ||
                (ret = bb_set(&challenge, settings->sourceSystemTitle, 8)) != 0)
            {
                return ret;
            }
#endif //DLMS_IGNORE_MALLOC
            if (settings->server)
            {
                if ((ret = bb_set(&challenge, settings->ctoSChallenge.data, settings->ctoSChallenge.size)) != 0 ||
                    (ret = bb_set(&challenge, settings->stoCChallenge.data, settings->stoCChallenge.size)) != 0)
                {
                    return ret;
                }
            }
            else
            {
                if ((ret = bb_set(&challenge, settings->stoCChallenge.data, settings->stoCChallenge.size)) != 0 ||
                    (ret = bb_set(&challenge, settings->ctoSChallenge.data, settings->ctoSChallenge.size)) != 0)
                {
                    return ret;
                }
            }
#endif //DLMS_IGNORE_HIGH_GMAC
#endif //DLMS_IGNORE_HIGH_SHA256
        }
        else
        {
            if ((ret = bb_set(&challenge, data->data, data->size)) != 0 ||
                (ret = bb_set(&challenge, secret->data, secret->size)) != 0)
            {
                return ret;
            }
        }
    }
    if (settings->authentication == DLMS_AUTHENTICATION_HIGH_MD5)
    {
        //If MD5 is not used.
#ifdef DLMS_IGNORE_HIGH_MD5
        return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
        ret = gxmd5_encrypt(&challenge, reply);
        bb_clear(&challenge);
        return ret;
#endif //DLMS_IGNORE_HIGH_MD5
    }
    else if (settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA1)
    {
        //If SHA1 is not used.
#ifdef DLMS_IGNORE_HIGH_SHA1
        return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
        ret = gxsha1_encrypt(&challenge, reply);
        bb_clear(&challenge);
        return ret;
#endif //DLMS_IGNORE_HIGH_SHA1
    }
    else if (settings->authentication == DLMS_AUTHENTICATION_HIGH_SHA256)
    {
        //If SHA256 is not used.
#ifdef DLMS_IGNORE_HIGH_SHA256
        return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
        ret = gxsha256_encrypt(&challenge, reply);
        bb_clear(&challenge);
        return ret;
#endif //DLMS_IGNORE_HIGH_SHA256
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    else if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC)
    {
        //If GMAC is not used.
#ifdef DLMS_IGNORE_HIGH_GMAC
        bb_clear(&challenge);
        return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
        ret = cip_encrypt(
            &settings->cipher,
            DLMS_SECURITY_AUTHENTICATION,
            DLMS_COUNT_TYPE_TAG,
            ic,
            0,
            secret->data,
#ifdef DLMS_IGNORE_MALLOC
            settings->cipher.blockCipherKey,
#else
            & settings->cipher.blockCipherKey,
#endif //DLMS_IGNORE_MALLOC
            data);
        if (ret == 0)
        {
            if ((ret = bb_setUInt8(reply, DLMS_SECURITY_AUTHENTICATION)) != 0 ||
                (ret = bb_setUInt32(reply, ic)) != 0 ||
                (ret = bb_set2(reply, data, 0, data->size)) != 0)
            {

            }
        }
        bb_clear(&challenge);
#endif //DLMS_IGNORE_HIGH_GMAC
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return ret;
}

int dlms_parseSnrmUaResponse(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    dlmsVARIANT value;
    unsigned char ch, id, len;
    int ret;
    //If default settings are used.
    if (data->size - data->position == 0)
    {
        return 0;
    }
    // Skip FromatID
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    // Skip Group ID.
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    // Skip Group len
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    while (data->position < data->size)
    {
        if ((ret = bb_getUInt8(data, &id)) != 0)
        {
            return ret;
        }
        if ((ret = bb_getUInt8(data, &len)) != 0)
        {
            return ret;
        }
        switch (len)
        {
        case 1:
            ret = bb_getUInt8(data, &value.bVal);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            value.vt = DLMS_DATA_TYPE_UINT8;
            break;
        case 2:
            ret = bb_getUInt16(data, &value.uiVal);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            value.vt = DLMS_DATA_TYPE_UINT16;
            break;
        case 4:
            ret = bb_getUInt32(data, &value.ulVal);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            value.vt = DLMS_DATA_TYPE_UINT32;
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        // RX / TX are delivered from the partner's point of view =>
        // reversed to ours
        switch (id)
        {
        case HDLC_INFO_MAX_INFO_TX:
            if (var_toInteger(&value) < settings->maxInfoRX)
            {
                settings->maxInfoRX = (uint16_t)var_toInteger(&value);
            }
            break;
        case HDLC_INFO_MAX_INFO_RX:
            if (var_toInteger(&value) < settings->maxInfoTX)
            {
                settings->maxInfoTX = (uint16_t)var_toInteger(&value);
            }
            break;
        case HDLC_INFO_WINDOW_SIZE_TX:
            if (var_toInteger(&value) < settings->windowSizeRX)
            {
                settings->windowSizeRX = (unsigned char)var_toInteger(&value);
            }
            break;
        case HDLC_INFO_WINDOW_SIZE_RX:
            if (var_toInteger(&value) < settings->windowSizeTX)
            {
                settings->windowSizeTX = (unsigned char)var_toInteger(&value);
            }
            break;
        default:
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        }
    }
    return ret;
}

int dlms_appendHdlcParameter(gxByteBuffer* data, uint16_t value)
{
    if (value < 0x100)
    {
        bb_setUInt8(data, 1);
        bb_setUInt8(data, (unsigned char)value);
    }
    else
    {
        bb_setUInt8(data, 2);
        bb_setUInt16(data, value);
    }
    return 0;
}

int dlms_isPduFull(dlmsSettings* settings, gxByteBuffer* data, uint16_t* size)
{
    unsigned char ret;
    if (bb_isAttached(data))
    {
        uint16_t len = 0;
        if (size != NULL)
        {
            if (*size == 0)
            {
                *size = (uint16_t)data->size;
            }
            len = *size;
        }
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (settings->cipher.security != DLMS_SECURITY_NONE)
        {
            len += 20 + CIPHERING_HEADER_SIZE + (uint16_t)data->size;
        }
        else
#endif //DLMS_IGNORE_HIGH_GMAC
        {
            len += 20 + (uint16_t)data->size;
        }
        ret = settings->maxPduSize < len;
    }
    else
    {
        ret = 0;
    }
    return ret;
}
