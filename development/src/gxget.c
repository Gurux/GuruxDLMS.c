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

#include "../include/gxignore.h"
#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include <math.h>

#include "../include/gxget.h"
#include "../include/gxarray.h"
#include "../include/bitarray.h"
#include "../include/cosem.h"
#include "../include/dlms.h"
#include "../include/gxkey.h"
#include "../include/helpers.h"
#include "../include/serverevents.h"

int cosem_getByteBuffer(gxValueEventArg* e)
{
    e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
    e->byteArray = 1;
#ifndef DLMS_IGNORE_MALLOC
    if (e->value.byteArr == NULL)
    {
        e->value.byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
        if (e->value.byteArr == NULL)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        bb_init(e->value.byteArr);
    }
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

#ifndef DLMS_IGNORE_DATA
int cosem_getData(gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    if (e->index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        if (e->value.vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            if ((ret = cosem_getByteBuffer(e)) != 0)
            {
                return ret;
            }
        }
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
        ret = var_copy(&e->value, &((gxRegister*)e->target)->value);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            gxByteBuffer* data = e->value.byteArr;
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setInt8(data, ((gxRegister*)e->target)->scaler)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
                (ret = bb_setUInt8(data, ((gxRegister*)e->target)->unit)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_REGISTER_TABLE
int cosem_getRegisterTable(
    gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
        ret = DLMS_ERROR_CODE_OK;
        //TODO:  ret = var_copy(&e->value, &((gxRegisterTable*)e->target)->value);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            gxByteBuffer* data = e->value.byteArr;
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setInt8(data, ((gxRegisterTable*)e->target)->scaler)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
                (ret = bb_setUInt8(data, ((gxRegisterTable*)e->target)->unit)) != 0)
            {
                //Error code is returned at the end of the function.
            }
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
    gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &((gxClock*)e->target)->time);
        }
#else
        ret = var_setDateTimeAsOctetString(&e->value, &((gxClock*)e->target)->time);
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &((gxClock*)e->target)->begin);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &((gxClock*)e->target)->begin);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 6)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &((gxClock*)e->target)->end);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &((gxClock*)e->target)->end);
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxtime* tm;
    dlmsVARIANT tmp;
    gxActionSchedule* object = (gxActionSchedule*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            gxByteBuffer* data = e->value.byteArr;
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Add LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(6, data)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                (ret = bb_set(data, obj_getLogicalName((gxObject*)object->executedScript), 6)) != 0 ||
#else
                (ret = bb_set(data, object->executedScriptLogicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                //Add executed script selector.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, object->executedScriptSelector)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
    }
    else if (e->index == 3)
    {
        ret = var_setEnum(&e->value, object->type);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->executionTime.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }

        for (pos = 0; pos != object->executionTime.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0)
            {
                break;
            }
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm)) != 0)
            {
                break;
            }
            if ((ret = var_setDateTime(&tmp, tm)) != 0)
            {
                break;
            }
            tmp.vt = DLMS_DATA_TYPE_TIME;
#else
            if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0)
            {
                break;
            }
            GX_TIME(tmp) = tm;
#endif //DLMS_IGNORE_MALLOC
            ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data);
            if (ret != 0)
            {
                break;
            }
#ifndef DLMS_IGNORE_MALLOC
            tmp.vt = DLMS_DATA_TYPE_DATE;
#else
            GX_DATE(tmp) = tm;
#endif //DLMS_IGNORE_MALLOC
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
    gxDayProfileAction* action;
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
#ifndef DLMS_IGNORE_MALLOC
            (ret = arr_getByIndex(list, pos, (void**)&dp)) != 0 ||
#else
            (ret = arr_getByIndex(list, pos, (void**)&dp, sizeof(gxDayProfile))) != 0 ||
#endif //DLMS_IGNORE_MALLOC
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(ba, dp->dayId)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(dp->daySchedules.size, ba)) != 0)
        {
            break;
        }

        for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2)
        {
            const unsigned char* ln;
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&action)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&action, sizeof(gxDayProfileAction))) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if (action->script == NULL)
            {
                ln = EMPTY_LN;
            }
            else
#endif //DLMS_IGNORE_OBJECT_POINTERS
            {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                ln = action->script->logicalName;
#else
                ln = action->scriptLogicalName;
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = var_setTimeAsOctetString(&tmp, &action->startTime)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(ba, 3)) != 0 ||
                (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, ba)) != 0 ||
                (ret = var_clear(&tmp)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(ba, 6)) != 0 ||
                (ret = bb_set(ba, ln, 6)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(ba, action->scriptSelector)) != 0)
            {
                break;
            }

#else
            if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(ba, 3)) != 0 ||
                (ret = cosem_setTimeAsOctectString(ba, &action->startTime)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(ba, 6)) != 0 ||
                (ret = bb_set(ba, ln, 6)) != 0 ||
                (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(ba, action->scriptSelector)) != 0)

            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 8)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&wp, sizeof(gxWeekProfile))) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, (unsigned char)wp->name.size)) != 0 ||
            (ret = bb_set(ba, wp->name.value, wp->name.size)) != 0 ||
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
#else
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 8)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&wp)) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, (unsigned char)bb_size(&wp->name))) != 0 ||
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
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int getActivityCalendarSeasonProfile(gxArray* list, gxByteBuffer* ba)
{
    int pos, ret = 0;
    gxSeasonProfile* sp;
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
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(list, pos, (void**)&sp, sizeof(gxSeasonProfile))) != 0)
        {
            break;
        }
        GX_DATETIME(tmp) = &sp->start;
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 3)) != 0 ||
            //Add name.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, (unsigned char)sp->name.size)) != 0 ||
            (ret = bb_set(ba, sp->name.value, sp->name.size)) != 0 ||
            //Add start time.
            (ret = cosem_setDateTimeAsOctectString(ba, &sp->start)) != 0 ||
            //Add week day.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, (unsigned char)sp->weekName.size)) != 0 ||
            (ret = bb_set(ba, sp->weekName.value, sp->weekName.size)) != 0)
        {
            break;
        }
#else
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

#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    gxActivityCalendar* object = (gxActivityCalendar*)e->target;
    if (e->index == 2)
    {
        ret = getActivityCalendarName(&object->calendarNameActive, data);
    }
    else if (e->index == 3)
    {
        ret = getActivityCalendarSeasonProfile(&object->seasonProfileActive, data);
    }
    else if (e->index == 4)
    {
        ret = getActivityCalendarWeekProfileTable(&object->weekProfileTableActive, data);
    }
    else if (e->index == 5)
    {
        ret = getActivityCalendarDayProfileTable(&object->dayProfileTableActive, data);
    }
    else if (e->index == 6)
    {
        ret = getActivityCalendarName(&object->calendarNamePassive, data);
    }
    else if (e->index == 7)
    {
        ret = getActivityCalendarSeasonProfile(&object->seasonProfilePassive, data);
    }
    else if (e->index == 8)
    {
        ret = getActivityCalendarWeekProfileTable(&object->weekProfileTablePassive, data);
    }
    else if (e->index == 9)
    {
        ret = getActivityCalendarDayProfileTable(&object->dayProfileTablePassive, data);
    }
    else if (e->index == 10)
    {
        ret = cosem_setDateTimeAsOctectString(data, &object->time);
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
    gxObject* object,
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

#ifndef DLMS_IGNORE_SERVER
// Returns LN Association View.
int getLNObjects(
    dlmsSettings* settings,
    gxValueEventArg* e,
    gxByteBuffer* data)
{
    uint16_t pduSize;
    gxAssociationLogicalName* object = (gxAssociationLogicalName*)e->target;
    int ret;
    uint16_t pos;
    unsigned char found = 0;
    gxObject* it;
    unsigned char ln[] = { 0, 0, 40, 0, 0, 255 };
    if ((ret = bb_capacity(data, settings->maxPduSize)) != 0)
    {
        return ret;
    }
    //Add count only for first time.
    if (!e->transaction)
    {
        uint16_t count = object->objectList.size;
        e->transactionEndIndex = object->objectList.size;

        //Find current association and if nout found add it.
        for (pos = 0; pos != object->objectList.size; ++pos)
        {
            ret = oa_getByIndex(&object->objectList, pos, &it);
            if (ret != 0)
            {
                return ret;
            }
            if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
            {
                if (memcmp(ln, it->logicalName, 6) == 0)
                {
                    found = 1;
                }
                else if (it != e->target)
                {
                    //Remove extra association view.
                    --count;
                }
            }
        }
        if (!found)
        {
            ++count;
        }
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(count, data)) != 0)
        {
            return ret;
        }
        if (!found)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                //Count
                (ret = bb_setUInt8(data, 4)) != 0 ||
                //ClassID
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)) != 0 ||
                //Version
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)object->base.version)) != 0 ||
                //LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, ln, 6)) != 0)
            {
                return ret;
            }
            //Access rights.
            ret = getLNAccessRights(settings, &object->base, data);
            if (ret != 0)
            {
                return ret;
            }
        }
    }
    for (pos = (uint16_t)e->transactionStartIndex; pos != object->objectList.size; ++pos)
    {
        ret = oa_getByIndex(&object->objectList, pos, &it);
        if (ret != 0)
        {
            return ret;
        }
        if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
        {
            if (!(it == e->target || memcmp(ln, it->logicalName, 6) == 0))
            {
                ++e->transactionStartIndex;
                continue;
            }
        }
        pduSize = (uint16_t)data->size;
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
            if (!e->skipMaxPduSize && dlms_isPduFull(settings, data, NULL))
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
        }
        ++e->transactionStartIndex;
    }
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        data->size = pduSize;
        ret = 0;
    }
    return ret;
}
#endif //DLMS_IGNORE_SERVER

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
/*
* Returns User list.
*/
int getUserList(
    dlmsSettings* settings,
    gxValueEventArg* e,
    gxByteBuffer* data)
{
    unsigned char len;
    uint16_t pos = 0;
    int ret;
#ifndef DLMS_IGNORE_MALLOC
    gxKey2* it;
#else
    gxUser* it;
#endif //DLMS_IGNORE_MALLOC
    unsigned id;
    char* name;
    gxAssociationLogicalName* target = (gxAssociationLogicalName*)e->target;
    // Add count only for first time.
    if (e->transactionStartIndex == 0)
    {
        e->transactionEndIndex = target->userList.size;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(target->userList.size, data)) != 0)
        {
            return ret;
        }
    }
    for (pos = (uint16_t)e->transactionStartIndex; pos < target->userList.size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&target->userList, pos, (void**)&it)) != 0)
        {
            break;
        }
        id = it->key;
        name = it->value;
        len = (unsigned char)strlen(name);
