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
    arr->position = 0;
    arr->size = 0;
}

//Allocate new size for the array in bytes.
int arr_capacity(gxArray* arr, int capacity)
{
    arr->capacity = capacity;
    if (arr->capacity != 0)
    {
        if (arr->data == NULL)
        {
            arr->data = (void**)gxmalloc(arr->capacity * sizeof(void*));
        }
        else
        {
            arr->data = (void**)gxrealloc(arr->data, arr->capacity * sizeof(void*));
        }
        if (arr->data == NULL)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
    }
    return 0;
}

//Push new data to the gxArray.
int arr_push(gxArray * arr, void* item)
{
    if (arr->size >= arr->capacity)
    {
        arr->capacity += GXARRAY_CAPACITY;
        if (arr->data == NULL)
        {
            arr->data = (void**)gxmalloc(arr->capacity * sizeof(void*));
        }
        else
        {
            arr->data = (void**)gxrealloc(arr->data, arr->capacity * sizeof(void*));
        }
        if (arr->data == NULL)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
    }
    arr->data[arr->size] = item;
    ++arr->size;
    return 0;
}

void arr_clear(
    gxArray* arr)
{
    int pos;
    if (arr->size != 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            gxfree(arr->data[pos]);
        }
    }
    if (arr->capacity != 0)
    {
        gxfree(arr->data);
        arr->data = NULL;
    }
    arr->capacity = 0;
    arr->size = 0;
    arr->position = 0;
}

void arr_empty(
    gxArray* arr)
{
    if (arr->size != 0)
    {
        gxfree(arr->data);
        arr->data = NULL;
    }
    arr->capacity = 0;
    arr->size = 0;
    arr->position = 0;
}

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

int arr_getByIndex(gxArray* arr, int index, void** value)
{
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = arr->data[index];
    return 0;
}

void arr_clearKeyValuePair(gxArray* arr)
{
    gxKey* k;
    int pos;
    if (arr->size != 0)
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
    }
    arr->capacity = 0;
    arr->size = 0;
    arr->position = 0;
}

void arr_clearStrings(gxArray* arr)
{
    gxByteBuffer* it;
    int pos;
    if (arr->size != 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            it = (gxByteBuffer*)arr->data[pos];
            gxfree(it->data);
            gxfree(it);
        }
        gxfree(arr->data);
        arr->data = NULL;
    }
    arr->capacity = 0;
    arr->size = 0;
    arr->position = 0;
}

int arr_removeByIndex(gxArray* arr, int index, void** value)
{
    int pos;
    if (index < 0 || index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = arr->data[index];
    for (pos = index; pos != arr->size; ++pos)
    {
        arr->data[pos] = arr->data[pos + 1];
    }
    --arr->size;
    return 0;
}
