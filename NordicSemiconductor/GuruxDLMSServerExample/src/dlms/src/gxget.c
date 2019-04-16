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
#include <string.h>
#include <math.h>

#include "../include/gxget.h"
#include "../include/gxarray.h"
#include "../include/bitarray.h"
#include "../include/dlms.h"
#include "../include/gxkey.h"
#include "../include/helpers.h"
#include "../include/serverevents.h"

int cosem_getByteBuffer(dlmsVARIANT* target)
{
    target->vt = DLMS_DATA_TYPE_OCTET_STRING;
    if (target->byteArr == NULL)
    {
        target->byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        if (target->byteArr == NULL)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        bb_init(target->byteArr);
    }
    return 0;
}

#ifndef DLMS_IGNORE_DATA
int cosem_getData(gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK;
    if (e->index == 2)
    {
        return var_copy(&e->value, &((gxData*)e->target)->value);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int cosem_getRegister(
    gxValueEventArg *e)
{
    if (e->index == 2)
    {
        return var_copy(&e->value, &((gxRegister*)e->target)->value);
    }
    else if (e->index == 3)
    {
        int ret;
        e->byteArray = 1;
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setInt8(data, ((gxRegister*)e->target)->scaler)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, ((gxRegister*)e->target)->unit)) != 0)
        {
            return ret;
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_REGISTER_TABLE
int cosem_getRegisterTable(
    gxValueEventArg *e)
{
    int ret;
    if (e->index == 2)
    {
        ret = DLMS_ERROR_CODE_OK;
        //TODO:  ret = var_copy(&e->value, &((gxRegisterTable*)e->target)->value);
    }
    else if (e->index == 4)
    {
        e->byteArray = 1;
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setInt8(data, ((gxRegisterTable*)e->target)->scaler)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, ((gxRegisterTable*)e->target)->unit)) != 0)
        {
            return ret;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_TABLE
#ifndef DLMS_IGNORE_CLOCK
int cosem_getClock(
    gxValueEventArg *e)
{
    int ret;
    if (e->index == 2)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &((gxClock*)e->target)->time);
    }
    else if (e->index == 3)
    {
        ret = var_setInt16(&e->value, ((gxClock*)e->target)->timeZone);
    }
    else if (e->index == 4)
    {
        ret = var_setUInt8(&e->value, ((gxClock*)e->target)->status);
    }
    else if (e->index == 5)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &((gxClock*)e->target)->begin);
    }
    else if (e->index == 6)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &((gxClock*)e->target)->end);
    }
    else if (e->index == 7)
    {
        ret = var_setInt8(&e->value, ((gxClock*)e->target)->deviation);
    }
    else if (e->index == 8)
    {
        ret = var_setBoolean(&e->value, ((gxClock*)e->target)->enabled);
    }
    else if (e->index == 9)
    {
        ret = var_setEnum(&e->value, ((gxClock*)e->target)->clockBase);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int cosem_getActionSchedule(
    gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxtime *tm;
    dlmsVARIANT tmp;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //Add LN.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(6, data)) != 0 ||
            (ret = bb_set(data, ((gxActionSchedule*)e->target)->executedScriptLogicalName, 6)) != 0 ||
            //Add executed script selector.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, ((gxActionSchedule*)e->target)->executedScriptSelector)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, ((gxActionSchedule*)e->target)->type);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(((gxActionSchedule*)e->target)->executionTime.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }

        for (pos = 0; pos != ((gxActionSchedule*)e->target)->executionTime.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = arr_getByIndex(&((gxActionSchedule*)e->target)->executionTime, pos, (void**)&tm)) != 0 ||
                (ret = var_setDateTime(&tmp, tm)) != 0)
            {
                break;
            }
            tmp.vt = DLMS_DATA_TYPE_TIME;
            ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data);
            if (ret != 0)
            {
                break;
            }
            tmp.vt = DLMS_DATA_TYPE_DATE;
            ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data);
            if (ret != 0)
            {
                break;
            }
            ret = var_clear(&tmp);
        }
        ret = var_clear(&tmp);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int getActivityCalendarDayProfileTable(gxArray* list, gxByteBuffer* ba)
{
    dlmsVARIANT tmp;
    gxDayProfile* dp;
    gxDayProfileAction *action;
    int ret, pos, pos2;
    if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        //Add count
        (ret = hlp_setObjectCount(list->size, ba)) != 0 ||
        (ret = var_init(&tmp)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != list->size; ++pos)
    {
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 2)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&dp)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, dp->dayId)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(dp->daySchedules.size, ba)) != 0)
        {
            return ret;
        }

        for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2)
        {
            if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(ba, 3)) != 0 ||
                (ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&action)) != 0 ||
                (ret = var_setTimeAsOctetString(&tmp, &action->startTime)) != 0 ||
                (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, ba)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(ba, 6)) != 0 ||
                (ret = bb_set(ba, action->scriptLogicalName, 6)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(ba, action->scriptSelector)) != 0)
                if (ret != 0)
                {
                    return ret;
                }
        }
    }
    var_clear(&tmp);
    return ret;
}

int getActivityCalendarWeekProfileTable(gxArray* list, gxByteBuffer* ba)
{
    int ret = 0, pos;
    gxWeekProfile* wp;
    if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        //Add count
        (ret = hlp_setObjectCount(list->size, ba)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != list->size; ++pos)
    {
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 8)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&wp)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, (unsigned char)wp->name.size)) != 0 ||
            (ret = bb_set2(ba, &wp->name, 0, bb_size(&wp->name))) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, wp->monday)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, wp->tuesday)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, wp->wednesday)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, wp->thursday)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, wp->friday)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, wp->saturday)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, wp->sunday)) != 0)
        {
            break;
        }
    }
    return ret;
}

int getActivityCalendarSeasonProfile(gxArray* list, gxByteBuffer* ba)
{
    int pos, ret = 0;
    gxSeasonProfile *sp;
    dlmsVARIANT tmp;
    if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        //Add count
        (ret = hlp_setObjectCount(list->size, ba)) != 0 ||
        (ret = var_init(&tmp)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != list->size; ++pos)
    {
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 3)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&sp)) != 0 ||
            //Add name.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, (unsigned char)bb_size(&sp->name))) != 0 ||
            (ret = bb_set2(ba, &sp->name, 0, bb_size(&sp->name))) != 0 ||
            //Add start time.
            (ret = var_setDateTime(&tmp, &sp->start)) != 0 ||
            (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, ba)) != 0 ||
            (ret = var_clear(&tmp)) != 0 ||
            //Add week day.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, (unsigned char)bb_size(&sp->weekName))) != 0 ||
            (ret = bb_set2(ba, &sp->weekName, 0, bb_size(&sp->weekName))) != 0)
        {
            break;
        }
    }
    var_clear(&tmp);
    return ret;
}

int getActivityCalendarName(gxByteBuffer* name, gxByteBuffer* ba)
{
    int ret;
    if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = hlp_setObjectCount(name->size, ba)) != 0 ||
        (ret = bb_set(ba, name->data, name->size)) != 0)
    {
        return ret;
    }
    return ret;
}

int cosem_getActivityCalendar(
    gxValueEventArg *e)
{
    int ret;
    if (e->index != 10)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
    }
    gxByteBuffer *data = e->value.byteArr;
    gxActivityCalendar* object = (gxActivityCalendar*)e->target;
    if (e->index == 2)
    {
        e->byteArray = 1;
        ret = getActivityCalendarName(&object->calendarNameActive, data);
    }
    else if (e->index == 3)
    {
        e->byteArray = 1;
        ret = getActivityCalendarSeasonProfile(&object->seasonProfileActive, data);
    }
    else if (e->index == 4)
    {
        e->byteArray = 1;
        ret = getActivityCalendarWeekProfileTable(&object->weekProfileTableActive, data);
    }
    else if (e->index == 5)
    {
        e->byteArray = 1;
        ret = getActivityCalendarDayProfileTable(&object->dayProfileTableActive, data);
    }
    else if (e->index == 6)
    {
        e->byteArray = 1;
        ret = getActivityCalendarName(&object->calendarNamePassive, data);
    }
    else if (e->index == 7)
    {
        e->byteArray = 1;
        ret = getActivityCalendarSeasonProfile(&object->seasonProfilePassive, data);
    }
    else if (e->index == 8)
    {
        e->byteArray = 1;
        ret = getActivityCalendarWeekProfileTable(&object->weekProfileTablePassive, data);
    }
    else if (e->index == 9)
    {
        e->byteArray = 1;
        ret = getActivityCalendarDayProfileTable(&object->dayProfileTablePassive, data);
    }
    else if (e->index == 10)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &object->time);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_SERVER
int getLNAccessRights(
    dlmsSettings* settings,
    gxObject *object,
    gxByteBuffer* data)
{
    unsigned char pos, cnt;
    int ret;
    unsigned char ch;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
        (ret = bb_setUInt8(data, 2)) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0)
    {
        return ret;
    }
    cnt = obj_attributeCount(object);
    hlp_setObjectCount(cnt, data);
    for (pos = 1; pos != cnt + 1; ++pos)
    {
        ch = (unsigned char)svr_getAttributeAccess(settings, object, pos);
        //attribute_access_item
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //Add index.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, pos)) != 0 ||
            //Add value.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, ch)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_NONE)) != 0)
        {
            return ret;
        }
    }
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0)
    {
        return ret;
    }
#ifdef INDIAN_STANDARD
    if ((ret = hlp_setObjectCount(0, data)) != 0)
    {
        return ret;
    }
#else
    cnt = obj_methodCount(object);
    if ((ret = hlp_setObjectCount(cnt, data)) != 0)
    {
        return ret;
    }
    for (pos = 1; pos != cnt + 1; ++pos)
    {
        ch = (unsigned char)svr_getMethodAccess(settings, object, pos);
        //attribute_access_item
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //Add index.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, pos)) != 0 ||
            //Add value.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, ch)) != 0)
        {
            return ret;
        }
    }
#endif //INDIAN_STANDARD
    return 0;
}
#endif //DLMS_IGNORE_SERVER

