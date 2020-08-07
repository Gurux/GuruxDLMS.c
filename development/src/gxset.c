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

#include "../include/gxset.h"
#include "../include/dlms.h"
#include "../include/gxkey.h"
#include "../include/cosem.h"
#include "../include/gxget.h"
#include "../include/server.h"

#ifndef DLMS_IGNORE_DATA
int cosem_setData(gxData* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = cosem_getVariant(value->byteArr, &object->value);
        }
        else
        {
            ret = var_copy(&object->value, value);
        }
#else
        ret = var_copy(&object->value, value);
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            ret = cosem_getVariant(value->byteArr, &object->value);
        }
        else
        {
            ret = var_copy(&object->value, value);
        }
#else
        ret = var_copy(&object->value, value);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = va_getCapacity(&object->tableCellValues);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->tableCellValues.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = va_getByIndex(&object->tableCellValues, pos, &tmp)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, tmp)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT tmp;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->time);
        if (ret == 0)
        {
#ifndef DLMS_IGNORE_SERVER
            //If user set's new time transform it to the same time zone as the server is.
            if (settings->server)
            {
                //Convert time to UCT if time zone is given.
                time_toUTC(&object->time);
                clock_updateDST(object, &object->time);
            }
#endif// DLMS_IGNORE_SERVER
        }
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->begin);
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
        object->begin.skip |= DATETIME_SKIPS_YEAR | DATETIME_SKIPS_SECOND | DATETIME_SKIPS_MS | DATETIME_SKIPS_DEVITATION;
    }
    else if (index == 6)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->end);
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
        object->end.skip |= DATETIME_SKIPS_YEAR | DATETIME_SKIPS_SECOND | DATETIME_SKIPS_MS | DATETIME_SKIPS_DEVITATION;
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

int updateSeasonProfile(gxArray* profile, dlmsVARIANT* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxSeasonProfile* sp = NULL;
    obj_clearSeasonProfile(profile);
#ifdef DLMS_IGNORE_MALLOC
    uint16_t count = arr_getCapacity(profile);
    if ((ret = cosem_checkArray(data->byteArr, &count)) == 0) {
        profile->size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(profile, pos, (void**)&sp, sizeof(gxSeasonProfile))) != 0 ||
                (ret = cosem_checkStructure(data->byteArr, 3)) != 0 ||
                (ret = cosem_getOctectString2(data->byteArr, sp->name.value, sizeof(sp->name.value), &sp->name.size)) != 0 ||
                (ret = cosem_getDateTimeFromOctectString(data->byteArr, &sp->start)) != 0 ||
                (ret = cosem_getOctectString2(data->byteArr, sp->weekName.value, sizeof(sp->weekName.value), &sp->weekName.size)) != 0)
            {
                break;
            }
        }
    }
#else
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
        bb_init(&sp->name);
        bb_set2(&sp->name, tmp->byteArr, 0, bb_size(tmp->byteArr));

        ret = va_getByIndex(it->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
#ifdef DLMS_IGNORE_MALLOC
        GX_DATETIME(&tm) = &sp->start;
#endif //DLMS_IGNORE_MALLOC
        ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &tm);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
#ifndef DLMS_IGNORE_MALLOC
        time_copy(&sp->start, tm.dateTime);
        var_clear(&tm);
#endif //DLMS_IGNORE_MALLOC
        ret = va_getByIndex(it->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        bb_init(&sp->weekName);
        bb_set2(&sp->weekName, tmp->byteArr, 0, bb_size(tmp->byteArr));
#ifndef DLMS_IGNORE_MALLOC
        arr_push(profile, sp);
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret != 0 && sp != NULL)
    {
        gxfree(sp);
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int updateWeekProfileTable(gxArray* profile, dlmsVARIANT* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxWeekProfile* wp = NULL;
    obj_clearWeekProfileTable(profile);
#ifdef DLMS_IGNORE_MALLOC
    uint16_t count = arr_getCapacity(profile);
    if ((ret = cosem_checkArray(data->byteArr, &count)) == 0) {
        profile->size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(profile, pos, (void**)&wp, sizeof(gxWeekProfile))) != 0 ||
                (ret = cosem_checkStructure(data->byteArr, 8)) != 0 ||
                (ret = cosem_getOctectString2(data->byteArr, wp->name.value, sizeof(wp->name.value), &wp->name.size)) != 0 ||
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
#else
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
        bb_init(&wp->name);
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
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int updateDayProfileTable(dlmsSettings* settings, gxArray* profile, dlmsVARIANT* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos, pos2;
    gxDayProfile* dp = NULL;
    gxDayProfileAction* ac = NULL;
    obj_clearDayProfileTable(profile);
#ifdef DLMS_IGNORE_MALLOC
    uint16_t count = arr_getCapacity(profile);
    uint16_t count2;
    if ((ret = cosem_checkArray(data->byteArr, &count)) == 0) {
        profile->size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(profile, pos, (void**)&dp, sizeof(gxDayProfile))) != 0)
            {
                break;
            }
            count2 = arr_getCapacity(&dp->daySchedules);
            if ((ret = cosem_checkStructure(data->byteArr, 2)) != 0 ||
                (ret = cosem_getUInt8(data->byteArr, &dp->dayId)) != 0 ||
                (ret = cosem_checkArray(data->byteArr, &count2)) != 0)
            {
                break;
            }
            dp->daySchedules.size = count2;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
            for (pos2 = 0; pos2 != count2; ++pos2)
            {
                if ((ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&ac, sizeof(gxDayProfileAction))) != 0 ||
                    (ret = cosem_checkStructure(data->byteArr, 3)) != 0 ||
                    (ret = cosem_getTimeFromOctectString(data->byteArr, &ac->startTime)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctectString2(data->byteArr, ln, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctectString2(data->byteArr, ac->scriptLogicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(data->byteArr, &ac->scriptSelector)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, &ac->script)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
    }
#else
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
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int cosem_setActivityCalendar(dlmsSettings* settings, gxActivityCalendar* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT tm;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->calendarNameActive);
#else
        bb_clear(&object->calendarNameActive);
        ret = bb_set2(&object->calendarNameActive, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->calendarNamePassive);
#else
        bb_clear(&object->calendarNamePassive);
        ret = bb_set2(&object->calendarNamePassive, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->time);
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxtime* tm;
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
            (ret = cosem_getOctectString2(value->byteArr, object->executedScriptLogicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getUInt16(value->byteArr, &object->executedScriptSelector)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0)
        {
            if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&object->executedScript)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        object->type = (DLMS_SINGLE_ACTION_SCHEDULE_TYPE)var_toInteger(value);
    }
    else if (index == 4)
    {
        arr_clear(&object->executionTime);
#ifdef DLMS_IGNORE_MALLOC
        gxtime t;
        uint16_t count = arr_getCapacity(&object->executionTime);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0) {
            object->executionTime.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getTimeFromOctectString(value->byteArr, &t)) != 0 ||
                    (ret = cosem_getDateFromOctectString(value->byteArr, tm)) != 0)
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
                tm->value.tm_hour = t.value.tm_hour;
                tm->value.tm_min = t.value.tm_min;
                tm->value.tm_sec = t.value.tm_sec;
                tm->skip |= t.skip;
#endif // DLMS_USE_EPOCH_TIME
                tm->deviation = 0;
                tm->skip |= DATETIME_SKIPS_DEVITATION;
            }
        }
#else
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
                date.dateTime->deviation = 0;
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
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
int cosem_setAssociationLogicalName(
    dlmsSettings* settings,
    gxAssociationLogicalName* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret = 0, pos = 0;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp, * tmp2;
#endif //DLMS_IGNORE_MALLOC
    unsigned char ch;
    if (index == 2)
    {
#if !defined(DLMS_IGNORE_MALLOC)
        gxObject* obj = NULL;
        int version;
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
                version = var_toInteger(tmp2);

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
                    obj->version = version;
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

                ret = obj_updateAttributeAccessModes(obj, tmp2->Arr);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                oa_push(&object->objectList, obj);
            }
        }
#endif //!defined(DLMS_IGNORE_MALLOC)
    }
    else if (index == 3)
    {
#ifndef DLMS_IGNORE_MALLOC
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
#else
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->clientSAP)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->serverSAP)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 5)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint32_t conformance;
        //Get structure count.
        if ((ret = cosem_checkStructure(value->byteArr, 6)) != 0 ||
            (ret = cosem_getIntegerFromBitString(value->byteArr, &conformance)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->xDLMSContextInfo.maxReceivePduSize)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->xDLMSContextInfo.maxSendPduSize)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->xDLMSContextInfo.dlmsVersionNumber)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->xDLMSContextInfo.qualityOfService)) != 0 ||
            (ret = cosem_getOctectString(value->byteArr, &object->xDLMSContextInfo.cypheringInfo)) != 0)
        {
            return ret;
        }
        object->xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)conformance;
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
        else  if (value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            ret = va_getByIndex(value->Arr, 0, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.jointIsoCtt = var_toInteger(tmp);
            ret = va_getByIndex(value->Arr, 1, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.country = var_toInteger(tmp);
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 7)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->secret);
#else
        bb_clear(&object->secret);
        if (value->byteArr != NULL)
        {
            ret = bb_set2(&object->secret, value->byteArr, 0, bb_size(value->byteArr));
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 8)
    {
#ifdef DLMS_IGNORE_MALLOC
        if (value->vt != DLMS_DATA_TYPE_ENUM)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        object->associationStatus = (DLMS_ASSOCIATION_STATUS)value->bVal;
#else
        object->associationStatus = (DLMS_ASSOCIATION_STATUS)var_toInteger(value);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 9)
    {
#ifdef DLMS_IGNORE_MALLOC
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        unsigned char ln[6];
        if ((ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln, (gxObject**)&object->securitySetup);
        }
#else
        ret = cosem_getOctectString2(value->byteArr, object->securitySetupReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
        if (bb_size(value->byteArr) != 6)
        {
            return DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SECURITY_SETUP, value->byteArr->data, (gxObject**)&object->securitySetup);
#else
        memcpy(object->securitySetupReference, value->byteArr->data, 6);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 10)
    {
#ifdef DLMS_IGNORE_MALLOC
        gxUser* it;
        uint16_t count = arr_getCapacity(&object->userList);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->userList.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->userList, pos, (void**)&it, sizeof(gxUser))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &it->id)) != 0 ||
                    (ret = cosem_getString2(value->byteArr, it->name, sizeof(it->name))) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 11)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->currentUser.id)) != 0 ||
            (ret = cosem_getString2(value->byteArr, object->currentUser.name, sizeof(object->currentUser.name))) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#ifndef DLMS_IGNORE_MALLOC
            ret = obj_updateAttributeAccessModes(obj, it->Arr);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
#endif //DLMS_IGNORE_MALLOC
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

int cosem_setAssociationShortName(
    dlmsSettings* settings,
    gxAssociationShortName* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret;
#ifndef DLMS_IGNORE_MALLOC
    uint16_t sn;
    DLMS_OBJECT_TYPE type;
    int pos, version;
    dlmsVARIANT* tmp, * tmp2;
    gxObject* obj = NULL;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
#ifndef DLMS_IGNORE_MALLOC
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

                sn = (uint16_t)var_toInteger(tmp2);
                ret = oa_findBySN(&settings->objects, sn, &obj);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if (obj == NULL)
                {
                    ret = va_getByIndex(tmp->Arr, 1, &tmp2);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }

                    type = (DLMS_OBJECT_TYPE)var_toInteger(tmp2);

                    ret = va_getByIndex(tmp->Arr, 2, &tmp2);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }

                    version = var_toInteger(tmp2);
                    ret = va_getByIndex(tmp->Arr, 3, &tmp2);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }

                    ret = cosem_createObject(type, &obj);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        //If unknown object.
                        continue;
                    }
                    ret = cosem_setLogicalName(obj, tmp2->byteArr->data);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    obj->shortName = sn;
                    obj->version = version;
                    //Add object to released objects list.
                    ret = oa_push(&settings->releasedObjects, obj);
                }
                oa_push(&object->objectList, obj);
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        return updateSNAccessRights(&object->objectList, value->Arr);
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        unsigned char ln[6];
        if ((ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln, (gxObject**)&object->securitySetup);
        }
#else
        ret = cosem_getOctectString2(value->byteArr, object->securitySetupReference, 6, NULL);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
#else
        if (bb_size(value->byteArr) != 6)
        {
            return DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SECURITY_SETUP, value->byteArr->data, (gxObject**)&object->securitySetup);
#else
        memcpy(object->securitySetupReference, value->byteArr->data, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
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
    int ret = 0, pos;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
        object->mode = (DLMS_AUTO_ANSWER_MODE)var_toInteger(value);
    }
    else if (index == 3)
    {
        arr_clearKeyValuePair(&object->listeningWindow);
#ifdef DLMS_IGNORE_MALLOC
        gxTimePair* it;
        uint16_t count = arr_getCapacity(&object->listeningWindow);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->listeningWindow.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&it, sizeof(gxTimePair))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &it->first)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &it->second)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->numberOfRingsInListeningWindow)) != 0 ||
            (ret = cosem_getUInt8(value->byteArr, &object->numberOfRingsOutListeningWindow)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        gxTimePair* k;
        uint16_t count = arr_getCapacity(&object->callingWindow);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->callingWindow.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k, sizeof(gxTimePair)) != 0) ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &k->first)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &k->second)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 6)
    {
#ifdef DLMS_IGNORE_MALLOC
        gxDestination* d;
        uint16_t count = arr_getCapacity(&object->destinations);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->destinations.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->destinations, pos, (void**)&d, sizeof(gxDestination)) != 0) ||
                    (ret = cosem_getOctectString2(value->byteArr, d->value, sizeof(d->value), &d->size)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                bb_init(str);
                bb_set2(str, tmp->byteArr, 0, bb_size(tmp->byteArr));
                arr_push(&object->destinations, str);
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
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int cosem_setDemandRegister(gxDemandRegister* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp;
    dlmsVARIANT tmp2;
#endif //DLMS_IGNORE_MALLOC

    if (index == 2)
    {
        if (object->scaler != 0)
        {
#ifndef DLMS_IGNORE_FLOAT64
            ret = var_setDouble(&object->currentAverageValue, var_toDouble(value) * hlp_getScaler(object->scaler));
#else
            ret = var_copy(&object->currentAverageValue, value);
#endif //DLMS_IGNORE_FLOAT64
        }
        else
        {
            ret = var_copy(&object->currentAverageValue, value);
        }
    }
    else if (index == 3)
    {
        if (object->scaler != 0)
        {
#ifndef DLMS_IGNORE_FLOAT64
            ret = var_setDouble(&object->lastAverageValue, var_toDouble(value) * hlp_getScaler(object->scaler));
#else
            ret = var_copy(&object->lastAverageValue, value);
#endif //DLMS_IGNORE_FLOAT64
        }
        else
        {
            ret = var_copy(&object->lastAverageValue, value);
        }
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 5)
    {
        ret = var_copy(&object->status, value);
    }
    else if (index == 6)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->captureTime);
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 7)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->startTimeCurrent);
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->macAddress);
#else
        bb_clear(&object->macAddress);
        ret = bb_set2(&object->macAddress, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp;
    dlmsVARIANT tmp2;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
        ret = var_copy(&object->value, value);
    }
    else if (index == 3)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->scaler)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, &object->unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
        ret = var_copy(&object->status, value);
    }
    else if (index == 5)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->captureTime);
#else
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
int cosem_setGprsSetup(gxGPRSSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret;
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->apn);
#else
        bb_clear(&object->apn);
        if (value->vt == DLMS_DATA_TYPE_STRING)
        {
            bb_set(&object->apn, (unsigned char*)value->strVal->data, value->strVal->size);
        }
        else
        {
            bb_set2(&object->apn, value->byteArr, 0, bb_size(value->byteArr));
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        object->pinCode = value->uiVal;
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
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
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_GPRS_SETUP

#ifndef DLMS_IGNORE_SECURITY_SETUP
int cosem_setSecuritySetup(gxSecuritySetup* object, unsigned char index, dlmsVARIANT* value)
{
    int pos, ret = 0;
    gxCertificateInfo* it = NULL;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp, * tmp3;
#endif //DLMS_IGNORE_MALLOC

    switch (index)
    {
    case 2:
        object->securityPolicy = (unsigned char)var_toInteger(value);
        break;
    case 3:
        object->securitySuite = (DLMS_SECURITY_SUITE)var_toInteger(value);
        break;
    case 4:
        bb_clear(&object->clientSystemTitle);
        bb_set2(&object->clientSystemTitle, value->byteArr, 0, bb_size(value->byteArr));
        break;
    case 5:
        bb_clear(&object->serverSystemTitle);
        bb_set2(&object->serverSystemTitle, value->byteArr, 0, bb_size(value->byteArr));
        break;
    case 6:
        obj_clearCertificateInfo(&object->certificates);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->certificates);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->certificates.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->certificates, pos, (void**)&it, sizeof(gxCertificateInfo))) != 0 ||
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
#else
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
                    gxfree(it->subjectAltName);
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->deviceAddress);
#else
        bb_clear(&object->deviceAddress);
        ret = bb_set2(&object->deviceAddress, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 7)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->password1);
#else
        bb_clear(&object->password1);
        ret = bb_set2(&object->password1, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 8)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->password2);
#else
        bb_clear(&object->password2);
        ret = bb_set2(&object->password2, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 9)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->password5);
#else
        bb_clear(&object->password5);
        ret = bb_set2(&object->password5, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
int cosem_setIP4Setup(dlmsSettings* settings, gxIp4Setup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0, pos;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp, * tmp3;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* ipItem;
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
            {
                return ret;
            }
        }
#else
        ret = cosem_getOctectString2(value->byteArr, object->dataLinkLayerReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, value->byteArr->data, &object->dataLinkLayer)) != 0)
        {
            return ret;
        }
