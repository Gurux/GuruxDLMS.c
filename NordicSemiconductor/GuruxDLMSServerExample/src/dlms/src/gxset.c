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

int cosem_setData(gxData* object, unsigned char index, dlmsVARIANT *value)
{
    if (index == 2)
    {
        return var_copy(&object->value, value);
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}
#endif //DLMS_IGNORE_DATA

#ifndef DLMS_IGNORE_REGISTER
int cosem_setRegister(gxRegister* object, unsigned char index, dlmsVARIANT *value)
{
    int ret;
    dlmsVARIANT *tmp;
    if (index == 2)
    {
        return var_copy(&object->value, value);
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
        object->unitRead = 1;
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_REGISTER

#ifndef DLMS_IGNORE_REGISTER_TABLE
int cosem_setRegistertable(gxRegisterTable* object, unsigned char index, dlmsVARIANT *value)
{
    //TODO:
    int ret;
    dlmsVARIANT *tmp;
    if (index == 2)
    {
        //        return var_copy(&object->value, value);
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_REGISTER_TABLE

#ifndef DLMS_IGNORE_CLOCK
int cosem_setClock(gxClock* object, unsigned char index, dlmsVARIANT *value)
{
    int ret;
    dlmsVARIANT tmp;
    if (index == 2)
    {
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
            var_clear(&tmp);
        }
        else
        {
            time_clear(&object->time);
        }
    }
    else if (index == 3)
    {
        object->timeZone = (unsigned short)var_toInteger(value);
    }
    else if (index == 4)
    {
        object->status = (DLMS_CLOCK_STATUS)var_toInteger(value);
    }
    else if (index == 5)
    {
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
    }
    else if (index == 6)
    {
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
    }
    else if (index == 7)
    {
        object->deviation = (char)var_toInteger(value);
    }
    else if (index == 8)
    {
        object->enabled = (unsigned char)var_toInteger(value);
    }
    else if (index == 9)
    {
        object->clockBase = (DLMS_CLOCK_BASE)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_CLOCK

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR

int updateSeasonProfile(gxArray* profile, variantArray* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxSeasonProfile* sp;
    dlmsVARIANT tm;
    dlmsVARIANT *tmp, *it;
    obj_clearSeasonProfile(profile);
    var_init(&tm);
    for (pos = 0; pos != data->size; ++pos)
    {
        sp = (gxSeasonProfile*)gxmalloc(sizeof(gxSeasonProfile));
        ret = va_get(data, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(sp);
            return ret;
        }
        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(sp);
            return ret;
        }
        bb_init(&sp->name);
        bb_set2(&sp->name, tmp->byteArr, 0, bb_size(tmp->byteArr));

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(sp);
            return ret;
        }
        ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &tm);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(sp);
            return ret;
        }
        time_copy(&sp->start, tm.dateTime);
        var_clear(&tm);

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(sp);
            return ret;
        }
        bb_init(&sp->weekName);
        bb_set2(&sp->weekName, tmp->byteArr, 0, bb_size(tmp->byteArr));
        arr_push(profile, sp);
    }
    return ret;
}

int updateWeekProfileTable(gxArray* profile, variantArray* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    gxWeekProfile* wp;
    dlmsVARIANT *tmp, *it;
    obj_clearWeekProfileTable(profile);
    for (pos = 0; pos != data->size; ++pos)
    {
        wp = (gxWeekProfile*)gxmalloc(sizeof(gxWeekProfile));
        ret = va_get(data, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        bb_init(&wp->name);
        bb_set2(&wp->name, tmp->byteArr, 0, bb_size(tmp->byteArr));

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        wp->monday = (unsigned char)var_toInteger(tmp);

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        wp->tuesday = (unsigned char)var_toInteger(tmp);

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        wp->wednesday = (unsigned char)var_toInteger(tmp);

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        wp->thursday = (unsigned char)var_toInteger(tmp);

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        wp->friday = (unsigned char)var_toInteger(tmp);

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        wp->saturday = (unsigned char)var_toInteger(tmp);

        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(wp);
            return ret;
        }
        wp->sunday = (unsigned char)var_toInteger(tmp);
        arr_push(profile, wp);
    }
    return ret;
}

int updateDayProfileTableActive(gxArray* profile, variantArray* data)
{
    int ret = DLMS_ERROR_CODE_OK, pos, pos2;
    dlmsVARIANT *tmp, *tmp2, *it, *it2;
    dlmsVARIANT tm;
    gxDayProfile* dp;
    gxDayProfileAction* ac;

    obj_clearDayProfileTable(profile);
    for (pos = 0; pos != data->size; ++pos)
    {
        dp = (gxDayProfile*)gxmalloc(sizeof(gxDayProfile));
        arr_init(&dp->daySchedules);
        ret = va_get(data, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(dp);
            return ret;
        }
        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(dp);
            return ret;
        }
        dp->dayId = (unsigned char)var_toInteger(tmp);
        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            gxfree(dp);
            return ret;
        }
        for (pos2 = 0; pos2 != tmp->Arr->size; ++pos2)
        {
            ac = (gxDayProfileAction*)gxmalloc(sizeof(gxDayProfileAction));
            ret = va_get(tmp->Arr, &it2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                gxfree(dp);
                gxfree(ac);
                return ret;
            }
            //Get start time.
            ret = va_get(it2->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                gxfree(dp);
                gxfree(ac);
                return ret;
            }
            var_init(&tm);
            ret = dlms_changeType2(tmp2, DLMS_DATA_TYPE_TIME, &tm);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                gxfree(dp);
                gxfree(ac);
                return ret;
            }
            time_copy(&ac->startTime, tm.dateTime);
            var_clear(&tm);
            //Get script logical name.
            ret = va_get(it2->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                gxfree(dp);
                gxfree(ac);
                return ret;
            }
            memcpy(ac->scriptLogicalName, tmp2->byteArr->data, 6);
            ret = va_get(it2->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                gxfree(dp);
                gxfree(ac);
                return ret;
            }
            ac->scriptSelector = (unsigned short)var_toInteger(tmp2);
            arr_push(&dp->daySchedules, ac);
        }
        arr_push(profile, dp);
    }
    return ret;
}