unsigned char dlms_isPduFull(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    unsigned char ret;
    if (bb_isAttached(data))
    {
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (settings->cipher.security != DLMS_SECURITY_NONE)
        {
            ret = data->size > (unsigned short)(bb_getCapacity(data) - PDU_MAX_HEADER_SIZE - CIPHERING_HEADER_SIZE);
        }
        else
#endif //DLMS_IGNORE_HIGH_GMAC
        {
            ret = data->size > (unsigned short)(bb_getCapacity(data) - PDU_MAX_HEADER_SIZE);
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}

#ifndef DLMS_IGNORE_SERVER
// Returns LN Association View.
int getLNObjects(
    dlmsSettings* settings,
    gxValueEventArg *e,
    gxByteBuffer* data)
{
    unsigned short pduSize;
    gxAssociationLogicalName *object = (gxAssociationLogicalName*)e->target;
    int ret;
    unsigned short pos;
    gxObject *it;
    if ((ret = bb_capacity(data, settings->maxPduSize)) != 0)
    {
        return ret;
    }
    //Add count only for first time.
    if (!e->transaction)
    {
        settings->count = object->objectList.size;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->objectList.size, data)) != 0)
        {
            return ret;
        }
    }
    for (pos = 0; pos != object->objectList.size; ++pos)
    {
        if (!(pos + 1 <= settings->index))
        {
            ret = oa_getByIndex(&object->objectList, pos, &it);
            if (ret != 0)
            {
                return ret;
            }
            pduSize = (unsigned short)data->size;
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                //Count
                (ret = bb_setUInt8(data, 4)) != 0 ||
                //ClassID
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, it->objectType)) != 0 ||
                //Version
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)it->version)) != 0 ||
                //LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, it->logicalName, 6)) != 0)
            {
                break;
            }
            //Access rights.
            ret = getLNAccessRights(settings, it, data);
            if (ret != 0)
            {
                break;
            }
            if (settings->server)
            {
                //If PDU is full.
                if (!e->skipMaxPduSize && dlms_isPduFull(settings, data))
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
            }
            ++settings->index;
        }
    }
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        data->size = pduSize;
        ret = 0;
    }
    return ret;
}
#endif //DLMS_IGNORE_SERVER

/*
* Returns User list.
*/
int getUserList(
    dlmsSettings* settings,
    gxValueEventArg *e,
    gxByteBuffer *data)
{
    unsigned char len, pos = 0;
    int ret;
    gxKey2* it;
    gxAssociationLogicalName* target = (gxAssociationLogicalName*)e->target;
    // Add count only for first time.
    if (settings->index == 0)
    {
        settings->count = target->userList.size;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(target->userList.size, data)) != 0)
        {
            return ret;
        }
    }
    for (pos = 0; pos != target->userList.size; ++pos)
    {
        if (!(pos + 1 <= settings->index))
        {
            if ((ret = arr_getByIndex(&target->userList, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                // Count
                (ret = bb_setUInt8(data, 2)) != 0 ||
                // Id
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->key)) != 0)
            {
                return ret;
            }
            ++settings->index;
            // Name
            len = (unsigned char)strlen((char*)it->value);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING)) != 0 ||
                (ret = hlp_setObjectCount(len, data)) != 0 ||
                (ret = bb_set(data, it->value, len)) != 0)
            {
                return ret;
            }
        }
    }
    return 0;
}

int cosem_getAssociationLogicalName(
    dlmsSettings* settings,
    gxValueEventArg *e)
{
    int ret = 0;
    if (e->index == 2)
    {
#ifndef DLMS_IGNORE_SERVER
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        ret = getLNObjects(settings, e, data);
#endif //DLMS_IGNORE_SERVER
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            //Add count
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->clientSAP)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, ((gxAssociationLogicalName*)e->target)->serverSAP)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            //Add count
            (ret = bb_setUInt8(data, 0x7)) != 0 ||
            (ret = bb_setUInt8(data, 0x60)) != 0 ||
            (ret = bb_setUInt8(data, 0x85)) != 0 ||
            (ret = bb_setUInt8(data, 0x74)) != 0 ||
            //Add data.
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->applicationContextName.identifiedOrganization)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->applicationContextName.dlmsUA)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->applicationContextName.applicationContext)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->applicationContextName.contextId)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        gxByteBuffer tmp;
        e->byteArray = 1;
        //Add count
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
            //Add data.
            (ret = bb_init(&tmp)) != 0 ||
            (ret = bb_setUInt32(&tmp, ((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.conformance)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = hlp_setObjectCount(3 * 8, data)) != 0 ||
            (ret = bb_set(data, tmp.data + 1, 3)) != 0 ||
            (ret = bb_clear(&tmp)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, ((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.maxReceivePduSize)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, ((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.maxSendPpuSize)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.dlmsVersionNumber)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.qualityOfService)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.cypheringInfo.size, data)) != 0 ||
            (ret = bb_set2(data, &((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.cypheringInfo, 0, ((gxAssociationLogicalName*)e->target)->xDLMSContextInfo.cypheringInfo.size)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 6)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            //Add count
            (ret = bb_setUInt8(data, 0x7)) != 0 ||
            //Add data.
            (ret = bb_setUInt8(data, 0x60)) != 0 ||
            (ret = bb_setUInt8(data, 0x85)) != 0 ||
            (ret = bb_setUInt8(data, 0x74)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->authenticationMechanismName.identifiedOrganization)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->authenticationMechanismName.dlmsUA)) != 0 ||
            (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->authenticationMechanismName.authenticationMechanismName)) != 0 ||
            (ret = bb_setUInt8(data, (unsigned char)((gxAssociationLogicalName*)e->target)->authenticationMechanismName.mechanismId)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 7)
    {
        ret = var_addOctetString(&e->value, &((gxAssociationLogicalName*)e->target)->secret);
    }
    else if (e->index == 8)
    {
        ret = var_setEnum(&e->value, ((gxAssociationLogicalName*)e->target)->associationStatus);
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
            (ret = bb_set(data, ((gxAssociationLogicalName*)e->target)->securitySetupReference, 6)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 10)
    {
        e->byteArray = 1;
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        ret = getUserList(settings, e, data);
    }
    else if (e->index == 11)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            // Add structure size.
            (ret = bb_setUInt8(data, 2)) != 0)
        {
            return ret;
        }
        if (((gxAssociationLogicalName*)e->target)->currentUser.value == NULL)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            char* str = (char*)((gxAssociationLogicalName*)e->target)->currentUser.value;
            unsigned char len = (unsigned char)strlen(str);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, ((gxAssociationLogicalName*)e->target)->currentUser.key)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING)) != 0 ||
                (ret = hlp_setObjectCount(len, data)) != 0 ||
                (ret = bb_set(data, (unsigned char*)str, len)) != 0)
            {
                return ret;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
// Returns SN Association View.
int getSNObjects(
    dlmsSettings* settings,
    gxValueEventArg *e,
    gxByteBuffer* ba)
{
    unsigned short pduSize;
    objectArray* list = &((gxAssociationShortName*)e->target)->objectList;
    unsigned short pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* it;
    if ((ret = bb_capacity(ba, settings->maxPduSize)) != 0)
    {
        return ret;
    }
    //Add count only for first time.
    if (!e->transaction)
    {
        settings->count = list->size;
        bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY);
        //Add count
        if ((ret = hlp_setObjectCount(list->size, ba)) != 0)
        {
            return ret;
        }
    }
    for (pos = 0; pos != list->size; ++pos)
    {
        if (!(pos + 1 <= settings->index))
        {
            pduSize = (unsigned short)ba->size;
            if ((ret = oa_getByIndex(list, pos, &it)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                //Count
                (ret = bb_setUInt8(ba, 4)) != 0 ||
                //Base address.
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_INT16)) != 0 ||
                (ret = bb_setInt16(ba, it->shortName)) != 0 ||
                //ClassID.
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(ba, it->objectType)) != 0 ||
                //Version.
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(ba, (unsigned char)it->version)) != 0 ||
                //LN.
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(ba, 6)) != 0 ||
                (ret = bb_set(ba, it->logicalName, 6)) != 0)
            {
                break;
            }
            if (settings->server)
            {
                //If PDU is full.
                if (dlms_isPduFull(settings, ba))
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
            }
            ++settings->index;
        }
    }
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        ba->size = pduSize;
        ret = 0;
    }
    return ret;
}

