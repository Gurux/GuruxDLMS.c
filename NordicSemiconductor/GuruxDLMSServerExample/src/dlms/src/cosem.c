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

#include "../include/cosem.h"
#include "../include/helpers.h"
#include "../include/lnparameters.h"
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#include "../include/snparameters.h"
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

//cosem_getObjectSize returns zero if object is not supported.
int cosem_getObjectSize(DLMS_OBJECT_TYPE type)
{
    int size = 0;
    switch (type)
    {
#ifndef DLMS_IGNORE_DATA
    case DLMS_OBJECT_TYPE_DATA:
        size = sizeof(gxData);
        break;
#endif //DLMS_IGNORE_DATA
#ifndef DLMS_IGNORE_REGISTER
    case DLMS_OBJECT_TYPE_REGISTER:
        size = sizeof(gxRegister);
        break;
#endif //DLMS_IGNORE_REGISTER
#ifndef DLMS_IGNORE_CLOCK
    case DLMS_OBJECT_TYPE_CLOCK:
        size = sizeof(gxClock);
        break;
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        size = sizeof(gxActionSchedule);
        break;
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        size = sizeof(gxActivityCalendar);
        break;
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        size = sizeof(gxAssociationLogicalName);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        size = sizeof(gxAssociationShortName);
        break;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_AUTO_ANSWER
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        size = sizeof(gxAutoAnswer);
        break;
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_AUTO_CONNECT
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        size = sizeof(gxAutoConnect);
        break;
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        size = sizeof(gxDemandRegister);
        break;
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        size = sizeof(gxMacAddressSetup);
        break;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        size = sizeof(gxExtendedRegister);
        break;
#endif //DLMS_IGNORE_EXTENDED_REGISTER
#ifndef DLMS_IGNORE_GPRS_SETUP
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        size = sizeof(gxGPRSSetup);
        break;
#endif //DLMS_IGNORE_GPRS_SETUP
#ifndef DLMS_IGNORE_SECURITY_SETUP
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        size = sizeof(gxSecuritySetup);
        break;
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        size = sizeof(gxIecHdlcSetup);
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        size = sizeof(gxLocalPortSetup);
        break;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        size = sizeof(gxIecTwistedPairSetup);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        size = sizeof(gxIp4Setup);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        size = sizeof(gxMbusSlavePortSetup);
        break;
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        size = sizeof(gxImageTransfer);
        break;
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        size = sizeof(gxDisconnectControl);
        break;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL
#ifndef DLMS_IGNORE_LIMITER
    case DLMS_OBJECT_TYPE_LIMITER:
        size = sizeof(gxLimiter);
        break;
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        size = sizeof(gxMBusClient);
        break;
#endif //
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        size = sizeof(gxModemConfiguration);
        break;
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_PPP_SETUP
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        size = sizeof(gxPppSetup);
        break;
#endif //DLMS_IGNORE_PPP_SETUP
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        size = sizeof(gxProfileGeneric);
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        size = sizeof(gxRegisterActivation);
        break;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        size = sizeof(gxRegisterMonitor);
        break;
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_REGISTER_TABLE
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        size = sizeof(gxRegisterTable);
        break;
#endif //DLMS_IGNORE_REGISTER_TABLE
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
        size = sizeof(gxZigBeeSasStartup);
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        size = sizeof(gxZigBeeSasJoin);
        break;
#endif //ZIG_BEE_SAS_JOIN
#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        size = sizeof(gxZigBeeSasApsFragmentation);
        break;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        size = sizeof(gxZigBeeNetworkControl);
        break;
#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        size = sizeof(gxSapAssignment);
        break;
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        size = sizeof(gxSchedule);
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        size = sizeof(gxScriptTable);
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SMTP_SETUP
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
        size = sizeof(gxSmtpSetup);
        break;
#endif //DLMS_IGNORE_SMTP_SETUP
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        size = sizeof(gxSpecialDaysTable);
        break;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_STATUS_MAPPING
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        size = sizeof(gxStatusMapping);
        break;
#endif //DLMS_IGNORE_STATUS_MAPPING
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        size = sizeof(gxTcpUdpSetup);
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_UTILITY_TABLES
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        size = sizeof(gxUtilityTables);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        size = sizeof(gxMBusMasterPortSetup);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#ifndef DLMS_IGNORE_MESSAGE_HANDLER
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        size = sizeof(gxMessageHandler);
        break;
#endif //DLMS_IGNORE_MESSAGE_HANDLER
#ifndef DLMS_IGNORE_PUSH_SETUP
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        size = sizeof(gxPushSetup);
        break;
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_DATA_PROTECTION
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
        size = sizeof(gxDataProtection);
        break;
#endif //DLMS_IGNORE_DATA_PROTECTION
#ifndef DLMS_IGNORE_ACCOUNT
    case DLMS_OBJECT_TYPE_ACCOUNT:
        size = sizeof(gxAccount);
        break;
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
    case DLMS_OBJECT_TYPE_CREDIT:
        size = sizeof(gxCredit);
        break;
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    case DLMS_OBJECT_TYPE_CHARGE:
        size = sizeof(gxCharge);
        break;
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        size = sizeof(gxTokenGateway);
        break;
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        size = sizeof(gxGsmDiagnostic);
        break;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC
#ifndef DLMS_IGNORE_COMPACT_DATA
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        size = sizeof(gxCompactData);
        break;
#endif //DLMS_IGNORE_COMPACT_DATA
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        size = sizeof(gxTariffPlan);
        break;
#endif //DLMS_ITALIAN_STANDARD
    default:
        return 0;
    }
#if _MSC_VER > 1400
    {
        /*
        char buff[40];
            obj_typeToString(type, buff);
            printf("Object %s size: %d\r\n", buff, size);
        */
    }
#endif
    return size;
}

