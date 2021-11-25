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

#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif

#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include <math.h>

#include "../include/gxset.h"
#include "../include/dlms.h"
#include "../include/gxkey.h"
#include "../include/cosem.h"
#include "../include/gxget.h"
#include "../include/server.h"

// Get item from the buffer if DLMS_IGNORE_MALLOC is defined.
// Otherwice create the object dynamically.
int cosem_getArrayItem(gxArray* arr, uint16_t index, void** value, uint16_t itemSize)
{
#ifdef DLMS_IGNORE_MALLOC
    return arr_getByIndex2(arr, index, value, itemSize);
#else
    unsigned char* it = gxcalloc(1, itemSize);
    *value = it;
    return arr_push(arr, it);
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int cosem_verifyArray(gxByteBuffer* bb, gxArray* arr, uint16_t* count)
{
#ifdef DLMS_IGNORE_MALLOC
    * count = arr_getCapacity(arr);
    int ret = cosem_checkArray(bb, count);
    if (ret == 0)
    {
        arr->size = *count;
    }
    return ret;
#else
    * count = 0xFFFF;
    int ret = cosem_checkArray(bb, count);
    if (ret == 0)
    {
        arr_capacity(arr, *count);
    }
    return ret;
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int cosem_getVariantArrayItem(variantArray* arr, uint16_t index, dlmsVARIANT** value)
{
#ifdef DLMS_IGNORE_MALLOC
    return va_getByIndex(arr, index, value);
#else
    dlmsVARIANT* it = gxmalloc(sizeof(dlmsVARIANT));
    var_init(it);
    *value = it;
    return va_push(arr, it);
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int cosem_verifyVariantArray(gxByteBuffer* bb, variantArray* arr, uint16_t* count)
{
#ifdef DLMS_IGNORE_MALLOC
    * count = va_getCapacity(arr);
    int ret = cosem_checkArray(bb, count);
    if (ret == 0)
    {
        arr->size = *count;
    }
    return ret;
#else
    * count = 0xFFFF;
    int ret = cosem_checkArray(bb, count);
    if (ret == 0)
    {
        va_capacity(arr, *count);
    }
    return ret;
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int cosem_verifyObjectArray(gxByteBuffer* bb, objectArray* arr, uint16_t* count)
{
#ifdef DLMS_IGNORE_MALLOC
    * count = oa_getCapacity(arr);
    int ret = cosem_checkArray(bb, count);
    if (ret == 0)
    {
        arr->size = *count;
    }
    return ret;
#else
    * count = 0xFFFF;
    int ret = cosem_checkArray(bb, count);
    if (ret == 0)
    {
        oa_capacity(arr, *count);
    }
    return ret;
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int cosem_updateVariant(dlmsVARIANT* target, gxValueEventArg* e)
{
    int ret;
    //If data is coming in action it's plain value.
    if (e->action)
    {
        if (e->value.vt == (DLMS_DATA_TYPE_OCTET_STRING | DLMS_DATA_TYPE_BYREF) ||
            e->value.vt == (DLMS_DATA_TYPE_STRING | DLMS_DATA_TYPE_BYREF) ||
            e->value.vt == (DLMS_DATA_TYPE_STRING_UTF8 | DLMS_DATA_TYPE_BYREF))
        {
            if ((target->vt & e->value.vt) == 0)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            else
            {
                gxByteBuffer tmp;
                bb_attach(&tmp, target->pVal, target->size, target->capacity);
                bb_clear(&tmp);
                if ((ret = bb_set(&tmp, e->value.pVal, e->value.size)) == 0)
                {
                    target->size = e->value.size;
                }
            }
        }
        else
        {
            *target = e->value;
            ret = 0;
        }
    }
    else
    {
        ret = cosem_getVariant(e->value.byteArr, target);
    }
    return ret;
}

#ifndef DLMS_IGNORE_DATA
int cosem_setData(gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
        ret = cosem_updateVariant(&((gxData*)e->target)->value, e);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DATA

#ifndef DLMS_IGNORE_REGISTER
int cosem_setRegister(gxRegister* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getVariant(value->byteArr, &object->value);
    }
    else if (index == 3)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
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
#endif //DLMS_IGNORE_REGISTER

#ifndef DLMS_IGNORE_REGISTER_TABLE
int cosem_setRegistertable(gxRegisterTable* object, unsigned char index, dlmsVARIANT* value)
{
    int pos, ret;
    dlmsVARIANT* tmp;
    if (index == 2)
    {
        uint16_t count;
        if ((ret = cosem_verifyVariantArray(value->byteArr, &object->tableCellValues, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getVariantArrayItem(&object->tableCellValues, pos, &tmp)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, tmp)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 4)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
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
#endif //DLMS_IGNORE_REGISTER_TABLE

#ifndef DLMS_IGNORE_CLOCK
int cosem_setClock(dlmsSettings* settings, gxClock* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->time);
#ifndef DLMS_IGNORE_SERVER
        if (ret == 0)
        {
            //If user set's new time transform it to the same time zone as the server is.
            if (settings->server)
            {
                //Convert time to UCT if time zone is given.
                time_toUTC(&object->time);
                clock_updateDST(object, &object->time);
            }
        }
#endif// DLMS_IGNORE_SERVER
        break;
    case 3:
        ret = cosem_getInt16(value->byteArr, &object->timeZone);
        break;
    case 4:
        if ((ret = cosem_getUInt8(value->byteArr, &ch)) == 0)
        {
            object->status = (DLMS_CLOCK_STATUS)ch;
        }
        break;
    case 5:
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->begin);
#ifndef DLMS_IGNORE_SERVER
        if (ret == 0)
        {
            //If user set's new time transform it to the same time zone as the server is.
            if (settings->server)
            {
                //Convert time to UCT if time zone is given.
                time_toUTC(&object->time);
                clock_updateDST(object, &object->time);
            }
        }
#endif// DLMS_IGNORE_SERVER
        break;
    case 6:
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->end);
#ifndef DLMS_IGNORE_SERVER
        if (ret == 0)
        {
            //If user set's new time transform it to the same time zone as the server is.
            if (settings->server)
            {
                //Convert time to UCT if time zone is given.
                time_toUTC(&object->time);
                clock_updateDST(object, &object->time);
            }
        }
#endif// DLMS_IGNORE_SERVER
        break;
    case 7:
        ret = cosem_getInt8(value->byteArr, &object->deviation);
#ifndef DLMS_IGNORE_SERVER
        if (settings->server)
        {
            clock_updateDST(object, &object->time);
        }
#endif// DLMS_IGNORE_SERVER
        break;
    case 8:
        ret = cosem_getBoolean(value->byteArr, &object->enabled);
#ifndef DLMS_IGNORE_SERVER
        if (settings->server)
        {
            clock_updateDST(object, &object->time);
        }
#endif //DLMS_IGNORE_SERVER
        break;
    case 9:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->clockBase = (DLMS_CLOCK_BASE)ch;
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}

#endif //DLMS_IGNORE_CLOCK

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR

// The season profiles list is sorted according to season_start (in increasing order).
int cosem_orderSeasonProfile(gxArray* profile)
{
    int ret = 0;
    uint16_t pos, pos2, minPos;
    gxSeasonProfile* sp, * sp2;
    uint32_t tmp, next1, next2;
    for (pos = 0; pos < profile->size - 1; ++pos)
    {
        if ((ret = cosem_getArrayItem(profile, pos, (void**)&sp, sizeof(gxSeasonProfile))) != 0)
        {
            break;
        }
        next1 = time_toUnixTime2(&sp->start);
        next2 = 0xFFFFFFFF;
        for (pos2 = pos + 1; pos2 < profile->size; ++pos2)
        {
            if ((ret = cosem_getArrayItem(profile, pos2, (void**)&sp2, sizeof(gxSeasonProfile))) != 0)
            {
                break;
            }
            tmp = time_toUnixTime2(&sp2->start);
            if (tmp < next2)
            {
                next2 = tmp;
                minPos = pos2;
            }
        }
        if (ret != 0)
        {
            break;
        }
        if (next2 < next1)
        {
            gxSeasonProfile tmp;
            if ((ret = arr_swap(profile, pos, minPos, sizeof(gxSeasonProfile), (void*)&tmp)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int updateSeasonProfile(gxArray* profile, dlmsVARIANT* data)
{
    int ret, pos;
    gxSeasonProfile* sp = NULL;
    obj_clearSeasonProfile(profile);
    uint16_t count;
    if ((ret = cosem_verifyArray(data->byteArr, profile, &count)) == 0) {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
            if ((ret = cosem_getArrayItem(profile, pos, (void**)&sp, sizeof(gxSeasonProfile))) != 0 ||
                (ret = cosem_checkStructure(data->byteArr, 3)) != 0 ||
                (ret = cosem_getOctetString(data->byteArr, &sp->name)) != 0 ||
                (ret = cosem_getDateTimeFromOctetString(data->byteArr, &sp->start)) != 0 ||
                (ret = cosem_getOctetString(data->byteArr, &sp->weekName)) != 0)
            {
                break;
            }
#else
            if ((ret = cosem_getArrayItem(profile, pos, (void**)&sp, sizeof(gxSeasonProfile))) != 0 ||
                (ret = cosem_checkStructure(data->byteArr, 3)) != 0 ||
                (ret = cosem_getOctetString2(data->byteArr, sp->name.value, sizeof(sp->name.value), &sp->name.size)) != 0 ||
                (ret = cosem_getDateTimeFromOctetString(data->byteArr, &sp->start)) != 0 ||
                (ret = cosem_getOctetString2(data->byteArr, sp->weekName.value, sizeof(sp->weekName.value), &sp->weekName.size)) != 0)
            {
                break;
            }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
        }
    }
    if (ret == 0)
    {
        ret = cosem_orderSeasonProfile(profile);
    }
    return ret;
}

int updateWeekProfileTable(gxArray* profile, dlmsVARIANT* data)
{
    int ret, pos;
    gxWeekProfile* wp = NULL;
    obj_clearWeekProfileTable(profile);
    uint16_t count;
    if ((ret = cosem_verifyArray(data->byteArr, profile, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(profile, pos, (void**)&wp, sizeof(gxWeekProfile))) != 0 ||
                (ret = cosem_checkStructure(data->byteArr, 8)) != 0 ||
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
                (ret = cosem_getOctetString(data->byteArr, &wp->name)) != 0 ||
#else
                (ret = cosem_getOctetString2(data->byteArr, wp->name.value, sizeof(wp->name.value), &wp->name.size)) != 0 ||
#endif //DLMS_COSEM_EXACT_DATA_TYPES
                (ret = cosem_getUInt8(data->byteArr, &wp->monday)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &wp->tuesday)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &wp->wednesday)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &wp->thursday)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &wp->friday)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &wp->saturday)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &wp->sunday)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int updateDayProfileTable(dlmsSettings* settings, gxArray* profile, dlmsVARIANT* data)
{
    int ret, pos, pos2;
    gxDayProfile* dp = NULL;
    gxDayProfileAction* ac = NULL;
    obj_clearDayProfileTable(profile);
    uint16_t count;
    uint16_t count2;
    if ((ret = cosem_verifyArray(data->byteArr, profile, &count)) == 0) {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(profile, pos, (void**)&dp, sizeof(gxDayProfile))) != 0)
            {
                break;
            }
            count2 = arr_getCapacity(&dp->daySchedules);
            if ((ret = cosem_checkStructure(data->byteArr, 2)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &dp->dayId)) != 0 ||
                (ret = cosem_verifyArray(data->byteArr, &dp->daySchedules, &count2)) != 0)
            {
                break;
            }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
            for (pos2 = 0; pos2 != count2; ++pos2)
            {
                if ((ret = cosem_getArrayItem(&dp->daySchedules, pos2, (void**)&ac, sizeof(gxDayProfileAction))) != 0 ||
                    (ret = cosem_checkStructure(data->byteArr, 3)) != 0 ||
                    (ret = cosem_getTimeFromOctetString(data->byteArr, &ac->startTime)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctetString2(data->byteArr, ln, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctetString2(data->byteArr, ac->scriptLogicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(data->byteArr, &ac->scriptSelector)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, &ac->script)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
    }
    return ret;
}

int cosem_setActivityCalendar(dlmsSettings* settings, gxActivityCalendar* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getOctetString(value->byteArr, &object->calendarNameActive);
    }
    else if (index == 3)
    {
        ret = updateSeasonProfile(&object->seasonProfileActive, value);
    }
    else if (index == 4)
    {
        ret = updateWeekProfileTable(&object->weekProfileTableActive, value);
    }
    else if (index == 5)
    {
        ret = updateDayProfileTable(settings, &object->dayProfileTableActive, value);
    }
    else if (index == 6)
    {
        ret = cosem_getOctetString(value->byteArr, &object->calendarNamePassive);
    }
    else if (index == 7)
    {
        ret = updateSeasonProfile(&object->seasonProfilePassive, value);
    }
    else if (index == 8)
    {
        ret = updateWeekProfileTable(&object->weekProfileTablePassive, value);
    }
    else if (index == 9)
    {
        ret = updateDayProfileTable(settings, &object->dayProfileTablePassive, value);
    }
    else if (index == 10)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->time);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif // DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int cosem_setActionSchedule(
    dlmsSettings* settings,
    gxActionSchedule* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret, pos;
    gxtime* tm;
    unsigned char ch;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
            (ret = cosem_getOctetString2(value->byteArr, object->executedScriptLogicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getUInt16(value->byteArr, &object->executedScriptSelector)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0)
        {
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&object->executedScript)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->type = (DLMS_SINGLE_ACTION_SCHEDULE_TYPE)ch;
        }
    }
    else if (index == 4)
    {
        arr_clear(&object->executionTime);
        gxtime t;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->executionTime, &count)) == 0) {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getTimeFromOctetString(value->byteArr, &t)) != 0 ||
                    (ret = cosem_getDateFromOctetString(value->byteArr, tm)) != 0)
                {
                    break;
                }
#ifdef DLMS_USE_EPOCH_TIME
                time_clearTime(tm);
                time_clearDate(&t);
                time_addHours(tm, time_getHours(&t));
                time_addMinutes(tm, time_getMinutes(&t));
                time_addSeconds(tm, time_getSeconds(&t));
                tm->skip |= t.skip;
#else
                time_clearTime(tm);
                time_clearDate(&t);
                tm->value.tm_hour = t.value.tm_hour;
                tm->value.tm_min = t.value.tm_min;
                tm->value.tm_sec = t.value.tm_sec;
                tm->skip |= t.skip;
#endif // DLMS_USE_EPOCH_TIME
                tm->deviation = (short)0x8000;
                tm->skip |= DATETIME_SKIPS_DEVITATION;
            }
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE

int cosem_updateAttributeAccessModes(gxObject* object, gxByteBuffer* data)
{
    signed char id;
    unsigned char value, selector;
    int ret;
    uint16_t pos, cnt, pos2, cnt2;
    if ((ret = cosem_getStructure(data, &cnt)) != 0 ||
        //If accessmodes are not returned. Some meters do not return them.
        cnt != 2)
    {
        return ret;
    }
    //If access modes are not retrieved yet.
    if (object->access == NULL || object->access->attributeAccessModes.size == 0)
    {
#ifndef DLMS_IGNORE_MALLOC
        if (object->access == NULL)
        {
            object->access = (gxAccess*)gxmalloc(sizeof(gxAccess));
        }
        BYTE_BUFFER_INIT(&object->access->attributeAccessModes);
        BYTE_BUFFER_INIT(&object->access->methodAccessModes);
        cnt = obj_methodCount(object);
        bb_capacity(&object->access->methodAccessModes, cnt);
        object->access->methodAccessModes.size = object->access->methodAccessModes.capacity;
        cnt = obj_attributeCount(object);
        bb_capacity(&object->access->attributeAccessModes, cnt);
        object->access->attributeAccessModes.size = object->access->attributeAccessModes.capacity;
#else
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_MALLOC
    }
    //Get attribute access list.
    if ((ret = cosem_checkArray(data, &cnt)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != cnt; ++pos)
    {
        if ((ret = cosem_checkStructure(data, 3)) != 0 ||
            (ret = cosem_getInt8(data, &id)) != 0 ||
            (ret = cosem_getEnum(data, &value)) != 0)
        {
            break;
        }
        if (!(id > (char)object->access->attributeAccessModes.size))
        {
            if ((ret = bb_setUInt8ByIndex(&object->access->attributeAccessModes, (unsigned char)(id - 1), value)) != 0)
            {
                break;
            }
        }
        //Get selector(s)
        if ((ret = bb_getUInt8(data, &selector)) != 0)
        {
            break;
        }
        if (selector != DLMS_DATA_TYPE_NONE)
        {
            //Skip selector.
            --data->position;
            //Selectors are not saved nowhere.
            if ((ret = cosem_checkArray(data, &cnt2)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != cnt2; ++pos2)
            {
                if ((ret = cosem_getInt8(data, &id)) != 0)
                {
                    break;
                }
            }
        }
    }
    //Get method access list.
    cnt = (uint16_t)object->access->methodAccessModes.size;
    if ((ret = cosem_checkArray(data, &cnt)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != cnt; ++pos)
    {
        if ((ret = cosem_checkStructure(data, 2)) != 0 ||
            (ret = cosem_getInt8(data, &id)) != 0)
        {
            break;
        }
        if ((ret = cosem_getEnum(data, &value)) != 0)
        {
            //Version 0 returns method access as boolean.
            if ((ret = cosem_getBoolean(data, &value)) != 0)
            {
                break;
            }
        }
        if (!(id > (char)object->access->methodAccessModes.size))
        {
            if ((ret = bb_setUInt8ByIndex(&object->access->methodAccessModes, (unsigned char)(id - 1), value)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
int cosem_parseLNObjects(dlmsSettings* settings, gxByteBuffer* data, objectArray* objects)
{
#ifdef DLMS_IGNORE_MALLOC
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
#else
    gxObject* object;
    int ret;
    uint16_t pos, count = objects->capacity;
    unsigned char version;
    uint16_t class_id;
    unsigned char ln[6];
    if ((ret = cosem_checkArray(data, &count)) == 0)
    {
        oa_capacity(objects, count);
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_checkStructure(data, 4)) != 0 ||
                (ret = cosem_getUInt16(data, &class_id)) != 0 ||
                (ret = cosem_getUInt8(data, &version)) != 0 ||
                (ret = cosem_getOctetString2(data, ln, sizeof(ln), NULL)) != 0)
            {
                break;
            }
            object = NULL;
            ret = cosem_createObject(class_id, &object);
            //If known object.
            if (object != NULL && ret == 0)
            {
                object->version = version;
                ret = cosem_updateAttributeAccessModes(object, data);
                if (ret != 0)
                {
                    break;
                }
                cosem_setLogicalName(object, ln);
                oa_push(objects, object);
                oa_push(&settings->releasedObjects, object);
            }
            else
            {
                if (ret != DLMS_ERROR_CODE_UNAVAILABLE_OBJECT)
                {
                    break;
                }
                ret = 0;
                gxfree(object);
            }
        }
    }
    return ret;
#endif //DLMS_IGNORE_MALLOC
}

int cosem_setAssociationLogicalName(
    dlmsSettings* settings,
    gxAssociationLogicalName* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret, pos = 0;
    unsigned char ch;
    if (index == 2)
    {
        oa_empty(&object->objectList);
        ret = cosem_parseLNObjects(settings, value->byteArr, &object->objectList);
    }
    else if (index == 3)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->clientSAP)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->serverSAP)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (index == 4)
    {
        //Value of the object identifier encoded in BER
        if ((ret = bb_getUInt8(value->byteArr, &ch)) == 0)
        {
            if (ch == 0x60)
            {
                object->applicationContextName.jointIsoCtt = 0;
                object->applicationContextName.country = 0;
                object->applicationContextName.countryName = 0;
                if ((ret = bb_getUInt8ByIndex(value->byteArr, 3, &object->applicationContextName.identifiedOrganization)) == 0 &&
                    (ret = bb_getUInt8ByIndex(value->byteArr, 4, &object->applicationContextName.dlmsUA)) == 0 &&
                    (ret = bb_getUInt8ByIndex(value->byteArr, 5, &object->applicationContextName.applicationContext)) == 0 &&
                    (ret = bb_getUInt8ByIndex(value->byteArr, 6, &object->applicationContextName.contextId)) == 0)
                {
                }
            }
            else
            {
                //Get Tag.
                if (ch != 2 ||
                    //Get Len.
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 7 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->applicationContextName.jointIsoCtt)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->applicationContextName.country)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x12 ||
                    (ret = bb_getUInt16(value->byteArr, &object->applicationContextName.countryName)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->applicationContextName.identifiedOrganization)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->applicationContextName.dlmsUA)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->applicationContextName.applicationContext)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->applicationContextName.contextId)) != 0)
                {
                    if (ret == 0)
                    {
                        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    }
                }
            }
        }
    }
    else if (index == 5)
    {
        uint32_t conformance;
        //Get structure count.
        if ((ret = cosem_checkStructure(value->byteArr, 6)) != 0 ||
            (ret = cosem_getIntegerFromBitString(value->byteArr, &conformance)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->xDLMSContextInfo.maxReceivePduSize)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->xDLMSContextInfo.maxSendPduSize)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->xDLMSContextInfo.dlmsVersionNumber)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->xDLMSContextInfo.qualityOfService)) != 0 ||
            (ret = cosem_getOctetString(value->byteArr, &object->xDLMSContextInfo.cypheringInfo)) != 0)
        {
            return ret;
        }
        object->xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)conformance;
    }
    else if (index == 6)
    {
        //Value of the object identifier encoded in BER
        if ((ret = bb_getUInt8(value->byteArr, &ch)) == 0)
        {
            if (ch == 0x60)
            {
                object->authenticationMechanismName.jointIsoCtt = 0;
                object->authenticationMechanismName.country = 0;
                object->authenticationMechanismName.countryName = 0;
                if ((ret = bb_getUInt8ByIndex(value->byteArr, 3, &object->authenticationMechanismName.identifiedOrganization)) == 0 &&
                    (ret = bb_getUInt8ByIndex(value->byteArr, 4, &object->authenticationMechanismName.dlmsUA)) == 0 &&
                    (ret = bb_getUInt8ByIndex(value->byteArr, 5, &object->authenticationMechanismName.authenticationMechanismName)) == 0 &&
                    (ret = bb_getUInt8ByIndex(value->byteArr, 6, &ch)) == 0)
                {
                    object->authenticationMechanismName.mechanismId = (DLMS_AUTHENTICATION)ch;
                }
            }
            else
            {
                //Get Tag.
                if (ch != 2 ||
                    //Get Len.
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 7 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->authenticationMechanismName.jointIsoCtt)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->authenticationMechanismName.country)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x12 ||
                    (ret = bb_getUInt16(value->byteArr, &object->authenticationMechanismName.countryName)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->authenticationMechanismName.identifiedOrganization)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->authenticationMechanismName.dlmsUA)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &object->authenticationMechanismName.authenticationMechanismName)) != 0 ||
                    //Get tag
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0 ||
                    ch != 0x11 ||
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0)
                {
                    if (ret == 0)
                    {
                        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    }
                }
                else
                {
                    object->authenticationMechanismName.mechanismId = (DLMS_AUTHENTICATION)ch;
                }
            }
        }
    }
    else if (index == 7)
    {
        ret = cosem_getOctetString(value->byteArr, &object->secret);
    }
    else if (index == 8)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->associationStatus = (DLMS_ASSOCIATION_STATUS)ch;
        }
    }
    else if (index == 9)
    {
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        unsigned char ln[6];
        if ((ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln, (gxObject**)&object->securitySetup);
        }
#else
        ret = cosem_getOctetString2(value->byteArr, object->securitySetupReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 10)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->userList, &count)) == 0)
        {
#ifndef DLMS_COSEM_EXACT_DATA_TYPES
            gxUser* it;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->userList, pos, (void**)&it, sizeof(gxUser))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &it->id)) != 0 ||
                    (ret = cosem_getString2(value->byteArr, it->name, sizeof(it->name))) != 0)
                {
                    break;
                }
            }