#ifndef DLMS_IGNORE_SERVER
int getSNAccessRights(
    dlmsSettings* settings,
    gxObject* object,
    gxByteBuffer* ba)
{
    int ret;
    unsigned char ch, pos, cnt;
    if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
        (ret = bb_setUInt8(ba, 3)) != 0 ||
        //Add SN.
        (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
        (ret = bb_setUInt16(ba, object->shortName)) != 0 ||
        //Add attribute access modes.
        (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0)
    {
        return ret;
    }
    cnt = obj_attributeCount(object);
    hlp_setObjectCount(cnt, ba);
    for (pos = 1; pos != cnt + 1; ++pos)
    {
        ch = (unsigned char)svr_getAttributeAccess(settings, object, pos);
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 3)) != 0 ||
            //Add index.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setInt8(ba, pos)) != 0 ||
            //Add access.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setInt8(ba, ch)) != 0 ||
            //Add None.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_NONE)) != 0)
        {
            return ret;
        }
    }
    //Add mthod access modes.
    cnt = obj_methodCount(object);
    bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY);
    hlp_setObjectCount(cnt, ba);
    for (pos = 1; pos != cnt + 1; ++pos)
    {
        ch = (unsigned char)svr_getMethodAccess(settings, object, pos);
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 3)) != 0 ||
            //Add index.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setInt8(ba, pos)) != 0 ||
            //Add access.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setInt8(ba, ch)) != 0)
        {
            return ret;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int cosem_getAssociationShortName(
    dlmsSettings* settings,
    gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK;
    unsigned short pos;
    gxObject* it;
    if ((ret = cosem_getByteBuffer(&e->value)) != 0)
    {
        return ret;
    }
    gxByteBuffer *data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = getSNObjects(settings, e, data);
    }
    else if (e->index == 3)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(((gxAssociationShortName*)e->target)->objectList.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != ((gxAssociationShortName*)e->target)->objectList.size; ++pos)
        {
            ret = oa_getByIndex(&((gxAssociationShortName*)e->target)->objectList, pos, &it);
            if (ret != 0)
            {
                break;
            }
            ret = getSNAccessRights(settings, it, data);
            if (ret != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
            (ret = bb_set(data, ((gxAssociationShortName*)e->target)->securitySetupReference, 6)) != 0)
        {
            return ret;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    e->byteArray = 1;
    return ret;
}
#endif //DLMS_IGNORE_SERVER
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_AUTO_ANSWER
int cosem_getAutoAnswer(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    dlmsVARIANT tmp;
    gxKey* kv;
    gxAutoAnswer* object = (gxAutoAnswer*)e->target;
    if (e->index == 2)
    {
        return var_setEnum(&e->value, object->mode);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->listeningWindow.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->listeningWindow.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&kv)) != 0)
            {
                var_clear(&tmp);
                return ret;
            }
            //start_time
            if ((ret = var_setDateTime(&tmp, (gxtime*)kv->key)) != 0 ||
                (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0)
            {
                var_clear(&tmp);
                return ret;
            }
            var_clear(&tmp);
            //end_time
            if ((ret = var_setDateTime(&tmp, (gxtime*)kv->value)) != 0 ||
                (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0)
            {
                var_clear(&tmp);
                return ret;
            }
        }
        var_clear(&tmp);
    }
    else if (e->index == 4)
    {
        ret = var_setEnum(&e->value, object->status);
    }
    else if (e->index == 5)
    {
        ret = var_setUInt8(&e->value, object->numberOfCalls);
    }
    else if (e->index == 6)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, object->numberOfRingsInListeningWindow)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, object->numberOfRingsOutListeningWindow)) != 0)
        {
            return ret;
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
int cosem_getAutoConnect(
    gxValueEventArg *e)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxByteBuffer *ba;
    gxKey *k;
    dlmsVARIANT tmp;
    gxAutoConnect* object = (gxAutoConnect*)e->target;
    if (e->index == 2)
    {
        ret = var_setEnum(&e->value, object->mode);
    }
    else if (e->index == 3)
    {
        ret = var_setUInt8(&e->value, object->repetitions);
    }
    else if (e->index == 4)
    {
        ret = var_setUInt16(&e->value, object->repetitionDelay);
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->callingWindow.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->callingWindow.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k)) != 0 ||
                (ret = var_setDateTime(&tmp, (gxtime*)k->key)) != 0 ||
                (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                (ret = var_setDateTime(&tmp, (gxtime*)k->value)) != 0 ||
                (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0)
            {
                break;
            }
        }
        var_clear(&tmp);
    }
    else if (e->index == 6)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->destinations.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->destinations.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = arr_getByIndex(&object->destinations, pos, (void**)&ba)) != 0 ||
                (ret = hlp_setObjectCount(ba->size, data)) != 0 ||
                (ret = bb_set(data, ba->data, ba->size)) != 0)
            {
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
int cosem_getDemandRegister(
    gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxDemandRegister* object = (gxDemandRegister*)e->target;
    if (e->index == 2)
    {
        ret = var_setUInt32(&e->value, (long)(var_toDouble(&object->currentAvarageValue) / hlp_getScaler(object->scaler)));
    }
    else if (e->index == 3)
    {
        ret = var_setUInt32(&e->value, (long)(var_toDouble(&object->lastAvarageValue) / hlp_getScaler(object->scaler)));
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setInt8(data, object->scaler)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->unit)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 5)
    {
        ret = var_copy(&e->value, &object->status);
    }
    else if (e->index == 6)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &object->captureTime);
    }
    else if (e->index == 7)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &object->startTimeCurrent);
    }
    else if (e->index == 8)
    {
        ret = var_setUInt32(&e->value, object->period);
    }
    else if (e->index == 9)
    {
        ret = var_setUInt16(&e->value, object->numberOfPeriods);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int cosem_getMacAddressSetup(
    gxValueEventArg *e)
{
    int ret = 0;
    gxMacAddressSetup* object = (gxMacAddressSetup*)e->target;
    if (e->index == 2)
    {
        ret = var_addOctetString(&e->value, &object->macAddress);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int cosem_getExtendedRegister(
    gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT tmp;
    gxExtendedRegister* object = (gxExtendedRegister*)e->target;
    if (e->index == 2)
    {
        ret = var_copy(&e->value, &object->base.value);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setInt8(data, object->base.scaler)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->base.unit)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 4)
    {
        ret = var_copy(&e->value, &object->status);
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        var_init(&tmp);
        ret = var_setDateTime(&tmp, &object->captureTime);
        if (ret == 0)
        {
            e->byteArray = 1;
            ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data);
        }
        var_clear(&tmp);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

#ifndef DLMS_IGNORE_GPRS_SETUP
int cosem_getQualityOfService(gxQualityOfService* service, gxByteBuffer* ba)
{
    int ret;
    if ((ret = bb_setUInt8(ba, 5)) != 0 ||
        //Precedence
        (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(ba, service->precedence)) != 0 ||
        //Delay
        (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(ba, service->delay)) != 0 ||
        //Reliability
        (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(ba, service->reliability)) != 0 ||
        //PeakThroughput
        (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(ba, service->peakThroughput)) != 0 ||
        //MeanThroughput
        (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(ba, service->meanThroughput)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getGprsSetup(
    gxValueEventArg *e)
{
    int ret;
    gxGPRSSetup* object = (gxGPRSSetup*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        ret = var_addOctetString(&e->value, &object->apn);
    }
    else if (e->index == 3)
    {
        ret = var_setUInt16(&e->value, object->pinCode);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = cosem_getQualityOfService(&object->defaultQualityOfService, data)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = cosem_getQualityOfService(&object->requestedQualityOfService, data)) != 0)
        {
            return ret;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
int cosem_getSecuritySetup(
    gxValueEventArg *e)
{
    int ret;
    gxSecuritySetup* object = (gxSecuritySetup*)e->target;
    if (e->index == 2)
    {
        ret = var_setEnum(&e->value, object->securityPolicy);
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, object->securitySuite);
    }
    else if (e->index == 4)
    {
        ret = var_addOctetString(&e->value, &object->clientSystemTitle);
    }
    else if (e->index == 5)
    {
        ret = var_addOctetString(&e->value, &object->serverSystemTitle);
    }
    else if (e->index == 6)
    {
        gxCertificateInfo* it;
        int pos, len;
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->certificates.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->certificates.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->certificates, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = hlp_setObjectCount(6, data)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
                (ret = bb_setUInt8(data, it->entity)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
                (ret = bb_setUInt8(data, it->type)) != 0)
            {
                break;
            }
            len = (int)strlen(it->serialNumber);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, len)) != 0 ||
                (ret = bb_set(data, (unsigned char*)it->serialNumber, len)) != 0)
            {
                break;
            }
            len = (int)strlen(it->issuer);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, len)) != 0 ||
                (ret = bb_set(data, (unsigned char*)it->issuer, len)) != 0)
            {
                break;
            }
            len = (int)strlen(it->subject);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, len)) != 0 ||
                (ret = bb_set(data, (unsigned char*)it->subject, len)) != 0)
            {
                break;
            }
            len = (int)strlen(it->subjectAltName);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, len)) != 0 ||
                (ret = bb_set(data, (unsigned char*)it->subjectAltName, len)) != 0)
            {
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int cosem_getIecHdlcSetup(
    gxValueEventArg *e)
{
    int ret;
    gxIecHdlcSetup* object = (gxIecHdlcSetup*)e->target;
    switch (e->index)
    {
    case 2:
        ret = var_setEnum(&e->value, object->communicationSpeed);
        break;
    case 3:
        ret = var_setUInt8(&e->value, object->windowSizeTransmit);
        break;
    case 4:
        ret = var_setUInt8(&e->value, object->windowSizeReceive);
        break;
    case 5:
        ret = var_setUInt16(&e->value, object->maximumInfoLengthTransmit);
        break;
    case 6:
        ret = var_setUInt16(&e->value, object->maximumInfoLengthReceive);
        break;
    case 7:
        ret = var_setUInt16(&e->value, object->interCharachterTimeout);
        break;
    case 8:
        ret = var_setUInt16(&e->value, object->inactivityTimeout);
        break;
    case 9:
        ret = var_setUInt16(&e->value, object->deviceAddress);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int cosem_getIecLocalPortSetup(
    gxValueEventArg *e)
{
    int ret;
    gxLocalPortSetup* object = (gxLocalPortSetup*)e->target;
    if (e->index == 2)
    {
        ret = var_setEnum(&e->value, object->defaultMode);
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, object->defaultBaudrate);
    }
    else if (e->index == 4)
    {
        ret = var_setEnum(&e->value, object->proposedBaudrate);
    }
    else if (e->index == 5)
    {
        ret = var_setEnum(&e->value, object->responseTime);
    }
    else if (e->index == 6)
    {
        ret = var_addOctetString(&e->value, &object->deviceAddress);
    }
    else if (e->index == 7)
    {
        ret = var_addOctetString(&e->value, &object->password1);
    }
    else if (e->index == 8)
    {
        ret = var_addOctetString(&e->value, &object->password2);
    }
    else if (e->index == 9)
    {
        ret = var_addOctetString(&e->value, &object->password5);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif // DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
int cosem_getIP4Setup(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    dlmsVARIANT *tmp;
    gxip4SetupIpOption* it;
    gxIp4Setup* object = (gxIp4Setup*)e->target;
    if (e->index == 2)
    {
        ret = var_addOctetString(&e->value, &object->dataLinkLayerReference);
    }
    else if (e->index == 3)
    {
        ret = var_setUInt32(&e->value, object->ipAddress);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->multicastIPAddress.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
        {
            if ((ret = va_getByIndex(&object->multicastIPAddress, pos, &tmp)) != 0 ||
                (ret = var_getBytes(tmp, data)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->ipOptions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->ipOptions.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->ipOptions, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Type
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->type)) != 0 ||
                //Len
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)it->length)) != 0 ||
                //Data
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)bb_size(&it->data))) != 0 ||
                (ret = bb_set2(data, &it->data, 0, bb_size(&it->data))) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 6)
    {
        ret = var_setUInt32(&e->value, object->subnetMask);
    }
    else if (e->index == 7)
    {
        ret = var_setUInt32(&e->value, object->gatewayIPAddress);
    }
    else if (e->index == 8)
    {
        ret = var_setBoolean(&e->value, object->useDHCP);
    }
    else if (e->index == 9)
    {
        ret = var_setUInt32(&e->value, object->primaryDNSAddress);
    }
    else if (e->index == 10)
    {
        ret = var_setUInt32(&e->value, object->secondaryDNSAddress);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int cosem_getMbusSlavePortSetup(
    gxValueEventArg *e)
{
    int ret;
    gxMbusSlavePortSetup* object = (gxMbusSlavePortSetup*)e->target;
    if (e->index == 2)
    {
        ret = var_setEnum(&e->value, object->defaultBaud);
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, object->availableBaud);
    }
    else if (e->index == 4)
    {
        ret = var_setEnum(&e->value, object->addressState);
    }
    else if (e->index == 5)
    {
        ret = var_setUInt8(&e->value, object->busAddress);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int cosem_getImageTransfer(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    gxImageActivateInfo *ai;
    gxImageTransfer* object = (gxImageTransfer*)e->target;
    if (e->index == 2)
    {
        ret = var_setUInt32(&e->value, object->imageBlockSize);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        e->value.byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        bb_init(e->value.byteArr);
        e->byteArray = 1;
        if ((ret = bb_setUInt8(e->value.byteArr, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->imageTransferredBlocksStatus.size, e->value.byteArr)) != 0)
        {
            return ret;
        }
        if (object->imageTransferredBlocksStatus.size != 0)
        {
            ret = bb_set(e->value.byteArr, object->imageTransferredBlocksStatus.data,
                ba_getByteCount(object->imageTransferredBlocksStatus.size));
        }
    }
    else if (e->index == 4)
    {
        ret = var_setUInt32(&e->value, object->imageFirstNotTransferredBlockNumber);
    }
    else if (e->index == 5)
    {
        ret = var_setBoolean(&e->value, object->imageTransferEnabled);
    }
    else if (e->index == 6)
    {
        ret = var_setEnum(&e->value, object->imageTransferStatus);
    }
    else if (e->index == 7)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        //ImageActivateInfo is returned only after verification is succeeded.
        if (object->imageTransferStatus != DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = hlp_setObjectCount(object->imageActivateInfo.size, data)) != 0)
            {
                return ret;
            }
            for (pos = 0; pos != object->imageActivateInfo.size; ++pos)
            {
                if ((ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&ai)) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 3)) != 0 ||
                    //Size
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
                    (ret = bb_setUInt32(data, ai->size)) != 0 ||
                    //Add identification.
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(ai->identification.size, data)) != 0 ||
                    (ret = bb_set(data, ai->identification.data, ai->identification.size)) != 0 ||
                    //Add signature.
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(ai->signature.size, data)) != 0 ||
                    (ret = bb_set(data, ai->signature.data, ai->signature.size)) != 0)
                {
                    break;
                }
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER

