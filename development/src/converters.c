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

#if defined(_WIN32) || defined(_WIN64)
#include <Ws2tcpip.h>
#endif
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#include <stdio.h> //printf needs this or error is generated.
#endif
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include "../include/gxignore.h"
#include "../include/converters.h"
#include "../include/gxobjects.h"
#ifndef DLMS_IGNORE_MALLOC
#include "../include/gxmem.h"
#ifndef DLMS_IGNORE_STRING_CONVERTER
#include <string.h>
#include "../include/objectarray.h"
#include "../include/gxkey.h"
#endif //DLMS_IGNORE_STRING_CONVERTER
#else
#include "../include/enums.h"
#endif //DLMS_IGNORE_MALLOC
#include <string.h>
#include "../include/helpers.h"

int obj_typeToString(DLMS_OBJECT_TYPE type, char* buff)
{
    const char* str = obj_typeToString2(type);
    if (str != NULL)
    {
        memcpy(buff, str, strlen(str));
    }
    else
    {
        buff[0] = '\0';
    }
    return 0;
}

int obj_UInt16ArrayToString(gxByteBuffer* bb, gxArray* arr)
{
    int ret = 0;
    uint16_t pos;
    uint16_t* it;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(bb, ", ");
        }
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = arr_getByIndex2(arr, pos, (void**)&it, sizeof(uint16_t))) != 0 ||
            (ret = bb_addIntAsString(bb, *it)) != 0)
        {
            break;
        }
#else
        if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0 ||
            (ret = bb_addIntAsString(bb, *it)) != 0)
        {
            break;
        }
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    }
    return ret;
}

const char* obj_typeToString2(DLMS_OBJECT_TYPE type)
{
    const char* ret;
    switch (type)
    {
    case DLMS_OBJECT_TYPE_NONE:
        ret = GET_STR_FROM_EEPROM("");
        break;
    case DLMS_OBJECT_TYPE_DATA:
        ret = GET_STR_FROM_EEPROM("Data");
        break;
    case DLMS_OBJECT_TYPE_REGISTER:
        ret = GET_STR_FROM_EEPROM("Register");
        break;
    case DLMS_OBJECT_TYPE_CLOCK:
        ret = GET_STR_FROM_EEPROM("Clock");
        break;
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
        ret = GET_STR_FROM_EEPROM("ActionSchedule");
        break;
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        ret = GET_STR_FROM_EEPROM("ActivityCalendar");
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        ret = GET_STR_FROM_EEPROM("AssociationLogicalName");
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        ret = GET_STR_FROM_EEPROM("AssociationShortName");
        break;
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
        ret = GET_STR_FROM_EEPROM("AutoAnswer");
        break;
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
        ret = GET_STR_FROM_EEPROM("AutoConnect");
        break;
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        ret = GET_STR_FROM_EEPROM("DemandRegister");
        break;
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
        ret = GET_STR_FROM_EEPROM("MACAddressSetup");
        break;
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        ret = GET_STR_FROM_EEPROM("ExtendedRegister");
        break;
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
        ret = GET_STR_FROM_EEPROM("GPRSSetup");
        break;
    case DLMS_OBJECT_TYPE_SECURITY_SETUP:
        ret = GET_STR_FROM_EEPROM("SecuritySetup");
        break;
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
        ret = GET_STR_FROM_EEPROM("IECHDLCSetup");
        break;
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
        ret = GET_STR_FROM_EEPROM("IECLocalPortSetup");
        break;
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
        ret = GET_STR_FROM_EEPROM("IECTwistedPairSetup");
        break;
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = GET_STR_FROM_EEPROM("IP4Setup");
        break;
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = GET_STR_FROM_EEPROM("IP6Setup");
        break;
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        ret = GET_STR_FROM_EEPROM("MBUSSlavePortSetup");
        break;
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        ret = GET_STR_FROM_EEPROM("ImageTransfer");
        break;
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        ret = GET_STR_FROM_EEPROM("DisconnectControl");
        break;
    case DLMS_OBJECT_TYPE_LIMITER:
        ret = GET_STR_FROM_EEPROM("Limiter");
        break;
    case DLMS_OBJECT_TYPE_MBUS_CLIENT:
        ret = GET_STR_FROM_EEPROM("MBUSClient");
        break;
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
        ret = GET_STR_FROM_EEPROM("ModemConfiguration");
        break;
    case DLMS_OBJECT_TYPE_PPP_SETUP:
        ret = GET_STR_FROM_EEPROM("PPPSetup");
        break;
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        ret = GET_STR_FROM_EEPROM("ProfileGeneric");
        break;
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        ret = GET_STR_FROM_EEPROM("RegisterActivation");
        break;
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
        ret = GET_STR_FROM_EEPROM("RegisterMonitor");
        break;
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        ret = GET_STR_FROM_EEPROM("RegisterTable");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
        ret = GET_STR_FROM_EEPROM("ZigBeeSASStartup");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
        ret = GET_STR_FROM_EEPROM("ZigBeeSASJoin");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
        ret = GET_STR_FROM_EEPROM("ZigBeeSASAPSFragmentation");
        break;
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
        ret = GET_STR_FROM_EEPROM("ZigBeeNetworkControl");
        break;
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
        ret = GET_STR_FROM_EEPROM("SAPAssignment");
        break;
#ifndef DLMS_IGNORE_SCHEDULE
    case DLMS_OBJECT_TYPE_SCHEDULE:
        ret = GET_STR_FROM_EEPROM("Schedule");
        break;
#endif //DLMS_IGNORE_SCHEDULE
#ifndef DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        ret = GET_STR_FROM_EEPROM("ScriptTable");
        break;
#endif //DLMS_IGNORE_SCRIPT_TABLE
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
        ret = GET_STR_FROM_EEPROM("SMTPSetup");
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        ret = GET_STR_FROM_EEPROM("SpecialDaysTable");
        break;
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
        ret = GET_STR_FROM_EEPROM("StatusMapping");
        break;
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
        ret = GET_STR_FROM_EEPROM("TCPUDPSetup");
        break;
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        ret = GET_STR_FROM_EEPROM("UtilityTables");
        break;
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = GET_STR_FROM_EEPROM("MBUSMasterPortSetup");
        break;
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        ret = GET_STR_FROM_EEPROM("PushSetup");
        break;
    case DLMS_OBJECT_TYPE_DATA_PROTECTION:
        ret = GET_STR_FROM_EEPROM("DataProtection");
        break;
    case DLMS_OBJECT_TYPE_ACCOUNT:
        ret = GET_STR_FROM_EEPROM("Account");
        break;
    case DLMS_OBJECT_TYPE_CREDIT:
        ret = GET_STR_FROM_EEPROM("Credit");
        break;
    case DLMS_OBJECT_TYPE_CHARGE:
        ret = GET_STR_FROM_EEPROM("Charge");
        break;
    case DLMS_OBJECT_TYPE_TOKEN_GATEWAY:
        ret = GET_STR_FROM_EEPROM("TokenGateway");
        break;
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = GET_STR_FROM_EEPROM("LLCSSCSSetup");
        break;
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = GET_STR_FROM_EEPROM("PrimeNbOfdmPLCphysicalLayerCounters");
        break;
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = GET_STR_FROM_EEPROM("PrimeNbOfdmPLCMacSetup");
        break;
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = GET_STR_FROM_EEPROM("PrimeNbOfdmPLCMacFunctionalParameters");
        break;
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = GET_STR_FROM_EEPROM("PrimeNbOfdmPLCMacCounters");
        break;
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = GET_STR_FROM_EEPROM("PrimeNbOfdmPLCMacNetworkAdministrationData");
        break;
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = GET_STR_FROM_EEPROM("PrimeNbOfdmPLCApplicationsIdentification");
        break;
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        ret = GET_STR_FROM_EEPROM("ParameterMonitor");
        break;
    case DLMS_OBJECT_TYPE_COMPACT_DATA:
        ret = GET_STR_FROM_EEPROM("CompactData");
        break;
    case DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC:
        ret = GET_STR_FROM_EEPROM("GsmDiagnostic");
        break;
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = GET_STR_FROM_EEPROM("Arbitrator");
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = GET_STR_FROM_EEPROM("Iec8802LlcType1Setup");
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ret = GET_STR_FROM_EEPROM("Iec8802LlcType2Setup");
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ret = GET_STR_FROM_EEPROM("Iec8802LlcType3Setup");
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ret = GET_STR_FROM_EEPROM("SFSKActiveInitiator");
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ret = GET_STR_FROM_EEPROM("FSKMacCounters");
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ret = GET_STR_FROM_EEPROM("SFSKMacSynchronizationTimeouts");
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ret = GET_STR_FROM_EEPROM("SFSKPhyMacSetUp");
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ret = GET_STR_FROM_EEPROM("SFSKReportingSystemList");
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
#ifdef DLMS_ITALIAN_STANDARD
    case DLMS_OBJECT_TYPE_TARIFF_PLAN:
        ret = GET_STR_FROM_EEPROM("TariffPlan");
        break;
#endif //DLMS_ITALIAN_STANDARD
    default: //Unknown type.
        ret = NULL;
        break;
    }
    return ret;
}

#ifndef DLMS_IGNORE_MALLOC
#ifndef DLMS_IGNORE_STRING_CONVERTER

