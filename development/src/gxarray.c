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
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include "../include/errorcodes.h"
#include "../include/gxarray.h"
#include "../include/bytebuffer.h"
#include "../include/gxkey.h"

//Initialize gxArray.
void arr_init(gxArray* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
    arr->size = 0;
#ifndef DLMS_IGNORE_MALLOC
    arr->position = 0;
#endif //DLMS_IGNORE_MALLOC
}

char arr_isAttached(gxArray* arr)
{
    return (arr->capacity & 0x8000) == 0x8000;
}

uint16_t arr_getCapacity(gxArray* arr)
{
    return arr->capacity & 0x7FFF;
}

void arr_attach(
    gxArray* arr,
    void* value,
    uint16_t count,
    uint16_t capacity)
{
    arr->data = value;
    arr->capacity = (uint16_t)(0x8000 | capacity);
    arr->size = count;
#ifndef DLMS_IGNORE_MALLOC
    arr->position = 0;
#endif //DLMS_IGNORE_MALLOC
}


//Allocate new size for the array in bytes.
int arr_capacity(gxArray* arr, int capacity)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!arr_isAttached(arr))
    {
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
                arr->data = (void**)gxmalloc(capacity * sizeof(void*));
                if (arr->data == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
            }
            else
            {
#ifdef gxrealloc
                void* tmp = (void**)gxrealloc(arr->data, capacity * sizeof(void*));
                //If not enought memory available.
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr->data = tmp;
 #else
                //If compiler doesn't support realloc.
                void* old = arr->data;
                arr->data = (void**)gxmalloc(capacity * sizeof(void*));
                //If not enought memory available.
                if (arr->data == NULL)
                {
                    arr->data = old;
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(arr->data, old, sizeof(void*) * arr->size);
                gxfree(old);
 #endif // gxrealloc     
            }
        }
        arr->capacity = (uint16_t)capacity;
    }
#endif //DLMS_IGNORE_MALLOC
    if (arr_getCapacity(arr) < capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return 0;
}


#ifndef DLMS_IGNORE_MALLOC
int arr_push(gxArray* arr, void* item)
{
    if (arr->size >= arr->capacity)
    {
        int ret = arr_capacity(arr, arr->capacity + GXARRAY_CAPACITY);
        if (ret != 0)
        {
            return ret;
        }
    }
    arr->data[arr->size] = item;
    ++arr->size;
    return 0;
}
#endif //DLMS_IGNORE_MALLOC

void arr_clear(
    gxArray* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    if (arr->size != 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            gxfree(arr->data[pos]);
        }
    }
    if (!arr_isAttached(arr) && arr->capacity != 0)
    {
        gxfree(arr->data);
        arr->data = NULL;
        arr->capacity = 0;
    }
    if (!arr_isAttached(arr))
    {
        arr->size = 0;
    }
#else
    arr->size = 0;
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
    arr->position = 0;
#endif //DLMS_IGNORE_MALLOC
}

void arr_empty(
    gxArray* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    if (arr->size != 0)
    {
        gxfree(arr->data);
        arr->data = NULL;
    }
    arr->capacity = 0;
    if (!arr_isAttached(arr))
    {
        arr->size = 0;
    }
#else
    arr->size = 0;
#endif //DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_MALLOC
    arr->position = 0;
#endif //DLMS_IGNORE_MALLOC
}

