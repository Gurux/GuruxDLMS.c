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
        gxValueEventArg *e);

    int cosem_getRegister(
        gxValueEventArg *e);

    int cosem_getClock(
        gxValueEventArg *e);

    int cosem_getActionSchedule(
        gxValueEventArg *e);

    int cosem_getActivityCalendar(
        gxValueEventArg *e);

    int cosem_getAssociationLogicalName(
        dlmsSettings* settings,
        gxValueEventArg *e);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    int cosem_getAssociationShortName(
        dlmsSettings* settings,
        gxValueEventArg *e);
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    int cosem_getAutoAnswer(
        gxValueEventArg *e);

    int cosem_getAutoConnect(
        gxValueEventArg *e);

    int cosem_getDemandRegister(
        gxValueEventArg *e);

    int cosem_getMacAddressSetup(
        gxValueEventArg *e);

    int cosem_getExtendedRegister(
        gxValueEventArg *e);

    int cosem_getGprsSetup(
        gxValueEventArg *e);

    int cosem_getSecuritySetup(
        gxValueEventArg *e);

    int cosem_getIecHdlcSetup(
        gxValueEventArg *e);

    int cosem_getIecLocalPortSetup(
        gxValueEventArg *e);

    int cosem_getIP4Setup(
        gxValueEventArg *e);

    int cosem_getProfileGeneric(
        dlmsSettings* settings,
        gxValueEventArg *e);

    int cosem_getMbusSlavePortSetup(
        gxValueEventArg *e);

    int cosem_getDisconnectControl(
        gxValueEventArg *e);

    int cosem_getLimiter(
        gxValueEventArg *e);

    int cosem_getmMbusClient(
        gxValueEventArg *e);

    int cosem_getModemConfiguration(
        gxValueEventArg *e);

    int cosem_getPppSetup(
        gxValueEventArg *e);

    int cosem_getRegisterActivation(
        gxValueEventArg *e);

    int cosem_getRegisterMonitor(
        gxValueEventArg *e);

    int cosem_getSapAssignment(
        gxValueEventArg *e);

    int cosem_getSchedule(
        gxValueEventArg *e);

    int cosem_getScriptTable(
        gxValueEventArg *e);

    int cosem_getSpecialDaysTable(
        gxValueEventArg *e);

    int cosem_getTcpUdpSetup(
        gxValueEventArg *e);

    int cosem_getMbusMasterPortSetup(
        gxValueEventArg *e);

    int cosem_getMessageHandler(
        gxValueEventArg *e);

    int cosem_getPushSetup(
        gxValueEventArg *e);

    int cosem_getValue(
        dlmsSettings* settings,
        gxValueEventArg *e);

    int cosem_getColumns(
        gxArray* captureObjects,
        unsigned char selector,
        dlmsVARIANT *parameters,
        gxArray *columns);

    int cosem_getRow(
        gxArray* table,
        int index,
        gxArray *captureObjects,
        gxArray* columns,
        gxByteBuffer* data);

    int cosem_getTariffPlan(
        gxValueEventArg *e);


    int cosem_getGsmDiagnostic(
        gxValueEventArg *e);

#ifdef  __cplusplus
}
#endif

#endif //COSEM_GET_H