#else
            gxKey2* it;
            for (pos = 0; pos != count; ++pos)
            {
                it = (gxKey2*)gxmalloc(sizeof(gxKey2));
                if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &it->key)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ch)) != 0)
                {
                    break;
                }
                if (ch != DLMS_DATA_TYPE_OCTET_STRING)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                    break;
                }
                else
                {
                    //Get length.
                    if ((ret = cosem_getUInt8(value->byteArr, &ch)) == 0)
                    {
                        it->value = gxmalloc(ch + 1);
                        if ((ret = cosem_getString2(value->byteArr, it->value, ch)) != 0)
                        {
                            gxfree(it->value);
                            gxfree(it);
                            break;
                        }
                        arr_push(&object->userList, it);
                    }
                }
            }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
        }
    }
    else if (index == 11)
    {
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
        unsigned char ch;
        if (object->currentUser.value != NULL)
        {
            gxfree(object->currentUser.value);
        }
        if ((ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
            (ret = cosem_getUInt8(value->byteArr, &object->currentUser.key)) == 0 &&
            (ret = bb_getUInt8(value->byteArr, &ch)) == 0)
        {
            if (ch != DLMS_DATA_TYPE_STRING)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            else
            {
                //Get length.
                if ((ret = bb_getUInt8(value->byteArr, &ch)) == 0)
                {
                    object->currentUser.value = gxmalloc(ch + 1);
                    if ((ret = bb_get(value->byteArr, object->currentUser.value, ch)) != 0)
                    {
                        gxfree(object->currentUser.value);
                    }
                }
            }
        }
#else
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->currentUser.id)) != 0 ||
            (ret = cosem_getString2(value->byteArr, object->currentUser.name, sizeof(object->currentUser.name))) != 0)
        {
            //Error code is returned at the end of the function.
        }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int updateSNAccessRights(
    objectArray* objectList,
    variantArray* data)
{
    uint16_t sn;
    int pos, ret;
    dlmsVARIANT* it, * tmp;
    gxObject* obj = NULL;
    for (pos = 0; pos != data->size; ++pos)
    {
        ret = va_getByIndex(data, pos, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_getByIndex(it->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        sn = (uint16_t)var_toInteger(tmp);

        ret = oa_findBySN(objectList, sn, &obj);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj != NULL)
        {
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int cosem_parseSNObjects(dlmsSettings* settings, gxByteBuffer* data, objectArray* objects)
{
#ifdef DLMS_IGNORE_MALLOC
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
#else
    gxDataInfo info;
    short sn;
    DLMS_OBJECT_TYPE class_id;
    dlmsVARIANT value;
    dlmsVARIANT* it1 = NULL, * it2 = NULL, * it3 = NULL, * ln = NULL;
    gxObject* object;
    int ret;
    uint16_t count, pos;
    unsigned char size, version;
    var_init(&value);
    //Get array tag.
    if ((ret = bb_getUInt8(data, &size)) != DLMS_ERROR_CODE_OK)
    {
        return 0;
    }
    //Check that data is in the array
    if (size != 0x01)
    {
        return DLMS_ERROR_CODE_INVALID_RESPONSE;
    }
    //get object count
    if ((ret = hlp_getObjectCount2(data, &count)) != 0)
    {
        return ret;
    }
    oa_capacity(objects, (uint16_t)count);
    for (pos = 0; pos != count; ++pos)
    {
        var_clear(&value);
        object = NULL;
        di_init(&info);
        if ((ret = dlms_getData(data, &info, &value)) != 0)
        {
            break;
        }
        if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr->size != 4)
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        }
        if ((ret = va_getByIndex(value.Arr, 0, &it1)) != 0 ||
            (ret = va_getByIndex(value.Arr, 1, &it2)) != 0 ||
            (ret = va_getByIndex(value.Arr, 2, &it3)) != 0 ||
            (ret = va_getByIndex(value.Arr, 3, &ln)) != 0)
        {
            break;
        }
        if (it1->vt != DLMS_DATA_TYPE_INT16 ||
            it2->vt != DLMS_DATA_TYPE_UINT16 ||
            it3->vt != DLMS_DATA_TYPE_UINT8 ||
            ln->vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        }
        sn = (short)var_toInteger(it1);
        class_id = (DLMS_OBJECT_TYPE)var_toInteger(it2);
        version = (unsigned char)var_toInteger(it3);
        ret = cosem_createObject(class_id, &object);
        //If known object.
        if (ret == 0)
        {
            object->shortName = sn;
            object->version = version;
            cosem_setLogicalName(object, ln->byteArr->data);
            oa_push(objects, object);
            oa_push(&settings->releasedObjects, object);
        }
        else
        {
            if (ret != DLMS_ERROR_CODE_UNAVAILABLE_OBJECT)
            {
                break;
            }
            ret = 0;
        }
    }
    var_clear(&value);
    return ret;
#endif //DLMS_IGNORE_MALLOC
}

int cosem_setAssociationShortName(
    dlmsSettings* settings,
    gxAssociationShortName* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
    }
    else if (index == 3)
    {
        return updateSNAccessRights(&object->objectList, value->Arr);
    }
    else if (index == 4)
    {
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        unsigned char ln[6];
        if ((ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln, (gxObject**)&object->securitySetup);
        }
#else
        ret = cosem_getOctetString2(value->byteArr, object->securitySetupReference, 6, NULL);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_AUTO_ANSWER
int cosem_setAutoAnswer(gxAutoAnswer* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret, pos;
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->mode = (DLMS_AUTO_ANSWER_MODE)ch;
        }
    }
    else if (index == 3)
    {
        arr_clearKeyValuePair(&object->listeningWindow);
        gxTimePair* it;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->listeningWindow, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->listeningWindow, pos, (void**)&it, sizeof(gxTimePair))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getDateTimeFromOctetString(value->byteArr, &it->first)) != 0 ||
                    (ret = cosem_getDateTimeFromOctetString(value->byteArr, &it->second)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 4)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->status = (DLMS_AUTO_ANSWER_STATUS)ch;
        }
    }
    else if (index == 5)
    {
        ret = cosem_getUInt8(value->byteArr, &object->numberOfCalls);
    }
    else if (index == 6)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->numberOfRingsInListeningWindow)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->numberOfRingsOutListeningWindow)) != 0)
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
#endif //DLMS_IGNORE_AUTO_ANSWER

