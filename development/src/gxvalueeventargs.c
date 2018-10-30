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

//Allocate new size for the array in bytes.
void vec_capacity(gxValueEventCollection* arr, unsigned char capacity)
{
    arr->capacity = capacity;
    if (arr->data == NULL)
    {
        arr->data = (gxValueEventArg**)gxmalloc(arr->capacity * sizeof(gxValueEventArg*));
    }
    else
    {
        arr->data = (gxValueEventArg**)gxrealloc(arr->data, arr->capacity * sizeof(gxValueEventArg*));
    }
}

//Push new data to the gxValueEventCollection.
void vec_push(gxValueEventCollection * arr, gxValueEventArg* item)
{
    if (arr->size >= arr->capacity)
    {
        arr->capacity += 2;
        if (arr->data == NULL)
        {
            arr->data = (gxValueEventArg**)gxmalloc(arr->capacity * sizeof(gxValueEventArg*));
        }
        else
        {
            arr->data = (gxValueEventArg**)gxrealloc(arr->data, arr->capacity * sizeof(gxValueEventArg*));
        }
    }
    arr->data[arr->size] = item;
    ++arr->size;
}

void vec_empty(
    gxValueEventCollection* arr)
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

void vec_clear(
    gxValueEventCollection* arr)
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
    arr->size = 0;
    arr->position = 0;
}

int vec_get(gxValueEventCollection* arr, gxValueEventArg** value)
{
    if (arr->position >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = arr->data[arr->position];
    ++arr->position;
    return 0;
}

int vec_getByIndex(gxValueEventCollection* arr, int index, gxValueEventArg** value)
{
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = arr->data[index];
    return 0;
}

void ve_init(gxValueEventArg * ve)
{
    var_init(&ve->value);
    ve->handled = 0;
    ve->target = NULL;
    ve->index = 0;
    ve->dataType = DLMS_DATA_TYPE_NONE;
    ve->selector = 0;
    var_init(&ve->parameters);
    ve->error = DLMS_ERROR_CODE_OK;
    ve->action = 0;
    ve->byteArray = 0;
    oa_init(&ve->releasedObjects);
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
    ve->dataType = DLMS_DATA_TYPE_NONE;
    ve->selector = 0;
    var_clear(&ve->parameters);
    ve->error = DLMS_ERROR_CODE_OK;
    ve->action = 0;
    ve->byteArray = 0;
    oa_clear(&ve->releasedObjects);
    ve->skipMaxPduSize = 0;
    ve->transactionStartIndex = 0;
    ve->transactionEndIndex = 0;
    ve->transaction = 0;
}
