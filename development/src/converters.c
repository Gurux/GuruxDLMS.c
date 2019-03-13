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
#include "../include/gxmem.h"
#include "../include/converters.h"
#ifndef GX_DLMS_MICROCONTROLLER
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#include <stdio.h> //printf needs this or error is generated.
#endif
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include "../include/helpers.h"
#include "../include/objectarray.h"
#include "../include/gxkey.h"

void append(char* buff, char* data)
{
#if _MSC_VER > 1400
    strcat_s(buff, 30, data);
#else
    strcat(buff, data);
#endif
}

int obj_typeToString(DLMS_OBJECT_TYPE type, char* buff)
{
    buff[0] = '\0';
    switch (type)
    {
    case DLMS_OBJECT_TYPE_DATA:
        append(buff, "Data");
        break;
    case DLMS_OBJECT_TYPE_REGISTER:
        append(buff, "Register");
        break;
    case DLMS_OBJECT_TYPE_CLOCK:
        append(buff, "Clock");
        break;
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        append(buff, "Action Schedule");
        break;
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        append(buff, "Activity Calendar");
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        append(buff, "Association Logical Name");
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        append(buff, "Association Short Name");
        break;
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        append(buff, "Auto Answer");
        break;
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        append(buff, "Auto Connect");
        break;
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        append(buff, "Demand Register");
        break;
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        append(buff, "MAC Address Setup");
        break;
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        append(buff, "Extended Register");
        break;
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        append(buff, "GPRS Setup");
        break;
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        append(buff, "Security Setup");
        break;
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        append(buff, "IEC HDLC Setup");
        break;
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        append(buff, "IEC Local Port Setup");
        break;
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        append(buff, "IEC Twisted Pair Setup");
        break;
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        append(buff, "IP4 Setup");
        break;
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        append(buff, "MBUS Slave Port Setup");
        break;
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        append(buff, "Image Transfer");
        break;
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        append(buff, "Disconnect Control");
        break;
    case DLMS_OBJECT_TYPE_LIMITER:
        append(buff, "Limiter");
        break;
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        append(buff, "MBUS Client");
        break;
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        append(buff, "Modem Configuration");
        break;
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        append(buff, "PPP Setup");
        break;
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        append(buff, "Profile Generic");
        break;
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        append(buff, "Register Activation");
        break;
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        append(buff, "Register Monitor");
        break;
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        append(buff, "Register Table");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
        append(buff, "ZigBee SAS Startup");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        append(buff, "ZigBee SAS Join");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        append(buff, "ZigBee SAS APS Fragmentation");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        append(buff, "ZigBee Network Control");
        break;
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        append(buff, "SAP Assignment");
        break;
    case DLMS_OBJECT_TYPE_SCHEDULE:
        append(buff, "Schedule");
        break;
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        append(buff, "Script Table");
        break;
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
        append(buff, "SMTP Setup");
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        append(buff, "Special Days Table");
        break;
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        append(buff, "Status Mapping");
        break;
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        append(buff, "TCP UDP Setup");
        break;
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        append(buff, "Utility Tables");
        break;
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        append(buff, "MBUS Master Port Setup");
        break;
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        append(buff, "Message Handler");
        break;
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        append(buff, "Push Setup");
        break;
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
        append(buff, "Data Protection");
        break;
    case DLMS_OBJECT_TYPE_ACCOUNT:
        append(buff, "Account");
        break;
    case DLMS_OBJECT_TYPE_CREDIT:
        append(buff, "Credit");
        break;
    case DLMS_OBJECT_TYPE_CHARGE:
        append(buff, "Charge");
        break;
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        append(buff, "Token Gateway");
        break;
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        append(buff, "Tariff Plan");
        break;
#endif //DLMS_ITALIAN_STANDARD
    default: //Unknown type.
        break;
    }
    return 0;
}