#ifndef DLMS_IGNORE_AUTO_CONNECT
int cosem_setAutoConnect(gxAutoConnect* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret, pos;
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->mode = (DLMS_AUTO_CONNECT_MODE)ch;
        }
    }
    else if (index == 3)
    {
        ret = cosem_getUInt8(value->byteArr, &object->repetitions);
    }
    else if (index == 4)
    {
        ret = cosem_getUInt16(value->byteArr, &object->repetitionDelay);
    }
    else if (index == 5)
    {
        gxTimePair* k;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->callingWindow, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->callingWindow, pos, (void**)&k, sizeof(gxTimePair)) != 0) ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getDateTimeFromOctetString(value->byteArr, &k->first)) != 0 ||
                    (ret = cosem_getDateTimeFromOctetString(value->byteArr, &k->second)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 6)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->destinations, &count)) == 0)
        {
#ifndef DLMS_COSEM_EXACT_DATA_TYPES
            gxDestination* d;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->destinations, pos, (void**)&d, sizeof(gxDestination)) != 0) ||
                    (ret = cosem_getOctetString2(value->byteArr, d->value, sizeof(d->value), &d->size)) != 0)
                {
                    break;
                }
            }
#else
            gxByteBuffer* it;
            for (pos = 0; pos != count; ++pos)
            {
                it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(it);
                if ((ret = cosem_getOctetString(value->byteArr, it)) != 0 ||
                    (ret = arr_push(&object->destinations, it)) != 0)
                {
                    break;
                }
            }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
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
int cosem_setDemandRegister(gxDemandRegister* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getVariant(value->byteArr, &object->currentAverageValue);
    }
    else if (index == 3)
    {
        ret = cosem_getVariant(value->byteArr, &object->lastAverageValue);
    }
    else if (index == 4)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (index == 5)
    {
        ret = cosem_getVariant(value->byteArr, &object->status);
    }
    else if (index == 6)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->captureTime);
    }
    else if (index == 7)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->startTimeCurrent);
    }
    else if (index == 8)
    {
        ret = cosem_getUInt32(value->byteArr, &object->period);
    }
    else if (index == 9)
    {
        ret = cosem_getUInt16(value->byteArr, &object->numberOfPeriods);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER

#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int cosem_setMacAddressSetup(gxMacAddressSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getOctetString3(value->byteArr, &object->macAddress, 0);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int cosem_setExtendedRegister(gxExtendedRegister* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getVariant(value->byteArr, &object->value);
    }
    else if (index == 3)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (index == 4)
    {
        ret = cosem_getVariant(value->byteArr, &object->status);
    }
    else if (index == 5)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->captureTime);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

#ifndef DLMS_IGNORE_GPRS_SETUP
int cosem_setGprsSetup(gxGPRSSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getOctetString(value->byteArr, &object->apn);
    }
    else if (index == 3)
    {
        ret = cosem_getUInt16(value->byteArr, &object->pinCode);
    }
    else if (index == 4)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_checkStructure(value->byteArr, 5)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->defaultQualityOfService.precedence)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->defaultQualityOfService.delay)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->defaultQualityOfService.reliability)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->defaultQualityOfService.peakThroughput)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->defaultQualityOfService.meanThroughput)) != 0 ||
            (ret = cosem_checkStructure(value->byteArr, 5)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->requestedQualityOfService.precedence)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->requestedQualityOfService.delay)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->requestedQualityOfService.reliability)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->requestedQualityOfService.peakThroughput)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->requestedQualityOfService.meanThroughput)) != 0)
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
#endif //DLMS_IGNORE_GPRS_SETUP

#ifndef DLMS_IGNORE_SECURITY_SETUP
int cosem_setSecuritySetup(dlmsSettings* settings, gxSecuritySetup* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int pos, ret;
    gxCertificateInfo* it = NULL;
    switch (index)
    {
    case 2:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->securityPolicy = (DLMS_SECURITY_POLICY)ch;
        }
        break;
    case 3:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->securitySuite = (DLMS_SECURITY_SUITE)ch;
        }
        break;
    case 4:
        ret = cosem_getOctetString(value->byteArr, &object->clientSystemTitle);
        break;
    case 5:
        ret = cosem_getOctetString(value->byteArr, &object->serverSystemTitle);
        break;
    case 6:
        obj_clearCertificateInfo(&object->certificates);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->certificates, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->certificates, pos, (void**)&it, sizeof(gxCertificateInfo))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 6)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&it->entity)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&it->type)) != 0 ||
                    (ret = cosem_getString2(value->byteArr, it->serialNumber, sizeof(it->serialNumber))) != 0 ||
                    (ret = cosem_getString2(value->byteArr, it->issuer, sizeof(it->issuer))) != 0 ||
                    (ret = cosem_getString2(value->byteArr, it->subject, sizeof(it->subject))) != 0 ||
                    (ret = cosem_getString2(value->byteArr, it->subject, sizeof(it->subjectAltName))) != 0)
                {
                    break;
                }
            }
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int cosem_setIecHdlcSetup(gxIecHdlcSetup* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret;
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->communicationSpeed = (DLMS_BAUD_RATE)ch;
        }
    }
    else if (index == 3)
    {
        ret = cosem_getUInt8(value->byteArr, &object->windowSizeTransmit);
    }
    else if (index == 4)
    {
        ret = cosem_getUInt8(value->byteArr, &object->windowSizeReceive);
    }
    else if (index == 5)
    {
        ret = cosem_getUInt16(value->byteArr, &object->maximumInfoLengthTransmit);
    }
    else if (index == 6)
    {
        ret = cosem_getUInt16(value->byteArr, &object->maximumInfoLengthReceive);
    }
    else if (index == 7)
    {
        ret = cosem_getUInt16(value->byteArr, &object->interCharachterTimeout);
    }
    else if (index == 8)
    {
        ret = cosem_getUInt16(value->byteArr, &object->inactivityTimeout);
    }
    else if (index == 9)
    {
        ret = cosem_getUInt16(value->byteArr, &object->deviceAddress);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int cosem_setIecLocalPortSetup(gxLocalPortSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    unsigned char ch;
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->defaultMode = (DLMS_OPTICAL_PROTOCOL_MODE)ch;
        }
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->defaultBaudrate = (DLMS_BAUD_RATE)ch;
        }
    }
    else if (index == 4)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->proposedBaudrate = (DLMS_BAUD_RATE)ch;
        }
    }
    else if (index == 5)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->responseTime = (DLMS_LOCAL_PORT_RESPONSE_TIME)ch;
        }
    }
    else if (index == 6)
    {
        ret = cosem_getOctetString(value->byteArr, &object->deviceAddress);
    }
    else if (index == 7)
    {
        ret = cosem_getOctetString(value->byteArr, &object->password1);
    }
    else if (index == 8)
    {
        ret = cosem_getOctetString(value->byteArr, &object->password2);
    }
    else if (index == 9)
    {
        ret = cosem_getOctetString(value->byteArr, &object->password5);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int cosem_setIecTwistedPairSetup(gxIecTwistedPairSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    uint16_t count, pos;
    unsigned char ch;
    signed char it;
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->mode = (DLMS_IEC_TWISTED_PAIR_SETUP_MODE)ch;
        }
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->speed = (DLMS_BAUD_RATE)ch;
        }
    }
    else if (index == 4)
    {
        bb_clear(&object->primaryAddresses);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getUInt8(value->byteArr, &ch)) != 0 ||
                    (ret = bb_setUInt8(&object->primaryAddresses, ch)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        bb_clear(&object->tabis);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getInt8(value->byteArr, &it)) != 0 ||
                    (ret = bb_setUInt8(&object->tabis, it)) != 0)
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
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_IP4_SETUP
int cosem_setIP4Setup(dlmsSettings* settings, gxIp4Setup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    gxip4SetupIpOption* ipItem;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
            {
                return ret;
            }
        }
#else
        ret = cosem_getOctetString2(value->byteArr, object->dataLinkLayerReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        ret = cosem_getUInt32(value->byteArr, &object->ipAddress);
    }
    else if (index == 4)
    {
        uint32_t* v;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->multicastIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->multicastIPAddress, pos, (void**)&v, sizeof(uint32_t))) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, v)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->ipOptions, &count)) == 0)
        {
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
            for (pos = 0; pos != count; ++pos)
            {
                ipItem = (gxip4SetupIpOption*)gxmalloc(sizeof(gxip4SetupIpOption));
                BYTE_BUFFER_INIT(&ipItem->data);
                if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&ipItem->type)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ipItem->length)) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, &ipItem->data)) != 0 ||
                    (ret = arr_push(&object->multicastIPAddress, ipItem)) != 0)
                {
                    break;
                }
            }
