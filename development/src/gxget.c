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

#ifdef DLMS_IGNORE_OBJECT_POINTERS
#define OBJECT_TYPE it->objectType
#define SCRIPT_LOGICAL_NAME action->scriptLogicalName
#else
#define OBJECT_TYPE it->target->objectType
#define SCRIPT_LOGICAL_NAME action->script == NULL ? EMPTY_LN : action->script->logicalName
#endif //DLMS_IGNORE_OBJECT_POINTERS

int cosem_getByteBuffer(gxValueEventArg* e)
{
    if (e->value.vt != DLMS_DATA_TYPE_OCTET_STRING)
    {
        e->value.vt = DLMS_DATA_TYPE_OCTET_STRING;
#ifndef DLMS_IGNORE_MALLOC
        if (e->value.byteArr == NULL)
        {
            e->value.byteArr = (gxByteBuffer*)gxmalloc(sizeof(gxByteBuffer));
            if (e->value.byteArr == NULL)
            {
                return DLMS_ERROR_CODE_OUTOFMEMORY;
            }
            BYTE_BUFFER_INIT(e->value.byteArr);
        }
#endif //DLMS_IGNORE_MALLOC
    }
    e->byteArray = 1;
    return 0;
}

#ifndef DLMS_IGNORE_DATA
int cosem_getData(gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
        //Return plain value if request is coming from action.
        if (e->action)
        {
            ret = var_copy(&e->value, &((gxData*)e->target)->value);
        }
        else
        {
            ret = cosem_setVariant(e->value.byteArr, &((gxData*)e->target)->value);
        }
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
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        //Return plain value if request is coming from action.
        if (e->action)
        {
            ret = var_copy(&e->value, &((gxRegister*)e->target)->value);
        }
        else
        {
            ret = cosem_setVariant(e->value.byteArr, &((gxRegister*)e->target)->value);
        }
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_setInt8(data, ((gxRegister*)e->target)->scaler)) != 0 ||
            (ret = cosem_setEnum(data, ((gxRegister*)e->target)->unit)) != 0)
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
int cosem_getRegisterTable(
    gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
        //TODO:  ret = cosem_setVariant(e->value.byteArr, &((gxRegisterTable*)e->target)->value);
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    else if (e->index == 4)
    {
        gxByteBuffer* data = e->value.byteArr;
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_setInt8(data, ((gxRegisterTable*)e->target)->scaler)) != 0 ||
            (ret = cosem_setEnum(data, ((gxRegisterTable*)e->target)->unit)) != 0)
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
int cosem_getClock(
    gxValueEventArg* e)
{
    int ret;
    if (e->index == 2)
    {
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &((gxClock*)e->target)->time);
    }
    else if (e->index == 3)
    {
        ret = cosem_setInt16(e->value.byteArr, ((gxClock*)e->target)->timeZone);
    }
    else if (e->index == 4)
    {
        ret = cosem_setEnum(e->value.byteArr, ((gxClock*)e->target)->status);
    }
    else if (e->index == 5)
    {
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &((gxClock*)e->target)->begin);
    }
    else if (e->index == 6)
    {
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &((gxClock*)e->target)->end);
    }
    else if (e->index == 7)
    {
        ret = cosem_setInt8(e->value.byteArr, ((gxClock*)e->target)->deviation);
    }
    else if (e->index == 8)
    {
        ret = cosem_setBoolean(e->value.byteArr, ((gxClock*)e->target)->enabled);
    }
    else if (e->index == 9)
    {
        ret = cosem_setEnum(e->value.byteArr, ((gxClock*)e->target)->clockBase);
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
    int ret = DLMS_ERROR_CODE_OK;
    uint16_t pos;
    gxtime* tm;
    gxActionSchedule* object = (gxActionSchedule*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            //Add LN.
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = hlp_setObjectCount(6, data)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            (ret = bb_set(data, obj_getLogicalName((gxObject*)object->executedScript), 6)) != 0 ||
#else
            (ret = bb_set(data, object->executedScriptLogicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
            //Add executed script selector.
            (ret = cosem_setUInt16(data, object->executedScriptSelector)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(e->value.byteArr, object->type);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_setArray(data, object->executionTime.size)) == 0)
        {
            for (pos = 0; pos != object->executionTime.size; ++pos)
            {
                if ((ret = cosem_setStructure(data, 2)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm, sizeof(gxtime))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = cosem_setTimeAsOctetString(data, tm)) != 0 ||
                    (ret = cosem_setDateAsOctetString(data, tm)) != 0)
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
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int getActivityCalendarDayProfileTable(gxArray* list, gxByteBuffer* ba)
{
    gxDayProfile* dp;
    gxDayProfileAction* action;
    int ret;
    uint16_t pos, pos2;
    if ((ret = cosem_setArray(ba, list->size)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != list->size; ++pos)
    {
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(list, pos, (void**)&dp)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(list, pos, (void**)&dp, sizeof(gxDayProfile))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cosem_setStructure(ba, 2)) != 0 ||
            (ret = cosem_setUInt8(ba, dp->dayId)) != 0 ||
            //Add count
            (ret = cosem_setArray(ba, dp->daySchedules.size)) != 0)
        {
            break;
        }
        for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2)
        {
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
            if ((ret = cosem_setStructure(ba, 3)) != 0 ||
                (ret = cosem_setTimeAsOctetString(ba, &action->startTime)) != 0 ||
                (ret = cosem_setOctetString2(ba, SCRIPT_LOGICAL_NAME, 6)) != 0 ||
                (ret = cosem_setUInt16(ba, action->scriptSelector)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int getActivityCalendarWeekProfileTable(gxArray* list, gxByteBuffer* ba)
{
    int ret = 0;
    uint16_t pos;
    gxWeekProfile* wp;
    //Add count
    if ((ret = cosem_setArray(ba, list->size)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != list->size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = cosem_setStructure(ba, 8)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&wp, sizeof(gxWeekProfile))) != 0 ||
            (ret = cosem_setOctetString2(ba, wp->name.value, wp->name.size)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->monday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->tuesday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->wednesday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->thursday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->friday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->saturday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->sunday)) != 0)
        {
            break;
        }
#else
        if ((ret = cosem_setStructure(ba, 8)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&wp)) != 0 ||
            (ret = cosem_setOctetString(ba, &wp->name)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->monday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->tuesday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->wednesday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->thursday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->friday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->saturday)) != 0 ||
            (ret = cosem_setUInt8(ba, wp->sunday)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int getActivityCalendarSeasonProfile(gxArray* list, gxByteBuffer* ba)
{
    int ret = 0;
    uint16_t pos;
    gxSeasonProfile* sp;
    //Add count
    if ((ret = cosem_setArray(ba, list->size)) != 0)
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
        if ((ret = cosem_setStructure(ba, 3)) != 0 ||
            //Add name.
            (ret = cosem_setOctetString2(ba, sp->name.value, sp->name.size)) != 0 ||
            //Add start time.
            (ret = cosem_setDateTimeAsOctetString(ba, &sp->start)) != 0 ||
            //Add week day.
            (ret = cosem_setOctetString2(ba, sp->weekName.value, sp->weekName.size)) != 0)
        {
            break;
        }
#else
        if ((ret = cosem_setStructure(ba, 3)) != 0 ||
            (ret = arr_getByIndex(list, pos, (void**)&sp)) != 0 ||
            //Add name.
            (ret = cosem_setOctetString(ba, &sp->name)) != 0 ||
            //Add start time.
            (ret = cosem_setDateTimeAsOctetString(ba, &sp->start)) != 0 ||
            //Add week day.
            (ret = cosem_setOctetString(ba, &sp->weekName)) != 0)
        {
            break;
        }

#endif //DLMS_IGNORE_MALLOC
    }
    return ret;
}

int getActivityCalendarName(
    gxByteBuffer* name,
    gxByteBuffer* ba)
{
    return cosem_setOctetString(ba, name);
}

int cosem_getActivityCalendar(
    gxValueEventArg* e)
{
    int ret;
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
        ret = cosem_setDateTimeAsOctetString(data, &object->time);
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
    cnt = obj_attributeCount(object);
    if ((ret = cosem_setStructure(data, 2)) != 0 ||
        (ret = cosem_setArray(data, cnt)) != 0)
    {
        return ret;
    }
    for (pos = 1; pos != cnt + 1; ++pos)
    {
        ch = (unsigned char)svr_getAttributeAccess(settings, object, pos);
        //attribute_access_item
        if ((ret = cosem_setStructure(data, 3)) != 0 ||
            //Add index.
            (ret = cosem_setInt8(data, pos)) != 0 ||
            //Add value.
            (ret = cosem_setEnum(data, ch)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_NONE)) != 0)
        {
            return ret;
        }
    }
#ifdef INDIAN_STANDARD
    if ((ret = cosem_setArray(data, 0)) != 0)
    {
        return ret;
    }
#else
    cnt = obj_methodCount(object);
    if ((ret = cosem_setArray(data, cnt)) != 0)
    {
        return ret;
    }
    for (pos = 1; pos != cnt + 1; ++pos)
    {
        ch = (unsigned char)svr_getMethodAccess(settings, object, pos);
        //attribute_access_item
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            //Add index.
            (ret = cosem_setInt8(data, pos)) != 0 ||
            //Add value.
            (ret = cosem_setEnum(data, ch)) != 0)
        {
            return ret;
        }
    }
#endif //INDIAN_STANDARD
    return 0;
}
#endif //DLMS_IGNORE_SERVER

#ifndef DLMS_IGNORE_SERVER

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
// Returns LN Association View.
int getLNObjects(
    dlmsSettings* settings,
    gxValueEventArg* e,
    gxByteBuffer* data)
{
    uint16_t pduSize = 0;
    gxAssociationLogicalName* object = (gxAssociationLogicalName*)e->target;
    int ret;
    uint16_t pos;
    unsigned char found = 0;
    gxObject* it, * it2;
    unsigned char ln[] = { 0, 0, 40, 0, 0, 255 };
    if ((ret = bb_capacity(data, settings->maxPduSize)) != 0)
    {
        return ret;
    }
    //Add count only for first time.
    if (!e->transaction)
    {
        uint16_t count = object->objectList.size;
        //Find current association and if nout found add it.
        for (pos = 0; pos != object->objectList.size; ++pos)
        {
            if ((ret = oa_getByIndex(&object->objectList, pos, &it)) != 0)
            {
                return ret;
            }
            if (e->target != it && it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
            {
                if (memcmp(ln, it->logicalName, 6) == 0)
                {
                    found = 1;
                }
                else
                {
                    //Remove extra association view.
                    --count;
                }
            }
        }
        //Remove objects that are only internal use.
        for (pos = 0; pos != settings->internalObjects.size; ++pos)
        {
            if ((ret = oa_getByIndex(&settings->internalObjects, pos, &it)) != 0)
            {
                return ret;
            }
            oa_findByLN(&object->objectList, it->objectType, it->logicalName, &it2);
            if (it2 != NULL)
            {
                --count;
            }
        }
        if (!found)
        {
            ++count;
        }
        e->transactionEndIndex = count;
        if ((ret = cosem_setArray(data, count)) != 0)
        {
            return ret;
        }
        if (!found)
        {
            //Count
            if ((ret = cosem_setStructure(data, 4)) != 0 ||
                //ClassID
                (ret = cosem_setUInt16(data, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)) != 0 ||
                //Version
                (ret = cosem_setUInt8(data, object->base.version)) != 0 ||
                //LN.
                (ret = cosem_setOctetString2(data, ln, 6)) != 0)
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
    uint16_t pos2 = 0;
    for (pos = 0; pos != object->objectList.size; ++pos)
    {
        ret = oa_getByIndex(&object->objectList, pos, &it);
        if (ret != 0)
        {
            return ret;
        }
        //Remove objects that are only for internal use.
        if ((ret = oa_findByLN(&settings->internalObjects, it->objectType, it->logicalName, &it2)) != 0)
        {
            break;
        }
        //If this is not ignored.
        if (it2 == NULL)
        {
            if (it->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
            {
                if (!(it == e->target || memcmp(ln, it->logicalName, 6) == 0))
                {
                    continue;
                }
            }
            if (!(pos2 < (uint16_t)e->transactionStartIndex))
            {
                pduSize = (uint16_t)data->size;
                //Count
                if ((ret = cosem_setStructure(data, 4)) != 0 ||
                    //ClassID
                    (ret = cosem_setUInt16(data, it->objectType)) != 0 ||
                    //Version
                    (ret = cosem_setUInt8(data, (unsigned char)it->version)) != 0 ||
                    //LN.
                    (ret = cosem_setOctetString2(data, it->logicalName, 6)) != 0)
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
            ++pos2;
        }
    }
    if (ret == DLMS_ERROR_CODE_OUTOFMEMORY)
    {
        data->size = pduSize;
        ret = 0;
    }
    else if (pos == object->objectList.size)
    {
        //If all objects fit to one PDU.
        e->transactionEndIndex = e->transactionStartIndex = 0;
    }
    return ret;
}
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
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
        if ((ret = cosem_setArray(data, target->userList.size)) != 0)
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
        // Count
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            // Id
            (ret = cosem_setUInt8(data, (unsigned char)id)) != 0 ||
            // Name
            (ret = cosem_setOctetString2(data, (unsigned char*)name, len)) != 0)
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
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
#ifndef DLMS_IGNORE_SERVER
        ret = getLNObjects(settings, e, data);
#endif //DLMS_IGNORE_SERVER
    }
    else if (e->index == 3)
    {
        //Add count
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_setInt8(data, object->clientSAP)) != 0 ||
            (ret = cosem_setUInt16(data, object->serverSAP)) != 0)
        {
        }
    }
    else if (e->index == 4)
    {
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
        }
    }
    else if (e->index == 5)
    {
        unsigned char buff[4];
        gxByteBuffer tmp;
        bb_attach(&tmp, buff, 0, sizeof(buff));
        //Add count
        if ((ret = cosem_setStructure(data, 6)) != 0 ||
            //Add data.
            (ret = cosem_setBitString(data, object->xDLMSContextInfo.conformance, 24)) != 0 ||
            (ret = cosem_setUInt16(data, object->xDLMSContextInfo.maxReceivePduSize)) != 0 ||
            (ret = cosem_setUInt16(data, object->xDLMSContextInfo.maxSendPduSize)) != 0 ||
            (ret = cosem_setUInt8(data, object->xDLMSContextInfo.dlmsVersionNumber)) != 0 ||
            (ret = cosem_setInt8(data, object->xDLMSContextInfo.qualityOfService)) != 0 ||
            (ret = cosem_setOctetString(data, &object->xDLMSContextInfo.cypheringInfo)) != 0)
        {
        }
    }
    else if (e->index == 6)
    {
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
    else if (e->index == 7)
    {
        ret = cosem_setOctetString(e->value.byteArr, &object->secret);
    }
    else if (e->index == 8)
    {
        ret = cosem_setEnum(e->value.byteArr, object->associationStatus);
    }
    else if (e->index == 9)
    {
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        ret = cosem_setOctetString2(e->value.byteArr, obj_getLogicalName((gxObject*)object->securitySetup), 6);
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->securitySetupReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (e->index == 10)
    {
        ret = getUserList(settings, e, e->value.byteArr);
    }
    else if (e->index == 11)
    {
        // Add structure size.
        if ((ret = cosem_setStructure(data, 2)) != 0)
        {
            return ret;
        }
#ifdef DLMS_IGNORE_MALLOC
        char* str = (char*)object->currentUser.name;
        unsigned char len = (unsigned char)strlen(str);
        if ((ret = cosem_setUInt8(data, object->currentUser.id)) != 0 ||
            (ret = cosem_setOctetString2(data, (unsigned char*)str, len)) != 0)
        {
            return ret;
        }
#else
        if (object->currentUser.value == NULL)
        {
            if ((ret = cosem_setUInt8(data, 0)) != 0 ||
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
            if ((ret = cosem_setUInt8(data, object->currentUser.key)) != 0 ||
                (ret = cosem_setOctetString2(data, (unsigned char*)str, len)) != 0)
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
    uint16_t pduSize = 0;
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
        //Add count
        if ((ret = cosem_setArray(ba, list->size)) != 0)
        {
            return ret;
        }
    }
    for (pos = (uint16_t)e->transactionStartIndex; pos != list->size; ++pos)
    {
        pduSize = (uint16_t)ba->size;
        if ((ret = oa_getByIndex(list, pos, &it)) != 0 ||
            //Count
            (ret = cosem_setStructure(ba, 4)) != 0 ||
            //Base address.
            (ret = cosem_setInt16(ba, it->shortName)) != 0 ||
            //ClassID.
            (ret = cosem_setUInt16(ba, it->objectType)) != 0 ||
            //Version.
            (ret = cosem_setUInt8(ba, (unsigned char)it->version)) != 0 ||
            //LN.
            (ret = cosem_setOctetString2(ba, it->logicalName, 6)) != 0)
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
    cnt = obj_attributeCount(object);
    if ((ret = cosem_setStructure(ba, 3)) != 0 ||
        //Add SN.
        (ret = cosem_setUInt16(ba, object->shortName)) != 0 ||
        //Add attribute access modes.
        (ret = cosem_setArray(ba, cnt)) != 0)
    {
        return ret;
    }
    for (pos = 1; pos != cnt + 1; ++pos)
    {
        ch = (unsigned char)svr_getAttributeAccess(settings, object, pos);
        if ((ret = cosem_setStructure(ba, 3)) != 0 ||
            //Add index.
            (ret = cosem_setInt8(ba, pos)) != 0 ||
            //Add access.
            (ret = cosem_setEnum(ba, ch)) != 0 ||
            //Add None.
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_NONE)) != 0)
        {
            return ret;
        }
    }
    //Add mthod access modes.
    cnt = obj_methodCount(object);
    if ((ret = cosem_setArray(ba, cnt)) == 0)
    {
        for (pos = 1; pos != cnt + 1; ++pos)
        {
            ch = (unsigned char)svr_getMethodAccess(settings, object, pos);
            if ((ret = cosem_setStructure(ba, 3)) != 0 ||
                //Add index.
                (ret = cosem_setInt8(ba, pos)) != 0 ||
                //Add access.
                (ret = cosem_setEnum(ba, ch)) != 0)
            {
                return ret;
            }
        }
    }
    return ret;
}

int cosem_getAssociationShortName(
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    uint16_t pos;
    gxObject* it;
    gxAssociationShortName* object = (gxAssociationShortName*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = getSNObjects(settings, e, data);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_setArray(data, object->objectList.size)) == 0)
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
        ret = cosem_setOctetString2(e->value.byteArr, obj_getLogicalName((gxObject*)object->securitySetup), 6);
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->securitySetupReference, 6);
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
    int ret = 0;
    uint16_t pos;
    gxAutoAnswer* object = (gxAutoAnswer*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setEnum(data, object->mode);
    }
    else if (e->index == 3)
    {
#ifndef DLMS_IGNORE_MALLOC
        gxKey* kv;
#else
        gxTimePair* kv;
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cosem_setArray(data, object->listeningWindow.size)) == 0)
        {
            for (pos = 0; pos != object->listeningWindow.size; ++pos)
            {
                if ((ret = cosem_setStructure(data, 2)) != 0)
                {
                    break;
                }
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&kv)) != 0 ||
                    (ret = cosem_setDateTimeAsOctetString(data, (gxtime*)kv->key)) != 0 ||
                    (ret = cosem_setDateTimeAsOctetString(data, (gxtime*)kv->value)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&kv, sizeof(gxTimePair))) != 0 ||
                    //start_time
                    (ret = cosem_setDateTimeAsOctetString(data, &kv->first)) != 0 ||
                    //end_time
                    (ret = cosem_setDateTimeAsOctetString(data, &kv->second)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    else if (e->index == 4)
    {
        ret = cosem_setEnum(data, object->status);
    }
    else if (e->index == 5)
    {
        ret = cosem_setUInt8(data, object->numberOfCalls);
    }
    else if (e->index == 6)
    {
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_setUInt8(data, object->numberOfRingsInListeningWindow)) != 0 ||
            (ret = cosem_setUInt8(data, object->numberOfRingsOutListeningWindow)) != 0)
        {
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
int cosem_getAutoConnect(
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    uint16_t pos;
#ifndef DLMS_IGNORE_MALLOC
    gxKey* k;
    gxByteBuffer* it;
#else
    gxTimePair* k;
    gxDestination* it;
#endif //DLMS_IGNORE_MALLOC
    gxAutoConnect* object = (gxAutoConnect*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setEnum(data, object->mode);
    }
    else if (e->index == 3)
    {
        ret = cosem_setUInt8(data, object->repetitions);
    }
    else if (e->index == 4)
    {
        ret = cosem_setUInt16(data, object->repetitionDelay);
    }
    else if (e->index == 5)
    {
        //Add count
        if ((ret = cosem_setArray(data, object->callingWindow.size)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->callingWindow.size; ++pos)
        {
            if ((ret = cosem_setStructure(data, 2)) != 0)
            {
                break;
            }
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k)) != 0 ||
                (ret = cosem_setDateTimeAsOctetString(data, (gxtime*)k->key)) != 0 ||
                (ret = cosem_setDateTimeAsOctetString(data, (gxtime*)k->value)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k, sizeof(gxTimePair))) != 0)
            {
                break;
            }
            if ((ret = cosem_setDateTimeAsOctetString(data, &k->first)) != 0 ||
                (ret = cosem_setDateTimeAsOctetString(data, &k->second)) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC
        }
    }
    else if (e->index == 6)
    {
        //Add count
        if ((ret = cosem_setArray(data, object->destinations.size)) != 0)
        {
            return ret;
        }
        for (pos = 0; pos != object->destinations.size; ++pos)
        {
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->destinations, pos, (void**)&it)) != 0 ||
                (ret = cosem_setOctetString(data, it)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->destinations, pos, (void**)&it, sizeof(gxDestination))) != 0 ||
                (ret = cosem_setOctetString2(data, it->value, it->size)) != 0)
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
    int ret;
    gxByteBuffer* data = e->value.byteArr;
    gxDemandRegister* object = (gxDemandRegister*)e->target;
    switch (e->index)
    {
    case 2:
        //Return plain value if request is coming from action.
        if (e->action)
        {
            ret = var_copy(&e->value, &object->currentAverageValue);
        }
        else
        {
            ret = cosem_setVariant(data, &object->currentAverageValue);
        }
        break;
    case 3:
        //Return plain value if request is coming from action.
        if (e->action)
        {
            ret = var_copy(&e->value, &object->lastAverageValue);
        }
        else
        {
            ret = cosem_setVariant(data, &object->lastAverageValue);
        }
        break;
    case 4:
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_setInt8(data, object->scaler)) != 0 ||
            (ret = cosem_setEnum(data, object->unit)) != 0)
        {
        }
        break;
    case 5:
        //Return plain value if request is coming from action.
        if (e->action)
        {
            ret = var_copy(&e->value, &object->status);
        }
        else
        {
            ret = cosem_setVariant(data, &object->status);
        }
        break;
    case 6:
        ret = cosem_setDateTimeAsOctetString(data, &object->captureTime);
        break;
    case 7:
        ret = cosem_setDateTimeAsOctetString(data, &object->startTimeCurrent);
        break;
    case 8:
        ret = cosem_setUInt32(data, object->period);
        break;
    case 9:
        ret = cosem_setUInt16(data, object->numberOfPeriods);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int cosem_getMacAddressSetup(
    gxValueEventArg* e)
{
    int ret = 0;
    if (e->index == 2)
    {
        gxMacAddressSetup* object = (gxMacAddressSetup*)e->target;
        ret = cosem_setOctetString(e->value.byteArr, &object->macAddress);
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
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        //Return plain value if request is coming from action.
        if (e->action)
        {
            ret = var_copy(&e->value, &object->value);
        }
        else
        {
            ret = cosem_setVariant(data, &object->value);
        }
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_setInt8(data, object->scaler)) != 0 ||
            (ret = cosem_setUInt8(data, object->unit)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 4)
    {
        //Return plain value if request is coming from action.
        if (e->action)
        {
            ret = var_copy(&e->value, &object->status);
        }
        else
        {
            ret = cosem_setVariant(data, &object->status);
        }
    }
    else if (e->index == 5)
    {
        ret = cosem_setDateTimeAsOctetString(data, &object->captureTime);
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
    if ((ret = cosem_setStructure(ba, 5)) != 0 ||
        //Precedence
        (ret = cosem_setUInt8(ba, service->precedence)) != 0 ||
        //Delay
        (ret = cosem_setUInt8(ba, service->delay)) != 0 ||
        //Reliability
        (ret = cosem_setUInt8(ba, service->reliability)) != 0 ||
        //PeakThroughput
        (ret = cosem_setUInt8(ba, service->peakThroughput)) != 0 ||
        //MeanThroughput
        (ret = cosem_setUInt8(ba, service->meanThroughput)) != 0)
    {
    }
    return ret;
}

int cosem_getGprsSetup(
    gxValueEventArg* e)
{
    int ret;
    gxGPRSSetup* object = (gxGPRSSetup*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setOctetString(data, &object->apn);
    }
    else if (e->index == 3)
    {
        ret = cosem_setUInt16(data, object->pinCode);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_getQualityOfService(&object->defaultQualityOfService, data)) != 0 ||
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
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setEnum(data, object->securityPolicy);
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(data, object->securitySuite);
    }
    else if (e->index == 4)
    {
        ret = cosem_setOctetString(data, &object->clientSystemTitle);
    }
    else if (e->index == 5)
    {
        ret = cosem_setOctetString(data, &object->serverSystemTitle);
    }
    else if (e->index == 6)
    {
        gxCertificateInfo* it;
        uint16_t pos, len;
        if ((ret = cosem_setArray(data, object->certificates.size)) == 0)
        {
            for (pos = 0; pos != object->certificates.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->certificates, pos, (void**)&it, sizeof(gxCertificateInfo))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->certificates, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setStructure(data, 6)) != 0 ||
                    (ret = cosem_setEnum(data, it->entity)) != 0 ||
                    (ret = cosem_setEnum(data, it->type)) != 0)
                {
                    break;
                }
                len = (uint16_t)strlen(it->serialNumber);
                if ((ret = cosem_setOctetString2(data, (unsigned char*)it->serialNumber, len)) != 0)
                {
                    break;
                }
                len = (uint16_t)strlen(it->issuer);
                if ((ret = cosem_setOctetString2(data, (unsigned char*)it->issuer, len)) != 0)
                {
                    break;
                }
                len = (uint16_t)strlen(it->subject);
                if ((ret = cosem_setOctetString2(data, (unsigned char*)it->subject, len)) != 0)
                {
                    break;
                }
                len = (uint16_t)strlen(it->subjectAltName);
                if ((ret = cosem_setOctetString2(data, (unsigned char*)it->subjectAltName, len)) != 0)
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
        ret = cosem_setEnum(e->value.byteArr, object->communicationSpeed);
        break;
    case 3:
        ret = cosem_setUInt8(e->value.byteArr, object->windowSizeTransmit);
        break;
    case 4:
        ret = cosem_setUInt8(e->value.byteArr, object->windowSizeReceive);
        break;
    case 5:
        ret = cosem_setUInt16(e->value.byteArr, object->maximumInfoLengthTransmit);
        break;
    case 6:
        ret = cosem_setUInt16(e->value.byteArr, object->maximumInfoLengthReceive);
        break;
    case 7:
        ret = cosem_setUInt16(e->value.byteArr, object->interCharachterTimeout);
        break;
    case 8:
        ret = cosem_setUInt16(e->value.byteArr, object->inactivityTimeout);
        break;
    case 9:
        ret = cosem_setUInt16(e->value.byteArr, object->deviceAddress);
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
        ret = cosem_setEnum(e->value.byteArr, object->defaultMode);
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(e->value.byteArr, object->defaultBaudrate);
    }
    else if (e->index == 4)
    {
        ret = cosem_setEnum(e->value.byteArr, object->proposedBaudrate);
    }
    else if (e->index == 5)
    {
        ret = cosem_setEnum(e->value.byteArr, object->responseTime);
    }
    else if (e->index == 6)
    {
        ret = cosem_setOctetString(e->value.byteArr, &object->deviceAddress);
    }
    else if (e->index == 7)
    {
        ret = cosem_setOctetString(e->value.byteArr, &object->password1);
    }
    else if (e->index == 8)
    {
        ret = cosem_setOctetString(e->value.byteArr, &object->password2);
    }
    else if (e->index == 9)
    {
        ret = cosem_setOctetString(e->value.byteArr, &object->password5);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif // DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int cosem_getIecTwistedPairSetup(
    gxValueEventArg* e)
{
    unsigned char ch;
    int pos, ret = 0;
    gxIecTwistedPairSetup* object = (gxIecTwistedPairSetup*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setEnum(e->value.byteArr, object->mode);
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(e->value.byteArr, object->speed);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_setArray(data, (unsigned short)object->primaryAddresses.size)) == 0)
        {
            for (pos = 0; pos != object->primaryAddresses.size; ++pos)
            {
                if ((ret = bb_getUInt8ByIndex(&object->primaryAddresses, pos, &ch)) != 0 ||
                    (ret = cosem_setUInt8(data, ch)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_setArray(data, (unsigned short)object->tabis.size)) == 0)
        {
            for (pos = 0; pos != object->tabis.size; ++pos)
            {
                if ((ret = bb_getUInt8ByIndex(&object->tabis, pos, &ch)) != 0 ||
                    (ret = cosem_setInt8(data, (char)ch)) != 0)
                {
                    break;
                }
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_IP4_SETUP
int cosem_getIP4Setup(
    gxValueEventArg* e)
{
    int ret = 0;
    uint16_t pos;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    dlmsVARIANT* tmp;
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxip4SetupIpOption* it;
    gxIp4Setup* object = (gxIp4Setup*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->dataLinkLayer == NULL)
        {
            ret = cosem_setOctetString2(data, EMPTY_LN, 6);
        }
        else
        {
            ret = cosem_setOctetString2(data, object->dataLinkLayer->logicalName, 6);
            //Error code is returned at the end of the function.
        }
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->dataLinkLayerReference, 6);
        {
            //Error code is returned at the end of the function.
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (e->index == 3)
    {
        ret = cosem_setUInt32(data, object->ipAddress);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_setArray(data, object->multicastIPAddress.size)) == 0)
        {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            uint32_t* v;
            for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
            {
                if ((ret = arr_getByIndex2(&object->multicastIPAddress, pos, (void**)&v, sizeof(uint32_t))) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
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
    }
    else if (e->index == 5)
    {
        if ((ret = cosem_setArray(data, object->ipOptions.size)) == 0)
        {
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
    }
    else if (e->index == 6)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->subnetMask);
    }
    else if (e->index == 7)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->gatewayIPAddress);
    }
    else if (e->index == 8)
    {
        ret = cosem_setBoolean(e->value.byteArr, object->useDHCP);
    }
    else if (e->index == 9)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->primaryDNSAddress);
    }
    else if (e->index == 10)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->secondaryDNSAddress);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP

