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
unsigned short bit_getByteCount(unsigned short bitCount)
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
    return (unsigned short)d;
}

//Return byte index where bit is saved.
int getByteIndex(int bitCount)
{
    double d = bitCount;
    d /= 8;
    return (int)d;
}

//Initialize bit array.
void bit_init(bitArray* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
    arr->size = 0;
}

//Allocate new size for the array in bytes.
void bit_capacity(bitArray *arr, unsigned short capacity)
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
            arr->data = (unsigned char*)gxmalloc(bit_getByteCount(arr->capacity));
        }
        else
        {
            arr->data = (unsigned char*)gxrealloc(arr->data, bit_getByteCount(arr->capacity));
        }
    }
}

//Push new data to the bit array.
void bit_set(bitArray *arr, unsigned char item)
{
    bit_setByIndex(arr, arr->size, item);
    ++arr->size;
}

//Set bit by index.
void bit_setByIndex(bitArray *arr, int index, unsigned char item)
{
    unsigned char newItem = 0;
    int byteIndex;
    item = item == 0 ? 0 : 1;
    if (index >= arr->capacity)
    {
        arr->capacity += BIT_ARRAY_CAPACITY;
        //If we are adding a bit to the higher than next byte.
        if (index >= arr->capacity)
        {
            bit_setByIndex(arr, index, item);
            return;
        }
        if (arr->data == NULL)
        {
            arr->data = (unsigned char*)gxmalloc(bit_getByteCount(arr->capacity));
        }
        else
        {
            arr->data = (unsigned char*)gxrealloc(arr->data, bit_getByteCount(arr->capacity));
        }
        newItem = 1;
    }
    byteIndex = getByteIndex(index);
    if (index % 8 == 0 || newItem)
    {
        arr->data[byteIndex] = item << 7;
    }
    else
    {
        arr->data[byteIndex] |= (item << (7 - (index % 8)));
    }
}

//Add bits from byte array to bit array.
int bit_add(bitArray *arr, gxByteBuffer * bytes, unsigned short count, unsigned char intelByteOrder)
{
    unsigned short index, pos, bytePos;
    int ret;
    unsigned char ch = 0;
    if (count == -1)
    {
        count = (unsigned short) (bytes->size - bytes->position);
    }
    if (intelByteOrder)
    {
        index = (unsigned short) (bytes->position + bit_getByteCount(count) - 1);
    }
    else
    {
        index = (unsigned short) bytes->position;
    }
    for (pos = 0; pos != count; ++pos)
    {
        //Get next byte.
        if ((pos % 8) == 0)
        {
            bytePos = 7;
            ret = bb_getUInt8ByIndex(bytes, index, &ch);
            if (ret != 0)
            {
                return ret;
            }
            if (intelByteOrder)
            {
                --index;
            }
            else
            {
                ++index;
            }
            ++bytes->position;
        }
        bit_setByIndex(arr, pos, (unsigned char)(ch & (1 << bytePos)));
        --bytePos;
        ++arr->size;
    }
    return 0;
}

int bit_copy(
    bitArray *target,
    unsigned char *source,
    unsigned short count)
{
    bit_clear(target);
    if (count != 0)
    {
        bit_capacity(target, count);
        memcpy(target->data, source, bit_getByteCount(count));
        target->size = count;
#ifndef GX_DLMS_MICROCONTROLLER
        target->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
    }
    return 0;
}

void bit_clear(bitArray *arr)
{
    if (arr->data != NULL)
    {
        gxfree(arr->data);
        arr->data = NULL;
    }
    arr->capacity = 0;
    arr->size = 0;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
}

#ifndef GX_DLMS_MICROCONTROLLER
int bit_get(bitArray *arr, unsigned char* value)
{
    int ret = bit_getByIndex(arr, arr->position, value);
    if (ret == 0)
    {
        ++arr->position;
    }
    return ret;
}
#endif //GX_DLMS_MICROCONTROLLER

int bit_getByIndex(bitArray *arr, int index, unsigned char *value)
{
    char ch;
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    ch = arr->data[getByteIndex(index)];
    *value = (ch &  (1 << (7 - (index % 8)))) != 0;
    return 0;
}

int bit_toInteger(bitArray *arr, int *value)
{
    *value = 0;
    unsigned char ch;
    int pos, ret;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = bit_getByIndex(arr, pos, &ch)) != 0)
        {
            return ret;
        }
        *value |= ch << (arr->size - pos - 1);
    }
    return 0;
}


char* bit_toString(bitArray *arr)
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
        ret = bit_getByIndex(arr, pos, &ch);
        assert(ret == 0);
#else
        bit_getByIndex(arr, pos, &ch);
#endif
        buff[pos] = ch == 0 ? '0' : '1';
    }
    *(buff + arr->size) = 0;
    return buff;
}