#else
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->ipOptions, pos, (void**)&ipItem, sizeof(gxip4SetupIpOption))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&ipItem->type)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ipItem->length)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, ipItem->data.value, sizeof(ipItem->data.value), &ipItem->data.size)) != 0)
                {
                    break;
                }
            }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
        }
    }
    else if (index == 6)
    {
        ret = cosem_getUInt32(value->byteArr, &object->subnetMask);
    }
    else if (index == 7)
    {
        ret = cosem_getUInt32(value->byteArr, &object->gatewayIPAddress);
    }
    else if (index == 8)
    {
        ret = cosem_getBoolean(value->byteArr, &object->useDHCP);
    }
    else if (index == 9)
    {
        ret = cosem_getUInt32(value->byteArr, &object->primaryDNSAddress);
    }
    else if (index == 10)
    {
        ret = cosem_getUInt32(value->byteArr, &object->secondaryDNSAddress);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP
int cosem_setIP6Setup(dlmsSettings* settings, gxIp6Setup* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret, pos;
    IN6_ADDR* v;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
            {
                return ret;
            }
        }
#else
        ret = cosem_getOctetString2(value->byteArr, object->dataLinkLayerReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->addressConfigMode = (DLMS_IP6_ADDRESS_CONFIG_MODE)ch;
        }
    }
    else if (index == 4)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->unicastIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->unicastIPAddress, pos, (void**)&v, sizeof(IN6_ADDR))) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, (unsigned char*)v, sizeof(IN6_ADDR), NULL)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->multicastIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->multicastIPAddress, pos, (void**)&v, sizeof(IN6_ADDR))) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, (unsigned char*)v, sizeof(IN6_ADDR), NULL)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 6)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->gatewayIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->gatewayIPAddress, pos, (void**)&v, sizeof(IN6_ADDR))) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, (unsigned char*)v, sizeof(IN6_ADDR), NULL)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 7)
    {
        ret = cosem_getOctetString2(value->byteArr, (unsigned char*)&object->primaryDNSAddress, sizeof(IN6_ADDR), NULL);
    }
    else if (index == 8)
    {
        ret = cosem_getOctetString2(value->byteArr, (unsigned char*)&object->secondaryDNSAddress, sizeof(IN6_ADDR), NULL);
    }
    else if (index == 9)
    {
        ret = cosem_getUInt8(value->byteArr, &object->trafficClass);
    }
    else if (index == 10)
    {
        uint16_t count;
        gxNeighborDiscoverySetup* it;
        if ((ret = cosem_verifyArray(value->byteArr, &object->neighborDiscoverySetup, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->neighborDiscoverySetup, pos, (void**)&it, sizeof(gxNeighborDiscoverySetup))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &it->maxRetry)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &it->retryWaitTime)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &it->sendPeriod)) != 0)
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
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
int cosem_setUtilityTables(gxUtilityTables* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getUInt16(value->byteArr, &object->tableId);
    }
    else if (index == 3)
    {
        //Skip length.
        ret = 0;
    }
    else if (index == 4)
    {
        ret = cosem_getOctetString(value->byteArr, &object->buffer);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int cosem_setMbusSlavePortSetup(gxMbusSlavePortSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    unsigned char ch;
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->defaultBaud = (DLMS_BAUD_RATE)ch;
        }
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->availableBaud = (DLMS_BAUD_RATE)ch;
        }
    }
    else if (index == 4)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->addressState = (DLMS_ADDRESS_STATE)ch;
        }
    }
    else if (index == 5)
    {
        ret = cosem_getUInt8(value->byteArr, &object->busAddress);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int cosem_setDisconnectControl(gxDisconnectControl* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret;
    if (index == 2)
    {
        ret = cosem_getBoolean(value->byteArr, &object->outputState);
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->controlState = (DLMS_CONTROL_STATE)ch;
        }
    }
    else if (index == 4)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->controlMode = (DLMS_CONTROL_MODE)ch;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int cosem_setLimiter(dlmsSettings* settings, gxLimiter* object, unsigned char index, dlmsVARIANT* value)
{
    DLMS_OBJECT_TYPE ot = DLMS_OBJECT_TYPE_NONE;
    int ret, pos;
    uint16_t* it;
    if (index == 2)
    {
        unsigned char ln[6];
        ot = 0;
        if ((ret = cosem_checkStructure(value->byteArr, 3)) == 0 &&
            (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&ot)) == 0 &&
            (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0 &&
            (ret = cosem_getInt8(value->byteArr, &object->selectedAttributeIndex)) == 0)
        {
            ret = cosem_findObjectByLN(settings, ot, ln, &object->monitoredValue);
        }
    }
    else if (index == 3)
    {
        ret = cosem_getVariant(value->byteArr, &object->thresholdActive);
    }
    else if (index == 4)
    {
        ret = cosem_getVariant(value->byteArr, &object->thresholdNormal);
    }
    else if (index == 5)
    {
        ret = cosem_getVariant(value->byteArr, &object->thresholdEmergency);
    }
    else if (index == 6)
    {
        ret = cosem_getUInt32(value->byteArr, &object->minOverThresholdDuration);
    }
    else if (index == 7)
    {
        ret = cosem_getUInt32(value->byteArr, &object->minUnderThresholdDuration);
    }
    else if (index == 8)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->emergencyProfile.id)) != 0 ||
            (ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->emergencyProfile.activationTime)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->emergencyProfile.duration)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (index == 9)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->emergencyProfileGroupIDs, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(uint16_t))) != 0 ||
                    (ret = bb_getUInt16(value->byteArr, it)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 10)
    {
        ret = cosem_getBoolean(value->byteArr, &object->emergencyProfileActive);
    }
    else if (index == 11)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char lnOver[6];
        unsigned char lnUnder[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
            (ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getOctetString2(value->byteArr, lnOver, 6, NULL)) == 0 &&
#else
            (ret = cosem_getOctetString2(value->byteArr, object->actionOverThreshold.logicalName, 6, NULL)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getUInt16(value->byteArr, &object->actionOverThreshold.scriptSelector)) == 0 &&
            (ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getOctetString2(value->byteArr, lnUnder, 6, NULL)) == 0 &&
#else
            (ret = cosem_getOctetString2(value->byteArr, object->actionUnderThreshold.logicalName, 6, NULL)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getUInt16(value->byteArr, &object->actionUnderThreshold.scriptSelector)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnOver, (gxObject**)&object->actionOverThreshold.script)) == 0 &&
                (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnUnder, (gxObject**)&object->actionUnderThreshold.script)) == 0)
            {

            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
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
int cosem_setmMbusClient(dlmsSettings* settings, gxMBusClient* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret, pos;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP, ln, &object->mBusPort)) != 0)
            {
                return ret;
            }
        }
#else
        ret = cosem_getOctetString2(value->byteArr, object->mBusPortReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
#if defined(DLMS_COSEM_EXACT_DATA_TYPES)
        arr_clearKeyValuePair(&object->captureDefinition);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->captureDefinition, &count)) == 0)
        {
            gxByteBuffer* start, * end;
            for (pos = 0; pos != count; ++pos)
            {
                start = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                end = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(start);
                BYTE_BUFFER_INIT(end);
                if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, start)) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, end)) != 0 ||
                    (ret = arr_push(&object->captureDefinition, key_init(start, end))) != 0)
                {
                    break;
                }
            }
        }
#else
        gxCaptureDefinition* it;
        uint16_t count = arr_getCapacity(&object->captureDefinition);
        if ((ret = cosem_verifyArray(value->byteArr, &object->captureDefinition, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->captureDefinition, pos, (void**)&it, sizeof(gxCaptureDefinition))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, it->data.value, sizeof(it->data.value), &it->data.size)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, it->value.value, sizeof(it->value.value), &it->value.size)) != 0)
                {
                    break;
                }
            }
        }
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    }
    else if (index == 4)
    {
        ret = cosem_getUInt32(value->byteArr, &object->capturePeriod);
    }
    else if (index == 5)
    {
        ret = cosem_getUInt8(value->byteArr, &object->primaryAddress);
    }
    else if (index == 6)
    {
        ret = cosem_getUInt32(value->byteArr, &object->identificationNumber);
    }
    else if (index == 7)
    {
        ret = cosem_getUInt16(value->byteArr, &object->manufacturerID);
    }
    else if (index == 8)
    {
        ret = cosem_getUInt8(value->byteArr, &object->dataHeaderVersion);
    }
    else if (index == 9)
    {
        ret = cosem_getUInt8(value->byteArr, &object->deviceType);
    }
    else if (index == 10)
    {
        ret = cosem_getUInt8(value->byteArr, &object->accessNumber);
    }
    else if (index == 11)
    {
        ret = cosem_getUInt8(value->byteArr, &object->status);
    }
    else if (index == 12)
    {
        ret = cosem_getUInt8(value->byteArr, &object->alarm);
    }
    else if (index == 13 && object->base.version != 0)
    {
        ret = cosem_getUInt16(value->byteArr, &object->configuration);
    }
    else if (index == 14 && object->base.version != 0)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->encryptionKeyStatus = (DLMS_MBUS_ENCRYPTION_KEY_STATUS)ch;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int cosem_setModemConfiguration(gxModemConfiguration* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret, pos;
    gxModemInitialisation* modemInit;
#if !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxModemProfile* mp;
#endif //!defined(DLMS_COSEM_EXACT_DATA_TYPES)
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->communicationSpeed = (DLMS_BAUD_RATE)ch;
        }
    }
    else if (index == 3)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->initialisationStrings, &count)) == 0)
        {
            unsigned cnt = object->base.version != 0 ? 3 : 2;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->initialisationStrings, pos, (void**)&modemInit, sizeof(gxModemInitialisation))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, cnt)) != 0 ||
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
                    (ret = cosem_getOctetString(value->byteArr, &modemInit->request)) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, &modemInit->response)) != 0)
#else
                    (ret = cosem_getOctetString2(value->byteArr, modemInit->request.value, sizeof(modemInit->request.value), &modemInit->request.size)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, modemInit->response.value, sizeof(modemInit->response.value), &modemInit->response.size)) != 0)
#endif //DLMS_COSEM_EXACT_DATA_TYPES
                {
                    break;
                }
                if (object->base.version != 0)
                {
                    if ((ret = cosem_getUInt16(value->byteArr, &modemInit->delay)) != 0)
                    {
                        break;
                    }
                }
            }
        }
    }
    else if (index == 4)
    {
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->modemProfile, &count)) == 0)
        {
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
            gxByteBuffer* it;
            for (pos = 0; pos != count; ++pos)
            {
                it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(it);
                if ((ret = cosem_getOctetString(value->byteArr, it)) != 0 ||
                    (ret = arr_push(&object->modemProfile, it)) != 0)
                {
                    break;
                }
            }
#else
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->modemProfile, pos, (void**)&mp, sizeof(gxModemProfile))) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, mp->value, sizeof(mp->value), &mp->size)) != 0)
                {
                    break;
                }
            }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
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
int cosem_setPppSetup(dlmsSettings* settings, gxPppSetup* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret, pos;
    gxpppSetupLcpOption* lcpItem;
    gxpppSetupIPCPOption* ipcpItem;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->phy)) != 0)
            {
                return ret;
            }
        }
#else
        ret = cosem_getOctetString2(value->byteArr, object->PHYReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        arr_clear(&object->lcpOptions);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->lcpOptions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->lcpOptions, pos, (void**)&lcpItem, sizeof(gxpppSetupLcpOption))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&lcpItem->type)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &lcpItem->length)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, &lcpItem->data)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 4)
    {
        arr_clear(&object->ipcpOptions);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->ipcpOptions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->ipcpOptions, pos, (void**)&ipcpItem, sizeof(gxpppSetupIPCPOption))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&ipcpItem->type)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ipcpItem->length)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, &ipcpItem->data)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        if ((ret = bb_getUInt8(value->byteArr, &ch)) == 0)
        {
            bb_clear(&object->userName);
            bb_clear(&object->password);
            if (ch == 2)
            {
                --value->byteArr->position;
                if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, &object->userName)) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, &object->password)) != 0)
                {
                    //Error code is returned at the end of the function.
                }
            }
            else if (ch != 0)
            {
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
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
int cosem_setRegisterActivation(
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    int ret, pos, pos2;
    gxRegisterActivation* object = (gxRegisterActivation*)e->target;
    unsigned char index = e->index;
    dlmsVARIANT* value = &e->value;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* objectDefinition;
#else
    gxObject* objectDefinition;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if (index == 2)
    {
#ifdef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count = 0xFFFF;
#else
        uint16_t count;
        uint16_t ot;
        unsigned char ln[6];
        obj_clearRegisterActivationAssignment(&object->registerAssignment);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_verifyArray(value->byteArr, &object->registerAssignment, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                (ret = cosem_getUInt16(value->byteArr, &ot)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0)
                {
                    break;
                }
#ifndef DLMS_COSEM_EXACT_DATA_TYPES
                if ((ret = arr_getByIndexRef(&object->registerAssignment, pos, (void**)&objectDefinition)) != 0)
                {
                    break;
                }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
                if ((ret = cosem_findObjectByLN(settings, (DLMS_OBJECT_TYPE)ot, ln, &objectDefinition)) != 0)
                {
                    break;
                }
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
                if ((ret = arr_push(&object->registerAssignment, objectDefinition)) != 0)
                {
                    break;
                }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
#else
                    (ret = cosem_getArrayItem(&object->registerAssignment, pos, (void**)&objectDefinition, sizeof(gxObjectDefinition))) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&objectDefinition->objectType)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, objectDefinition->logicalName, 6, NULL)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
    }
    else if (index == 3)
    {
        obj_clearRegisterActivationMaskList(&object->maskList);
        gxRegisterActivationMask* k;
        uint16_t size;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->maskList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getArrayItem(&object->maskList, pos, (void**)&k, sizeof(gxRegisterActivationMask))) != 0 ||
#ifndef DLMS_COSEM_EXACT_DATA_TYPES
                    (ret = cosem_getOctetString2(value->byteArr, k->name, sizeof(k->name), &size)) != 0)
#else
                    (ret = cosem_getOctetString(value->byteArr, &k->name)) != 0)
#endif //DLMS_COSEM_EXACT_DATA_TYPES
                {
                    break;
                }
#ifndef DLMS_COSEM_EXACT_DATA_TYPES
                k->length = (unsigned char)size;