int getIpv6Address(IN6_ADDR* address, gxValueEventArg* e)
{
    unsigned char* tmp;
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    tmp = address->u.Byte;
#else //Linux includes.
    tmp = address->s6_addr;
#endif
    return cosem_setOctetString2(e->value.byteArr, tmp, 16);
}

int getAddressList(gxArray* list, gxValueEventArg* e)
{
    IN6_ADDR* it;
    int ret = 0;
    uint16_t pos;
    if ((ret = cosem_setArray(e->value.byteArr, list->size)) == 0)
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
            if ((ret = getIpv6Address(it, e)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int cosem_getIP6Setup(
    gxValueEventArg* e)
{
    int ret = 0;
    gxIp6Setup* object = (gxIp6Setup*)e->target;
    switch (e->index)
    {
    case 2:
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->dataLinkLayer == NULL)
        {
            ret = cosem_setOctetString2(e->value.byteArr, EMPTY_LN, 6);
        }
        else
        {
            ret = cosem_setOctetString2(e->value.byteArr, object->dataLinkLayer->logicalName, 6);
            //Error code is returned at the end of the function.
        }
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->dataLinkLayerReference, 6);
        {
            //Error code is returned at the end of the function.
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    break;
    case 3:
        ret = cosem_setEnum(e->value.byteArr, object->addressConfigMode);
        break;
    case 4:
        ret = getAddressList(&object->unicastIPAddress, e);
        break;
    case 5:
        ret = getAddressList(&object->multicastIPAddress, e);
        break;
    case 6:
        ret = getAddressList(&object->gatewayIPAddress, e);
        break;
    case 7:
        ret = getIpv6Address(&object->primaryDNSAddress, e);
        break;
    case 8:
        ret = getIpv6Address(&object->secondaryDNSAddress, e);
        break;
    case 9:
        ret = cosem_setUInt8(e->value.byteArr, object->trafficClass);
        break;
    case 10:
    {
        gxNeighborDiscoverySetup* it;
        uint16_t pos;
        if ((ret = cosem_setArray(e->value.byteArr, object->neighborDiscoverySetup.size)) == 0)
        {
            for (pos = 0; pos != object->neighborDiscoverySetup.size; ++pos)
            {
                if ((ret = arr_getByIndex2(&object->neighborDiscoverySetup, pos, (void**)&it, sizeof(gxNeighborDiscoverySetup))) != 0)
                {
                    break;
                }
                if ((ret = cosem_setStructure(e->value.byteArr, 3)) != 0 ||
                    (ret = cosem_setUInt8(e->value.byteArr, it->maxRetry)) != 0 ||
                    (ret = cosem_setUInt16(e->value.byteArr, it->retryWaitTime)) != 0 ||
                    (ret = cosem_setUInt32(e->value.byteArr, it->sendPeriod)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int cosem_getMbusSlavePortSetup(
    gxValueEventArg* e)
{
    int ret;
    gxMbusSlavePortSetup* object = (gxMbusSlavePortSetup*)e->target;
    if (e->index == 2)
    {
        ret = cosem_setEnum(e->value.byteArr, object->defaultBaud);
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(e->value.byteArr, object->availableBaud);
    }
    else if (e->index == 4)
    {
        ret = cosem_setEnum(e->value.byteArr, object->addressState);
    }
    else if (e->index == 5)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->busAddress);
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
    int ret = 0;
    uint16_t pos;
    gxImageActivateInfo* ai;
    gxImageTransfer* object = (gxImageTransfer*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->imageBlockSize);
    }
    else if (e->index == 3)
    {
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
        ret = cosem_setUInt32(e->value.byteArr, object->imageFirstNotTransferredBlockNumber);
    }
    else if (e->index == 5)
    {
        ret = cosem_setBoolean(e->value.byteArr, object->imageTransferEnabled);
    }
    else if (e->index == 6)
    {
        ret = cosem_setEnum(e->value.byteArr, object->imageTransferStatus);
    }
    else if (e->index == 7)
    {
        //ImageActivateInfo is returned only after verification is succeeded.
        if (object->imageTransferStatus != DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL)
        {
            if ((ret = cosem_setArray(data, 0)) != 0)
            {
                return ret;
            }
        }
        else
        {
            if ((ret = cosem_setArray(data, object->imageActivateInfo.size)) == 0)
            {
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
    uint16_t pduSize = 0;
    int ret = 0;
    uint32_t pos;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
#else
    gxKey* it;
    if ((ret = bb_capacity(ba, (list->size * 19) + 2)) != 0)
    {
        return ret;
    }
#endif //DLMS_IGNORE_MALLOC
    //Add count only for first time.
    if (!e->transaction)
    {
        e->transactionEndIndex = list->size;
        if ((ret = cosem_setArray(ba, list->size)) != 0)
        {
            return ret;
        }
    }
    for (pos = e->transactionStartIndex; pos != list->size; ++pos)
    {
        pduSize = (uint16_t)ba->size;
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(list, (uint16_t)pos, (void**)&it, sizeof(gxTarget))) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
        if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
        {
            break;
        }
#endif //DLMS_COSEM_EXACT_DATA_TYPES
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = bb_setUInt8(ba, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(ba, 4)) != 0 ||
            //ClassID
            (ret = bb_setUInt8(ba, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(ba, OBJECT_TYPE)) != 0 ||
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
        if ((ret = arr_getByIndex(list, (uint16_t)pos, (void**)&it)) != 0 ||
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
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
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
    if ((ret = arr_getByIndex(table, (uint16_t)index, (void**)&row)) != 0)
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
    uint16_t pduSize = 0;
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
                    if ((ret = cosem_setArray(e->value.byteArr, cnt)) != 0 ||
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
                e->transactionStartIndex = (uint16_t)var_toInteger(it);
                if (e->transactionStartIndex == 0)
                {
                    e->transactionStartIndex = 1;
                }
                if ((ret = va_getByIndex(e->parameters.Arr, 1, &it)) == 0)
                {
                    e->transactionEndIndex = (uint16_t)var_toInteger(it);
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
        ret = getProfileGenericData(settings, object, e, e->value.byteArr);
#else
        ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 3)
    {
        ret = getColumns(settings, &object->captureObjects, e->value.byteArr, e);
    }
    else if (e->index == 4)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->capturePeriod);
    }
    else if (e->index == 5)
    {
        ret = cosem_setEnum(e->value.byteArr, object->sortMethod);
    }
    else  if (e->index == 6)
    {
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
        ret = cosem_setUInt32(e->value.byteArr, object->entriesInUse);
    }
    else if (e->index == 8)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->profileEntries);
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
        ret = cosem_setBoolean(e->value.byteArr, object->outputState);
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(e->value.byteArr, object->controlState);
    }
    else if (e->index == 4)
    {
        ret = cosem_setEnum(e->value.byteArr, object->controlMode);
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
    gxByteBuffer* data = e->value.byteArr;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    uint16_t* it;
#else
    dlmsVARIANT* it;
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    if (e->index == 2)
    {
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
        ret = cosem_setVariant(e->value.byteArr, &object->thresholdActive);
    }
    else if (e->index == 4)
    {
        ret = cosem_setVariant(e->value.byteArr, &object->thresholdNormal);
    }
    else if (e->index == 5)
    {
        ret = cosem_setVariant(e->value.byteArr, &object->thresholdEmergency);
    }
    else if (e->index == 6)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->minOverThresholdDuration);
    }
    else if (e->index == 7)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->minUnderThresholdDuration);
    }
    else if (e->index == 8)
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 3)) != 0 ||
            //Id
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
            (ret = bb_setUInt16(data, object->emergencyProfile.id)) != 0 ||
            //Datetime
            (ret = cosem_setDateTimeAsOctetString(data, &object->emergencyProfile.activationTime)) != 0 ||
            //Duration
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT32)) != 0 ||
            (ret = bb_setUInt32(data, object->emergencyProfile.duration)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_setArray(data, object->emergencyProfileGroupIDs.size)) == 0)
        {
            for (pos = 0; pos != object->emergencyProfileGroupIDs.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
                if ((ret = arr_getByIndex2(&object->emergencyProfileGroupIDs, pos, (void**)&it, sizeof(uint16_t))) != 0 ||
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
    }
    else if (e->index == 10)
    {
        ret = cosem_setBoolean(e->value.byteArr, object->emergencyProfileActive);
    }
    else if (e->index == 11)
    {
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
    int ret = 0;
    uint16_t pos;
    gxMBusClient* object = (gxMBusClient*)e->target;
    gxByteBuffer* data = e->value.byteArr;
#ifdef DLMS_IGNORE_MALLOC
    gxCaptureDefinition* it;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    if (e->index == 2)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->mBusPort == NULL)
        {
            ret = cosem_setOctetString2(e->value.byteArr, EMPTY_LN, 6);
        }
        else
        {
            ret = cosem_setOctetString2(e->value.byteArr, object->mBusPort->logicalName, 6);
        }
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->mBusPortReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_setArray(data, object->captureDefinition.size)) == 0)
        {
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
    }
    else if (e->index == 4)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->capturePeriod);
    }
    else if (e->index == 5)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->primaryAddress);
    }
    else if (e->index == 6)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->identificationNumber);
    }
    else if (e->index == 7)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->manufacturerID);
    }
    else if (e->index == 8)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->dataHeaderVersion);
    }
    else if (e->index == 9)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->deviceType);
    }
    else if (e->index == 10)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->accessNumber);
    }
    else if (e->index == 11)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->status);
    }
    else if (e->index == 12)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->alarm);
    }
    else if (e->index == 13 && object->base.version != 0)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->configuration);
    }
    else if (e->index == 14 && object->base.version != 0)
    {
        ret = cosem_setEnum(e->value.byteArr, object->encryptionKeyStatus);
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
    int ret = 0;
    uint16_t pos;
    gxModemInitialisation* mi;
#ifdef DLMS_IGNORE_MALLOC
    gxModemProfile* ba;
#else
    gxByteBuffer* ba;
#endif //DLMS_IGNORE_MALLOC
    gxModemConfiguration* target = ((gxModemConfiguration*)e->target);
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setEnum(e->value.byteArr, target->communicationSpeed);
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_setArray(data, target->initialisationStrings.size)) == 0)
        {
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
    }
    else if (e->index == 4)
    {
        //Modem profile is defined on DLMS standard. Add default values.
        if (settings->server && target->modemProfile.size == 0)
        {
            if ((ret = cosem_setArray(data, 17)) != 0 ||
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
                (ret = cosem_add(data, "CONNECT 2400", 12) != 0) ||
                (ret = cosem_add(data, "CONNECT 4800", 12) != 0) ||
                (ret = cosem_add(data, "CONNECT 9600", 12) != 0) ||
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
            if ((ret = cosem_setArray(data, target->modemProfile.size)) == 0)
            {
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
    int ret = 0;
    uint16_t pos;
    gxpppSetupLcpOption* lcpItem;
    gxpppSetupIPCPOption* ipcpItem;
    gxPppSetup* object = (gxPppSetup*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
#ifdef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->phy == NULL)
        {
            ret = cosem_setOctetString2(e->value.byteArr, EMPTY_LN, 6);
        }
        else
        {
            ret = cosem_setOctetString2(e->value.byteArr, object->phy->logicalName, 6);
        }
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->PHYReference, sizeof(object->PHYReference));
#endif //DLMS_IGNORE_OBJECT_POINTERS
#else
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->phy == NULL)
        {
            ret = cosem_setOctetString2(e->value.byteArr, EMPTY_LN, 6);
        }
        else
        {
            ret = cosem_setOctetString2(e->value.byteArr, object->phy->logicalName, 6);
        }
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->PHYReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_setArray(data, object->lcpOptions.size)) == 0)
        {
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
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_setArray(data, object->ipcpOptions.size)) == 0)
        {
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
    }
    else if (e->index == 5)
    {
        if (bb_size(&object->userName) == 0)
        {
            ret = bb_setUInt8(data, DLMS_DATA_TYPE_NONE);
        }
        else
        {
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
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    int ret;
    uint16_t pos;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxObject* od;
#else
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
#else
    gxObject* od;
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_MALLOC
    gxRegisterActivation* object = (gxRegisterActivation*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        //Add count only for first time.
        uint16_t pduSize = 0;
        if (!e->transaction)
        {
            e->transactionEndIndex = object->registerAssignment.size;
            ret = cosem_setArray(data, object->registerAssignment.size);
        }
        else
        {
            ret = 0;
        }
        if (ret == 0)
        {
            for (pos = (uint16_t)e->transactionStartIndex; pos != object->registerAssignment.size; ++pos)
            {
                pduSize = (uint16_t)data->size;
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES))
                if ((ret = oa_getByIndex(&object->registerAssignment, pos, &od)) != 0 ||
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
#else
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndexRef(&object->registerAssignment, pos, (void**)&od)) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
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
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
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
        }
    }
    else if (e->index == 3)
    {
#ifdef DLMS_IGNORE_MALLOC
        gxRegisterActivationMask* a;
#else
        gxKey* a;
#endif //DLMS_IGNORE_MALLOC
        uint32_t pos2;
        if ((ret = cosem_setArray(data, object->maskList.size)) == 0)
        {
            for (pos = 0; pos != object->maskList.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&a, sizeof(gxRegisterActivationMask))) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 2)) != 0)
                {
                    break;
                }
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = bb_setUInt8(data, a->length)) != 0 ||
                    (ret = bb_set(data, a->name, a->length)) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                    (ret = bb_setUInt8(data, a->count)) != 0)
                {
                    break;
                }
                for (pos2 = 0; pos2 != a->count; ++pos2)
                {
                    if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT8)) != 0 ||
                        (ret = bb_setUInt8(data, a->indexes[pos2])) != 0)
                    {
                        return ret;
                    }
                }
