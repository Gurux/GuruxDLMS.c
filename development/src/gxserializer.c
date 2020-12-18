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
#include "../include/gxserializer.h"
#include "../include/helpers.h"
#include "../include/objectarray.h"
#include "../include/cosem.h"
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif
#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif

#define IS_ATTRIBUTE_SET(ATTRIBUTES, INDEX) (ATTRIBUTES & 1 << (INDEX - 1)) != 0

//Returns ignored attributes.
uint16_t ser_getIgnoredAttributes(gxSerializerSettings* serializeSettings, gxObject* object)
{
    uint16_t ret = 0;
    if (serializeSettings != NULL)
    {
        int pos;
        for (pos = 0; pos != serializeSettings->count; ++pos)
        {
            if (serializeSettings->ignoredAttributes[pos].target == object ||
                serializeSettings->ignoredAttributes[pos].objectType == object->objectType)
            {
                ret |= serializeSettings->ignoredAttributes[pos].attributes;
            }
        }
    }
    return ret;
}

//Is attribute serialized.
unsigned char ser_isSerialized(gxSerializerSettings* serializeSettings, gxObject* object, unsigned char attribute)
{
    if (serializeSettings != NULL)
    {
        int pos;
        for (pos = 0; pos != serializeSettings->count; ++pos)
        {
            if (serializeSettings->ignoredAttributes[pos].target == object ||
                serializeSettings->ignoredAttributes[pos].objectType == object->objectType)
            {
                if (IS_ATTRIBUTE_SET(serializeSettings->ignoredAttributes[pos].attributes, attribute))
                {
                    return 0;
                }
                //Break, if all object with this type are not ignored.
                if (serializeSettings->ignoredAttributes[pos].objectType == object->objectType)
                {
                    break;
                }
            }
        }
    }
    return 1;
}