int cosem_createObject(DLMS_OBJECT_TYPE type, gxObject** object)
{
    int ret;
    unsigned char ln[] = { 0,0,40,0,0,255 };
    int size = cosem_getObjectSize(type);
    if (size == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    *object = (gxObject*)gxcalloc(1, size);
    if (*object == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    (*object)->objectType = type;
    /*
    #if _MSC_VER > 1400
    {
    char buff[40];
    obj_typeToString(type, buff);
    printf("Object %s size: %d\r\n", buff, (int)_msize(*object));
    }
    #endif
    */

    if (type == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
    {
    }
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    else if (type == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
    {
    }
    else
    {
        memset(ln, 0, 6);
    }
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    ret = cosem_init2(*object, type, ln);
    (*object)->free = 1;
    return ret;
}

int cosem_createObject2(
    DLMS_OBJECT_TYPE type,
    const char* ln,
    gxObject** object)
{
    int ret = cosem_createObject(type, object);
    if (ret != 0)
    {
        return ret;
    }
    hlp_setLogicalName((*object)->logicalName, ln);
    return 0;
}

int cosem_setLogicalName(
    gxObject* object,
    unsigned char* value)
{
    memcpy(object->logicalName, value, 6);
    return 0;
}

int cosem_init(
    gxObject* object,
    DLMS_OBJECT_TYPE type,
    const char* logicalNameString)
{
    unsigned char ln[6];
    hlp_setLogicalName(ln, logicalNameString);
    return cosem_init2(object, type, ln);
}

int cosem_init2(
    gxObject* object,
    DLMS_OBJECT_TYPE type,
    const unsigned char* ln)
{
    int size = cosem_getObjectSize(type);
    if (size == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    memset(object, 0, size);
    object->objectType = type;
    object->logicalName[0] = ln[0];
    object->logicalName[1] = ln[1];
    object->logicalName[2] = ln[2];
    object->logicalName[3] = ln[3];
    object->logicalName[4] = ln[4];
    object->logicalName[5] = ln[5];
    //Set default values, if any.
    switch (type)
    {
    case DLMS_OBJECT_TYPE_DATA:
        break;
    case DLMS_OBJECT_TYPE_REGISTER:
        break;
    case DLMS_OBJECT_TYPE_CLOCK:
        break;
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        break;
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
    {
        gxAssociationLogicalName * it = (gxAssociationLogicalName*)object;
        object->version = 2;
        it->xDLMSContextInfo.dlmsVersionNumber = 6;
        it->applicationContextName.jointIsoCtt = 2;
        it->applicationContextName.country = 16;
        it->applicationContextName.countryName = 756;
        it->applicationContextName.identifiedOrganization = 5;
        it->applicationContextName.dlmsUA = 8;
        it->applicationContextName.applicationContext = 1;
        it->applicationContextName.contextId = DLMS_APPLICATION_CONTEXT_NAME_LOGICAL_NAME;
        it->authenticationMechanismName.jointIsoCtt = 2;
        it->authenticationMechanismName.country = 16;
        it->authenticationMechanismName.countryName = 756;
        it->authenticationMechanismName.identifiedOrganization = 5;
        it->authenticationMechanismName.dlmsUA = 8;
        it->authenticationMechanismName.authenticationMechanismName = 2;
    }
    break;
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        break;
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        break;
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        break;
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        break;
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        break;
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        break;
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        break;
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        object->version = 1;
        ((gxIecHdlcSetup*)object)->communicationSpeed = DLMS_BAUD_RATE_9600;
        ((gxIecHdlcSetup*)object)->windowSizeTransmit = 1;
        ((gxIecHdlcSetup*)object)->windowSizeReceive = 1;
        ((gxIecHdlcSetup*)object)->maximumInfoLengthTransmit = 128;
        ((gxIecHdlcSetup*)object)->maximumInfoLengthReceive = 128;
        ((gxIecHdlcSetup*)object)->interCharachterTimeout = 25;
        ((gxIecHdlcSetup*)object)->inactivityTimeout = 120;
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        break;
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        break;
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        break;
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        break;
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        break;
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        break;
    case DLMS_OBJECT_TYPE_LIMITER:
        break;
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        break;
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        break;
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        break;
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ((gxProfileGeneric*)object)->maxRowCount = 1;
        break;
#endif //DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        break;
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        break;
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        break;
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        break;
    case DLMS_OBJECT_TYPE_SCHEDULE:
        break;
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        break;
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        break;
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        break;
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        break;
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        break;
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        break;
    case DLMS_OBJECT_TYPE_MESSAGE_HANDLER:
        break;
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        break;
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
        break;
    case DLMS_OBJECT_TYPE_ACCOUNT:
        break;
    case DLMS_OBJECT_TYPE_CREDIT:
        break;
    case DLMS_OBJECT_TYPE_CHARGE:
        break;
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        break;
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        object->version = 1;
        break;
    default:
        break;
    }
    return 0;
}