#else
                if ((ret = arr_getByIndex(&object->maskList, pos, (void**)&a)) != 0 ||
                    (ret = cosem_setStructure(data, 2)) != 0 ||
                    (ret = cosem_setOctetString(data, ((gxByteBuffer*)a->key))) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_ARRAY)) != 0 ||
                    (ret = bb_setUInt8(data, (unsigned char)bb_size((gxByteBuffer*)a->value))) != 0)
                {
                    break;
                }
                for (pos2 = 0; pos2 != bb_size((gxByteBuffer*)a->value); ++pos2)
                {
                    if ((ret = cosem_setUInt8(data, ((gxByteBuffer*)a->value)->data[pos2])) != 0)
                    {
                        break;
                    }
                }
                if (ret != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    else if (e->index == 4)
    {
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
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    uint16_t pos;
    dlmsVARIANT_PTR tmp;
    gxActionSet* actionSet;
    gxRegisterMonitor* object = (gxRegisterMonitor*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        //Add count
        if ((ret = cosem_setArray(data, object->thresholds.size)) == 0)
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
        //Add count
        if ((ret = cosem_setArray(data, object->actions.size)) == 0)
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
    int ret;
    uint16_t pos;
    gxSapItem* it;
    gxSapAssignment* object = (gxSapAssignment*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = cosem_setArray(data, object->sapAssignmentList.size)) == 0)
        {
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
    int ret = DLMS_ERROR_CODE_OK;
    uint16_t pos;
    gxScheduleEntry* se;
    gxSchedule* object = (gxSchedule*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = cosem_setArray(data, object->entries.size)) != 0)
        {
            return ret;
        }
        bitArray ba;
        ba_init(&ba);
        for (pos = 0; pos != object->entries.size; ++pos)
        {
            if ((ret = cosem_setStructure(data, 10)) != 0 ||
#ifdef DLMS_IGNORE_MALLOC
            (ret = arr_getByIndex(&object->entries, pos, (void**)&se, sizeof(gxScheduleEntry))) != 0 ||
#else
                (ret = arr_getByIndex(&object->entries, pos, (void**)&se)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                //Add index.
                (ret = cosem_setUInt16(data, se->index)) != 0 ||
                //Add enable.
                (ret = cosem_setBoolean(data, se->enable)) != 0 ||
                //Add logical Name.
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                (ret = cosem_setOctetString2(data, obj_getLogicalName((gxObject*)se->scriptTable), 6)) != 0 ||
#else
                (ret = cosem_setOctetString2(data, se->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                //Add script selector.
                (ret = cosem_setUInt16(data, se->scriptSelector)) != 0 ||
                //Add switch time.
                (ret = cosem_setTimeAsOctetString(data, &se->switchTime)) != 0 ||
                //Add validity window.
                (ret = cosem_setUInt16(data, se->validityWindow)) != 0 ||
                //Add exec week days.
                (ret = cosem_setBitString(data, se->execWeekdays, 7)) != 0 ||
                //Add exec spec days.
                (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                (ret = hlp_setObjectCount(se->execSpecDays.size, data)) != 0 ||
                (ret = bb_set(data, se->execSpecDays.data, ba_getByteCount(se->execSpecDays.size))) != 0 ||
                //Add begin date.
                (ret = cosem_setDateTimeAsOctetString(data, &se->beginDate)) != 0 ||
                //Add end date.
                (ret = cosem_setDateTimeAsOctetString(data, &se->endDate)) != 0)
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
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int cosem_getScriptTable(
    gxValueEventArg* e)
{
    int ret;
    uint16_t pos, pos2;
    gxScript* it;
    gxScriptAction* a;
    gxScriptTable* object = (gxScriptTable*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = cosem_setArray(data, object->scripts.size)) == 0)
        {
            for (pos = 0; pos != object->scripts.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&it, sizeof(gxScript))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->scripts, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setStructure(data, 2)) != 0 ||
                    //Script_identifier:
                    (ret = cosem_setUInt16(data, it->id)) != 0 ||
                    (ret = cosem_setArray(data, it->actions.size)) != 0)
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
                        (ret = cosem_setStructure(data, 5)) != 0 ||
                        //service_id
                        (ret = cosem_setEnum(data, a->type)) != 0 ||
                        //class_id
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_setUInt16(data, a->target == NULL ? 0 : a->target->objectType)) != 0 ||
#else
                        (ret = cosem_setUInt16(data, a->objectType)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        //logical_name
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                        (ret = cosem_setOctetString2(data, obj_getLogicalName(a->target), 6)) != 0 ||
#else
                        (ret = cosem_setOctetString2(data, a->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                        //index
                        (ret = cosem_setInt8(data, a->index)) != 0 ||
                        //parameter
                        (ret = cosem_setVariant(data, &a->parameter)) != 0)
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
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int cosem_getSpecialDaysTable(
    gxValueEventArg* e)
{
    int ret;
    uint16_t pos;
    dlmsVARIANT tmp;
    gxSpecialDay* sd;
    gxSpecialDaysTable* object = (gxSpecialDaysTable*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = var_init(&tmp)) != 0 ||
            (ret = cosem_setArray(data, object->entries.size)) == 0)
        {
            for (pos = 0; pos != object->entries.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd, sizeof(gxSpecialDay))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->entries, pos, (void**)&sd)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setStructure(data, 3)) != 0 ||
                    //Index
                    (ret = cosem_setUInt16(data, sd->index)) != 0 ||
                    //Date
                    (ret = cosem_setDateAsOctetString(data, &sd->date)) != 0 ||
                    //DayId
                    (ret = cosem_setUInt8(data, sd->dayId)) != 0)
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
int cosem_getTcpUdpSetup(
    gxValueEventArg* e)
{
    int ret;
    gxTcpUdpSetup* object = (gxTcpUdpSetup*)e->target;
    if (e->index == 2)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->port);
    }
    else if (e->index == 3)
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        ret = cosem_setOctetString2(e->value.byteArr, obj_getLogicalName(object->ipSetup), 6);
#else
        ret = cosem_setOctetString2(e->value.byteArr, object->ipReference, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    else if (e->index == 4)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->maximumSegmentSize);
    }
    else if (e->index == 5)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->maximumSimultaneousConnections);
    }
    else if (e->index == 6)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->inactivityTimeout);
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
        ret = cosem_setUInt16(e->value.byteArr, object->tableId);
    }
    else if (e->index == 3)
    {
        //Skip length.
    }
    else if (e->index == 4)
    {
        ret = cosem_setOctetString(e->value.byteArr, &object->buffer);
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
        ret = cosem_setEnum(e->value.byteArr, object->commSpeed);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

#ifndef DLMS_IGNORE_PUSH_SETUP
int cosem_getPushSetup(
    gxValueEventArg* e)
{
    int ret = 0;
    uint16_t pos;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
    gxTimePair* d;
#else
    gxKey* it;
#endif //DLMS_IGNORE_MALLOC
    gxPushSetup* object = (gxPushSetup*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = cosem_setArray(data, object->pushObjectList.size)) == 0)
        {
            for (pos = 0; pos != object->pushObjectList.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->pushObjectList, pos, (void**)&it, sizeof(gxTarget))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->pushObjectList, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setStructure(data, 4)) != 0 ||
                    //Type.
                    (ret = cosem_setUInt16(data, OBJECT_TYPE)) != 0 ||
                    //LN
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_setOctetString2(data, it->logicalName, 6)) != 0 ||
#else
                    (ret = cosem_setOctetString2(data, it->target->logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    //attributeIndex
                    (ret = cosem_setInt8(data, it->attributeIndex)) != 0 ||
                    //dataIndex
                    (ret = cosem_setUInt16(data, it->dataIndex)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->pushObjectList, pos, (void**)&it)) != 0 ||
                    (ret = cosem_setStructure(data, 4)) != 0 ||
                    //Type.
                    (ret = cosem_setUInt16(data, ((gxObject*)it->key)->objectType)) != 0 ||
                    //LN
                    (ret = cosem_setOctetString2(data, ((gxObject*)it->key)->logicalName, 6)) != 0 ||
                    //attributeIndex
                    (ret = cosem_setInt8(data, ((gxTarget*)it->value)->attributeIndex)) != 0 ||
                    //dataIndex
                    (ret = cosem_setUInt16(data, ((gxTarget*)it->value)->dataIndex)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    else if (e->index == 3)
    {
        if ((ret = cosem_setStructure(data, 3)) != 0 ||
            //Service
            (ret = cosem_setEnum(data, object->service)) != 0 ||
            (ret = cosem_setOctetString(data, &object->destination)) != 0 ||
            //Message
            (ret = cosem_setEnum(data, object->message)) != 0)
        {
        }
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_setArray(data, object->communicationWindow.size)) == 0)
        {
            for (pos = 0; pos != object->communicationWindow.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&d, sizeof(gxTimePair))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&d)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setStructure(data, 2)) != 0 ||
                    //Start date time.
                    (ret = cosem_setDateTimeAsOctetString(data, &d->first)) != 0 ||
                    //End date time.
                    (ret = cosem_setDateTimeAsOctetString(data, &d->second)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->communicationWindow, pos, (void**)&it)) != 0 ||
                    (ret = cosem_setStructure(data, 2)) != 0 ||
                    //Start date time.
                    (ret = cosem_setDateTimeAsOctetString(data, (gxtime*)it->key)) != 0 ||
                    //End date time.
                    (ret = cosem_setDateTimeAsOctetString(data, (gxtime*)it->value)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
    }
    else if (e->index == 5)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->randomisationStartInterval);
    }
    else if (e->index == 6)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->numberOfRetries);
    }
    else if (e->index == 7)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->repetitionDelay);
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
    int ret = 0;
    uint16_t pos;
    gxActiveDevice* it;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->enableDisableJoining);
    }
    else if (e->index == 3)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->joinTimeout);
    }
    else if (e->index == 4)
    {
        if ((ret = cosem_setArray(data, object->activeDevices.size)) != 0)
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
                (ret = cosem_setStructure(data, 14)) != 0 ||
                //mac address.
                (ret = cosem_setOctetString(data, &it->macAddress)) != 0 ||
                //status
                (ret = cosem_setBitString(data, it->status, 5)) != 0 ||
                //max RSSI
                (ret = cosem_setInt8(data, it->maxRSSI)) != 0 ||
                //average RSSI
                (ret = cosem_setInt8(data, it->averageRSSI)) != 0 ||
                //min RSSI
                (ret = cosem_setInt8(data, it->minRSSI)) != 0 ||
                //max LQI
                (ret = cosem_setUInt8(data, it->maxLQI)) != 0 ||
                //average LQI
                (ret = cosem_setUInt8(data, it->averageLQI)) != 0 ||
                //min LQI
                (ret = cosem_setUInt8(data, it->minLQI)) != 0 ||
                //last communication date time
                (ret = cosem_setDateTimeAsOctetString(data, &it->lastCommunicationDateTime)) != 0 ||
                //number of hops
                (ret = cosem_setUInt8(data, it->numberOfHops)) != 0 ||
                //transmission failures
                (ret = cosem_setUInt8(data, it->transmissionFailures)) != 0 ||
                //transmission successes
                (ret = cosem_setUInt8(data, it->transmissionSuccesses)) != 0 ||
                //application version
                (ret = cosem_setUInt8(data, it->applicationVersion)) != 0 ||
                //stack version
                (ret = cosem_setUInt8(data, it->stackVersion)) != 0)
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
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_CHARGE
int getUnitCharge(gxUnitCharge* target, gxValueEventArg* e)
{
    int ret = 0;
    uint16_t pos;
    gxChargeTable* it;
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = cosem_setStructure(data, 3)) != 0 ||
        //-------------
        //charge per unit scaling
        (ret = cosem_setStructure(data, 2)) != 0 ||
        //commodity scale
        (ret = cosem_setInt8(data, target->chargePerUnitScaling.commodityScale)) != 0 ||
        //price scale
        (ret = cosem_setInt8(data, target->chargePerUnitScaling.priceScale)) != 0 ||
        //-------------
        //commodity
        (ret = cosem_setStructure(data, 3)) != 0 ||
        //type
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_setUInt16(data, target->commodity.target == 0 ? 0 : target->commodity.target->objectType)) != 0 ||
#else
        (ret = cosem_setUInt16(data, target->commodity.type)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //logicalName
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        (ret = cosem_setOctetString2(data, obj_getLogicalName(target->commodity.target), 6)) != 0 ||
#else
        (ret = cosem_setOctetString2(data, target->commodity.logicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //attributeIndex
        (ret = cosem_setInt8(data, target->commodity.attributeIndex)) != 0 ||
        //-------------
        //chargeTables
        (ret = cosem_setArray(data, target->chargeTables.size)) != 0)
    {
        return ret;
    }
    for (pos = 0; pos != target->chargeTables.size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&it, sizeof(gxChargeTable))) != 0 ||
            (ret = cosem_setStructure(data, 2)) != 0 ||
            //index
            (ret = cosem_setOctetString2(data, it->index.data, it->index.size)) != 0 ||
            //chargePerUnit
            (ret = cosem_setInt16(data, it->chargePerUnit)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(&target->chargeTables, pos, (void**)&it)) != 0 ||
            (ret = cosem_setStructure(data, 2)) != 0 ||
            //index
            (ret = cosem_setOctetString(data, &it->index)) != 0 ||
            //chargePerUnit
            (ret = cosem_setInt16(data, it->chargePerUnit)) != 0)
        {
            break;
        }
