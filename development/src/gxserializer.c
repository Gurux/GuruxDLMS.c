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
#ifndef DLMS_IGNORE_SERIALIZER

#include "../include/gxmem.h"
#include "../include/gxserializer.h"
#include "../include/helpers.h"
#include "../include/objectarray.h"
#include "../include/cosem.h"
#include "../include/dlms.h"
#include "../include/bytebuffer.h"
#ifdef DLMS_DEBUG
#include "../include/serverevents.h"
#endif //DLMS_DEBUG

//Serialization version is increased every time when structure of serialized data is changed.
#define SERIALIZATION_VERSION 1

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif
#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif

unsigned char ser_isEof(gxSerializerSettings* serializeSettings)
{
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    return feof(serializeSettings->stream);
#else
    return !(serializeSettings->position < SERIALIZER_SIZE());
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
}

int ser_loadUInt8(gxSerializerSettings* serializeSettings, unsigned char* value)
{
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    return fread(value, sizeof(unsigned char), 1, serializeSettings->stream) != 1;
#else
    int ret = SERIALIZER_LOAD((uint16_t)serializeSettings->position, sizeof(unsigned char), (void*)value);
    if (ret == 0)
    {
        ++serializeSettings->position;
    }
    return ret;
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
}

int ser_loadUInt16(gxSerializerSettings* serializeSettings, uint16_t* value)
{
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    return fread(value, sizeof(uint16_t), 1, serializeSettings->stream) != 1;
#else
    int ret = SERIALIZER_LOAD(serializeSettings->position, sizeof(uint16_t), (void*) value);
    if (ret == 0)
    {
        serializeSettings->position += sizeof(uint16_t);
    }
    return ret;
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
}

int ser_loadUInt32(gxSerializerSettings* serializeSettings, void* value)
{
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    return fread(value, sizeof(uint32_t), 1, serializeSettings->stream) != 1;
#else
    int ret = SERIALIZER_LOAD(serializeSettings->position, sizeof(uint32_t), value);
    if (ret == 0)
    {
        serializeSettings->position += sizeof(uint32_t);
    }
    return ret;
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
}

int ser_loadUInt64(gxSerializerSettings* serializeSettings, uint64_t* value)
{
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    return fread(value, sizeof(uint64_t), 1, serializeSettings->stream) != 1;
#else
    int ret = SERIALIZER_LOAD(serializeSettings->position, sizeof(uint64_t), (void*) value);
    if (ret == 0)
    {
        serializeSettings->position += sizeof(uint64_t);
    }
    return ret;
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
}

int ser_loadInt8(gxSerializerSettings* serializeSettings, signed char* value)
{
    return ser_loadUInt8(serializeSettings, (unsigned char*)value);
}

int ser_loadInt16(gxSerializerSettings* serializeSettings, int16_t* value)
{
    return ser_loadUInt16(serializeSettings, (uint16_t*)value);
}

int ser_loadInt32(gxSerializerSettings* serializeSettings, int32_t* value)
{
    return ser_loadUInt32(serializeSettings, (uint32_t*)value);
}

int ser_loadInt64(gxSerializerSettings* serializeSettings, int64_t* value)
{
    return ser_loadUInt64(serializeSettings, (uint64_t*)value);
}

int ser_seek(gxSerializerSettings* serializeSettings, int count)
{
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    if (fseek(serializeSettings->stream, count, SEEK_CUR) != 0)
    {
        return DLMS_ERROR_CODE_HARDWARE_FAULT;
    }
#else
    serializeSettings->position += count;
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    return 0;
}

#ifdef DLMS_IGNORE_MALLOC
//Whether to save the item.
unsigned char ser_serialize(gxSerializerSettings* serializeSettings)
{
    if (serializeSettings->savedObject != NULL && serializeSettings->currentIndex != 0)
    {
        return serializeSettings->savedObject == serializeSettings->currentObject &&
            (serializeSettings->savedAttributes & 1 << (serializeSettings->currentIndex - 1)) != 0;
    }
    return 1;
}
#endif //DLMS_IGNORE_MALLOC

int ser_save(gxSerializerSettings* serializeSettings, const void* item, uint16_t count)
{
#ifdef DLMS_IGNORE_MALLOC
    if (!ser_serialize(serializeSettings))
    {
        return ser_seek(serializeSettings, count);
    }
#endif //DLMS_IGNORE_MALLOC
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    if (fwrite(item, count, 1, serializeSettings->stream) != 1)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return 0;
#else
    int ret = SERIALIZER_SAVE(serializeSettings->position, count, item);
    //Update changed positions.
    if (serializeSettings->position < serializeSettings->updateStart)
    {
        serializeSettings->updateStart = serializeSettings->position;
    }
    serializeSettings->position += count;
    if (serializeSettings->updateEnd < serializeSettings->position)
    {
        serializeSettings->updateEnd = serializeSettings->position;
    }
    return ret;
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
}

int ser_saveUInt8(
    gxSerializerSettings* serializeSettings,
    unsigned char item)
{
    return ser_save(serializeSettings, &item, 1);
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int ser_set(
    gxSerializerSettings* serializeSettings,
    const unsigned char* pSource,
    uint32_t count
#ifdef DLMS_IGNORE_MALLOC
    , uint32_t capacity
#endif //DLMS_IGNORE_MALLOC
)
#else
int ser_set(
    gxSerializerSettings* serializeSettings,
    const unsigned char* pSource,
    uint16_t count
#ifdef DLMS_IGNORE_MALLOC
    , uint16_t capacity
#endif //DLMS_IGNORE_MALLOC
)
#endif
{
    int ret;
#ifdef DLMS_IGNORE_MALLOC
    if (capacity < count)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (!ser_serialize(serializeSettings))
    {
        return ser_seek(serializeSettings, capacity);
    }
#endif //DLMS_IGNORE_MALLOC
    ret = 0;
    uint16_t pos;
    for (pos = 0; pos != count; ++pos)
    {
        if ((ret = ser_saveUInt8(serializeSettings, pSource[pos])) != 0)
        {
            break;
        }
    }
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
#ifdef DLMS_IGNORE_MALLOC
    serializeSettings->position += capacity - count;
    //Update changed positions.
    if (serializeSettings->position < serializeSettings->updateStart)
    {
        serializeSettings->updateStart = serializeSettings->position;
    }
    if (serializeSettings->updateEnd < serializeSettings->position)
    {
        serializeSettings->updateEnd = serializeSettings->position;
    }
#endif //DLMS_IGNORE_MALLOC
#endif //!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    return ret;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int ser_get(
    gxSerializerSettings* serializeSettings,
    unsigned char* value,
    uint32_t count
#ifdef DLMS_IGNORE_MALLOC
    , uint32_t capacity
#endif //DLMS_IGNORE_MALLOC
)
#else
int ser_get(
    gxSerializerSettings* serializeSettings,
    unsigned char* value,
    uint16_t count
#ifdef DLMS_IGNORE_MALLOC
    , uint16_t capacity
#endif //DLMS_IGNORE_MALLOC
)
#endif
{
    int ret = 0;
#ifdef DLMS_IGNORE_MALLOC
    if (capacity < count)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#endif //DLMS_IGNORE_MALLOC
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    if (fread(value, sizeof(unsigned char), count, serializeSettings->stream) != count)
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#else
    if ((ret = SERIALIZER_LOAD((uint16_t)serializeSettings->position, count, value)) == 0)
    {
        serializeSettings->position += count;
    }
#endif//!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
#ifdef DLMS_IGNORE_MALLOC
    if (ret == 0)
    {
        serializeSettings->position += capacity - count;
    }
#endif //#ifdef DLMS_IGNORE_MALLOC
#endif //!(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    return ret;
}


#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int ser_set2(
    gxByteBuffer* arr,
    gxSerializerSettings* serializeSettings,
    uint32_t count
#ifdef DLMS_IGNORE_MALLOC
    , uint16_t capacity
#endif //DLMS_IGNORE_MALLOC
)
#else
int ser_set2(
    gxByteBuffer* arr,
    gxSerializerSettings* serializeSettings,
    uint16_t count
#ifdef DLMS_IGNORE_MALLOC
    , uint16_t capacity
#endif //DLMS_IGNORE_MALLOC
)
#endif
{
    int ret = 0;
#ifdef DLMS_IGNORE_MALLOC
    if (capacity != 0)
#else
    if (count != 0)
#endif //DLMS_IGNORE_MALLOC
    {
        if (count != 0)
        {
            ret = bb_allocate(arr, arr->size, count);
        }
        if (ret == 0)
        {
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
            int pos;
            for (pos = 0; pos != count; ++pos)
            {
                if (fread(&arr->data[pos + arr->size], sizeof(unsigned char), 1, serializeSettings->stream) != 1)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
                }
            }
#else
            if ((ret = SERIALIZER_LOAD(serializeSettings->position, count, arr->data + arr->size)) == 0)
            {
                serializeSettings->position += count;
            }
#endif //(!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
            if (ret == 0)
            {
                arr->size += count;
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
#ifdef DLMS_IGNORE_MALLOC
                serializeSettings->position += capacity - count;
#endif //DLMS_IGNORE_MALLOC
#endif //!(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
            }
        }
    }
    return ret;
}

int ser_loadObjectCount(gxSerializerSettings* serializeSettings, uint16_t* count)
{
    int ret;
#ifdef DLMS_IGNORE_MALLOC
    uint16_t tmp;
    ret = ser_loadUInt16(serializeSettings, &tmp);
    *count = tmp;
#else
    unsigned char ch;
    ret = ser_loadUInt8(serializeSettings, &ch);
    if (ret != 0)
    {
        return ret;
    }
    if (ch > 0x80)
    {
        if (ch == 0x81)
        {
            ret = ser_loadUInt8(serializeSettings, &ch);
            *count = ch;
        }
        else if (ch == 0x82)
        {
            uint16_t tmp;
            ret = ser_loadUInt16(serializeSettings, &tmp);
            *count = tmp;
        }
        else if (ch == 0x84)
        {
            uint32_t value;
            ret = ser_loadUInt32(serializeSettings, &value);
            *count = (uint16_t)value;
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else
    {
        *count = ch;
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int ser_checkArray2(gxSerializerSettings* serializeSettings, uint16_t* count, unsigned char checkDataType)
{
    int ret;
    unsigned char ch;
    uint16_t cnt;
    if (checkDataType)
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) != 0)
        {
            return ret;
        }
        if (ch != DLMS_DATA_TYPE_ARRAY)
        {
            return DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
    }
    //Get array count.
    if ((ret = ser_loadObjectCount(serializeSettings, &cnt)) != 0)
    {
        return ret;
    }
#ifndef DLMS_COSEM_EXACT_DATA_TYPES
    if (*count < cnt)
    {
        return DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
    }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
    * count = cnt;
    return 0;
}


int ser_loadDateTime(gxtime* value, gxSerializerSettings* serializeSettings, DLMS_DATA_TYPE type)
{
    int ret = 0;
    time_clear(value);
    uint16_t size;
    if (type == DLMS_DATA_TYPE_DATETIME)
    {
        size = 12;
    }
    else if (type == DLMS_DATA_TYPE_DATE)
    {
        size = 5;
    }
    else if (type == DLMS_DATA_TYPE_TIME)
    {
        size = 4;
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (ret == 0)
    {
        dlmsVARIANT tmp;
        GX_DATETIME(tmp) = value;
        unsigned char buff[12];
        gxByteBuffer bb;
        bb_attach(&bb, buff, size, sizeof(buff));
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        int pos;
        for (pos = 0; pos != size; ++pos)
        {
            if (fread(&buff[pos], sizeof(unsigned char), 1, serializeSettings->stream) != 1)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                break;
            }
        }
#else
        if ((ret = SERIALIZER_LOAD(serializeSettings->position, size, buff)) == 0)
        {
            serializeSettings->position += size;
        }
#endif //!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        ret = dlms_changeType(&bb, type, &tmp);
    }
    return ret;
}

int ser_loadVariantArray(gxSerializerSettings* serializeSettings, variantArray* arr, uint16_t* count)
{
    int ret = ser_loadObjectCount(serializeSettings, count);
    if (ret == 0)
    {
        va_clear(arr);
#ifdef DLMS_IGNORE_MALLOC
        arr->size = *count;
        ret = ser_loadObjectCount(serializeSettings, count);
#else
#endif //DLMS_IGNORE_MALLOC
        va_capacity(arr, *count);
    }
    return ret;
}

#if defined(DLMS_IGNORE_MALLOC)
int ser_loadBitStringtoVariant(gxSerializerSettings* serializeSettings, dlmsVARIANT* value)
{
    int ret;
    uint16_t count;
    if ((ret = ser_loadObjectCount(serializeSettings, &count)) != 0)
    {
        return ret;
    }
    uint16_t capacity;
    if ((ret = ser_loadObjectCount(serializeSettings, &capacity)) != 0)
    {
        return ret;
    }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    if (value->capacity != capacity)
    {
        printf("Warning! Serialized capacity is different than allocated capacity.\n");
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    value->size = count;
    return ser_get(serializeSettings, value->pVal, ba_getByteCount(count), ba_getByteCount(capacity));
}
#endif //DLMS_IGNORE_MALLOC


int ser_loadOctetString(gxSerializerSettings* serializeSettings, gxByteBuffer* value)
{
    int ret;
    uint16_t count;
    if ((ret = ser_loadObjectCount(serializeSettings, &count)) != 0)
    {
        return ret;
    }
#ifdef DLMS_IGNORE_MALLOC
    uint16_t capacity;
    if ((ret = ser_loadObjectCount(serializeSettings, &capacity)) != 0)
    {
        return ret;
    }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    if (bb_getCapacity(value) != capacity)
    {
        printf("Warning! Serialized capacity is different than allocated capacity.\n");
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    if ((ret = bb_clear(value)) == 0 &&
        (ret = ser_set2(value, serializeSettings, count, capacity)) == 0)
    {
    }
#else
    if ((ret = bb_clear(value)) == 0 &&
        (ret = ser_set2(value, serializeSettings, count)) == 0)
    {
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int ser_loadOctetString2(gxSerializerSettings* serializeSettings, char** value)
{
    int ret;
    uint16_t count;
    if ((ret = ser_loadObjectCount(serializeSettings, &count)) != 0)
    {
        return ret;
    }
#ifdef DLMS_IGNORE_MALLOC
    uint16_t capacity;
    if ((ret = ser_loadObjectCount(serializeSettings, &capacity)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
#if !defined(DLMS_USE_CUSTOM_MALLOC) && !defined(DLMS_IGNORE_MALLOC)
    if (*value != NULL)
    {
        gxfree(*value);
    }
    *value = (char*)gxmalloc(count + 1);
#endif //#if !defined(DLMS_USE_CUSTOM_MALLOC) && !defined(DLMS_IGNORE_MALLOC)
    if ((ret = ser_get(serializeSettings, (unsigned char*)*value, count
#ifdef DLMS_IGNORE_MALLOC
        , capacity
#endif//DLMS_IGNORE_MALLOC
    )) == 0)
    {
        (*value)[count] = 0;
    }
    return ret;
}

int ser_loadOctetString3(
    gxSerializerSettings* serializeSettings,
    unsigned char* value,
    uint16_t* count)
{
    int ret;
    if ((ret = ser_loadObjectCount(serializeSettings, count)) != 0)
    {
        return ret;
    }
#ifdef DLMS_IGNORE_MALLOC
    uint16_t capacity;
    if ((ret = ser_loadObjectCount(serializeSettings, &capacity)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
    ret = ser_get(serializeSettings, value, *count
#ifdef DLMS_IGNORE_MALLOC
        , capacity
#endif //DLMS_IGNORE_MALLOC
    );
#ifdef DLMS_IGNORE_MALLOC
    if (*count < capacity)
    {
        value[*count] = 0;
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int ser_getOctetString2(gxSerializerSettings* serializeSettings, unsigned char* value, uint16_t* size)
{
    uint16_t tmp;
    int ret;
    if ((ret = ser_loadOctetString3(serializeSettings, value, &tmp)) == 0)
    {
        if (size != NULL)
        {
            *size = tmp;
        }
    }
    return ret;
}

int ser_getOctetString(gxSerializerSettings* serializeSettings, gxByteBuffer* value)
{
    int ret;
    uint16_t count;
    if ((ret = ser_loadObjectCount(serializeSettings, &count)) != 0)
    {
        return ret;
    }
    bb_clear(value);
#ifdef DLMS_IGNORE_MALLOC
    uint16_t capacity;
    if ((ret = ser_loadObjectCount(serializeSettings, &capacity)) != 0)
    {
        return ret;
    }
    bb_capacity(value, capacity);
#else
    if ((ret = bb_capacity(value, count)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
    value->size = count;
    return ret = ser_get(serializeSettings, value->data, count
#ifdef DLMS_IGNORE_MALLOC
        , capacity
#endif //DLMS_IGNORE_MALLOC
    );
}

#ifndef DLMS_IGNORE_FLOAT32
int ser_loadFloat(
    gxSerializerSettings* serializeSettings,
    float* value)
{
    typedef union
    {
        float value;
        unsigned char b[sizeof(float)];
    } HELPER;
    HELPER tmp;
    int ret;
    if ((ret = ser_loadUInt8(serializeSettings, &tmp.b[3])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[2])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[1])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[0])) == 0)
    {
        *value = tmp.value;
    }
    return ret;
}
#endif //DLMS_IGNORE_FLOAT32

#ifndef DLMS_IGNORE_FLOAT64
int ser_loadDouble(
    gxSerializerSettings* serializeSettings,
    double* value)
{
    typedef union
    {
        double value;
        unsigned char b[sizeof(double)];
    } HELPER;
    HELPER tmp;
    int ret;
    if ((ret = ser_loadUInt8(serializeSettings, &tmp.b[7])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[6])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[5])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[4])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[3])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[2])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[1])) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &tmp.b[0])) == 0)
    {
        *value = tmp.value;
    }
    return ret;
}
#endif //DLMS_IGNORE_FLOAT64


int ser_loadBitString(gxSerializerSettings* serializeSettings, bitArray* value)
{
    int ret;
    uint16_t count;
    if ((ret = ser_loadObjectCount(serializeSettings, &count)) != 0)
    {
        return ret;
    }
#ifdef DLMS_IGNORE_MALLOC
    uint16_t capacity;
    if ((ret = ser_loadObjectCount(serializeSettings, &capacity)) != 0)
    {
        return ret;
    }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    uint16_t size = ba_getByteCount(ba_getCapacity(value));
    if (size != capacity)
    {
        printf("Warning! Serialized capacity is different than allocated capacity.\n");
    }
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#endif //DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_MALLOC
    if ((ret = ba_capacity(value, count)) != 0)
    {
        return ret;
    }
#endif // DLMS_IGNORE_MALLOC
    value->size = count;
    ret = ser_get(serializeSettings, value->data, ba_getByteCount(count)
#ifdef DLMS_IGNORE_MALLOC
        , capacity
#endif //DLMS_IGNORE_MALLOC
    );
    return ret;
}

// Get item from the buffer if DLMS_IGNORE_MALLOC is defined.
// Otherwice create the variant object dynamically.
int ser_getVariantArrayItem(variantArray* arr, uint16_t index, dlmsVARIANT** value)
{
#ifdef DLMS_IGNORE_MALLOC
    int ret = va_getByIndex(arr, index, value);
    if (ret == 0)
    {
        var_clear(*value);
    }
    return ret;
#else
    * value = gxmalloc(sizeof(dlmsVARIANT));
    if (*value == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    var_init(*value);
    return va_push(arr, *value);
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int ser_loadVariant(dlmsVARIANT* data,
    gxSerializerSettings* serializeSettings)
{
    int ret;
    unsigned char ch;
    var_clear(data);
    if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
    {
        //Update data type if not reference.
        if ((data->vt & DLMS_DATA_TYPE_BYREF) == 0)
        {
            data->vt = ch;
        }
        if (ch == DLMS_DATA_TYPE_NONE)
        {
            //Do nothing.
            return DLMS_ERROR_CODE_OK;
        }
        switch (data->vt & ~DLMS_DATA_TYPE_BYREF)
        {
        case DLMS_DATA_TYPE_ARRAY:
        case DLMS_DATA_TYPE_STRUCTURE:
        {
            uint16_t pos, count;
            dlmsVARIANT* tmp;
#ifndef DLMS_IGNORE_MALLOC
            data->Arr = gxmalloc(sizeof(variantArray));
            va_init(data->Arr);
#else
            dlmsVARIANT tmp2;
            var_init(&tmp2);
            va_clear(data->Arr);
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_loadVariantArray(serializeSettings, data->Arr, &count)) == 0)
            {
                for (pos = 0; pos != count; ++pos)
                {
                    if (pos < data->Arr->size)
                    {
                        if ((ret = ser_getVariantArrayItem(data->Arr, pos, &tmp)) != 0)
                        {
                            break;
                        }
                    }
#ifdef DLMS_IGNORE_MALLOC
                    else
                    {
                        var_clear(&tmp2);
                        tmp = &tmp2;
                    }
#endif //DLMS_IGNORE_MALLOC
                    if ((ret = ser_loadVariant(tmp, serializeSettings)) != 0)
                    {
                        break;
                    }
                }
#ifdef DLMS_IGNORE_MALLOC
                //Return original size.
                data->Arr->size = count;
#endif //DLMS_IGNORE_MALLOC
            }
            break;
        }
        case DLMS_DATA_TYPE_BOOLEAN:
        {
            ret = ser_loadUInt8(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->boolVal : data->pboolVal);
            break;
        }
        case DLMS_DATA_TYPE_BIT_STRING:
#if !defined(DLMS_IGNORE_MALLOC)
            data->bitArr = (bitArray*)gxmalloc(sizeof(bitArray));
            ba_init(data->bitArr);
            ret = ser_loadBitString(serializeSettings, data->bitArr);
#else
            ret = ser_loadBitStringtoVariant(serializeSettings, data);
#endif //!defined(DLMS_IGNORE_MALLOC)
            break;
        case DLMS_DATA_TYPE_INT32:
            ret = ser_loadInt32(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->lVal : data->plVal);
            break;
        case DLMS_DATA_TYPE_UINT32:
            ret = ser_loadUInt32(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->ulVal : data->pulVal);
            break;
        case DLMS_DATA_TYPE_STRING:
#if !defined(DLMS_IGNORE_MALLOC)
            data->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            bb_init(data->byteArr);
            ret = ser_getOctetString(serializeSettings, data->byteArr);
#else
            ret = ser_loadOctetString3(serializeSettings, (unsigned char*)data->pVal, &data->size);
#endif //!defined(DLMS_IGNORE_MALLOC)
            break;
        case DLMS_DATA_TYPE_STRING_UTF8:
#if !defined(DLMS_IGNORE_MALLOC)
            data->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            bb_init(data->byteArr);
            ret = ser_getOctetString(serializeSettings, data->byteArr);
#else
            ret = ser_loadOctetString3(serializeSettings, data->pVal, &data->size);
#endif //!defined(DLMS_IGNORE_MALLOC)
            break;
        case DLMS_DATA_TYPE_OCTET_STRING:
#if !defined(DLMS_IGNORE_MALLOC)
            data->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            bb_init(data->byteArr);
            ret = ser_getOctetString(serializeSettings, data->byteArr);
#else
            ret = ser_loadOctetString3(serializeSettings, data->pVal, &data->size);
#endif // DLMS_IGNORE_MALLOC
            break;
        case DLMS_DATA_TYPE_BINARY_CODED_DESIMAL:
            ret = ser_loadUInt8(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->bVal : data->pbVal);
            break;
        case DLMS_DATA_TYPE_INT8:
            ret = ser_loadInt8(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->cVal : data->pcVal);
            break;
        case DLMS_DATA_TYPE_INT16:
            ret = ser_loadInt16(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->iVal : data->piVal);
            break;
        case DLMS_DATA_TYPE_UINT8:
            ret = ser_loadUInt8(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->bVal : data->pbVal);
            break;
        case DLMS_DATA_TYPE_UINT16:
            ret = ser_loadUInt16(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->uiVal : data->puiVal);
            break;
        case DLMS_DATA_TYPE_COMPACT_ARRAY:
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        case DLMS_DATA_TYPE_INT64:
            ret = ser_loadInt64(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->llVal : data->pllVal);
            break;
        case DLMS_DATA_TYPE_UINT64:
            ret = ser_loadUInt64(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->ullVal : data->pullVal);
            break;
        case DLMS_DATA_TYPE_ENUM:
            ret = ser_loadUInt8(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->bVal : data->pbVal);
            break;
#ifndef DLMS_IGNORE_FLOAT32
        case DLMS_DATA_TYPE_FLOAT32:
            ret = ser_loadFloat(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->fltVal : data->pfltVal);
            break;
#endif //DLMS_IGNORE_FLOAT32
#ifndef DLMS_IGNORE_FLOAT64
        case DLMS_DATA_TYPE_FLOAT64:
            ret = ser_loadDouble(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) == 0 ? &data->dblVal : data->pdblVal);
            break;
#endif //DLMS_IGNORE_FLOAT64
        case DLMS_DATA_TYPE_DATETIME:
#if defined(DLMS_IGNORE_MALLOC)
            ret = ser_loadDateTime((gxtime*)data->pVal, serializeSettings, DLMS_DATA_TYPE_DATETIME);
#else
            data->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
            ret = ser_loadDateTime(data->dateTime, serializeSettings, DLMS_DATA_TYPE_DATETIME);
#endif //DLMS_IGNORE_MALLOC
            break;
        case DLMS_DATA_TYPE_DATE:
#if defined(DLMS_IGNORE_MALLOC)
            ret = ser_loadDateTime((gxtime*)data->pVal, serializeSettings, DLMS_DATA_TYPE_DATE);
#else
            data->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
            ret = ser_loadDateTime(data->dateTime, serializeSettings, DLMS_DATA_TYPE_DATE);
#endif //DLMS_IGNORE_MALLOC
            break;
        case DLMS_DATA_TYPE_TIME:
#if defined(DLMS_IGNORE_MALLOC)
            ret = ser_loadDateTime((gxtime*)data->pVal, serializeSettings, DLMS_DATA_TYPE_TIME);
#else
            data->dateTime = (gxtime*)gxmalloc(sizeof(gxtime));
            ret = ser_loadDateTime(data->dateTime, serializeSettings, DLMS_DATA_TYPE_TIME);
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
    }
    return ret;
}

int ser_saveUInt16(
    gxSerializerSettings* serializeSettings,
    uint16_t item)
{
#ifdef DLMS_IGNORE_MALLOC
    if (!ser_serialize(serializeSettings))
    {
        return ser_seek(serializeSettings, sizeof(uint16_t));
    }
#endif //DLMS_IGNORE_MALLOC
    return ser_save(serializeSettings, &item, sizeof(uint16_t));
}

int ser_saveUInt32(
    gxSerializerSettings* serializeSettings,
    uint32_t item)
{
#ifdef DLMS_IGNORE_MALLOC
    if (!ser_serialize(serializeSettings))
    {
        return ser_seek(serializeSettings, sizeof(uint32_t));
    }
#endif //DLMS_IGNORE_MALLOC
    return ser_save(serializeSettings, &item, sizeof(uint32_t));
}

int ser_saveUInt64(
    gxSerializerSettings* serializeSettings,
    uint64_t item)
{
#ifdef DLMS_IGNORE_MALLOC
    if (!ser_serialize(serializeSettings))
    {
        return ser_seek(serializeSettings, sizeof(uint64_t));
    }
#endif //DLMS_IGNORE_MALLOC
    return ser_save(serializeSettings, &item, sizeof(uint64_t));
}

int ser_saveInt8(
    gxSerializerSettings* serializeSettings,
    signed char item)
{
    return ser_saveUInt8(serializeSettings, (unsigned char)item);
}

int ser_saveInt16(
    gxSerializerSettings* serializeSettings,
    int16_t item)
{
    return ser_saveUInt16(serializeSettings, (uint16_t)item);
}

int ser_saveInt32(
    gxSerializerSettings* serializeSettings,
    int32_t item)
{
    return ser_saveUInt32(serializeSettings, (uint32_t)item);
}

int ser_saveInt64(
    gxSerializerSettings* serializeSettings,
    int64_t item)
{
    return ser_saveUInt64(serializeSettings, (uint64_t)item);
}


int ser_saveDateTime2(
    gxtime* dateTime,
    gxSerializerSettings* serializeSettings)
{
    int ret;
    unsigned char buff[12];
    gxByteBuffer bb;
    BB_ATTACH(bb, buff, 0);
    if ((ret = var_getDateTime2(dateTime, &bb)) == 0 &&
        (ret = ser_set(serializeSettings, bb.data, bb.size
#ifdef DLMS_IGNORE_MALLOC
            , bb_getCapacity(&bb)
#endif //DLMS_IGNORE_MALLOC
        )) == 0)
    {

    }
    return ret;
}

int ser_saveDate(
    gxtime* dateTime,
    gxSerializerSettings* serializeSettings)
{
    int ret;
    unsigned char buff[5];
    gxByteBuffer bb;
    BB_ATTACH(bb, buff, 0);
    if ((ret = var_getDate(dateTime, &bb)) == 0 &&
        (ret = ser_set(serializeSettings, bb.data, bb.size
#ifdef DLMS_IGNORE_MALLOC
            , bb_getCapacity(&bb)
#endif //DLMS_IGNORE_MALLOC
        )) == 0)
    {

    }
    return ret;
}

int ser_saveTime(
    gxtime* dateTime,
    gxSerializerSettings* serializeSettings)
{
    int ret;
    unsigned char buff[4];
    gxByteBuffer bb;
    BB_ATTACH(bb, buff, 0);
    if ((ret = var_getTime(dateTime, &bb)) == 0 &&
        (ret = ser_set(serializeSettings, bb.data, bb.size
#ifdef DLMS_IGNORE_MALLOC
            , bb_getCapacity(&bb)
#endif //DLMS_IGNORE_MALLOC
        )) == 0)
    {

    }
    return ret;
}


#ifndef DLMS_IGNORE_FLOAT32
int ser_saveFloat(
    gxSerializerSettings* serializeSettings,
    float value)
{
    typedef union
    {
        float value;
        char b[sizeof(float)];
    } HELPER;
    HELPER tmp;
    tmp.value = value;
    int ret;
    if ((ret = ser_saveUInt8(serializeSettings, tmp.b[3])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[2])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[1])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[0])) == 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_FLOAT32

#ifndef DLMS_IGNORE_FLOAT64
int ser_saveDouble(
    gxSerializerSettings* serializeSettings,
    double value)
{
    typedef union
    {
        double value;
        char b[sizeof(double)];
    } HELPER;

    HELPER tmp;
    tmp.value = value;
    int ret;
    if ((ret = ser_saveUInt8(serializeSettings, tmp.b[7])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[6])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[5])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[4])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[3])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[2])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[1])) == 0 &&
        (ret = ser_saveUInt8(serializeSettings, tmp.b[0])) == 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_FLOAT64

