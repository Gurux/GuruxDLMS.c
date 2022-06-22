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
#endif

#include "../include/gxmem.h"
#ifndef DLMS_IGNORE_STRING_CONVERTER
#include <stdio.h> //printf needs this or error is generated.
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#endif //DLMS_IGNORE_STRING_CONVERTER

#include <string.h> /* memset */

#include "../include/variant.h"
#include "../include/errorcodes.h"
#include "../include/helpers.h"

int var_setEnum(dlmsVARIANT* data, unsigned char value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_ENUM;
    data->bVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setUInt8(dlmsVARIANT* data, unsigned char value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_UINT8;
    data->bVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setUInt16(dlmsVARIANT* data, uint16_t value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_UINT16;
    data->uiVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setUInt32(dlmsVARIANT* data, uint32_t value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_UINT32;
    data->ulVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setUInt64(dlmsVARIANT* data, uint64_t value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_UINT64;
    data->ullVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setInt8(dlmsVARIANT* data, char value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_INT8;
    data->cVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setInt16(dlmsVARIANT* data, short value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_INT16;
    data->iVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setInt32(dlmsVARIANT* data, int32_t value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_INT32;
    data->lVal = value;
    return DLMS_ERROR_CODE_OK;
}

int var_setInt64(dlmsVARIANT* data, int64_t value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_INT64;
    data->llVal = value;
    return DLMS_ERROR_CODE_OK;
}

#ifndef DLMS_IGNORE_FLOAT64
int var_setDouble(dlmsVARIANT* data, double value)
{
    var_clear(data);
    data->vt = DLMS_DATA_TYPE_FLOAT64;
    data->dblVal = value;
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_FLOAT64

int var_getUInt8(dlmsVARIANT* data, unsigned char* value)
{
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->bVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_getUInt16(dlmsVARIANT* data, uint16_t* value)
{
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->uiVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_getUInt32(dlmsVARIANT* data, uint32_t* value)
{
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->lVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_getUInt64(dlmsVARIANT* data, uint64_t* value)
{
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->ullVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_getInt8(dlmsVARIANT* data, char* value)
{
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->cVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_getInt16(dlmsVARIANT* data, short* value)
{
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->iVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_getInt32(dlmsVARIANT* data, int32_t* value)
{
    var_clear(data);
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->lVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_getInt64(dlmsVARIANT* data, int64_t* value)
{
    if (data->vt == DLMS_DATA_TYPE_NONE)
    {
        *value = 0;
    }
    else
    {
        *value = data->llVal;
    }
    return DLMS_ERROR_CODE_OK;
}

int var_addBytes(dlmsVARIANT* data, const unsigned char* value, uint16_t count)
{
    if (data->vt != DLMS_DATA_TYPE_OCTET_STRING)
    {
#ifdef DLMS_IGNORE_MALLOC
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#else
        var_clear(data);
        data->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(data->byteArr);
        data->vt = DLMS_DATA_TYPE_OCTET_STRING;
#endif //DLMS_IGNORE_MALLOC
    }
#ifndef DLMS_IGNORE_MALLOC
    else
    {
        bb_clear(data->byteArr);
    }
#endif //DLMS_IGNORE_MALLOC
    return bb_set(data->byteArr, value, count);
}

#ifndef DLMS_IGNORE_MALLOC
int var_setString(dlmsVARIANT* data, const char* value, uint16_t count)
{
    var_clear(data);
    if (data->vt != DLMS_DATA_TYPE_STRING)
    {
        var_clear(data);
        data->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(data->strVal);
        data->vt = DLMS_DATA_TYPE_STRING;
    }
    bb_set(data->strVal, (const unsigned char*)value, count);
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MALLOC

int var_addOctetString(
    dlmsVARIANT* data,
    gxByteBuffer* ba)
{
    var_clear(data);
    return var_addBytes(data, ba->data + ba->position, (uint16_t)(ba->size - ba->position));
}

int var_addByteArray(
    dlmsVARIANT* data,
    gxByteBuffer* ba,
    uint16_t index,
    uint16_t count)
{
    return var_addBytes(data, ba->data + index, count);
}

//Initialize variant.
int var_init(dlmsVARIANT* data)
{
    data->vt = DLMS_DATA_TYPE_NONE;
    data->byteArr = NULL;
    return DLMS_ERROR_CODE_OK;
}

#ifndef DLMS_IGNORE_MALLOC
void var_attachArray(dlmsVARIANT* data,
    const variantArray* arr,
    const uint16_t count)
{
    data->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
    data->vt = DLMS_DATA_TYPE_ARRAY;
    data->Arr->capacity = 0x8000 + count;
    data->Arr->size = count;
    data->Arr->data = (void**)arr->data;
}
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
void var_attachStructure(dlmsVARIANT* data,
    const dlmsVARIANT** arr,
    const uint16_t count)
{
    data->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
    data->vt = DLMS_DATA_TYPE_STRUCTURE;
    data->Arr->capacity = 0x8000 + count;
    data->Arr->size = count;
    data->Arr->data = (void**)arr;
}
#endif //DLMS_IGNORE_MALLOC

//Clear variant.
int var_clear(dlmsVARIANT* data)
{
#ifdef DLMS_IGNORE_MALLOC
    //Referenced values are not cleared. User must do it.
    if ((data->vt & DLMS_DATA_TYPE_BYREF) == 0)
    {
        data->llVal = 0;
        data->vt = DLMS_DATA_TYPE_NONE;
    }
    data->size = 0;
#else
    //Referenced values are not cleared. User must do it.
    if ((data->vt & DLMS_DATA_TYPE_BYREF) != 0)
    {
        return 0;
    }
    switch (data->vt)
    {
    case DLMS_DATA_TYPE_OCTET_STRING:
        if (data->byteArr != NULL)
        {
            bb_clear(data->byteArr);
            if (!bb_isAttached(data->byteArr))
            {
                gxfree(data->byteArr);
                data->byteArr = NULL;
            }
        }
        break;
    case DLMS_DATA_TYPE_STRING_UTF8:
        if (data->strUtfVal != NULL)
        {
            bb_clear(data->strUtfVal);
            if (!bb_isAttached(data->strUtfVal))
            {
                gxfree(data->strUtfVal);
                data->strUtfVal = NULL;
            }
        }
        break;
    case DLMS_DATA_TYPE_STRING:
        if (data->strVal != NULL)
        {
            bb_clear(data->strVal);
            gxfree(data->strVal);
        }
        break;
    case DLMS_DATA_TYPE_ARRAY:
    case DLMS_DATA_TYPE_STRUCTURE:
    case DLMS_DATA_TYPE_COMPACT_ARRAY:
        if (data->Arr != NULL)
        {
            va_clear(data->Arr);
            gxfree(data->Arr);
            data->Arr = NULL;
        }
        break;
    case DLMS_DATA_TYPE_BIT_STRING:
        if (data->bitArr != NULL)
        {
            ba_clear(data->bitArr);
            gxfree(data->bitArr);
        }
        break;
    case DLMS_DATA_TYPE_DATETIME:
    case DLMS_DATA_TYPE_DATE:
    case DLMS_DATA_TYPE_TIME:
        if (data->dateTime != NULL)
        {
            gxfree(data->dateTime);
            data->dateTime = NULL;
        }
        break;
    default:
        data->llVal = 0;
        break;
    }
    data->vt = DLMS_DATA_TYPE_NONE;
#endif //DLMS_IGNORE_MALLOC
    return DLMS_ERROR_CODE_OK;
}

int var_getDateTime2(
    gxtime* dateTime,
    gxByteBuffer* ba)
{
    uint16_t year = 0xFFFF;
    unsigned char month = 0xFF, day = 0xFF, hour = 0xFF, minute = 0xFF, second = 0xFF, dayOfWeek = 0xFF;
#ifdef DLMS_USE_EPOCH_TIME
    time_fromUnixTime2(dateTime->value, &year, &month,
        &day, &hour, &minute, &second, &dayOfWeek);
    //Add year.
    if ((dateTime->skip & DATETIME_SKIPS_YEAR) != 0)
    {
        year = 0xFFFF;
    }
    if ((dateTime->skip & (DATETIME_SKIPS_YEAR | DATETIME_SKIPS_MONTH | DATETIME_SKIPS_DAY)) != 0)
    {
        dayOfWeek = 0xFF;
    }
#else
    if (dateTime->value.tm_isdst && (dateTime->status & DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) == 0)
    {
        time_addMinutes(dateTime, -60);
    }
    //Add year.
    if (dateTime->value.tm_year != -1 && (dateTime->skip & DATETIME_SKIPS_YEAR) == 0)
    {
        year = (uint16_t)(1900 + dateTime->value.tm_year);
    }
    if (dateTime->value.tm_mon != -1 && (dateTime->skip & DATETIME_SKIPS_MONTH) == 0)
    {
        if (dateTime->extraInfo == DLMS_DATE_TIME_EXTRA_INFO_DST_BEGIN)
        {
            month = 0xFE;
        }
        else if (dateTime->extraInfo == DLMS_DATE_TIME_EXTRA_INFO_DST_END)
        {
            month = 0xFD;
        }
        else
        {
            month = (unsigned char)dateTime->value.tm_mon + 1;
        }
    }
    if (dateTime->value.tm_mday != -1 && (dateTime->skip & DATETIME_SKIPS_DAY) == 0)
    {
        if (dateTime->extraInfo == DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY)
        {
            day = 0xFE;
        }
        else if (dateTime->extraInfo == DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY2)
        {
            day = 0xFD;
        }
        else
        {
            day = (unsigned char)dateTime->value.tm_mday;
        }
    }
    //Add week day
    if ((dateTime->value.tm_wday != -1 && dateTime->skip & DATETIME_SKIPS_DAYOFWEEK) == 0)
    {
        dayOfWeek = (unsigned char)dateTime->value.tm_wday;
    }
    //Add Hours
    if (dateTime->value.tm_hour != -1 && (dateTime->skip & DATETIME_SKIPS_HOUR) == 0)
    {
        hour = (unsigned char)dateTime->value.tm_hour;
    }
    //Add Minutes
    if (dateTime->value.tm_min != -1 && (dateTime->skip & DATETIME_SKIPS_MINUTE) == 0)
    {
        minute = (unsigned char)dateTime->value.tm_min;
    }
    //Add seconds.
    if (dateTime->value.tm_sec != -1 && (dateTime->skip & DATETIME_SKIPS_SECOND) == 0)
    {
        second = (unsigned char)dateTime->value.tm_sec;
    }
#endif // DLMS_USE_EPOCH_TIME
    //Add year.
    bb_setUInt16(ba, year);
    //Add month
    if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_DST_BEGIN) != 0)
    {
        bb_setUInt8(ba, 0xFE);
    }
    else if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_DST_END) != 0)
    {
        bb_setUInt8(ba, 0xFD);
    }
    else if ((dateTime->skip & DATETIME_SKIPS_MONTH) == 0)
    {
        bb_setUInt8(ba, month);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add day
    if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY) != 0)
    {
        bb_setUInt8(ba, 0xFE);
    }
    else if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY2) != 0)
    {
        bb_setUInt8(ba, 0xFD);
    }
    else if ((dateTime->skip & DATETIME_SKIPS_DAY) == 0)
    {
        bb_setUInt8(ba, day);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add week day
    if ((dateTime->skip & DATETIME_SKIPS_DAYOFWEEK) == 0)
    {
        //If Sunday.
        if (dayOfWeek == 0)
        {
            dayOfWeek = 7;
        }
        bb_setUInt8(ba, dayOfWeek);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }

    //Add Hours
    if ((dateTime->skip & DATETIME_SKIPS_HOUR) == 0)
    {
        bb_setUInt8(ba, hour);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add Minutes
    if ((dateTime->skip & DATETIME_SKIPS_MINUTE) == 0)
    {
        bb_setUInt8(ba, minute);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add seconds.
    if ((dateTime->skip & DATETIME_SKIPS_SECOND) == 0)
    {
        bb_setUInt8(ba, second);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add ms.
#ifdef DLMS_ITALIAN_STANDARD
    //Italian standard uses 0 for ms.
    bb_setUInt8(ba, 0x00);
#else
    if ((dateTime->skip & DATETIME_SKIPS_MS) == 0)
    {
        bb_setUInt8(ba, 0x00);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
#endif //DLMS_ITALIAN_STANDARD
    //Add Deviation
    if (year == 0xFFFF || (dateTime->skip & DATETIME_SKIPS_DEVITATION) != 0)
    {
        bb_setInt16(ba, 0x8000);//(not specified)
    }
    else
    {
        bb_setInt16(ba, dateTime->deviation);
    }
    //Add clock status
    if ((dateTime->skip & DATETIME_SKIPS_STATUS) != 0)
    {
        bb_setUInt8(ba, 0xFF);
    }
    else
    {
        bb_setUInt8(ba, dateTime->status);
    }
    return 0;
}

int var_getDate(
    gxtime* dateTime,
    gxByteBuffer* ba)
{
    uint16_t year = 0xFFFF;
    unsigned char month = 0xFF, day = 0xFF, dayOfWeek = 0xFF;
#ifdef DLMS_USE_EPOCH_TIME
    time_fromUnixTime2(dateTime->value, &year, &month,
        &day, NULL, NULL, NULL, &dayOfWeek);
    //Add year.
    if ((dateTime->skip & DATETIME_SKIPS_YEAR) != 0)
    {
        year = 0xFFFF;
    }
#else
    //Add year.
    if (dateTime->value.tm_year != -1 && (dateTime->skip & DATETIME_SKIPS_YEAR) == 0)
    {
        year = (uint16_t)(1900 + dateTime->value.tm_year);
    }
    if (dateTime->value.tm_mon != -1 && (dateTime->skip & DATETIME_SKIPS_MONTH) == 0)
    {
        month = (unsigned char)dateTime->value.tm_mon + 1;
    }
    if (dateTime->value.tm_mday != -1 && (dateTime->skip & DATETIME_SKIPS_DAY) == 0)
    {
        day = (unsigned char)dateTime->value.tm_mday;
    }
    //Add week day
    if ((dateTime->value.tm_wday != -1 && dateTime->skip & DATETIME_SKIPS_DAYOFWEEK) == 0)
    {
        dayOfWeek = (unsigned char)dateTime->value.tm_wday;
    }
#endif // DLMS_USE_EPOCH_TIME
    //Add year.
    bb_setUInt16(ba, year);
    //Add month
    if ((dateTime->skip & DATETIME_SKIPS_MONTH) == 0)
    {
        if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_DST_BEGIN) != 0)
        {
            month = 0xFE;
        }
        else if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_DST_END) != 0)
        {
            month = 0xFD;
        }
        bb_setUInt8(ba, month);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add day
    if ((dateTime->skip & DATETIME_SKIPS_DAY) == 0)
    {
        if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY) != 0)
        {
            day = 0xFE;
        }
        else if ((dateTime->extraInfo & DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY2) != 0)
        {
            day = 0xFD;
        }
        bb_setUInt8(ba, day);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add week day
    if ((dateTime->skip & DATETIME_SKIPS_DAYOFWEEK) == 0)
    {
        //If Sunday.
        if (dayOfWeek == 0)
        {
            dayOfWeek = 7;
        }
        bb_setUInt8(ba, dayOfWeek);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    return 0;
}

int var_getTime(
    gxtime* dateTime,
    gxByteBuffer* ba)
{
    unsigned char hour = 0xFF, minute = 0xFF, second = 0xFF;
#ifdef DLMS_USE_EPOCH_TIME
    int ret = time_fromUnixTime2(dateTime->value, NULL, NULL,
        NULL, &hour, &minute, &second, NULL);
    if (ret != 0)
    {
        return ret;
    }
#else
    //Add Hours
    if (dateTime->value.tm_hour != -1 && (dateTime->skip & DATETIME_SKIPS_HOUR) == 0)
    {
        hour = (unsigned char)dateTime->value.tm_hour;
    }
    //Add Minutes
    if (dateTime->value.tm_min != -1 && (dateTime->skip & DATETIME_SKIPS_MINUTE) == 0)
    {
        minute = (unsigned char)dateTime->value.tm_min;
    }
    //Add seconds.
    if (dateTime->value.tm_sec != -1 && (dateTime->skip & DATETIME_SKIPS_SECOND) == 0)
    {
        second = (unsigned char)dateTime->value.tm_sec;
    }
#endif // DLMS_USE_EPOCH_TIME
    //Add Hours
    if ((dateTime->skip & DATETIME_SKIPS_HOUR) == 0)
    {
        bb_setUInt8(ba, hour);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add Minutes
    if ((dateTime->skip & DATETIME_SKIPS_MINUTE) == 0)
    {
        bb_setUInt8(ba, minute);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add seconds.
    if ((dateTime->skip & DATETIME_SKIPS_SECOND) == 0)
    {
        bb_setUInt8(ba, second);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
    //Add ms.
#ifdef DLMS_ITALIAN_STANDARD
    //Italian standard uses 0 for ms.
    bb_setUInt8(ba, 0x00);
#else
    if ((dateTime->skip & DATETIME_SKIPS_MS) == 0)
    {
        bb_setUInt8(ba, 0x00);
    }
    else
    {
        bb_setUInt8(ba, 0xFF);
    }
#endif //DLMS_ITALIAN_STANDARD
    return 0;
}

//Get bytes from variant value.
int var_getBytes(
    dlmsVARIANT* data,
    gxByteBuffer* ba)
{
    return var_getBytes2(data, data->vt, ba);
}

#ifndef DLMS_IGNORE_MALLOC
/**
* Convert octetstring to DLMS bytes.
*
* buff
*            Byte buffer where data is write.
* value
*            Added value.
*/
int var_setOctetString(gxByteBuffer* buff, dlmsVARIANT* value)
{
    if (value->vt == DLMS_DATA_TYPE_STRING)
    {
        gxByteBuffer bb;
        BYTE_BUFFER_INIT(&bb);
        bb_addHexString(&bb, (char*)value->strVal->data);
        hlp_setObjectCount(bb.size, buff);
        bb_set2(buff, &bb, 0, bb.size);
    }
    else if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
        if (value->byteArr == NULL)
        {
            hlp_setObjectCount(0, buff);
        }
        else
        {
            hlp_setObjectCount(value->byteArr->size, buff);
            bb_set(buff, value->byteArr->data, value->byteArr->size);
        }
    }
    else if (value->vt == DLMS_DATA_TYPE_NONE)
    {
        hlp_setObjectCount(0, buff);
    }
    else
    {
        // Invalid data type.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return 0;
}
#endif //DLMS_IGNORE_MALLOC


//Get bytes from variant value.
int var_getBytes2(
    dlmsVARIANT* data,
    DLMS_DATA_TYPE type,
    gxByteBuffer* ba)
{
    return var_getBytes3(data, type, ba, 1);
}

//Returns bytes as Big Endian byteorder.
int var_getBytes3(
    dlmsVARIANT* data,
    DLMS_DATA_TYPE type,
    gxByteBuffer* ba,
    unsigned char addType)
{
    int ret = 0, pos;
    if ((type & DLMS_DATA_TYPE_BYREF) != 0)
    {
        return var_getBytes3(data, type & ~DLMS_DATA_TYPE_BYREF, ba, addType);
    }
    if (type == DLMS_DATA_TYPE_STRUCTURE ||
        type == DLMS_DATA_TYPE_ARRAY)
    {
        dlmsVARIANT* tmp;
        if ((ret = bb_setUInt8(ba, type)) == 0 &&
            (ret = hlp_setObjectCount(data->Arr != NULL ? data->Arr->size : 0, ba)) == 0)
        {
            if (data->Arr != NULL)
            {
                for (pos = 0; pos != data->Arr->size; ++pos)
                {
                    if ((ret = va_getByIndex(data->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK ||
                        (ret = var_getBytes(tmp, ba)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                }
            }
        }
        return ret;
    }
    if (addType)
    {
        if ((ret = bb_setUInt8(ba, type)) != 0)
        {
            return ret;
        }
    }
    switch (type)
    {
    case DLMS_DATA_TYPE_NONE:
        break;
    case DLMS_DATA_TYPE_UINT8:
    case DLMS_DATA_TYPE_ENUM:
        ret = bb_setUInt8(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pbVal : data->bVal);
        break;
    case DLMS_DATA_TYPE_BOOLEAN:
        if ((data->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = bb_setUInt8(ba, *data->pbVal == 0 ? 0 : 1);
        }
        else
        {
            ret = bb_setUInt8(ba, data->bVal == 0 ? 0 : 1);
        }
        break;
    case DLMS_DATA_TYPE_UINT16:
        ret = bb_setUInt16(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->puiVal : data->uiVal);
        break;
    case DLMS_DATA_TYPE_UINT32:
        ret = bb_setUInt32(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pulVal : data->ulVal);
        break;
    case DLMS_DATA_TYPE_UINT64:
        ret = bb_setUInt64(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pullVal : data->ullVal);
        break;
    case DLMS_DATA_TYPE_INT8:
        ret = bb_setInt8(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pcVal : data->cVal);
        break;
    case DLMS_DATA_TYPE_INT16:
        ret = bb_setInt16(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->puiVal : data->uiVal);
        break;
    case DLMS_DATA_TYPE_INT32:
        ret = bb_setUInt32(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->plVal : data->lVal);
        break;
    case DLMS_DATA_TYPE_INT64:
        ret = bb_setInt64(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pllVal : data->llVal);
        break;
    case DLMS_DATA_TYPE_FLOAT32:
#ifndef DLMS_IGNORE_FLOAT32
        ret = bb_setFloat(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pfltVal : data->fltVal);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_FLOAT32
        break;
    case DLMS_DATA_TYPE_FLOAT64:
#ifndef DLMS_IGNORE_FLOAT64
        ret = bb_setDouble(ba, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pdblVal : data->dblVal);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_FLOAT64
        break;
    case DLMS_DATA_TYPE_STRING:
#ifndef DLMS_IGNORE_MALLOC
        if (data->strVal == NULL)
        {
            ret = hlp_setObjectCount(0, ba);
        }
        else
        {
            if ((ret = hlp_setObjectCount(data->strVal->size, ba)) == 0)
            {
                ret = bb_set(ba, data->strVal->data, data->strVal->size);
            }
        }
#else
        if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_STRING))
        {
            if ((ret = hlp_setObjectCount(data->size, ba)) != 0 ||
                (ret = bb_set(ba, data->pbVal, data->size)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_OCTET_STRING:
#ifndef DLMS_IGNORE_MALLOC
        if (data->vt == DLMS_DATA_TYPE_DATETIME)
        {
            if ((ret = bb_setUInt8(ba, 12)) == 0)
            {
                ret = var_getDateTime2(data->dateTime, ba);
            }
        }
        else if (data->vt == DLMS_DATA_TYPE_DATE)
        {
            if ((ret = bb_setUInt8(ba, 5)) == 0)
            {
                ret = var_getDate(data->dateTime, ba);
            }
        }
        else if (data->vt == DLMS_DATA_TYPE_TIME)
        {
            if ((ret = bb_setUInt8(ba, 4)) == 0)
            {
                ret = var_getTime(data->dateTime, ba);
            }
        }
        else
        {
            ret = var_setOctetString(ba, data);
        }
#else
        if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATETIME))
        {
            if ((ret = bb_setUInt8(ba, 12)) == 0)
            {
                ret = var_getDateTime2(data->pVal, ba);
            }
        }
        else if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATE))
        {
            if ((ret = bb_setUInt8(ba, 5)) == 0)
            {
                ret = var_getDate(data->pVal, ba);
            }
        }
        else if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_TIME))
        {
            if ((ret = bb_setUInt8(ba, 4)) == 0)
            {
                ret = var_getTime(data->pVal, ba);
            }
        }
        else if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_OCTET_STRING))
        {
            if ((ret = hlp_setObjectCount(data->size, ba)) != 0 ||
                (ret = bb_set(ba, data->pbVal, data->size)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
        else
        {
            if ((ret = hlp_setObjectCount(data->byteArr->size, ba)) != 0 ||
                (ret = bb_set(ba, data->byteArr->data, data->byteArr->size)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_DATETIME:
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = var_getDateTime2(data->pVal, ba);
#else
        ret = var_getDateTime2(data->dateTime, ba);
#endif //DLMS_IGNORE_MALLOC
        break;
    }
    case DLMS_DATA_TYPE_DATE:
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = var_getDate(data->pVal, ba);
#else
        ret = var_getDate(data->dateTime, ba);
#endif //DLMS_IGNORE_MALLOC

        break;
    }
    case DLMS_DATA_TYPE_TIME:
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = var_getTime(data->pVal, ba);
#else
        ret = var_getTime(data->dateTime, ba);
#endif //DLMS_IGNORE_MALLOC

        break;
    }
    case DLMS_DATA_TYPE_BIT_STRING:
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = hlp_setObjectCount(data->size, ba)) == 0)
        {
            ret = bb_set(ba, data->pVal, ba_getByteCount(data->size));
        }
#else
        if ((ret = hlp_setObjectCount(data->bitArr->size, ba)) == 0)
        {
            ret = bb_set(ba, data->bitArr->data, ba_getByteCount(data->bitArr->size));
        }
#endif //DLMS_IGNORE_MALLOC
        break;
    }
    default:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

//Get size in bytes.
int var_getSize(DLMS_DATA_TYPE vt)
{
    int nSize = -1;
    switch (vt)
    {
    case DLMS_DATA_TYPE_NONE:
        nSize = 0;
        break;
    case DLMS_DATA_TYPE_BOOLEAN:
    case DLMS_DATA_TYPE_INT8:
    case DLMS_DATA_TYPE_UINT8:
    case DLMS_DATA_TYPE_ENUM:
        nSize = 1;
        break;
    case DLMS_DATA_TYPE_INT16:
    case DLMS_DATA_TYPE_UINT16:
        nSize = 2;
        break;
    case DLMS_DATA_TYPE_INT32:
    case DLMS_DATA_TYPE_UINT32:
    case DLMS_DATA_TYPE_FLOAT32:
        nSize = 4;
        break;
    case DLMS_DATA_TYPE_INT64:
    case DLMS_DATA_TYPE_UINT64:
    case DLMS_DATA_TYPE_FLOAT64:
        nSize = 8;
        break;
    case DLMS_DATA_TYPE_BIT_STRING:
    case DLMS_DATA_TYPE_OCTET_STRING:
    case DLMS_DATA_TYPE_STRING:
    case DLMS_DATA_TYPE_STRING_UTF8:
        nSize = -1;
        break;
    case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
        break;
    case DLMS_DATA_TYPE_DATETIME:
        nSize = 12;
        break;
        //case DLMS_DATA_TYPE_DATE:
        //case DLMS_DATA_TYPE_TIME:
        //case DLMS_DATA_TYPE_ARRAY:
        //case DLMS_DATA_TYPE_STRUCTURE:
        //case DLMS_DATA_TYPE_COMPACT_ARRAY:
    default:
        break;
    }
    return nSize;
}

//Convert variant value to integer.
int var_toInteger(dlmsVARIANT* data)
{
    int ret;
    if ((data->vt & DLMS_DATA_TYPE_BYREF) != 0)
    {
        dlmsVARIANT tmp;
        if ((ret = var_copy(&tmp, data)) == 0)
        {
            ret = var_toInteger(&tmp);
        }
        else
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
            ret = -1;
        }
        return ret;
    }
    switch (data->vt)
    {
    case DLMS_DATA_TYPE_NONE:
        ret = 0;
        break;
    case DLMS_DATA_TYPE_BOOLEAN:
        ret = data->boolVal ? 1 : 0;
        break;
    case DLMS_DATA_TYPE_INT32:
        ret = data->lVal;
        break;
    case DLMS_DATA_TYPE_UINT32:
        ret = data->ulVal;
        break;
    case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = 0;
        break;
    case DLMS_DATA_TYPE_STRING_UTF8:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = 0;
        break;
    case DLMS_DATA_TYPE_INT8:
        ret = data->cVal;
        break;
    case DLMS_DATA_TYPE_INT16:
        ret = data->iVal;
        break;
    case DLMS_DATA_TYPE_UINT8:
        ret = data->bVal;
        break;
    case DLMS_DATA_TYPE_UINT16:
        ret = data->uiVal;
        break;
    case DLMS_DATA_TYPE_INT64:
        ret = (int)data->llVal;
        break;
    case DLMS_DATA_TYPE_UINT64:
        ret = (int)data->ullVal;
        break;
    case DLMS_DATA_TYPE_ENUM:
        ret = data->bVal;
        break;
#ifndef DLMS_IGNORE_FLOAT32
    case DLMS_DATA_TYPE_FLOAT32:
        ret = (int)data->fltVal;
        break;
#endif //DLMS_IGNORE_FLOAT32
#ifndef DLMS_IGNORE_FLOAT64
    case DLMS_DATA_TYPE_FLOAT64:
        ret = (int)data->dblVal;
        break;
#endif //DLMS_IGNORE_FLOAT64
#ifndef DLMS_IGNORE_MALLOC
    case DLMS_DATA_TYPE_STRING:
        ret = hlp_stringToInt((const char*)data->strVal);
        break;
    case DLMS_DATA_TYPE_BIT_STRING:
    {
        uint32_t value;
        ba_toInteger(data->bitArr, &value);
        ret = (int)value;
    }
    break;
#endif //DLMS_IGNORE_MALLOC
    default:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = 0;
        break;
    }
    return ret;
}

char va_isAttached(variantArray* arr)
{
    return (arr->capacity & 0x8000) == 0x8000;
}

uint16_t va_size(variantArray* arr)
{
    return arr == NULL ? 0 : arr->size;
}

uint16_t va_getCapacity(variantArray* arr)
{
    return arr->capacity & 0x7FFF;
}

//Initialize variantArray.
void va_init(variantArray* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
    arr->size = 0;
}

//Allocate new size for the array in bytes.
int va_capacity(variantArray* arr, uint16_t capacity)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!va_isAttached(arr))
    {
        if (capacity == 0)
        {
            if (arr->capacity != 0)
            {
                gxfree(arr->data);
                arr->size = 0;
            }
        }
        else
        {
            if (arr->capacity == 0)
            {
                arr->data = (void**)gxmalloc(capacity * sizeof(dlmsVARIANT*));
                if (arr->data == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
            }
            else
            {
#ifdef gxrealloc
                //If compiler supports realloc.
                void** tmp = (void**)gxrealloc(arr->data, capacity * sizeof(dlmsVARIANT*));
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr->data = tmp;
#else
                //If compiler doesn't support realloc.
                void** old = arr->data;
                arr->data = (void**)gxmalloc(capacity * sizeof(dlmsVARIANT*));
                //If not enought memory available.
                if (arr->data == NULL)
                {
                    arr->data = old;
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(arr->data, old, sizeof(dlmsVARIANT*) * arr->size);
                gxfree(old);
 #endif // gxrealloc  
            }
        }
        arr->capacity = capacity;
    }
#endif //DLMS_IGNORE_MALLOC
    if (va_getCapacity(arr) < capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return 0;
}

//Push new data to the variantArray.
int va_push(variantArray* arr, dlmsVARIANT* item)
{
    dlmsVARIANT** p;
#ifndef DLMS_IGNORE_MALLOC
    if (!va_isAttached(arr))
    {
        if (arr->size >= arr->capacity)
        {
            arr->capacity += VARIANT_ARRAY_CAPACITY;
            if (arr->size == 0)
            {
                arr->data = (void**)gxmalloc(arr->capacity * sizeof(dlmsVARIANT*));
            }
            else
            {
#ifdef gxrealloc
                //If compiler supports realloc.
                void** tmp = (void**)gxrealloc(arr->data, arr->capacity * sizeof(dlmsVARIANT*));
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr->data = tmp;
#else
                //If compiler doesn't support realloc.
                void** old = arr->data;
                arr->data = (void**)gxmalloc(arr->capacity * sizeof(dlmsVARIANT*));
                //If not enought memory available.
                if (arr->data == NULL)
                {
                    arr->data = old;
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(arr->data, old, sizeof(dlmsVARIANT*) * arr->size);
                gxfree(old);
 #endif // gxrealloc  
            }
        }
    }
#endif //DLMS_IGNORE_MALLOC
    if (va_getCapacity(arr) <= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    p = (dlmsVARIANT**)arr->data;
    p[arr->size] = item;
    ++arr->size;
    return 0;
}

void va_clear(
    variantArray* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    unsigned char attached = va_isAttached(arr);
    if (arr->data != NULL && !attached)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            var_clear((dlmsVARIANT*)arr->data[pos]);
            gxfree(arr->data[pos]);
        }
        gxfree(arr->data);
        arr->data = NULL;
    }
    if (!attached)
    {
        arr->capacity = 0;
    }
#endif //DLMS_IGNORE_MALLOC
    arr->size = 0;
}

#ifdef DLMS_IGNORE_MALLOC
void va_attach(
    variantArray* trg,
    dlmsVARIANT* src,
    uint16_t size,
    uint16_t capacity)
{
    trg->data = src;
    trg->capacity = (uint16_t)(0x8000 | capacity);
    trg->size = size;
}
#endif //DLMS_IGNORE_MALLOC

void va_attach2(
    variantArray* trg,
    variantArray* src)
{
    trg->capacity = src->capacity;
    trg->data = src->data;
    trg->size = src->size;
    src->data = NULL;
    src->size = src->capacity = 0;
}

//Get item from variant array by index.
int va_getByIndex(variantArray* arr, int index, dlmsVARIANT_PTR* item)
{
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }

#ifdef DLMS_IGNORE_MALLOC
    dlmsVARIANT_PTR p = (dlmsVARIANT_PTR)arr->data;
    *item = &p[index];
    return DLMS_ERROR_CODE_OK;
#else
    dlmsVARIANT** p = (dlmsVARIANT**)arr->data;
    *item = p[index];
    return DLMS_ERROR_CODE_OK;
#endif //DLMS_IGNORE_MALLOC
}

#ifndef DLMS_IGNORE_MALLOC
int va_copyArray(
    variantArray* target,
    variantArray* source)
{
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT* tmp, * tmp2;
    int pos;
    va_clear(target);
    for (pos = 0; pos != source->size; ++pos)
    {
        ret = va_getByIndex(source, pos, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        tmp2 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
        var_init(tmp2);
        ret = var_copy(tmp2, tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        va_push(target, tmp2);
    }
    return ret;
}

int va_addValue(
    variantArray* target,
    dlmsVARIANT* value,
    uint16_t count)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT* tmp;
    for (pos = 0; pos != count; ++pos)
    {
        tmp = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
        if (tmp == NULL)
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
            break;
        }
        if ((ret = var_init(tmp)) != 0 ||
            (ret = var_copy(tmp, value)) != 0 ||
            (ret = va_push(target, tmp)) != 0)
        {
            break;
        }
    }
    return ret;
}

#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
//Note! var_toString do not clear existing bytearray.
int var_toString(dlmsVARIANT* item, gxByteBuffer* value)
{
    int ret = DLMS_ERROR_CODE_OK;
    uint16_t pos;
    if (item->vt == DLMS_DATA_TYPE_ARRAY || item->vt == DLMS_DATA_TYPE_STRUCTURE)
    {
        dlmsVARIANT* it;
        bb_setInt8(value, item->vt == DLMS_DATA_TYPE_ARRAY ? '{' : '[');
        for (pos = 0; pos != item->Arr->size; ++pos)
        {
            if (pos != 0)
            {
                bb_setInt8(value, ',');
                bb_setInt8(value, ' ');
            }
            if ((ret = va_getByIndex(item->Arr, pos, &it)) != 0 ||
                (ret = var_toString(it, value)) != 0)
            {
                break;
            }
        }
        bb_setInt8(value, item->vt == DLMS_DATA_TYPE_ARRAY ? '}' : ']');
    }
    else
    {
        dlmsVARIANT tmp;
        var_init(&tmp);
        ret = var_copy(&tmp, item);
        if (ret == 0)
        {
            ret = var_changeType(&tmp, DLMS_DATA_TYPE_STRING);
            if (ret == 0 && tmp.strVal != NULL)
            {
                bb_set(value, tmp.strVal->data, tmp.strVal->size);
            }
        }
        var_clear(&tmp);
    }
    return ret;
}


//Note! va_toString do not clear existing bytearray.
int va_toString(
    variantArray* items,
    gxByteBuffer* ba)
{
    dlmsVARIANT* it;
    int pos, ret = DLMS_ERROR_CODE_OK;
    for (pos = 0; pos != items->size; ++pos)
    {
        if ((ret = va_getByIndex(items, pos, &it)) != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        if ((ret = var_toString(it, ba)) != 0)
        {
            return ret;
        }
    }
    return ret;
}
static int convert(dlmsVARIANT* item, DLMS_DATA_TYPE type)
{
    int ret, fromSize, toSize;
    uint16_t pos;
    char buff[250];
    dlmsVARIANT tmp, tmp3;
    dlmsVARIANT* it;
    if (item->vt == type)
    {
        return DLMS_ERROR_CODE_OK;
    }
    var_init(&tmp);
    var_init(&tmp3);
    ret = var_copy(&tmp, item);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    var_clear(item);
    if (type == DLMS_DATA_TYPE_STRING)
    {
        item->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(item->strVal);
        switch (tmp.vt)
        {
        case DLMS_DATA_TYPE_ARRAY:
        case DLMS_DATA_TYPE_STRUCTURE:
        {
            bb_setUInt8(item->strVal, '{');
            for (pos = 0; pos != tmp.Arr->size; ++pos)
            {
                ret = va_getByIndex(tmp.Arr, pos, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if (pos != 0)
                {
                    bb_setUInt8(item->strVal, ',');
                    bb_setUInt8(item->strVal, ' ');
                }
                ret = var_copy(&tmp3, it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = var_toString(&tmp3, item->strVal);
                var_clear(&tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    var_clear(&tmp);
                    return ret;
                }
            }
            bb_setUInt8(item->strVal, '}');
            bb_setUInt8(item->strVal, '\0');
            --item->strVal->size;
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_BOOLEAN:
        {
            if (tmp.boolVal == 0)
            {
                bb_addString(item->strVal, "False");
            }
            else
            {
                bb_addString(item->strVal, "True");
            }
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_INT32:
        {
            hlp_intToString(buff, 250, tmp.lVal, 1, 0);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_UINT32:
        {
            hlp_uint64ToString(buff, 250, tmp.ulVal, 0);
            if ((ret = bb_addString(item->strVal, buff)) == 0)
            {
                item->vt = type;
            }
            var_clear(&tmp);
            return ret;
        }
        case DLMS_DATA_TYPE_INT8:
        {
            hlp_intToString(buff, 250, tmp.cVal, 1, 0);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_INT16:
        {
            hlp_intToString(buff, 250, tmp.iVal, 1, 0);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_UINT8:
        {
            hlp_intToString(buff, 250, tmp.bVal, 0, 0);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_UINT16:
        {
            hlp_intToString(buff, 250, tmp.uiVal, 0, 0);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_INT64:
        {
            hlp_int64ToString(buff, 250, tmp.llVal, 1);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_UINT64:
        {
            hlp_uint64ToString(buff, 250, tmp.ullVal, 0);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        case DLMS_DATA_TYPE_ENUM:
        {
            hlp_intToString(buff, 250, tmp.bVal, 0, 0);
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#if !defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_FLOAT32)
        case DLMS_DATA_TYPE_FLOAT32:
        {
#if _MSC_VER > 1000
            sprintf_s(buff, 250, "%f", tmp.fltVal);
#else
            sprintf(buff, "%f", tmp.fltVal);
#endif
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#endif //!defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_FLOAT32)
#if !defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_FLOAT64)
        case DLMS_DATA_TYPE_FLOAT64:
        {
#if _MSC_VER > 1000
            sprintf_s(buff, 250, "%lf", tmp.dblVal);
#else
            sprintf(buff, "%lf", tmp.dblVal);
#endif
            bb_addString(item->strVal, buff);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#endif //!defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_FLOAT64)
        case DLMS_DATA_TYPE_BIT_STRING:
        {
            char* str = ba_toString(tmp.bitArr);
            bb_attachString(item->strVal, str);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#ifndef DLMS_IGNORE_STRING_CONVERTER
        case DLMS_DATA_TYPE_DATETIME:
        case DLMS_DATA_TYPE_DATE:
        case DLMS_DATA_TYPE_TIME:
        {
            time_toString(tmp.dateTime, item->strVal);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#endif //DLMS_IGNORE_STRING_CONVERTER
        case DLMS_DATA_TYPE_OCTET_STRING:
        {
#ifndef DLMS_IGNORE_STRING_CONVERTER
            if (tmp.byteArr != NULL)
            {
                char* str = bb_toHexString(tmp.byteArr);
                bb_addString(item->strVal, str);
                gxfree(str);
            }
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
#else
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_STRING_CONVERTER
        }
        case DLMS_DATA_TYPE_NONE:
        {
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        default:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        }
    }
    else if (item->vt == DLMS_DATA_TYPE_STRING)
    {
        if (type == DLMS_DATA_TYPE_BOOLEAN)
        {
            item->boolVal = strcmp((char*)tmp.strVal->data, "False") == 0 ? 0 : 1;
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_INT32)
        {
            item->lVal = hlp_stringToInt((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_UINT32)
        {
            item->ulVal = hlp_stringToInt((char*)tmp.strVal->data) & 0xFFFFFFFF;
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_INT8)
        {
            item->cVal = (char)hlp_stringToInt((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_INT16)
        {
            item->iVal = (short)hlp_stringToInt((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_UINT8)
        {
            item->bVal = (unsigned char)hlp_stringToInt((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_UINT16)
        {
            item->uiVal = (uint16_t)hlp_stringToInt((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_INT64)
        {
            item->llVal = hlp_stringToInt64((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_UINT64)
        {
            item->ullVal = (uint64_t)hlp_stringToInt64((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
        else if (type == DLMS_DATA_TYPE_ENUM)
        {
            item->bVal = (unsigned char)hlp_stringToInt((char*)tmp.strVal->data);
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#if !defined(DLMS_IGNORE_FLOAT32) && !defined(DLMS_IGNORE_STRING_CONVERTER)
        else if (type == DLMS_DATA_TYPE_FLOAT32)
        {
#if _MSC_VER > 1000
            sscanf_s((char*)tmp.strVal->data, "%f", &item->fltVal);
#else
            sscanf((char*)tmp.strVal->data, "%f", &item->fltVal);
#endif
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#endif //DLMS_IGNORE_FLOAT32
#if !defined(DLMS_IGNORE_FLOAT64) && !defined(DLMS_IGNORE_STRING_CONVERTER)
        else if (type == DLMS_DATA_TYPE_FLOAT64)
        {
#if _MSC_VER > 1000
            sscanf_s((char*)tmp.strVal->data, "%lf", &item->dblVal);
#else
            sscanf((char*)tmp.strVal->data, "%lf", &item->dblVal);
#endif
            item->vt = type;
            var_clear(&tmp);
            return DLMS_ERROR_CODE_OK;
        }
#endif //DLMS_IGNORE_FLOAT64
        else if (type == DLMS_DATA_TYPE_OCTET_STRING)
        {
            char* pBuff = (char*)tmp.strVal->data;
            item->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            BYTE_BUFFER_INIT(item->byteArr);
            bb_addHexString(item->byteArr, pBuff);
            item->vt = type;
            var_clear(&tmp);
            bb_trim(item->byteArr);
            return DLMS_ERROR_CODE_OK;
        }
        return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
    }
    fromSize = var_getSize(tmp.vt);
    toSize = var_getSize(item->vt);
    //If we try to change bigger valut to smaller check that value is not too big.
    //Example Int16 to Int8.
    if (fromSize > toSize)
    {
        unsigned char* pValue = &tmp.bVal;
        for (pos = (unsigned char)toSize; pos != (unsigned char)fromSize; ++pos)
        {
            if (pValue[pos] != 0)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
        }
    }
    if (fromSize > toSize)
    {
        memcpy(&item->bVal, &tmp.bVal, toSize);
    }
    else
    {
        memset(&item->bVal, 0, toSize);
        memcpy(&item->bVal, &tmp.bVal, fromSize);
    }
    item->vt = type;
    var_clear(&tmp);
    return DLMS_ERROR_CODE_OK;
}

int var_changeType(dlmsVARIANT* value, DLMS_DATA_TYPE newType)
{
    if (newType == value->vt)
    {
        return DLMS_ERROR_CODE_OK;
    }
    if (newType == DLMS_DATA_TYPE_NONE)
    {
        return var_clear(value);
    }
    if (value->vt == DLMS_DATA_TYPE_ARRAY && newType == DLMS_DATA_TYPE_OCTET_STRING)
    {
        return DLMS_ERROR_CODE_OK;
    }
    if (value->vt == DLMS_DATA_TYPE_STRING)
    {
        return convert(value, newType);
    }
    switch (newType)
    {
    case DLMS_DATA_TYPE_STRING:
    case DLMS_DATA_TYPE_BOOLEAN:
    case DLMS_DATA_TYPE_INT32:
    case DLMS_DATA_TYPE_UINT32:
    case DLMS_DATA_TYPE_INT8:
    case DLMS_DATA_TYPE_INT16:
    case DLMS_DATA_TYPE_UINT8:
    case DLMS_DATA_TYPE_UINT16:
    case DLMS_DATA_TYPE_INT64:
    case DLMS_DATA_TYPE_UINT64:
    case DLMS_DATA_TYPE_ENUM:
#ifndef DLMS_IGNORE_FLOAT32
    case DLMS_DATA_TYPE_FLOAT32:
#endif //DLMS_IGNORE_FLOAT32
#ifndef DLMS_IGNORE_FLOAT64
    case DLMS_DATA_TYPE_FLOAT64:
#endif //DLMS_IGNORE_FLOAT64
        return convert(value, newType);
    default:
        //Handled later.
        break;
    }
    switch (value->vt)
    {
    case DLMS_DATA_TYPE_BOOLEAN:
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    case DLMS_DATA_TYPE_BIT_STRING:
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    case DLMS_DATA_TYPE_INT32:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_UINT32:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_OCTET_STRING:
        switch (newType)
        {
        case DLMS_DATA_TYPE_DATETIME:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        case DLMS_DATA_TYPE_DATE:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        case DLMS_DATA_TYPE_TIME:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
        switch (newType)
        {
        case DLMS_DATA_TYPE_INT32:
            break;
        case DLMS_DATA_TYPE_UINT32:
            break;
        case DLMS_DATA_TYPE_STRING:
            break;
        case DLMS_DATA_TYPE_INT8:
            break;
        case DLMS_DATA_TYPE_INT16:
            break;
        case DLMS_DATA_TYPE_UINT8:
            break;
        case DLMS_DATA_TYPE_UINT16:
            break;
        case DLMS_DATA_TYPE_INT64:
            break;
        case DLMS_DATA_TYPE_UINT64:
            break;
        case DLMS_DATA_TYPE_ENUM:
            break;
        case DLMS_DATA_TYPE_FLOAT32:
            break;
        case DLMS_DATA_TYPE_FLOAT64:
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        break;
    case DLMS_DATA_TYPE_INT8:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_INT16:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_UINT8:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_UINT16:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_INT64:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_UINT64:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_ENUM:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_FLOAT32:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_FLOAT64:
        switch (newType)
        {
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            return DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    case DLMS_DATA_TYPE_DATETIME:
        switch (newType)
        {
        case DLMS_DATA_TYPE_OCTET_STRING:
            break;
        case DLMS_DATA_TYPE_STRING:
            break;
        case DLMS_DATA_TYPE_DATE:
            break;
        case DLMS_DATA_TYPE_TIME:
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        break;
    case DLMS_DATA_TYPE_DATE:
        switch (newType)
        {
        case DLMS_DATA_TYPE_OCTET_STRING:
            break;
        case DLMS_DATA_TYPE_STRING:
            break;
        case DLMS_DATA_TYPE_DATETIME:
            break;
        case DLMS_DATA_TYPE_DATE:
            break;
        case DLMS_DATA_TYPE_TIME:
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        break;
    case DLMS_DATA_TYPE_TIME:
        switch (newType)
        {
        case DLMS_DATA_TYPE_OCTET_STRING:
            break;
        case DLMS_DATA_TYPE_STRING:
            break;
        case DLMS_DATA_TYPE_DATETIME:
            break;
        case DLMS_DATA_TYPE_DATE:
            break;
        case DLMS_DATA_TYPE_TIME:
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        break;
    case DLMS_DATA_TYPE_ARRAY:
    case DLMS_DATA_TYPE_STRUCTURE:
    case DLMS_DATA_TYPE_COMPACT_ARRAY:
    default:
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MALLOC

//copy variant.
int var_copy(dlmsVARIANT* target, dlmsVARIANT* source)
{
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* it;
    dlmsVARIANT* item;
#endif //DLMS_IGNORE_MALLOC
    int ret = DLMS_ERROR_CODE_OK;
    if ((source->vt & DLMS_DATA_TYPE_BYREF) != 0)
    {
        if (source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_ARRAY) ||
            source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_STRUCTURE))
        {
            target->vt = source->vt & ~DLMS_DATA_TYPE_BYREF;
            target->pVal = source->pVal;
            return 0;
        }
        if (source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_OCTET_STRING) ||
            source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_STRING) ||
            source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_BIT_STRING) ||
            source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATETIME) ||
            source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATE) ||
            source->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_TIME))
        {
            return var_getBytes2(source, source->vt, target->byteArr);
        }
        target->vt = source->vt & ~DLMS_DATA_TYPE_BYREF;
        target->ullVal = *source->pullVal;
        return 0;
    }
    if ((target->vt & DLMS_DATA_TYPE_BYREF) != 0)
    {
        uint16_t count;
        if (source->vt == DLMS_DATA_TYPE_OCTET_STRING || source->vt == DLMS_DATA_TYPE_STRING)
        {
            if ((ret = hlp_getObjectCount2(source->byteArr, &count)) != 0)
            {
                return ret;
            }
#ifdef DLMS_IGNORE_MALLOC
            if (count > target->capacity)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            target->size = count;
#endif //DLMS_IGNORE_MALLOC
            memcpy(target->pVal, source->byteArr + source->byteArr->position, count);
        }
        else
        {
            count = (uint16_t)hlp_getDataTypeSize(source->vt);
            if (count != hlp_getDataTypeSize(target->vt ^ DLMS_DATA_TYPE_BYREF))
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            memcpy(target->pVal, &source->bVal, count);
        }
        return 0;
    }
#ifndef DLMS_IGNORE_MALLOC
    unsigned char attaced = 0;
    if ((target->vt == DLMS_DATA_TYPE_ARRAY || target->vt == DLMS_DATA_TYPE_STRUCTURE) && va_isAttached(target->Arr))
    {
        attaced = 1;
    }
    else
#endif //DLMS_IGNORE_MALLOC
    {
        ret = var_clear(target);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        target->vt = source->vt;
    }
    if (source->vt == DLMS_DATA_TYPE_STRING)
    {
#ifndef DLMS_IGNORE_MALLOC
        if (source->strVal != NULL)
        {
            target->strVal = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            BYTE_BUFFER_INIT(target->strVal);
            bb_set(target->strVal, source->strVal->data, source->strVal->size);
        }
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_MALLOC
    }
    else if (source->vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
#ifndef DLMS_IGNORE_MALLOC
        if (source->byteArr != 0)
        {
            target->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            BYTE_BUFFER_INIT(target->byteArr);
            bb_set(target->byteArr, source->byteArr->data, source->byteArr->size);
        }
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_MALLOC
    }
    else if (source->vt == DLMS_DATA_TYPE_ARRAY ||
        source->vt == DLMS_DATA_TYPE_STRUCTURE)
    {
#ifndef DLMS_IGNORE_MALLOC
        int pos;
        if (source->Arr != NULL && source->Arr->size != 0)
        {
            if (target->Arr == NULL)
            {
                target->Arr = (variantArray*)gxmalloc(sizeof(variantArray));
                va_init(target->Arr);
            }
            va_capacity(target->Arr, source->Arr->size);
            for (pos = 0; pos != source->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(source->Arr, pos, &it)) != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if (attaced)
                {
                    if ((ret = va_getByIndex(target->Arr, pos, &item)) != DLMS_ERROR_CODE_OK ||
                        (ret = var_copy(item, it)) != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                }
                else
                {
                    item = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                    ret = var_init(item);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = var_copy(item, it);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    va_push(target->Arr, item);
                }
            }
        }
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_MALLOC
    }
#ifndef DLMS_IGNORE_MALLOC
    else if (source->vt == DLMS_DATA_TYPE_DATETIME)
    {
        ret = var_setDateTime(target, source->dateTime);
    }
    else if (source->vt == DLMS_DATA_TYPE_DATE)
    {
        ret = var_setDate(target, source->dateTime);
    }
    else if (source->vt == DLMS_DATA_TYPE_TIME)
    {
        ret = var_setTime(target, source->dateTime);
    }
#endif //DLMS_IGNORE_MALLOC
    else if (source->vt == DLMS_DATA_TYPE_BIT_STRING)
    {
#ifndef DLMS_IGNORE_MALLOC
        target->bitArr = (bitArray*)gxmalloc(sizeof(bitArray));
        ba_init(target->bitArr);
        ret = ba_copy(target->bitArr, source->bitArr->data, (uint16_t)source->bitArr->size);
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = var_getSize(source->vt);
        if (ret > 0)
        {
            memcpy(&target->pVal, &source->pVal, ret);
        }
        ret = 0;
    }
    return ret;
}

#ifndef DLMS_IGNORE_MALLOC
int var_setDateTime(dlmsVARIANT* target, gxtime* value)
{
    int ret;
    if ((ret = var_clear(target)) == DLMS_ERROR_CODE_OK)
    {
        target->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
        time_copy(target->dateTime, value);
        target->vt = DLMS_DATA_TYPE_DATETIME;
    }
    return ret;
}

int var_setDate(dlmsVARIANT* target, gxtime* value)
{
    int ret;
    if ((ret = var_clear(target)) == DLMS_ERROR_CODE_OK)
    {
        target->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
        time_copy(target->dateTime, value);
        target->vt = DLMS_DATA_TYPE_DATE;
    }
    return ret;
}

int var_setTime(dlmsVARIANT* target, gxtime* value)
{
    int ret;
    if ((ret = var_clear(target)) == DLMS_ERROR_CODE_OK)
    {
        target->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
        time_copy(target->dateTime, value);
        target->vt = DLMS_DATA_TYPE_TIME;
    }
    return ret;
}

int var_setDateTimeAsOctetString(
    dlmsVARIANT* target,
    gxtime* value)
{
    int ret;
    if ((ret = var_clear(target)) == DLMS_ERROR_CODE_OK)
    {
        target->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(target->byteArr);
        bb_capacity(target->byteArr, 12);
        if ((ret = var_getDateTime2(value, target->byteArr)) == 0)
        {
            target->vt = DLMS_DATA_TYPE_OCTET_STRING;
        }
    }
    return ret;
}

int var_setDateAsOctetString(
    dlmsVARIANT* target,
    gxtime* value)
{
    int ret;
    if ((ret = var_clear(target)) == DLMS_ERROR_CODE_OK)
    {
        target->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(target->byteArr);
        bb_capacity(target->byteArr, 5);
        if ((ret = var_getDate(value, target->byteArr)) == 0)
        {
            target->vt = DLMS_DATA_TYPE_OCTET_STRING;
        }
    }
    return ret;
}

int var_setTimeAsOctetString(
    dlmsVARIANT* target,
    gxtime* value)
{
    int ret;
    if ((ret = var_clear(target)) == DLMS_ERROR_CODE_OK)
    {
        target->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        BYTE_BUFFER_INIT(target->byteArr);
        bb_capacity(target->byteArr, 4);
        if ((ret = var_getTime(value, target->byteArr)) == 0)
        {
            target->vt = DLMS_DATA_TYPE_OCTET_STRING;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_MALLOC
int var_setBoolean(dlmsVARIANT* target, char value)
{
    int ret;
    if ((ret = var_clear(target)) == DLMS_ERROR_CODE_OK)
    {
        target->boolVal = value;
        target->vt = DLMS_DATA_TYPE_BOOLEAN;
    }
    return ret;
}

#ifdef DLMS_IGNORE_MALLOC
//Attach value by ref.
int var_byRef(dlmsVARIANT* target, DLMS_DATA_TYPE type, void* value)
{
    int ret;
    ret = var_clear(target);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    target->pVal = value;
    target->vt = type | DLMS_DATA_TYPE_BYREF;
    return ret;
}
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
void var_attach(
    dlmsVARIANT* target,
    gxByteBuffer* source)
{
    target->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
    target->byteArr->data = source->data;
    target->byteArr->capacity = source->capacity;
    target->byteArr->size = source->size;
    target->byteArr->position = source->position;
    source->data = 0;
    source->size = source->position = source->capacity = 0;
    target->vt = DLMS_DATA_TYPE_OCTET_STRING;
}
#endif //DLMS_IGNORE_MALLOC

int var_getDateTime(dlmsVARIANT* target, gxtime* value)
{
    if (target->vt == DLMS_DATA_TYPE_NONE)
    {
        time_clear(value);
    }
#ifndef DLMS_IGNORE_MALLOC
    else if (target->vt == DLMS_DATA_TYPE_DATETIME ||
        target->vt == DLMS_DATA_TYPE_DATE ||
        target->vt == DLMS_DATA_TYPE_TIME)
    {
        value->extraInfo = target->dateTime->extraInfo;
        value->skip = target->dateTime->skip;
        value->status = target->dateTime->status;
        value->value = target->dateTime->value;
    }
#endif //DLMS_IGNORE_MALLOC
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

double var_toDouble(dlmsVARIANT* target)
{
    switch (target->vt)
    {
    case DLMS_DATA_TYPE_NONE:
    {
        return 0;
    }
    case DLMS_DATA_TYPE_BOOLEAN:
    {
        return target->boolVal ? 1 : 0;
    }
    case DLMS_DATA_TYPE_INT32:
    {
        return target->lVal;
    }
    case DLMS_DATA_TYPE_UINT32:
    {
        return target->ulVal;
    }
    case DLMS_DATA_TYPE_STRING_UTF8:
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        return 0;
    }
    case DLMS_DATA_TYPE_STRING:
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        return 0;
    }
    case DLMS_DATA_TYPE_INT8:
    {
        return target->cVal;
    }
    case DLMS_DATA_TYPE_INT16:
    {
        return target->iVal;
    }
    case DLMS_DATA_TYPE_UINT8:
    {
        return target->bVal;
    }
    case DLMS_DATA_TYPE_UINT16:
    {
        return target->uiVal;
    }
    case DLMS_DATA_TYPE_INT64:
    {
        return (double)target->llVal;
    }
    case DLMS_DATA_TYPE_UINT64:
    {
        return (double)target->ullVal;
    }
    case DLMS_DATA_TYPE_ENUM:
    {
        return target->bVal;
    }
#ifndef DLMS_IGNORE_FLOAT32
    case DLMS_DATA_TYPE_FLOAT32:
    {
        return target->fltVal;
    }
#endif //DLMS_IGNORE_FLOAT32
#ifndef DLMS_IGNORE_FLOAT64
    case DLMS_DATA_TYPE_FLOAT64:
    {
        return target->dblVal;
    }
#endif //DLMS_IGNORE_FLOAT64
    default:
        break;
    }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    assert(0);
#endif
    return 0;
}

#if !defined(DLMS_IGNORE_STRING_CONVERTER) && !defined(DLMS_IGNORE_MALLOC)
//Print content of the variant to cout.
int var_print(const char* format, dlmsVARIANT* target)
{
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT tmp;
    var_init(&tmp);
    ret = var_copy(&tmp, target);
    if (ret == 0)
    {
        ret = var_changeType(&tmp, DLMS_DATA_TYPE_STRING);
        if (ret == 0 && tmp.strVal != NULL)
        {
            if (format == NULL)
            {
                format = "%s\r\n";
            }
            printf(format, tmp.strVal->data);
        }
    }
    var_clear(&tmp);
    return ret;
}

int va_print(
    variantArray* items)
{
    dlmsVARIANT* it;
    int pos, ret = DLMS_ERROR_CODE_OK;
    const char* format = "%s,\r\n";
    for (pos = 0; pos != items->size; ++pos)
    {
        if ((ret = va_getByIndex(items, pos, &it)) != 0)
        {
            return ret;
        }

        if (pos + 1 == items->size)
        {
            format = NULL;
        }
        if ((ret = var_print(format, it)) != 0)
        {
            return ret;
        }
    }
    return ret;
}

#endif //!defined(DLMS_IGNORE_STRING_CONVERTER) && defined(DLMS_IGNORE_MALLOC)
