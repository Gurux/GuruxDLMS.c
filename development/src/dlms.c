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
/**
* Get used glo message.
*
* @param command
*            Executed DLMS_COMMAND_
* @return Integer value of glo message.
*/
unsigned char dlms_getGloMessage(DLMS_COMMAND command)
{
    unsigned char cmd;
    switch (command)
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_READ_REQUEST:
        cmd = DLMS_COMMAND_GLO_READ_REQUEST;
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_GET_REQUEST:
        cmd = DLMS_COMMAND_GLO_GET_REQUEST;
        break;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_WRITE_REQUEST:
        cmd = DLMS_COMMAND_GLO_WRITE_REQUEST;
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_SET_REQUEST:
        cmd = DLMS_COMMAND_GLO_SET_REQUEST;
        break;
    case DLMS_COMMAND_METHOD_REQUEST:
        cmd = DLMS_COMMAND_GLO_METHOD_REQUEST;
        break;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_READ_RESPONSE:
        cmd = DLMS_COMMAND_GLO_READ_RESPONSE;
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_GET_RESPONSE:
        cmd = DLMS_COMMAND_GLO_GET_RESPONSE;
        break;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_WRITE_RESPONSE:
        cmd = DLMS_COMMAND_GLO_WRITE_RESPONSE;
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_COMMAND_SET_RESPONSE:
        cmd = DLMS_COMMAND_GLO_SET_RESPONSE;
        break;
    case DLMS_COMMAND_METHOD_RESPONSE:
        cmd = DLMS_COMMAND_GLO_METHOD_RESPONSE;
        break;
    case DLMS_COMMAND_DATA_NOTIFICATION:
        cmd = DLMS_COMMAND_GENERAL_GLO_CIPHERING;
        break;
    case DLMS_COMMAND_RELEASE_REQUEST:
        cmd = DLMS_COMMAND_RELEASE_REQUEST;
        break;
    case DLMS_COMMAND_RELEASE_RESPONSE:
        cmd = DLMS_COMMAND_RELEASE_RESPONSE;
        break;
    default:
        cmd = DLMS_COMMAND_NONE;
    }
    return cmd;
}
#endif //DLMS_IGNORE_HIGH_GMAC