// Set count of items.
int ser_saveObjectCount(uint32_t count, gxSerializerSettings* serializeSettings)
{
    int ret;
#ifdef DLMS_IGNORE_MALLOC
    ret = ser_saveUInt16(serializeSettings, (uint16_t)count);
#else
    if (count < 0x80)
    {
        ret = ser_saveUInt8(serializeSettings, (unsigned char)count);
    }
    else if (count < 0x100)
    {
        if ((ret = ser_saveUInt8(serializeSettings, 0x81)) == 0)
        {
            ret = ser_saveUInt8(serializeSettings, (unsigned char)count);
        }
    }
    else if (count < 0x10000)
    {
        if ((ret = ser_saveUInt8(serializeSettings, 0x82)) == 0)
        {
            ret = ser_saveUInt16(serializeSettings, (uint16_t)count);
        }
    }
    else
    {
        if ((ret = ser_saveUInt8(serializeSettings, 0x84)) == 0)
        {
            ret = ser_saveUInt32(serializeSettings, count);
        }
    }
#endif// DLMS_IGNORE_MALLOC
    return ret;
}

int ser_setOctetString2(
    gxSerializerSettings* serializeSettings,
    const unsigned char* value,
    uint16_t size,
    uint16_t capacity)
{
    int ret;
    if (value == NULL)
    {
        size = 0;
    }
    if ((ret = ser_saveObjectCount(size, serializeSettings)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
    (ret = ser_saveObjectCount(capacity, serializeSettings)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
        (ret = ser_set(serializeSettings, value, size
#ifdef DLMS_IGNORE_MALLOC
            , capacity
#endif //DLMS_IGNORE_MALLOC
        )) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}


//Returns bytes as Big Endian byteorder.
int ser_saveBytes3(
    dlmsVARIANT* data,
    DLMS_DATA_TYPE type,
    gxSerializerSettings* serializeSettings)
{
    int ret = 0, pos;
    if ((type & DLMS_DATA_TYPE_BYREF) != 0)
    {
        return ser_saveBytes3(data, type & ~DLMS_DATA_TYPE_BYREF, serializeSettings);
    }
    if ((ret = ser_saveUInt8(serializeSettings, type)) != 0)
    {
        return ret;
    }
    if (type == DLMS_DATA_TYPE_STRUCTURE ||
        type == DLMS_DATA_TYPE_ARRAY)
    {
        dlmsVARIANT* tmp;
        if ((ret = ser_saveObjectCount(data->Arr->size, serializeSettings)) == 0)
        {
#ifdef DLMS_IGNORE_MALLOC
            uint16_t originalSize = va_size(data->Arr);
            uint16_t count = va_getCapacity(data->Arr);
            if ((ret = ser_saveObjectCount(count, serializeSettings)) != 0)
            {
                return ret;
            }
            data->Arr->size = count;
#endif //DLMS_IGNORE_MALLOC
            for (pos = 0; pos != va_size(data->Arr); ++pos)
            {
                if ((ret = va_getByIndex(data->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_saveBytes3(tmp, tmp->vt, serializeSettings)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            //Return original size.
            data->Arr->size = originalSize;
#endif //DLMS_IGNORE_MALLOC
        }
        return ret;
    }
    switch (type)
    {
    case DLMS_DATA_TYPE_NONE:
        break;
    case DLMS_DATA_TYPE_UINT8:
    case DLMS_DATA_TYPE_ENUM:
        ret = ser_saveUInt8(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pbVal : data->bVal);
        break;
    case DLMS_DATA_TYPE_BOOLEAN:
        if ((data->vt & DLMS_DATA_TYPE_BYREF) != 0)
        {
            ret = ser_saveUInt8(serializeSettings, *data->pbVal == 0 ? 0 : 1);
        }
        else
        {
            ret = ser_saveUInt8(serializeSettings, data->bVal == 0 ? 0 : 1);
        }
        break;
    case DLMS_DATA_TYPE_UINT16:
        ret = ser_saveUInt16(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->puiVal : data->uiVal);
        break;
    case DLMS_DATA_TYPE_UINT32:
        ret = ser_saveUInt32(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pulVal : data->ulVal);
        break;
    case DLMS_DATA_TYPE_UINT64:
        ret = ser_saveUInt64(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pullVal : data->ullVal);
        break;
    case DLMS_DATA_TYPE_INT8:
        ret = ser_saveUInt8(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pcVal : data->cVal);
        break;
    case DLMS_DATA_TYPE_INT16:
        ret = ser_saveInt16(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->puiVal : data->uiVal);
        break;
    case DLMS_DATA_TYPE_INT32:
        ret = ser_saveUInt32(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->plVal : data->lVal);
        break;
    case DLMS_DATA_TYPE_INT64:
        ret = ser_saveInt64(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pllVal : data->llVal);
        break;
    case DLMS_DATA_TYPE_FLOAT32:
#ifndef DLMS_IGNORE_FLOAT32
        ret = ser_saveFloat(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pfltVal : data->fltVal);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_FLOAT32
        break;
    case DLMS_DATA_TYPE_FLOAT64:
#ifndef DLMS_IGNORE_FLOAT64
        ret = ser_saveDouble(serializeSettings, (data->vt & DLMS_DATA_TYPE_BYREF) != 0 ? *data->pdblVal : data->dblVal);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_FLOAT64
        break;
    case DLMS_DATA_TYPE_STRING:
#ifdef DLMS_IGNORE_MALLOC
        ret = ser_setOctetString2(serializeSettings, data->pbVal, data->size, data->capacity);
#else
        ret = ser_setOctetString2(serializeSettings, data->byteArr->data, (uint16_t)data->byteArr->size, (uint16_t)data->byteArr->capacity);
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_OCTET_STRING:
#ifndef DLMS_IGNORE_MALLOC
        if (data->vt == DLMS_DATA_TYPE_DATETIME)
        {
            if ((ret = ser_saveUInt8(serializeSettings, 12)) == 0)
            {
                ret = ser_saveDateTime2(data->dateTime, serializeSettings);
            }
        }
        else if (data->vt == DLMS_DATA_TYPE_DATE)
        {
            if ((ret = ser_saveUInt8(serializeSettings, 5)) == 0)
            {
                ret = ser_saveDate(data->dateTime, serializeSettings);
            }
        }
        else if (data->vt == DLMS_DATA_TYPE_TIME)
        {
            if ((ret = ser_saveUInt8(serializeSettings, 4)) == 0)
            {
                ret = ser_saveTime(data->dateTime, serializeSettings);
            }
        }
        else
        {
            ret = ser_setOctetString2(serializeSettings, data->byteArr->data, (uint16_t)data->byteArr->size, (uint16_t)data->byteArr->capacity);
        }
#else
        if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATETIME))
        {
            if ((ret = ser_saveUInt8(serializeSettings, 12)) == 0)
            {
                ret = ser_saveDateTime2(data->pVal, serializeSettings);
            }
        }
        else if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_DATE))
        {
            if ((ret = ser_saveUInt8(serializeSettings, 5)) == 0)
            {
                ret = ser_saveDate(data->pVal, serializeSettings);
            }
        }
        else if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_TIME))
        {
            if ((ret = ser_saveUInt8(serializeSettings, 4)) == 0)
            {
                ret = ser_saveTime(data->pVal, serializeSettings);
            }
        }
        else if (data->vt == (DLMS_DATA_TYPE_BYREF | DLMS_DATA_TYPE_OCTET_STRING))
        {
            if ((ret = ser_saveObjectCount(data->size, serializeSettings)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
            (ret = ser_saveObjectCount(data->capacity, serializeSettings)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                (ret = ser_set(serializeSettings, data->pbVal, data->size, data->capacity)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
#endif //DLMS_IGNORE_MALLOC
        break;
    case DLMS_DATA_TYPE_DATETIME:
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = ser_saveDateTime2(data->pVal, serializeSettings);
#else
        ret = ser_saveDateTime2(data->dateTime, serializeSettings);
#endif //DLMS_IGNORE_MALLOC
        break;
    }
    case DLMS_DATA_TYPE_DATE:
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = ser_saveDate(data->pVal, serializeSettings);
#else
        ret = ser_saveDate(data->dateTime, serializeSettings);
#endif //DLMS_IGNORE_MALLOC

        break;
    }
    case DLMS_DATA_TYPE_TIME:
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = ser_saveTime(data->pVal, serializeSettings);
#else
        ret = ser_saveTime(data->dateTime, serializeSettings);
#endif //DLMS_IGNORE_MALLOC

        break;
    }
    case DLMS_DATA_TYPE_BIT_STRING:
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = ser_saveObjectCount(data->size, serializeSettings)) == 0 &&
            (ret = ser_saveObjectCount(data->capacity, serializeSettings)) == 0)
        {
            ret = ser_set(serializeSettings, data->pVal, ba_getByteCount(data->size), ba_getByteCount(data->capacity));
        }
#else
        if ((ret = ser_saveObjectCount(data->bitArr->size, serializeSettings)) == 0)
        {
            ret = ser_set(serializeSettings, data->bitArr->data, ba_getByteCount(data->bitArr->size)
#ifdef DLMS_IGNORE_MALLOC
                , ba_getByteCount(ba_getCapacity(data->bitArr))
#endif //DLMS_IGNORE_MALLOC
            );
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

//Returns bytes as Big Endian byteorder.
int ser_saveBytes(
    dlmsVARIANT* data,
    gxSerializerSettings* serializeSettings)
{
    return ser_saveBytes3(data, data->vt, serializeSettings);
}

unsigned char isAttributeSet(gxSerializerSettings* serializeSettings, uint16_t attributes, unsigned char index)
{
#ifdef DLMS_IGNORE_MALLOC
    serializeSettings->currentIndex = index;
#endif //DLMS_IGNORE_MALLOC
    return (attributes & 1 << (index - 1)) != 0;
}

//Returns ignored attributes.
uint16_t ser_getIgnoredAttributes(gxSerializerSettings* serializeSettings, gxObject* object)
{
    uint16_t ret = 0;
    if (serializeSettings != NULL)
    {
        int pos;
        for (pos = 0; pos != serializeSettings->count; ++pos)
        {
            if (serializeSettings->ignoredAttributes[pos].target == object ||
                serializeSettings->ignoredAttributes[pos].objectType == object->objectType)
            {
                ret |= serializeSettings->ignoredAttributes[pos].attributes;
            }
        }
    }
    return ret;
}

int ser_saveDateTime(gxtime* value, gxSerializerSettings* serializeSettings)
{
    int ret;
    if ((ret = ser_saveDateTime2(value, serializeSettings)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int ser_saveOctetString(gxSerializerSettings* serializeSettings, gxByteBuffer* value)
{
    int ret;
    if ((ret = ser_saveObjectCount(bb_size(value), serializeSettings)) == 0 &&
#ifndef DLMS_IGNORE_MALLOC
    (ret = ser_set(serializeSettings, value->data, bb_size(value)
#ifdef DLMS_IGNORE_MALLOC
        , bb_getCapacity(value)
#endif //DLMS_IGNORE_MALLOC
    )) == 0)
#else
        (ret = ser_saveObjectCount(bb_getCapacity(value), serializeSettings)) == 0 &&
        (ret = ser_set(serializeSettings, value->data, bb_getCapacity(value), bb_getCapacity(value))) == 0)
#endif //DLMS_IGNORE_MALLOC            
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int ser_saveOctetString3(gxSerializerSettings* serializeSettings, char* value, uint16_t len, uint16_t capacity)
{
    int ret;
    if ((ret = ser_saveObjectCount(len, serializeSettings)) != 0 ||
#ifndef DLMS_IGNORE_MALLOC
    (ret = ser_set(serializeSettings, (unsigned char*)value, len)) != 0)
#else
        (ret = ser_saveObjectCount(capacity, serializeSettings)) != 0 ||
        (ret = ser_set(serializeSettings, (unsigned char*)value, len, capacity)) != 0)
#endif //DLMS_IGNORE_MALLOC
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int ser_saveOctetString2(gxSerializerSettings* serializeSettings, char* value, uint16_t capacity)
{
    uint16_t len;
    if (value != NULL)
    {
        len = (uint16_t)strlen(value);
    }
    else
    {
        len = 0;
    }
    return ser_saveOctetString3(serializeSettings, value, len, capacity);
}

int ser_saveBitString(gxSerializerSettings* serializeSettings, bitArray* value)
{
    int ret;
    if ((ret = ser_saveObjectCount(value->size, serializeSettings)) != 0 ||
#ifndef DLMS_IGNORE_MALLOC
    (ret = ser_set(serializeSettings, value->data, ba_getByteCount(value->size))) != 0)
#else
        (ret = ser_saveObjectCount(ba_getByteCount(ba_getCapacity(value)), serializeSettings)) != 0 ||
        (ret = ser_set(serializeSettings, value->data, ba_getByteCount(ba_getCapacity(value)), ba_getByteCount(ba_getCapacity(value)))) != 0)
#endif

    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int ser_saveObjectArrayCount(gxSerializerSettings* serializeSettings, objectArray* arr, uint16_t* count)
{
    int ret;
    if ((ret = ser_saveObjectCount(arr->size, serializeSettings)) == 0)
    {
        *count = arr->size;
#ifdef DLMS_IGNORE_MALLOC
        arr->size = oa_getCapacity(arr);
        if ((ret = ser_saveObjectCount(arr->size, serializeSettings)) != 0)
        {
            return ret;
        }
#else
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int ser_saveVariantArrayCount(gxSerializerSettings* serializeSettings, variantArray* arr, uint16_t* count)
{
    int ret;
    if ((ret = ser_saveObjectCount(arr->size, serializeSettings)) == 0)
    {
        *count = arr->size;
#ifdef DLMS_IGNORE_MALLOC
        arr->size = va_getCapacity(arr);
        if ((ret = ser_saveObjectCount(arr->size, serializeSettings)) != 0)
        {
            return ret;
        }
#else
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int ser_saveArrayCount(gxSerializerSettings* serializeSettings, gxArray* arr, uint16_t* count)
{
    int ret;
    if ((ret = ser_saveObjectCount(arr->size, serializeSettings)) == 0)
    {
        *count = arr->size;
#ifdef DLMS_IGNORE_MALLOC
        arr->size = arr_getCapacity(arr);
        if ((ret = ser_saveObjectCount(arr->size, serializeSettings)) != 0)
        {
            return ret;
        }
#else
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

#ifndef DLMS_IGNORE_DATA
int ser_saveData(gxSerializerSettings* serializeSettings, gxData* object)
{
    if (!isAttributeSet(serializeSettings, ser_getIgnoredAttributes(serializeSettings, (gxObject*)object), 2))
    {
        return ser_saveBytes(&object->value, serializeSettings);
    }
    return 0;
}

#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int ser_saveRegister(
    gxSerializerSettings* serializeSettings,
    gxRegister* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_saveUInt8(serializeSettings, object->scaler)) != 0 ||
            (ret = ser_saveUInt8(serializeSettings, object->unit)) != 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_saveBytes(&object->value, serializeSettings)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
int ser_saveClock(
    gxSerializerSettings* serializeSettings,
    gxClock* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveDateTime(&object->time, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveInt16(serializeSettings, object->timeZone)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->status)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveDateTime(&object->begin, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveDateTime(&object->end, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt8(serializeSettings, object->deviation)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt8(serializeSettings, object->enabled)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveUInt8(serializeSettings, object->clockBase)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int ser_saveScriptTable(
    gxSerializerSettings* serializeSettings,
    gxScriptTable* object)
{
    int ret = 0, pos, pos2;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        uint16_t count, count2;
        gxScript* s;
        gxScriptAction* sa;
        if ((ret = ser_saveArrayCount(serializeSettings, &object->scripts, &count)) == 0)
        {
            for (pos = 0; pos != object->scripts.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->scripts, pos, (void**)&s, sizeof(gxScript), 0)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->scripts, pos, (void**)&s, 0)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = ser_saveUInt16(serializeSettings, s->id)) == 0 &&
                    (ret = ser_saveArrayCount(serializeSettings, &s->actions, &count2)) == 0)
                {
                    for (pos2 = 0; pos2 != s->actions.size; ++pos2)
                    {
#ifdef DLMS_IGNORE_MALLOC
                        if ((ret = arr_getByIndex3(&s->actions, pos2, (void**)&sa, sizeof(gxScriptAction), 0)) != 0 ||
                            (ret = ser_saveUInt8(serializeSettings, sa->type)) != 0)
                        {
                            break;
                        }
#else
                        if ((ret = arr_getByIndex3(&s->actions, pos2, (void**)&sa, 0)) != 0 ||
                            (ret = ser_saveUInt8(serializeSettings, sa->type)) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        if (sa->target == NULL)
                        {
                            if ((ret = ser_saveUInt16(serializeSettings, 0)) != 0 ||
                                (ret = ser_set(serializeSettings, EMPTY_LN, sizeof(EMPTY_LN)
#ifdef DLMS_IGNORE_MALLOC
                                    , sizeof(EMPTY_LN)
#endif //DLMS_IGNORE_MALLOC
                                )) != 0)
                            {
                                break;
                            }
                        }
                        else
                        {
                            if ((ret = ser_saveUInt16(serializeSettings, sa->target->objectType)) != 0 ||
                                (ret = ser_set(serializeSettings, sa->target->logicalName, sizeof(sa->target->logicalName)
#ifdef DLMS_IGNORE_MALLOC
                                    , sizeof(sa->target->logicalName)
#endif //DLMS_IGNORE_MALLOC
                                )) != 0)
                            {
                                break;
                            }
                        }
#else
                        if ((ret = ser_saveUInt16(serializeSettings, sa->objectType)) != 0 ||
                            (ret = ser_set(serializeSettings, sa->logicalName, sizeof(sa->logicalName))) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        if ((ret = ser_saveUInt8(serializeSettings, sa->index)) != 0 ||
                            (ret = ser_saveBytes(&sa->parameter, serializeSettings)) != 0)
                        {
                            break;
                        }
                    }
#ifdef DLMS_IGNORE_MALLOC
                    s->actions.size = count2;
#endif //DLMS_IGNORE_MALLOC
                }
                if (ret != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->scripts.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int ser_saveSpecialDaysTable(
    gxSerializerSettings* serializeSettings,
    gxSpecialDaysTable* object)
{
    int ret = 0;
    uint16_t pos, count;
    gxSpecialDay* sd;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_saveArrayCount(serializeSettings, &object->entries, &count)) == 0)
        {
            for (pos = 0; pos != object->entries.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->entries, pos, (void**)&sd, sizeof(gxSpecialDay), 0)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->entries, pos, (void**)&sd, 0)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = ser_saveUInt16(serializeSettings, sd->index)) != 0 ||
                    (ret = ser_saveDateTime(&sd->date, serializeSettings)) != 0 ||
                    (ret = ser_saveUInt8(serializeSettings, sd->dayId)) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->entries.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int ser_saveTcpUdpSetup(
    gxSerializerSettings* serializeSettings,
    gxTcpUdpSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt16(serializeSettings, object->port)) != 0) ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_set(serializeSettings, obj_getLogicalName(object->ipSetup), 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0) ||
#else
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_set(&ba, object->ipReference, 6)) != 0) ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt16(serializeSettings, object->maximumSegmentSize)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->maximumSimultaneousConnections)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt16(serializeSettings, object->inactivityTimeout)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int ser_saveMBusMasterPortSetup(
    gxSerializerSettings* serializeSettings,
    gxMBusMasterPortSetup* object)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        return ser_saveUInt8(serializeSettings, object->commSpeed);
    }
    return 0;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

int saveTimeWindow(gxSerializerSettings* serializeSettings, gxArray* arr)
{
    int ret;
    uint16_t count, pos;
#ifdef DLMS_IGNORE_MALLOC
    gxTimePair* k;
#else
    gxKey* k;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = ser_saveArrayCount(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(arr, pos, (void**)&k, sizeof(gxTimePair), 0)) != 0 ||
                (ret = ser_saveDateTime(&k->first, serializeSettings)) != 0 ||
                (ret = ser_saveDateTime(&k->second, serializeSettings)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(arr, pos, (void**)&k), 0) != 0 ||
                (ret = ser_saveDateTime((gxtime*)k->key, serializeSettings)) != 0 ||
                (ret = ser_saveDateTime((gxtime*)k->value, serializeSettings)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
#ifdef DLMS_IGNORE_MALLOC
        arr->size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int saveObjectsInternal(gxSerializerSettings* serializeSettings, gxArray* objects)
{
    uint16_t count, pos;
    int ret;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = ser_saveArrayCount(serializeSettings, objects, &count)) == 0)
    {
        for (pos = 0; pos != objects->size; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(objects, pos, (void**)&it, sizeof(gxTarget), 0)) != 0 ||
                (ret = ser_saveUInt16(serializeSettings, pos < count ? it->target->objectType : 0)) != 0 ||
                (ret = ser_set(serializeSettings, pos < count ? obj_getLogicalName(it->target) : EMPTY_LN, 6, 6)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, pos < count ? it->attributeIndex : 0)) != 0 ||
                (ret = ser_saveUInt16(serializeSettings, pos < count ? it->dataIndex : 0)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(objects, pos, (void**)&it, 0)) != 0 ||
                (ret = ser_saveUInt16(serializeSettings, pos < objects->size ? it->target->objectType : 0)) != 0 ||
                (ret = ser_set(serializeSettings, obj_getLogicalName(it->target), 6, 6)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, pos < objects->size ? it->attributeIndex : 0)) != 0 ||
                (ret = ser_saveUInt16(serializeSettings, pos < objects->size ? it->dataIndex : 0)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
#else
            if ((ret = arr_getByIndex3(objects, pos, (void**)&it, 0)) != 0 ||
                (ret = ser_saveUInt16(serializeSettings, pos < objects->size ? ((gxObject*)it->key)->objectType : 0)) != 0 ||
                (ret = ser_set(serializeSettings, obj_getLogicalName((gxObject*)it->key), 6
#ifdef DLMS_IGNORE_MALLOC
                    , 6
#endif //DLMS_IGNORE_MALLOC
                )) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, pos < objects->size ? ((gxTarget*)it->value)->attributeIndex : 0)) != 0 ||
                (ret = ser_saveUInt16(serializeSettings, pos < objects->size ? ((gxTarget*)it->value)->dataIndex : 0)) != 0)
            {
                break;
            }
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        }
#ifdef DLMS_IGNORE_MALLOC
        objects->size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP
int ser_savePushSetup(
    gxSerializerSettings* serializeSettings,
    gxPushSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = saveObjectsInternal(serializeSettings, &object->pushObjectList)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) &&
            ((ret = ser_saveUInt8(serializeSettings, object->service)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, &object->destination)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, object->message)) != 0)) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = saveTimeWindow(serializeSettings, &object->communicationWindow)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt16(serializeSettings, object->randomisationStartInterval)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt8(serializeSettings, object->numberOfRetries)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt16(serializeSettings, object->repetitionDelay)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_AUTO_CONNECT
int ser_saveAutoConnect(
    gxSerializerSettings* serializeSettings,
    gxAutoConnect* object)
{
#ifdef DLMS_IGNORE_MALLOC
    gxDestination* dest;
#else
    gxByteBuffer* dest;
#endif //DLMS_IGNORE_MALLOC
    uint16_t pos, count;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->mode)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->repetitions)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt16(serializeSettings, object->repetitionDelay)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = saveTimeWindow(serializeSettings, &object->callingWindow)) != 0))
    {
    }
    else
    {
        if (!isAttributeSet(serializeSettings, ignored, 6))
        {
            if ((ret = ser_saveArrayCount(serializeSettings, &object->destinations, &count)) == 0)
            {
                for (pos = 0; pos != object->destinations.size; ++pos)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = arr_getByIndex3(&object->destinations, pos, (void**)&dest, sizeof(gxDestination), 0)) != 0 ||
                        (ret = ser_saveOctetString3(serializeSettings, (char*)dest->value, dest->size, sizeof(dest->value))) != 0)
                    {
                        break;
                    }
#else
                    if ((ret = arr_getByIndex3(&object->destinations, pos, (void**)&dest, 0)) != 0 ||
                        (ret = ser_saveOctetString3(serializeSettings, (char*)dest->data, (unsigned short)dest->size, (unsigned short)dest->capacity)) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->destinations.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int ser_saveSeasonProfile(gxArray* arr, gxSerializerSettings* serializeSettings)
{
    gxSeasonProfile* it;
    int ret;
    uint16_t pos, count;
    if ((ret = ser_saveArrayCount(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(arr, pos, (void**)&it, sizeof(gxSeasonProfile), 0)) != 0)
            {
                break;
            }
            if ((ret = ser_saveOctetString3(serializeSettings, (char*)it->name.value, it->name.size, sizeof(it->name.value))) != 0 ||
                (ret = ser_saveDateTime(&it->start, serializeSettings)) != 0 ||
                (ret = ser_saveOctetString3(serializeSettings, (char*)it->weekName.value, it->weekName.size, sizeof(it->weekName.value))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(arr, pos, (void**)&it, 0)) != 0)
            {
                break;
            }
            if ((ret = ser_saveOctetString(serializeSettings, &it->name)) != 0 ||
                (ret = ser_saveDateTime(&it->start, serializeSettings)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, &it->weekName)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
#ifdef DLMS_IGNORE_MALLOC
        arr->size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int ser_saveweekProfile(gxArray* arr, gxSerializerSettings* serializeSettings)
{
    gxWeekProfile* it;
    int ret;
    uint16_t pos, count;
    if ((ret = ser_saveArrayCount(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(arr, pos, (void**)&it, sizeof(gxWeekProfile), 0)) != 0 ||
                (ret = ser_saveOctetString3(serializeSettings, (char*)it->name.value, it->name.size, sizeof(it->name.value))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(arr, pos, (void**)&it, 0)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, &it->name)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_saveUInt8(serializeSettings, it->monday)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, it->tuesday)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, it->wednesday)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, it->thursday)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, it->friday)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, it->saturday)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, it->sunday)) != 0)
            {
                break;
            }
        }
