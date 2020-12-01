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

int ser_saveDateTime(gxtime* value, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = var_getDateTime2(value, serializer)) != 0)
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
int ser_saveData(gxData* object, gxByteBuffer* serializer)
{
    return var_getBytes(&object->value, serializer);
}

#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int ser_saveRegister(gxRegister* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setInt8(serializer, object->scaler)) != 0 ||
        (ret = bb_setUInt8(serializer, object->unit)) != 0 ||
        (ret = var_getBytes(&object->value, serializer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
int ser_saveClock(gxClock* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_saveDateTime(&object->time, serializer)) != 0 ||
        (ret = bb_setInt16(serializer, object->timeZone)) != 0 ||
        (ret = bb_setUInt8(serializer, object->status)) != 0 ||
        (ret = ser_saveDateTime(&object->begin, serializer)) != 0 ||
        (ret = ser_saveDateTime(&object->end, serializer)) != 0 ||
        (ret = bb_setInt8(serializer, object->deviation)) != 0 ||
        (ret = bb_setUInt8(serializer, object->enabled)) != 0 ||
        (ret = bb_setUInt8(serializer, object->clockBase)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int ser_saveScriptTable(gxScriptTable* object, gxByteBuffer* serializer)
{
    int ret, pos, pos2;
    gxScript* s;
    gxScriptAction* sa;
    if ((ret = hlp_setObjectCount(object->scripts.size, serializer)) == 0)
    {
        for (pos = 0; pos != object->scripts.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&s, sizeof(gxScriptAction))) != 0)
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
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int ser_saveSpecialDaysTable(gxSpecialDaysTable* object, gxByteBuffer* serializer)
{
    int ret, pos;
    gxSpecialDay* sd;
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
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int ser_saveTcpUdpSetup(gxTcpUdpSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt16(serializer, object->port)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    (ret = bb_set(serializer, obj_getLogicalName(object->ipSetup), 6)) != 0 ||
#else
        (ret = bb_set(&ba, object->ipReference, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_setUInt16(serializer, object->maximumSegmentSize)) != 0 ||
        (ret = bb_setUInt8(serializer, object->maximumSimultaneousConnections)) != 0 ||
        (ret = bb_setUInt16(serializer, object->inactivityTimeout)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int ser_saveMBusMasterPortSetup(gxMBusMasterPortSetup* object, gxByteBuffer* serializer)
{
    return bb_setUInt8(serializer, object->commSpeed);
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

#ifndef DLMS_IGNORE_MESSAGE_HANDLER
int ser_saveMessageHandler(
    gxMessageHandler* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_MESSAGE_HANDLER

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
#ifdef DLMS_IGNORE_MALLOC
    gxTarget* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = hlp_setObjectCount(objects->size, serializer)) == 0)
    {
        for (pos = 0; pos != objects->size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(objects, pos, (void**)&it, sizeof(gxTarget))) != 0 ||
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
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP
int ser_savePushSetup(gxPushSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_saveObjects2(serializer, &object->pushObjectList)) != 0 ||
        (ret = bb_setUInt8(serializer, object->service)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
        (ret = ser_saveOctetString3(serializer, (char*)object->destination.value, object->destination.size)) != 0 ||
#else
        (ret = ser_saveOctetString2(serializer, object->destination)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
        (ret = bb_setUInt8(serializer, object->message)) != 0 ||
        (ret = saveTimeWindow(serializer, &object->communicationWindow)) != 0 ||
        (ret = bb_setUInt16(serializer, object->randomisationStartInterval)) != 0 ||
        (ret = bb_setUInt8(serializer, object->numberOfRetries)) != 0 ||
        (ret = bb_setUInt16(serializer, object->repetitionDelay)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_AUTO_CONNECT
int ser_saveAutoConnect(gxAutoConnect* object, gxByteBuffer* serializer)
{
#ifdef DLMS_IGNORE_MALLOC
    gxDestination* dest;
#else
    gxByteBuffer* dest;
#endif //DLMS_IGNORE_MALLOC
    int pos, ret;
    if ((ret = bb_setUInt8(serializer, object->mode)) == 0 &&
        (ret = bb_setUInt8(serializer, object->repetitions)) == 0 &&
        (ret = bb_setUInt16(serializer, object->repetitionDelay)) == 0 &&
        (ret = saveTimeWindow(serializer, &object->callingWindow)) == 0)
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
                (ret = hlp_setObjectCount(dp->daySchedules.size, serializer)) == 0)
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

int ser_saveActivityCalendar(gxActivityCalendar* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_saveOctetString(serializer, &object->calendarNameActive)) != 0 ||
        (ret = ser_saveSeasonProfile(&object->seasonProfileActive, serializer)) != 0 ||
        (ret = ser_saveweekProfile(&object->weekProfileTableActive, serializer)) != 0 ||
        (ret = ser_saveDayProfile(&object->dayProfileTableActive, serializer)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->calendarNameActive)) != 0 ||
        (ret = ser_saveSeasonProfile(&object->seasonProfilePassive, serializer)) != 0 ||
        (ret = ser_saveweekProfile(&object->weekProfileTablePassive, serializer)) != 0 ||
        (ret = ser_saveDayProfile(&object->dayProfileTablePassive, serializer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_SECURITY_SETUP
int ser_saveSecuritySetup(gxSecuritySetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->securityPolicy)) != 0 ||
        (ret = bb_setUInt8(serializer, object->securitySuite)) != 0 ||
        (ret = bb_set(serializer, object->serverSystemTitle.data, object->serverSystemTitle.size)) != 0 ||
        (ret = bb_set(serializer, object->clientSystemTitle.data, object->clientSystemTitle.size)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int ser_saveHdlcSetup(gxIecHdlcSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->communicationSpeed)) != 0 ||
        (ret = bb_setUInt8(serializer, object->windowSizeTransmit)) != 0 ||
        (ret = bb_setUInt8(serializer, object->windowSizeReceive)) != 0 ||
        (ret = bb_setUInt16(serializer, object->maximumInfoLengthTransmit)) != 0 ||
        (ret = bb_setUInt16(serializer, object->maximumInfoLengthReceive)) != 0 ||
        (ret = bb_setUInt16(serializer, object->interCharachterTimeout)) != 0 ||
        (ret = bb_setUInt16(serializer, object->inactivityTimeout)) != 0 ||
        (ret = bb_setUInt16(serializer, object->deviceAddress)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int ser_saveLocalPortSetup(gxLocalPortSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->defaultMode)) != 0 ||
        (ret = bb_setUInt8(serializer, object->defaultBaudrate)) != 0 ||
        (ret = bb_setUInt8(serializer, object->proposedBaudrate)) != 0 ||
        (ret = bb_setUInt8(serializer, object->responseTime)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->deviceAddress)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->password1)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->password2)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->password5)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int ser_IecTwistedPairSetup(gxIecTwistedPairSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->mode)) == 0 &&
        (ret = bb_setUInt8(serializer, object->speed)) == 0 &&
        (ret = ser_saveOctetString(serializer, &object->primaryAddresses)) == 0 &&
        (ret = ser_saveOctetString(serializer, &object->tabis)) == 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int ser_saveDemandRegister(gxDemandRegister* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = var_getBytes(&object->currentAverageValue, serializer)) != 0 ||
        (ret = var_getBytes(&object->lastAverageValue, serializer)) != 0 ||
        (ret = bb_setInt8(serializer, object->scaler)) != 0 ||
        (ret = bb_setUInt8(serializer, object->unit)) != 0 ||
        (ret = var_getBytes(&object->status, serializer)) != 0 ||
        (ret = ser_saveDateTime(&object->captureTime, serializer)) != 0 ||
        (ret = ser_saveDateTime(&object->startTimeCurrent, serializer)) != 0 ||
        (ret = bb_setUInt32(serializer, object->period)) != 0 ||
        (ret = bb_setUInt16(serializer, object->numberOfPeriods)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int ser_saveRegisterActivation(gxRegisterActivation* object, gxByteBuffer* serializer)
{
    int pos, ret;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifdef DLMS_IGNORE_MALLOC
    gxRegisterActivationMask* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = hlp_setObjectCount(object->registerAssignment.size, serializer)) == 0)
    {
        for (pos = 0; pos != object->registerAssignment.size; ++pos)
        {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od, sizeof(gxObject))) != 0 ||
#else
            if ((ret = oa_getByIndex(&object->registerAssignment, pos, &od)) != 0 ||
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
        if (ret == 0 && (ret = hlp_setObjectCount(object->maskList.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->maskList.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask))) != 0 ||
                    (ret = ser_saveOctetString3(serializer, (char*)it->name, it->length)) != 0 ||
                    (ret = ser_saveOctetString3(serializer, (char*)it->indexes, it->count)) != 0)
                {
                    break;
                }
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
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int ser_saveActionItem(gxActionItem* item, gxByteBuffer* serializer)
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

int ser_saveRegisterMonitor(gxRegisterMonitor* object, gxByteBuffer* serializer)
{
    int pos, ret;
    dlmsVARIANT* tmp;
    gxActionSet* as;
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
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0 &&
            ((ret = bb_setUInt16(serializer, object->monitoredValue.target == NULL ? 0 : object->monitoredValue.target->objectType)) != 0 ||
                (ret = bb_set(serializer, obj_getLogicalName(object->monitoredValue.target), 6)) != 0))
        {

        }
#else
        if (ret == 0 &&
            (ret = bb_setUInt16(serializer, object->monitoredValue.target == NULL ? 0 : object->monitoredValue.objectType)) != 0 ||
            (ret = bb_set(serializer, obj_getLogicalName(object->monitoredValue.logicalName), 6)) != 0)
        {

        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0 && (ret = bb_setInt8(serializer, object->monitoredValue.attributeIndex)) == 0)
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
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int ser_saveActionSchedule(gxActionSchedule* object, gxByteBuffer* serializer)
{
    int pos = 0, ret;
    gxtime* tm;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_set(serializer, obj_getLogicalName((gxObject*)object->executedScript), 6)) == 0 &&

#else
    if ((ret = bb_set(serializer, object->executedScriptLogicalName, 6)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_setUInt16(serializer, object->executedScriptSelector)) == 0 &&
        (ret = bb_setUInt8(serializer, object->type)) == 0 &&
        (ret = hlp_setObjectCount(object->executionTime.size, serializer)) == 0)
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
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int ser_saveSapAssignment(gxSapAssignment* object, gxByteBuffer* serializer)
{
    int ret = 0, pos = 0;
    gxSapItem* it;
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
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_AUTO_ANSWER
int ser_saveAutoAnswer(gxAutoAnswer* object, gxByteBuffer* serializer)
{
    int ret = 0;
    if ((ret = bb_setUInt8(serializer, object->mode)) == 0 &&
        (ret = saveTimeWindow(serializer, &object->listeningWindow)) == 0)
    {
        if ((ret = bb_setUInt8(serializer, object->status)) == 0 &&
            (ret = bb_setUInt8(serializer, object->numberOfCalls)) == 0 &&
            (ret = bb_setUInt8(serializer, object->numberOfRingsInListeningWindow)) == 0 &&
            (ret = bb_setUInt8(serializer, object->numberOfRingsOutListeningWindow)) == 0 &&
            (ret = bb_setUInt8(serializer, object->numberOfRingsOutListeningWindow)) == 0)
        {

        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int ser_saveIp4Setup(gxIp4Setup* object, gxByteBuffer* serializer)
{
    int ret, pos;
#ifdef DLMS_IGNORE_MALLOC
    uint32_t* tmp;
#else
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* ip;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_set(serializer, obj_getLogicalName(object->dataLinkLayer), 6)) == 0 &&

#else
    if ((ret = bb_set(serializer, object->dataLinkLayerReference, 6)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_setUInt32(serializer, object->ipAddress)) == 0 &&
        (ret = hlp_setObjectCount(object->multicastIPAddress.size, serializer)) == 0)
    {
        for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->multicastIPAddress, pos, (void**)&tmp, sizeof(uint32_t))) != 0 ||
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
    if (ret == 0 && (ret = hlp_setObjectCount(object->ipOptions.size, serializer)) == 0)
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
        if (ret == 0 &&
            ((ret = bb_setUInt32(serializer, object->subnetMask)) != 0 ||
                (ret = bb_setUInt32(serializer, object->gatewayIPAddress)) != 0 ||
                (ret = bb_setUInt8(serializer, object->useDHCP)) != 0 ||
                (ret = bb_setUInt32(serializer, object->primaryDNSAddress)) != 0 ||
                (ret = bb_setUInt32(serializer, object->secondaryDNSAddress)) != 0))
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
    return ser_saveOctetString3(serializer, (char*)tmp, 16);
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

int ser_saveIp6Setup(gxIp6Setup* object, gxByteBuffer* serializer)
{
    int ret;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_set(serializer, obj_getLogicalName(object->dataLinkLayer), 6)) == 0 &&
#else
    if ((ret = bb_set(serializer, object->dataLinkLayerReference, 6)) == 0 &&
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_setUInt8(serializer, object->addressConfigMode)) == 0 &&
        (ret = saveAddressList(&object->unicastIPAddress, serializer)) == 0 &&
        (ret = saveAddressList(&object->multicastIPAddress, serializer)) == 0 &&
        (ret = saveAddressList(&object->gatewayIPAddress, serializer)) == 0 &&
        (ret = saveIpv6Address(&object->primaryDNSAddress, serializer)) == 0 &&
        (ret = saveIpv6Address(&object->secondaryDNSAddress, serializer)) == 0 &&
        (ret = bb_setUInt8(serializer, object->trafficClass)) == 0)
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
int ser_saveUtilityTables(gxUtilityTables* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt16(serializer, object->tableId)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->buffer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int ser_saveMbusSlavePortSetup(gxMbusSlavePortSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->defaultBaud)) != 0 ||
        (ret = bb_setUInt8(serializer, object->availableBaud)) != 0 ||
        (ret = bb_setUInt8(serializer, object->addressState)) != 0 ||
        (ret = bb_setUInt8(serializer, object->busAddress)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int ser_saveImageTransfer(gxImageTransfer* object, gxByteBuffer* serializer)
{
    int pos, ret;
    gxImageActivateInfo* it;
    if ((ret = bb_setUInt32(serializer, object->imageBlockSize)) == 0 &&
        (ret = ser_saveBitString(serializer, &object->imageTransferredBlocksStatus)) == 0 &&
        (ret = bb_setUInt32(serializer, object->imageFirstNotTransferredBlockNumber)) == 0 &&
        (ret = bb_setUInt8(serializer, object->imageTransferEnabled)) == 0 &&
        (ret = bb_setUInt8(serializer, object->imageTransferStatus)) == 0)
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
int ser_saveDisconnectControl(gxDisconnectControl* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->outputState)) != 0 ||
        (ret = bb_setUInt8(serializer, object->controlState)) != 0 ||
        (ret = bb_setUInt8(serializer, object->controlMode)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int ser_saveLimiter(gxLimiter* object, gxByteBuffer* serializer)
{
    int pos, ret;
    dlmsVARIANT* it;
    if ((ret = bb_set(serializer, obj_getLogicalName(object->monitoredValue), 6)) == 0 &&
        (ret = bb_setInt8(serializer, object->selectedAttributeIndex)) == 0 &&
        (ret = var_getBytes(&object->thresholdActive, serializer)) == 0 &&
        (ret = var_getBytes(&object->thresholdNormal, serializer)) == 0 &&
        (ret = var_getBytes(&object->thresholdEmergency, serializer)) == 0 &&
        (ret = bb_setInt32(serializer, object->minOverThresholdDuration)) == 0 &&
        (ret = bb_setInt32(serializer, object->minUnderThresholdDuration)) == 0 &&
        (ret = bb_setUInt16(serializer, object->emergencyProfile.id)) == 0 &&
        (ret = ser_saveDateTime(&object->emergencyProfile.activationTime, serializer)) == 0 &&
        (ret = bb_setUInt32(serializer, object->emergencyProfile.duration)) == 0)
    {
        if ((ret = hlp_setObjectCount(object->emergencyProfileGroupIDs.size, serializer)) == 0)
        {
            for (pos = 0; pos != object->emergencyProfileGroupIDs.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(dlmsVARIANT))) != 0 ||
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
        if (ret == 0 && ((ret = bb_setUInt8(serializer, object->emergencyProfileActive)) != 0 ||
            (ret = ser_saveActionItem(&object->actionOverThreshold, serializer)) != 0 ||
            (ret = ser_saveActionItem(&object->actionUnderThreshold, serializer)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int ser_saveMBusClient(gxMBusClient* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt32(serializer, object->capturePeriod)) != 0 ||
        (ret = bb_setUInt8(serializer, object->primaryAddress)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_set(serializer, obj_getLogicalName(object->mBusPort), 6)) != 0 ||
#else
        (ret = bb_set(serializer, obj_getLogicalName(object->mBusPortReference), 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = bb_setUInt32(serializer, object->identificationNumber)) != 0 ||
        (ret = bb_setUInt16(serializer, object->manufacturerID)) != 0 ||
        (ret = bb_setUInt8(serializer, object->dataHeaderVersion)) != 0 ||
        (ret = bb_setUInt8(serializer, object->deviceType)) != 0 ||
        (ret = bb_setUInt8(serializer, object->accessNumber)) != 0 ||
        (ret = bb_setUInt8(serializer, object->status)) != 0 ||
        (ret = bb_setUInt8(serializer, object->alarm)) != 0 ||
        (ret = bb_setUInt16(serializer, object->configuration)) != 0 ||
        (ret = bb_setUInt8(serializer, object->encryptionKeyStatus)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int ser_saveModemConfiguration(gxModemConfiguration* object, gxByteBuffer* serializer)
{
    int pos, ret;
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = bb_setInt8(serializer, object->communicationSpeed)) == 0 &&
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
        if (ret == 0 &&
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
    }
    return ret;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int ser_saveMacAddressSetup(gxMacAddressSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_saveOctetString(serializer, &object->macAddress)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
int ser_saveQualityOfService(gxQualityOfService* object, gxByteBuffer* serializer)
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

int ser_saveGPRSSetup(gxGPRSSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_saveOctetString(serializer, &object->apn)) != 0 ||
        (ret = bb_setInt16(serializer, object->pinCode)) != 0 ||
        (ret = ser_saveQualityOfService(&object->defaultQualityOfService, serializer)) != 0 ||
        (ret = ser_saveQualityOfService(&object->requestedQualityOfService, serializer)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int ser_saveExtendedRegister(gxExtendedRegister* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = var_getBytes(&object->value, serializer)) != 0 ||
        (ret = bb_setInt8(serializer, object->scaler)) != 0 ||
        (ret = bb_setUInt8(serializer, object->unit)) != 0 ||
        (ret = var_getBytes(&object->status, serializer)) != 0 ||
        (ret = ser_saveDateTime(&object->captureTime, serializer)) != 0)
    {
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

int ser_saveAuthenticationMechanismName(gxByteBuffer* serializer, gxAuthenticationMechanismName* object)
{
    int ret;
    if ((ret = bb_setUInt32(serializer, object->jointIsoCtt)) != 0 ||
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

int ser_saveAssociationLogicalName(gxAssociationLogicalName* object, gxByteBuffer* serializer)
{
    int pos, ret = 0;
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#else
    gxKey2* it;
#endif //DLMS_IGNORE_MALLOC
    if ((ret = bb_setInt8(serializer, object->clientSAP)) != 0 ||
        (ret = bb_setUInt16(serializer, object->serverSAP)) != 0 ||
        //This is not needed. (ret = ser_saveApplicationContextName(serializer, &object->applicationContextName)) != 0 ||
        //This is not needed. (ret = ser_savexDLMSContextType(serializer, &object->xDLMSContextInfo)) != 0 ||
        //This is not needed. (ret = ser_saveAuthenticationMechanismName(serializer, &object->authenticationMechanismName)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->secret)) != 0 ||
        (ret = bb_setUInt8(serializer, object->associationStatus)) != 0)
    {

    }
    //Security Setup Reference is from version 1.
    if (object->base.version > 0)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = bb_set(serializer, obj_getLogicalName((gxObject*)object->securitySetup), 6);
#else
        ret = bb_set(&ba, object->securitySetupReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    if (object->base.version > 1)
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
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int ser_saveAssociationShortName(gxAssociationShortName* object, gxByteBuffer* serializer)
{
    return 0;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int ser_savePppSetup(gxPppSetup* object, gxByteBuffer* serializer)
{
    int ret;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_set(serializer, obj_getLogicalName(object->phy), 6)) != 0 ||
#else
    if ((ret = bb_set(&ba, object->PHYReference, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        (ret = ser_saveOctetString(serializer, &object->userName)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->password)) != 0 ||
        (ret = bb_setUInt8(serializer, object->authentication)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int ser_saveProfileGeneric(gxProfileGeneric* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_saveObjects2(serializer, &object->captureObjects)) != 0 ||
        (ret = bb_setUInt32(serializer, object->capturePeriod)) != 0 ||
        (ret = bb_setUInt8(serializer, object->sortMethod)) != 0 ||
        (ret = bb_set(serializer, obj_getLogicalName(object->sortObject), 6)) != 0 ||
        (ret = bb_setUInt32(serializer, object->entriesInUse)) != 0 ||
        (ret = bb_setUInt32(serializer, object->profileEntries)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int ser_saveAccount(
    gxAccount* object,
    gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setUInt8(serializer, object->paymentMode)) != 0 ||
        (ret = bb_setUInt8(serializer, object->accountStatus)) != 0 ||
        (ret = bb_setUInt8(serializer, object->currentCreditInUse)) != 0 ||
        (ret = bb_setUInt8(serializer, object->currentCreditStatus)) != 0 ||
        (ret = bb_setInt32(serializer, object->availableCredit)) != 0 ||
        (ret = bb_setInt32(serializer, object->amountToClear)) != 0 ||
        (ret = bb_setInt32(serializer, object->clearanceThreshold)) != 0 ||
        (ret = bb_setInt32(serializer, object->aggregatedDebt)) != 0)
    {

    }
    ser_saveDateTime(&object->accountActivationTime, serializer);
    ser_saveDateTime(&object->accountClosureTime, serializer);
    if ((ret = bb_setInt32(serializer, object->lowCreditThreshold)) != 0 ||
        (ret = bb_setInt32(serializer, object->nextCreditAvailableThreshold)) != 0 ||
        (ret = bb_setUInt16(serializer, object->maxProvision)) != 0 ||
        (ret = bb_setInt32(serializer, object->maxProvisionPeriod)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int ser_saveCredit(gxCredit* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setInt32(serializer, object->currentCreditAmount)) != 0 ||
        (ret = bb_setUInt8(serializer, object->type)) != 0 ||
        (ret = bb_setUInt8(serializer, object->priority)) != 0 ||
        (ret = bb_setInt32(serializer, object->warningThreshold)) != 0 ||
        (ret = bb_setInt32(serializer, object->limit)) != 0 ||
        (ret = bb_setUInt8(serializer, object->creditConfiguration)) != 0 ||
        (ret = bb_setUInt8(serializer, object->status)) != 0 ||
        (ret = bb_setInt32(serializer, object->presetCreditAmount)) != 0 ||
        (ret = bb_setInt32(serializer, object->creditAvailableThreshold)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
int ser_saveCharge(gxCharge* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_setInt16(serializer, object->totalAmountPaid)) != 0 ||
        (ret = bb_setUInt8(serializer, object->chargeType)) != 0 ||
        (ret = bb_setUInt8(serializer, object->priority)) != 0 ||
        // (ret = bb_setUInt8(serializer, object->unitChargeActive)) != 0 ||
        // (ret = bb_setUInt8(serializer, object->unitChargePassive)) != 0 ||
        (ret = ser_saveDateTime(&object->unitChargeActivationTime, serializer)) != 0 ||
        (ret = bb_setUInt32(serializer, object->period)) != 0 ||
        (ret = ser_saveDateTime(&object->lastCollectionTime, serializer)) != 0 ||
        (ret = bb_setUInt32(serializer, object->lastCollectionAmount)) != 0 ||
        (ret = bb_setUInt32(serializer, object->totalAmountRemaining)) != 0 ||
        (ret = bb_setUInt16(serializer, object->proportion)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int ser_saveTokenGateway(
    gxTokenGateway* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int ser_saveGsmDiagnostic(gxGsmDiagnostic* object, gxByteBuffer* serializer)
{
    int ret, pos;
    gxAdjacentCell* it;
#ifdef DLMS_IGNORE_MALLOC
    if ((ret = ser_saveOctetString(serializer, &object->operatorName)) != 0)
    {
        return ret;
    }
#else
    if ((ret = ser_saveOctetString2(serializer, object->operatorName)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
    if ((ret = bb_setUInt8(serializer, object->status)) == 0 &&
        (ret = bb_setUInt8(serializer, object->circuitSwitchStatus)) == 0 &&
        (ret = bb_setUInt8(serializer, object->packetSwitchStatus)) == 0 &&
        (ret = bb_setUInt32(serializer, object->cellInfo.cellId)) == 0 &&
        (ret = bb_setUInt16(serializer, object->cellInfo.locationId)) == 0 &&
        (ret = bb_setUInt8(serializer, object->cellInfo.signalQuality)) == 0 &&
        (ret = bb_setUInt8(serializer, object->cellInfo.ber)) == 0)
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
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
int ser_saveCompactData(gxCompactData* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_saveOctetString(serializer, &object->buffer)) != 0 ||
        (ret = ser_saveObjects2(serializer, &object->captureObjects)) != 0 ||
        (ret = bb_setUInt8(serializer, object->templateId)) != 0 ||
        (ret = ser_saveOctetString(serializer, &object->templateDescription)) != 0 ||
        (ret = bb_setUInt8(serializer, object->captureMethod)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int ser_saveLlcSscsSetup(gxLlcSscsSetup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int ser_savePrimeNbOfdmPlcPhysicalLayerCounters(gxPrimeNbOfdmPlcPhysicalLayerCounters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int  ser_savePrimeNbOfdmPlcMacSetup(gxPrimeNbOfdmPlcMacSetup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int  ser_savePrimeNbOfdmPlcMacFunctionalParameters(gxPrimeNbOfdmPlcMacFunctionalParameters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int  ser_savePrimeNbOfdmPlcMacCounters(gxPrimeNbOfdmPlcMacCounters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int ser_savePrimeNbOfdmPlcMacNetworkAdministrationData(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int ser_savePrimeNbOfdmPlcApplicationsIdentification(gxPrimeNbOfdmPlcApplicationsIdentification* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
int ser_saveArbitrator(gxArbitrator* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int ser_saveIec8802LlcType1Setup(gxIec8802LlcType1Setup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int ser_saveIec8802LlcType2Setup(gxIec8802LlcType2Setup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int ser_saveIec8802LlcType3Setup(gxIec8802LlcType3Setup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int ser_saveSFSKActiveInitiator(gxSFSKActiveInitiator* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int ser_saveFSKMacCounters(gxFSKMacCounters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int ser_saveSFSKMacSynchronizationTimeouts(gxSFSKMacSynchronizationTimeouts* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int ser_saveSFSKPhyMacSetUp(gxSFSKPhyMacSetUp* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int ser_saveSFSKReportingSystemList(gxSFSKReportingSystemList* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifndef DLMS_IGNORE_SCHEDULE
int ser_saveSchedule(gxSchedule* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SCHEDULE

#ifdef DLMS_ITALIAN_STANDARD
int ser_saveTariffPlan(gxTariffPlan* object, gxByteBuffer* serializer)
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
        ret = ser_saveData((gxData*)object, serializer);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = ser_saveRegister((gxRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = ser_saveClock((gxClock*)object, serializer);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = ser_saveActionSchedule((gxActionSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = ser_saveActivityCalendar((gxActivityCalendar*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = ser_saveAssociationLogicalName((gxAssociationLogicalName*)object, serializer);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = ser_saveAssociationShortName((gxAssociationShortName*)object, serializer);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = ser_saveAutoAnswer((gxAutoAnswer*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = ser_saveAutoConnect((gxAutoConnect*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = ser_saveDemandRegister((gxDemandRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = ser_saveMacAddressSetup((gxMacAddressSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = ser_saveExtendedRegister((gxExtendedRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
#ifndef DLMS_IGNORE_GPRS_SETUP
        ret = ser_saveGPRSSetup((gxGPRSSetup*)object, serializer);
#endif //DLMS_IGNORE_GPRS_SETUP
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = ser_saveSecuritySetup((gxSecuritySetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = ser_saveHdlcSetup((gxIecHdlcSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = ser_saveLocalPortSetup((gxLocalPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = ser_IecTwistedPairSetup((gxIecTwistedPairSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = ser_saveIp4Setup((gxIp4Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = ser_saveIp6Setup((gxIp6Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = ser_saveMbusSlavePortSetup((gxMbusSlavePortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = ser_saveImageTransfer((gxImageTransfer*)object, serializer);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = ser_saveDisconnectControl((gxDisconnectControl*)object, serializer);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = ser_saveLimiter((gxLimiter*)object, serializer);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = ser_saveMBusClient((gxMBusClient*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = ser_saveModemConfiguration((gxModemConfiguration*)object, serializer);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = ser_savePppSetup((gxPppSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = ser_saveProfileGeneric((gxProfileGeneric*)object, serializer);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = ser_saveRegisterActivation((gxRegisterActivation*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = ser_saveRegisterMonitor((gxRegisterMonitor*)object, serializer);
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
        ret = ser_saveSapAssignment((gxSapAssignment*)object, serializer);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = ser_saveSchedule((gxSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = ser_saveScriptTable((gxScriptTable*)object, serializer);
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
        ret = ser_saveSpecialDaysTable((gxSpecialDaysTable*)object, serializer);
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
        ret = ser_saveTcpUdpSetup((gxTcpUdpSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = ser_saveUtilityTables((gxUtilityTables*)object, serializer);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = ser_saveMBusMasterPortSetup((gxMBusMasterPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        ret = ser_saveMessageHandler((gxMessageHandler*)object, serializer);
        break;
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = ser_savePushSetup((gxPushSetup*)object, serializer);
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
        ret = ser_saveAccount((gxAccount*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = ser_saveCredit((gxCredit*)object, serializer);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = ser_saveCharge((gxCharge*)object, serializer);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = ser_saveTokenGateway((gxTokenGateway*)object, serializer);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = ser_saveGsmDiagnostic((gxGsmDiagnostic*)object, serializer);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = ser_saveCompactData((gxCompactData*)object, serializer);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = ser_saveLlcSscsSetup((gxLlcSscsSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = ser_savePrimeNbOfdmPlcPhysicalLayerCounters((gxPrimeNbOfdmPlcPhysicalLayerCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = ser_savePrimeNbOfdmPlcMacSetup((gxPrimeNbOfdmPlcMacSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = ser_savePrimeNbOfdmPlcMacFunctionalParameters((gxPrimeNbOfdmPlcMacFunctionalParameters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = ser_savePrimeNbOfdmPlcMacCounters((gxPrimeNbOfdmPlcMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = ser_savePrimeNbOfdmPlcMacNetworkAdministrationData((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = ser_savePrimeNbOfdmPlcApplicationsIdentification((gxPrimeNbOfdmPlcApplicationsIdentification*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = ser_saveArbitrator((gxArbitrator*)object, serializer);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = ser_saveIec8802LlcType1Setup((gxIec8802LlcType1Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ser_saveIec8802LlcType2Setup((gxIec8802LlcType2Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ser_saveIec8802LlcType3Setup((gxIec8802LlcType3Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ser_saveSFSKActiveInitiator((gxSFSKActiveInitiator*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ser_saveFSKMacCounters((gxFSKMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ser_saveSFSKMacSynchronizationTimeouts((gxSFSKMacSynchronizationTimeouts*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ser_saveSFSKPhyMacSetUp((gxSFSKPhyMacSetUp*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ser_saveSFSKReportingSystemList((gxSFSKReportingSystemList*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = ser_saveTariffPlan((gxTariffPlan*)object, serializer);
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

int ser_loadDateTime(gxtime* value, gxByteBuffer* serializer)
{
    return cosem_getDateTime(serializer, value);
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
int ser_loadData(gxData* object, gxByteBuffer* serializer)
{
    return getVariantFromBytes(&object->value, serializer);
}

#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int ser_loadRegister(gxRegister* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getInt8(serializer, &object->scaler)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->unit)) != 0 ||
        (ret = getVariantFromBytes(&object->value, serializer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
int ser_loadClock(gxClock* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_loadDateTime(&object->time, serializer)) != 0 ||
        (ret = bb_getInt16(serializer, &object->timeZone)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->status)) != 0 ||
        (ret = ser_loadDateTime(&object->begin, serializer)) != 0 ||
        (ret = ser_loadDateTime(&object->end, serializer)) != 0 ||
        (ret = bb_getInt8(serializer, &object->deviation)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->enabled)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->clockBase)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int ser_loadScriptTable(
    dlmsSettings* settings,
    gxScriptTable* object,
    gxByteBuffer* serializer)
{
    int ret, pos, pos2;
    gxScript* s;
    gxScriptAction* sa;
    uint16_t count, count2;
    obj_clearScriptTable(&object->scripts);
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&s, sizeof(gxScript))) != 0)
            {
                break;
            }
#else
            s = (gxScript*)gxmalloc(sizeof(gxScript));
            arr_init(&s->actions);
            if (s == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = arr_push(&object->scripts, s)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_getUInt16(serializer, &s->id)) != 0)
            {
                break;
            }
            if ((ret = hlp_getObjectCount2(serializer, &count2)) == 0)
            {
                for (pos2 = 0; pos2 != count2; ++pos2)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = arr_getByIndex(&s->actions, pos2, (void**)&sa, sizeof(gxScriptAction))) != 0)
                    {
                        break;
                    }
#else
                    sa = (gxScriptAction*)gxmalloc(sizeof(gxScriptAction));
                    var_init(&sa->parameter);
                    if (sa == NULL)
                    {
                        return DLMS_ERROR_CODE_OUTOFMEMORY;
                    }
                    sa->type = 0;
                    if ((ret = arr_push(&s->actions, sa)) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
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
                    if ((ret = oa_findByLN(&settings->objects, ot, ln, &sa->target)) != 0)
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
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int ser_loadSpecialDaysTable(
    gxSpecialDaysTable* object,
    gxByteBuffer* serializer)
{
    int ret, pos;
    gxSpecialDay* sd;
    uint16_t count;
    arr_clear(&object->entries);
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd, sizeof(gxSpecialDay))) != 0)
            {
                break;
            }
#else
            sd = (gxSpecialDay*)gxmalloc(sizeof(gxSpecialDay));
            if (sd == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = arr_push(&object->entries, sd)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_getUInt16(serializer, &sd->index)) != 0 ||
                (ret = ser_loadDateTime(&sd->date, serializer)) != 0 ||
                (ret = bb_getUInt8(serializer, &sd->dayId)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int ser_loadTcpUdpSetup(dlmsSettings* settings,
    gxTcpUdpSetup* object,
    gxByteBuffer* serializer)
{
    int ret;
    unsigned char ln[6];
    if ((ret = bb_getUInt16(serializer, &object->port)) == 0 &&
        (ret = bb_get(serializer, ln, 6)) == 0)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->ipSetup)) != 0)
        {
            return ret;
        }
#else
        if (ret = bb_get(&ba, object->ipReference, 6)) != 0 ||
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if ((ret = bb_getUInt16(serializer, &object->maximumSegmentSize)) != 0 ||
            (ret = bb_getUInt8(serializer, &object->maximumSimultaneousConnections)) != 0 ||
            (ret = bb_getUInt16(serializer, &object->inactivityTimeout)) != 0)
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int ser_loadMBusMasterPortSetup(gxMBusMasterPortSetup* object, gxByteBuffer* serializer)
{
    return bb_getUInt8(serializer, (unsigned char*)&object->commSpeed);
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

#ifndef DLMS_IGNORE_MESSAGE_HANDLER
int ser_loadMessageHandler(
    gxMessageHandler* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_MESSAGE_HANDLER

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
#ifdef DLMS_IGNORE_MALLOC
    gxTarget* it;
#else
    gxObject* key;
    gxTarget* value;
#endif //DLMS_IGNORE_MALLOC
    obj_clearProfileGenericCaptureObjects(objects);
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(objects, pos, (void**)&it, sizeof(gxTarget))) != 0)
            {
                break;
            }
            if ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
                (ret = bb_get(serializer, ln, 6)) != 0 ||
                (ret = bb_getInt8(serializer, &it->attributeIndex)) != 0 ||
                (ret = bb_getUInt16(serializer, &it->dataIndex)) != 0 ||
                (ret = oa_findByLN(&settings->objects, ot, ln, &it->target)) != 0)
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
                (ret = oa_findByLN(&settings->objects, ot, ln, &key)) != 0 ||
                (ret = bb_getInt8(serializer, &value->attributeIndex)) != 0 ||
                (ret = bb_getUInt16(serializer, &value->dataIndex)) != 0)
            {
                gxfree(value);
                break;
            }
            if (key == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = arr_push(objects, key_init(key, value))) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
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
int ser_loadPushSetup(dlmsSettings* settings, gxPushSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_loadObjects2(settings, serializer, &object->pushObjectList)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->service)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
        (ret = ser_loadOctetString3(serializer, object->destination.value, &object->destination.size)) != 0 ||
#else
        (ret = ser_loadOctetString2(serializer, &object->destination)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->message)) != 0 ||
        (ret = loadTimeWindow(serializer, &object->communicationWindow)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->randomisationStartInterval)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->numberOfRetries)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->repetitionDelay)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_AUTO_CONNECT
int ser_loadAutoConnect(gxAutoConnect* object, gxByteBuffer* serializer)
{
    uint16_t count;
#ifdef DLMS_IGNORE_MALLOC
    gxDestination* dest;
#else
    gxByteBuffer* dest;
#endif //DLMS_IGNORE_MALLOC
    int pos, ret;
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->mode)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->repetitions)) == 0 &&
        (ret = bb_getUInt16(serializer, &object->repetitionDelay)) == 0 &&
        (ret = loadTimeWindow(serializer, &object->callingWindow)) == 0)
    {
        arr_clearStrings(&object->destinations);
        if (ret == 0 && (ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
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
int ser_loadSeasonProfile(gxArray* arr, gxByteBuffer* serializer)
{
    gxSeasonProfile* it;
    int pos, ret;
    uint16_t count;
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&it, sizeof(gxSeasonProfile))) != 0)
            {
                break;
            }
            if ((ret = ser_loadOctetString3(serializer, it->name.value, &it->name.size)) != 0 ||
                (ret = ser_loadDateTime(&it->start, serializer)) != 0 ||
                (ret = ser_loadOctetString3(serializer, it->weekName.value, &it->weekName.size)) != 0)
            {
                break;
            }
#else
            it = (gxSeasonProfile*)gxmalloc(sizeof(gxSeasonProfile));
            if (it == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
            bb_init(&it->name);
            bb_init(&it->weekName);
            if ((ret = arr_push(arr, it)) != 0)
            {
                break;
            }
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

int ser_loadweekProfile(gxArray* arr, gxByteBuffer* serializer)
{
    gxWeekProfile* it;
    int pos, ret;
    uint16_t count;
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&it, sizeof(gxWeekProfile))) != 0)
            {
                break;
            }
            if ((ret = ser_loadOctetString3(serializer, it->name.value, &it->name.size)) != 0)
            {
                break;
            }
#else
            it = (gxWeekProfile*)gxmalloc(sizeof(gxWeekProfile));
            if (it == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            bb_init(&it->name);
            if ((ret = arr_push(arr, it)) != 0)
            {
                break;
            }
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

int ser_loadDayProfile(dlmsSettings* settings, gxArray* arr, gxByteBuffer* serializer)
{
    gxDayProfile* dp;
    gxDayProfileAction* it;
    int pos, pos2, ret;
    uint16_t count, count2;
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(arr, pos, (void**)&dp, sizeof(gxDayProfile))) != 0)
            {
                break;
            }
#else
            dp = (gxDayProfile*)gxmalloc(sizeof(gxDayProfile));
            if (dp == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            arr_init(&dp->daySchedules);
            if ((ret = arr_push(arr, dp)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_getUInt8(serializer, &dp->dayId)) != 0 ||
                (ret = hlp_getObjectCount2(serializer, &count2)) == 0)
            {
                break;
            }
            for (pos2 = 0; pos2 != count2; ++pos2)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&it, sizeof(gxDayProfileAction))) != 0)
                {
                    break;
                }
#else
                it = (gxDayProfileAction*)gxmalloc(sizeof(gxDayProfileAction));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                if ((ret = arr_push(&dp->daySchedules, it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC

#ifndef DLMS_IGNORE_OBJECT_POINTERS
                unsigned char ln[6];
                if ((ret = bb_get(serializer, ln, 6)) != 0)
                {
                    break;
                }
                if ((ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &it->script)) != 0)
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
    dlmsSettings* settings,
    gxActivityCalendar* object,
    gxByteBuffer* serializer)
{
    int ret;
    obj_clear((gxObject*)object);
    if ((ret = ser_loadOctetString(serializer, &object->calendarNameActive)) != 0 ||
        (ret = ser_loadSeasonProfile(&object->seasonProfileActive, serializer)) != 0 ||
        (ret = ser_loadweekProfile(&object->weekProfileTableActive, serializer)) != 0 ||
        (ret = ser_loadDayProfile(settings, &object->dayProfileTableActive, serializer)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->calendarNameActive)) != 0 ||
        (ret = ser_loadSeasonProfile(&object->seasonProfilePassive, serializer)) != 0 ||
        (ret = ser_loadweekProfile(&object->weekProfileTablePassive, serializer)) != 0 ||
        (ret = ser_loadDayProfile(settings, &object->dayProfileTablePassive, serializer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_SECURITY_SETUP
int ser_loadSecuritySetup(gxSecuritySetup* object, gxByteBuffer* serializer)
{
    unsigned char v;
    int ret;
    if ((ret = bb_getUInt8(serializer, &v)) == 0)
    {
        object->securityPolicy = (DLMS_SECURITY_POLICY)v;
        if ((ret = bb_getUInt8(serializer, &v)) == 0 &&
            (ret = bb_clear(&object->serverSystemTitle)) == 0 &&
            (ret = bb_clear(&object->clientSystemTitle)) == 0 &&
            (ret = bb_set2(&object->serverSystemTitle, serializer, serializer->position, 8)) == 0 &&
            (ret = bb_set2(&object->clientSystemTitle, serializer, serializer->position, 8)) == 0)
        {
            object->securitySuite = (DLMS_SECURITY_SUITE)v;
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int ser_loadHdlcSetup(gxIecHdlcSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->communicationSpeed)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->windowSizeTransmit)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->windowSizeReceive)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->maximumInfoLengthTransmit)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->maximumInfoLengthReceive)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->interCharachterTimeout)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->inactivityTimeout)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->deviceAddress)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int ser_loadLocalPortSetup(gxLocalPortSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->defaultMode)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->defaultBaudrate)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->proposedBaudrate)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->responseTime)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->deviceAddress)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->password1)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->password2)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->password5)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int ser_loadIecTwistedPairSetup(gxIecTwistedPairSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->mode)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->speed)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->primaryAddresses)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->tabis)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int ser_loadDemandRegister(gxDemandRegister* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = getVariantFromBytes(&object->currentAverageValue, serializer)) != 0 ||
        (ret = getVariantFromBytes(&object->lastAverageValue, serializer)) != 0 ||
        (ret = bb_getInt8(serializer, &object->scaler)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->unit)) != 0 ||
        (ret = getVariantFromBytes(&object->status, serializer)) != 0 ||
        (ret = ser_loadDateTime(&object->captureTime, serializer)) != 0 ||
        (ret = ser_loadDateTime(&object->startTimeCurrent, serializer)) != 0 ||
        (ret = bb_getUInt32(serializer, &object->period)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->numberOfPeriods)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int ser_loadRegisterActivation(
    dlmsSettings* settings,
    gxRegisterActivation* object,
    gxByteBuffer* serializer)
{
    int pos, ret;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
#ifdef DLMS_IGNORE_MALLOC
    gxRegisterActivationMask* it;
    uint16_t v;
#else
    gxByteBuffer* key, * value;
#endif //DLMS_IGNORE_MALLOC
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    uint16_t count;
    obj_clearRegisterActivationMaskList(&object->maskList);
    bb_clear(&object->activeMask);
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            object->registerAssignment.size = count;
            if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od, sizeof(gxObject))) != 0 ||
                (ret = bb_getUInt16(serializer, &od->objectType)) != 0 ||
                (ret = bb_get(serializer, od->logicalName, 6)) != 0)
            {
                break;
            }
#else
            unsigned char ln[6];
            uint16_t ot;
            if ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
                (ret = bb_get(serializer, ln, 6)) != 0 ||
                (ret = oa_findByLN(&settings->objects, ot, ln, &od)) != 0 ||
                (ret = oa_push(&object->registerAssignment, od)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
        if (ret == 0 && (ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
            object->maskList.size = count;
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&it, sizeof(gxRegisterActivationMask))) != 0)
                {
                    break;
                }

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
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_SCRIPT_TABLE, ln, (gxObject**)&item->script)) != 0)
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
    dlmsSettings* settings,
    gxRegisterMonitor* object,
    gxByteBuffer* serializer)
{
    int pos, ret;
    dlmsVARIANT_PTR tmp;
    gxActionSet* as;
    uint16_t count;
    obj_clear((gxObject*)object);
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        object->thresholds.size = count;
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
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        uint16_t ot;
        unsigned char ln[6];
        if (ret == 0 &&
            ((ret = bb_getUInt16(serializer, &ot)) != 0 ||
                (ret = bb_get(serializer, ln, 6)) != 0 ||
                (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->monitoredValue.target)) != 0))
        {

        }
#else
        if (ret == 0 &&
            (ret = bb_getUInt16(serializer, &object->monitoredValue.objectType)) != 0)
            (ret = bb_get(serializer, object->monitoredValue.logicalName, 6)) != 0 ||
        {

        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if (ret == 0 && (ret = bb_getInt8(serializer, &object->monitoredValue.attributeIndex)) == 0)
        {
            if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
            {
                object->actions.size = count;
                for (pos = 0; pos != count; ++pos)
                {
#ifdef DLMS_IGNORE_MALLOC
                    if ((ret = arr_getByIndex(&object->actions, pos, (void**)&as, sizeof(gxActionSet))) != 0)
                    {
                        break;
                    }
#else
                    as = (gxActionSet*)gxmalloc(sizeof(gxActionSet));
                    if (as == NULL)
                    {
                        return DLMS_ERROR_CODE_OUTOFMEMORY;
                    }
                    if ((ret = arr_push(&object->actions, as)) != 0)
                    {
                        break;
                    }
#endif //DLMS_IGNORE_MALLOC
                    if ((ret = ser_loadActionItem(settings, &as->actionUp, serializer)) != 0 ||
                        (ret = ser_loadActionItem(settings, &as->actionDown, serializer)) != 0)
                    {
                        break;
                    }
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int ser_loadActionSchedule(
    dlmsSettings* settings,
    gxActionSchedule* object,
    gxByteBuffer* serializer)
{
    int pos = 0, ret;
    gxtime* tm;
    object->type = 0;
    uint16_t count;
    obj_clear((gxObject*)object);
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    if ((ret = bb_get(serializer, ln, 6)) != 0 ||
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, (gxObject**)&object->executedScript)) != 0)
    {
        return ret;
    }
#else
    if ((ret = bb_get(serializer, object->executedScriptLogicalName, 6)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_getUInt16(serializer, &object->executedScriptSelector)) == 0 &&
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->type)) == 0 &&
        (ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {

#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0 ||
                (ret = ser_loadDateTime(tm, serializer)) != 0)
            {
                break;
            }
#else
            tm = (gxtime*)gxmalloc(sizeof(gxtime));
            if ((ret = ser_loadDateTime(tm, serializer)) != 0 ||
                (ret = arr_push(&object->executionTime, tm)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int ser_loadSapAssignment(gxSapAssignment* object, gxByteBuffer* serializer)
{
    int ret = 0, pos = 0;
    gxSapItem* it;
    uint16_t count;
    obj_clear((gxObject*)object);
    //Add count.
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->sapAssignmentList, pos, (void**)&it, sizeof(gxSapItem))) != 0)
            {
                break;
            }
            if ((ret = bb_getUInt16(serializer, &it->id)) != 0 ||
                (ret = ser_loadOctetString3(serializer, it->name.value, &it->name.size)) != 0)
            {
                break;
            }
#else
            it = (gxSapItem*)gxmalloc(sizeof(gxSapItem));
            if (it == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            bb_init(&it->name);
            if ((ret = arr_push(&object->sapAssignmentList, it)) != 0)
            {
                break;
            }
            if ((ret = bb_getUInt16(serializer, &it->id)) != 0 ||
                (ret = ser_loadOctetString(serializer, &it->name)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_AUTO_ANSWER
int ser_loadAutoAnswer(gxAutoAnswer* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->mode)) == 0 &&
        (ret = loadTimeWindow(serializer, &object->listeningWindow)) == 0)
    {
        if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->status)) == 0 &&
            (ret = bb_getUInt8(serializer, &object->numberOfCalls)) == 0 &&
            (ret = bb_getUInt8(serializer, &object->numberOfRingsInListeningWindow)) == 0 &&
            (ret = bb_getUInt8(serializer, &object->numberOfRingsOutListeningWindow)) == 0 &&
            (ret = bb_getUInt8(serializer, &object->numberOfRingsOutListeningWindow)) == 0)
        {

        }
    }
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int ser_loadIp4Setup(
    dlmsSettings* settings,
    gxIp4Setup* object,
    gxByteBuffer* serializer)
{
    int ret;
#ifdef DLMS_IGNORE_MALLOC
    uint32_t* v;
#else
    dlmsVARIANT* tmp;
#endif //DLMS_IGNORE_MALLOC
    gxip4SetupIpOption* ip;
    uint16_t pos, count;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    obj_clear((gxObject*)object);
    if ((ret = bb_get(serializer, ln, 6)) != 0 ||
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
    {
        return ret;
    }
#else
    if ((ret = bb_get(serializer, object->dataLinkLayerReference, 6)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_getUInt32(serializer, &object->ipAddress)) == 0 &&
        (ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->multicastIPAddress, pos, (void**)&v, sizeof(uint32_t))) != 0 ||
                (ret = bb_getUInt32(serializer, v)) != 0)
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
    if (ret == 0 && (ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->ipOptions, pos, (void**)&ip, sizeof(gxip4SetupIpOption))) != 0)
            {
                break;
            }
            if ((ret = bb_getUInt8(serializer, (unsigned char*)&ip->type)) != 0 ||
                (ret = ser_loadOctetString3(serializer, ip->data.value, &ip->data.size)) != 0)
            {
                break;
            }
#else
            ip = (gxip4SetupIpOption*)gxmalloc(sizeof(gxip4SetupIpOption));
            if (ip == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = arr_push(&object->ipOptions, ip)) != 0)
            {
                break;
            }
            if ((ret = bb_getUInt8(serializer, (unsigned char*)&ip->type)) != 0 ||
                (ret = ser_loadOctetString(serializer, &ip->data)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
        if (ret == 0 &&
            ((ret = bb_getUInt32(serializer, &object->subnetMask)) != 0 ||
                (ret = bb_getUInt32(serializer, &object->gatewayIPAddress)) != 0 ||
                (ret = bb_getUInt8(serializer, &object->useDHCP)) != 0 ||
                (ret = bb_getUInt32(serializer, &object->primaryDNSAddress)) != 0 ||
                (ret = bb_getUInt32(serializer, &object->secondaryDNSAddress)) != 0))
        {
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP
int ser_loadIp6Setup(
    dlmsSettings* settings,
    gxIp6Setup* object,
    gxByteBuffer* serializer)
{
    int ret;
    IN6_ADDR* ip;
    uint16_t pos, count;
    unsigned char ch;
    //unsigned char tmp[6];
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    obj_clear((gxObject*)object);
    if ((ret = bb_get(serializer, ln, 6)) != 0 ||
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->dataLinkLayer)) != 0)
    {
        return ret;
    }
#else
    if ((ret = bb_get(serializer, object->dataLinkLayerReference, 6)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    bb_getUInt8(serializer, &ch);
    object->addressConfigMode = (DLMS_IP6_ADDRESS_CONFIG_MODE)ch;
    arr_clear(&object->unicastIPAddress);
    if (ret == 0 && (ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        object->unicastIPAddress.size = count;
#endif //DLMS_IGNORE_MALLOC
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->unicastIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                (ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0)
            {
                break;
            }
#else
            ip = (IN6_ADDR*)gxmalloc(sizeof(IN6_ADDR));
            if ((ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0 ||
                (ret = arr_push(&object->unicastIPAddress, ip)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }

    arr_clear(&object->multicastIPAddress);
    if (ret == 0 && (ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        object->multicastIPAddress.size = count;
#endif //DLMS_IGNORE_MALLOC
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->multicastIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                (ret = bb_get(serializer, (unsigned char*) ip, 16)) != 0)
            {
                break;
            }
#else
            ip = (IN6_ADDR*)gxmalloc(sizeof(IN6_ADDR));
            if ((ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0 ||
                (ret = arr_push(&object->multicastIPAddress, ip)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }

    arr_clear(&object->gatewayIPAddress);
    if (ret == 0 && (ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
#ifdef DLMS_IGNORE_MALLOC
        object->gatewayIPAddress.size = count;
#endif //DLMS_IGNORE_MALLOC
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->gatewayIPAddress, pos, (void**)&ip, sizeof(IN6_ADDR))) != 0 ||
                (ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0)
            {
                break;
            }
#else
            ip = (IN6_ADDR*)gxmalloc(sizeof(IN6_ADDR));
            if ((ret = bb_get(serializer, (unsigned char*)ip, 16)) != 0 ||
                (ret = arr_push(&object->gatewayIPAddress, ip)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }

    if ((ret = bb_get(serializer, (unsigned char*)&object->primaryDNSAddress, 16)) != 0)
    {
        return ret;
    }
    if ((ret = bb_get(serializer, (unsigned char*)&object->secondaryDNSAddress, 16)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(serializer, &ch)) != 0)
    {
        return ret;
    }
    object->trafficClass = (DLMS_IP6_ADDRESS_CONFIG_MODE)ch;
    arr_clear(&object->neighborDiscoverySetup);
    gxNeighborDiscoverySetup* it2;
    if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->neighborDiscoverySetup, pos, (void**)&it2, sizeof(gxNeighborDiscoverySetup))) != 0)
            {
                break;
            }
#else
            it2 = (gxNeighborDiscoverySetup*)gxmalloc(sizeof(gxNeighborDiscoverySetup));
            if (it2 == NULL)
            {
                ret = DLMS_ERROR_CODE_OUTOFMEMORY;
                break;
            }
            if ((ret = arr_push(&object->neighborDiscoverySetup, it2)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
            if ((ret = bb_getUInt8(serializer, &it2->maxRetry)) != 0 ||
                (ret = bb_getUInt16(serializer, &it2->retryWaitTime)) != 0 ||
                (ret = bb_getUInt32(serializer, &it2->sendPeriod)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
int ser_loadUtilityTables(gxUtilityTables* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt16(serializer, &object->tableId)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->buffer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int ser_loadMbusSlavePortSetup(gxMbusSlavePortSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->defaultBaud)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->availableBaud)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->addressState)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->busAddress)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int ser_loadImageTransfer(
    gxImageTransfer* object,
    gxByteBuffer* serializer)
{
    uint16_t count;
    int pos, ret;
    gxImageActivateInfo* it;
    obj_clear((gxObject*)object);
    if ((ret = bb_getUInt32(serializer, &object->imageBlockSize)) == 0 &&
        (ret = ser_loadBitString(serializer, &object->imageTransferredBlocksStatus)) == 0 &&
        (ret = bb_getUInt32(serializer, &object->imageFirstNotTransferredBlockNumber)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->imageTransferEnabled)) == 0 &&
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->imageTransferStatus)) == 0)
    {
        if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&it, sizeof(gxImageActivateInfo))) != 0)
                {
                    break;
                }
                if ((ret = bb_getUInt32(serializer, &it->size)) != 0 ||
                    (ret = ser_loadOctetString3(serializer, it->identification.data, &it->identification.size)) != 0 ||
                    (ret = ser_loadOctetString3(serializer, it->signature.data, &it->signature.size)) != 0)
                {
                    break;
                }
#else
                it = (gxImageActivateInfo*)gxmalloc(sizeof(gxImageActivateInfo));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                if ((ret = arr_push(&object->imageActivateInfo, it)) != 0)
                {
                    break;
                }
                if ((ret = bb_getUInt32(serializer, &it->size)) != 0 ||
                    (ret = ser_loadOctetString(serializer, &it->identification)) != 0 ||
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
int ser_loadDisconnectControl(gxDisconnectControl* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, &object->outputState)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->controlState)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->controlMode)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int ser_loadLimiter(dlmsSettings* settings,
    gxLimiter* object,
    gxByteBuffer* serializer)
{
    int pos, ret;
#ifdef DLMS_IGNORE_MALLOC
    uint16_t* it;
#else
    dlmsVARIANT* it;
#endif //DLMS_IGNORE_MALLOC
    uint16_t count;
    unsigned char ln[6];
    obj_clear((gxObject*)object);
    if ((ret = bb_get(serializer, ln, 6)) == 0 &&
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->monitoredValue)) == 0 &&
        (ret = bb_getInt8(serializer, &object->selectedAttributeIndex)) == 0 &&
        (ret = getVariantFromBytes(&object->thresholdActive, serializer)) == 0 &&
        (ret = getVariantFromBytes(&object->thresholdNormal, serializer)) == 0 &&
        (ret = getVariantFromBytes(&object->thresholdEmergency, serializer)) == 0 &&
        (ret = bb_getInt32(serializer, &object->minOverThresholdDuration)) == 0 &&
        (ret = bb_getInt32(serializer, &object->minUnderThresholdDuration)) == 0 &&
        (ret = bb_getUInt16(serializer, &object->emergencyProfile.id)) == 0 &&
        (ret = ser_loadDateTime(&object->emergencyProfile.activationTime, serializer)) == 0 &&
        (ret = bb_getUInt32(serializer, &object->emergencyProfile.duration)) == 0)
        if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(uint16_t))) != 0 ||
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
            if (ret == 0 && ((ret = bb_getUInt8(serializer, &object->emergencyProfileActive)) != 0 ||
                (ret = ser_loadActionItem(settings, &object->actionOverThreshold, serializer)) != 0 ||
                (ret = ser_loadActionItem(settings, &object->actionUnderThreshold, serializer)) != 0))
            {
            }
        }
    return ret;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int ser_loadMBusClient(dlmsSettings* settings, gxMBusClient* object, gxByteBuffer* serializer)
{
    int ret;
    unsigned char ln[6];
    if ((ret = bb_getUInt32(serializer, &object->capturePeriod)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->primaryAddress)) != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_get(serializer, ln, 6)) != 0 ||
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->mBusPort)) != 0)
    {
        return ret;
    }
#else
    if ((ret = bb_get(serializer, object->mBusPortReference, 6)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_getUInt32(serializer, &object->identificationNumber)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->manufacturerID)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->dataHeaderVersion)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->deviceType)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->accessNumber)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->status)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->alarm)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->configuration)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->encryptionKeyStatus)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int ser_loadModemConfiguration(
    gxModemConfiguration* object,
    gxByteBuffer* serializer)
{
    int pos, ret;
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    uint16_t count;
    obj_clear((gxObject*)object);
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->communicationSpeed)) == 0 &&
        (ret = hlp_getObjectCount2(serializer, &count)) == 0)
    {
        for (pos = 0; pos != count; ++pos)
        {
#ifdef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->initialisationStrings, pos, (void**)&mi, sizeof(gxModemInitialisation))) != 0 ||
                (ret = ser_loadOctetString3(serializer, mi->request.value, &mi->request.size)) != 0 ||
                (ret = ser_loadOctetString3(serializer, mi->response.value, &mi->response.size)) != 0 ||
                (ret = bb_getUInt16(serializer, &mi->delay)) != 0)
            {
                break;
            }
#else
            mi = (gxModemInitialisation*)gxmalloc(sizeof(gxModemInitialisation));
            bb_init(&mi->request);
            bb_init(&mi->response);
            if (mi == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            if ((ret = arr_push(&object->initialisationStrings, mi)) != 0)
            {
                break;
            }
            if ((ret = ser_loadOctetString(serializer, &mi->request)) != 0 ||
                (ret = ser_loadOctetString(serializer, &mi->response)) != 0 ||
                (ret = bb_getUInt16(serializer, &mi->delay)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
        if (ret == 0 &&
            (ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
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
int ser_loadMacAddressSetup(gxMacAddressSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_loadOctetString(serializer, &object->macAddress)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
int ser_loadQualityOfService(gxQualityOfService* object, gxByteBuffer* serializer)
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

int ser_loadGPRSSetup(gxGPRSSetup* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = ser_loadOctetString(serializer, &object->apn)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->pinCode)) != 0 ||
        (ret = ser_loadQualityOfService(&object->defaultQualityOfService, serializer)) != 0 ||
        (ret = ser_loadQualityOfService(&object->requestedQualityOfService, serializer)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int ser_loadExtendedRegister(gxExtendedRegister* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = getVariantFromBytes(&object->value, serializer)) != 0 ||
        (ret = bb_getInt8(serializer, &object->scaler)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->unit)) != 0 ||
        (ret = getVariantFromBytes(&object->status, serializer)) != 0 ||
        (ret = ser_loadDateTime(&object->captureTime, serializer)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

int ser_loadApplicationContextName(gxByteBuffer* serializer, gxApplicationContextName* object)
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
    int ret;
    if ((ret = bb_getUInt8(serializer, &object->jointIsoCtt)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->country)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->countryName)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->identifiedOrganization)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->dlmsUA)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->authenticationMechanismName)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->mechanismId)) != 0)
    {

    }
    return ret;
}

int ser_loadAssociationLogicalName(
    dlmsSettings* settings,
    gxAssociationLogicalName* object,
    gxByteBuffer* serializer)
{
#ifdef DLMS_IGNORE_MALLOC
    gxUser* it;
#endif //DLMS_IGNORE_MALLOC

    int pos, ret = 0;
    uint16_t count;
    if ((ret = bb_getInt8(serializer, &object->clientSAP)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->serverSAP)) != 0 ||
        //This is not needed (ret = ser_loadApplicationContextName(serializer, &object->applicationContextName)) != 0 ||
        //This is not needed (ret = ser_loadxDLMSContextType(serializer, &object->xDLMSContextInfo)) != 0 ||
        //This is not needed (ret = ser_loadAuthenticationMechanismName(serializer, &object->authenticationMechanismName)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->secret)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->associationStatus)) != 0)
    {

    }
    //Security Setup Reference is from version 1.
    if (object->base.version > 0)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char ln[6];
        if ((ret = bb_get(serializer, ln, 6)) != 0 ||
            (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, (gxObject**)&object->securitySetup)) != 0)
        {
            return ret;
        }

#else
        if ((ret = bb_get(&ba, object->securitySetupReference, 6)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    obj_clearUserList(&object->userList);
    if (object->base.version > 1)
    {
        if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->userList, pos, (void**)&it, sizeof(gxUser))) != 0 ||
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
    return ret;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int ser_loadAssociationShortName(gxAssociationShortName* object, gxByteBuffer* serializer)
{
    return 0;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int ser_loadPppSetup(dlmsSettings* settings,
    gxPppSetup* object,
    gxByteBuffer* serializer)
{
    int ret;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    unsigned char ln[6];
    if ((ret = bb_get(serializer, ln, 6)) != 0 ||
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->phy)) != 0)
    {
        return ret;
    }
#else
    if ((ret = bb_get(serializer, object->PHYReference, 6)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = ser_loadOctetString(serializer, &object->userName)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->password)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->authentication)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int ser_loadProfileGeneric(
    dlmsSettings* settings,
    gxProfileGeneric* object,
    gxByteBuffer* serializer)
{
    int ret;
    unsigned char ln[6];
    if ((ret = obj_clearProfileGenericCaptureObjects(&object->captureObjects)) != 0 ||
        (ret = ser_loadObjects2(settings, serializer, &object->captureObjects)) != 0 ||
        (ret = bb_getUInt32(serializer, &object->capturePeriod)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->sortMethod)) != 0 ||
        (ret = bb_get(serializer, ln, 6)) != 0 ||
        (ret = oa_findByLN(&settings->objects, DLMS_OBJECT_TYPE_NONE, ln, &object->sortObject)) != 0 ||
        (ret = bb_getUInt32(serializer, &object->entriesInUse)) != 0 ||
        (ret = bb_getUInt32(serializer, &object->profileEntries)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int ser_loadAccount(gxAccount* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getUInt8(serializer, (unsigned char*)&object->paymentMode)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->accountStatus)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->currentCreditInUse)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->currentCreditStatus)) != 0 ||
        (ret = bb_getInt32(serializer, &object->availableCredit)) != 0 ||
        (ret = bb_getInt32(serializer, &object->amountToClear)) != 0 ||
        (ret = bb_getInt32(serializer, &object->clearanceThreshold)) != 0 ||
        (ret = bb_getInt32(serializer, &object->aggregatedDebt)) != 0)
    {

    }
    ser_loadDateTime(&object->accountActivationTime, serializer);
    ser_loadDateTime(&object->accountClosureTime, serializer);
    if ((ret = bb_getInt32(serializer, &object->lowCreditThreshold)) != 0 ||
        (ret = bb_getInt32(serializer, &object->nextCreditAvailableThreshold)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->maxProvision)) != 0 ||
        (ret = bb_getInt32(serializer, &object->maxProvisionPeriod)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int ser_loadCredit(gxCredit* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getInt32(serializer, &object->currentCreditAmount)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->type)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->priority)) != 0 ||
        (ret = bb_getInt32(serializer, &object->warningThreshold)) != 0 ||
        (ret = bb_getInt32(serializer, &object->limit)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->creditConfiguration)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->status)) != 0 ||
        (ret = bb_getInt32(serializer, &object->presetCreditAmount)) != 0 ||
        (ret = bb_getInt32(serializer, &object->creditAvailableThreshold)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
int ser_loadCharge(gxCharge* object, gxByteBuffer* serializer)
{
    int ret;
    if ((ret = bb_getInt16(serializer, &object->totalAmountPaid)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->chargeType)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->priority)) != 0 ||
        // (ret = bb_getUInt8(serializer, &object->unitChargeActive)) != 0 ||
        // (ret = bb_getUInt8(serializer, &object->unitChargePassive)) != 0 ||
        (ret = ser_loadDateTime(&object->unitChargeActivationTime, serializer)) != 0 ||
        (ret = bb_getUInt32(serializer, &object->period)) != 0 ||
        (ret = ser_loadDateTime(&object->lastCollectionTime, serializer)) != 0 ||
        (ret = bb_getInt32(serializer, &object->lastCollectionAmount)) != 0 ||
        (ret = bb_getInt32(serializer, &object->totalAmountRemaining)) != 0 ||
        (ret = bb_getUInt16(serializer, &object->proportion)) != 0)
    {
    }
    return ret;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int ser_loadTokenGateway(
    gxTokenGateway* object,
    gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int ser_loadGsmDiagnostic(
    gxGsmDiagnostic* object,
    gxByteBuffer* serializer)
{
    int ret, pos;
    gxAdjacentCell* it;
    uint16_t count;
    obj_clear((gxObject*)object);
#ifdef DLMS_IGNORE_MALLOC
    if ((ret = ser_loadOctetString(serializer, &object->operatorName)) == 0 &&
#else
    if ((ret = ser_loadOctetString2(serializer, &object->operatorName)) == 0 &&
#endif //DLMS_IGNORE_MALLOC
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->status)) == 0 &&
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->circuitSwitchStatus)) == 0 &&
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->packetSwitchStatus)) == 0 &&
        (ret = bb_getUInt32(serializer, &object->cellInfo.cellId)) == 0 &&
        (ret = bb_getUInt16(serializer, &object->cellInfo.locationId)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->cellInfo.signalQuality)) == 0 &&
        (ret = bb_getUInt8(serializer, &object->cellInfo.ber)) == 0)
    {
        if ((ret = hlp_getObjectCount2(serializer, &count)) == 0)
        {
            for (pos = 0; pos != count; ++pos)
            {

#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it, sizeof(gxAdjacentCell))) != 0)
                {
                    break;
                }
#else
                it = (gxAdjacentCell*)gxmalloc(sizeof(gxAdjacentCell));
                if (it == NULL)
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                if ((ret = arr_push(&object->adjacentCells, it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_getUInt32(serializer, &it->cellId)) != 0 ||
                    (ret = bb_getUInt8(serializer, &it->signalQuality)) != 0)
                {
                    break;
                }
            }
        }
    }
    return ret;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
int ser_loadCompactData(
    dlmsSettings* settings,
    gxCompactData* object,
    gxByteBuffer* serializer)
{
    int ret;
    if ((ret = obj_clearProfileGenericCaptureObjects(&object->captureObjects)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->buffer)) != 0 ||
        (ret = ser_loadObjects2(settings, serializer, &object->captureObjects)) != 0 ||
        (ret = bb_getUInt8(serializer, &object->templateId)) != 0 ||
        (ret = ser_loadOctetString(serializer, &object->templateDescription)) != 0 ||
        (ret = bb_getUInt8(serializer, (unsigned char*)&object->captureMethod)) != 0)
    {

    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int ser_loadLlcSscsSetup(gxLlcSscsSetup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int ser_loadPrimeNbOfdmPlcPhysicalLayerCounters(gxPrimeNbOfdmPlcPhysicalLayerCounters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int  ser_loadPrimeNbOfdmPlcMacSetup(gxPrimeNbOfdmPlcMacSetup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int  ser_loadPrimeNbOfdmPlcMacFunctionalParameters(gxPrimeNbOfdmPlcMacFunctionalParameters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int  ser_loadPrimeNbOfdmPlcMacCounters(gxPrimeNbOfdmPlcMacCounters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int  ser_loadPrimeNbOfdmPlcMacNetworkAdministrationData(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int  ser_loadPrimeNbOfdmPlcApplicationsIdentification(gxPrimeNbOfdmPlcApplicationsIdentification* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION

#ifndef DLMS_IGNORE_ARBITRATOR
int ser_loadArbitrator(gxArbitrator* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int ser_loadIec8802LlcType1Setup(gxIec8802LlcType1Setup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int ser_loadIec8802LlcType2Setup(gxIec8802LlcType2Setup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int ser_loadIec8802LlcType3Setup(gxIec8802LlcType3Setup* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int ser_loadSFSKActiveInitiator(gxSFSKActiveInitiator* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int ser_loadFSKMacCounters(gxFSKMacCounters* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int ser_loadSFSKMacSynchronizationTimeouts(gxSFSKMacSynchronizationTimeouts* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int ser_loadSFSKPhyMacSetUp(gxSFSKPhyMacSetUp* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int ser_loadSFSKReportingSystemList(gxSFSKReportingSystemList* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifndef DLMS_IGNORE_SCHEDULE
int ser_loadSchedule(gxSchedule* object, gxByteBuffer* serializer)
{
    return 0;
}
#endif //DLMS_IGNORE_SCHEDULE

#ifdef DLMS_ITALIAN_STANDARD
int ser_loadTariffPlan(gxTariffPlan* object, gxByteBuffer* serializer)
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
        ret = ser_loadData((gxData*)object, serializer);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = ser_loadRegister((gxRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = ser_loadClock((gxClock*)object, serializer);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = ser_loadActionSchedule(settings, (gxActionSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = ser_loadActivityCalendar(settings, (gxActivityCalendar*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = ser_loadAssociationLogicalName(settings, (gxAssociationLogicalName*)object, serializer);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = ser_loadAssociationShortName((gxAssociationShortName*)object, serializer);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = ser_loadAutoAnswer((gxAutoAnswer*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = ser_loadAutoConnect((gxAutoConnect*)object, serializer);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = ser_loadDemandRegister((gxDemandRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = ser_loadMacAddressSetup((gxMacAddressSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = ser_loadExtendedRegister((gxExtendedRegister*)object, serializer);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
#ifndef DLMS_IGNORE_GPRS_SETUP
        ret = ser_loadGPRSSetup((gxGPRSSetup*)object, serializer);
#endif //DLMS_IGNORE_GPRS_SETUP
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = ser_loadSecuritySetup((gxSecuritySetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = ser_loadHdlcSetup((gxIecHdlcSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = ser_loadLocalPortSetup((gxLocalPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = ser_loadIecTwistedPairSetup((gxIecTwistedPairSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = ser_loadIp4Setup(settings, (gxIp4Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = ser_loadIp6Setup(settings, (gxIp6Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = ser_loadMbusSlavePortSetup((gxMbusSlavePortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = ser_loadImageTransfer((gxImageTransfer*)object, serializer);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = ser_loadDisconnectControl((gxDisconnectControl*)object, serializer);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = ser_loadLimiter(settings, (gxLimiter*)object, serializer);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = ser_loadMBusClient(settings, (gxMBusClient*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = ser_loadModemConfiguration((gxModemConfiguration*)object, serializer);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = ser_loadPppSetup(settings, (gxPppSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = ser_loadProfileGeneric(settings, (gxProfileGeneric*)object, serializer);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = ser_loadRegisterActivation(settings, (gxRegisterActivation*)object, serializer);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = ser_loadRegisterMonitor(settings, (gxRegisterMonitor*)object, serializer);
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
        ret = ser_loadSapAssignment((gxSapAssignment*)object, serializer);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = ser_loadSchedule((gxSchedule*)object, serializer);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = ser_loadScriptTable(settings, (gxScriptTable*)object, serializer);
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
        ret = ser_loadSpecialDaysTable((gxSpecialDaysTable*)object, serializer);
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
        ret = ser_loadTcpUdpSetup(settings, (gxTcpUdpSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = ser_loadUtilityTables((gxUtilityTables*)object, serializer);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = ser_loadMBusMasterPortSetup((gxMBusMasterPortSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        ret = ser_loadMessageHandler((gxMessageHandler*)object, serializer);
        break;
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = ser_loadPushSetup(settings, (gxPushSetup*)object, serializer);
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
        ret = ser_loadAccount((gxAccount*)object, serializer);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = ser_loadCredit((gxCredit*)object, serializer);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = ser_loadCharge((gxCharge*)object, serializer);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = ser_loadTokenGateway((gxTokenGateway*)object, serializer);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = ser_loadGsmDiagnostic((gxGsmDiagnostic*)object, serializer);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = ser_loadCompactData(settings, (gxCompactData*)object, serializer);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = ser_loadLlcSscsSetup((gxLlcSscsSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = ser_loadPrimeNbOfdmPlcPhysicalLayerCounters((gxPrimeNbOfdmPlcPhysicalLayerCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = ser_loadPrimeNbOfdmPlcMacSetup((gxPrimeNbOfdmPlcMacSetup*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = ser_loadPrimeNbOfdmPlcMacFunctionalParameters((gxPrimeNbOfdmPlcMacFunctionalParameters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = ser_loadPrimeNbOfdmPlcMacCounters((gxPrimeNbOfdmPlcMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = ser_loadPrimeNbOfdmPlcMacNetworkAdministrationData((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = ser_loadPrimeNbOfdmPlcApplicationsIdentification((gxPrimeNbOfdmPlcApplicationsIdentification*)object, serializer);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        break;
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = ser_loadArbitrator((gxArbitrator*)object, serializer);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = ser_loadIec8802LlcType1Setup((gxIec8802LlcType1Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ser_loadIec8802LlcType2Setup((gxIec8802LlcType2Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ser_loadIec8802LlcType3Setup((gxIec8802LlcType3Setup*)object, serializer);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ser_loadSFSKActiveInitiator((gxSFSKActiveInitiator*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ser_loadFSKMacCounters((gxFSKMacCounters*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ser_loadSFSKMacSynchronizationTimeouts((gxSFSKMacSynchronizationTimeouts*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ser_loadSFSKPhyMacSetUp((gxSFSKPhyMacSetUp*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ser_loadSFSKReportingSystemList((gxSFSKReportingSystemList*)object, serializer);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = ser_loadTariffPlan((gxTariffPlan*)object, serializer);
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