#endif //DLMS_IGNORE_MALLOC
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
        ret = cosem_setInt32(e->value.byteArr, object->totalAmountPaid);
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(e->value.byteArr, object->chargeType);
    }
    else if (e->index == 4)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->priority);
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
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &object->unitChargeActivationTime);
    }
    else if (e->index == 8)
    {
        ret = cosem_setUInt32(e->value.byteArr, object->period);
    }
    else if (e->index == 9)
    {
        ret = cosem_setBitString(e->value.byteArr, object->chargeConfiguration, 2);
    }
    else if (e->index == 10)
    {
        ret = cosem_setDateTime(e->value.byteArr, &object->lastCollectionTime);
    }
    else if (e->index == 11)
    {
        ret = cosem_setInt32(e->value.byteArr, object->lastCollectionAmount);
    }
    else if (e->index == 12)
    {
        ret = cosem_setInt32(e->value.byteArr, object->totalAmountRemaining);
    }
    else if (e->index == 13)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->proportion);
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
    int ret = 0;
    uint16_t pos;
    gxByteBuffer* data = e->value.byteArr;
#ifdef DLMS_IGNORE_MALLOC
    gxTokenGatewayDescription* it;
#else
    gxByteBuffer* it;
#endif //DLMS_IGNORE_MALLOC
    gxTokenGateway* object = (gxTokenGateway*)e->target;
    switch (e->index)
    {
    case 2:
        ret = cosem_setOctetString2(data, object->token.data, (uint16_t)object->token.size);
        break;
    case 3:
        ret = cosem_setDateTimeAsOctetString(data, &object->time);
        break;
    case 4:
        if ((ret = cosem_setArray(data, object->descriptions.size)) == 0)
        {
            for (pos = 0; pos != object->descriptions.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->descriptions, pos, (void**)&it, sizeof(gxTokenGatewayDescription))) != 0)
                {
                    break;
                }
                if ((ret = cosem_setOctetString2(data, it->value, it->size)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->descriptions, pos, (void**)&it)) != 0 ||
                    (ret = cosem_setOctetString(data, it)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
        }
        break;
    case 5:
        ret = cosem_setEnum(e->value.byteArr, object->deliveryMethod);
        break;
    case 6:
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            (ret = cosem_setEnum(data, object->status)) != 0 ||
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
            (ret = hlp_setObjectCount(object->dataValue.size, data)) != 0 ||
            (ret = bb_set(data, object->dataValue.data, ba_getByteCount(object->dataValue.size))) != 0)
        {
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
        ret = cosem_setInt32(e->value.byteArr, object->currentCreditAmount);
    }
    else if (e->index == 3)
    {
        ret = cosem_setEnum(e->value.byteArr, object->type);
    }
    else if (e->index == 4)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->priority);
    }
    else if (e->index == 5)
    {
        ret = cosem_setInt32(e->value.byteArr, object->warningThreshold);
    }
    else if (e->index == 6)
    {
        ret = cosem_setInt32(e->value.byteArr, object->limit);
    }
    else if (e->index == 7)
    {
        ret = cosem_setBitString(e->value.byteArr, object->creditConfiguration, 5);
    }
    else if (e->index == 8)
    {
        ret = cosem_setEnum(e->value.byteArr, object->status);
    }
    else if (e->index == 9)
    {
        ret = cosem_setInt32(e->value.byteArr, object->presetCreditAmount);
    }
    else if (e->index == 10)
    {
        ret = cosem_setInt32(e->value.byteArr, object->creditAvailableThreshold);
    }
    else if (e->index == 11)
    {
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &object->period);
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
    int ret = 0;
    uint16_t pos;
    unsigned char* it;
    gxCreditChargeConfiguration* ccc;
    gxTokenGatewayConfiguration* gwc;
    gxAccount* object = (gxAccount*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if (e->index == 2)
    {
        if ((ret = cosem_setStructure(data, 2)) != 0 ||
            //payment mode
            (ret = cosem_setEnum(data, object->paymentMode & 0x3)) != 0 ||
            //account status
            (ret = cosem_setEnum(data, object->accountStatus & 0x3)) != 0)
        {
        }
    }
    else if (e->index == 3)
    {
        ret = cosem_setUInt8(e->value.byteArr, object->currentCreditInUse);
    }
    else if (e->index == 4)
    {
        ret = cosem_setBitString(e->value.byteArr, object->currentCreditStatus, 7);
    }
    else if (e->index == 5)
    {
        ret = cosem_setInt32(e->value.byteArr, object->availableCredit);
    }
    else if (e->index == 6)
    {
        ret = cosem_setInt32(e->value.byteArr, object->amountToClear);
    }
    else if (e->index == 7)
    {
        ret = cosem_setInt32(e->value.byteArr, object->clearanceThreshold);
    }
    else if (e->index == 8)
    {
        ret = cosem_setInt32(e->value.byteArr, object->aggregatedDebt);
    }
    else if (e->index == 9)
    {
        if ((ret = cosem_setArray(data, object->creditReferences.size)) == 0)
        {
            for (pos = 0; pos != object->creditReferences.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->creditReferences, pos, (void**)&it, 6)) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->creditReferences, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setOctetString2(data, it, 6)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (e->index == 10)
    {
        if ((ret = cosem_setArray(data, object->chargeReferences.size)) == 0)
        {
            for (pos = 0; pos != object->chargeReferences.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->chargeReferences, pos, (void**)&it, 6)) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->chargeReferences, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setOctetString2(data, it, 6)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (e->index == 11)
    {
        if ((ret = cosem_setArray(data, object->creditChargeConfigurations.size)) == 0)
        {
            for (pos = 0; pos != object->creditChargeConfigurations.size; ++pos)
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->creditChargeConfigurations, pos, (void**)&ccc, sizeof(gxCreditChargeConfiguration))) != 0 ||
#else
                    if ((ret = arr_getByIndex(&object->creditChargeConfigurations, pos, (void**)&ccc)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                        (ret = cosem_setStructure(data, 3)) != 0 ||
                        //credit reference
                        (ret = cosem_setOctetString2(data, ccc->creditReference, 6)) != 0 ||
                        //charge reference
                        (ret = cosem_setOctetString2(data, ccc->chargeReference, 6)) != 0 ||
                        //collection configuration
                        (ret = cosem_setBitString(data, ccc->collectionConfiguration, 3)) != 0)
                {
                    break;
                }
        }
    }
    else if (e->index == 12)
    {
        if ((ret = cosem_setArray(data, object->tokenGatewayConfigurations.size)) == 0)
        {
            for (pos = 0; pos != object->tokenGatewayConfigurations.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->tokenGatewayConfigurations, pos, (void**)&gwc, sizeof(gxTokenGatewayConfiguration))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->tokenGatewayConfigurations, pos, (void**)&gwc)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setStructure(data, 2)) != 0 ||
                    //credit reference
                    (ret = cosem_setOctetString2(data, gwc->creditReference, 6)) != 0 ||
                    //token proportion
                    (ret = cosem_setUInt8(data, gwc->tokenProportion)) != 0)
                {
                    break;
                }
            }
        }
    }
    else if (e->index == 13)
    {
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &object->accountActivationTime);
    }
    else if (e->index == 14)
    {
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &object->accountClosureTime);
    }
    else if (e->index == 15)
    {
        if ((ret = cosem_setStructure(data, 3)) != 0 ||
            //Name
            (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRING_UTF8)) != 0)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = hlp_setObjectCount(object->currency.name.size, data)) != 0 ||
            (ret = bb_set(data, object->currency.name.data, object->currency.name.size)) != 0)
        {
            return ret;
        }