int ser_saveDateTime(gxtime* value, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = cosem_setDateTime(serializer, value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int ser_saveOctetString(gxByteBuffer* serializer, gxByteBuffer* value)
{
    int ret;
    if (value == NULL)
    {
        if ((ret = bb_setUInt8(serializer, 0)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if ((ret = hlp_setObjectCount((uint16_t)value->size, serializer)) != 0 ||
        (ret = bb_set(serializer, value->data, (uint16_t)value->size)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int ser_saveOctetString3(gxByteBuffer* serializer, char* value, uint16_t len)
{
    int ret;
    if (value == NULL || len == 0)
    {
        if ((ret = bb_setUInt8(serializer, 0)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else
    {
        if ((ret = hlp_setObjectCount(len, serializer)) != 0 ||
            (ret = bb_set(serializer, (unsigned char*)value, len)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    return ret;
}

int ser_saveOctetString2(gxByteBuffer* serializer, char* value)
{
    int ret;
    if (value == NULL)
    {
        if ((ret = bb_setUInt8(serializer, 0)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else
    {
        uint16_t len = (uint16_t)strlen(value);
        ret = ser_saveOctetString3(serializer, value, len);
    }
    return ret;
}

int ser_saveBitString(gxByteBuffer* serializer, bitArray* value)
{
    int ret;
    if (value == NULL)
    {
        if ((ret = bb_setUInt8(serializer, 0)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else
    {
        if ((ret = hlp_setObjectCount(value->size, serializer)) != 0 ||
            (ret = bb_set(serializer, value->data, ba_getByteCount(value->size))) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    return ret;
}


#ifndef DLMS_IGNORE_DATA
int ser_saveData(gxSerializerSettings* serializeSettings, gxData* object, gxByteBuffer* serializer)
{
    if (!IS_ATTRIBUTE_SET(ser_getIgnoredAttributes(serializeSettings, (gxObject*)object), 2))
    {
        return var_getBytes(&object->value, serializer);
    }
    return 0;
}

#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int ser_saveRegister(
    gxSerializerSettings* serializeSettings,
    gxRegister* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_setInt8(serializer, object->scaler)) != 0 ||
            (ret = bb_setUInt8(serializer, object->unit)) != 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = var_getBytes(&object->value, serializer)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
int ser_saveClock(
    gxSerializerSettings* serializeSettings,
    gxClock* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_saveDateTime(&object->time, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setInt16(serializer, object->timeZone)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->status)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_saveDateTime(&object->begin, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_saveDateTime(&object->end, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setInt8(serializer, object->deviation)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt8(serializer, object->enabled)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setUInt8(serializer, object->clockBase)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int ser_saveScriptTable(
    gxSerializerSettings* serializeSettings,
    gxScriptTable* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos, pos2;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        gxScript* s;
        gxScriptAction* sa;
        if ((ret = hlp_setObjectCount(object->scripts.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->scripts.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&s, sizeof(gxScript))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&s)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_setUInt16(serializer, s->id)) == 0 && (ret = hlp_setObjectCount(s->actions.size, serializer)) == 0)
                {
                    for (pos2 = 0; pos2 != s->actions.size; ++pos2)
                    {
#ifdef DLMS_IGNORE_MALLOC
                        if ((ret = arr_getByIndex(&s->actions, pos2, (void**)&sa, sizeof(gxScriptAction))) != 0 ||
                            (ret = bb_setUInt8(serializer, sa->type)) != 0)
                        {
                            break;
                        }
#else
                        if ((ret = arr_getByIndex(&s->actions, pos2, (void**)&sa)) != 0 ||
                            (ret = bb_setUInt8(serializer, sa->type)) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        if (sa->target == NULL)
                        {
                            if ((ret = bb_setUInt16(serializer, 0)) != 0 ||
                                (ret = bb_set(serializer, EMPTY_LN, sizeof(EMPTY_LN))) != 0)
                            {
                                break;
                            }
                        }
                        else
                        {
                            if ((ret = bb_setUInt16(serializer, sa->target->objectType)) != 0 ||
                                (ret = bb_set(serializer, sa->target->logicalName, sizeof(sa->target->logicalName))) != 0)
                            {
                                break;
                            }
                        }
#else
                        if ((ret = bb_setUInt16(serializer, sa->objectType)) != 0 ||
                            (ret = bb_set(serializer, sa->logicalName, sizeof(sa->logicalName))) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        if ((ret = bb_setInt8(serializer, sa->index)) != 0 ||
                            (ret = var_getBytes(&sa->parameter, serializer)) != 0)
                        {
                            break;
                        }
                    }
                }
                if (ret != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int ser_saveSpecialDaysTable(
    gxSerializerSettings* serializeSettings,
    gxSpecialDaysTable* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos;
    gxSpecialDay* sd;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = hlp_setObjectCount(object->entries.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->entries.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd, sizeof(gxSpecialDay))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_setUInt16(serializer, sd->index)) != 0 ||
                    (ret = ser_saveDateTime(&sd->date, serializer)) != 0 ||
                    (ret = bb_setUInt8(serializer, sd->dayId)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int ser_saveTcpUdpSetup(
    gxSerializerSettings* serializeSettings,
    gxTcpUdpSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt16(serializer, object->port)) != 0) ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_set(serializer, obj_getLogicalName(object->ipSetup), 6)) != 0) ||
#else
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_set(&ba, object->ipReference, 6)) != 0) ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt16(serializer, object->maximumSegmentSize)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->maximumSimultaneousConnections)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt16(serializer, object->inactivityTimeout)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int ser_saveMBusMasterPortSetup(
    gxSerializerSettings* serializeSettings,
    gxMBusMasterPortSetup* object,
    gxByteBuffer* serializer)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        return bb_setUInt8(serializer, object->commSpeed);
    }
    return 0;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

int saveTimeWindow(gxByteBuffer* serializer, gxArray* arr)
{
    int ret;
    uint16_t pos;
#ifdef DLMS_IGNORE_MALLOC
    gxTimePair* k;
#else
    gxKey* k;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = hlp_setObjectCount(arr->size, serializer)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&k, sizeof(gxTimePair))) != 0 ||
                (ret = ser_saveDateTime(&k->first, serializer)) != 0 ||
                (ret = ser_saveDateTime(&k->second, serializer)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(arr, pos, (void**)&k)) != 0 ||
                (ret = ser_saveDateTime((gxtime*)k->key, serializer)) != 0 ||
                (ret = ser_saveDateTime((gxtime*)k->value, serializer)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}


int ser_saveObjects2(gxByteBuffer* serializer, gxArray* objects)
{
    uint16_t pos;
    int ret;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = hlp_setObjectCount(objects->size, serializer)) == 0)
    {
        for (pos = 0; pos != objects->size; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(objects, pos, (void**)&it, sizeof(gxTarget))) != 0 ||
#else
            if ((ret = arr_getByIndex(objects, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                (ret = bb_setUInt16(serializer, it->target->objectType)) != 0 ||
                (ret = bb_set(serializer, it->target->logicalName, 6)) != 0 ||
                (ret = bb_setInt8(serializer, it->attributeIndex)) != 0 ||
                (ret = bb_setUInt16(serializer, it->dataIndex)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(objects, pos, (void**)&it)) != 0 ||
                (ret = bb_setUInt16(serializer, ((gxObject*)it->key)->objectType)) != 0 ||
                (ret = bb_set(serializer, ((gxObject*)it->key)->logicalName, 6)) != 0 ||
                (ret = bb_setInt8(serializer, ((gxTarget*)it->value)->attributeIndex)) != 0 ||
                (ret = bb_setUInt16(serializer, ((gxTarget*)it->value)->dataIndex)) != 0)
            {
                break;
            }
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP
int ser_savePushSetup(
    gxSerializerSettings* serializeSettings,
    gxPushSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_saveObjects2(serializer, &object->pushObjectList)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) &&
            ((ret = bb_setUInt8(serializer, object->service)) != 0 ||
                (ret = ser_saveOctetString(serializer, &object->destination)) != 0 ||
                (ret = bb_setUInt8(serializer, object->message)) != 0)) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = saveTimeWindow(serializer, &object->communicationWindow)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt16(serializer, object->randomisationStartInterval)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt8(serializer, object->numberOfRetries)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt16(serializer, object->repetitionDelay)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_AUTO_CONNECT
int ser_saveAutoConnect(
    gxSerializerSettings* serializeSettings,
    gxAutoConnect* object,
    gxByteBuffer* serializer)
{
#ifdef DLMS_IGNORE_MALLOC
    gxDestination* dest;
#else
    gxByteBuffer* dest;
#endif //DLMS_IGNORE_MALLOC
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->mode)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->repetitions)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt16(serializer, object->repetitionDelay)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = saveTimeWindow(serializer, &object->callingWindow)) != 0))
    {
    }
    else
    {
        if (!IS_ATTRIBUTE_SET(ignored, 6))
        {
            if ((ret = hlp_setObjectCount(object->destinations.size, serializer)) == 0)
            {
                for (pos = 0; pos != object->destinations.size; ++pos)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = arr_getByIndex(&object->destinations, pos, (void**)&dest, sizeof(gxDestination))) != 0 ||
                        (ret = ser_saveOctetString3(serializer, (char*)dest->value, dest->size)) != 0)
                    {
                        break;
                    }
#else
                    if ((ret = arr_getByIndex(&object->destinations, pos, (void**)&dest)) != 0 ||
                        (ret = ser_saveOctetString3(serializer, (char*)dest->data, (unsigned short)dest->size)) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int ser_saveSeasonProfile(gxArray* arr, gxByteBuffer* serializer)
{
    gxSeasonProfile* it;
    int pos, ret;
    if ((ret = hlp_setObjectCount(arr->size, serializer)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&it, sizeof(gxSeasonProfile))) != 0)
            {
                break;
            }
            if ((ret = ser_saveOctetString3(serializer, (char*)it->name.value, it->name.size)) != 0 ||
                (ret = ser_saveDateTime(&it->start, serializer)) != 0 ||
                (ret = ser_saveOctetString3(serializer, (char*)it->weekName.value, it->weekName.size)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
            {
                break;
            }
            if ((ret = ser_saveOctetString(serializer, &it->name)) != 0 ||
                (ret = ser_saveDateTime(&it->start, serializer)) != 0 ||
                (ret = ser_saveOctetString(serializer, &it->weekName)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}

int ser_saveweekProfile(gxArray* arr, gxByteBuffer* serializer)
{
    gxWeekProfile* it;
    int pos, ret;
    if ((ret = hlp_setObjectCount(arr->size, serializer)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&it, sizeof(gxWeekProfile))) != 0 ||
                (ret = ser_saveOctetString3(serializer, (char*)it->name.value, it->name.size)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0 ||
                (ret = ser_saveOctetString(serializer, &it->name)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_setUInt8(serializer, it->monday)) != 0 ||
                (ret = bb_setUInt8(serializer, it->tuesday)) != 0 ||
                (ret = bb_setUInt8(serializer, it->wednesday)) != 0 ||
                (ret = bb_setUInt8(serializer, it->thursday)) != 0 ||
                (ret = bb_setUInt8(serializer, it->friday)) != 0 ||
                (ret = bb_setUInt8(serializer, it->saturday)) != 0 ||
                (ret = bb_setUInt8(serializer, it->sunday)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int ser_saveDayProfile(gxArray* arr, gxByteBuffer* serializer)
{
    gxDayProfile* dp;
    gxDayProfileAction* it;
    int pos, pos2, ret;
    if ((ret = hlp_setObjectCount(arr->size, serializer)) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&dp, sizeof(gxDayProfile))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(arr, pos, (void**)&dp)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_setUInt8(serializer, dp->dayId)) != 0 ||
                (ret = hlp_setObjectCount(dp->daySchedules.size, serializer)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&it, sizeof(gxDayProfileAction))) != 0 ||
#else
                if ((ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                (ret = bb_set(serializer, obj_getLogicalName(it->script), 6)) != 0 ||
#else
                    (ret = hlp_appendLogicalName(serializer, it->scriptLogicalName)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = bb_setUInt16(serializer, it->scriptSelector)) != 0 ||
                    (ret = ser_saveDateTime(&it->startTime, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}

int ser_saveActivityCalendar(
    gxSerializerSettings* serializeSettings,
    gxActivityCalendar* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_saveOctetString(serializer, &object->calendarNameActive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = ser_saveSeasonProfile(&object->seasonProfileActive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_saveweekProfile(&object->weekProfileTableActive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_saveDayProfile(&object->dayProfileTableActive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_saveOctetString(serializer, &object->calendarNamePassive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_saveSeasonProfile(&object->seasonProfilePassive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = ser_saveweekProfile(&object->weekProfileTablePassive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = ser_saveDayProfile(&object->dayProfileTablePassive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = ser_saveDateTime(&object->time, serializer)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_SECURITY_SETUP
int ser_saveSecuritySetup(
    gxSerializerSettings* serializeSettings,
    gxSecuritySetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->securityPolicy)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->securitySuite)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_set(serializer, object->serverSystemTitle.data, object->serverSystemTitle.size)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_set(serializer, object->clientSystemTitle.data, object->clientSystemTitle.size)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int ser_saveHdlcSetup(
    gxSerializerSettings* serializeSettings,
    gxIecHdlcSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->communicationSpeed)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->windowSizeTransmit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->windowSizeReceive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt16(serializer, object->maximumInfoLengthTransmit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt16(serializer, object->maximumInfoLengthReceive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt16(serializer, object->interCharachterTimeout)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt16(serializer, object->inactivityTimeout)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setUInt16(serializer, object->deviceAddress)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int ser_saveLocalPortSetup(
    gxSerializerSettings* serializeSettings,
    gxLocalPortSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->defaultMode)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->defaultBaudrate)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->proposedBaudrate)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->responseTime)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_saveOctetString(serializer, &object->deviceAddress)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_saveOctetString(serializer, &object->password1)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = ser_saveOctetString(serializer, &object->password2)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = ser_saveOctetString(serializer, &object->password5)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int ser_saveIecTwistedPairSetup(
    gxSerializerSettings* serializeSettings,
    gxIecTwistedPairSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->mode)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->speed)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_saveOctetString(serializer, &object->primaryAddresses)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_saveOctetString(serializer, &object->tabis)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int ser_saveDemandRegister(
    gxSerializerSettings* serializeSettings,
    gxDemandRegister* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = var_getBytes(&object->currentAverageValue, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = var_getBytes(&object->lastAverageValue, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setInt8(serializer, object->scaler)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->unit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = var_getBytes(&object->status, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_saveDateTime(&object->captureTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = ser_saveDateTime(&object->startTimeCurrent, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setUInt32(serializer, object->period)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = bb_setUInt16(serializer, object->numberOfPeriods)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int ser_saveRegisterActivation(
    gxSerializerSettings* serializeSettings,
    gxRegisterActivation* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxRegisterActivationMask* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = hlp_setObjectCount(object->registerAssignment.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->registerAssignment.size; ++pos)
            {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
#if defined(DLMS_IGNORE_MALLOC)
                if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od, sizeof(gxObject))) != 0 ||
#else
#if defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od)) != 0 ||
#else
                if ((ret = oa_getByIndex(&object->registerAssignment, pos, &od)) != 0 ||
#endif //defined(DLMS_COSEM_EXACT_DATA_TYPES)
#endif //DLMS_IGNORE_MALLOC
#else
                if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = bb_setUInt16(serializer, od->objectType)) != 0 ||
                    (ret = bb_set(serializer, od->logicalName, 6)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3) &&
        (ret = hlp_setObjectCount(object->maskList.size, serializer)) == 0)
    {
        for (pos = 0; pos != object->maskList.size; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask))) != 0 ||
                (ret = ser_saveOctetString3(serializer, (char*)it->name, it->length)) != 0 ||
                (ret = ser_saveOctetString3(serializer, (char*)it->indexes, it->count)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex2(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask))) != 0 ||
                (ret = ser_saveOctetString(serializer, &it->name)) != 0 ||
                (ret = ser_saveOctetString(serializer, &it->indexes)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
#else
            if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&it)) != 0 ||
                (ret = ser_saveOctetString(serializer, (gxByteBuffer*)it->key)) != 0 ||
                (ret = ser_saveOctetString(serializer, (gxByteBuffer*)it->value)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        ret = ser_saveOctetString(serializer, &object->activeMask);
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int ser_saveActionItem(
    gxActionItem* item,
    gxByteBuffer* serializer)
{
    int ret;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_set(serializer, obj_getLogicalName((gxObject*)item->script), 6)) != 0 ||
#else
    if ((ret = bb_set(serializer, item->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_setUInt16(serializer, item->scriptSelector)) != 0)
    {
    }
    return ret;
}

int ser_saveRegisterMonitor(
    gxSerializerSettings* serializeSettings,
    gxRegisterMonitor* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    dlmsVARIANT* tmp;
    gxActionSet* as;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = hlp_setObjectCount(object->thresholds.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->thresholds.size; ++pos)
            {
                if ((ret = va_getByIndex(&object->thresholds, pos, &tmp)) != 0 ||
                    (ret = var_getBytes(tmp, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (((ret = bb_setUInt16(serializer, object->monitoredValue.target == NULL ? 0 : object->monitoredValue.target->objectType)) != 0 ||
            (ret = bb_set(serializer, obj_getLogicalName(object->monitoredValue.target), 6)) != 0))
        {

        }
#else
        if ((ret = bb_setUInt16(serializer, object->monitoredValue.target == NULL ? 0 : object->monitoredValue.objectType)) != 0 ||
            (ret = bb_set(serializer, obj_getLogicalName(object->monitoredValue.logicalName), 6)) != 0)
        {

        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0)
        {
            ret = bb_setInt8(serializer, object->monitoredValue.attributeIndex);
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = hlp_setObjectCount(object->actions.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->actions.size; ++pos)
            {

#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->actions, pos, (void**)&as, sizeof(gxActionSet))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->actions, pos, (void**)&as)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = ser_saveActionItem(&as->actionUp, serializer)) != 0 ||
                    (ret = ser_saveActionItem(&as->actionDown, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int ser_saveActionSchedule(
    gxSerializerSettings* serializeSettings,
    gxActionSchedule* object,
    gxByteBuffer* serializer)
{
    int pos = 0, ret = 0;
    gxtime* tm;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = bb_set(serializer, obj_getLogicalName((gxObject*)object->executedScript), 6)) == 0)
        {
            ret = bb_setUInt16(serializer, object->executedScriptSelector);
        }
#else
        if ((ret = bb_set(serializer, object->executedScriptLogicalName, 6)) == 0)
        {
            ret = bb_setUInt16(serializer, object->executedScriptSelector);
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        ret = bb_setUInt8(serializer, object->type);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        //Add count.
        if ((ret = hlp_setObjectCount(object->executionTime.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->executionTime.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = ser_saveDateTime(tm, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int ser_saveSapAssignment(
    gxSerializerSettings* serializeSettings,
    gxSapAssignment* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos = 0;
    gxSapItem* it;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        //Add count.
        if ((ret = hlp_setObjectCount(object->sapAssignmentList.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->sapAssignmentList.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0 ||
                    (ret = bb_setUInt16(serializer, it->id)) != 0 ||
                    (ret = ser_saveOctetString3(serializer, (char*)it->name.value, it->name.size)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->sapAssignmentList, pos, (void**)&it)) != 0 ||
                    (ret = bb_setUInt16(serializer, it->id)) != 0 ||
                    (ret = ser_saveOctetString(serializer, &it->name)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_AUTO_ANSWER
int ser_saveAutoAnswer(
    gxSerializerSettings* serializeSettings,
    gxAutoAnswer* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->mode)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = saveTimeWindow(serializer, &object->listeningWindow)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->status)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->numberOfCalls)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt8(serializer, object->numberOfRingsInListeningWindow)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt8(serializer, object->numberOfRingsOutListeningWindow)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt8(serializer, object->numberOfRingsOutListeningWindow)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int ser_saveIp4Setup(
    gxSerializerSettings* serializeSettings,
    gxIp4Setup* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    uint32_t* tmp;
#else
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* ip;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = bb_set(serializer, obj_getLogicalName(object->dataLinkLayer), 6);

#else
        ret = bb_set(serializer, object->dataLinkLayerReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        (ret = bb_setUInt32(serializer, object->ipAddress));
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = hlp_setObjectCount(object->multicastIPAddress.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_getByIndex2(&object->multicastIPAddress, pos, (void**)&tmp, sizeof(uint32_t))) != 0 ||
                    (ret = bb_setUInt32(serializer, *tmp)) != 0)
                {
                    break;
                }
#else
                if ((ret = va_getByIndex(&object->multicastIPAddress, pos, &tmp)) != 0 ||
                    (ret = bb_setUInt32(serializer, tmp->ulVal)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5) &&
        (ret = hlp_setObjectCount(object->ipOptions.size, serializer)) == 0)
    {
        for (pos = 0; pos != object->ipOptions.size; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->ipOptions, pos, (void**)&ip, sizeof(gxip4SetupIpOption))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->ipOptions, pos, (void**)&ip)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_setUInt8(serializer, ip->type)) != 0)
            {
                break;
            }

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = ser_saveOctetString3(serializer, (char*)ip->data.value, ip->data.size)) != 0)
            {
                break;
            }
#else
            if ((ret = ser_saveOctetString(serializer, &ip->data)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt32(serializer, object->subnetMask)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt32(serializer, object->gatewayIPAddress)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt8(serializer, object->useDHCP)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setUInt32(serializer, object->primaryDNSAddress)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = bb_setUInt32(serializer, object->secondaryDNSAddress)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP

int saveIpv6Address(IN6_ADDR* address, gxByteBuffer* serializer)
{
    unsigned char* tmp;
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    tmp = address->u.Byte;
#else //Linux includes.
    tmp = address->s6_addr;
#endif
    return bb_set(serializer, tmp, 16);
}

int saveAddressList(gxArray* list, gxByteBuffer* serializer)
{
    IN6_ADDR* it;
    int pos, ret = 0;
    if ((ret = hlp_setObjectCount(list->size, serializer)) == 0)
    {
        for (pos = 0; pos != list->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(list, pos, (void**)&it, sizeof(IN6_ADDR))) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = saveIpv6Address(it, serializer)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int ser_saveIp6Setup(
    gxSerializerSettings* serializeSettings,
    gxIp6Setup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = bb_set(serializer, obj_getLogicalName(object->dataLinkLayer), 6);
#else
        ret = bb_set(serializer, object->dataLinkLayerReference, 6));
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->addressConfigMode)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = saveAddressList(&object->unicastIPAddress, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = saveAddressList(&object->multicastIPAddress, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = saveAddressList(&object->gatewayIPAddress, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = saveIpv6Address(&object->primaryDNSAddress, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = saveIpv6Address(&object->secondaryDNSAddress, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setUInt8(serializer, object->trafficClass)) != 0))
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 10))
    {
        gxNeighborDiscoverySetup* it;
        int pos;
        if ((ret = hlp_setObjectCount(object->neighborDiscoverySetup.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->neighborDiscoverySetup.size; ++pos)
            {
                if ((ret = arr_getByIndex2(&object->neighborDiscoverySetup, pos, (void**)&it, sizeof(gxNeighborDiscoverySetup))) != 0)
                {
                    break;
                }
                if ((ret = bb_setUInt8(serializer, it->maxRetry)) != 0 ||
                    (ret = bb_setUInt16(serializer, it->retryWaitTime)) != 0 ||
                    (ret = bb_setUInt32(serializer, it->sendPeriod)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
int ser_saveUtilityTables(
    gxSerializerSettings* serializeSettings,
    gxUtilityTables* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt16(serializer, object->tableId)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_saveOctetString(serializer, &object->buffer)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int ser_saveMbusSlavePortSetup(
    gxSerializerSettings* serializeSettings,
    gxMbusSlavePortSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->defaultBaud)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->availableBaud)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->addressState)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->busAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int ser_saveImageTransfer(
    gxSerializerSettings* serializeSettings,
    gxImageTransfer* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    gxImageActivateInfo* it;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt32(serializer, object->imageBlockSize)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = ser_saveBitString(serializer, &object->imageTransferredBlocksStatus)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt32(serializer, object->imageFirstNotTransferredBlockNumber)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->imageTransferEnabled)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt8(serializer, object->imageTransferStatus)) != 0))
    {
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        if ((ret = hlp_setObjectCount(object->imageActivateInfo.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->imageActivateInfo.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&it, sizeof(gxImageActivateInfo))) != 0)
                {
                    break;
                }
                if ((ret = bb_setUInt32(serializer, it->size)) != 0 ||
                    (ret = ser_saveOctetString3(serializer, (char*)it->identification.data, it->identification.size)) != 0 ||
                    (ret = ser_saveOctetString3(serializer, (char*)it->signature.data, it->signature.size)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&it)) != 0)
                {
                    break;
                }
                if ((ret = bb_setUInt32(serializer, it->size)) != 0 ||
                    (ret = ser_saveOctetString(serializer, &it->identification)) != 0 ||
                    (ret = ser_saveOctetString(serializer, &it->signature)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int ser_saveDisconnectControl(
    gxSerializerSettings* serializeSettings,
    gxDisconnectControl* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt8(serializer, object->outputState)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->controlState)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->controlMode)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int ser_saveLimiter(
    gxSerializerSettings* serializeSettings,
    gxLimiter* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    dlmsVARIANT* it;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_set(serializer, obj_getLogicalName(object->monitoredValue), 6)) == 0)
            ret = bb_setInt8(serializer, object->selectedAttributeIndex);
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 3) && (ret = var_getBytes(&object->thresholdActive, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = var_getBytes(&object->thresholdNormal, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = var_getBytes(&object->thresholdEmergency, serializer)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt32(serializer, object->minOverThresholdDuration)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt32(serializer, object->minUnderThresholdDuration)) != 0))
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8))
    {
        if ((ret = bb_setUInt16(serializer, object->emergencyProfile.id)) != 0 ||
            (ret = ser_saveDateTime(&object->emergencyProfile.activationTime, serializer)) != 0 ||
            (ret = bb_setUInt32(serializer, object->emergencyProfile.duration)) != 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 9))
    {
        if ((ret = hlp_setObjectCount(object->emergencyProfileGroupIDs.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->emergencyProfileGroupIDs.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_getByIndex2(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(dlmsVARIANT))) != 0 ||
                    (ret = var_getBytes(it, serializer)) != 0)
                {
                    break;
                }
#else
                if ((ret = va_getByIndex(&object->emergencyProfileGroupIDs, pos, &it)) != 0 ||
                    (ret = var_getBytes(it, serializer)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 10))
    {
        ret = bb_setUInt8(serializer, object->emergencyProfileActive);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 11))
    {
        if ((ret = ser_saveActionItem(&object->actionOverThreshold, serializer)) == 0)
        {
            ret = ser_saveActionItem(&object->actionUnderThreshold, serializer);
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int ser_saveMBusClient(
    gxSerializerSettings* serializeSettings,
    gxMBusClient* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt32(serializer, object->capturePeriod)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->primaryAddress)) != 0) ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_set(serializer, obj_getLogicalName(object->mBusPort), 6)) != 0) ||
#else
        (!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_set(serializer, obj_getLogicalName(object->mBusPortReference), 6)) != 0) ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt32(serializer, object->identificationNumber)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt16(serializer, object->manufacturerID)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt8(serializer, object->dataHeaderVersion)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setUInt8(serializer, object->deviceType)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = bb_setUInt8(serializer, object->accessNumber)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 11) && (ret = bb_setUInt8(serializer, object->status)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 12) && (ret = bb_setUInt8(serializer, object->alarm)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 13) && (ret = bb_setUInt16(serializer, object->configuration)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 14) && (ret = bb_setUInt8(serializer, object->encryptionKeyStatus)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int ser_saveModemConfiguration(
    gxSerializerSettings* serializeSettings,
    gxModemConfiguration* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = bb_setInt8(serializer, object->communicationSpeed);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3) &&
        (ret = hlp_setObjectCount(object->initialisationStrings.size, serializer)) == 0)
    {
        for (pos = 0; pos != object->initialisationStrings.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->initialisationStrings, pos, (void**)&mi, sizeof(gxModemInitialisation))) != 0 ||
                (ret = ser_saveOctetString3(serializer, (char*)mi->request.value, mi->request.size)) != 0 ||
                (ret = ser_saveOctetString3(serializer, (char*)mi->response.value, mi->response.size)) != 0 ||
                (ret = bb_setUInt16(serializer, mi->delay)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->initialisationStrings, pos, (void**)&mi)) != 0 ||
                (ret = ser_saveOctetString(serializer, &mi->request)) != 0 ||
                (ret = ser_saveOctetString(serializer, &mi->response)) != 0 ||
                (ret = bb_setInt16(serializer, mi->delay)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4) &&
        (ret = hlp_setObjectCount(object->modemProfile.size, serializer)) == 0)
    {
        for (pos = 0; pos != object->modemProfile.size; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->modemProfile, pos, (void**)&it, sizeof(gxModemProfile))) != 0 ||
                (ret = bb_set(serializer, it->value, it->size)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->modemProfile, pos, (void**)&it)) != 0 ||
                (ret = bb_set(serializer, it->data, it->size)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int ser_saveMacAddressSetup(
    gxSerializerSettings* serializeSettings,
    gxMacAddressSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = ser_saveOctetString(serializer, &object->macAddress);
    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
int ser_saveQualityOfService(
    gxQualityOfService* object,
    gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->precedence)) != 0 ||
        (ret = bb_setUInt8(serializer, object->delay)) != 0 ||
        (ret = bb_setUInt8(serializer, object->reliability)) != 0 ||
        (ret = bb_setUInt8(serializer, object->peakThroughput)) != 0 ||
        (ret = bb_setUInt8(serializer, object->meanThroughput)) != 0)
    {

    }
    return ret;
}

int ser_saveGPRSSetup(
    gxSerializerSettings* serializeSettings,
    gxGPRSSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_saveOctetString(serializer, &object->apn)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setInt16(serializer, object->pinCode)) != 0))
    {

    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if (((ret = ser_saveQualityOfService(&object->defaultQualityOfService, serializer)) != 0 ||
            (ret = ser_saveQualityOfService(&object->requestedQualityOfService, serializer)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int ser_saveExtendedRegister(
    gxSerializerSettings* serializeSettings,
    gxExtendedRegister* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = var_getBytes(&object->value, serializer);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_setInt8(serializer, object->scaler)) != 0 ||
            (ret = bb_setUInt8(serializer, object->unit)) != 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        ret = var_getBytes(&object->status, serializer);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        ret = ser_saveDateTime(&object->captureTime, serializer);
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

int ser_saveApplicationContextName(gxByteBuffer* serializer, gxApplicationContextName* object)
{
    int ret;
    if ((ret = bb_set(serializer, object->logicalName, 6)) != 0 ||
        (ret = bb_setUInt8(serializer, object->jointIsoCtt)) != 0 ||
        (ret = bb_setUInt8(serializer, object->country)) != 0 ||
        (ret = bb_setUInt16(serializer, object->countryName)) != 0 ||
        (ret = bb_setUInt8(serializer, object->identifiedOrganization)) != 0 ||
        (ret = bb_setUInt8(serializer, object->dlmsUA)) != 0 ||
        (ret = bb_setUInt8(serializer, object->applicationContext)) != 0 ||
        (ret = bb_setUInt8(serializer, object->contextId)) != 0)
    {

    }
    return ret;
}

int ser_savexDLMSContextType(gxByteBuffer* serializer, gxXDLMSContextType* object)
{
    int ret;
    if ((ret = bb_setUInt32(serializer, object->conformance)) != 0 ||
        (ret = bb_setUInt16(serializer, object->maxReceivePduSize)) != 0 ||
        (ret = bb_setUInt16(serializer, object->maxSendPduSize)) != 0 ||
        (ret = bb_setInt8(serializer, object->qualityOfService)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->cypheringInfo)) != 0)
    {

    }
    return ret;
}

int ser_saveAuthenticationMechanismName(
    gxByteBuffer* serializer,
    gxAuthenticationMechanismName* object)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->jointIsoCtt)) != 0 ||
        (ret = bb_setUInt8(serializer, object->country)) != 0 ||
        (ret = bb_setUInt16(serializer, object->countryName)) != 0 ||
        (ret = bb_setUInt8(serializer, object->identifiedOrganization)) != 0 ||
        (ret = bb_setUInt8(serializer, object->dlmsUA)) != 0 ||
        (ret = bb_setUInt8(serializer, object->authenticationMechanismName)) != 0 ||
        (ret = bb_setUInt8(serializer, object->mechanismId)) != 0)
    {

    }
    return ret;
}

int ser_saveAssociationLogicalName(
    gxSerializerSettings* serializeSettings,
    gxAssociationLogicalName* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#else
    gxKey2* it;
#endif //DLMS_IGNORE_MALLOC
    if (!IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_setInt8(serializer, object->clientSAP)) != 0 ||
            (ret = bb_setUInt16(serializer, object->serverSAP)) != 0)
        {
        }
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_saveApplicationContextName(serializer, &object->applicationContextName)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_savexDLMSContextType(serializer, &object->xDLMSContextInfo)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_saveAuthenticationMechanismName(serializer, &object->authenticationMechanismName)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_saveOctetString(serializer, &object->secret)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt8(serializer, object->associationStatus)) != 0))
        {
        }
    }
    //Security Setup Reference is from version 1.
    if (object->base.version > 0)
    {
        if (!IS_ATTRIBUTE_SET(ignored, 9))
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ret = bb_set(serializer, obj_getLogicalName((gxObject*)object->securitySetup), 6);
#else
            ret = bb_set(&ba, object->securitySetupReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    if (object->base.version > 1)
    {
        if (!IS_ATTRIBUTE_SET(ignored, 10))
        {
            if ((ret = hlp_setObjectCount(object->userList.size, serializer)) == 0)
            {
                for (pos = 0; pos != object->userList.size; ++pos)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = arr_getByIndex(&object->userList, pos, (void**)&it, sizeof(gxUser))) != 0 ||
                        (ret = bb_setUInt8(serializer, it->id)) != 0 ||
                        (ret = ser_saveOctetString2(serializer, it->name)) != 0)
                    {
                        break;
                    }
#else
                    if ((ret = arr_getByIndex(&object->userList, pos, (void**)&it)) != 0 ||
                        (ret = bb_setUInt8(serializer, it->key)) != 0 ||
                        (ret = ser_saveOctetString2(serializer, (char*)it->value)) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int ser_saveAssociationShortName(
    gxSerializerSettings* serializeSettings,
    gxAssociationShortName* object,
    gxByteBuffer* serializer)
{
    return 0;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int ser_savePppSetup(
    gxSerializerSettings* serializeSettings,
    gxPppSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#ifndef DLMS_IGNORE_OBJECT_POINTER1S
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_set(serializer, obj_getLogicalName(object->phy), 6)) != 0))
    {
    }
#else
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_set(&ba, object->PHYReference, 6)) != 0))
    {
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        if ((ret = ser_saveOctetString(serializer, &object->userName)) != 0 ||
            (ret = ser_saveOctetString(serializer, &object->password)) != 0 ||
            (ret = bb_setUInt8(serializer, object->authentication)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int ser_saveProfileGeneric(
    gxSerializerSettings* serializeSettings,
    gxProfileGeneric* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 3) && (ret = ser_saveObjects2(serializer, &object->captureObjects)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt32(serializer, object->capturePeriod)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->sortMethod)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_set(serializer, obj_getLogicalName(object->sortObject), 6)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt32(serializer, object->entriesInUse)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt32(serializer, object->profileEntries)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int ser_saveAccount(
    gxSerializerSettings* serializeSettings,
    gxAccount* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_setUInt8(serializer, object->paymentMode)) == 0)
        {
            ret = bb_setUInt8(serializer, object->accountStatus);
        }
    }
    if ((!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->currentCreditInUse)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->currentCreditStatus)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setInt32(serializer, object->availableCredit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setInt32(serializer, object->amountToClear)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setInt32(serializer, object->clearanceThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setInt32(serializer, object->aggregatedDebt)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 13) && (ret = ser_saveDateTime(&object->accountActivationTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 14) && (ret = ser_saveDateTime(&object->accountClosureTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 16) && (ret = bb_setInt32(serializer, object->lowCreditThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 17) && (ret = bb_setInt32(serializer, object->nextCreditAvailableThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 18) && (ret = bb_setUInt16(serializer, object->maxProvision)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 19) && (ret = bb_setInt32(serializer, object->maxProvisionPeriod)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int ser_saveCredit(
    gxSerializerSettings* serializeSettings,
    gxCredit* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setInt32(serializer, object->currentCreditAmount)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->type)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->priority)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setInt32(serializer, object->warningThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setInt32(serializer, object->limit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_setUInt8(serializer, object->creditConfiguration)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt8(serializer, object->status)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setInt32(serializer, object->presetCreditAmount)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = bb_setInt32(serializer, object->creditAvailableThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 11) && (ret = ser_saveDateTime(&object->period, serializer)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE

int ser_saveUnitCharge(gxByteBuffer* serializer, gxUnitCharge* target)
{
    int ret = 0, pos;
    gxChargeTable* it;
    if (//commodity scale
        (ret = cosem_setInt8(serializer, target->chargePerUnitScaling.commodityScale)) != 0 ||
        //price scale
        (ret = cosem_setInt8(serializer, target->chargePerUnitScaling.priceScale)) != 0 ||
        //-------------
        //commodity
        //type
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_setUInt16(serializer, target->commodity.target == 0 ? 0 : target->commodity.target->objectType)) != 0 ||
#else
        (ret = cosem_setUInt16(serializer, target->commodity.type)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //logicalName
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_setOctetString2(serializer, obj_getLogicalName(target->commodity.target), 6)) != 0 ||
#else
        (ret = cosem_setOctetString2(serializer, target->commodity.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //attributeIndex
        (ret = cosem_setInt8(serializer, target->commodity.attributeIndex)) != 0 ||
        //-------------
        //chargeTables
        (ret = hlp_setObjectCount(target->chargeTables.size, serializer)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != target->chargeTables.size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&it, sizeof(gxChargeTable))) != 0 ||
            //index
            (ret = cosem_setOctetString2(serializer, it->index.data, it->index.size)) != 0 ||
            //chargePerUnit
            (ret = cosem_setInt16(serializer, it->chargePerUnit)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&it)) != 0 ||
            //index
            (ret = cosem_setOctetString(serializer, &it->index)) != 0 ||
            //chargePerUnit
            (ret = cosem_setInt16(serializer, it->chargePerUnit)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}


int ser_saveCharge(
    gxSerializerSettings* serializeSettings,
    gxCharge* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setInt32(serializer, object->totalAmountPaid)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->chargeType)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->priority)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_saveUnitCharge(serializer, &object->unitChargeActive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_saveUnitCharge(serializer, &object->unitChargePassive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_saveDateTime(&object->unitChargeActivationTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_setUInt32(serializer, object->period)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_setUInt8(serializer, object->chargeConfiguration)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = ser_saveDateTime(&object->lastCollectionTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 11) && (ret = bb_setUInt32(serializer, object->lastCollectionAmount)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 12) && (ret = bb_setUInt32(serializer, object->totalAmountRemaining)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 13) && (ret = bb_setUInt16(serializer, object->proportion)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int ser_saveTokenGateway(
    gxSerializerSettings* serializeSettings,
    gxTokenGateway* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int ser_saveGsmDiagnostic(
    gxSerializerSettings* serializeSettings,
    gxGsmDiagnostic* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxAdjacentCell* it;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = ser_saveOctetString(serializer, &object->operatorName);
#else
        ret = ser_saveOctetString2(serializer, object->operatorName);
#endif //DLMS_IGNORE_MALLOC
    }
    if ((!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt8(serializer, object->status)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->circuitSwitchStatus)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_setUInt8(serializer, object->packetSwitchStatus)) != 0))
    {
    }

    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 6))
    {
        if ((ret = bb_setUInt32(serializer, object->cellInfo.cellId)) != 0 ||
            (ret = bb_setUInt16(serializer, object->cellInfo.locationId)) != 0 ||
            (ret = bb_setUInt8(serializer, object->cellInfo.signalQuality)) != 0 ||
            (ret = bb_setUInt8(serializer, object->cellInfo.ber)) != 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        if ((ret = hlp_setObjectCount(object->adjacentCells.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->adjacentCells.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it, sizeof(gxAdjacentCell))) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_setUInt32(serializer, it->cellId)) != 0 ||
                    (ret = bb_setUInt8(serializer, it->signalQuality)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8))
    {
        ret = ser_saveDateTime(&object->captureTime, serializer);
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
int ser_saveCompactData(
    gxSerializerSettings* serializeSettings,
    gxCompactData* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_saveOctetString(serializer, &object->buffer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = ser_saveObjects2(serializer, &object->captureObjects)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_setUInt8(serializer, object->templateId)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_saveOctetString(serializer, &object->templateDescription)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_setUInt8(serializer, object->captureMethod)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int ser_saveLlcSscsSetup(
    gxSerializerSettings* serializeSettings,
    gxLlcSscsSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_setUInt16(serializer, object->serviceNodeAddress)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_setUInt16(serializer, object->baseNodeAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int ser_savePrimeNbOfdmPlcPhysicalLayerCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcPhysicalLayerCounters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int ser_savePrimeNbOfdmPlcMacSetup(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacSetup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int ser_savePrimeNbOfdmPlcMacFunctionalParameters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacFunctionalParameters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int ser_savePrimeNbOfdmPlcMacCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacCounters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int ser_savePrimeNbOfdmPlcMacNetworkAdministrationData(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int ser_savePrimeNbOfdmPlcApplicationsIdentification(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcApplicationsIdentification* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
int ser_saveArbitrator(
    gxSerializerSettings* serializeSettings,
    gxArbitrator* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int ser_saveIec8802LlcType1Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType1Setup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int ser_saveIec8802LlcType2Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType2Setup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int ser_saveIec8802LlcType3Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType3Setup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int ser_saveSFSKActiveInitiator(
    gxSerializerSettings* serializeSettings,
    gxSFSKActiveInitiator* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int ser_saveFSKMacCounters(
    gxSerializerSettings* serializeSettings,
    gxFSKMacCounters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int ser_saveSFSKMacSynchronizationTimeouts(
    gxSerializerSettings* serializeSettings,
    gxSFSKMacSynchronizationTimeouts* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int ser_saveSFSKPhyMacSetUp(
    gxSerializerSettings* serializeSettings,
    gxSFSKPhyMacSetUp* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int ser_saveSFSKReportingSystemList(
    gxSerializerSettings* serializeSettings,
    gxSFSKReportingSystemList* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifndef DLMS_IGNORE_SCHEDULE
int ser_saveSchedule(
    gxSerializerSettings* serializeSettings,
    gxSchedule* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SCHEDULE

#ifdef DLMS_ITALIAN_STANDARD
int ser_saveTariffPlan(
    gxSerializerSettings* serializeSettings,
    gxTariffPlan* object,
    gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_addString(serializer, object->calendarName)) != 0 ||
        (ret = bb_setUInt8(serializer, object->enabled)) != 0 ||
        (ret = ser_saveDateTime(&object->activationTime, serializer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_ITALIAN_STANDARD

int ser_saveObject(
    gxSerializerSettings* serializeSettings,
    gxObject* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    switch (object->objectType)
    {
#ifndef DLMS_IGNORE_DATA
    case DLMS_OBJECT_TYPE_DATA:
        ret = ser_saveData(serializeSettings, (gxData*)object, serializer);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = ser_saveRegister(serializeSettings, (gxRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = ser_saveClock(serializeSettings, (gxClock*)object, serializer);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = ser_saveActionSchedule(serializeSettings, (gxActionSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = ser_saveActivityCalendar(serializeSettings, (gxActivityCalendar*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = ser_saveAssociationLogicalName(serializeSettings, (gxAssociationLogicalName*)object, serializer);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = ser_saveAssociationShortName(serializeSettings, (gxAssociationShortName*)object, serializer);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = ser_saveAutoAnswer(serializeSettings, (gxAutoAnswer*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = ser_saveAutoConnect(serializeSettings, (gxAutoConnect*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = ser_saveDemandRegister(serializeSettings, (gxDemandRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = ser_saveMacAddressSetup(serializeSettings, (gxMacAddressSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = ser_saveExtendedRegister(serializeSettings, (gxExtendedRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
#ifndef DLMS_IGNORE_GPRS_SETUP
        ret = ser_saveGPRSSetup(serializeSettings, (gxGPRSSetup*)object, serializer);
#endif //DLMS_IGNORE_GPRS_SETUP
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = ser_saveSecuritySetup(serializeSettings, (gxSecuritySetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = ser_saveHdlcSetup(serializeSettings, (gxIecHdlcSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = ser_saveLocalPortSetup(serializeSettings, (gxLocalPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = ser_saveIecTwistedPairSetup(serializeSettings, (gxIecTwistedPairSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = ser_saveIp4Setup(serializeSettings, (gxIp4Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = ser_saveIp6Setup(serializeSettings, (gxIp6Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = ser_saveMbusSlavePortSetup(serializeSettings, (gxMbusSlavePortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = ser_saveImageTransfer(serializeSettings, (gxImageTransfer*)object, serializer);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = ser_saveDisconnectControl(serializeSettings, (gxDisconnectControl*)object, serializer);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = ser_saveLimiter(serializeSettings, (gxLimiter*)object, serializer);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = ser_saveMBusClient(serializeSettings, (gxMBusClient*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = ser_saveModemConfiguration(serializeSettings, (gxModemConfiguration*)object, serializer);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = ser_savePppSetup(serializeSettings, (gxPppSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = ser_saveProfileGeneric(serializeSettings, (gxProfileGeneric*)object, serializer);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = ser_saveRegisterActivation(serializeSettings, (gxRegisterActivation*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = ser_saveRegisterMonitor(serializeSettings, (gxRegisterMonitor*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_REGISTER_TABLE
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_REGISTER_TABLE
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = ser_saveSapAssignment(serializeSettings, (gxSapAssignment*)object, serializer);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = ser_saveSchedule(serializeSettings, (gxSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = ser_saveScriptTable(serializeSettings, (gxScriptTable*)object, serializer);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SMTP_SETUP
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_SMTP_SETUP
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = ser_saveSpecialDaysTable(serializeSettings, (gxSpecialDaysTable*)object, serializer);
        break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_STATUS_MAPPING
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_STATUS_MAPPING
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = ser_saveTcpUdpSetup(serializeSettings, (gxTcpUdpSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = ser_saveUtilityTables(serializeSettings, (gxUtilityTables*)object, serializer);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = ser_saveMBusMasterPortSetup(serializeSettings, (gxMBusMasterPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = ser_savePushSetup(serializeSettings, (gxPushSetup*)object, serializer);
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
        ret = ser_saveAccount(serializeSettings, (gxAccount*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = ser_saveCredit(serializeSettings, (gxCredit*)object, serializer);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = ser_saveCharge(serializeSettings, (gxCharge*)object, serializer);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = ser_saveTokenGateway(serializeSettings, (gxTokenGateway*)object, serializer);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = ser_saveGsmDiagnostic(serializeSettings, (gxGsmDiagnostic*)object, serializer);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = ser_saveCompactData(serializeSettings, (gxCompactData*)object, serializer);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = ser_saveLlcSscsSetup(serializeSettings, (gxLlcSscsSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = ser_savePrimeNbOfdmPlcPhysicalLayerCounters(serializeSettings, (gxPrimeNbOfdmPlcPhysicalLayerCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = ser_savePrimeNbOfdmPlcMacSetup(serializeSettings, (gxPrimeNbOfdmPlcMacSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = ser_savePrimeNbOfdmPlcMacFunctionalParameters(serializeSettings, (gxPrimeNbOfdmPlcMacFunctionalParameters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = ser_savePrimeNbOfdmPlcMacCounters(serializeSettings, (gxPrimeNbOfdmPlcMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = ser_savePrimeNbOfdmPlcMacNetworkAdministrationData(serializeSettings, (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = ser_savePrimeNbOfdmPlcApplicationsIdentification(serializeSettings, (gxPrimeNbOfdmPlcApplicationsIdentification*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = ser_saveArbitrator(serializeSettings, (gxArbitrator*)object, serializer);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = ser_saveIec8802LlcType1Setup(serializeSettings, (gxIec8802LlcType1Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ser_saveIec8802LlcType2Setup(serializeSettings, (gxIec8802LlcType2Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ser_saveIec8802LlcType3Setup(serializeSettings, (gxIec8802LlcType3Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ser_saveSFSKActiveInitiator(serializeSettings, (gxSFSKActiveInitiator*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ser_saveFSKMacCounters(serializeSettings, (gxFSKMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ser_saveSFSKMacSynchronizationTimeouts(serializeSettings, (gxSFSKMacSynchronizationTimeouts*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ser_saveSFSKPhyMacSetUp(serializeSettings, (gxSFSKPhyMacSetUp*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ser_saveSFSKReportingSystemList(serializeSettings, (gxSFSKReportingSystemList*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = ser_saveTariffPlan(serializeSettings, (gxTariffPlan*)object, serializer);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default: //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

//Serialize objects to bytebuffer.
int ser_saveObjects(
    gxSerializerSettings* serializeSettings,
    gxObject** object,
    uint16_t count,
    gxByteBuffer* serializer)
{
    uint16_t pos;
    int ret = 0;
    //Serializer version number.
    bb_setUInt8(serializer, 1);
    for (pos = 0; pos != count; ++pos)
    {
        if ((ret = ser_saveObject(serializeSettings, object[pos], serializer)) != 0)
        {
            break;
        }
    }
    return ret;
}

// --------------------------------------------------------------------------
//Load functionality.

// Get item from the buffer if DLMS_IGNORE_MALLOC is defined.
// Otherwice create the object dynamically.
int ser_getArrayItem(gxArray* arr, uint16_t index, void** value, uint16_t itemSize)
{
#ifdef DLMS_IGNORE_MALLOC
    return arr_getByIndex2(arr, index, value, itemSize);
#else
    unsigned char* it = gxcalloc(1, itemSize);
    *value = it;
    return arr_push(arr, it);
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int ser_verifyArray(gxByteBuffer* bb, gxArray* arr, uint16_t* count)
{
#ifdef DLMS_IGNORE_MALLOC
    * count = arr_getCapacity(arr);
    int ret = cosem_checkArray2(bb, count, 0);
    if (ret == 0)
    {
        arr->size = *count;
    }
    return ret;
#else
    * count = 0xFFFF;
    int ret = hlp_getObjectCount2(bb, count);
    if (ret == 0)
    {
        arr_clear(arr);
        arr_capacity(arr, *count);
    }
    return ret;
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}

int ser_verifyObjectArray(gxByteBuffer* bb, objectArray* arr, uint16_t* count)
{
    oa_empty(arr);
    *count = 0xFFFF;
    int ret = hlp_getObjectCount2(bb, count);
    if (ret == 0)
    {
        oa_capacity(arr, *count);
    }
    return ret;
}

int ser_verifyVariantArray(gxByteBuffer* bb, variantArray* arr, uint16_t* count)
{
#ifdef DLMS_IGNORE_MALLOC
    * count = va_getCapacity(arr);
    int ret = cosem_checkArray2(bb, count, 0);
    if (ret == 0)
    {
        arr->size = *count;
    }
    return ret;
#else
    * count = 0xFFFF;
    int ret = hlp_getObjectCount2(bb, count);
    if (ret == 0)
    {
        va_clear(arr);
        va_capacity(arr, *count);
    }
    return ret;
#endif //DLMS_COSEM_EXACT_DATA_TYPES
}


int ser_loadDateTime(gxtime* value, gxByteBuffer* serializer)
{
    return cosem_getDateTime2(serializer, value, 1);
}

int ser_loadOctetString(gxByteBuffer* serializer, gxByteBuffer* value)
{
    int ret;
    uint16_t count;
    if ((ret = hlp_getObjectCount2(serializer, &count)) != 0)
    {
        return ret;
    }
    if ((ret = bb_clear(value)) != 0 ||
        (ret = bb_set2(value, serializer, serializer->position, count)) != 0)
    {
    }
    return ret;
}

int ser_loadOctetString2(gxByteBuffer* serializer, char** value)
{
    int ret;
    uint16_t count;
    if ((ret = hlp_getObjectCount2(serializer, &count)) != 0)
    {
        return ret;
    }
#if !defined(DLMS_USE_CUSTOM_MALLOC) && !defined(DLMS_IGNORE_MALLOC)
    if (*value != NULL)
    {
        gxfree(*value);
    }
    *value = (char*)gxmalloc(count + 1);
#endif //#if !defined(DLMS_USE_CUSTOM_MALLOC) && !defined(DLMS_IGNORE_MALLOC)
    (*value)[count] = 0;
    return bb_get(serializer, (unsigned char*)*value, count);
}


int ser_loadOctetString3(
    gxByteBuffer* serializer,
    unsigned char* value,
    uint16_t* count)
{
    int ret;
    if ((ret = hlp_getObjectCount2(serializer, count)) != 0)
    {
        return ret;
    }
    value[*count] = 0;
    return bb_get(serializer, value, *count);
}

int ser_loadBitString(gxByteBuffer* serializer, bitArray* value)
{
    int ret;
    uint16_t count;
    if ((ret = hlp_getObjectCount2(serializer, &count)) != 0)
    {
        return ret;
    }
    if ((ret = ba_copy(value, serializer->data + serializer->position, count)) == 0)
    {
        serializer->position += ba_getByteCount(count);
    }
    return ret;
}

int getVariantFromBytes(dlmsVARIANT* data,
    gxByteBuffer* serializer)
{
    return cosem_getVariant(serializer, data);
}

#ifndef DLMS_IGNORE_DATA
int ser_loadData(gxSerializerSettings* serializeSettings, gxData* object, gxByteBuffer* serializer)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        return getVariantFromBytes(&object->value, serializer);
    }
    return 0;
}

#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int ser_loadRegister(gxSerializerSettings* serializeSettings, gxRegister* object, gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getInt8(serializer, &object->scaler)) != 0 ||
            (ret = bb_getUInt8(serializer, &object->unit)) != 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = getVariantFromBytes(&object->value, serializer);
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
int ser_loadClock(gxSerializerSettings* serializeSettings, gxClock* object, gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_loadDateTime(&object->time, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getInt16(serializer, &object->timeZone)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->status)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_loadDateTime(&object->begin, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_loadDateTime(&object->end, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_getInt8(serializer, &object->deviation)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getUInt8(serializer, &object->enabled)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->clockBase)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int ser_loadScriptTable(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxScriptTable* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos, pos2;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxScript* s;
    gxScriptAction* sa;
    uint16_t count, count2;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        obj_clearScriptTable(&object->scripts);
        if ((ret = ser_verifyArray(serializer, &object->scripts, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->scripts, pos, (void**)&s, sizeof(gxScript))) != 0)
                {
                    break;
                }
                if ((ret = bb_getUInt16(serializer, &s->id)) != 0)
                {
                    break;
                }
                if ((ret = ser_verifyArray(serializer, &s->actions, &count2)) == 0)
                {
                    for (pos2 = 0; pos2 != count2; ++pos2)
                    {
                        if ((ret = ser_getArrayItem(&s->actions, pos2, (void**)&sa, sizeof(gxScriptAction))) != 0)
                        {
                            break;
                        }
                        if ((ret = bb_getUInt8(serializer, (unsigned char*)&sa->type)) != 0)
                        {
                            break;
                        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        uint16_t ot;
                        unsigned char ln[6];
                        if ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
                            (ret = bb_get(serializer, ln, 6)) != 0)
                        {
                            break;
                        }
                        if ((ret = cosem_findObjectByLN(settings, ot, ln, &sa->target)) != 0)
                        {
                            break;
                        }
                        if (sa->target == NULL)
                        {
                            ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                            break;
                        }
#else
                        if ((ret = bb_getUInt16(serializer, sa->objectType)) != 0 ||
                            (ret = bb_get(serializer, sa->logicalName, sizeof(sa->logicalName))) != 0)
                        {
                            break;
                        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        if ((ret = bb_getInt8(serializer, &sa->index)) != 0 ||
                            (ret = getVariantFromBytes(&sa->parameter, serializer)) != 0)
                        {
                            break;
                        }
                    }
                }
                if (ret != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int ser_loadSpecialDaysTable(
    gxSerializerSettings* serializeSettings,
    gxSpecialDaysTable* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxSpecialDay* sd;
    uint16_t count;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        arr_clear(&object->entries);
        if ((ret = ser_verifyArray(serializer, &object->entries, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->entries, pos, (void**)&sd, sizeof(gxSpecialDay))) != 0)
                {
                    break;
                }
                if ((ret = bb_getUInt16(serializer, &sd->index)) != 0 ||
                    (ret = ser_loadDateTime(&sd->date, serializer)) != 0 ||
                    (ret = bb_getUInt8(serializer, &sd->dayId)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int ser_loadTcpUdpSetup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxTcpUdpSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char ln[6];
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = bb_getUInt16(serializer, &object->port);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_get(serializer, ln, 6)) == 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->ipSetup)) != 0)
            {
                return ret;
            }
#else
            if (ret = bb_get(&ba, object->ipReference, 6)) != 0 ||
            {
                return ret;
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt16(serializer, &object->maximumSegmentSize)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt8(serializer, &object->maximumSimultaneousConnections)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt16(serializer, &object->inactivityTimeout)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int ser_loadMBusMasterPortSetup(
    gxSerializerSettings* serializeSettings,
    gxMBusMasterPortSetup* object,
    gxByteBuffer* serializer)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        return bb_getUInt8(serializer, (unsigned char*)&object->commSpeed);
    }
    return 0;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

int ser_loadObjects2(
    dlmsSettings* settings,
    gxByteBuffer* serializer,
    gxArray* objects)
{
    uint16_t pos;
    int ret;
    uint16_t count;
    uint16_t ot;
    unsigned char ln[6];
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
#else
    gxObject* key;
    gxTarget* value;
#endif //DLMS_IGNORE_MALLOC
    obj_clearProfileGenericCaptureObjects(objects);
    if ((ret = ser_verifyArray(serializer, objects, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            if ((ret = ser_getArrayItem(objects, pos, (void**)&it, sizeof(gxTarget))) != 0)
            {
                break;
            }
            if ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
                (ret = bb_get(serializer, ln, 6)) != 0 ||
                (ret = bb_getInt8(serializer, &it->attributeIndex)) != 0 ||
                (ret = bb_getUInt16(serializer, &it->dataIndex)) != 0 ||
                (ret = cosem_findObjectByLN(settings, ot, ln, &it->target)) != 0)
            {
                break;
            }

#else
            value = (gxTarget*)gxmalloc(sizeof(gxTarget));
            if (value == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
                (ret = bb_get(serializer, ln, 6)) != 0 ||
                (ret = cosem_findObjectByLN(settings, ot, ln, &key)) != 0 ||
                (ret = bb_getInt8(serializer, &value->attributeIndex)) != 0 ||
                (ret = bb_getUInt16(serializer, &value->dataIndex)) != 0)
            {
                gxfree(value);
                break;
            }
            if ((ret = arr_push(objects, key_init(key, value))) != 0)
            {
                break;
            }
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
        }
    }
    return ret;
}

int loadTimeWindow(
    gxByteBuffer* serializer,
    gxArray* arr)
{
    int ret;
    uint16_t pos;
    gxtime* start, * end;
    uint16_t count;
#ifdef DLMS_IGNORE_MALLOC
    gxTimePair* k;
#endif //DLMS_IGNORE_MALLOC
    arr_clearKeyValuePair(arr);
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        arr->size = count;
#endif //DLMS_IGNORE_MALLOC
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&k, sizeof(gxTimePair))) != 0)
            {
                break;
            }
            start = &k->first;
            end = &k->second;
#else
            start = (gxtime*)gxmalloc(sizeof(gxtime));
            if (start == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            end = (gxtime*)gxmalloc(sizeof(gxtime));
            if (end == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = arr_push(arr, key_init(start, end))) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = ser_loadDateTime(start, serializer)) != 0 ||
                (ret = ser_loadDateTime(end, serializer)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP
int ser_loadPushSetup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxPushSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = ser_loadObjects2(settings, serializer, &object->pushObjectList);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->service)) != 0 ||
            (ret = ser_loadOctetString(serializer, &object->destination)) != 0 ||
            (ret = bb_getUInt8(serializer, (unsigned char*)&object->message)) != 0)
        {
        }
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 4) && (ret = loadTimeWindow(serializer, &object->communicationWindow)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt16(serializer, &object->randomisationStartInterval)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt8(serializer, &object->numberOfRetries)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_getUInt16(serializer, &object->repetitionDelay)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_AUTO_CONNECT
int ser_loadAutoConnect(
    gxSerializerSettings* serializeSettings,
    gxAutoConnect* object,
    gxByteBuffer* serializer)
{
    uint16_t count;
#ifdef DLMS_IGNORE_MALLOC
    gxDestination* dest;
#else
    gxByteBuffer* dest;
#endif //DLMS_IGNORE_MALLOC
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = bb_getUInt8(serializer, (unsigned char*)&object->mode);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        ret = bb_getUInt8(serializer, &object->repetitions);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        ret = bb_getUInt16(serializer, &object->repetitionDelay);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        ret = loadTimeWindow(serializer, &object->callingWindow);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 6))
    {
        arr_clearStrings(&object->destinations);
        if (ret == 0 && (ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
#ifdef DLMS_IGNORE_MALLOC
            object->destinations.size = count;
#endif //DLMS_IGNORE_MALLOC
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->destinations, pos, (void**)&dest, sizeof(gxDestination))) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString3(serializer, dest->value, &dest->size)) != 0)
                {
                    break;
                }
#else
                dest = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (dest == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                bb_init(dest);
                if ((ret = arr_push(&object->destinations, dest)) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString(serializer, dest)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int ser_loadSeasonProfile(
    gxArray* arr,
    gxByteBuffer* serializer)
{
    gxSeasonProfile* it;
    int pos, ret;
    uint16_t count;
    if ((ret = obj_clearSeasonProfile(arr)) == 0 &&
        (ret = ser_verifyArray(serializer, arr, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(arr, pos, (void**)&it, sizeof(gxSeasonProfile))) != 0)
            {
                break;
            }
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = ser_loadOctetString3(serializer, it->name.value, &it->name.size)) != 0 ||
                (ret = ser_loadDateTime(&it->start, serializer)) != 0 ||
                (ret = ser_loadOctetString3(serializer, it->weekName.value, &it->weekName.size)) != 0)
            {
                break;
            }
#else
            if ((ret = ser_loadOctetString(serializer, &it->name)) != 0 ||
                (ret = ser_loadDateTime(&it->start, serializer)) != 0 ||
                (ret = ser_loadOctetString(serializer, &it->weekName)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}

int ser_loadweekProfile(
    gxArray* arr,
    gxByteBuffer* serializer)
{
    gxWeekProfile* it;
    int pos, ret;
    uint16_t count;
    if ((ret = obj_clearWeekProfileTable(arr)) == 0 &&
        (ret = ser_verifyArray(serializer, arr, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(arr, pos, (void**)&it, sizeof(gxWeekProfile))) != 0)
            {
                break;
            }
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = ser_loadOctetString3(serializer, it->name.value, &it->name.size)) != 0)
            {
                break;
            }
#else
            if ((ret = ser_loadOctetString(serializer, &it->name)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_getUInt8(serializer, &it->monday)) != 0 ||
                (ret = bb_getUInt8(serializer, &it->tuesday)) != 0 ||
                (ret = bb_getUInt8(serializer, &it->wednesday)) != 0 ||
                (ret = bb_getUInt8(serializer, &it->thursday)) != 0 ||
                (ret = bb_getUInt8(serializer, &it->friday)) != 0 ||
                (ret = bb_getUInt8(serializer, &it->saturday)) != 0 ||
                (ret = bb_getUInt8(serializer, &it->sunday)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int ser_loadDayProfile(
    dlmsSettings* settings,
    gxArray* arr,
    gxByteBuffer* serializer)
{
    gxDayProfile* dp;
    gxDayProfileAction* it;
    int pos, pos2, ret;
    uint16_t count, count2;

    if ((ret = obj_clearDayProfileTable(arr)) == 0 &&
        (ret = ser_verifyArray(serializer, arr, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(arr, pos, (void**)&dp, sizeof(gxDayProfile))) != 0)
            {
                break;
            }
            if ((ret = bb_getUInt8(serializer, &dp->dayId)) != 0 ||
                (ret = ser_verifyArray(serializer, &dp->daySchedules, &count2)) != 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != count2; ++pos2)
            {
                if ((ret = ser_getArrayItem(&dp->daySchedules, pos2, (void**)&it, sizeof(gxDayProfileAction))) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                unsigned char ln[6];
                if ((ret = bb_get(serializer, ln, 6)) != 0)
                {
                    break;
                }
                if ((ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &it->script)) != 0)
                {
                    break;
                }
#else
                if ((ret = bb_get(serializer, it->scriptLogicalName, 6)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
                if ((ret = bb_getUInt16(serializer, &it->scriptSelector)) != 0 ||
                    (ret = ser_loadDateTime(&it->startTime, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}

int ser_loadActivityCalendar(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxActivityCalendar* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_loadOctetString(serializer, &object->calendarNameActive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = ser_loadSeasonProfile(&object->seasonProfileActive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_loadweekProfile(&object->weekProfileTableActive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_loadDayProfile(settings, &object->dayProfileTableActive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_loadOctetString(serializer, &object->calendarNamePassive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_loadSeasonProfile(&object->seasonProfilePassive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = ser_loadweekProfile(&object->weekProfileTablePassive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = ser_loadDayProfile(settings, &object->dayProfileTablePassive, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = ser_loadDateTime(&object->time, serializer)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_SECURITY_SETUP
int ser_loadSecuritySetup(
    gxSerializerSettings* serializeSettings,
    gxSecuritySetup* object,
    gxByteBuffer* serializer)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char v;
    int ret = 0;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_getUInt8(serializer, &v)) == 0)
        {
            object->securityPolicy = (DLMS_SECURITY_POLICY)v;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, &v)) == 0)
        {
            object->securitySuite = (DLMS_SECURITY_SUITE)v;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = bb_clear(&object->serverSystemTitle)) == 0)
        {
            ret = bb_set2(&object->serverSystemTitle, serializer, serializer->position, 8);
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        if ((ret = bb_clear(&object->clientSystemTitle)) == 0)
        {
            ret = bb_set2(&object->clientSystemTitle, serializer, serializer->position, 8);
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int ser_loadHdlcSetup(
    gxSerializerSettings* serializeSettings,
    gxIecHdlcSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->communicationSpeed)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getUInt8(serializer, &object->windowSizeTransmit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt8(serializer, &object->windowSizeReceive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt16(serializer, &object->maximumInfoLengthTransmit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt16(serializer, &object->maximumInfoLengthReceive)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_getUInt16(serializer, &object->interCharachterTimeout)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getUInt16(serializer, &object->inactivityTimeout)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_getUInt16(serializer, &object->deviceAddress)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int ser_loadLocalPortSetup(
    gxSerializerSettings* serializeSettings,
    gxLocalPortSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    unsigned char ch;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->defaultMode = (DLMS_OPTICAL_PROTOCOL_MODE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->defaultBaudrate = (DLMS_BAUD_RATE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->proposedBaudrate = (DLMS_BAUD_RATE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->responseTime = (DLMS_LOCAL_PORT_RESPONSE_TIME)ch;
        }
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_loadOctetString(serializer, &object->deviceAddress)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_loadOctetString(serializer, &object->password1)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = ser_loadOctetString(serializer, &object->password2)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = ser_loadOctetString(serializer, &object->password5)) != 0))
        {

        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int ser_loadIecTwistedPairSetup(
    gxSerializerSettings* serializeSettings,
    gxIecTwistedPairSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->mode)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->speed)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_loadOctetString(serializer, &object->primaryAddresses)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_loadOctetString(serializer, &object->tabis)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int ser_loadDemandRegister(
    gxSerializerSettings* serializeSettings,
    gxDemandRegister* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = getVariantFromBytes(&object->currentAverageValue, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = getVariantFromBytes(&object->lastAverageValue, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getInt8(serializer, &object->scaler)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt8(serializer, &object->unit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = getVariantFromBytes(&object->status, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_loadDateTime(&object->captureTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = ser_loadDateTime(&object->startTimeCurrent, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_getUInt32(serializer, &object->period)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = bb_getUInt16(serializer, &object->numberOfPeriods)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int ser_loadRegisterActivation(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxRegisterActivation* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char ln[6];
    uint16_t ot;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxRegisterActivationMask* it;
#if defined(DLMS_IGNORE_MALLOC)
    uint16_t v;
#endif //defined(DLMS_IGNORE_MALLOC)
#else
    gxByteBuffer* key, * value;
#endif //DLMS_IGNORE_MALLOC
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    uint16_t count;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        obj_clearRegisterActivationAssignment(&object->registerAssignment);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = ser_verifyArray(serializer, &object->registerAssignment, &count)) == 0)
#else
        if ((ret = ser_verifyObjectArray(serializer, &object->registerAssignment, &count)) == 0)
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od, sizeof(gxObject))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
                    (ret = bb_get(serializer, ln, 6)) != 0 ||
                    (ret = cosem_findObjectByLN(settings, ot, ln, &od)) != 0)
                {
                    break;
                }
#if defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_push(&object->registerAssignment, od)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = oa_push(&object->registerAssignment, od)) != 0)
                {
                    break;
                }
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        obj_clearRegisterActivationMaskList(&object->maskList);
        if (ret == 0 && (ret = ser_verifyArray(serializer, &object->maskList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = ser_getArrayItem(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask))) != 0)
                {
                    break;
                }
#if defined(DLMS_IGNORE_MALLOC)
                if ((ret = ser_loadOctetString3(serializer, it->name, &v)) != 0)
                {
                    break;
                }
                it->length = (unsigned char)v;
                if ((ret = ser_loadOctetString3(serializer, it->indexes, &v)) != 0)
                {
                    break;
                }
                it->count = (unsigned char)v;
#else
                if ((ret = ser_loadOctetString(serializer, &it->name)) != 0 ||
                    (ret = ser_loadOctetString(serializer, &it->indexes)) != 0)
                {
                    break;
                }
#endif //defined(DLMS_IGNORE_MALLOC)
#else
                key = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (key == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                bb_init(key);
                value = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (value == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                bb_init(value);
                if ((ret = arr_push(&object->maskList, key_init(key, value))) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString(serializer, key)) != 0 ||
                    (ret = ser_loadOctetString(serializer, value)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        ret = ser_loadOctetString(serializer, &object->activeMask);
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int ser_loadActionItem(
    dlmsSettings* settings,
    gxActionItem* item,
    gxByteBuffer* serializer)
{
    int ret;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    if ((ret = bb_get(serializer, ln, 6)) != 0 ||
        (ret = bb_getUInt16(serializer, &item->scriptSelector)) != 0 ||
        (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&item->script)) != 0)
    {
    }
#else
    if ((ret = bb_get(serializer, item->logicalName, 6)) != 0 ||
        (ret = bb_getUInt16(serializer, item->scriptSelector)) != 0)
    {
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    return ret;
}

int ser_loadRegisterMonitor(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxRegisterMonitor* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    dlmsVARIANT_PTR tmp;
    gxActionSet* as;
    uint16_t count;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = ser_verifyVariantArray(serializer, &object->thresholds, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = va_getByIndex(&object->thresholds, pos, &tmp)) != 0)
                {
                    break;
                }
                var_clear(tmp);
#else
                tmp = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                if (tmp == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                var_init(tmp);
                if ((ret = va_push(&object->thresholds, tmp)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = getVariantFromBytes(tmp, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t ot;
        unsigned char ln[6];
        if ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
            (ret = bb_get(serializer, ln, 6)) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->monitoredValue.target)) != 0 ||
            (ret = bb_getInt8(serializer, &object->monitoredValue.attributeIndex)) != 0)
        {

        }
#else
        if ((ret = bb_getUInt16(serializer, &object->monitoredValue.objectType)) != 0)
            (ret = bb_get(serializer, object->monitoredValue.logicalName, 6)) != 0 ||
            (ret = bb_getInt8(serializer, &object->monitoredValue.attributeIndex)) != 0)
    {

    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = ser_verifyArray(serializer, &object->actions, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->actions, pos, (void**)&as, sizeof(gxActionSet))) != 0 ||
                    (ret = ser_loadActionItem(settings, &as->actionUp, serializer)) != 0 ||
                    (ret = ser_loadActionItem(settings, &as->actionDown, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int ser_loadActionSchedule(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxActionSchedule* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    unsigned char ch;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    gxtime* tm;
    uint16_t count;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = bb_get(serializer, ln, 6)) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, (gxObject**)&object->executedScript)) != 0 ||
            (ret = bb_getUInt16(serializer, &object->executedScriptSelector)) != 0)
        {
        }
#else
        if ((ret = bb_get(serializer, object->executedScriptLogicalName, 6)) != 0 ||
            (ret = bb_getUInt16(serializer, &object->executedScriptSelector)) == 0)
        {
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->type = ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = ser_verifyArray(serializer, &object->executionTime, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0 ||
                    (ret = ser_loadDateTime(tm, serializer)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int ser_loadSapAssignment(
    gxSerializerSettings* serializeSettings,
    gxSapAssignment* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos;
    gxSapItem* it;
    uint16_t count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        obj_clear((gxObject*)object);
        //Add count.
        if ((ret = ser_verifyArray(serializer, &object->sapAssignmentList, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0 ||
                    (ret = bb_getUInt16(serializer, &it->id)) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = ser_loadOctetString3(serializer, it->name.value, &it->name.size)) != 0)
                {
                    break;
                }
#else
                if ((ret = ser_loadOctetString(serializer, &it->name)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_AUTO_ANSWER
int ser_loadAutoAnswer(
    gxSerializerSettings* serializeSettings,
    gxAutoAnswer* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    unsigned char ch;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->mode = ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        ret = loadTimeWindow(serializer, &object->listeningWindow);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->status = ch;
        }
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt8(serializer, &object->numberOfCalls)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt8(serializer, &object->numberOfRingsInListeningWindow)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_getUInt8(serializer, &object->numberOfRingsOutListeningWindow)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getUInt8(serializer, &object->numberOfRingsOutListeningWindow)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int ser_loadIp4Setup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxIp4Setup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    uint32_t* tmp;
#else
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* ip;
    uint16_t pos, count;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_get(serializer, ln, 6)) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
        {
            return ret;
        }
#else
    if ((ret = bb_get(serializer, object->dataLinkLayerReference, 6)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
{
    ret = bb_getUInt32(serializer, &object->ipAddress);
}
if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
{
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        object->multicastIPAddress.size = count;
#endif //DLMS_IGNORE_MALLOC
        for (pos = 0; pos != count; ++pos)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            if ((ret = ser_getArrayItem(&object->multicastIPAddress, pos, (void**)&tmp, sizeof(uint32_t))) != 0 ||
                (ret = bb_getUInt32(serializer, tmp)) != 0)
            {
                break;
            }
#else
            tmp = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
            if (tmp == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
            tmp->vt = DLMS_DATA_TYPE_UINT32;
            if ((ret = va_push(&object->multicastIPAddress, tmp)) != 0 ||
                (ret = bb_getUInt32(serializer, &tmp->ulVal)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
}
if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
{
    if ((ret = ser_verifyArray(serializer, &object->ipOptions, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ser_getArrayItem(&object->ipOptions, pos, (void**)&ip, sizeof(gxip4SetupIpOption))) != 0 ||
                (ret = bb_getUInt8(serializer, (unsigned char*)&ip->type)) != 0)
            {
                break;
            }
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = ser_loadOctetString3(serializer, ip->data.value, &ip->data.size)) != 0)
            {
                break;
            }
#else
            if ((ret = ser_loadOctetString(serializer, &ip->data)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
}
if (ret == 0)
{
    if ((!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt32(serializer, &object->subnetMask)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_getUInt32(serializer, &object->gatewayIPAddress)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getUInt8(serializer, &object->useDHCP)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_getUInt32(serializer, &object->primaryDNSAddress)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = bb_getUInt32(serializer, &object->secondaryDNSAddress)) != 0))
    {
    }
}
return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP
int ser_loadIp6Setup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxIp6Setup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    IN6_ADDR* ip;
    uint16_t pos, count;
    unsigned char ch;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        obj_clear((gxObject*)object);
        if ((ret = bb_get(serializer, ln, 6)) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
        {
            return ret;
        }
#else
        if ((ret = bb_get(serializer, object->dataLinkLayerReference, 6)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->addressConfigMode = (DLMS_IP6_ADDRESS_CONFIG_MODE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        arr_clear(&object->unicastIPAddress);
        if (ret == 0 && (ret = ser_verifyArray(serializer, &object->unicastIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->unicastIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                    (ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        arr_clear(&object->multicastIPAddress);
        if (ret == 0 && (ret = ser_verifyArray(serializer, &object->multicastIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->multicastIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                    (ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 6))
    {
        arr_clear(&object->gatewayIPAddress);
        if (ret == 0 && (ret = ser_verifyArray(serializer, &object->gatewayIPAddress, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->gatewayIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                    (ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        ret = bb_get(serializer, (unsigned char*)&object->primaryDNSAddress, 16);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8))
    {
        ret = bb_get(serializer, (unsigned char*)&object->secondaryDNSAddress, 16);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 9))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->trafficClass = (DLMS_IP6_ADDRESS_CONFIG_MODE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 10))
    {
        arr_clear(&object->neighborDiscoverySetup);
        gxNeighborDiscoverySetup* it2;
        if ((ret = ser_verifyArray(serializer, &object->neighborDiscoverySetup, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->neighborDiscoverySetup, pos, (void**)&it2, sizeof(gxNeighborDiscoverySetup))) != 0 ||
                    (ret = bb_getUInt8(serializer, &it2->maxRetry)) != 0 ||
                    (ret = bb_getUInt16(serializer, &it2->retryWaitTime)) != 0 ||
                    (ret = bb_getUInt32(serializer, &it2->sendPeriod)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
int ser_loadUtilityTables(
    gxSerializerSettings* serializeSettings,
    gxUtilityTables* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_getUInt16(serializer, &object->tableId)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_loadOctetString(serializer, &object->buffer)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int ser_loadMbusSlavePortSetup(
    gxSerializerSettings* serializeSettings,
    gxMbusSlavePortSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->defaultBaud)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->availableBaud)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->addressState)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->busAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int ser_loadImageTransfer(
    gxSerializerSettings* serializeSettings,
    gxImageTransfer* object,
    gxByteBuffer* serializer)
{
    uint16_t count;
    int pos, ret = 0;
    gxImageActivateInfo* it;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    obj_clear((gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_getUInt32(serializer, &object->imageBlockSize)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = ser_loadBitString(serializer, &object->imageTransferredBlocksStatus)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt32(serializer, &object->imageFirstNotTransferredBlockNumber)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt8(serializer, &object->imageTransferEnabled)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->imageTransferStatus)) != 0))
    {
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        if ((ret = ser_verifyArray(serializer, &object->imageActivateInfo, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->imageActivateInfo, pos, (void**)&it, sizeof(gxImageActivateInfo))) != 0 ||
                    (ret = bb_getUInt32(serializer, &it->size)) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = ser_loadOctetString3(serializer, it->identification.data, &it->identification.size)) != 0 ||
                    (ret = ser_loadOctetString3(serializer, it->signature.data, &it->signature.size)) != 0)
                {
                    break;
                }
#else
                if ((ret = ser_loadOctetString(serializer, &it->identification)) != 0 ||
                    (ret = ser_loadOctetString(serializer, &it->signature)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int ser_loadDisconnectControl(
    gxSerializerSettings* serializeSettings,
    gxDisconnectControl* object,
    gxByteBuffer* serializer)
{
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char v;
    int ret = 0;
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = bb_getUInt8(serializer, &object->outputState);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, &v)) == 0)
        {
            object->controlState = (DLMS_CONTROL_STATE)v;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = bb_getUInt8(serializer, &v)) == 0)
        {
            object->controlMode = (DLMS_CONTROL_MODE)v;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int ser_loadLimiter(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxLimiter* object,
    gxByteBuffer* serializer)
{
    int pos, ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    uint16_t* it;
#else
    dlmsVARIANT* it;
#endif //DLMS_IGNORE_MALLOC
    uint16_t count;
    unsigned char ln[6];
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_get(serializer, ln, 6)) == 0 &&
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->monitoredValue)) == 0 &&
            (ret = bb_getInt8(serializer, &object->selectedAttributeIndex)) == 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        ret = getVariantFromBytes(&object->thresholdActive, serializer);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        ret = getVariantFromBytes(&object->thresholdNormal, serializer);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        ret = getVariantFromBytes(&object->thresholdEmergency, serializer);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 6))
    {
        ret = bb_getUInt32(serializer, &object->minOverThresholdDuration);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        ret = bb_getUInt32(serializer, &object->minUnderThresholdDuration);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8))
    {
        if ((ret = bb_getUInt16(serializer, &object->emergencyProfile.id)) == 0 &&
            (ret = ser_loadDateTime(&object->emergencyProfile.activationTime, serializer)) == 0 &&
            (ret = bb_getUInt32(serializer, &object->emergencyProfile.duration)) == 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 9))
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = ser_verifyArray(serializer, &object->emergencyProfileGroupIDs, &count)) == 0)
#else
        if ((ret = ser_verifyVariantArray(serializer, &object->emergencyProfileGroupIDs, &count)) == 0)
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        {
            for (pos = 0; pos != count; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = ser_getArrayItem(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(uint16_t))) != 0 ||
                    (ret = bb_getUInt16(serializer, it)) != 0)
                {
                    break;
                }
#else
                it = (dlmsVARIANT*)gxmalloc(sizeof(dlmsVARIANT));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                var_init(it);
                if ((ret = va_push(&object->emergencyProfileGroupIDs, it)) != 0 ||
                    (ret = getVariantFromBytes(it, serializer)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
        if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 10))
        {
            ret = bb_getUInt8(serializer, &object->emergencyProfileActive);
        }
        if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 11))
        {
            if ((ret = ser_loadActionItem(settings, &object->actionOverThreshold, serializer)) != 0 ||
                (ret = ser_loadActionItem(settings, &object->actionUnderThreshold, serializer)) != 0)
            {
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int ser_loadMBusClient(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxMBusClient* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    unsigned char ch;
    unsigned char ln[6];
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt32(serializer, &object->capturePeriod)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt8(serializer, &object->primaryAddress)) != 0))
    {
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = bb_get(serializer, ln, 6)) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->mBusPort)) != 0)
        {
            return ret;
        }
#else
        if ((ret = bb_get(serializer, object->mBusPortReference, 6)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if ((!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt32(serializer, &object->identificationNumber)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_getUInt16(serializer, &object->manufacturerID)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getUInt8(serializer, &object->dataHeaderVersion)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_getUInt8(serializer, &object->deviceType)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 10) && (ret = bb_getUInt8(serializer, &object->accessNumber)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 11) && (ret = bb_getUInt8(serializer, &object->status)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 12) && (ret = bb_getUInt8(serializer, &object->alarm)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 13) && (ret = bb_getUInt16(serializer, &object->configuration)) != 0))
    {
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 14) && (ret = bb_getUInt8(serializer, &ch)) == 0)
    {
        object->encryptionKeyStatus = (DLMS_MBUS_ENCRYPTION_KEY_STATUS)ch;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int ser_loadModemConfiguration(
    gxSerializerSettings* serializeSettings,
    gxModemConfiguration* object,
    gxByteBuffer* serializer)
{
    unsigned char ch;
    int pos, ret = 0;
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    uint16_t count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->communicationSpeed = (DLMS_BAUD_RATE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = ser_verifyArray(serializer, &object->initialisationStrings, &count)) == 0)
        {
#ifdef DLMS_IGNORE_MALLOC
            object->initialisationStrings.size = count;
#endif //DLMS_IGNORE_MALLOC
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->initialisationStrings, pos, (void**)&mi, sizeof(gxModemInitialisation))) != 0)
                {
                    break;
                }
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = ser_loadOctetString3(serializer, mi->request.value, &mi->request.size)) != 0 ||
                    (ret = ser_loadOctetString3(serializer, mi->response.value, &mi->response.size)) != 0 ||
                    (ret = bb_getUInt16(serializer, &mi->delay)) != 0)
                {
                    break;
                }
#else
                if ((ret = ser_loadOctetString(serializer, &mi->request)) != 0 ||
                    (ret = ser_loadOctetString(serializer, &mi->response)) != 0 ||
                    (ret = bb_getUInt16(serializer, &mi->delay)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
#ifdef DLMS_IGNORE_MALLOC
            object->modemProfile.size = count;
#endif //DLMS_IGNORE_MALLOC
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->modemProfile, pos, (void**)&it, sizeof(gxModemProfile))) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString3(serializer, it->value, &it->size)) != 0)
                {
                    break;
                }
#else
                it = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                bb_init(it);
                if ((ret = arr_push(&object->modemProfile, it)) != 0)
                {
                    break;
                }
                if ((ret = ser_loadOctetString(serializer, it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int ser_loadMacAddressSetup(
    gxSerializerSettings* serializeSettings,
    gxMacAddressSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = ser_loadOctetString(serializer, &object->macAddress)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
int ser_loadQualityOfService(
    gxQualityOfService* object,
    gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, &object->precedence)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->delay)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->reliability)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->peakThroughput)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->meanThroughput)) != 0)
    {

    }
    return ret;
}

int ser_loadGPRSSetup(
    gxSerializerSettings* serializeSettings,
    gxGPRSSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_loadOctetString(serializer, &object->apn)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getUInt16(serializer, &object->pinCode)) != 0))
    {

    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        if ((ret = ser_loadQualityOfService(&object->defaultQualityOfService, serializer)) != 0 ||
            (ret = ser_loadQualityOfService(&object->requestedQualityOfService, serializer)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int ser_loadExtendedRegister(
    gxSerializerSettings* serializeSettings,
    gxExtendedRegister* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = getVariantFromBytes(&object->value, serializer);
    }
    if (!IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getInt8(serializer, &object->scaler)) != 0 ||
            (ret = bb_getUInt8(serializer, &object->unit)) != 0)
        {
        }
    }
    if ((!IS_ATTRIBUTE_SET(ignored, 4) && (ret = getVariantFromBytes(&object->status, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_loadDateTime(&object->captureTime, serializer)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

int ser_loadApplicationContextName(
    gxByteBuffer* serializer,
    gxApplicationContextName* object)
{
    int ret;
    if ((ret = bb_get(serializer, object->logicalName, 6)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->jointIsoCtt)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->country)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->countryName)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->identifiedOrganization)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->dlmsUA)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->applicationContext)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->contextId)) != 0)
    {

    }
    return ret;
}

int ser_loadxDLMSContextType(gxByteBuffer* serializer, gxXDLMSContextType* object)
{
    int ret;
    if ((ret = bb_getUInt32(serializer, (uint32_t*)&object->conformance)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->maxReceivePduSize)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->maxSendPduSize)) != 0 ||
        (ret = bb_getInt8(serializer, &object->qualityOfService)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->cypheringInfo)) != 0)
    {

    }
    return ret;
}

int ser_loadAuthenticationMechanismName(gxByteBuffer* serializer, gxAuthenticationMechanismName* object)
{
    unsigned char ch;
    int ret;
    if ((ret = bb_getUInt8(serializer, &object->jointIsoCtt)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->country)) == 0 &&
        (ret = bb_getUInt16(serializer, &object->countryName)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->identifiedOrganization)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->dlmsUA)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->authenticationMechanismName)) == 0 &&
        (ret = bb_getUInt8(serializer, &ch)) == 0)
    {
        object->mechanismId = ch;
    }
    return ret;
}

int ser_loadAssociationLogicalName(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxAssociationLogicalName* object,
    gxByteBuffer* serializer)
{
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#endif //DLMS_IGNORE_MALLOC
    unsigned char ch;
    int pos, ret = 0;
    uint16_t count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getInt8(serializer, &object->clientSAP)) != 0 ||
            (ret = bb_getUInt16(serializer, &object->serverSAP)) != 0)
        {
        }
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 4) && (ret = ser_loadApplicationContextName(serializer, &object->applicationContextName)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_loadxDLMSContextType(serializer, &object->xDLMSContextInfo)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = ser_loadAuthenticationMechanismName(serializer, &object->authenticationMechanismName)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = ser_loadOctetString(serializer, &object->secret)) != 0))
        {

        }
        if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->associationStatus = ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 9))
    {
        //Security Setup Reference is from version 1.
        if (object->base.version > 0)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            unsigned char ln[6];
            if ((ret = bb_get(serializer, ln, 6)) != 0 ||
                (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_SECURITY_SETUP, ln, (gxObject**)&object->securitySetup)) != 0)
            {
            }
#else
            if ((ret = bb_get(&ba, object->securitySetupReference, 6)) != 0)
            {
            }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 10))
    {
        obj_clearUserList(&object->userList);
        if (object->base.version > 1)
        {
            if ((ret = ser_verifyArray(serializer, &object->userList, &count)) == 0)
            {
                for (pos = 0; pos != count; ++pos)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = ser_getArrayItem(&object->userList, pos, (void**)&it, sizeof(gxUser))) != 0 ||
                        (ret = bb_getUInt8(serializer, &it->id)) != 0 ||
                        (ret = ser_loadOctetString2(serializer, (char**)&it->name)) != 0)
                    {
                        break;
                    }
#else
                    uint8_t id;
                    gxByteBuffer* value = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
                    if (value == NULL)
                    {
                        return DLMS_ERROR_CODE_OUTOFMEMORY;
                    }
                    if ((ret = bb_getUInt8(serializer, &id)) != 0 ||
                        (ret = ser_loadOctetString(serializer, value)) != 0)
                    {
                        break;
                    }
                    if ((ret = arr_push(&object->userList, key_init2(id, value))) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                }
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int ser_loadAssociationShortName(
    gxSerializerSettings* serializeSettings,
    gxAssociationShortName* object,
    gxByteBuffer* serializer)
{
    return 0;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int ser_loadPppSetup(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxPppSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = bb_get(serializer, ln, 6)) != 0 ||
            (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->phy)) != 0)
        {
            return ret;
        }
#else
        if ((ret = bb_get(serializer, object->PHYReference, 6)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        if ((ret = ser_loadOctetString(serializer, &object->userName)) != 0 ||
            (ret = ser_loadOctetString(serializer, &object->password)) != 0 ||
            (ret = bb_getUInt8(serializer, (unsigned char*)&object->authentication)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int ser_loadProfileGeneric(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxProfileGeneric* object,
    gxByteBuffer* serializer)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    unsigned char ln[6];
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_loadObjects2(settings, serializer, &object->captureObjects)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getUInt32(serializer, &object->capturePeriod)) != 0))
    {
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt8(serializer, &ch)) == 0)
    {
        object->sortMethod = ch;
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_get(serializer, ln, 6)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = cosem_findObjectByLN(settings, DLMS_OBJECT_TYPE_NONE, ln, &object->sortObject)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getUInt32(serializer, &object->entriesInUse)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 9) && (ret = bb_getUInt32(serializer, &object->profileEntries)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int ser_loadAccount(
    gxSerializerSettings* serializeSettings,
    gxAccount* object,
    gxByteBuffer* serializer)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->paymentMode = ch;
            if ((ret = bb_getUInt8(serializer, &ch)) == 0)
            {
                object->accountStatus = ch;
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        ret = bb_getUInt8(serializer, &object->currentCreditInUse);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt8(serializer, &ch)) == 0)
    {
        object->currentCreditStatus = ch;
    }
    if ((!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getInt32(serializer, &object->availableCredit)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getInt32(serializer, &object->amountToClear)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 7) && (ret = bb_getInt32(serializer, &object->clearanceThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 8) && (ret = bb_getInt32(serializer, &object->aggregatedDebt)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 13) && (ret = ser_loadDateTime(&object->accountActivationTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 14) && (ret = ser_loadDateTime(&object->accountClosureTime, serializer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 16) && (ret = bb_getInt32(serializer, &object->lowCreditThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 17) && (ret = bb_getInt32(serializer, &object->nextCreditAvailableThreshold)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 18) && (ret = bb_getUInt16(serializer, &object->maxProvision)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 19) && (ret = bb_getInt32(serializer, &object->maxProvisionPeriod)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int ser_loadCredit(
    gxSerializerSettings* serializeSettings,
    gxCredit* object,
    gxByteBuffer* serializer)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = bb_getInt32(serializer, &object->currentCreditAmount);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->type = (DLMS_CREDIT_TYPE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        ret = bb_getUInt8(serializer, &object->priority);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        ret = bb_getInt32(serializer, &object->warningThreshold);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 6))
    {
        ret = bb_getInt32(serializer, &object->limit);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->creditConfiguration = (DLMS_CREDIT_CONFIGURATION)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8))
    {
        ret = bb_getUInt8(serializer, &object->status);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 9))
    {
        ret = bb_getInt32(serializer, &object->presetCreditAmount);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 10))
    {
        ret = bb_getInt32(serializer, &object->creditAvailableThreshold);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 11))
    {
        ret = ser_loadDateTime(&object->period, serializer);
    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE

int ser_loadUnitCharge(
    gxByteBuffer* serializer,
    dlmsSettings* settings,
    gxUnitCharge* target)
{
    gxChargeTable* ct;
    int ret, pos;
    if ((ret = obj_clearChargeTables(&target->chargeTables)) == 0)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t type;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t count;
        if (//charge per unit scaling
            (ret = cosem_getInt8(serializer, &target->chargePerUnitScaling.commodityScale)) != 0 ||
            (ret = cosem_getInt8(serializer, &target->chargePerUnitScaling.priceScale)) != 0 ||
            //commodity
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getUInt16(serializer, &type)) != 0 ||
            (ret = cosem_getOctetString2(serializer, ln, 6, NULL)) != 0 ||
#else
            (ret = cosem_getInt16(serializer, (short*)&target->commodity.type)) != 0 ||
            (ret = cosem_getOctetString2(serializer, target->commodity.logicalName, 6, NULL)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            (ret = cosem_getInt8(serializer, &target->commodity.attributeIndex)) != 0 ||
            (ret = ser_verifyArray(serializer, &target->chargeTables, &count)) != 0)
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
            if ((ret = ser_getArrayItem(&target->chargeTables, pos, (void**)&ct, sizeof(gxChargeTable))) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
            (ret = cosem_getOctetString2(serializer, ct->index.data, sizeof(ct->index.data), &ct->index.size)) != 0 ||
#else
                (ret = cosem_getOctetString(serializer, &ct->index)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                (ret = cosem_getInt16(serializer, &ct->chargePerUnit)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int ser_loadCharge(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxCharge* object,
    gxByteBuffer* serializer)
{
    unsigned char ch;
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
        ret = bb_getInt32(serializer, &object->totalAmountPaid);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 3))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->chargeType = (DLMS_CHARGE_TYPE)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 4))
    {
        ret = bb_getUInt8(serializer, &object->priority);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 5))
    {
        ret = ser_loadUnitCharge(serializer, settings, &object->unitChargeActive);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 6))
    {
        ret = ser_loadUnitCharge(serializer, settings, &object->unitChargePassive);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        ret = ser_loadDateTime(&object->unitChargeActivationTime, serializer);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8))
    {
        ret = bb_getUInt32(serializer, &object->period);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 9))
    {
        if ((ret = bb_getUInt8(serializer, &ch)) == 0)
        {
            object->chargeConfiguration = (DLMS_CHARGE_CONFIGURATION)ch;
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 10))
    {
        ret = ser_loadDateTime(&object->lastCollectionTime, serializer);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 11))
    {
        ret = bb_getInt32(serializer, &object->lastCollectionAmount);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 12))
    {
        ret = bb_getInt32(serializer, &object->totalAmountRemaining);
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 13))
    {
        ret = bb_getUInt16(serializer, &object->proportion);
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int ser_loadTokenGateway(
    gxSerializerSettings* serializeSettings,
    gxTokenGateway* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int ser_loadGsmDiagnostic(
    gxSerializerSettings* serializeSettings,
    gxGsmDiagnostic* object,
    gxByteBuffer* serializer)
{
    int ret = 0, pos;
    gxAdjacentCell* it;
    uint16_t count;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if (!IS_ATTRIBUTE_SET(ignored, 2))
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = ser_loadOctetString(serializer, &object->operatorName);
#else
        ret = ser_loadOctetString2(serializer, &object->operatorName);
#endif //DLMS_IGNORE_MALLOC
    }
    if (ret == 0)
    {
        if ((!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->status)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->circuitSwitchStatus)) != 0) ||
            (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->packetSwitchStatus)) != 0))
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 6))
    {
        if ((ret = bb_getUInt32(serializer, &object->cellInfo.cellId)) == 0 &&
            (ret = bb_getUInt16(serializer, &object->cellInfo.locationId)) == 0 &&
            (ret = bb_getUInt8(serializer, &object->cellInfo.signalQuality)) == 0 &&
            (ret = bb_getUInt8(serializer, &object->cellInfo.ber)) == 0)
        {
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 7))
    {
        if ((ret = ser_verifyArray(serializer, &object->adjacentCells, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = ser_getArrayItem(&object->adjacentCells, pos, (void**)&it, sizeof(gxAdjacentCell))) != 0 ||
                    (ret = bb_getUInt32(serializer, &it->cellId)) != 0 ||
                    (ret = bb_getUInt8(serializer, &it->signalQuality)) != 0)
                {
                    break;
                }
            }
        }
    }
    if (ret == 0 && !IS_ATTRIBUTE_SET(ignored, 8))
    {
        ret = ser_loadDateTime(&object->captureTime, serializer);
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
int ser_loadCompactData(
    gxSerializerSettings* serializeSettings,
    dlmsSettings* settings,
    gxCompactData* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = ser_loadOctetString(serializer, &object->buffer)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = ser_loadObjects2(settings, serializer, &object->captureObjects)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 4) && (ret = bb_getUInt8(serializer, &object->templateId)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 5) && (ret = ser_loadOctetString(serializer, &object->templateDescription)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 6) && (ret = bb_getUInt8(serializer, (unsigned char*)&object->captureMethod)) != 0))
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int ser_loadLlcSscsSetup(
    gxSerializerSettings* serializeSettings,
    gxLlcSscsSetup* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    uint16_t ignored = ser_getIgnoredAttributes(serializeSettings, (gxObject*)object);
    if ((!IS_ATTRIBUTE_SET(ignored, 2) && (ret = bb_getUInt16(serializer, &object->serviceNodeAddress)) != 0) ||
        (!IS_ATTRIBUTE_SET(ignored, 3) && (ret = bb_getUInt16(serializer, &object->baseNodeAddress)) != 0))
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int ser_loadPrimeNbOfdmPlcPhysicalLayerCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcPhysicalLayerCounters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int ser_loadPrimeNbOfdmPlcMacSetup(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacSetup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int ser_loadPrimeNbOfdmPlcMacFunctionalParameters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacFunctionalParameters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int ser_loadPrimeNbOfdmPlcMacCounters(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacCounters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int  ser_loadPrimeNbOfdmPlcMacNetworkAdministrationData(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int  ser_loadPrimeNbOfdmPlcApplicationsIdentification(
    gxSerializerSettings* serializeSettings,
    gxPrimeNbOfdmPlcApplicationsIdentification* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION

#ifndef DLMS_IGNORE_ARBITRATOR
int ser_loadArbitrator(
    gxSerializerSettings* serializeSettings,
    gxArbitrator* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int ser_loadIec8802LlcType1Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType1Setup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int ser_loadIec8802LlcType2Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType2Setup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int ser_loadIec8802LlcType3Setup(
    gxSerializerSettings* serializeSettings,
    gxIec8802LlcType3Setup* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int ser_loadSFSKActiveInitiator(
    gxSerializerSettings* serializeSettings,
    gxSFSKActiveInitiator* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int ser_loadFSKMacCounters(
    gxSerializerSettings* serializeSettings,
    gxFSKMacCounters* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int ser_loadSFSKMacSynchronizationTimeouts(
    gxSerializerSettings* serializeSettings,
    gxSFSKMacSynchronizationTimeouts* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int ser_loadSFSKPhyMacSetUp(
    gxSerializerSettings* serializeSettings,
    gxSFSKPhyMacSetUp* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int ser_loadSFSKReportingSystemList(
    gxSerializerSettings* serializeSettings,
    gxSFSKReportingSystemList* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifndef DLMS_IGNORE_SCHEDULE
int ser_loadSchedule(
    gxSerializerSettings* serializeSettings,
    gxSchedule* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SCHEDULE

#ifdef DLMS_ITALIAN_STANDARD
int ser_loadTariffPlan(
    gxSerializerSettings* serializeSettings,
    gxTariffPlan* object,
    gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_addString(serializer, object->calendarName)) != 0 ||
        (ret = bb_getUInt8(serializer, object->enabled)) != 0 ||
        (ret = ser_loadDateTime(&object->activationTime, serializer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_ITALIAN_STANDARD

int ser_loadObject(
    dlmsSettings* settings,
    gxSerializerSettings* serializeSettings,
    gxObject* object,
    gxByteBuffer* serializer)
{
    int ret = 0;
    switch (object->objectType)
    {
#ifndef DLMS_IGNORE_DATA

    case DLMS_OBJECT_TYPE_DATA:
        ret = ser_loadData(serializeSettings, (gxData*)object, serializer);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = ser_loadRegister(serializeSettings, (gxRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = ser_loadClock(serializeSettings, (gxClock*)object, serializer);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = ser_loadActionSchedule(serializeSettings, settings, (gxActionSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = ser_loadActivityCalendar(serializeSettings, settings, (gxActivityCalendar*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = ser_loadAssociationLogicalName(serializeSettings, settings, (gxAssociationLogicalName*)object, serializer);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = ser_loadAssociationShortName(serializeSettings, (gxAssociationShortName*)object, serializer);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = ser_loadAutoAnswer(serializeSettings, (gxAutoAnswer*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = ser_loadAutoConnect(serializeSettings, (gxAutoConnect*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = ser_loadDemandRegister(serializeSettings, (gxDemandRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = ser_loadMacAddressSetup(serializeSettings, (gxMacAddressSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = ser_loadExtendedRegister(serializeSettings, (gxExtendedRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
#ifndef DLMS_IGNORE_GPRS_SETUP
        ret = ser_loadGPRSSetup(serializeSettings, (gxGPRSSetup*)object, serializer);
#endif //DLMS_IGNORE_GPRS_SETUP
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = ser_loadSecuritySetup(serializeSettings, (gxSecuritySetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = ser_loadHdlcSetup(serializeSettings, (gxIecHdlcSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = ser_loadLocalPortSetup(serializeSettings, (gxLocalPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = ser_loadIecTwistedPairSetup(serializeSettings, (gxIecTwistedPairSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = ser_loadIp4Setup(serializeSettings, settings, (gxIp4Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = ser_loadIp6Setup(serializeSettings, settings, (gxIp6Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = ser_loadMbusSlavePortSetup(serializeSettings, (gxMbusSlavePortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = ser_loadImageTransfer(serializeSettings, (gxImageTransfer*)object, serializer);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = ser_loadDisconnectControl(serializeSettings, (gxDisconnectControl*)object, serializer);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = ser_loadLimiter(serializeSettings, settings, (gxLimiter*)object, serializer);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = ser_loadMBusClient(serializeSettings, settings, (gxMBusClient*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = ser_loadModemConfiguration(serializeSettings, (gxModemConfiguration*)object, serializer);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = ser_loadPppSetup(serializeSettings, settings, (gxPppSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = ser_loadProfileGeneric(serializeSettings, settings, (gxProfileGeneric*)object, serializer);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = ser_loadRegisterActivation(serializeSettings, settings, (gxRegisterActivation*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = ser_loadRegisterMonitor(serializeSettings, settings, (gxRegisterMonitor*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_REGISTER_TABLE
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_REGISTER_TABLE
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = ser_loadSapAssignment(serializeSettings, (gxSapAssignment*)object, serializer);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = ser_loadSchedule(serializeSettings, (gxSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = ser_loadScriptTable(serializeSettings, settings, (gxScriptTable*)object, serializer);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SMTP_SETUP
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_SMTP_SETUP
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = ser_loadSpecialDaysTable(serializeSettings, (gxSpecialDaysTable*)object, serializer);
        break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_STATUS_MAPPING
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_STATUS_MAPPING
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = ser_loadTcpUdpSetup(serializeSettings, settings, (gxTcpUdpSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = ser_loadUtilityTables(serializeSettings, (gxUtilityTables*)object, serializer);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = ser_loadMBusMasterPortSetup(serializeSettings, (gxMBusMasterPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = ser_loadPushSetup(serializeSettings, settings, (gxPushSetup*)object, serializer);
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
        ret = ser_loadAccount(serializeSettings, (gxAccount*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = ser_loadCredit(serializeSettings, (gxCredit*)object, serializer);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = ser_loadCharge(serializeSettings, settings, (gxCharge*)object, serializer);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = ser_loadTokenGateway(serializeSettings, (gxTokenGateway*)object, serializer);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = ser_loadGsmDiagnostic(serializeSettings, (gxGsmDiagnostic*)object, serializer);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = ser_loadCompactData(serializeSettings, settings, (gxCompactData*)object, serializer);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = ser_loadLlcSscsSetup(serializeSettings, (gxLlcSscsSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = ser_loadPrimeNbOfdmPlcPhysicalLayerCounters(serializeSettings, (gxPrimeNbOfdmPlcPhysicalLayerCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = ser_loadPrimeNbOfdmPlcMacSetup(serializeSettings, (gxPrimeNbOfdmPlcMacSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = ser_loadPrimeNbOfdmPlcMacFunctionalParameters(serializeSettings, (gxPrimeNbOfdmPlcMacFunctionalParameters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = ser_loadPrimeNbOfdmPlcMacCounters(serializeSettings, (gxPrimeNbOfdmPlcMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = ser_loadPrimeNbOfdmPlcMacNetworkAdministrationData(serializeSettings, (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = ser_loadPrimeNbOfdmPlcApplicationsIdentification(serializeSettings, (gxPrimeNbOfdmPlcApplicationsIdentification*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        break;
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = ser_loadArbitrator(serializeSettings, (gxArbitrator*)object, serializer);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = ser_loadIec8802LlcType1Setup(serializeSettings, (gxIec8802LlcType1Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ser_loadIec8802LlcType2Setup(serializeSettings, (gxIec8802LlcType2Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ser_loadIec8802LlcType3Setup(serializeSettings, (gxIec8802LlcType3Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ser_loadSFSKActiveInitiator(serializeSettings, (gxSFSKActiveInitiator*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ser_loadFSKMacCounters(serializeSettings, (gxFSKMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ser_loadSFSKMacSynchronizationTimeouts(serializeSettings, (gxSFSKMacSynchronizationTimeouts*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ser_loadSFSKPhyMacSetUp(serializeSettings, (gxSFSKPhyMacSetUp*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ser_loadSFSKReportingSystemList(serializeSettings, (gxSFSKReportingSystemList*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = ser_loadTariffPlan(serializeSettings, (gxTariffPlan*)object, serializer);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default: //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

//Serialize objects to bytebuffer.
int ser_loadObjects(
    dlmsSettings* settings,
    gxSerializerSettings* serializeSettings,
    gxObject** object,
    uint16_t count,
    gxByteBuffer* serializer)
{
    uint16_t pos;
    int ret = 0;
    //Serializer version number.
    unsigned char version;
    if ((ret = bb_getUInt8(serializer, &version)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
            //If all data is read.
            if (bb_available(serializer) == 0)
            {
                break;
            }
            if ((ret = ser_loadObject(settings, serializeSettings, object[pos], serializer)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}
