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

#ifndef COSEM_GET_H
#define COSEM_GET_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxobjects.h"
#include "dlmssettings.h"

    int cosem_getData(
        gxValueEventArg* e);

    int cosem_getRegister(
        gxValueEventArg* e);

#ifndef DLMS_IGNORE_CLOCK
    int cosem_getClock(
        gxValueEventArg* e);

    //Update Daylight Saving time flag if DST is used.
    void clock_updateDST(
        gxClock* object,
        gxtime* value);

    //Convert UTC date time to meter date time.
    int clock_utcToMeterTime(
        gxClock* object,
        gxtime* value);

#endif // DLMS_IGNORE_CLOCK

    int cosem_getActionSchedule(
        gxValueEventArg* e);

    int cosem_getActivityCalendar(
        gxValueEventArg* e);

    int cosem_getAssociationLogicalName(
        dlmsSettings* settings,
        gxValueEventArg* e);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    int cosem_getAssociationShortName(
        dlmsSettings* settings,
        gxValueEventArg* e);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    int cosem_getAutoAnswer(
        gxValueEventArg* e);

    int cosem_getAutoConnect(
        gxValueEventArg* e);

    int cosem_getDemandRegister(
        gxValueEventArg* e);

    int cosem_getMacAddressSetup(
        gxValueEventArg* e);

    int cosem_getExtendedRegister(
        gxValueEventArg* e);

    int cosem_getGprsSetup(
        gxValueEventArg* e);

    int cosem_getSecuritySetup(
        gxValueEventArg* e);

    int cosem_getIecHdlcSetup(
        gxValueEventArg* e);

    int cosem_getIecLocalPortSetup(
        gxValueEventArg* e);

    int cosem_getIP4Setup(
        gxValueEventArg* e);

    int cosem_getProfileGeneric(
        dlmsSettings* settings,
        gxValueEventArg* e);

    int cosem_getMbusSlavePortSetup(
        gxValueEventArg* e);

    int cosem_getDisconnectControl(
        gxValueEventArg* e);

    int cosem_getLimiter(
        gxValueEventArg* e);

    int cosem_getmMbusClient(
        gxValueEventArg* e);

    int cosem_getModemConfiguration(
        dlmsSettings* settings,
        gxValueEventArg* e);

    int cosem_getPppSetup(
        gxValueEventArg* e);

    int cosem_getRegisterActivation(
        dlmsSettings* settings,
        gxValueEventArg* e);

    int cosem_getRegisterMonitor(
        gxValueEventArg* e);

    int cosem_getSapAssignment(
        gxValueEventArg* e);

    int cosem_getSchedule(
        gxValueEventArg* e);

    int cosem_getScriptTable(
        gxValueEventArg* e);

    int cosem_getSpecialDaysTable(
        gxValueEventArg* e);

    int cosem_getTcpUdpSetup(
        gxValueEventArg* e);

    int cosem_getMbusMasterPortSetup(
        gxValueEventArg* e);

    int cosem_getMessageHandler(
        gxValueEventArg* e);

    int cosem_getPushSetup(
        gxValueEventArg* e);

    int cosem_getValue(
        dlmsSettings* settings,
        gxValueEventArg* e);


#ifndef DLMS_IGNORE_G3_PLC_MAC_SETUP
    int cosem_getG3PlcMacSetupNeighbourTables(
        gxArray* tables,
        uint16_t address,
        uint16_t count,
        gxValueEventArg* e);

    int cosem_getG3PlcMacSetupPosTables(
        gxArray* tables,
        uint16_t address,
        uint16_t count,
        gxValueEventArg* e);
#endif //DLMS_IGNORE_G3_PLC_MAC_SETUP

#ifndef DLMS_IGNORE_MALLOC
    int cosem_getRow(
        gxArray* table,
        int index,
        gxArray* captureObjects,
        gxArray* columns,
        gxByteBuffer* data);
#endif //DLMS_IGNORE_MALLOC

    int cosem_getTariffPlan(
        gxValueEventArg* e);

#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    int cosem_getGsmDiagnostic(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    int cosem_getParameterMonitor(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_COMPACT_DATA
    //Convert compact data buffer to array of values.
    int compactData_getValues(
        dlmsSettings* settings,
        gxByteBuffer* templateDescription,
        gxByteBuffer* buffer,
        variantArray* values);
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    int cosem_getLlcSscsSetup(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    int cosem_getPrimeNbOfdmPlcPhysicalLayerCounters(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    int cosem_getPrimeNbOfdmPlcMacSetup(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    int cosem_getPrimeNbOfdmPlcMacFunctionalParameters(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    int cosem_getPrimeNbOfdmPlcMacCounters(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    int  cosem_getPrimeNbOfdmPlcMacNetworkAdministrationData(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    int  cosem_getPrimeNbOfdmPlcApplicationsIdentification(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_ARBITRATOR
    int cosem_getArbitrator(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    int cosem_getIec8802LlcType1Setup(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    int cosem_getIec8802LlcType2Setup(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    int cosem_getIec8802LlcType3Setup(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    int cosem_getSFSKActiveInitiator(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    int cosem_getFSKMacCounters(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    int cosem_getSFSKMacSynchronizationTimeouts(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    int cosem_getSFSKPhyMacSetUp(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    int cosem_getSFSKReportingSystemList(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifndef DLMS_IGNORE_LTE_MONITORING
    int cosem_getLteMonitoring(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_LTE_MONITORING
#ifndef DLMS_IGNORE_NTP_SETUP
    int cosem_getNtpSetup(
        gxValueEventArg* e);
#endif //DLMS_IGNORE_NTP_SETUP

#ifdef  __cplusplus
}
#endif

#endif //COSEM_GET_H