#if !defined(DLMS_IGNORE_PROFILE_GENERIC) && !defined(DLMS_IGNORE_CONPACT_DATA)
int getColumns(
    dlmsSettings* settings,
    gxArray *list,
    gxByteBuffer *ba,
    gxValueEventArg *e)
{
    unsigned short pduSize;
    int pos, ret;
    gxKey *it;
    if ((ret = bb_capacity(ba, (list->size * 19) + 2)) != 0)
    {
        return ret;
    }
    //Add count only for first time.
    if (!e->transaction)
    {
        settings->count = list->size;
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(list->size, ba)) != 0)
        {
            return ret;
        }
    }
    for (pos = 0; pos != list->size; ++pos)
    {
        if (!(pos + 1 <= settings->index))
        {
            pduSize = (unsigned short)ba->size;
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(ba, 4)) != 0 ||
                //ClassID
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(ba, ((gxObject*)it->key)->objectType)) != 0 ||
                //LN
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(ba, 6)) != 0 ||
                (ret = bb_set(ba, ((gxObject*)it->key)->logicalName, 6)) != 0 ||
                //Attribute Index
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setInt8(ba, ((gxCaptureObject*)it->value)->attributeIndex)) != 0 ||
                //Data Index
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(ba, ((gxCaptureObject*)it->value)->dataIndex)) != 0)
            {
                break;
            }
            if (settings->server)
            {
                //If PDU is full.
                if (dlms_isPduFull(settings, ba))
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
            }
            ++settings->index;
        }
    }
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        ba->size = pduSize;
        ret = 0;
    }
    return ret;
}

/**
     * Get selected columns.
     *
     * @param cols
     *            selected columns.
     * @param columns
     *            Selected columns.
     */
int getSelectedColumns(
    gxArray* captureObjects,
    variantArray *cols,
    gxArray *columns)
{
    gxKey *c;
    dlmsVARIANT *it, *it2;
    unsigned char *ln;
    DLMS_OBJECT_TYPE ot;
    int ret, pos, pos2, dataIndex;
    unsigned char attributeIndex;
    for (pos = 0; pos != cols->size; ++pos)
    {
        if ((ret = va_getByIndex(cols, pos, &it)) != 0 ||
            //Get object type.
            (ret = va_getByIndex(it->Arr, 0, &it2)) != 0)
        {
            break;
        }
        ot = (DLMS_OBJECT_TYPE)var_toInteger(it2);
        //Get logical name.
        if ((ret = va_getByIndex(it->Arr, 1, &it2)) != 0)
        {
            break;
        }
        ln = it2->byteArr->data;
        //Get attribute index.
        if ((ret = va_getByIndex(it->Arr, 2, &it2)) != 0)
        {
            break;
        }
        attributeIndex = (char)var_toInteger(it2);
        //Get data index.
        if ((ret = va_getByIndex(it->Arr, 3, &it2)) != 0)
        {
            break;
        }
        dataIndex = var_toInteger(it2);
        // Find columns and update only them.
        for (pos2 = 0; pos2 != captureObjects->size; ++pos2)
        {
            if ((ret = arr_getByIndex(captureObjects, pos2, (void**)&c)) != 0)
            {
                return ret;
            }
            if (((gxObject*)c->key)->objectType == ot &&
                ((gxCaptureObject*)c->value)->attributeIndex == attributeIndex &&
                ((gxCaptureObject*)c->value)->dataIndex == dataIndex &&
                memcmp(((gxObject*)c->key)->logicalName, ln, 6) == 0)
            {
                if ((ret = arr_push(columns, c)) != 0)
                {
                    return ret;
                }
                break;
            }
        }
    }
    return 0;
}

int cosem_getColumns(
    gxArray* captureObjects,
    unsigned char selector,
    dlmsVARIANT * parameters,
    gxArray *columns)
{
    unsigned long pos, start = 0, count = 0;
    unsigned short addAllColumns = 1;
    int ret = 0;
    gxKey *k;
    dlmsVARIANT *it;
    arr_empty(columns);
    if (parameters->vt == DLMS_DATA_TYPE_STRUCTURE)
    {
        if (selector == 1) //Read by range
        {
            if (parameters->Arr->size > 3)
            {
                if ((ret = va_getByIndex(parameters->Arr, 3, &it)) == 0)
                {
                    ret = getSelectedColumns(captureObjects, it->Arr, columns);
                }
                addAllColumns = 0;
            }
        }
        else if (selector == 2) //Read by entry.
        {
            if (parameters->Arr->size > 2)
            {
                if ((ret = va_getByIndex(parameters->Arr, 2, &it)) != 0)
                {
                    return ret;
                }
                start = var_toInteger(it);
            }
            if (parameters->Arr->size > 3)
            {
                if ((ret = va_getByIndex(parameters->Arr, 3, &it)) != 0)
                {
                    return ret;
                }
                count = var_toInteger(it);
            }
            else if (start != 1)
            {
                count = captureObjects->size;
            }
            if (start != 1 || count != 0)
            {
                addAllColumns = 0;
                if (count == 0)
                {
                    count = captureObjects->size - start;
                }
                for (pos = start - 1; pos != count; ++pos)
                {
                    if ((ret = arr_getByIndex(captureObjects, pos, (void**)&k)) != 0 ||
                        (ret = arr_push(columns, k)) != 0)
                    {
                        return ret;
                    }
                }
            }
        }
        //Add all objects.
        if (addAllColumns)
        {
            for (pos = 0; pos != captureObjects->size; ++pos)
            {
                if ((ret = arr_getByIndex(captureObjects, pos, (void**)&k)) != 0)
                {
                    break;
                }
                if ((ret = arr_push(columns, k)) != 0)
                {
                    return ret;
                }
            }
        }
    }
    return ret;
}

int cosem_getRow(
    gxArray* table,
    int index,
    gxArray *captureObjects,
    gxArray* columns,
    gxByteBuffer* data)
{
    int ret, pos;
    dlmsVARIANT *col;
    variantArray *row;
    if ((ret = arr_getByIndex(table, index, (void**)&row)) != 0)
    {
        return ret;
    }
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0)
    {
        return ret;
    }
    if (columns->size != 0)
    {
        if ((ret = hlp_setObjectCount(columns->size, data)) != 0)
        {
            return ret;
        }
    }
    else
    {
        if ((ret = hlp_setObjectCount(captureObjects->size, data)) != 0)
        {
            return ret;
        }
    }
    int colPos = 0;
    for (pos = 0; pos != row->size; ++pos)
    {
        if (columns->size != 0)
        {
            if (captureObjects->data[pos] != columns->data[colPos])
            {
                continue;
            }
            ++colPos;
        }
        if ((ret = va_getByIndex(row, pos, &col)) != 0)
        {
            return ret;
        }
        if (col->vt == DLMS_DATA_TYPE_DATETIME)
        {
            if ((ret = var_getBytes2(col, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = dlms_setData(data, col->vt, col)) != 0)
            {
                return ret;
            }
        }
    }
    return 0;
}

int profileGeneric_getData(
    dlmsSettings* settings,
    gxValueEventArg *e,
    gxArray *table,
    gxArray *captureObjects,
    gxByteBuffer* data)
{
    gxArray columns;
    unsigned short pduSize;
    int ret = 0, pos;
    //Add count only for first time.
    if (!e->transaction)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0)
        {
            return ret;
        }
        if (e->transactionEndIndex != 0)
        {
            settings->count = e->transactionEndIndex - e->transactionStartIndex;
            if ((ret = hlp_setObjectCount(settings->count, data)) != 0)
            {
                return ret;
            }
        }
        else
        {
            settings->count = table->size;
            if ((ret = hlp_setObjectCount(table->size, data)) != 0)
            {
                return ret;
            }
        }
    }
    arr_init(&columns);
    if (e->selector == 2)
    {
        if ((ret = cosem_getColumns(captureObjects, e->selector, &e->parameters, &columns)) != 0)
        {
            return ret;
        }
    }
    for (pos = 0; pos != table->size; ++pos)
    {
        pduSize = (unsigned short)data->size;
        if ((ret = cosem_getRow(table, pos, captureObjects, &columns, data)) != 0)
        {
            break;
        }
        //If PDU is full.
        if (data->size > (unsigned short)(settings->maxPduSize - PDU_MAX_HEADER_SIZE))
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
            break;
        }
        ++settings->index;
    }
    arr_empty(&columns);
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        data->size = pduSize;
        ret = 0;
    }
    if (e->transactionEndIndex != 0)
    {
        e->transactionStartIndex += (unsigned short)pos;
    }
    return ret;
}