const char* obj_getUnitAsString(unsigned char unit)
{
    switch (unit)
    {
    case 0:
        return "None";
    case 1:
        return "Year";
        break;
    case 2:
        return "Month";
        break;
    case 3:
        return "Week";
        break;
    case 4:
        return "Day";
        break;
    case 5:
        return "Hour";
        break;
    case 6:
        return "Minute";
        break;
    case 7:
        return "Second";
        break;
    case 8:
        return "PhaseAngle";
        break;
    case 9:
        return "Temperature";
        break;
    case 10:
        return "LocalCurrency";
        break;
    case 11:
        return "Length";
        break;
    case 12:
        return "Speed";
        break;
    case 13:
        return "Volume";
        break;
    case 14:
        return "CorrectedVolume";
        break;
    case 15:
        return "VolumeFlux";
        break;
    case 16:
        return "CorrectedVolumeFlux";
        break;
    case 17:
        return "VolumeFlux";
        break;
    case 18:
        return "CorrectedVolumeFlux";
        break;
    case 19:
        return "Volume";
        break;
    case 20:
        return "MassKg";
        break;
    case 21:
        return "Force";
        break;
    case 22:
        return "Energy";
        break;
    case 23:
        return "PressurePascal";
        break;
    case 24:
        return "PressureBar";
        break;
    case 25:
        return "Energy";
        break;
    case 26:
        return "ThermalPower";
        break;
    case 27:
        return "ActivePower";
        break;
    case 28:
        return "ApparentPower";
        break;
    case 29:
        return "ReactivePower";
        break;
    case 30:
        return "ActiveEnergy";
        break;
    case 31:
        return "ApparentEnergy";
        break;
    case 32:
        return "ReactiveEnergy";
        break;
    case 33:
        return "Current";
        break;
    case 34:
        return "ElectricalCharge";
        break;
    case 35:
        return "Voltage";
        break;
    case 36:
        return "Electrical field strength E V/m";
        break;
    case 37:
        return "Capacity C farad C/V = As/V";
        break;
    case 38:
        return "Resistance";
        break;
    case 39:
        return "Resistivity";
        break;
    case 40:
        return "Magnetic flux F weber Wb = Vs";
        break;
    case 41:
        return "Induction T tesla Wb/m2";
        break;
    case 42:
        return "Magnetic field strength H A/m";
        break;
    case 43:
        return "Inductivity L henry H = Wb/A";
        break;
    case 44:
        return "Frequency";
        break;
    case 45:
        return "ActiveEnergy";
        break;
    case 46:
        return "ReactiveEnergy";
        break;
    case 47:
        return "ApparentEnergy";
        break;
    case 48:
        return "V260*60s";
        break;
    case 49:
        return "A260*60s";
        break;
    case 50:
        return "Mass";
        break;
    case 51:
        return "ConductanceSiemens";
        break;
    case 254:
        return "OtherUnit";
        break;
    case 255:
        return "NoUnit";
        break;
    }
    return "";
}
#ifndef DLMS_IGNORE_DATA
int obj_DataToString(gxData* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    ret = var_toString(&object->value, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
int obj_RegisterToString(gxRegister* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 3 Value: Scaler: ");
    bb_addIntAsString(&ba, object->scaler);
    bb_addString(&ba, " Unit: ");
    bb_addString(&ba, obj_getUnitAsString(object->unit));
    bb_addString(&ba, "\r\nIndex: 2 Value: ");
    ret = var_toString(&object->value, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
int obj_clockToString(gxClock* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    ret = time_toString(&object->time, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->timeZone);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    ret = time_toString(&object->begin, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    ret = time_toString(&object->end, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->deviation);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->enabled);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->clockBase);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int obj_ScriptTableToString(gxScriptTable* object, char** buff)
{
    int ret, pos, pos2;
    gxByteBuffer ba;
    gxScript *s;
    gxScriptAction *sa;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    for (pos = 0; pos != object->scripts.size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        ret = arr_getByIndex(&object->scripts, pos, (void**)&s);
        if (ret != 0)
        {
            return ret;
        }
        bb_addIntAsString(&ba, s->id);
        bb_addString(&ba, "\r\n");
        for (pos2 = 0; pos2 != s->actions.size; ++pos2)
        {
            ret = arr_getByIndex(&s->actions, pos2, (void**)&sa);
            if (ret != 0)
            {
                return ret;
            }
            if (pos2 != 0)
            {
                bb_addString(&ba, ", ");
            }
            bb_addIntAsString(&ba, sa->type);
            bb_addString(&ba, " ");
            bb_addIntAsString(&ba, sa->objectType);
            bb_addString(&ba, " ");
            hlp_appendLogicalName(&ba, sa->logicalName);
            bb_addString(&ba, " ");
            bb_addIntAsString(&ba, sa->index);
            bb_addString(&ba, " ");
            ret = var_toString(&sa->parameter, &ba);
            if (ret != 0)
            {
                return ret;
            }
        }
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int obj_specialDaysTableToString(gxSpecialDaysTable* object, char** buff)
{
    int ret, pos;
    gxSpecialDay *sd;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    for (pos = 0; pos != object->entries.size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        ret = arr_getByIndex(&object->entries, pos, (void**)&sd);
        if (ret != 0)
        {
            return ret;
        }
        bb_addIntAsString(&ba, sd->index);
        bb_addString(&ba, " ");
        ret = time_toString(&sd->date, &ba);
        if (ret != 0)
        {
            return ret;
        }
        bb_addString(&ba, " ");
        bb_addIntAsString(&ba, sd->dayId);
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int obj_TcpUdpSetupToString(gxTcpUdpSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->port);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_attachString(&ba, bb_toString(&object->ipReference));
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->maximumSegmentSize);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->maximumSimultaneousConnections);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->inactivityTimeout);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int obj_mBusMasterPortSetupToString(gxMBusMasterPortSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->commSpeed);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
int obj_messageHandlerToString(
    gxMessageHandler* object,
    char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    //TODO: bb_addIntAsString(&ba, object->listeningWindow);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    //TODO: bb_addIntAsString(&ba, object->allowedSenders);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    //TODO: bb_addIntAsString(&ba, object->sendersAndActions);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
int obj_pushSetupToString(gxPushSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    //TODO: bb_addIntAsString(&ba, object->pushObjectList);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    //TODO: bb_addIntAsString(&ba, object->sendDestinationAndMethod);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    //TODO: bb_addIntAsString(&ba, object->communicationWindow);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->randomisationStartInterval);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->numberOfRetries);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->repetitionDelay);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_AUTO_CONNECT
int obj_autoConnectToString(gxAutoConnect* object, char** buff)
{
    gxKey* k;
    int pos, ret;
    gxByteBuffer ba, *dest;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->mode);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->repetitions);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->repetitionDelay);
    bb_addString(&ba, "\r\nIndex: 5 Value: [");
    for (pos = 0; pos != object->callingWindow.size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        ret = arr_getByIndex(&object->callingWindow, pos, (void**)&k);
        if (ret != 0)
        {
            return ret;
        }
        time_toString((gxtime*)k->key, &ba);
        bb_addString(&ba, " ");
        time_toString((gxtime*)k->value, &ba);
    }
    bb_addString(&ba, "]");
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    for (pos = 0; pos != object->destinations.size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        ret = arr_getByIndex(&object->destinations, pos, (void**)&dest);
        if (ret != 0)
        {
            return ret;
        }
        bb_set2(&ba, dest, 0, bb_size(dest));
    }
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int obj_seasonProfileToString(gxArray* arr, gxByteBuffer* ba)
{
    gxSeasonProfile* it;
    int pos, ret;
    bb_addString(ba, "[");
    for (pos = 0; pos != arr->size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        ret = arr_getByIndex(arr, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_attachString(ba, bb_toString(&it->name));
        bb_addString(ba, " ");
        time_toString(&it->start, ba);
        bb_addString(ba, " ");
        bb_attachString(ba, bb_toString(&it->weekName));
    }
    bb_addString(ba, "]");
    return 0;
}
int obj_weekProfileToString(gxArray* arr, gxByteBuffer* ba)
{
    gxWeekProfile* it;
    int pos, ret;
    bb_addString(ba, "[");
    for (pos = 0; pos != arr->size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        ret = arr_getByIndex(arr, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_attachString(ba, bb_toString(&it->name));
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->monday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->tuesday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->wednesday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->thursday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->friday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->saturday);
        bb_addString(ba, " ");
        bb_addIntAsString(ba, it->sunday);
    }
    bb_addString(ba, "]");
    return 0;
}

int obj_dayProfileToString(gxArray* arr, gxByteBuffer* ba)
{
    gxDayProfile* dp;
    gxDayProfileAction *it;
    int pos, pos2, ret;
    bb_addString(ba, "[");
    for (pos = 0; pos != arr->size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        ret = arr_getByIndex(arr, pos, (void**)&dp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_addIntAsString(ba, dp->dayId);
        bb_addString(ba, "[");
        for (pos2 = 0; pos2 != dp->daySchedules.size; ++pos2)
        {
            if (pos2 != 0)
            {
                bb_addString(ba, ", ");
            }
            ret = arr_getByIndex(&dp->daySchedules, pos2, (void**)&it);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            hlp_appendLogicalName(ba, it->scriptLogicalName);
            bb_addString(ba, " ");
            bb_addIntAsString(ba, it->scriptSelector);
            bb_addString(ba, " ");
            time_toString(&it->startTime, ba);
        }
        bb_addString(ba, "]");
    }
    bb_addString(ba, "]");
    return 0;
}

int obj_activityCalendarToString(gxActivityCalendar* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_attachString(&ba, bb_toString(&object->calendarNameActive));
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    ret = obj_seasonProfileToString(&object->seasonProfileActive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    ret = obj_weekProfileToString(&object->weekProfileTableActive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    ret = obj_dayProfileToString(&object->dayProfileTableActive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_attachString(&ba, bb_toString(&object->calendarNamePassive));
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    ret = obj_seasonProfileToString(&object->seasonProfilePassive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    ret = obj_weekProfileToString(&object->weekProfileTablePassive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    ret = obj_dayProfileToString(&object->dayProfileTablePassive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_SECURITY_SETUP
int obj_securitySetupToString(gxSecuritySetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->securityPolicy);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->securitySuite);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_attachString(&ba, bb_toString(&object->serverSystemTitle));
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_attachString(&ba, bb_toString(&object->clientSystemTitle));
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int obj_hdlcSetupToString(gxIecHdlcSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->communicationSpeed);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->windowSizeTransmit);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->windowSizeReceive);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->maximumInfoLengthTransmit);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->maximumInfoLengthReceive);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->interCharachterTimeout);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->inactivityTimeout);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->deviceAddress);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int obj_localPortSetupToString(gxLocalPortSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->defaultMode);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->defaultBaudrate);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->proposedBaudrate);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->responseTime);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_attachString(&ba, bb_toString(&object->deviceAddress));
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_attachString(&ba, bb_toString(&object->password1));
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_attachString(&ba, bb_toString(&object->password2));
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    bb_attachString(&ba, bb_toString(&object->password5));
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_DEMAND_REGISTER
int obj_demandRegisterToString(gxDemandRegister* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    ret = var_toString(&object->currentAvarageValue, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    ret = var_toString(&object->lastAvarageValue, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 4 Value: Scaler: ");
    bb_addDoubleAsString(&ba, hlp_getScaler(object->scaler));
    bb_addString(&ba, " Unit: ");
    bb_addString(&ba, obj_getUnitAsString(object->unit));

    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    ret = var_toString(&object->status, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    time_toString(&object->captureTime, &ba);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    time_toString(&object->startTimeCurrent, &ba);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->period);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->numberOfPeriods);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int obj_registerActivationToString(gxRegisterActivation* object, char** buff)
{
    int pos, ret;
    gxObjectDefinition* od;
    gxKey *it;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    for (pos = 0; pos != object->registerAssignment.size; ++pos)
    {
        ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_addIntAsString(&ba, od->classId);
        bb_addString(&ba, " ");
        hlp_appendLogicalName(&ba, od->logicalName);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(ret == 0);
#endif
    }
    bb_addString(&ba, "]\r\n");
    bb_addString(&ba, "Index: 3 Value: [");
    for (pos = 0; pos != object->maskList.size; ++pos)
    {
        ret = arr_getByIndex(&object->maskList, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_attachString(&ba, bb_toString((gxByteBuffer*)it->key));
        bb_addString(&ba, " ");
        bb_attachString(&ba, bb_toString((gxByteBuffer*)it->value));
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
void actionItemToString(gxActionItem* item, gxByteBuffer* ba)
{
    hlp_appendLogicalName(ba, item->logicalName);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, item->scriptSelector);
}
int obj_registerMonitorToString(gxRegisterMonitor* object, char** buff)
{
    char str[30];
    int pos, ret;
    dlmsVARIANT *tmp;
    gxByteBuffer ba;
    gxActionSet* as;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    for (pos = 0; pos != object->thresholds.size; ++pos)
    {
        ret = va_getByIndex(&object->thresholds, pos, &tmp);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        ret = var_toString(tmp, &ba);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(ret == 0);
#endif
    }
    bb_addString(&ba, "]\r\nIndex: 3 Value: ");
    hlp_appendLogicalName(&ba, object->monitoredValue.logicalName);
    bb_addString(&ba, " ");
    obj_typeToString(object->monitoredValue.objectType, str);
    bb_addString(&ba, str);
    bb_addString(&ba, " ");
    bb_addIntAsString(&ba, object->monitoredValue.attributeIndex);

    bb_addString(&ba, "\r\nIndex: 4 Value: [");
    for (pos = 0; pos != object->actions.size; ++pos)
    {
        ret = arr_getByIndex(&object->actions, pos, (void**)&as);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        actionItemToString(&as->actionUp, &ba);
        bb_addString(&ba, " ");
        actionItemToString(&as->actionDown, &ba);
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int obj_actionScheduleToString(gxActionSchedule* object, char** buff)
{
    int pos = 0, ret;
    gxtime* tm;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    hlp_appendLogicalName(&ba, object->executedScriptLogicalName);
    bb_addString(&ba, " ");
    bb_addIntAsString(&ba, object->executedScriptSelector);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->type);

    bb_addString(&ba, "\r\nIndex: 4 Value: [");
    for (pos = 0; pos != object->executionTime.size; ++pos)
    {
        ret = arr_getByIndex(&object->executionTime, pos, (void**)&tm);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        else
        {
            if (pos != 0)
            {
                bb_addString(&ba, ", ");
            }
            ret = time_toString(tm, &ba);
            if (ret != 0)
            {
                return ret;
            }
        }
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int obj_sapAssignmentToString(gxSapAssignment* object, char** buff)
{
    int pos, ret;
    gxSapItem *it;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    for (pos = 0; pos != object->sapAssignmentList.size; ++pos)
    {
        ret = arr_getByIndex(&object->sapAssignmentList, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_addIntAsString(&ba, it->id);
        bb_addString(&ba, " ");
        bb_set2(&ba, &it->name, 0, bb_size(&it->name));
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_AUTO_ANSWER
int obj_autoAnswerToString(gxAutoAnswer* object, char** buff)
{
    int pos, ret = 0;
    gxKey* it;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->mode);
    bb_addString(&ba, "\r\nIndex: 3 Value: [");
    for (pos = 0; pos != object->listeningWindow.size; ++pos)
    {
        ret = arr_getByIndex(&object->listeningWindow, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        time_toString((gxtime*)it->key, &ba);
        bb_addString(&ba, " ");
        time_toString((gxtime*)it->value, &ba);
    }
    bb_addString(&ba, "]\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->numberOfCalls);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->numberOfRingsInListeningWindow);
    bb_addString(&ba, " ");
    bb_addIntAsString(&ba, object->numberOfRingsOutListeningWindow);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int obj_ip4SetupToString(gxIp4Setup* object, char** buff)
{
    int ret, pos;
    dlmsVARIANT *tmp;
    gxip4SetupIpOption *ip;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_attachString(&ba, bb_toString(&object->dataLinkLayerReference));
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->ipAddress);
    bb_addString(&ba, "\r\nIndex: 4 Value: [");
    for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
    {
        ret = va_getByIndex(&object->multicastIPAddress, pos, &tmp);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        ret = var_toString(tmp, &ba);
        if (ret != 0)
        {
            return ret;
        }
    }
    bb_addString(&ba, "]\r\nIndex: 5 Value: [");
    for (pos = 0; pos != object->ipOptions.size; ++pos)
    {
        ret = arr_getByIndex(&object->ipOptions, pos, (void**)&ip);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_addIntAsString(&ba, ip->type);
        bb_addString(&ba, " ");
        bb_attachString(&ba, bb_toString(&ip->data));
    }
    bb_addString(&ba, "]\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->subnetMask);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->gatewayIPAddress);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->useDHCP);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->primaryDNSAddress);
    bb_addString(&ba, "\r\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->secondaryDNSAddress);
    bb_addString(&ba, "\r\nIndex: 11 Value: ");
    var_toString(&object->value, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int obj_mbusSlavePortSetupToString(gxMbusSlavePortSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->defaultBaud);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->availableBaud);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->addressState);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->busAddress);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int obj_imageTransferToString(gxImageTransfer* object, char** buff)
{
    int pos, ret;
    gxImageActivateInfo* it;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->imageBlockSize);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_attachString(&ba, ba_toString(&object->imageTransferredBlocksStatus));
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->imageFirstNotTransferredBlockNumber);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->imageTransferEnabled);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->imageTransferStatus);
    bb_addString(&ba, "\r\nIndex: 7 Value: [");
    for (pos = 0; pos != object->imageActivateInfo.size; ++pos)
    {
        ret = arr_getByIndex(&object->imageActivateInfo, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_addIntAsString(&ba, it->size);
        bb_addString(&ba, " ");
        bb_attachString(&ba, bb_toHexString(&it->identification));
        bb_addString(&ba, " ");
        bb_attachString(&ba, bb_toHexString(&it->signature));
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int obj_disconnectControlToString(gxDisconnectControl* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->outputState);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->controlState);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->controlMode);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
int obj_limiterToString(gxLimiter* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addString(&ba, " ");
    if (object->monitoredValue != NULL)
    {
        hlp_appendLogicalName(&ba, object->monitoredValue->logicalName);
        bb_addString(&ba, ": ");
        bb_addIntAsString(&ba, object->selectedAttributeIndex);
    }
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    ret = var_toString(&object->thresholdActive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    ret = var_toString(&object->thresholdNormal, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    ret = var_toString(&object->thresholdEmergency, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->minOverThresholdDuration);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->minUnderThresholdDuration);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->emergencyProfile.id);
    bb_addString(&ba, " ");
    time_toString(&object->emergencyProfile.activationTime, &ba);
    bb_addString(&ba, " ");
    bb_addIntAsString(&ba, object->emergencyProfile.duration);

    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    if ((ret = va_toString(&object->emergencyProfileGroupIDs, &ba)) != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->emergencyProfileActive);
    bb_addString(&ba, "\r\nIndex: 11 Value: ");
    actionItemToString(&object->actionOverThreshold, &ba);
    bb_addString(&ba, "\r\nIndex: 12 Value: ");
    actionItemToString(&object->actionUnderThreshold, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int obj_mBusClientToString(gxMBusClient* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->capturePeriod);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->primaryAddress);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_attachString(&ba, bb_toString(&object->mBusPortReference));
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    //TODO: bb_addIntAsString(&ba, object->captureDefinition);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->identificationNumber);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->manufacturerID);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->dataHeaderVersion);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->deviceType);
    bb_addString(&ba, "\r\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->accessNumber);
    bb_addString(&ba, "\r\nIndex: 11 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\r\nIndex: 12 Value: ");
    bb_addIntAsString(&ba, object->alarm);
    bb_addString(&ba, "\r\nIndex: 13 Value: ");
    bb_addIntAsString(&ba, object->primaryAddress);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int obj_modemConfigurationToString(gxModemConfiguration* object, char** buff)
{
    int pos, ret;
    gxModemInitialisation* mi;
    gxByteBuffer ba, *it;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->communicationSpeed);
    bb_addString(&ba, "\r\nIndex: 3 Value: [");
    for (pos = 0; pos != object->initialisationStrings.size; ++pos)
    {
        ret = arr_getByIndex(&object->initialisationStrings, pos, (void**)&mi);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_attachString(&ba, bb_toString(&mi->request));
        bb_addString(&ba, " ");
        bb_attachString(&ba, bb_toString(&mi->response));
        bb_addString(&ba, " ");
        bb_addIntAsString(&ba, mi->delay);
    }
    bb_addString(&ba, "]\r\nIndex: 4 Value: [");
    for (pos = 0; pos != object->modemProfile.size; ++pos)
    {
        ret = arr_getByIndex(&object->modemProfile, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_attachString(&ba, bb_toString(it));
    }
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int obj_macAddressSetupToString(gxMacAddressSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_attachString(&ba, bb_toString(&object->macAddress));
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
void qualityOfServiceToString(gxQualityOfService* target, gxByteBuffer* ba)
{
    bb_addIntAsString(ba, target->precedence);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->delay);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->reliability);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->peakThroughput);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, target->meanThroughput);
    bb_addString(ba, " ");
}

int obj_GPRSSetupToString(gxGPRSSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_attachString(&ba, bb_toString(&object->apn));
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->pinCode);
    bb_addString(&ba, "\r\nIndex: 4 Value: [");
    qualityOfServiceToString(&object->defaultQualityOfService, &ba);
    bb_addString(&ba, "]\r\nIndex: 5 Value: [");
    qualityOfServiceToString(&object->requestedQualityOfService, &ba);
    bb_addString(&ba, "]\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
int obj_extendedRegisterToString(gxExtendedRegister* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    ret = var_toString(&object->base.value, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 3 Value: Scaler: ");
    bb_addIntAsString(&ba, object->base.scaler);
    bb_addString(&ba, " Unit: ");
    bb_addString(&ba, obj_getUnitAsString(object->base.unit));
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    ret = var_toString(&object->status, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    time_toString(&object->captureTime, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

int obj_objectsToString(gxByteBuffer* ba, objectArray* objects)
{
    char str[30];
    unsigned short pos;
    int ret = DLMS_ERROR_CODE_OK;
    gxObject* it;
    for (pos = 0; pos != objects->size; ++pos)
    {
        ret = oa_getByIndex(objects, pos, &it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
        ret = obj_typeToString((DLMS_OBJECT_TYPE)it->objectType, str);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(ret == 0);
#endif
        bb_addString(ba, str);
        bb_addString(ba, " ");
        hlp_appendLogicalName(ba, it->logicalName);
    }
    return ret;
}

int obj_rowsToString(gxByteBuffer* ba, gxArray* buffer)
{
    dlmsVARIANT *tmp;
    variantArray* va;
    int r, c, ret;
    for (r = 0; r != buffer->size; ++r)
    {
        ret = arr_getByIndex(buffer, r, (void**)&va);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        for (c = 0; c != va->size; ++c)
        {
            if (c != 0)
            {
                bb_addString(ba, " | ");
            }
            ret = va_getByIndex(va, c, &tmp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
            ret = var_toString(tmp, ba);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
            }
        }
        bb_addString(ba, "\r\n");
    }
    return 0;
}

void obj_applicationContextNameToString(gxByteBuffer* ba, gxApplicationContextName* object)
{
    hlp_appendLogicalName(ba, object->logicalName);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->jointIsoCtt);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->country);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->countryName);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->identifiedOrganization);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->dlmsUA);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->applicationContext);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->contextId);
}

void obj_xDLMSContextTypeToString(gxByteBuffer* ba, gxXDLMSContextType* object)
{
    bb_addIntAsString(ba, object->conformance);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->maxReceivePduSize);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->maxSendPpuSize);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->qualityOfService);
    bb_addString(ba, " ");
    bb_attachString(ba, bb_toString(&object->cypheringInfo));
}

void obj_authenticationMechanismNameToString(gxByteBuffer* ba, gxAuthenticationMechanismName* object)
{
    bb_addIntAsString(ba, object->jointIsoCtt);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->country);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->countryName);
    bb_addIntAsString(ba, object->identifiedOrganization);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->dlmsUA);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->authenticationMechanismName);
    bb_addString(ba, " ");
    bb_addIntAsString(ba, object->mechanismId);
}

int obj_associationLogicalNameToString(gxAssociationLogicalName* object, char** buff)
{
    int pos, ret = 0;
    gxKey2 *it;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    obj_objectsToString(&ba, &object->objectList);
    bb_addString(&ba, "]\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->clientSAP);
    bb_addString(&ba, "/");
    bb_addIntAsString(&ba, object->serverSAP);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    obj_applicationContextNameToString(&ba, &object->applicationContextName);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    obj_xDLMSContextTypeToString(&ba, &object->xDLMSContextInfo);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    obj_authenticationMechanismNameToString(&ba, &object->authenticationMechanismName);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_attachString(&ba, bb_toString(&object->secret));
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->associationStatus);
    //Security Setup Reference is from version 1.
    if (object->base.version > 0)
    {
        bb_addString(&ba, "\r\nIndex: 9 Value: ");
        hlp_appendLogicalName(&ba, object->securitySetupReference);
    }
    if (object->base.version > 1)
    {
        bb_addString(&ba, "\r\nIndex: 10 Value: [\r\n");
        for (pos = 0; pos != object->userList.size; ++pos)
        {
            if ((ret = arr_getByIndex(&object->userList, pos, (void**)&it)) != 0)
            {
                return ret;
            }
            bb_addString(&ba, "ID: ");
            bb_addIntAsString(&ba, it->key);
            bb_addString(&ba, " Name: ");
            bb_addString(&ba, (char*)it->value);
        }
        bb_addString(&ba, "]\r\n");
    }
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int obj_associationShortNameToString(gxAssociationShortName* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    obj_objectsToString(&ba, &object->objectList);
    bb_addString(&ba, "]\r\nIndex: 3 Value: ");
    //TODO: Show access rights.
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    hlp_appendLogicalName(&ba, object->securitySetupReference);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int obj_pppSetupToString(gxPppSetup* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    //TODO: ipcpOptions
    bb_addString(&ba, "]\r\nIndex: 3 Value: ");
    bb_attachString(&ba, bb_toString(&object->PHYReference));
    bb_addString(&ba, "\r\nIndex: 4 Value: [");
    //TODO: lcpOptions
    bb_addString(&ba, "]\r\nIndex: 5 Value: ");
    bb_attachString(&ba, bb_toString(&object->userName));
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_attachString(&ba, bb_toString(&object->password));
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->authentication);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_PPP_SETUP

int obj_CaptureObjectsToString(gxByteBuffer* ba, gxArray* objects)
{
    gxKey *kv;
    int ret, pos;
    objectArray tmp;
    oa_init(&tmp);
    oa_capacity(&tmp, objects->size);
    for (pos = 0; pos != objects->size; ++pos)
    {
        ret = arr_getByIndex(objects, pos, (void**)&kv);
        if (ret != 0)
        {
            return ret;
        }
        oa_push(&tmp, (gxObject*)kv->key);
    }
    ret = obj_objectsToString(ba, &tmp);
    if (ret != 0)
    {
        return ret;
    }
    oa_empty(&tmp);
    return 0;
}

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int obj_ProfileGenericToString(gxProfileGeneric* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [\r\n");
    obj_rowsToString(&ba, &object->buffer);
    bb_addString(&ba, "]\r\nIndex: 3 Value: [");
    obj_CaptureObjectsToString(&ba, &object->captureObjects);
    bb_addString(&ba, "]\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->capturePeriod);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->sortMethod);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    if (object->sortObject != NULL)
    {
        ret = hlp_appendLogicalName(&ba, object->sortObject->logicalName);
        if (ret != 0)
        {
            return ret;
        }
    }
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->entriesInUse);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->profileEntries);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int obj_accountToString(gxAccount* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: [\r\n");
    bb_addIntAsString(&ba, object->paymentMode);
    bb_addString(&ba, "\r\n");
    bb_addIntAsString(&ba, object->accountStatus);
    bb_addString(&ba, "]");
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->currentCreditInUse);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->currentCreditStatus);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->availableCredit);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->amountToClear);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->clearanceThreshold);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->aggregatedDebt);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    //TODO:
//    bb_addIntAsString(&ba, object->creditReferences);
    bb_addString(&ba, "\r\nIndex: 10 Value: ");
    //  bb_addIntAsString(&ba, object->chargeReferences);
    bb_addString(&ba, "\r\nIndex: 11 Value: ");
    //bb_addIntAsString(&ba, &object->creditChargeConfigurations);

    bb_addString(&ba, "\r\nIndex: 12 Value: ");
    //  bb_addIntAsString(&ba, object->tokenGatewayConfigurations);
    bb_addString(&ba, "\r\nIndex: 13 Value: ");
    time_toString(&object->accountActivationTime, &ba);
    bb_addString(&ba, "\r\nIndex: 14 Value: ");
    time_toString(&object->accountClosureTime, &ba);
    bb_addString(&ba, "\r\nIndex: 15 Value: ");
    // bb_addIntAsString(&ba, object->currency);
    bb_addString(&ba, "\r\nIndex: 16 Value: ");
    bb_addIntAsString(&ba, object->lowCreditThreshold);
    bb_addString(&ba, "\r\nIndex: 17 Value: ");
    bb_addIntAsString(&ba, object->nextCreditAvailableThreshold);
    bb_addString(&ba, "\r\nIndex: 18 Value: ");
    bb_addIntAsString(&ba, object->maxProvision);
    bb_addString(&ba, "\r\nIndex: 19 Value: ");
    bb_addIntAsString(&ba, object->maxProvisionPeriod);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int obj_creditToString(gxCredit* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->currentCreditAmount);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->type);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->priority);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->warningThreshold);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->limit);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    //    bb_addIntAsString(&ba, object->creditConfiguration);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->presetCreditAmount);
    bb_addString(&ba, "\r\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->creditAvailableThreshold);
    bb_addString(&ba, "\r\nIndex: 11 Value: ");
    time_toString(&object->period, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
int obj_chargeToString(gxCharge* object, char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->totalAmountPaid);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->chargeType);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->priority);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    //TODO:
//    bb_addIntAsString(&ba, object->unitChargeActive);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    //  bb_addIntAsString(&ba, object->unitChargePassive);
    bb_addString(&ba, "\r\nIndex: 7 Value: ");
    time_toString(&object->unitChargeActivationTime, &ba);
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->period);
    bb_addString(&ba, "\r\nIndex: 9 Value: ");
    //  bb_addIntAsString(&ba, object->chargeConfiguration);
    bb_addString(&ba, "\r\nIndex: 10 Value: ");
    time_toString(&object->lastCollectionTime, &ba);
    bb_addString(&ba, "\r\nIndex: 11 Value: ");
    bb_addIntAsString(&ba, object->lastCollectionAmount);
    bb_addString(&ba, "\r\nIndex: 12 Value: ");
    bb_addIntAsString(&ba, object->totalAmountRemaining);
    bb_addString(&ba, "\r\nIndex: 13 Value: ");
    bb_addIntAsString(&ba, object->proportion);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