#else
        if ((ret = arr_getByIndex(&target->userList, pos, (void**)&it, sizeof(gxUser))) != 0)
        {
            break;
        }
        id = it->id;
        name = it->name;
        len = (unsigned char)strlen(it->name);
#endif //DLMS_IGNORE_MALLOC
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            // Count
            (ret = bb_setUInt8(data, 2)) != 0 ||
            // Id
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, id)) != 0 ||
            // Name
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING)) != 0 ||
            (ret = hlp_setObjectCount(len, data)) != 0 ||
            (ret = bb_set(data, (unsigned char*)name, len)) != 0)
        {
            return ret;
        }
        ++e->transactionStartIndex;
    }
    return 0;
}

int cosem_getAssociationLogicalName(
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    int ret = 0;
    gxAssociationLogicalName* object = ((gxAssociationLogicalName*)e->target);
    if (e->index == 2)
    {
#ifndef DLMS_IGNORE_SERVER
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        ret = getLNObjects(settings, e, data);
#endif //DLMS_IGNORE_SERVER
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            //Add count
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, object->clientSAP)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->serverSAP)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            //Add count
            (ret = bb_setUInt8(data, 0x7)) != 0 ||
            (ret = bb_setUInt8(data, 0x60)) != 0 ||
            (ret = bb_setUInt8(data, 0x85)) != 0 ||
            (ret = bb_setUInt8(data, 0x74)) != 0 ||
            //Add data.
            (ret = bb_setUInt8(data, object->applicationContextName.identifiedOrganization)) != 0 ||
            (ret = bb_setUInt8(data, object->applicationContextName.dlmsUA)) != 0 ||
            (ret = bb_setUInt8(data, object->applicationContextName.applicationContext)) != 0 ||
            (ret = bb_setUInt8(data, object->applicationContextName.contextId)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        unsigned char buff[4];
        gxByteBuffer tmp;
        bb_attach(&tmp, buff, 0, sizeof(buff));
        //Add count
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
            //Add data.
            (ret = cosem_setBitString(data, object->xDLMSContextInfo.conformance, 24)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->xDLMSContextInfo.maxReceivePduSize)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->xDLMSContextInfo.maxSendPduSize)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, object->xDLMSContextInfo.dlmsVersionNumber)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, object->xDLMSContextInfo.qualityOfService)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->xDLMSContextInfo.cypheringInfo.size, data)) != 0 ||
            (ret = bb_set2(data, &object->xDLMSContextInfo.cypheringInfo, 0, object->xDLMSContextInfo.cypheringInfo.size)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 6)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            gxByteBuffer* data = e->value.byteArr;
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                //Add count
                (ret = bb_setUInt8(data, 0x7)) != 0 ||
                //Add data.
                (ret = bb_setUInt8(data, 0x60)) != 0 ||
                (ret = bb_setUInt8(data, 0x85)) != 0 ||
                (ret = bb_setUInt8(data, 0x74)) != 0 ||
                (ret = bb_setUInt8(data, object->authenticationMechanismName.identifiedOrganization)) != 0 ||
                (ret = bb_setUInt8(data, object->authenticationMechanismName.dlmsUA)) != 0 ||
                (ret = bb_setUInt8(data, object->authenticationMechanismName.authenticationMechanismName)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)object->authenticationMechanismName.mechanismId)) != 0)
            {
                return ret;
            }
        }
    }
    else if (e->index == 7)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = cosem_setOctectString(e->value.byteArr, &object->secret);
        }
    }
    else if (e->index == 8)
    {
        ret = var_setEnum(&e->value, object->associationStatus);
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
            ret = cosem_setOctectString2(e->value.byteArr, obj_getLogicalName((gxObject*)object->securitySetup), 6);
#else
            ret = cosem_setOctectString2(e->value.byteArr, object->securitySetupReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    else if (e->index == 10)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = getUserList(settings, e, e->value.byteArr);
        }
    }
    else if (e->index == 11)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            // Add structure size.
            (ret = bb_setUInt8(data, 2)) != 0)
        {
            return ret;
        }
#ifdef DLMS_IGNORE_MALLOC
        char* str = (char*)object->currentUser.name;
        unsigned char len = (unsigned char)strlen(str);
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
            (ret = bb_setUInt8(data, object->currentUser.id)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING)) != 0 ||
            (ret = hlp_setObjectCount(len, data)) != 0 ||
            (ret = bb_set(data, (unsigned char*)str, len)) != 0)
        {
            return ret;
        }
#else
        if (object->currentUser.value == NULL)
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
            char* str = (char*)object->currentUser.value;
            unsigned char len = (unsigned char)strlen(str);
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                (ret = bb_setUInt8(data, object->currentUser.key)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING)) != 0 ||
                (ret = hlp_setObjectCount(len, data)) != 0 ||
                (ret = bb_set(data, (unsigned char*)str, len)) != 0)
            {
                return ret;
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
// Returns SN Association View.
int getSNObjects(
    dlmsSettings* settings,
    gxValueEventArg* e,
    gxByteBuffer* ba)
{
    uint16_t pduSize;
    objectArray* list = &((gxAssociationShortName*)e->target)->objectList;
    uint16_t pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* it;
    if ((ret = bb_capacity(ba, settings->maxPduSize)) != 0)
    {
        return ret;
    }
    //Add count only for first time.
    if (!e->transaction)
    {
        e->transactionEndIndex = list->size;
        bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY);
        //Add count
        if ((ret = hlp_setObjectCount(list->size, ba)) != 0)
        {
            return ret;
        }
    }
    for (pos = (uint16_t)e->transactionStartIndex; pos != list->size; ++pos)
    {
        pduSize = (uint16_t)ba->size;
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
            if (dlms_isPduFull(settings, ba, NULL))
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
        }
        ++e->transactionStartIndex;
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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    uint16_t pos;
    gxObject* it;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxAssociationShortName* object = (gxAssociationShortName*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = getSNObjects(settings, e, data);
    }
    else if (e->index == 3)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) == 0 &&
            (ret = hlp_setObjectCount(object->objectList.size, data)) == 0)
        {
            for (pos = 0; pos != object->objectList.size; ++pos)
            {
                if ((ret = oa_getByIndex(&object->objectList, pos, &it)) != 0 ||
                    (ret = getSNAccessRights(settings, it, data)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (e->index == 4)
    {
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        ret = cosem_setOctectString2(e->value.byteArr, obj_getLogicalName((gxObject*)object->securitySetup), 6);
#else
        ret = cosem_setOctectString2(e->value.byteArr, object->securitySetupReference, 6);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SERVER
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_AUTO_ANSWER
int cosem_getAutoAnswer(
    gxValueEventArg* e)
{
    int ret = 0, pos;
    dlmsVARIANT f, s;
    gxAutoAnswer* object = (gxAutoAnswer*)e->target;
    if (e->index == 2)
    {
        return var_setEnum(&e->value, object->mode);
    }
    else if (e->index == 3)
    {
#ifndef DLMS_IGNORE_MALLOC
        gxKey* kv;
#else
        gxTimePair* kv;
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->listeningWindow.size, data)) != 0 ||
            (ret = var_init(&f)) != 0 ||
            (ret = var_init(&s)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->listeningWindow.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0)
            {
                var_clear(&f);
                var_clear(&s);
                break;
            }
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&kv)) != 0 ||
                (ret = var_setDateTime(&f, (gxtime*)kv->key)) != 0 ||
                (ret = var_setDateTime(&s, (gxtime*)kv->value)) != 0)
            {
                var_clear(&f);
                var_clear(&s);
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&kv, sizeof(gxTimePair))) != 0)
            {
                var_clear(&f);
                var_clear(&s);
                break;
            }
            GX_DATETIME(f) = &kv->first;
            GX_DATETIME(s) = &kv->second;
#endif //DLMS_IGNORE_MALLOC
            //start_time
            if ((ret = var_getBytes2(&f, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0 ||
                //end_time
                (ret = var_getBytes2(&s, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0)
            {
                var_clear(&f);
                var_clear(&s);
                break;
            }
            var_clear(&f);
            var_clear(&s);
        }
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
    gxValueEventArg* e)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    gxKey* k;
    gxByteBuffer* it;
#else
    gxTimePair* k;
    gxDestination* it;
#endif //DLMS_IGNORE_MALLOC
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
        dlmsVARIANT f, s;
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->callingWindow.size, data)) != 0 ||
            (ret = var_init(&f)) != 0 ||
            (ret = var_init(&s)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->callingWindow.size; ++pos)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0)
            {
                break;
            }

