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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif

#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include "../include/errorcodes.h"
#include "../include/bitarray.h"
#include "../include/helpers.h"

//Returs needed amount of bytes to store bits.
uint16_t ba_getByteCount(uint16_t bitCount)
{
    double d = bitCount;
    if (bitCount != 0)
    {
        d /= 8;
        if (bitCount == 0 || (bitCount % 8) != 0)
        {
            ++d;
        }
    }
    return (uint16_t)d;
}

//Return byte index where bit is saved.
int getByteIndex(int bitCount)
{
    double d = bitCount;
    d /= 8;
    return (int)d;
}

//Initialize bit array.
void ba_init(bitArray* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
    arr->size = 0;
}


char ba_isAttached(bitArray* arr)
{
    return (arr->capacity & 0x8000) == 0x8000;
}

uint16_t ba_getCapacity(bitArray* arr)
{
    return arr->capacity & 0x7FFF;
}

uint16_t ba_size(bitArray* bb)
{
    return bb != 0 ? bb->size : 0;
}

void ba_attach(
    bitArray* arr,
    unsigned char* value,
    uint16_t count,
    uint16_t capacity)
{
    arr->data = value;
    arr->capacity = (uint16_t)(0x8000 | capacity);
    arr->size = count;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
}

//Allocate new size for the array in bytes.
int ba_capacity(bitArray* arr, uint16_t capacity)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!ba_isAttached(arr))
    {
        arr->capacity = capacity;
        if (capacity == 0)
        {
            if (arr->data != NULL)
            {
                gxfree(arr->data);
                arr->data = NULL;
            }
        }
        else
        {
            if (arr->data == NULL)
            {
                arr->data = (unsigned char*)gxmalloc(ba_getByteCount(arr->capacity));
                if (arr->data == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
            }
            else
            {
#ifdef gxrealloc
                //If compiler supports realloc.
                unsigned char* tmp = (unsigned char*)gxrealloc(arr->data, ba_getByteCount(arr->capacity));
                //If not enought memory available.
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr->data = tmp;
 #else
                //If compiler doesn't support realloc.
                unsigned char* old = arr->data;
                arr->data = (unsigned char*)gxmalloc(ba_getByteCount(arr->capacity));
                //If not enought memory available.
                if (arr->data == NULL)
                {
                    arr->data = old;
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(arr->data, old, ba_getByteCount(arr->size));
                gxfree(old);
 #endif // gxrealloc  
            }
        }
    }
#endif //DLMS_IGNORE_MALLOC
    if (ba_getCapacity(arr) < capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return 0;
}

//Push new data to the bit array.
int ba_set(bitArray* arr, unsigned char item)
{
    int ret = ba_setByIndex(arr, arr->size, item);
    if (ret == 0)
    {
        ++arr->size;
    }
    return ret;
}

//Set bit by index.
int ba_setByIndex(bitArray* arr, uint16_t index, unsigned char item)
{
    int ret;
    unsigned char newItem = 0;
    int byteIndex;
    item = item == 0 ? 0 : 1;
    if (!ba_isAttached(arr))
    {
        if (index >= arr->capacity)
        {
            if ((ret = ba_capacity(arr, arr->capacity + BIT_ARRAY_CAPACITY)) != 0)
            {
                return ret;
            }
            //If we are adding a bit to the higher than next byte.
            if (index >= arr->capacity)
            {
                return ba_setByIndex(arr, index, item);
            }
            newItem = 1;
        }
    }
    if (index >= arr->capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    byteIndex = getByteIndex(index);
    if (index % 8 == 0 || newItem)
    {
        arr->data[byteIndex] = (unsigned char)(item << 7);
    }
    else
    {
        arr->data[byteIndex] |= (item << (7 - (index % 8)));
    }
    return 0;
}

int ba_copy(
    bitArray* target,
    unsigned char* source,
    uint16_t count)
{
    int ret = 0;
    ba_clear(target);
    if (count != 0)
    {
        if ((ret = ba_capacity(target, count)) == 0)
        {
            memcpy(target->data, source, ba_getByteCount(count));
            target->size = count;
#ifndef GX_DLMS_MICROCONTROLLER
            target->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
        }
    }
    return ret;
}

void ba_clear(bitArray* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    if (arr->data != NULL)
    {
        gxfree(arr->data);
        arr->data = NULL;
    }
    arr->capacity = 0;
#endif //DLMS_IGNORE_MALLOC
    arr->size = 0;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
}

#ifndef GX_DLMS_MICROCONTROLLER
int ba_get(bitArray* arr, unsigned char* value)
{
    int ret = ba_getByIndex(arr, arr->position, value);
    if (ret == 0)
    {
        ++arr->position;
    }
    return ret;
}
#endif //GX_DLMS_MICROCONTROLLER

int ba_getByIndex(bitArray* arr, int index, unsigned char* value)
{
    char ch;
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    ch = arr->data[getByteIndex(index)];
    *value = (ch & (1 << (7 - (index % 8)))) != 0;
    return 0;
}

int ba_toInteger(bitArray* arr, uint32_t* value)
{
    *value = 0;
    unsigned char ch;
    int pos, ret;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = ba_getByIndex(arr, pos, &ch)) != 0)
        {
            return ret;
        }
        *value |= ch << pos;
    }
    return 0;
}

#ifndef DLMS_IGNORE_MALLOC
char* ba_toString(bitArray* arr)
{
    unsigned char ch;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    int pos, ret;
#else
    int pos;
#endif
    char* buff = (char*)gxmalloc(arr->size + 1);
    for (pos = 0; pos != arr->size; ++pos)
    {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        ret = ba_getByIndex(arr, pos, &ch);
        assert(ret == 0);
#else
        ba_getByIndex(arr, pos, &ch);
#endif
        buff[pos] = ch == 0 ? '0' : '1';
    }
    *(buff + arr->size) = 0;
    return buff;
}
#endif //DLMS_IGNORE_MALLOC

int ba_toString2(
    gxByteBuffer* bb,
    bitArray* ba)
{
    unsigned char ch;
    int pos, ret = 0;
    if ((ret = bb_capacity(bb, bb->size + ba->size)) == 0)
    {
        for (pos = 0; pos != ba->size; ++pos)
        {
            if ((ret = ba_getByIndex(ba, pos, &ch)) != 0 ||
                (ret = bb_setInt8(bb, ch == 0 ? '0' : '1')) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