#endif //DLMS_COSEM_EXACT_DATA_TYPES
                size = sizeof(k->indexes);
                if ((ret = cosem_checkArray(value->byteArr, &size)) == 0)
                {
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
                    unsigned char ch;
                    for (pos2 = 0; pos2 != size; ++pos2)
                    {
                        if ((ret = cosem_getUInt8(value->byteArr, &ch)) != 0 ||
                            (ret = bb_setUInt8(&k->indexes, ch)) != 0)
                        {
                            break;
                        }
                    }
#else
                    k->count = (unsigned char)size;
                    for (pos2 = 0; pos2 != size; ++pos2)
                    {
                        if ((ret = cosem_getUInt8(value->byteArr, &k->indexes[pos2])) != 0)
                        {
                            break;
                        }
                    }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
                }
            }
        }
    }
    else if (index == 4 && (value->vt & DLMS_DATA_TYPE_OCTET_STRING) != 0)
    {
        //If data is coming in action it's plain value.
        if (e->action && value->vt == (DLMS_DATA_TYPE_OCTET_STRING | DLMS_DATA_TYPE_BYREF))
        {
            bb_clear(&object->activeMask);
            ret = bb_set(&object->activeMask, value->pVal, value->size);
        }
        else
        {
            ret = cosem_getOctetString(value->byteArr, &object->activeMask);
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
int cosem_setRegisterMonitor(dlmsSettings* settings, gxRegisterMonitor* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    gxActionSet* actionSet;
    dlmsVARIANT* tmp;
    if (index == 2)
    {
        va_clear(&object->thresholds);
        uint16_t count;
        if ((ret = cosem_verifyVariantArray(value->byteArr, &object->thresholds, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getVariantArrayItem(&object->thresholds, pos, &tmp)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, tmp)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 3)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_checkStructure(value->byteArr, 3)) == 0 &&
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getUInt16(value->byteArr, &type)) == 0 &&
            (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0 &&
#else
            (ret = cosem_getUInt16(value->byteArr, (short*)&object->monitoredValue.objectType)) == 0 &&
            (ret = cosem_getOctetString2(value->byteArr, object->monitoredValue.logicalName, 6, NULL)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getInt8(value->byteArr, &object->monitoredValue.attributeIndex)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = cosem_findObjectByLN(settings, type, ln, &object->monitoredValue.target);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //Error code is returned at the end of the function.
        }
    }
    else if (index == 4)
    {
        obj_clearRegisterMonitorActions(&object->actions);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char lnUp[6];
        unsigned char lnDown[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS

        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->actions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->actions, pos, (void**)&actionSet, sizeof(gxActionSet))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    //Update action up.
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctetString2(value->byteArr, lnUp, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctetString2(value->byteArr, actionSet->actionUp.logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &actionSet->actionUp.scriptSelector)) != 0 ||
                    //Update action down.
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctetString2(value->byteArr, lnDown, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctetString2(value->byteArr, actionSet->actionDown.logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &actionSet->actionDown.scriptSelector)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnUp, (gxObject**)&actionSet->actionUp.script)) != 0 ||
                    (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnDown, (gxObject**)&actionSet->actionDown.script)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
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
int cosem_setSapAssignment(gxSapAssignment* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    gxSapItem* it = NULL;
    if (index == 2)
    {
        obj_clearSapList(&object->sapAssignmentList);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->sapAssignmentList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &it->id)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
                    (ret = cosem_getOctetString2(value->byteArr, it->name.value, sizeof(it->name.value), &it->name.size)) != 0)
#else
                    (ret = cosem_getOctetString(value->byteArr, &it->name)) != 0)
#endif //DLMS_IGNORE_MALLOC
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
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
int cosem_setSchedule(dlmsSettings* settings, gxSchedule* object, unsigned char index, dlmsVARIANT* value)
{
    gxScheduleEntry* se;
    int ret, pos;
    if (index == 2)
    {
        obj_clearScheduleEntries(&object->entries);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->entries, &count)) == 0)
        {
            uint32_t execWeekdays;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->entries, pos, (void**)&se, sizeof(gxScheduleEntry))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 10)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &se->index)) != 0 ||
                    (ret = cosem_getBoolean(value->byteArr, &se->enable)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctetString2(value->byteArr, se->logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &se->scriptSelector)) != 0 ||
                    (ret = cosem_getTimeFromOctetString(value->byteArr, &se->switchTime)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &se->validityWindow)) != 0 ||
                    (ret = cosem_getIntegerFromBitString(value->byteArr, &execWeekdays)) != 0 ||
                    (ret = cosem_getBitString(value->byteArr, &se->execSpecDays)) != 0 ||
                    (ret = cosem_getDateFromOctetString(value->byteArr, &se->beginDate)) != 0 ||
                    (ret = cosem_getDateFromOctetString(value->byteArr, &se->endDate)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&se->scriptTable)) != 0)
                {
                    break;
                }
#else
#endif //DLMS_IGNORE_OBJECT_POINTERS
                se->execWeekdays = execWeekdays;
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
int cosem_setScriptTable(dlmsSettings* settings, gxScriptTable* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos, pos2;
    gxScriptAction* scriptAction;
    gxScript* script;
    if (index == 2)
    {
        obj_clearScriptTable(&object->scripts);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count, count2;
        if ((ret = cosem_verifyArray(value->byteArr, &object->scripts, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->scripts, pos, (void**)&script, sizeof(gxScript))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &script->id)) != 0)
                {
                    break;
                }
                if ((ret = cosem_verifyArray(value->byteArr, &script->actions, &count2)) != 0)
                {
                    break;
                }
                for (pos2 = 0; pos2 != count2; ++pos2)
                {
                    if ((ret = cosem_getArrayItem(&script->actions, pos2, (void**)&scriptAction, sizeof(gxScriptAction))) != 0 ||
                        (ret = cosem_checkStructure(value->byteArr, 5)) != 0 ||
                        (ret = cosem_getEnum(value->byteArr, (unsigned char*)&scriptAction->type)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_getUInt16(value->byteArr, &type)) != 0 ||
                        (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
                        (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&scriptAction->objectType)) != 0 ||
                        (ret = cosem_getOctetString2(value->byteArr, scriptAction->logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_getInt8(value->byteArr, &scriptAction->index)) != 0 ||
                        (ret = cosem_getVariant(value->byteArr, &scriptAction->parameter)) != 0)
                    {
                        break;
                    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    if ((ret = cosem_findObjectByLN(settings, type, ln, &scriptAction->target)) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
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
int cosem_setSpecialDaysTable(gxSpecialDaysTable* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    gxSpecialDay* specialDay;
    if (index == 2)
    {
        arr_clear(&object->entries);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->entries, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->entries, pos, (void**)&specialDay, sizeof(gxSpecialDay))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &specialDay->index)) != 0 ||
                    (ret = cosem_getDateFromOctetString(value->byteArr, &specialDay->date)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &specialDay->dayId)) != 0)
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
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int cosem_setTcpUdpSetup(dlmsSettings* settings, gxTcpUdpSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
        ret = cosem_getUInt16(value->byteArr, &object->port);
    }
    else if (index == 3)
    {
        if (value->vt == DLMS_DATA_TYPE_NONE)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            object->ipSetup = NULL;
#else
            memset(object->ipReference, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            ret = 0;
        }
        else
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
            if ((ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0)
            {
                ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_IP4_SETUP, ln, &object->ipSetup);
            }
#else
            ret = bb_get(value->byteArr, object->ipReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    else if (index == 4)
    {
        ret = cosem_getUInt16(value->byteArr, &object->maximumSegmentSize);
    }
    else if (index == 5)
    {
        ret = cosem_getUInt8(value->byteArr, &object->maximumSimultaneousConnections);
    }
    else if (index == 6)
    {
        ret = cosem_getUInt16(value->byteArr, &object->inactivityTimeout);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int cosem_setMbusMasterPortSetup(gxMBusMasterPortSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    unsigned char ch;
    if (index == 2)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->commSpeed = (DLMS_BAUD_RATE)ch;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

#ifndef DLMS_IGNORE_PUSH_SETUP
int cosem_setPushSetup(dlmsSettings* settings, gxPushSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    gxTarget* it;
    if (index == 2)
    {
        DLMS_OBJECT_TYPE type = DLMS_OBJECT_TYPE_NONE;
        obj_clearPushObjectList(&object->pushObjectList);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->pushObjectList, &count)) == 0) {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->pushObjectList, pos, (void**)&it, sizeof(gxTarget))) == 0)
                {
                    if ((ret = cosem_checkStructure(value->byteArr, 4)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &it->objectType)) != 0 ||
                        (ret = cosem_getOctetString2(value->byteArr, it->logicalName, 6, NULL)) != 0 ||
#else
                        (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&type)) != 0 ||
                        (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_getInt8(value->byteArr, &it->attributeIndex)) != 0 ||
                        (ret = cosem_getUInt16(value->byteArr, &it->dataIndex)) != 0)
                    {
                        break;
                    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    if ((ret = cosem_findObjectByLN(settings, type, ln, &it->target)) != 0)
                    {
                        break;
                    }
                    if (it->target == NULL)
                    {
#ifdef DLMS_DEBUG
                        svr_notifyTrace2("Invalid object", type, ln, -1);
#endif //DLMS_DEBUG
                        object->pushObjectList.size = pos;
                        return DLMS_ERROR_CODE_INVALID_PARAMETER;
                    }
#ifdef DLMS_DEBUG
                    svr_notifyTrace2("Adding object ", type, ln, 0);
#endif //DLMS_DEBUG
#endif //DLMS_IGNORE_OBJECT_POINTERS
                }
            }
        }
    }
    else if (index == 3)
    {
        if ((ret = bb_clear(&object->destination)) != 0 ||
            (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->service)) != 0 ||
            (ret = cosem_getOctetString(value->byteArr, &object->destination)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->message)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (index == 4)
    {
        gxTimePair* it;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->communicationWindow, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->communicationWindow, pos, (void**)&it, sizeof(gxTimePair))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getDateTimeFromOctetString(value->byteArr, &it->first)) != 0 ||
                    (ret = cosem_getDateTimeFromOctetString(value->byteArr, &it->second)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        ret = cosem_getUInt16(value->byteArr, &object->randomisationStartInterval);
    }
    else if (index == 6)
    {
        ret = cosem_getUInt8(value->byteArr, &object->numberOfRetries);
    }
    else if (index == 7)
    {
        ret = cosem_getUInt16(value->byteArr, &object->repetitionDelay);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_CHARGE
int setUnitCharge(dlmsSettings* settings, gxUnitCharge* target, dlmsVARIANT* value)
{
    gxChargeTable* ct;
    int ret, pos;
    ret = obj_clearChargeTables(&target->chargeTables);
    if (ret != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    uint16_t type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
    uint16_t count;
    if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
        //charge per unit scaling
        (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
        (ret = cosem_getInt8(value->byteArr, &target->chargePerUnitScaling.commodityScale)) != 0 ||
        (ret = cosem_getInt8(value->byteArr, &target->chargePerUnitScaling.priceScale)) != 0 ||
        //commodity
        (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getUInt16(value->byteArr, &type)) != 0 ||
        (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
        (ret = cosem_getInt16(value->byteArr, (short*)&target->commodity.type)) != 0 ||
        (ret = cosem_getOctetString2(value->byteArr, target->commodity.logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getInt8(value->byteArr, &target->commodity.attributeIndex)) != 0 ||
        (ret = cosem_verifyArray(value->byteArr, &target->chargeTables, &count)) != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = cosem_findObjectByLN(settings, type, ln, &target->commodity.target)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    for (pos = 0; pos != count; ++pos)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getArrayItem(&target->chargeTables, pos, (void**)&ct, sizeof(gxChargeTable))) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
            (ret = cosem_getOctetString2(value->byteArr, ct->index.data, sizeof(ct->index.data), &ct->index.size)) != 0 ||
#else
            (ret = cosem_getOctetString(value->byteArr, &ct->index)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
            (ret = cosem_getInt16(value->byteArr, &ct->chargePerUnit)) != 0)
        {
            break;
        }
    }
    return ret;
}

int cosem_setCharge(dlmsSettings* settings, gxCharge* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    unsigned char ch;
    if (index == 2)
    {
        ret = cosem_getInt32(value->byteArr, &object->totalAmountPaid);
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->chargeType = (DLMS_CHARGE_TYPE)ch;
        }
    }
    else if (index == 4)
    {
        ret = cosem_getUInt8(value->byteArr, &object->priority);
    }
    else if (index == 5)
    {
        ret = setUnitCharge(settings, &object->unitChargeActive, value);
    }
    else if (index == 6)
    {
        ret = setUnitCharge(settings, &object->unitChargePassive, value);
    }
    else if (index == 7)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->unitChargeActivationTime);
    }
    else if (index == 8)
    {
        ret = cosem_getUInt32(value->byteArr, &object->period);
    }
    else if (index == 9)
    {
        uint32_t val;
        if ((ret = cosem_getIntegerFromBitString(value->byteArr, &val)) == 0)
        {
            object->chargeConfiguration = val;
        }
    }
    else if (index == 10)
    {
        ret = cosem_getDateTime(value->byteArr, &object->lastCollectionTime);
    }
    else if (index == 11)
    {
        ret = cosem_getInt32(value->byteArr, &object->lastCollectionAmount);
    }
    else if (index == 12)
    {
        ret = cosem_getInt32(value->byteArr, &object->totalAmountRemaining);
    }
    else if (index == 13)
    {
        ret = cosem_getUInt16(value->byteArr, &object->proportion);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_CREDIT
int cosem_setCredit(gxCredit* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret;
    if (index == 2)
    {
        ret = cosem_getInt32(value->byteArr, &object->currentCreditAmount);
    }
    else if (index == 3)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->type = (DLMS_CREDIT_TYPE)ch;
        }
    }
    else if (index == 4)
    {
        ret = cosem_getUInt8(value->byteArr, &object->priority);
    }
    else if (index == 5)
    {
        ret = cosem_getInt32(value->byteArr, &object->warningThreshold);
    }
    else if (index == 6)
    {
        ret = cosem_getInt32(value->byteArr, &object->limit);
    }
    else if (index == 7)
    {
        uint32_t val;
        if ((ret = cosem_getIntegerFromBitString(value->byteArr, &val)) == 0)
        {
            object->creditConfiguration = (DLMS_CREDIT_CONFIGURATION)val;
        }
    }
    else if (index == 8)
    {
        ret = cosem_getEnum(value->byteArr, &object->status);
    }
    else if (index == 9)
    {
        ret = cosem_getInt32(value->byteArr, &object->presetCreditAmount);
    }
    else if (index == 10)
    {
        ret = cosem_getInt32(value->byteArr, &object->creditAvailableThreshold);
    }
    else if (index == 11)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->period);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_ACCOUNT