#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k)) != 0 ||
                (ret = var_setDateTime(&f, (gxtime*)k->key)) != 0 ||
                (ret = var_setDateTime(&s, (gxtime*)k->value)) != 0 ||
                (ret = var_getBytes2(&f, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0 ||
                (ret = var_getBytes2(&s, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0)
            {
                var_clear(&f);
                var_clear(&s);
                break;
            }
            var_clear(&f);
            var_clear(&s);
#else
            if ((ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k, sizeof(gxTimePair))) != 0)
            {
                break;
            }
            if ((ret = cosem_setDateTimeAsOctectString(data, &k->first)) != 0 ||
                (ret = cosem_setDateTimeAsOctectString(data, &k->second)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 6)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->destinations.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->destinations.size; ++pos)
        {
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = arr_getByIndex(&object->destinations, pos, (void**)&it)) != 0 ||
                (ret = hlp_setObjectCount(it->size, data)) != 0 ||
                (ret = bb_set(data, it->data, it->size)) != 0)
            {
                break;
            }
#else
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = arr_getByIndex(&object->destinations, pos, (void**)&it, sizeof(gxDestination))) != 0 ||
                (ret = hlp_setObjectCount(it->size, data)) != 0 ||
                (ret = bb_set(data, it->value, it->size)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC

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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxDemandRegister* object = (gxDemandRegister*)e->target;
    if (e->index == 2)
    {
        ret = var_setUInt32(&e->value, (int32_t)(var_toDouble(&object->currentAverageValue) / hlp_getScaler(object->scaler)));
    }
    else if (e->index == 3)
    {
        ret = var_setUInt32(&e->value, (int32_t)(var_toDouble(&object->lastAverageValue) / hlp_getScaler(object->scaler)));
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->captureTime);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->captureTime);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 7)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->startTimeCurrent);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->startTimeCurrent);
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret = 0;
    gxMacAddressSetup* object = (gxMacAddressSetup*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = cosem_setOctectString(e->value.byteArr, &object->macAddress);
        }
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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxExtendedRegister* object = (gxExtendedRegister*)e->target;
    if (e->index == 2)
    {
        ret = var_copy(&e->value, &object->value);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
    else if (e->index == 4)
    {
        ret = var_copy(&e->value, &object->status);
    }
    else if (e->index == 5)
    {

#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->captureTime);
#else
        dlmsVARIANT tmp;
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        var_init(&tmp);
        ret = var_setDateTime(&tmp, &object->captureTime);
        if (ret == 0)
        {
            ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data);
        }
        var_clear(&tmp);
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret;
    gxGPRSSetup* object = (gxGPRSSetup*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = cosem_setOctectString(e->value.byteArr, &object->apn);
        }
    }
    else if (e->index == 3)
    {
        ret = var_setUInt16(&e->value, object->pinCode);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
    gxValueEventArg* e)
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setOctectString(e->value.byteArr, &object->clientSystemTitle);
#else
        ret = var_addOctetString(&e->value, &object->clientSystemTitle);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 5)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setOctectString(e->value.byteArr, &object->serverSystemTitle);
#else
        ret = var_addOctetString(&e->value, &object->serverSystemTitle);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 6)
    {
        gxCertificateInfo* it;
        int pos, len;
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->certificates.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->certificates.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->certificates, pos, (void**)&it, sizeof(gxCertificateInfo))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->certificates, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
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
    gxValueEventArg* e)
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setOctectString(e->value.byteArr, &object->deviceAddress);
#else
        ret = var_addOctetString(&e->value, &object->deviceAddress);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 7)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setOctectString(e->value.byteArr, &object->password1);
#else
        ret = var_addOctetString(&e->value, &object->password1);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 8)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setOctectString(e->value.byteArr, &object->password2);
#else
        ret = var_addOctetString(&e->value, &object->password2);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 9)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setOctectString(e->value.byteArr, &object->password5);
#else
        ret = var_addOctetString(&e->value, &object->password5);
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret = 0, pos;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* it;
    gxIp4Setup* object = (gxIp4Setup*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if (object->dataLinkLayer == NULL)
            {
                ret = cosem_setOctectString2(e->value.byteArr, EMPTY_LN, 6);
            }
            else
            {
                ret = cosem_setOctectString2(e->value.byteArr, object->dataLinkLayer->logicalName, 6);
                //Error code is returned at the end of the function.
            }
#else
            ret = cosem_setOctectString2(e->value.byteArr, object->dataLinkLayerReference, 6);
            {
                //Error code is returned at the end of the function.
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    else if (e->index == 3)
    {
        ret = var_setUInt32(&e->value, object->ipAddress);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->multicastIPAddress.size, data)) != 0)
        {
            return ret;
        }
#ifdef DLMS_IGNORE_MALLOC
        uint32_t* v;
        for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->multicastIPAddress, pos, (void**)&v, sizeof(uint32_t))) != 0 ||
                (ret = bb_setUInt32(data, *v)) != 0)
            {
                break;
            }
        }
#else
        for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
        {
            if ((ret = va_getByIndex(&object->multicastIPAddress, pos, &tmp)) != 0 ||
                (ret = var_getBytes(tmp, data)) != 0)
            {
                break;
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->ipOptions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->ipOptions.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->ipOptions, pos, (void**)&it, sizeof(gxip4SetupIpOption))) != 0 ||
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
                (ret = bb_setUInt8(data, (unsigned char)it->data.size)) != 0 ||
                (ret = bb_set(data, it->data.value, it->data.size)) != 0)
            {
                break;
            }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
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
    gxValueEventArg* e)
{
    int ret = 0, pos;
    gxImageActivateInfo* ai;
    gxImageTransfer* object = (gxImageTransfer*)e->target;
    if (e->index == 2)
    {
        ret = var_setUInt32(&e->value, object->imageBlockSize);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&ai, sizeof(gxImageActivateInfo))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&ai)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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

#if !(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA))
int getColumns(
    dlmsSettings* settings,
    gxArray* list,
    gxByteBuffer* ba,
    gxValueEventArg* e)
{
    uint16_t pduSize;
    int pos, ret;
#ifdef DLMS_IGNORE_MALLOC
    gxTarget* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = bb_capacity(ba, (list->size * 19) + 2)) != 0)
    {
        return ret;
    }
    //Add count only for first time.
    if (!e->transaction)
    {
        e->transactionEndIndex = list->size;
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(list->size, ba)) != 0)
        {
            return ret;
        }
    }
    for (pos = e->transactionStartIndex; pos != list->size; ++pos)
    {
        pduSize = (uint16_t)ba->size;
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(gxTarget))) != 0 ||
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 4)) != 0 ||
            //ClassID
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
            (ret = bb_setUInt16(ba, it->objectType)) != 0 ||
#else
            (ret = bb_setUInt16(ba, it->target->objectType)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //LN
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(ba, 6)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
            (ret = bb_set(ba, it->logicalName, 6)) != 0 ||
#else
            (ret = bb_set(ba, it->target->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //Attribute Index
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setInt8(ba, it->attributeIndex)) != 0 ||
            //Data Index
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(ba, it->dataIndex)) != 0)
        {
            break;
        }
#else
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
            (ret = bb_setInt8(ba, ((gxTarget*)it->value)->attributeIndex)) != 0 ||
            //Data Index
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(ba, ((gxTarget*)it->value)->dataIndex)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC

        if (settings->server)
        {
            //If PDU is full.
            if (dlms_isPduFull(settings, ba, NULL))
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
        }
        ++e->transactionStartIndex;
    }
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        ba->size = pduSize;
        ret = 0;
    }
    return ret;
}

#endif //!(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA))

#ifndef DLMS_IGNORE_PROFILE_GENERIC

#ifndef DLMS_IGNORE_MALLOC
int cosem_getRow(
    gxArray* table,
    int index,
    gxArray* captureObjects,
    gxArray* columns,
    gxByteBuffer* data)
{
    int ret, pos;
    dlmsVARIANT* col;
    variantArray* row;
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
    for (pos = 0; pos != captureObjects->size; ++pos)
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
    gxValueEventArg* e,
    gxArray* table,
    gxArray* captureObjects,
    gxByteBuffer* data)
{
    gxArray columns;
    uint16_t pduSize;
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
            if ((ret = hlp_setObjectCount(e->transactionEndIndex - e->transactionStartIndex, data)) != 0)
            {
                return ret;
            }
        }
        else
        {
            e->transactionEndIndex = table->size;
            if ((ret = hlp_setObjectCount(e->transactionEndIndex, data)) != 0)
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
        pduSize = (uint16_t)data->size;
        if ((ret = cosem_getRow(table, pos, captureObjects, &columns, data)) != 0)
        {
            break;
        }
        //If PDU is full.
        if (data->size > (uint16_t)(settings->maxPduSize - PDU_MAX_HEADER_SIZE))
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
            break;
        }
        ++e->transactionStartIndex;
    }
    arr_empty(&columns);
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        data->size = pduSize;
        ret = 0;
    }
    return ret;
}

