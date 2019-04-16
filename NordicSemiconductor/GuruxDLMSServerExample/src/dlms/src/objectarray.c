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
#include "../include/objectarray.h"

//Initialize objectArray.
void oa_init(objectArray* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
    arr->size = 0;
}


char oa_isAttached(objectArray* arr)
{
    return (arr->capacity & 0x8000) == 0x8000;
}

unsigned short oa_getCapacity(objectArray* arr)
{
    return arr->capacity & 0x7FFF;
}

//Allocate new size for the array in bytes.
void oa_capacity(objectArray* arr, const unsigned short capacity)
{
    if (!oa_isAttached(arr))
    {
        arr->capacity = capacity;
        if (arr->data == NULL)
        {
            arr->data = (gxObject**)gxmalloc(arr->capacity * sizeof(gxObject*));
        }
        else
        {
            arr->data = (gxObject**)gxrealloc(arr->data, arr->capacity * sizeof(gxObject*));
        }
    }
}

//Attach object to objectArray.
void oa_attach(objectArray * arr, const gxObject** item, const unsigned short count)
{
    arr->capacity = 0x8000 + count;
    arr->size = count;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
    arr->data = (gxObject**)item;
}

int oa_verify(objectArray * arr)
{
    unsigned short pos;
    int ret;
    gxObject *it;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = oa_getByIndex(arr, pos, &it)) != 0)
        {
            return ret;
        }
        if (it->objectType == 0)
        {
            //If init2 is not called.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

//Push new data to the objectArray.
int oa_push(objectArray * arr, gxObject* item)
{
    if (!oa_isAttached(arr) && arr->size >= arr->capacity)
    {
        arr->capacity += OBJECT_ARRAY_CAPACITY;
        if (arr->data == NULL)
        {
            arr->data = (gxObject**)gxmalloc(arr->capacity * sizeof(gxObject*));
        }
        else
        {
            arr->data = (gxObject**)gxrealloc(arr->data, arr->capacity * sizeof(gxObject*));
        }
    }
    if (oa_getCapacity(arr) <= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    arr->data[arr->size] = item;
    ++arr->size;
    return DLMS_ERROR_CODE_OK;
}

//Copy content of object array.
void oa_copy(objectArray *target, objectArray* source)
{
    int pos;
    oa_clear(target);
    oa_capacity(target, source->size);
    for (pos = 0; pos != source->size; ++pos)
    {
        target->data[pos] = source->data[pos];
    }
    target->size = source->size;
}

void oa_clear2(
    objectArray* arr,
    unsigned short index,
    unsigned short count)
{
    unsigned short pos;
    if (arr->data != NULL)
    {
        //Clear objects first.
        for (pos = 0; pos != count; ++pos)
        {
            obj_clear(arr->data[index + pos]);
        }
        for (pos = 0; pos != count; ++pos)
        {
            gxfree(arr->data[index + pos]);
        }
    }
    arr->size = index;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
}

void oa_clear(objectArray* arr)
{
    unsigned short pos;
    if (arr->data != NULL)
    {
        //Clear objects first.
        for (pos = 0; pos != arr->size; ++pos)
        {
            obj_clear(arr->data[pos]);
        }
        for (pos = 0; pos != arr->size; ++pos)
        {
            if (arr->data[pos]->free)
            {
                gxfree(arr->data[pos]);
            }
        }
        if (!oa_isAttached(arr))
        {
            gxfree(arr->data);
            arr->data = NULL;
            arr->capacity = 0;
        }
    }
    arr->size = 0;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
}

void oa_empty(objectArray* arr)
{
    if (!oa_isAttached(arr))
    {
        if (arr->data != NULL)
        {
            gxfree(arr->data);
            arr->data = NULL;
        }
        arr->capacity = 0;
    }
    arr->size = 0;
#ifndef GX_DLMS_MICROCONTROLLER
    arr->position = 0;
#endif //GX_DLMS_MICROCONTROLLER
}

//Get item from object array by index.
int oa_getByIndex(
    objectArray* arr,
    unsigned short index,
    gxObject** item)
{
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *item = arr->data[index];
    return DLMS_ERROR_CODE_OK;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int oa_findBySN(
    objectArray* objects,
    unsigned short sn,
    gxObject** object)
{
    unsigned short pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* obj = NULL;
    *object = NULL;
    for (pos = 0; pos != objects->size; ++pos)
    {
        ret = oa_getByIndex(objects, pos, &obj);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj->shortName == sn)
        {
            *object = obj;
            return 0;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

int oa_findByLN(
    objectArray* objects,
    DLMS_OBJECT_TYPE type,
    const unsigned char *ln,
    gxObject** object)
{
    unsigned short pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* obj = NULL;
    *object = NULL;
    if (ln == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    for (pos = 0; pos != objects->size; ++pos)
    {
        ret = oa_getByIndex(objects, pos, &obj);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if ((obj->objectType == type || DLMS_OBJECT_TYPE_NONE == type) && memcmp(obj->logicalName, ln, 6) == 0)
        {
            *object = obj;
            return 0;
        }
    }
    return ret;
}

int oa_getObjects(objectArray* src, DLMS_OBJECT_TYPE type, objectArray* objects)
{
    unsigned short pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* obj = NULL;
    if (src == NULL || objects == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    for (pos = 0; pos != src->size; ++pos)
    {
        ret = oa_getByIndex(src, pos, &obj);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj->objectType == type || type == DLMS_OBJECT_TYPE_NONE)
        {
            oa_push(objects, obj);
        }
    }
    //Trim array.
    oa_capacity(objects, objects->size);
    return ret;
}

int oa_getObjects2(
    objectArray* src,
    DLMS_OBJECT_TYPE* types,
    unsigned char typeCount,
    objectArray* objects)
{
    unsigned short pos, pos2;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* obj = NULL;
    oa_empty(objects);
    if (src == NULL || objects == NULL || types == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    for (pos = 0; pos != src->size; ++pos)
    {
        ret = oa_getByIndex(src, pos, &obj);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        for (pos2 = 0; pos2 != typeCount; ++pos2)
        {
            if (types[pos2] == obj->objectType)
            {
                oa_push(objects, obj);
                break;
            }
        }
    }
    //Trim array.
    oa_capacity(objects, objects->size);
    return ret;
}