#else
        ret = bb_get(value->byteArr, object->dataLinkLayerReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        object->ipAddress = var_toInteger(value);
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint32_t* v;
        uint16_t count = arr_getCapacity(&object->multicastIPAddress);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->multicastIPAddress.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->multicastIPAddress, pos, (void**)&v, sizeof(uint32_t))) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, v)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                if ((ret = var_copy(tmp, tmp3)) != 0 ||
                    (ret = va_push(&object->multicastIPAddress, tmp3)) != 0)
                {
                    break;
                }
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 5)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->ipOptions);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->ipOptions.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->ipOptions, pos, (void**)&ipItem, sizeof(gxip4SetupIpOption))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&ipItem->type)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ipItem->length)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, ipItem->data.value, sizeof(ipItem->data.value), &ipItem->data.size)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                bb_init(&ipItem->data);
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
#endif //DLMS_IGNORE_MALLOC
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
    DLMS_OBJECT_TYPE ot = DLMS_OBJECT_TYPE_NONE;
    int ret = DLMS_ERROR_CODE_OK, pos;
#ifdef DLMS_IGNORE_MALLOC
    uint16_t* tmp;
#else
    dlmsVARIANT* tmp, * tmp3;
    dlmsVARIANT tmp2;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        unsigned char ln[6];
        ot = 0;
        if ((ret = cosem_checkStructure(value->byteArr, 3)) == 0 &&
            (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&ot)) == 0 &&
            (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0 &&
            (ret = cosem_getInt8(value->byteArr, &object->selectedAttributeIndex)) == 0)
        {
            ret = oa_findByLN(&settings->objects, ot, ln, &object->monitoredValue);
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
            (ret = cosem_getUInt16(value->byteArr, &object->emergencyProfile.id)) != 0 ||
            (ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->emergencyProfile.activationTime)) != 0 ||
            (ret = cosem_getUInt32(value->byteArr, &object->emergencyProfile.duration)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 9)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->emergencyProfileGroupIDs);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->emergencyProfileGroupIDs.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->emergencyProfileGroupIDs, pos, (void**)&tmp, sizeof(uint16_t))) != 0 ||
                    (ret = bb_getUInt16(value->byteArr, tmp)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 10)
    {
        object->emergencyProfileActive = value->boolVal;
    }
    else if (index == 11)
    {
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char lnOver[6];
        unsigned char lnUnder[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
            (ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getOctectString2(value->byteArr, lnOver, 6, NULL)) == 0 &&
#else
            (ret = cosem_getOctectString2(value->byteArr, object->actionOverThreshold.logicalName, 6, NULL)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getUInt16(value->byteArr, &object->actionOverThreshold.scriptSelector)) == 0 &&
            (ret = cosem_checkStructure(value->byteArr, 2)) == 0 &&
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getOctectString2(value->byteArr, lnUnder, 6, NULL)) == 0 &&
#else
            (ret = cosem_getOctectString2(value->byteArr, object->actionUnderThreshold.logicalName, 6, NULL)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getUInt16(value->byteArr, &object->actionUnderThreshold.scriptSelector)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnOver, (gxObject**)&object->actionOverThreshold.script)) == 0 &&
                (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnUnder, (gxObject**)&object->actionUnderThreshold.script)) == 0)
            {

            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //Error code is returned at the end of the function.
        }
#else
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
            if ((ret = cosem_createObject(ot, (gxObject**)&object->actionOverThreshold.script)) != 0)
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
            if ((ret = cosem_createObject(ot, (gxObject**)&object->actionUnderThreshold.script)) != 0)
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP, ln, &object->mBusPort)) != 0)
            {
                return ret;
            }
        }
#else
        ret = cosem_getOctectString2(value->byteArr, object->mBusPortReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
#ifdef DLMS_IGNORE_MALLOC
        gxCaptureDefinition* it;
        uint16_t count = arr_getCapacity(&object->captureDefinition);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->captureDefinition.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->captureDefinition, pos, (void**)&it, sizeof(gxCaptureDefinition))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, it->data.value, sizeof(it->data.value), &it->data.size)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, it->value.value, sizeof(it->value.value), &it->value.size)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                bb_init(start);
                bb_init(end);
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* mp;
#else
    dlmsVARIANT* tmp, * tmp3;
    gxByteBuffer* str;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
        object->communicationSpeed = (DLMS_BAUD_RATE)var_toInteger(value);
    }
    else if (index == 3)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->initialisationStrings);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            unsigned cnt = object->base.version != 0 ? 3 : 2;
            object->initialisationStrings.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->initialisationStrings, pos, (void**)&modemInit, sizeof(gxModemInitialisation))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, cnt)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, modemInit->request.value, sizeof(modemInit->request.value), &modemInit->request.size)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, modemInit->response.value, sizeof(modemInit->response.value), &modemInit->response.size)) != 0)
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
#else
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
                bb_init(&modemInit->request);
                bb_init(&modemInit->response);
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->modemProfile);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->modemProfile.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->modemProfile, pos, (void**)&mp, sizeof(gxModemProfile))) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, mp->value, sizeof(mp->value), &mp->size)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#ifndef DLMS_IGNORE_MALLOC
                str = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                bb_init(str);
#else
                if (ret = arr_getByIndex(&object->modemProfile, pos, (void**)&str) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                bb_set2(str, tmp->byteArr, 0, bb_size(tmp->byteArr));
#ifndef DLMS_IGNORE_MALLOC
                arr_push(&object->modemProfile, str);
#endif //DLMS_IGNORE_MALLOC
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
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
int cosem_setPppSetup(dlmsSettings* settings, gxPppSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxpppSetupLcpOption* lcpItem;
    gxpppSetupIPCPOption* ipcpItem;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp;
    dlmsVARIANT* tmp3;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0)
        {
            if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->phy)) != 0)
            {
                return ret;
            }
        }
#else
        ret = cosem_getOctectString2(value->byteArr, object->PHYReference, 6, NULL);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        arr_clear(&object->lcpOptions);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->lcpOptions);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->lcpOptions.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->lcpOptions, pos, (void**)&lcpItem, sizeof(gxpppSetupLcpOption))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&lcpItem->type)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &lcpItem->length)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, &lcpItem->data)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                lcpItem->length = var_toInteger(tmp3);
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
        arr_clear(&object->ipcpOptions);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->ipcpOptions);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->ipcpOptions.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->ipcpOptions, pos, (void**)&ipcpItem, sizeof(gxpppSetupIPCPOption))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, (unsigned char*)&ipcpItem->type)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ipcpItem->length)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, &ipcpItem->data)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                ipcpItem->length = var_toInteger(tmp3);
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 5)
    {
#ifdef DLMS_IGNORE_MALLOC
        if (value->byteArr == NULL)
        {
            bb_clear(&object->userName);
            bb_clear(&object->password);
        }
        else if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getOctectString(value->byteArr, &object->userName)) != 0 ||
            (ret = cosem_getOctectString(value->byteArr, &object->password)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int cosem_setRegisterActivation(dlmsSettings* settings, gxRegisterActivation* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0, pos;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* objectDefinition;
#else
    gxObject* objectDefinition;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
        uint16_t count = oa_getCapacity(&object->registerAssignment);
#else
        uint16_t count = arr_getCapacity(&object->registerAssignment);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->registerAssignment.size = count;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
#ifdef DLMS_IGNORE_MALLOC
                (ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&objectDefinition, sizeof(gxObject*))) != 0 ||
#else
                    (ret = oa_getByIndex(&object->registerAssignment, pos, &objectDefinition)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&objectDefinition->objectType)) != 0 ||
#else
                    (ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&objectDefinition, sizeof(gxObjectDefinition))) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&objectDefinition->objectType)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctectString2(value->byteArr, objectDefinition->logicalName, 6, NULL)) != 0)
                {
                    break;
                }
            }
        }
#else
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        obj_clearRegisterActivationAssignment(&object->registerAssignment);
#else
        obj_clearRegisterActivationAssignment(&object->registerAssignment);
#endif //DLMS_IGNORE_OBJECT_POINTERS
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
                type = var_toInteger(tmp3);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                const unsigned char* ln = tmp3->byteArr->data;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                objectDefinition = (gxObjectDefinition*)gxmalloc(sizeof(gxObjectDefinition));
                objectDefinition->objectType = (DLMS_OBJECT_TYPE)type;
                memcpy(objectDefinition->logicalName, ln, 6);
