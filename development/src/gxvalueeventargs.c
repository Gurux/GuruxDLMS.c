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
#include "../include/gxvalueeventargs.h"
#include "../include/objectarray.h"

void vec_init(gxValueEventCollection* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
    arr->position = 0;
    arr->size = 0;
}

char vec_isAttached(gxValueEventCollection* arr)
{
    return (arr->capacity & 0x80) == 0x80;
}

unsigned char vec_getCapacity(gxValueEventCollection* arr)
{
    return arr->capacity & 0x7F;
}

#ifdef DLMS_IGNORE_MALLOC
void vec_attach(
    gxValueEventCollection* arr,
    gxValueEventArg* value,
    unsigned char count,
    unsigned char capacity)
{
    arr->data = value;
    arr->capacity = (uint16_t)(0x80 | capacity);
    arr->size = count;
    arr->position = 0;
}
#endif //DLMS_IGNORE_MALLOC

//Allocate new size for the array in bytes.
int vec_capacity(gxValueEventCollection* arr, unsigned char capacity)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!vec_isAttached(arr))
    {
        arr->capacity = capacity;
        if (arr->data == NULL)
        {
            arr->data = (gxValueEventArg * *)gxmalloc(arr->capacity * sizeof(gxValueEventArg*));
        }
        else
        {
#ifdef gxrealloc
            //If compiler supports realloc.
            arr->data = (gxValueEventArg * *)gxrealloc(arr->data, arr->capacity * sizeof(gxValueEventArg*));
#else
            //If compiler doesn't supports realloc.
            gxValueEventArg ** old = arr->data;
            arr->data = (gxValueEventArg * *)gxmalloc(arr->capacity * sizeof(gxValueEventArg*));
            //If not enought memory available.
            if (arr->data == NULL)
            {
                arr->data = old;
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            memcpy(arr->data, old, sizeof(gxValueEventArg*) * arr->size);
            gxfree(old);
#endif //gxrealloc
        }
    }
#endif //DLMS_IGNORE_MALLOC
    if (vec_getCapacity(arr) < capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return 0;
}

#ifndef DLMS_IGNORE_MALLOC
//Push new data to the gxValueEventCollection.
int vec_push(gxValueEventCollection * arr, gxValueEventArg* item)
{
    int ret = 0;
    if (!vec_isAttached(arr))
    {
        if (arr->size >= vec_getCapacity(arr))
        {
            if ((ret = vec_capacity(arr, arr->capacity + 2)) != 0)
            {
                return ret;
            }
        }
    }
    if (vec_getCapacity(arr) <= arr->size)
    {
        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    else
    {
        arr->data[arr->size] = item;
        ++arr->size;
    }
    return ret;
}
#endif //DLMS_IGNORE_MALLOC

void vec_empty(
    gxValueEventCollection* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!vec_isAttached(arr))
    {
        if (arr->size != 0)
        {
            gxfree(arr->data);
            arr->data = NULL;
        }
        arr->capacity = 0;
}
#endif //DLMS_IGNORE_MALLOC
    arr->size = 0;
    arr->position = 0;
}

void vec_clear(
    gxValueEventCollection* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!vec_isAttached(arr))
    {
        int pos;
        if (arr->size != 0)
        {
            for (pos = 0; pos != arr->size; ++pos)
            {
                ve_clear(arr->data[pos]);
                gxfree(arr->data[pos]);
            }
            gxfree(arr->data);
            arr->data = NULL;
        }
        arr->capacity = 0;
    }
#endif //DLMS_IGNORE_MALLOC
    arr->size = 0;
    arr->position = 0;
}

int vec_getByIndex(gxValueEventCollection* arr, int index, gxValueEventArg** value)
{
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
#ifdef DLMS_IGNORE_MALLOC
    *value = &arr->data[index];
#else
    * value = arr->data[index];
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

void ve_init(gxValueEventArg * ve)
{
    var_init(&ve->value);
    ve->handled = 0;
    ve->target = NULL;
    ve->index = 0;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    ve->dataType = DLMS_DATA_TYPE_NONE;
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    ve->selector = 0;
    var_init(&ve->parameters);
    ve->error = DLMS_ERROR_CODE_OK;
    ve->action = 0;
    ve->byteArray = 0;
    ve->skipMaxPduSize = 0;
    ve->transactionStartIndex = 0;
    ve->transactionEndIndex = 0;
    ve->transaction = 0;
}

void ve_clear(gxValueEventArg * ve)
{
    var_clear(&ve->value);
    ve->handled = 0;
    ve->target = NULL;
    ve->index = 0;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    ve->dataType = DLMS_DATA_TYPE_NONE;
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    ve->selector = 0;
    var_clear(&ve->parameters);
    ve->error = DLMS_ERROR_CODE_OK;
    ve->action = 0;
    ve->byteArray = 0;
    ve->skipMaxPduSize = 0;
    ve->transactionStartIndex = 0;
    ve->transactionEndIndex = 0;
    ve->transaction = 0;
}
