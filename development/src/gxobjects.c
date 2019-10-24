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
gxTarget * co_init(
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
        ret = arr_getByIndex(buffer, pos, (void**)& va);
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
#ifndef DLMS_IGNORE_MALLOC
    gxKey* it;
    int pos, ret;
    //Clear push objects.
    for (pos = 0; pos != buffer->size; ++pos)
    {
        ret = arr_getByIndex(buffer, pos, (void**)& it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        gxfree((gxTarget*)it->value);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(buffer);
    return 0;
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
        ret = arr_getByIndex(list, pos, (void**)& it);
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
        ret = arr_getByIndex(list, pos, (void**)& it);
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
        ret = arr_getByIndex(buffer, pos, (void**)& it);
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
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxKey* kv;
    //Objects are not cleared because client owns them and clears them later.
    for (pos = 0; pos != captureObjects->size; ++pos)
    {
        ret = arr_getByIndex(captureObjects, pos, (void**)& kv);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        gxfree(kv->value);
    }
#endif //DLMS_IGNORE_MALLOC
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
        ret = arr_getByIndex(list, pos, (void**)& sp);
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
        ret = arr_getByIndex(list, pos, (void**)& wp);
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
        ret = arr_getByIndex(list, pos, (void**)& it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        for (pos2 = 0; pos2 != it->daySchedules.size; ++pos2)
        {
            ret = arr_getByIndex(&it->daySchedules, pos2, (void**)& dp);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                return ret;
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
        ret = arr_getByIndex(list, pos, (void**)& it);
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
        ret = arr_getByIndex(list, pos, (void**)& s);
        if (ret != 0)
        {
            break;
        }
        for (pos2 = 0; pos2 != s->actions.size; ++pos2)
        {
            ret = arr_getByIndex(&s->actions, pos2, (void**)& sa);
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
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION


#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
int obj_clearRegisterActivationAssignment(objectArray* list)
{
    oa_clear(list);
    return 0;
}
#else
int obj_clearRegisterActivationAssignment(gxArray* list)
{
    arr_clear(list);
    return 0;
}
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))

int obj_clearRegisterActivationMaskList(gxArray* list)
{
#ifdef DLMS_IGNORE_MALLOC
    list->size = 0;
    return 0;
#else
    int pos, ret = 0;
    gxKey* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)& it);
        if (ret != 0)
        {
            return ret;
        }
        bb_clear((gxByteBuffer*)it->key);
        bb_clear((gxByteBuffer*)it->value);
    }
    arr_clearKeyValuePair(list);
    return ret;
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
        ret = arr_getByIndex(list, pos, (void**)& it);
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
        ret = arr_getByIndex(list, pos, (void**)& it);
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
        ret = arr_getByIndex(list, pos, (void**)& it);
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
        ret = arr_getByIndex(list, pos, (void**)& it);
        if (ret != 0)
        {
            return ret;
        }
        bb_clear(&it->macAddress);
        ba_clear(&it->status);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_CHARGE
int obj_clearChargeTables(gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxChargeTable* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)& it);
        if (ret != 0)
        {
            return ret;
        }
        bb_clear(&it->index);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(list);
    return ret;
}
#endif //DLMS_IGNORE_CHARGE

int obj_clearUserList(gxArray* list)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    int pos;
    gxKey2* it;
    for (pos = 0; pos != list->size; ++pos)
    {
        ret = arr_getByIndex(list, pos, (void**)& it);
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

#ifndef DLMS_IGNORE_SECURITY_SETUP
int obj_clearCertificateInfo(gxArray* arr)
{
    int ret = 0;
#ifndef DLMS_IGNORE_MALLOC
    gxCertificateInfo* it;
    unsigned short pos;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = arr_getByIndex(arr, pos, (void**)& it)) != 0)
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
            ret = obj_clearSeasonProfile(&((gxActivityCalendar*)object)->seasonProfileActive);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(ret == 0);
#endif
            ret = obj_clearWeekProfileTable(&((gxActivityCalendar*)object)->weekProfileTableActive);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(ret == 0);
#endif

            ret = obj_clearDayProfileTable(&((gxActivityCalendar*)object)->dayProfileTableActive);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(ret == 0);
#endif

            ret = obj_clearSeasonProfile(&((gxActivityCalendar*)object)->seasonProfilePassive);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(ret == 0);
#endif

            ret = obj_clearWeekProfileTable(&((gxActivityCalendar*)object)->weekProfileTablePassive);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(ret == 0);