#else
                if ((ret = oa_findByLN(&settings->objects, type, ln, &objectDefinition)) != 0)
                {
                    break;
                }
                if (objectDefinition == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                oa_push(&object->registerAssignment, objectDefinition);
#else
                arr_push(&object->registerAssignment, objectDefinition);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
#ifdef DLMS_IGNORE_OBJECT_POINTERS
            if (ret != 0 && objectDefinition != NULL)
            {
                gxfree(objectDefinition);
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        obj_clearRegisterActivationMaskList(&object->maskList);
#ifdef DLMS_IGNORE_MALLOC
        gxRegisterActivationMask* k;
        uint16_t count = arr_getCapacity(&object->maskList);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            unsigned char ch;
            unsigned char ln[6];
            object->maskList.capacity = count;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = arr_getByIndex(&object->maskList, pos, (void**)&k, sizeof(gxRegisterActivationMask))) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
                    (ret = bb_getUInt8(value->byteArr, &ch)) != 0)
                {
                    break;
                }
                if (ch != DLMS_DATA_TYPE_ARRAY)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                else
                {
                    if ((ret = bb_getUInt8(value->byteArr, &ch)) == 0)
                    {
                        if (ch > sizeof(k->indexes))
                        {
                            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                        else
                        {
                            for (pos = 0; pos != ch; ++pos)
                            {
                                if ((ret = cosem_getUInt8(value->byteArr, &k->indexes[pos])) != 0)
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
#else
        int pos2;
        dlmsVARIANT* tmp, * tmp3;
        gxByteBuffer* start, * end;
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                start = end = NULL;
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
                bb_init(start);
                end = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                bb_init(end);
                bb_set(start, tmp3->byteArr->data, tmp3->byteArr->size);
                ret = va_getByIndex(tmp->Arr, 1, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                for (pos2 = 0; pos2 != tmp3->Arr->size; ++pos2)
                {
                    ret = va_getByIndex(tmp3->Arr, pos2, &tmp);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    bb_setUInt8(end, (unsigned char)var_toInteger(tmp));
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = bb_get(value->byteArr, object->activeMask, 6);
#else
        bb_clear(&object->activeMask);
        bb_set2(&object->activeMask, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
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
    gxActionSet* actionSet;
    dlmsVARIANT* tmp;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp3, * tmp4;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
        va_clear(&object->thresholds);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = va_getCapacity(&object->thresholds);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->thresholds.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = va_getByIndex(&object->thresholds, pos, &tmp)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, tmp)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        signed short type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = cosem_checkStructure(value->byteArr, 3)) == 0 &&
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getInt16(value->byteArr, (short*)&type)) == 0 &&
            (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0 &&
#else
            (ret = cosem_getInt16(value->byteArr, (short*)&object->monitoredValue.objectType)) == 0 &&
            (ret = cosem_getOctectString2(value->byteArr, object->monitoredValue.logicalName, 6, NULL)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getInt8(value->byteArr, &object->monitoredValue.attributeIndex)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = oa_findByLN(&settings->objects, type, ln, &object->monitoredValue.target);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //Error code is returned at the end of the function.
        }
#else
        ret = va_getByIndex(value->Arr, 0, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        type = var_toInteger(tmp);
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
        obj_clearRegisterMonitorActions(&object->actions);
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char lnUp[6];
        unsigned char lnDown[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS

        uint16_t count;
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->actions.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->actions, pos, (void**)&actionSet, sizeof(gxActionSet))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    //Update action up.
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctectString2(value->byteArr, lnUp, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctectString2(value->byteArr, actionSet->actionUp.logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &actionSet->actionUp.scriptSelector)) != 0 ||
                    //Update action down.
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctectString2(value->byteArr, lnDown, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctectString2(value->byteArr, actionSet->actionDown.logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &actionSet->actionDown.scriptSelector)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnUp, (gxObject**)&actionSet->actionUp.script)) != 0 ||
                    (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, lnDown, (gxObject**)&actionSet->actionDown.script)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count;
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->sapAssignmentList.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &it->id)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, it->name.value, sizeof(it->name.value), &it->name.size)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                bb_init(&it->name);
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
#endif //DLMS_IGNORE_MALLOC
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
        arr_clear(&object->entries);
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count = arr_getCapacity(&object->entries);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            uint32_t execWeekdays;
            object->entries.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->entries, pos, (void**)&se, sizeof(gxScheduleEntry))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 10)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &se->index)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &se->enable)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctectString2(value->byteArr, se->logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &se->scriptSelector)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &se->switchTime)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &se->validityWindow)) != 0 ||
                    (ret = cosem_getIntegerFromBitString(value->byteArr, &execWeekdays)) != 0 ||
                    (ret = cosem_getBitString(value->byteArr, &se->execSpecDays)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &se->beginDate)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &se->endDate)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&se->scriptTable)) != 0)
                {
                    break;
                }
#else
                se->execWeekdays = execWeekdays;
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
#else
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
#ifdef DLMS_IGNORE_MALLOC
                GX_DATETIME(&tmp3) = &se->switchTime;
#endif //DLMS_IGNORE_MALLOC

                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifndef DLMS_IGNORE_MALLOC
                time_copy(&se->switchTime, tmp3.dateTime);
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
                GX_DATETIME(&tmp3) = &se->beginDate;
#endif //DLMS_IGNORE_MALLOC
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifndef DLMS_IGNORE_MALLOC
                time_copy(&se->beginDate, tmp3.dateTime);
#endif //DLMS_IGNORE_MALLOC

                ret = va_getByIndex(tmp->Arr, 9, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                GX_DATETIME(&tmp3) = &se->endDate;
#endif //DLMS_IGNORE_MALLOC
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
#ifndef DLMS_IGNORE_MALLOC
                time_copy(&se->endDate, tmp3.dateTime);
                arr_push(&object->entries, se);
#endif //DLMS_IGNORE_MALLOC
            }
#ifndef DLMS_IGNORE_MALLOC
            if (ret != 0 && se != NULL)
            {
                gxfree(se);
            }
#endif //DLMS_IGNORE_MALLOC
        }
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count = arr_getCapacity(&object->scripts), count2;
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->scripts.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&script, sizeof(gxScript))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &script->id)) != 0)
                {
                    break;
                }
                count2 = arr_getCapacity(&script->actions);
                if ((ret = cosem_checkArray(value->byteArr, &count2)) != 0)
                {
                    break;
                }
                for (pos2 = 0; pos2 != count2; ++pos2)
                {
                    if ((ret = arr_getByIndex(&script->actions, pos2, (void**)&scriptAction, sizeof(gxScriptAction))) != 0 ||
                        (ret = cosem_checkStructure(value->byteArr, 5)) != 0 ||
                        (ret = cosem_getEnum(value->byteArr, (unsigned char*)&scriptAction->type)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_getUInt16(value->byteArr, &type)) != 0 ||
                        (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
                        (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&scriptAction->objectType)) != 0 ||
                        (ret = cosem_getOctectString2(value->byteArr, scriptAction->logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_getInt8(value->byteArr, &scriptAction->index)) != 0 ||
                        (ret = cosem_getVariant(value->byteArr, &scriptAction->parameter)) != 0)
                    {
                        break;
                    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    if ((ret = oa_findByLN(&settings->objects, type, ln, &scriptAction->target)) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                }
            }
        }
#else
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
#ifndef DLMS_IGNORE_MALLOC
                        scriptAction = (gxScriptAction*)gxmalloc(sizeof(gxScriptAction));
                        if (scriptAction == NULL)
                        {
                            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                            break;
                        }
#else
                        if (ret = arr_getByIndex(&script->actions, pos2, (void**)&scriptAction) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
                        if (scriptAction != NULL)
                        {
                            gxfree(scriptAction);
                        }
#endif //DLMS_IGNORE_MALLOC
                        break;
                    }
                }
