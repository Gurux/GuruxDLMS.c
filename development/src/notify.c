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

int notify_getData(
    dlmsSettings* settings,
    gxByteBuffer* reply,
    gxReplyData *data)
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
    if ((ret = cosem_getValue(settings, &e)) != 0)
    {
        return ret;
    }
    if (e.byteArray)
    {
        bb_set(buff, e.value.byteArr->data, e.value.byteArr->size);
        var_clear(&e.value);
        return 0;
    }
    ret = dlms_setData(buff, e.value.vt, &e.value);
    var_clear(&e.value);
    return ret;
}

int notify_generateDataNotificationMessages2(
    dlmsSettings* settings,
    struct tm* time,
    gxByteBuffer* data,
    message* messages)
{
    int ret;
    if (settings->useLogicalNameReferencing)
    {
        gxLNParameters p;
        params_initLN(&p, settings, 0, DLMS_COMMAND_DATA_NOTIFICATION, 0, data, NULL, 0xff, DLMS_COMMAND_NONE);
        p.time = time;
        ret = dlms_getLnMessages(&p, messages);
    }
    else
    {
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        gxSNParameters p;
        params_initSN(&p, settings, DLMS_COMMAND_DATA_NOTIFICATION, 1, 0, data, NULL, DLMS_COMMAND_NONE);
        ret = dlms_getSnMessages(&p, messages);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    }
    return ret;
    //    return dlms_getMessages(settings, DLMS_COMMAND_DATA_NOTIFICATION, 0, data, date, messages);
}

int notify_generateDataNotificationMessages(
    dlmsSettings* settings,
    struct tm* date,
    gxArray* objects,
    message* messages)
{
    int pos, ret = 0;
    gxListItem *it;
    gxByteBuffer buff;
    bb_init(&buff);
    bb_setUInt8(&buff, DLMS_DATA_TYPE_STRUCTURE);
    hlp_setObjectCount(objects->size, &buff);
    for (pos = 0; pos != objects->size; ++pos)
    {
        if ((ret = arr_getByIndex(objects, pos, (void**)&it)) != 0)
        {
            return ret;
        }
        ret = notify_addData(settings, it->key, it->value, &buff);
        if (ret != 0)
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

#ifndef DLMS_IGNORE_PUSH_SETUP

int notify_generatePushSetupMessages(
    dlmsSettings* settings,
    struct tm* date,
    gxPushSetup* push,
    message* messages)
{
    int pos, ret = 0;
    gxKey *it;
    gxByteBuffer buff;
    if (push == NULL || messages == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    mes_clear(messages);
    bb_init(&buff);
    bb_setUInt8(&buff, DLMS_DATA_TYPE_STRUCTURE);
    hlp_setObjectCount(push->pushObjectList.size, &buff);
    for (pos = 0; pos != push->pushObjectList.size; ++pos)
    {
        if ((ret = arr_getByIndex(&push->pushObjectList, pos, (void**)&it)) != 0)
        {
            return ret;
        }
        ret = notify_addData(settings, (gxObject*)it->key, ((gxCaptureObject*)it->value)->attributeIndex, &buff);
        if (ret != 0)
        {
            break;
        }
    }
    if (ret == 0)
    {
        ret = notify_generateDataNotificationMessages2(settings, date, &buff, messages);
    }
    bb_clear(&buff);
    return ret;
}

int notify_parsePush(
    dlmsSettings* settings,
    variantArray* data,
    gxArray* items)
{
    gxListItem *k;
    gxObject *obj;
    unsigned char index;
    int classID, pos, ret;
    gxValueEventArg e;
    dlmsVARIANT *it, *list, *tmp;
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
                if ((ret = cosem_createObject((DLMS_OBJECT_TYPE)classID, &obj)) != 0)
                {
                    return ret;
                }
                memcpy(obj->logicalName, tmp->byteArr, 6);
                oa_push(&settings->objects, obj);
            }
            if ((ret = va_getByIndex(it->Arr, 2, &tmp)) != 0)
            {
                return ret;
            }
            index = (unsigned char)var_toInteger(tmp);
            arr_push(items, key_init(obj, (void*)(unsigned long long)index));
        }
    }
    ve_init(&e);
    for (pos = 0; pos != items->size; ++pos)
    {
        if ((ret = arr_getByIndex(items, pos, (void**)&k)) != 0)
        {
            return ret;
        }
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
#endif //DLMS_IGNORE_PUSH_SETUP