#endif

            ret = obj_clearDayProfileTable(&((gxActivityCalendar*)object)->dayProfileTablePassive);
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(ret == 0);
#endif
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
            var_clear(&((gxDemandRegister*)object)->currentAvarageValue);
            var_clear(&((gxDemandRegister*)object)->lastAvarageValue);
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
            bb_clear(&((gxSecuritySetup*)object)->serverSystemTitle);
            bb_clear(&((gxSecuritySetup*)object)->clientSystemTitle);
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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
            break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
        case DLMS_OBJECT_TYPE_IP4_SETUP:
            memset(((gxIp4Setup*)object)->dataLinkLayerReference, 0, 6);
            var_clear(&((gxIp4Setup*)object)->value);
#ifdef DLMS_IGNORE_MALLOC
            arr_clear(&((gxIp4Setup*)object)->multicastIPAddress);
#else
            va_clear(&((gxIp4Setup*)object)->multicastIPAddress);
#endif //DLMS_IGNORE_MALLOC

            obj_clearIP4SetupOptions(&((gxIp4Setup*)object)->ipOptions);
            break;
#endif //DLMS_IGNORE_IP4_SETUP
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
                if (arr_getByIndex(&((gxImageTransfer*)object)->imageActivateInfo, pos, (void**)& it) == 0)
                {
                    bb_clear(&it->identification);
                    bb_clear(&it->signature);
                }
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
#ifdef DLMS_IGNORE_MALLOC
            arr_clear(&((gxLimiter*)object)->emergencyProfileGroupIDs);
#else
            va_clear(&((gxLimiter*)object)->emergencyProfileGroupIDs);
#endif //DLMS_IGNORE_MALLOC
            break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
        case DLMS_OBJECT_TYPE_MBUS_CLIENT:
            memset(((gxMBusClient*)object)->mBusPortReference, 0, 6);
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
            memset(((gxPppSetup*)object)->PHYReference, 0, 6);
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
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
            oa_clear(&((gxRegisterActivation*)object)->registerAssignment);
#else
            obj_clearRegisterActivationAssignment(&((gxRegisterActivation*)object)->registerAssignment);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))
            obj_clearRegisterActivationMaskList(&((gxRegisterActivation*)object)->maskList);
#ifdef DLMS_IGNORE_MALLOC
            memset(((gxRegisterActivation*)object)->activeMask, 0, 6);
#else
            bb_clear(&((gxRegisterActivation*)object)->activeMask);
#endif //DLMS_IGNORE_MALLOC

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
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            assert(0);
#endif
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
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
        case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
            arr_clearKeyValuePair(&((gxMessageHandler*)object)->listeningWindow);
            va_clear(&((gxMessageHandler*)object)->allowedSenders);
            arr_clear(&((gxMessageHandler*)object)->sendersAndActions);
            break;
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
        case DLMS_OBJECT_TYPE_PUSH_SETUP:
            obj_clearPushObjectList(&((gxPushSetup*)object)->pushObjectList);
#ifdef DLMS_IGNORE_MALLOC
            ((gxPushSetup*)object)->destination.size = 0;
#else
            if (((gxPushSetup*)object)->destination != NULL)
            {
                gxfree(((gxPushSetup*)object)->destination);
                ((gxPushSetup*)object)->destination = NULL;
            }
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
            gxfree(((gxAccount*)object)->currency.name);
            ((gxAccount*)object)->currency.name = NULL;
#endif //DLMS_IGNORE_MALLOC
            break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
        case DLMS_OBJECT_TYPE_CREDIT:
            ba_clear(&((gxCredit*)object)->creditConfiguration);
            break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
        case DLMS_OBJECT_TYPE_CHARGE:
            ba_clear(&((gxCharge*)object)->chargeConfiguration);
            arr_clear(&((gxCharge*)object)->unitChargeActive.chargeTables);
            arr_clear(&((gxCharge*)object)->unitChargePassive.chargeTables);
            break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
        case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
            bb_clear(&((gxTokenGateway*)object)->token);
            arr_clear(&((gxTokenGateway*)object)->descriptions);
            ba_clear(&((gxTokenGateway*)object)->dataValue);
            break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
        case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
#ifndef DLMS_IGNORE_MALLOC
            gxfree(((gxGsmDiagnostic*)object)->operatorName);
            ((gxGsmDiagnostic*)object)->operatorName = NULL;
#else
            bb_clear(&((gxGsmDiagnostic*)object)->operatorName);