#ifndef DLMS_IGNORE_MALLOC
                if (ret != 0 && script != NULL)
                {
                    gxfree(script);
                }
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
                script = (gxScript*)gxmalloc(sizeof(gxScript));
                if (script == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
#else
                if (ret = arr_getByIndex(&object->scripts, 0, (void**)&script) != 0)
                {
                    return ret;
                }
#endif //DLMS_IGNORE_MALLOC
                arr_init(&script->actions);
                script->id = (uint16_t)var_toInteger(tmp3);
#ifndef DLMS_IGNORE_MALLOC
                arr_push(&object->scripts, script);
#endif //DLMS_IGNORE_MALLOC
                ret = va_getByIndex(tmp->Arr, 3, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
#ifndef DLMS_IGNORE_MALLOC
                scriptAction = (gxScriptAction*)gxmalloc(sizeof(gxScriptAction));
                if (scriptAction == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
#else
                if (ret = arr_getByIndex(&script->actions, 0, (void**)&scriptAction) != 0)
                {
                    return ret;
                }
#endif //DLMS_IGNORE_MALLOC
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
                type = var_toInteger(tmp3);
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
#ifndef DLMS_IGNORE_MALLOC
                arr_push(&script->actions, scriptAction);
#endif //DLMS_IGNORE_MALLOC
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
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int cosem_setSpecialDaysTable(gxSpecialDaysTable* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0, pos;
    gxSpecialDay* specialDay;
    if (index == 2)
    {
        arr_clear(&object->entries);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->entries);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->entries.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->entries, pos, (void**)&specialDay, sizeof(gxSpecialDay))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, &specialDay->index)) != 0 ||
                    (ret = cosem_getDateFromOctectString(value->byteArr, &specialDay->date)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &specialDay->dayId)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
            if ((ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0)
            {
                ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_IP4_SETUP, ln, &object->ipSetup);
            }
#else
            ret = bb_get(value->byteArr, object->ipReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
            if ((ret = bb_get(value->byteArr, ln, 6)) != 0 ||
                (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_IP4_SETUP, ln, &object->ipSetup)) != 0)
            {

            }
#else
            ret = bb_get(value->byteArr, object->ipReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
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

#ifndef DLMS_IGNORE_MESSAGE_HANDLER
int cosem_setMessageHandler(
    gxMessageHandler* object,
    unsigned char index,
    dlmsVARIANT* value)
{
    int ret, pos;
#ifdef DLMS_IGNORE_MALLOC
    gxTimePair* it;
    dlmsVARIANT* tmp;
#else
    dlmsVARIANT* tmp, * tmp2;
    dlmsVARIANT start, end;
    gxtime* s, * e;
    var_init(&start);
    var_init(&end);
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
        arr_clear(&object->listeningWindow);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->listeningWindow);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->listeningWindow.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&it, sizeof(gxTimePair))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &it->first)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &it->second)) != 0)
                {
                    break;
                }
            }
        }
#else
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_getByIndex(tmp2->Arr, 0, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &start);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = va_getByIndex(tmp2->Arr, 1, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &end);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                s = (gxtime*)gxmalloc(sizeof(gxtime));
                e = (gxtime*)gxmalloc(sizeof(gxtime));
                time_copy(s, start.dateTime);
                time_copy(e, end.dateTime);
                arr_push(&object->listeningWindow, key_init(s, e));
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        va_clear(&object->allowedSenders);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = va_getCapacity(&object->allowedSenders);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->allowedSenders.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = va_getByIndex(&object->allowedSenders, pos, &tmp)) != 0 ||
                    (ret = cosem_getVariant(value->byteArr, tmp)) != 0)
                {
                    break;
                }
            }
        }
#else
        if (value->Arr != NULL)
        {
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
                va_push(&object->allowedSenders, tmp2);
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
        arr_clear(&object->sendersAndActions);
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                //TODO: Check this.
                //Object[] tmp2 = (Object[]) tmp[1];
                /*TODO:
                KeyValuePair<int, GXDLMSScriptAction> executed_script = new KeyValuePair<int, GXDLMSScriptAction>(Convert.ToInt32(tmp2[1], tmp2[2]));
                m_SendersAndActions.Add(new KeyValuePair<string, KeyValuePair<int, GXDLMSScriptAction>>(id, tmp[1] as GXDateTime));
                }
                va_push(&object->sendersAndActions, tmp);
                */
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
int cosem_setPushSetup(dlmsSettings* settings, gxPushSetup* object, unsigned char index, dlmsVARIANT* value)
{
    int ret, pos;
    gxTarget* it;
#ifndef DLMS_IGNORE_MALLOC
    gxObject* obj;
    dlmsVARIANT* tmp, * tmp3;
    gxtime* s, * e;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
        DLMS_OBJECT_TYPE type = DLMS_OBJECT_TYPE_NONE;
        obj_clearPushObjectList(&object->pushObjectList);
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count = arr_getCapacity(&object->pushObjectList);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0) {
            object->pushObjectList.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->pushObjectList, pos, (void**)&it, sizeof(gxTarget))) == 0)
                {
                    if ((ret = cosem_checkStructure(value->byteArr, 4)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getUInt16(value->byteArr, &it->objectType)) != 0 ||
                        (ret = cosem_getOctectString2(value->byteArr, it->logicalName, 6, NULL)) != 0 ||
#else
                        (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&type)) != 0 ||
                        (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_getInt8(value->byteArr, &it->attributeIndex)) != 0 ||
                        (ret = cosem_getUInt16(value->byteArr, &it->dataIndex)) != 0)
                    {
                        break;
                    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    if ((ret = oa_findByLN(&settings->objects, type, ln, &it->target)) != 0)
                    {
                        return ret;
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
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
#ifdef DLMS_IGNORE_MALLOC
        object->destination.size = 0;
        if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->service)) != 0 ||
            (ret = cosem_getOctectString2(value->byteArr, object->destination.value, sizeof(object->destination.value), &object->destination.size)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->message)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
        if (object->destination != NULL)
        {
            gxfree(object->destination);
            object->destination = NULL;
        }
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
            object->destination = (char*)gxmalloc(tmp->byteArr->size + 1);
            memcpy(object->destination, tmp->byteArr->data, tmp->byteArr->size);
            object->destination[tmp->byteArr->size] = '\0';
        }
        //Message.
        ret = va_getByIndex(value->Arr, 2, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->message = (DLMS_MESSAGE_TYPE)var_toInteger(tmp);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
        gxTimePair* it;
        uint16_t count = arr_getCapacity(&object->communicationWindow);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->communicationWindow.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&it, sizeof(gxTimePair))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &it->first)) != 0 ||
                    (ret = cosem_getDateTimeFromOctectString(value->byteArr, &it->second)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
    uint16_t count = arr_getCapacity(&target->chargeTables);
    if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
        //charge per unit scaling
        (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
        (ret = cosem_getInt8(value->byteArr, &target->chargePerUnitScaling.commodityScale)) != 0 ||
        (ret = cosem_getInt8(value->byteArr, &target->chargePerUnitScaling.priceScale)) != 0 ||
        //commodity
        (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getInt16(value->byteArr, &type)) != 0 ||
        (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
#else
        (ret = cosem_getInt16(value->byteArr, (short*)&target->commodity.type)) != 0 ||
        (ret = cosem_getOctectString2(value->byteArr, target->commodity.logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_getInt8(value->byteArr, &target->commodity.attributeIndex)) != 0 ||
        (ret = cosem_checkArray(value->byteArr, &count)) != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = oa_findByLN(&settings->objects, type, ln, &target->commodity.target)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    target->chargeTables.size = count;
    for (pos = 0; pos != count; ++pos)
    {
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&ct, sizeof(gxChargeTable))) != 0 ||
            (ret = cosem_getOctectString2(value->byteArr, ct->index.data, sizeof(ct->index.data), &ct->index.size)) != 0 ||
            (ret = cosem_getInt16(value->byteArr, &ct->chargePerUnit)) != 0)
        {
            break;
        }
    }
#else
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
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&ct)) != 0)
        {
            break;
        }
        bb_init(&ct->index);
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
        arr_push(&target->chargeTables, ct);
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}

int cosem_setCharge(dlmsSettings* settings, gxCharge* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT tmp;
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->unitChargeActivationTime);
#else
        //Some meters are returning empty octect string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tmp);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp);
            if (ret != 0)
            {
                return ret;
            }
            time_copy(&object->unitChargeActivationTime, tmp.dateTime);
            var_clear(&tmp);
        }
        else
        {
            time_clear(&object->unitChargeActivationTime);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 8)
    {
        object->period = var_toInteger(value);
    }
    else if (index == 9)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint32_t val;
        if ((ret = cosem_getIntegerFromBitString(value->byteArr, &val)) == 0)
        {
            object->chargeConfiguration = val;
        }
#else
        object->chargeConfiguration = var_toInteger(value);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 10)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTime(value->byteArr, &object->lastCollectionTime);
#else
        //Some meters are returning empty octect string here.
        if (value->vt == DLMS_DATA_TYPE_DATETIME && value->dateTime != NULL)
        {
            time_copy(&object->lastCollectionTime, value->dateTime);
        }
        else
        {
            time_clear(&object->lastCollectionTime);
        }
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT tmp;
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        uint32_t val;
        if ((ret = cosem_getIntegerFromBitString(value->byteArr, &val)) == 0)
        {
            object->creditConfiguration = val;
        }