int getProfileGenericData(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    gxValueEventArg *e,
    gxByteBuffer* reply)
{
    int ret;
    gxArray items;
    variantArray *row;
    dlmsVARIANT *it;
    gxByteBuffer bb;
    gxArray captureObjects;
    //If all data is read.
    if (e->selector == 0 || e->parameters.vt == DLMS_DATA_TYPE_NONE || e->transactionEndIndex != 0)
    {
        return profileGeneric_getData(settings, e, &object->buffer, &object->captureObjects, reply);
    }
    arr_init(&captureObjects);
    if ((ret = cosem_getColumns(&object->captureObjects, e->selector, &e->parameters, &captureObjects)) == 0)
    {
        //Do not clear items.
        arr_init(&items);
        if (e->selector == 1) //Read by range
        {
            unsigned short cnt = 0;
            gxtime start, end;
            dlmsVARIANT value;
            var_init(&value);
            if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) == 0)
            {
                if (it->vt == DLMS_DATA_TYPE_UINT32)
                {
                    time_init4(&start, (time_t)it->ulVal);
                }
                else
                {
                    if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &value)) != 0)
                    {
                        arr_empty(&items);
                        var_clear(&value);
                        return ret;
                    }
                    start = *value.dateTime;
                    var_clear(&value);
                }
                if ((ret = va_getByIndex(e->parameters.Arr, 2, &it)) != 0)
                {
                    return ret;
                }
                if (it->vt == DLMS_DATA_TYPE_UINT32)
                {
                    time_init4(&end, (time_t)it->ulVal);
                }
                else
                {
                    if ((ret = dlms_changeType(it->byteArr, DLMS_DATA_TYPE_DATETIME, &value)) != 0)
                    {
                        arr_empty(&items);
                        var_clear(&value);
                        return ret;
                    }
                    end = *value.dateTime;
                    var_clear(&value);
                }
                for (; settings->position < object->buffer.size; ++settings->position)
                {
                    if ((ret = arr_getByIndex(&object->buffer, settings->position, (void**)&row)) != 0 ||
                        (ret = va_getByIndex(row, 0, &it)) != 0)
                    {
                        break;
                    }
                    if (time_compare(&start, it->dateTime) != -1 && time_compare(it->dateTime, &end) != -1)
                    {
                        ++cnt;
                        //If Size of PDU is full.
                        if (reply->size < settings->maxPduSize)
                        {
                            ++settings->index;
                            if ((ret = cosem_getRow(&object->buffer, settings->position, &captureObjects, &object->captureObjects, reply)) != 0)
                            {
                                break;
                            }
                        }
                        else if (settings->count != 0)
                        {
                            ++settings->position;
                            //We must loop items throught if count is unknown.
                            break;
                        }
                    }
                }
                //Update count if items are read first time.
                if (ret == 0 && settings->count == 0)
                {
                    settings->count = cnt;
                    if ((ret = bb_init(&bb)) != 0 ||
                        (ret = bb_setUInt8(&bb, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                        (ret = hlp_setObjectCount(cnt, &bb)) != 0 ||
                        (ret = bb_set(&bb, reply->data, reply->size)) != 0)
                    {
                        return ret;
                    }
                    reply->position = reply->size = 0;
                    if ((ret = bb_set(reply, bb.data, bb.size)) != 0 ||
                        (ret = bb_clear(&bb)) != 0)
                    {
                        return ret;
                    }
                }
            }
        }
        else if (e->selector == 2) //Read by entry.
        {
            int start, count;
            if ((ret = va_getByIndex(e->parameters.Arr, 0, &it)) == 0)
            {
                start = var_toInteger(it);
                if (start == 0)
                {
                    start = 1;
                }
                if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) == 0)
                {
                    count = var_toInteger(it);
                    if (count == 0)
                    {
                        count = object->buffer.size;
                    }
                    if (start + count > object->buffer.size + 1)
                    {
                        count = object->buffer.size;
                    }
                    if (settings->count == 0)
                    {
                        settings->count = (unsigned short)count;
                        if ((ret = bb_setUInt8(reply, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                            (ret = hlp_setObjectCount(settings->count, reply)) != 0)
                        {
                            return ret;
                        }
                    }
                    for (; settings->position < count; ++settings->position)
                    {
                        if ((ret = arr_getByIndex(&object->buffer, settings->position + start - 1, (void**)&row)) != 0 ||
                            (ret = cosem_getRow(&object->buffer, settings->position, &captureObjects, &object->captureObjects, reply)) != 0)
                        {
                            break;
                        }
                        ++settings->index;
                    }
                }
            }
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    arr_empty(&captureObjects);
    arr_empty(&items);
    return ret;
}

