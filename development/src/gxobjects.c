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
#ifndef GX_DLMS_MICROCONTROLLER
#include <stdio.h> //printf needs this or error is generated.
#endif
#endif
#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>

#include "../include/helpers.h"
#include "../include/gxobjects.h"
#include "../include/objectarray.h"

const unsigned char* obj_getLogicalName(gxObject* target)
{
    if (target == NULL)
    {
        return EMPTY_LN;
    }
    return target->logicalName;
}

#ifndef DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_MALLOC
//Create capture object with given attribute and data indexes.
gxTarget* co_init(
    unsigned char attributeIndex,
    unsigned char dataIndex)
{
    gxTarget* obj = (gxTarget*)gxmalloc(sizeof(gxTarget));
    obj->attributeIndex = attributeIndex;
    obj->dataIndex = dataIndex;
    return obj;
}
#endif //DLMS_IGNORE_MALLOC

int obj_clearProfileGenericBuffer(gxArray* buffer)
{
    //Clear data rows.
#ifndef DLMS_IGNORE_MALLOC
    variantArray* va;
    int pos, ret;
    for (pos = 0; pos != buffer->size; ++pos)
    {
        ret = arr_getByIndex(buffer, pos, (void**)&va);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        va_clear(va);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(buffer);
    return 0;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC

#if !(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA) && defined(DLMS_IGNORE_PUSH_SETUP))
int obj_clearPushObjectList(gxArray* buffer)
{
    int ret = 0;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    int pos;
    gxKey* kv;
    //Objects are not cleared because client owns them and clears them later.
    for (pos = 0; pos != buffer->size; ++pos)
    {
        ret = arr_getByIndex(buffer, pos, (void**)&kv);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        gxfree(kv->value);
    }
#endif // !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    arr_clear(buffer);
    return ret;
}
#endif //!(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_OBJECT_TYPE_PUSH_SETUP) && defined(DLMS_IGNORE_PUSH_SETUP))

#ifndef DLMS_IGNORE_ACCOUNT

int obj_clearCreditChargeConfigurations(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    gxCreditChargeConfiguration* it;
    int pos;
    //Clear push objects.
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        memset(it->creditReference, 0, sizeof(it->creditReference));
        memset(it->chargeReference, 0, sizeof(it->creditReference));
        it->collectionConfiguration = DLMS_CREDIT_COLLECTION_CONFIGURATION_DISCONNECTED;
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}

int obj_clearTokenGatewayConfigurations(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    gxTokenGatewayConfiguration* it;
    int pos;
    //Clear push objects.
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        memset(it->creditReference, 0, 6);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int obj_clearSapList(gxArray* buffer)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxSapItem* it;
    //Objects are not cleared because client owns them and clears them later.
    for (pos = 0; pos != buffer->size; ++pos)
    {
        ret = arr_getByIndex(buffer, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        bb_clear(&it->name);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(buffer);
    return ret;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#if !(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA))
int obj_clearProfileGenericCaptureObjects(gxArray* captureObjects)
{
    int ret = DLMS_ERROR_CODE_OK;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    int pos;
    gxKey* kv;
    //Objects are not cleared because client owns them and clears them later.
    for (pos = 0; pos != captureObjects->size; ++pos)
    {
        ret = arr_getByIndex(captureObjects, pos, (void**)&kv);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        gxfree(kv->value);
    }
#endif // !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    arr_clear(captureObjects);
    return ret;
}
#endif //!(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA))

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int obj_clearSeasonProfile(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxSeasonProfile* sp;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&sp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        bb_clear(&sp->name);
        bb_clear(&sp->weekName);
    };
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}

int obj_clearWeekProfileTable(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxWeekProfile* wp;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&wp);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        bb_clear(&wp->name);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}

int obj_clearDayProfileTable(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos, pos2;
    gxDayProfile* it;
    gxDayProfileAction* dp;
    for (pos = 0; pos != list->size; ++pos)
    {
        if ((ret = arr_getByIndex(list, pos, (void**)&it)) != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        for (pos2 = 0; pos2 != it->daySchedules.size; ++pos2)
        {
            if ((ret = arr_getByIndex(&it->daySchedules, pos2, (void**)&dp)) != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if (ret != 0)
            {
                break;
            }
        }
        arr_clear(&it->daySchedules);
    };
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_REGISTER_MONITOR
int obj_clearRegisterMonitorActions(gxArray* list)
{
    arr_clear(list);
    return 0;
}
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int obj_clearModemConfigurationInitialisationStrings(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxModemInitialisation* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        bb_clear(&it->request);
        bb_clear(&it->response);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION

#ifndef DLMS_IGNORE_SCHEDULE
int obj_clearScheduleEntries(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxScheduleEntry* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
        {
            break;
        }
        ba_clear(&it->execSpecDays);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_SCHEDULE

int obj_clearByteBufferList(gxArray* list)
{
#ifdef DLMS_IGNORE_MALLOC
    list->size = 0;
    return 0;
#else
    int pos, ret = 0;
    gxByteBuffer* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        bb_clear(it);
    }
    arr_clear(list);
    return ret;
#endif //DLMS_IGNORE_MALLOC
}

#ifndef DLMS_IGNORE_SCRIPT_TABLE
int obj_clearScriptTable(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos, pos2;
    gxScript* s;
    gxScriptAction* sa;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&s);
        if (ret != 0)
        {
            break;
        }
        for (pos2 = 0; pos2 != s->actions.size; ++pos2)
        {
            ret = arr_getByIndex(&s->actions, pos2, (void**)&sa);
            if (ret != 0)
            {
                break;
            }
            var_clear(&sa->parameter);
        }
        arr_clear(&s->actions);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_SCRIPT_TABLE

#ifndef DLMS_IGNORE_CHARGE
int obj_clearChargeTables(gxArray* list)
{
    int ret = DLMS_ERROR_CODE_OK;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxChargeTable* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0)
        {
            break;
        }
        bb_clear(&it->index);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_CHARGE

#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES))
int obj_clearRegisterActivationAssignment(objectArray* list)
{
    oa_empty(list);
    return 0;
}
#else
int obj_clearRegisterActivationAssignment(gxArray* list)
{
    arr_empty(list);
    return 0;
}
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))

int obj_clearRegisterActivationMaskList(gxArray* list)
{
#ifdef DLMS_IGNORE_MALLOC
    list->size = 0;
    return 0;
#else
#ifdef DLMS_COSEM_EXACT_DATA_TYPES
    int ret = 0, pos;
    gxRegisterActivationMask* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            break;
        }
        bb_clear(&it->name);
        bb_clear(&it->indexes);
    }
    arr_clear(list);
    return ret;
#else
    int ret = 0, pos;
    gxKey* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            break;
        }
        bb_clear((gxByteBuffer*)it->key);
        bb_clear((gxByteBuffer*)it->value);
    }
    arr_clearKeyValuePair(list);
    return ret;
#endif //DLMS_COSEM_EXACT_DATA_TYPES
#endif //DLMS_IGNORE_MALLOC
}
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_IP4_SETUP
int obj_clearIP4SetupOptions(gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxip4SetupIpOption* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        bb_clear(&it->data);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_PPP_SETUP
int obj_clearPPPSetupIPCPOptions(gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxpppSetupIPCPOption* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        var_clear(&it->data);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}

int obj_clearPPPSetupLCPOptions(gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxpppSetupLcpOption* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        var_clear(&it->data);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
int obj_clearActiveDevices(gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxActiveDevice* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        bb_clear(&it->macAddress);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL

int obj_clearUserList(gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxKey2* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        gxfree(it->value);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}

int obj_clearBitArrayList(
    gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    bitArray* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        ba_clear(it);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}

int obj_clearVariantList(
    gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    dlmsVARIANT* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        var_clear(it);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
//Clear available switches.
int obj_clearAvailableSwitches(
    gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxMacAvailableSwitch* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)&it);
        if (ret != 0)
        {
            return ret;
        }
        bb_clear(&it->sna);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA

#ifndef DLMS_IGNORE_SECURITY_SETUP
int obj_clearCertificateInfo(gxArray* arr)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    gxCertificateInfo* it;
    uint16_t pos;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
        {
            break;
        }
        gxfree(it->serialNumber);
        gxfree(it->issuer);
        gxfree(it->subject);
        gxfree(it->subjectAltName);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(arr);
    return ret;
}
#endif //DLMS_IGNORE_SECURITY_SETUP

void obj_clear(gxObject* object)
{
    int ret = 0;
    if (object != NULL)
    {
        if (object->access != NULL)
        {
            bb_clear(&object->access->attributeAccessModes);
            bb_clear(&object->access->methodAccessModes);
#ifndef DLMS_IGNORE_MALLOC
            gxfree(object->access);
            object->access = NULL;
#endif //DLMS_IGNORE_MALLOC
        }
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        object->shortName = 0;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        switch (object->objectType)
        {
#ifndef DLMS_IGNORE_DATA
        case DLMS_OBJECT_TYPE_DATA:
            var_clear(&((gxData*)object)->value);
            break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
        case DLMS_OBJECT_TYPE_REGISTER:
            var_clear(&((gxData*)object)->value);
            break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
        case DLMS_OBJECT_TYPE_CLOCK:
            //Clock object do not need to clean.
            break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
        case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
            arr_clear(&((gxActionSchedule*)object)->executionTime);
            break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
        case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
            bb_clear(&((gxActivityCalendar*)object)->calendarNameActive);
            bb_clear(&((gxActivityCalendar*)object)->calendarNamePassive);
            if ((ret = obj_clearSeasonProfile(&((gxActivityCalendar*)object)->seasonProfileActive)) != 0 ||
                (ret = obj_clearWeekProfileTable(&((gxActivityCalendar*)object)->weekProfileTableActive)) != 0 ||
                (ret = obj_clearDayProfileTable(&((gxActivityCalendar*)object)->dayProfileTableActive)) != 0 ||
                (ret = obj_clearSeasonProfile(&((gxActivityCalendar*)object)->seasonProfilePassive)) != 0 ||
                (ret = obj_clearWeekProfileTable(&((gxActivityCalendar*)object)->weekProfileTablePassive)) != 0 ||
                (ret = obj_clearDayProfileTable(&((gxActivityCalendar*)object)->dayProfileTablePassive)) != 0)
            {
            }
            break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
        case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
            oa_empty(&((gxAssociationLogicalName*)object)->objectList);
            ((gxAssociationLogicalName*)object)->xDLMSContextInfo.conformance = (DLMS_CONFORMANCE)0;
            bb_clear(&((gxAssociationLogicalName*)object)->xDLMSContextInfo.cypheringInfo);
            bb_clear(&((gxAssociationLogicalName*)object)->secret);
            obj_clearUserList(&((gxAssociationLogicalName*)object)->userList);
#ifdef DLMS_IGNORE_MALLOC
            ((gxAssociationLogicalName*)object)->currentUser.id = -1;
            ((gxAssociationLogicalName*)object)->currentUser.name[0] = 0;
#else
            if (((gxAssociationLogicalName*)object)->currentUser.value != NULL)
            {
                gxfree(((gxAssociationLogicalName*)object)->currentUser.value);
                ((gxAssociationLogicalName*)object)->currentUser.value = NULL;
            }
#endif //DLMS_IGNORE_MALLOC
            break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            oa_empty(&((gxAssociationShortName*)object)->objectList);
            bb_clear(&((gxAssociationShortName*)object)->secret);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
            break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
        case DLMS_OBJECT_TYPE_AUTO_ANSWER:
            arr_clearKeyValuePair(&((gxAutoAnswer*)object)->listeningWindow);
            break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
        case DLMS_OBJECT_TYPE_AUTO_CONNECT:
            arr_clearKeyValuePair(&((gxAutoConnect*)object)->callingWindow);
            arr_clearStrings(&((gxAutoConnect*)object)->destinations);
            break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
        case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
            var_clear(&((gxDemandRegister*)object)->currentAverageValue);
            var_clear(&((gxDemandRegister*)object)->lastAverageValue);
            var_clear(&((gxDemandRegister*)object)->status);
            break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
        case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
            bb_clear(&((gxMacAddressSetup*)object)->macAddress);
            break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
        case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
            var_clear(&((gxExtendedRegister*)object)->status);
            break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
        case DLMS_OBJECT_TYPE_GPRS_SETUP:
            bb_clear(&((gxGPRSSetup*)object)->apn);
            break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
        case DLMS_OBJECT_TYPE_SECURITY_SETUP:
            bb_clear(&((gxSecuritySetup*)object)->clientSystemTitle);
            bb_clear(&((gxSecuritySetup*)object)->serverSystemTitle);
            obj_clearCertificateInfo(&((gxSecuritySetup*)object)->certificates);
            break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
        case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
            //Do nothing.
            break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
        case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
            bb_clear(&((gxLocalPortSetup*)object)->password1);
            bb_clear(&((gxLocalPortSetup*)object)->password2);
            bb_clear(&((gxLocalPortSetup*)object)->password5);
            bb_clear(&((gxLocalPortSetup*)object)->deviceAddress);
            break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
        case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
            bb_clear(&((gxIecTwistedPairSetup*)object)->primaryAddresses);
            bb_clear(&((gxIecTwistedPairSetup*)object)->tabis);
            break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
        case DLMS_OBJECT_TYPE_IP4_SETUP:
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ((gxIp4Setup*)object)->dataLinkLayer = NULL;
#else
            memset(((gxIp4Setup*)object)->dataLinkLayerReference, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            var_clear(&((gxIp4Setup*)object)->value);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            arr_clear(&((gxIp4Setup*)object)->multicastIPAddress);
#else
            va_clear(&((gxIp4Setup*)object)->multicastIPAddress);
#endif //DLMS_IGNORE_MALLOC

            obj_clearIP4SetupOptions(&((gxIp4Setup*)object)->ipOptions);
            break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
        case DLMS_OBJECT_TYPE_IP6_SETUP:
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ((gxIp4Setup*)object)->dataLinkLayer = NULL;
#else
            memset(((gxIp6Setup*)object)->dataLinkLayerReference, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#ifdef DLMS_IGNORE_MALLOC
            arr_clear(&((gxIp6Setup*)object)->multicastIPAddress);
            arr_clear(&((gxIp6Setup*)object)->unicastIPAddress);
            arr_clear(&((gxIp6Setup*)object)->gatewayIPAddress);
            arr_clear(&((gxIp6Setup*)object)->neighborDiscoverySetup);
#else
            arr_clear(&((gxIp6Setup*)object)->multicastIPAddress);
            arr_clear(&((gxIp6Setup*)object)->unicastIPAddress);
            arr_clear(&((gxIp6Setup*)object)->gatewayIPAddress);
            arr_clear(&((gxIp6Setup*)object)->neighborDiscoverySetup);
#endif //DLMS_IGNORE_MALLOC
            break;
#endif //DLMS_IGNORE_IP6_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
        case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
            //Do nothing.
            break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
        case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        {
#ifndef DLMS_IGNORE_MALLOC
            gxImageActivateInfo* it;
            int pos = 0;
            while (pos != ((gxImageTransfer*)object)->imageActivateInfo.size)
            {
                if (arr_getByIndex(&((gxImageTransfer*)object)->imageActivateInfo, pos, (void**)&it) == 0)
                {
                    bb_clear(&it->identification);
                    bb_clear(&it->signature);
                }
                ++pos;
            }
#endif //DLMS_IGNORE_MALLOC
            ba_clear(&((gxImageTransfer*)object)->imageTransferredBlocksStatus);
            arr_clear(&((gxImageTransfer*)object)->imageActivateInfo);
            break;
        }
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
        case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
            //Do nothing.
            break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
        case DLMS_OBJECT_TYPE_LIMITER:
            var_clear(&((gxLimiter*)object)->thresholdActive);
            var_clear(&((gxLimiter*)object)->thresholdNormal);
            var_clear(&((gxLimiter*)object)->thresholdEmergency);
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
            arr_clear(&((gxLimiter*)object)->emergencyProfileGroupIDs);
#else
            va_clear(&((gxLimiter*)object)->emergencyProfileGroupIDs);
#endif //DLMS_IGNORE_MALLOC
            break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
        case DLMS_OBJECT_TYPE_MBUS_CLIENT:
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ((gxMBusClient*)object)->mBusPort = NULL;
#else
            memset(((gxMBusClient*)object)->mBusPortReference, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            arr_clearKeyValuePair(&((gxMBusClient*)object)->captureDefinition);
            break;
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
        case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
            ret = obj_clearModemConfigurationInitialisationStrings(&((gxModemConfiguration*)object)->initialisationStrings);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(ret == 0);
#endif
            arr_clearStrings(&((gxModemConfiguration*)object)->modemProfile);
            break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
        case DLMS_OBJECT_TYPE_PPP_SETUP:
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ((gxPppSetup*)object)->phy = NULL;
#else
            memset(((gxPppSetup*)object)->PHYReference, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            bb_clear(&((gxPppSetup*)object)->userName);
            bb_clear(&((gxPppSetup*)object)->password);
            obj_clearPPPSetupIPCPOptions(&((gxPppSetup*)object)->ipcpOptions);
            obj_clearPPPSetupLCPOptions(&((gxPppSetup*)object)->lcpOptions);
            break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
        case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
#ifndef DLMS_IGNORE_MALLOC
            obj_clearProfileGenericBuffer(&((gxProfileGeneric*)object)->buffer);
#endif //DLMS_IGNORE_MALLOC
            obj_clearProfileGenericCaptureObjects(&((gxProfileGeneric*)object)->captureObjects);
            break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
        case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES))
            oa_empty(&((gxRegisterActivation*)object)->registerAssignment);
#else
            obj_clearRegisterActivationAssignment(&((gxRegisterActivation*)object)->registerAssignment);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
            obj_clearRegisterActivationMaskList(&((gxRegisterActivation*)object)->maskList);
            bb_clear(&((gxRegisterActivation*)object)->activeMask);
            break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
        case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
            va_clear(&((gxRegisterMonitor*)object)->thresholds);
            obj_clearRegisterMonitorActions(&((gxRegisterMonitor*)object)->actions);
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
            obj_clearActiveDevices(&((gxZigBeeNetworkControl*)object)->activeDevices);
            break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
        case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
            obj_clearSapList(&((gxSapAssignment*)object)->sapAssignmentList);
            break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
        case DLMS_OBJECT_TYPE_SCHEDULE:
            obj_clearScheduleEntries(&((gxSchedule*)object)->entries);
            break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
        case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
            obj_clearScriptTable(&((gxScriptTable*)object)->scripts);
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
            arr_clear(&((gxSpecialDaysTable*)object)->entries);
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
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ((gxTcpUdpSetup*)object)->ipSetup = NULL;
#else
            memset(&((gxTcpUdpSetup*)object)->ipReference, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS

            break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
        case DLMS_OBJECT_TYPE_UTILITY_TABLES:
            bb_clear(&((gxUtilityTables*)object)->buffer);
            break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
        case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
            //Do nothing.
            break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_PUSH_SETUP
        case DLMS_OBJECT_TYPE_PUSH_SETUP:
            obj_clearPushObjectList(&((gxPushSetup*)object)->pushObjectList);
#ifdef DLMS_IGNORE_MALLOC
            ((gxPushSetup*)object)->destination.size = 0;
#else
            bb_clear(&((gxPushSetup*)object)->destination);
#endif //DLMS_IGNORE_MALLOC
            arr_clearKeyValuePair(&((gxPushSetup*)object)->communicationWindow);
            break;
        case DLMS_OBJECT_TYPE_DATA_PROTECTION:
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
            break;
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_ACCOUNT
        case DLMS_OBJECT_TYPE_ACCOUNT:
            arr_clear(&((gxAccount*)object)->creditReferences);
            arr_clear(&((gxAccount*)object)->chargeReferences);
            arr_clear(&((gxAccount*)object)->creditChargeConfigurations);
            arr_clear(&((gxAccount*)object)->tokenGatewayConfigurations);
#ifdef DLMS_IGNORE_MALLOC
            ((gxAccount*)object)->currency.name.size = 0;
#else
            bb_clear(&((gxAccount*)object)->currency.name);
#endif //DLMS_IGNORE_MALLOC
            break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
        case DLMS_OBJECT_TYPE_CREDIT:
            ((gxCredit*)object)->creditConfiguration = DLMS_CREDIT_CONFIGURATION_NONE;
            break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
        case DLMS_OBJECT_TYPE_CHARGE:
            ((gxCharge*)object)->chargeConfiguration = DLMS_CHARGE_CONFIGURATION_NONE;
            obj_clearChargeTables(&((gxCharge*)object)->unitChargeActive.chargeTables);
            obj_clearChargeTables(&((gxCharge*)object)->unitChargePassive.chargeTables);
            break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
        case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
            bb_clear(&((gxTokenGateway*)object)->token);
            obj_clearByteBufferList(&((gxTokenGateway*)object)->descriptions);
            ba_clear(&((gxTokenGateway*)object)->dataValue);
            break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
        case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
            gxfree(((gxGsmDiagnostic*)object)->operatorName);
            ((gxGsmDiagnostic*)object)->operatorName = NULL;
#else
            bb_clear(&((gxGsmDiagnostic*)object)->operatorName);
#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
            arr_clear(&((gxGsmDiagnostic*)object)->adjacentCells);
            break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
        case DLMS_OBJECT_TYPE_COMPACT_DATA:
            bb_clear(&((gxCompactData*)object)->buffer);
            obj_clearProfileGenericCaptureObjects(&((gxCompactData*)object)->captureObjects);
            bb_clear(&((gxCompactData*)object)->templateDescription);
            break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
        case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            ((gxParameterMonitor*)object)->changedParameter.target = NULL;
#else
            ((gxParameterMonitor*)object)->changedParameter.type = DLMS_OBJECT_TYPE_NONE;
            memset(((gxParameterMonitor*)object)->changedParameter.logicalName, 0, 6);
#endif //DLMS_IGNORE_OBJECT_POINTERS
            var_clear(&((gxParameterMonitor*)object)->changedParameter.value);
            obj_clearParametersList(&((gxParameterMonitor*)object)->parameters);
            break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
        case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
            break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
        case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
            break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
        case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
            break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
        case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
            bb_clear(&((gxPrimeNbOfdmPlcMacFunctionalParameters*)object)->sna);
            break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
        case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
            break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
        case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
            arr_clear(&((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object)->multicastEntries);
            arr_clear(&((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object)->switchTable);
            arr_clear(&((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object)->directTable);
            obj_clearAvailableSwitches(&((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object)->availableSwitches);
            arr_clear(&((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object)->communications);
            break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
        case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
            bb_clear(&((gxPrimeNbOfdmPlcApplicationsIdentification*)object)->firmwareVersion);
            break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
        case DLMS_OBJECT_TYPE_ARBITRATOR:
            arr_clear(&((gxArbitrator*)object)->actions);
            obj_clearBitArrayList(&((gxArbitrator*)object)->permissionsTable);
            obj_clearVariantList(&((gxArbitrator*)object)->weightingsTable);
            obj_clearBitArrayList(&((gxArbitrator*)object)->mostRecentRequestsTable);
            break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
        case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
            break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
        case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
            break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
        case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
            break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
        case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
            bb_clear(&((gxSFSKActiveInitiator*)object)->systemTitle);
            break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
        case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
            arr_clear(&((gxFSKMacCounters*)object)->synchronizationRegister);
            arr_clear(&((gxFSKMacCounters*)object)->broadcastFramesCounter);
            break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
        case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
            break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
        case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
            arr_clear(&((gxSFSKPhyMacSetUp*)object)->macGroupAddresses);
            break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
        case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
            obj_clearByteBufferList(&((gxSFSKReportingSystemList*)object)->reportingSystemList);
            break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
        case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        {
            gxfree(((gxTariffPlan*)object)->calendarName);
            ba_clear(&((gxTariffPlan*)object)->plan.weeklyActivation);
            arr_clear(&((gxTariffPlan*)object)->plan.specialDays);
            break;
        }
#endif //DLMS_ITALIAN_STANDARD
        default://Unknown type.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
            break;
        }
    }
#if !(defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    if (ret == 0)
    {
        //Remove warning.
    }
#endif

}

unsigned char obj_attributeCount(gxObject* object)
{
    unsigned char ret;
    switch (object->objectType)
    {
    case DLMS_OBJECT_TYPE_DATA:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = 3;
        break;
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = 10;
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        //user_list and current_user are in version 2.
        if (object->version > 1)
        {
            ret = 11;
        }
        //Security Setup Reference is from version 1.
        else if (object->version > 0)
        {
            ret = 9;
        }

        else
        {
            ret = 8;
        }
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        if (object->version < 2)
        {
            ret = 2;
        }
        else if (object->version < 3)
        {
            ret = 4;
        }
        else
        {
            ret = 6;
        }
        break;
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = 6;
        break;
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = 6;
        break;
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = 9;
        break;
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = 9;
        break;
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = 5;
        break;
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        if (object->version == 0)
        {
            ret = 5;
        }
        else
        {
            ret = 6;
        }
        break;
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = 9;
        break;
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = 9;
        break;
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = 5;
        break;
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = 10;
        break;
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = 10;
        break;
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = 5;
        break;
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = 7;
        break;
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = 11;
        break;
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = 12;
        break;
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = 5;
        break;
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = 8;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
        ret = 12;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        ret = 5;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        ret = 3;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = 2;
        break;
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = 2;
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = 2;
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = 6;
        break;
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = 7;
        break;
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
        ret = 6;
        break;
    case DLMS_OBJECT_TYPE_ACCOUNT:
        ret = 19;
        break;
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = 11;
        break;
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = 13;
        break;
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = 6;
        break;
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        return 6;
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = 5;
        break;
#endif //DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = 8;
        break;
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        ret = 4;
        break;
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = 3;
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = 5;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = 8;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = 14;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = 7;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = 6;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = 4;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = 6;
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = 1;
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ret = 9;
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ret = 6;
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ret = 2;
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ret = 8;
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ret = 5;
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ret = 14;
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ret = 2;
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    default:
        //Unknown type.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = 1;
        break;
    }
    return ret;
}

//Returns collection of attributes to read.
int obj_getAttributeIndexToRead(gxObject* object, gxByteBuffer* ba)
{
    unsigned char ch, pos;
    int ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    unsigned char cnt;
    bb_clear(ba);

    //If register unit and scaler are read first if not read yet
    if (object->objectType == DLMS_OBJECT_TYPE_REGISTER)
    {
#ifndef DLMS_IGNORE_REGISTER
        if (object->access == NULL)
        {
            if ((ret = bb_setUInt8(ba, 3)) == 0 &&
                (ret = bb_setUInt8(ba, 2)) == 0)
            {

            }
        }
        else
        {
            ret = 0;
            if (!((gxRegister*)object)->unitRead)
            {
                ret = bb_getUInt8ByIndex(&object->access->attributeAccessModes, 3 - 1, &ch);
                if (ret == 0 && ch != DLMS_ACCESS_MODE_NONE)
                {
                    ret = bb_setUInt8(ba, 3);
                }
            }
            if (ret == 0)
            {
                ret = bb_getUInt8ByIndex(&object->access->attributeAccessModes, 2 - 1, &ch);
                if (ret == 0 && ch != DLMS_ACCESS_MODE_NONE)
                {
                    ret = bb_setUInt8(ba, 2);
                }
            }
        }
#endif //DLMS_IGNORE_REGISTER
    }
    else if (object->objectType == DLMS_OBJECT_TYPE_EXTENDED_REGISTER)
    {
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
        if (!((gxExtendedRegister*)object)->unit == 0)
        {
            ret = bb_setUInt8(ba, 3);
        }
        if (ret == 0 && (ret = bb_setUInt8(ba, 2)) == 0 &&
            (ret = bb_setUInt8(ba, 4)) == 0 &&
            (ret = bb_setUInt8(ba, 5)) == 0)
        {

        }
#endif //DLMS_IGNORE_EXTENDED_REGISTER
    }
    else if (object->objectType == DLMS_OBJECT_TYPE_DEMAND_REGISTER)
    {
#ifndef DLMS_IGNORE_DEMAND_REGISTER
        if (!((gxDemandRegister*)object)->unitRead)
        {
            ret = bb_setUInt8(ba, 4);
        }
        else
        {
            ret = 0;
        }
        if (ret == 0 && (ret = bb_setUInt8(ba, 2)) == 0 &&
            (ret = bb_setUInt8(ba, 3)) == 0 &&
            (ret = bb_setUInt8(ba, 5)) == 0 &&
            (ret = bb_setUInt8(ba, 6)) == 0 &&
            (ret = bb_setUInt8(ba, 7)) == 0 &&
            (ret = bb_setUInt8(ba, 8)) == 0 &&
            (ret = bb_setUInt8(ba, 9)) == 0)
        {

        }
#endif //DLMS_IGNORE_DEMAND_REGISTER
    }
    else
    {
        cnt = obj_attributeCount(object) + 1;
        if ((ret = bb_capacity(ba, cnt)) == 0)
        {
            for (pos = 2; pos < cnt; ++pos)
            {
                if (object->access == NULL)
                {
                    if ((ret = bb_setUInt8(ba, pos)) != 0)
                    {
                        break;
                    }
                }
                else
                {
                    ret = bb_getUInt8ByIndex(&object->access->attributeAccessModes, pos - 1, &ch);
                    if (ret == 0 || ch != DLMS_ACCESS_MODE_NONE)
                    {
                        ret = bb_setUInt8(ba, pos);
                    }
                    else
                    {
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
                        printf("Attribute access is denied.");
#endif
                    }
                }
            }
        }
    }
    return ret;
}


unsigned char obj_methodCount(gxObject* object)
{
    unsigned char ret;
    switch (object->objectType)
    {
    case DLMS_OBJECT_TYPE_DATA:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        if (object->version > 1)
        {
            ret = 6;
        }
        else
        {
            ret = 4;
        }
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        if (object->version < 3)
        {
            ret = 8;
        }
        else
        {
            ret = 10;
        }
        break;
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = 6;
        break;
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = 3;
        break;
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = 4;
        break;
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = 8;
        break;
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = 3;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = 1;
        break;
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = 3;
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = 1;
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
        ret = 3;
        break;
    case DLMS_OBJECT_TYPE_ACCOUNT:
        ret = 3;
        break;
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = 3;
        break;
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = 5;
        break;
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = 1;
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        ret = 11;
        break;
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = 0;
        break;
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = 2;
        break;
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        ret = 2;
        break;
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = 1;
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = 1;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = 0;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = 0;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = 1;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = 1;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = 0;
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = 2;
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = 0;
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ret = 0;
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ret = 0;
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ret = 1;
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ret = 1;
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ret = 0;
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ret = 0;
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ret = 0;
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = 0;
        break;
#endif //DLMS_ITALIAN_STANDARD
    default:
        //Unknown type.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        ret = 0;
    }
    return ret;
}

#ifndef DLMS_IGNORE_DATA_PROTECTION
void init_ProtectionParameter(gxProtectionParameter* target)
{
    target->type = DLMS_PROTECTION_TYPE_AUTHENTICATION;
    target->options.info.identifiedOptions = DLMS_IDENTIFIED_KEY_INFO_OPTIONS_GLOBAL_UNICAST_ENCRYPTION_KEY;
    target->options.info.type = DLMS_KEY_INFO_TYPE_IDENTIFIED_KEY;
    BYTE_BUFFER_INIT(&target->options.id);
    BYTE_BUFFER_INIT(&target->options.info.agreedOptions.cipheredData);
    BYTE_BUFFER_INIT(&target->options.info.agreedOptions.parameters);
    BYTE_BUFFER_INIT(&target->options.info.wrappedKeyoptions.data);
    BYTE_BUFFER_INIT(&target->options.information);
    BYTE_BUFFER_INIT(&target->options.originator);
    BYTE_BUFFER_INIT(&target->options.recipient);
}

void clear_ProtectionParameter(gxProtectionParameter* target)
{
    target->type = DLMS_PROTECTION_TYPE_AUTHENTICATION;
    target->options.info.identifiedOptions = DLMS_IDENTIFIED_KEY_INFO_OPTIONS_GLOBAL_UNICAST_ENCRYPTION_KEY;
    target->options.info.type = DLMS_KEY_INFO_TYPE_IDENTIFIED_KEY;
    bb_clear(&target->options.id);
    bb_clear(&target->options.info.agreedOptions.cipheredData);
    bb_clear(&target->options.info.agreedOptions.parameters);
    bb_clear(&target->options.info.wrappedKeyoptions.data);
    bb_clear(&target->options.information);
    bb_clear(&target->options.originator);
    bb_clear(&target->options.recipient);
}
#endif //DLMS_IGNORE_DATA_PROTECTION


#ifndef DLMS_IGNORE_PARAMETER_MONITOR
int obj_clearParametersList(gxArray* buffer)
{
    int ret = 0;
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    int pos;
    gxKey* kv;
    //Objects are not cleared because client owns them and clears them later.
    for (pos = 0; pos != buffer->size; ++pos)
    {
        ret = arr_getByIndex(buffer, pos, (void**)&kv);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        gxfree(kv->value);
    }
#endif // !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    arr_clear(buffer);
    return ret;
}
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SERVER
void clock_updateDST(gxClock* object, gxtime* value)
{
    if (object->enabled && time_compare(&object->begin, value) != 1 && time_compare(&object->end, value) != -1)
    {
        object->status |= DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE;
    }
    else
    {
        object->status &= ~DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE;
    }
    object->time.status = object->status;
}
#endif //DLMS_IGNORE_SERVER

int clock_utcToMeterTime(gxClock* object, gxtime* value)
{
    if (value->deviation == 0 && object->timeZone != 0 && object->timeZone != -32768) //-32768 = 0x8000
    {
#ifdef DLMS_USE_UTC_TIME_ZONE
        time_addMinutes(value, object->timeZone);
#else
        time_addMinutes(value, -object->timeZone);
#endif //DLMS_USE_UTC_TIME_ZONE
        value->deviation = object->timeZone;
    }
    //If DST is enabled for the meter and it's not set for the time.
    if ((object->status & DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) != 0)
    {
#ifdef DLMS_USE_UTC_TIME_ZONE
        time_addMinutes(value, object->deviation);
        value->deviation += object->deviation;
#else
        time_addMinutes(value, object->deviation);
        value->deviation -= object->deviation;
#endif //DLMS_USE_UTC_TIME_ZONE
        value->status |= DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE;
    }
    else if ((object->status & DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) == 0)
    {
        value->status &= ~DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE;
    }
    else if ((object->status & DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) != 0 && (value->status & DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE) != 0)
    {
#ifdef DLMS_USE_UTC_TIME_ZONE
        value->deviation += object->deviation;
#else
        value->deviation -= object->deviation;
#endif //DLMS_USE_UTC_TIME_ZONE
    }
    return 0;
}

#endif //DLMS_IGNORE_CLOCK