#else
        object->creditConfiguration = var_toInteger(value);
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->period);
#else
        var_init(&tmp);
        ret = dlms_changeType(value->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp);
        if (ret == 0)
        {
            time_copy(&object->period, tmp.dateTime);
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
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_ACCOUNT
int cosem_setAccount(gxAccount* object, unsigned char index, dlmsVARIANT* value)
{
    int ret = 0, pos;
    unsigned char* ba;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* it;
    dlmsVARIANT tmp;
    dlmsVARIANT* tmp2;
#endif //DLMS_IGNORE_MALLOC
    gxCreditChargeConfiguration* ccc;
    gxTokenGatewayConfiguration* gwc;
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->paymentMode)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->accountStatus)) != 0)
        {
            //Error is returned at the end of the function.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        object->currentCreditInUse = (unsigned char)var_toInteger(value);
    }
    else if (index == 4)
    {
#ifdef DLMS_IGNORE_MALLOC
        uint32_t v;
        if ((ret = cosem_getIntegerFromBitString(value->byteArr, &v)) == 0)
        {
            object->currentCreditStatus = (DLMS_ACCOUNT_CREDIT_STATUS)v;
        }
#else
        if (value->bitArr == NULL || value->bitArr->size == 0)
        {
            return DLMS_ERROR_CODE_READ_WRITE_DENIED;
        }
        else
        {
            object->currentCreditStatus = (DLMS_ACCOUNT_CREDIT_STATUS)var_toInteger(value);
        }
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->creditReferences);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->creditReferences.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->creditReferences, pos, (void**)&ba, 6)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, ba, 6, NULL)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 10)
    {
        arr_clear(&object->chargeReferences);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->chargeReferences);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->chargeReferences.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->chargeReferences, pos, (void**)&ba, 6)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, ba, 6, NULL)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 11)
    {
        obj_clearCreditChargeConfigurations(&object->creditChargeConfigurations);
#ifdef DLMS_IGNORE_MALLOC
        uint32_t v;
        uint16_t count = arr_getCapacity(&object->creditChargeConfigurations);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->creditChargeConfigurations.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->creditChargeConfigurations, pos, (void**)&ccc, sizeof(gxCreditChargeConfiguration))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, ccc->creditReference, 6, NULL)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, ccc->chargeReference, 6, NULL)) != 0 ||
                    (ret = cosem_getIntegerFromBitString(value->byteArr, &v)) != 0)
                {
                    break;
                }
                ccc->collectionConfiguration = (DLMS_CREDIT_COLLECTION_CONFIGURATION)v;
            }
        }
#else
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ccc = NULL;
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 12)
    {
        obj_clearTokenGatewayConfigurations(&object->tokenGatewayConfigurations);
        gwc = NULL;
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->tokenGatewayConfigurations);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->tokenGatewayConfigurations.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->tokenGatewayConfigurations, pos, (void**)&gwc, sizeof(gxTokenGatewayConfiguration))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, gwc->creditReference, 6, NULL)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &gwc->tokenProportion)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 13)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->accountActivationTime);
#else
        var_init(&tmp);
        ret = dlms_changeType(value->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp);
        if (ret == 0)
        {
            time_copy(&object->accountActivationTime, tmp.dateTime);
        }
        var_clear(&tmp);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 14)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->accountClosureTime);
#else
        ret = dlms_changeType(value->byteArr, DLMS_DATA_TYPE_DATETIME, &tmp);
        if (ret == 0)
        {
            time_copy(&object->accountClosureTime, tmp.dateTime);
        }
        var_clear(&tmp);
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 15)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
            (ret = cosem_getUtf8String2(value->byteArr, object->currency.name.value, sizeof(object->currency.name.value), &object->currency.name.size)) != 0 ||
            (ret = cosem_getInt8(value->byteArr, &object->currency.scale)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->currency.unit)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#else
        gxfree(object->currency.name);
        //Name
        ret = va_getByIndex(value->Arr, 0, &it);
        if (ret != 0)
        {
            return ret;
        }
        if (it->strVal != NULL && it->strVal->size != 0)
        {
#ifndef DLMS_IGNORE_MALLOC
            object->currency.name = gxmalloc(it->strVal->size + 1);
#endif //DLMS_IGNORE_MALLOC
            memcpy(object->currency.name, it->strVal->data, it->strVal->size);
            object->currency.name[it->strVal->size] = '\0';
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
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* it, * tmp;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
        object->imageBlockSize = var_toInteger(value);
    }
    else if (index == 3)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getBitString(value->byteArr, &object->imageTransferredBlocksStatus);
#else
        ba_clear(&object->imageTransferredBlocksStatus);
        if (value->bitArr != NULL)
        {
            ba_copy(&object->imageTransferredBlocksStatus, value->bitArr->data, (uint16_t)value->bitArr->size);
        }
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->imageActivateInfo);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->imageActivateInfo.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&item, sizeof(gxImageActivateInfo))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &item->size)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, item->identification.data, sizeof(item->identification.data), &item->identification.size)) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, item->signature.data, sizeof(item->signature.data), &item->signature.size)) != 0)
                {
                    break;
                }
            }
        }
#else
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
                bb_init(&item->identification);
                bb_init(&item->signature);
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
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    gxObject* obj;
    DLMS_OBJECT_TYPE type;
    dlmsVARIANT* tmp, * tmp2;
#endif //DLMS_IGNORE_MALLOC
    gxTarget* co;
    int pos, ret;
    if ((ret = obj_clearProfileGenericCaptureObjects(objects)) != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
#ifdef DLMS_IGNORE_MALLOC
    uint16_t type;
    uint16_t count = arr_getCapacity(objects);
    if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
    {
        unsigned char ln[6];
        objects->size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(objects, pos, (void**)&co, sizeof(gxTarget))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 4)) != 0 ||
                (ret = cosem_getUInt16(value->byteArr, &type)) != 0 ||
                (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &co->attributeIndex)) != 0 ||
                (ret = cosem_getUInt16(value->byteArr, &co->dataIndex)) != 0)
            {
                break;
            }
            ret = oa_findByLN(&settings->objects, type, ln, &co->target);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if (co->target == NULL)
            {
                ret = DLMS_ERROR_CODE_UNAVAILABLE_OBJECT;
                break;
            }
        }
    }
#else
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
#ifndef DLMS_IGNORE_MALLOC
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
#else
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_MALLOC
            }
#ifndef DLMS_IGNORE_MALLOC
            co = (gxTarget*)gxmalloc(sizeof(gxTarget));
            if (co == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
#else
            if (ret = arr_getByIndex(objects, pos, (void**)&co) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
            arr_push(objects, key_init(obj, co));
#endif //DLMS_IGNORE_MALLOC
        }
    }
    //Trim array.
    arr_capacity(objects, objects->size);
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    int pos, pos2;
#endif //DLMS_IGNORE_MALLOC
    DLMS_OBJECT_TYPE type;
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp, * row, * data;
    variantArray* va;
#endif //DLMS_IGNORE_MALLOC
    if (index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
#else
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
                    if (data->vt == DLMS_DATA_TYPE_OCTET_STRING || data->vt == DLMS_DATA_TYPE_UINT32)
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
                        //Some meters return NULL date time string.
                        if (data->vt == DLMS_DATA_TYPE_OCTET_STRING && obj->objectType == DLMS_OBJECT_TYPE_CLOCK && t->attributeIndex == 2)
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
                        }
                        else if (data->vt == DLMS_DATA_TYPE_UINT32 && obj->objectType == DLMS_OBJECT_TYPE_DATA && t->attributeIndex == 2 &&
                            memcmp(obj->logicalName, UNIX_TIME, 6) == 0)
                        {
                            gxtime tmp4;
                            time_initUnix(&tmp4, data->ulVal);
                            var_setDateTime(data, &tmp4);
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
#endif //DLMS_IGNORE_MALLOC
    }
    else if (index == 3)
    {
        object->entriesInUse = 0;
#ifndef DLMS_IGNORE_MALLOC
        ret = obj_clearProfileGenericBuffer(&object->buffer);
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 4)) != 0)
        {
            unsigned char ln[6];
            type = 0;
            object->sortObject = NULL;
            if ((ret = cosem_getInt16(value->byteArr, (signed short*)&type)) == 0 &&
                (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0 &&
                (ret = oa_findByLN(&settings->objects, type, ln, &object->sortObject)) == 0 &&
                (ret = cosem_getInt8(value->byteArr, &object->sortObjectAttributeIndex)) == 0 &&
                (ret = cosem_getUInt16(value->byteArr, &object->sortObjectDataIndex)) == 0)
            {
                if (object->sortObject == NULL)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT tmp2;
    dlmsVARIANT* tmp, * it;
#endif //DLMS_IGNORE_MALLOC
    gxAdjacentCell* ac;
    switch (index)
    {
    case 2:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getString(value->byteArr, &object->operatorName);
#else
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
#ifdef DLMS_IGNORE_MALLOC
            bb_clear(&object->operatorName);
            if (value->strVal != NULL && bb_size(value->strVal) != 0)
            {
                bb_set(&object->operatorName, value->strVal->data, value->strVal->size);
            }
#else
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
#endif //DLMS_IGNORE_MALLOC
        }
        else
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
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
        if ((ret = cosem_checkStructure(value->byteArr, count)) != 0)
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
#else
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
#endif //DLMS_IGNORE_MALLOC
        break;
    case 7:
        arr_clear(&object->adjacentCells);
#ifdef DLMS_IGNORE_MALLOC
        uint16_t count = arr_getCapacity(&object->adjacentCells);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->adjacentCells.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&ac, sizeof(gxAdjacentCell))) != 0 ||
                    (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                    (ret = cosem_getUInt32(value->byteArr, &ac->cellId)) != 0 ||
                    (ret = cosem_getUInt8(value->byteArr, &ac->signalQuality)) != 0)
                {
                    break;
                }
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
        break;
    case 8:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTime(value->byteArr, &object->captureTime);
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp;
    dlmsVARIANT tmp2;
    dlmsVARIANT* it;
#endif //DLMS_IGNORE_MALLOC
    switch (index)
    {
    case 2:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->token);
#else
        bb_clear(&object->token);
        ret = bb_set2(&object->token, value->byteArr, 0, bb_size(value->byteArr));
#endif //DLMS_IGNORE_MALLOC
        break;
    case 3:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->time);