int cosem_getProfileGeneric(
    dlmsSettings* settings,
    gxValueEventArg *e)
{
    unsigned char empty[6] = { 0 };
    int ret = 0;
    gxProfileGeneric* object = (gxProfileGeneric*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        ret = getProfileGenericData(settings, object, e, data);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        ret = getColumns(settings, &object->captureObjects, data, e);
    }
    else if (e->index == 4)
    {
        ret = var_setUInt32(&e->value, object->capturePeriod);
    }
    else if (e->index == 5)
    {
        ret = var_setEnum(&e->value, object->sortMethod);
    }
    else  if (e->index == 6)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 4)) != 0)
        {
            return ret;
        }
        if (object->sortObject == NULL)
        {
            //ClassID
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, 0)) != 0 ||
                //LN
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, empty, 6)) != 0 ||
                //Selected Attribute Index
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0 ||
                //Selected Data Index
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            //ClassID
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, object->sortObject->objectType)) != 0 ||
                //LN
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, object->sortObject->logicalName, 6)) != 0 ||
                //Selected Attribute Index
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, object->sortObjectAttributeIndex)) != 0 ||
                //Selected Data Index
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, object->sortObjectDataIndex)) != 0)
            {
                return ret;
            }
        }
    }
    else if (e->index == 7)
    {
        ret = var_setUInt32(&e->value, object->entriesInUse);
    }
    else if (e->index == 8)
    {
        ret = var_setUInt32(&e->value, object->profileEntries);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int cosem_getDisconnectControl(
    gxValueEventArg *e)
{
    int ret = 0;
    gxDisconnectControl* object = (gxDisconnectControl*)e->target;
    if (e->index == 2)
    {
        ret = var_setBoolean(&e->value, object->outputState);
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, object->controlState);
    }
    else if (e->index == 4)
    {
        ret = var_setEnum(&e->value, object->controlMode);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int cosem_getLimiter(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    unsigned char empty[6] = { 0 };
    dlmsVARIANT tmp;
    dlmsVARIANT *it;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //objectType
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0)
        {
            return ret;
        }
        if (((gxLimiter*)e->target)->monitoredValue == NULL)
        {
            if ((ret = bb_setUInt16(data, 0)) != 0 ||
                //LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, empty, 6)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = bb_setUInt16(data, ((gxLimiter*)e->target)->monitoredValue->objectType)) != 0 ||
                //LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, ((gxLimiter*)e->target)->monitoredValue->logicalName, 6)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, ((gxLimiter*)e->target)->selectedAttributeIndex)) != 0)
            {
                return ret;
            }
        }
    }
    else if (e->index == 3)
    {
        ret = var_copy(&e->value, &((gxLimiter*)e->target)->thresholdActive);
    }
    else if (e->index == 4)
    {
        ret = var_copy(&e->value, &((gxLimiter*)e->target)->thresholdNormal);
    }
    else if (e->index == 5)
    {
        ret = var_copy(&e->value, &((gxLimiter*)e->target)->thresholdEmergency);
    }
    else if (e->index == 6)
    {
        ret = var_setUInt32(&e->value, ((gxLimiter*)e->target)->minOverThresholdDuration);
    }
    else if (e->index == 7)
    {
        ret = var_setUInt32(&e->value, ((gxLimiter*)e->target)->minUnderThresholdDuration);
    }
    else if (e->index == 8)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //Id
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, ((gxLimiter*)e->target)->emergencyProfile.id)) != 0 ||
            //Date
            (ret = var_init(&tmp)) != 0 ||
            (ret = var_setDateTime(&tmp, &((gxLimiter*)e->target)->emergencyProfile.activationTime)) != 0 ||
            (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0 ||
            (ret = var_clear(&tmp)) != 0 ||
            //Duration
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
            (ret = bb_setUInt32(data, ((gxLimiter*)e->target)->emergencyProfile.duration)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_clear(data)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(((gxLimiter*)e->target)->emergencyProfileGroupIDs.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != ((gxLimiter*)e->target)->emergencyProfileGroupIDs.size; ++pos)
        {
            if ((ret = va_getByIndex(&((gxLimiter*)e->target)->emergencyProfileGroupIDs, pos, &it)) != 0 ||
                (ret = var_getBytes(it, data)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 10)
    {
        ret = var_setBoolean(&e->value, ((gxLimiter*)e->target)->emergencyProfileActive);
    }
    else if (e->index == 11)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //Action over threshold
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //LN
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
            (ret = bb_set(data, ((gxLimiter*)e->target)->actionOverThreshold.logicalName, 6)) != 0 ||
            //Script selector
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, ((gxLimiter*)e->target)->actionOverThreshold.scriptSelector)) != 0 ||
            //Action under threshold
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //LN
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
            (ret = bb_set(data, ((gxLimiter*)e->target)->actionUnderThreshold.logicalName, 6)) != 0 ||
            //Script selector
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, ((gxLimiter*)e->target)->actionUnderThreshold.scriptSelector)) != 0)
        {
            return ret;
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int cosem_getmMbusClient(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    gxKey *it;
    if (e->index == 2)
    {
        ret = var_addOctetString(&e->value, &((gxMBusClient*)e->target)->mBusPortReference);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(((gxMBusClient*)e->target)->captureDefinition.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != ((gxMBusClient*)e->target)->captureDefinition.size; ++pos)
        {
            if ((ret = arr_getByIndex(&((gxMBusClient*)e->target)->captureDefinition, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(((gxByteBuffer*)it->key)->size, data)) != 0 ||
                (ret = bb_set2(data, (gxByteBuffer*)it->key, 0, bb_size((gxByteBuffer*)it->key))) != 0 ||
                //
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(((gxByteBuffer*)it->value)->size, data)) != 0 ||
                (ret = bb_set2(data, (gxByteBuffer*)it->value, 0, bb_size((gxByteBuffer*)it->value))) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 4)
    {
        ret = var_setUInt32(&e->value, ((gxMBusClient*)e->target)->capturePeriod);
    }
    else if (e->index == 5)
    {
        ret = var_setUInt8(&e->value, ((gxMBusClient*)e->target)->primaryAddress);
    }
    else if (e->index == 6)
    {
        ret = var_setUInt32(&e->value, ((gxMBusClient*)e->target)->identificationNumber);
    }
    else if (e->index == 7)
    {
        ret = var_setUInt16(&e->value, ((gxMBusClient*)e->target)->manufacturerID);
    }
    else if (e->index == 8)
    {
        ret = var_setUInt8(&e->value, ((gxMBusClient*)e->target)->dataHeaderVersion);
    }
    else if (e->index == 9)
    {
        ret = var_setUInt8(&e->value, ((gxMBusClient*)e->target)->deviceType);
    }
    else if (e->index == 10)
    {
        ret = var_setUInt8(&e->value, ((gxMBusClient*)e->target)->accessNumber);
    }
    else if (e->index == 11)
    {
        ret = var_setUInt8(&e->value, ((gxMBusClient*)e->target)->status);
    }
    else if (e->index == 12)
    {
        ret = var_setUInt8(&e->value, ((gxMBusClient*)e->target)->alarm);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int cosem_getModemConfiguration(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    gxModemInitialisation *mi;
    gxByteBuffer *ba;
    if (e->index == 2)
    {
        ret = var_setEnum(&e->value, ((gxModemConfiguration*)e->target)->communicationSpeed);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(((gxModemConfiguration*)e->target)->initialisationStrings.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != ((gxModemConfiguration*)e->target)->initialisationStrings.size; ++pos)
        {
            if ((ret = arr_getByIndex(&((gxModemConfiguration*)e->target)->initialisationStrings, pos, (void**)&mi)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Add request.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(mi->request.size, data)) != 0 ||
                (ret = bb_set(data, mi->request.data, mi->request.size)) != 0 ||
                //Add response.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(mi->response.size, data)) != 0 ||
                (ret = bb_set(data, mi->response.data, mi->response.size)) != 0 ||
                //Add delay.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, mi->delay)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(((gxModemConfiguration*)e->target)->modemProfile.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != ((gxModemConfiguration*)e->target)->modemProfile.size; ++pos)
        {
            if ((ret = arr_getByIndex(&((gxModemConfiguration*)e->target)->modemProfile, pos, (void**)&ba)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(ba->size, data)) != 0 ||
                (ret = bb_set(data, ba->data, ba->size)) != 0)
            {
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
int cosem_getPppSetup(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    gxpppSetupLcpOption* lcpItem;
    gxpppSetupIPCPOption* ipcpItem;
    gxPppSetup* object = (gxPppSetup*)e->target;
    if (e->index == 2)
    {
        ret = var_addOctetString(&e->value, &object->PHYReference);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->lcpOptions.size, data)) != 0)
        {
            return ret;
        }

        for (pos = 0; pos != object->lcpOptions.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->lcpOptions, pos, (void**)&lcpItem)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Add type.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, lcpItem->type)) != 0 ||
                //Add length.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)lcpItem->length)) != 0 ||
                (ret = var_getBytes(&lcpItem->data, data)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->ipcpOptions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->ipcpOptions.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->ipcpOptions, pos, (void**)&ipcpItem)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Add type.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, ipcpItem->type)) != 0 ||
                //Add length.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)ipcpItem->length)) != 0 ||
                (ret = var_getBytes(&ipcpItem->data, data)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //Add user name.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->userName.size, data)) != 0 ||
            (ret = bb_set2(data, &object->userName, 0, bb_size(&object->userName))) != 0 ||
            //Add pw.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->password.size, data)) != 0 ||
            (ret = bb_set2(data, &object->password, 0, bb_size(&object->password))) != 0)
        {
            return ret;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int cosem_getRegisterActivation(
    gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxObjectDefinition *od;
    gxKey *it;
    gxRegisterActivation* object = (gxRegisterActivation*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->registerAssignment.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->registerAssignment.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, od->classId)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, od->logicalName, 6)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        unsigned long pos2;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->maskList.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->maskList.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)((gxByteBuffer*)it->key)->size)) != 0 ||
                (ret = bb_set2(data, (gxByteBuffer*)it->key, 0, bb_size((gxByteBuffer*)it->key))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)((gxByteBuffer*)it->value)->size)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != ((gxByteBuffer*)it->value)->size; ++pos2)
            {
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                    (ret = bb_setUInt8(data, ((gxByteBuffer*)it->value)->data[pos2])) != 0)
                {
                    return ret;
                }
            }
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->activeMask.size, data)) != 0 ||
            (ret = bb_set2(data, &object->activeMask, 0, bb_size(&object->activeMask))) != 0)
        {
            return ret;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int cosem_getRegisterMonitor(
    gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    dlmsVARIANT* tmp;
    gxActionSet* actionSet;
    gxRegisterMonitor* object = (gxRegisterMonitor*)e->target;
    if ((ret = cosem_getByteBuffer(&e->value)) != 0)
    {
        return ret;
    }
    gxByteBuffer *data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->thresholds.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->thresholds.size; ++pos)
        {
            if ((ret = va_getByIndex(&object->thresholds, pos, &tmp)) != 0 ||
                (ret = var_getBytes(tmp, data)) != 0)
            {
                return ret;
            }
        }
    }
    else if (e->index == 3)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //Object type
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->monitoredValue.objectType)) != 0 ||
            //LN
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(6, data)) != 0 ||
            (ret = bb_set(data, object->monitoredValue.logicalName, 6)) != 0 ||
            //attribute index.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, object->monitoredValue.attributeIndex)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 4)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->actions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->actions.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = arr_getByIndex(&object->actions, pos, (void**)&actionSet)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Add LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, actionSet->actionUp.logicalName, 6)) != 0 ||
                //Add script selector.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, actionSet->actionUp.scriptSelector)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Add LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, actionSet->actionDown.logicalName, 6)) != 0 ||
                //Add script selector.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, actionSet->actionDown.scriptSelector)) != 0)
            {
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    e->byteArray = 1;
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int cosem_getSapAssignment(
    gxValueEventArg *e)
{
    int ret, pos;
    gxSapItem *it;
    gxSapAssignment* object = (gxSapAssignment*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->sapAssignmentList.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->sapAssignmentList.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = arr_getByIndex(&object->sapAssignmentList, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, it->id)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(it->name.size, data)) != 0 ||
                (ret = bb_set2(data, &it->name, 0, bb_size(&it->name))) != 0)
            {
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
int cosem_getSchedule(
    gxValueEventArg *e)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxScheduleEntry *se;
    dlmsVARIANT tmp;
    gxSchedule* object = (gxSchedule*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->entries.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->entries.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 10)) != 0 ||
                (ret = arr_getByIndex(&object->entries, pos, (void**)&se)) != 0 ||
                //Add index.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, se->index)) != 0 ||
                //Add enable.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BOOLEAN)) != 0 ||
                (ret = bb_setUInt8(data, se->enable)) != 0 ||
                //Add logical Name.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, se->logicalName, 6)) != 0 ||
                //Add script selector.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, se->scriptSelector)) != 0 ||
                //Add switch time.
                (ret = var_setDateTime(&tmp, &se->switchTime)) != 0 ||
                (ret = var_getBytes(&tmp, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                //Add validity window.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, se->validityWindow)) != 0 ||
                //Add exec week days.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                (ret = hlp_setObjectCount(se->execWeekdays.size, data)) != 0 ||
                (ret = bb_set(data, se->execWeekdays.data, ba_getByteCount(se->execWeekdays.size))) != 0 ||
                //Add exec spec days.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                (ret = hlp_setObjectCount(se->execSpecDays.size, data)) != 0 ||
                (ret = bb_set(data, se->execSpecDays.data, ba_getByteCount(se->execSpecDays.size))) != 0 ||
                //Add begin date.
                (ret = var_setDateTime(&tmp, &se->beginDate)) != 0 ||
                (ret = var_getBytes(&tmp, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                //Add end date.
                (ret = var_setDateTime(&tmp, &se->endDate)) != 0 ||
                (ret = var_getBytes(&tmp, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0)
            {
                var_clear(&tmp);
                break;
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int cosem_getScriptTable(
    gxValueEventArg *e)
{
    int ret, pos, pos2;
    gxScript *it;
    gxScriptAction *a;
    gxScriptTable* object = (gxScriptTable*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->scripts.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->scripts.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Script_identifier:
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, it->id)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)it->actions.size)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != it->actions.size; ++pos2)
            {
                if ((ret = arr_getByIndex(&it->actions, pos2, (void**)&a)) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 5)) != 0 ||
                    //service_id
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
                    (ret = bb_setUInt8(data, a->type)) != 0 ||
                    //class_id
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                    (ret = bb_setUInt16(data, a->objectType)) != 0 ||
                    //logical_name
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = bb_setUInt8(data, 6)) != 0 ||
                    (ret = bb_set(data, a->logicalName, 6)) != 0 ||
                    //index
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                    (ret = bb_setInt8(data, a->index)) != 0 ||
                    //parameter
                    (ret = var_getBytes(&a->parameter, data)) != 0)
                {
                    return ret;
                }
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int cosem_getSpecialDaysTable(
    gxValueEventArg *e)
{
    int ret, pos;
    dlmsVARIANT tmp;
    gxSpecialDay* sd;
    gxSpecialDaysTable* object = (gxSpecialDaysTable*)e->target;
    if ((ret = cosem_getByteBuffer(&e->value)) != 0)
    {
        return ret;
    }
    gxByteBuffer *data = e->value.byteArr;
    if (e->index == 2)
    {
        e->byteArray = 1;
        if ((ret = var_init(&tmp)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->entries.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->entries.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd)) != 0 ||
                (ret = var_setDate(&tmp, &sd->date)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Index
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, sd->index)) != 0 ||
                //Date
                (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                //DayId
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, sd->dayId)) != 0)
            {
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int cosem_getTcpUdpSetup(
    gxValueEventArg *e)
{
    int ret;
    gxTcpUdpSetup* object = (gxTcpUdpSetup*)e->target;
    if (e->index == 2)
    {
        ret = var_setUInt16(&e->value, object->port);
    }
    else if (e->index == 3)
    {
        ret = var_addOctetString(&e->value, &object->ipReference);
    }
    else if (e->index == 4)
    {
        ret = var_setUInt16(&e->value, object->maximumSegmentSize);
    }
    else if (e->index == 5)
    {
        ret = var_setUInt8(&e->value, object->maximumSimultaneousConnections);
    }
    else if (e->index == 6)
    {
        ret = var_setUInt16(&e->value, object->inactivityTimeout);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int cosem_getMbusMasterPortSetup(
    gxValueEventArg *e)
{
    int ret;
    gxMBusMasterPortSetup* object = (gxMBusMasterPortSetup*)e->target;
    if (e->index == 2)
    {
        ret = var_setEnum(&e->value, object->commSpeed);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
int cosem_getMessageHandler(
    gxValueEventArg *e)
{
    int ret, pos;
    gxKey *it;
    dlmsVARIANT *tmp;
    dlmsVARIANT tmp2;
    gxMessageHandler* object = (gxMessageHandler*)e->target;
    if ((ret = cosem_getByteBuffer(&e->value)) != 0)
    {
        return ret;
    }
    gxByteBuffer *data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = var_init(&tmp2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->listeningWindow.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->listeningWindow.size; ++pos)
        {
            if ((ret = arr_get(&object->listeningWindow, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Add start
                (ret = var_setDateTime(&tmp2, (gxtime*)it->key)) != 0 ||
                (ret = var_getBytes(&tmp2, data)) != 0 ||
                (ret = var_clear(&tmp2)) != 0 ||
                //Add end
                (ret = var_setDateTime(&tmp2, (gxtime*)it->value)) != 0 ||
                (ret = var_getBytes(&tmp2, data)) != 0 ||
                (ret = var_clear(&tmp2)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 3)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->allowedSenders.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->allowedSenders.size; ++pos)
        {
            if ((ret = va_getByIndex(&object->allowedSenders, pos, &tmp)) != 0 ||
                (ret = var_getBytes(tmp, data)) != 0)
            {
                return ret;
            }
        }
    }
    else if (e->index == 4)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = bb_setUInt8(data, 0)) != 0 ||
            (ret = hlp_setObjectCount(object->sendersAndActions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->sendersAndActions.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->sendersAndActions, pos, (void**)&tmp)) != 0 ||
                (ret = var_getBytes(tmp, data)) != 0)
            {
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    e->byteArray = 1;
    return ret;
}
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
int cosem_getPushSetup(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    gxKey *it;
    dlmsVARIANT tmp;
    gxPushSetup* object = (gxPushSetup*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->pushObjectList.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->pushObjectList.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->pushObjectList, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 4)) != 0 ||
                //Type.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, ((gxObject*)it->key)->objectType)) != 0 ||
                //LN
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, ((gxObject*)it->key)->logicalName, 6)) != 0 ||
                //attributeIndex
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, ((gxCaptureObject*)it->value)->attributeIndex)) != 0 ||
                //dataIndex
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, ((gxCaptureObject*)it->value)->dataIndex)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //Service
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->service)) != 0 ||
            //Destination
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0)
        {
            return ret;
        }
        if (object->destination != NULL)
        {
            unsigned char len = (unsigned char)strlen(object->destination);
            if ((ret = bb_setUInt8(data, len)) != 0 ||
                (ret = bb_set(data, (unsigned char*)object->destination, len)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = bb_setUInt8(data, 0)) != 0)
            {
                return ret;
            }
        }
        //Message
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->message)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->communicationWindow.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->communicationWindow.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Start date time.
                (ret = var_setDateTimeAsOctetString(&tmp, (gxtime*)it->key)) != 0 ||
                (ret = var_getBytes(&tmp, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                //End date time.
                (ret = var_setDateTimeAsOctetString(&tmp, (gxtime*)it->value)) != 0 ||
                (ret = var_getBytes(&tmp, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0)
            {
                var_clear(&tmp);
                break;
            }
        }
    }
    else if (e->index == 5)
    {
        ret = var_setUInt16(&e->value, object->randomisationStartInterval);
    }
    else if (e->index == 6)
    {
        ret = var_setUInt8(&e->value, object->numberOfRetries);
    }
    else if (e->index == 7)
    {
        ret = var_setUInt16(&e->value, object->repetitionDelay);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
int cosem_getZigbeeNetworkControl(
    gxValueEventArg *e)
{
    gxZigBeeNetworkControl* object = (gxZigBeeNetworkControl*)e->target;
    int ret = 0, pos;
    gxActiveDevice *it;
    dlmsVARIANT tmp;
    if (e->index == 2)
    {
        ret = var_setUInt8(&e->value, object->enableDisableJoining);
    }
    else if (e->index == 3)
    {
        ret = var_setUInt16(&e->value, object->joinTimeout);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->activeDevices.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }

        for (pos = 0; pos != object->activeDevices.size; ++pos)
        {
            if ((ret = arr_get(&object->activeDevices, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 14)) != 0 ||
                //mac address.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)it->macAddress.size)) != 0 ||
                (ret = bb_set2(data, &it->macAddress, 0, bb_size(&it->macAddress))) != 0 ||
                //status
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)it->status.size)) != 0 ||
                (ret = bb_set(data, it->status.data, ba_getByteCount(it->status.size))) != 0 ||
                //max RSSI
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, it->maxRSSI)) != 0 ||
                //average RSSI
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, it->averageRSSI)) != 0 ||
                //min RSSI
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, it->minRSSI)) != 0 ||
                //max LQI
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->maxLQI)) != 0 ||
                //average LQI
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->averageLQI)) != 0 ||
                //min LQI
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->minLQI)) != 0 ||
                //last communication date time
                (ret = var_setDateTime(&tmp, &it->lastCommunicationDateTime)) != 0 ||
                (ret = var_getBytes(&tmp, data)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                //number of hops
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->numberOfHops)) != 0 ||
                //transmission failures
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->transmissionFailures)) != 0 ||
                //transmission successes
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->transmissionSuccesses)) != 0 ||
                //application version
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->applicationVersion)) != 0 ||
                //stack version
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->stackVersion)) != 0)
            {
                var_clear(&tmp);
                break;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_CHARGE
int getUnitCharge(gxUnitCharge* target, dlmsVARIANT *value)
{
    int ret = 0, pos;
    gxChargeTable *it;
    if ((ret = cosem_getByteBuffer(value)) != 0)
    {
        return ret;
    }
    gxByteBuffer *data = value->byteArr;

    value->vt = DLMS_DATA_TYPE_OCTET_STRING;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
        (ret = bb_setUInt8(data, 3)) != 0 ||
        //-------------
        //charge per unit scaling
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
        (ret = bb_setUInt8(data, 2)) != 0 ||
        //commodity scale
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
        (ret = bb_setUInt8(data, target->chargePerUnitScaling.commodityScale)) != 0 ||
        //price scale
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
        (ret = bb_setUInt8(data, target->chargePerUnitScaling.priceScale)) != 0 ||
        //-------------
        //commodity
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
        (ret = bb_setUInt8(data, 3)) != 0 ||
        //type
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
        (ret = bb_setUInt16(data, target->commodity.type)) != 0 ||
        //logicalName
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(data, 6)) != 0 ||
        (ret = bb_set(data, target->commodity.logicalName, 6)) != 0 ||
        //attributeIndex
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
        (ret = bb_setUInt8(data, target->commodity.attributeIndex)) != 0 ||
        //-------------
        //chargeTables
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = hlp_setObjectCount(target->chargeTables.size, data)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != target->chargeTables.size; ++pos)
    {
        if ((ret = arr_get(&target->chargeTables, (void**)&it)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //index
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, (unsigned char)it->index.size)) != 0 ||
            (ret = bb_set2(data, &it->index, 0, bb_size(&it->index))) != 0 ||
            //chargePerUnit
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT16)) != 0 ||
            (ret = bb_setInt16(data, it->chargePerUnit)) != 0)
        {
            break;
        }
    }
    return ret;
}

