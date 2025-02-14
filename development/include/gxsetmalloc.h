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

#ifndef COSEM_SET_MALLOC_H
#define COSEM_SET_MALLOC_H

#include "gxignore.h"
#if !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxobjects.h"
#include "dlmssettings.h"

#ifndef DLMS_IGNORE_DATA
    int cosem_setData(gxValueEventArg* e);
#endif //DLMS_IGNORE_DATA

#ifndef DLMS_IGNORE_REGISTER
    int cosem_setRegister(gxRegister* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_REGISTER

#ifndef DLMS_IGNORE_CLOCK
    int cosem_setClock(dlmsSettings* settings, gxClock* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_CLOCK

#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    int cosem_setActionSchedule(dlmsSettings* settings, gxActionSchedule* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_ACTION_SCHEDULE

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    int cosem_setActivityCalendar(dlmsSettings* settings, gxActivityCalendar* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    int cosem_parseLNObjects(dlmsSettings* settings, gxByteBuffer* data, objectArray* objects);
    int cosem_setAssociationLogicalName(dlmsSettings* settings, gxAssociationLogicalName* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    int cosem_parseSNObjects(dlmsSettings* settings, gxByteBuffer* data, objectArray* objects);
    int cosem_setAssociationShortName(dlmsSettings* settings, gxAssociationShortName* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_AUTO_ANSWER
    int cosem_setAutoAnswer(gxAutoAnswer* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_AUTO_ANSWER

#ifndef DLMS_IGNORE_AUTO_CONNECT
    int cosem_setAutoConnect(gxAutoConnect* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_DEMAND_REGISTER
    int cosem_setDemandRegister(gxDemandRegister* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_DEMAND_REGISTER

#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    int cosem_setMacAddressSetup(gxMacAddressSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    int cosem_setExtendedRegister(gxExtendedRegister* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_EXTENDED_REGISTER

#ifndef DLMS_IGNORE_GPRS_SETUP
    int cosem_setGprsSetup(gxGPRSSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_GPRS_SETUP

#ifndef DLMS_IGNORE_SECURITY_SETUP
    int cosem_setSecuritySetup(dlmsSettings* settings, gxSecuritySetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_SECURITY_SETUP

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    int cosem_setIecHdlcSetup(gxIecHdlcSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_IEC_HDLC_SETUP

#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    int cosem_setIecLocalPortSetup(gxLocalPortSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IP4_SETUP
    int cosem_setIP4Setup(dlmsSettings* settings, gxIp4Setup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC
    int cosem_setProfileGeneric(dlmsSettings* settings, gxProfileGeneric* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_PROFILE_GENERIC

#ifndef DLMS_IGNORE_UTILITY_TABLES
    int cosem_setUtilityTables(gxUtilityTables* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    int cosem_setMbusSlavePortSetup(gxMbusSlavePortSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP

#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    int cosem_setDisconnectControl(gxDisconnectControl* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_DISCONNECT_CONTROL

#ifndef DLMS_IGNORE_LIMITER
    int cosem_setLimiter(dlmsSettings* settings, gxLimiter* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_LIMITER

#ifndef DLMS_IGNORE_MBUS_CLIENT
    int cosem_setmMbusClient(dlmsSettings* settings, gxMBusClient* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_MBUS_CLIENT

#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    int cosem_setModemConfiguration(gxModemConfiguration* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_MODEM_CONFIGURATION

#ifndef DLMS_IGNORE_PPP_SETUP
    int cosem_setPppSetup(dlmsSettings* settings, gxPppSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    int cosem_setRegisterActivation(dlmsSettings* settings, gxValueEventArg* e);
#endif //DLMS_IGNORE_REGISTER_ACTIVATION

#ifndef DLMS_IGNORE_REGISTER_MONITOR
    int cosem_setRegisterMonitor(dlmsSettings* settings, gxRegisterMonitor* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_REGISTER_MONITOR

#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    int cosem_setSapAssignment(gxSapAssignment* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_SCHEDULE
    int cosem_setSchedule(dlmsSettings* settings, gxSchedule* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_SCHEDULE

#ifndef DLMS_IGNORE_SCRIPT_TABLE
    int cosem_setScriptTable(dlmsSettings* settings, gxScriptTable* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_SCRIPT_TABLE

#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    int cosem_setSpecialDaysTable(gxSpecialDaysTable* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE

#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    int cosem_setTcpUdpSetup(dlmsSettings* settings, gxTcpUdpSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_DIAGNOSTIC
    int cosem_setMbusDiagnostic(gxMbusDiagnostic* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_MBUS_DIAGNOSTIC
#ifndef DLMS_IGNORE_MBUS_PORT_SETUP
    int cosem_setMbusPortSetup(gxMBusPortSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_MBUS_PORT_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    int cosem_setMbusMasterPortSetup(gxMBusMasterPortSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

#ifndef DLMS_IGNORE_G3_PLC_MAC_LAYER_COUNTERS
    int cosem_setG3PlcMacLayerCounters(gxG3PlcMacLayerCounters* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_G3_PLC_MAC_LAYER_COUNTERS
#ifndef DLMS_IGNORE_G3_PLC_MAC_SETUP
    int cosem_setG3PlcMacSetup(gxG3PlcMacSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_G3_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_G3_PLC_6LO_WPAN
    int cosem_setG3Plc6LoWPAN(gxG3Plc6LoWPAN* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_G3_PLC_6LO_WPAN
#ifndef DLMS_IGNORE_FUNCTION_CONTROL
    int cosem_setFunctionControl(
        dlmsSettings* settings,
        gxFunctionControl* object, 
        unsigned char index, 
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_FUNCTION_CONTROL
#ifndef DLMS_IGNORE_ARRAY_MANAGER
    int cosem_setArrayManager(dlmsSettings* settings, gxArrayManager* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_ARRAY_MANAGER

#ifndef DLMS_IGNORE_PUSH_SETUP
    int cosem_setPushSetup(dlmsSettings* settings, gxPushSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    int cosem_setGsmDiagnostic(gxGsmDiagnostic* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
    int compactData_updateTemplateDescription(
        dlmsSettings* settings,
        gxCompactData* object);
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    int cosem_setIecTwistedPairSetup(gxIecTwistedPairSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP
    int cosem_setIP6Setup(dlmsSettings* settings, gxIp6Setup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_IMAGE_TRANSFER
    int cosem_setImageTransfer(gxImageTransfer* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_IMAGE_TRANSFER

#ifndef DLMS_IGNORE_REGISTER_TABLE
    int cosem_setRegistertable(gxRegisterTable* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_REGISTER_TABLE

#ifndef DLMS_IGNORE_ACCOUNT
    int cosem_setAccount(gxAccount* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_ACCOUNT

#ifndef DLMS_IGNORE_CREDIT
    int cosem_setCredit(gxCredit* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
    int cosem_setCharge(dlmsSettings* settings, gxCharge* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_CHARGE
#ifndef DLMS_IGNORE_TOKEN_GATEWAY
    int cosem_setTokenGateway(gxTokenGateway* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_TOKEN_GATEWAY

#ifndef DLMS_IGNORE_COMPACT_DATA
    int cosem_setCompactData(
        dlmsSettings* settings,
        gxCompactData* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_PARAMETER_MONITOR
        int cosem_setParameterMonitor(
            dlmsSettings* settings,
            gxParameterMonitor* object,
            unsigned char index,
            dlmsVARIANT* value);
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    int cosem_setLlcSscsSetup(
        dlmsSettings* settings,
        gxLlcSscsSetup* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    int cosem_setPrimeNbOfdmPlcPhysicalLayerCounters(
        dlmsSettings* settings,
        gxPrimeNbOfdmPlcPhysicalLayerCounters* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    int cosem_setPrimeNbOfdmPlcMacSetup(
        dlmsSettings* settings,
        gxPrimeNbOfdmPlcMacSetup* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    int cosem_setPrimeNbOfdmPlcMacFunctionalParameters(
        dlmsSettings* settings,
        gxPrimeNbOfdmPlcMacFunctionalParameters* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    int cosem_setPrimeNbOfdmPlcMacCounters(
        dlmsSettings* settings,
        gxPrimeNbOfdmPlcMacCounters* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    int cosem_setPrimeNbOfdmPlcMacNetworkAdministrationData(
        dlmsSettings* settings,
        gxPrimeNbOfdmPlcMacNetworkAdministrationData* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    int cosem_setPrimeNbOfdmPlcApplicationsIdentification(
        dlmsSettings* settings,
        gxPrimeNbOfdmPlcApplicationsIdentification* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION

#ifndef DLMS_IGNORE_ARBITRATOR
    int cosem_setArbitrator(
        dlmsSettings* settings,
        gxArbitrator* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_ARBITRATOR

#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    int cosem_setIec8802LlcType1Setup(
        dlmsSettings* settings,
        gxIec8802LlcType1Setup* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP

#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    int cosem_setIec8802LlcType2Setup(
        dlmsSettings* settings,
        gxIec8802LlcType2Setup* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP

#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    int cosem_setIec8802LlcType3Setup(
        dlmsSettings* settings,
        gxIec8802LlcType3Setup* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP

#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    int cosem_setSFSKActiveInitiator(
        dlmsSettings* settings,
        gxSFSKActiveInitiator* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR

#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    int cosem_setFSKMacCounters(
        dlmsSettings* settings,
        gxFSKMacCounters* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS

#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    int cosem_setSFSKMacSynchronizationTimeouts(
        dlmsSettings* settings,
        gxSFSKMacSynchronizationTimeouts* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS

#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    int cosem_setSFSKPhyMacSetUp(
        dlmsSettings* settings,
        gxSFSKPhyMacSetUp* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP

#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    int cosem_setSFSKReportingSystemList(
        dlmsSettings* settings,
        gxSFSKReportingSystemList* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifndef DLMS_IGNORE_LTE_MONITORING
    int cosem_setLteMonitoring(
        dlmsSettings* settings,
        gxLteMonitoring* object,
        unsigned char index,
        dlmsVARIANT* value);
#endif //DLMS_IGNORE_LTE_MONITORING

#ifndef DLMS_IGNORE_NTP_SETUP
    int cosem_setNtpSetup(gxNtpSetup* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_IGNORE_NTP_SETUP

#ifdef DLMS_ITALIAN_STANDARD
    int cosem_setTariffPlan(gxTariffPlan* object, unsigned char index, dlmsVARIANT* value);
#endif //DLMS_ITALIAN_STANDARD
#ifdef  __cplusplus
}
#endif

#endif //!defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
#endif//COSEM_SET_MALLOC_H
