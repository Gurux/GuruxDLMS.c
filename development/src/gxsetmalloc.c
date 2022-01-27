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
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)

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

#ifndef DLMS_IGNORE_DATA
int cosem_setData(gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
        ret = var_copy(&((gxData*)e->target)->value, &e->value);
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
    int ret = 0;
    if (index == 2)
    {
        ret = var_copy(&object->value, value);
    }
    else if (index == 3)
    {
        dlmsVARIANT* tmp;
        if (value->vt != DLMS_DATA_TYPE_STRUCTURE)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->scaler = (char)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->unit = (unsigned char)var_toInteger(tmp);
        object->unitRead = 1;
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
    int pos, ret = 0;
    dlmsVARIANT_PTR tmp;
    if (index == 2)
    {
        if (value->Arr != NULL)
        {
            dlmsVARIANT* tmp2;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                tmp2 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                var_init(tmp2);
                ret = var_copy(tmp2, tmp);
                if (ret != 0)
                {
                    break;
                }
                va_push(&object->tableCellValues, tmp2);
            }
        }
    }
    else if (index == 4)
    {
        if (value->vt != DLMS_DATA_TYPE_STRUCTURE)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->scaler = (char)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->unit = (unsigned char)var_toInteger(tmp);
        object->unitRead = 1;
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
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT tmp;
    if (index == 2)
    {
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tmp);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp);
            if (ret != 0)
            {
                return ret;
            }
            time_copy(&object->time, tmp.dateTime);
            //If user set's new time transform it to the same time zone as the server is.
#ifndef DLMS_IGNORE_SERVER
            if (settings->server)
            {
                //Convert time to UCT if time zone is given.
                time_toUTC(&object->time);
                clock_updateDST(object, &object->time);
            }
#endif// DLMS_IGNORE_SERVER
            var_clear(&tmp);
        }
        else
        {
            time_clear(&object->time);
        }
    }
    else if (index == 3)
    {
        object->timeZone = (short)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->status = (DLMS_CLOCK_STATUS)var_toInteger(value);
    }
    else if (index == 5)
    {
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tmp);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp);
            if (ret != 0)
            {
                return ret;
            }
            time_copy(&object->begin, tmp.dateTime);
            var_clear(&tmp);
        }
        else
        {
            time_clear(&object->begin);
        }
    }
    else if (index == 6)
    {
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tmp);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp);
            if (ret != 0)
            {
                return ret;
            }
            time_copy(&object->end, tmp.dateTime);
            var_clear(&tmp);
        }
        else
        {
            time_clear(&object->end);
        }
    }
    else if (index == 7)
    {
        object->deviation = (char)var_toInteger(value);
#ifndef DLMS_IGNORE_SERVER
        if (settings->server)
        {
            clock_updateDST(object, &object->time);
        }
#endif// DLMS_IGNORE_SERVER
    }
    else if (index == 8)
    {
        object->enabled = (unsigned char)var_toInteger(value);
#ifndef DLMS_IGNORE_SERVER
        if (settings->server)
        {
            clock_updateDST(object, &object->time);
        }
#endif //DLMS_IGNORE_SERVER
    }
    else if (index == 9)
    {
        object->clockBase = (DLMS_CLOCK_BASE)var_toInteger(value);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

#endif //DLMS_IGNORE_CLOCK

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR


// The season profiles list is sorted according to season_start (in increasing order).
int cosem_orderSeasonProfile(gxArray* profile)
{
    int ret = 0;
    uint16_t pos, pos2, minPos = 0;
    gxSeasonProfile* sp, * sp2;
    uint32_t tmp, next1, next2;
    for (pos = 0; pos < profile->size - 1; ++pos)
    {
        if ((ret = arr_getByIndex(profile, pos, (void**)&sp)) != 0)
        {
            break;
        }
        next1 = time_toUnixTime2(&sp->start);
        next2 = 0xFFFFFFFF;
        for (pos2 = pos + 1; pos2 < profile->size; ++pos2)
        {
            if ((ret = arr_getByIndex(profile, pos2, (void**)&sp2)) != 0)
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
            if ((ret = arr_swap(profile, pos, minPos)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int updateSeasonProfile(gxArray* profile, dlmsVARIANT* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxSeasonProfile* sp = NULL;
    obj_clearSeasonProfile(profile);
    dlmsVARIANT tm;
    dlmsVARIANT* tmp, * it;
    var_init(&tm);
    for (pos = 0; pos != data->Arr->size; ++pos)
    {
        sp = (gxSeasonProfile*)gxmalloc(sizeof(gxSeasonProfile));
        if (sp == NULL)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        ret = va_getByIndex(data->Arr, pos, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ret = va_getByIndex(it->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        BYTE_BUFFER_INIT(&sp->name);
        bb_set2(&sp->name, tmp->byteArr, 0, bb_size(tmp->byteArr));

        ret = va_getByIndex(it->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &tm);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        time_copy(&sp->start, tm.dateTime);
        var_clear(&tm);
        ret = va_getByIndex(it->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        BYTE_BUFFER_INIT(&sp->weekName);
        bb_set2(&sp->weekName, tmp->byteArr, 0, bb_size(tmp->byteArr));
        arr_push(profile, sp);
    }
    if (ret != 0 && sp != NULL)
    {
        gxfree(sp);
    }
    return ret;
}

int updateWeekProfileTable(gxArray* profile, dlmsVARIANT* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxWeekProfile* wp = NULL;
    obj_clearWeekProfileTable(profile);
    dlmsVARIANT* tmp, * it;
    for (pos = 0; pos != data->Arr->size; ++pos)
    {
        wp = (gxWeekProfile*)gxmalloc(sizeof(gxWeekProfile));
        if (wp == NULL)
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
            break;
        }
        ret = va_getByIndex(data->Arr, pos, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ret = va_getByIndex(it->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        BYTE_BUFFER_INIT(&wp->name);
        bb_set2(&wp->name, tmp->byteArr, 0, bb_size(tmp->byteArr));
        ret = va_getByIndex(it->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        wp->monday = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(it->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        wp->tuesday = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(it->Arr, 3, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        wp->wednesday = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(it->Arr, 4, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        wp->thursday = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(it->Arr, 5, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        wp->friday = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(it->Arr, 6, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        wp->saturday = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(it->Arr, 7, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        wp->sunday = (unsigned char)var_toInteger(tmp);
        arr_push(profile, wp);
    }
    if (ret != 0 && wp != NULL)
    {
        gxfree(wp);
    }
    return ret;
}

int updateDayProfileTable(dlmsSettings* settings, gxArray* profile, dlmsVARIANT* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos, pos2;
    gxDayProfile* dp = NULL;
    gxDayProfileAction* ac = NULL;
    obj_clearDayProfileTable(profile);
    dlmsVARIANT* tmp, * tmp2, * it, * it2;
    dlmsVARIANT tm;
    for (pos = 0; pos != data->Arr->size; ++pos)
    {
        dp = (gxDayProfile*)gxmalloc(sizeof(gxDayProfile));
        if (dp == NULL)
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
            break;
        }
        arr_init(&dp->daySchedules);
        ret = va_getByIndex(data->Arr, pos, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ret = va_getByIndex(it->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        dp->dayId = (unsigned char)var_toInteger(tmp);
        ret = va_getByIndex(it->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        for (pos2 = 0; pos2 != tmp->Arr->size; ++pos2)
        {
            ac = (gxDayProfileAction*)gxmalloc(sizeof(gxDayProfileAction));
            if (ac == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            ret = va_getByIndex(tmp->Arr, pos2, &it2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            //Get start time.
            ret = va_getByIndex(it2->Arr, 0, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            var_init(&tm);
            ret = dlms_changeType2(tmp2, DLMS_DATA_TYPE_TIME, &tm);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            time_copy(&ac->startTime, tm.dateTime);
            var_clear(&tm);
            //Get script logical name.
            ret = va_getByIndex(it2->Arr, 1, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if (tmp2->vt == DLMS_DATA_TYPE_OCTET_STRING && tmp2->byteArr != NULL)
            {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, tmp2->byteArr->data, &ac->script)) != 0)
                {
                    break;
                }
                if (ac->script == NULL)
                {
                    if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, &ac->script)) != 0)
                    {
                        return ret;
                    }
                    memcpy(ac->script->logicalName, tmp2->byteArr->data, tmp2->byteArr->size);
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, ac->script);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                }
#else
                memcpy(ac->scriptLogicalName, tmp2->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
            ret = va_getByIndex(it2->Arr, 2, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            ac->scriptSelector = (uint16_t)var_toInteger(tmp2);
            arr_push(&dp->daySchedules, ac);
        }
        if (ret != 0)
        {
            break;
        }
        arr_push(profile, dp);
    }
    if (ret != 0)
    {
        if (dp)
        {
            gxfree(dp);
        }
        if (ac)
        {
            gxfree(ac);
        }
    }
    return ret;
}

int cosem_setActivityCalendar(dlmsSettings* settings, gxActivityCalendar* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT tm;
    if (index == 2)
    {
        bb_clear(&object->calendarNameActive);
        ret = bb_set2(&object->calendarNameActive, value->byteArr, 0, bb_size(value->byteArr));
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
        bb_clear(&object->calendarNamePassive);
        ret = bb_set2(&object->calendarNamePassive, value->byteArr, 0, bb_size(value->byteArr));
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
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tm);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tm);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            time_copy(&object->time, tm.dateTime);
            var_clear(&tm);
        }
        else
        {
            time_clear(&object->time);
        }
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
    int ret = 0, pos;
    dlmsVARIANT* tmp;
    gxtime* tm;
    if (index == 2)
    {
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0)
        {
            if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, tmp->byteArr->data, (gxObject**)&object->executedScript)) != 0)
            {
                //Error code is returned at the end of the function.
            }
            if (object->executedScript == NULL)
            {
                //create object.
                ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, (gxObject**)&object->executedScript);
                if (ret == DLMS_ERROR_CODE_OK)
                {
                    ret = cosem_setLogicalName((gxObject*)object->executedScript, tmp->byteArr->data);
                    if (ret == DLMS_ERROR_CODE_OK)
                    {
                        //Add object to released objects list.
                        ret = oa_push(&settings->releasedObjects, (gxObject*)object->executedScript);
                    }
                }
            }
        }
#else
        memcpy(object->executedScriptLogicalName, tmp->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->executedScriptSelector = (uint16_t)var_toInteger(tmp);
    }
    else if (index == 3)
    {
        object->type = (DLMS_SINGLE_ACTION_SCHEDULE_TYPE)var_toInteger(value);
    }
    else if (index == 4)
    {
        arr_clear(&object->executionTime);
        dlmsVARIANT time, date;
        dlmsVARIANT* tmp2;
        var_init(&time);
        var_init(&date);
        if (value->Arr != NULL)
        {
            arr_capacity(&object->executionTime, value->Arr->size);
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = dlms_changeType2(tmp2, DLMS_DATA_TYPE_TIME, &time);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 1, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(tmp2, DLMS_DATA_TYPE_DATE, &date);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
#ifdef DLMS_USE_EPOCH_TIME
                time_addHours(date.dateTime, time_getHours(time.dateTime));
                time_addMinutes(date.dateTime, time_getMinutes(time.dateTime));
                time_addSeconds(date.dateTime, time_getSeconds(time.dateTime));
                date.dateTime->skip = (DATETIME_SKIPS)(date.dateTime->skip & time.dateTime->skip);
#else
                date.dateTime->value.tm_hour = time.dateTime->value.tm_hour;
                date.dateTime->value.tm_min = time.dateTime->value.tm_min;
                date.dateTime->value.tm_sec = time.dateTime->value.tm_sec;
                date.dateTime->skip &= ~(DATETIME_SKIPS_HOUR | DATETIME_SKIPS_MINUTE | DATETIME_SKIPS_SECOND | DATETIME_SKIPS_MS);
                date.dateTime->skip |= time.dateTime->skip & (DATETIME_SKIPS_HOUR | DATETIME_SKIPS_MINUTE | DATETIME_SKIPS_SECOND | DATETIME_SKIPS_MS);
#endif // DLMS_USE_EPOCH_TIME
                date.dateTime->deviation = (short)0x8000;
                date.dateTime->skip |= DATETIME_SKIPS_DEVITATION;
                tm = (gxtime*)gxmalloc(sizeof(gxtime));
                if (tm == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                time_copy(tm, date.dateTime);
                arr_push(&object->executionTime, tm);
                var_clear(&time);
                var_clear(&date);
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

int cosem_updateAttributeAccessModes(gxObject* object, variantArray* arr)
{
    unsigned char id;
    int ret;
    uint16_t pos, cnt;
    dlmsVARIANT* tmp, * it, * value;
    //If accessmodes are not returned. Some meters do not return them.
    if (arr->size != 2)
    {
        return 0;
    }
    ret = va_getByIndex(arr, 0, &tmp);
    if (ret != 0)
    {
        return ret;
    }
    //If access modes are not retrieved yet.
    if (object->access == NULL || object->access->attributeAccessModes.size == 0)
    {
        if (object->access == NULL)
        {
            object->access = (gxAccess*)gxcalloc(1, sizeof(gxAccess));
        }
        cnt = obj_attributeCount(object);
        bb_capacity(&object->access->attributeAccessModes, cnt);
        object->access->attributeAccessModes.size = object->access->attributeAccessModes.capacity;

        cnt = obj_methodCount(object);
        bb_capacity(&object->access->methodAccessModes, cnt);
        object->access->methodAccessModes.size = object->access->methodAccessModes.capacity;
    }
    for (pos = 0; pos != tmp->Arr->size; ++pos)
    {
        ret = va_getByIndex(tmp->Arr, pos, &it);
        if (ret != 0)
        {
            return ret;
        }
        if (it->vt != DLMS_DATA_TYPE_STRUCTURE ||
            it->Arr->size != 3)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }

        //Get ID.
        ret = va_getByIndex(it->Arr, 0, &value);
        if (ret != 0)
        {
            return ret;
        }

        id = (unsigned char)var_toInteger(value);
        if (!(id > object->access->attributeAccessModes.size))
        {
            ret = va_getByIndex(it->Arr, 1, &value);
            if (ret != 0)
            {
                return ret;
            }
            //DLMS_ACCESS_MODE
            object->access->attributeAccessModes.data[id - 1] = (unsigned char)var_toInteger(value);
        }
    }

    //Get method access modes.
    ret = va_getByIndex(arr, 1, &tmp);
    if (ret != 0)
    {
        return ret;
    }
    for (pos = 0; pos != tmp->Arr->size; ++pos)
    {
        ret = va_getByIndex(tmp->Arr, pos, &it);
        if (ret != 0)
        {
            return ret;
        }
        if (it->vt != DLMS_DATA_TYPE_STRUCTURE ||
            it->Arr->size != 2)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }

        //Get ID.
        ret = va_getByIndex(it->Arr, 0, &value);
        if (ret != 0)
        {
            return ret;
        }

        id = (unsigned char)var_toInteger(value);
        if (!(id > object->access->methodAccessModes.size))
        {
            ret = va_getByIndex(it->Arr, 1, &value);
            if (ret != 0)
            {
                return ret;
            }
            //DLMS_METHOD_ACCESS_MODE
            object->access->methodAccessModes.data[id - 1] = (unsigned char)var_toInteger(value);
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

int cosem_parseLNObjects(dlmsSettings* settings, gxByteBuffer* data, objectArray* objects)
{
    gxObject* object;
    int ret;
    uint16_t pos, count;
    unsigned char size;
    oa_clear(&settings->objects, 1);
    unsigned char version;
    gxDataInfo info;
    DLMS_OBJECT_TYPE class_id = 0;
    dlmsVARIANT value;
    dlmsVARIANT* it1 = NULL, * it2 = NULL, * it3 = NULL, * ln = NULL;
    var_init(&value);
    //Get array tag.
    ret = bb_getUInt8(data, &size);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    //Check that data is in the array
    if (size != DLMS_DATA_TYPE_ARRAY)
    {
        return DLMS_ERROR_CODE_INVALID_RESPONSE;
    }
    //get object count
    if (hlp_getObjectCount2(data, &count) != 0)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    oa_capacity(objects, (uint16_t)count);
    for (pos = 0; pos != count; ++pos)
    {
        var_clear(&value);
        object = NULL;
        di_init(&info);
        if ((ret = dlms_getData(data, &info, &value)) != 0)
        {
            //This fixs Iskraemeco (MT-880) bug.
            if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
            {
                ret = 0;
            }
            break;
        }
        if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr->size != 4)
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        }
        if ((ret = va_getByIndex(value.Arr, 0, &it1)) != 0 ||
            (ret = va_getByIndex(value.Arr, 1, &it2)) != 0 ||
            (ret = va_getByIndex(value.Arr, 2, &ln)) != 0 ||
            (ret = va_getByIndex(value.Arr, 3, &it3)) != 0)
        {
            break;
        }
        if (it1->vt != DLMS_DATA_TYPE_UINT16 ||
            it2->vt != DLMS_DATA_TYPE_UINT8 ||
            ln->vt != DLMS_DATA_TYPE_OCTET_STRING ||
            it3->vt != DLMS_DATA_TYPE_STRUCTURE ||
            it3->Arr->size != 2)
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        }
        class_id = (DLMS_OBJECT_TYPE)var_toInteger(it1);
        version = (unsigned char)var_toInteger(it2);
        ret = cosem_createObject(class_id, &object);
        //If known object.
        if (ret == 0)
        {
            object->version = version;
            ret = cosem_updateAttributeAccessModes(object, it3->Arr);
            if (ret != 0)
            {
                break;
            }
            cosem_setLogicalName(object, ln->byteArr->data);
            oa_push(&settings->objects, object);
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
}

int cosem_setAssociationLogicalName(
    dlmsSettings* settings,
    gxAssociationLogicalName* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret = 0, pos = 0;
    dlmsVARIANT* tmp, * tmp2;
    unsigned char ch;
    if (index == 2)
    {
        gxObject* obj = NULL;
        unsigned char version;
        DLMS_OBJECT_TYPE type;
        oa_empty(&object->objectList);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = va_getByIndex(tmp->Arr, 0, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                type = (DLMS_OBJECT_TYPE)var_toInteger(tmp2);
                ret = va_getByIndex(tmp->Arr, 1, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                version = (unsigned char)var_toInteger(tmp2);

                //Get Logical name.
                ret = va_getByIndex(tmp->Arr, 2, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = oa_findByLN(&settings->objects, type, tmp2->byteArr->data, &obj);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if (obj == NULL)
                {
                    ret = cosem_createObject(type, &obj);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        //If unknown object.
                        if (ret == DLMS_ERROR_CODE_INVALID_PARAMETER)
                        {
                            continue;
                        }
                        return ret;
                    }
                    ret = cosem_setLogicalName(obj, tmp2->byteArr->data);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    obj->version = (unsigned char)version;
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, obj);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                }
                ret = va_getByIndex(tmp->Arr, 3, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = cosem_updateAttributeAccessModes(obj, tmp2->Arr);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                oa_push(&object->objectList, obj);
            }
        }
    }
    else if (index == 3)
    {
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->clientSAP = (unsigned char)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->serverSAP = (uint16_t)var_toInteger(tmp);
    }
    else if (index == 4)
    {
        //Value of the object identifier encoded in BER
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            if (value->byteArr->data[0] == 0x60)
            {
                object->applicationContextName.jointIsoCtt = 0;
                object->applicationContextName.country = 0;
                object->applicationContextName.countryName = 0;
                ret = bb_getUInt8ByIndex(value->byteArr, 3, &object->applicationContextName.identifiedOrganization);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = bb_getUInt8ByIndex(value->byteArr, 4, &object->applicationContextName.dlmsUA);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = bb_getUInt8ByIndex(value->byteArr, 5, &object->applicationContextName.applicationContext);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = bb_getUInt8ByIndex(value->byteArr, 6, &object->applicationContextName.contextId);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
            }
            else
            {
                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 2)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                //Get Len.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 7)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                //Get tag
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                ret = bb_getUInt8(value->byteArr, &object->applicationContextName.jointIsoCtt);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                //Get tag
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt8(value->byteArr, &object->applicationContextName.country);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                //Get tag
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x12)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt16(value->byteArr, &object->applicationContextName.countryName);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x12)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                //Get tag
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt8(value->byteArr, &object->applicationContextName.identifiedOrganization);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                //Get tag
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt8(value->byteArr, &object->applicationContextName.dlmsUA);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                //Get tag
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt8(value->byteArr, &object->applicationContextName.applicationContext);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                //Get tag
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt8(value->byteArr, &object->applicationContextName.contextId);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
            }
        }
        else if (value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            ret = va_getByIndex(value->Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.jointIsoCtt = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.country = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 2, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.countryName = (uint16_t)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 3, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.identifiedOrganization = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 4, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.dlmsUA = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 5, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.applicationContext = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 6, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.contextId = (unsigned char)var_toInteger(tmp);
        }
    }
    else if (index == 5)
    {
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.maxReceivePduSize = (uint16_t)var_toInteger(tmp);

        ret = va_getByIndex(value->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.maxSendPduSize = (uint16_t)var_toInteger(tmp);

        ret = va_getByIndex(value->Arr, 3, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.dlmsVersionNumber = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(value->Arr, 4, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.qualityOfService = (unsigned char)var_toInteger(tmp);

        ret = va_getByIndex(value->Arr, 5, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_clear(&object->xDLMSContextInfo.cypheringInfo);
        if (tmp->byteArr != NULL)
        {
            bb_set2(&object->xDLMSContextInfo.cypheringInfo, tmp->byteArr, 0, tmp->byteArr->size);
        }
    }
    else if (index == 6)
    {
        //Value of the object identifier encoded in BER
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            if (value->byteArr->data[0] == 0x60)
            {
                object->authenticationMechanismName.jointIsoCtt = 0;
                object->authenticationMechanismName.country = 0;
                object->authenticationMechanismName.countryName = 0;
                pos += 2;
                object->authenticationMechanismName.identifiedOrganization = value->byteArr->data[++pos];
                object->authenticationMechanismName.dlmsUA = value->byteArr->data[++pos];
                object->authenticationMechanismName.authenticationMechanismName = value->byteArr->data[++pos];
                object->authenticationMechanismName.mechanismId = (DLMS_AUTHENTICATION)value->byteArr->data[++pos];
            }
            else
            {
                if ((ret = cosem_checkStructure(value->byteArr, 7)) == 0 &&
                    (ret = cosem_getUInt8(value->byteArr, &object->authenticationMechanismName.jointIsoCtt)) == 0 &&
                    (ret = cosem_getUInt8(value->byteArr, &object->authenticationMechanismName.country)) == 0 &&
                    (ret = cosem_getUInt16(value->byteArr, &object->authenticationMechanismName.countryName)) == 0 &&
                    (ret = cosem_getUInt8(value->byteArr, &object->authenticationMechanismName.identifiedOrganization)) == 0 &&
                    (ret = cosem_getUInt8(value->byteArr, &object->authenticationMechanismName.dlmsUA)) == 0 &&
                    (ret = cosem_getUInt8(value->byteArr, &object->authenticationMechanismName.authenticationMechanismName)) == 0 &&
                    (ret = cosem_getUInt8(value->byteArr, &ch)) == 0)
                {
                    object->authenticationMechanismName.mechanismId = (DLMS_AUTHENTICATION)ch;
                }
            }
        }
        else  if (value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            ret = va_getByIndex(value->Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.jointIsoCtt = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.country = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 2, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.countryName = (uint16_t)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 3, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.identifiedOrganization = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 4, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.dlmsUA = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 5, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.authenticationMechanismName = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 6, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.mechanismId = (DLMS_AUTHENTICATION)var_toInteger(tmp);
        }
    }
    else if (index == 7)
    {
        bb_clear(&object->secret);
        if (value->byteArr != NULL)
        {
            ret = bb_set2(&object->secret, value->byteArr, 0, bb_size(value->byteArr));
        }
    }
    else if (index == 8)
    {
        object->associationStatus = (DLMS_ASSOCIATION_STATUS)var_toInteger(value);
    }
    else if (index == 9)
    {
        if (bb_size(value->byteArr) != 6)
        {
            return DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SECURITY_SETUP, value->byteArr->data, (gxObject**)&object->securitySetup);
#else
        memcpy(object->securitySetupReference, value->byteArr->data, 6);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
    }
    else if (index == 10)
    {
        obj_clearUserList(&object->userList);
        if (value->Arr != NULL)
        {
            gxKey2* it;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it = (gxKey2*)gxmalloc(sizeof(gxKey2));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                it->key = (unsigned char)var_toInteger(tmp2);
                ret = va_getByIndex(tmp->Arr, 1, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it->value = (char*)gxmalloc(tmp->strVal->size + 1);
                if (it->value == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                ((char*)it->value)[tmp->strVal->size] = 0;
                memcpy(it->value, tmp->strVal->data, tmp->strVal->size);
                arr_push(&object->userList, it);
            }
        }
    }
    else if (index == 11)
    {
        if (object->currentUser.value != NULL)
        {
            gxfree(object->currentUser.value);
        }
        if (value->Arr->size == 2)
        {
            ret = va_getByIndex(value->Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->currentUser.key = (unsigned char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            if (tmp->strVal != NULL && tmp->strVal->size != 0)
            {
                object->currentUser.value = gxmalloc(tmp->strVal->size + 1);
                if (object->currentUser.value == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(object->currentUser.value, tmp->strVal, tmp->strVal->size);
            }
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
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
            ret = cosem_updateAttributeAccessModes(obj, it->Arr);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            /*
            ret = va_getByIndex(it->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
            return ret;
            }
            //Update attribute Access
            for(pos2 = 0; pos2 != tmp->Arr.size; ++pos2)
            {
            ret = va_getByIndex(tmp->Arr, pos, &access);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //Get attribute ID.
            ret = va_getByIndex(&access->Arr, 0, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //TODO: id = var_toInteger(tmp2);
            //Get access mode.
            ret = va_getByIndex(&access->Arr, 1, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //TODO: obj->SetAccess(id, (DLMS_ACCESS_MODE) var_toInteger(tmp2));
            }

            //Update method Access
            ret = va_getByIndex(it->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
            return ret;
            }
            for(pos2 = 0; pos2 != tmp->Arr.size; ++pos2)
            {
            ret = va_getByIndex(tmp->Arr, pos, &access);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //Get attribute ID.
            ret = va_getByIndex(&access->Arr, 0, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //TODO: id = var_toInteger(tmp2);
            //Get access mode.
            ret = va_getByIndex(&access->Arr, 1, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //TODO: obj->SetMethodAccess(id, (DLMS_ACCESS_MODE) var_toInteger(tmp2));
            }
            */
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int cosem_parseSNObjects(dlmsSettings* settings, gxByteBuffer* data, objectArray* objects)
{
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
}

int cosem_setAssociationShortName(
    dlmsSettings* settings,
    gxAssociationShortName* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
    int pos;
    if (index == 2)
    {
        dlmsVARIANT* it, * it1 = NULL, * it2 = NULL, * it3 = NULL, * ln = NULL;
        unsigned char version;
        short sn;
        DLMS_OBJECT_TYPE class_id;
        gxObject* obj;
        oa_empty(&object->objectList);
        if ((ret = oa_capacity(&object->objectList, value->Arr->size)) == 0)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                obj = NULL;
                if ((ret = va_getByIndex(value->Arr, 0, &it)) != 0 ||
                    (ret = va_getByIndex(it->Arr, 0, &it1)) != 0 ||
                    (ret = va_getByIndex(it->Arr, 1, &it2)) != 0 ||
                    (ret = va_getByIndex(it->Arr, 2, &it3)) != 0 ||
                    (ret = va_getByIndex(it->Arr, 3, &ln)) != 0)
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
                ret = cosem_createObject(class_id, &obj);
                //If known object.
                if (ret == 0)
                {
                    obj->shortName = sn;
                    obj->version = version;
                    cosem_setLogicalName(obj, ln->byteArr->data);
                    oa_push(&object->objectList, obj);
                    oa_push(&settings->releasedObjects, obj);
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
        }
    }
    else if (index == 3)
    {
        ret = updateSNAccessRights(&object->objectList, value->Arr);
    }
    else if (index == 4)
    {
        if (bb_size(value->byteArr) != 6)
        {
            ret = DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
        else
        {
#ifndef DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SECURITY_SETUP, value->byteArr->data, (gxObject**)&object->securitySetup);
#else
            memcpy(object->securitySetupReference, value->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif // DLMS_IGNORE_SECURITY_SETUP
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_AUTO_ANSWER
int cosem_setAutoAnswer(gxAutoAnswer* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0, pos;
    dlmsVARIANT* tmp;
    if (index == 2)
    {
        object->mode = (DLMS_AUTO_ANSWER_MODE)var_toInteger(value);
    }
    else if (index == 3)
    {
        arr_clearKeyValuePair(&object->listeningWindow);
        dlmsVARIANT* tmp3;
        dlmsVARIANT start, end;
        gxtime* s, * e;
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                var_init(&start);
                var_init(&end);
                ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &start);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &end);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                s = (gxtime*)gxmalloc(sizeof(gxtime));
                e = (gxtime*)gxmalloc(sizeof(gxtime));
                time_copy(s, start.dateTime);
                time_copy(e, end.dateTime);
                arr_push(&object->listeningWindow, key_init(s, e));
                var_clear(&start);
                var_clear(&end);
            }
        }
    }
    else if (index == 4)
    {
        object->status = (DLMS_AUTO_ANSWER_STATUS)var_toInteger(value);
    }
    else if (index == 5)
    {
        object->numberOfCalls = (unsigned char)var_toInteger(value);
    }
    else if (index == 6)
    {
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->numberOfRingsInListeningWindow = (unsigned char)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->numberOfRingsOutListeningWindow = (unsigned char)var_toInteger(tmp);
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
    int ret = 0, pos;
    if (index == 2)
    {
        object->mode = (DLMS_AUTO_CONNECT_MODE)var_toInteger(value);
    }
    else if (index == 3)
    {
        object->repetitions = (unsigned char)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->repetitionDelay = (uint16_t)var_toInteger(value);
    }
    else if (index == 5)
    {
        dlmsVARIANT* tmp, * tmp3;
        dlmsVARIANT start, end;
        arr_clearKeyValuePair(&object->callingWindow);
        if (value->Arr != NULL)
        {
            if ((ret = arr_capacity(&object->callingWindow, value->Arr->size)) == 0)
            {
                for (pos = 0; pos != value->Arr->size; ++pos)
                {
                    ret = va_getByIndex(value->Arr, pos, &tmp);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    gxtime* s, * e;
                    var_init(&start);
                    var_init(&end);
                    ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &start);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &end);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    s = (gxtime*)gxmalloc(sizeof(gxtime));
                    e = (gxtime*)gxmalloc(sizeof(gxtime));
                    arr_push(&object->callingWindow, key_init(s, e));
                    time_copy(s, start.dateTime);
                    time_copy(e, end.dateTime);
                    var_clear(&start);
                    var_clear(&end);
                }
            }
        }
    }
    else if (index == 6)
    {
        dlmsVARIANT* tmp;
        gxByteBuffer* str;
        arr_clearStrings(&object->destinations);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                str = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(str);
                bb_set2(str, tmp->byteArr, 0, bb_size(tmp->byteArr));
                arr_push(&object->destinations, str);
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
int cosem_setDemandRegister(gxDemandRegister* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT* tmp;
    dlmsVARIANT tmp2;
    if (index == 2)
    {
        ret = var_copy(&object->currentAverageValue, value);
    }
    else if (index == 3)
    {
        ret = var_copy(&object->lastAverageValue, value);
    }
    else if (index == 4)
    {
        if (value->vt != DLMS_DATA_TYPE_STRUCTURE)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->scaler = (char)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->unit = (unsigned char)var_toInteger(tmp);
        object->unitRead = 1;
    }
    else if (index == 5)
    {
        ret = var_copy(&object->status, value);
    }
    else if (index == 6)
    {
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            ret = var_init(&tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            time_copy(&object->captureTime, tmp2.dateTime);
            var_clear(&tmp2);
        }
        else
        {
            time_clear(&object->captureTime);
        }
    }
    else if (index == 7)
    {
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            ret = var_init(&tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            time_copy(&object->startTimeCurrent, tmp2.dateTime);
            var_clear(&tmp2);
        }
        else
        {
            time_clear(&object->startTimeCurrent);
        }
    }
    else if (index == 8)
    {
        object->period = var_toInteger(value);
    }
    else if (index == 9)
    {
        object->numberOfPeriods = (uint16_t)var_toInteger(value);
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
    int ret = DLMS_ERROR_CODE_OK;
    if (index == 2)
    {
        bb_clear(&object->macAddress);
        ret = bb_set2(&object->macAddress, value->byteArr, 0, bb_size(value->byteArr));
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
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT* tmp;
    dlmsVARIANT tmp2;
    if (index == 2)
    {
        ret = var_copy(&object->value, value);
    }
    else if (index == 3)
    {
        if (value->vt != DLMS_DATA_TYPE_STRUCTURE)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->scaler = (char)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->unit = (unsigned char)var_toInteger(tmp);
    }
    else if (index == 4)
    {
        ret = var_copy(&object->status, value);
    }
    else if (index == 5)
    {
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = var_init(&tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp2);
            if (ret == 0)
            {
                time_copy(&object->captureTime, tmp2.dateTime);
                var_clear(&tmp2);
            }
        }
        else
        {
            time_copy(&object->captureTime, value->dateTime);
        }
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
        bb_clear(&object->apn);
        if (value->vt == DLMS_DATA_TYPE_STRING)
        {
            bb_set(&object->apn, (unsigned char*)value->strVal->data, value->strVal->size);
        }
        else
        {
            bb_set2(&object->apn, value->byteArr, 0, bb_size(value->byteArr));
        }
    }
    else if (index == 3)
    {
        object->pinCode = value->uiVal;
    }
    else if (index == 4)
    {
        dlmsVARIANT* tmp;
        dlmsVARIANT* tmp3;
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_getByIndex(tmp->Arr, 0, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.precedence = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 1, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.delay = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 2, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.reliability = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 3, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.peakThroughput = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 4, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.meanThroughput = (unsigned char)var_toInteger(tmp3);

        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_getByIndex(tmp->Arr, 0, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.precedence = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 1, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.delay = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 2, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.reliability = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 3, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.peakThroughput = (unsigned char)var_toInteger(tmp3);
        ret = va_getByIndex(tmp->Arr, 4, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.meanThroughput = (unsigned char)var_toInteger(tmp3);
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
    int pos, ret = 0;
    gxCertificateInfo* it = NULL;
    dlmsVARIANT* tmp, * tmp3;
    switch (index)
    {
    case 2:
        object->securityPolicy = (DLMS_SECURITY_POLICY)var_toInteger(value);
        break;
    case 3:
        object->securitySuite = (DLMS_SECURITY_SUITE)var_toInteger(value);
        break;
    case 4:
        if ((ret = bb_clear(&object->clientSystemTitle)) == 0)
        {
            if (!(value->byteArr == NULL || bb_available(value->byteArr) != 8))
            {
                ret = bb_set2(&object->clientSystemTitle, value->byteArr, value->byteArr->position, 8);
            }
        }
        break;
    case 5:
        if ((ret = bb_clear(&object->serverSystemTitle)) == 0)
        {
            if (!(value->byteArr == NULL || bb_available(value->byteArr) != 8))
            {
                ret = bb_set2(&object->serverSystemTitle, value->byteArr, value->byteArr->position, 8);
            }
        }
        break;
    case 6:
        obj_clearCertificateInfo(&object->certificates);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it = (gxCertificateInfo*)gxmalloc(sizeof(gxCertificateInfo));
                if (it == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                //entity
                if ((ret = va_getByIndex(tmp->Arr, 0, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->entity = (DLMS_CERTIFICATE_ENTITY)var_toInteger(tmp3);
                //type
                if ((ret = va_getByIndex(tmp->Arr, 1, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->type = (DLMS_CERTIFICATE_TYPE)var_toInteger(tmp3);
                //serialNumber
                if ((ret = va_getByIndex(tmp->Arr, 2, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->serialNumber = gxmalloc(tmp3->byteArr->size + 1);
                    if (it->serialNumber == NULL)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    }
                    memcpy(it->serialNumber, tmp3->byteArr->data, tmp3->byteArr->size);
                    it->serialNumber[tmp3->byteArr->size] = 0;
                }
                else
                {
                    it->serialNumber = NULL;
                }
                //issuer
                if ((ret = va_getByIndex(tmp->Arr, 3, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->issuer = gxmalloc(tmp3->byteArr->size + 1);
                    if (it->issuer == NULL)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                        break;
                    }
                    memcpy(it->issuer, tmp3->byteArr->data, tmp3->byteArr->size);
                    it->issuer[tmp3->byteArr->size] = 0;
                }
                else
                {
                    it->issuer = NULL;
                }
                //subject
                if ((ret = va_getByIndex(tmp->Arr, 4, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->subject = gxmalloc(tmp3->byteArr->size + 1);
                    if (it->subject == NULL)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                        break;
                    }
                    memcpy(it->subject, tmp3->byteArr->data, tmp3->byteArr->size);
                    it->subject[tmp3->byteArr->size] = 0;
                }
                else
                {
                    it->subject = NULL;
                }
                //subjectAltName.
                if ((ret = va_getByIndex(tmp->Arr, 5, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->subjectAltName = gxmalloc(tmp3->byteArr->size + 1);
                    if (it->subjectAltName == NULL)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                        break;
                    }
                    memcpy(it->subjectAltName, tmp3->byteArr->data, tmp3->byteArr->size);
                    it->subjectAltName[tmp3->byteArr->size] = 0;
                }
                else
                {
                    it->subjectAltName = NULL;
                }
                arr_push(&object->certificates, it);
            }
            if (ret != 0 && it != NULL)
            {
                if (it->serialNumber != NULL)
                {
                    gxfree(it->serialNumber);
                }
                if (it->subject != NULL)
                {
                    gxfree(it->subject);
                }
                if (it->issuer != NULL)
                {
                    gxfree(it->issuer);
                }
                gxfree(it);
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
    if (index == 2)
    {
        object->communicationSpeed = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else if (index == 3)
    {
        object->windowSizeTransmit = (unsigned char)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->windowSizeReceive = (unsigned char)var_toInteger(value);
    }
    else if (index == 5)
    {
        object->maximumInfoLengthTransmit = (uint16_t)var_toInteger(value);
    }
    else if (index == 6)
    {
        object->maximumInfoLengthReceive = (uint16_t)var_toInteger(value);
    }
    else if (index == 7)
    {
        object->interCharachterTimeout = (uint16_t)var_toInteger(value);
    }
    else if (index == 8)
    {
        object->inactivityTimeout = (uint16_t)var_toInteger(value);
    }
    else if (index == 9)
    {
        object->deviceAddress = (uint16_t)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int cosem_setIecLocalPortSetup(gxLocalPortSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0;
    if (index == 2)
    {
        object->defaultMode = (DLMS_OPTICAL_PROTOCOL_MODE)var_toInteger(value);
    }
    else if (index == 3)
    {
        object->defaultBaudrate = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->proposedBaudrate = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else if (index == 5)
    {
        object->responseTime = (DLMS_LOCAL_PORT_RESPONSE_TIME)var_toInteger(value);
    }
    else if (index == 6)
    {
        bb_clear(&object->deviceAddress);
        ret = bb_set2(&object->deviceAddress, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 7)
    {
        bb_clear(&object->password1);
        ret = bb_set2(&object->password1, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 8)
    {
        bb_clear(&object->password2);
        ret = bb_set2(&object->password2, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 9)
    {
        bb_clear(&object->password5);
        ret = bb_set2(&object->password5, value->byteArr, 0, bb_size(value->byteArr));
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
    int pos, ret = 0;
    dlmsVARIANT* tmp;
    if (index == 2)
    {
        object->mode = var_toInteger(value);
    }
    else if (index == 3)
    {
        object->speed = var_toInteger(value);
    }
    else if (index == 4)
    {
        bb_clear(&object->primaryAddresses);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK ||
                    (ret = bb_setUInt8(&object->primaryAddresses, tmp->bVal)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        bb_clear(&object->tabis);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK ||
                    (ret = bb_setUInt8(&object->tabis, tmp->bVal)) != DLMS_ERROR_CODE_OK)
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
    int ret = 0, pos;
    dlmsVARIANT* tmp, * tmp3;
    gxip4SetupIpOption* ipItem = NULL;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, value->byteArr->data, &object->dataLinkLayer)) != 0)
        {
            return ret;
        }
#else
        ret = bb_get(value->byteArr, object->dataLinkLayerReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        object->ipAddress = var_toInteger(value);
    }
    else if (index == 4)
    {
        va_clear(&object->multicastIPAddress);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                tmp3 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));

                if ((ret = var_init(tmp3)) != 0 ||
                    (ret = var_copy(tmp, tmp3)) != 0 ||
                    (ret = va_push(&object->multicastIPAddress, tmp3)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        arr_clear(&object->ipOptions);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ipItem = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ipItem = (gxip4SetupIpOption*)gxmalloc(sizeof(gxip4SetupIpOption));
                if (ipItem == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                BYTE_BUFFER_INIT(&ipItem->data);
                ipItem->type = (DLMS_IP_OPTION_TYPE)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ipItem->length = (unsigned char)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 2, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                bb_set(&ipItem->data, tmp3->byteArr->data, tmp3->byteArr->size);
                arr_push(&object->ipOptions, ipItem);
            }
            if (ret != DLMS_ERROR_CODE_OK && ipItem != NULL)
            {
                gxfree(ipItem);
            }
        }
    }
    else if (index == 6)
    {
        object->subnetMask = var_toInteger(value);
    }
    else if (index == 7)
    {
        object->gatewayIPAddress = var_toInteger(value);
    }
    else if (index == 8)
    {
        object->useDHCP = value->boolVal;
    }
    else if (index == 9)
    {
        object->primaryDNSAddress = var_toInteger(value);
    }
    else if (index == 10)
    {
        object->secondaryDNSAddress = var_toInteger(value);
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
    int ret = 0, pos;
    dlmsVARIANT* tmp, * tmp3;
    IN6_ADDR* ip;
    gxNeighborDiscoverySetup* it;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, value->byteArr->data, &object->dataLinkLayer)) != 0)
        {
            return ret;
        }
#else
        ret = bb_get(value->byteArr, object->dataLinkLayerReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        object->addressConfigMode = var_toInteger(value);
    }
    else if (index == 4)
    {
        arr_clear(&object->unicastIPAddress);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (tmp->byteArr->size != 16)
                {
                    ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    break;
                }
                ip = (IN6_ADDR*)gxmalloc(sizeof(IN6_ADDR));
                memcpy(ip, tmp->byteArr->data, tmp->byteArr->size);
                if ((ret = arr_push(&object->unicastIPAddress, ip)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 5)
    {
        arr_clear(&object->multicastIPAddress);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (tmp->byteArr->size != 16)
                {
                    ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    break;
                }
                ip = (IN6_ADDR*)gxmalloc(sizeof(IN6_ADDR));
                memcpy(ip, tmp->byteArr->data, tmp->byteArr->size);
                if ((ret = arr_push(&object->multicastIPAddress, ip)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 6)
    {
        arr_clear(&object->gatewayIPAddress);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (tmp->byteArr->size != 16)
                {
                    ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
                    break;
                }
                ip = (IN6_ADDR*)gxmalloc(sizeof(IN6_ADDR));
                memcpy(ip, tmp->byteArr->data, tmp->byteArr->size);
                if ((ret = arr_push(&object->gatewayIPAddress, ip)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (index == 7)
    {
        if (bb_size(value->byteArr) == 0)
        {
            memset(&object->primaryDNSAddress, 0, sizeof(IN6_ADDR));
        }
        else if (bb_size(value->byteArr) != 16)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            memcpy(&object->primaryDNSAddress, value->byteArr->data, value->byteArr->size);
        }
    }
    else if (index == 8)
    {
        if (bb_size(value->byteArr) == 0)
        {
            memset(&object->secondaryDNSAddress, 0, sizeof(IN6_ADDR));
        }
        else if (bb_size(value->byteArr) != 16)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_CLASS_OR_OBJECT;
        }
        else
        {
            memcpy(&object->secondaryDNSAddress, value->byteArr->data, value->byteArr->size);
        }
    }
    else if (index == 9)
    {
        object->trafficClass = (DLMS_IP6_ADDRESS_CONFIG_MODE)var_toInteger(value);
    }
    else if (index == 10)
    {
        arr_clear(&object->neighborDiscoverySetup);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it = (gxNeighborDiscoverySetup*)gxmalloc(sizeof(gxNeighborDiscoverySetup));
                if (it == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                if ((ret = va_getByIndex(tmp->Arr, 0, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->maxRetry = (unsigned char)var_toInteger(tmp3);
                if ((ret = va_getByIndex(tmp->Arr, 1, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->retryWaitTime = (uint16_t)var_toInteger(tmp3);
                if ((ret = va_getByIndex(tmp->Arr, 2, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->sendPeriod = var_toInteger(tmp3);
                if ((ret = arr_push(&object->neighborDiscoverySetup, it)) != 0)
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
    if (index == 2)
    {
        object->tableId = (uint16_t)var_toInteger(value);
    }
    else if (index == 3)
    {
        //Skip length.
    }
    else if (index == 4)
    {
        bb_clear(&object->buffer);
        bb_set2(&object->buffer, value->byteArr, 0, bb_size(value->byteArr));
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int cosem_setMbusSlavePortSetup(gxMbusSlavePortSetup* object, unsigned char index, dlmsVARIANT* value)
{
    if (index == 2)
    {
        object->defaultBaud = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else if (index == 3)
    {
        object->availableBaud = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->addressState = (DLMS_ADDRESS_STATE)var_toInteger(value);
    }
    else if (index == 5)
    {
        object->busAddress = (unsigned char)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int cosem_setDisconnectControl(gxDisconnectControl* object, unsigned char index, dlmsVARIANT* value)
{
    if (index == 2)
    {
        object->outputState = value->boolVal;
    }
    else if (index == 3)
    {
        object->controlState = (DLMS_CONTROL_STATE)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->controlMode = (DLMS_CONTROL_MODE)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int cosem_setLimiter(dlmsSettings* settings, gxLimiter* object, unsigned char index, dlmsVARIANT* value)
{
    DLMS_OBJECT_TYPE ot;
    int ret = DLMS_ERROR_CODE_OK, pos;
    dlmsVARIANT* tmp, * tmp3;
    dlmsVARIANT tmp2;
    if (index == 2)
    {
        if (value->vt != DLMS_DATA_TYPE_STRUCTURE)
        {
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ot = (DLMS_OBJECT_TYPE)var_toInteger(tmp);
        //Get LN.
        ret = va_getByIndex(value->Arr, 1, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        //Get attribute index.
        ret = va_getByIndex(value->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->selectedAttributeIndex = (unsigned char)var_toInteger(tmp);
        if (ot != 0)
        {
            if ((ret = oa_findByLN(&settings->objects, ot, tmp3->byteArr->data, &object->monitoredValue)) == 0 &&
                object->monitoredValue == NULL)
            {
                if ((ret = cosem_createObject(ot, &object->monitoredValue)) != 0)
                {
                    return ret;
                }
                memcpy(object->monitoredValue->logicalName, tmp3->byteArr->data, tmp3->byteArr->size);
                //Add object to released objects list.
                ret = oa_push(&settings->releasedObjects, object->monitoredValue);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
            }
        }
        else
        {
            object->monitoredValue = NULL;
        }
    }
    else if (index == 3)
    {
        ret = var_copy(&object->thresholdActive, value);
    }
    else if (index == 4)
    {
        ret = var_copy(&object->thresholdNormal, value);
    }
    else if (index == 5)
    {
        ret = var_copy(&object->thresholdEmergency, value);
    }
    else if (index == 6)
    {
        object->minOverThresholdDuration = var_toInteger(value);
    }
    else if (index == 7)
    {
        object->minUnderThresholdDuration = var_toInteger(value);
    }
    else if (index == 8)
    {
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->emergencyProfile.id = (uint16_t)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = var_init(&tmp2);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &tmp2);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        time_copy(&object->emergencyProfile.activationTime, tmp2.dateTime);
        var_clear(&tmp2);
        ret = va_getByIndex(value->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->emergencyProfile.duration = var_toInteger(tmp);
    }
    else if (index == 9)
    {
        va_clear(&object->emergencyProfileGroupIDs);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                tmp3 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                var_init(tmp3);
                ret = var_copy(tmp3, tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                va_push(&object->emergencyProfileGroupIDs, tmp3);
            }
        }
    }
    else if (index == 10)
    {
        object->emergencyProfileActive = value->boolVal;
    }
    else if (index == 11)
    {
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_getByIndex(tmp->Arr, 0, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, tmp3->byteArr->data, (gxObject**)&object->actionOverThreshold.script)) != 0)
        {
            return ret;
        }
        if (object->actionOverThreshold.script == NULL)
        {
            if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, (gxObject**)&object->actionOverThreshold.script)) != 0)
            {
                return ret;
            }
            memcpy(object->actionOverThreshold.script->base.logicalName, tmp3->byteArr->data, tmp3->byteArr->size);
            //Add object to released objects list.
            ret = oa_push(&settings->releasedObjects, (gxObject*)object->actionOverThreshold.script);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
        }
#else
        memcpy(object->actionOverThreshold.logicalName, tmp3->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        ret = va_getByIndex(tmp->Arr, 1, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->actionOverThreshold.scriptSelector = (uint16_t)var_toInteger(tmp3);

        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_getByIndex(tmp->Arr, 0, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, tmp3->byteArr->data, (gxObject**)&object->actionUnderThreshold.script)) != 0)
        {
            return ret;
        }
        if (object->actionUnderThreshold.script == NULL)
        {
            if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, (gxObject**)&object->actionUnderThreshold.script)) != 0)
            {
                return ret;
            }
            memcpy(object->actionUnderThreshold.script->base.logicalName, tmp3->byteArr->data, tmp3->byteArr->size);
            //Add object to released objects list.
            ret = oa_push(&settings->releasedObjects, (gxObject*)object->actionUnderThreshold.script);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
        }
#else
        memcpy(object->actionUnderThreshold.logicalName, tmp3->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        ret = va_getByIndex(tmp->Arr, 1, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->actionUnderThreshold.scriptSelector = (uint16_t)var_toInteger(tmp3);
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
    int ret = DLMS_ERROR_CODE_OK, pos;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP, value->byteArr->data, &object->mBusPort)) != 0)
        {
            return ret;
        }
        if (object->mBusPort == NULL)
        {
            if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP, &object->mBusPort)) != 0)
            {
                return ret;
            }
            memcpy(object->mBusPort->logicalName, value->byteArr->data, value->byteArr->size);
            //Add object to released objects list.
            ret = oa_push(&settings->releasedObjects, object->mBusPort);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
        }
#else
        ret = bb_get(value->byteArr, object->mBusPortReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        arr_clearKeyValuePair(&object->captureDefinition);
        dlmsVARIANT* tmp, * tmp3;
        gxByteBuffer* start, * end;
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                start = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                end = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(start);
                BYTE_BUFFER_INIT(end);
                bb_set(start, tmp3->byteArr->data, tmp3->byteArr->size);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(start);
                    gxfree(end);
                    break;
                }
                bb_set(end, tmp3->byteArr->data, tmp3->byteArr->size);
                arr_push(&object->captureDefinition, key_init(start, end));
            }
        }
    }
    else if (index == 4)
    {
        object->capturePeriod = var_toInteger(value);
    }
    else if (index == 5)
    {
        object->primaryAddress = (unsigned char)var_toInteger(value);
    }
    else if (index == 6)
    {
        object->identificationNumber = var_toInteger(value);
    }
    else if (index == 7)
    {
        object->manufacturerID = (uint16_t)var_toInteger(value);
    }
    else if (index == 8)
    {
        object->dataHeaderVersion = (unsigned char)var_toInteger(value);
    }
    else if (index == 9)
    {
        object->deviceType = (unsigned char)var_toInteger(value);
    }
    else if (index == 10)
    {
        object->accessNumber = (unsigned char)var_toInteger(value);
    }
    else if (index == 11)
    {
        object->status = (unsigned char)var_toInteger(value);
    }
    else if (index == 12)
    {
        object->alarm = (unsigned char)var_toInteger(value);
    }
    else if (index == 13 && object->base.version != 0)
    {
        object->configuration = (uint16_t)var_toInteger(value);
    }
    else if (index == 14 && object->base.version != 0)
    {
        object->encryptionKeyStatus = (DLMS_MBUS_ENCRYPTION_KEY_STATUS)var_toInteger(value);
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
    int ret = 0, pos;
    gxModemInitialisation* modemInit;
    dlmsVARIANT* tmp, * tmp3;
    gxByteBuffer* str;
    if (index == 2)
    {
        object->communicationSpeed = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else if (index == 3)
    {
        obj_clearModemConfigurationInitialisationStrings(&object->initialisationStrings);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                modemInit = (gxModemInitialisation*)gxmalloc(sizeof(gxModemInitialisation));
                if (modemInit == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                BYTE_BUFFER_INIT(&modemInit->request);
                BYTE_BUFFER_INIT(&modemInit->response);
                bb_set(&modemInit->request, tmp3->byteArr->data, tmp3->byteArr->size);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                bb_set(&modemInit->response, tmp3->byteArr->data, tmp3->byteArr->size);
                if (tmp->Arr->size > 2)
                {
                    ret = va_getByIndex(tmp->Arr, 2, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    modemInit->delay = tmp3->uiVal;
                }
                arr_push(&object->initialisationStrings, modemInit);
            }
        }
    }
    else if (index == 4)
    {
        arr_clearStrings(&object->modemProfile);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                str = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(str);
                bb_set2(str, tmp->byteArr, 0, bb_size(tmp->byteArr));
                arr_push(&object->modemProfile, str);
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
int cosem_setPppSetup(dlmsSettings* settings, gxPppSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxpppSetupLcpOption* lcpItem = NULL;
    gxpppSetupIPCPOption* ipcpItem = NULL;
    dlmsVARIANT* tmp;
    dlmsVARIANT* tmp3;
    if (index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, value->byteArr->data, &object->phy)) != 0)
        {
            return ret;
        }
        if (object->phy == NULL)
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
        }
#else
        ret = bb_get(value->byteArr, object->PHYReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (index == 3)
    {
        arr_clear(&object->lcpOptions);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                lcpItem = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                lcpItem = (gxpppSetupLcpOption*)gxmalloc(sizeof(gxpppSetupLcpOption));
                if (lcpItem == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                var_init(&lcpItem->data);
                lcpItem->type = (DLMS_PPP_SETUP_LCP_OPTION_TYPE)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                lcpItem->length = (unsigned char)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 2, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = var_copy(&lcpItem->data, tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                arr_push(&object->lcpOptions, lcpItem);
            }
            if (ret != 0 && lcpItem != NULL)
            {
                gxfree(lcpItem);
            }
        }
    }
    else if (index == 4)
    {
        arr_clear(&object->ipcpOptions);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ipcpItem = (gxpppSetupIPCPOption*)gxmalloc(sizeof(gxpppSetupIPCPOption));
                if (ipcpItem == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                var_init(&ipcpItem->data);
                ipcpItem->type = (DLMS_PPP_SETUP_IPCP_OPTION_TYPE)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ipcpItem->length = (unsigned char)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 2, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = var_copy(&ipcpItem->data, tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                arr_push(&object->ipcpOptions, ipcpItem);
            }
            if (ret != 0 && ipcpItem != NULL)
            {
                gxfree(ipcpItem);
            }
        }
    }
    else if (index == 5)
    {
        if (value->Arr == NULL || value->Arr->size == 0)
        {
            bb_clear(&object->userName);
            bb_clear(&object->password);
        }
        else if (value->Arr->size == 2)
        {
            //Get user name.
            ret = va_getByIndex(value->Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            bb_clear(&object->userName);
            bb_set2(&object->userName, tmp->byteArr, 0, bb_size(tmp->byteArr));
            //Get password.
            ret = va_getByIndex(value->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            bb_clear(&object->password);
            bb_set2(&object->password, tmp->byteArr, 0, bb_size(tmp->byteArr));
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
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
int cosem_setRegisterActivation(dlmsSettings* settings,
    gxValueEventArg* e)
{
    int ret = 0, pos;
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
        obj_clearRegisterActivationAssignment(&object->registerAssignment);
        dlmsVARIANT* tmp, * tmp3;
        if (value->Arr != NULL)
        {
            short type;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                objectDefinition = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                type = (short)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                unsigned char* ln = tmp3->byteArr->data;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                objectDefinition = (gxObjectDefinition*)gxmalloc(sizeof(gxObjectDefinition));
                objectDefinition->objectType = (DLMS_OBJECT_TYPE)type;
                memcpy(objectDefinition->logicalName, ln, 6);
                arr_push(&object->registerAssignment, objectDefinition);
#else
                if (type != 0)
                {
                    if ((ret = cosem_findObjectByLN(settings, type, ln, &objectDefinition)) != 0)
                    {
                        break;
                    }
                    oa_push(&object->registerAssignment, objectDefinition);
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
#ifdef DLMS_IGNORE_OBJECT_POINTERS
            if (ret != 0 && objectDefinition != NULL)
            {
                gxfree(objectDefinition);
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    else if (index == 3)
    {
        obj_clearRegisterActivationMaskList(&object->maskList);
        int pos2;
        gxByteBuffer* start = NULL, * end = NULL;
        dlmsVARIANT* tmp, * tmp3;
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                start = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(start);
                end = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(end);
                bb_set(start, tmp3->byteArr->data, tmp3->byteArr->size);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                for (pos2 = 0; pos2 != tmp3->Arr->size; ++pos2)
                {
                    if ((ret = va_getByIndex(tmp3->Arr, pos2, &tmp)) != 0 ||
                        (ret = bb_setUInt8(end, (unsigned char)var_toInteger(tmp))) != 0)
                    {
                        break;
                    }
                }
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                arr_push(&object->maskList, key_init(start, end));
            }
            if (ret != 0)
            {
                if (start != NULL)
                {
                    gxfree(start);
                }
                if (end != NULL)
                {
                    gxfree(end);
                }
            }
        }
    }
    else if (index == 4)
    {
        bb_clear(&object->activeMask);
        ret = bb_set2(&object->activeMask, value->byteArr, 0, bb_size(value->byteArr));
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
    int ret = 0, pos;
    gxActionSet* actionSet = NULL;
    dlmsVARIANT* tmp;
    dlmsVARIANT* tmp3, * tmp4;
    if (index == 2)
    {
        va_clear(&object->thresholds);
        if (value->Arr != NULL)
        {
            va_capacity(&object->thresholds, value->Arr->size);
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                tmp3 = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                tmp3 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                if (tmp3 == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                var_init(tmp3);
                ret = var_copy(tmp3, tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                va_push(&object->thresholds, tmp3);
            }
        }
    }
    else if (index == 3)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        signed short type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        type = (short)var_toInteger(tmp);
#else
        object->monitoredValue.objectType = (DLMS_OBJECT_TYPE)var_toInteger(tmp);
#endif //DLMS_IGNORE_OBJECT_POINTERS

        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, type, tmp->byteArr->data, &object->monitoredValue.target)) != 0)
        {
            return ret;
        }
        if (object->monitoredValue.target == NULL)
        {
            ret = cosem_createObject(type, &object->monitoredValue.target);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            ret = cosem_setLogicalName(object->monitoredValue.target, tmp->byteArr->data);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //Add object to released objects list.
            ret = oa_push(&settings->releasedObjects, object->monitoredValue.target);
        }
#else
        memcpy(object->monitoredValue.logicalName, tmp->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        ret = va_getByIndex(value->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->monitoredValue.attributeIndex = (unsigned char)var_toInteger(tmp);
    }
    else if (index == 4)
    {
        obj_clearRegisterMonitorActions(&object->actions);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                actionSet = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp4);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                actionSet = (gxActionSet*)gxmalloc(sizeof(gxActionSet));
                //Update action up.
                ret = va_getByIndex(tmp4->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp3->Arr, 0, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                actionSet->actionUp.script = NULL;
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, tmp->byteArr->data, (gxObject**)&actionSet->actionUp.script)) != 0)
                {
                    return ret;
                }
                if (actionSet->actionUp.script == NULL)
                {
                    ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, (gxObject**)&actionSet->actionUp.script);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = cosem_setLogicalName((gxObject*)actionSet->actionUp.script, tmp->byteArr->data);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, (gxObject*)actionSet->actionUp.script);
                }
#else
                memcpy(actionSet->actionUp.logicalName, tmp->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
                ret = va_getByIndex(tmp3->Arr, 1, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                actionSet->actionUp.scriptSelector = (uint16_t)var_toInteger(tmp);
                //Update action down.
                ret = va_getByIndex(tmp4->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp3->Arr, 0, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                actionSet->actionDown.script = NULL;
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, tmp->byteArr->data, (gxObject**)&actionSet->actionDown.script)) != 0)
                {
                    return ret;
                }
                if (actionSet->actionDown.script == NULL)
                {
                    ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, (gxObject**)&actionSet->actionDown.script);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = cosem_setLogicalName((gxObject*)actionSet->actionDown.script, tmp->byteArr->data);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, (gxObject*)actionSet->actionDown.script);
                }
#else
                memcpy(actionSet->actionDown.logicalName, tmp->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
                ret = va_getByIndex(tmp3->Arr, 1, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                actionSet->actionDown.scriptSelector = (uint16_t)var_toInteger(tmp);
                arr_push(&object->actions, actionSet);
            }
            if (ret != 0 && actionSet != NULL)
            {
                gxfree(actionSet);
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
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxSapItem* it = NULL;
    if (index == 2)
    {
        obj_clearSapList(&object->sapAssignmentList);
        dlmsVARIANT* tmp, * tmp2;
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                it = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it = (gxSapItem*)gxmalloc(sizeof(gxSapItem));
                if (it == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                BYTE_BUFFER_INIT(&it->name);
                it->id = (uint16_t)var_toInteger(tmp2);
                ret = va_getByIndex(tmp->Arr, 1, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                bb_set(&it->name, tmp2->byteArr->data, tmp2->byteArr->size);
                arr_push(&object->sapAssignmentList, it);
            }
            if (ret != 0 && it != NULL)
            {
                gxfree(it);
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
    int ret = 0, pos;
    if (index == 2)
    {
        obj_clearScheduleEntries(&object->entries);
        dlmsVARIANT* tmp, * it;
        dlmsVARIANT tmp3;
        var_init(&tmp3);
        if (value->Arr != NULL)
        {
            se = NULL;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                se = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                se = (gxScheduleEntry*)gxmalloc(sizeof(gxScheduleEntry));
                if (se == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                se->execWeekdays = 0;
                ba_init(&se->execSpecDays);
                ret = va_getByIndex(tmp->Arr, 0, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                se->index = (uint16_t)var_toInteger(it);
                ret = va_getByIndex(tmp->Arr, 1, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                se->enable = (unsigned char)var_toInteger(it);

                ret = va_getByIndex(tmp->Arr, 2, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, it->byteArr->data, (gxObject**)&se->scriptTable)) != 0)
                {
                    break;
                }
                if (se->scriptTable == NULL)
                {
                    ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, (gxObject**)&se->scriptTable);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = cosem_setLogicalName((gxObject*)se->scriptTable, tmp->byteArr->data);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, (gxObject*)se->scriptTable);
                }
#else
                memcpy(se->logicalName, it->byteArr->data, bb_size(it->byteArr));
#endif //DLMS_IGNORE_OBJECT_POINTERS
                ret = va_getByIndex(tmp->Arr, 3, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                se->scriptSelector = (uint16_t)var_toInteger(it);
                ret = va_getByIndex(tmp->Arr, 4, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_TIME, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                time_copy(&se->switchTime, tmp3.dateTime);
                ret = va_getByIndex(tmp->Arr, 5, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                se->validityWindow = (uint16_t)var_toInteger(it);

                ret = va_getByIndex(tmp->Arr, 6, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (it->bitArr != NULL)
                {
                    uint32_t val;
                    if ((ret = ba_toInteger(it->bitArr, &val)) == 0)
                    {
                        se->execWeekdays = val;
                    }
                }
                ret = va_getByIndex(tmp->Arr, 7, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if (it->bitArr != NULL)
                {
                    ba_copy(&se->execSpecDays, it->bitArr->data, (uint16_t)it->bitArr->size);
                }
                ret = va_getByIndex(tmp->Arr, 8, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                time_copy(&se->beginDate, tmp3.dateTime);
                var_clear(&tmp3);
                ret = va_getByIndex(tmp->Arr, 9, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                time_copy(&se->endDate, tmp3.dateTime);
                arr_push(&object->entries, se);
                var_clear(&tmp3);
            }
            if (ret != 0 && se != NULL)
            {
                gxfree(se);
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
    int ret = 0, pos, pos2;
    gxScriptAction* scriptAction;
    gxScript* script;
    if (index == 2)
    {
        obj_clearScriptTable(&object->scripts);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
        dlmsVARIANT* tmp, * tmp2, * tmp3;
        if (value->Arr->size != 0)
        {
            script = NULL;
            ret = va_getByIndex(value->Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //Fix Xemex bug here.
            //Xemex meters do not return array as they should be according standard.
            if (value->vt == DLMS_DATA_TYPE_ARRAY)
            {
                for (pos = 0; pos != value->Arr->size; ++pos)
                {
                    script = NULL;
                    ret = va_getByIndex(value->Arr, pos, &tmp);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    script = (gxScript*)gxmalloc(sizeof(gxScript));
                    if (script == NULL)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                        break;
                    }
                    arr_init(&script->actions);
                    script->id = (uint16_t)var_toInteger(tmp3);
                    arr_push(&object->scripts, script);
                    ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    scriptAction = NULL;
                    for (pos2 = 0; pos2 != tmp3->Arr->size; ++pos2)
                    {
                        scriptAction = NULL;
                        ret = va_getByIndex(tmp3->Arr, pos2, &tmp2);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            break;
                        }
                        ret = va_getByIndex(tmp2->Arr, 0, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            break;
                        }
                        scriptAction = (gxScriptAction*)gxmalloc(sizeof(gxScriptAction));
                        if (scriptAction == NULL)
                        {
                            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                            break;
                        }
                        var_init(&scriptAction->parameter);
                        scriptAction->type = (DLMS_SCRIPT_ACTION_TYPE)var_toInteger(tmp);
                        ret = va_getByIndex(tmp2->Arr, 1, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            break;
                        }
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                        scriptAction->objectType = (DLMS_OBJECT_TYPE)var_toInteger(tmp);
#else
                        type = (DLMS_OBJECT_TYPE)var_toInteger(tmp);
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        ret = va_getByIndex(tmp2->Arr, 2, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            break;
                        }
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                        if (bb_size(tmp->byteArr) == 6)
                        {
                            memcpy(scriptAction->logicalName, tmp->byteArr->data, 6);
                        }
#else
                        if ((ret = oa_findByLN(&settings->objects, type, tmp->byteArr->data, &scriptAction->target)) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        ret = va_getByIndex(tmp2->Arr, 3, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            break;
                        }
                        scriptAction->index = (char)var_toInteger(tmp);
                        ret = va_getByIndex(tmp2->Arr, 4, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            break;
                        }
                        var_copy(&scriptAction->parameter, tmp);
                        arr_push(&script->actions, scriptAction);
                    }
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        if (scriptAction != NULL)
                        {
                            gxfree(scriptAction);
                        }
                        break;
                    }
                }
                if (ret != 0 && script != NULL)
                {
                    gxfree(script);
                }
            }
            else //Read Xemex meter here.
            {
                ret = va_getByIndex(value->Arr, 0, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 1, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp2->Arr, 2, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                script = (gxScript*)gxmalloc(sizeof(gxScript));
                if (script == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                arr_init(&script->actions);
                script->id = (uint16_t)var_toInteger(tmp3);
                arr_push(&object->scripts, script);
                ret = va_getByIndex(tmp->Arr, 3, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                scriptAction = (gxScriptAction*)gxmalloc(sizeof(gxScriptAction));
                if (scriptAction == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                var_init(&scriptAction->parameter);
                scriptAction->type = (DLMS_SCRIPT_ACTION_TYPE)var_toInteger(tmp3);
                ret = va_getByIndex(tmp2->Arr, 4, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

#ifdef DLMS_IGNORE_OBJECT_POINTERS
                scriptAction->objectType = (DLMS_OBJECT_TYPE)var_toInteger(tmp3);
#else
                type = (uint16_t)var_toInteger(tmp3);
#endif //DLMS_IGNORE_OBJECT_POINTERS
                ret = va_getByIndex(tmp2->Arr, 5, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                memcpy(scriptAction->logicalName, tmp3->byteArr->data, 6);
#else
                if ((ret = oa_findByLN(&settings->objects, type, tmp3->byteArr->data, &scriptAction->target)) != 0)
                {
                    return ret;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                ret = va_getByIndex(tmp2->Arr, 6, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                scriptAction->index = (char)var_toInteger(tmp3);
                ret = va_getByIndex(tmp2->Arr, 7, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                var_copy(&scriptAction->parameter, tmp);
                arr_push(&script->actions, scriptAction);
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
    int ret = 0, pos;
    gxSpecialDay* specialDay;
    if (index == 2)
    {
        arr_clear(&object->entries);
        dlmsVARIANT* tmp, * tmp3;
        dlmsVARIANT tmp2;
        if (value->Arr != NULL)
        {
            specialDay = NULL;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                specialDay = NULL;
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                specialDay = (gxSpecialDay*)gxmalloc(sizeof(gxSpecialDay));
                if (specialDay == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                specialDay->index = (uint16_t)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                var_init(&tmp2);
                ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATE, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                time_copy(&specialDay->date, tmp2.dateTime);
                var_clear(&tmp2);
                ret = va_getByIndex(tmp->Arr, 2, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                specialDay->dayId = (unsigned char)var_toInteger(tmp3);
                arr_push(&object->entries, specialDay);
            }
            if (ret != 0 && specialDay != NULL)
            {
                gxfree(specialDay);
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
    int ret = 0;
    if (index == 2)
    {
        object->port = (uint16_t)var_toInteger(value);
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
        }
        else
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
            if ((ret = bb_get(value->byteArr, ln, 6)) != 0 ||
                (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_IP4_SETUP, ln, &object->ipSetup)) != 0)
            {

            }
#else
            ret = bb_get(value->byteArr, object->ipReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    else if (index == 4)
    {
        object->maximumSegmentSize = (uint16_t)var_toInteger(value);
    }
    else if (index == 5)
    {
        object->maximumSimultaneousConnections = (unsigned char)var_toInteger(value);
    }
    else if (index == 6)
    {
        object->inactivityTimeout = (uint16_t)var_toInteger(value);
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
    if (index == 2)
    {
        object->commSpeed = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

#ifndef DLMS_IGNORE_PUSH_SETUP
int cosem_setPushSetup(dlmsSettings* settings, gxPushSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    gxTarget* it;
    gxObject* obj;
    dlmsVARIANT* tmp, * tmp3;
    gxtime* s, * e;
    if (index == 2)
    {
        DLMS_OBJECT_TYPE type = DLMS_OBJECT_TYPE_NONE;
        obj_clearPushObjectList(&object->pushObjectList);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                type = (DLMS_OBJECT_TYPE)var_toInteger(tmp3);
                //Get LN.

                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                obj = NULL;
                if ((ret = oa_findByLN(&settings->objects, type, tmp3->byteArr->data, &obj)) != 0)
                {
                    return ret;
                }
                if (obj == NULL)
                {
                    if ((ret = cosem_createObject(type, &obj)) != 0)
                    {
                        return ret;
                    }
                    oa_push(&settings->releasedObjects, obj);
                    memcpy(obj->logicalName, tmp3->byteArr->data, tmp3->byteArr->size);
                }
                it = (gxTarget*)gxmalloc(sizeof(gxTarget));
                if (it == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 2, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->attributeIndex = (unsigned char)var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 3, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->dataIndex = (unsigned char)var_toInteger(tmp3);
                arr_push(&object->pushObjectList, key_init(obj, it));
            }
        }
    }
    else if (index == 3)
    {
        bb_clear(&object->destination);
        //Get service.
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->service = (DLMS_SERVICE_TYPE)var_toInteger(tmp);
        //Destination.
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (tmp->byteArr != NULL && tmp->byteArr->size != 0)
        {
            bb_clear(&object->destination);
            bb_set(&object->destination, tmp->byteArr->data, tmp->byteArr->size);
        }
        //Message.
        ret = va_getByIndex(value->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->message = (DLMS_MESSAGE_TYPE)var_toInteger(tmp);
    }
    else if (index == 4)
    {
        arr_clearKeyValuePair(&object->communicationWindow);
        if (value->Arr != NULL)
        {
            dlmsVARIANT start, end;
            var_init(&start);
            var_init(&end);
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != 0 ||
                    (ret = va_getByIndex(tmp->Arr, 0, &tmp3)) != 0 ||
                    (ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &start)) != 0 ||
                    (ret = va_getByIndex(tmp->Arr, 1, &tmp3)) != 0 ||
                    (ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &end)) != 0)
                {
                    break;
                }
                s = (gxtime*)gxmalloc(sizeof(gxtime));
                e = (gxtime*)gxmalloc(sizeof(gxtime));
                time_copy(s, start.dateTime);
                time_copy(e, end.dateTime);
                arr_push(&object->communicationWindow, key_init(s, e));
            }
            var_clear(&start);
            var_clear(&end);
        }
    }
    else if (index == 5)
    {
        object->randomisationStartInterval = (uint16_t)var_toInteger(value);
    }
    else if (index == 6)
    {
        object->numberOfRetries = (unsigned char)var_toInteger(value);
    }
    else if (index == 7)
    {
        object->repetitionDelay = (uint16_t)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
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
    signed short type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    dlmsVARIANT* it, * it2, * tmp;
    //charge per unit scaling
    ret = va_getByIndex(value->Arr, 0, &it);
    if (ret != 0)
    {
        return ret;
    }
    //commodity scale
    ret = va_getByIndex(it->Arr, 0, &it2);
    if (ret != 0)
    {
        return ret;
    }
    target->chargePerUnitScaling.commodityScale = (char)var_toInteger(it2);
    //price scale
    ret = va_getByIndex(it->Arr, 1, &it2);
    if (ret != 0)
    {
        return ret;
    }
    target->chargePerUnitScaling.priceScale = (char)var_toInteger(it2);
    //commodity
    ret = va_getByIndex(value->Arr, 1, &it);
    if (ret != 0)
    {
        return ret;
    }
    //type
    ret = va_getByIndex(it->Arr, 0, &it2);
    if (ret != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    type = (DLMS_OBJECT_TYPE)var_toInteger(it2);
#else
    target->commodity.type = (DLMS_OBJECT_TYPE)var_toInteger(it2);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    //LN
    ret = va_getByIndex(it->Arr, 1, &it2);
    if (ret != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (bb_size(it2->byteArr) == 6)
    {
        if ((ret = oa_findByLN(&settings->objects, type, it2->byteArr->data, &target->commodity.target)) != 0)
        {
            return ret;
        }
    }
    else
    {
        target->commodity.target = NULL;
    }
#else
    memset(target->commodity.logicalName, 0, 6);
    if (it2->byteArr != NULL && it2->byteArr->size == 6)
    {
        memcpy(target->commodity.logicalName, it2->byteArr->data, 6);
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    //attribute index
    ret = va_getByIndex(it->Arr, 2, &it2);
    if (ret != 0)
    {
        return ret;
    }
    target->commodity.attributeIndex = (unsigned char)var_toInteger(it2);
    //chargeTables
    obj_clearChargeTables(&target->chargeTables);
    ret = va_getByIndex(value->Arr, 2, &it);
    if (ret != 0)
    {
        return ret;
    }
    for (pos = 0; pos != it->Arr->size; ++pos)
    {
        ret = va_getByIndex(it->Arr, pos, &it2);
        if (ret != 0)
        {
            break;
        }
        ct = (gxChargeTable*)gxmalloc(sizeof(gxChargeTable));
        if (ct == NULL)
        {
            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
            break;
        }
        arr_push(&target->chargeTables, ct);
        BYTE_BUFFER_INIT(&ct->index);
        //index
        ret = va_getByIndex(it2->Arr, 0, &tmp);
        if (ret != 0)
        {
            break;
        }
        bb_set2(&ct->index, tmp->byteArr, 0, bb_size(tmp->byteArr));
        //chargePerUnit
        ret = va_getByIndex(it2->Arr, 1, &tmp);
        if (ret != 0)
        {
            break;
        }
        ct->chargePerUnit = (short)var_toInteger(tmp);
    }
    return ret;
}

int cosem_setCharge(dlmsSettings* settings, gxCharge* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0;
    dlmsVARIANT tmp;
    if (index == 2)
    {
        object->totalAmountPaid = (short)var_toInteger(value);
    }
    else if (index == 3)
    {
        object->chargeType = (unsigned char)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->priority = (unsigned char)var_toInteger(value);
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
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tmp);
            if ((ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp)) != 0)
            {
                var_clear(&tmp);
                return ret;
            }
            time_copy(&object->unitChargeActivationTime, tmp.dateTime);
            var_clear(&tmp);
        }
        else
        {
            time_clear(&object->unitChargeActivationTime);
        }
    }
    else if (index == 8)
    {
        object->period = var_toInteger(value);
    }
    else if (index == 9)
    {
        object->chargeConfiguration = var_toInteger(value);
    }
    else if (index == 10)
    {
        if (value->vt == DLMS_DATA_TYPE_DATETIME)
        {
            object->lastCollectionTime = *value->dateTime;
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else if (index == 11)
    {
        object->lastCollectionAmount = var_toInteger(value);
    }
    else if (index == 12)
    {
        object->totalAmountRemaining = var_toInteger(value);
    }
    else if (index == 13)
    {
        object->proportion = (uint16_t)var_toInteger(value);
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
    int ret = 0;
    dlmsVARIANT tmp;
    if (index == 2)
    {
        object->currentCreditAmount = var_toInteger(value);
    }
    else if (index == 3)
    {
        object->type = (unsigned char)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->priority = (unsigned char)var_toInteger(value);
    }
    else if (index == 5)
    {
        object->warningThreshold = var_toInteger(value);
    }
    else if (index == 6)
    {
        object->limit = var_toInteger(value);
    }
    else if (index == 7)
    {
        object->creditConfiguration = var_toInteger(value);
    }
    else if (index == 8)
    {
        object->status = (unsigned char)var_toInteger(value);
    }
    else if (index == 9)
    {
        object->presetCreditAmount = var_toInteger(value);
    }
    else if (index == 10)
    {
        object->creditAvailableThreshold = var_toInteger(value);
    }
    else if (index == 11)
    {
        var_init(&tmp);
        ret = dlms_changeType(value->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp);
        if (ret == 0)
        {
            time_copy(&object->period, tmp.dateTime);
        }
        var_clear(&tmp);
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
    int ret = 0, pos;
    unsigned char* ba;
    dlmsVARIANT* it;
    dlmsVARIANT tmp;
    dlmsVARIANT* tmp2;
    gxCreditChargeConfiguration* ccc;
    gxTokenGatewayConfiguration* gwc;
    if (index == 2)
    {
        //payment mode
        ret = va_getByIndex(value->Arr, 0, &it);
        if (ret != 0)
        {
            return ret;
        }
        object->paymentMode = (DLMS_ACCOUNT_PAYMENT_MODE)var_toInteger(it);
        //account status
        ret = va_getByIndex(value->Arr, 1, &it);
        if (ret != 0)
        {
            return ret;
        }
        object->accountStatus = (DLMS_ACCOUNT_STATUS)var_toInteger(it);
    }
    else if (index == 3)
    {
        object->currentCreditInUse = (unsigned char)var_toInteger(value);
    }
    else if (index == 4)
    {
        if (value->bitArr == NULL || value->bitArr->size == 0)
        {
            return DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        else
        {
            object->currentCreditStatus = (DLMS_ACCOUNT_CREDIT_STATUS)var_toInteger(value);
        }
    }
    else if (index == 5)
    {
        object->availableCredit = var_toInteger(value);
    }
    else if (index == 6)
    {
        object->amountToClear = var_toInteger(value);
    }
    else if (index == 7)
    {
        object->clearanceThreshold = var_toInteger(value);
    }
    else if (index == 8)
    {
        object->aggregatedDebt = var_toInteger(value);
    }
    else if (index == 9)
    {
        arr_clear(&object->creditReferences);
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &it);
            if (ret != 0)
            {
                return ret;
            }
            if (it->byteArr == NULL || it->byteArr->size != 6)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            ba = (unsigned char*)gxmalloc(6);
            memcpy(ba, it->byteArr->data, 6);
            arr_push(&object->creditReferences, ba);
        }
    }
    else if (index == 10)
    {
        arr_clear(&object->chargeReferences);
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &it);
            if (ret != 0)
            {
                return ret;
            }
            if (it->byteArr == NULL || it->byteArr->size != 6)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            ba = (unsigned char*)gxmalloc(6);
            memcpy(ba, it->byteArr->data, 6);
            arr_push(&object->chargeReferences, ba);
        }
    }
    else if (index == 11)
    {
        ccc = NULL;
        obj_clearCreditChargeConfigurations(&object->creditChargeConfigurations);
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &it);
            if (ret != 0)
            {
                break;
            }
            ccc = (gxCreditChargeConfiguration*)gxmalloc(sizeof(gxCreditChargeConfiguration));
            if (ccc == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
            //credit reference
            ret = va_getByIndex(it->Arr, 0, &tmp2);
            if (ret != 0)
            {
                break;
            }
            memcpy(ccc->creditReference, tmp2->byteArr->data, tmp2->byteArr->size);
            //charge reference
            ret = va_getByIndex(it->Arr, 1, &tmp2);
            if (ret != 0)
            {
                break;
            }
            memcpy(ccc->chargeReference, tmp2->byteArr->data, tmp2->byteArr->size);
            //collection configuration
            ret = va_getByIndex(it->Arr, 2, &tmp2);
            if (ret != 0)
            {
                break;
            }
            ccc->collectionConfiguration = (DLMS_CREDIT_COLLECTION_CONFIGURATION)var_toInteger(tmp2);
            arr_push(&object->creditChargeConfigurations, ccc);
        }
        if (ret != 0 && ccc != NULL)
        {
            gxfree(ccc);
        }
    }
    else if (index == 12)
    {
        obj_clearTokenGatewayConfigurations(&object->tokenGatewayConfigurations);
        gwc = NULL;
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            gwc = NULL;
            if ((ret = va_getByIndex(value->Arr, pos, &it)) != 0)
            {
                break;
            }
            gwc = (gxTokenGatewayConfiguration*)gxmalloc(sizeof(gxTokenGatewayConfiguration));
            if (gwc == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
            memset(gwc->creditReference, 0, 6);
            //credit reference
            if ((ret = va_getByIndex(it->Arr, 0, &tmp2)) != 0)
            {
                return ret;
            }
            if (tmp2->byteArr->size == 6)
            {
                memcpy(gwc->creditReference, tmp2->byteArr->data, tmp2->byteArr->size);
            }
            //token proportion
            if ((ret = va_getByIndex(it->Arr, 1, &tmp2)) != 0)
            {
                break;
            }
            gwc->tokenProportion = (unsigned char)var_toInteger(tmp2);
            arr_push(&object->tokenGatewayConfigurations, gwc);
        }
        if (ret != 0 && gwc != NULL)
        {
            gxfree(gwc);
        }
    }
    else if (index == 13)
    {
        var_init(&tmp);
        ret = dlms_changeType(value->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp);
        if (ret == 0)
        {
            time_copy(&object->accountActivationTime, tmp.dateTime);
        }
        var_clear(&tmp);
    }
    else if (index == 14)
    {
        var_init(&tmp);
        ret = dlms_changeType(value->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp);
        if (ret == 0)
        {
            time_copy(&object->accountClosureTime, tmp.dateTime);
        }
        var_clear(&tmp);
    }
    else if (index == 15)
    {
        bb_clear(&object->currency.name);
        //Name
        ret = va_getByIndex(value->Arr, 0, &it);
        if (ret != 0)
        {
            return ret;
        }
        if (it->strVal != NULL && it->strVal->size != 0)
        {
            bb_set2(&object->currency.name, it->strVal, 0, bb_size(it->strVal));
        }
        //scale
        ret = va_getByIndex(value->Arr, 1, &it);
        if (ret != 0)
        {
            return ret;
        }
        object->currency.scale = (char)var_toInteger(it);
        //unit
        ret = va_getByIndex(value->Arr, 2, &it);
        if (ret != 0)
        {
            return ret;
        }
        object->currency.unit = (DLMS_CURRENCY)var_toInteger(it);
    }
    else if (index == 16)
    {
        object->lowCreditThreshold = var_toInteger(value);
    }
    else if (index == 17)
    {
        object->nextCreditAvailableThreshold = var_toInteger(value);
    }
    else if (index == 18)
    {
        object->maxProvision = (uint16_t)var_toInteger(value);
    }
    else if (index == 19)
    {
        object->maxProvisionPeriod = var_toInteger(value);
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
    int pos, ret = 0;
    gxImageActivateInfo* item;
    dlmsVARIANT* it, * tmp;
    if (index == 2)
    {
        object->imageBlockSize = var_toInteger(value);
    }
    else if (index == 3)
    {
        ba_clear(&object->imageTransferredBlocksStatus);
        if (value->bitArr != NULL)
        {
            ba_copy(&object->imageTransferredBlocksStatus, value->bitArr->data, (uint16_t)value->bitArr->size);
        }
    }
    else if (index == 4)
    {
        object->imageFirstNotTransferredBlockNumber = var_toInteger(value);
    }
    else if (index == 5)
    {
        object->imageTransferEnabled = var_toInteger(value) == 0 ? 0 : 1;
    }
    else if (index == 6)
    {
        object->imageTransferStatus = (DLMS_IMAGE_TRANSFER_STATUS)var_toInteger(value);
    }
    else if (index == 7)
    {
        arr_clear(&object->imageActivateInfo);
        item = NULL;
        if (value->Arr != NULL)
        {
            item = NULL;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &it);
                if (ret != 0)
                {
                    break;
                }
                item = (gxImageActivateInfo*)gxmalloc(sizeof(gxImageActivateInfo));
                if (item == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                BYTE_BUFFER_INIT(&item->identification);
                BYTE_BUFFER_INIT(&item->signature);
                ret = va_getByIndex(it->Arr, 0, &tmp);
                if (ret != 0)
                {
                    break;
                }
                item->size = var_toInteger(tmp);
                ret = va_getByIndex(it->Arr, 1, &tmp);
                if (ret != 0)
                {
                    break;
                }
                bb_set2(&item->identification, tmp->byteArr, 0, bb_size(tmp->byteArr));
                ret = va_getByIndex(it->Arr, 2, &tmp);
                if (ret != 0)
                {
                    break;
                }
                bb_set2(&item->signature, tmp->byteArr, 0, bb_size(tmp->byteArr));
                arr_push(&object->imageActivateInfo, item);
            }
            if (ret != 0 && item != NULL)
            {
                gxfree(item);
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
    gxObject* obj;
    DLMS_OBJECT_TYPE type;
    dlmsVARIANT* tmp, * tmp2;
    gxTarget* co;
    int pos, ret;
    if ((ret = obj_clearProfileGenericCaptureObjects(objects)) != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    arr_capacity(objects, value->Arr->size);
    if (value->Arr != NULL)
    {
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if (tmp->Arr->size != 4)
            {
                //Invalid structure format.
                ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                break;
            }
            ret = va_getByIndex(tmp->Arr, 0, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            type = (DLMS_OBJECT_TYPE)var_toInteger(tmp2);
            //Get LN.
            ret = va_getByIndex(tmp->Arr, 1, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            ret = oa_findByLN(&settings->objects, type, tmp2->byteArr->data, &obj);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if (obj == NULL)
            {
                ret = cosem_createObject(type, &obj);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = cosem_setLogicalName(obj, tmp2->byteArr->data);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                //Add object to released objects list.
                ret = oa_push(&settings->releasedObjects, obj);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
            }
            co = (gxTarget*)gxmalloc(sizeof(gxTarget));
            if (co == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
            ret = va_getByIndex(tmp->Arr, 2, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            co->attributeIndex = (unsigned char)var_toInteger(tmp2);
            ret = va_getByIndex(tmp->Arr, 3, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            co->dataIndex = (unsigned char)var_toInteger(tmp2);
            arr_push(objects, key_init(obj, co));
        }
    }
    //Trim array.
    arr_capacity(objects, objects->size);
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
    int ret = 0;
    uint16_t pos, pos2;
    DLMS_OBJECT_TYPE type;
    dlmsVARIANT* tmp, * row, * data;
    variantArray* va;
    if (index == 2)
    {
        static unsigned char UNIX_TIME[6] = { 0, 0, 1, 1, 0, 255 };
        if (object->captureObjects.size == 0)
        {
            //Read capture objects first.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if ((ret = obj_clearProfileGenericBuffer(&object->buffer)) == 0)
        {
            if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
            {
                uint16_t rIndex, rCount = arr_getCapacity(&object->buffer);
                dlmsVARIANT* row;
                if ((ret = cosem_checkArray(value->byteArr, &rCount)) == 0)
                {
                    object->buffer.size = rCount;
                    if (rCount != 0 && (ret = arr_getByIndex(&object->buffer, 0, (void**)&row)) == 0)
                    {
                        uint16_t cCount = row->Arr->size;
                        if (cCount > object->captureObjects.size)
                        {
                            //Number of columns do not match.
                            return DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                        for (rIndex = 0; rIndex != rCount; ++rIndex)
                        {
                            if ((ret = arr_getByIndex(&object->buffer, rIndex, (void**)&row)) != 0 ||
                                (ret = cosem_checkStructure(value->byteArr, (unsigned char)cCount)) != 0)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (value->vt == DLMS_DATA_TYPE_ARRAY)
        {
            gxtime lastDate;
            time_initUnix(&lastDate, 0);
            //Allocate array.
            arr_capacity(&object->buffer, value->Arr->size);
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &row);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                if (row->Arr->size != object->captureObjects.size)
                {
                    //Number of columns do not match.
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                for (pos2 = 0; pos2 < row->Arr->size; ++pos2)
                {
                    ret = va_getByIndex(row->Arr, pos2, &data);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    if (data->vt == DLMS_DATA_TYPE_NONE || data->vt == DLMS_DATA_TYPE_OCTET_STRING || data->vt == DLMS_DATA_TYPE_UINT32)
                    {
                        gxObject* obj;
                        gxTarget* t;
                        gxKey* k;
                        if ((ret = arr_getByIndex(&object->captureObjects, pos2, (void**)&k)) != 0)
                        {
                            return ret;
                        }
                        obj = (gxObject*)k->key;
                        t = (gxTarget*)k->value;
                        if (obj->objectType == DLMS_OBJECT_TYPE_CLOCK && t->attributeIndex == 2)
                        {
                            if (data->vt == DLMS_DATA_TYPE_OCTET_STRING)
                            {
                                dlmsVARIANT tmp3;
                                var_init(&tmp3);
                                var_attach(&tmp3, data->byteArr);
                                ret = dlms_changeType(tmp3.byteArr, DLMS_DATA_TYPE_DATETIME, data);
                                var_clear(&tmp3);
                                if (ret != 0)
                                {
                                    return ret;
                                }
                                lastDate = *data->dateTime;
                            }
                            //Some meters returns NULL date time to save bytes.
                            else if (data->vt == DLMS_DATA_TYPE_NONE)
                            {
                                if (object->sortMethod == DLMS_SORT_METHOD_FIFO || object->sortMethod == DLMS_SORT_METHOD_SMALLEST)
                                {
                                    time_addSeconds(&lastDate, object->capturePeriod);
                                }
                                else
                                {
                                    int tm = object->capturePeriod;
                                    time_addSeconds(&lastDate, -tm);
                                }
                                var_setDateTime(data, &lastDate);
                            }
                        }
                        else if (data->vt == DLMS_DATA_TYPE_UINT32 && obj->objectType == DLMS_OBJECT_TYPE_DATA && t->attributeIndex == 2 &&
                            memcmp(obj->logicalName, UNIX_TIME, 6) == 0)
                        {
                            gxtime tmp4;
                            time_initUnix(&tmp4, data->ulVal);
                            var_setDateTime(data, &tmp4);
                            lastDate = *data->dateTime;
                        }
                    }
                }
                //Attach rows from parser.
                va = (variantArray*)gxmalloc(sizeof(variantArray));
                va_init(va);
                va_attach2(va, row->Arr);
                arr_push(&object->buffer, va);
            }
        }
        if (settings->server)
        {
            object->entriesInUse = object->buffer.size;
        }
        //Trim array.
        arr_capacity(&object->buffer, object->buffer.size);
    }
    else if (index == 3)
    {
        object->entriesInUse = 0;
        ret = obj_clearProfileGenericBuffer(&object->buffer);
        if (ret == DLMS_ERROR_CODE_OK)
        {
            ret = setCaptureObjects(settings, &object->captureObjects, value);
        }
    }
    else if (index == 4)
    {
        object->capturePeriod = var_toInteger(value);
    }
    else if (index == 5)
    {
        object->sortMethod = (DLMS_SORT_METHOD)var_toInteger(value);
    }
    else if (index == 6)
    {
        if (value->vt == DLMS_DATA_TYPE_NONE)
        {
            object->sortObject = NULL;
        }
        else
        {
            if (value->Arr == NULL || value->Arr->size != 4)
            {
                //Invalid structure format.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            ret = va_getByIndex(value->Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            type = (DLMS_OBJECT_TYPE)var_toInteger(tmp);

            ret = va_getByIndex(value->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }

            object->sortObject = NULL;
            ret = oa_findByLN(&settings->objects, type, tmp->byteArr->data, &object->sortObject);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            if (object->sortObject == NULL)
            {
                ret = cosem_createObject(type, &object->sortObject);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = cosem_setLogicalName(object->sortObject, tmp->byteArr->data);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                //Add object to released objects list.
                ret = oa_push(&settings->releasedObjects, object->sortObject);
            }
            ret = va_getByIndex(value->Arr, 2, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->sortObjectAttributeIndex = (char)var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 3, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->sortObjectDataIndex = (uint16_t)var_toInteger(tmp);
        }
    }
    else if (index == 7)
    {
        object->entriesInUse = var_toInteger(value);
    }
    else if (index == 8)
    {
        object->profileEntries = var_toInteger(value);
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
    int ret = 0, pos;
    dlmsVARIANT tmp2;
    dlmsVARIANT* tmp, * it;
    gxAdjacentCell* ac;
    switch (index)
    {
    case 2:
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            if (object->operatorName != NULL)
            {
                gxfree(object->operatorName);
                object->operatorName = NULL;
            }

            if (value->byteArr != NULL && bb_size(value->byteArr) != 0)
            {
                object->operatorName = (char*)gxmalloc(value->byteArr->size + 1);
                if (object->operatorName == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(object->operatorName, value->strVal, value->byteArr->size);
                object->operatorName[value->byteArr->size] = '\0';
            }
        }
        else if (value->vt == DLMS_DATA_TYPE_STRING)
        {
            if (object->operatorName != NULL)
            {
                gxfree(object->operatorName);
                object->operatorName = NULL;
            }
            object->operatorName = NULL;
            if (value->strVal != NULL && bb_size(value->strVal) != 0)
            {
                object->operatorName = (char*)gxmalloc(value->strVal->size + 1);
                if (object->operatorName == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                memcpy(object->operatorName, value->strVal, value->strVal->size);
                object->operatorName[value->strVal->size] = '\0';
            }
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        break;
    case 3:
        object->status = (DLMS_GSM_STATUS)var_toInteger(value);
        break;
    case 4:
        object->circuitSwitchStatus = (DLMS_GSM_CIRCUIT_SWITCH_STATUS)var_toInteger(value);
        break;
    case 5:
        object->packetSwitchStatus = (DLMS_GSM_PACKET_SWITCH_STATUS)var_toInteger(value);
        break;
    case 6:
        if (value != NULL && value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            if ((ret = va_getByIndex(value->Arr, 0, &tmp)) != 0)
            {
                return ret;
            }
            object->cellInfo.cellId = (uint32_t)var_toInteger(tmp);
            if ((ret = va_getByIndex(value->Arr, 1, &tmp)) != 0)
            {
                return ret;
            }
            object->cellInfo.locationId = (uint16_t)var_toInteger(tmp);
            if ((ret = va_getByIndex(value->Arr, 2, &tmp)) != 0)
            {
                return ret;
            }
            object->cellInfo.signalQuality = (unsigned char)var_toInteger(tmp);
            if ((ret = va_getByIndex(value->Arr, 3, &tmp)) != 0)
            {
                return ret;
            }
            object->cellInfo.ber = (unsigned char)var_toInteger(tmp);
            if (object->base.version != 0)
            {
                if ((ret = va_getByIndex(value->Arr, 4, &tmp)) != 0)
                {
                    return ret;
                }
                object->cellInfo.mobileCountryCode = (uint16_t)var_toInteger(tmp);

                if ((ret = va_getByIndex(value->Arr, 5, &tmp)) != 0)
                {
                    return ret;
                }
                object->cellInfo.mobileNetworkCode = (uint16_t)var_toInteger(tmp);

                if ((ret = va_getByIndex(value->Arr, 6, &tmp)) != 0)
                {
                    return ret;
                }
                object->cellInfo.channelNumber = tmp->ulVal;
            }
        }
        break;
    case 7:
        arr_clear(&object->adjacentCells);
        if (value != NULL && value->vt == DLMS_DATA_TYPE_ARRAY)
        {
            ac = NULL;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ac = NULL;
                ret = va_getByIndex(value->Arr, pos, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ret = va_getByIndex(it->Arr, 0, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ac = (gxAdjacentCell*)gxmalloc(sizeof(gxAdjacentCell));
                if (ac == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                ac->cellId = var_toInteger(tmp);
                ret = va_getByIndex(it->Arr, 1, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ac->signalQuality = (unsigned char)var_toInteger(tmp);
                arr_push(&object->adjacentCells, ac);
            }
            if (ret != 0 && ac != NULL)
            {
                gxfree(ac);
            }
        }
        break;
    case 8:
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tmp2);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp2);
            if (ret != 0)
            {
                return ret;
            }
            time_copy(&object->captureTime, tmp2.dateTime);
            var_clear(&tmp2);
        }
        else if (value->vt == DLMS_DATA_TYPE_DATETIME)
        {
            time_copy(&object->captureTime, value->dateTime);
        }
        else
        {
            time_clear(&object->captureTime);
        }
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
    int ret = DLMS_ERROR_CODE_OK, pos;
    dlmsVARIANT* tmp;
    dlmsVARIANT tmp2;
    dlmsVARIANT* it;
    switch (index)
    {
    case 2:
        bb_clear(&object->token);
        ret = bb_set2(&object->token, value->byteArr, 0, bb_size(value->byteArr));
        break;
    case 3:
        time_clear(&object->time);
        if (value->byteArr == NULL)
        {
            time_clear(&object->time);
        }
        else
        {
            var_init(&tmp2);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp2);
            if (ret != 0)
            {
                return ret;
            }
            time_copy(&object->time, tmp2.dateTime);
            var_clear(&tmp2);
        }
        break;
    case 4:
        obj_clearByteBufferList(&object->descriptions);
        if (value != NULL)
        {
            gxByteBuffer* d;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &it)) != 0)
                {
                    return ret;
                }
                d = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (d == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                BYTE_BUFFER_INIT(d);
                bb_set2(d, it->strVal, 0, it->strVal->size);
                arr_push(&object->descriptions, d);
            }
        }
        break;
    case 5:
        object->deliveryMethod = (DLMS_TOKEN_DELIVERY)var_toInteger(value);
        break;
    case 6:
        if ((ret = va_getByIndex(value->Arr, 0, &tmp)) != 0)
        {
            return ret;
        }
        object->status = (DLMS_TOKEN_STATUS_CODE)var_toInteger(tmp);
        if ((ret = va_getByIndex(value->Arr, 1, &tmp)) != 0)
        {
            return ret;
        }
        ba_clear(&object->dataValue);
        if (tmp->strVal != NULL && tmp->strVal->size != 0)
        {
            ba_copy(&object->dataValue, tmp->bitArr->data, tmp->bitArr->size);
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
    int ret;
    uint16_t pos;
    gxByteBuffer tmp;
    gxValueEventCollection args;
    gxValueEventArg e;
    ve_init(&e);
    gxKey* kv;
    bb_clear(&object->buffer);
    bb_clear(&object->templateDescription);
    e.action = 1;
    e.target = &object->base;
    e.index = 2;
    vec_init(&args);
    BYTE_BUFFER_INIT(&tmp);
    vec_push(&args, &e);
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
            ret = arr_getByIndex(&object->captureObjects, pos, (void**)&kv);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&object->buffer);
                return ret;
            }
            e.target = (gxObject*)kv->key;
            e.index = ((gxTarget*)kv->value)->attributeIndex;
            if ((ret = cosem_getValue(settings, &e)) != 0)
            {
                var_clear(&e.value);
                bb_clear(&object->buffer);
                return ret;
            }
            if (e.byteArray)
            {
                if (bb_size(e.value.byteArr) == 0)
                {
                    bb_setUInt8(&object->templateDescription, 0);
                }
                else
                {
                    if (e.value.byteArr->data[0] == DLMS_DATA_TYPE_ARRAY ||
                        e.value.byteArr->data[0] == DLMS_DATA_TYPE_STRUCTURE)
                    {
                        gxDataInfo info;
                        dlmsVARIANT value;
                        uint16_t count;
                        di_init(&info);
                        var_init(&value);
                        e.value.byteArr->position = 1;
                        if ((ret = hlp_getObjectCount2(e.value.byteArr, &count)) != 0 ||
                            ((gxTarget*)kv->value)->dataIndex > count)
                        {
                            var_clear(&e.value);
                            bb_clear(&object->buffer);
                            return DLMS_ERROR_CODE_OUTOFMEMORY;
                        }
                        //If all data is captured.
                        if (((gxTarget*)kv->value)->dataIndex == 0)
                        {
                            bb_setUInt8(&object->templateDescription, e.value.byteArr->data[0]);
                            if (e.value.byteArr->data[0] == DLMS_DATA_TYPE_ARRAY)
                            {
                                bb_setUInt16(&object->templateDescription, e.value.byteArr->data[1]);
                            }
                            else
                            {
                                bb_setUInt8(&object->templateDescription, e.value.byteArr->data[1]);
                            }
                            for (unsigned char pos = 0; pos < count; ++pos)
                            {
                                di_init(&info);
                                var_clear(&value);
                                if ((ret = dlms_getData(e.value.byteArr, &info, &value)) != 0)
                                {
                                    var_clear(&value);
                                    var_clear(&e.value);
                                    bb_clear(&object->buffer);
                                    return ret;
                                }
                                if (info.type == DLMS_DATA_TYPE_STRUCTURE || info.type == DLMS_DATA_TYPE_ARRAY)
                                {
                                    dlmsVARIANT* value2;
                                    bb_setUInt8(&object->templateDescription, info.type);
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
                                if (e.value.byteArr->data[0] == DLMS_DATA_TYPE_ARRAY)
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            for (unsigned char pos = 0; pos < ((gxTarget*)kv->value)->dataIndex; ++pos)
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
                        bb_setUInt8(&object->templateDescription, e.value.byteArr->data[0]);
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
    int ret = DLMS_ERROR_CODE_OK;
    switch (index)
    {
    case 2:
        bb_clear(&object->buffer);
        if (value->byteArr != NULL)
        {
            ret = bb_set(&object->buffer, value->byteArr->data, value->byteArr->size);
        }
        break;
    case 3:
        ret = setCaptureObjects(settings, &object->captureObjects, value);
        if (ret == 0 && settings->server)
        {
            ret = compactData_updateTemplateDescription(settings, object);
        }
        break;
    case 4:
        object->templateId = (unsigned char)var_toInteger(value);
        break;
    case 5:
        bb_clear(&object->templateDescription);
        if (value->byteArr != NULL)
        {
            ret = bb_set(&object->templateDescription, value->byteArr->data, value->byteArr->size);
        }
        break;
    case 6:
        object->captureMethod = (DLMS_CAPTURE_METHOD)var_toInteger(value);
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
    int ret = 0, pos;
    dlmsVARIANT* tmp, * tmp3;
    dlmsVARIANT tmp2;
    DLMS_OBJECT_TYPE type;
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
        if (value->Arr != NULL)
        {
            if ((ret = va_getByIndex(value->Arr, 0, &tmp3)) == DLMS_ERROR_CODE_OK)
            {
                type = (DLMS_OBJECT_TYPE)var_toInteger(tmp3);
                //Get LN.
                if ((ret = va_getByIndex(value->Arr, 1, &tmp3)) == DLMS_ERROR_CODE_OK)
                {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    if ((ret = oa_findByLN(&settings->objects, type, tmp3->byteArr->data, &object->changedParameter.target)) != 0)
                    {
                        return ret;
                    }
                    if (object->changedParameter.target == NULL)
                    {
                        if ((ret = cosem_createObject(type, &object->changedParameter.target)) != 0)
                        {
                            return ret;
                        }
                        oa_push(&settings->releasedObjects, object->changedParameter.target);
                        memcpy(object->changedParameter.target->logicalName, tmp3->byteArr->data, tmp3->byteArr->size);
                    }
                    if ((ret = va_getByIndex(value->Arr, 2, &tmp3)) == DLMS_ERROR_CODE_OK)
                    {
                        object->changedParameter.attributeIndex = (unsigned char)var_toInteger(tmp3);
                        if ((ret = va_getByIndex(value->Arr, 3, &tmp3)) == DLMS_ERROR_CODE_OK)
                        {
                            ret = var_copy(&object->changedParameter.value, tmp3);
                        }
                    }
#else
                    object->changedParameter.type = type;
                    memcpy(object->changedParameter.logicalName, tmp3->byteArr->data, 6);
                    if ((ret = va_getByIndex(value->Arr, 2, &tmp3)) == DLMS_ERROR_CODE_OK)
                    {
                        object->changedParameter.attributeIndex = (unsigned char)var_toInteger(tmp3);
                        if ((ret = va_getByIndex(value->Arr, 3, &tmp3)) == DLMS_ERROR_CODE_OK)
                        {
                            ret = var_copy(&object->changedParameter.value, tmp3);
                        }
                    }
#endif //DLMS_IGNORE_OBJECT_POINTERS

                }
            }
        }
        break;
    }
    case 3:
    {
        //Some meters are returning empty octet string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            ret = var_init(&tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            time_copy(&object->captureTime, tmp2.dateTime);
            var_clear(&tmp2);
        }
        else
        {
            time_clear(&object->captureTime);
        }
    }
    break;
    case 4:
    {
        obj_clearParametersList(&object->parameters);
        if (value->Arr != NULL)
        {
            gxTarget* it;
            gxObject* obj;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp->Arr, 0, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                type = (DLMS_OBJECT_TYPE)var_toInteger(tmp3);
                //Get LN.

                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                obj = NULL;
                if ((ret = oa_findByLN(&settings->objects, type, tmp3->byteArr->data, &obj)) != 0)
                {
                    return ret;
                }
                if (obj == NULL)
                {
                    if ((ret = cosem_createObject(type, &obj)) != 0)
                    {
                        return ret;
                    }
                    oa_push(&settings->releasedObjects, obj);
                    memcpy(obj->logicalName, tmp3->byteArr->data, tmp3->byteArr->size);
                }
                it = (gxTarget*)gxmalloc(sizeof(gxTarget));
                if (it == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                ret = va_getByIndex(tmp->Arr, 2, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->attributeIndex = (unsigned char)var_toInteger(tmp3);
                arr_push(&object->parameters, key_init(obj, it));
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
    int ret = 0;
    switch (index)
    {
    case 2:
        object->serviceNodeAddress = (uint16_t)var_toInteger(value);
        break;
    case 3:
        object->baseNodeAddress = (uint16_t)var_toInteger(value);
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
    int ret = 0;
    switch (index)
    {
    case 2:
        object->crcIncorrectCount = (uint16_t)var_toInteger(value);
        break;
    case 3:
        object->crcFailedCount = (uint16_t)var_toInteger(value);
        break;
    case 4:
        object->txDropCount = (uint16_t)var_toInteger(value);
        break;
    case 5:
        object->rxDropCount = (uint16_t)var_toInteger(value);
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
    int ret = 0;
    switch (index)
    {
    case 2:
        object->macMinSwitchSearchTime = (unsigned char)var_toInteger(value);
        break;
    case 3:
        object->macMaxPromotionPdu = (unsigned char)var_toInteger(value);
        break;
    case 4:
        object->macPromotionPduTxPeriod = (unsigned char)var_toInteger(value);
        break;
    case 5:
        object->macBeaconsPerFrame = (unsigned char)var_toInteger(value);
        break;
    case 6:
        object->macScpMaxTxAttempts = (unsigned char)var_toInteger(value);
        break;
    case 7:
        object->macCtlReTxTimer = (unsigned char)var_toInteger(value);
        break;
    case 8:
        object->macMaxCtlReTx = (unsigned char)var_toInteger(value);
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
    int ret = 0;
    switch (index)
    {
    case 2:
        object->lnId = (short)var_toInteger(value);
        break;
    case 3:
        object->lsId = (unsigned char)var_toInteger(value);
        break;
    case 4:
        object->sId = (unsigned char)var_toInteger(value);
        break;
    case 5:
        object->sna.size = 0;
        ret = bb_set2(&object->sna, value->byteArr, 0, bb_size(value->byteArr));
        break;
    case 6:
        object->state = (DLMS_MAC_STATE)var_toInteger(value);
        break;
    case 7:
        object->scpLength = (short)var_toInteger(value);
        break;
    case 8:
        object->nodeHierarchyLevel = (unsigned char)var_toInteger(value);
        break;
    case 9:
        object->beaconSlotCount = (unsigned char)var_toInteger(value);
        break;
    case 10:
        object->beaconRxSlot = (unsigned char)var_toInteger(value);
        break;
    case 11:
        object->beaconTxSlot = (unsigned char)var_toInteger(value);
        break;
    case 12:
        object->beaconRxFrequency = (unsigned char)var_toInteger(value);
        break;
    case 13:
        object->beaconTxFrequency = (unsigned char)var_toInteger(value);
        break;
    case 14:
        object->capabilities = (DLMS_MAC_CAPABILITIES)var_toInteger(value);
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
    int ret = 0;
    switch (index)
    {
    case 2:
        object->txDataPktCount = var_toInteger(value);
        break;
    case 3:
        object->rxDataPktCount = var_toInteger(value);
        break;
    case 4:
        object->txCtrlPktCount = var_toInteger(value);
        break;
    case 5:
        object->rxCtrlPktCount = var_toInteger(value);
        break;
    case 6:
        object->csmaFailCount = var_toInteger(value);
        break;
    case 7:
        object->csmaChBusyCount = var_toInteger(value);
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
    int ret = 0;
    int pos;
    gxMacMulticastEntry* it;
    arr_clear(&object->multicastEntries);
    if (value->Arr != NULL)
    {
        dlmsVARIANT* tmp;
        dlmsVARIANT* tmp2;
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it = (gxMacMulticastEntry*)gxmalloc(sizeof(gxMacMulticastEntry));
            if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->id = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->members = (short)var_toInteger(tmp2);
            arr_push(&object->multicastEntries, it);
        }
    }
    return 0;
}


int cosem_setSwitchTable(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    arr_empty(&object->switchTable);
    if (value->Arr != NULL)
    {
        dlmsVARIANT* it;
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            if ((ret = va_getByIndex(value->Arr, pos, &it)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            uint16_t* tmp = (uint16_t*)gxmalloc(sizeof(uint16_t));
            *tmp = (uint16_t)var_toInteger(it);
            arr_push(&object->switchTable, tmp);
        }
    }
    return 0;
}

int cosem_setDirectTable(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    gxMacDirectTable* it;
    arr_clear(&object->directTable);
    if (value->Arr != NULL)
    {
        dlmsVARIANT* tmp;
        dlmsVARIANT* tmp2;
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it = (gxMacDirectTable*)gxmalloc(sizeof(gxMacDirectTable));
            if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->sourceSId = (short)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->sourceLnId = (short)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 2, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->sourceLcId = (short)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 3, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->destinationSId = (short)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 4, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->destinationLnId = (short)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 5, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->destinationLcId = (short)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 6, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if ((ret = bb_get(tmp2->byteArr, it->did, sizeof(it->did))) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            arr_push(&object->directTable, it);
        }
    }
    return 0;
}

int cosem_setAvailableSwitches(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    gxMacAvailableSwitch* it;
    obj_clearAvailableSwitches(&object->availableSwitches);
    if (value->Arr != NULL)
    {
        dlmsVARIANT* tmp;
        dlmsVARIANT* tmp2;
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it = (gxMacAvailableSwitch*)gxmalloc(sizeof(gxMacAvailableSwitch));
            BYTE_BUFFER_INIT(&it->sna);
            bb_capacity(&it->sna, tmp2->byteArr->size);
            if ((ret = bb_set(&it->sna, tmp2->byteArr->data, tmp2->byteArr->size)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            it->lsId = (short)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 2, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            it->level = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 3, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            it->rxLevel = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 4, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            it->rxSnr = (signed char)var_toInteger(tmp2);
            arr_push(&object->availableSwitches, it);
        }
    }
    return 0;
}

int cosem_setCommunications(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    gxMacPhyCommunication* it;
    arr_clear(&object->communications);
    if (value->Arr != NULL)
    {
        dlmsVARIANT* tmp;
        dlmsVARIANT* tmp2;
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_getByIndex(value->Arr, pos, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it = (gxMacPhyCommunication*)gxmalloc(sizeof(gxMacPhyCommunication));
            if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if ((ret = bb_get(tmp2->byteArr, it->eui, sizeof(it->eui))) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->txPower = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 2, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->txCoding = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 3, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->rxCoding = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 4, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->rxLvl = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 5, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->snr = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 6, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->txPowerModified = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 7, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->txCodingModified = (signed char)var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 8, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->rxCodingModified = (signed char)var_toInteger(tmp2);
            arr_push(&object->communications, it);
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
    int ret = 0;
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
    int ret = 0;
    switch (index)
    {
    case 2:
    {
        bb_clear(&object->firmwareVersion);
        bb_set2(&object->firmwareVersion, value->byteArr, 0, bb_size(value->byteArr));
    }
    break;
    case 3:
        object->vendorId = (uint16_t)var_toInteger(value);
        break;
    case 4:
        object->productId = (uint16_t)var_toInteger(value);
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
    int pos, ret = 0;
    gxActionItem* it;
    bitArray* ba;
    dlmsVARIANT* tmp, * tmp2;
    switch (index)
    {
    case 2:
    {
        arr_clear(&object->actions);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                it = (gxActionItem*)gxmalloc(sizeof(gxActionItem));
                it->script = NULL;
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, tmp2->byteArr->data, (gxObject**)&it->script)) != 0)
                {
                    break;
                }
                if (it->script == NULL)
                {
                    if ((ret = cosem_createObject(DLMS_OBJECT_TYPE_SCRIPT_TABLE, (gxObject**)&it->script)) != 0)
                    {
                        return ret;
                    }
                    memcpy(it->script->base.logicalName, tmp2->byteArr->data, tmp2->byteArr->size);
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, &it->script->base);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                }
#else
                memcpy(it->scriptLogicalName, tmp2->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->scriptSelector = (uint16_t)var_toInteger(tmp2);
                arr_push(&object->actions, it);
            }
        }
    }
    break;
    case 3:
    {
        obj_clearBitArrayList(&object->permissionsTable);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                ba = (bitArray*)gxmalloc(sizeof(bitArray));
                ba_init(ba);
                if ((ba_copy(ba, tmp->bitArr->data, tmp->bitArr->size)) != 0)
                {
                    break;
                }
                arr_push(&object->permissionsTable, ba);
            }
        }
    }
    break;
    case 4:
    {
        arr_clear(&object->weightingsTable);
        if (value->Arr != NULL)
        {
            dlmsVARIANT* tmp2;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                tmp2 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                var_init(tmp2);
                ret = var_copy(tmp2, tmp);
                if (ret != 0)
                {
                    break;
                }
                arr_push(&object->weightingsTable, tmp2);
            }
        }
    }
    break;
    case 5:
    {
        obj_clearBitArrayList(&object->mostRecentRequestsTable);
        if (value->Arr != NULL)
        {
            bitArray* it;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it = (bitArray*)gxmalloc(sizeof(bitArray));
                ba_init(it);
                if ((ba_copy(it, tmp->bitArr->data, tmp->bitArr->size)) != 0)
                {
                    break;
                }
                arr_push(&object->mostRecentRequestsTable, it);
            }
        }
    }
    break;
    case 6:
        object->lastOutcome = (unsigned char)var_toInteger(value);
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
    int ret = DLMS_ERROR_CODE_OK;
    switch (index)
    {
    case 2:
        object->maximumOctetsUiPdu = (uint16_t)var_toInteger(value);
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
    int ret = DLMS_ERROR_CODE_OK;
    switch (index)
    {
    case 2:
        object->transmitWindowSizeK = (unsigned char)var_toInteger(value);
        break;
    case 3:
        object->transmitWindowSizeRW = (unsigned char)var_toInteger(value);
        break;
    case 4:
        object->maximumOctetsPdu = (uint16_t)var_toInteger(value);
        break;
    case 5:
        object->maximumNumberTransmissions = (unsigned char)var_toInteger(value);
        break;
    case 6:
        object->acknowledgementTimer = (uint16_t)var_toInteger(value);
        break;
    case 7:
        object->bitTimer = (uint16_t)var_toInteger(value);
        break;
    case 8:
        object->rejectTimer = (uint16_t)var_toInteger(value);
        break;
    case 9:
        object->busyStateTimer = (uint16_t)var_toInteger(value);
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
    int ret = DLMS_ERROR_CODE_OK;
    switch (index)
    {
    case 2:
        object->maximumOctetsACnPdu = (uint16_t)var_toInteger(value);
        break;
    case 3:
        object->maximumTransmissions = (unsigned char)var_toInteger(value);
        break;
    case 4:
        object->acknowledgementTime = (uint16_t)var_toInteger(value);
        break;
    case 5:
        object->receiveLifetime = (uint16_t)var_toInteger(value);
        break;
    case 6:
        object->transmitLifetime = (uint16_t)var_toInteger(value);
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
    int ret = 0;
    dlmsVARIANT* tmp;
    switch (index)
    {
    case 2:
        if (value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            bb_clear(&object->systemTitle);
            if ((ret = va_getByIndex(value->Arr, 0, &tmp)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if (tmp->byteArr != NULL)
            {
                if ((ret = bb_set(&object->systemTitle, tmp->byteArr->data, bb_size(tmp->byteArr))) != 0)
                {
                    break;
                }
            }
            if ((ret = va_getByIndex(value->Arr, 1, &tmp)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            object->macAddress = (uint16_t)var_toInteger(tmp);
            if ((ret = va_getByIndex(value->Arr, 2, &tmp)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            object->lSapSelector = (unsigned char)var_toInteger(tmp);
        }
        else
        {
            bb_clear(&object->systemTitle);
            object->macAddress = 0;
            object->lSapSelector = 0;
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
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxUint16PairUint32* it;
    dlmsVARIANT* tmp, * tmp2;
    switch (index)
    {
    case 2:
    {
        arr_clear(&object->synchronizationRegister);
        if (value->vt == DLMS_DATA_TYPE_ARRAY)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it = (gxUint16PairUint32*)gxmalloc(sizeof(gxUint16PairUint32));
                if (it == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->first = (uint16_t)var_toInteger(tmp2);
                if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->second = var_toInteger(tmp2);
                arr_push(&object->synchronizationRegister, it);
            }
        }
    }
    break;
    case 3:
    {
        if (value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            if ((ret = va_getByIndex(value->Arr, 0, &tmp)) == DLMS_ERROR_CODE_OK)
            {
                object->physicalLayerDesynchronization = var_toInteger(tmp);
                if ((ret = va_getByIndex(value->Arr, 1, &tmp)) == DLMS_ERROR_CODE_OK)
                {
                    object->timeOutNotAddressedDesynchronization = var_toInteger(tmp);
                    if ((ret = va_getByIndex(value->Arr, 2, &tmp)) == DLMS_ERROR_CODE_OK)
                    {
                        object->timeOutFrameNotOkDesynchronization = var_toInteger(tmp);
                        if ((ret = va_getByIndex(value->Arr, 3, &tmp)) == DLMS_ERROR_CODE_OK)
                        {
                            object->writeRequestDesynchronization = var_toInteger(tmp);
                            if ((ret = va_getByIndex(value->Arr, 4, &tmp)) == DLMS_ERROR_CODE_OK)
                            {
                                object->wrongInitiatorDesynchronization = var_toInteger(tmp);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            object->physicalLayerDesynchronization = 0;
            object->timeOutNotAddressedDesynchronization = 0;
            object->timeOutFrameNotOkDesynchronization = 0;
            object->writeRequestDesynchronization = 0;
            object->wrongInitiatorDesynchronization = 0;
        }
    }
    break;
    case 4:
    {
        arr_clear(&object->broadcastFramesCounter);
        if (value->vt == DLMS_DATA_TYPE_ARRAY)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it = (gxUint16PairUint32*)gxmalloc(sizeof(gxUint16PairUint32));
                if (it == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                if ((ret = va_getByIndex(tmp->Arr, 0, &tmp2)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->first = (uint16_t)var_toInteger(tmp2);
                if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                it->second = var_toInteger(tmp2);
                arr_push(&object->broadcastFramesCounter, it);
            }
        }
    }
    break;
    case 5:
        object->repetitionsCounter = var_toInteger(value);
        break;
    case 6:
        object->transmissionsCounter = var_toInteger(value);
        break;
    case 7:
        object->crcOkFramesCounter = var_toInteger(value);
        break;
    case 8:
        object->crcNOkFramesCounter = var_toInteger(value);
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
    int ret = 0;
    switch (index)
    {
    case 2:
        object->searchInitiatorTimeout = (uint16_t)var_toInteger(value);
        break;
    case 3:
        object->synchronizationConfirmationTimeout = (uint16_t)var_toInteger(value);
        break;
    case 4:
        object->timeOutNotAddressed = (uint16_t)var_toInteger(value);
        break;
    case 5:
        object->timeOutFrameNotOK = (uint16_t)var_toInteger(value);
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
    int pos, ret = 0;
    dlmsVARIANT* it;
    switch (index)
    {
    case 2:
        object->initiatorElectricalPhase = (DLMS_INITIATOR_ELECTRICAL_PHASE)var_toInteger(value);
        break;
    case 3:
        object->deltaElectricalPhase = (DLMS_DELTA_ELECTRICAL_PHASE)var_toInteger(value);
        break;
    case 4:
        object->maxReceivingGain = (unsigned char)var_toInteger(value);
        break;
    case 5:
        object->maxTransmittingGain = (unsigned char)var_toInteger(value);
        break;
    case 6:
        object->searchInitiatorThreshold = (unsigned char)var_toInteger(value);
        break;
    case 7:
    {
        if (value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            if ((ret = va_getByIndex(value->Arr, 0, &it)) == DLMS_ERROR_CODE_OK)
            {
                object->markFrequency = var_toInteger(it);
                if ((ret = va_getByIndex(value->Arr, 1, &it)) == DLMS_ERROR_CODE_OK)
                {
                    object->spaceFrequency = var_toInteger(it);
                }
            }
        }
        else
        {
            object->markFrequency = 0;
            object->spaceFrequency = 0;
        }
        break;
    }
    case 8:
        object->macAddress = (uint16_t)var_toInteger(value);
        break;
    case 9:
    {
        arr_clear(&object->macGroupAddresses);
        if (value->vt == DLMS_DATA_TYPE_ARRAY)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &it)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                uint16_t* v = gxmalloc(sizeof(uint16_t));
                *v = (uint16_t)var_toInteger(it);
                arr_push(&object->macGroupAddresses, v);
            }
        }
        break;
    }
    case 10:
        object->repeater = (DLMS_REPEATER)var_toInteger(value);
        break;
    case 11:
        object->repeaterStatus = value->boolVal;
        break;
    case 12:
        object->minDeltaCredit = (unsigned char)var_toInteger(value);
        break;
    case 13:
        object->initiatorMacAddress = (uint16_t)var_toInteger(value);
        break;
    case 14:
        object->synchronizationLocked = (unsigned char)var_toInteger(value);
        break;
    case 15:
        object->transmissionSpeed = (DLMS_BAUD_RATE)var_toInteger(value);
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
    int pos, ret = 0;
    dlmsVARIANT* tmp;
    gxByteBuffer* it;
    if (index == 2)
    {
        obj_clearByteBufferList(&object->reportingSystemList);
        if (value->vt == DLMS_DATA_TYPE_ARRAY)
        {
            if (value->vt == DLMS_DATA_TYPE_ARRAY)
            {
                for (pos = 0; pos != value->Arr->size; ++pos)
                {
                    if ((ret = va_getByIndex(value->Arr, pos, &tmp)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                    BYTE_BUFFER_INIT(it);
                    bb_set(it, tmp->byteArr->data, tmp->byteArr->size);
                    if (it == NULL)
                    {
                        ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                        break;
                    }
                    arr_push(&object->reportingSystemList, it);
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

#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