int cosem_getCharge(
    gxValueEventArg *e)
{
    gxCharge* object = (gxCharge*)e->target;
    int ret = 0;
    if (e->index == 2)
    {
        ret = var_setInt32(&e->value, object->totalAmountPaid);
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, object->chargeType);
    }
    else if (e->index == 4)
    {
        ret = var_setUInt8(&e->value, object->priority);
    }
    else if (e->index == 5)
    {
        e->byteArray = 1;
        ret = getUnitCharge(&object->unitChargeActive, &e->value);
    }
    else if (e->index == 6)
    {
        e->byteArray = 1;
        ret = getUnitCharge(&object->unitChargePassive, &e->value);
    }
    else if (e->index == 7)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &object->unitChargeActivationTime);
    }
    else if (e->index == 8)
    {
        ret = var_setUInt32(&e->value, object->period);
    }
    else if (e->index == 9)
    {
        e->byteArray = 1;
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->chargeConfiguration.size, data)) != 0 ||
            (ret = bb_set(data, object->chargeConfiguration.data, ba_getByteCount(object->chargeConfiguration.size))) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 10)
    {
        ret = var_setDateTimeAsOctetString(&e->value, &object->lastCollectionTime);
    }
    else if (e->index == 11)
    {
        ret = var_setInt32(&e->value, object->lastCollectionAmount);
    }
    else if (e->index == 12)
    {
        ret = var_setInt32(&e->value, object->totalAmountRemaining);
    }
    else if (e->index == 13)
    {
        ret = var_setUInt16(&e->value, object->proportion);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int cosem_getTokenGateway(
    gxValueEventArg *e)
{
    int pos, ret = 0;
    gxByteBuffer *data;
    char* it;
    gxTokenGateway* object = (gxTokenGateway*)e->target;
    switch (e->index)
    {
    case 2:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        ret = bb_set2(data, &object->token, 0, bb_size(&object->token));
        break;
    case 3:
        ret = var_setDateTimeAsOctetString(&e->value, &((gxTokenGateway*)e->target)->time);
        break;
    case 4:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->descriptions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->descriptions.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->descriptions, pos, (void**)&it)) != 0)
            {
                break;
            }
            int len = (int)strlen(it);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(len, data)) != 0 ||
                (ret = bb_set(data, (unsigned char*)it, len)) != 0)
            {
                break;
            }
        }
        break;
    case 5:
        ret = var_setEnum(&e->value, object->deliveryMethod);
        break;
    case 6:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->status)) != 0 ||

            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->dataValue.size, data)) != 0 ||
            (ret = bb_set(data, object->dataValue.data, ba_getByteCount(object->dataValue.size))) != 0)
        {
            return ret;
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_CREDIT
int cosem_getCredit(
    gxValueEventArg *e)
{
    gxCredit* object = (gxCredit*)e->target;
    int ret = 0;
    if (e->index == 2)
    {
        ret = var_setInt32(&e->value, object->currentCreditAmount);
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, object->type);
    }
    else if (e->index == 4)
    {
        ret = var_setUInt8(&e->value, object->priority);
    }
    else if (e->index == 5)
    {
        ret = var_setInt32(&e->value, object->warningThreshold);
    }
    else if (e->index == 6)
    {
        ret = var_setInt32(&e->value, object->limit);
    }
    else if (e->index == 7)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->creditConfiguration.size, data)) != 0 ||
            (ret = bb_set(data, object->creditConfiguration.data, ba_getByteCount(object->creditConfiguration.size))) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 8)
    {
        ret = var_setEnum(&e->value, object->status);
    }
    else if (e->index == 9)
    {
        ret = var_setInt32(&e->value, object->presetCreditAmount);
    }
    else if (e->index == 10)
    {
        ret = var_setInt32(&e->value, object->creditAvailableThreshold);
    }
    else if (e->index == 11)
    {
        ret = var_setDateTime(&e->value, &object->period);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_ACCOUNT
int cosem_getAccount(
    gxValueEventArg *e)
{
    int ret = 0, pos;
    unsigned char* it;
    gxCreditChargeConfiguration *ccc;
    gxTokenGatewayConfiguration *gwc;
    gxAccount* object = (gxAccount*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = hlp_setObjectCount(2, data)) != 0 ||
            //payment mode
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->paymentMode & 0x3)) != 0 ||
            //account status
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->accountStatus & 0x3)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 3)
    {
        ret = var_setUInt8(&e->value, object->currentCreditInUse);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 8)) != 0 ||
            (ret = bb_setUInt8(data, object->currentCreditStatus & 0xFF)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 5)
    {
        ret = var_setInt32(&e->value, object->availableCredit);
    }
    else if (e->index == 6)
    {
        ret = var_setInt32(&e->value, object->amountToClear);
    }
    else if (e->index == 7)
    {
        ret = var_setInt32(&e->value, object->clearanceThreshold);
    }
    else if (e->index == 8)
    {
        ret = var_setInt32(&e->value, object->aggregatedDebt);
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->creditReferences.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->creditReferences.size; ++pos)
        {
            if ((ret = arr_get(&object->creditReferences, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, it, 6)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 10)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->chargeReferences.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->chargeReferences.size; ++pos)
        {
            if ((ret = arr_get(&object->chargeReferences, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, it, 6)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 11)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        bitArray ba;
        ba_init(&ba);
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->creditChargeConfigurations.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->creditChargeConfigurations.size; ++pos)
        {
            if ((ret = arr_get(&object->creditChargeConfigurations, (void**)&ccc)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = hlp_setObjectCount(3, data)) != 0 ||
                //credit reference
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(6, data)) != 0 ||
                (ret = bb_set(data, ccc->creditReference, 6)) != 0 ||
                //charge reference
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(6, data)) != 0 ||
                (ret = bb_set(data, ccc->chargeReference, 6)) != 0 ||
                //collection configuration
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)(ccc->collectionConfiguration << 5))) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 12)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->tokenGatewayConfigurations.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->tokenGatewayConfigurations.size; ++pos)
        {
            if ((ret = arr_get(&object->tokenGatewayConfigurations, (void**)&gwc)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = hlp_setObjectCount(2, data)) != 0 ||
                //credit reference
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(6, data)) != 0 ||
                (ret = bb_set(data, gwc->creditReference, 6)) != 0 ||
                //token proportion
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, gwc->tokenProportion)) != 0)
            {
                break;
            }
        }
    }
    else if (e->index == 13)
    {
        ret = var_setDateTime(&e->value, &object->accountActivationTime);
    }
    else if (e->index == 14)
    {
        ret = var_setDateTime(&e->value, &object->accountClosureTime);
    }
    else if (e->index == 15)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = hlp_setObjectCount(3, data)) != 0 ||
            //Name
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING_UTF8)) != 0)
        {
            return ret;
        }
        if (object->currency.name == NULL)
        {
            if ((ret = bb_setUInt8(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            unsigned int len = (unsigned int)strlen(object->currency.name);
            if ((ret = hlp_setObjectCount(len, data)) != 0 ||
                (ret = bb_set(data, (unsigned char*)object->currency.name, len)) != 0)
            {
                return ret;
            }
        }
        //scale
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, object->currency.scale)) != 0 ||
            //unit
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
            (ret = bb_setUInt8(data, object->currency.unit)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 16)
    {
        ret = var_setInt32(&e->value, object->lowCreditThreshold);
    }
    else if (e->index == 17)
    {
        ret = var_setInt32(&e->value, object->nextCreditAvailableThreshold);
    }
    else if (e->index == 18)
    {
        ret = var_setUInt16(&e->value, object->maxProvision);
    }
    else if (e->index == 19)
    {
        ret = var_setInt32(&e->value, object->maxProvisionPeriod);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT

#ifndef DLMS_IGNORE_COMPACT_DATA
int cosem_getCompactData(
    dlmsSettings* settings,
    gxValueEventArg *e)
{
    gxByteBuffer* data;
    gxCompactData* object = (gxCompactData*)e->target;
    int ret;
    switch (e->index)
    {
    case 2:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        e->byteArray = 1;
        ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING);
        hlp_setObjectCount(object->buffer.size, data);
        bb_set(data, object->buffer.data, object->buffer.size);
        break;
    case 3:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        e->byteArray = 1;
        ret = getColumns(settings, &object->captureObjects, data, e);
        break;
    case 4:
        ret = var_setUInt8(&e->value, object->templateId);
        break;
    case 5:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING);
        hlp_setObjectCount(object->templateDescription.size, data);
        bb_set(data, object->templateDescription.data, object->templateDescription.size);
        break;
    case 6:
        ret = var_setEnum(&e->value, object->captureMethod);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

int cosem_getValue(
    dlmsSettings* settings,
    gxValueEventArg *e)
{
    int ret = 0;
    if (e->index == 1)
    {
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        gxByteBuffer *data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
            (ret = bb_set(data, e->target->logicalName, 6)) != 0)
        {
            return ret;
        }
        return DLMS_ERROR_CODE_OK;
    }
    switch (e->target->objectType)
    {
#ifndef DLMS_IGNORE_DATA
    case DLMS_OBJECT_TYPE_DATA:
        ret = cosem_getData(e);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = cosem_getRegister(e);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = cosem_getClock(e);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = cosem_getActionSchedule(e);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = cosem_getActivityCalendar(e);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = cosem_getAssociationLogicalName(settings, e);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_SERVER
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        ret = cosem_getAssociationShortName(settings, e);
        break;
#endif //DLMS_IGNORE_SERVER
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = cosem_getAutoAnswer(e);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = cosem_getAutoConnect(e);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = cosem_getDemandRegister(e);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = cosem_getMacAddressSetup(e);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = cosem_getExtendedRegister(e);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        ret = cosem_getGprsSetup(e);
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = cosem_getSecuritySetup(e);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = cosem_getIecHdlcSetup(e);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = cosem_getIecLocalPortSetup(e);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = cosem_getIP4Setup(e);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = cosem_getMbusSlavePortSetup(e);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = cosem_getImageTransfer(e);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = cosem_getDisconnectControl(e);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = cosem_getLimiter(e);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = cosem_getmMbusClient(e);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = cosem_getModemConfiguration(e);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = cosem_getPppSetup(e);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = cosem_getProfileGeneric(settings, e);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = cosem_getRegisterActivation(e);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = cosem_getRegisterMonitor(e);
        break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_REGISTER_TABLE
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        ret = cosem_getRegisterTable(e);
        break;
#endif //DLMS_IGNORE_REGISTER_TABLE
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        ret = cosem_getZigbeeNetworkControl(e);
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = cosem_getSapAssignment(e);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = cosem_getSchedule(e);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = cosem_getScriptTable(e);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = cosem_getSpecialDaysTable(e);
        break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_STATUS_MAPPING
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_STATUS_MAPPING
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = cosem_getTcpUdpSetup(e);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = cosem_getMbusMasterPortSetup(e);
        break;
#endif //
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        ret = cosem_getMessageHandler(e);
        break;
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = cosem_getPushSetup(e);
        break;
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_DATA_PROTECTION
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_DATA_PROTECTION
#ifndef DLMS_IGNORE_ACCOUNT
    case DLMS_OBJECT_TYPE_ACCOUNT:
        ret = cosem_getAccount(e);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = cosem_getCredit(e);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = cosem_getCharge(e);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = cosem_getTokenGateway(e);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = cosem_getGsmDiagnostic(e);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = cosem_getCompactData(settings, e);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = cosem_getTariffPlan(e);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default:
        //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

#ifdef DLMS_ITALIAN_STANDARD

int getInterval(gxInterval* interval, gxByteBuffer* data)
{
    int ret;
    unsigned char b = interval->startHour;
    b |= (unsigned char)(interval->intervalTariff << 5);
    b |= (unsigned char)((interval->useInterval ? 1 : 0) << 7);
    unsigned short v = interval->specialDayMonth;
    v |= (unsigned short)(interval->specialDay << 8);
    v |= (unsigned short)((interval->specialDayEnabled ? 1 : 0) << 15);
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(data, 5)) != 0 ||
        (ret = bb_setUInt8(data, b)) != 0 ||
        (ret = bb_setUInt16(data, interval->weeklyActivation)) != 0 ||
        (ret = bb_setUInt16(data, v)) != 0)
    {
        return ret;
    }
    return ret;
}

int getSeason(gxBandDescriptor* season, gxByteBuffer* data)
{
    int ret;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = bb_setUInt8(data, 5)) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, season->dayOfMonth)) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, season->month)) != 0 ||
        (ret = getInterval(&season->workingDayIntervals, data)) != 0 ||
        (ret = getInterval(&season->saturdayIntervals, data)) != 0 ||
        (ret = getInterval(&season->holidayIntervals, data)) != 0)
    {
        return ret;
    }
    return ret;
}