#else
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
#endif //DLMS_IGNORE_MALLOC
        break;
    case 4:
        arr_clear(&object->descriptions);
#ifdef DLMS_IGNORE_MALLOC
        gxTokenGatewayDescription* d;
        uint16_t count = arr_getCapacity(&object->descriptions);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
        {
            object->descriptions.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->descriptions, pos, (void**)&d, sizeof(gxTokenGatewayDescription))) != 0 ||
                    (ret = cosem_getOctectString2(value->byteArr, d->value, sizeof(d->value), &d->size)) != 0)
                {
                    break;
                }
            }
        }
#else
        if (value != NULL)
        {
            char* d;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_getByIndex(value->Arr, pos, &it)) != 0)
                {
                    return ret;
                }
                d = (char*)gxmalloc(it->strVal->size + 1);
                if (d == NULL)
                {
                    ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                    break;
                }
                memcpy(d, it->strVal->data, it->strVal->size);
                d[it->strVal->size] = '\0';
                arr_push(&object->descriptions, d);
            }
        }
#endif //DLMS_IGNORE_MALLOC
        break;
    case 5:
        object->deliveryMethod = (DLMS_TOKEN_DELIVERY)var_toInteger(value);
        break;
    case 6:
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
            (ret = cosem_getEnum(value->byteArr, (unsigned char*)&object->status)) != 0 ||
            (ret = cosem_getBitString(value->byteArr, &object->dataValue)) != 0)
        {
            //Result code is returned at the end of the method.
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
    gxTarget* kv;
    unsigned char tmp2[100];
    gxByteBuffer bb;
    bb_attach(&bb, tmp2, 0, sizeof(tmp2));
#else
    gxKey* kv;
#endif //DLMS_IGNORE_MALLOC
    bb_clear(&object->buffer);
    bb_clear(&object->templateDescription);
    e.action = 1;
    e.target = &object->base;
    e.index = 2;
    vec_init(&args);
    bb_init(&tmp);
#ifndef DLMS_IGNORE_MALLOC
    vec_push(&args, &e);
#else
    unsigned char tmp3[100];
    bb_attach(&tmp, tmp3, 0, sizeof(tmp3));
#endif //DLMS_IGNORE_MALLOC
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
            if ((ret = arr_getByIndex(&object->captureObjects, pos, (void**)&kv, sizeof(gxTarget))) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
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
#else
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
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->buffer);
#else
        bb_clear(&object->buffer);
        if (value->byteArr != NULL)
        {
            ret = bb_set(&object->buffer, value->byteArr->data, value->byteArr->size);
        }
#endif //DLMS_IGNORE_MALLOC
        break;
    case 3:
        ret = setCaptureObjects(settings, &object->captureObjects, value);
        if (ret == 0 && settings->server)
        {
            ret = compactData_updateTemplateDescription(settings, object);
        }
        break;
    case 4:
        object->templateId = var_toInteger(value);
        break;
    case 5:
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString(value->byteArr, &object->templateDescription);
#else
        bb_clear(&object->templateDescription);
        if (value->byteArr != NULL)
        {
            ret = bb_set(&object->templateDescription, value->byteArr->data, value->byteArr->size);
        }
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_MALLOC
    dlmsVARIANT* tmp, * tmp3;
    dlmsVARIANT tmp2;
    DLMS_OBJECT_TYPE type;
#endif //DLMS_IGNORE_MALLOC
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
#ifdef DLMS_IGNORE_MALLOC
        unsigned char ln[6];
        uint16_t type;
        if ((ret = cosem_checkStructure(value->byteArr, 4)) == 0 &&
            (ret = cosem_getUInt16(value->byteArr, &type)) == 0 &&
            (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) == 0 &&
            (ret = cosem_getInt8(value->byteArr, &object->changedParameter.attributeIndex)) == 0 &&
            (ret = cosem_getVariant(value->byteArr, &object->changedParameter.value)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = oa_findByLN(&settings->objects, type, ln, &object->changedParameter.target);
#else
            object->changedParameter.type = type;
            memcpy(object->changedParameter.logicalName, ln, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
        break;
    }
    case 3:
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getDateTimeFromOctectString(value->byteArr, &object->captureTime);
#else
        //Some meters are returning empty octect string here.
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
#endif //DLMS_IGNORE_MALLOC
    }
    break;
    case 4:
    {
#ifdef DLMS_IGNORE_MALLOC
        uint16_t type;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        gxTarget* it;
        uint16_t count = arr_getCapacity(&object->parameters);
        if ((ret = cosem_checkArray(value->byteArr, &count)) == 0) {
            object->parameters.size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(&object->parameters, pos, (void**)&it, sizeof(gxTarget))) != 0)
                {
                    break;
                }
                if ((ret = cosem_checkStructure(value->byteArr, 3)) != 0 ||
                    (ret = cosem_getUInt16(value->byteArr, (uint16_t*)&type)) != 0 ||
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getOctectString2(value->byteArr, it->logicalName, 6, NULL)) != 0 ||
#else
                    (ret = cosem_getOctectString2(value->byteArr, ln, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_getInt8(value->byteArr, &it->attributeIndex)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = oa_findByLN(&settings->objects, type, ln, &it->target)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
            }
        }
#else
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
#endif //DLMS_IGNORE_MALLOC
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
        object->serviceNodeAddress = var_toInteger(value);
        break;
    case 3:
        object->baseNodeAddress = var_toInteger(value);
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
        object->crcIncorrectCount = var_toInteger(value);
        break;
    case 3:
        object->crcFailedCount = var_toInteger(value);
        break;
    case 4:
        object->txDropCount = var_toInteger(value);
        break;
    case 5:
        object->rxDropCount = var_toInteger(value);
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
        object->macMinSwitchSearchTime = var_toInteger(value);
        break;
    case 3:
        object->macMaxPromotionPdu = var_toInteger(value);
        break;
    case 4:
        object->macPromotionPduTxPeriod = var_toInteger(value);
        break;
    case 5:
        object->macBeaconsPerFrame = var_toInteger(value);
        break;
    case 6:
        object->macScpMaxTxAttempts = var_toInteger(value);
        break;
    case 7:
        object->macCtlReTxTimer = var_toInteger(value);
        break;
    case 8:
        object->macMaxCtlReTx = var_toInteger(value);
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
        object->lnId = var_toInteger(value);
        break;
    case 3:
        object->lsId = var_toInteger(value);
        break;
    case 4:
        object->sId = var_toInteger(value);
        break;
    case 5:
        object->sna.size = 0;
        ret = bb_set2(&object->sna, value->byteArr, 0, bb_size(value->byteArr));
        break;
    case 6:
        object->state = (DLMS_MAC_STATE)var_toInteger(value);
        break;
    case 7:
        object->scpLength = var_toInteger(value);
        break;
    case 8:
        object->nodeHierarchyLevel = var_toInteger(value);
        break;
    case 9:
        object->beaconSlotCount = var_toInteger(value);
        break;
    case 10:
        object->beaconRxSlot = var_toInteger(value);
        break;
    case 11:
        object->beaconTxSlot = var_toInteger(value);
        break;
    case 12:
        object->beaconRxFrequency = var_toInteger(value);
        break;
    case 13:
        object->beaconTxFrequency = var_toInteger(value);
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
#ifdef DLMS_IGNORE_MALLOC
    uint16_t count = arr_getCapacity(&object->multicastEntries);
    if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
    {
        object->multicastEntries.size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(&object->multicastEntries, pos, (void**)&it, sizeof(gxMacMulticastEntry))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 2)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->id)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->members)) != 0)
            {
                break;
            }
        }
    }
#else
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
            it->id = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->members = var_toInteger(tmp2);
            arr_push(&object->multicastEntries, it);
        }
    }
#endif //DLMS_IGNORE_MALLOC
    return 0;
}


int cosem_setSwitchTable(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    arr_empty(&object->switchTable);
#ifdef DLMS_IGNORE_MALLOC
    short* it;
    uint16_t count = arr_getCapacity(&object->switchTable);
    if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
    {
        object->switchTable.size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(&object->switchTable, pos, (void**)&it, sizeof(short))) != 0 ||
                (ret = cosem_getInt16(value->byteArr, it)) != 0)
            {
                break;
            }
        }
    }