unsigned char dlms_getInvokeIDPriority(dlmsSettings *settings)
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
long dlms_getLongInvokeIDPriority(dlmsSettings *settings)
{
    long value = 0;
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
    dlmsSettings * settings,
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

#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
int dlms_setData2(
    unsigned char *buff,
    unsigned long length,
    DLMS_DATA_TYPE type,
    dlmsVARIANT *value)
#else
int dlms_setData2(
    unsigned char *buff,
    unsigned short length,
    DLMS_DATA_TYPE type,
    dlmsVARIANT *value)
#endif
{
    gxByteBuffer bb;
    bb_attach(&bb, buff, length, length);
    return dlms_setData(&bb, type, value);
}

int dlms_setData(gxByteBuffer *buff, DLMS_DATA_TYPE type, dlmsVARIANT *value)
{
    int ret;
    ret = var_changeType(value, type);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    return var_getBytes2(value, type, buff);
}

int getCount(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
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
int getArray(gxByteBuffer *buff, gxDataInfo *info, unsigned short index, dlmsVARIANT *value)
{
    dlmsVARIANT *tmp;
    gxDataInfo info2;
    unsigned long size;
    unsigned short pos, startIndex;
    int ret;
    if (info->count == 0)
    {
        if ((ret = getCount(buff, info, value)) != 0)
        {
            return ret;
        }
        /*
        int cnt = hlp_getObjectCount2(buff);
        if (cnt == -1)
        {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        info->count = cnt;
        value->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
        if (value->Arr == NULL)
        {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        va_init(value->Arr);
        va_capacity(value->Arr, cnt);
        value->vt = DLMS_DATA_TYPE_ARRAY;
        */
    }
    size = buff->size - buff->position;
    if (info->count != 0 && size < 1)
    {
        info->compleate = 0;
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
        if (!info2.compleate)
        {
            var_clear(tmp);
            gxfree(tmp);
            buff->position = startIndex;
            info->compleate = 0;
            break;
        }
        else
        {
            if (info2.count == info2.index)
            {
                startIndex = (unsigned short)buff->position;
                va_push(value->Arr, tmp);
            }
        }
    }
    info->index = pos;
    return DLMS_ERROR_CODE_OK;
}


/**
* Get UInt32 value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed UInt32 value.
*/
int getUInt32(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT * value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 4)
    {
        info->compleate = 0;
        return 0;
    }
    value->vt = DLMS_DATA_TYPE_UINT32;
    if ((ret = bb_getUInt32(buff, &value->ulVal)) != 0)
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
int getInt32(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT * value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 4)
    {
        info->compleate = 0;
        return 0;
    }
    value->vt = DLMS_DATA_TYPE_INT32;
    if ((ret = bb_getInt32(buff, &value->lVal)) != 0)
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
static int getBitString(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT * value)
{
    double t;
    unsigned short byteCnt;
    unsigned short cnt = 0;
    int ret;
    if (hlp_getObjectCount2(buff, &cnt) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    t = cnt;
    t /= 8;
    if (cnt % 8 != 0)
    {
        ++t;
    }
    byteCnt = (unsigned short)t;
    // If there is not enough data available.
    if (buff->size - buff->position < byteCnt)
    {
        info->compleate = 0;
        return 0;
    }
    value->bitArr = (bitArray*)gxmalloc(sizeof(bitArray));
    ba_init(value->bitArr);
    if ((ret = ba_add(value->bitArr, buff, cnt, 0)) != 0)
    {
        return ret;
    }
    return 0;
}

static int getBool(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    unsigned char ch;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_BOOLEAN;
    value->boolVal = ch != 0;
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
int getString(gxByteBuffer *buff, gxDataInfo *info, unsigned char knownType, dlmsVARIANT *value)
{
    unsigned short len = 0;
    if (knownType)
    {
        len = (unsigned short)buff->size;
    }
    else
    {
        if (hlp_getObjectCount2(buff, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        // If there is not enough data available.
        if (buff->size - buff->position < (unsigned short)len)
        {
            info->compleate = 0;
            return 0;
        }
    }
    value->vt = DLMS_DATA_TYPE_STRING;
    if (len > 0)
    {
        value->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        bb_init(value->strVal);
        bb_set(value->strVal, buff->data + buff->position, (unsigned short)len);
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        buff->position += (unsigned long)len;
#else
        buff->position += (unsigned short)len;
#endif
    }
    return 0;
}


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
    gxByteBuffer *buff,
    gxDataInfo *info,
    unsigned char knownType,
    dlmsVARIANT *value)
{
    unsigned short len = 0;
    var_clear(value);
    if (knownType)
    {
        len = (unsigned short)buff->size;
    }
    else
    {
        if (hlp_getObjectCount2(buff, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        // If there is not enough data available.
        if (buff->size - buff->position < (unsigned short)(2 * len))
        {
            info->compleate = 0;
            return 0;
        }
    }
    if (len > 0)
    {
        value->strUtfVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        bb_init(value->strUtfVal);
        bb_set(value->strUtfVal, buff->data + buff->position, (unsigned short)len);
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        buff->position += (unsigned long)len;
#else
        buff->position += (unsigned short)len;
#endif
        value->vt = DLMS_DATA_TYPE_STRING_UTF8;
    }
    else
    {
        value->strUtfVal = NULL;
    }
    return 0;
}

/**
* Get octect std::string value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  parsed octet std::string value.
*/
int getOctetString(gxByteBuffer *buff, gxDataInfo *info, unsigned char knownType, dlmsVARIANT *value)
{
    unsigned short len;
    int ret = 0;
    if (knownType)
    {
        len = (unsigned short)buff->size;
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
            info->compleate = 0;
            return 0;
        }
    }
    if (len == 0)
    {
        var_clear(value);
    }
    else
    {
        ret = var_addBytes(value, buff->data + buff->position, len);
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        buff->position += (unsigned long)len;
#else
        buff->position += (unsigned short)len;
#endif
    }
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
int getBcd(gxByteBuffer *buff, gxDataInfo *info, unsigned char knownType, dlmsVARIANT *value)
{
    unsigned char idHigh, idLow;
    static const char hexArray[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    int ret, a;
    unsigned short len;
    unsigned char ch;
    value->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
    bb_init(value->strVal);
    value->vt = DLMS_DATA_TYPE_STRING;
    if (knownType)
    {
        len = (unsigned short)buff->size;
    }
    else
    {
        if (hlp_getObjectCount2(buff, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        // If there is not enough data available.
        if ((buff->size - buff->position) < (unsigned short)len)
        {
            info->compleate = 0;
            return 0;
        }
    }
    bb_capacity(value->strVal, (unsigned short)(len * 2));
    for (a = 0; a != len; ++a)
    {
        if ((ret = bb_getUInt8(buff, &ch)) != 0)
        {
            return ret;
        }
        idHigh = ch >> 4;
        idLow = ch & 0x0F;
        bb_setInt8(value->strVal, hexArray[idHigh]);
        bb_setInt8(value->strVal, hexArray[idLow]);
    }
    return 0;
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
int getUInt8(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getUInt8(buff, &value->bVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_UINT8;
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
int getInt16(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 2)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getInt16(buff, &value->iVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_INT16;
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
int getInt8(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getInt8(buff, &value->cVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_INT8;
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
int getUInt16(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 2)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getUInt16(buff, &value->uiVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_UINT16;
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
int getInt64(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 8)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getInt64(buff, &value->llVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_INT64;
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
int getUInt64(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 8)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getUInt64(buff, &value->ullVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_UINT64;
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
int getEnum(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 1)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getUInt8(buff, &value->bVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_ENUM;
    return 0;
}

#ifndef GX_DLMS_MICROCONTROLLER
/**
* Get double value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed double value.
*/
int getDouble(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 8)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getDouble(buff, &value->dblVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_FLOAT64;
    return 0;
}

/**
* Get float value from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed float value.
*/
int getFloat(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    // If there is not enough data available.
    if (buff->size - buff->position < 4)
    {
        info->compleate = 0;
        return 0;
    }
    if ((ret = bb_getFloat(buff, &value->fltVal)) != 0)
    {
        return ret;
    }
    value->vt = DLMS_DATA_TYPE_FLOAT32;
    return 0;
}

#endif //GX_DLMS_MICROCONTROLLER

/**
* Get time from DLMS data.
*
* buff
*            Received DLMS data.
* info
*            Data info.
* Returns  Parsed time.
*/
int getTime(gxByteBuffer* buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret;
    unsigned char ch, hour, minute, second;
    int ms = -1;
    if (buff->size - buff->position < 4)
    {
        // If there is not enough data available.
        info->compleate = 0;
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
    value->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
    time_init(value->dateTime, -1, -1, -1, hour, minute, second, ms, 0x8000);
    value->vt = DLMS_DATA_TYPE_TIME;
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
int getDate(gxByteBuffer* buff, gxDataInfo *info, dlmsVARIANT *value)
{
    int ret, month, day;
    unsigned short year;
    unsigned char ch;
    if (buff->size - buff->position < 5)
    {
        // If there is not enough data available.
        info->compleate = 0;
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
    // Get day
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
    value->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
    time_init(value->dateTime, year, month, day, -1, -1, -1, -1, 0x8000);
    value->vt = DLMS_DATA_TYPE_DATE;
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
int getDateTime(gxByteBuffer *buff, gxDataInfo *info, dlmsVARIANT *value)
{
    unsigned short year;
    int ret, ms, status;
    unsigned char ch;
    // If there is not enough data available.
    if (buff->size - buff->position < 12)
    {
        info->compleate = 0;
        return 0;
    }
    // Get year.
    if ((ret = bb_getUInt16(buff, &year)) != 0)
    {
        return ret;
    }
    value->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
    value->dateTime->value.tm_yday = 0;
    value->dateTime->value.tm_year = year;
    // Get month
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->dateTime->value.tm_mon = ch;
    // Get day
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->dateTime->value.tm_mday = ch;
    // Skip week day
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->dateTime->value.tm_wday = ch;
    // Get time.
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->dateTime->value.tm_hour = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->dateTime->value.tm_min = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->dateTime->value.tm_sec = ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    value->dateTime->skip = DATETIME_SKIPS_NONE;
    ms = ch;
    if (ms != 0xFF)
    {
        ms *= 10;
    }
    else
    {
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_MS);
    }
    if ((ret = bb_getInt16(buff, &value->dateTime->deviation)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    status = ch;
    value->dateTime->status = (DLMS_CLOCK_STATUS)status;
    if (year < 1 || year == 0xFFFF)
    {
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_YEAR);
        value->dateTime->value.tm_year = 0;
    }
    else
    {
        value->dateTime->value.tm_year -= 1900;
    }
    if (value->dateTime->value.tm_wday < 0 || value->dateTime->value.tm_wday > 7)
    {
        value->dateTime->value.tm_wday = 0;
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_DAYOFWEEK);
    }
    value->dateTime->daylightSavingsBegin = value->dateTime->value.tm_mon == 0xFE;
    value->dateTime->daylightSavingsEnd = value->dateTime->value.tm_mon == 0xFD;
    if (value->dateTime->value.tm_mon < 1 || value->dateTime->value.tm_mon > 12)
    {
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_MONTH);
        value->dateTime->value.tm_mon = 0;
    }
    else
    {
        value->dateTime->value.tm_mon -= 1;
    }
    if (value->dateTime->value.tm_mday == -1 || value->dateTime->value.tm_mday == 0 || value->dateTime->value.tm_mday > 31)
    {
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_DAY);
        value->dateTime->value.tm_mday = 1;
    }
    else if (value->dateTime->value.tm_mday < 0)
    {
        //TODO: day = cal.GetActualMaximum(Calendar.DATE) + day + 3;
    }
    if (value->dateTime->value.tm_hour < 0 || value->dateTime->value.tm_hour > 24)
    {
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_HOUR);
        value->dateTime->value.tm_hour = 0;
    }
    if (value->dateTime->value.tm_min < 0 || value->dateTime->value.tm_min > 60)
    {
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_MINUTE);
        value->dateTime->value.tm_min = 0;
    }
    if (value->dateTime->value.tm_sec < 0 || value->dateTime->value.tm_sec > 60)
    {
        value->dateTime->skip = (DATETIME_SKIPS)(value->dateTime->skip | DATETIME_SKIPS_SECOND);
        value->dateTime->value.tm_sec = 0;
    }
    value->dateTime->value.tm_isdst = (status & DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) != 0;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    mktime(&value->dateTime->value);
#endif
    return 0;
}

int dlms_getData(gxByteBuffer* data, gxDataInfo *info, dlmsVARIANT *value)
{
    unsigned char ch, knownType;
    unsigned long startIndex = data->position;
    int ret;
    var_clear(value);
    if (data->position == data->size)
    {
        info->compleate = 0;
        return 0;
    }
    info->compleate = 1;
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
        info->compleate = 0;
        return 0;
    }
    switch (info->type)
    {
    case DLMS_DATA_TYPE_ARRAY:
    case DLMS_DATA_TYPE_STRUCTURE:
    {
        ret = getArray(data, info, (unsigned short)startIndex, value);
        value->vt = info->type;
        break;
    }
    case DLMS_DATA_TYPE_BOOLEAN:
    {
        ret = getBool(data, info, value);
        break;
    }
    case DLMS_DATA_TYPE_BIT_STRING:
        ret = getBitString(data, info, value);
        break;
    case DLMS_DATA_TYPE_INT32:
        ret = getInt32(data, info, value);
        break;
    case DLMS_DATA_TYPE_UINT32:
        ret = getUInt32(data, info, value);
        break;
    case DLMS_DATA_TYPE_STRING:
        ret = getString(data, info, knownType, value);
        break;
    case DLMS_DATA_TYPE_STRING_UTF8:
        ret = getUtfString(data, info, knownType, value);
        break;
    case DLMS_DATA_TYPE_OCTET_STRING:
        ret = getOctetString(data, info, knownType, value);
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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
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
#ifndef GX_DLMS_MICROCONTROLLER
    case DLMS_DATA_TYPE_FLOAT32:
        ret = getFloat(data, info, value);
        break;
    case DLMS_DATA_TYPE_FLOAT64:
        ret = getDouble(data, info, value);
        break;
#endif //GX_DLMS_MICROCONTROLLER
    case DLMS_DATA_TYPE_DATETIME:
        ret = getDateTime(data, info, value);
        break;
    case DLMS_DATA_TYPE_DATE:
        ret = getDate(data, info, value);
        break;
    case DLMS_DATA_TYPE_TIME:
        ret = getTime(data, info, value);
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
    if (ret == 0)
    {
        value->vt = info->type;
    }
    return ret;
}

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
    gxByteBuffer *buff,
    unsigned long *address,
    unsigned char isClientAddress)
{
    unsigned char ch;
    unsigned short s, pos;
    unsigned long l;
    int ret, size = 0;
    for (pos = (unsigned short)buff->position; pos != (unsigned short)buff->size; ++pos)
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
    if (isClientAddress && size != 1)
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
    unsigned short index)
{
    unsigned char ch;
    int ret;
    unsigned long source, target;
    // Get destination and source addresses.
    if ((ret = dlms_getHDLCAddress(reply, &target, 0)) != 0)
    {
        return ret;
    }
    if ((ret = dlms_getHDLCAddress(reply, &source, 1)) != 0)
    {
        return ret;
    }
    if (server)
    {
        // Check that server addresses match.
        if (settings->serverAddress != 0 && settings->serverAddress != (int)target)
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
            settings->serverAddress = (unsigned short)target;
        }

        // Check that client addresses match.
        if (settings->clientAddress != 0 && settings->clientAddress != (int)source)
        {
            // Get frame command.
            if (bb_getUInt8ByIndex(reply, reply->position, &ch) != 0)
            {
                return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
            }
            //If SNRM and client has not call disconnect and changes client ID.
            if (ch == DLMS_COMMAND_SNRM)
            {
                settings->clientAddress = (unsigned short)source;
            }
            else
            {
                return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
            }
        }
        else
        {
            settings->clientAddress = (unsigned short)source;
        }
    }
    else
    {
        // Check that client addresses match.
        if (settings->clientAddress != (int)target)
        {
            // If echo.
            if (settings->clientAddress == (int)source && settings->serverAddress == (int)target)
            {
                reply->position = index + 1;
            }
            return DLMS_ERROR_CODE_FALSE;
        }
        // Check that server addresses match.
        if (settings->serverAddress != (int)source)
        {
            return DLMS_ERROR_CODE_FALSE;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int dlms_getAddress(long value, unsigned long* address, int* size)
{
    if (value < 0x80)
    {
        *address = (unsigned char)(value << 1 | 1);
        *size = 1;
        return 0;
    }
    else if (value < 0x4000)
    {
        *address = (unsigned short)((value & 0x3F80) << 2 | (value & 0x7F) << 1 | 1);
        *size = 2;
    }
    else if (value < 0x10000000)
    {
        *address = (unsigned long)((value & 0xFE00000) << 4 | (value & 0x1FC000) << 3
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
    unsigned long value,
    gxByteBuffer* bytes)
{
    int ret, size;
    unsigned long address;
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
        bb_setUInt16(bytes, (unsigned short)address);
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
    unsigned short crc;
    int ret;
    unsigned short frameSize, len;
    gxByteBuffer primaryAddress, secondaryAddress;
    bb_clear(reply);
    bb_init(&primaryAddress);
    bb_init(&secondaryAddress);
    if (settings->server)
    {
        bb_capacity(&primaryAddress, 1);
        bb_capacity(&secondaryAddress, 4);
        if ((ret = dlms_getAddressBytes(settings->clientAddress, &primaryAddress)) == 0)
        {
            ret = dlms_getAddressBytes(settings->serverAddress, &secondaryAddress);
        }
    }
    else
    {
        bb_capacity(&primaryAddress, 4);
        bb_capacity(&secondaryAddress, 1);
        if ((ret = dlms_getAddressBytes(settings->serverAddress, &primaryAddress)) == 0)
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
        len = (unsigned short)(data->size - data->position);
        // Is last packet.
        ret = bb_setUInt8(reply, (unsigned char)(0xA0 | (((7 + primaryAddress.size + secondaryAddress.size + len) >> 8) & 0x7)));
    }
    else if (ret == 0)
    {
        // More data to left.
        ret = bb_setUInt8(reply, 0xA8);
        len = frameSize;
    }
    // Frame len.
    if (ret == 0 && len == 0)
    {
        ret = bb_setUInt8(reply, (unsigned char)(5 + primaryAddress.size + secondaryAddress.size + len));
    }
    else if (ret == 0)
    {
        if ((ret = bb_capacity(reply, (unsigned short)(11 + len))) == 0)
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
        crc = countCRC(reply, 1, reply->size - 1);
        ret = bb_setUInt16(reply, crc);
    }
    if (ret == 0 && len != 0)
    {
        // Add data.
        if ((ret = bb_set2(reply, data, data->position, len)) == 0)
        {
            // Add data CRC.
            crc = countCRC(reply, 1, reply->size - 1);
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

void dlms_getDataFromFrame(
    gxByteBuffer* reply,
    gxReplyData* data)
{
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long offset = data->data.size;
    unsigned long cnt;
#else
    unsigned short offset = data->data.size;
    unsigned short cnt;
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
        bb_capacity(&data->data, offset + cnt);
        bb_set2(&data->data, reply, reply->position, cnt);
    }
    // Set position to begin of new data.
    data->data.position = offset;
}

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
    unsigned short eopPos;
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long pos, frameLen = 0;
    unsigned long packetStartID = reply->position;
#else
    unsigned short pos, frameLen = 0;
    unsigned short packetStartID = (unsigned short)reply->position;
#endif
    unsigned short crc, crcRead;
    // If whole frame is not received yet.
    if (reply->size - reply->position < 9)
    {
        data->complete = 0;
        return 0;
    }
    data->complete = 1;
    // Find start of HDLC frame.
    for (pos = (unsigned short)reply->position; pos < reply->size; ++pos)
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
    eopPos = (unsigned short)(frameLen + packetStartID + 1);
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
    if ((*frame & 0x8) != 0)
    {
        data->moreData |= DLMS_DATA_REQUEST_TYPES_FRAME;
    }
    else
    {
        data->moreData = ((DLMS_DATA_REQUEST_TYPES)(data->moreData & ~DLMS_DATA_REQUEST_TYPES_FRAME));
    }
    // Get frame type.
    if ((ret = bb_getUInt8(reply, frame)) != 0)
    {
        return ret;
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
            return DLMS_ERROR_CODE_WRONG_CRC;
        }
        // Remove CRC and EOP from packet length.
        data->packetLength = eopPos - 2;
    }
    else
    {
        data->packetLength = eopPos - 2;
    }

    if ((*frame & HDLC_FRAME_TYPE_U_FRAME) == HDLC_FRAME_TYPE_U_FRAME)
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
    }
    else if ((*frame & HDLC_FRAME_TYPE_S_FRAME) == HDLC_FRAME_TYPE_S_FRAME)
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
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
                unsigned long pos = reply->position;
#else
                unsigned short pos = reply->position;
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

int dlms_checkWrapperAddress(dlmsSettings* settings,
    gxByteBuffer *buff)
{
#ifndef DLMS_IGNORE_WRAPPER
    int ret;
    unsigned short value;
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
            return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
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
            return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
        }
        else
        {
            settings->clientAddress = value;
        }
    }
    return DLMS_ERROR_CODE_OK;
#else
    (settings);
    (buff);
    return DLMS_ERROR_CODE_HARDWARE_FAULT;
#endif //DLMS_IGNORE_WRAPPER
}

#ifndef DLMS_IGNORE_WRAPPER
int dlms_getTcpData(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    gxReplyData* data)
{
    int ret, pos;
    unsigned short value;
    // If whole frame is not received yet.
    if (buff->size - buff->position < 8)
    {
        data->complete = 0;
        return DLMS_ERROR_CODE_OK;
    }
    pos = buff->position;

    data->complete = 0;
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
            if ((ret = dlms_checkWrapperAddress(settings, buff)) != 0)
            {
                return ret;
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

int dlms_getDataFromBlock(gxByteBuffer* data, unsigned short index)
{
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long pos, len = data->position - index;
#else
    unsigned short pos, len = data->position - index;
#endif
    if (data->size == data->position)
    {
        bb_clear(data);
        return 0;
    }
    if (len < 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
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
    // Get next frame.
    if ((type & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {
        if ((ret = dlms_getHdlcFrame(settings, getReceiverReady(settings), NULL, reply)) != 0)
        {
            return ret;
        }
        return ret;
    }
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
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
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
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    bb_init(&bb);
    if (settings->useLogicalNameReferencing)
    {
        bb_setUInt32(&bb, settings->blockIndex);
    }
    else
    {
        bb_setUInt16(&bb, (unsigned short)settings->blockIndex);
    }
    ++settings->blockIndex;
    mes_init(&tmp);
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, cmd, DLMS_GET_COMMAND_TYPE_NEXT_DATA_BLOCK, &bb, NULL, 0xFF);
        ret = dlms_getLnMessages(&p, &tmp);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        gxSNParameters p;
        params_initSN(&p, settings, cmd, 1, DLMS_VARIABLE_ACCESS_SPECIFICATION_BLOCK_NUMBER_ACCESS, &bb, NULL);
        ret = dlms_getSnMessages(&p, &tmp);
#else 
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    if (ret == 0)
    {
        bb_set2(reply, (gxByteBuffer*)tmp.data[0], 0, tmp.data[0]->size);
    }
    bb_clear(&bb);
    mes_clear(&tmp);
    return ret;
}

#ifndef DLMS_IGNORE_WRAPPER
int dlms_getWrapperFrame(
    dlmsSettings* settings,
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
            if ((ret = bb_setUInt16(reply, settings->serverAddress)) == 0)
            {
                ret = bb_setUInt16(reply, settings->clientAddress);
            }
        }
        else
        {
            if ((ret = bb_setUInt16(reply, settings->clientAddress)) == 0)
            {
                ret = bb_setUInt16(reply, settings->serverAddress);
            }
        }
        // Data length.
        if (ret == 0 && (ret = bb_setUInt16(reply, (unsigned short)data->size)) == 0)
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

int dlms_handleGetResponse(
    dlmsSettings* settings,
    gxReplyData* reply,
    unsigned short index)
{
    int ret;
    unsigned short count;
    unsigned char ch;
    unsigned long number;
    short type;
    // Get type.
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
    {
        return ret;
    }
    type = ch;
    // Get invoke ID and priority.
    if ((ret = bb_getUInt8(&reply->data, &ch)) != 0)
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
                if (count > (unsigned short)(reply->data.size - reply->data.position))
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
        // Get response with list.
        // Get item count.
        hlp_getObjectCount2(&reply->data, &count);
        dlms_getDataFromBlock(&reply->data, 0);
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
    unsigned short count, pos;
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
    unsigned short pos, count;
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
    unsigned short index;
    int ret, pos;
    dlmsVARIANT* tmp;
    dlmsVARIANT value;
    gxDataInfo info;
    di_init(&info);
    var_init(&value);
    if (reply->dataValue.vt == DLMS_DATA_TYPE_ARRAY)
    {
        info.type = DLMS_DATA_TYPE_ARRAY;
        info.count = (unsigned short)reply->totalCount;
        info.index = (unsigned short)reply->data.size;
    }
    index = (unsigned short)(reply->data.position);
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
                for (pos = 0; pos != value.Arr->size; ++pos)
                {
                    if ((ret = va_getByIndex(value.Arr, pos, &tmp)) != 0)
                    {
                        return ret;
                    }
                    va_push(reply->dataValue.Arr, tmp);
                }
            }
        }
        reply->readPosition = reply->data.position;
        // Element count.
        reply->totalCount = info.count;
    }
    else if (info.compleate
        && reply->command == DLMS_COMMAND_DATA_NOTIFICATION)
    {
        // If last item is null. This is a special case.
        reply->readPosition = reply->data.position;
    }
    reply->data.position = index;

    // If last data frame of the data block is read.
    if (reply->command != DLMS_COMMAND_DATA_NOTIFICATION
        && info.compleate && reply->moreData == DLMS_DATA_REQUEST_TYPES_NONE)
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
    unsigned short index)
{
    int ret;
    unsigned short number;
    unsigned short blockLength;
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
    if (blockLength != reply->data.size - reply->data.position)
    {
        //Invalid block length.
        return DLMS_ERROR_CODE_BLOCK_UNAVAILABLE;
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
    unsigned short index)
{
    int ret;
    unsigned char ch;
    unsigned short number;
    unsigned short pos, cnt = reply->totalCount;
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
    va_init(&values);
    for (pos = 0; pos != cnt; ++pos)
    {
        if (reply->data.size - reply->data.position == 0)
        {
            if (cnt != 1)
            {
                dlms_getDataFromBlock(&reply->data, 0);
            }
            return DLMS_ERROR_CODE_FALSE;
        }
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
                if (reply->data.position == (unsigned long)reply->readPosition)
                {
                    // If multiple values remove command.
                    if (cnt != 1 && reply->totalCount == 0)
                    {
                        ++index;
                    }
                    reply->totalCount = 0;
                    reply->data.position = index;
                    dlms_getDataFromBlock(&reply->data, 0);
                    var_clear(&reply->dataValue);
                    // Ask that data is parsed after last block is received.
                    reply->commandType = DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT;
                    return DLMS_ERROR_CODE_FALSE;
                }
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
            break;
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
        reply->dataValue.Arr = gxmalloc(sizeof(variantArray));
        va_init(reply->dataValue.Arr);
        va_attach(reply->dataValue.Arr, &values);
    }
    if (cnt != 1)
    {
        return DLMS_ERROR_CODE_FALSE;
    }
    return 0;
}

int dlms_handleMethodResponse(
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
    if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
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
            if (ch != 1)
            {
                //ParseApplicationAssociationResponse failed. Invalid tag.
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
            {
                return ret;
            }
            if (ch != 0)
            {
                return ch;
            }
            return dlms_getDataFromBlock(&data->data, 0);
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
    unsigned long ul;
    int ret;
    unsigned short index = (unsigned short)(reply->data.position - 1);
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
    gxReplyData* data)
{
    unsigned char ch, type, invokeId;
    int ret;
    if ((ret = bb_getUInt8(&data->data, &type)) != 0)
    {
        return ret;
    }

    //Invoke ID and priority.
    if ((ret = bb_getUInt8(&data->data, &invokeId)) != 0)
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
        unsigned long  tmp;
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
    dlmsVARIANT *value,
    DLMS_DATA_TYPE type,
    dlmsVARIANT *newValue)
{
    gxByteBuffer bb;
    if (value->byteArr != NULL)
    {
        bb_attach(&bb, value->byteArr->data, value->byteArr->size, value->byteArr->size);
    }
    return dlms_changeType(&bb, type, newValue);

}

int dlms_changeType(
    gxByteBuffer* value,
    DLMS_DATA_TYPE type,
    dlmsVARIANT *newValue)
{
    int ret;
    gxDataInfo info;
    di_init(&info);
    var_clear(newValue);
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
#ifndef GX_DLMS_MICROCONTROLLER
        char *tmp = bb_toHexString(value);
        newValue->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        bb_init(newValue->strVal);
        bb_addString(newValue->strVal, tmp);
        gxfree(tmp);
        newValue->vt = DLMS_DATA_TYPE_STRING;
        return DLMS_ERROR_CODE_OK;
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //GX_DLMS_MICROCONTROLLER       
    }
    info.type = type;
    if ((ret = dlms_getData(value, &info, newValue)) != 0)
    {
        return ret;
    }
    value->position = 0;
    if (!info.compleate)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    if (type == DLMS_DATA_TYPE_OCTET_STRING && newValue->vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
#ifndef GX_DLMS_MICROCONTROLLER
        char *tmp = bb_toHexString(value);
        newValue->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        bb_init(newValue->strVal);
        bb_addString(newValue->strVal, tmp);
        gxfree(tmp);
        newValue->vt = DLMS_DATA_TYPE_STRING;
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //GX_DLMS_MICROCONTROLLER
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
    unsigned long id;
    int ret;
    unsigned char len;
    gxByteBuffer tmp;
    dlmsVARIANT t;
    unsigned short start = (unsigned short)(reply->data.position - 1);
    // Get invoke id.
    if ((ret = bb_getUInt32(&reply->data, &id)) != 0)
    {
        return ret;
    }
    // Get date time.
    reply->time = NULL;
    if ((ret = bb_getUInt8(&reply->data, &len)) != 0)
    {
        return ret;
    }
    if (len != 0)
    {
        var_init(&t);
        bb_init(&tmp);
        bb_set2(&tmp, &reply->data, reply->data.position, len);
        if ((ret = dlms_changeType(&tmp, DLMS_DATA_TYPE_DATETIME, &t)) != 0)
        {
            return ret;
        }
        *reply->time = t.dateTime->value;
    }
    if ((ret = dlms_getDataFromBlock(&reply->data, start)) != 0)
    {
        return ret;
    }
    return dlms_getValueFromData(settings, reply);
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
    unsigned char ch, window, bn, bna;
    unsigned short index = (unsigned short)(data->data.position - 1);
    data->gbt = 1;
    if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
    {
        return ret;
    }
    // Is streaming active.
    //TODO: bool streaming = (ch & 0x40) == 1;
    window = (ch & 0x3F);
    // Block number.
    if ((ret = bb_getUInt8(&data->data, &bn)) != 0)
    {
        return ret;
    }
    // Block number acknowledged.
    if ((ret = bb_getUInt8(&data->data, &bna)) != 0)
    {
        return ret;
    }
    // Get APU tag.
    if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0)
    {
        //Invalid APU.
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    // Get Addl tag.
    if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0)
    {
        //Invalid APU.
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    data->command = DLMS_COMMAND_NONE;
    if (window != 0)
    {
        unsigned short len;
        if (hlp_getObjectCount2(&data->data, &len) != 0)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        if (len != (data->data.size - data->data.position))
        {
            data->complete = 0;
            return 0;
        }
    }
    if ((ret = dlms_getDataFromBlock(&data->data, index)) != 0 ||
        (ret = dlms_getPdu(settings, data, 0)) != 0)
    {
        return ret;
    }
    // Is Last block,
    if ((ch & 0x80) == 0)
    {
        data->moreData = (DLMS_DATA_REQUEST_TYPES)(data->moreData | DLMS_DATA_REQUEST_TYPES_BLOCK);
    }
    else
    {
        data->moreData = (DLMS_DATA_REQUEST_TYPES)(data->moreData & ~DLMS_DATA_REQUEST_TYPES_BLOCK);
    }
    // Get data if all data is read or we want to peek data.
    if (data->data.position != data->data.size
        && (
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            data->command == DLMS_COMMAND_READ_RESPONSE ||
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            data->command == DLMS_COMMAND_GET_RESPONSE)
        && (data->moreData == DLMS_DATA_REQUEST_TYPES_NONE
            || data->peek))
    {
        data->data.position = 0;
        ret = dlms_getValueFromData(settings, data);
    }
    return ret;
}

int dlms_handledGloDedRequest(dlmsSettings* settings,
    gxReplyData* data)
{
    int ret = 0;
#ifdef DLMS_IGNORE_HIGH_GMAC
    ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
    // If all frames are read.
    if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        unsigned char ch;
        DLMS_SECURITY security;
        --data->data.position;
        if (settings->cipher.dedicatedKey != NULL && (settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0)
        {
            if ((ret = cip_decrypt(&settings->cipher,
                &settings->sourceSystemTitle,
                settings->cipher.dedicatedKey,
                &data->data,
                &security)) != 0)
            {
                return ret;
            }
        }
        //If pre-set connection is made.
        else if (settings->sourceSystemTitle.size == 0)
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
                settings->connected |= DLMS_CONNECTION_STATE_DLMS;
                if ((ret = svr_connected((dlmsServerSettings*)settings)) != 0)
                {
                    return ret;
                }
            }
#endif //DLMS_IGNORE_SERVER
            if ((ret = cip_decrypt(&settings->cipher,
                settings->preEstablishedSystemTitle,
                &settings->cipher.blockCipherKey,
                &data->data,
                &security)) != 0)
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
                &settings->sourceSystemTitle,
                &settings->cipher.blockCipherKey,
                &data->data,
                &security)) != 0)
            {
                return ret;
            }
        }
        // Get command.
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        data->command = (DLMS_COMMAND)ch;
    }
    else
    {
        data->data.position = (data->data.position - 1);
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return ret;
}

int dlms_handledGloDedResponse(dlmsSettings* settings,
    gxReplyData* data, unsigned short index)
{
    int ret = 0;
#ifdef DLMS_IGNORE_HIGH_GMAC
    ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#else
    if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        DLMS_SECURITY security;
        gxByteBuffer bb;
        --data->data.position;
        bb_init(&bb);
        bb_set2(&bb, &data->data, 0, bb_size(&data->data));
        data->data.position = data->data.size = index;
        if (settings->cipher.dedicatedKey != NULL && (settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0)
        {
            if ((ret = cip_decrypt(&settings->cipher,
                &settings->sourceSystemTitle,
                settings->cipher.dedicatedKey,
                &bb,
                &security)) != 0)
            {
                bb_clear(&bb);
                return ret;
            }
        }
        else
        {
            if ((ret = cip_decrypt(&settings->cipher,
                &settings->sourceSystemTitle,
                &settings->cipher.blockCipherKey,
                &bb,
                &security)) != 0)
            {
                bb_clear(&bb);
                return ret;
            }
        }
        bb_set2(&data->data, &bb, bb.position, bb.size - bb.position);
        bb_clear(&bb);
        data->command = DLMS_COMMAND_NONE;
        ret = dlms_getPdu(settings, data, 0);
        data->cipherIndex = (unsigned short)data->data.size;
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return ret;
}

int dlms_getPdu(
    dlmsSettings* settings,
    gxReplyData* data,
    unsigned char first)
{
    int ret = DLMS_ERROR_CODE_OK;
    unsigned short index;
    unsigned char ch;
    DLMS_COMMAND cmd = data->command;
    // If header is not read yet or GBT message.
    if (cmd == DLMS_COMMAND_NONE || data->gbt)
    {
        // If PDU is missing.
        if (data->data.size - data->data.position == 0)
        {
            // Invalid PDU.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        index = (unsigned short)(data->data.position);
        // Get command.
        if ((ret = bb_getUInt8(&data->data, &ch)) != 0)
        {
            return ret;
        }
        cmd = (DLMS_COMMAND)ch;
        data->command = cmd;
        switch (cmd)
        {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        case DLMS_COMMAND_READ_RESPONSE:
            if ((ret = dlms_handleReadResponse(settings, data, index)) != 0)
            {
                if (ret == DLMS_ERROR_CODE_FALSE)
                {
                    return DLMS_ERROR_CODE_OK;
                }
                return ret;
            }
            break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        case DLMS_COMMAND_GET_RESPONSE:
            if ((ret = dlms_handleGetResponse(settings, data, index)) != 0)
            {
                if (ret == DLMS_ERROR_CODE_FALSE)
                {
                    return DLMS_ERROR_CODE_OK;
                }
                return ret;
            }
            break;
        case DLMS_COMMAND_SET_RESPONSE:
            ret = dlms_handleSetResponse(data);
            break;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        case DLMS_COMMAND_WRITE_RESPONSE:
            ret = dlms_handleWriteResponse(data);
            break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        case DLMS_COMMAND_METHOD_RESPONSE:
            ret = dlms_handleMethodResponse(data);
            break;
        case DLMS_COMMAND_GENERAL_BLOCK_TRANSFER:
            ret = dlms_handleGbt(settings, data);
            break;
        case DLMS_COMMAND_AARQ:
        case DLMS_COMMAND_AARE:
            // This is parsed later.
            data->data.position = (data->data.position - 1);
            break;
        case DLMS_COMMAND_RELEASE_RESPONSE:
            break;
        case DLMS_COMMAND_EXCEPTION_RESPONSE:
            /* TODO:
            throw new GXDLMSException(
            StateError.values()[data->data.getUInt8() - 1],
            ServiceError.values()[data->data.getUInt8() - 1]);
            */
        case DLMS_COMMAND_GET_REQUEST:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        case DLMS_COMMAND_READ_REQUEST:
        case DLMS_COMMAND_WRITE_REQUEST:
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        case DLMS_COMMAND_SET_REQUEST:
        case DLMS_COMMAND_METHOD_REQUEST:
        case DLMS_COMMAND_RELEASE_REQUEST:
            // Server handles this.
            if ((data->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
            {
                break;
            }
            break;
#ifndef DLMS_IGNORE_HIGH_GMAC
        case DLMS_COMMAND_GLO_READ_REQUEST:
        case DLMS_COMMAND_GLO_WRITE_REQUEST:
        case DLMS_COMMAND_GLO_GET_REQUEST:
        case DLMS_COMMAND_GLO_SET_REQUEST:
        case DLMS_COMMAND_GLO_METHOD_REQUEST:
            ret = dlms_handledGloDedRequest(settings, data);
            // Server handles this.
            break;
        case DLMS_COMMAND_GLO_READ_RESPONSE:
        case DLMS_COMMAND_GLO_WRITE_RESPONSE:
        case DLMS_COMMAND_GLO_GET_RESPONSE:
        case DLMS_COMMAND_GLO_SET_RESPONSE:
        case DLMS_COMMAND_GLO_METHOD_RESPONSE:
            // If all frames are read.
            ret = dlms_handledGloDedResponse(settings, data, index);
            break;
        case DLMS_COMMAND_DED_GET_REQUEST:
        case DLMS_COMMAND_DED_SET_REQUEST:
        case DLMS_COMMAND_DED_METHOD_REQUEST:
            ret = dlms_handledGloDedRequest(settings, data);
            // Server handles this.
            break;
        case DLMS_COMMAND_DED_GET_RESPONSE:
        case DLMS_COMMAND_DED_SET_RESPONSE:
        case DLMS_COMMAND_DED_METHOD_RESPONSE:
        case DLMS_COMMAND_DED_EVENT_NOTIFICATION:
            ret = dlms_handledGloDedResponse(settings, data, index);
            break;
        case DLMS_COMMAND_GENERAL_GLO_CIPHERING:
        case DLMS_COMMAND_GENERAL_DED_CIPHERING:
            if (settings->server)
            {
                ret = dlms_handledGloDedRequest(settings, data);
            }
            else
            {
                ret = dlms_handledGloDedResponse(settings, data, index);
            }
            break;
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
            if (data->command == DLMS_COMMAND_AARE
                || data->command == DLMS_COMMAND_AARQ)
            {
                data->data.position = 0;
            }
            else
            {
                data->data.position = 1;
            }
            resetBlockIndex(settings);
        }
#ifndef DLMS_IGNORE_HIGH_GMAC
        // Get command if operating as a server.
        if (settings->server)
        {
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
        }
        else
        {
            // Client do not need a command any more.
            data->command = DLMS_COMMAND_NONE;
            // Ciphered messages are handled after whole PDU is received.
            switch (cmd)
            {
            case DLMS_COMMAND_GLO_READ_RESPONSE:
            case DLMS_COMMAND_GLO_WRITE_RESPONSE:
            case DLMS_COMMAND_GLO_GET_RESPONSE:
            case DLMS_COMMAND_GLO_SET_RESPONSE:
            case DLMS_COMMAND_GLO_METHOD_RESPONSE:
                data->data.position = data->cipherIndex;
                ret = dlms_getPdu(settings, data, 0);
                break;
            default:
                break;
            }
        }
#endif //DLMS_IGNORE_HIGH_GMAC
    }

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    // Get data only blocks if SN is used. This is faster.
    if (cmd == DLMS_COMMAND_READ_RESPONSE
        && data->commandType == DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT
        && (data->moreData &  DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {
        return 0;
    }
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    // Get data if all data is read or we want to peek data.
    if (data->data.position != data->data.size
        && (
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            cmd == DLMS_COMMAND_READ_RESPONSE ||
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            cmd == DLMS_COMMAND_GET_RESPONSE)
        && (data->moreData == DLMS_DATA_REQUEST_TYPES_NONE
            || data->peek))
    {
        ret = dlms_getValueFromData(settings, data);
    }
    return ret;
}

/**
* Add LLC bytes to generated message.
*
* @param settings
*            DLMS settings.
* @param data
*            Data where bytes are added.
*/
void dlms_addLLCBytes(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    if (settings->server)
    {
        bb_set(data, LLC_REPLY_BYTES, 3);
    }
    else
    {
        bb_set(data, LLC_SEND_BYTES, 3);
    }
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int dlms_appendMultipleSNBlocks(
    gxSNParameters *p,
    gxByteBuffer *reply)
{
    unsigned long maxSize;
#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char ciphering = p->settings->cipher.security != DLMS_SECURITY_NONE;
#else 
    unsigned char ciphering = 0;
#endif //DLMS_IGNORE_HIGH_GMAC
    unsigned long hSize = reply->size + 3;
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
        if (p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
        {
            maxSize -= 3;
        }
    }
    maxSize -= hlp_getObjectCountSizeInBytes(maxSize);
    if ((unsigned short)(p->data->size - p->data->position) > maxSize)
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
    int ret, cnt = 0;
    unsigned char cipherSize = 0;
#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char ciphering = p->command != DLMS_COMMAND_AARQ && p->command != DLMS_COMMAND_AARE && p->settings->cipher.security != DLMS_SECURITY_NONE;
#else 
    unsigned char ciphering = 0;
#endif //DLMS_IGNORE_HIGH_GMAC
    gxByteBuffer *h;
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
    if (!ciphering && p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        dlms_addLLCBytes(p->settings, h);
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
        if (p->time == NULL)
        {
            bb_setUInt8(h, (unsigned char)DLMS_DATA_TYPE_NONE);
        }
        else
        {
            // Data is send in octet string. Remove data type.
            int pos = reply->size;
            dlmsVARIANT tmp;
            tmp.dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
            tmp.dateTime->value = *p->time;
            tmp.vt = DLMS_DATA_TYPE_DATETIME;
            if ((ret = dlms_setData(reply, DLMS_DATA_TYPE_OCTET_STRING, &tmp)) != 0)
            {
                var_clear(&tmp);
                return ret;
            }
            var_clear(&tmp);
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
                if (!ciphering && p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
                {
                    dlms_addLLCBytes(p->settings, reply);
                }
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
        gxByteBuffer tmp;
        bb_init(&tmp);
        ret = cip_encrypt(
            &p->settings->cipher,
            p->settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            p->settings->cipher.invocationCounter + 1,
            dlms_getGloMessage(p->command),
            &p->settings->cipher.systemTitle,
            &p->settings->cipher.blockCipherKey,
            reply,
            &tmp);
        if (ret != 0)
        {
            bb_clear(&tmp);
            return ret;
        }
        reply->position = reply->size = 0;
        if (p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
        {
            dlms_addLLCBytes(p->settings, reply);
        }
        bb_set2(reply, &tmp, 0, tmp.size);
        bb_clear(&tmp);
    }
#endif //DLMS_IGNORE_HIGH_GMAC
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
    gxLNParameters *p,
    gxByteBuffer *reply,
    unsigned char ciphering)
{
    // Check is all data fit to one message if data is given.
    int len = p->data->size - p->data->position;
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
    if (p->multipleBlocks)
    {
        // Add command type and invoke and priority.
        p->lastBlock = p->settings->index == p->settings->count && !(8 + reply->size + len > p->settings->maxPduSize);
    }
    if (p->lastBlock)
    {
        // Add command type and invoke and priority.
        p->lastBlock = p->settings->index == p->settings->count && !(8 + reply->size + len > p->settings->maxPduSize);
    }
}

int dlms_getLNPdu(
    gxLNParameters *p,
    gxByteBuffer *reply)
{
    int ret;
#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char ciphering = p->command != DLMS_COMMAND_AARQ && p->command != DLMS_COMMAND_AARE &&
        p->settings->cipher.security != DLMS_SECURITY_NONE;
#else 
    unsigned char ciphering = 0;
#endif //DLMS_IGNORE_HIGH_GMAC
    unsigned short len = 0;
    if (p->command == DLMS_COMMAND_AARQ)
    {
        if (!ciphering && p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
        {
            dlms_addLLCBytes(p->settings, reply);
        }
        if ((ret = bb_set2(reply, p->attributeDescriptor, 0, p->attributeDescriptor->size)) != 0)
        {
            return ret;
        }
    }
    else
    {
        gxByteBuffer header;
        gxByteBuffer *h;
        if (p->settings->server)
        {
            bb_attach(&header, pduAttributes, 0, sizeof(pduAttributes));
            h = &header;
        }
        else
        {
            h = reply;
        }
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (!ciphering && p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
#else
        if (p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
#endif // DLMS_IGNORE_HIGH_GMAC
        {
            dlms_addLLCBytes(p->settings, h);
        }
        if ((p->settings->negotiatedConformance & DLMS_CONFORMANCE_GENERAL_BLOCK_TRANSFER) != 0)
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
        // Add command.
        bb_setUInt8(h, (unsigned char)p->command);

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
                    bb_setUInt32(h, p->invokeId);
                }
                else
                {
                    bb_setUInt32(h, dlms_getLongInvokeIDPriority(p->settings));
                }
            }
            // Add date time.
            if (p->time == NULL)
            {
                bb_setUInt8(h, DLMS_DATA_TYPE_NONE);
            }
            else
            {
                // Data is send in octet string. Remove data type.
                unsigned short pos = (unsigned short)h->size;
                dlmsVARIANT tmp;
                tmp.dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
                tmp.dateTime->value = *p->time;
                tmp.vt = DLMS_DATA_TYPE_DATETIME;
                if ((ret = dlms_setData(h, DLMS_DATA_TYPE_OCTET_STRING, &tmp)) != 0)
                {
                    var_clear(&tmp);
                    return ret;
                }
                var_clear(&tmp);
                bb_move(h, pos + 1, pos, reply->size - pos - 1);
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
            bb_setUInt8(h, p->requestType);
            // Add Invoke Id And Priority.
            if (p->invokeId != 0)
            {
                bb_setUInt8(h, p->invokeId);
            }
            else
            {
                bb_setUInt8(h, dlms_getInvokeIDPriority(p->settings));
            }
        }
        // Add attribute descriptor.
        if (p->attributeDescriptor != NULL)
        {
            bb_set2(reply, p->attributeDescriptor, 0, p->attributeDescriptor->size);
        }
        if (p->command != DLMS_COMMAND_EVENT_NOTIFICATION &&
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
                    bb_setUInt8(h, 1);
                    p->settings->count = 0;
                    p->settings->index = 0;
                }
                else
                {
                    bb_setUInt8(h, 0);
                }
                // Block index.
                bb_setUInt32(h, p->blockIndex);
                p->blockIndex = p->blockIndex + 1;
                // Add status if reply.
                if (p->status != 0xFF)
                {
                    if (p->status != 0 && p->command == DLMS_COMMAND_GET_RESPONSE)
                    {
                        bb_setUInt8(h, 1);
                    }
                    bb_setUInt8(h, p->status);
                }
                // Block size.
                if (p->data != NULL)
                {
                    len = (unsigned short)(p->data->size - p->data->position);
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
                    len = (unsigned short)(p->settings->maxPduSize - h->size);
#ifndef DLMS_IGNORE_HIGH_GMAC
                    if (ciphering)
                    {
                        len -= CIPHERING_HEADER_SIZE;
                    }
#endif //DLMS_IGNORE_HIGH_GMAC
                    len -= hlp_getObjectCountSizeInBytes(len);
                }
                hlp_setObjectCount(len, h);
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
                    bb_set2(reply, p->data, p->data->position, len);
                }
            }
        }
        // Add data that fits to one block.
        if (len == 0)
        {
            // Add status if reply.
            if (p->status != 0xFF)
            {
                if (p->status != 0
                    && p->command == DLMS_COMMAND_GET_RESPONSE)
                {
                    bb_setUInt8(h, 1);
                }
                bb_setUInt8(h, p->status);
            }
            if (p->data != NULL && p->data->size != 0)
            {
                len = (unsigned short)(p->data->size - p->data->position);
                if (len + reply->size > p->settings->maxPduSize)
                {
                    len = (unsigned short)(p->settings->maxPduSize - h->size - p->data->size - p->data->position);
                }
                if (p->settings->server)
                {
                    bb_insert(h->data, h->size, reply, 0);
                }
                else
                {
                    bb_set2(reply, p->data, p->data->position, len);
                }
            }
            else if (p->settings->server)
            {
                bb_insert(h->data, h->size, reply, 0);
            }
        }
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (ciphering && p->command != DLMS_COMMAND_RELEASE_REQUEST)
        {
            gxByteBuffer tmp;
            gxByteBuffer* key;
            bb_init(&tmp);
            if (p->settings->cipher.dedicatedKey != NULL && (p->settings->connected & DLMS_CONNECTION_STATE_DLMS) != 0)
            {
                key = p->settings->cipher.dedicatedKey;
            }
            else
            {
                key = &p->settings->cipher.blockCipherKey;
            }
            ret = cip_encrypt(
                &p->settings->cipher,
                p->settings->cipher.security,
                DLMS_COUNT_TYPE_PACKET,
                p->settings->cipher.invocationCounter,
                dlms_getGloMessage(p->command),
                &p->settings->cipher.systemTitle,
                key,
                reply,
                &tmp);
            ++p->settings->cipher.invocationCounter;
            if (ret != 0)
            {
                bb_clear(&tmp);
                return ret;
            }
            reply->position = reply->size = 0;
            if (p->settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
            {
                dlms_addLLCBytes(p->settings, reply);
            }
            bb_set2(reply, &tmp, 0, tmp.size);
            bb_clear(&tmp);
        }
#endif //DLMS_IGNORE_HIGH_GMAC1
    }
    return 0;
}

int dlms_getLnMessages(
    gxLNParameters *p,
    message* messages)
{
    int ret;
    gxByteBuffer reply;
    gxByteBuffer *it;
    unsigned char frame = 0;
    if (p->command == DLMS_COMMAND_DATA_NOTIFICATION ||
        p->command == DLMS_COMMAND_EVENT_NOTIFICATION)
    {
        frame = 0x13;
    }
    bb_init(&reply);
    do
    {
        if ((ret = dlms_getLNPdu(p, &reply)) == 0)
        {
            p->lastBlock = 1;
            if (p->attributeDescriptor == NULL)
            {
                ++p->settings->blockIndex;
            }
        }
        while (ret == 0 && reply.position != reply.size)
        {
            it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            bb_init(it);
#ifndef DLMS_IGNORE_WRAPPER
            if (p->settings->interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
            {
                ret = dlms_getWrapperFrame(p->settings, &reply, it);
            }
            else
#endif //DLMS_IGNORE_WRAPPER
            {
                ret = dlms_getHdlcFrame(p->settings, frame, &reply, it);
                if (reply.position != reply.size)
                {
                    frame = getNextSend(p->settings, 0);
                }
            }
            mes_push(messages, it);
        }
        bb_clear(&reply);
    } while (ret == 0 && p->data != NULL && p->data->position != p->data->size);
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int dlms_getSnMessages(
    gxSNParameters *p,
    message* messages)
{
    int ret;
    gxByteBuffer data;
    gxByteBuffer *it;
    unsigned char frame = 0x0;
    if (p->command == DLMS_COMMAND_AARQ)
    {
        frame = 0x10;
    }
    else if (p->command == DLMS_COMMAND_INFORMATION_REPORT)
    {
        frame = 0x13;
    }
    else if (p->command == DLMS_COMMAND_NONE)
    {
        frame = getNextSend(p->settings, 1);
    }
    bb_init(&data);
    do
    {
        ret = dlms_getSNPdu(p, &data);
        // Command is not add to next PDUs.
        while (ret == 0 && data.position != data.size)
        {
            it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            bb_init(it);
            if (p->settings->interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
            {
                ret = dlms_getWrapperFrame(p->settings, &data, it);
            }
            else
            {
                ret = dlms_getHdlcFrame(p->settings, frame, &data, it);
                if (data.position != data.size)
                {
                    if (p->settings->server)
                    {
                        frame = 0;
                    }
                    else
                    {
                        frame = getNextSend(p->settings, 0);
                    }
                }
            }
            if (ret != 0)
            {
                break;
            }
            mes_push(messages, it);
        }
        bb_clear(&data);
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
    int ret;
    unsigned char frame = 0;
    // If DLMS frame is generated.
    if (settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC)
    {
        if ((ret = dlms_getHdlcData(settings->server, settings, reply, data, &frame, data->preEstablished, first)) != 0)
        {
            return ret;
        }
    }
#ifndef DLMS_IGNORE_WRAPPER
    else if (settings->interfaceType == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        if ((ret = dlms_getTcpData(settings, reply, data)) != 0)
        {
            return ret;
        }
    }
#endif //DLMS_IGNORE_WRAPPER
    else
    {
        // Invalid Interface type.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    // If all data is not read yet.
    if (!data->complete)
    {
        return 0;
    }
    dlms_getDataFromFrame(reply, data);
    // If keepalive or get next frame request.
    if (frame != 0x13 && (frame & 0x1) != 0)
    {
        if (settings->interfaceType == DLMS_INTERFACE_TYPE_HDLC && data->data.size != 0)
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
    return dlms_getPdu(settings, data, first);
}

int dlms_generateChallenge(
    gxByteBuffer* challenge)
{
    // Random challenge is 8 to 64 bytes.
    // Texas Instruments accepts only 16 byte long challenge.
    // For this reason challenge size is 16 bytes at the moment.
    int pos, len = 16;//hlp_rand() % 58 + 8;
    bb_clear(challenge);
    for (pos = 0; pos != len; ++pos)
    {
        bb_setUInt8(challenge, hlp_rand());
    }
    return 0;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int dlms_getActionInfo(
    DLMS_OBJECT_TYPE objectType,
    unsigned char *value,
    unsigned char *count)
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
    long ic,
    gxByteBuffer* data,
    gxByteBuffer* secret,
    gxByteBuffer* reply)
{
    int ret = 0;
    gxByteBuffer challenge;
    bb_clear(reply);
    if (settings->authentication == DLMS_AUTHENTICATION_HIGH)
    {
#ifndef DLMS_IGNORE_AES
        gxByteBuffer s;
        unsigned short len = (unsigned short)data->size;
        bb_init(&s);
        bb_attach(&challenge, pduAttributes, 0, sizeof(pduAttributes));
        if (len % 16 != 0)
        {
            len += (16 - (data->size % 16));
        }
        if (secret->size > data->size)
        {
            len = (unsigned short)secret->size;
            if (len % 16 != 0)
            {
                len += (16 - (secret->size % 16));
            }
        }
        bb_set(&s, secret->data, secret->size);
        bb_zero(&s, s.size, len - s.size);
        bb_set(&challenge, data->data, data->size);
        bb_zero(&challenge, challenge.size, len - challenge.size);
        bb_capacity(reply, challenge.size);
        gxaes_ecb_encrypt(challenge.data, s.data, reply->data, s.size);
        reply->size = s.size;
        bb_clear(&challenge);
        bb_clear(&s);
        return 0;
#else        
        return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#endif //DLMS_IGNORE_AES
    }
    // Get server Challenge.
    // Get shared secret
    bb_init(&challenge);
    if (settings->authentication != DLMS_AUTHENTICATION_HIGH_GMAC)
    {
        bb_set(&challenge, data->data, data->size);
        bb_set(&challenge, secret->data, secret->size);
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
            secret,
            &settings->cipher.blockCipherKey,
            data,
            &challenge);
        if (ret == 0)
        {
            bb_setUInt8(reply, DLMS_SECURITY_AUTHENTICATION);
            bb_setUInt32(reply, ic);
            bb_set2(reply, &challenge, 0, challenge.size);
        }
        bb_clear(&challenge);
#endif //DLMS_IGNORE_HIGH_GMAC
    }
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
        settings->maxInfoRX = DEFAULT_MAX_INFO_RX;
        settings->maxInfoTX = DEFAULT_MAX_INFO_TX;
        settings->windowSizeRX = DEFAULT_MAX_WINDOW_SIZE_RX;
        settings->windowSizeTX = DEFAULT_MAX_WINDOW_SIZE_TX;
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
            settings->maxInfoRX = (unsigned short)var_toInteger(&value);
            break;
        case HDLC_INFO_MAX_INFO_RX:
            settings->maxInfoTX = (unsigned short)var_toInteger(&value);
            break;
        case HDLC_INFO_WINDOW_SIZE_TX:
            settings->windowSizeRX = (unsigned char)var_toInteger(&value);
            break;
        case HDLC_INFO_WINDOW_SIZE_RX:
            settings->windowSizeTX = (unsigned char)var_toInteger(&value);
            break;
        default:
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        }
    }
    return ret;
}

int dlms_appendHdlcParameter(gxByteBuffer* data, unsigned short value)
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