int cosem_getTariffPlan(gxValueEventArg *e)
{
    unsigned short it;
    int pos, ret;
    gxByteBuffer* data;
    gxTariffPlan* object = (gxTariffPlan*)e->target;
    switch (e->index)
    {
    case 2:
        if (object->calendarName == NULL)
        {
            ret = var_setString(&e->value, object->calendarName, 0);
        }
        else
        {
            ret = var_setString(&e->value, object->calendarName, (unsigned short)strlen(object->calendarName));
        }
        break;
    case 3:
        ret = var_setBoolean(&e->value, object->enabled);
        break;
    case 4:
    {
        data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 4)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, object->plan.defaultTariffBand)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = getSeason(&object->plan.winterSeason, data)) != 0 ||
            (ret = getSeason(&object->plan.summerSeason, data)) != 0 ||

            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->plan.weeklyActivation.size, data)) != 0 ||
            (ret = bb_set(data, object->plan.weeklyActivation.data, ba_getByteCount(object->plan.weeklyActivation.size))) != 0 ||

            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = bb_setUInt8(data, (unsigned char)object->plan.specialDays.size)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->plan.specialDays.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->plan.specialDays, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, it)) != 0)
            {
                return ret;
            }
        }
    }
    break;
    case 5:
    {
        data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            //Count
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //Time
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 4)) != 0 ||
            (ret = var_getTime(&object->activationTime, data)) != 0 ||
            //Date
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 5)) != 0 ||
            (ret = var_getDate(&object->activationTime, data)) != 0)
        {
            return ret;
        }
    }
    break;
    default:
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}
#endif //DLMS_ITALIAN_STANDARD

#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int cosem_getGsmDiagnostic(
    gxValueEventArg *e)
{
    gxAdjacentCell *it;
    gxByteBuffer *data;
    gxGsmDiagnostic* object = (gxGsmDiagnostic*)e->target;
    int pos, ret;
    switch (e->index)
    {
    case 2:
        if (object->operatorName == NULL)
        {
            ret = var_setString(&e->value, object->operatorName, 0);
        }
        else
        {
            ret = var_setString(&e->value, object->operatorName, (unsigned short)strlen(object->operatorName));
        }
        break;
    case 3:
        ret = var_setEnum(&e->value, object->status);
        break;
    case 4:
        ret = var_setEnum(&e->value, object->circuitSwitchStatus);
        break;
    case 5:
        ret = var_setEnum(&e->value, object->packetSwitchStatus);
        break;
    case 6:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0)
        {
            return ret;
        }
        if (object->base.version == 0)
        {
            if ((ret = bb_setUInt8(data, 4)) != 0 ||
                //cellId.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, (unsigned short)object->cellInfo.cellId)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = bb_setUInt8(data, 7)) != 0 ||
                //cellId.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
                (ret = bb_setUInt32(data, object->cellInfo.cellId)) != 0)
            {
                return ret;
            }
        }
        //LocationId.
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->cellInfo.locationId)) != 0 ||
            //SignalQuality.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, object->cellInfo.signalQuality)) != 0 ||
            //Ber.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, object->cellInfo.ber) != 0))
        {
            return ret;
        }
        if (object->base.version > 0)
        {
            if (//mobileCountryCode.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, object->cellInfo.mobileCountryCode)) != 0 ||
                //MobileNetworkCode.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, object->cellInfo.mobileNetworkCode)) != 0 ||
                //ChannelNumber.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
                (ret = bb_setUInt32(data, object->cellInfo.channelNumber)) != 0)
            {
                return ret;
            }
        }
        break;
    case 7:
        if ((ret = cosem_getByteBuffer(&e->value)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        e->byteArray = 1;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->adjacentCells.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->adjacentCells.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0)
            {
                break;
            }
            if (object->base.version == 0)
            {
                //cellId.
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                    (ret = bb_setUInt16(data, (unsigned short)it->cellId)) != 0)
                {
                    break;
                }
            }
            else
            {
                //cellId.
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
                    (ret = bb_setUInt32(data, it->cellId)) != 0)
                {
                    break;
                }
            }
            //SignalQuality.
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, it->signalQuality)) != 0)
            {
                break;
            }
        }
        break;
    case 8:
        ret = var_setDateTime(&e->value, &object->captureTime);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