#endif //DLMS_IGNORE_MALLOC
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
    switch (object->objectType)
    {
    case DLMS_OBJECT_TYPE_DATA:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_REGISTER:
    {
        return 3;
    }
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
    {
        return 10;
    }
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
    {
        //user_list and current_user are in version 2.
        if (object->version > 1)
            return 11;
        //Security Setup Reference is from version 1.
        if (object->version > 0)
            return 9;
        return 8;
    }
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
    {
        return 6;
    }
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
    {
        return 6;
    }
    case DLMS_OBJECT_TYPE_CLOCK:
    {
        return 9;
    }
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
    {
        return 9;
    }
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
    {
        return 5;
    }
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
    {
        if (object->version == 0)
        {
            return 5;
        }
        return 6;
    }
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
    {
        return 9;
    }
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
    {
        return 9;
    }
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_IP4_SETUP:
    {
        return 10;
    }
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
    {
        return 5;
    }
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
    {
        return 7;
    }
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_LIMITER:
    {
        return 11;
    }
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
    {
        return 12;
    }
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_PPP_SETUP:
    {
        return 5;
    }
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
    {
        return 8;
    }
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
    {
        return 12;
    }
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
    {
        return 5;
    }
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
    {
        return 3;
    }
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
    {
        return 2;
    }
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
    {
        return 2;
    }
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
    {
        return 2;
    }
#endif //DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
    {
        return 6;
    }
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
    {
        return 7;
    }
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
    {
        return 6;
    }
    case DLMS_OBJECT_TYPE_ACCOUNT:
        return 19;
    case DLMS_OBJECT_TYPE_CREDIT:
        return 11;
    case DLMS_OBJECT_TYPE_CHARGE:
        return 13;
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        return 6;
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        return 6;
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        return 5;
#endif //DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        return 8;
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        return 4;
    default:
        //Unknown type.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        break;
    }
    return 1;
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
    switch (object->objectType)
    {
    case DLMS_OBJECT_TYPE_DATA:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_REGISTER:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
    {
        if (object->version > 1)
        {
            return 6;
        }
        return 4;
    }
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
    {
        return 8;
    }
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_CLOCK:
    {
        return 6;
    }
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_IP4_SETUP:
    {
        return 3;
    }
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
    {
        return 4;
    }
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_LIMITER:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
    {
        return 8;
    }
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_PPP_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
    {
        return 3;
    }
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
    {
        return 1;
    }
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
    {
        return 3;
    }
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
    {
        return 1;
    }
#endif //DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
    {
        return 2;
    }
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
    {
        return 0;
    }
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
    {
        return 1;
    }
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
    {
        return 3;
    }
    case DLMS_OBJECT_TYPE_ACCOUNT:
        return 3;
    case DLMS_OBJECT_TYPE_CREDIT:
        return 3;
    case DLMS_OBJECT_TYPE_CHARGE:
        return 5;
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        return 1;
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        return 11;
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        return 0;
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        return 2;
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        return 2;
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        return 0;
#endif //DLMS_ITALIAN_STANDARD
    default:
        //Unknown type.
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(0);
#endif
        return 1;
    }
}

#if !defined(DLMS_IGNORE_MALLOC)
int obj_updateAttributeAccessModes(gxObject* object, variantArray* arr)
{
    unsigned char id;
    int ret;
    unsigned short pos, cnt;
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
#endif //!defined(DLMS_IGNORE_MALLOC)

#ifndef DLMS_IGNORE_DATA_PROTECTION
void init_ProtectionParameter(gxProtectionParameter* target)
{
    target->type = DLMS_PROTECTION_TYPE_AUTHENTICATION;
    target->options.info.identifiedOptions = DLMS_IDENTIFIED_KEY_INFO_OPTIONS_GLOBAL_UNICAST_ENCRYPTION_KEY;
    target->options.info.type = DLMS_KEY_INFO_TYPE_IDENTIFIED_KEY;
    bb_init(&target->options.id);
    bb_init(&target->options.info.agreedOptions.cipheredData);
    bb_init(&target->options.info.agreedOptions.parameters);
    bb_init(&target->options.info.wrappedKeyoptions.data);
    bb_init(&target->options.information);
    bb_init(&target->options.originator);
    bb_init(&target->options.recipient);
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
#ifndef DLMS_IGNORE_MALLOC
    gxKey* it;
    int pos, ret;
    //Clear push objects.
    for (pos = 0; pos != buffer->size; ++pos)
    {
        ret = arr_getByIndex(buffer, pos, (void**)& it);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        gxfree((gxTarget*)it->value);
    }
#endif //DLMS_IGNORE_MALLOC
    arr_clear(buffer);
    return 0;
}
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SERVER
void clock_updateDST(gxClock* object, gxtime* value)
{
    if (object->enabled && time_compare(&object->begin, value) != 1 && time_compare(value, &object->end) != -1)
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
