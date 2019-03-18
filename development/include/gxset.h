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

#ifndef COSEM_SET_H
#define COSEM_SET_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxobjects.h"
#include "dlmssettings.h"

#ifndef DLMS_IGNORE_DATA
    int cosem_setData(gxData* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_DATA

#ifndef DLMS_IGNORE_REGISTER
    int cosem_setRegister(gxRegister* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_REGISTER

#ifndef DLMS_IGNORE_CLOCK
    int cosem_setClock(gxClock* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_CLOCK

#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    int cosem_setActionSchedule(gxActionSchedule* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_ACTION_SCHEDULE

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
    int cosem_setActivityCalendar(gxActivityCalendar* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR

    int cosem_setAssociationLogicalName(dlmsSettings* settings, gxAssociationLogicalName* object, unsigned char index, dlmsVARIANT *value);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    int cosem_setAssociationShortName(dlmsSettings* settings, gxAssociationShortName* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_AUTO_ANSWER
    int cosem_setAutoAnswer(gxAutoAnswer* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_AUTO_ANSWER

#ifndef DLMS_IGNORE_AUTO_CONNECT
    int cosem_setAutoConnect(gxAutoConnect* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_DEMAND_REGISTER
    int cosem_setDemandRegister(gxDemandRegister* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_DEMAND_REGISTER

#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
    int cosem_setMacAddressSetup(gxMacAddressSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    int cosem_setExtendedRegister(gxExtendedRegister* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_EXTENDED_REGISTER

#ifndef DLMS_IGNORE_GPRS_SETUP
    int cosem_setGprsSetup(gxGPRSSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_GPRS_SETUP

#ifndef DLMS_IGNORE_SECURITY_SETUP
    int cosem_setSecuritySetup(gxSecuritySetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_SECURITY_SETUP

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    int cosem_setIecHdlcSetup(gxIecHdlcSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_IEC_HDLC_SETUP

#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    int cosem_setIecLocalPortSetup(gxLocalPortSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IP4_SETUP
    int cosem_setIP4Setup(gxIp4Setup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC
    int cosem_setProfileGeneric(dlmsSettings* settings, gxProfileGeneric* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_PROFILE_GENERIC

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    int cosem_setMbusSlavePortSetup(gxMbusSlavePortSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP

#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    int cosem_setDisconnectControl(gxDisconnectControl* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_DISCONNECT_CONTROL

#ifndef DLMS_IGNORE_LIMITER
    int cosem_setLimiter(dlmsSettings* settings, gxLimiter* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_LIMITER

#ifndef DLMS_IGNORE_MBUS_CLIENT
    int cosem_setmMbusClient(gxMBusClient* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_MBUS_CLIENT

#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
    int cosem_setModemConfiguration(gxModemConfiguration* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_MODEM_CONFIGURATION

#ifndef DLMS_IGNORE_PPP_SETUP
    int cosem_setPppSetup(gxPppSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    int cosem_setRegisterActivation(gxRegisterActivation* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_REGISTER_ACTIVATION

#ifndef DLMS_IGNORE_REGISTER_MONITOR
    int cosem_setRegisterMonitor(gxRegisterMonitor* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_REGISTER_MONITOR

#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
    int cosem_setSapAssignment(gxSapAssignment* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_SCHEDULE
    int cosem_setSchedule(gxSchedule* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_SCHEDULE

#ifndef DLMS_IGNORE_SCRIPT_TABLE
    int cosem_setScriptTable(gxScriptTable* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_SCRIPT_TABLE

#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    int cosem_setSpecialDaysTable(gxSpecialDaysTable* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE

#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    int cosem_setTcpUdpSetup(gxTcpUdpSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_TCP_UDP_SETUP

#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    int cosem_setMbusMasterPortSetup(gxMBusMasterPortSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

#ifndef DLMS_IGNORE_MESSAGE_HANDLER
    int cosem_setMessageHandler(gxMessageHandler* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_MESSAGE_HANDLER

#ifndef DLMS_IGNORE_PUSH_SETUP
    int cosem_setPushSetup(dlmsSettings* settings, gxPushSetup* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    int cosem_setGsmDiagnostic(gxGsmDiagnostic* object, unsigned char index, dlmsVARIANT *value);
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_COMPACT_DATA
    int compactData_updateTemplateDescription(
        dlmsSettings* settings,
        gxCompactData* object);
#endif //DLMS_IGNORE_COMPACT_DATA

    int cosem_setValue(dlmsSettings* settings, gxValueEventArg *e);

#ifdef  __cplusplus
}
#endif

#endif//COSEM_SET_H