int getProfileGenericData(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    gxValueEventArg* e,
    gxByteBuffer* reply)
{
    int ret;
    gxArray items;
    variantArray* row;
    dlmsVARIANT* it;
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
            uint16_t cnt = 0;
            gxtime start, end;
            dlmsVARIANT value;
            var_init(&value);
            if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) == 0)
            {
                if (it->vt == DLMS_DATA_TYPE_UINT32)
                {
                    time_initUnix(&start, it->ulVal);
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
                    time_initUnix(&end, it->ulVal);
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
                gxtime* t;
                uint16_t pos;
                for (pos = 0; pos != object->buffer.size; ++pos)
                {
                    if ((ret = arr_getByIndex(&object->buffer, pos, (void**)&row)) != 0 ||
                        (ret = va_getByIndex(row, 0, &it)) != 0)
                    {
                        break;
                    }
                    t = it->dateTime;
                    if (time_compare(&start, t) != -1 && time_compare(t, &end) != -1)
                    {
                        ++cnt;
                        //If Size of PDU is full.
                        if (reply->size < settings->maxPduSize)
                        {
                            ++e->transactionEndIndex;
                            if ((ret = cosem_getRow(&object->buffer, pos, &captureObjects, &object->captureObjects, reply)) != 0)
                            {
                                break;
                            }
                        }
                        else if (e->transactionEndIndex != 0)
                        {
                            ++e->transactionStartIndex;
                            //We must loop items throught if count is unknown.
                            break;
                        }
                    }
                }
                //Update count if items are read first time.
                if (ret == 0 && bb_size(e->value.byteArr) == 0)
                {
                    if ((ret = bb_setUInt8(e->value.byteArr, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                        (ret = hlp_setObjectCount(cnt, e->value.byteArr)) != 0 ||
                        (ret = bb_set(e->value.byteArr, reply->data, reply->size)) != 0)
                    {
                        return ret;
                    }
                    reply->position = reply->size = 0;
                    if ((ret = bb_set(reply, e->value.byteArr->data, e->value.byteArr->size)) != 0)
                    {
                        return ret;
                    }
                }
            }
        }
        else if (e->selector == 2) //Read by entry.
        {
            if ((ret = va_getByIndex(e->parameters.Arr, 0, &it)) == 0)
            {
                e->transactionStartIndex = var_toInteger(it);
                if (e->transactionStartIndex == 0)
                {
                    e->transactionStartIndex = 1;
                }
                if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) == 0)
                {
                    e->transactionEndIndex = var_toInteger(it);
                    if (e->transactionEndIndex == 0)
                    {
                        e->transactionEndIndex = object->buffer.size;
                    }
                    if (e->transactionStartIndex + e->transactionEndIndex > (uint32_t)(object->buffer.size + 1))
                    {
                        e->transactionEndIndex = object->buffer.size;
                    }
                    if (e->transactionStartIndex == 0)
                    {
                        e->transactionEndIndex = (uint16_t)object->buffer.size;
                        if ((ret = bb_setUInt8(reply, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                            (ret = hlp_setObjectCount(e->transactionEndIndex, reply)) != 0)
                        {
                            return ret;
                        }
                    }
                    for (; e->transactionStartIndex < e->transactionEndIndex; ++e->transactionStartIndex)
                    {
                        if ((ret = arr_getByIndex(&object->buffer, (uint16_t)e->transactionStartIndex, (void**)&row)) != 0 ||
                            (ret = cosem_getRow(&object->buffer, e->transactionStartIndex, &captureObjects, &object->captureObjects, reply)) != 0)
                        {
                            break;
                        }
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
#endif //DLMS_IGNORE_MALLOC

int cosem_getProfileGeneric(
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    unsigned char empty[6] = { 0 };
    int ret = 0;
    gxProfileGeneric* object = (gxProfileGeneric*)e->target;
    if (e->index == 2)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = getProfileGenericData(settings, object, e, e->value.byteArr);
        }
#else
        ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = getColumns(settings, &object->captureObjects, e->value.byteArr, e);
        }
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
    gxValueEventArg* e)
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
    gxValueEventArg* e)
{
    int ret = 0, pos;
    gxLimiter* object = (gxLimiter*)e->target;
#ifdef DLMS_IGNORE_MALLOC
    uint16_t* it;
#else
    dlmsVARIANT tmp;
    dlmsVARIANT* it;
#endif //DLMS_IGNORE_MALLOC

    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //objectType
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0)
        {
            return ret;
        }
        if (object->monitoredValue == NULL)
        {
            if ((ret = bb_setUInt16(data, 0)) != 0 ||
                //LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, EMPTY_LN, 6)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = bb_setUInt16(data, object->monitoredValue->objectType)) != 0 ||
                //LN.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, object->monitoredValue->logicalName, 6)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, object->selectedAttributeIndex)) != 0)
            {
                return ret;
            }
        }
    }
    else if (e->index == 3)
    {
        ret = var_copy(&e->value, &object->thresholdActive);
    }
    else if (e->index == 4)
    {
        ret = var_copy(&e->value, &object->thresholdNormal);
    }
    else if (e->index == 5)
    {
        ret = var_copy(&e->value, &object->thresholdEmergency);
    }
    else if (e->index == 6)
    {
        ret = var_setUInt32(&e->value, object->minOverThresholdDuration);
    }
    else if (e->index == 7)
    {
        ret = var_setUInt32(&e->value, object->minUnderThresholdDuration);
    }
    else if (e->index == 8)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //Id
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->emergencyProfile.id)) != 0 ||
            //Date
#ifdef DLMS_IGNORE_MALLOC
            (ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->emergencyProfile.activationTime)) != 0 ||
#else
            (ret = var_init(&tmp)) != 0 ||
            (ret = var_setDateTime(&tmp, &object->emergencyProfile.activationTime)) != 0 ||
            (ret = var_getBytes2(&tmp, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0 ||
            (ret = var_clear(&tmp)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
            //Duration
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
            (ret = bb_setUInt32(data, object->emergencyProfile.duration)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_clear(data)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->emergencyProfileGroupIDs.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->emergencyProfileGroupIDs.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(uint16_t))) != 0 ||
                (ret = bb_setUInt16(data, *it)) != 0)
            {
                break;
            }
#else
            if ((ret = va_getByIndex(&object->emergencyProfileGroupIDs, pos, &it)) != 0 ||
                (ret = var_getBytes(it, data)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 10)
    {
        ret = var_setBoolean(&e->value, object->emergencyProfileActive);
    }
    else if (e->index == 11)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //Action over threshold
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //LN
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = bb_set(data, obj_getLogicalName((gxObject*)object->actionOverThreshold.script), 6)) != 0 ||
#else
            (ret = bb_set(data, object->actionOverThreshold.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //Script selector
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->actionOverThreshold.scriptSelector)) != 0 ||
            //Action under threshold
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //LN
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = bb_set(data, obj_getLogicalName((gxObject*)object->actionUnderThreshold.script), 6)) != 0 ||
#else
            (ret = bb_set(data, object->actionUnderThreshold.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //Script selector
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->actionUnderThreshold.scriptSelector)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int cosem_getmMbusClient(
    gxValueEventArg* e)
{
    int ret = 0, pos;
    gxMBusClient* object = (gxMBusClient*)e->target;
#ifdef DLMS_IGNORE_MALLOC
    gxCaptureDefinition* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if (object->mBusPort == NULL)
            {
                ret = cosem_setOctectString2(e->value.byteArr, EMPTY_LN, 6);
            }
            else
            {
                ret = cosem_setOctectString2(e->value.byteArr, object->mBusPort->logicalName, 6);
            }
#else
            ret = cosem_setOctectString2(e->value.byteArr, object->mBusPortReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->captureDefinition.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->captureDefinition.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->captureDefinition, pos, (void**)&it, sizeof(gxCaptureDefinition))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(it->data.size, data)) != 0 ||
                (ret = bb_set(data, it->data.value, it->data.size)) != 0 ||
                //
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(it->value.size, data)) != 0 ||
                (ret = bb_set(data, it->value.value, it->value.size)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->captureDefinition, pos, (void**)&it)) != 0 ||
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
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 4)
    {
        ret = var_setUInt32(&e->value, object->capturePeriod);
    }
    else if (e->index == 5)
    {
        ret = var_setUInt8(&e->value, object->primaryAddress);
    }
    else if (e->index == 6)
    {
        ret = var_setUInt32(&e->value, object->identificationNumber);
    }
    else if (e->index == 7)
    {
        ret = var_setUInt16(&e->value, object->manufacturerID);
    }
    else if (e->index == 8)
    {
        ret = var_setUInt8(&e->value, object->dataHeaderVersion);
    }
    else if (e->index == 9)
    {
        ret = var_setUInt8(&e->value, object->deviceType);
    }
    else if (e->index == 10)
    {
        ret = var_setUInt8(&e->value, object->accessNumber);
    }
    else if (e->index == 11)
    {
        ret = var_setUInt8(&e->value, object->status);
    }
    else if (e->index == 12)
    {
        ret = var_setUInt8(&e->value, object->alarm);
    }
    else if (e->index == 13 && object->base.version != 0)
    {
        ret = var_setUInt16(&e->value, object->configuration);
    }
    else if (e->index == 14 && object->base.version != 0)
    {
        ret = var_setEnum(&e->value, object->encryptionKeyStatus);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION

int cosem_add(gxByteBuffer* data, const char* value, unsigned char len)
{
    int ret;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = hlp_setObjectCount(len, data)) != 0 ||
        (ret = bb_set(data, (unsigned char*)value, len)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getModemConfiguration(
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    int ret = 0, pos;
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* ba;
#else
    gxByteBuffer* ba;
#endif //DLMS_IGNORE_MALLOC
    gxModemConfiguration* target = ((gxModemConfiguration*)e->target);
    if (e->index == 2)
    {
        ret = var_setEnum(&e->value, target->communicationSpeed);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(target->initialisationStrings.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != target->initialisationStrings.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&target->initialisationStrings, pos, (void**)&mi, sizeof(gxModemInitialisation))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Add request.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(mi->request.size, data)) != 0 ||
                (ret = bb_set(data, mi->request.value, mi->request.size)) != 0 ||
                //Add response.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(mi->response.size, data)) != 0 ||
                (ret = bb_set(data, mi->response.value, mi->response.size)) != 0 ||
                //Add delay.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, mi->delay)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&target->initialisationStrings, pos, (void**)&mi)) != 0 ||
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
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        //Modem profile is defined on DLMS standard. Add default values.
        if (settings->server && target->modemProfile.size == 0)
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = hlp_setObjectCount(17, data)) != 0 ||
                (ret = cosem_add(data, "OK", 2) != 0) ||
                (ret = cosem_add(data, "CONNECT", 7) != 0) ||
                (ret = cosem_add(data, "RING", 4) != 0) ||
                (ret = cosem_add(data, "NO CARRIER", 10) != 0) ||
                (ret = cosem_add(data, "ERROR", 5) != 0) ||
                (ret = cosem_add(data, "CONNECT 1200", 12) != 0) ||
                (ret = cosem_add(data, "NO DIAL TONE", 12) != 0) ||
                (ret = cosem_add(data, "BUSY", 4) != 0) ||
                (ret = cosem_add(data, "NO ANSWER", 9) != 0) ||
                (ret = cosem_add(data, "CONNECT 600", 11) != 0) ||
                (ret = cosem_add(data, "CONNECT 2400", 11) != 0) ||
                (ret = cosem_add(data, "CONNECT 4800", 11) != 0) ||
                (ret = cosem_add(data, "CONNECT 9600", 11) != 0) ||
                (ret = cosem_add(data, "CONNECT 14 400", 14) != 0) ||
                (ret = cosem_add(data, "CONNECT 28 800", 14) != 0) ||
                (ret = cosem_add(data, "CONNECT 33 600", 14) != 0) ||
                (ret = cosem_add(data, "CONNECT 56 000", 14) != 0))
            {
                return ret;
            }
        }
        else
        {
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = hlp_setObjectCount(target->modemProfile.size, data)) != 0)
            {
                return ret;
            }
            for (pos = 0; pos != target->modemProfile.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&target->modemProfile, pos, (void**)&ba, sizeof(gxModemProfile))) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(ba->size, data)) != 0 ||
                    (ret = bb_set(data, ba->value, ba->size)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&target->modemProfile, pos, (void**)&ba)) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(ba->size, data)) != 0 ||
                    (ret = bb_set(data, ba->data, ba->size)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret = 0, pos;
    gxpppSetupLcpOption* lcpItem;
    gxpppSetupIPCPOption* ipcpItem;
    gxPppSetup* object = (gxPppSetup*)e->target;
    if (e->index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->phy == NULL)
        {
            ret = cosem_setOctectString2(e->value.byteArr, EMPTY_LN, 6);
        }
        else
        {
            ret = cosem_setOctectString2(e->value.byteArr, object->phy->logicalName, 6);
        }
#else
        ret = cosem_setOctectString2(e->value.byteArr, object->PHYReference, sizeof(object->PHYReference));
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            if (object->phy == NULL)
            {
                ret = cosem_setOctectString2(e->value.byteArr, EMPTY_LN, 6);
            }
            else
            {
                ret = cosem_setOctectString2(e->value.byteArr, object->phy->logicalName, 6);
            }
        }
