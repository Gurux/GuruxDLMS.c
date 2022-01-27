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
#if defined(_WIN64) || defined(_WIN32) || defined(__linux__)
#include <stdio.h>
#endif //defined(_WIN64) || defined(_WIN32) || defined(__linux__)
#include <string.h>
#include "../include/objectarray.h"
#include "../include/helpers.h"

#if defined(_WIN64) || defined(_WIN32) || defined(__linux__)
#include <stdio.h>
#include "../include/helpers.h"
#endif //defined(_WIN64) || defined(_WIN32) || defined(__linux__)

//Initialize objectArray.
void oa_init(objectArray* arr)
{
    arr->capacity = 0;
    arr->data = NULL;
#if !(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    arr->position = 0;
#endif //!(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    arr->size = 0;
}

char oa_isAttached(objectArray* arr)
{
    return (arr->capacity & 0x8000) == 0x8000;
}

uint16_t oa_getCapacity(objectArray* arr)
{
    return arr->capacity & 0x7FFF;
}

//Allocate new size for the array in bytes.
int oa_capacity(objectArray* arr, const uint16_t capacity)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!oa_isAttached(arr))
    {
        arr->capacity = capacity;
        if (arr->data == NULL)
        {
            arr->data = (gxObject**)gxmalloc(arr->capacity * sizeof(gxObject*));
        }
        else
        {
#ifdef gxrealloc
            //If compiler supports realloc.
            arr->data = (gxObject**)gxrealloc(arr->data, arr->capacity * sizeof(gxObject*));
 #else
            //If compiler doesn't support realloc.
            gxObject** old = arr->data;
            arr->data = (gxObject**)gxmalloc(arr->capacity * sizeof(gxObject*));
            //If not enought memory available.
            if (arr->data == NULL)
            {
                arr->data = old;
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            memcpy(arr->data, old, sizeof(gxObject*) * arr->size);
            gxfree(old);
 #endif // gxrealloc    
        }
    }
#endif //DLMS_IGNORE_MALLOC
    if (oa_getCapacity(arr) < capacity)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    return 0;
}

//Attach object to objectArray.
void oa_attach(objectArray* arr, gxObject** item, uint16_t count)
{
    arr->capacity = 0x8000 + count;
    arr->size = count;
#if !(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    arr->position = 0;
#endif //!(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    arr->data = item;
}

int oa_verify(objectArray* arr)
{
    uint16_t pos;
    int ret;
    gxObject* it;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = oa_getByIndex(arr, pos, &it)) != 0)
        {
            return ret;
        }
        if (it->objectType == 0)
        {
#if defined(_WIN64) || defined(_WIN32) || defined(__linux__)
            if (pos > 0)
            {
                char ln[25];
                if ((ret = oa_getByIndex(arr, pos - 1, &it)) != 0 ||
                    (ret = hlp_getLogicalNameToString(it->logicalName, ln)) != 0)
                {
                    return ret;
                }
                printf("Last initialized object:%s\n", ln);
            }
#endif
            //If init2 is not called.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

#ifndef DLMS_IGNORE_MALLOC
//Push new data to the objectArray.
int oa_push(objectArray* arr, gxObject* item)
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
#ifdef gxrealloc
            //If compiler supports realloc.
            arr->data = (gxObject**)gxrealloc(arr->data, arr->capacity * sizeof(gxObject*));
 #else
            //If compiler doesn't support realloc.
            gxObject** old = arr->data;
            arr->data = (gxObject**)gxmalloc(arr->capacity * sizeof(gxObject*));
            //If not enought memory available.
            if (arr->data == NULL)
            {
                arr->data = old;
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            memcpy(arr->data, old, sizeof(gxObject*) * arr->size);
            gxfree(old);
 #endif // gxrealloc    
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
#endif //DLMS_IGNORE_MALLOC

void oa_copy(objectArray* target, objectArray* source)
{
    int pos;
    oa_empty(target);
    oa_capacity(target, source->size);
    for (pos = 0; pos != source->size; ++pos)
    {
        target->data[pos] = source->data[pos];
    }
    target->size = source->size;
}

void oa_move(objectArray* target, objectArray* source)
{
    int pos;
    oa_empty(target);
    oa_capacity(target, source->size);
    for (pos = 0; pos != source->size; ++pos)
    {
        target->data[pos] = source->data[pos];
    }
    target->size = source->size;
    source->size = 0;
#if !(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    source->position = 0;
#endif //!(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
}

void oa_clear2(
    objectArray* arr,
    uint16_t index,
    uint16_t count)
{
    uint16_t pos;
    if (arr->data != NULL)
    {
        //Clear objects first.
        for (pos = 0; pos != count; ++pos)
        {
            obj_clear(arr->data[index + pos]);
        }
#ifndef DLMS_IGNORE_MALLOC
        for (pos = 0; pos != count; ++pos)
        {
            gxfree(arr->data[index + pos]);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    arr->size = index;
#if !(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    arr->position = 0;
#endif //!(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
}

void oa_clear(objectArray* arr, unsigned char releaseObjects)
{
#ifndef DLMS_IGNORE_MALLOC
    uint16_t pos;
    if (arr->data != NULL)
    {
        //Clear objects first.
        for (pos = 0; pos != arr->size; ++pos)
        {
            obj_clear(arr->data[pos]);
        }
        if (releaseObjects)
        {
            for (pos = 0; pos != arr->size; ++pos)
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
#endif //DLMS_IGNORE_MALLOC
    arr->size = 0;
#if !(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    arr->position = 0;
#endif //!(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
}

void oa_empty(objectArray* arr)
{
#ifndef DLMS_IGNORE_MALLOC
    if (!oa_isAttached(arr))
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
#if !(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    arr->position = 0;
#endif //!(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
}

//Get item from object array by index.
int oa_getByIndex(
    const objectArray* arr,
    uint16_t index,
    gxObject** item)
{
    if (index >= arr->size)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *item = (gxObject*)arr->data[index];
    return DLMS_ERROR_CODE_OK;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int oa_findBySN(
    objectArray* objects,
    uint16_t sn,
    gxObject** object)
{
    uint16_t pos;
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
    const unsigned char* ln,
    gxObject** object)
{
    uint16_t pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* obj = NULL;
    *object = NULL;
    if (ln == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    for (pos = 0; pos != objects->size; ++pos)
    {
        if ((ret = oa_getByIndex(objects, pos, &obj)) != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        if ((obj->objectType == type || DLMS_OBJECT_TYPE_NONE == type) && memcmp(obj->logicalName, ln, 6) == 0)
        {
            *object = obj;
            break;
        }
    }
    return ret;
}

int oa_getObjects(objectArray* src, DLMS_OBJECT_TYPE type, objectArray* objects)
{
    uint16_t pos;
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
#ifdef DLMS_IGNORE_MALLOC
            if (objects->data == NULL || !(objects->size < oa_getCapacity(objects)))
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            objects->data[objects->size] = obj;
            ++objects->size;
#else
            oa_push(objects, obj);
#endif //DLMS_IGNORE_MALLOC
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
    uint16_t pos, pos2;
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
#ifdef DLMS_IGNORE_MALLOC
                if (!(objects->size < oa_getCapacity(objects)))
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
                }
                memcpy(&objects->data[objects->size], obj, sizeof(gxObject));
                ++objects->size;
#else
                oa_push(objects, obj);
#endif //DLMS_IGNORE_MALLOC
                break;
            }
        }
    }
    //Trim array.
    oa_capacity(objects, objects->size);
    return ret;
}