const char* obj_getUnitAsString(unsigned char unit)
{
    const char* ret;
    switch (unit)
    {
    case DLMS_UNIT_NONE:
        ret = GET_STR_FROM_EEPROM("None");
        break;
    case DLMS_UNIT_YEAR:
        ret = GET_STR_FROM_EEPROM("Year");
        break;
    case DLMS_UNIT_MONTH:
        ret = GET_STR_FROM_EEPROM("Month");
        break;
    case DLMS_UNIT_WEEK:
        ret = GET_STR_FROM_EEPROM("Week");
        break;
    case DLMS_UNIT_DAY:
        ret = GET_STR_FROM_EEPROM("Day");
        break;
    case DLMS_UNIT_HOUR:
        ret = GET_STR_FROM_EEPROM("Hour");
        break;
    case DLMS_UNIT_MINUTE:
        ret = GET_STR_FROM_EEPROM("Minute");
        break;
    case DLMS_UNIT_SECOND:
        ret = GET_STR_FROM_EEPROM("Second");
        break;
    case DLMS_UNIT_PHASE_ANGLE_DEGREE:
        ret = GET_STR_FROM_EEPROM("PhaseAngle");
        break;
    case DLMS_UNIT_TEMPERATURE:
        ret = GET_STR_FROM_EEPROM("Temperature");
        break;
    case DLMS_UNIT_LOCAL_CURRENCY:
        ret = GET_STR_FROM_EEPROM("LocalCurrency");
        break;
    case DLMS_UNIT_LENGTH:
        ret = GET_STR_FROM_EEPROM("Length");
        break;
    case DLMS_UNIT_SPEED:
        ret = GET_STR_FROM_EEPROM("Speed");
        break;
    case DLMS_UNIT_VOLUME_CUBIC_METER:
        ret = GET_STR_FROM_EEPROM("Volume Cubic Meter");
        break;
    case DLMS_UNIT_CORRECTED_VOLUME:
        ret = GET_STR_FROM_EEPROM("Corrected volume");
        break;
    case DLMS_UNIT_VOLUME_FLUX_HOUR:
        ret = GET_STR_FROM_EEPROM("Volume flux hour");
        break;
    case DLMS_UNIT_CORRECTED_VOLUME_FLUX_HOUR:
        ret = GET_STR_FROM_EEPROM("Corrected volume flux hour");
        break;
    case DLMS_UNIT_VOLUME_FLUX_DAY:
        ret = GET_STR_FROM_EEPROM("Volume flux day");
        break;
    case DLMS_UNIT_CORRECTED_VOLUME_FLUX_DAY:
        ret = GET_STR_FROM_EEPROM("Corrected volume flux day");
        break;
    case DLMS_UNIT_VOLUME_LITER:
        ret = GET_STR_FROM_EEPROM("Volume liter");
        break;
    case DLMS_UNIT_MASS_KG:
        ret = GET_STR_FROM_EEPROM("Mass Kg");
        break;
    case DLMS_UNIT_FORCE:
        ret = GET_STR_FROM_EEPROM("Force");
        break;
    case DLMS_UNIT_ENERGY:
        ret = GET_STR_FROM_EEPROM("Energy");
        break;
    case DLMS_UNIT_PRESSURE_PASCAL:
        ret = GET_STR_FROM_EEPROM("Pressure pascal");
        break;
    case DLMS_UNIT_PRESSURE_BAR:
        ret = GET_STR_FROM_EEPROM("Pressure Bar");
        break;
    case DLMS_UNIT_ENERGY_JOULE:
        ret = GET_STR_FROM_EEPROM("Energy joule");
        break;
    case DLMS_UNIT_THERMAL_POWER:
        ret = GET_STR_FROM_EEPROM("Thermal power");
        break;
    case DLMS_UNIT_ACTIVE_POWER:
        ret = GET_STR_FROM_EEPROM("Active power");
        break;
    case DLMS_UNIT_APPARENT_POWER:
        ret = GET_STR_FROM_EEPROM("Apparent power");
        break;
    case DLMS_UNIT_REACTIVE_POWER:
        ret = GET_STR_FROM_EEPROM("Reactive power");
        break;
    case DLMS_UNIT_ACTIVE_ENERGY:
        ret = GET_STR_FROM_EEPROM("Active energy");
        break;
    case DLMS_UNIT_APPARENT_ENERGY:
        ret = GET_STR_FROM_EEPROM("Apparent energy");
        break;
    case DLMS_UNIT_REACTIVE_ENERGY:
        ret = GET_STR_FROM_EEPROM("Reactive energy");
        break;
    case DLMS_UNIT_CURRENT:
        ret = GET_STR_FROM_EEPROM("Current");
        break;
    case DLMS_UNIT_ELECTRICAL_CHARGE:
        ret = GET_STR_FROM_EEPROM("ElectricalCharge");
        break;
    case DLMS_UNIT_VOLTAGE:
        ret = GET_STR_FROM_EEPROM("Voltage");
        break;
    case DLMS_UNIT_ELECTRICAL_FIELD_STRENGTH:
        ret = GET_STR_FROM_EEPROM("Electrical field strength E V/m");
        break;
    case DLMS_UNIT_CAPACITY:
        ret = GET_STR_FROM_EEPROM("Capacity C farad C/V = As/V");
        break;
    case DLMS_UNIT_RESISTANCE:
        ret = GET_STR_FROM_EEPROM("Resistance");
        break;
    case DLMS_UNIT_RESISTIVITY:
        ret = GET_STR_FROM_EEPROM("Resistivity");
        break;
    case DLMS_UNIT_MAGNETIC_FLUX:
        ret = GET_STR_FROM_EEPROM("Magnetic flux F weber Wb = Vs");
        break;
    case DLMS_UNIT_INDUCTION:
        ret = GET_STR_FROM_EEPROM("Induction T tesla Wb/m2");
        break;
    case DLMS_UNIT_MAGNETIC:
        ret = GET_STR_FROM_EEPROM("Magnetic field strength H A/m");
        break;
    case DLMS_UNIT_INDUCTIVITY:
        ret = GET_STR_FROM_EEPROM("Inductivity L henry H = Wb/A");
        break;
    case DLMS_UNIT_FREQUENCY:
        ret = GET_STR_FROM_EEPROM("Frequency");
        break;
    case DLMS_UNIT_ACTIVE:
        ret = GET_STR_FROM_EEPROM("Active energy");
        break;
    case DLMS_UNIT_REACTIVE:
        ret = GET_STR_FROM_EEPROM("Reactive energy");
        break;
    case DLMS_UNIT_APPARENT:
        ret = GET_STR_FROM_EEPROM("Apparent energy");
        break;
    case DLMS_UNIT_V260:
        ret = GET_STR_FROM_EEPROM("V260*60s");
        break;
    case DLMS_UNIT_A260:
        ret = GET_STR_FROM_EEPROM("A260*60s");
        break;
    case DLMS_UNIT_MASS_KG_PER_SECOND:
        ret = GET_STR_FROM_EEPROM("Mass");
        break;
    case DLMS_UNIT_CONDUCTANCE:
        ret = GET_STR_FROM_EEPROM("Conductance siemens");
        break;
    case DLMS_UNIT_KELVIN:
        ret = GET_STR_FROM_EEPROM("Kelvin");
        break;
    case DLMS_UNIT_RU2H:
        ret = GET_STR_FROM_EEPROM("RU2h");
        break;
    case DLMS_UNIT_RI2H:
        ret = GET_STR_FROM_EEPROM("RI2h");
        break;
    case DLMS_UNIT_CUBIC_METER_RV:
        ret = GET_STR_FROM_EEPROM("Cubic meter RV");
        break;
    case DLMS_UNIT_PERCENTAGE:
        ret = GET_STR_FROM_EEPROM("Percentage");
        break;
    case DLMS_UNIT_AMPERE_HOURS:
        ret = GET_STR_FROM_EEPROM("Ampere hours");
        break;
    case DLMS_UNIT_ENERGY_PER_VOLUME:
        ret = GET_STR_FROM_EEPROM("Energy per volume");
        break;
    case DLMS_UNIT_WOBBE:
        ret = GET_STR_FROM_EEPROM("Wobbe");
        break;
    case DLMS_UNIT_MOLE_PERCENT:
        ret = GET_STR_FROM_EEPROM("Mole percent");
        break;
    case DLMS_UNIT_MASS_DENSITY:
        ret = GET_STR_FROM_EEPROM("Mass density");
        break;
    case DLMS_UNIT_PASCAL_SECOND:
        ret = GET_STR_FROM_EEPROM("Pascal second");
        break;
    case DLMS_UNIT_JOULE_KILOGRAM:
        ret = GET_STR_FROM_EEPROM("Joule kilogram");
        break;
    case DLMS_UNIT_PRESSURE_GRAM_PER_SQUARE_CENTIMETER:
        ret = GET_STR_FROM_EEPROM("Pressure, gram per square centimeter.");
        break;
    case DLMS_UNIT_PRESSURE_ATMOSPHERE:
        ret = GET_STR_FROM_EEPROM("Pressure, atmosphere.");
        break;
    case DLMS_UNIT_SIGNAL_STRENGTH_MILLI_WATT:
        ret = GET_STR_FROM_EEPROM("Signal strength, dB milliwatt");
        break;
    case DLMS_UNIT_SIGNAL_STRENGTH_MICRO_VOLT:
        //logarithmic unit that expresses the ratio between two values of a physical quantity
        ret = GET_STR_FROM_EEPROM("Signal strength, dB microvolt");
        break;
    case DLMS_UNIT_DB:
        ret = GET_STR_FROM_EEPROM("dB");
        break;
    case DLMS_UNIT_INCH:
        ret = GET_STR_FROM_EEPROM("Inch");
        break;
    case DLMS_UNIT_FOOT:
        ret = GET_STR_FROM_EEPROM("Foot");
        break;
    case DLMS_UNIT_POUND:
        ret = GET_STR_FROM_EEPROM("Pound");
        break;
    case DLMS_UNIT_FAHRENHEIT:
        ret = GET_STR_FROM_EEPROM("Fahrenheit");
        break;
    case DLMS_UNIT_RANKINE:
        ret = GET_STR_FROM_EEPROM("Rankine");
        break;
    case DLMS_UNIT_SQUARE_INCH:
        ret = GET_STR_FROM_EEPROM("Square inch");
        break;
    case DLMS_UNIT_SQUARE_FOOT:
        ret = GET_STR_FROM_EEPROM("Square foot");
        break;
    case DLMS_UNIT_ACRE:
        ret = GET_STR_FROM_EEPROM("Acre");
        break;
    case DLMS_UNIT_CUBIC_INCH:
        ret = GET_STR_FROM_EEPROM("Cubic inch");
        break;
    case DLMS_UNIT_CUBIC_FOOT:
        ret = GET_STR_FROM_EEPROM("Cubic foot");
        break;
    case DLMS_UNIT_ACRE_FOOT:
        ret = GET_STR_FROM_EEPROM("Acre foot");
        break;
    case DLMS_UNIT_GALLON_IMPERIAL:
        ret = GET_STR_FROM_EEPROM("Gallon Imperial");
        break;
    case DLMS_UNIT_GALLON_US:
        ret = GET_STR_FROM_EEPROM("GallonUS");
        break;
    case DLMS_UNIT_POUND_FORCE:
        ret = GET_STR_FROM_EEPROM("Pound force");
        break;
    case DLMS_UNIT_POUND_FORCE_PER_SQUARE_INCH:
        ret = GET_STR_FROM_EEPROM("Pound force per square inch");
        break;
    case DLMS_UNIT_POUND_PER_CUBIC_FOOT:
        ret = GET_STR_FROM_EEPROM("Pound per cubic foot");
        break;
    case DLMS_UNIT_POUND_PER_FOOT_SECOND:
        ret = GET_STR_FROM_EEPROM("Pound per foot second");
        break;
    case DLMS_UNIT_BRITISH_THERMAL_UNIT:
        ret = GET_STR_FROM_EEPROM("British thermal unit");
        break;
    case DLMS_UNIT_THERM_EU:
        ret = GET_STR_FROM_EEPROM("Therm EU");
        break;
    case DLMS_UNIT_THERM_US:
        ret = GET_STR_FROM_EEPROM("Therm US");
        break;
    case DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_POUND:
        ret = GET_STR_FROM_EEPROM("British thermal unit per pound");
        break;
    case DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_CUBIC_FOOT:
        ret = GET_STR_FROM_EEPROM("British thermal unit per cubic foot");
        break;
    case DLMS_UNIT_CUBIC_FEET:
        ret = GET_STR_FROM_EEPROM("Cubic feet");
        break;
    case DLMS_UNIT_FOOT_PER_SECOND:
        ret = GET_STR_FROM_EEPROM("Foot per second");
        break;
    case DLMS_UNIT_CUBIC_FOOT_PER_MIN:
        ret = GET_STR_FROM_EEPROM("Foot per min");
        break;
    case DLMS_UNIT_CUBIC_FOOT_PER_DAY:
        ret = GET_STR_FROM_EEPROM("Foot per day");
        break;
    case DLMS_UNIT_ACRE_FOOT_PER_SECOND:
        ret = GET_STR_FROM_EEPROM("Acre foot per second");
        break;
    case DLMS_UNIT_ACRE_FOOT_PER_MIN:
        ret = GET_STR_FROM_EEPROM("Acre foot per min");
        break;
    case DLMS_UNIT_ACRE_FOOT_PER_HOUR:
        ret = GET_STR_FROM_EEPROM("Acre foot per hour");
        break;
    case DLMS_UNIT_ACRE_FOOT_PER_DAY:
        ret = GET_STR_FROM_EEPROM("Acre foot per day");
        break;
    case DLMS_UNIT_IMPERIAL_GALLON:
        ret = GET_STR_FROM_EEPROM("Imperial gallon");
        break;
    case DLMS_UNIT_IMPERIAL_GALLON_PER_SECOND:
        ret = GET_STR_FROM_EEPROM("Imperial gallon per second");
        break;
    case DLMS_UNIT_IMPERIAL_GALLON_PER_MIN:
        ret = GET_STR_FROM_EEPROM("Imperial gallon per min");
        break;
    case DLMS_UNIT_IMPERIAL_GALLON_PER_HOUR:
        ret = GET_STR_FROM_EEPROM("Imperial gallon per hour");
        break;
    case DLMS_UNIT_IMPERIAL_GALLON_PER_DAY:
        ret = GET_STR_FROM_EEPROM("Imperial gallon per day");
        break;
    case DLMS_UNIT_US_GALLON:
        ret = GET_STR_FROM_EEPROM("US Gallon");
        break;
    case DLMS_UNIT_US_GALLON_PER_SECOND:
        ret = GET_STR_FROM_EEPROM("US gallon per second");
        break;
    case DLMS_UNIT_US_GALLON_PER_MIN:
        ret = GET_STR_FROM_EEPROM("US gallon per min");
        break;
    case DLMS_UNIT_US_GALLON_PER_HOUR:
        ret = GET_STR_FROM_EEPROM("US gallon per hour");
        break;
    case DLMS_UNIT_US_GALLON_PER_DAY:
        ret = GET_STR_FROM_EEPROM("US gallon per day");
        break;
    case DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_SECOND:
        ret = GET_STR_FROM_EEPROM("British thermal unit per second");
        break;
    case DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_MIN:
        ret = GET_STR_FROM_EEPROM("British thermal unit per min");
        break;
    case DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_HOUR:
        ret = GET_STR_FROM_EEPROM("British thermal unit per hour");
        break;
    case DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_DAY:
        ret = GET_STR_FROM_EEPROM("British thermal unit per day");
        break;
    case DLMS_UNIT_OTHER:
        ret = GET_STR_FROM_EEPROM("Other unit");
        break;
    case DLMS_UNIT_NO_UNIT:
        ret = GET_STR_FROM_EEPROM("NoUnit");
        break;
    default:
        ret = NULL;
        break;
    }
    return ret;
}