#else
        if ((ret = cosem_getByteBuffer(e)) == 0 &&
            (ret = cosem_setOctectString2(e->value.byteArr, object->PHYReference, 6)) == 0)
        {
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->lcpOptions.size, data)) != 0)
        {
            return ret;
        }

        for (pos = 0; pos != object->lcpOptions.size; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->lcpOptions, pos, (void**)&lcpItem, sizeof(gxpppSetupLcpOption))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->lcpOptions, pos, (void**)&lcpItem)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->ipcpOptions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->ipcpOptions.size; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->ipcpOptions, pos, (void**)&ipcpItem, sizeof(gxpppSetupIPCPOption))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->ipcpOptions, pos, (void**)&ipcpItem)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        if (bb_size(&object->userName) == 0)
        {
            e->value.vt = DLMS_DATA_TYPE_NONE;
            return 0;
        }
        else
        {
            gxByteBuffer* data = e->value.byteArr;
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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
#ifdef DLMS_IGNORE_MALLOC
    gxObject* od;
#else
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
    gxRegisterActivation* object = (gxRegisterActivation*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->registerAssignment.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->registerAssignment.size; ++pos)
        {
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
            if ((ret = oa_getByIndex(&object->registerAssignment, pos, &od)) != 0 ||
#else
            if ((ret = arr_getByIndex2(&object->registerAssignment, pos, (void**)&od, sizeof(gxObject))) != 0 ||
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, od->objectType)) != 0 ||
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
#ifdef DLMS_IGNORE_MALLOC
        gxRegisterActivationMask* a;
#else
        gxKey* a;
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        uint32_t pos2;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->maskList.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->maskList.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&a, sizeof(gxRegisterActivationMask))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0)
            {
                break;
            }
            unsigned char cnt;
            switch (a->target->objectType)
            {
            case DLMS_OBJECT_TYPE_REGISTER:
                cnt = 3;
                break;
            case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
                cnt = 5;
                break;
            case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
                cnt = 9;
                break;
            default:
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, a->target->logicalName, 6)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = bb_setUInt8(data, cnt)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != cnt; ++pos2)
            {
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                    (ret = bb_setUInt8(data, a->indexes[pos2])) != 0)
                {
                    return ret;
                }
            }
#else
            if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&a)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)((gxByteBuffer*)a->key)->size)) != 0 ||
                (ret = bb_set2(data, (gxByteBuffer*)a->key, 0, bb_size((gxByteBuffer*)a->key))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)((gxByteBuffer*)a->value)->size)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != ((gxByteBuffer*)a->value)->size; ++pos2)
            {
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                    (ret = bb_setUInt8(data, ((gxByteBuffer*)a->value)->data[pos2])) != 0)
                {
                    return ret;
                }
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
        (ret = hlp_setObjectCount(6, data)) != 0 ||
            (ret = bb_set(data, object->activeMask, 6)) != 0)
#else
            (ret = hlp_setObjectCount(object->activeMask.size, data)) != 0 ||
            (ret = bb_set2(data, &object->activeMask, 0, bb_size(&object->activeMask))) != 0)
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    dlmsVARIANT_PTR tmp;
    gxActionSet* actionSet;
    gxRegisterMonitor* object = (gxRegisterMonitor*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) == 0 &&
            //Add count
            (ret = hlp_setObjectCount(object->thresholds.size, data)) == 0)
        {
            for (pos = 0; pos != object->thresholds.size; ++pos)
            {

                if ((ret = va_getByIndex(&object->thresholds, pos, &tmp)) != 0 ||
                    (ret = var_getBytes(tmp, data)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (e->index == 3)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            //Object type
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->monitoredValue.target != NULL ? object->monitoredValue.target->objectType : 0)) != 0 ||
            //LN
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(6, data)) != 0 ||
            (ret = bb_set(data, obj_getLogicalName(object->monitoredValue.target), 6)) != 0 ||
#else
            //Object type
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->monitoredValue.objectType)) != 0 ||
            //LN
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(6, data)) != 0 ||
            (ret = bb_set(data, object->monitoredValue.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //attribute index.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
            (ret = bb_setUInt8(data, object->monitoredValue.attributeIndex)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (e->index == 4)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) == 0 &&
            //Add count
            (ret = hlp_setObjectCount(object->actions.size, data)) == 0)
        {
            for (pos = 0; pos != object->actions.size; ++pos)
            {
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 2)) != 0 ||

#ifdef DLMS_IGNORE_MALLOC
                    (ret = arr_getByIndex(&object->actions, pos, (void**)&actionSet, sizeof(gxActionSet))) != 0 ||
#else
                    (ret = arr_getByIndex(&object->actions, pos, (void**)&actionSet)) != 0 ||
#endif //DLMS_IGNORE_MALLOC


                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 2)) != 0 ||
                    //Add LN.
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = bb_set(data, obj_getLogicalName((gxObject*)actionSet->actionUp.script), 6)) != 0 ||
#else
                    (ret = bb_set(data, actionSet->actionUp.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    //Add script selector.
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                    (ret = bb_setUInt16(data, actionSet->actionUp.scriptSelector)) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 2)) != 0 ||
                    //Add LN.
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = bb_set(data, obj_getLogicalName((gxObject*)actionSet->actionDown.script), 6)) != 0 ||
#else
                    (ret = bb_set(data, actionSet->actionDown.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    //Add script selector.
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                    (ret = bb_setUInt16(data, actionSet->actionDown.scriptSelector)) != 0)
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
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int cosem_getSapAssignment(
    gxValueEventArg* e)
{
    int ret, pos;
    gxSapItem* it;
    gxSapAssignment* object = (gxSapAssignment*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            //Add count
            (ret = hlp_setObjectCount(object->sapAssignmentList.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->sapAssignmentList.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                (ret = arr_getByIndex(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, it->id)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(it->name.size, data)) != 0 ||
                (ret = bb_set(data, it->name.value, it->name.size)) != 0)
            {
                break;
            }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxScheduleEntry* se;
    dlmsVARIANT tmp;
    gxSchedule* object = (gxSchedule*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
#ifdef DLMS_IGNORE_MALLOC
                (ret = arr_getByIndex(&object->entries, pos, (void**)&se, sizeof(gxScheduleEntry))) != 0 ||
#else
                (ret = arr_getByIndex(&object->entries, pos, (void**)&se)) != 0 ||
#endif //DLMS_IGNORE_MALLOC

                //Add index.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, se->index)) != 0 ||
                //Add enable.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BOOLEAN)) != 0 ||
                (ret = bb_setUInt8(data, se->enable)) != 0 ||
                //Add logical Name.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                (ret = bb_set(data, obj_getLogicalName((gxObject*)se->scriptTable), 6)) != 0 ||
#else
                (ret = bb_set(data, se->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS

                //Add script selector.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, se->scriptSelector)) != 0 ||
                //Add switch time.
                (ret = cosem_setDateTimeAsOctectString(data, &se->switchTime)) != 0 ||
                //Add validity window.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, se->validityWindow)) != 0 ||
                //Add exec week days.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                (ret = hlp_setObjectCount(7, data)) != 0 ||
                (ret = bb_setUInt8(data, se->execWeekdays)) != 0 ||
                //Add exec spec days.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                (ret = hlp_setObjectCount(se->execSpecDays.size, data)) != 0 ||
                (ret = bb_set(data, se->execSpecDays.data, ba_getByteCount(se->execSpecDays.size))) != 0 ||
                //Add begin date.
                (ret = cosem_setDateTimeAsOctectString(data, &se->beginDate)) != 0 ||
                //Add end date.
                (ret = cosem_setDateTimeAsOctectString(data, &se->endDate)) != 0)
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
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int cosem_getScriptTable(
    gxValueEventArg* e)
{
    int ret, pos, pos2;
    gxScript* it;
    gxScriptAction* a;
    gxScriptTable* object = (gxScriptTable*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->scripts.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->scripts.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&it, sizeof(gxScript))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&it->actions, pos2, (void**)&a, sizeof(gxScriptAction))) != 0 ||
#else
                if ((ret = arr_getByIndex(&it->actions, pos2, (void**)&a)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 5)) != 0 ||
                    //service_id
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ENUM)) != 0 ||
                    (ret = bb_setUInt8(data, a->type)) != 0 ||
                    //class_id
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = bb_setUInt16(data, a->target == NULL ? 0 : a->target->objectType)) != 0 ||
#else
                    (ret = bb_setUInt16(data, a->objectType)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    //logical_name
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = bb_set(data, obj_getLogicalName(a->target), 6)) != 0 ||
#else
                    (ret = bb_set(data, a->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    //index
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                    (ret = bb_setInt8(data, a->index)) != 0 ||
                    //parameter
                    (ret = var_getBytes(&a->parameter, data)) != 0)
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
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int cosem_getSpecialDaysTable(
    gxValueEventArg* e)
{
    int ret, pos;
    dlmsVARIANT tmp;
    gxSpecialDay* sd;
    gxSpecialDaysTable* object = (gxSpecialDaysTable*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = var_init(&tmp)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->entries.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->entries.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd, sizeof(gxSpecialDay))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Index
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, sd->index)) != 0 ||
                //Date
                (ret = cosem_setDateAsOctectString(data, &sd->date)) != 0 ||
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
    gxValueEventArg* e)
{
    int ret;
    gxTcpUdpSetup* object = (gxTcpUdpSetup*)e->target;
    if (e->index == 2)
    {
        ret = var_setUInt16(&e->value, object->port);
    }
    else if (e->index == 3)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = cosem_setOctectString2(e->value.byteArr, obj_getLogicalName(object->ipSetup), 6);
#else
        ret = cosem_setOctectString2(e->value.byteArr, object->ipReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
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

#ifndef DLMS_IGNORE_UTILITY_TABLES
int cosem_getUtilityTables(
    gxValueEventArg* e)
{
    int ret = 0;
    gxUtilityTables* object = (gxUtilityTables*)e->target;
    if (e->index == 2)
    {
        ret = var_setUInt16(&e->value, object->tableId);
    }
    else if (e->index == 3)
    {
        //Skip length.
    }
    else if (e->index == 4)
    {
        bb_clear(&object->buffer);
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(e->value.byteArr, &object->buffer);
#else
        ret = var_addBytes(&e->value, object->buffer.data, (uint16_t)object->buffer.size);
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int cosem_getMbusMasterPortSetup(
    gxValueEventArg* e)
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
    gxValueEventArg* e)
{
    int ret, pos;
#ifdef DLMS_IGNORE_MALLOC
    gxTimePair* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    dlmsVARIANT_PTR tmp;
    dlmsVARIANT tmp2;
    gxMessageHandler* object = (gxMessageHandler*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = var_init(&tmp2)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->listeningWindow.size, data)) != 0)
        {
            return ret;
        }
#ifdef DLMS_IGNORE_MALLOC
        for (pos = 0; pos != object->listeningWindow.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&it, sizeof(gxTimePair))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Add start
                (ret = cosem_setDateTimeAsOctectString(data, &it->first)) != 0 ||
                //Add end
                (ret = cosem_setDateTimeAsOctectString(data, &it->second)) != 0)
            {
                break;
            }
        }
#else
        for (pos = 0; pos != object->listeningWindow.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&it)) != 0 ||
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 3)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) == 0 &&
            (ret = hlp_setObjectCount(object->allowedSenders.size, data)) == 0)
        {
            for (pos = 0; pos != object->allowedSenders.size; ++pos)
            {
                if ((ret = va_getByIndex(&object->allowedSenders, pos, &tmp)) != 0 ||
                    (ret = var_getBytes(tmp, data)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (e->index == 4)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) == 0 &&
            (ret = bb_setUInt8(data, 0)) == 0 &&
            (ret = hlp_setObjectCount(object->sendersAndActions.size, data)) == 0)
        {
            for (pos = 0; pos != object->sendersAndActions.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->sendersAndActions, pos, (void**)&tmp, sizeof(dlmsVARIANT))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->sendersAndActions, pos, (void**)&tmp)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = var_getBytes(tmp, data)) != 0)
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
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
int cosem_getPushSetup(
    gxValueEventArg* e)
{
    int ret = 0, pos;
#ifdef DLMS_IGNORE_MALLOC
    gxTarget* it;
    gxTimePair* d;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    dlmsVARIANT tmp;
    gxPushSetup* object = (gxPushSetup*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->pushObjectList.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->pushObjectList.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->pushObjectList, pos, (void**)&it, sizeof(gxTarget))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 4)) != 0 ||
                //Type.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                (ret = bb_setUInt16(data, it->objectType)) != 0 ||
