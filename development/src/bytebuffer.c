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

#include "../include/gxmem.h"
#ifndef DLMS_IGNORE_STRING_CONVERTER
#include <stdio.h> //printf needs this or error is generated.
#endif //DLMS_IGNORE_STRING_CONVERTER

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#endif
#include <string.h>
#include "../include/errorcodes.h"
#include "../include/bytebuffer.h"
#include "../include/helpers.h"

char bb_isAttached(gxByteBuffer* arr)
{
    if (arr == NULL)
    {
        return 0;
    }
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    //If byte buffer is attached.
    return (arr->capacity & 0x80000000) == 0x80000000;
#else
    return (arr->capacity & 0x8000) == 0x8000;
#endif
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
uint32_t bb_available(gxByteBuffer* arr)
#else
uint16_t bb_available(gxByteBuffer* arr)
#endif
{
    if (arr == NULL)
    {
        return 0;
    }
    return arr->size - arr->position;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
uint32_t bb_getCapacity(gxByteBuffer* arr)
#else
uint16_t bb_getCapacity(gxByteBuffer* arr)
#endif
{
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER)&& (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    //If byte buffer is attached.
    return arr->capacity & 0x7FFFFFFF;
#else
    return arr->capacity & 0x7FFF;
#endif
}


#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
uint32_t bb_size(gxByteBuffer* arr)
#else
uint16_t bb_size(gxByteBuffer* arr)
#endif
{
    return arr != NULL ? arr->size : 0;
}

int BYTE_BUFFER_INIT(
    gxByteBuffer* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
    arr->position = 0;
    arr->size = 0;
    return 0;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_capacity(
    gxByteBuffer* arr,
    uint32_t capacity)
#else
int bb_capacity(
    gxByteBuffer* arr,
    uint16_t capacity)
#endif
{
#ifndef DLMS_IGNORE_MALLOC
    //Capacity can't change if it's attached.
    if (!bb_isAttached(arr))
    {
        if (capacity == 0)
        {
            if (arr->data != NULL)
            {
                gxfree(arr->data);
                arr->data = NULL;
                arr->size = 0;
            }
        }
        else
        {
            if (arr->capacity == 0)
            {
                arr->data = (unsigned char*)gxmalloc(capacity);
                if (arr->data == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
            }
            else
            {
#ifdef gxrealloc
                //If compiler supports realloc.
                unsigned char* tmp = (unsigned char*)gxrealloc(arr->data, capacity);
                //If not enought memory available.
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr->data = tmp;
 #else
                //If compiler doesn't support realloc.
                unsigned char* old = arr->data;
                arr->data = (unsigned char*)gxmalloc(capacity);
                //If not enought memory available.
                if (arr->data == NULL)
                {
                    arr->data = old;
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(arr->data, old, arr->size);
                gxfree(old);
 #endif // gxrealloc       
            }    
            if (arr->size > capacity)
            {
                arr->size = capacity;
            }
        }
        arr->capacity = capacity;
    }
#endif //DLMS_IGNORE_MALLOC
    if (bb_getCapacity(arr) < capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return DLMS_ERROR_CODE_OK;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_zero(
    gxByteBuffer* arr,
    uint32_t index,
    uint32_t count)
#else
int bb_zero(
    gxByteBuffer* arr,
    uint16_t index,
    uint16_t count)
#endif
{
    int ret;
    if (index + count > arr->capacity)
    {
        if ((ret = bb_capacity(arr, (index + count))) != 0)
        {
            return ret;
        }
    }
    if (arr->size < index + count)
    {
        arr->size = index + count;
    }
    memset(arr->data + index, 0, count);
    return 0;
}

int bb_setUInt8(
    gxByteBuffer* arr,
    unsigned char item)
{
    int ret = bb_setUInt8ByIndex(arr, bb_size(arr), item);
    if (ret == 0)
    {
        ++arr->size;
    }
    return ret;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_insertUInt8(
    gxByteBuffer* arr,
    uint32_t index,
    unsigned char item)
#else
int bb_insertUInt8(
    gxByteBuffer* arr,
    uint16_t index,
    unsigned char item)
#endif
{
    int ret;
    if ((ret = bb_move(arr, index, index + 1, arr->size)) == 0)
    {
        ret = bb_setUInt8ByIndex(arr, index, item);
    }
    return ret;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_allocate(
    gxByteBuffer* arr,
    uint32_t index,
    uint32_t dataSize)
#else
int bb_allocate(
    gxByteBuffer* arr,
    uint16_t index,
    uint16_t dataSize)
#endif
{
#ifndef DLMS_IGNORE_MALLOC
    if (!bb_isAttached(arr) && (arr->capacity == 0 || index + dataSize > arr->capacity))
    {
        unsigned char empty = arr->capacity == 0;
        //If data is append fist time.
        if (dataSize > VECTOR_CAPACITY || arr->capacity == 0)
        {
            arr->capacity += dataSize;
        }
        else
        {
            arr->capacity += VECTOR_CAPACITY;
        }
        if (empty)
        {
            arr->data = (unsigned char*)gxmalloc(arr->capacity);
            if (arr->data == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
        }
        else
        {
#ifdef gxrealloc
            //If compiler supports realloc.
            unsigned char* tmp = (unsigned char*)gxrealloc(arr->data, arr->capacity);
            if (tmp == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            arr->data = tmp;
#else
            //If compiler doesn't supports realloc.
            unsigned char* old = arr->data;
            arr->data = (unsigned char*)gxmalloc(arr->capacity);
            //If not enought memory available.
            if (arr->data == NULL)
            {
                arr->data = old;
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            memcpy(arr->data, old, arr->size);
            gxfree(old);
#endif //gxrealloc
        }
    }
#endif //DLMS_IGNORE_MALLOC
    if (bb_getCapacity(arr) < index + dataSize)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return 0;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_setUInt8ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    unsigned char item)
#else
int bb_setUInt8ByIndex(
    gxByteBuffer* arr,
    uint16_t index,
    unsigned char item)
#endif
{
    if (arr == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    int ret = bb_allocate(arr, index, 1);
    if (ret == 0)
    {
        arr->data[index] = item;
    }
    return ret;
}

int bb_setUInt16(
    gxByteBuffer* arr,
    uint16_t item)
{
    int ret = bb_setUInt16ByIndex(arr, arr->size, item);
    if (ret == 0)
    {
        arr->size += 2;
    }
    return ret;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_setUInt16ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    uint16_t item)
#else
int bb_setUInt16ByIndex(
    gxByteBuffer* arr,
    uint16_t index,
    uint16_t item)
#endif
{
    int ret = 0;
    if (index + 2 > arr->size)
    {
        bb_allocate(arr, arr->size, 2);
    }
    if (ret == 0)
    {
        arr->data[index] = (item >> 8) & 0xFF;
        arr->data[index + 1] = item & 0xFF;
    }
    return ret;
}

int bb_setUInt32(
    gxByteBuffer* arr,
    uint32_t item)
{
    int ret = bb_setUInt32ByIndex(arr, arr->size, item);
    if (ret == 0)
    {
        arr->size += 4;
    }
    return ret;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_setUInt32ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    uint32_t item)
#else
int bb_setUInt32ByIndex(
    gxByteBuffer* arr,
    uint16_t index,
    uint32_t item)
#endif //defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
{
    int ret = bb_allocate(arr, index, 4);
    if (ret == 0)
    {
        PUT32(arr->data + index, item);
    }
    return ret;
}

int bb_setUInt64(
    gxByteBuffer* arr,
    uint64_t item)
{
    int ret = bb_allocate(arr, arr->size, 8);
    if (ret == 0)
    {
        ((unsigned char*)arr->data)[arr->size + 7] = item & 0xFF;
        item >>= 8;
        ((unsigned char*)arr->data)[arr->size + 6] = item & 0xFF;
        item >>= 8;
        ((unsigned char*)arr->data)[arr->size + 5] = item & 0xFF;
        item >>= 8;
        ((unsigned char*)arr->data)[arr->size + 4] = item & 0xFF;
        item >>= 8;
        ((unsigned char*)arr->data)[arr->size + 3] = item & 0xFF;
        item >>= 8;
        ((unsigned char*)arr->data)[arr->size + 2] = item & 0xFF;
        item >>= 8;
        ((unsigned char*)arr->data)[arr->size + 1] = item & 0xFF;
        item >>= 8;
        ((unsigned char*)arr->data)[arr->size] = item & 0xFF;
        arr->size += 8;
    }
    return ret;
}

#ifndef DLMS_IGNORE_FLOAT32
int bb_setFloat(
    gxByteBuffer* arr,
    float value)
{
    typedef union
    {
        float value;
        char b[sizeof(float)];
    } HELPER;

    HELPER tmp;
    tmp.value = value;
    int ret = bb_allocate(arr, arr->size, 4);
    if (ret == 0)
    {
        arr->data[arr->size] = tmp.b[3];
        arr->data[arr->size + 1] = tmp.b[2];
        arr->data[arr->size + 2] = tmp.b[1];
        arr->data[arr->size + 3] = tmp.b[0];
        arr->size += 4;
    }
    return ret;
}
#endif //DLMS_IGNORE_FLOAT32

#ifndef DLMS_IGNORE_FLOAT64
int bb_setDouble(
    gxByteBuffer* arr,
    double value)
{
    typedef union
    {
        double value;
        char b[sizeof(double)];
    } HELPER;

    HELPER tmp;
    tmp.value = value;
    int ret = bb_allocate(arr, arr->size, 8);
    if (ret == 0)
    {
        arr->data[arr->size] = tmp.b[7];
        arr->data[arr->size + 1] = tmp.b[6];
        arr->data[arr->size + 2] = tmp.b[5];
        arr->data[arr->size + 3] = tmp.b[4];
        arr->data[arr->size + 4] = tmp.b[3];
        arr->data[arr->size + 5] = tmp.b[2];
        arr->data[arr->size + 6] = tmp.b[1];
        arr->data[arr->size + 7] = tmp.b[0];
        arr->size += 8;
    }
    return ret;
}
#endif //DLMS_IGNORE_FLOAT64

int bb_setInt8(
    gxByteBuffer* arr,
    char item)
{
    return bb_setUInt8(arr, (unsigned char)item);
}

int bb_setInt16(
    gxByteBuffer* arr,
    short item)
{
    return bb_setUInt16(arr, (uint16_t)item);
}

int bb_setInt32(
    gxByteBuffer* arr,
    int32_t item)
{
    return bb_setUInt32(arr, (uint32_t)item);
}

int bb_setInt64(
    gxByteBuffer* arr,
    int64_t item)
{
    return bb_setUInt64(arr, (uint64_t)item);
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_set(
    gxByteBuffer* arr,
    const unsigned char* pSource,
    uint32_t count)
#else
int bb_set(
    gxByteBuffer* arr,
    const unsigned char* pSource,
    uint16_t count)
#endif
{
    if (count != 0)
    {
        int ret = bb_allocate(arr, arr->size, count);
        if (ret == 0)
        {
            if (arr->size + count > arr->capacity)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            memcpy(arr->data + arr->size, pSource, count);
            arr->size += count;
        }
        return ret;
    }
    else
    {
        return 0;
    }
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_set2(
    gxByteBuffer* arr,
    gxByteBuffer* data,
    uint32_t index,
    uint32_t count)
#else
int bb_set2(
    gxByteBuffer* arr,
    gxByteBuffer* data,
    uint16_t index,
    uint16_t count)
#endif
{
    if (data != NULL && count != 0)
    {
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
        if (count == (uint32_t)-1)
#else
        if (count == (uint16_t)-1)
#endif
        {
            count = data->size - index;
        }
        int ret = bb_set(arr, data->data + index, count);
        if (ret == 0)
        {
            data->position += count;
        }
        return ret;
}
    return 0;
}

int bb_addString(
    gxByteBuffer* arr,
    const char* value)
{
    if (value != NULL)
    {
        int len = (int)strlen(value);
        if (len > 0)
        {
            int ret = bb_set(arr, (const unsigned char*)value, (uint16_t)(len + 1));
            if (ret == 0)
            {
                //Add end of string, but that is not added to the length.
                arr->data[arr->size - 1] = '\0';
                --arr->size;
            }
            return ret;
        }
    }
    return 0;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_attach(
    gxByteBuffer* arr,
    unsigned char* value,
    uint32_t count,
    uint32_t capacity)
#else
int bb_attach(
    gxByteBuffer* arr,
    unsigned char* value,
    uint16_t count,
    uint16_t capacity)
#endif
{
    //If capacity is 1 value is cast t
    if (value == NULL || capacity < count)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    arr->data = value;
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    if (capacity >= 0x80000000)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    arr->capacity = (0x80000000 | capacity);
#else
    if (capacity >= 0x8000)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    arr->capacity = (uint16_t)(0x8000 | capacity);
#endif
    arr->size = count;
    arr->position = 0;
    return 0;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_attachString2(
    gxByteBuffer* arr,
    char* value,
    uint32_t count,
    uint32_t capacity)
#else
int bb_attachString2(
    gxByteBuffer* arr,
    char* value,
    uint16_t count,
    uint16_t capacity)
#endif
{
    return bb_attach(arr, (unsigned char*)value, count, capacity);
}

#ifndef DLMS_IGNORE_MALLOC
int bb_attachString(
    gxByteBuffer* arr,
    char* value)
{
    int len = (int)strlen(value);
    int ret = bb_set(arr, (const unsigned char*)value, (uint16_t)len);
    gxfree(value);
    return ret;
}
#endif //DLMS_IGNORE_MALLOC

int bb_clear(
    gxByteBuffer* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    //If byte buffer is attached.
    if (!bb_isAttached(arr))
    {
        if (arr->data != NULL)
        {
            gxfree(arr->data);
            arr->data = NULL;
        }
        arr->capacity = 0;
    }
#endif //DLMS_IGNORE_MALLOC
    arr->size = 0;
    arr->position = 0;
    return 0;
}

int bb_empty(
    gxByteBuffer* arr)
{
    arr->size = 0;
    arr->position = 0;
    return 0;
}

int bb_getUInt8(
    gxByteBuffer* arr,
    unsigned char* value)
{
    if (arr->position >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = ((unsigned char*)arr->data)[arr->position];
    ++arr->position;
    return 0;
}

int bb_getInt8(
    gxByteBuffer* arr,
    signed char* value)
{
    if (arr->position >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = (signed char)((unsigned char*)arr->data)[arr->position];
    ++arr->position;
    return 0;
}

int bb_getUInt8ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    unsigned char* value)
{
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = ((unsigned char*)arr->data)[index];
    return 0;
}


int bb_getUInt16(
    gxByteBuffer* arr,
    uint16_t* value)
{
    if (arr->position + 2 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = (uint16_t)(((unsigned char*)arr->data)[arr->position] << 8 |
        ((unsigned char*)arr->data)[arr->position + 1]);
    arr->position += 2;
    return 0;
}

int bb_getUInt32(
    gxByteBuffer* arr,
    uint32_t* value)
{

    if (arr->position + 4 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = GETU32(arr->data + arr->position);
    arr->position += 4;
    return 0;
}

int bb_getInt16(
    gxByteBuffer* arr,
    short* value)
{

    if (arr->position + 2 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = (short)(((unsigned char*)arr->data)[arr->position] << 8 |
        ((unsigned char*)arr->data)[arr->position + 1]);
    arr->position += 2;
    return 0;
}

int bb_getUInt24(
    gxByteBuffer* arr,
    uint32_t* value)
{
    int ret = bb_getUInt24ByIndex(arr, arr->position, value);
    arr->position += 3;
    return ret;
}

int bb_getInt32(
    gxByteBuffer* arr,
    int32_t* value)
{

    int ret = bb_getUInt32ByIndex(arr, arr->position, (uint32_t*)value);
    arr->position += 4;
    return ret;
}

int bb_getUInt24ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    uint32_t* value)
{
    if (index + 3 > bb_size(arr))
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = ((uint32_t)(arr->data)[index] << 16) |
        ((uint32_t)(arr->data)[index + 1] << 8) |
        ((uint32_t)(arr->data)[index + 2]);
    return 0;
}

int bb_getUInt32ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    uint32_t* value)
{

    if (index + 4 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = GETU32(arr->data + index);
    return 0;
}

int bb_getInt64(
    gxByteBuffer* arr,
    int64_t* value)
{
    int ret = bb_getUInt64ByIndex(arr, arr->position, (uint64_t*)value);
    if (ret == 0)
    {
        arr->position += 8;
    }
    return ret;
}

int bb_getUInt64(
    gxByteBuffer* arr,
    uint64_t* value)
{
    int ret = bb_getUInt64ByIndex(arr, arr->position, value);
    if (ret == 0)
    {
        arr->position += 8;
    }
    return ret;
}

int bb_getUInt64ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    uint64_t* value)
{
    if (index + 8 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = GETU32(arr->data + index);
    //Some 32 bit microcontrollers can't handle *value <<= 32;
    //For this reason value is sifted on two parts.
    *value <<= 16;
    *value <<= 16;
    *value |= GETU32(arr->data + index + 4);
    return 0;
}

int bb_getUInt128ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    unsigned char* value)
{
    int ret = bb_getUInt32ByIndex(arr, index, (uint32_t*)value);
    if (ret == 0)
    {
        ret = bb_getUInt32ByIndex(arr, index + 4, (uint32_t*)value + 1);
        if (ret == 0)
        {
            ret = bb_getUInt32ByIndex(arr, index + 8, (uint32_t*)value + 2);
            if (ret == 0)
            {
                ret = bb_getUInt32ByIndex(arr, index + 12, (uint32_t*)value + 3);
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_FLOAT32
int bb_getFloat(
    gxByteBuffer* arr,
    float* value)
{
    typedef union
    {
        float value;
        char b[sizeof(float)];
    } HELPER;
    HELPER tmp;
    if (arr->position + 4 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    tmp.b[3] = arr->data[arr->position];
    tmp.b[2] = arr->data[arr->position + 1];
    tmp.b[1] = arr->data[arr->position + 2];
    tmp.b[0] = arr->data[arr->position + 3];
    *value = tmp.value;
    arr->position += 4;
    return 0;
}
#endif //DLMS_IGNORE_FLOAT32

#ifndef DLMS_IGNORE_FLOAT64
int bb_getDouble(
    gxByteBuffer* arr,
    double* value)
{
    typedef union
    {
        double value;
        char b[sizeof(double)];
    } HELPER;
    HELPER tmp;
    if (arr->position + 8 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    tmp.b[7] = arr->data[arr->position];
    tmp.b[6] = arr->data[arr->position + 1];
    tmp.b[5] = arr->data[arr->position + 2];
    tmp.b[4] = arr->data[arr->position + 3];
    tmp.b[3] = arr->data[arr->position + 4];
    tmp.b[2] = arr->data[arr->position + 5];
    tmp.b[1] = arr->data[arr->position + 6];
    tmp.b[0] = arr->data[arr->position + 7];
    *value = tmp.value;
    arr->position += 8;
    return 0;
}
#endif //DLMS_IGNORE_FLOAT64

int bb_getUInt16ByIndex(
    gxByteBuffer* arr,
    uint32_t index,
    uint16_t* value)
{
    if (index + 2 > arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = (uint16_t)(((unsigned char*)arr->data)[index] << 8 |
        ((unsigned char*)arr->data)[index + 1]);
    return 0;
}

#ifndef DLMS_IGNORE_MALLOC
int bb_addHexString(
    gxByteBuffer* arr,
    const char* str)
{
    uint16_t count;
    int ret;
    unsigned char* buffer = NULL;
    ret = hlp_hexToBytes(str, &buffer, &count);
    if (ret != 0)
    {
        return ret;
    }
    if (buffer != NULL)
    {
        bb_set(arr, buffer, count);
        gxfree(buffer);
    }
    return 0;
}
#endif //DLMS_IGNORE_MALLOC

int bb_addHexString2(
    gxByteBuffer* arr,
    const char* str)
{
    if (str == NULL)
    {
        return 0;
    }
    int len = (int)strlen(str);
    if (len == 0)
    {
        return 0;
    }
    int ret = 0;
    int lastValue = -1;
    for (int pos = 0; pos != len; ++pos)
    {
        if (*str >= '0' && *str < 'g')
        {
            if (lastValue == -1)
            {
                lastValue = hlp_getValue(*str);
            }
            else if (lastValue != -1)
            {
                if ((ret = bb_setUInt8(arr, (unsigned char)(lastValue << 4 | hlp_getValue(*str)))) != 0)
                {
                    break;
                }
                lastValue = -1;
            }
        }
        else if (lastValue != -1)
        {
            ret = bb_setUInt8(arr, hlp_getValue(*str));
            lastValue = -1;
        }
        ++str;
    }
    return ret;
}

int bb_addLogicalName(
    gxByteBuffer* arr,
    const unsigned char* str)
{
    int ret;
    char buffer[25];
    if ((ret = hlp_bytesToHex2(str, 6, buffer, sizeof(buffer))) == 0)
    {
        ret = bb_addString(arr, buffer);
    }
    return ret;
}

int bb_toHexString2(
    gxByteBuffer* arr,
    char* buffer,
    uint16_t size)
{
    return hlp_bytesToHex2(arr->data, (uint16_t)arr->size, buffer, size);
}

#if !(defined(DLMS_IGNORE_STRING_CONVERTER) || defined(DLMS_IGNORE_MALLOC))
char* bb_toString(
    gxByteBuffer* arr)
{
    char* buff = (char*)gxmalloc(arr->size + 1);
    if (buff != NULL)
    {
        memcpy(buff, arr->data, arr->size);
        *(buff + arr->size) = 0;
    }
    return buff;
}

char* bb_toHexString(
    gxByteBuffer* arr)
{
    char* buff = hlp_bytesToHex(arr->data, arr->size);
    return buff;
}

void bb_addDoubleAsString(
    gxByteBuffer* bb,
    double value)
{
    char buff[20];
    //Show as integer value if there is no fractal part.
    if (value - (int32_t)value == 0)
    {
        bb_addIntAsString(bb, (int)value);
    }
    else
    {
#if _MSC_VER > 1000
        sprintf_s(buff, 20, "%lf", value);
#else
        sprintf(buff, "%lf", value);
#endif
        bb_addString(bb, buff);
}
}
#endif //!(defined(DLMS_IGNORE_STRING_CONVERTER) || defined(DLMS_IGNORE_MALLOC))

int bb_addIntAsString(gxByteBuffer* bb, int value)
{
    return bb_addIntAsString2(bb, value, 0);
}

int bb_addIntAsString2(gxByteBuffer* bb, int value, unsigned char digits)
{
    int ret;
    char str[20];
    hlp_intToString(str, 20, value, 1, digits);
    if ((ret = bb_addString(bb, str)) != 0)
    {

    }
    return ret;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_subArray(
    gxByteBuffer* bb,
    uint32_t index,
    uint32_t count,
    gxByteBuffer* target)
#else
int bb_subArray(
    gxByteBuffer* bb,
    uint16_t index,
    uint16_t count,
    gxByteBuffer* target)
#endif
{
    bb_clear(target);
    bb_set2(target, bb, index, count);
    return 0;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_insert(const unsigned char* src,
    uint32_t count,
    gxByteBuffer* target,
    uint32_t index)
#else
int bb_insert(const unsigned char* src,
    uint16_t count,
    gxByteBuffer* target,
    uint16_t index)
#endif
{
    int ret;
    if (target->size == 0)
    {
        ret = bb_set(target, src, count);
    }
    else
    {
        if ((ret = bb_capacity(target, target->size + count)) == 0 &&
            (ret = bb_move(target, index, index + count, target->size - index)) == 0)
        {
            //Do not use memcpy here!
            memmove(target->data + index, src + index, count);
        }
    }
    return ret;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_move(
    gxByteBuffer* bb,
    uint32_t srcPos,
    uint32_t destPos,
    uint32_t count)
#else
int bb_move(
    gxByteBuffer* bb,
    uint16_t srcPos,
    uint16_t destPos,
    uint16_t count)
#endif
{
    //If items are removed.
    if (srcPos > destPos)
    {
        if (bb->size < destPos + count)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else
    {
        //Append data.
        if (bb_getCapacity(bb) < count + destPos)
        {
            int ret;
            if (bb_isAttached(bb))
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            if ((ret = bb_capacity(bb, count + destPos)) != 0)
            {
                return ret;
            }
        }
    }
    if (count != 0)
    {
        //Do not use memcpy here!
        memmove(bb->data + destPos, bb->data + srcPos, count);
        bb->size = (destPos + count);
        if (bb->position > bb->size)
        {
            bb->position = bb->size;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int bb_trim(
    gxByteBuffer* bb)
{
    int ret;
    if (bb->size == bb->position)
    {
        bb->size = 0;
    }
    else
    {
        if ((ret = bb_move(bb, bb->position, 0, bb->size - bb->position)) != 0)
        {
            return ret;
        }
    }
    bb->position = 0;
    return DLMS_ERROR_CODE_OK;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
unsigned char bb_compare(
    gxByteBuffer* bb,
    unsigned char* buff,
    uint32_t length)
#else
unsigned char bb_compare(
    gxByteBuffer* bb,
    unsigned char* buff,
    uint16_t length)
#endif

{
    unsigned char equal;
    if (bb_available(bb) != length)
    {
        return 0;
    }
    equal = memcmp(bb->data + bb->position, buff, length) == 0;
    if (equal)
    {
        bb->position += length;
    }
    return equal;
}

#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
int bb_get(
    gxByteBuffer* bb,
    unsigned char* value,
    uint32_t count)
#else
int bb_get(
    gxByteBuffer* bb,
    unsigned char* value,
    uint16_t count)
#endif
{
    if (bb == NULL || value == NULL || bb->size - bb->position < count)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    memcpy(value, bb->data + bb->position, count);
    bb->position += count;
    return 0;
}

uint32_t bb_indexOf(
    gxByteBuffer* bb,
    char ch)
{
    uint32_t pos;
    if (bb == NULL)
    {
        return (uint32_t)-1;
    }
    for (pos = bb->position; pos < bb->size; ++pos)
    {
        if (bb->data[pos] == ch)
        {
            return pos;
        }
    }
    return (uint32_t)-1;
}

#ifndef GX_DLMS_MICROCONTROLLER
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
void bb_print(gxByteBuffer* bb)
{
    const char hexArray[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    uint32_t pos;
    char hexChars[4];
    hexChars[2] = ' ';
    hexChars[3] = '\0';
    for (pos = 0; pos != bb->size; ++pos)
    {
        hexChars[0] = hexArray[bb->data[pos] >> 4];
        hexChars[1] = hexArray[bb->data[pos] & 0x0F];
        printf("%s", hexChars);
    }
}
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#endif //GX_DLMS_MICROCONTROLLER