int cosem_setAccount(gxAccount* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    unsigned char m, s;
    unsigned char* ba;
    gxCreditChargeConfiguration* ccc;
    gxTokenGatewayConfiguration* gwc;
    if (index == 2)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
            (ret = cosem_getEnum(value->byteArr, &m)) == 0 &&
            (ret = cosem_getEnum(value->byteArr, &s)) == 0)
        {
            object->paymentMode = (DLMS_ACCOUNT_PAYMENT_MODE)m;
            object->accountStatus = (DLMS_ACCOUNT_STATUS)s;
            //Error is returned at the end of the function.
        }
    }
    else if (index == 3)
    {
        ret = cosem_getUInt8(value->byteArr, &object->currentCreditInUse);
    }
    else if (index == 4)
    {
        uint32_t v;
        if ((ret = cosem_getIntegerFromBitString(value->byteArr, &v)) == 0)
        {
            object->currentCreditStatus = (DLMS_ACCOUNT_CREDIT_STATUS)v;
        }
    }
    else if (index == 5)
    {
        ret = cosem_getInt32(value->byteArr, &object->availableCredit);
    }
    else if (index == 6)
    {
        ret = cosem_getInt32(value->byteArr, &object->amountToClear);
    }
    else if (index == 7)
    {
        ret = cosem_getInt32(value->byteArr, &object->clearanceThreshold);
    }
    else if (index == 8)
    {
        ret = cosem_getInt32(value->byteArr, &object->aggregatedDebt);
    }
    else if (index == 9)
    {
        arr_clear(&object->creditReferences);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->creditReferences, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->creditReferences, pos, (void**)&ba, 6)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, ba, 6, NULL)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 10)
    {
        arr_clear(&object->chargeReferences);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->chargeReferences, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->chargeReferences, pos, (void**)&ba, 6)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, ba, 6, NULL)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 11)
    {
        obj_clearCreditChargeConfigurations(&object->creditChargeConfigurations);
        uint32_t v;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->creditChargeConfigurations, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->creditChargeConfigurations, pos, (void**)&ccc, sizeof(gxCreditChargeConfiguration))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, ccc->creditReference, 6, NULL)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, ccc->chargeReference, 6, NULL)) != 0 ||
                    (ret = cosem_getIntegerFromBitString(value->byteArr, &v)) != 0)
                {
                    break;
                }
                ccc->collectionConfiguration = (DLMS_CREDIT_COLLECTION_CONFIGURATION)v;
            }
        }
    }
    else if (index == 12)
    {
        obj_clearTokenGatewayConfigurations(&object->tokenGatewayConfigurations);
        gwc = NULL;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->tokenGatewayConfigurations, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->tokenGatewayConfigurations, pos, (void**)&gwc, sizeof(gxTokenGatewayConfiguration))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, gwc->creditReference, 6, NULL)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &gwc->tokenProportion)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 13)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->accountActivationTime);
    }
    else if (index == 14)
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->accountClosureTime);
    }
    else if (index == 15)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
        (ret = cosem_getUtf8String2(value->byteArr, object->currency.name.value, sizeof(object->currency.name.value), &object->currency.name.size)) != 0 ||
#else
            (ret = cosem_getUtf8String(value->byteArr, &object->currency.name)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
            (ret = cosem_getInt8(value->byteArr, &object->currency.scale)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->currency.unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (index == 16)
    {
        ret = cosem_getInt32(value->byteArr, &object->lowCreditThreshold);
    }
    else if (index == 17)
    {
        ret = cosem_getInt32(value->byteArr, &object->nextCreditAvailableThreshold);
    }
    else if (index == 18)
    {
        ret = cosem_getUInt16(value->byteArr, &object->maxProvision);
    }
    else if (index == 19)
    {
        ret = cosem_getInt32(value->byteArr, &object->maxProvisionPeriod);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int cosem_setImageTransfer(gxImageTransfer* object, unsigned char index, dlmsVARIANT* value)
{
    int pos, ret;
    unsigned char ch;
    gxImageActivateInfo* item;
    if (index == 2)
    {
        ret = cosem_getUInt32(value->byteArr, &object->imageBlockSize);
    }
    else if (index == 3)
    {
        ret = cosem_getBitString(value->byteArr, &object->imageTransferredBlocksStatus);
    }
    else if (index == 4)
    {
        ret = cosem_getUInt32(value->byteArr, &object->imageFirstNotTransferredBlockNumber);
    }
    else if (index == 5)
    {
        ret = cosem_getBoolean(value->byteArr, &object->imageTransferEnabled);
    }
    else if (index == 6)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->imageTransferStatus = (DLMS_IMAGE_TRANSFER_STATUS)ch;
        }
    }
    else if (index == 7)
    {
        arr_clear(&object->imageActivateInfo);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->imageActivateInfo, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->imageActivateInfo, pos, (void**)&item, sizeof(gxImageActivateInfo))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &item->size)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
                    (ret = cosem_getOctetString2(value->byteArr, item->identification.data, sizeof(item->identification.data), &item->identification.size)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, item->signature.data, sizeof(item->signature.data), &item->signature.size)) != 0)
#else
                    (ret = cosem_getOctetString(value->byteArr, &item->identification)) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, &item->signature)) != 0)
#endif //DLMS_IGNORE_MALLOC
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
int setCaptureObjects(
    dlmsSettings* settings,
    gxArray* objects,
    dlmsVARIANT* value)
{
    gxTarget* co;
    int pos, ret;
    if ((ret = obj_clearProfileGenericCaptureObjects(objects)) != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    uint16_t type;
    uint16_t count;
    if ((ret = cosem_verifyArray(value->byteArr, objects, &count)) == 0)
    {
        unsigned char ln[6];
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(objects, pos, (void**)&co, sizeof(gxTarget))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 4)) != 0 ||
                (ret = cosem_getUInt16(value->byteArr, &type)) != 0 ||
                (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &co->attributeIndex)) != 0 ||
                (ret = cosem_getUInt16(value->byteArr, &co->dataIndex)) != 0)
            {
                obj_clearProfileGenericCaptureObjects(objects);
                break;
            }
            ret = cosem_findObjectByLN(settings, type, ln, &co->target);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                obj_clearProfileGenericCaptureObjects(objects);
                break;
            }
        }
    }
    return ret;
}
#endif //!(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_CONPACT_DATA))

#ifndef DLMS_IGNORE_PROFILE_GENERIC
int cosem_setProfileGeneric(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    unsigned char ch;
    DLMS_OBJECT_TYPE type;
    if (index == 2)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    else if (index == 3)
    {
        object->entriesInUse = 0;
        ret = setCaptureObjects(settings, &object->captureObjects, value);
    }
    else if (index == 4)
    {
        ret = cosem_getUInt32(value->byteArr, &object->capturePeriod);
    }
    else if (index == 5)
    {
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->sortMethod = (DLMS_SORT_METHOD)ch;
        }
    }
    else if (index == 6)
    {
        if ((ret = cosem_checkStructure(value->byteArr, 4)) != 0)
        {
            unsigned char ln[6];
            type = 0;
            object->sortObject = NULL;
            if ((ret = cosem_getInt16(value->byteArr, (signed short*)&type)) == 0 &&
                (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0 &&
                (ret = cosem_findObjectByLN(settings, type, ln, &object->sortObject)) == 0 &&
                (ret = cosem_getInt8(value->byteArr, &object->sortObjectAttributeIndex)) == 0 &&
                (ret = cosem_getUInt16(value->byteArr, &object->sortObjectDataIndex)) == 0)
            {
            }
        }
    }
    else if (index == 7)
    {
        ret = cosem_getUInt32(value->byteArr, &object->entriesInUse);
    }
    else if (index == 8)
    {
        ret = cosem_getUInt32(value->byteArr, &object->profileEntries);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int cosem_setGsmDiagnostic(gxGsmDiagnostic* object, unsigned char index, dlmsVARIANT* value)
{
    unsigned char ch;
    int ret, pos;
    gxAdjacentCell* ac;
    switch (index)
    {
    case 2:
        ret = cosem_getString(value->byteArr, &object->operatorName);
        break;
    case 3:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->status = (DLMS_GSM_STATUS)ch;
        }
        break;
    case 4:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->circuitSwitchStatus = (DLMS_GSM_CIRCUIT_SWITCH_STATUS)ch;
        }
        break;
    case 5:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->packetSwitchStatus = (DLMS_GSM_PACKET_SWITCH_STATUS)ch;
        }
        break;
    case 6:
    {
        unsigned char count;
        if (object->base.version != 0)
        {
            count = 7;
        }
        else
        {
            count = 4;
        }
        if ((ret = cosem_checkStructure(value->byteArr, count)) == 0)
        {
            if ((ret = cosem_getUInt32(value->byteArr, &object->cellInfo.cellId)) != 0 ||
                (ret = cosem_getUInt16(value->byteArr, &object->cellInfo.locationId)) != 0 ||
                (ret = cosem_getUInt8(value->byteArr, &object->cellInfo.signalQuality)) != 0 ||
                (ret = cosem_getUInt8(value->byteArr, &object->cellInfo.ber)) != 0)
            {
                break;
            }
            if (object->base.version != 0)
            {
                if ((ret = cosem_getUInt16(value->byteArr, &object->cellInfo.mobileCountryCode)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &object->cellInfo.mobileNetworkCode)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &object->cellInfo.channelNumber)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 7:
        arr_clear(&object->adjacentCells);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->adjacentCells, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->adjacentCells, pos, (void**)&ac, sizeof(gxAdjacentCell))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &ac->cellId)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ac->signalQuality)) != 0)
                {
                    break;
                }
            }
        }
        break;
    case 8:
        ret = cosem_getDateTime(value->byteArr, &object->captureTime);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int cosem_setTokenGateway(gxTokenGateway* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    unsigned char ch;
    uint16_t count;
#ifdef DLMS_IGNORE_MALLOC
    gxTokenGatewayDescription* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    switch (index)
    {
    case 2:
        ret = cosem_getOctetString(value->byteArr, &object->token);
        break;
    case 3:
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->time);
        break;
    case 4:
        obj_clearByteBufferList(&object->descriptions);
        if ((ret = cosem_verifyArray(value->byteArr, &object->descriptions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = cosem_getArrayItem(&object->descriptions, pos, (void**)&it, sizeof(gxTokenGatewayDescription))) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, it->value, sizeof(it->value), &it->size)) != 0)
                {
                    break;
                }
#else
                if ((ret = cosem_getArrayItem(&object->descriptions, pos, (void**)&it, sizeof(gxByteBuffer))) != 0 ||
                    (ret = cosem_getOctetString(value->byteArr, it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
        break;
    case 5:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->deliveryMethod = (DLMS_TOKEN_DELIVERY)ch;
        }
        break;
    case 6:
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->status)) != 0 ||
            (ret = cosem_getBitString(value->byteArr, &object->dataValue)) != 0)
        {
            //Result code is returned at the end of the method.
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY

#ifndef DLMS_IGNORE_COMPACT_DATA

int compactData_updateTemplateDescription(
    dlmsSettings* settings,
    gxCompactData* object)
{
    int ret, pos;
    gxByteBuffer tmp;
    gxValueEventCollection args;
    gxValueEventArg e;
    ve_init(&e);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* kv;
#else
    gxKey* kv;
#endif //DLMS_IGNORE_MALLOC
    unsigned char tmp2[100];
    gxByteBuffer bb;
    bb_attach(&bb, tmp2, 0, sizeof(tmp2));
    bb_clear(&object->buffer);
    bb_clear(&object->templateDescription);
    e.action = 1;
    e.target = &object->base;
    e.index = 2;
    vec_init(&args);
    BYTE_BUFFER_INIT(&tmp);
    unsigned char tmp3[100];
    bb_attach(&tmp, tmp3, 0, sizeof(tmp3));
    if (!e.handled)
    {
        if ((ret = bb_setUInt8(&object->templateDescription, DLMS_DATA_TYPE_STRUCTURE)) != DLMS_ERROR_CODE_OK)
        {
            bb_clear(&object->buffer);
            return ret;
        }
        hlp_setObjectCount(object->captureObjects.size, &object->templateDescription);
        for (pos = 0; pos != object->captureObjects.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex2(&object->captureObjects, pos, (void**)&kv, sizeof(gxTarget))) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->captureObjects, pos, (void**)&kv)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            e.value.byteArr = &bb;
            e.value.vt = DLMS_DATA_TYPE_OCTET_STRING;
            e.target = kv->target;
            e.index = kv->attributeIndex;
            if ((ret = cosem_getValue(settings, &e)) != 0)
            {
                break;
            }
            if (e.byteArray)
            {
                if (bb_size(e.value.byteArr) == 0)
                {
                    bb_setUInt8(&object->templateDescription, 0);
                }
                else
                {
                    unsigned char ch;
                    if ((ret = bb_getUInt8(e.value.byteArr, &ch)) != 0)
                    {
                        break;
                    }
                    if (ch == DLMS_DATA_TYPE_ARRAY ||
                        ch == DLMS_DATA_TYPE_STRUCTURE)
                    {
                        gxDataInfo info;
                        dlmsVARIANT value;
                        uint16_t count;
                        di_init(&info);
                        var_init(&value);
                        if ((ret = hlp_getObjectCount2(e.value.byteArr, &count)) != 0 ||
                            kv->dataIndex > count)
                        {
                            break;
                        }
                        //If all data is captured.
                        if (kv->dataIndex == 0)
                        {
                            uint16_t count2;
                            if ((ret = bb_setUInt8(&object->templateDescription, ch)) != 0)
                            {
                                break;
                            }
                            if (ch == DLMS_DATA_TYPE_ARRAY)
                            {
                                if ((ret = bb_setUInt16(&object->templateDescription, count)) != 0)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                if ((ret = bb_setUInt8(&object->templateDescription, (unsigned char)count)) != 0)
                                {
                                    break;
                                }
                            }
                            for (unsigned char pos = 0; pos < count; ++pos)
                            {
                                di_init(&info);
                                var_clear(&value);
                                if ((ret = dlms_getData(e.value.byteArr, &info, &value)) != 0)
                                {
                                    break;
                                }
                                if (info.type == DLMS_DATA_TYPE_STRUCTURE || info.type == DLMS_DATA_TYPE_ARRAY)
                                {
                                    bb_setUInt8(&object->templateDescription, info.type);
                                    ++value.byteArr->position;
                                    if ((ret = hlp_getObjectCount2(value.byteArr, &count2)) != 0 ||
                                        (ret = hlp_setObjectCount(count2, &object->templateDescription)) != 0)
                                    {
                                        break;
                                    }
                                    for (uint16_t pos = 0; pos < count2; ++pos)
                                    {
                                        var_clear(&value);
                                        if ((ret = dlms_getData(e.value.byteArr, &info, &value)) != 0 ||
                                            (ret = bb_setUInt8(&object->templateDescription, value.vt)) != 0)
                                        {
                                            var_clear(&value);
                                            var_clear(&e.value);
                                            bb_clear(&object->buffer);
                                            return ret;
                                        }
                                    }
                                }
                                else
                                {
                                    bb_setUInt8(&object->templateDescription, info.type);
                                }
                                if (e.value.byteArr->data[0] == DLMS_DATA_TYPE_ARRAY)
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            for (unsigned char pos = 0; pos < kv->dataIndex; ++pos)
                            {
                                var_clear(&value);
                                di_init(&info);
                                if ((ret = dlms_getData(e.value.byteArr, &info, &value)) != 0)
                                {
                                    var_clear(&value);
                                    var_clear(&e.value);
                                    bb_clear(&object->buffer);
                                    return ret;
                                }
                                if (!info.complete)
                                {
                                    return DLMS_ERROR_CODE_READ_WRITE_DENIED;
                                }
                            }
                            if (info.type == DLMS_DATA_TYPE_STRUCTURE)
                            {
                                dlmsVARIANT* value2;
                                bb_setUInt8(&object->templateDescription, DLMS_DATA_TYPE_STRUCTURE);
                                bb_setUInt8(&object->templateDescription, (unsigned char)value.Arr->size);
                                for (uint16_t pos = 0; pos < value.Arr->size; ++pos)
                                {
                                    if ((ret = va_getByIndex(value.Arr, pos, &value2)) != 0)
                                    {
                                        var_clear(&value);
                                        var_clear(&e.value);
                                        bb_clear(&object->buffer);
                                        return ret;
                                    }
                                    bb_setUInt8(&object->templateDescription, value2->vt);
                                }
                            }
                            else
                            {
                                bb_setUInt8(&object->templateDescription, info.type);
                            }
                        }
                        var_clear(&value);
                    }
                    else
                    {
                        bb_setUInt8(&object->templateDescription, ch);
                    }
                }
            }
            else
            {
                if ((ret = dlms_setData(&tmp, e.value.vt, &e.value)) != 0)
                {
                    var_clear(&e.value);
                    bb_clear(&tmp);
                    bb_clear(&object->buffer);
                    return ret;
                }
                bb_setUInt8(&object->templateDescription, tmp.data[0]);
                bb_clear(&tmp);
            }
            var_clear(&e.value);
            ve_clear(&e);
        }
    }
    bb_clear(&tmp);
    //svr_postGet(settings, &args);
    vec_empty(&args);
    return 0;
}