#else
                (ret = bb_setUInt16(data, it->target->objectType)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                //LN
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                (ret = bb_set(data, it->logicalName, 6)) != 0 ||
#else
                (ret = bb_set(data, it->target->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                //attributeIndex
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, it->attributeIndex)) != 0 ||
                //dataIndex
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, it->dataIndex)) != 0)
            {
                break;
            }
#else
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
                (ret = bb_setUInt8(data, ((gxTarget*)it->value)->attributeIndex)) != 0 ||
                //dataIndex
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, ((gxTarget*)it->value)->dataIndex)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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

#ifdef DLMS_IGNORE_MALLOC
        if (object->destination.size != 0)
        {
            if ((ret = bb_setUInt8(data, (unsigned char)object->destination.size)) != 0 ||
                (ret = bb_set(data, object->destination.value, object->destination.size)) != 0)
            {
                return ret;
            }
        }
#else
        if (object->destination != NULL)
        {
            unsigned char len = (unsigned char)strlen(object->destination);
            if ((ret = bb_setUInt8(data, len)) != 0 ||
                (ret = bb_set(data, (unsigned char*)object->destination, len)) != 0)
            {
                return ret;
            }
        }
#endif //DLMS_IGNORE_MALLOC
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->communicationWindow.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->communicationWindow.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&d, sizeof(gxTimePair))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0 ||
                //Start date time.
                (ret = cosem_setDateTimeAsOctectString(data, &d->first)) != 0 ||
                //End date time.
                (ret = cosem_setDateTimeAsOctectString(data, &d->second)) != 0)
            {
                var_clear(&tmp);
                break;
            }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    gxZigBeeNetworkControl* object = (gxZigBeeNetworkControl*)e->target;
    int ret = 0, pos;
    gxActiveDevice* it;
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->activeDevices.size, data)) != 0 ||
            (ret = var_init(&tmp)) != 0)
        {
            return ret;
        }

        for (pos = 0; pos != object->activeDevices.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->activeDevices, pos, (void**)&it, sizeof(gxActiveDevice))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->activeDevices, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
                (ret = cosem_setDateTimeAsOctectString(data, &it->lastCommunicationDateTime)) != 0 ||
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
int getUnitCharge(gxUnitCharge* target, gxValueEventArg* e)
{
    int ret = 0, pos;
    gxChargeTable* it;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
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
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_setUInt16(data, target->commodity.target == 0 ? 0 : target->commodity.target->objectType)) != 0 ||
#else
        (ret = bb_setUInt16(data, target->commodity.type)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //logicalName
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_set(data, obj_getLogicalName(target->commodity.target), 6)) != 0 ||
#else
        (ret = bb_set(data, target->commodity.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
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
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&it, sizeof(gxChargeTable))) != 0 ||
#else
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //index
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, (unsigned char)it->index.size)) != 0 ||
            (ret = bb_set(data, it->index.data, it->index.size)) != 0 ||
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
    gxValueEventArg* e)
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
        ret = getUnitCharge(&object->unitChargeActive, e);
    }
    else if (e->index == 6)
    {
        ret = getUnitCharge(&object->unitChargePassive, e);
    }
    else if (e->index == 7)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->unitChargeActivationTime);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->unitChargeActivationTime);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 8)
    {
        ret = var_setUInt32(&e->value, object->period);
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        ret = cosem_setBitString(e->value.byteArr, object->chargeConfiguration, 2);
    }
    else if (e->index == 10)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTime(e->value.byteArr, &object->lastCollectionTime);
#else
        ret = var_setDateTime(&e->value, &object->lastCollectionTime);
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int pos, ret = 0;
    gxByteBuffer* data;
#ifdef DLMS_IGNORE_MALLOC
    gxTokenGatewayDescription* it;
#else
    char* it;
#endif //DLMS_IGNORE_MALLOC
    gxTokenGateway* object = (gxTokenGateway*)e->target;
    switch (e->index)
    {
    case 2:
        if ((ret = cosem_getByteBuffer(e)) == 0)
        {
            ret = cosem_setOctectString2(e->value.byteArr, object->token.data, (uint16_t)object->token.size);
        }
        break;
    case 3:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->time);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->time);
#endif //DLMS_IGNORE_MALLOC
        break;
    case 4:
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->descriptions.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->descriptions.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->descriptions, pos, (void**)&it, sizeof(gxTokenGatewayDescription))) != 0)
            {
                break;
            }
            if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = hlp_setObjectCount(it->size, data)) != 0 ||
                (ret = bb_set(data, it->value, it->size)) != 0)
            {
                break;
            }