#ifndef DLMS_IGNORE_MALLOC
int arr_get(gxArray* arr, void** value)
{
    if (arr->position >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = arr->data[arr->position];
    ++arr->position;
    return 0;
}
#endif //DLMS_IGNORE_MALLOC



#ifndef DLMS_IGNORE_MALLOC
int arr_getByIndex(gxArray* arr, uint16_t index, void** value)
{
    if (arr == NULL || value == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = arr->data[index];
    return 0;
}

int arr_getByIndex3(gxArray* arr, uint16_t index, void** value, unsigned char checkSize)
{
    if (arr == NULL || value == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (index >= arr->size)
    {
        if (checkSize || index >= arr->capacity)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
    }
    *value = arr->data[index];
    return 0;
}
#else
int arr_getByIndex(gxArray* arr, uint16_t index, void** value, uint16_t itemSize)
{
    if (arr == NULL || value == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = (void*)((unsigned char*)arr->data + (index * itemSize));
    return 0;
}
int arr_getByIndex3(gxArray* arr, uint16_t index, void** value, uint16_t itemSize, unsigned char checkSize)
{
    if (arr == NULL || value == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (index >= arr->size)
    {
        if (checkSize || index >= arr->capacity)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
    }
    *value = (void*)((unsigned char*)arr->data + (index * itemSize));
    return 0;
}
#endif //DLMS_IGNORE_MALLOC


int arr_getByIndex2(gxArray* arr, uint16_t index, void** value, uint16_t itemSize)
{
#ifndef DLMS_IGNORE_MALLOC
    return arr_getByIndex(arr, index, value);
#else
    return arr_getByIndex(arr, index, value, itemSize);
#endif //DLMS_IGNORE_MALLOC
}

int arr_getByIndex4(gxArray* arr, uint16_t index, void** value, uint16_t itemSize, unsigned char checkSize)
{
#ifndef DLMS_IGNORE_MALLOC
    return arr_getByIndex3(arr, index, value, checkSize);
#else
    return arr_getByIndex3(arr, index, value, itemSize, checkSize);
#endif //DLMS_IGNORE_MALLOC
}

int arr_getByIndexRef(gxArray* arr, uint16_t index, void** value)
{
    if (arr == NULL || value == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = (void*)((gxArray**)arr->data)[index];
    return 0;
}

int arr_setByIndexRef(gxArray* arr, void* value)
{
    if (!(arr->size < arr_getCapacity(arr)))
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    ((gxArray**)arr->data)[arr->size] = value;
    ++arr->size;
    return 0;
}


void arr_clearKeyValuePair(gxArray* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    gxKey* k;
    int pos;
    if (arr->capacity != 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            k = (gxKey*)arr->data[pos];
            gxfree(k->key);
            gxfree(k->value);
            gxfree(k);
        }
        gxfree(arr->data);
        arr->data = NULL;
        arr->capacity = 0;
        arr->size = 0;
        arr->position = 0;
    }
#else
    arr_clear(arr);
#endif //DLMS_IGNORE_MALLOC
}

void arr_clearStrings(gxArray* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    gxByteBuffer* it;
    int pos;
    if (arr->capacity != 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            it = (gxByteBuffer*)arr->data[pos];
            gxfree(it->data);
            gxfree(it);
        }
        gxfree(arr->data);
        arr->data = NULL;
        arr->capacity = 0;
        arr->size = 0;
        arr->position = 0;
    }
#else
    arr_clear(arr);
#endif //DLMS_IGNORE_MALLOC
}

#ifndef DLMS_IGNORE_MALLOC
int arr_removeByIndex(gxArray* arr, uint16_t index, void** value)
{
    int pos;
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    if (value != NULL)
    {
        *value = arr->data[index];
    }
    for (pos = index; pos != arr->size; ++pos)
    {
        arr->data[pos] = arr->data[pos + 1];
    }
    --arr->size;
    return 0;
}
#else
int arr_removeByIndex(
    gxArray* arr,
    uint16_t index,
    uint16_t itemSize)
{
    int ret;
    uint16_t pos;
    void* prev = NULL;
    void* item = NULL;
    if ((ret = arr_getByIndex(arr, index, &prev, itemSize)) == 0)
    {
        for (pos = index + 1; pos < arr->size; ++pos)
        {
            if ((ret = arr_getByIndex(arr, pos, &item, itemSize)) == 0)
            {
                memcpy(prev, item, itemSize);
                prev = item;
            }
        }
        --arr->size;
    }
    return 0;
}

#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_MALLOC
int arr_swap(gxArray* arr, uint16_t index1, uint16_t index2)
{
    void* tmp;
    if (!(index1 < arr->size || index2 < arr->size))
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    tmp = arr->data[index1];
    arr->data[index1] = arr->data[index2];
    arr->data[index2] = tmp;
    return 0;
}
#else
int arr_swap(
    gxArray* arr,
    uint16_t index1,
    uint16_t index2,
    uint16_t itemSize,
    void* tmp)
{
    int ret;
    void* prev = NULL;
    void* item = NULL;
    if ((ret = arr_getByIndex(arr, index1, &prev, itemSize)) == 0 &&
        (ret = arr_getByIndex(arr, index2, &item, itemSize)) == 0)
    {
        memcpy(tmp, prev, itemSize);
        memcpy(prev, item, itemSize);
        memcpy(item, tmp, itemSize);
    }
    return 0;
}
#endif //DLMS_IGNORE_MALLOC