int cosem_setCompactData(
    dlmsSettings* settings,
    gxCompactData* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    unsigned char ch;
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getOctetString(value->byteArr, &object->buffer);
        break;
    case 3:
        ret = setCaptureObjects(settings, &object->captureObjects, value);
        if (ret == 0 && settings->server)
        {
            ret = compactData_updateTemplateDescription(settings, object);
        }
        break;
    case 4:
        ret = cosem_getUInt8(value->byteArr, &object->templateId);
        break;
    case 5:
        ret = cosem_getOctetString(value->byteArr, &object->templateDescription);
        break;
    case 6:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->captureMethod = (DLMS_CAPTURE_METHOD)ch;
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_PARAMETER_MONITOR
int cosem_setParameterMonitor(
    dlmsSettings* settings,
    gxParameterMonitor* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret, pos;
    switch (index)
    {
    case 2:
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        object->changedParameter.target = NULL;
#else
        object->changedParameter.type = DLMS_OBJECT_TYPE_NONE;
        memset(object->changedParameter.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        uint16_t type;
        if ((ret = cosem_checkStructure(value->byteArr, 4)) == 0 &&
            (ret = cosem_getUInt16(value->byteArr, &type)) == 0 &&
            (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) == 0 &&
            (ret = cosem_getInt8(value->byteArr, &object->changedParameter.attributeIndex)) == 0 &&
            (ret = cosem_getVariant(value->byteArr, &object->changedParameter.value)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = cosem_findObjectByLN(settings, type, ln, &object->changedParameter.target);
#else
            object->changedParameter.type = type;
            memcpy(object->changedParameter.logicalName, ln, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
        break;
    }
    case 3:
    {
        ret = cosem_getDateTimeFromOctetString(value->byteArr, &object->captureTime);
    }
    break;
    case 4:
    {
        uint16_t type;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        gxTarget* it;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->parameters, &count)) == 0) {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->parameters, pos, (void**)&it, sizeof(gxTarget))) != 0)
                {
                    break;
                }
                if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&type)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctetString2(value->byteArr, it->logicalName, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getInt8(value->byteArr, &it->attributeIndex)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = cosem_findObjectByLN(settings, type, ln, &it->target)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
        break;
    }
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int cosem_setLlcSscsSetup(
    dlmsSettings* settings,
    gxLlcSscsSetup* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt16(value->byteArr, &object->serviceNodeAddress);
        break;
    case 3:
        ret = cosem_getUInt16(value->byteArr, &object->baseNodeAddress);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int cosem_setPrimeNbOfdmPlcPhysicalLayerCounters(
    dlmsSettings* settings,
    gxPrimeNbOfdmPlcPhysicalLayerCounters* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt16(value->byteArr, &object->crcIncorrectCount);
        break;
    case 3:
        ret = cosem_getUInt16(value->byteArr, &object->crcFailedCount);
        break;
    case 4:
        ret = cosem_getUInt16(value->byteArr, &object->txDropCount);
        break;
    case 5:
        ret = cosem_getUInt16(value->byteArr, &object->rxDropCount);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}

#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int cosem_setPrimeNbOfdmPlcMacSetup(
    dlmsSettings* settings,
    gxPrimeNbOfdmPlcMacSetup* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt8(value->byteArr, &object->macMinSwitchSearchTime);
        break;
    case 3:
        ret = cosem_getUInt8(value->byteArr, &object->macMaxPromotionPdu);
        break;
    case 4:
        ret = cosem_getUInt8(value->byteArr, &object->macPromotionPduTxPeriod);
        break;
    case 5:
        ret = cosem_getUInt8(value->byteArr, &object->macBeaconsPerFrame);
        break;
    case 6:
        ret = cosem_getUInt8(value->byteArr, &object->macScpMaxTxAttempts);
        break;
    case 7:
        ret = cosem_getUInt8(value->byteArr, &object->macCtlReTxTimer);
        break;
    case 8:
        ret = cosem_getUInt8(value->byteArr, &object->macMaxCtlReTx);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}

#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int cosem_setPrimeNbOfdmPlcMacFunctionalParameters(
    dlmsSettings* settings,
    gxPrimeNbOfdmPlcMacFunctionalParameters* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    unsigned char ch;
    uint16_t v;
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getInt16(value->byteArr, &object->lnId);
        break;
    case 3:
        ret = cosem_getUInt8(value->byteArr, &object->lsId);
        break;
    case 4:
        ret = cosem_getUInt8(value->byteArr, &object->sId);
        break;
    case 5:
        ret = cosem_getOctetString(value->byteArr, &object->sna);
        break;
    case 6:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->state = (DLMS_MAC_STATE)ch;
        }
        break;
    case 7:
        ret = cosem_getInt16(value->byteArr, &object->scpLength);
        break;
    case 8:
        ret = cosem_getUInt8(value->byteArr, &object->nodeHierarchyLevel);
        break;
    case 9:
        ret = cosem_getUInt8(value->byteArr, &object->beaconSlotCount);
        break;
    case 10:
        ret = cosem_getUInt8(value->byteArr, &object->beaconRxSlot);
        break;
    case 11:
        ret = cosem_getUInt8(value->byteArr, &object->beaconTxSlot);
        break;
    case 12:
        ret = cosem_getUInt8(value->byteArr, &object->beaconRxFrequency);
        break;
    case 13:
        ret = cosem_getUInt8(value->byteArr, &object->beaconTxFrequency);
        break;
    case 14:
        if ((ret = cosem_getUInt16(value->byteArr, &v)) == 0)
        {
            object->capabilities = (DLMS_MAC_CAPABILITIES)v;
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int cosem_setPrimeNbOfdmPlcMacCounters(
    dlmsSettings* settings,
    gxPrimeNbOfdmPlcMacCounters* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt32(value->byteArr, &object->txDataPktCount);
        break;
    case 3:
        ret = cosem_getUInt32(value->byteArr, &object->rxDataPktCount);
        break;
    case 4:
        ret = cosem_getUInt32(value->byteArr, &object->txCtrlPktCount);
        break;
    case 5:
        ret = cosem_getUInt32(value->byteArr, &object->rxCtrlPktCount);
        break;
    case 6:
        ret = cosem_getUInt32(value->byteArr, &object->csmaFailCount);
        break;
    case 7:
        ret = cosem_getUInt32(value->byteArr, &object->csmaChBusyCount);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA

int cosem_setMulticastEntries(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret;
    int pos;
    gxMacMulticastEntry* it;
    arr_clear(&object->multicastEntries);
    uint16_t count;
    if ((ret = cosem_verifyArray(value->byteArr, &object->multicastEntries, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(&object->multicastEntries, pos, (void**)&it, sizeof(gxMacMulticastEntry))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->id)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->members)) != 0)
            {
                break;
            }
        }
    }
    return 0;
}


int cosem_setSwitchTable(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret;
    int pos;
    arr_empty(&object->switchTable);
    short* it;
    uint16_t count;
    if ((ret = cosem_verifyArray(value->byteArr, &object->switchTable, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(&object->switchTable, pos, (void**)&it, sizeof(short))) != 0 ||
                (ret = cosem_getInt16(value->byteArr, it)) != 0)
            {
                break;
            }
        }
    }
    return 0;
}

int cosem_setDirectTable(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret;
    int pos;
    gxMacDirectTable* it;
    arr_clear(&object->directTable);
    uint16_t count;
    if ((ret = cosem_verifyArray(value->byteArr, &object->directTable, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(&object->directTable, pos, (void**)&it, sizeof(gxMacDirectTable))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 7)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->sourceSId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->sourceLnId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->sourceLcId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->destinationSId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->destinationLnId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->destinationLcId)) != 0 ||
                (ret = cosem_getOctetString2(value->byteArr, it->did, sizeof(it->did), NULL)) != 0)
            {
                break;
            }
        }
    }
    return 0;
}

int cosem_setAvailableSwitches(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret;
    int pos;
    gxMacAvailableSwitch* it;
    obj_clearAvailableSwitches(&object->availableSwitches);
    uint16_t count;
    if ((ret = cosem_verifyArray(value->byteArr, &object->availableSwitches, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(&object->availableSwitches, pos, (void**)&it, sizeof(gxMacAvailableSwitch))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 5)) != 0 ||
                (ret = cosem_getOctetString(value->byteArr, &it->sna)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->lsId)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->level)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->rxLevel)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->rxSnr)) != 0)
            {
                break;
            }
        }
    }
    return 0;
}

int cosem_setCommunications(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret;
    int pos;
    gxMacPhyCommunication* it;
    arr_clear(&object->communications);
    uint16_t count;
    if ((ret = cosem_verifyArray(value->byteArr, &object->communications, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = cosem_getArrayItem(&object->communications, pos, (void**)&it, sizeof(gxMacPhyCommunication))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 9)) != 0 ||
                (ret = cosem_getOctetString2(value->byteArr, it->eui, sizeof(it->eui), NULL)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->txPower)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->txCoding)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->rxCoding)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->rxLvl)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->snr)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->txPowerModified)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->txCodingModified)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->rxCodingModified)) != 0)
            {
                break;
            }
        }
    }
    return 0;
}

int cosem_setPrimeNbOfdmPlcMacNetworkAdministrationData(
    dlmsSettings* settings,
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_setMulticastEntries(object, value);
        break;
    case 3:
        ret = cosem_setSwitchTable(object, value);
        break;
    case 4:
        ret = cosem_setDirectTable(object, value);
        break;
    case 5:
        ret = cosem_setAvailableSwitches(object, value);
        break;
    case 6:
        ret = cosem_setCommunications(object, value);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}

#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int cosem_setPrimeNbOfdmPlcApplicationsIdentification(
    dlmsSettings* settings,
    gxPrimeNbOfdmPlcApplicationsIdentification* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getOctetString(value->byteArr, &object->firmwareVersion);
        break;
    case 3:
        ret = cosem_getUInt16(value->byteArr, &object->vendorId);
        break;
    case 4:
        ret = cosem_getUInt16(value->byteArr, &object->productId);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION

#ifndef DLMS_IGNORE_ARBITRATOR
int cosem_setArbitrator(
    dlmsSettings* settings,
    gxArbitrator* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int pos, ret;
    gxActionItem* it;
    bitArray* ba;
    switch (index)
    {
    case 2:
    {
        arr_clear(&object->actions);
        unsigned char ln[6];
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->actions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->actions, pos, (void**)&it, sizeof(gxActionItem))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, ln, 6, NULL)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &it->scriptSelector)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&it->script)) != 0)
                {
                    break;
                }