#else
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
#endif //DLMS_IGNORE_MALLOC
        }
        break;
    case 5:
        ret = var_setEnum(&e->value, object->deliveryMethod);
        break;
    case 6:
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
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
    gxValueEventArg* e)
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        ret = cosem_setBitString(e->value.byteArr, object->creditConfiguration, 5);
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->period);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->period);
#endif //DLMS_IGNORE_MALLOC
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
    gxValueEventArg* e)
{
    int ret = 0, pos;
    unsigned char* it;
    gxCreditChargeConfiguration* ccc;
    gxTokenGatewayConfiguration* gwc;
    gxAccount* object = (gxAccount*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        ret = cosem_setBitString(e->value.byteArr, object->currentCreditStatus, 7);
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->creditReferences.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->creditReferences.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->creditReferences, pos, (void**)&it, 6)) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->creditReferences, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->chargeReferences.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->chargeReferences.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->chargeReferences, pos, (void**)&it, 6)) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->chargeReferences, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->creditChargeConfigurations.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->creditChargeConfigurations.size; ++pos)
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->creditChargeConfigurations, pos, (void**)&ccc, sizeof(gxCreditChargeConfiguration))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->creditChargeConfigurations, pos, (void**)&ccc)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
                    (ret = cosem_setBitString(data, ccc->collectionConfiguration, 3)) != 0)
                {
                    break;
                }
    }
    else if (e->index == 12)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->tokenGatewayConfigurations.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->tokenGatewayConfigurations.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->tokenGatewayConfigurations, pos, (void**)&gwc, sizeof(gxTokenGatewayConfiguration))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->tokenGatewayConfigurations, pos, (void**)&gwc)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->accountActivationTime);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->accountActivationTime);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 14)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->accountClosureTime);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->accountClosureTime);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 15)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = hlp_setObjectCount(3, data)) != 0 ||
            //Name
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING_UTF8)) != 0)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_MALLOC
        if (object->currency.name == NULL)
        {
            if ((ret = bb_setUInt8(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            uint16_t len = (uint16_t)strlen(object->currency.name);
            if ((ret = hlp_setObjectCount(len, data)) != 0 ||
                (ret = bb_set(data, (unsigned char*)object->currency.name, len)) != 0)
            {
                return ret;
            }
        }
#else
        if ((ret = hlp_setObjectCount(object->currency.name.size, data)) != 0 ||
            (ret = bb_set(data, (unsigned char*)object->currency.name.value, object->currency.name.size)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_MALLOC

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

//Convert compact data buffer to array of values.
int compactData_getValues2(
    dlmsSettings* settings,
    gxByteBuffer* templateDescription,
    gxByteBuffer* buffer,
    variantArray* values,
    unsigned char appendAA)
{
    int ret;
    gxDataInfo info;
    dlmsVARIANT tmp;
    gxByteBuffer data;
    //If templateDescription or buffer is not given.
    if (values == NULL || bb_size(templateDescription) == 0 || bb_size(buffer) == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    va_clear(values);
    bb_init(&data);
    bb_set(&data, templateDescription->data, templateDescription->size);
    hlp_setObjectCount(buffer->size, &data);
    bb_set(&data, buffer->data, buffer->size);
    var_init(&tmp);
    di_init(&info);
#ifdef DLMS_ITALIAN_STANDARD
    info.appendAA = appendAA;
#endif //DLMS_ITALIAN_STANDARD
    info.type = DLMS_DATA_TYPE_COMPACT_ARRAY;
    if ((ret = dlms_getData(&data, &info, &tmp)) == 0 && tmp.Arr != NULL)
    {
        va_attach2(values, tmp.Arr);
    }
    var_clear(&tmp);
    bb_clear(&data);
    return ret;
}

//Convert compact data buffer to array of values.
int compactData_getValues(
    dlmsSettings* settings,
    gxByteBuffer* templateDescription,
    gxByteBuffer* buffer,
    variantArray* values)
{
    return compactData_getValues2(settings, templateDescription, buffer, values, 0);
}

int cosem_getCompactData(
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    gxByteBuffer* data;
    gxCompactData* object = (gxCompactData*)e->target;
    int ret;
    switch (e->index)
    {
    case 2:
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING);
        hlp_setObjectCount(object->buffer.size, data);
        bb_set(data, object->buffer.data, object->buffer.size);
        break;
    case 3:
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        ret = getColumns(settings, &object->captureObjects, data, e);
        break;
    case 4:
        ret = var_setUInt8(&e->value, object->templateId);
        break;
    case 5:
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
    if (e->index == 1)
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
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
        ret = cosem_getModemConfiguration(settings, e);
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
        ret = cosem_getUtilityTables(e);
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
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        ret = cosem_getParameterMonitor(e);
        break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = cosem_getLlcSscsSetup(e);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = cosem_getPrimeNbOfdmPlcPhysicalLayerCounters(e);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = cosem_getPrimeNbOfdmPlcMacSetup(e);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = cosem_getPrimeNbOfdmPlcMacFunctionalParameters(e);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = cosem_getPrimeNbOfdmPlcMacCounters(e);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = cosem_getPrimeNbOfdmPlcMacNetworkAdministrationData(e);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = cosem_getPrimeNbOfdmPlcApplicationsIdentification(e);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
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

unsigned char getInterval(gxInterval* interval)
{
    unsigned char b = (unsigned char)(interval->useInterval ? 1 : 0);
    b |= (unsigned char)(interval->intervalTariff << 1);
    b |= (unsigned char)(interval->startHour << 3);
    return b;
}

int getIntervals(gxInterval* interval, gxByteBuffer* data)
{
    int ret;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = bb_setUInt8(data, 5)) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, getInterval(interval))) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, getInterval(interval + 1))) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, getInterval(interval + 2))) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, getInterval(interval + 3))) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, getInterval(interval + 4))) != 0)
    {
        return ret;
    }
    return ret;
}

int getSeason(gxBandDescriptor* season, gxByteBuffer* data)
{
    int ret;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
        (ret = bb_setUInt8(data, 5)) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, season->dayOfMonth)) != 0 ||
        (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(data, season->month)) != 0 ||
        (ret = getIntervals(season->workingDayIntervals, data)) != 0 ||
        (ret = getIntervals(season->saturdayIntervals, data)) != 0 ||
        (ret = getIntervals(season->holidayIntervals, data)) != 0)
    {
        return ret;
    }
    return ret;
}