int cosem_setActivityCalendar(gxActivityCalendar* object, unsigned char index, dlmsVARIANT *value)
{
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT tm;
    if (index == 2)
    {
        bb_clear(&object->calendarNameActive);
        bb_set2(&object->calendarNameActive, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 3)
    {
        ret = updateSeasonProfile(&object->seasonProfileActive, value->Arr);
    }
    else if (index == 4)
    {
        ret = updateWeekProfileTable(&object->weekProfileTableActive, value->Arr);
    }
    else if (index == 5)
    {
        ret = updateDayProfileTableActive(&object->dayProfileTableActive, value->Arr);
    }
    else if (index == 6)
    {
        bb_clear(&object->calendarNamePassive);
        bb_set2(&object->calendarNamePassive, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 7)
    {
        ret = updateSeasonProfile(&object->seasonProfilePassive, value->Arr);
    }
    else if (index == 8)
    {
        ret = updateWeekProfileTable(&object->weekProfileTablePassive, value->Arr);
    }
    else if (index == 9)
    {
        ret = updateDayProfileTableActive(&object->dayProfileTablePassive, value->Arr);
    }
    else if (index == 10)
    {
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
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif // DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int cosem_setActionSchedule(
    gxActionSchedule* object,
    unsigned char index,
    dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp2;
    dlmsVARIANT time, date;
    gxtime* tm;
    if (index == 2)
    {
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        memcpy(object->executedScriptLogicalName, tmp->byteArr->data, 6);
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->executedScriptSelector = (unsigned short)var_toInteger(tmp);
    }
    else if (index == 3)
    {
        object->type = (DLMS_SINGLE_ACTION_SCHEDULE_TYPE)var_toInteger(value);
    }
    else if (index == 4)
    {
        arr_clear(&object->executionTime);
        var_init(&time);
        var_init(&date);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = dlms_changeType2(tmp2, DLMS_DATA_TYPE_TIME, &time);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(tmp2, DLMS_DATA_TYPE_DATE, &date);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                date.dateTime->value.tm_hour = time.dateTime->value.tm_hour;
                date.dateTime->value.tm_min = time.dateTime->value.tm_min;
                date.dateTime->value.tm_sec = time.dateTime->value.tm_sec;
                date.dateTime->skip = (DATETIME_SKIPS)(date.dateTime->skip & time.dateTime->skip);
                tm = (gxtime*)gxmalloc(sizeof(gxtime));
                time_copy(tm, date.dateTime);
                arr_push(&object->executionTime, tm);
                var_clear(&time);
                var_clear(&date);
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE

int cosem_setAssociationLogicalName(
    dlmsSettings* settings,
    gxAssociationLogicalName* object,
    unsigned char index,
    dlmsVARIANT *value)
{
    int ret, pos = 0, version;
    dlmsVARIANT *tmp, *tmp2;
    //gxByteBuffer bb;
    DLMS_OBJECT_TYPE type;
    gxObject* obj = NULL;
    unsigned char ch;
    if (index == 2)
    {
        oa_empty(&object->objectList);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                type = (DLMS_OBJECT_TYPE)var_toInteger(tmp2);
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                version = var_toInteger(tmp2);

                //Get Logical name.
                ret = va_get(tmp->Arr, &tmp2);
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
                }
                ret = va_get(tmp->Arr, &tmp2);
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
    }
    else if (index == 3)
    {
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->clientSAP = (unsigned char)var_toInteger(tmp);
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->serverSAP = (unsigned short)var_toInteger(tmp);
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
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.jointIsoCtt = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.country = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.countryName = (unsigned short)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.identifiedOrganization = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.dlmsUA = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.applicationContext = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->applicationContextName.contextId = (unsigned char)var_toInteger(tmp);
        }
    }
    else if (index == 5)
    {
        int val;
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if ((ret = ba_toInteger(tmp->bitArr, &val)) != 0)
        {
            return ret;
        }
        object->xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)val;
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.maxReceivePduSize = (unsigned short)var_toInteger(tmp);

        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.maxSendPpuSize = (unsigned short)var_toInteger(tmp);

        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.dlmsVersionNumber = (unsigned char)var_toInteger(tmp);

        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->xDLMSContextInfo.qualityOfService = (unsigned char)var_toInteger(tmp);

        ret = va_get(value->Arr, &tmp);
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
                ++pos;
                object->authenticationMechanismName.country = 0;
                ++pos;
                object->authenticationMechanismName.countryName = 0;
                //                    ++pos;
                object->authenticationMechanismName.identifiedOrganization = value->byteArr->data[++pos];
                object->authenticationMechanismName.dlmsUA = value->byteArr->data[++pos];
                object->authenticationMechanismName.authenticationMechanismName = value->byteArr->data[++pos];
                object->authenticationMechanismName.mechanismId = (DLMS_AUTHENTICATION)value->byteArr->data[++pos];
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

                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                object->authenticationMechanismName.jointIsoCtt = ch;
                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                object->authenticationMechanismName.country = ch;

                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x12)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt16(value->byteArr, &object->authenticationMechanismName.countryName);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                object->authenticationMechanismName.identifiedOrganization = ch;
                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }

                object->authenticationMechanismName.dlmsUA = ch;

                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                object->authenticationMechanismName.authenticationMechanismName = ch;
                //Get Tag.
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK || ch != 0x11)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                ret = bb_getUInt8(value->byteArr, &ch);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                object->authenticationMechanismName.mechanismId = (DLMS_AUTHENTICATION)ch;
            }
        }
        else  if (value->vt == DLMS_DATA_TYPE_STRUCTURE)
        {
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.jointIsoCtt = var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.country = var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.countryName = (unsigned short)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.identifiedOrganization = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.dlmsUA = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->authenticationMechanismName.authenticationMechanismName = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
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
            bb_set2(&object->secret, value->byteArr, 0, value->byteArr->size);
        }
    }
    else if (index == 8)
    {
        object->associationStatus = (DLMS_ASSOCIATION_STATUS)var_toInteger(value);
    }
    else if (index == 9)
    {
        if (value->byteArr == NULL || value->byteArr->size != 6)
        {
            ret = DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
        else
        {
            memcpy(object->securitySetupReference, value->byteArr->data, 6);
        }
    }
    else if (index == 10)
    {
        obj_clearUserList(&object->userList);
        if (value->Arr != NULL)
        {
            gxKey2* it;
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it = (gxKey2*)gxmalloc(sizeof(gxKey2));
                it->key = (unsigned char)var_toInteger(tmp2);
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it->value = gxmalloc(tmp->strVal->size + 1);
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
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->currentUser.key = (unsigned char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            if (tmp->strVal != NULL && tmp->strVal->size != 0)
            {
                object->currentUser.value = gxmalloc(tmp->strVal->size + 1);
                memcpy(object->currentUser.value, tmp->strVal, tmp->strVal->size);
            }
        }
        else
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int updateSNAccessRights(
    objectArray* objectList,
    variantArray* data)
{
    unsigned short sn;
    int pos, ret;
    dlmsVARIANT *it, *tmp;
    gxObject* obj = NULL;
    for (pos = 0; pos != data->size; ++pos)
    {
        ret = va_get(data, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_get(it->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        sn = (unsigned short)var_toInteger(tmp);

        ret = oa_findBySN(objectList, sn, &obj);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (obj != NULL)
        {
            ret = obj_updateAttributeAccessModes(obj, it->Arr);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            /*
            ret = va_get(it->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
            return ret;
            }
            //Update attribute Access
            for(pos2 = 0; pos2 != tmp->Arr.size; ++pos2)
            {
            ret = va_get(tmp->Arr, &access);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //Get attribute ID.
            ret = va_get(&access->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //TODO: id = var_toInteger(tmp2);
            //Get access mode.
            ret = va_get(&access->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //TODO: obj->SetAccess(id, (DLMS_ACCESS_MODE) var_toInteger(tmp2));
            }

            //Update method Access
            ret = va_get(it->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
            return ret;
            }
            for(pos2 = 0; pos2 != tmp->Arr.size; ++pos2)
            {
            ret = va_get(tmp->Arr, &access);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //Get attribute ID.
            ret = va_get(&access->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            //TODO: id = var_toInteger(tmp2);
            //Get access mode.
            ret = va_get(&access->Arr, &tmp2);
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
    dlmsVARIANT *value)
{
    unsigned short sn;
    DLMS_OBJECT_TYPE type;
    int pos, ret, version;
    dlmsVARIANT *tmp, *tmp2;
    gxObject* obj = NULL;
    if (index == 2)
    {
        oa_empty(&object->objectList);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                sn = (unsigned short)var_toInteger(tmp2);
                ret = oa_findBySN(&settings->objects, sn, &obj);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if (obj == NULL)
                {
                    ret = va_get(tmp->Arr, &tmp2);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }

                    type = (DLMS_OBJECT_TYPE)var_toInteger(tmp2);

                    ret = va_get(tmp->Arr, &tmp2);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }

                    version = var_toInteger(tmp2);
                    ret = va_get(tmp->Arr, &tmp2);
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
                }
                oa_push(&object->objectList, obj);
            }
        }
    }
    else if (index == 3)
    {
        return updateSNAccessRights(&object->objectList, value->Arr);
    }
    else if (index == 4)
    {
        if (value->byteArr == NULL || value->byteArr->size != 6)
        {
            ret = DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
        else
        {
            memcpy(object->securitySetupReference, value->byteArr->data, 6);
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_AUTO_ANSWER
int cosem_setAutoAnswer(gxAutoAnswer* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp3;
    dlmsVARIANT start, end;
    gxtime* s, *e;
    if (index == 2)
    {
        object->mode = (DLMS_AUTO_CONNECT_MODE)var_toInteger(value);
    }
    else if (index == 3)
    {
        arr_clearKeyValuePair(&object->listeningWindow);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
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
                ret = va_get(tmp->Arr, &tmp3);
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_AUTO_ANSWER

#ifndef DLMS_IGNORE_AUTO_CONNECT
int cosem_setAutoConnect(gxAutoConnect* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp3;
    dlmsVARIANT start, end;
    gxByteBuffer* str;
    gxtime* s, *e;
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
        object->repetitionDelay = (unsigned short)var_toInteger(value);
    }
    else if (index == 5)
    {
        arr_clearKeyValuePair(&object->callingWindow);
        if (value->Arr != NULL)
        {
            arr_capacity(&object->callingWindow, value->Arr->size);
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
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
                ret = va_get(tmp->Arr, &tmp3);
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
                arr_push(&object->callingWindow, key_init(s, e));
                var_clear(&start);
                var_clear(&end);
            }
        }
    }
    else if (index == 6)
    {
        arr_clearStrings(&object->destinations);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
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
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int cosem_setDemandRegister(gxDemandRegister* object, unsigned char index, dlmsVARIANT *value)
{
    int ret;
    dlmsVARIANT *tmp;
    dlmsVARIANT tmp2;
    if (index == 2)
    {
        if (object->scaler != 0)
        {
            ret = var_setDouble(&object->currentAvarageValue, var_toDouble(value) * hlp_getScaler(object->scaler));
        }
        else
        {
            ret = var_copy(&object->currentAvarageValue, value);
        }
    }
    else if (index == 3)
    {
        if (object->scaler != 0)
        {
            ret = var_setDouble(&object->lastAvarageValue, var_toDouble(value) * hlp_getScaler(object->scaler));
        }
        else
        {
            ret = var_copy(&object->lastAvarageValue, value);
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
    else if (index == 5)
    {
        ret = var_copy(&object->status, value);
    }
    else if (index == 6)
    {
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
    }
    else if (index == 7)
    {
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
            time_clear(&object->captureTime);
        }
    }
    else if (index == 8)
    {
        object->period = var_toInteger(value);
    }
    else if (index == 9)
    {
        object->numberOfPeriods = (unsigned short)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER

#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int cosem_setMacAddressSetup(gxMacAddressSetup* object, unsigned char index, dlmsVARIANT *value)
{
    if (index == 2)
    {
        bb_clear(&object->macAddress);
        bb_set2(&object->macAddress, value->byteArr, 0, bb_size(value->byteArr));
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int cosem_setExtendedRegister(gxExtendedRegister* object, unsigned char index, dlmsVARIANT *value)
{
    int ret = DLMS_ERROR_CODE_OK;
    dlmsVARIANT *tmp;
    dlmsVARIANT tmp2;
    if (index == 2)
    {
        ret = var_copy(&object->base.value, value);
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
        object->base.scaler = (char)var_toInteger(tmp);
        ret = va_getByIndex(value->Arr, 1, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->base.unit = (unsigned char)var_toInteger(tmp);
        object->base.unitRead = 1;
    }
    else if (index == 4)
    {
        ret = var_copy(&object->status, value);
    }
    else if (index == 5)
    {
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            var_init(&tmp2);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp2);
            time_copy(&object->captureTime, tmp2.dateTime);
            var_clear(&tmp2);
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
int cosem_setGprsSetup(gxGPRSSetup* object, unsigned char index, dlmsVARIANT *value)
{
    int ret;
    dlmsVARIANT *tmp, *tmp3;
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
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.precedence = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.delay = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.reliability = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.peakThroughput = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->defaultQualityOfService.meanThroughput = (unsigned char)var_toInteger(tmp3);

        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.precedence = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.delay = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.reliability = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->requestedQualityOfService.peakThroughput = (unsigned char)var_toInteger(tmp3);
        ret = va_get(tmp->Arr, &tmp3);
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
int cosem_setSecuritySetup(gxSecuritySetup* object, unsigned char index, dlmsVARIANT *value)
{
    int pos, ret;
    gxCertificateInfo *it;
    dlmsVARIANT *tmp, *tmp3;
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
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_get(value->Arr, &tmp)) != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it = (gxCertificateInfo*)gxmalloc(sizeof(gxCertificateInfo));
                //entity
                if ((ret = va_getByIndex(tmp->Arr, 0, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    gxfree(it);
                    return ret;
                }
                it->entity = (DLMS_CERTIFICATE_ENTITY)var_toInteger(tmp3);
                //type
                if ((ret = va_getByIndex(tmp->Arr, 1, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    gxfree(it);
                    return ret;
                }
                it->type = (DLMS_CERTIFICATE_TYPE)var_toInteger(tmp3);
                //serialNumber
                if ((ret = va_getByIndex(tmp->Arr, 2, &tmp3)) != DLMS_ERROR_CODE_OK)
                {
                    gxfree(it);
                    return ret;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->serialNumber = gxmalloc(tmp3->byteArr->size + 1);
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
                    gxfree(it->serialNumber);
                    gxfree(it);
                    return ret;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->issuer = gxmalloc(tmp3->byteArr->size + 1);
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
                    gxfree(it->issuer);
                    gxfree(it->serialNumber);
                    gxfree(it);
                    return ret;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->subject = gxmalloc(tmp3->byteArr->size + 1);
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
                    gxfree(it->subject);
                    gxfree(it->issuer);
                    gxfree(it->serialNumber);
                    gxfree(it);
                    return ret;
                }
                if (tmp3->byteArr != NULL && tmp3->byteArr->size != 0)
                {
                    it->subjectAltName = gxmalloc(tmp3->byteArr->size + 1);
                    memcpy(it->subjectAltName, tmp3->byteArr->data, tmp3->byteArr->size);
                    it->subjectAltName[tmp3->byteArr->size] = 0;
                }
                else
                {
                    it->subjectAltName = NULL;
                }
                arr_push(&object->certificates, it);
            }
        }
        break;
    default:
        return DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_SECURITY_SETUP

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int cosem_setIecHdlcSetup(gxIecHdlcSetup* object, unsigned char index, dlmsVARIANT *value)
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
        object->maximumInfoLengthTransmit = (unsigned short)var_toInteger(value);
    }
    else if (index == 6)
    {
        object->maximumInfoLengthReceive = (unsigned short)var_toInteger(value);
    }
    else if (index == 7)
    {
        object->interCharachterTimeout = (unsigned short)var_toInteger(value);
    }
    else if (index == 8)
    {
        object->inactivityTimeout = (unsigned short)var_toInteger(value);
    }
    else if (index == 9)
    {
        object->deviceAddress = (unsigned short)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int cosem_setIecLocalPortSetup(gxLocalPortSetup* object, unsigned char index, dlmsVARIANT *value)
{
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
        bb_set2(&object->deviceAddress, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 7)
    {
        bb_clear(&object->password1);
        bb_set2(&object->password1, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 8)
    {
        bb_clear(&object->password2);
        bb_set2(&object->password2, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 9)
    {
        bb_clear(&object->password5);
        bb_set2(&object->password5, value->byteArr, 0, bb_size(value->byteArr));
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
int cosem_setIP4Setup(gxIp4Setup* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp3;
    gxip4SetupIpOption* ipItem;

    if (index == 2)
    {
        bb_clear(&object->dataLinkLayerReference);
        bb_set2(&object->dataLinkLayerReference, value->byteArr, 0, bb_size(value->byteArr));
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
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                tmp3 = (dlmsVARIANT *)gxmalloc(sizeof(dlmsVARIANT));
                var_copy(tmp, tmp3);
                va_push(&object->multicastIPAddress, tmp3);
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
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ipItem = (gxip4SetupIpOption*)gxmalloc(sizeof(gxip4SetupIpOption));
                bb_init(&ipItem->data);
                ipItem->type = (DLMS_IP_OPTION_TYPE)var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ipItem->length = (short)var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                bb_set(&ipItem->data, tmp3->byteArr->data, tmp3->byteArr->size);
                arr_push(&object->ipOptions, ipItem);
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int cosem_setMbusSlavePortSetup(gxMbusSlavePortSetup* object, unsigned char index, dlmsVARIANT *value)
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
int cosem_setDisconnectControl(gxDisconnectControl* object, unsigned char index, dlmsVARIANT *value)
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
int cosem_setLimiter(dlmsSettings* settings, gxLimiter* object, unsigned char index, dlmsVARIANT *value)
{
    DLMS_OBJECT_TYPE ot;
    int ret, pos;
    dlmsVARIANT *tmp, *tmp3;
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
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ot = (DLMS_OBJECT_TYPE)var_toInteger(tmp);
        //Get LN.
        ret = va_get(value->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        //Get attribute index.
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->selectedAttributeIndex = (unsigned char)var_toInteger(tmp);
        oa_findByLN(&settings->objects, ot, tmp3->byteArr->data, &object->monitoredValue);
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
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->emergencyProfile.id = (unsigned short)var_toInteger(tmp);
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        var_init(&tmp2);
        ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &tmp2);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        time_copy(&object->emergencyProfile.activationTime, tmp2.dateTime);
        var_clear(&tmp2);
        ret = va_get(value->Arr, &tmp);
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
                ret = va_get(value->Arr, &tmp);
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
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        memcpy(object->actionOverThreshold.logicalName, tmp3->byteArr->data, 6);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->actionOverThreshold.scriptSelector = (unsigned short)var_toInteger(tmp3);

        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        memcpy(object->actionUnderThreshold.logicalName, tmp3->byteArr->data, 6);
        ret = va_get(tmp->Arr, &tmp3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->actionUnderThreshold.scriptSelector = (unsigned short)var_toInteger(tmp3);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int cosem_setmMbusClient(gxMBusClient* object, unsigned char index, dlmsVARIANT *value)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    dlmsVARIANT *tmp, *tmp3;
    gxByteBuffer *start, *end;
    if (index == 2)
    {
        bb_clear(&object->mBusPortReference);
        bb_set2(&object->mBusPortReference, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 3)
    {
        arr_clearKeyValuePair(&object->captureDefinition);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                start = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                end = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                bb_init(start);
                bb_init(end);
                bb_set(start, tmp3->byteArr->data, tmp3->byteArr->size);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(start);
                    gxfree(end);
                    return ret;
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
        object->manufacturerID = (unsigned short)var_toInteger(value);
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
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int cosem_setModemConfiguration(gxModemConfiguration* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp3;
    gxModemInitialisation* modemInit;
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
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                modemInit = (gxModemInitialisation*)gxmalloc(sizeof(gxModemInitialisation));
                bb_init(&modemInit->request);
                bb_init(&modemInit->response);
                bb_set(&modemInit->request, tmp3->byteArr->data, tmp3->byteArr->size);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                bb_set(&modemInit->response, tmp3->byteArr->data, tmp3->byteArr->size);
                if (tmp->Arr->size > 2)
                {
                    ret = va_get(tmp->Arr, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
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
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                str = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                bb_init(str);
                bb_set2(str, tmp->byteArr, 0, bb_size(tmp->byteArr));
                arr_push(&object->modemProfile, str);
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
int cosem_setPppSetup(gxPppSetup* object, unsigned char index, dlmsVARIANT *value)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    dlmsVARIANT *tmp, *tmp3;
    gxpppSetupLcpOption* lcpItem;
    gxpppSetupIPCPOption* ipcpItem;

    if (index == 2)
    {
        bb_clear(&object->PHYReference);
        bb_set2(&object->PHYReference, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 3)
    {
        arr_clear(&object->lcpOptions);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                lcpItem = (gxpppSetupLcpOption*)gxmalloc(sizeof(gxpppSetupLcpOption));
                var_init(&lcpItem->data);
                lcpItem->type = (DLMS_PPP_SETUP_LCP_OPTION_TYPE)var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(lcpItem);
                    return ret;
                }

                lcpItem->length = var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(lcpItem);
                    return ret;
                }
                ret = var_copy(&lcpItem->data, tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(lcpItem);
                    return ret;
                }
                arr_push(&object->lcpOptions, lcpItem);
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
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ipcpItem = (gxpppSetupIPCPOption*)gxmalloc(sizeof(gxpppSetupIPCPOption));
                var_init(&ipcpItem->data);
                ipcpItem->type = (DLMS_PPP_SETUP_IPCP_OPTION_TYPE)var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(ipcpItem);
                    return ret;
                }
                ipcpItem->length = var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(ipcpItem);
                    return ret;
                }
                ret = var_copy(&ipcpItem->data, tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    gxfree(ipcpItem);
                    return ret;
                }
                arr_push(&object->ipcpOptions, ipcpItem);
            }
        }
    }
    else if (index == 5)
    {
        //Get user name.
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_clear(&object->userName);
        bb_set2(&object->userName, tmp->byteArr, 0, bb_size(tmp->byteArr));
        //Get password.
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_clear(&object->password);
        bb_set2(&object->password, tmp->byteArr, 0, bb_size(tmp->byteArr));
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int cosem_setRegisterActivation(gxRegisterActivation* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos, pos2;
    dlmsVARIANT *tmp, *tmp3;
    gxByteBuffer *start, *end;
    gxObjectDefinition* objectDefinition;

    if (index == 2)
    {
        obj_clearRegisterActivationAssignment(&object->registerAssignment);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                objectDefinition = (gxObjectDefinition*)gxmalloc(sizeof(gxObjectDefinition));
                objectDefinition->classId = (DLMS_OBJECT_TYPE)var_toInteger(tmp3);

                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                memcpy(objectDefinition->logicalName, tmp3->byteArr->data, 6);
                arr_push(&object->registerAssignment, objectDefinition);
            }
        }
    }
    else if (index == 3)
    {
        obj_clearRegisterActivationMaskList(&object->maskList);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                start = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                bb_init(start);
                bb_set(start, tmp3->byteArr->data, tmp3->byteArr->size);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                end = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                bb_init(end);
                for (pos2 = 0; pos2 != tmp3->Arr->size; ++pos2)
                {
                    ret = va_get(tmp3->Arr, &tmp);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    bb_setUInt8(end, (unsigned char)var_toInteger(tmp));
                }
                arr_push(&object->maskList, key_init(start, end));
            }
        }
    }
    else if (index == 4)
    {
        bb_clear(&object->activeMask);
        bb_set2(&object->activeMask, value->byteArr, 0, bb_size(value->byteArr));
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int cosem_setRegisterMonitor(gxRegisterMonitor* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp3, *tmp4;
    gxActionSet* actionSet;

    if (index == 2)
    {
        va_clear(&object->thresholds);
        if (value->Arr != NULL)
        {
            va_capacity(&object->thresholds, value->Arr->size);
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                tmp3 = (dlmsVARIANT *)gxmalloc(sizeof(dlmsVARIANT));
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
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->monitoredValue.objectType = (DLMS_OBJECT_TYPE)var_toInteger(tmp);
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        memcpy(object->monitoredValue.logicalName, tmp->byteArr->data, 6);
        ret = va_get(value->Arr, &tmp);
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
                ret = va_get(value->Arr, &tmp4);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                actionSet = (gxActionSet*)gxmalloc(sizeof(gxActionSet));
                //Update action up.
                ret = va_get(tmp4->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp3->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                memcpy(actionSet->actionUp.logicalName, tmp->byteArr->data, 6);

                ret = va_get(tmp3->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                actionSet->actionUp.scriptSelector = (unsigned short)var_toInteger(tmp);
                //Update action down.
                ret = va_get(tmp4->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp3->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                memcpy(actionSet->actionDown.logicalName, tmp->byteArr->data, 6);
                ret = va_get(tmp3->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                actionSet->actionDown.scriptSelector = (unsigned short)var_toInteger(tmp);
                arr_push(&object->actions, actionSet);
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int cosem_setSapAssignment(gxSapAssignment* object, unsigned char index, dlmsVARIANT *value)
{
    int ret = DLMS_ERROR_CODE_OK, pos;
    dlmsVARIANT *tmp, *tmp2;
    gxSapItem* it;
    if (index == 2)
    {
        obj_clearSapList(&object->sapAssignmentList);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it = (gxSapItem*)gxmalloc(sizeof(gxSapItem));
                bb_init(&it->name);
                it->id = (unsigned short)var_toInteger(tmp2);
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                bb_set(&it->name, tmp2->byteArr->data, tmp2->byteArr->size);
                arr_push(&object->sapAssignmentList, it);
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
int cosem_setSchedule(gxSchedule* object, unsigned char index, dlmsVARIANT *value)
{
    gxScheduleEntry* se;
    int ret, pos;
    dlmsVARIANT *tmp, *it;
    dlmsVARIANT tmp3;
    if (index == 2)
    {
        arr_clear(&object->entries);
        var_init(&tmp3);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                se = (gxScheduleEntry*)gxmalloc(sizeof(gxScheduleEntry));
                ba_init(&se->execWeekdays);
                ba_init(&se->execSpecDays);
                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                se->index = (unsigned short)var_toInteger(it);
                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                se->enable = (unsigned char)var_toInteger(it);

                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                memcpy(se->logicalName, it->byteArr->data, it->byteArr->size);

                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                se->scriptSelector = (unsigned short)var_toInteger(it);

                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                time_copy(&se->switchTime, tmp3.dateTime);

                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                se->validityWindow = (unsigned short)var_toInteger(it);

                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if (it->bitArr != NULL)
                {
                    ba_copy(&se->execWeekdays, it->bitArr->data, (unsigned short)it->bitArr->size);
                }
                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if (it->bitArr != NULL)
                {
                    ba_copy(&se->execSpecDays, it->bitArr->data, (unsigned short)it->bitArr->size);
                }
                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                time_copy(&se->beginDate, tmp3.dateTime);

                ret = va_get(tmp->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(it, DLMS_DATA_TYPE_DATE, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                time_copy(&se->endDate, tmp3.dateTime);
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int cosem_setScriptTable(gxScriptTable* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos, pos2;
    dlmsVARIANT *tmp, *tmp2, *tmp3;
    gxScriptAction* scriptAction;
    gxScript* script;

    if (index == 2)
    {
        obj_clearScriptTable(&object->scripts);
        if (value->Arr->size != 0)
        {
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
                    ret = va_get(value->Arr, &tmp);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    ret = va_get(tmp->Arr, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    script = (gxScript*)gxmalloc(sizeof(gxScript));
                    arr_init(&script->actions);
                    script->id = (unsigned short)var_toInteger(tmp3);
                    arr_push(&object->scripts, script);

                    ret = va_get(tmp->Arr, &tmp3);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    for (pos2 = 0; pos2 != tmp3->Arr->size; ++pos2)
                    {
                        ret = va_get(tmp3->Arr, &tmp2);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            return ret;
                        }
                        ret = va_get(tmp2->Arr, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            return ret;
                        }

                        scriptAction = (gxScriptAction*)gxmalloc(sizeof(gxScriptAction));
                        var_init(&scriptAction->parameter);
                        scriptAction->type = (DLMS_SCRIPT_ACTION_TYPE)var_toInteger(tmp);
                        ret = va_get(tmp2->Arr, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            return ret;
                        }

                        scriptAction->objectType = (DLMS_OBJECT_TYPE)var_toInteger(tmp);
                        ret = va_get(tmp2->Arr, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            return ret;
                        }
                        memcpy(scriptAction->logicalName, tmp->byteArr, 6);
                        ret = va_get(tmp2->Arr, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            return ret;
                        }
                        scriptAction->index = (char)var_toInteger(tmp);
                        ret = va_get(tmp2->Arr, &tmp);
                        if (ret != DLMS_ERROR_CODE_OK)
                        {
                            return ret;
                        }

                        var_copy(&scriptAction->parameter, tmp);
                        arr_push(&script->actions, scriptAction);
                    }
                }
            }
            else //Read Xemex meter here.
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp2->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                script = (gxScript*)gxmalloc(sizeof(gxScript));
                arr_init(&script->actions);
                script->id = (unsigned short)var_toInteger(tmp3);
                arr_push(&object->scripts, script);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                scriptAction = (gxScriptAction*)gxmalloc(sizeof(gxScriptAction));
                var_init(&scriptAction->parameter);
                scriptAction->type = (DLMS_SCRIPT_ACTION_TYPE)var_toInteger(tmp3);
                ret = va_get(tmp2->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                scriptAction->objectType = (DLMS_OBJECT_TYPE)var_toInteger(tmp3);
                ret = va_get(tmp2->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                memcpy(scriptAction->logicalName, tmp3->byteArr->data, 6);
                ret = va_get(tmp2->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                scriptAction->index = (char)var_toInteger(tmp3);
                ret = va_get(tmp2->Arr, &tmp3);
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int cosem_setSpecialDaysTable(gxSpecialDaysTable* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp3;
    dlmsVARIANT tmp2;
    gxSpecialDay* specialDay;

    if (index == 2)
    {
        arr_clear(&object->entries);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                specialDay = (gxSpecialDay*)gxmalloc(sizeof(gxSpecialDay));

                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                specialDay->index = (unsigned short)var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
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

                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                specialDay->dayId = (unsigned char)var_toInteger(tmp3);
                arr_push(&object->entries, specialDay);
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int cosem_setTcpUdpSetup(gxTcpUdpSetup* object, unsigned char index, dlmsVARIANT *value)
{
    if (index == 2)
    {
        object->port = (unsigned short)var_toInteger(value);
    }
    else if (index == 3)
    {
        bb_clear(&object->ipReference);
        bb_set2(&object->ipReference, value->byteArr, 0, bb_size(value->byteArr));
    }
    else if (index == 4)
    {
        object->maximumSegmentSize = (unsigned short)var_toInteger(value);
    }
    else if (index == 5)
    {
        object->maximumSimultaneousConnections = (unsigned char)var_toInteger(value);
    }
    else if (index == 6)
    {
        object->inactivityTimeout = (unsigned short)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int cosem_setMbusMasterPortSetup(gxMBusMasterPortSetup* object, unsigned char index, dlmsVARIANT *value)
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
int cosem_setMessageHandler(gxMessageHandler* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT *tmp, *tmp2;
    dlmsVARIANT start, end;
    gxtime* s, *e;
    if (index == 2)
    {
        var_init(&start);
        var_init(&end);
        arr_clear(&object->listeningWindow);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp2);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp2->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = dlms_changeType2(tmp, DLMS_DATA_TYPE_DATETIME, &start);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }

                ret = va_get(tmp2->Arr, &tmp);
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
    }
    else if (index == 3)
    {
        va_clear(&object->allowedSenders);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                tmp2 = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                var_init(tmp2);
                ret = var_copy(tmp2, tmp);
                if (ret != 0)
                {
                    gxfree(tmp2);
                    return ret;
                }
                va_push(&object->allowedSenders, tmp2);
            }
        }
    }
    else if (index == 4)
    {
        arr_clear(&object->sendersAndActions);
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
int cosem_setPushSetup(dlmsSettings* settings, gxPushSetup* object, unsigned char index, dlmsVARIANT *value)
{
    gxObject *obj;
    int ret, pos;
    gxCaptureObject* it;
    dlmsVARIANT *tmp, *tmp3;
    dlmsVARIANT start, end;
    gxtime *s, *e;
    DLMS_OBJECT_TYPE type;
    if (index == 2)
    {
        obj_clearPushObjectList(&object->pushObjectList);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                type = (DLMS_OBJECT_TYPE)var_toInteger(tmp3);
                //Get LN.

                ret = va_get(tmp->Arr, &tmp3);
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
                    memcpy(obj->logicalName, tmp3->byteArr->data, tmp3->byteArr->size);
                }
                it = (gxCaptureObject*)gxmalloc(sizeof(gxCaptureObject));

                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it->attributeIndex = (unsigned char)var_toInteger(tmp3);
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                it->dataIndex = (unsigned char)var_toInteger(tmp3);
                arr_push(&object->pushObjectList, key_init(obj, it));
            }
        }
    }
    else if (index == 3)
    {
        if (object->destination != NULL)
        {
            gxfree(object->destination);
            object->destination = NULL;
        }
        //Get service.
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->service = (DLMS_SERVICE_TYPE)var_toInteger(tmp);
        //Destination.
        ret = va_get(value->Arr, &tmp);
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
        ret = va_get(value->Arr, &tmp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        object->message = (DLMS_MESSAGE_TYPE)var_toInteger(tmp);
    }
    else if (index == 4)
    {
        var_init(&start);
        var_init(&end);
        arr_clearKeyValuePair(&object->communicationWindow);
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {

                ret = va_get(value->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if ((ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &start)) != 0)
                {
                    return ret;
                }
                ret = va_get(tmp->Arr, &tmp3);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                if ((ret = dlms_changeType2(tmp3, DLMS_DATA_TYPE_DATETIME, &end)) != 0)
                {
                    return ret;
                }
                s = (gxtime*)gxmalloc(sizeof(gxtime));
                e = (gxtime*)gxmalloc(sizeof(gxtime));
                time_copy(s, start.dateTime);
                time_copy(e, end.dateTime);
                arr_push(&object->communicationWindow, key_init(s, e));
            }
        }
    }
    else if (index == 5)
    {
        object->randomisationStartInterval = (unsigned short)var_toInteger(value);
    }
    else if (index == 6)
    {
        object->numberOfRetries = (unsigned char)var_toInteger(value);
    }
    else if (index == 7)
    {
        object->repetitionDelay = (unsigned short)var_toInteger(value);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_CHARGE
int setUnitCharge(gxUnitCharge* target, dlmsVARIANT *value)
{
    gxChargeTable *ct;
    dlmsVARIANT *it, *it2, *tmp;
    int ret, pos;
    ret = obj_clearChargeTables(&target->chargeTables);
    if (ret != 0)
    {
        return ret;
    }

    //charge per unit scaling
    ret = va_get(value->Arr, &it);
    if (ret != 0)
    {
        return ret;
    }
    //commodity scale
    ret = va_get(it->Arr, &it2);
    if (ret != 0)
    {
        return ret;
    }
    target->chargePerUnitScaling.commodityScale = (char)var_toInteger(it2);
    //price scale
    ret = va_get(it->Arr, &it2);
    if (ret != 0)
    {
        return ret;
    }
    target->chargePerUnitScaling.priceScale = (char)var_toInteger(it2);
    //commodity
    ret = va_get(value->Arr, &it);
    if (ret != 0)
    {
        return ret;
    }
    //type
    ret = va_get(it->Arr, &it2);
    if (ret != 0)
    {
        return ret;
    }
    target->commodity.type = (DLMS_OBJECT_TYPE)var_toInteger(it2);
    //LN
    ret = va_get(it->Arr, &it2);
    if (ret != 0)
    {
        return ret;
    }
    memset(target->commodity.logicalName, 0, 6);
    if (it2->byteArr != NULL && it2->byteArr->size == 6)
    {
        memcpy(target->commodity.logicalName, it2->byteArr->data, 6);
    }
    //attribute index
    ret = va_get(it->Arr, &it2);
    if (ret != 0)
    {
        return ret;
    }
    target->commodity.attributeIndex = (unsigned char)var_toInteger(it2);
    //chargeTables
    ret = va_get(value->Arr, &it);
    if (ret != 0)
    {
        return ret;
    }
    for (pos = 0; pos != it->Arr->size; ++pos)
    {
        ret = va_get(it->Arr, &it2);
        if (ret != 0)
        {
            return ret;
        }
        ct = (gxChargeTable*)gxmalloc(sizeof(gxChargeTable));
        bb_init(&ct->index);
        //index
        ret = va_get(it2->Arr, &tmp);
        if (ret != 0)
        {
            return ret;
        }
        bb_set2(&ct->index, tmp->byteArr, 0, bb_size(tmp->byteArr));
        //chargePerUnit
        ret = va_get(it2->Arr, &tmp);
        if (ret != 0)
        {
            return ret;
        }
        ct->chargePerUnit = (short)var_toInteger(tmp);
        arr_push(&target->chargeTables, ct);
    }
    return ret;
}

int cosem_setCharge(gxCharge* object, unsigned char index, dlmsVARIANT *value)
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
        ret = setUnitCharge(&object->unitChargeActive, value);
    }
    else if (index == 6)
    {
        ret = setUnitCharge(&object->unitChargePassive, value);
    }
    else if (index == 7)
    {
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
    }
    else if (index == 8)
    {
        object->period = var_toInteger(value);
    }
    else if (index == 9)
    {
        ret = ba_copy(&object->chargeConfiguration, value->bitArr->data, (unsigned short)value->bitArr->size);
    }
    else if (index == 10)
    {
        //Some meters are returning empty octect string here.
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING && value->byteArr != NULL)
        {
            var_init(&tmp);
            ret = dlms_changeType2(value, DLMS_DATA_TYPE_DATETIME, &tmp);
            if (ret != 0)
            {
                return ret;
            }
            time_copy(&object->lastCollectionTime, tmp.dateTime);
            var_clear(&tmp);
        }
        else
        {
            time_clear(&object->lastCollectionTime);
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
        object->proportion = (unsigned short)var_toInteger(value);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_CREDIT
int cosem_setCredit(gxCredit* object, unsigned char index, dlmsVARIANT *value)
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
        ret = ba_copy(&object->creditConfiguration, value->bitArr->data, value->bitArr->size);
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
int cosem_setAccount(gxAccount* object, unsigned char index, dlmsVARIANT *value)
{
    int ret = 0, pos;
    unsigned char *ba;
    dlmsVARIANT *it, *tmp2;
    dlmsVARIANT tmp;
    gxCreditChargeConfiguration *ccc;
    gxTokenGatewayConfiguration *gwc;

    if (index == 2)
    {
        //payment mode
        ret = va_get(value->Arr, &it);
        if (ret != 0)
        {
            return ret;
        }
        object->paymentMode = (DLMS_ACCOUNT_PAYMENT_MODE)var_toInteger(it);
        //account status
        ret = va_get(value->Arr, &it);
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
            int v;
            if ((ret = ba_toInteger(value->bitArr, &v)) != 0)
            {
                return DLMS_ERROR_CODE_READ_WRITE_DENIED;
            }
            object->currentCreditStatus = (DLMS_ACCOUNT_CREDIT_STATUS)v;
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
            ret = va_get(value->Arr, &it);
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
            ret = va_get(value->Arr, &it);
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
        obj_clearCreditChargeConfigurations(&object->creditChargeConfigurations);
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_get(value->Arr, &it);
            if (ret != 0)
            {
                return ret;
            }
            ccc = (gxCreditChargeConfiguration*)gxmalloc(sizeof(gxCreditChargeConfiguration));
            ccc->collectionConfiguration = (DLMS_CREDIT_COLLECTION_CONFIGURATION)0;

            //credit reference
            ret = va_get(it->Arr, &tmp2);
            if (ret != 0)
            {
                gxfree(ccc);
                return ret;
            }
            memcpy(ccc->creditReference, tmp2->byteArr->data, tmp2->byteArr->size);
            //charge reference
            ret = va_get(it->Arr, &tmp2);
            if (ret != 0)
            {
                gxfree(ccc);
                return ret;
            }
            memcpy(ccc->chargeReference, tmp2->byteArr->data, tmp2->byteArr->size);
            //collection configuration
            ret = va_get(it->Arr, &tmp2);
            if (ret != 0)
            {
                gxfree(ccc);
                return ret;
            }
            if (tmp2->bitArr == NULL || tmp2->bitArr->size == 0)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            int v;
            if ((ret = ba_toInteger(tmp2->bitArr, &v)) != 0)
            {
                return DLMS_ERROR_CODE_READ_WRITE_DENIED;
            }
            ccc->collectionConfiguration = (DLMS_CREDIT_COLLECTION_CONFIGURATION)v;
            arr_push(&object->creditChargeConfigurations, ccc);
        }
    }
    else if (index == 12)
    {
        obj_clearTokenGatewayConfigurations(&object->tokenGatewayConfigurations);
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_get(value->Arr, &it);
            if (ret != 0)
            {
                return ret;
            }
            gwc = (gxTokenGatewayConfiguration*)gxmalloc(sizeof(gxTokenGatewayConfiguration));
            memset(gwc->creditReference, 0, 6);
            //credit reference
            ret = va_get(it->Arr, &tmp2);
            if (ret != 0)
            {
                return ret;
            }
            if (tmp2->byteArr->size == 6)
            {
                memcpy(gwc->creditReference, tmp2->byteArr->data, tmp2->byteArr->size);
            }
            //token proportion
            ret = va_get(it->Arr, &tmp2);
            if (ret != 0)
            {
                return ret;
            }
            gwc->tokenProportion = (unsigned char)var_toInteger(tmp2);
            arr_push(&object->tokenGatewayConfigurations, gwc);
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
        gxfree(object->currency.name);
        //Name
        ret = va_get(value->Arr, &it);
        if (ret != 0)
        {
            return ret;
        }
        if (it->strVal != NULL && it->strVal->size != 0)
        {
            object->currency.name = gxmalloc(it->strVal->size + 1);
            memcpy(object->currency.name, it->strVal->data, it->strVal->size);
            object->currency.name[it->strVal->size] = '\0';
        }
        //scale
        ret = va_get(value->Arr, &it);
        if (ret != 0)
        {
            return ret;
        }
        object->currency.scale = (char)var_toInteger(it);
        //unit
        ret = va_get(value->Arr, &it);
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
        object->maxProvision = (unsigned short)var_toInteger(value);
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
int cosem_setImageTransfer(gxImageTransfer* object, unsigned char index, dlmsVARIANT *value)
{
    int pos, ret;
    dlmsVARIANT *it, *tmp;
    gxImageActivateInfo* item;
    if (index == 2)
    {
        object->imageBlockSize = var_toInteger(value);
    }
    else if (index == 3)
    {
        ba_clear(&object->imageTransferredBlocksStatus);
        if (value->bitArr != NULL)
        {
            ba_copy(&object->imageTransferredBlocksStatus, value->bitArr->data, (unsigned short)value->bitArr->size);
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
        if (value->Arr != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_getByIndex(value->Arr, pos, &it);
                if (ret != 0)
                {
                    return ret;
                }
                item = (gxImageActivateInfo*)gxmalloc(sizeof(gxImageActivateInfo));
                bb_init(&item->identification);
                bb_init(&item->signature);
                ret = va_get(it->Arr, &tmp);
                if (ret != 0)
                {
                    gxfree(item);
                    return ret;
                }
                item->size = var_toInteger(tmp);
                ret = va_get(it->Arr, &tmp);
                if (ret != 0)
                {
                    gxfree(item);
                    return ret;
                }
                bb_set2(&item->identification, tmp->byteArr, 0, bb_size(tmp->byteArr));
                ret = va_get(it->Arr, &tmp);
                if (ret != 0)
                {
                    gxfree(item);
                    return ret;
                }
                bb_set2(&item->signature, tmp->byteArr, 0, bb_size(tmp->byteArr));
                arr_push(&object->imageActivateInfo, item);
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER

#if !defined(DLMS_IGNORE_PROFILE_GENERIC) && !defined(DLMS_IGNORE_CONPACT_DATA)
int setCaptureObjects(
    dlmsSettings* settings,
    gxArray* objects,
    dlmsVARIANT *value)
{
    DLMS_OBJECT_TYPE type;
    dlmsVARIANT *tmp, *tmp2;
    gxObject *obj;
    gxCaptureObject *co;
    int pos, ret;
    ret = obj_clearProfileGenericCaptureObjects(objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    arr_capacity(objects, value->Arr->size);
    if (value->Arr != NULL)
    {
        for (pos = 0; pos != value->Arr->size; ++pos)
        {
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            if (tmp->Arr->size != 4)
            {
                //Invalid structure format.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            ret = va_get(tmp->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            type = (DLMS_OBJECT_TYPE)var_toInteger(tmp2);
            //Get LN.
            ret = va_get(tmp->Arr, &tmp2);
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
                    return ret;
                }
                ret = cosem_setLogicalName(obj, tmp2->byteArr->data);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
            }
            co = (gxCaptureObject*)gxmalloc(sizeof(gxCaptureObject));
            ret = va_get(tmp->Arr, &tmp2);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            co->attributeIndex = (unsigned char)var_toInteger(tmp2);
            ret = va_get(tmp->Arr, &tmp2);
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
    return 0;
}
#endif //!defined(DLMS_IGNORE_PROFILE_GENERIC) && !defined(DLMS_IGNORE_CONPACT_DATA)

#ifndef DLMS_IGNORE_PROFILE_GENERIC
int cosem_setProfileGeneric(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    unsigned char index,
    dlmsVARIANT *value)
{
    static unsigned char UNIX_TIME[6] = { 0, 0, 1, 1, 0, 255 };

    int ret, pos, pos2;
    DLMS_OBJECT_TYPE type;
    dlmsVARIANT *tmp, *row, *data;
    variantArray *va;
    if (index == 2)
    {
        if (object->captureObjects.size == 0)
        {
            //Read capture objects first.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        ret = obj_clearProfileGenericBuffer(&object->buffer);
        if (ret != 0)
        {
            return ret;
        }
        if (value->Arr != NULL)
        {
            //Allocate array.
            arr_capacity(&object->buffer, value->Arr->size);
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &row);
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
                    ret = va_get(row->Arr, &data);
                    if (ret != DLMS_ERROR_CODE_OK)
                    {
                        return ret;
                    }
                    if (data->vt == DLMS_DATA_TYPE_OCTET_STRING || data->vt == DLMS_DATA_TYPE_UINT32)
                    {
                        gxKey* k;
                        if ((ret = arr_getByIndex(&object->captureObjects, pos2, (void**)&k)) != 0)
                        {
                            return ret;
                        }
                        //Some meters return NULL date time string.
                        if (data->vt == DLMS_DATA_TYPE_OCTET_STRING && ((gxObject*)k->key)->objectType == DLMS_OBJECT_TYPE_CLOCK && ((gxCaptureObject*)k->value)->attributeIndex == 2)
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
                        else if (data->vt == DLMS_DATA_TYPE_UINT32 && ((gxObject*)k->key)->objectType == DLMS_OBJECT_TYPE_DATA && ((gxCaptureObject*)k->value)->attributeIndex == 2 &&
                            memcmp(((gxObject*)k->key)->logicalName, UNIX_TIME, 6) == 0)
                        {
                            gxtime tmp4;
                            struct tm tmp5;
                            time_fromUnixTime((time_t)data->ulVal, &tmp5);
                            time_init2(&tmp4, &tmp5);
                            var_setDateTime(data, &tmp4);
                        }
                    }
                }
                //Attach rows from parser.
                va = (variantArray *)gxmalloc(sizeof(variantArray));
                va_init(va);
                va_attach(va, row->Arr);
                va->position = 0;
                arr_push(&object->buffer, va);
            }
        }
        object->entriesInUse = object->buffer.size;
        //Trim array.
        arr_capacity(&object->buffer, object->buffer.size);
    }
    else if (index == 3)
    {
        object->entriesInUse = 0;
        ret = obj_clearProfileGenericBuffer(&object->buffer);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        ret = setCaptureObjects(settings, &object->captureObjects, value);
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
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            type = (DLMS_OBJECT_TYPE)var_toInteger(tmp);

            ret = va_get(value->Arr, &tmp);
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
            }
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->sortObjectAttributeIndex = (char)var_toInteger(tmp);
            ret = va_get(value->Arr, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            object->sortObjectDataIndex = (unsigned short)var_toInteger(tmp);
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int cosem_setGsmDiagnostic(gxGsmDiagnostic* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT tmp2;
    dlmsVARIANT *tmp, *it;
    switch (index)
    {
    case 2:
        if (value->vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            gxfree(object->operatorName);
            if (value->byteArr != NULL && value->byteArr->size != 0)
            {
                object->operatorName = gxmalloc(value->byteArr->size + 1);
                memcpy(object->operatorName, value->strVal, value->byteArr->size);
                object->operatorName[value->byteArr->size] = '\0';
            }
        }
        else if (value->vt == DLMS_DATA_TYPE_STRING)
        {
            gxfree(object->operatorName);
            if (value->strVal != NULL && value->strVal->size != 0)
            {
                object->operatorName = gxmalloc(value->strVal->size + 1);
                memcpy(object->operatorName, value->strVal, value->strVal->size);
                object->operatorName[value->strVal->size] = '\0';
            }
        }
        else
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
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
            object->cellInfo.cellId = (unsigned long)var_toInteger(tmp);
            if ((ret = va_getByIndex(value->Arr, 1, &tmp)) != 0)
            {
                return ret;
            }
            object->cellInfo.locationId = (unsigned short)var_toInteger(tmp);
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
                object->cellInfo.mobileCountryCode = (unsigned short)var_toInteger(tmp);

                if ((ret = va_getByIndex(value->Arr, 5, &tmp)) != 0)
                {
                    return ret;
                }
                object->cellInfo.mobileNetworkCode = (unsigned short)var_toInteger(tmp);

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
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                ret = va_get(value->Arr, &it);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ret = va_get(it->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                gxAdjacentCell* ac = (gxAdjacentCell*)gxmalloc(sizeof(gxAdjacentCell));
                ac->cellId = var_toInteger(tmp);
                ret = va_get(it->Arr, &tmp);
                if (ret != DLMS_ERROR_CODE_OK)
                {
                    return ret;
                }
                ac->signalQuality = (unsigned char)var_toInteger(tmp);
                arr_push(&object->adjacentCells, ac);
            }
        }
        break;
    case 8:
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
        break;
    default:
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int cosem_setTokenGateway(gxTokenGateway* object, unsigned char index, dlmsVARIANT *value)
{
    int ret, pos;
    dlmsVARIANT tmp2;
    dlmsVARIANT *tmp, *it;
    switch (index)
    {
    case 2:
        bb_clear(&object->token);
        bb_set2(&object->token, value->byteArr, 0, bb_size(value->byteArr));
        break;
    case 3:
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
        arr_clear(&object->descriptions);
        if (value != NULL)
        {
            for (pos = 0; pos != value->Arr->size; ++pos)
            {
                if ((ret = va_get(value->Arr, &it)) != 0)
                {
                    return ret;
                }
                char* d = (char*)gxmalloc(it->strVal->size + 1);
                memcpy(d, it->strVal->data, it->strVal->size);
                d[it->strVal->size] = '\0';
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY

#ifndef DLMS_IGNORE_COMPACT_DATA

int compactData_updateTemplateDescription(
    dlmsSettings* settings,
    gxCompactData* object)
{
    int ret, pos;
    gxByteBuffer tmp;
    gxKey *kv;
    gxValueEventArg e;
    gxValueEventCollection args;
    bb_clear(&object->buffer);
    bb_clear(&object->templateDescription);
    ve_init(&e);
    e.action = 1;
    e.target = &object->base;
    e.index = 2;
    vec_init(&args);
    vec_push(&args, &e);
    // svr_preGet(settings, &args);
    bb_init(&tmp);
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
            e.index = ((gxCaptureObject*)kv->value)->attributeIndex;
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
                        unsigned short count;
                        di_init(&info);
                        var_init(&value);
                        e.value.byteArr->position = 1;
                        if ((ret = hlp_getObjectCount2(e.value.byteArr, &count)) != 0 ||
                            ((gxCaptureObject*)kv->value)->dataIndex > count)
                        {
                            var_clear(&e.value);
                            bb_clear(&object->buffer);
                            return DLMS_ERROR_CODE_OUTOFMEMORY;
                        }
                        //If all data is captured.
                        if (((gxCaptureObject*)kv->value)->dataIndex == 0)
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
									for (unsigned short pos = 0; pos < value.Arr->size; ++pos)
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
                            for (unsigned char pos = 0; pos < ((gxCaptureObject*)kv->value)->dataIndex; ++pos)
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
								if (!info.compleate)
								{
									return DLMS_ERROR_CODE_READ_WRITE_DENIED;
								}
                            }
                            if (info.type == DLMS_DATA_TYPE_STRUCTURE)
                            {
                                dlmsVARIANT* value2;
                                bb_setUInt8(&object->templateDescription, DLMS_DATA_TYPE_STRUCTURE);
                                bb_setUInt8(&object->templateDescription, (unsigned char)value.Arr->size);
                                for (unsigned short pos = 0; pos < value.Arr->size; ++pos)
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
        }
    }
    bb_clear(&tmp);
    //  svr_postGet(settings, &args);
    vec_empty(&args);
    return 0;
    return 0;
}

int cosem_setCompactData(
    dlmsSettings* settings,
    gxCompactData* object,
    unsigned char index,
    dlmsVARIANT *value)
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
        object->templateId = var_toInteger(value);
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

#ifdef DLMS_ITALIAN_STANDARD
int updateInterval(gxInterval* interval, gxByteBuffer* value)
{
    int ret;
    unsigned char b;
    unsigned short v;
    if ((ret = bb_getUInt8(value, &b)) != 0)
    {
        return ret;
    }
    interval->startHour = (unsigned char)(b & 0x1F);
    interval->intervalTariff = (DLMS_DEFAULT_TARIFF_BAND)((b >> 5) & 0x3);
    interval->useInterval = (b >> 7) != 0;

    if ((ret = bb_getUInt16(value, &v)) != 0)
    {
        return ret;
    }
    interval->weeklyActivation = (DLMS_WEEKLY_ACTIVATION)v;

    if ((ret = bb_getUInt16(value, &v)) != 0)
    {
        return ret;
    }
    interval->specialDayMonth = (unsigned char)(v & 0xF);
    interval->specialDay = (unsigned char)((v >> 8) & 0xF);
    interval->specialDayEnabled = (v >> 15) != 0;
    return 0;
}

int updateSeason(gxBandDescriptor* season, variantArray* value)
{
    int ret;
    dlmsVARIANT *tmp;
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
            (ret = updateInterval(&season->workingDayIntervals, tmp->byteArr)) != 0 ||
            (ret = va_getByIndex(value, 3, &tmp)) != 0 ||
            (ret = updateInterval(&season->saturdayIntervals, tmp->byteArr)) != 0 ||
            (ret = va_getByIndex(value, 4, &tmp)) != 0 ||
            (ret = updateInterval(&season->holidayIntervals, tmp->byteArr)) != 0)
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

int cosem_setTariffPlan(gxTariffPlan* object, unsigned char index, dlmsVARIANT *value)
{
    dlmsVARIANT tmp3;
    dlmsVARIANT *tmp, *tmp2;
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
        h = tmp3.dateTime->value.tm_hour;
        m = tmp3.dateTime->value.tm_min;
        s = tmp3.dateTime->value.tm_sec;
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
        object->activationTime.value.tm_hour = h;
        object->activationTime.value.tm_min = m;
        object->activationTime.value.tm_sec = s;
    }
    break;
    default:
        return DLMS_ERROR_CODE_READ_WRITE_DENIED;
    }
    return DLMS_ERROR_CODE_OK;
}

#endif //DLMS_ITALIAN_STANDARD

int cosem_setValue(dlmsSettings* settings, gxValueEventArg *e)
{
    int ret = DLMS_ERROR_CODE_OK;
    if (e->index == 1)
    {
        if (e->value.byteArr != NULL && e->value.byteArr->size != 6)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        memcpy(e->target->logicalName, e->value.byteArr, 6);
        return DLMS_ERROR_CODE_OK;
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
        ret = cosem_setClock((gxClock*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = cosem_setActionSchedule((gxActionSchedule*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = cosem_setActivityCalendar((gxActivityCalendar*)e->target, e->index, &e->value);
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
        ret = cosem_setIP4Setup((gxIp4Setup*)e->target, e->index, &e->value);
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
        ret = cosem_setmMbusClient((gxMBusClient*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = cosem_setModemConfiguration((gxModemConfiguration*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = cosem_setPppSetup((gxPppSetup*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = cosem_setProfileGeneric(settings, (gxProfileGeneric*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = cosem_setRegisterActivation((gxRegisterActivation*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = cosem_setRegisterMonitor((gxRegisterMonitor*)e->target, e->index, &e->value);
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
        ret = cosem_setSchedule((gxSchedule*)e->target, e->index, &e->value);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = cosem_setScriptTable((gxScriptTable*)e->target, e->index, &e->value);
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
        ret = cosem_setTcpUdpSetup((gxTcpUdpSetup*)e->target, e->index, &e->value);
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
        ret = cosem_setCharge((gxCharge*)e->target, e->index, &e->value);
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
