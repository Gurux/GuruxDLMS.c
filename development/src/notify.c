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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif
#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif

#include "../include/notify.h"
#include "../include/cosem.h"
#include "../include/gxset.h"
#include "../include/serverevents.h"

int notify_getData(
    dlmsSettings* settings,
    gxByteBuffer* reply,
    gxReplyData* data)
{
    return dlms_getData2(settings, reply, data, 0);
}

int notify_addData(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index,
    gxByteBuffer* buff)
{
    int ret;
    gxValueEventArg e;
    ve_init(&e);
    e.target = obj;
    e.index = index;
#ifdef DLMS_IGNORE_MALLOC
    e.value.byteArr = buff;
    e.value.vt = DLMS_DATA_TYPE_OCTET_STRING;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = cosem_getValue(settings, &e)) != 0)
    {
        return ret;
    }
    if (e.byteArray)
    {
        if (!bb_isAttached(buff))
        {
            bb_set(buff, e.value.byteArr->data, e.value.byteArr->size);
            var_clear(&e.value);
        }
        return 0;
    }
    ret = dlms_setData(buff, e.value.vt, &e.value);
    var_clear(&e.value);
    return ret;
}

int notify_generateDataNotificationMessages2(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t time,
#else
    struct tm* time,
#endif //DLMS_USE_EPOCH_TIME
    gxByteBuffer* data,
    message* messages)
{
    int ret;
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, DLMS_COMMAND_DATA_NOTIFICATION, 0, data, NULL, 0xff, DLMS_COMMAND_NONE, 0, 0);
        p.time = time;
        ret = dlms_getLnMessages(&p, messages);
    }
    else
    {
#if !defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
        gxSNParameters p;
        params_initSN(&p, settings, DLMS_COMMAND_DATA_NOTIFICATION, 1, 0, data, NULL, DLMS_COMMAND_NONE);
        ret = dlms_getSnMessages(&p, messages);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //!defined(DLMS_IGNORE_ASSOCIATION_SHORT_NAME) && !defined(DLMS_IGNORE_MALLOC)
    }
    return ret;
}

int notify_generateDataNotificationMessages(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t date,
#else
    struct tm* date,
#endif //DLMS_USE_EPOCH_TIME
    gxArray* objects,
    message* messages)
{
    int ret = 0;
	uint16_t pos;
    gxListItem* it;
    gxByteBuffer buff;
    BYTE_BUFFER_INIT(&buff);
    bb_setUInt8(&buff, DLMS_DATA_TYPE_STRUCTURE);
    hlp_setObjectCount(objects->size, &buff);
    for (pos = 0; pos != objects->size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(objects, pos, (void**)&it, sizeof(gxListItem))) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(objects, pos, (void**)&it)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC

        if ((ret = notify_addData(settings, it->key, it->value, &buff)) != 0)
        {
            break;
        }
    }
    if (ret == 0)
    {
        return notify_generateDataNotificationMessages2(settings, date, &buff, messages);
    }
    bb_clear(&buff);
    return ret;
}