int cosem_getTariffPlan(gxValueEventArg* e)
{
    uint16_t it;
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
            ret = var_setString(&e->value, object->calendarName, (uint16_t)strlen(object->calendarName));
        }
        break;
    case 3:
        ret = var_setBoolean(&e->value, object->enabled);
        break;
    case 4:
    {
        data = e->value.byteArr;
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
    gxValueEventArg* e)
{
    gxAdjacentCell* it;
    gxByteBuffer* data;
    gxGsmDiagnostic* object = (gxGsmDiagnostic*)e->target;
    int pos, ret;
    switch (e->index)
    {
    case 2:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setOctectString(e->value.byteArr, &object->operatorName);
#else
        if (object->operatorName == NULL)
        {
            ret = var_setString(&e->value, object->operatorName, 0);
        }
        else
        {
            ret = var_setString(&e->value, object->operatorName, (uint16_t)strlen(object->operatorName));
        }
#endif //DLMS_IGNORE_MALLOC
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0)
        {
            return ret;
        }
        if (object->base.version == 0)
        {
            if ((ret = bb_setUInt8(data, 4)) != 0 ||
                //cellId.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, (uint16_t)object->cellInfo.cellId)) != 0)
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
            (ret = bb_setUInt8(data, object->cellInfo.ber)) != 0)
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
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->adjacentCells.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->adjacentCells.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it, sizeof(gxAdjacentCell))) != 0 ||
#else
            if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 2)) != 0)
            {
                break;
            }
            if (object->base.version == 0)
            {
                //cellId.
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                    (ret = bb_setUInt16(data, (uint16_t)it->cellId)) != 0)
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->captureTime);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->captureTime);
#endif //DLMS_IGNORE_MALLOC
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_PARAMETER_MONITOR
int cosem_getParameterMonitor(
    gxValueEventArg* e)
{
    int ret = 0, pos;
    gxByteBuffer* data;
    gxParameterMonitor* object = (gxParameterMonitor*)e->target;
    switch (e->index)
    {
    case 2:
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 4)) != 0)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->changedParameter.target == NULL)
        {
            if ((ret = cosem_setUInt16(data, 0)) != 0 ||
                (ret = cosem_setOctectString2(data, EMPTY_LN, 6)) != 0 ||
                (ret = cosem_setInt8(data, 0)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
        else
        {
            if ((ret = cosem_setUInt16(data, object->changedParameter.target->objectType)) != 0 ||
                (ret = cosem_setOctectString2(data, object->changedParameter.target->logicalName, 6)) != 0 ||
                (ret = cosem_setInt8(data, object->changedParameter.attributeIndex)) != 0 ||
                (ret = cosem_setVariant(data, &object->changedParameter.value)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
#else
        if ((ret = cosem_setUInt16(data, object->changedParameter.type)) != 0 ||
            (ret = cosem_setOctectString2(data, object->changedParameter.logicalName, 6)) != 0 ||
            (ret = cosem_setInt8(data, object->changedParameter.attributeIndex)) != 0 ||
            (ret = cosem_setVariant(data, &object->changedParameter.value)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    break;
    case 3:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_setDateTimeAsOctectString(e->value.byteArr, &object->captureTime);
#else
        ret = var_setDateTimeAsOctetString(&e->value, &object->captureTime);
#endif //DLMS_IGNORE_MALLOC
        break;
    case 4:
    {
#ifdef DLMS_IGNORE_MALLOC
        gxTarget* it;
#else
        gxKey* it;
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        data = e->value.byteArr;
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
            (ret = hlp_setObjectCount(object->parameters.size, data)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->parameters.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->parameters, pos, (void**)&it, sizeof(gxTarget))) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Type.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                (ret = bb_setUInt16(data, it->objectType)) != 0 ||
#else
                (ret = bb_setUInt16(data, it->target->objectType)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                //LN
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                (ret = bb_set(data, it->logicalName, 6)) != 0 ||
#else
                (ret = bb_set(data, it->target->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                //attributeIndex
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, it->attributeIndex)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->parameters, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                (ret = bb_setUInt8(data, 3)) != 0 ||
                //Type.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                (ret = bb_setUInt16(data, ((gxObject*)it->key)->objectType)) != 0 ||
                //LN
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, 6)) != 0 ||
                (ret = bb_set(data, ((gxObject*)it->key)->logicalName, 6)) != 0 ||
                //attributeIndex
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_INT8)) != 0 ||
                (ret = bb_setUInt8(data, ((gxTarget*)it->value)->attributeIndex)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int cosem_getLlcSscsSetup(
    gxValueEventArg* e)
{
    int ret;
    gxLlcSscsSetup* object = (gxLlcSscsSetup*)e->target;
    switch (e->index)
    {
    case 2:
        ret = var_setUInt16(&e->value, object->serviceNodeAddress);
        break;
    case 3:
        ret = var_setUInt16(&e->value, object->baseNodeAddress);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int cosem_getPrimeNbOfdmPlcPhysicalLayerCounters(
    gxValueEventArg* e)
{
    int ret;
    gxPrimeNbOfdmPlcPhysicalLayerCounters* object = (gxPrimeNbOfdmPlcPhysicalLayerCounters*)e->target;
    switch (e->index)
    {
    case 2:
        ret = var_setUInt16(&e->value, object->crcIncorrectCount);
        break;
    case 3:
        ret = var_setUInt16(&e->value, object->crcFailedCount);
        break;
    case 4:
        ret = var_setUInt16(&e->value, object->txDropCount);
        break;
    case 5:
        ret = var_setUInt16(&e->value, object->rxDropCount);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}

#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int cosem_getPrimeNbOfdmPlcMacSetup(
    gxValueEventArg* e)
{
    int ret;
    gxPrimeNbOfdmPlcMacSetup* object = (gxPrimeNbOfdmPlcMacSetup*)e->target;
    switch (e->index)
    {
    case 2:
        ret = var_setUInt8(&e->value, object->macMinSwitchSearchTime);
        break;
    case 3:
        ret = var_setUInt8(&e->value, object->macMaxPromotionPdu);
        break;
    case 4:
        ret = var_setUInt8(&e->value, object->macPromotionPduTxPeriod);
        break;
    case 5:
        ret = var_setUInt8(&e->value, object->macBeaconsPerFrame);
        break;
    case 6:
        ret = var_setUInt8(&e->value, object->macScpMaxTxAttempts);
        break;
    case 7:
        ret = var_setUInt8(&e->value, object->macCtlReTxTimer);
        break;
    case 8:
        ret = var_setUInt8(&e->value, object->macMaxCtlReTx);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int cosem_getPrimeNbOfdmPlcMacFunctionalParameters(
    gxValueEventArg* e)
{
    int ret;
    gxPrimeNbOfdmPlcMacFunctionalParameters* object = (gxPrimeNbOfdmPlcMacFunctionalParameters*)e->target;
    switch (e->index)
    {
    case 2:
        ret = var_setInt16(&e->value, object->lnId);
        break;
    case 3:
        ret = var_setUInt8(&e->value, object->lsId);
        break;
    case 4:
        ret = var_setUInt8(&e->value, object->sId);
        break;
    case 5:
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t size;
        ret = cosem_getOctectString2(e->value.byteArr, object->sna.data, (uint16_t)object->sna.size, &size);
#else
        ret = var_addBytes(&e->value, object->sna.data, (uint16_t)object->sna.size);
#endif //DLMS_IGNORE_MALLOC
    }
    break;
    case 6:
        ret = var_setEnum(&e->value, object->state);
        break;
    case 7:
        ret = var_setUInt16(&e->value, object->scpLength);
        break;
    case 8:
        ret = var_setUInt8(&e->value, object->nodeHierarchyLevel);
        break;
    case 9:
        ret = var_setUInt8(&e->value, object->beaconSlotCount);
        break;
    case 10:
        ret = var_setUInt8(&e->value, object->beaconRxSlot);
        break;
    case 11:
        ret = var_setUInt8(&e->value, object->beaconTxSlot);
        break;
    case 12:
        ret = var_setUInt8(&e->value, object->beaconRxFrequency);
        break;
    case 13:
        ret = var_setUInt8(&e->value, object->beaconTxFrequency);
        break;
    case 14:
        ret = var_setEnum(&e->value, object->capabilities);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int cosem_getPrimeNbOfdmPlcMacCounters(
    gxValueEventArg* e)
{
    int ret;
    gxPrimeNbOfdmPlcMacCounters* object = (gxPrimeNbOfdmPlcMacCounters*)e->target;
    switch (e->index)
    {
    case 2:
        ret = var_setUInt32(&e->value, object->txDataPktCount);
        break;
    case 3:
        ret = var_setUInt32(&e->value, object->rxDataPktCount);
        break;
    case 4:
        ret = var_setUInt32(&e->value, object->txCtrlPktCount);
        break;
    case 5:
        ret = var_setUInt32(&e->value, object->rxCtrlPktCount);
        break;
    case 6:
        ret = var_setUInt32(&e->value, object->csmaFailCount);
        break;
    case 7:
        ret = var_setUInt32(&e->value, object->csmaChBusyCount);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA

int cosem_getMulticastEntries(gxValueEventArg* e)
{
    gxMacMulticastEntry* it;
    int ret, pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = hlp_setObjectCount(object->multicastEntries.size, data)) != 0)
    {
        return ret;
    }

    for (pos = 0; pos != object->multicastEntries.size; ++pos)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0)
        {
            break;
        }
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->multicastEntries, pos, (void**)&it)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&object->multicastEntries, pos, (void**)&it, sizeof(gxMacMulticastEntry))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC

        if ((ret = cosem_setInt8(data, it->id)) != 0 ||
            (ret = cosem_setInt16(data, it->members)) != 0)
        {
            break;
        }
    }
    return ret;
}

int cosem_getSwitchTable(gxValueEventArg* e)
{
    uint16_t* it;
    int ret, pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = hlp_setObjectCount(object->switchTable.size, data)) != 0)
    {
        return ret;
    }

    for (pos = 0; pos != object->switchTable.size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->switchTable, pos, (void**)&it)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&object->switchTable, pos, (void**)&it, sizeof(short))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cosem_setUInt16(data, *it)) != 0)
        {
            break;
        }
    }
    return ret;
}

int cosem_getDirectTable(gxValueEventArg* e)
{
    gxMacDirectTable* it;
    int ret, pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = hlp_setObjectCount(object->directTable.size, data)) != 0)
    {
        return ret;
    }

    for (pos = 0; pos != object->directTable.size; ++pos)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 7)) != 0)
        {
            break;
        }
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->directTable, pos, (void**)&it)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&object->directTable, pos, (void**)&it, sizeof(gxMacDirectTable))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC

        if ((ret = cosem_setInt16(data, it->sourceSId)) != 0 ||
            (ret = cosem_setInt16(data, it->sourceLnId)) != 0 ||
            (ret = cosem_setInt16(data, it->sourceLcId)) != 0 ||
            (ret = cosem_setInt16(data, it->destinationSId)) != 0 ||
            (ret = cosem_setInt16(data, it->destinationLnId)) != 0 ||
            (ret = cosem_setInt16(data, it->destinationLcId)) != 0 ||
            (ret = cosem_setOctectString2(data, it->did, sizeof(it->did))) != 0)
        {
            break;
        }
    }
    return ret;
}

int cosem_getAvailableSwitches(gxValueEventArg* e)
{
    gxMacAvailableSwitch* it;
    int ret, pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = hlp_setObjectCount(object->availableSwitches.size, data)) != 0)
    {
        return ret;
    }

    for (pos = 0; pos != object->availableSwitches.size; ++pos)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 5)) != 0)
        {
            break;
        }
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->availableSwitches, pos, (void**)&it)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&object->availableSwitches, pos, (void**)&it, sizeof(gxMacAvailableSwitch))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC

        if ((ret = cosem_setOctectString2(data, it->sna.data, (uint16_t)it->sna.size)) != 0 ||
            (ret = cosem_setInt16(data, it->lsId)) != 0 ||
            (ret = cosem_setInt8(data, it->level)) != 0 ||
            (ret = cosem_setInt8(data, it->rxLevel)) != 0 ||
            (ret = cosem_setInt8(data, it->rxSnr)) != 0)
        {
            break;
        }
    }
    return ret;
}

int cosem_getCommunications(gxValueEventArg* e)
{
    gxMacPhyCommunication* it;
    int ret, pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = hlp_setObjectCount(object->communications.size, data)) != 0)
    {
        return ret;
    }

    for (pos = 0; pos != object->communications.size; ++pos)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 9)) != 0)
        {
            break;
        }
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&object->communications, pos, (void**)&it)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&object->availableSwitches, pos, (void**)&it, sizeof(gxMacPhyCommunication))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC

        if ((ret = cosem_setOctectString2(data, it->eui, sizeof(it->eui))) != 0 ||
            (ret = cosem_setInt8(data, it->txPower)) != 0 ||
            (ret = cosem_setInt8(data, it->txCoding)) != 0 ||
            (ret = cosem_setInt8(data, it->rxCoding)) != 0 ||
            (ret = cosem_setInt8(data, it->rxLvl)) != 0 ||
            (ret = cosem_setInt8(data, it->snr)) != 0 ||
            (ret = cosem_setInt8(data, it->txPowerModified)) != 0 ||
            (ret = cosem_setInt8(data, it->txCodingModified)) != 0 ||
            (ret = cosem_setInt8(data, it->rxCodingModified)) != 0)
        {
            break;
        }
    }
    return ret;
}

int  cosem_getPrimeNbOfdmPlcMacNetworkAdministrationData(
    gxValueEventArg* e)
{
    int ret;
    switch (e->index)
    {
    case 2:
        ret = cosem_getMulticastEntries(e);
        break;
    case 3:
        ret = cosem_getSwitchTable(e);
        break;
    case 4:
        ret = cosem_getDirectTable(e);
        break;
    case 5:
        ret = cosem_getAvailableSwitches(e);
        break;
    case 6:
        ret = cosem_getCommunications(e);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int  cosem_getPrimeNbOfdmPlcApplicationsIdentification(
    gxValueEventArg* e)
{
    int ret;
    gxPrimeNbOfdmPlcApplicationsIdentification* object = (gxPrimeNbOfdmPlcApplicationsIdentification*)e->target;
    switch (e->index)
    {
    case 2:
    {
        if ((ret = cosem_getByteBuffer(e)) != 0)
        {
            return ret;
        }
        gxByteBuffer* data = e->value.byteArr;
        int len = (int)strlen(object->firmwareVersion);
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, len)) != 0 ||
            (ret = bb_set(data, (unsigned char*)object->firmwareVersion, len)) != 0)
        {
            break;
        }
    }
    break;
    case 3:
        ret = var_setUInt16(&e->value, object->vendorId);
        break;
    case 4:
        ret = var_setUInt16(&e->value, object->productId);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