int obj_tokenGatewayToString(
    gxTokenGateway* object,
    char** buff)
{
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int obj_GsmDiagnosticToString(gxGsmDiagnostic* object, char** buff)
{
    int pos, ret;
    gxAdjacentCell *it;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value:");
    bb_addString(&ba, object->operatorName);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->circuitSwitchStatus);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->packetSwitchStatus);
    bb_addString(&ba, "\r\nIndex: 6 Value: [");
    bb_addString(&ba, "Cell ID: ");
    bb_addIntAsString(&ba, object->cellInfo.cellId);
    bb_addString(&ba, "Location ID: ");
    bb_addIntAsString(&ba, object->cellInfo.locationId);
    bb_addString(&ba, "Signal Quality: ");
    bb_addIntAsString(&ba, object->cellInfo.signalQuality);
    bb_addString(&ba, "BER: ");
    bb_addIntAsString(&ba, object->cellInfo.ber);
    bb_addString(&ba, "]");

    bb_addString(&ba, "\r\nIndex: 7 Value:[\r\n");
    for (pos = 0; pos != object->adjacentCells.size; ++pos)
    {
        ret = arr_getByIndex(&object->adjacentCells, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        bb_addIntAsString(&ba, it->cellId);
        bb_addString(&ba, ":");
        bb_addIntAsString(&ba, it->signalQuality);
    }
    bb_addString(&ba, "]\r\n");
    bb_addString(&ba, "\r\nIndex: 8 Value: ");
    time_toString(&object->captureTime, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
int obj_CompactDataToString(gxCompactData* object, char** buff)
{
    gxByteBuffer ba;
    char* tmp;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value:");
    tmp = bb_toHexString(&object->buffer);
    bb_addString(&ba, tmp);
    gxfree(tmp);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    obj_CaptureObjectsToString(&ba, &object->captureObjects);
    bb_addString(&ba, "\r\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->templateId);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    tmp = bb_toHexString(&object->templateDescription);
    bb_addString(&ba, tmp);
    gxfree(tmp);
    bb_addString(&ba, "\r\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->captureMethod);
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_COMPACT_DATA
#ifdef DLMS_ITALIAN_STANDARD
int obj_TariffPlanToString(gxTariffPlan* object, char** buff)
{
    int pos, ret;
    gxAdjacentCell *it;
    gxByteBuffer ba;
    bb_init(&ba);
    bb_addString(&ba, "Index: 2 Value:");
    bb_addString(&ba, object->calendarName);
    bb_addString(&ba, "\r\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->enabled);
    bb_addString(&ba, "\r\nIndex: 5 Value: ");
    time_toString(&object->activationTime, &ba);
    bb_addString(&ba, "\r\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_ITALIAN_STANDARD

int obj_toString(gxObject* object, char** buff)
{
    int ret = 0;
    switch (object->objectType)
    {
#ifndef DLMS_IGNORE_DATA
    case DLMS_OBJECT_TYPE_DATA:
        ret = obj_DataToString((gxData*)object, buff);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = obj_RegisterToString((gxRegister*)object, buff);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = obj_clockToString((gxClock*)object, buff);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = obj_actionScheduleToString((gxActionSchedule*)object, buff);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = obj_activityCalendarToString((gxActivityCalendar*)object, buff);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = obj_associationLogicalNameToString((gxAssociationLogicalName*)object, buff);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        ret = obj_associationShortNameToString((gxAssociationShortName*)object, buff);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = obj_autoAnswerToString((gxAutoAnswer*)object, buff);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = obj_autoConnectToString((gxAutoConnect*)object, buff);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = obj_demandRegisterToString((gxDemandRegister*)object, buff);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = obj_macAddressSetupToString((gxMacAddressSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = obj_extendedRegisterToString((gxExtendedRegister*)object, buff);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
#ifndef DLMS_IGNORE_GPRS_SETUP
        ret = obj_GPRSSetupToString((gxGPRSSetup*)object, buff);
#endif //DLMS_IGNORE_GPRS_SETUP
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = obj_securitySetupToString((gxSecuritySetup*)object, buff);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = obj_hdlcSetupToString((gxIecHdlcSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = obj_localPortSetupToString((gxLocalPortSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = obj_ip4SetupToString((gxIp4Setup*)object, buff);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = obj_mbusSlavePortSetupToString((gxMbusSlavePortSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = obj_imageTransferToString((gxImageTransfer*)object, buff);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = obj_disconnectControlToString((gxDisconnectControl*)object, buff);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = obj_limiterToString((gxLimiter*)object, buff);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = obj_mBusClientToString((gxMBusClient*)object, buff);
        break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = obj_modemConfigurationToString((gxModemConfiguration*)object, buff);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = obj_pppSetupToString((gxPppSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = obj_ProfileGenericToString((gxProfileGeneric*)object, buff);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = obj_registerActivationToString((gxRegisterActivation*)object, buff);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = obj_registerMonitorToString((gxRegisterMonitor*)object, buff);
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
        ret = obj_sapAssignmentToString((gxSapAssignment*)object, buff);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = obj_ScriptTableToString((gxScriptTable*)object, buff);
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
        ret = obj_specialDaysTableToString((gxSpecialDaysTable*)object, buff);
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
        ret = obj_TcpUdpSetupToString((gxTcpUdpSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = obj_mBusMasterPortSetupToString((gxMBusMasterPortSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        ret = obj_messageHandlerToString((gxMessageHandler*)object, buff);
        break;
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = obj_pushSetupToString((gxPushSetup*)object, buff);
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
        ret = obj_accountToString((gxAccount*)object, buff);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = obj_creditToString((gxCredit*)object, buff);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = obj_chargeToString((gxCharge*)object, buff);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = obj_tokenGatewayToString((gxTokenGateway*)object, buff);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = obj_GsmDiagnosticToString((gxGsmDiagnostic*)object, buff);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = obj_CompactDataToString((gxCompactData*)object, buff);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = obj_TariffPlanToString((gxTariffPlan*)object, buff);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default: //Unknown type.
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

#endif //GX_DLMS_MICROCONTROLLER

const char* err_toString(int err)
{
    switch (err) {
    case DLMS_ERROR_CODE_REJECTED:
        return "Rejected.";
    case         DLMS_ERROR_CODE_FALSE:
    case    DLMS_ERROR_CODE_OK:
        return "OK.";
    case    DLMS_ERROR_CODE_HARDWARE_FAULT:
        return "Access Error : Device reports a hardware fault.";
    case    DLMS_ERROR_CODE_TEMPORARY_FAILURE:
        return "Access Error : Device reports a temporary failure.";
    case    DLMS_ERROR_CODE_READ_WRITE_DENIED:
        return "Access Error : Device reports Read-Write denied.";
    case    DLMS_ERROR_CODE_UNDEFINED_OBJECT:
        return "Access Error : Device reports a undefined object.";
    case    DLMS_ERROR_CODE_INCONSISTENT_OBJECT:
        return "Access Error : Device reports a inconsistent Class or Object.";
    case    DLMS_ERROR_CODE_UNAVAILABLE_OBJECT:
        return "Access Error : Device reports a unavailable object.";
    case    DLMS_ERROR_CODE_UNMATCH_TYPE:
        return "Access Error : Device reports a unmatched type.";
    case    DLMS_ERROR_CODE_VIOLATED:
        return "Access Error : Device reports scope of access violated.";
    case    DLMS_ERROR_CODE_BLOCK_UNAVAILABLE:
        return "Access Error : Data Block Unavailable.";
    case    DLMS_ERROR_CODE_READ_ABORTED:
        return "Access Error : Long Get Or Read Aborted.";
    case    DLMS_ERROR_CODE_NO_LONG_GET_OR_READ_IN_PROGRESS:
        return "Access Error : No Long Get Or Read In Progress.";
    case    DLMS_ERROR_CODE_WRITE_ABORTED:
        return "Access Error : Long Set Or Write Aborted.";
    case    DLMS_ERROR_CODE_NO_LONG_SET_OR_WRITE_IN_PROGRESS:
        return "Access Error : No Long Set Or Write In Progress.";
    case    DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID:
        return "Access Error : Data Block Number Invalid.";
    case    DLMS_ERROR_CODE_OTHER_REASON:
        return "Access Error : Other Reason.";
    case    DLMS_ERROR_CODE_UNKNOWN:
        return "Unknown error.";
    case    DLMS_ERROR_CODE_SEND_FAILED:
        return "Data send failed.";
    case    DLMS_ERROR_CODE_RECEIVE_FAILED:
        return "Data receive failed.";
    case    DLMS_ERROR_CODE_NOT_IMPLEMENTED:
        return "Not implmented.";
    case    DLMS_ERROR_CODE_DLMS_SECURITY_NOT_IMPLEMENTED:
        return "Secure connection is not supported.";
    case    DLMS_ERROR_CODE_INVALID_COMMAND:
        return "Invalid DLMS command.";
    case    DLMS_ERROR_CODE_INVALID_BLOCK_NUMBER:
        return "Invalid Block number.";
    case    DLMS_ERROR_CODE_INVALID_PARAMETER:
        return "Invalid parameter.";
    case    DLMS_ERROR_CODE_NOT_INITIALIZED:
        return "Server is not initialized.";
    case    DLMS_ERROR_CODE_OUTOFMEMORY:
        return "Not enough memory available.";
    case    DLMS_ERROR_CODE_NOT_REPLY:
        return "Packet is not a reply for a send packet.";
    case    DLMS_ERROR_CODE_INVALID_LOGICAL_NAME:
        return "Invalid Logical Name.";
    case    DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS:
        return "Client HDLC Address is not set.";
    case    DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS:
        return "Server HDLC Address is not set.";
    case    DLMS_ERROR_CODE_INVALID_DATA_FORMAT:
        return "Not a HDLC frame.";
    case    DLMS_ERROR_CODE_INVALID_VERSION_NUMBER:
        return "Invalid DLMS version number.";
    case    DLMS_ERROR_CODE_CLIENT_ADDRESS_NO_NOT_MATCH:
        return "Client addresses do not match.";
    case    DLMS_ERROR_CODE_SERVER_ADDRESS_NO_NOT_MATCH:
        return "Server addresses do not match.";
    case    DLMS_ERROR_CODE_WRONG_CRC:
        return "CRC do not match.";
    case    DLMS_ERROR_CODE_INVALID_RESPONSE:
        return "Invalid response.";
    case    DLMS_ERROR_CODE_INVALID_TAG:
        return "Invalid Tag.";
    case    DLMS_ERROR_CODE_ENCODING_FAILED:
        return "Encoding failed. Not enough data.";
    case    DLMS_ERROR_CODE_REJECTED_PERMAMENT:
        return "Rejected permament.";
    case    DLMS_ERROR_CODE_REJECTED_TRANSIENT:
        return "Rejected transient.";
    case    DLMS_ERROR_CODE_NO_REASON_GIVEN:
        return "No Reason Given.";
    case    DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED:
        return "Application context name not supported.";
    case    DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED:
        return "Authentication mechanism name not recognised.";
    case    DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_REQUIRED:
        return "Authentication mechanism name required.";
    case    DLMS_ERROR_CODE_AUTHENTICATION_FAILURE:
        return "Authentication failure.";
    case    DLMS_ERROR_CODE_AUTHENTICATION_REQUIRED:
        return "Authentication Required.";
    case    DLMS_ERROR_CODE_INVALID_FRAME_NUMBER:
        return "Invalid frame number";
    default:
        return "Unknown error.";
    }
}