#ifdef DLMS_IGNORE_MALLOC
        arr->size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int ser_saveDayProfile(gxArray* arr, gxSerializerSettings* serializeSettings)
{
    gxDayProfile* dp;
    gxDayProfileAction* it;
    int ret;
    uint16_t pos, pos2, count, count2;
    if ((ret = ser_saveArrayCount(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(arr, pos, (void**)&dp, sizeof(gxDayProfile), 0)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(arr, pos, (void**)&dp, 0)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_saveUInt8(serializeSettings, dp->dayId)) != 0 ||
                (ret = ser_saveArrayCount(serializeSettings, &dp->daySchedules, &count2)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&dp->daySchedules, pos2, (void**)&it, sizeof(gxDayProfileAction), 0)) != 0 ||
#else
                if ((ret = arr_getByIndex3(&dp->daySchedules, pos2, (void**)&it, 0)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                (ret = ser_set(serializeSettings, obj_getLogicalName(it->script), 6
#ifdef DLMS_IGNORE_MALLOC
                    , 6
#endif //DLMS_IGNORE_MALLOC
                )) != 0 ||
#else
                    (ret = hlp_appendLogicalName(serializeSettings, it->scriptLogicalName)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = ser_saveUInt16(serializeSettings, it->scriptSelector)) != 0 ||
                    (ret = ser_saveDateTime(&it->startTime, serializeSettings)) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            dp->daySchedules.size = count2;
#endif //DLMS_IGNORE_MALLOC
        }
#ifdef DLMS_IGNORE_MALLOC
        arr->size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int ser_saveActivityCalendar(
    gxSerializerSettings* serializeSettings,
    gxActivityCalendar* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveOctetString(serializeSettings, &object->calendarNameActive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveSeasonProfile(&object->seasonProfileActive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveweekProfile(&object->weekProfileTableActive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveDayProfile(&object->dayProfileTableActive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveOctetString(serializeSettings, &object->calendarNamePassive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveSeasonProfile(&object->seasonProfilePassive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveweekProfile(&object->weekProfileTablePassive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveDayProfile(&object->dayProfileTablePassive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_saveDateTime(&object->time, serializeSettings)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_SECURITY_SETUP
int ser_saveSecuritySetup(
    gxSerializerSettings* serializeSettings,
    gxSecuritySetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->securityPolicy)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->securitySuite)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveOctetString(serializeSettings, &object->serverSystemTitle)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveOctetString(serializeSettings, &object->clientSystemTitle)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int ser_saveHdlcSetup(
    gxSerializerSettings* serializeSettings,
    gxIecHdlcSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->communicationSpeed)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->windowSizeTransmit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->windowSizeReceive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt16(serializeSettings, object->maximumInfoLengthTransmit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt16(serializeSettings, object->maximumInfoLengthReceive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt16(serializeSettings, object->interCharachterTimeout)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt16(serializeSettings, object->inactivityTimeout)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveUInt16(serializeSettings, object->deviceAddress)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int ser_saveLocalPortSetup(
    gxSerializerSettings* serializeSettings,
    gxLocalPortSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->defaultMode)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->defaultBaudrate)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->proposedBaudrate)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->responseTime)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveOctetString(serializeSettings, &object->deviceAddress)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveOctetString(serializeSettings, &object->password1)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveOctetString(serializeSettings, &object->password2)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveOctetString(serializeSettings, &object->password5)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int ser_saveIecTwistedPairSetup(
    gxSerializerSettings* serializeSettings,
    gxIecTwistedPairSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->mode)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->speed)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveOctetString(serializeSettings, &object->primaryAddresses)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveOctetString(serializeSettings, &object->tabis)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int ser_saveDemandRegister(
    gxSerializerSettings* serializeSettings,
    gxDemandRegister* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveBytes(&object->currentAverageValue, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveBytes(&object->lastAverageValue, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->scaler)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->unit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveBytes(&object->status, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveDateTime(&object->captureTime, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveDateTime(&object->startTimeCurrent, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveUInt32(serializeSettings, object->period)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_saveUInt16(serializeSettings, object->numberOfPeriods)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int ser_saveRegisterActivation(
    gxSerializerSettings* serializeSettings,
    gxRegisterActivation* object)
{
    int ret = 0;
    uint16_t count, pos;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxRegisterActivationMask* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#if defined(DLMS_IGNORE_MALLOC)
        if ((ret = ser_saveArrayCount(serializeSettings, &object->registerAssignment, &count)) == 0)
#else
        if ((ret = ser_saveObjectArrayCount(serializeSettings, &object->registerAssignment, &count)) == 0)
#endif //DLMS_IGNORE_MALLOC
        {
            for (pos = 0; pos != object->registerAssignment.size; ++pos)
            {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
#if defined(DLMS_IGNORE_MALLOC)
                if ((ret = arr_getByIndexRef(&object->registerAssignment, pos, (void**)&od)) != 0 ||
#else
#if defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_getByIndex3(&object->registerAssignment, pos, (void**)&od, 0)) != 0 ||
#else
                if ((ret = oa_getByIndex(&object->registerAssignment, pos, &od)) != 0 ||
#endif //defined(DLMS_COSEM_EXACT_DATA_TYPES)
#endif //DLMS_IGNORE_MALLOC
#else
                if ((ret = arr_getByIndex3(&object->registerAssignment, pos, (void**)&od, 0)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = ser_saveUInt16(serializeSettings, pos < count && od != NULL ? od->objectType : 0)) != 0 ||
                    (ret = ser_set(serializeSettings, pos < count && od != NULL ? obj_getLogicalName(od) : EMPTY_LN, 6
#ifdef DLMS_IGNORE_MALLOC
                        , 6
#endif //DLMS_IGNORE_MALLOC
                    )) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->registerAssignment.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3) &&
        (ret = ser_saveArrayCount(serializeSettings, &object->maskList, &count)) == 0)
    {
        for (pos = 0; pos != object->maskList.size; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask), 0)) != 0 ||
                (ret = ser_saveOctetString3(serializeSettings, (char*)it->name, it->length, sizeof(it->name))) != 0 ||
                (ret = ser_saveOctetString3(serializeSettings, (char*)it->indexes, it->count, sizeof(it->indexes))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex4(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask), 0)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, &it->name)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, &it->indexes)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
#else
            if ((ret = arr_getByIndex3(&object->maskList, pos, (void**)&it, 0)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, (gxByteBuffer*)it->key)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, (gxByteBuffer*)it->value)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
#ifdef DLMS_IGNORE_MALLOC
        object->maskList.size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        ret = ser_saveOctetString(serializeSettings, &object->activeMask);
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int ser_saveActionItem(
    gxActionItem* item,
    gxSerializerSettings* serializeSettings)
{
    int ret;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = ser_set(serializeSettings, obj_getLogicalName((gxObject*)item->script), 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0 ||
#else
    if ((ret = ser_set(serializeSettings, item->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = ser_saveUInt16(serializeSettings, item->scriptSelector)) != 0)
    {
    }
    return ret;
}

int ser_saveRegisterMonitor(
    gxSerializerSettings* serializeSettings,
    gxRegisterMonitor* object)
{
    int ret = 0;
    uint16_t count, pos;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    dlmsVARIANT* tmp;
    gxActionSet* as;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_saveVariantArrayCount(serializeSettings, &object->thresholds, &count)) == 0)
        {
            for (pos = 0; pos != object->thresholds.size; ++pos)
            {
                if (pos < object->thresholds.size)
                {
                    if ((ret = va_getByIndex(&object->thresholds, pos, &tmp)) != 0)
                    {
                        break;
                    }
                }
                if ((ret = ser_saveBytes(tmp, serializeSettings)) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->thresholds.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (((ret = ser_saveUInt16(serializeSettings, object->monitoredValue.target == NULL ? 0 : object->monitoredValue.target->objectType)) != 0 ||
            (ret = ser_set(serializeSettings, obj_getLogicalName(object->monitoredValue.target), 6
#ifdef DLMS_IGNORE_MALLOC
                , 6
#endif //DLMS_IGNORE_MALLOC
            )) != 0))
        {

        }
#else
        if ((ret = ser_saveUInt16(serializeSettings, object->monitoredValue.target == NULL ? 0 : object->monitoredValue.objectType)) != 0 ||
            (ret = ser_set(serializeSettings, obj_getLogicalName(object->monitoredValue.logicalName), 6)) != 0)
        {

        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0)
        {
            ret = ser_saveUInt8(serializeSettings, object->monitoredValue.attributeIndex);
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        if ((ret = ser_saveArrayCount(serializeSettings, &object->actions, &count)) == 0)
        {
            for (pos = 0; pos != object->actions.size; ++pos)
            {

#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->actions, pos, (void**)&as, sizeof(gxActionSet), 0)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->actions, pos, (void**)&as, 0)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = ser_saveActionItem(&as->actionUp, serializeSettings)) != 0 ||
                    (ret = ser_saveActionItem(&as->actionDown, serializeSettings)) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->actions.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int ser_saveActionSchedule(
    gxSerializerSettings* serializeSettings,
    gxActionSchedule* object)
{
    uint16_t pos, count;
    int ret = 0;
    gxtime* tm;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = ser_set(serializeSettings, obj_getLogicalName((gxObject*)object->executedScript), 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) == 0)
        {
            ret = ser_saveUInt16(serializeSettings, object->executedScriptSelector);
        }
#else
        if ((ret = ser_set(serializeSettings, object->executedScriptLogicalName, 6)) == 0)
        {
            ret = ser_saveUInt16(serializeSettings, object->executedScriptSelector);
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        ret = ser_saveUInt8(serializeSettings, object->type);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        //Add count.
        if ((ret = ser_saveArrayCount(serializeSettings, &object->executionTime, &count)) == 0)
        {
            for (pos = 0; pos != object->executionTime.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->executionTime, pos, (void**)&tm, sizeof(gxtime), 0)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->executionTime, pos, (void**)&tm, 0)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = ser_saveDateTime(tm, serializeSettings)) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->executionTime.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int ser_saveSapAssignment(
    gxSerializerSettings* serializeSettings,
    gxSapAssignment* object)
{
    uint16_t pos, count;
    int ret = 0;
    gxSapItem* it;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        //Add count.
        if ((ret = ser_saveArrayCount(serializeSettings, &object->sapAssignmentList, &count)) == 0)
        {
            for (pos = 0; pos != object->sapAssignmentList.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem), 0)) != 0 ||
                    (ret = ser_saveUInt16(serializeSettings, it->id)) != 0 ||
                    (ret = ser_saveOctetString3(serializeSettings, (char*)it->name.value, it->name.size, sizeof(it->name.value))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->sapAssignmentList, pos, (void**)&it, 0)) != 0 ||
                    (ret = ser_saveUInt16(serializeSettings, it->id)) != 0 ||
                    (ret = ser_saveOctetString(serializeSettings, &it->name)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
#ifdef DLMS_IGNORE_MALLOC
            object->sapAssignmentList.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_AUTO_ANSWER
int ser_saveAutoAnswer(
    gxSerializerSettings* serializeSettings,
    gxAutoAnswer* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->mode)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = saveTimeWindow(serializeSettings, &object->listeningWindow)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->status)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->numberOfCalls)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt8(serializeSettings, object->numberOfRingsInListeningWindow)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt8(serializeSettings, object->numberOfRingsOutListeningWindow)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt8(serializeSettings, object->numberOfRingsOutListeningWindow)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int ser_saveIp4Setup(
    gxSerializerSettings* serializeSettings,
    gxIp4Setup* object)
{
    uint16_t pos, count;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    uint32_t* tmp;
#else
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* ip;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = ser_set(serializeSettings, obj_getLogicalName(object->dataLinkLayer), 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        );

#else
        ret = ser_set(serializeSettings, object->dataLinkLayerReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        (ret = ser_saveUInt32(serializeSettings, object->ipAddress));
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
#if defined(DLMS_IGNORE_MALLOC)
        if ((ret = ser_saveArrayCount(serializeSettings, &object->multicastIPAddress, &count)) == 0)
#else
        if ((ret = ser_saveVariantArrayCount(serializeSettings, &object->multicastIPAddress, &count)) == 0)
#endif //DLMS_IGNORE_MALLOC
        {
            for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_getByIndex4(&object->multicastIPAddress, pos, (void**)&tmp, sizeof(uint32_t), 0)) != 0 ||
                    (ret = ser_saveUInt32(serializeSettings, *tmp)) != 0)
                {
                    break;
                }
#else
                if ((ret = va_getByIndex(&object->multicastIPAddress, pos, &tmp)) != 0 ||
                    (ret = ser_saveUInt32(serializeSettings, tmp->ulVal)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
#ifdef DLMS_IGNORE_MALLOC
            object->multicastIPAddress.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5) &&
        (ret = ser_saveArrayCount(serializeSettings, &object->ipOptions, &count)) == 0)
    {
        for (pos = 0; pos != object->ipOptions.size; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(&object->ipOptions, pos, (void**)&ip, sizeof(gxip4SetupIpOption), 0)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(&object->ipOptions, pos, (void**)&ip, 0)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_saveUInt8(serializeSettings, ip->type)) != 0)
            {
                break;
            }

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = ser_saveOctetString3(serializeSettings, (char*)ip->data.value, ip->data.size, sizeof(ip->data.value))) != 0)
            {
                break;
            }
#else
            if ((ret = ser_saveOctetString(serializeSettings, &ip->data)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
#ifdef DLMS_IGNORE_MALLOC
        object->ipOptions.size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt32(serializeSettings, object->subnetMask)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt32(serializeSettings, object->gatewayIPAddress)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt8(serializeSettings, object->useDHCP)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveUInt32(serializeSettings, object->primaryDNSAddress)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_saveUInt32(serializeSettings, object->secondaryDNSAddress)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP

int saveIpv6Address(IN6_ADDR* address, gxSerializerSettings* serializeSettings)
{
    unsigned char* tmp;
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    tmp = address->u.Byte;
#else //Linux includes.
    tmp = address->s6_addr;
#endif
    return ser_set(serializeSettings, tmp, 16
#ifdef DLMS_IGNORE_MALLOC
        , 16
#endif //DLMS_IGNORE_MALLOC
    );
}

int saveAddressList(gxArray* list, gxSerializerSettings* serializeSettings)
{
    IN6_ADDR* it;
    uint16_t pos, count;
    int ret = 0;
    if ((ret = ser_saveArrayCount(serializeSettings, list, &count)) == 0)
    {
        for (pos = 0; pos != list->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(list, pos, (void**)&it, sizeof(IN6_ADDR), 0)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(list, pos, (void**)&it, 0)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = saveIpv6Address(it, serializeSettings)) != 0)
            {
                break;
            }
        }
#ifdef DLMS_IGNORE_MALLOC
        list->size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int ser_saveIp6Setup(
    gxSerializerSettings* serializeSettings,
    gxIp6Setup* object)
{
    uint16_t pos, count;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = ser_set(serializeSettings, obj_getLogicalName(object->dataLinkLayer), 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        );
#else
        ret = ser_set(serializeSettings, object->dataLinkLayerReference, 6));
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->addressConfigMode)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 4) && (ret = saveAddressList(&object->unicastIPAddress, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = saveAddressList(&object->multicastIPAddress, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = saveAddressList(&object->gatewayIPAddress, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = saveIpv6Address(&object->primaryDNSAddress, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 8) && (ret = saveIpv6Address(&object->secondaryDNSAddress, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveUInt8(serializeSettings, object->trafficClass)) != 0))
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
    {
        gxNeighborDiscoverySetup* it;
        if ((ret = ser_saveArrayCount(serializeSettings, &object->neighborDiscoverySetup, &count)) == 0)
        {
            for (pos = 0; pos != object->neighborDiscoverySetup.size; ++pos)
            {
                if ((ret = arr_getByIndex4(&object->neighborDiscoverySetup, pos, (void**)&it, sizeof(gxNeighborDiscoverySetup), 0)) != 0)
                {
                    break;
                }
                if ((ret = ser_saveUInt8(serializeSettings, it->maxRetry)) != 0 ||
                    (ret = ser_saveUInt16(serializeSettings, it->retryWaitTime)) != 0 ||
                    (ret = ser_saveUInt32(serializeSettings, it->sendPeriod)) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->neighborDiscoverySetup.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
int ser_saveUtilityTables(
    gxSerializerSettings* serializeSettings,
    gxUtilityTables* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt16(serializeSettings, object->tableId)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveOctetString(serializeSettings, &object->buffer)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int ser_saveMbusSlavePortSetup(
    gxSerializerSettings* serializeSettings,
    gxMbusSlavePortSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->defaultBaud)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->availableBaud)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->addressState)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->busAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int ser_saveImageTransfer(
    gxSerializerSettings* serializeSettings,
    gxImageTransfer* object)
{
    uint16_t pos, count;
    int ret = 0;
    gxImageActivateInfo* it;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt32(serializeSettings, object->imageBlockSize)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveBitString(serializeSettings, &object->imageTransferredBlocksStatus)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt32(serializeSettings, object->imageFirstNotTransferredBlockNumber)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->imageTransferEnabled)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt8(serializeSettings, object->imageTransferStatus)) != 0))
    {
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7))
    {
        if ((ret = ser_saveArrayCount(serializeSettings, &object->imageActivateInfo, &count)) == 0)
        {
            for (pos = 0; pos != object->imageActivateInfo.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->imageActivateInfo, pos, (void**)&it, sizeof(gxImageActivateInfo), 0)) != 0)
                {
                    break;
                }
                if ((ret = ser_saveUInt32(serializeSettings, it->size)) != 0 ||
                    (ret = ser_saveOctetString3(serializeSettings, (char*)it->identification.data, it->identification.size, sizeof(it->identification.data))) != 0 ||
                    (ret = ser_saveOctetString3(serializeSettings, (char*)it->signature.data, it->signature.size, sizeof(it->signature.data))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->imageActivateInfo, pos, (void**)&it, 0)) != 0)
                {
                    break;
                }
                if ((ret = ser_saveUInt32(serializeSettings, it->size)) != 0 ||
                    (ret = ser_saveOctetString(serializeSettings, &it->identification)) != 0 ||
                    (ret = ser_saveOctetString(serializeSettings, &it->signature)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
#ifdef DLMS_IGNORE_MALLOC
            object->imageActivateInfo.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int ser_saveDisconnectControl(
    gxSerializerSettings* serializeSettings,
    gxDisconnectControl* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt8(serializeSettings, object->outputState)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->controlState)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->controlMode)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int ser_saveLimiter(
    gxSerializerSettings* serializeSettings,
    gxLimiter* object)
{
    uint16_t pos, count;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    dlmsVARIANT* it;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_set(serializeSettings, obj_getLogicalName(object->monitoredValue), 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) == 0)
            ret = ser_saveUInt8(serializeSettings, object->selectedAttributeIndex);
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveBytes(&object->thresholdActive, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveBytes(&object->thresholdNormal, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveBytes(&object->thresholdEmergency, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt32(serializeSettings, object->minOverThresholdDuration)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt32(serializeSettings, object->minUnderThresholdDuration)) != 0))
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8))
    {
        if ((ret = ser_saveUInt16(serializeSettings, object->emergencyProfile.id)) != 0 ||
            (ret = ser_saveDateTime(&object->emergencyProfile.activationTime, serializeSettings)) != 0 ||
            (ret = ser_saveUInt32(serializeSettings, object->emergencyProfile.duration)) != 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 9))
    {
#if defined(DLMS_IGNORE_MALLOC)
        if ((ret = ser_saveArrayCount(serializeSettings, &object->emergencyProfileGroupIDs, &count)) == 0)
#else
        if ((ret = ser_saveVariantArrayCount(serializeSettings, &object->emergencyProfileGroupIDs, &count)) == 0)
#endif //DLMS_IGNORE_MALLOC
        {
            for (pos = 0; pos != object->emergencyProfileGroupIDs.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_getByIndex4(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(dlmsVARIANT), 0)) != 0 ||
                    (ret = ser_saveBytes(it, serializeSettings)) != 0)
                {
                    break;
                }
#else
                if ((ret = va_getByIndex(&object->emergencyProfileGroupIDs, pos, &it)) != 0 ||
                    (ret = ser_saveBytes(it, serializeSettings)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
#ifdef DLMS_IGNORE_MALLOC
            object->emergencyProfileGroupIDs.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
    {
        ret = ser_saveUInt8(serializeSettings, object->emergencyProfileActive);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 11))
    {
        if ((ret = ser_saveActionItem(&object->actionOverThreshold, serializeSettings)) == 0)
        {
            ret = ser_saveActionItem(&object->actionUnderThreshold, serializeSettings);
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int ser_saveMBusClient(
    gxSerializerSettings* serializeSettings,
    gxMBusClient* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt32(serializeSettings, object->capturePeriod)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->primaryAddress)) != 0) ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_set(serializeSettings, obj_getLogicalName(object->mBusPort), 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0) ||
#else
        (!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_set(serializeSettings, obj_getLogicalName(object->mBusPortReference), 6)) != 0) ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt32(serializeSettings, object->identificationNumber)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt16(serializeSettings, object->manufacturerID)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt8(serializeSettings, object->dataHeaderVersion)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveUInt8(serializeSettings, object->deviceType)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_saveUInt8(serializeSettings, object->accessNumber)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 11) && (ret = ser_saveUInt8(serializeSettings, object->status)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 12) && (ret = ser_saveUInt8(serializeSettings, object->alarm)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 13) && (ret = ser_saveUInt16(serializeSettings, object->configuration)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 14) && (ret = ser_saveUInt8(serializeSettings, object->encryptionKeyStatus)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int ser_saveModemConfiguration(
    gxSerializerSettings* serializeSettings,
    gxModemConfiguration* object)
{
    int ret = 0;
    uint16_t pos, count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_saveUInt8(serializeSettings, object->communicationSpeed);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3) &&
        (ret = ser_saveArrayCount(serializeSettings, &object->initialisationStrings, &count)) == 0)
    {
        for (pos = 0; pos != object->initialisationStrings.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(&object->initialisationStrings, pos, (void**)&mi, sizeof(gxModemInitialisation), 0)) != 0 ||
                (ret = ser_saveOctetString3(serializeSettings, (char*)mi->request.value, mi->request.size, sizeof(mi->request.value))) != 0 ||
                (ret = ser_saveOctetString3(serializeSettings, (char*)mi->response.value, mi->response.size, sizeof(mi->response.value))) != 0 ||
                (ret = ser_saveUInt16(serializeSettings, mi->delay)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(&object->initialisationStrings, pos, (void**)&mi, 0)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, &mi->request)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, &mi->response)) != 0 ||
                (ret = ser_saveInt16(serializeSettings, mi->delay)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
#ifdef DLMS_IGNORE_MALLOC
        object->initialisationStrings.size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4) &&
        (ret = ser_saveArrayCount(serializeSettings, &object->modemProfile, &count)) == 0)
    {
        for (pos = 0; pos != object->modemProfile.size; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(&object->modemProfile, pos, (void**)&it, sizeof(gxModemProfile), 0)) != 0 ||
                (ret = ser_saveOctetString3(serializeSettings, (char*)it->value, it->size, sizeof(it->value))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(&object->modemProfile, pos, (void**)&it, 0)) != 0 ||
                (ret = ser_saveOctetString(serializeSettings, it)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
#ifdef DLMS_IGNORE_MALLOC
        object->modemProfile.size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int ser_saveMacAddressSetup(
    gxSerializerSettings* serializeSettings,
    gxMacAddressSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_saveOctetString(serializeSettings, &object->macAddress);
    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
int ser_saveQualityOfService(
    gxQualityOfService* object,
    gxSerializerSettings* serializeSettings)
{
    int ret;
    if ((ret = ser_saveUInt8(serializeSettings, object->precedence)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->delay)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->reliability)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->peakThroughput)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->meanThroughput)) != 0)
    {

    }
    return ret;
}

int ser_saveGPRSSetup(
    gxSerializerSettings* serializeSettings,
    gxGPRSSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveOctetString(serializeSettings, &object->apn)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveInt16(serializeSettings, object->pinCode)) != 0))
    {

    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        if (((ret = ser_saveQualityOfService(&object->defaultQualityOfService, serializeSettings)) != 0 ||
            (ret = ser_saveQualityOfService(&object->requestedQualityOfService, serializeSettings)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int ser_saveExtendedRegister(
    gxSerializerSettings* serializeSettings,
    gxExtendedRegister* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_saveBytes(&object->value, serializeSettings);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_saveUInt8(serializeSettings, object->scaler)) != 0 ||
            (ret = ser_saveUInt8(serializeSettings, object->unit)) != 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        ret = ser_saveBytes(&object->status, serializeSettings);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        ret = ser_saveDateTime(&object->captureTime, serializeSettings);
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
int ser_saveApplicationContextName(gxSerializerSettings* serializeSettings, gxApplicationContextName* object)
{
    int ret;
    if ((ret = ser_set(serializeSettings, object->logicalName, 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->jointIsoCtt)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->country)) != 0 ||
        (ret = ser_saveUInt16(serializeSettings, object->countryName)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->identifiedOrganization)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->dlmsUA)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->applicationContext)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->contextId)) != 0)
    {

    }
    return ret;
}


int ser_savexDLMSContextType(gxSerializerSettings* serializeSettings, gxXDLMSContextType* object)
{
    int ret;
    if ((ret = ser_saveUInt32(serializeSettings, object->conformance)) != 0 ||
        (ret = ser_saveUInt16(serializeSettings, object->maxReceivePduSize)) != 0 ||
        (ret = ser_saveUInt16(serializeSettings, object->maxSendPduSize)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->qualityOfService)) != 0 ||
        (ret = ser_saveOctetString(serializeSettings, &object->cypheringInfo)) != 0)
    {

    }
    return ret;
}

int ser_saveAuthenticationMechanismName(
    gxSerializerSettings* serializeSettings,
    gxAuthenticationMechanismName* object)
{
    int ret;
    if ((ret = ser_saveUInt8(serializeSettings, object->jointIsoCtt)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->country)) != 0 ||
        (ret = ser_saveUInt16(serializeSettings, object->countryName)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->identifiedOrganization)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->dlmsUA)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->authenticationMechanismName)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->mechanismId)) != 0)
    {

    }
    return ret;
}

int ser_saveAssociationLogicalName(
    gxSerializerSettings* serializeSettings,
    gxAssociationLogicalName* object)
{
    int ret = 0;
    uint16_t pos, count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#else
    gxKey2* it;
#endif //DLMS_IGNORE_MALLOC

    //Save object list.
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        gxObject* obj;
        if ((ret = ser_saveObjectArrayCount(serializeSettings, &object->objectList, &count)) == 0)
        {
            for (pos = 0; pos != object->objectList.size; ++pos)
            {
                if ((ret = oa_getByIndex(&object->objectList, pos, &obj)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_saveUInt8(serializeSettings, obj->version)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_saveUInt16(serializeSettings, obj->objectType)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_set(serializeSettings, obj->logicalName, 6
#ifdef DLMS_IGNORE_MALLOC
                        , 6
#endif //DLMS_IGNORE_MALLOC
                    )) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->objectList.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
        if (ret != 0)
        {
            return ret;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_saveUInt8(serializeSettings, object->clientSAP)) != 0 ||
            (ret = ser_saveUInt16(serializeSettings, object->serverSAP)) != 0)
        {
        }
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveApplicationContextName(serializeSettings, &object->applicationContextName)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_savexDLMSContextType(serializeSettings, &object->xDLMSContextInfo)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveAuthenticationMechanismName(serializeSettings, &object->authenticationMechanismName)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveOctetString(serializeSettings, &object->secret)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt8(serializeSettings, object->associationStatus)) != 0))
        {
        }
    }
    //Security Setup Reference is from version 1.
    if (ret == 0 && object->base.version > 0)
    {
#ifndef DLMS_IGNORE_SECURITY_SETUP
        if (!isAttributeSet(serializeSettings, ignored, 9))
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = ser_set(serializeSettings, obj_getLogicalName((gxObject*)object->securitySetup), 6
#ifdef DLMS_IGNORE_MALLOC
                , 6
#endif //DLMS_IGNORE_MALLOC
            );
#else
            ret = ser_set(&ba, object->securitySetupReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
#endif //DLMS_IGNORE_SECURITY_SETUP
    }
    if (ret == 0 && object->base.version > 1)
    {
        if (!isAttributeSet(serializeSettings, ignored, 10) &&
            (ret = ser_saveArrayCount(serializeSettings, &object->userList, &count)) == 0)
        {
            for (pos = 0; pos != object->userList.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->userList, pos, (void**)&it, sizeof(gxUser), 0)) != 0 ||
                    (ret = ser_saveUInt8(serializeSettings, it->id)) != 0 ||
                    (ret = ser_saveOctetString2(serializeSettings, it->name, MAX_USER_NAME_LENGTH)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->userList, pos, (void**)&it, 0)) != 0 ||
                    (ret = ser_saveUInt8(serializeSettings, it->key)) != 0 ||
                    (ret = ser_saveOctetString2(serializeSettings, it->value, (uint16_t)((gxByteBuffer*)it->value)->size)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
#ifdef DLMS_IGNORE_MALLOC
            object->userList.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int ser_saveAssociationShortName(
    gxSerializerSettings* serializeSettings,
    gxAssociationShortName* object)
{
    int ret = 0;
    uint16_t pos, count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#else
    gxKey2* it;
#endif //DLMS_IGNORE_MALLOC

    //Save object list.
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        gxObject* obj;
        if ((ret = ser_saveObjectArrayCount(serializeSettings, &object->objectList, &count)) == 0)
        {
            for (pos = 0; pos != object->objectList.size; ++pos)
            {
                if ((ret = oa_getByIndex(&object->objectList, pos, &obj)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_saveInt16(serializeSettings, obj->shortName)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_saveUInt8(serializeSettings, obj->version)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_saveUInt16(serializeSettings, obj->objectType)) != DLMS_ERROR_CODE_OK ||
                    (ret = ser_set(serializeSettings, obj->logicalName, 6
#ifdef DLMS_IGNORE_MALLOC
                        , 6
#endif //DLMS_IGNORE_MALLOC
                    )) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->objectList.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
        if (ret != 0)
        {
            return ret;
        }
    }
    //Security Setup Reference is from version 2.
    if (ret == 0 && object->base.version > 2)
    {
#ifndef DLMS_IGNORE_SECURITY_SETUP
        if (!isAttributeSet(serializeSettings, ignored, 9))
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = ser_set(serializeSettings, obj_getLogicalName((gxObject*)object->securitySetup), 6
#ifdef DLMS_IGNORE_MALLOC
                , 6
#endif //DLMS_IGNORE_MALLOC
            );
#else
            ret = ser_set(&ba, object->securitySetupReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
#endif //DLMS_IGNORE_SECURITY_SETUP
    }
    if (ret == 0 && object->base.version > 3)
    {
        if (!isAttributeSet(serializeSettings, ignored, 10) &&
            (ret = ser_saveArrayCount(serializeSettings, &object->userList, &count)) == 0)
        {
            for (pos = 0; pos != object->userList.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->userList, pos, (void**)&it, sizeof(gxUser), 0)) != 0 ||
                    (ret = ser_saveUInt8(serializeSettings, it->id)) != 0 ||
                    (ret = ser_saveOctetString2(serializeSettings, it->name, MAX_USER_NAME_LENGTH)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex3(&object->userList, pos, (void**)&it, 0)) != 0 ||
                    (ret = ser_saveUInt8(serializeSettings, it->key)) != 0 ||
                    (ret = ser_saveOctetString2(serializeSettings, it->value, (uint16_t)((gxByteBuffer*)it->value)->size)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
#ifdef DLMS_IGNORE_MALLOC
            object->userList.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int ser_savePppSetup(
    gxSerializerSettings* serializeSettings,
    gxPppSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#ifndef DLMS_IGNORE_OBJECT_POINTER1S
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_set(serializeSettings, obj_getLogicalName(object->phy), 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0))
    {
    }
#else
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_set(&ba, object->PHYReference, 6)) != 0))
    {
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        if ((ret = ser_saveOctetString(serializeSettings, &object->userName)) != 0 ||
            (ret = ser_saveOctetString(serializeSettings, &object->password)) != 0 ||
            (ret = ser_saveUInt8(serializeSettings, object->authentication)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int ser_saveProfileGeneric(
    gxSerializerSettings* serializeSettings,
    gxProfileGeneric* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 3) && (ret = saveObjectsInternal(serializeSettings, &object->captureObjects)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt32(serializeSettings, object->capturePeriod)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->sortMethod)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_set(serializeSettings, obj_getLogicalName(object->sortObject), 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt32(serializeSettings, object->entriesInUse)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt32(serializeSettings, object->profileEntries)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int ser_saveAccount(
    gxSerializerSettings* serializeSettings,
    gxAccount* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_saveUInt8(serializeSettings, object->paymentMode)) == 0)
        {
            ret = ser_saveUInt8(serializeSettings, object->accountStatus);
        }
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->currentCreditInUse)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->currentCreditStatus)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveInt32(serializeSettings, object->availableCredit)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveInt32(serializeSettings, object->amountToClear)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveInt32(serializeSettings, object->clearanceThreshold)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveInt32(serializeSettings, object->aggregatedDebt)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 13) && (ret = ser_saveDateTime(&object->accountActivationTime, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 14) && (ret = ser_saveDateTime(&object->accountClosureTime, serializeSettings)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 16) && (ret = ser_saveInt32(serializeSettings, object->lowCreditThreshold)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 17) && (ret = ser_saveInt32(serializeSettings, object->nextCreditAvailableThreshold)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 18) && (ret = ser_saveUInt16(serializeSettings, object->maxProvision)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 19) && (ret = ser_saveInt32(serializeSettings, object->maxProvisionPeriod)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int ser_saveCredit(
    gxSerializerSettings* serializeSettings,
    gxCredit* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveInt32(serializeSettings, object->currentCreditAmount)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->type)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->priority)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveInt32(serializeSettings, object->warningThreshold)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveInt32(serializeSettings, object->limit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveUInt8(serializeSettings, object->creditConfiguration)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt8(serializeSettings, object->status)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveInt32(serializeSettings, object->presetCreditAmount)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_saveInt32(serializeSettings, object->creditAvailableThreshold)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 11) && (ret = ser_saveDateTime(&object->period, serializeSettings)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE

int ser_saveUnitCharge(gxSerializerSettings* serializeSettings, gxUnitCharge* target)
{
    uint16_t pos, count;
    int ret = 0;
    gxChargeTable* it;
    if (//commodity scale
        (ret = ser_saveUInt8(serializeSettings, target->chargePerUnitScaling.commodityScale)) != 0 ||
        //price scale
        (ret = ser_saveUInt8(serializeSettings, target->chargePerUnitScaling.priceScale)) != 0 ||
        //-------------
        //commodity
        //type
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = ser_saveUInt16(serializeSettings, target->commodity.target == NULL ? 0 : target->commodity.target->objectType)) != 0 ||
#else
        (ret = ser_saveUInt16(serializeSettings, target->commodity.type)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //logicalName
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = ser_setOctetString2(serializeSettings, obj_getLogicalName(target->commodity.target), 6, 6)) != 0 ||
#else
        (ret = ser_setOctetString2(serializeSettings, target->commodity.logicalName, 6, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //attributeIndex
        (ret = ser_saveUInt8(serializeSettings, target->commodity.attributeIndex)) != 0 ||
        //-------------
        //chargeTables
        (ret = ser_saveArrayCount(serializeSettings, &target->chargeTables, &count)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != target->chargeTables.size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex3(&target->chargeTables, pos, (void**)&it, sizeof(gxChargeTable), 0)) != 0 ||
            //index
            (ret = ser_setOctetString2(serializeSettings, it->index.data, it->index.size, sizeof(it->index.data))) != 0 ||
            //chargePerUnit
            (ret = ser_saveInt16(serializeSettings, it != NULL ? it->chargePerUnit : 0)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex3(&target->chargeTables, pos, (void**)&it, 0)) != 0 ||
            //index
            (ret = ser_setOctetString2(serializeSettings, it->index.data, (uint16_t)it->index.size, (uint16_t)it->index.capacity)) != 0 ||
            //chargePerUnit
            (ret = ser_saveInt16(serializeSettings, it->chargePerUnit)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
    }
#ifdef DLMS_IGNORE_MALLOC
    target->chargeTables.size = count;
#endif //DLMS_IGNORE_MALLOC
    return ret;
}


int ser_saveCharge(
    gxSerializerSettings* serializeSettings,
    gxCharge* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveInt32(serializeSettings, object->totalAmountPaid)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->chargeType)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->priority)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUnitCharge(serializeSettings, &object->unitChargeActive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUnitCharge(serializeSettings, &object->unitChargePassive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_saveDateTime(&object->unitChargeActivationTime, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_saveUInt32(serializeSettings, object->period)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_saveUInt8(serializeSettings, object->chargeConfiguration)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_saveDateTime(&object->lastCollectionTime, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 11) && (ret = ser_saveUInt32(serializeSettings, object->lastCollectionAmount)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 12) && (ret = ser_saveUInt32(serializeSettings, object->totalAmountRemaining)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 13) && (ret = ser_saveUInt16(serializeSettings, object->proportion)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int ser_saveTokenGateway(
    gxSerializerSettings* serializeSettings,
    gxTokenGateway* object)
{
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int ser_saveGsmDiagnostic(
    gxSerializerSettings* serializeSettings,
    gxGsmDiagnostic* object)
{
    uint16_t pos, count;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxAdjacentCell* it;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = ser_saveOctetString(serializeSettings, &object->operatorName);
#else
        ret = ser_saveOctetString2(serializeSettings, object->operatorName, (uint16_t)strlen(object->operatorName));
#endif //DLMS_IGNORE_MALLOC
    }
    if ((!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt8(serializeSettings, object->status)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->circuitSwitchStatus)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveUInt8(serializeSettings, object->packetSwitchStatus)) != 0))
    {
    }

    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 6))
    {
        if ((ret = ser_saveUInt32(serializeSettings, object->cellInfo.cellId)) != 0 ||
            (ret = ser_saveUInt16(serializeSettings, object->cellInfo.locationId)) != 0 ||
            (ret = ser_saveUInt8(serializeSettings, object->cellInfo.signalQuality)) != 0 ||
            (ret = ser_saveUInt8(serializeSettings, object->cellInfo.ber)) != 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7) &&
        (ret = ser_saveArrayCount(serializeSettings, &object->adjacentCells, &count)) == 0)
    {
        for (pos = 0; pos != object->adjacentCells.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(&object->adjacentCells, pos, (void**)&it, sizeof(gxAdjacentCell), 0)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex3(&object->adjacentCells, pos, (void**)&it, 0)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_saveUInt32(serializeSettings, it != NULL ? it->cellId : 0)) != 0 ||
                (ret = ser_saveUInt8(serializeSettings, it != NULL ? it->signalQuality : 0)) != 0)
            {
                break;
            }
        }
#ifdef DLMS_IGNORE_MALLOC
        object->adjacentCells.size = count;
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8))
    {
        ret = ser_saveDateTime(&object->captureTime, serializeSettings);
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
int ser_saveCompactData(
    gxSerializerSettings* serializeSettings,
    gxCompactData* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveOctetString(serializeSettings, &object->buffer)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = saveObjectsInternal(serializeSettings, &object->captureObjects)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_saveUInt8(serializeSettings, object->templateId)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_saveOctetString(serializeSettings, &object->templateDescription)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_saveUInt8(serializeSettings, object->captureMethod)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int ser_saveLlcSscsSetup(
    gxSerializerSettings* serializeSettings,
    gxLlcSscsSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_saveUInt16(serializeSettings, object->serviceNodeAddress)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_saveUInt16(serializeSettings, object->baseNodeAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int ser_savePrimeNbOfdmPlcPhysicalLayerCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcPhysicalLayerCounters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int ser_savePrimeNbOfdmPlcMacSetup(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacSetup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int ser_savePrimeNbOfdmPlcMacFunctionalParameters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacFunctionalParameters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int ser_savePrimeNbOfdmPlcMacCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacCounters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int ser_savePrimeNbOfdmPlcMacNetworkAdministrationData(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int ser_savePrimeNbOfdmPlcApplicationsIdentification(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcApplicationsIdentification* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
int ser_saveArbitrator(
    gxSerializerSettings* serializeSettings,
    gxArbitrator* object)
{
    return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int ser_saveIec8802LlcType1Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType1Setup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int ser_saveIec8802LlcType2Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType2Setup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int ser_saveIec8802LlcType3Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType3Setup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int ser_saveSFSKActiveInitiator(
    gxSerializerSettings* serializeSettings,
    gxSFSKActiveInitiator* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int ser_saveFSKMacCounters(
    gxSerializerSettings* serializeSettings,
    gxFSKMacCounters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int ser_saveSFSKMacSynchronizationTimeouts(
    gxSerializerSettings* serializeSettings,
    gxSFSKMacSynchronizationTimeouts* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int ser_saveSFSKPhyMacSetUp(
    gxSerializerSettings* serializeSettings,
    gxSFSKPhyMacSetUp* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int ser_saveSFSKReportingSystemList(
    gxSerializerSettings* serializeSettings,
    gxSFSKReportingSystemList* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifndef DLMS_IGNORE_SCHEDULE
int ser_saveSchedule(
    gxSerializerSettings* serializeSettings,
    gxSchedule* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SCHEDULE

#ifdef DLMS_ITALIAN_STANDARD
int ser_saveTariffPlan(
    gxSerializerSettings* serializeSettings,
    gxTariffPlan* object)
{
    int ret;
    if ((ret = bb_addString(serializeSettings, object->calendarName)) != 0 ||
        (ret = ser_saveUInt8(serializeSettings, object->enabled)) != 0 ||
        (ret = ser_saveDateTime(&object->activationTime, serializeSettings)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_ITALIAN_STANDARD

void ser_init(gxSerializerSettings* settings)
{
    memset(settings, 0, sizeof(gxSerializerSettings));
}

int ser_saveObject(
    gxSerializerSettings* serializeSettings,
    gxObject* object)
{
    int ret = 0;
    switch (object->objectType)
    {
#ifndef DLMS_IGNORE_DATA
    case DLMS_OBJECT_TYPE_DATA:
        ret = ser_saveData(serializeSettings, (gxData*)object);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = ser_saveRegister(serializeSettings, (gxRegister*)object);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = ser_saveClock(serializeSettings, (gxClock*)object);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = ser_saveActionSchedule(serializeSettings, (gxActionSchedule*)object);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = ser_saveActivityCalendar(serializeSettings, (gxActivityCalendar*)object);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = ser_saveAssociationLogicalName(serializeSettings, (gxAssociationLogicalName*)object);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = ser_saveAssociationShortName(serializeSettings, (gxAssociationShortName*)object);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = ser_saveAutoAnswer(serializeSettings, (gxAutoAnswer*)object);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = ser_saveAutoConnect(serializeSettings, (gxAutoConnect*)object);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = ser_saveDemandRegister(serializeSettings, (gxDemandRegister*)object);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = ser_saveMacAddressSetup(serializeSettings, (gxMacAddressSetup*)object);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = ser_saveExtendedRegister(serializeSettings, (gxExtendedRegister*)object);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
#ifndef DLMS_IGNORE_GPRS_SETUP
        ret = ser_saveGPRSSetup(serializeSettings, (gxGPRSSetup*)object);
#endif //DLMS_IGNORE_GPRS_SETUP
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = ser_saveSecuritySetup(serializeSettings, (gxSecuritySetup*)object);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = ser_saveHdlcSetup(serializeSettings, (gxIecHdlcSetup*)object);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = ser_saveLocalPortSetup(serializeSettings, (gxLocalPortSetup*)object);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = ser_saveIecTwistedPairSetup(serializeSettings, (gxIecTwistedPairSetup*)object);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = ser_saveIp4Setup(serializeSettings, (gxIp4Setup*)object);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = ser_saveIp6Setup(serializeSettings, (gxIp6Setup*)object);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = ser_saveMbusSlavePortSetup(serializeSettings, (gxMbusSlavePortSetup*)object);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = ser_saveImageTransfer(serializeSettings, (gxImageTransfer*)object);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = ser_saveDisconnectControl(serializeSettings, (gxDisconnectControl*)object);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = ser_saveLimiter(serializeSettings, (gxLimiter*)object);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = ser_saveMBusClient(serializeSettings, (gxMBusClient*)object);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = ser_saveModemConfiguration(serializeSettings, (gxModemConfiguration*)object);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = ser_savePppSetup(serializeSettings, (gxPppSetup*)object);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = ser_saveProfileGeneric(serializeSettings, (gxProfileGeneric*)object);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = ser_saveRegisterActivation(serializeSettings, (gxRegisterActivation*)object);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = ser_saveRegisterMonitor(serializeSettings, (gxRegisterMonitor*)object);
        break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_REGISTER_TABLE
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_REGISTER_TABLE
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = ser_saveSapAssignment(serializeSettings, (gxSapAssignment*)object);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = ser_saveSchedule(serializeSettings, (gxSchedule*)object);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = ser_saveScriptTable(serializeSettings, (gxScriptTable*)object);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SMTP_SETUP
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_SMTP_SETUP
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = ser_saveSpecialDaysTable(serializeSettings, (gxSpecialDaysTable*)object);
        break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_STATUS_MAPPING
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_STATUS_MAPPING
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = ser_saveTcpUdpSetup(serializeSettings, (gxTcpUdpSetup*)object);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = ser_saveUtilityTables(serializeSettings, (gxUtilityTables*)object);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = ser_saveMBusMasterPortSetup(serializeSettings, (gxMBusMasterPortSetup*)object);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = ser_savePushSetup(serializeSettings, (gxPushSetup*)object);
        break;
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_DATA_PROTECTION
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_DATA_PROTECTION
#ifndef DLMS_IGNORE_ACCOUNT
    case DLMS_OBJECT_TYPE_ACCOUNT:
        ret = ser_saveAccount(serializeSettings, (gxAccount*)object);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = ser_saveCredit(serializeSettings, (gxCredit*)object);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = ser_saveCharge(serializeSettings, (gxCharge*)object);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = ser_saveTokenGateway(serializeSettings, (gxTokenGateway*)object);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = ser_saveGsmDiagnostic(serializeSettings, (gxGsmDiagnostic*)object);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = ser_saveCompactData(serializeSettings, (gxCompactData*)object);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = ser_saveLlcSscsSetup(serializeSettings, (gxLlcSscsSetup*)object);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = ser_savePrimeNbOfdmPlcPhysicalLayerCounters(serializeSettings, (gxPrimeNbOfdmPlcPhysicalLayerCounters*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = ser_savePrimeNbOfdmPlcMacSetup(serializeSettings, (gxPrimeNbOfdmPlcMacSetup*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = ser_savePrimeNbOfdmPlcMacFunctionalParameters(serializeSettings, (gxPrimeNbOfdmPlcMacFunctionalParameters*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = ser_savePrimeNbOfdmPlcMacCounters(serializeSettings, (gxPrimeNbOfdmPlcMacCounters*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = ser_savePrimeNbOfdmPlcMacNetworkAdministrationData(serializeSettings, (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = ser_savePrimeNbOfdmPlcApplicationsIdentification(serializeSettings, (gxPrimeNbOfdmPlcApplicationsIdentification*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = ser_saveArbitrator(serializeSettings, (gxArbitrator*)object);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = ser_saveIec8802LlcType1Setup(serializeSettings, (gxIec8802LlcType1Setup*)object);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ser_saveIec8802LlcType2Setup(serializeSettings, (gxIec8802LlcType2Setup*)object);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ser_saveIec8802LlcType3Setup(serializeSettings, (gxIec8802LlcType3Setup*)object);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ser_saveSFSKActiveInitiator(serializeSettings, (gxSFSKActiveInitiator*)object);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ser_saveFSKMacCounters(serializeSettings, (gxFSKMacCounters*)object);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ser_saveSFSKMacSynchronizationTimeouts(serializeSettings, (gxSFSKMacSynchronizationTimeouts*)object);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ser_saveSFSKPhyMacSetUp(serializeSettings, (gxSFSKPhyMacSetUp*)object);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ser_saveSFSKReportingSystemList(serializeSettings, (gxSFSKReportingSystemList*)object);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = ser_saveTariffPlan(serializeSettings, (gxTariffPlan*)object);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default: //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
void ResetPosition(gxSerializerSettings* serializeSettings)
{
#ifdef DLMS_IGNORE_MALLOC
    serializeSettings->currentObject = NULL;
    serializeSettings->currentIndex = 0;
#endif //DLMS_IGNORE_MALLOC
    serializeSettings->position = 0;
    serializeSettings->updateEnd = 0;
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) && !defined(GX_DLMS_MICROCONTROLLER)
    serializeSettings->updateStart = 0xFFFFFFFF;
#else
    serializeSettings->updateStart = 0xFFFF;
#endif //defined(GX_DLMS_BYTE_BUFFER_SIZE_32) && !defined(GX_DLMS_MICROCONTROLLER)
}
#endif //!(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))

//Serialize objects to bytebuffer.
int ser_saveObjects(
    gxSerializerSettings* serializeSettings,
    gxObject** objects,
    uint16_t count)
{
    uint16_t pos;
    int ret = 0;
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    ResetPosition(serializeSettings);
#endif //!(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))

#ifdef DLMS_IGNORE_MALLOC
    if (serializeSettings->savedAttributes == 0)
    {
        serializeSettings->savedAttributes = 0xFFFF;
    }
#endif //DLMS_IGNORE_MALLOC
    unsigned char ver = 0;
    uint32_t dataSize = 0;
    //Serializer version number only if it's changed.
    ret = ser_loadUInt8(serializeSettings, &ver);
    //If version has changed.
    if (ret != 0 || ver != SERIALIZATION_VERSION)
    {
        if (ret == 0)
        {
            ret = ser_seek(serializeSettings, -1);
        }
        if ((ret = ser_saveUInt8(serializeSettings, SERIALIZATION_VERSION)) != 0 ||
            //Data size.
            (ret = ser_saveUInt32(serializeSettings, 0)) != 0)
        {
            return ret;
        }
#ifdef DLMS_IGNORE_MALLOC
        //All objects are saved on the first time.
        serializeSettings->savedAttributes = 0xFFFF;
        serializeSettings->savedObject = NULL;
#endif //DLMS_IGNORE_MALLOC
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) && !defined(GX_DLMS_MICROCONTROLLER)
        serializeSettings->updateStart = 0xFFFFFFFF;
#else
        serializeSettings->updateStart = 0xFFFF;
#endif //defined(GX_DLMS_BYTE_BUFFER_SIZE_32) && !defined(GX_DLMS_MICROCONTROLLER)
#endif //!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    }
    else
    {
        ret = ser_loadUInt32(serializeSettings, &dataSize);
    }
    if (ret == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            serializeSettings->currentObject = objects[pos];
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_saveObject(serializeSettings, objects[pos])) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
        }
    }
#ifdef DLMS_IGNORE_MALLOC
    serializeSettings->currentObject = NULL;
    serializeSettings->currentIndex = 0;
#endif //DLMS_IGNORE_MALLOC
    if (ret == 0)
    {
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        int index = ftell(serializeSettings->stream);
        uint32_t size = (uint32_t)(index - 5);
#else
        int index = serializeSettings->position;
        uint32_t size = (uint16_t)serializeSettings->position - 5;
#endif //#!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        if (dataSize == 0 || dataSize != size)
        {
            --index;
            ret = ser_seek(serializeSettings, -index);
            //Data size.
            ret = ser_saveUInt32(serializeSettings, size);
            //Seek to end.
            ret = ser_seek(serializeSettings, size);
        }
    }
    return ret;
}

int ser_saveObjects2(
    gxSerializerSettings* serializeSettings,
    objectArray* objects)
{
    uint16_t pos;
    int ret;
    gxObject* obj;
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    ResetPosition(serializeSettings);
#endif //!(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
#ifdef DLMS_IGNORE_MALLOC
    if (serializeSettings->savedAttributes == 0)
    {
        serializeSettings->savedAttributes = 0xFFFF;
    }
#endif //DLMS_IGNORE_MALLOC
    unsigned char ver = 0;
    //Serializer version number only if it's changed.
    ret = ser_loadUInt8(serializeSettings, &ver);
    uint32_t dataSize = 0;
    //If version has changed.
    if (ret != 0 || ver != SERIALIZATION_VERSION)
    {
        if (ret == 0)
        {
            ret = ser_seek(serializeSettings, -1);
        }
        if ((ret = ser_saveUInt8(serializeSettings, SERIALIZATION_VERSION)) != 0 ||
            //Data size.
            (ret = ser_saveUInt32(serializeSettings, 0)) != 0)
        {
            return ret;
        }
#ifdef DLMS_IGNORE_MALLOC
        //All objects are saved on the first time.
        serializeSettings->savedAttributes = 0xFFFF;
        serializeSettings->savedObject = NULL;
#endif //DLMS_IGNORE_MALLOC
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) && !defined(GX_DLMS_MICROCONTROLLER)
        serializeSettings->updateStart = 0xFFFFFFFF;
#else
        serializeSettings->updateStart = 0xFFFF;
#endif //defined(GX_DLMS_BYTE_BUFFER_SIZE_32) && !defined(GX_DLMS_MICROCONTROLLER)
#endif //!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    }
    else
    {
        ret = ser_loadUInt32(serializeSettings, &dataSize);
    }
    if (ret == 0)
    {
        for (pos = 0; pos != objects->size; ++pos)
        {
            if ((ret = oa_getByIndex(objects, pos, &obj)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
#ifdef DLMS_IGNORE_MALLOC
            serializeSettings->currentObject = obj;
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_saveObject(serializeSettings, obj)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
        }
    }

#ifdef DLMS_IGNORE_MALLOC
    serializeSettings->currentObject = NULL;
    serializeSettings->currentIndex = 0;
#endif //DLMS_IGNORE_MALLOC
    if (ret == 0)
    {
#if !defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        int index = ftell(serializeSettings->stream);
        uint32_t size = (uint32_t)(index - 5);
#else
        int index = serializeSettings->position;
        uint32_t size = (uint16_t)serializeSettings->position - 5;
#endif //#!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
        if (dataSize == 0 || dataSize != size)
        {
            --index;
            ret = ser_seek(serializeSettings, -index);
            //Data size.
            ret = ser_saveUInt32(serializeSettings, size);
            //Seek to end.
            ret = ser_seek(serializeSettings, size);
        }
    }
    return ret;
}

// --------------------------------------------------------------------------
//Load functionality.

// Get item from the buffer if DLMS_IGNORE_MALLOC is defined.
// Otherwice create the object dynamically.
int ser_getArrayItem(gxArray* arr, uint16_t index, void** value, uint16_t itemSize)
{
#ifdef DLMS_IGNORE_MALLOC
    return arr_getByIndex4(arr, index, value, itemSize, 0);
#else
    void* it = gxcalloc(1, itemSize);
    if (it == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = it;
    return arr_push(arr, it);
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int ser_loadArray(gxSerializerSettings* serializeSettings, gxArray* arr, uint16_t* count)
{
    int ret = ser_loadObjectCount(serializeSettings, count);
    if (ret == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        arr->size = *count;
        ret = ser_loadObjectCount(serializeSettings, count);
        if (arr_getCapacity(arr) != *count)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#else
        arr_clear(arr);
        arr_capacity(arr, *count);
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int ser_verifyObjectArray(gxSerializerSettings* serializeSettings, objectArray* arr, uint16_t* count)
{
    oa_empty(arr);
    int ret = ser_loadObjectCount(serializeSettings, count);
    if (ret == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t capacity;
        ret = ser_loadObjectCount(serializeSettings, &capacity);
        arr->size = capacity;
#else
        ret = oa_capacity(arr, *count);
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

#ifndef DLMS_IGNORE_DATA
int ser_loadData(gxSerializerSettings* serializeSettings, gxData* object)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        return ser_loadVariant(&object->value, serializeSettings);
    }
    return 0;
}

#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int ser_loadRegister(gxSerializerSettings* serializeSettings, gxRegister* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadInt8(serializeSettings, &object->scaler)) != 0 ||
            (ret = ser_loadUInt8(serializeSettings, &object->unit)) != 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_loadVariant(&object->value, serializeSettings);
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
int ser_loadClock(gxSerializerSettings* serializeSettings, gxClock* object)
{
    int ret = 0;
    unsigned char status = 0, clockBase = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!(!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadDateTime(&object->time, serializeSettings, DLMS_DATA_TYPE_DATETIME)) == 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadInt16(serializeSettings, &object->timeZone)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt8(serializeSettings, &status)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadDateTime(&object->begin, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadDateTime(&object->end, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadInt8(serializeSettings, &object->deviation)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadUInt8(serializeSettings, &object->enabled)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_loadUInt8(serializeSettings, &clockBase)) != 0))
    {
        object->status = status;
        object->clockBase = clockBase;
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int ser_loadScriptTable(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxScriptTable* object)
{
    int ret = 0, pos, pos2;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxScript* s;
    gxScriptAction* sa;
    uint16_t count, count2;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        obj_clearScriptTable(&object->scripts);
        if ((ret = ser_loadArray(serializeSettings, &object->scripts, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->scripts, pos, (void**)&s, sizeof(gxScript))) != 0)
                {
                    break;
                }
                if ((ret = ser_loadUInt16(serializeSettings, &s->id)) != 0)
                {
                    break;
                }
                if ((ret = ser_loadArray(serializeSettings, &s->actions, &count2)) == 0)
                {
                    for (pos2 = 0; pos2 != count2; ++pos2)
                    {
                        if ((ret = ser_getArrayItem(&s->actions, pos2, (void**)&sa, sizeof(gxScriptAction))) != 0)
                        {
                            break;
                        }
                        if ((ret = ser_loadUInt8(serializeSettings, (unsigned char*)&sa->type)) != 0)
                        {
                            break;
                        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        uint16_t ot;
                        unsigned char ln[6];
                        if ((ret = ser_loadUInt16(serializeSettings, &ot)) != 0 ||
                            (ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                                , 6
#endif //DLMS_IGNORE_MALLOC
                            )) != 0)
                        {
                            break;
                        }
                        if (pos2 < count2)
                        {
                            sa->target = NULL;
                            if ((ret = cosem_findObjectByLN(settings, ot, ln, &sa->target)) != 0)
                            {
                                break;
                            }
                            if (sa->target == NULL)
                            {
                                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                                break;
                            }
                        }
#else
                        if ((ret = ser_loadUInt16(serializeSettings, sa->objectType)) != 0 ||
                            (ret = ser_get(serializeSettings, sa->logicalName, sizeof(sa->logicalName))) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        if ((ret = ser_loadInt8(serializeSettings, &sa->index)) != 0 ||
                            (ret = ser_loadVariant(&sa->parameter, serializeSettings)) != 0)
                        {
                            break;
                        }
                    }
                }
                if (ret != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int ser_loadSpecialDaysTable(
    gxSerializerSettings* serializeSettings,
    gxSpecialDaysTable* object)
{
    int ret = 0, pos;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxSpecialDay* sd;
    uint16_t count;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        arr_clear(&object->entries);
        if ((ret = ser_loadArray(serializeSettings, &object->entries, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->entries, pos, (void**)&sd, sizeof(gxSpecialDay))) != 0)
                {
                    break;
                }
                if ((ret = ser_loadUInt16(serializeSettings, &sd->index)) != 0 ||
                    (ret = ser_loadDateTime(&sd->date, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0 ||
                    (ret = ser_loadUInt8(serializeSettings, &sd->dayId)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int ser_loadTcpUdpSetup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxTcpUdpSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char ln[6];
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_loadUInt16(serializeSettings, &object->port);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->ipSetup)) != 0)
            {
                return ret;
            }
#else
            if (ret = ser_get(&ba, object->ipReference, 6)) != 0 ||
            {
                return ret;
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt16(serializeSettings, &object->maximumSegmentSize)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, &object->maximumSimultaneousConnections)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt16(serializeSettings, &object->inactivityTimeout)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int ser_loadMBusMasterPortSetup(
    gxSerializerSettings* serializeSettings,
    gxMBusMasterPortSetup* object)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        return ser_loadUInt8(serializeSettings, (unsigned char*)&object->commSpeed);
    }
    return 0;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

int ser_loadObjectsInternal(
    dlmsSettings* settings,
    gxSerializerSettings* serializeSettings,
    gxArray* objects)
{
    uint16_t pos;
    int ret;
    uint16_t count;
    uint16_t ot;
    unsigned char ln[6];
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
#else
    gxObject* key;
    gxTarget* value;
#endif //DLMS_IGNORE_MALLOC
    obj_clearProfileGenericCaptureObjects(objects);
    if ((ret = ser_loadArray(serializeSettings, objects, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            if ((ret = ser_getArrayItem(objects, pos, (void**)&it, sizeof(gxTarget))) != 0)
            {
                break;
            }
            if ((ret = ser_loadUInt16(serializeSettings, &ot)) != 0 ||
                (ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                    , 6
#endif //DLMS_IGNORE_MALLOC
                )) != 0 ||
                (ret = ser_loadInt8(serializeSettings, &it->attributeIndex)) != 0 ||
                (ret = ser_loadUInt16(serializeSettings, &it->dataIndex)) != 0 ||
                (ret = cosem_findObjectByLN(settings, ot, ln, &it->target)) != 0)
            {
                break;
            }

#else
            value = (gxTarget*)gxmalloc(sizeof(gxTarget));
            if (value == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = ser_loadUInt16(serializeSettings, &ot)) != 0 ||
                (ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                    , 6
#endif //DLMS_IGNORE_MALLOC
                )) != 0 ||
                (ret = cosem_findObjectByLN(settings, ot, ln, &key)) != 0 ||
                (ret = ser_loadInt8(serializeSettings, &value->attributeIndex)) != 0 ||
                (ret = ser_loadUInt16(serializeSettings, &value->dataIndex)) != 0)
            {
                gxfree(value);
                break;
            }
            if ((ret = arr_push(objects, key_init(key, value))) != 0)
            {
                break;
            }
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
        }
    }
    return ret;
}

int loadTimeWindow(
    gxSerializerSettings* serializeSettings,
    gxArray* arr)
{
    int ret;
    uint16_t pos;
    gxtime* start, * end;
    uint16_t count;
#ifdef DLMS_IGNORE_MALLOC
    gxTimePair* k;
#endif //DLMS_IGNORE_MALLOC
    arr_clearKeyValuePair(arr);
    if ((ret = ser_loadArray(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex3(arr, pos, (void**)&k, sizeof(gxTimePair), 0)) != 0)
            {
                break;
            }
            start = &k->first;
            end = &k->second;
#else
            start = (gxtime*)gxmalloc(sizeof(gxtime));
            if (start == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            end = (gxtime*)gxmalloc(sizeof(gxtime));
            if (end == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = arr_push(arr, key_init(start, end))) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_loadDateTime(start, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0 ||
                (ret = ser_loadDateTime(end, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP
int ser_loadPushSetup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxPushSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_loadObjectsInternal(settings, serializeSettings, &object->pushObjectList);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->service)) != 0 ||
            (ret = ser_loadOctetString(serializeSettings, &object->destination)) != 0 ||
            (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->message)) != 0)
        {
        }
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 4) && (ret = loadTimeWindow(serializeSettings, &object->communicationWindow)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt16(serializeSettings, &object->randomisationStartInterval)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt8(serializeSettings, &object->numberOfRetries)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadUInt16(serializeSettings, &object->repetitionDelay)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_AUTO_CONNECT
int ser_loadAutoConnect(
    gxSerializerSettings* serializeSettings,
    gxAutoConnect* object)
{
    uint16_t count;
#ifdef DLMS_IGNORE_MALLOC
    gxDestination* dest;
#else
    gxByteBuffer* dest;
#endif //DLMS_IGNORE_MALLOC
    int pos, ret = 0;
    unsigned char ch;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->mode = (DLMS_AUTO_CONNECT_MODE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        ret = ser_loadUInt8(serializeSettings, &object->repetitions);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        ret = ser_loadUInt16(serializeSettings, &object->repetitionDelay);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        ret = loadTimeWindow(serializeSettings, &object->callingWindow);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 6))
    {
        arr_clearStrings(&object->destinations);
        if (ret == 0 && (ret = ser_loadArray(serializeSettings, &object->destinations, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->destinations, pos, (void**)&dest, sizeof(gxDestination), 0)) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString3(serializeSettings, dest->value, &dest->size)) != 0)
                {
                    break;
                }
#else
                dest = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (dest == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                BYTE_BUFFER_INIT(dest);
                if ((ret = arr_push(&object->destinations, dest)) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString(serializeSettings, dest)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int ser_loadSeasonProfile(
    gxArray* arr,
    gxSerializerSettings* serializeSettings)
{
    gxSeasonProfile* it;
    int pos, ret;
    uint16_t count;
    if ((ret = obj_clearSeasonProfile(arr)) == 0 &&
        (ret = ser_loadArray(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(arr, pos, (void**)&it, sizeof(gxSeasonProfile))) != 0)
            {
                break;
            }
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = ser_loadOctetString3(serializeSettings, it->name.value, &it->name.size)) != 0 ||
                (ret = ser_loadDateTime(&it->start, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0 ||
                (ret = ser_loadOctetString3(serializeSettings, it->weekName.value, &it->weekName.size)) != 0)
            {
                break;
            }
#else
            if ((ret = ser_loadOctetString(serializeSettings, &it->name)) != 0 ||
                (ret = ser_loadDateTime(&it->start, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0 ||
                (ret = ser_loadOctetString(serializeSettings, &it->weekName)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}

int ser_loadweekProfile(
    gxArray* arr,
    gxSerializerSettings* serializeSettings)
{
    gxWeekProfile* it;
    int pos, ret;
    uint16_t count;
    if ((ret = obj_clearWeekProfileTable(arr)) == 0 &&
        (ret = ser_loadArray(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(arr, pos, (void**)&it, sizeof(gxWeekProfile))) != 0)
            {
                break;
            }
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = ser_loadOctetString3(serializeSettings, it->name.value, &it->name.size)) != 0)
            {
                break;
            }
#else
            if ((ret = ser_loadOctetString(serializeSettings, &it->name)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_loadUInt8(serializeSettings, &it->monday)) != 0 ||
                (ret = ser_loadUInt8(serializeSettings, &it->tuesday)) != 0 ||
                (ret = ser_loadUInt8(serializeSettings, &it->wednesday)) != 0 ||
                (ret = ser_loadUInt8(serializeSettings, &it->thursday)) != 0 ||
                (ret = ser_loadUInt8(serializeSettings, &it->friday)) != 0 ||
                (ret = ser_loadUInt8(serializeSettings, &it->saturday)) != 0 ||
                (ret = ser_loadUInt8(serializeSettings, &it->sunday)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int ser_loadDayProfile(
    dlmsSettings* settings,
    gxArray* arr,
    gxSerializerSettings* serializeSettings)
{
    gxDayProfile* dp;
    gxDayProfileAction* it;
    int pos, pos2, ret;
    uint16_t count, count2;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = obj_clearDayProfileTable(arr)) == 0 &&
        (ret = ser_loadArray(serializeSettings, arr, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(arr, pos, (void**)&dp, sizeof(gxDayProfile))) != 0)
            {
                break;
            }
            if ((ret = ser_loadUInt8(serializeSettings, &dp->dayId)) != 0 ||
                (ret = ser_loadArray(serializeSettings, &dp->daySchedules, &count2)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != count2; ++pos2)
            {
                if ((ret = ser_getArrayItem(&dp->daySchedules, pos2, (void**)&it, sizeof(gxDayProfileAction))) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                    , 6
#endif //DLMS_IGNORE_MALLOC
                )) != 0)
                {
                    break;
                }
                if (pos2 < count2)
                {
                    if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &it->script)) != 0)
                    {
                        break;
                    }
                }
                else
                {
                    it->script = NULL;
                }
#else
                if ((ret = ser_get(serializeSettings, it->scriptLogicalName, 6)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = ser_loadUInt16(serializeSettings, &it->scriptSelector)) != 0 ||
                    (ret = ser_loadDateTime(&it->startTime, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}

int ser_loadActivityCalendar(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxActivityCalendar* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadOctetString(serializeSettings, &object->calendarNameActive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadSeasonProfile(&object->seasonProfileActive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadweekProfile(&object->weekProfileTableActive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadDayProfile(settings, &object->dayProfileTableActive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadOctetString(serializeSettings, &object->calendarNamePassive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadSeasonProfile(&object->seasonProfilePassive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadweekProfile(&object->weekProfileTablePassive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_loadDayProfile(settings, &object->dayProfileTablePassive, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_loadDateTime(&object->time, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_SECURITY_SETUP
int ser_loadSecuritySetup(
    gxSerializerSettings* serializeSettings,
    gxSecuritySetup* object)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char v;
    int ret = 0;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &v)) == 0)
        {
            object->securityPolicy = (DLMS_SECURITY_POLICY)v;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &v)) == 0)
        {
            object->securitySuite = (DLMS_SECURITY_SUITE)v;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        if ((ret = bb_clear(&object->serverSystemTitle)) == 0)
        {
            ret = ser_loadOctetString(serializeSettings, &object->serverSystemTitle);
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        if ((ret = bb_clear(&object->clientSystemTitle)) == 0)
        {
            ret = ser_loadOctetString(serializeSettings, &object->clientSystemTitle);
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int ser_loadHdlcSetup(
    gxSerializerSettings* serializeSettings,
    gxIecHdlcSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->communicationSpeed)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadUInt8(serializeSettings, &object->windowSizeTransmit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt8(serializeSettings, &object->windowSizeReceive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt16(serializeSettings, &object->maximumInfoLengthTransmit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt16(serializeSettings, &object->maximumInfoLengthReceive)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadUInt16(serializeSettings, &object->interCharachterTimeout)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadUInt16(serializeSettings, &object->inactivityTimeout)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_loadUInt16(serializeSettings, &object->deviceAddress)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int ser_loadLocalPortSetup(
    gxSerializerSettings* serializeSettings,
    gxLocalPortSetup* object)
{
    int ret = 0;
    unsigned char ch;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->defaultMode = (DLMS_OPTICAL_PROTOCOL_MODE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->defaultBaudrate = (DLMS_BAUD_RATE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->proposedBaudrate = (DLMS_BAUD_RATE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->responseTime = (DLMS_LOCAL_PORT_RESPONSE_TIME)ch;
        }
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadOctetString(serializeSettings, &object->deviceAddress)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadOctetString(serializeSettings, &object->password1)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadOctetString(serializeSettings, &object->password2)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_loadOctetString(serializeSettings, &object->password5)) != 0))
        {

        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int ser_loadIecTwistedPairSetup(
    gxSerializerSettings* serializeSettings,
    gxIecTwistedPairSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->mode)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->speed)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadOctetString(serializeSettings, &object->primaryAddresses)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadOctetString(serializeSettings, &object->tabis)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int ser_loadDemandRegister(
    gxSerializerSettings* serializeSettings,
    gxDemandRegister* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadVariant(&object->currentAverageValue, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadVariant(&object->lastAverageValue, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadInt8(serializeSettings, &object->scaler)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, &object->unit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadVariant(&object->status, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadDateTime(&object->captureTime, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadDateTime(&object->startTimeCurrent, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_loadUInt32(serializeSettings, &object->period)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_loadUInt16(serializeSettings, &object->numberOfPeriods)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int ser_loadRegisterActivation(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxRegisterActivation* object)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char ln[6];
    uint16_t ot;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxRegisterActivationMask* it;
#if defined(DLMS_IGNORE_MALLOC)
    uint16_t v;
    uint16_t size;
#endif //defined(DLMS_IGNORE_MALLOC)
#else
    gxByteBuffer* key, * value;
#endif //DLMS_IGNORE_MALLOC
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    uint16_t count;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        obj_clearRegisterActivationAssignment(&object->registerAssignment);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = ser_loadArray(serializeSettings, &object->registerAssignment, &count)) == 0)
        {
            size = object->registerAssignment.size;
            object->registerAssignment.size = count;
#else
        if ((ret = ser_verifyObjectArray(serializeSettings, &object->registerAssignment, &count)) == 0)
        {
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndexRef(&object->registerAssignment, pos, (void**)&od)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = ser_loadUInt16(serializeSettings, &ot)) != 0 ||
                    (ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                        , 6
#endif //DLMS_IGNORE_MALLOC
                    )) != 0 ||
                    (ot != 0 && (ret = cosem_findObjectByLN(settings, ot, ln, &od)) != 0))
                {
                    break;
                }
#if defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_push(&object->registerAssignment, od)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = oa_push(&object->registerAssignment, od)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
#ifdef DLMS_IGNORE_MALLOC
        object->registerAssignment.size = size;
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        obj_clearRegisterActivationMaskList(&object->maskList);
        if (ret == 0 && (ret = ser_loadArray(serializeSettings, &object->maskList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = ser_getArrayItem(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask))) != 0)
                {
                    break;
                }
#if defined(DLMS_IGNORE_MALLOC)
                if ((ret = ser_loadOctetString3(serializeSettings, it->name, &v)) != 0)
                {
                    break;
                }
                it->length = (unsigned char)v;
                if ((ret = ser_loadOctetString3(serializeSettings, it->indexes, &v)) != 0)
                {
                    break;
                }
                it->count = (unsigned char)v;
#else
                if ((ret = ser_loadOctetString(serializeSettings, &it->name)) != 0 ||
                    (ret = ser_loadOctetString(serializeSettings, &it->indexes)) != 0)
                {
                    break;
                }
#endif //defined(DLMS_IGNORE_MALLOC)
#else
                key = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (key == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                BYTE_BUFFER_INIT(key);
                value = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (value == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                BYTE_BUFFER_INIT(value);
                if ((ret = arr_push(&object->maskList, key_init(key, value))) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString(serializeSettings, key)) != 0 ||
                    (ret = ser_loadOctetString(serializeSettings, value)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        ret = ser_loadOctetString(serializeSettings, &object->activeMask);
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int ser_loadActionItem(
    dlmsSettings* settings,
    gxActionItem* item,
    gxSerializerSettings* serializeSettings)
{
    int ret;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0 ||
        (ret = ser_loadUInt16(serializeSettings, &item->scriptSelector)) != 0 ||
        (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&item->script)) != 0)
    {
    }
#else
    if ((ret = ser_get(serializeSettings, item->logicalName, 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0 ||
        (ret = ser_loadUInt16(serializeSettings, item->scriptSelector)) != 0)
    {
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    return ret;
}

int ser_loadRegisterMonitor(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxRegisterMonitor* object)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    dlmsVARIANT_PTR tmp;
    dlmsVARIANT tmp2;
    var_init(&tmp2);
    gxActionSet* as;
    uint16_t count;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        va_clear(&object->thresholds);
        if ((ret = ser_loadVariantArray(serializeSettings, &object->thresholds, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if (pos < object->thresholds.size)
                {
                    if ((ret = ser_getVariantArrayItem(&object->thresholds, pos, &tmp)) != 0)
                    {
                        break;
                    }
                }
                else
                {
                    var_clear(&tmp2);
                    tmp = &tmp2;
                }
                if ((ret = ser_loadVariant(tmp, serializeSettings)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t ot;
        unsigned char ln[6];
        if ((ret = ser_loadUInt16(serializeSettings, &ot)) != 0 ||
            (ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                , 6
#endif //DLMS_IGNORE_MALLOC
            )) != 0 ||
            (ret = cosem_findObjectByLN(settings, (DLMS_OBJECT_TYPE)ot, ln, &object->monitoredValue.target)) != 0 ||
            (ret = ser_loadInt8(serializeSettings, &object->monitoredValue.attributeIndex)) != 0)
        {

        }
#else
        if ((ret = ser_loadUInt16(serializeSettings, &object->monitoredValue.objectType)) != 0)
            (ret = ser_get(serializeSettings, object->monitoredValue.logicalName, 6
#ifdef DLMS_IGNORE_MALLOC
                , 6
#endif //DLMS_IGNORE_MALLOC
            )) != 0 ||
            (ret = ser_loadInt8(serializeSettings, &object->monitoredValue.attributeIndex)) != 0)
    {

        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        obj_clearRegisterMonitorActions(&object->actions);
        if ((ret = ser_loadArray(serializeSettings, &object->actions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->actions, pos, (void**)&as, sizeof(gxActionSet))) != 0 ||
                    (ret = ser_loadActionItem(settings, &as->actionUp, serializeSettings)) != 0 ||
                    (ret = ser_loadActionItem(settings, &as->actionDown, serializeSettings)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int ser_loadActionSchedule(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxActionSchedule* object)
{
    int pos, ret = 0;
    unsigned char ch;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxtime* tm;
    uint16_t count;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&object->executedScript)) != 0 ||
            (ret = ser_loadUInt16(serializeSettings, &object->executedScriptSelector)) != 0)
        {
        }
#else
        if ((ret = ser_get(serializeSettings, object->executedScriptLogicalName
#ifdef DLMS_IGNORE_MALLOC
            , capacity
#endif //DLMS_IGNORE_MALLOC
        )) != 0 ||
            (ret = ser_loadUInt16(serializeSettings, &object->executedScriptSelector)) == 0)
        {
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->type = ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        arr_clear(&object->executionTime);
        if ((ret = ser_loadArray(serializeSettings, &object->executionTime, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0 ||
                    (ret = ser_loadDateTime(tm, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int ser_loadSapAssignment(
    gxSerializerSettings* serializeSettings,
    gxSapAssignment* object)
{
    int ret = 0, pos;
    gxSapItem* it;
    uint16_t count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        obj_clearSapList(&object->sapAssignmentList);
        //Add count.
        if ((ret = ser_loadArray(serializeSettings, &object->sapAssignmentList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0 ||
                    (ret = ser_loadUInt16(serializeSettings, &it->id)) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = ser_loadOctetString3(serializeSettings, it->name.value, &it->name.size)) != 0)
                {
                    break;
                }
#else
                if ((ret = ser_loadOctetString(serializeSettings, &it->name)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_AUTO_ANSWER
int ser_loadAutoAnswer(
    gxSerializerSettings* serializeSettings,
    gxAutoAnswer* object)
{
    int ret = 0;
    unsigned char ch;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->mode = ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        ret = loadTimeWindow(serializeSettings, &object->listeningWindow);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->status = ch;
        }
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, &object->numberOfCalls)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt8(serializeSettings, &object->numberOfRingsInListeningWindow)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadUInt8(serializeSettings, &object->numberOfRingsOutListeningWindow)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadUInt8(serializeSettings, &object->numberOfRingsOutListeningWindow)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int ser_loadIp4Setup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxIp4Setup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    uint32_t* tmp;
#else
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* ip;
    uint16_t pos, count;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
        {
            return ret;
        }
#else
    if ((ret = ser_get(serializeSettings, object->dataLinkLayerReference, 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
{
    ret = ser_loadUInt32(serializeSettings, &object->ipAddress);
}
if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
{
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    arr_clear(&object->multicastIPAddress);
#else
    va_clear(&object->multicastIPAddress);
#endif //DLMS_IGNORE_MALLOC
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    if ((ret = ser_loadArray(serializeSettings, &object->multicastIPAddress, &count)) == 0)
#else
    if ((ret = ser_loadVariantArray(serializeSettings, &object->multicastIPAddress, &count)) == 0)
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    {
        for (pos = 0; pos != count; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            if ((ret = ser_getArrayItem(&object->multicastIPAddress, pos, (void**)&tmp, sizeof(uint32_t))) != 0 ||
                (ret = ser_loadUInt32(serializeSettings, tmp)) != 0)
            {
                break;
            }
#else
            tmp = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
            if (tmp == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
            tmp->vt = DLMS_DATA_TYPE_UINT32;
            if ((ret = va_push(&object->multicastIPAddress, tmp)) != 0 ||
                (ret = ser_loadUInt32(serializeSettings, &tmp->ulVal)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        arr_clear(&object->ipOptions);
        if ((ret = ser_loadArray(serializeSettings, &object->ipOptions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->ipOptions, pos, (void**)&ip, sizeof(gxip4SetupIpOption))) != 0 ||
                    (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&ip->type)) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = ser_loadOctetString3(serializeSettings, ip->data.value, &ip->data.size)) != 0)
                {
                    break;
                }
#else
                if ((ret = ser_loadOctetString(serializeSettings, &ip->data)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
}
if (ret == 0)
{
    if ((!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt32(serializeSettings, &object->subnetMask)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadUInt32(serializeSettings, &object->gatewayIPAddress)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadUInt8(serializeSettings, &object->useDHCP)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_loadUInt32(serializeSettings, &object->primaryDNSAddress)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_loadUInt32(serializeSettings, &object->secondaryDNSAddress)) != 0))
    {
    }
}
return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP
int ser_loadIp6Setup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxIp6Setup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    IN6_ADDR* ip;
    uint16_t pos, count;
    unsigned char ch;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        obj_clear((gxObject*)object);
        if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
        {
            return ret;
        }
#else
        if ((ret = ser_get(serializeSettings, object->dataLinkLayerReference, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->addressConfigMode = (DLMS_IP6_ADDRESS_CONFIG_MODE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        arr_clear(&object->unicastIPAddress);
        if (ret == 0 && (ret = ser_loadArray(serializeSettings, &object->unicastIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->unicastIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                    (ret = ser_get(serializeSettings, (unsigned char*)ip, 16
#ifdef DLMS_IGNORE_MALLOC
                        , 16
#endif //DLMS_IGNORE_MALLOC
                    )) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        arr_clear(&object->multicastIPAddress);
        if (ret == 0 && (ret = ser_loadArray(serializeSettings, &object->multicastIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->multicastIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                    (ret = ser_get(serializeSettings, (unsigned char*)ip, 16
#ifdef DLMS_IGNORE_MALLOC
                        , 16
#endif //DLMS_IGNORE_MALLOC
                    )) != 0)
                {
                    break;
                }
            }
#ifdef DLMS_IGNORE_MALLOC
            object->multicastIPAddress.size = count;
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 6))
    {
        arr_clear(&object->gatewayIPAddress);
        if (ret == 0 && (ret = ser_loadArray(serializeSettings, &object->gatewayIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->gatewayIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                    (ret = ser_get(serializeSettings, (unsigned char*)ip, 16
#ifdef DLMS_IGNORE_MALLOC
                        , 16
#endif //DLMS_IGNORE_MALLOC
                    )) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7))
    {
        ret = ser_get(serializeSettings, (unsigned char*)&object->primaryDNSAddress, 16
#ifdef DLMS_IGNORE_MALLOC
            , 16
#endif //DLMS_IGNORE_MALLOC
        );
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8))
    {
        ret = ser_get(serializeSettings, (unsigned char*)&object->secondaryDNSAddress, 16
#ifdef DLMS_IGNORE_MALLOC
            , 16
#endif //DLMS_IGNORE_MALLOC
        );
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 9))
    {
        ret = ser_loadUInt8(serializeSettings, &object->trafficClass);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
    {
        arr_clear(&object->neighborDiscoverySetup);
        gxNeighborDiscoverySetup* it2;
        if ((ret = ser_loadArray(serializeSettings, &object->neighborDiscoverySetup, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->neighborDiscoverySetup, pos, (void**)&it2, sizeof(gxNeighborDiscoverySetup))) != 0 ||
                    (ret = ser_loadUInt8(serializeSettings, &it2->maxRetry)) != 0 ||
                    (ret = ser_loadUInt16(serializeSettings, &it2->retryWaitTime)) != 0 ||
                    (ret = ser_loadUInt32(serializeSettings, &it2->sendPeriod)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
int ser_loadUtilityTables(
    gxSerializerSettings* serializeSettings,
    gxUtilityTables* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadUInt16(serializeSettings, &object->tableId)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadOctetString(serializeSettings, &object->buffer)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int ser_loadMbusSlavePortSetup(
    gxSerializerSettings* serializeSettings,
    gxMbusSlavePortSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->defaultBaud)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->availableBaud)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->addressState)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->busAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int ser_loadImageTransfer(
    gxSerializerSettings* serializeSettings,
    gxImageTransfer* object)
{
    uint16_t count;
    int pos, ret = 0;
    gxImageActivateInfo* it;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    obj_clear((gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadUInt32(serializeSettings, &object->imageBlockSize)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadBitString(serializeSettings, &object->imageTransferredBlocksStatus)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt32(serializeSettings, &object->imageFirstNotTransferredBlockNumber)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, &object->imageTransferEnabled)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->imageTransferStatus)) != 0))
    {
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7))
    {
        arr_clear(&object->imageActivateInfo);
        if ((ret = ser_loadArray(serializeSettings, &object->imageActivateInfo, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->imageActivateInfo, pos, (void**)&it, sizeof(gxImageActivateInfo))) != 0 ||
                    (ret = ser_loadUInt32(serializeSettings, &it->size)) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = ser_loadOctetString3(serializeSettings, it->identification.data, &it->identification.size)) != 0 ||
                    (ret = ser_loadOctetString3(serializeSettings, it->signature.data, &it->signature.size)) != 0)
                {
                    break;
                }
#else
                if ((ret = ser_loadOctetString(serializeSettings, &it->identification)) != 0 ||
                    (ret = ser_loadOctetString(serializeSettings, &it->signature)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int ser_loadDisconnectControl(
    gxSerializerSettings* serializeSettings,
    gxDisconnectControl* object)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char v;
    int ret = 0;
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_loadUInt8(serializeSettings, &object->outputState);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &v)) == 0)
        {
            object->controlState = (DLMS_CONTROL_STATE)v;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &v)) == 0)
        {
            object->controlMode = (DLMS_CONTROL_MODE)v;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int ser_loadLimiter(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxLimiter* object)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    uint16_t* it;
#else
    dlmsVARIANT* it;
#endif //DLMS_IGNORE_MALLOC
    uint16_t count;
    unsigned char ln[6];
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) == 0 &&
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->monitoredValue)) == 0 &&
            (ret = ser_loadInt8(serializeSettings, &object->selectedAttributeIndex)) == 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        ret = ser_loadVariant(&object->thresholdActive, serializeSettings);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        ret = ser_loadVariant(&object->thresholdNormal, serializeSettings);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        ret = ser_loadVariant(&object->thresholdEmergency, serializeSettings);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 6))
    {
        ret = ser_loadUInt32(serializeSettings, &object->minOverThresholdDuration);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7))
    {
        ret = ser_loadUInt32(serializeSettings, &object->minUnderThresholdDuration);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8))
    {
        if ((ret = ser_loadUInt16(serializeSettings, &object->emergencyProfile.id)) == 0 &&
            (ret = ser_loadDateTime(&object->emergencyProfile.activationTime, serializeSettings, DLMS_DATA_TYPE_DATETIME)) == 0 &&
            (ret = ser_loadUInt32(serializeSettings, &object->emergencyProfile.duration)) == 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 9))
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = ser_loadArray(serializeSettings, &object->emergencyProfileGroupIDs, &count)) == 0)
#else
        if ((ret = ser_loadVariantArray(serializeSettings, &object->emergencyProfileGroupIDs, &count)) == 0)
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        {
            for (pos = 0; pos != count; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = ser_getArrayItem(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(uint16_t))) != 0 ||
                    (ret = ser_loadUInt16(serializeSettings, it)) != 0)
                {
                    break;
                }
#else
                it = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                var_init(it);
                if ((ret = va_push(&object->emergencyProfileGroupIDs, it)) != 0 ||
                    (ret = ser_loadVariant(it, serializeSettings)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
        if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
        {
            ret = ser_loadUInt8(serializeSettings, &object->emergencyProfileActive);
        }
        if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 11))
        {
            if ((ret = ser_loadActionItem(settings, &object->actionOverThreshold, serializeSettings)) != 0 ||
                (ret = ser_loadActionItem(settings, &object->actionUnderThreshold, serializeSettings)) != 0)
            {
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int ser_loadMBusClient(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxMBusClient* object)
{
    int ret = 0;
    unsigned char ch;
    unsigned char ln[6];
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt32(serializeSettings, &object->capturePeriod)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, &object->primaryAddress)) != 0))
    {
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->mBusPort)) != 0)
        {
            return ret;
        }
#else
        if ((ret = ser_get(serializeSettings, object->mBusPortReference
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if ((!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt32(serializeSettings, &object->identificationNumber)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadUInt16(serializeSettings, &object->manufacturerID)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadUInt8(serializeSettings, &object->dataHeaderVersion)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 9) && (ret = ser_loadUInt8(serializeSettings, &object->deviceType)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 10) && (ret = ser_loadUInt8(serializeSettings, &object->accessNumber)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 11) && (ret = ser_loadUInt8(serializeSettings, &object->status)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 12) && (ret = ser_loadUInt8(serializeSettings, &object->alarm)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 13) && (ret = ser_loadUInt16(serializeSettings, &object->configuration)) != 0))
    {
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 14) && (ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
    {
        object->encryptionKeyStatus = (DLMS_MBUS_ENCRYPTION_KEY_STATUS)ch;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int ser_loadModemConfiguration(
    gxSerializerSettings* serializeSettings,
    gxModemConfiguration* object)
{
    unsigned char ch;
    int pos, ret = 0;
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    uint16_t count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->communicationSpeed = (DLMS_BAUD_RATE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        obj_clearModemConfigurationInitialisationStrings(&object->initialisationStrings);
        if ((ret = ser_loadArray(serializeSettings, &object->initialisationStrings, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->initialisationStrings, pos, (void**)&mi, sizeof(gxModemInitialisation))) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = ser_loadOctetString3(serializeSettings, mi->request.value, &mi->request.size)) != 0 ||
                    (ret = ser_loadOctetString3(serializeSettings, mi->response.value, &mi->response.size)) != 0 ||
                    (ret = ser_loadUInt16(serializeSettings, &mi->delay)) != 0)
                {
                    break;
                }
#else
                if ((ret = ser_loadOctetString(serializeSettings, &mi->request)) != 0 ||
                    (ret = ser_loadOctetString(serializeSettings, &mi->response)) != 0 ||
                    (ret = ser_loadUInt16(serializeSettings, &mi->delay)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        arr_clearStrings(&object->modemProfile);
        if ((ret = ser_loadArray(serializeSettings, &object->modemProfile, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex3(&object->modemProfile, pos, (void**)&it, sizeof(gxModemProfile), 0)) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString3(serializeSettings, it->value, &it->size)) != 0)
                {
                    break;
                }
#else
                it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                BYTE_BUFFER_INIT(it);
                if ((ret = arr_push(&object->modemProfile, it)) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString(serializeSettings, it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int ser_loadMacAddressSetup(
    gxSerializerSettings* serializeSettings,
    gxMacAddressSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_loadOctetString(serializeSettings, &object->macAddress)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
int ser_loadQualityOfService(
    gxQualityOfService* object,
    gxSerializerSettings* serializeSettings)
{
    int ret;
    if ((ret = ser_loadUInt8(serializeSettings, &object->precedence)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->delay)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->reliability)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->peakThroughput)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->meanThroughput)) != 0)
    {

    }
    return ret;
}

int ser_loadGPRSSetup(
    gxSerializerSettings* serializeSettings,
    gxGPRSSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadOctetString(serializeSettings, &object->apn)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadUInt16(serializeSettings, &object->pinCode)) != 0))
    {

    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        if ((ret = ser_loadQualityOfService(&object->defaultQualityOfService, serializeSettings)) != 0 ||
            (ret = ser_loadQualityOfService(&object->requestedQualityOfService, serializeSettings)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int ser_loadExtendedRegister(
    gxSerializerSettings* serializeSettings,
    gxExtendedRegister* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_loadVariant(&object->value, serializeSettings);
    }
    if (!isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadInt8(serializeSettings, &object->scaler)) != 0 ||
            (ret = ser_loadUInt8(serializeSettings, &object->unit)) != 0)
        {
        }
    }
    if ((!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadVariant(&object->status, serializeSettings)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadDateTime(&object->captureTime, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
int ser_loadApplicationContextName(
    gxSerializerSettings* serializeSettings,
    gxApplicationContextName* object)
{
    int ret;
    if ((ret = ser_get(serializeSettings, object->logicalName, 6
#ifdef DLMS_IGNORE_MALLOC
        , 6
#endif //DLMS_IGNORE_MALLOC
    )) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->jointIsoCtt)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->country)) != 0 ||
        (ret = ser_loadUInt16(serializeSettings, &object->countryName)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->identifiedOrganization)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->dlmsUA)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->applicationContext)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, &object->contextId)) != 0)
    {

    }
    return ret;
}

int ser_loadxDLMSContextType(gxSerializerSettings* serializeSettings, gxXDLMSContextType* object)
{
    int ret;
    if ((ret = ser_loadUInt32(serializeSettings, (uint32_t*)&object->conformance)) != 0 ||
        (ret = ser_loadUInt16(serializeSettings, &object->maxReceivePduSize)) != 0 ||
        (ret = ser_loadUInt16(serializeSettings, &object->maxSendPduSize)) != 0 ||
        (ret = ser_loadInt8(serializeSettings, &object->qualityOfService)) != 0 ||
        (ret = ser_loadOctetString(serializeSettings, &object->cypheringInfo)) != 0)
    {

    }
    return ret;
}

int ser_loadAuthenticationMechanismName(gxSerializerSettings* serializeSettings, gxAuthenticationMechanismName* object)
{
    unsigned char ch;
    int ret;
    if ((ret = ser_loadUInt8(serializeSettings, &object->jointIsoCtt)) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &object->country)) == 0 &&
        (ret = ser_loadUInt16(serializeSettings, &object->countryName)) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &object->identifiedOrganization)) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &object->dlmsUA)) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &object->authenticationMechanismName)) == 0 &&
        (ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
    {
        object->mechanismId = ch;
    }
    return ret;
}

int ser_loadAssociationLogicalName(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxAssociationLogicalName* object)
{
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#endif //DLMS_IGNORE_MALLOC
    unsigned char ch;
    int pos, ret = 0;
    uint16_t count, value;
#ifdef DLMS_IGNORE_MALLOC
    uint16_t capacity2;
#endif //DLMS_IGNORE_MALLOC
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        gxObject* obj = NULL;
        unsigned char version;
        DLMS_OBJECT_TYPE type = DLMS_OBJECT_TYPE_NONE;
        unsigned char ln[6];
        oa_empty(&object->objectList);
        if ((ret = ser_loadArray(serializeSettings, (gxArray*)&object->objectList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                obj = NULL;
                if ((ret = ser_loadUInt8(serializeSettings, &version)) != 0 ||
                    (ret = ser_loadUInt16(serializeSettings, &value)) != 0)
                {
                    break;
                }
                type = value;
                if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                    , 6
#endif //DLMS_IGNORE_MALLOC
                )) != 0 ||
                    (ret = oa_findByLN(&settings->objects, type, ln, &obj)) != 0)
                {
                    break;
                }
                if (obj == NULL)
                {
#ifdef DLMS_IGNORE_MALLOC
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
#else
                    if ((ret = cosem_createObject(type, &obj)) != DLMS_ERROR_CODE_OK)
                    {
                        //If unknown object.
                        if (ret == DLMS_ERROR_CODE_INVALID_PARAMETER)
                        {
                            ret = 0;
                            continue;
                        }
                        break;
                    }
                    if ((ret = cosem_setLogicalName(obj, ln)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    obj->version = version;
                    oa_push(&object->objectList, obj);
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
        if (ret != 0)
        {
            return ret;
        }
    }
    if (!isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadInt8(serializeSettings, &object->clientSAP)) != 0 ||
            (ret = ser_loadUInt16(serializeSettings, &object->serverSAP)) != 0)
        {
        }
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadApplicationContextName(serializeSettings, &object->applicationContextName)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadxDLMSContextType(serializeSettings, &object->xDLMSContextInfo)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadAuthenticationMechanismName(serializeSettings, &object->authenticationMechanismName)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadOctetString(serializeSettings, &object->secret)) != 0))
        {

        }
        if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->associationStatus = ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 9))
    {
#ifndef DLMS_IGNORE_SECURITY_SETUP
        //Security Setup Reference is from version 1.
        if (object->base.version > 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
            if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                , 6
#endif //DLMS_IGNORE_MALLOC
            )) != 0 ||
                (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln, (gxObject**)&object->securitySetup)) != 0)
            {
            }
#else
            if ((ret = ser_get(&ba, object->securitySetupReference, 6)) != 0)
            {
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
#endif //DLMS_IGNORE_SECURITY_SETUP
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
    {
        obj_clearUserList(&object->userList);
        if (object->base.version > 1)
        {
            if ((ret = ser_loadArray(serializeSettings, &object->userList, &count)) == 0)
            {
                for (pos = 0; pos != count; ++pos)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = ser_getArrayItem(&object->userList, pos, (void**)&it, sizeof(gxUser))) != 0 ||
                        (ret = ser_loadUInt8(serializeSettings, &it->id)) != 0 ||
                        (ret = ser_loadOctetString3(serializeSettings, (unsigned char*)it->name, &capacity2)) != 0)
                    {
                        break;
                    }
#else
                    uint8_t id;
                    gxByteBuffer* value = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                    if (value == NULL)
                    {
                        return DLMS_ERROR_CODE_OUTOFMEMORY;
                    }
                    if ((ret = ser_loadUInt8(serializeSettings, &id)) != 0 ||
                        (ret = ser_loadOctetString(serializeSettings, value)) != 0)
                    {
                        break;
                    }
                    if ((ret = arr_push(&object->userList, key_init2(id, value))) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int ser_loadAssociationShortName(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxAssociationShortName* object)
{
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#endif //DLMS_IGNORE_MALLOC
    int pos, ret = 0;
    uint16_t count, value;
#ifdef DLMS_IGNORE_MALLOC
    uint16_t capacity2;
#endif //DLMS_IGNORE_MALLOC
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        gxObject* obj = NULL;
        unsigned char version;
        DLMS_OBJECT_TYPE type = DLMS_OBJECT_TYPE_NONE;
        unsigned char ln[6];
        uint16_t shortName;
        oa_empty(&object->objectList);
        if ((ret = ser_loadArray(serializeSettings, (gxArray*)&object->objectList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                obj = NULL;
                if ((ret = ser_loadUInt16(serializeSettings, &shortName)) != 0 ||
                    (ret = ser_loadUInt8(serializeSettings, &version)) != 0 ||
                    (ret = ser_loadUInt16(serializeSettings, &value)) != 0)
                {
                    break;
                }
                type = value;
                if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                    , 6
#endif //DLMS_IGNORE_MALLOC
                )) != 0 ||
                    (ret = oa_findBySN(&settings->objects, shortName, &obj)) != 0)
                {
                    break;
                }
                if (obj == NULL)
                {
#ifdef DLMS_IGNORE_MALLOC
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
#else
                    if ((ret = cosem_createObject(type, &obj)) != DLMS_ERROR_CODE_OK)
                    {
                        //If unknown object.
                        if (ret == DLMS_ERROR_CODE_INVALID_PARAMETER)
                        {
                            ret = 0;
                            continue;
                        }
                        break;
                    }
                    if ((ret = cosem_setLogicalName(obj, ln)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    obj->shortName = shortName;
                    obj->version = version;
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, obj);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
#ifndef DLMS_IGNORE_MALLOC
                oa_push(&object->objectList, obj);
#endif //DLMS_IGNORE_MALLOC
                // obj->version = (unsigned char)version;
            }
        }
        if (ret != 0)
        {
            return ret;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 9))
    {
#ifndef DLMS_IGNORE_SECURITY_SETUP
        //Security Setup Reference is from version 1.
        if (object->base.version > 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
            if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
                , 6
#endif //DLMS_IGNORE_MALLOC
            )) != 0 ||
                (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln, (gxObject**)&object->securitySetup)) != 0)
            {
            }
#else
            if ((ret = ser_get(&ba, object->securitySetupReference, 6)) != 0)
            {
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
#endif //DLMS_IGNORE_SECURITY_SETUP
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
    {
        obj_clearUserList(&object->userList);
        if (object->base.version > 1)
        {
            if ((ret = ser_loadArray(serializeSettings, &object->userList, &count)) == 0)
            {
                for (pos = 0; pos != count; ++pos)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = ser_getArrayItem(&object->userList, pos, (void**)&it, sizeof(gxUser))) != 0 ||
                        (ret = ser_loadUInt8(serializeSettings, &it->id)) != 0 ||
                        (ret = ser_loadOctetString3(serializeSettings, (unsigned char*)it->name, &capacity2)) != 0)
                    {
                        break;
                    }
#else
                    uint8_t id;
                    gxByteBuffer* value = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                    if (value == NULL)
                    {
                        return DLMS_ERROR_CODE_OUTOFMEMORY;
                    }
                    if ((ret = ser_loadUInt8(serializeSettings, &id)) != 0 ||
                        (ret = ser_loadOctetString(serializeSettings, value)) != 0)
                    {
                        break;
                    }
                    if ((ret = arr_push(&object->userList, key_init2(id, value))) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
    }
    return ret;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int ser_loadPppSetup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxPppSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->phy)) != 0)
        {
            return ret;
        }
#else
        if ((ret = ser_get(serializeSettings, object->PHYReference, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        if ((ret = ser_loadOctetString(serializeSettings, &object->userName)) != 0 ||
            (ret = ser_loadOctetString(serializeSettings, &object->password)) != 0 ||
            (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->authentication)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int ser_loadProfileGeneric(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxProfileGeneric* object)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char ln[6];
    if ((!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadObjectsInternal(settings, serializeSettings, &object->captureObjects)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt32(serializeSettings, &object->capturePeriod)) != 0))
    {
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
    {
        object->sortMethod = ch;
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_get(serializeSettings, ln, 6
#ifdef DLMS_IGNORE_MALLOC
            , 6
#endif //DLMS_IGNORE_MALLOC
        )) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 6) && (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->sortObject)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadUInt32(serializeSettings, &object->entriesInUse)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadUInt32(serializeSettings, &object->profileEntries)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int ser_loadAccount(
    gxSerializerSettings* serializeSettings,
    gxAccount* object)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->paymentMode = ch;
            if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
            {
                object->accountStatus = ch;
            }
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        ret = ser_loadUInt8(serializeSettings, &object->currentCreditInUse);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
    {
        object->currentCreditStatus = ch;
    }
    if ((!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadInt32(serializeSettings, &object->availableCredit)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadInt32(serializeSettings, &object->amountToClear)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 7) && (ret = ser_loadInt32(serializeSettings, &object->clearanceThreshold)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 8) && (ret = ser_loadInt32(serializeSettings, &object->aggregatedDebt)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 13) && (ret = ser_loadDateTime(&object->accountActivationTime, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 14) && (ret = ser_loadDateTime(&object->accountClosureTime, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 16) && (ret = ser_loadInt32(serializeSettings, &object->lowCreditThreshold)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 17) && (ret = ser_loadInt32(serializeSettings, &object->nextCreditAvailableThreshold)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 18) && (ret = ser_loadUInt16(serializeSettings, &object->maxProvision)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 19) && (ret = ser_loadInt32(serializeSettings, &object->maxProvisionPeriod)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int ser_loadCredit(
    gxSerializerSettings* serializeSettings,
    gxCredit* object)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_loadInt32(serializeSettings, &object->currentCreditAmount);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->type = (DLMS_CREDIT_TYPE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        ret = ser_loadUInt8(serializeSettings, &object->priority);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        ret = ser_loadInt32(serializeSettings, &object->warningThreshold);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 6))
    {
        ret = ser_loadInt32(serializeSettings, &object->limit);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->creditConfiguration = (DLMS_CREDIT_CONFIGURATION)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8))
    {
        ret = ser_loadUInt8(serializeSettings, &object->status);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 9))
    {
        ret = ser_loadInt32(serializeSettings, &object->presetCreditAmount);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
    {
        ret = ser_loadInt32(serializeSettings, &object->creditAvailableThreshold);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 11))
    {
        ret = ser_loadDateTime(&object->period, serializeSettings, DLMS_DATA_TYPE_DATETIME);
    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE

int ser_loadUnitCharge(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxUnitCharge* target)
{
    gxChargeTable* ct;
    int ret, pos;
    if ((ret = obj_clearChargeTables(&target->chargeTables)) == 0)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count;
        if (//charge per unit scaling
            (ret = ser_loadInt8(serializeSettings, &target->chargePerUnitScaling.commodityScale)) != 0 ||
            (ret = ser_loadInt8(serializeSettings, &target->chargePerUnitScaling.priceScale)) != 0 ||
            //commodity
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = ser_loadUInt16(serializeSettings, &type)) != 0 ||
            (ret = ser_getOctetString2(serializeSettings, ln, NULL)) != 0 ||
#else
            (ret = cosem_getInt16(serializeSettings, (short*)&target->commodity.type)) != 0 ||
            (ret = ser_getOctetString2(serializeSettings, target->commodity.logicalName, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = ser_loadInt8(serializeSettings, &target->commodity.attributeIndex)) != 0 ||
            (ret = ser_loadArray(serializeSettings, &target->chargeTables, &count)) != 0)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_findObjectByLN(settings, type, ln, &target->commodity.target)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(&target->chargeTables, pos, (void**)&ct, sizeof(gxChargeTable))) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
            (ret = ser_getOctetString2(serializeSettings, ct->index.data, &ct->index.size)) != 0 ||
#else
                (ret = ser_getOctetString(serializeSettings, &ct->index)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                (ret = ser_loadInt16(serializeSettings, &ct->chargePerUnit)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int ser_loadCharge(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxCharge* object)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
        ret = ser_loadInt32(serializeSettings, &object->totalAmountPaid);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 3))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->chargeType = (DLMS_CHARGE_TYPE)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 4))
    {
        ret = ser_loadUInt8(serializeSettings, &object->priority);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 5))
    {
        ret = ser_loadUnitCharge(serializeSettings, settings, &object->unitChargeActive);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 6))
    {
        ret = ser_loadUnitCharge(serializeSettings, settings, &object->unitChargePassive);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7))
    {
        ret = ser_loadDateTime(&object->unitChargeActivationTime, serializeSettings, DLMS_DATA_TYPE_DATETIME);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8))
    {
        ret = ser_loadUInt32(serializeSettings, &object->period);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 9))
    {
        if ((ret = ser_loadUInt8(serializeSettings, &ch)) == 0)
        {
            object->chargeConfiguration = (DLMS_CHARGE_CONFIGURATION)ch;
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 10))
    {
        ret = ser_loadDateTime(&object->lastCollectionTime, serializeSettings, DLMS_DATA_TYPE_DATETIME);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 11))
    {
        ret = ser_loadInt32(serializeSettings, &object->lastCollectionAmount);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 12))
    {
        ret = ser_loadInt32(serializeSettings, &object->totalAmountRemaining);
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 13))
    {
        ret = ser_loadUInt16(serializeSettings, &object->proportion);
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int ser_loadTokenGateway(
    gxSerializerSettings* serializeSettings,
    gxTokenGateway* object)
{
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int ser_loadGsmDiagnostic(
    gxSerializerSettings* serializeSettings,
    gxGsmDiagnostic* object)
{
    int ret = 0, pos;
    gxAdjacentCell* it;
    uint16_t count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!isAttributeSet(serializeSettings, ignored, 2))
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = ser_loadOctetString(serializeSettings, &object->operatorName);
#else
        ret = ser_loadOctetString2(serializeSettings, &object->operatorName);
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0)
    {
        if ((!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->status)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->circuitSwitchStatus)) != 0) ||
            (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->packetSwitchStatus)) != 0))
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 6))
    {
        if ((ret = ser_loadUInt32(serializeSettings, &object->cellInfo.cellId)) == 0 &&
            (ret = ser_loadUInt16(serializeSettings, &object->cellInfo.locationId)) == 0 &&
            (ret = ser_loadUInt8(serializeSettings, &object->cellInfo.signalQuality)) == 0 &&
            (ret = ser_loadUInt8(serializeSettings, &object->cellInfo.ber)) == 0)
        {
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 7))
    {
        arr_clear(&object->adjacentCells);
        if ((ret = ser_loadArray(serializeSettings, &object->adjacentCells, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->adjacentCells, pos, (void**)&it, sizeof(gxAdjacentCell))) != 0 ||
                    (ret = ser_loadUInt32(serializeSettings, &it->cellId)) != 0 ||
                    (ret = ser_loadUInt8(serializeSettings, &it->signalQuality)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !isAttributeSet(serializeSettings, ignored, 8))
    {
        ret = ser_loadDateTime(&object->captureTime, serializeSettings, DLMS_DATA_TYPE_DATETIME);
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
int ser_loadCompactData(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxCompactData* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadOctetString(serializeSettings, &object->buffer)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadObjectsInternal(settings, serializeSettings, &object->captureObjects)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 4) && (ret = ser_loadUInt8(serializeSettings, &object->templateId)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 5) && (ret = ser_loadOctetString(serializeSettings, &object->templateDescription)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 6) && (ret = ser_loadUInt8(serializeSettings, (unsigned char*)&object->captureMethod)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int ser_loadLlcSscsSetup(
    gxSerializerSettings* serializeSettings,
    gxLlcSscsSetup* object)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!isAttributeSet(serializeSettings, ignored, 2) && (ret = ser_loadUInt16(serializeSettings, &object->serviceNodeAddress)) != 0) ||
        (!isAttributeSet(serializeSettings, ignored, 3) && (ret = ser_loadUInt16(serializeSettings, &object->baseNodeAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int ser_loadPrimeNbOfdmPlcPhysicalLayerCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcPhysicalLayerCounters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int ser_loadPrimeNbOfdmPlcMacSetup(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacSetup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int ser_loadPrimeNbOfdmPlcMacFunctionalParameters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacFunctionalParameters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int ser_loadPrimeNbOfdmPlcMacCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacCounters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int  ser_loadPrimeNbOfdmPlcMacNetworkAdministrationData(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int  ser_loadPrimeNbOfdmPlcApplicationsIdentification(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcApplicationsIdentification* object)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION

#ifndef DLMS_IGNORE_ARBITRATOR
int ser_loadArbitrator(
    gxSerializerSettings* serializeSettings,
    gxArbitrator* object)
{
    return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int ser_loadIec8802LlcType1Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType1Setup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int ser_loadIec8802LlcType2Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType2Setup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int ser_loadIec8802LlcType3Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType3Setup* object)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int ser_loadSFSKActiveInitiator(
    gxSerializerSettings* serializeSettings,
    gxSFSKActiveInitiator* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int ser_loadFSKMacCounters(
    gxSerializerSettings* serializeSettings,
    gxFSKMacCounters* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int ser_loadSFSKMacSynchronizationTimeouts(
    gxSerializerSettings* serializeSettings,
    gxSFSKMacSynchronizationTimeouts* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int ser_loadSFSKPhyMacSetUp(
    gxSerializerSettings* serializeSettings,
    gxSFSKPhyMacSetUp* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int ser_loadSFSKReportingSystemList(
    gxSerializerSettings* serializeSettings,
    gxSFSKReportingSystemList* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifndef DLMS_IGNORE_SCHEDULE
int ser_loadSchedule(
    gxSerializerSettings* serializeSettings,
    gxSchedule* object)
{
    return 0;
}
#endif //DLMS_IGNORE_SCHEDULE

#ifdef DLMS_ITALIAN_STANDARD
int ser_loadTariffPlan(
    gxSerializerSettings* serializeSettings,
    gxTariffPlan* object)
{
    int ret;
    if ((ret = bb_addString(serializeSettings, object->calendarName)) != 0 ||
        (ret = ser_loadUInt8(serializeSettings, object->enabled)) != 0 ||
        (ret = ser_loadDateTime(&object->activationTime, serializeSettings, DLMS_DATA_TYPE_DATETIME)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_ITALIAN_STANDARD

int ser_loadObject(
    dlmsSettings* settings,
    gxSerializerSettings* serializeSettings,
    gxObject* object)
{
    int ret = 0;
    switch (object->objectType)
    {
#ifndef DLMS_IGNORE_DATA

    case DLMS_OBJECT_TYPE_DATA:
        ret = ser_loadData(serializeSettings, (gxData*)object);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = ser_loadRegister(serializeSettings, (gxRegister*)object);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = ser_loadClock(serializeSettings, (gxClock*)object);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = ser_loadActionSchedule(serializeSettings, settings, (gxActionSchedule*)object);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = ser_loadActivityCalendar(serializeSettings, settings, (gxActivityCalendar*)object);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = ser_loadAssociationLogicalName(serializeSettings, settings, (gxAssociationLogicalName*)object);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = ser_loadAssociationShortName(serializeSettings, settings, (gxAssociationShortName*)object);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = ser_loadAutoAnswer(serializeSettings, (gxAutoAnswer*)object);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = ser_loadAutoConnect(serializeSettings, (gxAutoConnect*)object);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = ser_loadDemandRegister(serializeSettings, (gxDemandRegister*)object);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = ser_loadMacAddressSetup(serializeSettings, (gxMacAddressSetup*)object);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = ser_loadExtendedRegister(serializeSettings, (gxExtendedRegister*)object);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
#ifndef DLMS_IGNORE_GPRS_SETUP
        ret = ser_loadGPRSSetup(serializeSettings, (gxGPRSSetup*)object);
#endif //DLMS_IGNORE_GPRS_SETUP
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = ser_loadSecuritySetup(serializeSettings, (gxSecuritySetup*)object);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = ser_loadHdlcSetup(serializeSettings, (gxIecHdlcSetup*)object);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = ser_loadLocalPortSetup(serializeSettings, (gxLocalPortSetup*)object);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = ser_loadIecTwistedPairSetup(serializeSettings, (gxIecTwistedPairSetup*)object);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = ser_loadIp4Setup(serializeSettings, settings, (gxIp4Setup*)object);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = ser_loadIp6Setup(serializeSettings, settings, (gxIp6Setup*)object);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = ser_loadMbusSlavePortSetup(serializeSettings, (gxMbusSlavePortSetup*)object);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = ser_loadImageTransfer(serializeSettings, (gxImageTransfer*)object);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = ser_loadDisconnectControl(serializeSettings, (gxDisconnectControl*)object);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = ser_loadLimiter(serializeSettings, settings, (gxLimiter*)object);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = ser_loadMBusClient(serializeSettings, settings, (gxMBusClient*)object);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = ser_loadModemConfiguration(serializeSettings, (gxModemConfiguration*)object);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = ser_loadPppSetup(serializeSettings, settings, (gxPppSetup*)object);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = ser_loadProfileGeneric(serializeSettings, settings, (gxProfileGeneric*)object);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = ser_loadRegisterActivation(serializeSettings, settings, (gxRegisterActivation*)object);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = ser_loadRegisterMonitor(serializeSettings, settings, (gxRegisterMonitor*)object);
        break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_REGISTER_TABLE
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_REGISTER_TABLE
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = ser_loadSapAssignment(serializeSettings, (gxSapAssignment*)object);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = ser_loadSchedule(serializeSettings, (gxSchedule*)object);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = ser_loadScriptTable(serializeSettings, settings, (gxScriptTable*)object);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SMTP_SETUP
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_SMTP_SETUP
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = ser_loadSpecialDaysTable(serializeSettings, (gxSpecialDaysTable*)object);
        break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_STATUS_MAPPING
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_STATUS_MAPPING
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = ser_loadTcpUdpSetup(serializeSettings, settings, (gxTcpUdpSetup*)object);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = ser_loadUtilityTables(serializeSettings, (gxUtilityTables*)object);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = ser_loadMBusMasterPortSetup(serializeSettings, (gxMBusMasterPortSetup*)object);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = ser_loadPushSetup(serializeSettings, settings, (gxPushSetup*)object);
        break;
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_DATA_PROTECTION
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_DATA_PROTECTION
#ifndef DLMS_IGNORE_ACCOUNT
    case DLMS_OBJECT_TYPE_ACCOUNT:
        ret = ser_loadAccount(serializeSettings, (gxAccount*)object);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = ser_loadCredit(serializeSettings, (gxCredit*)object);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = ser_loadCharge(serializeSettings, settings, (gxCharge*)object);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = ser_loadTokenGateway(serializeSettings, (gxTokenGateway*)object);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = ser_loadGsmDiagnostic(serializeSettings, (gxGsmDiagnostic*)object);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = ser_loadCompactData(serializeSettings, settings, (gxCompactData*)object);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = ser_loadLlcSscsSetup(serializeSettings, (gxLlcSscsSetup*)object);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = ser_loadPrimeNbOfdmPlcPhysicalLayerCounters(serializeSettings, (gxPrimeNbOfdmPlcPhysicalLayerCounters*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = ser_loadPrimeNbOfdmPlcMacSetup(serializeSettings, (gxPrimeNbOfdmPlcMacSetup*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = ser_loadPrimeNbOfdmPlcMacFunctionalParameters(serializeSettings, (gxPrimeNbOfdmPlcMacFunctionalParameters*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = ser_loadPrimeNbOfdmPlcMacCounters(serializeSettings, (gxPrimeNbOfdmPlcMacCounters*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = ser_loadPrimeNbOfdmPlcMacNetworkAdministrationData(serializeSettings, (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = ser_loadPrimeNbOfdmPlcApplicationsIdentification(serializeSettings, (gxPrimeNbOfdmPlcApplicationsIdentification*)object);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        break;
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = ser_loadArbitrator(serializeSettings, (gxArbitrator*)object);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = ser_loadIec8802LlcType1Setup(serializeSettings, (gxIec8802LlcType1Setup*)object);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ser_loadIec8802LlcType2Setup(serializeSettings, (gxIec8802LlcType2Setup*)object);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ser_loadIec8802LlcType3Setup(serializeSettings, (gxIec8802LlcType3Setup*)object);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ser_loadSFSKActiveInitiator(serializeSettings, (gxSFSKActiveInitiator*)object);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ser_loadFSKMacCounters(serializeSettings, (gxFSKMacCounters*)object);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ser_loadSFSKMacSynchronizationTimeouts(serializeSettings, (gxSFSKMacSynchronizationTimeouts*)object);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ser_loadSFSKPhyMacSetUp(serializeSettings, (gxSFSKPhyMacSetUp*)object);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ser_loadSFSKReportingSystemList(serializeSettings, (gxSFSKReportingSystemList*)object);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = ser_loadTariffPlan(serializeSettings, (gxTariffPlan*)object);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default: //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

int ser_getDataSize(gxSerializerSettings* serializeSettings, void* size)
{
    int ret;
    //Serializer version number.
    unsigned char version;
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    ResetPosition(serializeSettings);
#endif //!(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    if ((ret = ser_loadUInt8(serializeSettings, &version)) == 0)
    {
        if (version == 0 || version > SERIALIZATION_VERSION)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace(GET_STR_FROM_EEPROM("ser_loadObject failed. Invalid version,"), version);
#endif //DLMS_DEBUG
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        ret = ser_loadUInt32(serializeSettings, size);
    }
    return ret;
}

//Serialize objects to bytebuffer.
int ser_loadObjects(
    dlmsSettings* settings,
    gxSerializerSettings* serializeSettings,
    gxObject** object,
    uint16_t count)
{
    uint16_t pos;
    int ret = 0;
    uint32_t size;
    //Serializer version number.
    if ((ret = ser_getDataSize(serializeSettings, &size)) == 0 && size != 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            //If all data is read.
            if (ser_isEof(serializeSettings))
            {
                break;
            }
#ifdef DLMS_DEBUG
            svr_notifyTrace(GET_STR_FROM_EEPROM("ser_loadObject"), pos);
#endif //DLMS_DEBUG
            if ((ret = ser_loadObject(settings, serializeSettings, object[pos])) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("ser_loadObject failed"), pos);
#endif //DLMS_DEBUG
                break;
            }
        }
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
        if (ret == 0 && serializeSettings->position - 5 != size)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
#endif //!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    }
    return ret;
}

//Serialize objects to bytebuffer.
int ser_loadObjects2(
    dlmsSettings* settings,
    gxSerializerSettings* serializeSettings,
    objectArray* objects)
{
    gxObject* obj;
    uint32_t size;
    uint16_t pos;
    int ret = 0;
    //Serializer version number.
    if ((ret = ser_getDataSize(serializeSettings, &size)) == 0)
    {
        for (pos = 0; pos != objects->size; ++pos)
        {
            //If all data is read.
            if (ser_isEof(serializeSettings))
            {
                break;
            }
#ifdef DLMS_DEBUG
            svr_notifyTrace(GET_STR_FROM_EEPROM("ser_loadObject"), pos);
#endif //DLMS_DEBUG
            if ((ret = oa_getByIndex(objects, pos, &obj)) != DLMS_ERROR_CODE_OK ||
                (ret = ser_loadObject(settings, serializeSettings, obj)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("ser_loadObject failed"), pos);
#endif //DLMS_DEBUG
                break;
            }
        }
#if !(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
        if (serializeSettings->position - 5 != size)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
#endif //!(!defined(GX_DLMS_SERIALIZER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    }
    return ret;
}
#endif //DLMS_IGNORE_SERIALIZER