#else
                memcpy(it->scriptLogicalName, ln, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
    }
    break;
    case 3:
    {
        obj_clearBitArrayList(&object->permissionsTable);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->permissionsTable, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->permissionsTable, pos, (void**)&ba, sizeof(bitArray))) != 0 ||
                    (ret = cosem_getBitString(value->byteArr, ba)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 4:
    {
        ret = 0;
        arr_clear(&object->weightingsTable);
    }
    break;
    case 5:
    {
        obj_clearBitArrayList(&object->mostRecentRequestsTable);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->mostRecentRequestsTable, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->mostRecentRequestsTable, pos, (void**)&it, sizeof(bitArray))) != 0 ||
                    (ret = cosem_getBitString(value->byteArr, ba)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 6:
        ret = cosem_getUInt8(value->byteArr, &object->lastOutcome);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int cosem_setIec8802LlcType1Setup(
    dlmsSettings* settings,
    gxIec8802LlcType1Setup* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt16(value->byteArr, &object->maximumOctetsUiPdu);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int cosem_setIec8802LlcType2Setup(
    dlmsSettings* settings,
    gxIec8802LlcType2Setup* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt8(value->byteArr, &object->transmitWindowSizeK);
        break;
    case 3:
        ret = cosem_getUInt8(value->byteArr, &object->transmitWindowSizeRW);
        break;
    case 4:
        ret = cosem_getUInt16(value->byteArr, &object->maximumOctetsPdu);
        break;
    case 5:
        ret = cosem_getUInt8(value->byteArr, &object->maximumNumberTransmissions);
        break;
    case 6:
        ret = cosem_getUInt16(value->byteArr, &object->acknowledgementTimer);
        break;
    case 7:
        ret = cosem_getUInt16(value->byteArr, &object->bitTimer);
        break;
    case 8:
        ret = cosem_getUInt16(value->byteArr, &object->rejectTimer);
        break;
    case 9:
        ret = cosem_getUInt16(value->byteArr, &object->busyStateTimer);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int cosem_setIec8802LlcType3Setup(
    dlmsSettings* settings,
    gxIec8802LlcType3Setup* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt16(value->byteArr, &object->maximumOctetsACnPdu);
        break;
    case 3:
        ret = cosem_getUInt8(value->byteArr, &object->maximumTransmissions);
        break;
    case 4:
        ret = cosem_getUInt16(value->byteArr, &object->acknowledgementTime);
        break;
    case 5:
        ret = cosem_getUInt16(value->byteArr, &object->receiveLifetime);
        break;
    case 6:
        ret = cosem_getUInt16(value->byteArr, &object->transmitLifetime);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int cosem_setSFSKActiveInitiator(
    dlmsSettings* settings,
    gxSFSKActiveInitiator* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
            (ret = cosem_getOctetString(value->byteArr, &object->systemTitle)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->macAddress)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->lSapSelector)) != 0)
        {
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int cosem_setFSKMacCounters(
    dlmsSettings* settings,
    gxFSKMacCounters* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int pos, ret;
    gxUint16PairUint32* it;
    switch (index)
    {
    case 2:
    {
        arr_clear(&object->synchronizationRegister);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->synchronizationRegister, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->synchronizationRegister, pos, (void**)&it, sizeof(gxUint16PairUint32))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &it->first)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &it->second)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 3:
    {
        if ((ret = cosem_checkStructure(value->byteArr, 5)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->physicalLayerDesynchronization)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->timeOutNotAddressedDesynchronization)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->timeOutFrameNotOkDesynchronization)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->writeRequestDesynchronization)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->wrongInitiatorDesynchronization)) != 0)
        {
            object->physicalLayerDesynchronization = 0;
            object->timeOutNotAddressedDesynchronization = 0;
            object->timeOutFrameNotOkDesynchronization = 0;
            object->writeRequestDesynchronization = 0;
            object->wrongInitiatorDesynchronization = 0;
            break;
        }
    }
    break;
    case 4:
    {
        arr_clear(&object->broadcastFramesCounter);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->broadcastFramesCounter, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->broadcastFramesCounter, pos, (void**)&it, sizeof(gxUint16PairUint32))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &it->first)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &it->second)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 5:
        ret = cosem_getUInt32(value->byteArr, &object->repetitionsCounter);
        break;
    case 6:
        ret = cosem_getUInt32(value->byteArr, &object->transmissionsCounter);
        break;
    case 7:
        ret = cosem_getUInt32(value->byteArr, &object->crcOkFramesCounter);
        break;
    case 8:
        ret = cosem_getUInt32(value->byteArr, &object->crcNOkFramesCounter);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int cosem_setSFSKMacSynchronizationTimeouts(
    dlmsSettings* settings,
    gxSFSKMacSynchronizationTimeouts* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    switch (index)
    {
    case 2:
        ret = cosem_getUInt16(value->byteArr, &object->searchInitiatorTimeout);
        break;
    case 3:
        ret = cosem_getUInt16(value->byteArr, &object->synchronizationConfirmationTimeout);
        break;
    case 4:
        ret = cosem_getUInt16(value->byteArr, &object->timeOutNotAddressed);
        break;
    case 5:
        ret = cosem_getUInt16(value->byteArr, &object->timeOutFrameNotOK);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int cosem_setSFSKPhyMacSetUp(
    dlmsSettings* settings,
    gxSFSKPhyMacSetUp* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    unsigned char ch;
    int pos, ret;
    switch (index)
    {
    case 2:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->initiatorElectricalPhase = (DLMS_INITIATOR_ELECTRICAL_PHASE)ch;
        }
        break;
    case 3:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->deltaElectricalPhase = (DLMS_DELTA_ELECTRICAL_PHASE)ch;
        }
        break;
    case 4:
        ret = cosem_getUInt8(value->byteArr, &object->maxReceivingGain);
        break;
    case 5:
        ret = cosem_getUInt8(value->byteArr, &object->maxTransmittingGain);
        break;
    case 6:
        ret = cosem_getUInt8(value->byteArr, &object->searchInitiatorThreshold);
        break;
    case 7:
    {
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->markFrequency)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->spaceFrequency)) != 0)
        {
            object->markFrequency = 0;
            object->spaceFrequency = 0;
        }
        break;
    }
    case 8:
        ret = cosem_getUInt16(value->byteArr, &object->macAddress);
        break;
    case 9:
    {
        arr_clear(&object->macGroupAddresses);
        uint16_t* v;
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->macGroupAddresses, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->macGroupAddresses, pos, (void**)&v, sizeof(uint16_t))) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, v)) != 0)
                {
                    break;
                }
            }
        }
        break;
    }
    case 10:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->repeater = (DLMS_REPEATER)ch;
        }
        break;
    case 11:
        ret = cosem_getBoolean(value->byteArr, &object->repeaterStatus);
        break;
    case 12:
        ret = cosem_getUInt8(value->byteArr, &object->minDeltaCredit);
        break;
    case 13:
        ret = cosem_getUInt16(value->byteArr, &object->initiatorMacAddress);
        break;
    case 14:
        ret = cosem_getBoolean(value->byteArr, &object->synchronizationLocked);
        break;
    case 15:
        if ((ret = cosem_getEnum(value->byteArr, &ch)) == 0)
        {
            object->transmissionSpeed = (DLMS_BAUD_RATE)ch;
        }
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP

#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int cosem_setSFSKReportingSystemList(
    dlmsSettings* settings,
    gxSFSKReportingSystemList* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int pos, ret;
    if (index == 2)
    {
        obj_clearByteBufferList(&object->reportingSystemList);
        uint16_t count;
        if ((ret = cosem_verifyArray(value->byteArr, &object->reportingSystemList, &count)) == 0)
        {
#ifndef DLMS_COSEM_EXACT_DATA_TYPES
            gxReportingSystemItem* v;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_getArrayItem(&object->reportingSystemList, pos, (void**)&v, sizeof(gxReportingSystemItem))) != 0 ||
                    (ret = cosem_getOctetString2(value->byteArr, v->name, sizeof(v->name), &v->size)) != 0)
                {
                    break;
                }
            }
#else
            gxByteBuffer* it;
            for (pos = 0; pos != count; ++pos)
            {
                it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(it);
                if ((ret = cosem_getOctetString(value->byteArr, it)) != 0 ||
                    (ret = arr_push(&object->reportingSystemList, it)) != 0)
                {
                    break;
                }
            }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifdef DLMS_ITALIAN_STANDARD
int updateIntervals(gxInterval* interval, gxByteBuffer* value)
{
    int ret;
    unsigned char b;
    if ((ret = bb_getUInt8(value, &b)) != 0)
    {
        return ret;
    }
    interval[0].startHour = (unsigned char)(b >> 3);
    interval[0].intervalTariff = (DLMS_DEFAULT_TARIFF_BAND)((b >> 1) & 0x3);
    interval[0].useInterval = (b & 0x1) != 0;
    if ((ret = bb_getUInt8(value, &b)) != 0)
    {
        return ret;
    }
    interval[1].startHour = (unsigned char)(b >> 3);
    interval[1].intervalTariff = (DLMS_DEFAULT_TARIFF_BAND)((b >> 1) & 0x3);
    interval[1].useInterval = (b & 0x1) != 0;
    if ((ret = bb_getUInt8(value, &b)) != 0)
    {
        return ret;
    }
    interval[2].startHour = (unsigned char)(b >> 3);
    interval[2].intervalTariff = (DLMS_DEFAULT_TARIFF_BAND)((b >> 1) & 0x3);
    interval[2].useInterval = (b & 0x1) != 0;
    if ((ret = bb_getUInt8(value, &b)) != 0)
    {
        return ret;
    }
    interval[3].startHour = (unsigned char)(b >> 3);
    interval[3].intervalTariff = (DLMS_DEFAULT_TARIFF_BAND)((b >> 1) & 0x3);
    interval[3].useInterval = (b & 0x1) != 0;
    if ((ret = bb_getUInt8(value, &b)) != 0)
    {
        return ret;
    }
    interval[4].startHour = (unsigned char)(b >> 3);
    interval[4].intervalTariff = (DLMS_DEFAULT_TARIFF_BAND)((b >> 1) & 0x3);
    interval[4].useInterval = (b & 0x1) != 0;
    return 0;
}

int updateSeason(gxBandDescriptor* season, variantArray* value)
{
    int ret;
    dlmsVARIANT* tmp;
    if (value->size == 5)
    {
        if ((ret = va_getByIndex(value, 0, &tmp)) != 0)
        {
            return ret;
        }
        season->dayOfMonth = tmp->bVal;
        if ((ret = va_getByIndex(value, 1, &tmp)) != 0)
        {
            return ret;
        }
        season->month = tmp->bVal;
        if ((ret = va_getByIndex(value, 2, &tmp)) != 0 ||
            (ret = updateIntervals(season->workingDayIntervals, tmp->byteArr)) != 0 ||
            (ret = va_getByIndex(value, 3, &tmp)) != 0 ||
            (ret = updateIntervals(season->saturdayIntervals, tmp->byteArr)) != 0 ||
            (ret = va_getByIndex(value, 4, &tmp)) != 0 ||
            (ret = updateIntervals(season->holidayIntervals, tmp->byteArr)) != 0)
        {
            return ret;
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    return ret;
}

int cosem_setTariffPlan(gxTariffPlan* object, unsigned char index, dlmsVARIANT* value)
{
    dlmsVARIANT tmp3;
    dlmsVARIANT* tmp, * tmp2;
    int ret, pos, h, m, s;
    switch (index)
    {
    case 2:
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            object->calendarName = (char*)gxmalloc(value->byteArr->size);
            memcpy(object->calendarName, value->byteArr->data, value->byteArr->size);
            object->calendarName[value->byteArr->size] = 0;
        }
        else
        {
            object->calendarName = (char*)gxmalloc(value->strVal->size + 1);
            memcpy(object->calendarName, value->strVal->data, value->strVal->size);
            object->calendarName[value->strVal->size] = 0;
        }
        break;
    case 3:
        object->enabled = value->boolVal;
        break;
    case 4:
    {
        if (value->Arr->size == 4)
        {
            if ((ret = va_getByIndex(value->Arr, 0, &tmp)) != 0)
            {
                return ret;
            }
            object->plan.defaultTariffBand = tmp->bVal;
            if ((ret = va_getByIndex(value->Arr, 1, &tmp)) != 0)
            {
                return ret;
            }
            if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != 0 ||
                (ret = updateSeason(&object->plan.winterSeason, tmp2->Arr)) != 0 ||
                (ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != 0 ||
                (ret = updateSeason(&object->plan.summerSeason, tmp2->Arr)) != 0)
            {
                return ret;
            }
            ba_clear(&object->plan.weeklyActivation);
            if ((ret = va_getByIndex(value->Arr, 2, &tmp)) != 0 ||
                (ret = ba_copy(&object->plan.weeklyActivation, tmp->bitArr->data, tmp->bitArr->size)) != 0)
            {
                return ret;
            }
            if ((ret = va_getByIndex(value->Arr, 3, &tmp)) != 0)
            {
                return ret;
            }
            arr_clear(&object->plan.specialDays);
            arr_capacity(&object->plan.specialDays, tmp->Arr->size);
            for (pos = 0; pos != tmp->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(tmp->Arr, pos, &tmp2)) != 0)
                {
                    return ret;
                }
                arr_push(&object->plan.specialDays, (void*)tmp2->ulVal);
            }
        }
        break;
    }
    case 5:
    {
        if ((ret = va_getByIndex(value->Arr, 0, &tmp)) != 0)
        {
            return ret;
        }
        var_init(&tmp3);
        if ((ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_TIME, &tmp3)) != 0)
        {
            return ret;
        }
        if ((tmp3.dateTime->skip & DATETIME_SKIPS_HOUR) == 0)
        {
            h = time_getHours(tmp3.dateTime);
        }
        else
        {
            h = 0;
        }
        if ((tmp3.dateTime->skip & DATETIME_SKIPS_MINUTE) == 0)
        {
            m = time_getMinutes(tmp3.dateTime);
        }
        else
        {
            m = 0;
        }
        if ((tmp3.dateTime->skip & DATETIME_SKIPS_SECOND) == 0)
        {
            s = time_getSeconds(tmp3.dateTime);
        }
        else
        {
            s = 0;
        }
        if ((ret = va_getByIndex(value->Arr, 1, &tmp2)) != 0)
        {
            return ret;
        }
        var_clear(&tmp3);
        if ((ret = dlms_changeType2(tmp2, DLMS_DATA_TYPE_DATE, &tmp3)) != 0)
        {
            return ret;
        }
        time_copy(&object->activationTime, tmp3.dateTime);
        object->activationTime.skip &= ~(DATETIME_SKIPS_HOUR | DATETIME_SKIPS_MINUTE | DATETIME_SKIPS_SECOND | DATETIME_SKIPS_MS);
        time_addHours(&object->activationTime, h);
        time_addMinutes(&object->activationTime, m);
        time_addSeconds(&object->activationTime, s);
    }
    break;
    default:
        return DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_ITALIAN_STANDARD

#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