#ifndef DLMS_IGNORE_DATA
int obj_DataToString(gxData* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    ret = var_toString(&object->value, &ba);
    bb_addString(&ba, "\n");
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
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 3 Value: Scaler: ");
    bb_addDoubleAsString(&ba, hlp_getScaler(object->scaler));
    bb_addString(&ba, " Unit: ");
    bb_addString(&ba, obj_getUnitAsString(object->unit));
    bb_addString(&ba, "\nIndex: 2 Value: ");
    ret = var_toString(&object->value, &ba);
    bb_addString(&ba, "\n");
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
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    ret = time_toString(&object->time, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->timeZone);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    ret = time_toString(&object->begin, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 6 Value: ");
    ret = time_toString(&object->end, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->deviation);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->enabled);
    bb_addString(&ba, "\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->clockBase);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_CLOCK
#ifndef DLMS_IGNORE_SCRIPT_TABLE
int obj_ScriptTableToString(gxScriptTable* object, char** buff)
{
    int ret;
    uint16_t pos, pos2;
    gxByteBuffer ba;
    gxScript* s;
    gxScriptAction* sa;
    BYTE_BUFFER_INIT(&ba);
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
        bb_addString(&ba, "\n");
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
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if (sa->target == NULL)
            {
                bb_addIntAsString(&ba, 0);
                bb_addString(&ba, " ");
                hlp_appendLogicalName(&ba, EMPTY_LN);
            }
            else
            {
                bb_addIntAsString(&ba, sa->target->objectType);
                bb_addString(&ba, " ");
                hlp_appendLogicalName(&ba, sa->target->logicalName);
            }
#else
            bb_addIntAsString(&ba, sa->objectType);
            bb_addString(&ba, " ");
            hlp_appendLogicalName(&ba, sa->logicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS

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
    bb_addString(&ba, "]\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
            }
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
int obj_specialDaysTableToString(gxSpecialDaysTable* object, char** buff)
{
    int ret;
    uint16_t pos;
    gxSpecialDay* sd;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
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
    bb_addString(&ba, "]\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
int obj_TcpUdpSetupToString(gxTcpUdpSetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->port);
    bb_addString(&ba, "\nIndex: 3 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    bb_addLogicalName(&ba, obj_getLogicalName((gxObject*)object->ipSetup));
#else
    bb_addLogicalName(&ba, object->ipReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS

    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->maximumSegmentSize);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->maximumSimultaneousConnections);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->inactivityTimeout);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
int obj_mBusMasterPortSetupToString(gxMBusMasterPortSetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->commSpeed);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

int obj_timeWindowToString(gxArray* arr, gxByteBuffer* bb)
{
    gxKey* it;
    int ret = 0;
    uint16_t pos;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
        {
            break;
        }
        if (pos != 0)
        {
            bb_addString(bb, ", ");
        }
        if ((ret = time_toString((gxtime*)it->key, bb)) != 0 ||
            (ret = bb_addString(bb, " ")) != 0 ||
            (ret = time_toString((gxtime*)it->value, bb)) != 0)
        {
            break;
        }
    }
    return ret;
}

int obj_CaptureObjectsToString(gxByteBuffer* ba, gxArray* objects)
{
    uint16_t pos;
    int ret = DLMS_ERROR_CODE_OK;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxTarget* it;
#else
    gxKey* it;
#endif //#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    for (pos = 0; pos != objects->size; ++pos)
    {
        if ((ret = arr_getByIndex(objects, pos, (void**)&it)) != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        if (pos != 0)
        {
            bb_addString(ba, ", ");
        }
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if ((ret = bb_addString(ba, obj_typeToString2((DLMS_OBJECT_TYPE)it->target->objectType))) != 0 ||
            (ret = bb_addString(ba, " ")) != 0 ||
            (ret = hlp_appendLogicalName(ba, it->target->logicalName)) != 0)
        {
            break;
        }
#else
        if ((ret = bb_addString(ba, obj_typeToString2(((gxObject*)it->key)->objectType))) != 0 ||
            (ret = bb_addString(ba, " ")) != 0 ||
            (ret = hlp_appendLogicalName(ba, ((gxObject*)it->key)->logicalName)) != 0)
        {
            break;
        }
#endif //#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    }
    return ret;
}

#ifndef DLMS_IGNORE_PUSH_SETUP
int obj_pushSetupToString(gxPushSetup* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    if ((ret = BYTE_BUFFER_INIT(&ba)) == 0 &&
        (ret = bb_addString(&ba, "Index: 2 Value: ")) == 0 &&
        (ret = obj_CaptureObjectsToString(&ba, &object->pushObjectList)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_set(&ba, object->destination.data, object->destination.size)) == 0 &&
        (ret = bb_addString(&ba, " ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->message)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 4 Value: ")) == 0 &&
        (ret = obj_timeWindowToString(&object->communicationWindow, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->randomisationStartInterval)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->numberOfRetries)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 7 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->repetitionDelay)) == 0 &&
        (ret = bb_addString(&ba, "\n")) == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_PUSH_SETUP
#ifndef DLMS_IGNORE_AUTO_CONNECT
int obj_autoConnectToString(gxAutoConnect* object, char** buff)
{
    gxKey* k;
    int ret;
    uint16_t pos;
    gxByteBuffer ba, * dest;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->mode);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->repetitions);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->repetitionDelay);
    bb_addString(&ba, "\nIndex: 5 Value: [");
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
    bb_addString(&ba, "\nIndex: 6 Value: ");
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
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
    }
#endif //DLMS_IGNORE_AUTO_CONNECT
#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR
int obj_seasonProfileToString(gxArray* arr, gxByteBuffer* ba)
{
    gxSeasonProfile* it;
    int ret;
    uint16_t pos;
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
        bb_addString(ba, "{");
        bb_attachString(ba, bb_toHexString(&it->name));
        bb_addString(ba, ", ");
        time_toString(&it->start, ba);
        bb_addString(ba, ", ");
        bb_attachString(ba, bb_toHexString(&it->weekName));
        bb_addString(ba, "}");
    }
    bb_addString(ba, "]");
    return 0;
}
int obj_weekProfileToString(gxArray* arr, gxByteBuffer* ba)
{
    gxWeekProfile* it;
    int ret;
    uint16_t pos;
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
    gxDayProfileAction* it;
    int ret;
    uint16_t pos, pos2;
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
#ifndef DLMS_IGNORE_OBJECT_POINTERS
            if (it->script == NULL)
            {
                hlp_appendLogicalName(ba, EMPTY_LN);
            }
            else
            {
                hlp_appendLogicalName(ba, it->script->logicalName);
            }
#else
            hlp_appendLogicalName(ba, it->scriptLogicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS
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
    BYTE_BUFFER_INIT(&ba);
    if ((ret = bb_addString(&ba, "Index: 2 Value: ")) == 0 &&
        (ret = bb_attachString(&ba, bb_toString(&object->calendarNameActive))) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 3 Value: ")) == 0 &&
        (ret = obj_seasonProfileToString(&object->seasonProfileActive, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 4 Value: ")) == 0 &&
        (ret = obj_weekProfileToString(&object->weekProfileTableActive, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 5 Value: ")) == 0 &&
        (ret = obj_dayProfileToString(&object->dayProfileTableActive, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_attachString(&ba, bb_toString(&object->calendarNamePassive))) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 7 Value: ")) == 0 &&
        (ret = obj_seasonProfileToString(&object->seasonProfilePassive, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 8 Value: ")) == 0 &&
        (ret = obj_weekProfileToString(&object->weekProfileTablePassive, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 9 Value: ")) == 0 &&
        (ret = obj_dayProfileToString(&object->dayProfileTablePassive, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 10 Value: ")) == 0 &&
        (ret = time_toString(&object->time, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\n")) == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_ACTIVITY_CALENDAR
#ifndef DLMS_IGNORE_SECURITY_SETUP
int obj_securitySetupToString(gxSecuritySetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->securityPolicy);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->securitySuite);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_attachString(&ba, bb_toHexString(&object->serverSystemTitle));
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_attachString(&ba, bb_toHexString(&object->clientSystemTitle));
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
int obj_hdlcSetupToString(gxIecHdlcSetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->communicationSpeed);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->windowSizeTransmit);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->windowSizeReceive);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->maximumInfoLengthTransmit);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->maximumInfoLengthReceive);
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->interCharachterTimeout);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->inactivityTimeout);
    bb_addString(&ba, "\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->deviceAddress);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
int obj_localPortSetupToString(gxLocalPortSetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->defaultMode);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->defaultBaudrate);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->proposedBaudrate);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->responseTime);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_attachString(&ba, bb_toString(&object->deviceAddress));
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_attachString(&ba, bb_toString(&object->password1));
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_attachString(&ba, bb_toString(&object->password2));
    bb_addString(&ba, "\nIndex: 9 Value: ");
    bb_attachString(&ba, bb_toString(&object->password5));
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
int obj_IecTwistedPairSetupToString(gxIecTwistedPairSetup* object, char** buff)
{
    int pos, ret;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    if ((ret = bb_addString(&ba, "Index: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->mode)) == 0 &&
        (ret = bb_addString(&ba, "Index: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, (int)object->speed)) == 0)
    {
        bb_addString(&ba, "Index: 4 Value: [");
        for (pos = 0; pos != object->primaryAddresses.size; ++pos)
        {
            if (pos != 0)
            {
                bb_addString(&ba, ", ");
            }
            if ((ret = bb_addIntAsString(&ba, object->primaryAddresses.data[pos])) != 0)
            {
                break;
            }
        }
        if (ret == 0 &&
            (ret = bb_addString(&ba, "]\n")) == 0 &&
            (ret = bb_addString(&ba, "Index: 5 Value: [")) == 0)
        {
            for (pos = 0; pos != object->tabis.size; ++pos)
            {
                if (pos != 0)
                {
                    bb_addString(&ba, ", ");
                }
                if ((ret = bb_addIntAsString(&ba, object->tabis.data[pos])) != 0)
                {
                    break;
                }
            }
            if (ret == 0)
            {
                ret = bb_addString(&ba, "]\n");
            }
        }
            }
    if (ret == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return ret;
    }
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_DEMAND_REGISTER
int obj_demandRegisterToString(gxDemandRegister* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    if ((ret = bb_addString(&ba, "Index: 2 Value: ")) == 0 &&
        (ret = var_toString(&object->currentAverageValue, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 3 Value: ")) == 0 &&
        (ret = var_toString(&object->lastAverageValue, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 4 Value: Scaler: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->scaler)) == 0 &&
        (ret = bb_addString(&ba, " Unit: ")) == 0 &&
        (ret = bb_addString(&ba, obj_getUnitAsString(object->unit))) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 5 Value: ")) == 0 &&
        (ret = var_toString(&object->status, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 6 Value: ")) == 0 &&
        (ret = time_toString(&object->captureTime, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 7 Value: ")) == 0 &&
        (ret = time_toString(&object->startTimeCurrent, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 8 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->period)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 9 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->numberOfPeriods)) == 0 &&
        (ret = bb_addString(&ba, "\n")) == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_DEMAND_REGISTER
#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
int obj_registerActivationToString(gxRegisterActivation* object, char** buff)
{
    int ret = 0;
    uint16_t pos;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
    gxObjectDefinition* od;
    gxKey* it;
#else
    gxObject* od;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    gxRegisterActivationMask* it;
#else
    gxKey* it;
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#endif //DLMS_IGNORE_OBJECT_POINTERS
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    for (pos = 0; pos != object->registerAssignment.size; ++pos)
    {
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = oa_getByIndex(&object->registerAssignment, pos, &od);
#else
        ret = arr_getByIndex(&object->registerAssignment, pos, (void**)&od);
#endif //DLMS_IGNORE_OBJECT_POINTERS
        if (ret != 0)
        {
            break;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
        }
        if ((ret = bb_addIntAsString(&ba, od->objectType)) != 0 ||
            (ret = bb_addString(&ba, " ")) != 0 ||
            (ret = hlp_appendLogicalName(&ba, od->logicalName)) != 0)
        {
            break;
        }
    }
    if (ret == 0)
    {
        if ((ret = bb_addString(&ba, "]\n")) == 0 &&
            (ret = bb_addString(&ba, "Index: 3 Value: [")) == 0)
        {
            for (pos = 0; pos != object->maskList.size; ++pos)
            {
                ret = arr_getByIndex(&object->maskList, pos, (void**)&it);
                if (ret != 0)
                {
                    break;
                }
                if (pos != 0)
                {
                    bb_addString(&ba, ", ");
            }
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
                bb_attachString(&ba, bb_toString((gxByteBuffer*)it->key));
                bb_addString(&ba, " ");
                bb_attachString(&ba, bb_toString((gxByteBuffer*)it->value));
#else
                if ((ret = bb_attachString(&ba, bb_toHexString(&it->name))) != 0 ||
                    (ret = bb_addString(&ba, ": ")) != 0 ||
                    (ret = bb_attachString(&ba, bb_toHexString(&it->indexes))) != 0)
                {
                    break;
                }
#endif //DLMS_IGNORE_OBJECT_POINTERS
        }
            bb_addString(&ba, "]\n");
            *buff = bb_toString(&ba);
    }
}
    bb_clear(&ba);
    return ret;
        }
#endif //DLMS_IGNORE_REGISTER_ACTIVATION
#ifndef DLMS_IGNORE_REGISTER_MONITOR
void actionItemToString(gxActionItem* item, gxByteBuffer* ba)
{
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (item->script == NULL)
    {
        hlp_appendLogicalName(ba, EMPTY_LN);
    }
    else
    {
        hlp_appendLogicalName(ba, item->script->base.logicalName);
    }
#else
    hlp_appendLogicalName(ba, item->logicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    bb_addString(ba, " ");
    bb_addIntAsString(ba, item->scriptSelector);
}
int obj_registerMonitorToString(gxRegisterMonitor* object, char** buff)
{
    int ret;
    uint16_t pos;
    dlmsVARIANT* tmp;
    gxByteBuffer ba;
    gxActionSet* as;
    BYTE_BUFFER_INIT(&ba);
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
    bb_addString(&ba, "]\nIndex: 3 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (object->monitoredValue.target == NULL)
    {
        hlp_appendLogicalName(&ba, EMPTY_LN);
        bb_addString(&ba, " ");
        bb_addString(&ba, obj_typeToString2(0));
    }
    else
    {
        hlp_appendLogicalName(&ba, object->monitoredValue.target->logicalName);
        bb_addString(&ba, " ");
        bb_addString(&ba, obj_typeToString2(object->monitoredValue.target->objectType));
    }
#else
    hlp_appendLogicalName(&ba, object->monitoredValue.logicalName);
    bb_addString(&ba, " ");
    bb_addString(&ba, obj_typeToString2(object->monitoredValue.objectType));
#endif //DLMS_IGNORE_OBJECT_POINTERS
    bb_addString(&ba, " ");
    bb_addIntAsString(&ba, object->monitoredValue.attributeIndex);

    bb_addString(&ba, "\nIndex: 4 Value: [");
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
    bb_addString(&ba, "]\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
    }
#endif //DLMS_IGNORE_REGISTER_MONITOR
#ifndef DLMS_IGNORE_ACTION_SCHEDULE
int obj_actionScheduleToString(gxActionSchedule* object, char** buff)
{
    int ret;
    uint16_t pos;
    gxtime* tm;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (object->executedScript == NULL)
    {
        hlp_appendLogicalName(&ba, EMPTY_LN);
}
    else
    {
        hlp_appendLogicalName(&ba, object->executedScript->base.logicalName);
    }
#else
    hlp_appendLogicalName(&ba, object->executedScriptLogicalName);
#endif //DLMS_IGNORE_OBJECT_POINTERS

    bb_addString(&ba, " ");
    bb_addIntAsString(&ba, object->executedScriptSelector);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->type);

    bb_addString(&ba, "\nIndex: 4 Value: [");
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
    bb_addString(&ba, "]\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return DLMS_ERROR_CODE_OK;
}
#endif //DLMS_IGNORE_ACTION_SCHEDULE
#ifndef DLMS_IGNORE_SAP_ASSIGNMENT
int obj_sapAssignmentToString(gxSapAssignment* object, char** buff)
{
    int ret;
    uint16_t pos;
    gxSapItem* it;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
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
        bb_addString(&ba, ": ");
        bb_set2(&ba, &it->name, 0, bb_size(&it->name));
    }
    bb_addString(&ba, "]\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_SAP_ASSIGNMENT
#ifndef DLMS_IGNORE_AUTO_ANSWER
int obj_autoAnswerToString(gxAutoAnswer* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    if ((ret = BYTE_BUFFER_INIT(&ba)) == 0 &&
        (ret = bb_addString(&ba, "Index: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->mode)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 3 Value: [")) == 0 &&
        (ret = obj_timeWindowToString(&object->listeningWindow, &ba)) == 0 &&
        (ret = bb_addString(&ba, "]\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->status)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->numberOfCalls)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->numberOfRingsInListeningWindow)) == 0 &&
        (ret = bb_addString(&ba, " ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->numberOfRingsOutListeningWindow)) == 0 &&
        (ret = bb_addString(&ba, "\n")) == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_AUTO_ANSWER
#ifndef DLMS_IGNORE_IP4_SETUP
int obj_ip4SetupToString(gxIp4Setup* object, char** buff)
{
    int ret;
    uint16_t pos;
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
    dlmsVARIANT* tmp;
#else
    uint32_t* tmp;
#endif
    gxip4SetupIpOption* ip;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (object->dataLinkLayer != NULL)
    {
        bb_addLogicalName(&ba, object->dataLinkLayer->logicalName);
    }
#else
    bb_addLogicalName(&ba, object->dataLinkLayerReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->ipAddress);
    bb_addString(&ba, "\nIndex: 4 Value: [");
    for (pos = 0; pos != object->multicastIPAddress.size; ++pos)
    {
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = va_getByIndex(&object->multicastIPAddress, pos, &tmp);
#else
        ret = arr_getByIndex2(&object->multicastIPAddress, pos, (void**)&tmp, sizeof(uint32_t));
#endif
        if (ret != 0)
        {
            return ret;
        }
        if (pos != 0)
        {
            bb_addString(&ba, ", ");
    }
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        ret = bb_addIntAsString(&ba, *tmp);
#else
        ret = var_toString(tmp, &ba);
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        if (ret != 0)
        {
            return ret;
        }
}
    bb_addString(&ba, "]\nIndex: 5 Value: [");
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
    bb_addString(&ba, "]\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->subnetMask);
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->gatewayIPAddress);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->useDHCP);
    bb_addString(&ba, "\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->primaryDNSAddress);
    bb_addString(&ba, "\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->secondaryDNSAddress);
    bb_addString(&ba, "\nIndex: 11 Value: ");
    var_toString(&object->value, &ba);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
    }
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP

int obj_getIPAddress(gxByteBuffer* ba, gxArray* arr)
{
    char tmp[64];
    int ret;
    uint16_t pos;
    IN6_ADDR* ip;
    if ((ret = bb_addString(ba, "{")) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            if ((ret = arr_getByIndex(arr, pos, (void**)&ip)) != 0)
            {
                break;
            }
            if (pos != 0)
            {
                bb_addString(ba, ", ");
            }
            //Add Ws2_32.lib for LabWindows/CVI.
            inet_ntop(AF_INET6, &ip, tmp, sizeof(tmp));
            bb_addString(ba, tmp);
        }
        if (ret == 0)
        {
            ret = bb_addString(ba, "}");
        }
    }
    return ret;
}

int obj_getNeighborDiscoverySetupAsString(gxByteBuffer* ba, gxArray* arr)
{
    int ret;
    uint16_t pos;
    gxNeighborDiscoverySetup* it;
    if ((ret = bb_addString(ba, "{")) == 0)
    {
        for (pos = 0; pos != arr->size; ++pos)
        {
            if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0)
            {
                break;
            }
            if (pos != 0)
            {
                bb_addString(ba, ", ");
            }
            if ((ret = bb_addString(ba, "[")) != 0 ||
                (ret = bb_addIntAsString(ba, it->maxRetry)) != 0 ||
                (ret = bb_addString(ba, ", ")) != 0 ||
                (ret = bb_addIntAsString(ba, it->retryWaitTime)) != 0 ||
                (ret = bb_addString(ba, ", ")) != 0 ||
                (ret = bb_addIntAsString(ba, it->sendPeriod)) != 0 ||
                (ret = bb_addString(ba, "]")) != 0)
            {
                break;
            }
        }
        if (ret == 0)
        {
            ret = bb_addString(ba, "}");
        }
    }
    return ret;
}

int obj_ip6SetupToString(gxIp6Setup* object, char** buff)
{
    char tmp[64];
    int ret;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (object->dataLinkLayer != NULL)
    {
        bb_addLogicalName(&ba, object->dataLinkLayer->logicalName);
    }
#else
    bb_addLogicalName(&ba, object->dataLinkLayerReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    if ((ret = bb_addString(&ba, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->addressConfigMode)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 4 Value: [")) == 0 &&
        (ret = obj_getIPAddress(&ba, &object->unicastIPAddress)) == 0 &&
        (ret = bb_addString(&ba, "]\nIndex: 5 Value: [")) == 0 &&
        (ret = obj_getIPAddress(&ba, &object->multicastIPAddress)) == 0 &&
        (ret = bb_addString(&ba, "]\nIndex: 6 Value: [")) == 0 &&
        (ret = obj_getIPAddress(&ba, &object->gatewayIPAddress)) == 0 &&
        (ret = bb_addString(&ba, "]\nIndex: 7 Value: ")) == 0)
    {
        //Add Ws2_32.lib for LabWindows/CVI.

        inet_ntop(AF_INET6, &object->primaryDNSAddress, tmp, sizeof(tmp));
        bb_addString(&ba, tmp);
        inet_ntop(AF_INET6, &object->secondaryDNSAddress, tmp, sizeof(tmp));
        bb_addString(&ba, tmp);
        if ((ret = bb_addIntAsString(&ba, object->trafficClass)) == 0 &&
            (ret = obj_getNeighborDiscoverySetupAsString(&ba, &object->neighborDiscoverySetup)) == 0 &&
            (ret = bb_addString(&ba, "\n")) == 0)
        {
        }
    }
    if (ret == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
int obj_UtilityTablesToString(gxUtilityTables* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->tableId);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, bb_size(&object->buffer));
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_attachString(&ba, bb_toHexString(&object->buffer));
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
int obj_mbusSlavePortSetupToString(gxMbusSlavePortSetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->defaultBaud);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->availableBaud);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->addressState);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->busAddress);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
#ifndef DLMS_IGNORE_IMAGE_TRANSFER
int obj_imageTransferToString(gxImageTransfer* object, char** buff)
{
    uint16_t pos;
    int ret;
    gxImageActivateInfo* it;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->imageBlockSize);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_attachString(&ba, ba_toString(&object->imageTransferredBlocksStatus));
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->imageFirstNotTransferredBlockNumber);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->imageTransferEnabled);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->imageTransferStatus);
    bb_addString(&ba, "\nIndex: 7 Value: [");
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
    bb_addString(&ba, "]\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_IMAGE_TRANSFER
#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
int obj_disconnectControlToString(gxDisconnectControl* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->outputState);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->controlState);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->controlMode);
    bb_addString(&ba, "\n");
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
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addString(&ba, " ");
    if (object->monitoredValue != NULL)
    {
        hlp_appendLogicalName(&ba, object->monitoredValue->logicalName);
        bb_addString(&ba, ": ");
        bb_addIntAsString(&ba, object->selectedAttributeIndex);
    }
    bb_addString(&ba, "\nIndex: 3 Value: ");
    ret = var_toString(&object->thresholdActive, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 4 Value: ");
    ret = var_toString(&object->thresholdNormal, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 5 Value: ");
    ret = var_toString(&object->thresholdEmergency, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->minOverThresholdDuration);
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->minUnderThresholdDuration);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->emergencyProfile.id);
    bb_addString(&ba, " ");
    time_toString(&object->emergencyProfile.activationTime, &ba);
    bb_addString(&ba, " ");
    bb_addIntAsString(&ba, object->emergencyProfile.duration);

    bb_addString(&ba, "\nIndex: 9 Value: ");
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    if ((ret = obj_UInt16ArrayToString(&ba, &object->emergencyProfileGroupIDs)) != 0)
    {
        return ret;
    }
#else
    if ((ret = va_toString(&object->emergencyProfileGroupIDs, &ba)) != 0)
    {
        return ret;
    }
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    bb_addString(&ba, "\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->emergencyProfileActive);
    bb_addString(&ba, "\nIndex: 11 Value: ");
    actionItemToString(&object->actionOverThreshold, &ba);
    bb_addString(&ba, "\nIndex: 12 Value: ");
    actionItemToString(&object->actionUnderThreshold, &ba);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_LIMITER
#ifndef DLMS_IGNORE_MBUS_CLIENT
int obj_mBusClientToString(gxMBusClient* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->capturePeriod);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->primaryAddress);
    bb_addString(&ba, "\nIndex: 4 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (object->mBusPort != NULL)
    {
        bb_addLogicalName(&ba, object->mBusPort->logicalName);
    }
#else
    bb_addLogicalName(&ba, object->mBusPortReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    bb_addString(&ba, "\nIndex: 5 Value: ");
    //TODO: bb_addIntAsString(&ba, object->captureDefinition);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->identificationNumber);
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->manufacturerID);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->dataHeaderVersion);
    bb_addString(&ba, "\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->deviceType);
    bb_addString(&ba, "\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->accessNumber);
    bb_addString(&ba, "\nIndex: 11 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\nIndex: 12 Value: ");
    bb_addIntAsString(&ba, object->alarm);
    bb_addString(&ba, "\nIndex: 13 Value: ");
    bb_addIntAsString(&ba, object->configuration);
    bb_addString(&ba, "\nIndex: 14 Value: ");
    bb_addIntAsString(&ba, object->encryptionKeyStatus);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MBUS_CLIENT
#ifndef DLMS_IGNORE_MODEM_CONFIGURATION
int obj_modemConfigurationToString(gxModemConfiguration* object, char** buff)
{
    uint16_t pos;
    int ret;
    gxModemInitialisation* mi;
    gxByteBuffer ba, * it;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->communicationSpeed);
    bb_addString(&ba, "\nIndex: 3 Value: [");
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
    bb_addString(&ba, "]\nIndex: 4 Value: [");
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
    bb_addString(&ba, "]\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_MODEM_CONFIGURATION
#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP
int obj_macAddressSetupToString(gxMacAddressSetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_attachString(&ba, bb_toString(&object->macAddress));
    bb_addString(&ba, "\n");
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
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_attachString(&ba, bb_toString(&object->apn));
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->pinCode);
    bb_addString(&ba, "\nIndex: 4 Value: [");
    qualityOfServiceToString(&object->defaultQualityOfService, &ba);
    bb_addString(&ba, "]\nIndex: 5 Value: [");
    qualityOfServiceToString(&object->requestedQualityOfService, &ba);
    bb_addString(&ba, "]\n");
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
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    ret = var_toString(&object->value, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 3 Value: Scaler: ");
    bb_addIntAsString(&ba, object->scaler);
    bb_addString(&ba, " Unit: ");
    bb_addString(&ba, obj_getUnitAsString(object->unit));
    bb_addString(&ba, "\nIndex: 4 Value: ");
    ret = var_toString(&object->status, &ba);
    if (ret != 0)
    {
        return ret;
    }
    bb_addString(&ba, "\nIndex: 5 Value: ");
    time_toString(&object->captureTime, &ba);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_EXTENDED_REGISTER

int obj_objectsToString(gxByteBuffer* ba, objectArray* objects)
{
    uint16_t pos;
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
        bb_addString(ba, obj_typeToString2((DLMS_OBJECT_TYPE)it->objectType));
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
        assert(ret == 0);
#endif
        bb_addString(ba, " ");
        hlp_appendLogicalName(ba, it->logicalName);
        }
    return ret;
    }

int obj_rowsToString(gxByteBuffer* ba, gxArray* buffer)
{
    dlmsVARIANT* tmp;
    variantArray* va;
    int ret;
    uint16_t r, c;
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
        bb_addString(ba, "\n");
        }
    return 0;
    }

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
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
    bb_addIntAsString(ba, object->maxSendPduSize);
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
    uint16_t pos;
    int ret = 0;
    gxKey2* it;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    obj_objectsToString(&ba, &object->objectList);
    bb_addString(&ba, "]\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->clientSAP);
    bb_addString(&ba, "/");
    bb_addIntAsString(&ba, object->serverSAP);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    obj_applicationContextNameToString(&ba, &object->applicationContextName);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    obj_xDLMSContextTypeToString(&ba, &object->xDLMSContextInfo);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    obj_authenticationMechanismNameToString(&ba, &object->authenticationMechanismName);
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_attachString(&ba, bb_toString(&object->secret));
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->associationStatus);
    //Security Setup Reference is from version 1.
    if (object->base.version > 0)
    {
        bb_addString(&ba, "\nIndex: 9 Value: ");
#ifndef DLMS_IGNORE_SECURITY_SETUP
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        bb_addLogicalName(&ba, obj_getLogicalName((gxObject*)object->securitySetup));
#else
        bb_addLogicalName(&ba, object->securitySetupReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_SECURITY_SETUP
    }
    if (object->base.version > 1)
    {
        bb_addString(&ba, "\nIndex: 10 Value: [\n");
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
        bb_addString(&ba, "]\n");
    }
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}

#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
int obj_associationShortNameToString(gxAssociationShortName* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    obj_objectsToString(&ba, &object->objectList);
    bb_addString(&ba, "]\nIndex: 3 Value: ");
    //TODO: Show access rights.
#ifndef DLMS_IGNORE_SECURITY_SETUP
    bb_addString(&ba, "\nIndex: 4 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    bb_addLogicalName(&ba, obj_getLogicalName((gxObject*)object->securitySetup));
#else
    hlp_appendLogicalName(&ba, object->securitySetupReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
#endif //DLMS_IGNORE_SECURITY_SETUP
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME
#ifndef DLMS_IGNORE_PPP_SETUP
int obj_pppSetupToString(gxPppSetup* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: [");
    //TODO: ipcpOptions
    bb_addString(&ba, "]\nIndex: 3 Value: ");
#ifndef DLMS_IGNORE_OBJECT_POINTERS
    if (object->phy != NULL)
    {
        bb_addLogicalName(&ba, object->phy->logicalName);
    }
#else
    bb_addLogicalName(&ba, object->PHYReference);
#endif //DLMS_IGNORE_OBJECT_POINTERS
    bb_addString(&ba, "\nIndex: 4 Value: [");
    //TODO: lcpOptions
    bb_addString(&ba, "]\nIndex: 5 Value: ");
    bb_attachString(&ba, bb_toString(&object->userName));
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_attachString(&ba, bb_toString(&object->password));
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->authentication);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_PROFILE_GENERIC

int obj_ProfileGenericToString(gxProfileGeneric* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: [\n");
    obj_rowsToString(&ba, &object->buffer);
    bb_addString(&ba, "]\nIndex: 3 Value: [");
    obj_CaptureObjectsToString(&ba, &object->captureObjects);
    bb_addString(&ba, "]\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->capturePeriod);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->sortMethod);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    if (object->sortObject != NULL)
    {
        ret = hlp_appendLogicalName(&ba, object->sortObject->logicalName);
        if (ret != 0)
        {
            return ret;
        }
    }
    bb_addString(&ba, "\nIndex: 7 Value: ");
    bb_addIntAsString(&ba, object->entriesInUse);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->profileEntries);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_ACCOUNT
int obj_appendReferences(gxByteBuffer* ba, gxArray* list)
{
    uint16_t pos;
    int ret = 0;
    unsigned char* it;
    for (pos = 0; pos != list->size; ++pos)
    {
#ifdef DLMS_IGNORE_MALLOC
        if ((ret = arr_getByIndex(list, pos, (void**)&it, 6)) != 0 ||
#else
        if ((ret = arr_getByIndex(list, pos, (void**)&it)) != 0 ||
#endif //DLMS_IGNORE_MALLOC
            (ret = hlp_appendLogicalName(ba, it)) != 0)
        {
            break;
        }
    }
    return ret;
}
int obj_accountToString(gxAccount* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    if ((ret = bb_addString(&ba, "Index: 2 Value: [\n")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->paymentMode)) == 0 &&
        (ret = bb_addString(&ba, "\n")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->accountStatus)) == 0 &&
        (ret = bb_addString(&ba, "]")) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->currentCreditInUse)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->currentCreditStatus)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->availableCredit)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->amountToClear)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 7 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->clearanceThreshold)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 8 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->aggregatedDebt)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 9 Value: ")) == 0 &&
        (ret = obj_appendReferences(&ba, &object->creditReferences)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 10 Value: ")) == 0 &&
        (ret = obj_appendReferences(&ba, &object->chargeReferences)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 11 Value: ")) == 0 &&
        //bb_addIntAsString(&ba, &object->creditChargeConfigurations)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 12 Value: ")) == 0 &&
        //  bb_addIntAsString(&ba, object->tokenGatewayConfigurations)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 13 Value: ")) == 0 &&
        (ret = time_toString(&object->accountActivationTime, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 14 Value: ")) == 0 &&
        (ret = time_toString(&object->accountClosureTime, &ba)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 15 Value: ")) == 0 &&
        (ret = bb_set(&ba, object->currency.name.data, object->currency.name.size)) == 0 &&
        (ret = bb_addIntAsString(&ba, object->currency.scale)) == 0 &&
        (ret = bb_addIntAsString(&ba, object->currency.unit)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 16 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->lowCreditThreshold)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 17 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->nextCreditAvailableThreshold)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 18 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->maxProvision)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 19 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->maxProvisionPeriod)) == 0 &&
        (ret = bb_addString(&ba, "\n")) == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_ACCOUNT
#ifndef DLMS_IGNORE_CREDIT
int obj_creditToString(gxCredit* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->currentCreditAmount);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->type);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->priority);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->warningThreshold);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->limit);
    bb_addString(&ba, "\nIndex: 7 Value: ");
    //    bb_addIntAsString(&ba, object->creditConfiguration);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\nIndex: 9 Value: ");
    bb_addIntAsString(&ba, object->presetCreditAmount);
    bb_addString(&ba, "\nIndex: 10 Value: ");
    bb_addIntAsString(&ba, object->creditAvailableThreshold);
    bb_addString(&ba, "\nIndex: 11 Value: ");
    time_toString(&object->period, &ba);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_CREDIT
#ifndef DLMS_IGNORE_CHARGE
int obj_chargeToString(gxCharge* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addIntAsString(&ba, object->totalAmountPaid);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->chargeType);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->priority);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    //TODO:
//    bb_addIntAsString(&ba, object->unitChargeActive);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    //  bb_addIntAsString(&ba, object->unitChargePassive);
    bb_addString(&ba, "\nIndex: 7 Value: ");
    time_toString(&object->unitChargeActivationTime, &ba);
    bb_addString(&ba, "\nIndex: 8 Value: ");
    bb_addIntAsString(&ba, object->period);
    bb_addString(&ba, "\nIndex: 9 Value: ");
    //  bb_addIntAsString(&ba, object->chargeConfiguration);
    bb_addString(&ba, "\nIndex: 10 Value: ");
    time_toString(&object->lastCollectionTime, &ba);
    bb_addString(&ba, "\nIndex: 11 Value: ");
    bb_addIntAsString(&ba, object->lastCollectionAmount);
    bb_addString(&ba, "\nIndex: 12 Value: ");
    bb_addIntAsString(&ba, object->totalAmountRemaining);
    bb_addString(&ba, "\nIndex: 13 Value: ");
    bb_addIntAsString(&ba, object->proportion);
    bb_addString(&ba, "\n");
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
    char* tmp;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    tmp = bb_toHexString(&object->token);
    bb_addString(&ba, tmp);
    gxfree(tmp);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    time_toString(&object->time, &ba);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->deliveryMethod);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\n");
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_TOKEN_GATEWAY
#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
int obj_GsmDiagnosticToString(gxGsmDiagnostic* object, char** buff)
{
    int ret;
    uint16_t pos;
    gxAdjacentCell* it;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    bb_set(&ba, object->operatorName.data, object->operatorName.size);
#else
    bb_addString(&ba, object->operatorName);
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->status);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->circuitSwitchStatus);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    bb_addIntAsString(&ba, object->packetSwitchStatus);
    bb_addString(&ba, "\nIndex: 6 Value: [");
    bb_addString(&ba, "Cell ID: ");
    bb_addIntAsString(&ba, object->cellInfo.cellId);
    bb_addString(&ba, "Location ID: ");
    bb_addIntAsString(&ba, object->cellInfo.locationId);
    bb_addString(&ba, "Signal Quality: ");
    bb_addIntAsString(&ba, object->cellInfo.signalQuality);
    bb_addString(&ba, "BER: ");
    bb_addIntAsString(&ba, object->cellInfo.ber);
    bb_addString(&ba, "]");

    bb_addString(&ba, "\nIndex: 7 Value:[\n");
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
    bb_addString(&ba, "]\n");
    bb_addString(&ba, "\nIndex: 8 Value: ");
    time_toString(&object->captureTime, &ba);
    bb_addString(&ba, "\n");
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
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    tmp = bb_toHexString(&object->buffer);
    bb_addString(&ba, tmp);
    gxfree(tmp);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    obj_CaptureObjectsToString(&ba, &object->captureObjects);
    bb_addString(&ba, "\nIndex: 4 Value: ");
    bb_addIntAsString(&ba, object->templateId);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    tmp = bb_toHexString(&object->templateDescription);
    bb_addString(&ba, tmp);
    gxfree(tmp);
    bb_addString(&ba, "\nIndex: 6 Value: ");
    bb_addIntAsString(&ba, object->captureMethod);
    *buff = bb_toString(&ba);
    bb_clear(&ba);
    return 0;
}
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
int obj_LlcSscsSetupToString(gxLlcSscsSetup* object, char** buff)
{
    int ret;
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    if ((ret = bb_addString(&ba, "Index: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->serviceNodeAddress)) == 0 &&
        (ret = bb_addString(&ba, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&ba, object->baseNodeAddress)) == 0)
    {
        *buff = bb_toString(&ba);
    }
    bb_clear(&ba);
    return ret;
}
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
int obj_PrimeNbOfdmPlcPhysicalLayerCountersToString(gxPrimeNbOfdmPlcPhysicalLayerCounters* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
int  obj_PrimeNbOfdmPlcMacSetupToString(gxPrimeNbOfdmPlcMacSetup* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
int  obj_PrimeNbOfdmPlcMacFunctionalParametersToString(gxPrimeNbOfdmPlcMacFunctionalParameters* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
int  obj_PrimeNbOfdmPlcMacCountersToString(gxPrimeNbOfdmPlcMacCounters* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
int  obj_PrimeNbOfdmPlcMacNetworkAdministrationDataToString(gxPrimeNbOfdmPlcMacNetworkAdministrationData* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
int  obj_PrimeNbOfdmPlcApplicationsIdentificationToString(gxPrimeNbOfdmPlcApplicationsIdentification* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION

#ifndef DLMS_IGNORE_SCHEDULE
int obj_scheduleToString(gxSchedule* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_SCHEDULE

#ifndef DLMS_IGNORE_PARAMETER_MONITOR

int obj_ParameterMonitorToString(gxParameterMonitor* object, char** buff)
{
    return 0;
}
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_ARBITRATOR
int obj_ArbitratorToString(gxArbitrator* object, char** buff)
{
    uint16_t pos;
    int ret = 0;
    gxByteBuffer bb;
    gxActionItem* it;
    bitArray* ba;
    BYTE_BUFFER_INIT(&bb);
    bb_addString(&bb, "Index: 2 Value:{");
    for (pos = 0; pos != object->actions.size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(&bb, ", ");
        }
        if ((ret = arr_getByIndex(&object->actions, pos, (void**)&it)) != 0)
        {
            break;
        }
        bb_addString(&bb, "[");
        hlp_appendLogicalName(&bb, it->script->base.logicalName);
        bb_addString(&bb, ", ");
        bb_addIntAsString(&bb, it->scriptSelector);
        bb_addString(&bb, "]");
    }
    bb_addString(&bb, "}");
    bb_addString(&bb, "\nIndex: 3 Value:{");
    for (pos = 0; pos != object->permissionsTable.size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(&bb, ", ");
        }
        if ((ret = arr_getByIndex(&object->permissionsTable, pos, (void**)&ba)) != 0 ||
            (ret = ba_toString2(&bb, ba)) != 0)
        {
            break;
        }
    }
    ret = bb_addString(&bb, "}");
    *buff = bb_toString(&bb);
    bb_clear(&bb);
    return 0;
}
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
int obj_Iec8802LlcType1SetupToString(gxIec8802LlcType1Setup* object, char** buff)
{
    int ret;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "Index: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->maximumOctetsUiPdu)) == 0)
    {
        *buff = bb_toString(&bb);
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
int obj_Iec8802LlcType2SetupToString(gxIec8802LlcType2Setup* object, char** buff)
{
    int ret;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "Index: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->transmitWindowSizeK)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->transmitWindowSizeRW)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->maximumOctetsPdu)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->maximumNumberTransmissions)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->acknowledgementTimer)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 7 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->bitTimer)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 8 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->rejectTimer)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 9 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->busyStateTimer)) == 0)
    {
        *buff = bb_toString(&bb);
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
int obj_Iec8802LlcType3SetupToString(gxIec8802LlcType3Setup* object, char** buff)
{
    int ret;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "\nIndex: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->maximumOctetsACnPdu)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->maximumTransmissions)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->acknowledgementTime)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->receiveLifetime)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->transmitLifetime)) == 0)
    {
        *buff = bb_toString(&bb);
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
int obj_SFSKActiveInitiatorToString(gxSFSKActiveInitiator* object, char** buff)
{
    int ret;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "\nIndex: 2 Value: ")) == 0 &&
        (ret = bb_attachString(&bb, bb_toHexString(&object->systemTitle))) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->macAddress)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->lSapSelector)) == 0)
    {
        *buff = bb_toString(&bb);
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS


int obj_getArrayAsString(gxByteBuffer* bb, gxArray* arr)
{
    uint16_t pos;
    int ret = 0;
    gxUint16PairUint32* it;
    for (pos = 0; pos != arr->size; ++pos)
    {
        if (pos != 0)
        {
            bb_addString(bb, ", ");
        }
        if ((ret = arr_getByIndex(arr, pos, (void**)&it)) != 0 ||
            (ret = bb_addIntAsString(bb, it->first)) != 0 ||
            (ret = bb_addString(bb, ": ")) != 0 ||
            (ret = bb_addIntAsString(bb, it->second)) != 0)
        {
            break;
        }
    }
    return ret;
}

int obj_FSKMacCountersToString(gxFSKMacCounters* object, char** buff)
{
    int ret;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "\nIndex: 2 Value:{")) == 0 &&
        (ret = obj_getArrayAsString(&bb, &object->synchronizationRegister)) == 0 &&
        (ret = bb_addString(&bb, "}\nIndex: 3 Value:[")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->physicalLayerDesynchronization)) == 0 &&
        (ret = bb_addString(&bb, ", ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->timeOutNotAddressedDesynchronization)) == 0 &&
        (ret = bb_addString(&bb, ", ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->timeOutFrameNotOkDesynchronization)) == 0 &&
        (ret = bb_addString(&bb, ", ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->writeRequestDesynchronization)) == 0 &&
        (ret = bb_addString(&bb, ", ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->wrongInitiatorDesynchronization)) == 0 &&
        (ret = bb_addString(&bb, "]\nIndex: 3 Value:{")) == 0 &&
        (ret = obj_getArrayAsString(&bb, &object->broadcastFramesCounter)) == 0 &&
        (ret = bb_addString(&bb, "}\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->repetitionsCounter)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->transmissionsCounter)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->crcOkFramesCounter)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 7 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->crcNOkFramesCounter)) == 0)
    {
        *buff = bb_toString(&bb);
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
int obj_SFSKMacSynchronizationTimeoutsToString(gxSFSKMacSynchronizationTimeouts* object, char** buff)
{
    int ret;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "\nIndex: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->searchInitiatorTimeout)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->synchronizationConfirmationTimeout)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->timeOutNotAddressed)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->timeOutFrameNotOK)) == 0)
    {
        *buff = bb_toString(&bb);
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP

int obj_SFSKPhyMacSetUpToString(gxSFSKPhyMacSetUp* object, char** buff)
{
    int ret = 0;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "\nIndex: 2 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->initiatorElectricalPhase)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 3 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->deltaElectricalPhase)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 4 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->maxReceivingGain)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 5 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->maxTransmittingGain)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 6 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->searchInitiatorThreshold)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 7 Value:[")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->markFrequency)) == 0 &&
        (ret = bb_addString(&bb, ", ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->spaceFrequency)) == 0 &&
        (ret = bb_addString(&bb, "]\nIndex: 8 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->macAddress)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 9 Value:{")) == 0 &&
        (ret = obj_UInt16ArrayToString(&bb, &object->macGroupAddresses)) == 0 &&
        (ret = bb_addString(&bb, "}\nIndex: 10 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->repeater)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 11 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->repeaterStatus)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 12 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->minDeltaCredit)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 13 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->initiatorMacAddress)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 14 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->synchronizationLocked)) == 0 &&
        (ret = bb_addString(&bb, "\nIndex: 15 Value: ")) == 0 &&
        (ret = bb_addIntAsString(&bb, object->transmissionSpeed)) == 0)
    {
        *buff = bb_toString(&bb);
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
int obj_SFSKReportingSystemListToString(gxSFSKReportingSystemList* object, char** buff)
{
    uint16_t pos;
    int ret = 0;
    gxByteBuffer* it;
    gxByteBuffer bb;
    BYTE_BUFFER_INIT(&bb);
    if ((ret = bb_addString(&bb, "\nIndex: 2 Value:{")) == 0)
    {
        for (pos = 0; pos != object->reportingSystemList.size; ++pos)
        {
            if (pos != 0)
            {
                bb_addString(&bb, ", ");
            }
            if ((ret = arr_getByIndex(&object->reportingSystemList, pos, (void**)&it)) != 0 ||
                (ret = bb_attachString(&bb, bb_toHexString(it))) != 0)
            {
                break;
            }
        }
        if (ret == 0)
        {
            if ((ret = bb_addString(&bb, "\n}")) == 0)
            {
                *buff = bb_toString(&bb);
            }
        }
    }
    bb_clear(&bb);
    return ret;
}
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifdef DLMS_ITALIAN_STANDARD
int obj_TariffPlanToString(gxTariffPlan* object, char** buff)
{
    gxByteBuffer ba;
    BYTE_BUFFER_INIT(&ba);
    bb_addString(&ba, "Index: 2 Value: ");
    bb_addString(&ba, object->calendarName);
    bb_addString(&ba, "\nIndex: 3 Value: ");
    bb_addIntAsString(&ba, object->enabled);
    bb_addString(&ba, "\nIndex: 5 Value: ");
    time_toString(&object->activationTime, &ba);
    bb_addString(&ba, "\n");
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
        ret = obj_IecTwistedPairSetupToString((gxIecTwistedPairSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
#ifndef DLMS_IGNORE_IP4_SETUP
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        ret = obj_ip4SetupToString((gxIp4Setup*)object, buff);
        break;
#endif //DLMS_IGNORE_IP4_SETUP
#ifndef DLMS_IGNORE_IP6_SETUP
    case DLMS_OBJECT_TYPE_IP6_SETUP:
        ret = obj_ip6SetupToString((gxIp6Setup*)object, buff);
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
        ret = obj_scheduleToString((gxSchedule*)object, buff);
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
        ret = obj_UtilityTablesToString((gxUtilityTables*)object, buff);
        break;
#endif //DLMS_IGNORE_UTILITY_TABLES
#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    case DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP:
        ret = obj_mBusMasterPortSetupToString((gxMBusMasterPortSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
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
#ifndef DLMS_IGNORE_LLC_SSCS_SETUP
    case DLMS_OBJECT_TYPE_LLC_SSCS_SETUP:
        ret = obj_LlcSscsSetupToString((gxLlcSscsSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS:
        ret = obj_PrimeNbOfdmPlcPhysicalLayerCountersToString((gxPrimeNbOfdmPlcPhysicalLayerCounters*)object, buff);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP:
        ret = obj_PrimeNbOfdmPlcMacSetupToString((gxPrimeNbOfdmPlcMacSetup*)object, buff);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS:
        ret = obj_PrimeNbOfdmPlcMacFunctionalParametersToString((gxPrimeNbOfdmPlcMacFunctionalParameters*)object, buff);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS:
        ret = obj_PrimeNbOfdmPlcMacCountersToString((gxPrimeNbOfdmPlcMacCounters*)object, buff);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA:
        ret = obj_PrimeNbOfdmPlcMacNetworkAdministrationDataToString((gxPrimeNbOfdmPlcMacNetworkAdministrationData*)object, buff);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    case DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION:
        ret = obj_PrimeNbOfdmPlcApplicationsIdentificationToString((gxPrimeNbOfdmPlcApplicationsIdentification*)object, buff);
        break;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    case DLMS_OBJECT_TYPE_PARAMETER_MONITOR:
        ret = obj_ParameterMonitorToString((gxParameterMonitor*)object, buff);
        break;
#endif //DLMS_IGNORE_PARAMETER_MONITOR
#ifndef DLMS_IGNORE_ARBITRATOR
    case DLMS_OBJECT_TYPE_ARBITRATOR:
        ret = obj_ArbitratorToString((gxArbitrator*)object, buff);
        break;
#endif //DLMS_IGNORE_ARBITRATOR
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP:
        ret = obj_Iec8802LlcType1SetupToString((gxIec8802LlcType1Setup*)object, buff);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP:
        ret = obj_Iec8802LlcType2SetupToString((gxIec8802LlcType2Setup*)object, buff);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    case DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP:
        ret = obj_Iec8802LlcType3SetupToString((gxIec8802LlcType3Setup*)object, buff);
        break;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    case DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR:
        ret = obj_SFSKActiveInitiatorToString((gxSFSKActiveInitiator*)object, buff);
        break;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    case DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS:
        ret = obj_FSKMacCountersToString((gxFSKMacCounters*)object, buff);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS
#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    case DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS:
        ret = obj_SFSKMacSynchronizationTimeoutsToString((gxSFSKMacSynchronizationTimeouts*)object, buff);
        break;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    case DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP:
        ret = obj_SFSKPhyMacSetUpToString((gxSFSKPhyMacSetUp*)object, buff);
        break;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP
#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
    case DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST:
        ret = obj_SFSKReportingSystemListToString((gxSFSKReportingSystemList*)object, buff);
        break;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST
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

#endif //DLMS_IGNORE_STRING_CONVERTER

const char* err_toString(int err)
{
    return hlp_getErrorMessage(err);
}
#endif //DLMS_IGNORE_MALLOC