#else
        if ((ret = hlp_setObjectCount(object->currency.name.size, data)) != 0 ||
            (ret = bb_set(data, (unsigned char*)object->currency.name.value, object->currency.name.size)) != 0)
        {
            return ret;
        }
#endif //DLMS_IGNORE_MALLOC
        //scale
        if ((ret = cosem_setInt8(data, object->currency.scale)) != 0 ||
            //unit
            (ret = cosem_setEnum(data, object->currency.unit)) != 0)
        {
            return ret;
        }
    }
    else if (e->index == 16)
    {
        ret = cosem_setInt32(e->value.byteArr, object->lowCreditThreshold);
    }
    else if (e->index == 17)
    {
        ret = cosem_setInt32(e->value.byteArr, object->nextCreditAvailableThreshold);
    }
    else if (e->index == 18)
    {
        ret = cosem_setUInt16(e->value.byteArr, object->maxProvision);
    }
    else if (e->index == 19)
    {
        ret = cosem_setInt32(e->value.byteArr, object->maxProvisionPeriod);
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
    BYTE_BUFFER_INIT(&data);
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
    gxByteBuffer* data = data = e->value.byteArr;
    gxCompactData* object = (gxCompactData*)e->target;
    int ret;
    switch (e->index)
    {
    case 2:
        ret = cosem_setOctetString(data, &object->buffer);
        break;
    case 3:
        ret = getColumns(settings, &object->captureObjects, data, e);
        break;
    case 4:
        ret = cosem_setUInt8(e->value.byteArr, object->templateId);
        break;
    case 5:
        ret = cosem_setOctetString(data, &object->templateDescription);
        break;
    case 6:
        ret = cosem_setEnum(e->value.byteArr, object->captureMethod);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_COMPACT_DATA

int cosem_getValue(
    dlmsSettings* settings,
    gxValueEventArg* e)
{
    int ret;
    if ((ret = cosem_getByteBuffer(e)) != 0)
    {
        return ret;
    }
    if (e->index == 1)
    {
        if ((ret = bb_setUInt8(e->value.byteArr, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(e->value.byteArr, 6)) != 0 ||
            (ret = bb_set(e->value.byteArr, e->target->logicalName, 6)) != 0)
        {
        }
        return ret;
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
        ret = cosem_getIecTwistedPairSetup(e);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = cosem_getIP4Setup(e);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = cosem_getIP6Setup(e);
        break;
#endif //DLMS_IGNORE_IP6_SETUP
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
        ret = cosem_getRegisterActivation(settings, e);
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
        ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        //TODO:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
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
        ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
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
        ret = DLMS_ERROR_CODE_NOT_IMPLEMENTED;
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
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = cosem_getArbitrator(e);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = cosem_getIec8802LlcType1Setup(e);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ret = cosem_getIec8802LlcType2Setup(e);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ret = cosem_getIec8802LlcType3Setup(e);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ret = cosem_getSFSKActiveInitiator(e);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ret = cosem_getFSKMacCounters(e);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ret = cosem_getSFSKMacSynchronizationTimeouts(e);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ret = cosem_getSFSKPhyMacSetUp(e);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ret = cosem_getSFSKReportingSystemList(e);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
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
    if ((ret = cosem_setArray(data, 5)) != 0 ||
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
            ret = cosem_setString(e->value.byteArr, object->calendarName, 0);
        }
        else
        {
            ret = cosem_setString(e->value.byteArr, object->calendarName, (uint16_t)strlen(object->calendarName));
        }
        break;
    case 3:
        ret = cosem_setBoolean(e->value.byteArr, object->enabled);
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
    gxByteBuffer* data = e->value.byteArr;
    gxGsmDiagnostic* object = (gxGsmDiagnostic*)e->target;
    int ret;
    uint16_t pos;
    switch (e->index)
    {
    case 2:
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = cosem_setString2(e->value.byteArr, &object->operatorName);
#else
        ret = cosem_setString(e->value.byteArr, object->operatorName,
            object->operatorName != NULL ? (uint16_t)strlen(object->operatorName) : 0);
#endif //DLMS_IGNORE_MALLOC
        break;
    case 3:
        ret = cosem_setEnum(e->value.byteArr, object->status);
        break;
    case 4:
        ret = cosem_setEnum(e->value.byteArr, object->circuitSwitchStatus);
        break;
    case 5:
        ret = cosem_setEnum(e->value.byteArr, object->packetSwitchStatus);
        break;
    case 6:
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) == 0)
        {
            if (object->base.version == 0)
            {
                if ((ret = bb_setUInt8(data, 4)) != 0 ||
                    //cellId.
                    (ret = cosem_setUInt16(data, (uint16_t)object->cellInfo.cellId)) != 0)
                {
                    return ret;
                }
            }
            else
            {
                if ((ret = bb_setUInt8(data, 7)) != 0 ||
                    //cellId.
                    (ret = cosem_setUInt32(data, object->cellInfo.cellId)) != 0)
                {
                    return ret;
                }
            }
            //LocationId.
            if ((ret = cosem_setUInt16(data, object->cellInfo.locationId)) != 0 ||
                //SignalQuality.
                (ret = cosem_setUInt8(data, object->cellInfo.signalQuality)) != 0 ||
                //Ber.
                (ret = cosem_setUInt8(data, object->cellInfo.ber)) != 0)
            {
                return ret;
            }
            if (object->base.version > 0)
            {
                if (//mobileCountryCode.
                    (ret = cosem_setUInt16(data, object->cellInfo.mobileCountryCode)) != 0 ||
                    //MobileNetworkCode.
                    (ret = cosem_setUInt16(data, object->cellInfo.mobileNetworkCode)) != 0 ||
                    //ChannelNumber.
                    (ret = cosem_setUInt32(data, object->cellInfo.channelNumber)) != 0)
                {
                }
            }
        }
        break;
    case 7:
        if ((ret = cosem_setArray(data, object->adjacentCells.size)) == 0)
        {
            for (pos = 0; pos != object->adjacentCells.size; ++pos)
            {
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it, sizeof(gxAdjacentCell))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = cosem_setStructure(data, 2)) != 0)
                {
                    break;
                }
                if (object->base.version == 0)
                {
                    //cellId.
                    if ((ret = cosem_setUInt16(data, (uint16_t)it->cellId)) != 0)
                    {
                        break;
                    }
                }
                else
                {
                    //cellId.
                    if ((ret = cosem_setUInt32(data, it->cellId)) != 0)
                    {
                        break;
                    }
                }
                //SignalQuality.
                if ((ret = cosem_setUInt8(data, it->signalQuality)) != 0)
                {
                    break;
                }
            }
        }
        break;
    case 8:
        ret = cosem_setDateTime(e->value.byteArr, &object->captureTime);
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
    int ret = 0;
    uint16_t pos;
    gxByteBuffer* data = e->value.byteArr;
    gxParameterMonitor* object = (gxParameterMonitor*)e->target;
    switch (e->index)
    {
    case 2:
    {
        if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
            (ret = bb_setUInt8(data, 4)) != 0)
        {
            return ret;
        }
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        if (object->changedParameter.target == NULL)
        {
            if ((ret = cosem_setUInt16(data, 0)) != 0 ||
                (ret = cosem_setOctetString2(data, EMPTY_LN, 6)) != 0 ||
                (ret = cosem_setInt8(data, 0)) != 0 ||
                (ret = bb_setUInt8(data, 0)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
        else
        {
            if ((ret = cosem_setUInt16(data, object->changedParameter.target->objectType)) != 0 ||
                (ret = cosem_setOctetString2(data, object->changedParameter.target->logicalName, 6)) != 0 ||
                (ret = cosem_setInt8(data, object->changedParameter.attributeIndex)) != 0 ||
                (ret = cosem_setVariant(data, &object->changedParameter.value)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
#else
        if ((ret = cosem_setUInt16(data, object->changedParameter.type)) != 0 ||
            (ret = cosem_setOctetString2(data, object->changedParameter.logicalName, 6)) != 0 ||
            (ret = cosem_setInt8(data, object->changedParameter.attributeIndex)) != 0 ||
            (ret = cosem_setVariant(data, &object->changedParameter.value)) != 0)
        {
            //Error code is returned at the end of the function.
        }
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }
    break;
    case 3:
        ret = cosem_setDateTimeAsOctetString(e->value.byteArr, &object->captureTime);
        break;
    case 4:
    {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxTarget* it;
#else
        gxKey* it;
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cosem_setArray(data, object->parameters.size)) == 0)
        {
            for (pos = 0; pos != object->parameters.size; ++pos)
            {
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#ifdef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->parameters, pos, (void**)&it, sizeof(gxTarget))) != 0 ||
#else
                if ((ret = arr_getByIndex(&object->parameters, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    (ret = bb_setUInt8(data, 3)) != 0 ||
                    //Type.
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_UINT16)) != 0 ||
                    (ret = bb_setUInt16(data, OBJECT_TYPE)) != 0 ||
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
                    (ret = cosem_setStructure(data, 3)) != 0 ||
                    //Type.
                    (ret = cosem_setUInt16(data, ((gxObject*)it->key)->objectType)) != 0 ||
                    //LN
                    (ret = cosem_setOctetString2(data, ((gxObject*)it->key)->logicalName, 6)) != 0 ||
                    //attributeIndex
                    (ret = cosem_setInt8(data, ((gxTarget*)it->value)->attributeIndex)) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
            }
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
        ret = cosem_setUInt16(e->value.byteArr, object->serviceNodeAddress);
        break;
    case 3:
        ret = cosem_setUInt16(e->value.byteArr, object->baseNodeAddress);
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
        ret = cosem_setUInt16(e->value.byteArr, object->crcIncorrectCount);
        break;
    case 3:
        ret = cosem_setUInt16(e->value.byteArr, object->crcFailedCount);
        break;
    case 4:
        ret = cosem_setUInt16(e->value.byteArr, object->txDropCount);
        break;
    case 5:
        ret = cosem_setUInt16(e->value.byteArr, object->rxDropCount);
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
        ret = cosem_setUInt8(e->value.byteArr, object->macMinSwitchSearchTime);
        break;
    case 3:
        ret = cosem_setUInt8(e->value.byteArr, object->macMaxPromotionPdu);
        break;
    case 4:
        ret = cosem_setUInt8(e->value.byteArr, object->macPromotionPduTxPeriod);
        break;
    case 5:
        ret = cosem_setUInt8(e->value.byteArr, object->macBeaconsPerFrame);
        break;
    case 6:
        ret = cosem_setUInt8(e->value.byteArr, object->macScpMaxTxAttempts);
        break;
    case 7:
        ret = cosem_setUInt8(e->value.byteArr, object->macCtlReTxTimer);
        break;
    case 8:
        ret = cosem_setUInt8(e->value.byteArr, object->macMaxCtlReTx);
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
        ret = cosem_setInt16(e->value.byteArr, object->lnId);
        break;
    case 3:
        ret = cosem_setUInt8(e->value.byteArr, object->lsId);
        break;
    case 4:
        ret = cosem_setUInt8(e->value.byteArr, object->sId);
        break;
    case 5:
    {
        ret = cosem_setOctetString(e->value.byteArr, &object->sna);
    }
    break;
    case 6:
        ret = cosem_setEnum(e->value.byteArr, object->state);
        break;
    case 7:
        ret = cosem_setUInt16(e->value.byteArr, object->scpLength);
        break;
    case 8:
        ret = cosem_setUInt8(e->value.byteArr, object->nodeHierarchyLevel);
        break;
    case 9:
        ret = cosem_setUInt8(e->value.byteArr, object->beaconSlotCount);
        break;
    case 10:
        ret = cosem_setUInt8(e->value.byteArr, object->beaconRxSlot);
        break;
    case 11:
        ret = cosem_setUInt8(e->value.byteArr, object->beaconTxSlot);
        break;
    case 12:
        ret = cosem_setUInt8(e->value.byteArr, object->beaconRxFrequency);
        break;
    case 13:
        ret = cosem_setUInt8(e->value.byteArr, object->beaconTxFrequency);
        break;
    case 14:
        ret = cosem_setEnum(e->value.byteArr, (unsigned char)object->capabilities);
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
        ret = cosem_setUInt32(e->value.byteArr, object->txDataPktCount);
        break;
    case 3:
        ret = cosem_setUInt32(e->value.byteArr, object->rxDataPktCount);
        break;
    case 4:
        ret = cosem_setUInt32(e->value.byteArr, object->txCtrlPktCount);
        break;
    case 5:
        ret = cosem_setUInt32(e->value.byteArr, object->rxCtrlPktCount);
        break;
    case 6:
        ret = cosem_setUInt32(e->value.byteArr, object->csmaFailCount);
        break;
    case 7:
        ret = cosem_setUInt32(e->value.byteArr, object->csmaChBusyCount);
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
    int ret;
    uint16_t pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = cosem_setArray(data, object->multicastEntries.size)) == 0)
    {
        for (pos = 0; pos != object->multicastEntries.size; ++pos)
        {
            if ((ret = cosem_setStructure(data, 2)) != 0)
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
    }
    return ret;
}

int cosem_getSwitchTable(gxValueEventArg* e)
{
    uint16_t* it;
    int ret;
    uint16_t pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = cosem_setArray(data, object->switchTable.size)) == 0)
    {
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
            if ((ret = cosem_setInt16(data, *it)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int cosem_getDirectTable(gxValueEventArg* e)
{
    gxMacDirectTable* it;
    int ret;
    uint16_t pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = cosem_setArray(data, object->directTable.size)) == 0)
    {
        for (pos = 0; pos != object->directTable.size; ++pos)
        {
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

            if ((ret = cosem_setStructure(data, 7)) != 0 ||
                (ret = cosem_setInt16(data, it->sourceSId)) != 0 ||
                (ret = cosem_setInt16(data, it->sourceLnId)) != 0 ||
                (ret = cosem_setInt16(data, it->sourceLcId)) != 0 ||
                (ret = cosem_setInt16(data, it->destinationSId)) != 0 ||
                (ret = cosem_setInt16(data, it->destinationLnId)) != 0 ||
                (ret = cosem_setInt16(data, it->destinationLcId)) != 0 ||
                (ret = cosem_setOctetString2(data, it->did, sizeof(it->did))) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int cosem_getAvailableSwitches(gxValueEventArg* e)
{
    gxMacAvailableSwitch* it;
    int ret;
    uint16_t pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = cosem_setArray(data, object->availableSwitches.size)) == 0)
    {
        for (pos = 0; pos != object->availableSwitches.size; ++pos)
        {
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

            if ((ret = cosem_setStructure(data, 5)) != 0 ||
                (ret = cosem_setOctetString2(data, it->sna.data, (uint16_t)it->sna.size)) != 0 ||
                (ret = cosem_setInt16(data, it->lsId)) != 0 ||
                (ret = cosem_setInt8(data, it->level)) != 0 ||
                (ret = cosem_setInt8(data, it->rxLevel)) != 0 ||
                (ret = cosem_setInt8(data, it->rxSnr)) != 0)
            {
                break;
            }
        }
    }
    return ret;
}

int cosem_getCommunications(gxValueEventArg* e)
{
    gxMacPhyCommunication* it;
    int ret;
    uint16_t pos;
    gxPrimeNbOfdmPlcMacNetworkAdministrationData* object = (gxPrimeNbOfdmPlcMacNetworkAdministrationData*)e->target;
    gxByteBuffer* data = e->value.byteArr;
    if ((ret = cosem_setArray(data, object->communications.size)) == 0)
    {
        for (pos = 0; pos != object->communications.size; ++pos)
        {
#ifndef DLMS_IGNORE_MALLOC
            if ((ret = arr_getByIndex(&object->communications, pos, (void**)&it)) != 0)
            {
                break;
            }
#else
            if ((ret = arr_getByIndex(&object->communications, pos, (void**)&it, sizeof(gxMacPhyCommunication))) != 0)
            {
                break;
            }
#endif //DLMS_IGNORE_MALLOC

            if ((ret = cosem_setStructure(data, 9)) != 0 ||
                (ret = cosem_setOctetString2(data, it->eui, sizeof(it->eui))) != 0 ||
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
        ret = cosem_setOctetString2(e->value.byteArr, object->firmwareVersion.data, (uint16_t)object->firmwareVersion.size);
    }
    break;
    case 3:
        ret = cosem_setUInt16(e->value.byteArr, object->vendorId);
        break;
    case 4:
        ret = cosem_setUInt16(e->value.byteArr, object->productId);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
int cosem_getArbitrator(gxValueEventArg* e)
{
    int ret;
    uint16_t pos;
    gxActionItem* it;
    gxByteBuffer* data = e->value.byteArr;
    gxArbitrator* object = (gxArbitrator*)e->target;
    switch (e->index)
    {
    case 2:
    {
        if ((ret = cosem_setArray(data, object->actions.size)) == 0)
        {
            for (pos = 0; pos != object->actions.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->actions, pos, (void**)&it)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->actions, pos, (void**)&it, sizeof(gxMacAvailableSwitch))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
                    //Count
                    (ret = bb_setUInt8(data, 2)) != 0 ||
                    (ret = bb_setUInt8(data, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
                    (ret = bb_setUInt8(data, 6)) != 0 ||
#ifndef DLMS_IGNORE_OBJECT_POINTERS
                    (ret = bb_set(data, obj_getLogicalName((gxObject*)it->script), 6)) != 0 ||
#else
                    (ret = bb_set(data, it->script->executedScriptLogicalName, 6)) != 0 ||
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    (ret = cosem_setUInt16(data, it->scriptSelector)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 3:
    {
        bitArray* a;
        if ((ret = cosem_setArray(data, object->permissionsTable.size)) == 0)
        {
            for (pos = 0; pos != object->permissionsTable.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->permissionsTable, pos, (void**)&a)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->permissionsTable, pos, (void**)&a, sizeof(bitArray))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(a->size, data)) != 0)
                {
                    break;
                }
                ret = bb_set(e->value.byteArr, a->data, ba_getByteCount(a->size));
            }
        }
    }
    break;
    case 4:
    {
        dlmsVARIANT* v;
        if ((ret = cosem_setArray(data, object->weightingsTable.size)) == 0)
        {
            for (pos = 0; pos != object->weightingsTable.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->weightingsTable, pos, (void**)&v)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->weightingsTable, pos, (void**)&v, sizeof(dlmsVARIANT))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = cosem_setVariant(data, v)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 5:
    {
        bitArray* a;
        if ((ret = cosem_setArray(data, object->mostRecentRequestsTable.size)) == 0)
        {
            for (pos = 0; pos != object->mostRecentRequestsTable.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->mostRecentRequestsTable, pos, (void**)&a)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->mostRecentRequestsTable, pos, (void**)&a, sizeof(bitArray))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = bb_setUInt8(data, DLMS_DATA_TYPE_BIT_STRING)) != 0 ||
                    (ret = hlp_setObjectCount(a->size, data)) != 0)
                {
                    break;
                }
                ret = bb_set(e->value.byteArr, a->data, ba_getByteCount(a->size));
            }
        }
    }
    break;
    case 6:
        ret = cosem_setUInt8(data, object->lastOutcome);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_ARBITRATOR

#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int cosem_getIec8802LlcType1Setup(
    gxValueEventArg* e)
{
    int ret = 0;
    gxByteBuffer* data = e->value.byteArr;
    gxIec8802LlcType1Setup* object = (gxIec8802LlcType1Setup*)e->target;
    if (e->index == 2)
    {
        ret = cosem_setUInt16(data, object->maximumOctetsUiPdu);
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int cosem_getIec8802LlcType2Setup(
    gxValueEventArg* e)
{
    int ret = 0;
    gxByteBuffer* data = e->value.byteArr;
    gxIec8802LlcType2Setup* object = (gxIec8802LlcType2Setup*)e->target;
    switch (e->index)
    {
    case 2:
        ret = cosem_setUInt8(data, object->transmitWindowSizeK);
        break;
    case 3:
        ret = cosem_setUInt8(data, object->transmitWindowSizeRW);
        break;
    case 4:
        ret = cosem_setUInt16(data, object->maximumOctetsPdu);
        break;
    case 5:
        ret = cosem_setUInt8(data, object->maximumNumberTransmissions);
        break;
    case 6:
        ret = cosem_setUInt16(data, object->acknowledgementTimer);
        break;
    case 7:
        ret = cosem_setUInt16(data, object->bitTimer);
        break;
    case 8:
        ret = cosem_setUInt16(data, object->rejectTimer);
        break;
    case 9:
        ret = cosem_setUInt16(data, object->busyStateTimer);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}

#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int cosem_getIec8802LlcType3Setup(
    gxValueEventArg* e)
{
    int ret = 0;
    gxByteBuffer* data = e->value.byteArr;
    gxIec8802LlcType3Setup* object = (gxIec8802LlcType3Setup*)e->target;
    switch (e->index)
    {
    case 2:
        ret = cosem_setUInt16(data, object->maximumOctetsACnPdu);
        break;
    case 3:
        ret = cosem_setUInt8(data, object->maximumTransmissions);
        break;
    case 4:
        ret = cosem_setUInt16(data, object->acknowledgementTime);
        break;
    case 5:
        ret = cosem_setUInt16(data, object->receiveLifetime);
        break;
    case 6:
        ret = cosem_setUInt16(data, object->transmitLifetime);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int cosem_getSFSKActiveInitiator(
    gxValueEventArg* e)
{
    int ret;
    gxByteBuffer* data = e->value.byteArr;
    gxSFSKActiveInitiator* object = (gxSFSKActiveInitiator*)e->target;
    if (e->index == 2)
    {
        if ((ret = cosem_setStructure(data, 3)) == 0 &&
            (ret = cosem_setOctetString(data, &object->systemTitle)) == 0 &&
            (ret = cosem_setUInt16(data, object->macAddress)) == 0 &&
            (ret = cosem_setUInt8(data, object->lSapSelector)) == 0)
        {
        }
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
int cosem_getFSKMacCounters(
    gxValueEventArg* e)
{
    int ret = 0;
    uint16_t pos;
    gxByteBuffer* data = e->value.byteArr;
    gxFSKMacCounters* object = (gxFSKMacCounters*)e->target;
    switch (e->index)
    {
    case 2:
    {
        gxUint16PairUint32* it;
        if ((ret = cosem_setArray(data, object->synchronizationRegister.size)) == 0)
        {
            for (pos = 0; pos != object->synchronizationRegister.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->synchronizationRegister, pos, (void**)&it)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->synchronizationRegister, pos, (void**)&it, sizeof(gxFSKMacCounters))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = cosem_setStructure(data, 2)) != 0 ||
                    (ret = cosem_setUInt16(data, it->first)) != 0 ||
                    (ret = cosem_setUInt32(data, it->second)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 3:
    {
        if ((ret = cosem_setStructure(data, 5)) == 0 &&
            (ret = cosem_setUInt32(data, object->physicalLayerDesynchronization)) == 0 &&
            (ret = cosem_setUInt32(data, object->timeOutNotAddressedDesynchronization)) == 0 &&
            (ret = cosem_setUInt32(data, object->timeOutFrameNotOkDesynchronization)) == 0 &&
            (ret = cosem_setUInt32(data, object->writeRequestDesynchronization)) == 0 &&
            (ret = cosem_setUInt32(data, object->wrongInitiatorDesynchronization)) == 0)
        {
        }
    }
    break;
    case 4:
    {
        gxUint16PairUint32* it;
        if ((ret = cosem_setArray(data, object->broadcastFramesCounter.size)) == 0)
        {
            for (pos = 0; pos != object->broadcastFramesCounter.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->broadcastFramesCounter, pos, (void**)&it)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->broadcastFramesCounter, pos, (void**)&it, sizeof(gxFSKMacCounters))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = cosem_setStructure(data, 2)) != 0 ||
                    (ret = cosem_setUInt16(data, it->first)) != 0 ||
                    (ret = cosem_setUInt32(data, it->second)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 5:
        ret = cosem_setUInt32(data, object->repetitionsCounter);
        break;
    case 6:
        ret = cosem_setUInt32(data, object->transmissionsCounter);
        break;
    case 7:
        ret = cosem_setUInt32(data, object->crcOkFramesCounter);
        break;
    case 8:
        ret = cosem_setUInt32(data, object->crcNOkFramesCounter);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS

#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int cosem_getSFSKMacSynchronizationTimeouts(
    gxValueEventArg* e)
{
    int ret = 0;
    gxByteBuffer* data = e->value.byteArr;
    gxSFSKMacSynchronizationTimeouts* object = (gxSFSKMacSynchronizationTimeouts*)e->target;
    switch (e->index)
    {
    case 2:
        ret = cosem_setUInt16(data, object->searchInitiatorTimeout);
        break;
    case 3:
        ret = cosem_setUInt16(data, object->synchronizationConfirmationTimeout);
        break;
    case 4:
        ret = cosem_setUInt16(data, object->timeOutNotAddressed);
        break;
    case 5:
        ret = cosem_setUInt16(data, object->timeOutFrameNotOK);
        break;
    default:
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
int cosem_getSFSKPhyMacSetUp(
    gxValueEventArg* e)
{
    int ret = 0;
    gxByteBuffer* data = e->value.byteArr;
    gxSFSKPhyMacSetUp* object = (gxSFSKPhyMacSetUp*)e->target;
    switch (e->index)
    {
    case 2:
        ret = cosem_setEnum(data, object->initiatorElectricalPhase);
        break;
    case 3:
        ret = cosem_setEnum(data, object->deltaElectricalPhase);
        break;
    case 4:
        ret = cosem_setUInt8(data, object->maxReceivingGain);
        break;
    case 5:
        ret = cosem_setUInt8(data, object->maxTransmittingGain);
        break;
    case 6:
        ret = cosem_setUInt8(data, object->searchInitiatorThreshold);
        break;
    case 7:
    {
        if ((ret = cosem_setStructure(data, 2)) == 0 &&
            (ret = cosem_setUInt32(data, object->markFrequency)) == 0 &&
            (ret = cosem_setUInt32(data, object->spaceFrequency)) == 0)
        {
        }
        break;
    }
    case 8:
        ret = cosem_setUInt16(data, object->macAddress);
        break;
    case 9:
    {
        uint16_t pos;
        uint16_t* it;
        if ((ret = cosem_setArray(data, object->macGroupAddresses.size)) == 0)
        {
            for (pos = 0; pos != object->macGroupAddresses.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->macGroupAddresses, pos, (void**)&it)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->macGroupAddresses, pos, (void**)&it, sizeof(uint16_t))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = cosem_setUInt16(data, *it)) != 0)
                {
                    break;
                }
            }
        }
    }
    break;
    case 10:
        ret = cosem_setEnum(data, object->repeater);
        break;
    case 11:
        ret = cosem_setBoolean(data, object->repeaterStatus);
        break;
    case 12:
        ret = cosem_setUInt8(data, object->minDeltaCredit);
        break;
    case 13:
        ret = cosem_setUInt16(data, object->initiatorMacAddress);
        break;
    case 14:
        ret = cosem_setBoolean(data, object->synchronizationLocked);
        break;
    case 15:
        ret = cosem_setEnum(data, object->transmissionSpeed);
        break;
    default:
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        break;
    }
    return ret;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int cosem_getSFSKReportingSystemList(
    gxValueEventArg* e)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxByteBuffer* data = e->value.byteArr;
    gxSFSKReportingSystemList* object = (gxSFSKReportingSystemList*)e->target;
    if (e->index == 2)
    {
        uint16_t pos;
        gxByteBuffer* it;
        if ((ret = cosem_setArray(data, object->reportingSystemList.size)) == 0)
        {
            for (pos = 0; pos != object->reportingSystemList.size; ++pos)
            {
#ifndef DLMS_IGNORE_MALLOC
                if ((ret = arr_getByIndex(&object->reportingSystemList, pos, (void**)&it)) != 0)
                {
                    break;
                }
#else
                if ((ret = arr_getByIndex(&object->reportingSystemList, pos, (void**)&it, sizeof(uint16_t))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_MALLOC
                if ((ret = cosem_setOctetString(data, it)) != 0)
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
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
