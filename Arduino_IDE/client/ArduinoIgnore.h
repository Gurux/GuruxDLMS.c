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
#ifndef ARDUINO_IGNORE_H
#define ARDUINO_IGNORE_H

#ifdef  __cplusplus
extern "C" {
#endif

//Use UTC time zone. Read more: https://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSClock
//#define DLMS_USE_UTC_TIME_ZONE

/* Uncomment defines to ignore non-needed parts to make image size smaller. */

// #define DLMS_IGNORE_HDLC
// #define DLMS_IGNORE_WRAPPER
#define DLMS_IGNORE_PLC
#define DLMS_IGNORE_SERVER
#define GX_DLMS_MICROCONTROLLER
#define DLMS_IGNORE_HIGH_SHA256
#define DLMS_IGNORE_HIGH_SHA1
#define DLMS_IGNORE_HIGH_MD5
// #define DLMS_IGNORE_AES
// #define DLMS_IGNORE_HIGH_GMAC
// #define DLMS_IGNORE_DATA
// #define DLMS_IGNORE_REGISTER
// #define DLMS_IGNORE_EXTENDED_REGISTER
// #define DLMS_IGNORE_DEMAND_REGISTER
// #define DLMS_IGNORE_REGISTER_ACTIVATION
// #define DLMS_IGNORE_PROFILE_GENERIC
// #define DLMS_IGNORE_CLOCK
// #define DLMS_IGNORE_SCRIPT_TABLE
// #define DLMS_IGNORE_SCHEDULE
// #define DLMS_IGNORE_SPECIAL_DAYS_TABLE
// #define DLMS_IGNORE_ASSOCIATION_SHORT_NAME
// #define DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
// #define DLMS_IGNORE_SAP_ASSIGNMENT
// #define DLMS_IGNORE_IMAGE_TRANSFER
// #define DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
// #define DLMS_IGNORE_ACTIVITY_CALENDAR
// #define DLMS_IGNORE_REGISTER_MONITOR
// #define DLMS_IGNORE_ACTION_SCHEDULE
// #define DLMS_IGNORE_IEC_HDLC_SETUP
// #define DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
// #define DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
// #define DLMS_IGNORE_UTILITY_TABLES
// #define DLMS_IGNORE_MODEM_CONFIGURATION
// #define DLMS_IGNORE_AUTO_ANSWER
// #define DLMS_IGNORE_AUTO_CONNECT
// #define DLMS_IGNORE_TCP_UDP_SETUP
// #define DLMS_IGNORE_IP4_SETUP
// #define DLMS_IGNORE_IP6_SETUP
// #define DLMS_IGNORE_MAC_ADDRESS_SETUP
// #define DLMS_IGNORE_PPP_SETUP
// #define DLMS_IGNORE_GPRS_SETUP
// #define DLMS_IGNORE_SMTP_SETUP
// #define DLMS_IGNORE_GSM_DIAGNOSTIC
// #define DLMS_IGNORE_REGISTER_TABLE
// #define DLMS_IGNORE_STATUS_MAPPING
// #define DLMS_IGNORE_SECURITY_SETUP
// #define DLMS_IGNORE_DISCONNECT_CONTROL
// #define DLMS_IGNORE_LIMITER
#define DLMS_IGNORE_MBUS_CLIENT
// #define DLMS_IGNORE_PUSH_SETUP
#define DLMS_IGNORE_PARAMETER_MONITOR
#define DLMS_IGNORE_WIRELESS_MODE_Q_CHANNEL
#define DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
#define DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
#define DLMS_IGNORE_ZIG_BEE_SAS_JOIN
#define DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
#define DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
#define DLMS_IGNORE_DATA_PROTECTION
// #define DLMS_IGNORE_ACCOUNT
// #define DLMS_IGNORE_CREDIT
// #define DLMS_IGNORE_CHARGE
#define DLMS_IGNORE_TOKEN_GATEWAY
#define DLMS_IGNORE_COMPACT_DATA
#define DLMS_IGNORE_LLC_SSCS_SETUP
#define DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
#define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
#define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
#define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
#define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
#define DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
#define DLMS_IGNORE_ARBITRATOR
#define DLMS_IGNORE_SERIALIZER

// #define DLMS_IGNORE_FLOAT32
// #define DLMS_IGNORE_FLOAT64

// #define DLMS_ITALIAN_STANDARD

//////////////////////////////////////////////////////////////////////
//Arduino client spesific settings.

//Use EPOCH time. This can be used to improve memory usage.
#define DLMS_USE_EPOCH_TIME
#define DLMS_IGNORE_NOTIFY
#define DLMS_IGNORE_SERVER
#define DLMS_IGNORE_NOTIFY
#ifdef  __cplusplus
}
#endif

#endif //ARDUINO_IGNORE_H