#else
    if (value->Arr != NULL)
    {
        dlmsVARIANT* it;
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            if ((ret = va_getByIndex(value->Arr, pos, &it)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            long val = var_toInteger(it);
            arr_push(&object->switchTable, (void*)val);
        }
    }
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

int cosem_setDirectTable(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    gxMacDirectTable* it;
    arr_clear(&object->directTable);
#ifdef DLMS_IGNORE_MALLOC
    uint16_t count = arr_getCapacity(&object->directTable);
    if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
    {
        object->directTable.size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(&object->directTable, pos, (void**)&it, sizeof(gxMacDirectTable))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 7)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->sourceSId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->sourceLnId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->sourceLcId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->destinationSId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->destinationLnId)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->destinationLcId)) != 0 ||
                (ret = cosem_getOctectString2(value->byteArr, it->did, sizeof(it->did), NULL)) != 0)
            {
                break;
            }
        }
    }
#else
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
            it->sourceSId = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 1, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->sourceLnId = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 2, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->sourceLcId = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 3, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->destinationSId = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 4, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->destinationLnId = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 5, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->destinationLcId = var_toInteger(tmp2);
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
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

int cosem_setAvailableSwitches(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    gxMacAvailableSwitch* it;
    obj_clearAvailableSwitches(&object->availableSwitches);
#ifdef DLMS_IGNORE_MALLOC
    uint16_t count = arr_getCapacity(&object->availableSwitches);
    if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
    {
        object->availableSwitches.size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(&object->availableSwitches, pos, (void**)&it, sizeof(gxMacAvailableSwitch))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 5)) != 0 ||
                (ret = cosem_getOctectString2(value->byteArr, it->sna.data, sizeof(it->sna), NULL)) != 0 ||
                (ret = cosem_getInt16(value->byteArr, &it->lsId)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->level)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->rxLevel)) != 0 ||
                (ret = cosem_getInt8(value->byteArr, &it->rxSnr)) != 0)
            {
                break;
            }
        }
    }
#else
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
            bb_init(&it->sna);
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
            it->lsId = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 2, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            it->level = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 3, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            it->rxLevel = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 4, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                gxfree(it);
                break;
            }
            it->rxSnr = var_toInteger(tmp2);
            arr_push(&object->availableSwitches, it);
        }
    }
#endif //DLMS_IGNORE_MALLOC
    return 0;
}

int cosem_setCommunications(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, dlmsVARIANT* value)
{
    int ret = 0;
    int pos;
    gxMacPhyCommunication* it;
    arr_clear(&object->communications);
#ifdef DLMS_IGNORE_MALLOC
    uint16_t count = arr_getCapacity(&object->communications);
    if ((ret = cosem_checkArray(value->byteArr, &count)) == 0)
    {
        object->communications.size = count;
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(&object->communications, pos, (void**)&it, sizeof(gxMacPhyCommunication))) != 0 ||
                (ret = cosem_checkStructure(value->byteArr, 9)) != 0 ||
                (ret = cosem_getOctectString2(value->byteArr, it->eui, sizeof(it->eui), NULL)) != 0 ||
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
#else
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
            it->txPower = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 2, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->txCoding = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 3, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->rxCoding = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 4, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->rxLvl = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 5, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->snr = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 6, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->txPowerModified = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 7, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->txCodingModified = var_toInteger(tmp2);
            if ((ret = va_getByIndex(tmp->Arr, 8, &tmp2)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            it->rxCodingModified = var_toInteger(tmp2);
            arr_push(&object->communications, it);
        }
    }
#endif //DLMS_IGNORE_MALLOC
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
        int len = bb_size(value->byteArr);
#ifdef DLMS_IGNORE_MALLOC
        ret = bb_get(value->byteArr, (unsigned char*)object->firmwareVersion, len);
        object->firmwareVersion[len] = 0;
#else
        gxfree(object->firmwareVersion);
        if (len == 0)
        {
            object->firmwareVersion = NULL;
        }
        else
        {
            object->firmwareVersion = gxmalloc(len + 1);
            if (object->firmwareVersion == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            else
            {
                ret = bb_get(value->byteArr, (unsigned char*)object->firmwareVersion, len);
                object->firmwareVersion[len] = 0;
            }
        }
#endif //DLMS_IGNORE_MALLOC
    }
    break;
    case 3:
        object->vendorId = var_toInteger(value);
        break;
    case 4:
        object->productId = var_toInteger(value);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
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

int cosem_setValue(dlmsSettings* settings, gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    if (e->index == 1)
    {
#ifdef DLMS_IGNORE_MALLOC
        ret = cosem_getOctectString2(e->value.byteArr, e->target->logicalName, 6, NULL);
#else
        if (e->value.byteArr == NULL || e->value.byteArr->size - e->value.byteArr->position != 6)
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        else
        {
            ret = bb_get(e->value.byteArr, e->target->logicalName, 6);
        }
#endif //DLMS_IGNORE_MALLOC
        return ret;
    }
    switch (e->target->objectType)
    {
#ifndef DLMS_IGNORE_DATA
    case DLMS_OBJECT_TYPE_DATA:
        ret = cosem_setData((gxData*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = cosem_setRegister((gxRegister*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = cosem_setClock(settings, (gxClock*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = cosem_setActionSchedule(settings, (gxActionSchedule*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = cosem_setActivityCalendar(settings, (gxActivityCalendar*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = cosem_setAssociationLogicalName(settings, (gxAssociationLogicalName*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        ret = cosem_setAssociationShortName(settings, (gxAssociationShortName*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = cosem_setAutoAnswer((gxAutoAnswer*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = cosem_setAutoConnect((gxAutoConnect*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = cosem_setDemandRegister((gxDemandRegister*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = cosem_setMacAddressSetup((gxMacAddressSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = cosem_setExtendedRegister((gxExtendedRegister*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        ret = cosem_setGprsSetup((gxGPRSSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = cosem_setSecuritySetup((gxSecuritySetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = cosem_setIecHdlcSetup((gxIecHdlcSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = cosem_setIecLocalPortSetup((gxLocalPortSetup*)e->target, e->index, &e->value);
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
        ret = cosem_setIP4Setup(settings, (gxIp4Setup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = cosem_setMbusSlavePortSetup((gxMbusSlavePortSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = cosem_setImageTransfer((gxImageTransfer*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = cosem_setDisconnectControl((gxDisconnectControl*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = cosem_setLimiter(settings, (gxLimiter*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = cosem_setmMbusClient(settings, (gxMBusClient*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = cosem_setModemConfiguration((gxModemConfiguration*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = cosem_setPppSetup(settings, (gxPppSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = cosem_setProfileGeneric(settings, (gxProfileGeneric*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = cosem_setRegisterActivation(settings, (gxRegisterActivation*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = cosem_setRegisterMonitor(settings, (gxRegisterMonitor*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_REGISTER_TABLE
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        ret = cosem_setRegistertable((gxRegisterTable*)e->target, e->index, &e->value);
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
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = cosem_setSapAssignment((gxSapAssignment*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = cosem_setSchedule(settings, (gxSchedule*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = cosem_setScriptTable(settings, (gxScriptTable*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = cosem_setSpecialDaysTable((gxSpecialDaysTable*)e->target, e->index, &e->value);
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
        ret = cosem_setTcpUdpSetup(settings, (gxTcpUdpSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = cosem_setUtilityTables((gxUtilityTables*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = cosem_setMbusMasterPortSetup((gxMBusMasterPortSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        ret = cosem_setMessageHandler((gxMessageHandler*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = cosem_setPushSetup(settings, (gxPushSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_DATA_PROTECTION
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_DATA_PROTECTION
#ifndef DLMS_IGNORE_ACCOUNT
    case DLMS_OBJECT_TYPE_ACCOUNT:
        ret = cosem_setAccount((gxAccount*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = cosem_setCredit((gxCredit*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = cosem_setCharge(settings, (gxCharge*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = cosem_setTokenGateway((gxTokenGateway*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = cosem_setGsmDiagnostic((gxGsmDiagnostic*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = cosem_setCompactData(settings, (gxCompactData*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        ret = cosem_setParameterMonitor(settings, (gxParameterMonitor*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = cosem_setLlcSscsSetup(settings, (gxLlcSscsSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = cosem_setPrimeNbOfdmPlcPhysicalLayerCounters(settings, (gxPrimeNbOfdmPlcPhysicalLayerCounters*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = cosem_setPrimeNbOfdmPlcMacSetup(settings, (gxPrimeNbOfdmPlcMacSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = cosem_setPrimeNbOfdmPlcMacFunctionalParameters(settings, (gxPrimeNbOfdmPlcMacFunctionalParameters*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = cosem_setPrimeNbOfdmPlcMacCounters(settings, (gxPrimeNbOfdmPlcMacCounters*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = cosem_setPrimeNbOfdmPlcMacNetworkAdministrationData(settings, (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = cosem_setPrimeNbOfdmPlcApplicationsIdentification(settings, (gxPrimeNbOfdmPlcApplicationsIdentification*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = cosem_setTariffPlan((gxTariffPlan*)e->target, e->index, &e->value);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default:
        //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