//Sends Event Notification Request.
int notify_generateEventNotificationMessages(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t time,
#else
    struct tm* time,
#endif //DLMS_USE_EPOCH_TIME
    gxListItem* item,
    variantArray* data,
    gxByteBuffer* pdu,
    message* messages)
{
    int ret;
    uint16_t pos;
    dlmsVARIANT* it;
    if ((ret = bb_setUInt16(pdu, item->key->objectType)) != 0 ||
        (ret = bb_set(pdu, item->key->logicalName, 6)) != 0 ||
        (ret = bb_setUInt8(pdu, item->value)) != 0)
    {
        return ret;
    }
    if (data == NULL)
    {
        if ((ret = notify_addData(settings, item->key, item->value, pdu)) != 0)
        {
            return ret;
        }
    }
    else
    {
        if ((ret = bb_setUInt8(pdu, DLMS_DATA_TYPE_ARRAY)) == 0 &&
            (ret = hlp_setObjectCount(data->size, pdu)) == 0)
        {
            for (pos = 0; pos != data->size; ++pos)
            {
                if ((ret = va_getByIndex(data, pos, &it)) != 0 ||
                    (ret = dlms_setData(pdu, it->vt, it)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, DLMS_COMMAND_EVENT_NOTIFICATION, 0, pdu, NULL, 0xff, DLMS_COMMAND_NONE, 0, 0);
        p.time = time;
        ret = dlms_getLnMessages(&p, messages);
    }
    return ret;
}


//Sends Event Notification Request.
int notify_generateEventNotificationMessages2(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t time,
#else
    struct tm* time,
#endif //DLMS_USE_EPOCH_TIME
    gxListItem* item,
    gxByteBuffer* data,
    gxByteBuffer* pdu,
    message* messages)
{
    int ret;
    if ((ret = bb_setUInt16(pdu, item->key->objectType)) != 0 ||
        (ret = bb_set(pdu, item->key->logicalName, 6)) != 0 ||
        (ret = bb_setUInt8(pdu, item->value)) != 0)
    {
        return ret;
    }
    ret = bb_set2(pdu, data, data->position, bb_available(data));
    if (ret == 0)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, DLMS_COMMAND_EVENT_NOTIFICATION, 0, pdu, NULL, 0xff, DLMS_COMMAND_NONE, 0, 0);
        p.time = time;
        ret = dlms_getLnMessages(&p, messages);
    }
    return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP

int notify_generatePushSetupMessages(
    dlmsSettings* settings,
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t date,
#else
    struct tm* date,
#endif //DLMS_USE_EPOCH_TIME
    gxPushSetup* push,
    message* messages)
{
    int ret = 0;
	uint16_t pos;
    gxByteBuffer pdu;
    gxValueEventCollection args;
#ifdef DLMS_IGNORE_MALLOC
    gxTarget* it;
    pdu = *settings->serializedPdu;
    gxValueEventArg p[1];
    ve_init(&p[0]);
    p[0].action = 1;
    vec_attach(&args, p, 1, 1);
#else
    gxValueEventArg e;
    gxKey* it;
    BYTE_BUFFER_INIT(&pdu);
    ve_init(&e);
    vec_init(&args);
    vec_push(&args, &e);
#endif //DLMS_IGNORE_MALLOC
    if (push == NULL || messages == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    mes_clear(messages);
    if ((ret = bb_setUInt8(&pdu, DLMS_DATA_TYPE_STRUCTURE)) == 0 &&
        (ret = hlp_setObjectCount(push->pushObjectList.size, &pdu)) == 0)
    {
        for (pos = 0; pos != push->pushObjectList.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&push->pushObjectList, pos, (void**)&it, sizeof(gxTarget))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&push->pushObjectList, pos, (void**)&it)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
#ifdef DLMS_IGNORE_MALLOC
            p[0].target = it->target;
            p[0].index = it->attributeIndex;
            svr_preRead(settings, &args);
            if (p[0].error != 0)
            {
                break;
            }
            if ((ret = notify_addData(settings, it->target, it->attributeIndex, &pdu)) != 0)
            {
                break;
            }
#else
            e.target = (gxObject*)it->key;
            e.index = ((gxTarget*)it->value)->attributeIndex;
#ifndef DLMS_IGNORE_SERVER
            svr_preRead(settings, &args);
#endif
            if (e.error != 0)
            {
                break;
            }
            if (e.value.vt != DLMS_DATA_TYPE_NONE)
            {
                ret = dlms_setData(&pdu, e.value.vt, &e.value);
                var_clear(&e.value);
            }
            else
            {
                if ((ret = notify_addData(settings, (gxObject*)it->key, ((gxTarget*)it->value)->attributeIndex, &pdu)) != 0)
                {
                    break;
                }
            }
#endif //DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_SERVER
            svr_postRead(settings, &args);
#endif //DLMS_IGNORE_SERVER
#ifdef DLMS_IGNORE_MALLOC
            ve_clear(&p[0]);
            if (p[0].error != 0)
            {
                break;
            }
#else
            ve_clear(&e);
            if (e.error != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    vec_empty(&args);
    if (ret == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        //Update size and position
        settings->serializedPdu->position = pdu.position;
        settings->serializedPdu->size = pdu.size;
#endif //DLMS_IGNORE_MALLOC
        ret = notify_generateDataNotificationMessages2(settings, date, &pdu, messages);
    }
#ifndef DLMS_IGNORE_MALLOC
    bb_clear(&pdu);
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int notify_parsePush(
    dlmsSettings* settings,
    variantArray* data,
    gxArray* items)
{
    gxListItem* k;
    gxObject* obj;
    unsigned char index;
    int classID, ret;
	uint16_t pos;
    gxValueEventArg e;
    dlmsVARIANT* it, * list, * tmp;
    if ((ret = va_getByIndex(data, 0, &list)) != 0)
    {
        return ret;
    }

    for (pos = 0; pos != list->Arr->size; ++pos)
    {
        if ((ret = va_getByIndex(list->Arr, pos, &it)) != 0)
        {
            return ret;
        }
        if ((ret = va_getByIndex(it->Arr, 0, &tmp)) != 0)
        {
            return ret;
        }
        classID = var_toInteger(tmp) & 0xFFFF;
        if (classID > 0)
        {
            if ((ret = va_getByIndex(it->Arr, 1, &tmp)) != 0)
            {
                return ret;
            }
            if ((ret = oa_findByLN(&settings->objects, (DLMS_OBJECT_TYPE)classID, tmp->byteArr->data, &obj)) != 0)
            {
                return ret;
            }
            if (obj == NULL)
            {
#ifdef DLMS_IGNORE_MALLOC
                return DLMS_ERROR_CODE_OUTOFMEMORY;
#else
                if ((ret = cosem_createObject((DLMS_OBJECT_TYPE)classID, &obj)) != 0)
                {
                    return ret;
                }
                memcpy(obj->logicalName, tmp->byteArr, 6);
                oa_push(&settings->objects, obj);
                //Add object to released objects list.
                ret = oa_push(&settings->releasedObjects, obj);
#endif //DLMS_IGNORE_MALLOC
            }
#ifdef DLMS_IGNORE_MALLOC
            return DLMS_ERROR_CODE_OUTOFMEMORY;
#else
            if ((ret = va_getByIndex(it->Arr, 2, &tmp)) != 0)
            {
                return ret;
            }
            index = (unsigned char)var_toInteger(tmp);
            arr_push(items, key_init(obj, (void*)(uint64_t)index));
#endif //DLMS_IGNORE_MALLOC
        }
    }
    ve_init(&e);
    for (pos = 0; pos != items->size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(items, pos, (void**)&k, sizeof(gxListItem))) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(items, pos, (void**)&k)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC

        obj = (gxObject*)k->key;
        if ((ret = va_getByIndex(data, pos, &it)) != 0)
        {
            return ret;
        }
        index = k->value;
        e.target = obj;
        e.index = index;
        e.value = *it;
        if ((ret = cosem_setValue(settings, &e)) != 0)
        {
            return ret;
        }
    }
    return ret;
}

#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
int notify_getPushValues(
    dlmsSettings* settings,
    gxPushSetup* pushSetup,
    variantArray* data,
    gxArray* items)
{
    gxObject* tmp;
    gxKey* k;
    int ret = 0;
	uint16_t pos;
    gxValueEventArg e;
    dlmsVARIANT* it;
    for (pos = 0; pos != pushSetup->pushObjectList.size; ++pos)
    {
        if ((ret = arr_getByIndex(&pushSetup->pushObjectList, pos, (void**)&k)) != 0)
        {
            break;
        }
        if ((ret = cosem_createObject(((gxObject*)k->key)->objectType, &tmp)) != 0)
        {
            break;
        }
        memcpy(tmp->logicalName, ((gxObject*)k->key)->logicalName, 6);

        if ((ret = va_getByIndex(data, pos, &it)) != 0)
        {
            return ret;
        }
        e.target = (gxObject*)tmp;
        e.index = ((gxTarget*)k->value)->attributeIndex;
        e.value = *it;
        if ((ret = cosem_setValue(settings, &e)) != 0)
        {
            break;
        }
        arr_push(items, key_init(e.target, co_init(e.index, 0)));
    }
    return ret;
}
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)

#endif //DLMS_IGNORE_PUSH_SETUP
