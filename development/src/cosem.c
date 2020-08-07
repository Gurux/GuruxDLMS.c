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
#include "../include/datainfo.h"
#include "../include/dlms.h"
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
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        size = sizeof(gxParameterMonitor);
        break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        size = sizeof(gxLlcSscsSetup);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        size = sizeof(gxPrimeNbOfdmPlcPhysicalLayerCounters);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        size = sizeof(gxPrimeNbOfdmPlcMacSetup);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        size = sizeof(gxPrimeNbOfdmPlcMacFunctionalParameters);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        size = sizeof(gxPrimeNbOfdmPlcMacCounters);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        size = sizeof(gxPrimeNbOfdmPlcMacNetworkAdministrationData);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        size = sizeof(gxPrimeNbOfdmPlcApplicationsIdentification);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
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
            printf("Object %s size: %d\r\n", obj_typeToString2(type), size);
            */
    }
#endif
    return size;
}

#ifndef DLMS_IGNORE_MALLOC
int cosem_createObject(DLMS_OBJECT_TYPE type, gxObject** object)
{
    int ret;
    unsigned char ln[] = { 0,0,40,0,0,255 };
    int size = cosem_getObjectSize(type);
    if (size == 0)
    {
        return DLMS_ERROR_CODE_UNAVAILABLE_OBJECT;
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
    printf("Object %s size: %d\r\n", obj_typeToString2(type), (int)_msize(*object));
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
#endif //DLMS_IGNORE_MALLOC

int cosem_setLogicalName(
    gxObject* object,
    unsigned char* value)
{
    memcpy(object->logicalName, value, 6);
    return 0;
}

#ifndef DLMS_IGNORE_MALLOC
int cosem_init(
    gxObject* object,
    DLMS_OBJECT_TYPE type,
    const char* logicalNameString)
{
    unsigned char ln[6];
    hlp_setLogicalName(ln, logicalNameString);
    return cosem_init2(object, type, ln);
}
#endif //DLMS_IGNORE_MALLOC

int cosem_init2(
    gxObject* object,
    DLMS_OBJECT_TYPE type,
    const unsigned char* ln)
{
    int size = cosem_getObjectSize(type);
    if (size == 0)
    {
        return DLMS_ERROR_CODE_UNAVAILABLE_OBJECT;
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
#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
    {
        gxAssociationLogicalName* it = (gxAssociationLogicalName*)object;
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
        it->serverSAP = 1;
    }
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    break;
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        break;
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        object->version = 2;
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
        ((gxIecHdlcSetup*)object)->deviceAddress = 0x10;
        break;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        object->version = 1;
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
        object->version = 1;
        break;
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        object->version = 1;
        break;
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        break;
#ifndef DLMS_IGNORE_PROFILE_GENERIC
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
#ifndef DLMS_IGNORE_MALLOC
        ((gxProfileGeneric*)object)->maxRowCount = 1;
#endif //DLMS_IGNORE_MALLOC
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
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        break;
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        break;
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ((gxTcpUdpSetup*)object)->maximumSimultaneousConnections = 1;
        ((gxTcpUdpSetup*)object)->maximumSegmentSize = 40;
        ((gxTcpUdpSetup*)object)->inactivityTimeout = 180;
        break;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
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

int cosem_checkStructure(gxByteBuffer* bb, uint16_t expectedItemCount)
{
    int ret;
    uint16_t cnt;
    if ((ret = cosem_getStructure(bb, &cnt)) == 0)
    {
        if (cnt != expectedItemCount)
        {
            ret = DLMS_ERROR_CODE_INCONSISTENT_OBJECT;
        }
    }
    return ret;
}

int cosem_getStructure(gxByteBuffer* bb, uint16_t* count)
{
    int ret;
    unsigned char value;
    uint16_t cnt;
    if ((ret = bb_getUInt8(bb, &value)) != 0)
    {
        return ret;
    }
    if (value != DLMS_DATA_TYPE_STRUCTURE)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    //Get structure count.
    if ((ret = hlp_getObjectCount2(bb, &cnt)) != 0)
    {
        return ret;
    }
    *count = cnt;
    return 0;
}

int cosem_checkArray(gxByteBuffer* bb, uint16_t* count)
{
    int ret;
    unsigned char ch;
    uint16_t cnt;
    if ((ret = bb_getUInt8(bb, &ch)) != 0)
    {
        return ret;
    }
    if (ch != DLMS_DATA_TYPE_ARRAY)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    //Get array count.
    if ((ret = hlp_getObjectCount2(bb, &cnt)) != 0)
    {
        return ret;
    }
    if (*count < cnt)
    {
        return DLMS_ERROR_CODE_INCONSISTENT_OBJECT;
    }
    *count = cnt;
    return 0;
}

int cosem_getUInt8(gxByteBuffer* bb, unsigned char* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_UINT8)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getUInt8(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getUInt16(gxByteBuffer* bb, uint16_t* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_UINT16)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getUInt16(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getUInt32(gxByteBuffer* bb, uint32_t* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_UINT32)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getUInt32(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getInt8(gxByteBuffer* bb, signed char* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_INT8)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getInt8(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getInt16(gxByteBuffer* bb, short* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_INT16)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getInt16(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getInt32(gxByteBuffer* bb, int32_t* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_INT32)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getInt32(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getOctectStringBase(gxByteBuffer* bb, gxByteBuffer* value, unsigned char type)
{
    int ret;
    unsigned char tmp;
    uint16_t count;
    if ((ret = bb_clear(value)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != type)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = hlp_getObjectCount2(bb, &count)) != 0)
    {
        return ret;
    }
    if (count > bb_getCapacity(value))
    {
        return DLMS_ERROR_CODE_INCONSISTENT_OBJECT;
    }
    if ((ret = bb_set2(value, bb, bb->position, count)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getOctectStringBase2(gxByteBuffer* bb, unsigned char* value, uint16_t capacity, uint16_t* size, unsigned char type)
{
    int ret;
    unsigned char tmp;
    uint16_t count;
    memset(value, 0, capacity);
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != type)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = hlp_getObjectCount2(bb, &count)) != 0)
    {
        return ret;
    }
    if (count > capacity)
    {
        return DLMS_ERROR_CODE_INCONSISTENT_OBJECT;
    }
    if (size != NULL)
    {
        *size = count;
    }
    return bb_get(bb, value, count);
}


int cosem_getOctectString(gxByteBuffer* bb, gxByteBuffer* value)
{
    return cosem_getOctectStringBase(bb, value, DLMS_DATA_TYPE_OCTET_STRING);
}

int cosem_getString(gxByteBuffer* bb, gxByteBuffer* value)
{
    return cosem_getOctectStringBase(bb, value, DLMS_DATA_TYPE_STRING);
}

int cosem_getOctectString2(gxByteBuffer* bb, unsigned char* value, uint16_t capacity, uint16_t* size)
{
    return cosem_getOctectStringBase2(bb, value, capacity, size, DLMS_DATA_TYPE_OCTET_STRING);
}

int cosem_getString2(gxByteBuffer* bb, char* value, uint16_t capacity)
{
    uint16_t size;
    int ret = cosem_getOctectStringBase2(bb, (unsigned char*)value, capacity - 1, &size, DLMS_DATA_TYPE_STRING);
    if (ret == 0)
    {
        value[size] = 0;
    }
    return ret;
}

int cosem_getDateTimeBase(gxByteBuffer* bb, gxtime* value, unsigned char type)
{
    gxDataInfo info;
    dlmsVARIANT tmp;
    time_clear(value);
#ifdef DLMS_IGNORE_MALLOC
    GX_DATETIME(tmp) = value;
#else
    var_init(&tmp);
#endif //DLMS_IGNORE_MALLOC
    di_init(&info);
    info.type = (DLMS_DATA_TYPE)type;
    return dlms_getData(bb, &info, &tmp);
}

int cosem_getDateTimeFromOctectStringBase(gxByteBuffer* bb, gxtime* value, unsigned char type)
{
    int ret;
    unsigned char ch;
    uint16_t count;
    gxDataInfo info;
    dlmsVARIANT tmp;
    time_clear(value);
#ifdef DLMS_IGNORE_MALLOC
    GX_DATETIME(tmp) = value;
#else
    tmp.dateTime = value;
    tmp.vt = (DLMS_DATA_TYPE)type;
#endif //DLMS_IGNORE_MALLOC

    if ((ret = bb_getUInt8(bb, &ch)) != 0)
    {
        return ret;
    }
    if (ch != DLMS_DATA_TYPE_OCTET_STRING)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = hlp_getObjectCount2(bb, &count)) != 0)
    {
        return ret;
    }
    if (type == DLMS_DATA_TYPE_DATETIME)
    {
        if (count != 12)
        {
            return DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
    }
    else if (type == DLMS_DATA_TYPE_DATE)
    {
        if (count != 5)
        {
            return DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
    }
    else if (type == DLMS_DATA_TYPE_TIME)
    {
        if (count != 4)
        {
            return DLMS_ERROR_CODE_UNMATCH_TYPE;
        }
    }
    di_init(&info);
    info.type = (DLMS_DATA_TYPE)type;
    return dlms_getData(bb, &info, &tmp);
}


int cosem_getDateTimeFromOctectString(gxByteBuffer* bb, gxtime* value)
{
    return cosem_getDateTimeFromOctectStringBase(bb, value, DLMS_DATA_TYPE_DATETIME);
}

int cosem_getDateFromOctectString(gxByteBuffer* bb, gxtime* value)
{
    return cosem_getDateTimeFromOctectStringBase(bb, value, DLMS_DATA_TYPE_DATE);
}

int cosem_getTimeFromOctectString(gxByteBuffer* bb, gxtime* value)
{
    return cosem_getDateTimeFromOctectStringBase(bb, value, DLMS_DATA_TYPE_TIME);
}

int cosem_getBitString(gxByteBuffer* bb, bitArray* value)
{
    int ret;
    unsigned char ch;
    uint16_t count;
    ba_clear(value);
    if ((ret = bb_getUInt8(bb, &ch)) != 0)
    {
        return ret;
    }
    if (ch != DLMS_DATA_TYPE_BIT_STRING)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = hlp_getObjectCount2(bb, &count)) != 0)
    {
        return ret;
    }
    return hlp_add(value, bb, count);
}

int cosem_getIntegerFromBitString(gxByteBuffer* bb, uint32_t* value)
{
    int ret;
    unsigned char ch;
    uint16_t count;
    bitArray ba;
    if ((ret = bb_getUInt8(bb, &ch)) != 0)
    {
        return ret;
    }
    if (ch != DLMS_DATA_TYPE_BIT_STRING)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = hlp_getObjectCount2(bb, &count)) != 0)
    {
        return ret;
    }
    ba_attach(&ba, bb->data + bb->position, count, count);
    return ba_toInteger(&ba, value);
}

int cosem_getVariant(gxByteBuffer* bb, dlmsVARIANT* value)
{
    int ret;
    unsigned char ch;
    gxDataInfo info;
    var_clear(value);
    if ((ret = bb_getUInt8(bb, &ch)) != 0)
    {
        return ret;
    }
    if (ch == DLMS_DATA_TYPE_NONE)
    {
        return 0;
    }
    di_init(&info);
    info.type = (DLMS_DATA_TYPE)ch;
    return dlms_getData(bb, &info, value);
}

int cosem_getEnum(gxByteBuffer* bb, unsigned char* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_ENUM)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getUInt8(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getBoolean(gxByteBuffer* bb, unsigned char* value)
{
    int ret;
    unsigned char tmp;
    if ((ret = bb_getUInt8(bb, &tmp)) != 0)
    {
        return ret;
    }
    if (tmp != DLMS_DATA_TYPE_BOOLEAN)
    {
        return DLMS_ERROR_CODE_UNMATCH_TYPE;
    }
    if ((ret = bb_getUInt8(bb, value)) != 0)
    {
        return ret;
    }
    return 0;
}

int cosem_getUtf8String(gxByteBuffer* bb, gxByteBuffer* value)
{
    return cosem_getOctectStringBase(bb, value, DLMS_DATA_TYPE_STRING_UTF8);
}

int cosem_getUtf8String2(gxByteBuffer* bb, char* value, uint16_t capacity, uint16_t* size)
{
    return cosem_getOctectStringBase2(bb, (unsigned char*)value, capacity, size, DLMS_DATA_TYPE_STRING_UTF8);
}

int cosem_setDateTimeAsOctectString(gxByteBuffer* bb, gxtime* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(bb, 12)) != 0 ||
        (ret = var_getDateTime2(value, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setDateAsOctectString(gxByteBuffer* bb, gxtime* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(bb, 5)) != 0 ||
        (ret = var_getDate(value, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setBitString(gxByteBuffer* bb, uint32_t value, uint16_t count)
{
    int ret = 0;
    uint16_t pos;
    uint16_t capacity = count == 0 ? 2 : (3 + (count / 8));
    capacity += (uint16_t)bb_size(bb);
    if (bb_getCapacity(bb) < capacity)
    {
        ret = bb_capacity(bb, capacity);
    }
    if (ret == 0 &&
        (ret = bb_setUInt8(bb, DLMS_DATA_TYPE_BIT_STRING)) == 0 &&
        (ret = hlp_setObjectCount(count, bb)) == 0)
    {
        bitArray ba;
        ba_attach(&ba, bb->data + bb->size, 0, (uint16_t)(8 * (bb->size - bb->size)));
        for (pos = 0; pos != count; ++pos)
        {
            if ((ret = ba_setByIndex(&ba, pos, value & 01)) != 0)
            {
                break;
            }
            value >>= 1;
        }
        bb->size += ba_getByteCount(count);
    }
    return ret;
}


int cosem_setTimeAsOctectString(gxByteBuffer* bb, gxtime* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(bb, 4)) != 0 ||
        (ret = var_getTime(value, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setOctectString(gxByteBuffer* bb, gxByteBuffer* value)
{
    int ret;
    if (value == NULL)
    {
        if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(bb, 0)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(bb, (unsigned char)value->size)) != 0 ||
        (ret = bb_set(bb, value->data, (uint16_t)value->size)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setDateTime(gxByteBuffer* bb, gxtime* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_DATETIME)) != 0 ||
        (ret = var_getDateTime2(value, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setDate(gxByteBuffer* bb, gxtime* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_DATE)) != 0 ||
        (ret = var_getDate(value, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setTime(gxByteBuffer* bb, gxtime* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_TIME)) != 0 ||
        (ret = var_getTime(value, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}


int cosem_setOctectString2(
    gxByteBuffer* bb,
    const unsigned char* value,
    uint16_t size)
{
    int ret;
    if (value == NULL || size == 0)
    {
        if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(bb, 0)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    else if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_OCTET_STRING)) != 0 ||
        (ret = bb_setUInt8(bb, (unsigned char)size)) != 0 ||
        (ret = bb_set(bb, value, size)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_getDateTime(gxByteBuffer* bb, gxtime* value)
{
    return cosem_getDateTimeBase(bb, value, DLMS_DATA_TYPE_DATETIME);
}

int cosem_getDate(gxByteBuffer* bb, gxtime* value)
{
    return cosem_getDateTimeBase(bb, value, DLMS_DATA_TYPE_DATE);
}

int cosem_getTime(gxByteBuffer* bb, gxtime* value)
{
    return cosem_getDateTimeBase(bb, value, DLMS_DATA_TYPE_TIME);
}


int cosem_setUInt8(gxByteBuffer* bb, unsigned char value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_UINT8)) != 0 ||
        (ret = bb_setUInt8(bb, value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setUInt16(gxByteBuffer* bb, uint16_t value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_UINT16)) != 0 ||
        (ret = bb_setUInt16(bb, value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setUInt32(gxByteBuffer* bb, uint32_t value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_UINT32)) != 0 ||
        (ret = bb_setUInt32(bb, value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setUInt64(gxByteBuffer* bb, uint64_t* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_UINT64)) != 0 ||
        (ret = bb_setUInt64(bb, *value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setInt8(gxByteBuffer* bb, char value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_INT8)) != 0 ||
        (ret = bb_setInt8(bb, value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setInt16(gxByteBuffer* bb, short value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_INT16)) != 0 ||
        (ret = bb_setInt16(bb, value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setInt32(gxByteBuffer* bb, int32_t value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_INT32)) != 0 ||
        (ret = bb_setInt32(bb, value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setInt64(gxByteBuffer* bb, int64_t* value)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_INT64)) != 0 ||
        (ret = bb_setInt64(bb, *value)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setVariant(gxByteBuffer* bb, dlmsVARIANT* value)
{
    if (value->vt == DLMS_DATA_TYPE_NONE)
    {
        return bb_setUInt8(bb, 0);
    }
    return dlms_setData(bb, value->vt, value);
}

int cosem_setStructure(gxByteBuffer* bb, uint16_t count)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_STRUCTURE)) != 0 ||
        (ret = hlp_setObjectCount(count, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

int cosem_setArray(gxByteBuffer* bb, uint16_t count)
{
    int ret;
    if ((ret = bb_setUInt8(bb, DLMS_DATA_TYPE_ARRAY)) != 0 ||
        (ret = hlp_setObjectCount(count, bb)) != 0)
    {
        //Error code is returned at the end of the function.
    }
    return ret;
}

#ifndef DLMS_IGNORE_PROFILE_GENERIC

/**
     * Get selected columns.
     *
     * @param cols
     *            selected columns.
     * @param columns
     *            Selected columns.
     */
#ifndef DLMS_IGNORE_MALLOC
int getSelectedColumns(
    gxArray* captureObjects,
    variantArray* cols,
    gxArray* columns)
{
    gxKey* c;
    dlmsVARIANT* it, * it2;
    unsigned char* ln;
    DLMS_OBJECT_TYPE ot;
    int ret, pos, pos2, dataIndex;
    unsigned char attributeIndex;
    for (pos = 0; pos != cols->size; ++pos)
    {
        if ((ret = va_getByIndex(cols, pos, &it)) != 0 ||
            //Get object type.
            (ret = va_getByIndex(it->Arr, 0, &it2)) != 0)
        {
            break;
        }
        ot = (DLMS_OBJECT_TYPE)var_toInteger(it2);
        //Get logical name.
        if ((ret = va_getByIndex(it->Arr, 1, &it2)) != 0)
        {
            break;
        }
        ln = it2->byteArr->data;
        //Get attribute index.
        if ((ret = va_getByIndex(it->Arr, 2, &it2)) != 0)
        {
            break;
        }
        attributeIndex = (char)var_toInteger(it2);
        //Get data index.
        if ((ret = va_getByIndex(it->Arr, 3, &it2)) != 0)
        {
            break;
        }
        dataIndex = var_toInteger(it2);
        // Find columns and update only them.
        for (pos2 = 0; pos2 != captureObjects->size; ++pos2)
        {
            if ((ret = arr_getByIndex(captureObjects, pos2, (void**)&c)) != 0)
            {
                return ret;
            }
            if (((gxObject*)c->key)->objectType == ot &&
                ((gxTarget*)c->value)->attributeIndex == attributeIndex &&
                ((gxTarget*)c->value)->dataIndex == dataIndex &&
                memcmp(((gxObject*)c->key)->logicalName, ln, 6) == 0)
            {
                if ((ret = arr_push(columns, c)) != 0)
                {
                    return ret;
                }
                break;
            }
        }
    }
    return 0;
}
#endif //DLMS_IGNORE_MALLOC

int cosem_getColumns(
    gxArray* captureObjects,
    unsigned char selector,
    dlmsVARIANT* parameters,
    gxArray* columns)
{
    uint16_t pos, start = 0, count = 0;
    int ret = 0;
    arr_empty(columns);
#ifdef DLMS_IGNORE_MALLOC
    int index = 0;
    gxTarget* k;
    gxTarget* k2;
    if (selector == 1) //Read by range
    {
        count = arr_getCapacity(columns);
        if ((ret = cosem_checkArray(parameters->byteArr, &count)) != 0)
        {
            return ret;
        }
        //Add all.
        if (count == 0)
        {
            count = captureObjects->size;
            columns->size = count;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = arr_getByIndex(captureObjects, pos, (void**)&k, sizeof(gxTarget))) != 0 ||
                    (ret = arr_getByIndex(columns, pos, (void**)&k2, sizeof(gxTarget))) != 0)
                {
                    break;
                }
                memcpy(k2, k, sizeof(gxTarget));
            }
        }
        else
        {
            int pos2;
            uint16_t ot;
            unsigned char ln[6];
            signed char aIndex;
            short dIndex;
            columns->size = 0;
            for (pos = 0; pos != count; ++pos)
            {
                if ((ret = cosem_checkStructure(parameters->byteArr, 4)) != 0 ||
                    (ret = cosem_getUInt16(parameters->byteArr, &ot)) != 0 ||
                    (ret = cosem_getOctectString2(parameters->byteArr, ln, 6, NULL)) != 0 ||
                    (ret = cosem_getInt8(parameters->byteArr, &aIndex)) != 0 ||
                    (ret = cosem_getInt16(parameters->byteArr, &dIndex)) != 0)
                {
                    break;
                }
                for (pos2 = 0; pos2 != captureObjects->size; ++pos2)
                {
                    if ((ret = arr_getByIndex(captureObjects, pos2, (void**)&k, sizeof(gxTarget))) != 0)
                    {
                        break;
                    }
#ifdef DLMS_IGNORE_OBJECT_POINTERS
                    if (k->objectType == ot && memcmp(k->logicalName, ln, 6) == 0)
#else
                    if (k->target->objectType == ot && memcmp(k->target->logicalName, ln, 6) == 0)
#endif //DLMS_IGNORE_OBJECT_POINTERS
                    {
                        if (!(columns->size < arr_getCapacity(columns)))
                        {
                            return DLMS_ERROR_CODE_INCONSISTENT_OBJECT;
                        }
                        ++columns->size;
                        if ((ret = arr_getByIndex(columns, index, (void**)&k2, sizeof(gxTarget))) != 0)
                        {
                            break;
                        }
                        memcpy(k2, k, sizeof(gxTarget));
                        ++index;
                        break;
                    }
                }
            }
        }
    }
    else if (selector == 2) //Read by entry.
    {
        if ((ret = cosem_getUInt16(parameters->byteArr, &start)) != 0 ||
            (ret = cosem_getUInt16(parameters->byteArr, &count)) != 0)
        {
            return ret;
        }
        if (count == 0)
        {
            count = captureObjects->size - start + 1;
        }
        if (!(count <= captureObjects->size || start < count))
        {
            return DLMS_ERROR_CODE_INCONSISTENT_OBJECT;
        }
        columns->size = count;
        for (pos = start - 1; pos != count; ++pos)
        {
            if ((ret = arr_getByIndex(captureObjects, pos, (void**)&k, sizeof(gxTarget))) != 0 ||
                (ret = arr_getByIndex(columns, index, (void**)&k2, sizeof(gxTarget))) != 0)
            {
                break;
            }
            memcpy(k2, k, sizeof(gxTarget));
            ++index;
        }
    }
#else
    uint16_t addAllColumns = 1;
    gxKey* k;
    dlmsVARIANT* it;
    if (parameters->vt == DLMS_DATA_TYPE_STRUCTURE)
    {
        if (selector == 1) //Read by range
        {
            if (parameters->Arr->size > 3)
            {
                if ((ret = va_getByIndex(parameters->Arr, 3, &it)) == 0)
                {
                    ret = getSelectedColumns(captureObjects, it->Arr, columns);
                }
                addAllColumns = 0;
            }
        }
        else if (selector == 2) //Read by entry.
        {
            if (parameters->Arr->size > 2)
            {
                if ((ret = va_getByIndex(parameters->Arr, 2, &it)) != 0)
                {
                    return ret;
                }
                start = var_toInteger(it);
            }
            if (parameters->Arr->size > 3)
            {
                if ((ret = va_getByIndex(parameters->Arr, 3, &it)) != 0)
                {
                    return ret;
                }
                count = var_toInteger(it);
            }
            else if (start != 1)
            {
                count = captureObjects->size;
            }
            if (start != 1 || count != 0)
            {
                addAllColumns = 0;
                if (count == 0)
                {
                    count = captureObjects->size - start;
                }
                for (pos = start - 1; pos != count; ++pos)
                {
                    if ((ret = arr_getByIndex(captureObjects, pos, (void**)&k)) != 0 ||
                        (ret = arr_push(columns, k)) != 0)
                    {
                        return ret;
                    }
                }
            }
        }
        //Add all objects.
        if (addAllColumns)
        {
            for (pos = 0; pos != captureObjects->size; ++pos)
            {
                if ((ret = arr_getByIndex(captureObjects, pos, (void**)&k)) != 0 ||
                    (ret = arr_push(columns, k)) != 0)
                {
                    break;
                }
            }
        }
    }
#endif //DLMS_IGNORE_MALLOC
    return ret;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC