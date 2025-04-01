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
#ifndef GXIGNORE_H
#define GXIGNORE_H

#ifdef  __cplusplus
extern "C" {
#endif

    /* Uncomment defines to ignore non-needed parts to make image size smaller. */

    // #define DLMS_IGNORE_HDLC
    // #define DLMS_IGNORE_WRAPPER
    // #define DLMS_IGNORE_PLC
    // #define DLMS_IGNORE_IEC
    // #define DLMS_IGNORE_NOTIFY
    // #define DLMS_IGNORE_SERVER
    // #define DLMS_IGNORE_CLIENT
    // #define GX_DLMS_MICROCONTROLLER
    // #define DLMS_IGNORE_HIGH_SHA256
    // #define DLMS_IGNORE_HIGH_SHA1
    // #define DLMS_IGNORE_HIGH_MD5
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
    // #define DLMS_IGNORE_MBUS_CLIENT
    // #define DLMS_IGNORE_PUSH_SETUP
    // #define DLMS_IGNORE_PARAMETER_MONITOR
    // #define DLMS_IGNORE_WIRELESS_MODE_Q_CHANNEL
    // #define DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    // #define DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    // #define DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    // #define DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    // #define DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    // #define DLMS_IGNORE_DATA_PROTECTION
    // #define DLMS_IGNORE_ACCOUNT
    // #define DLMS_IGNORE_CREDIT
    // #define DLMS_IGNORE_CHARGE
    // #define DLMS_IGNORE_TOKEN_GATEWAY
    // #define DLMS_IGNORE_COMPACT_DATA
    // #define DLMS_IGNORE_LLC_SSCS_SETUP
    // #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
    // #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
    // #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    // #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
    // #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    // #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    // #define DLMS_IGNORE_ARBITRATOR
    // #define DLMS_IGNORE_G3_PLC_MAC_LAYER_COUNTERS
    // #define DLMS_IGNORE_G3_PLC_MAC_SETUP
    // #define DLMS_IGNORE_G3_PLC_6LO_WPAN
    // #define DLMS_IGNORE_FUNCTION_CONTROL
    // #define DLMS_IGNORE_ARRAY_MANAGER
    // #define DLMS_IGNORE_NTP_SETUP

    // #define DLMS_IGNORE_MALLOC
    // #define DLMS_USE_CUSTOM_MALLOC

    // #define DLMS_IGNORE_OBJECT_POINTERS

    // #define DLMS_IGNORE_FLOAT32
    // #define DLMS_IGNORE_FLOAT64

    //Delta values are ignored.
    // #define DLMS_IGNORE_DELTA

    //Use EPOCH time. This can be used to improve memory usage.
    // #define DLMS_USE_EPOCH_TIME

    //Use UTC time zone. Read more: https://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSClock
    // #define DLMS_USE_UTC_TIME_ZONE

    // #define DLMS_IGNORE_SET
    // #define DLMS_IGNORE_ACTION

    // String converters are not used.
    // #define DLMS_IGNORE_STRING_CONVERTER

    //Framework send debug information that can be used in debugging.
    // #define DLMS_DEBUG

    //Defined if AVR is used.
    // #define USE_AVR

    //Defined if program memory (flash) is used instead of SRAM.
    // #define USE_PROGMEM

    // COSEM objects are using DLMS standard data types.
    // Using exact data types will save memory, but reading failes if meter returns wrong data type,
    // ex. Int8 when data type should be UInt8.
    // #define DLMS_COSEM_EXACT_DATA_TYPES

    // If innovation counter size is UInt64 and not default UInt32.
    // #define DLMS_COSEM_INVOCATION_COUNTER_SIZE64

    // Use 32 bit max size bytebuffer instead of 16 bit.
    // This might be used in client side if a lot of data is read from the meter.
    // #define GX_DLMS_BYTE_BUFFER_SIZE_32

    //Serializer is not used.
    //#define DLMS_IGNORE_SERIALIZER
    //Serializer is used instead of file system.
    //#define GX_DLMS_SERIALIZER

    //GENERAL_CIPHERING is not used.
    //#define DLMS_IGNORE_GENERAL_CIPHERING

    /**
    *  Ignore HDLC Control field check.
    Some meters are sending invalid control field.
    Note! It's not recommended to use this.
    */
    //#define DLMS_IGNORE_HDLC_CHECK

    // #define DLMS_ITALIAN_STANDARD
    // #define DLMS_INDONESIA_STANDARD

    // Show decoded PDU. 
    // This can be used to show decoded PDU.
    // #define DLMS_TRACE_PDU

    // Some DLMS standard define that invocation counter must between given range.
    // The server can validate IC using svr_validInvocationCounter method.
    //#define DLMS_INVOCATION_COUNTER_VALIDATOR

    // Server notifies invalid authentication error.
    //#define DLMS_NOTIFY_AUTHENTICATION_ERROR

    // Server notifies invalid ciphering error.
    // #define DLMS_NOTIFY_CIPHERING_ERROR

    //Security Suite 1 is used.
    // #define DLMS_SECURITY_SUITE_1

    //Security Suite 2 is used.
    //#define DLMS_SECURITY_SUITE_2

    // Meter can return a list of supported access selectors in the association view.
    // Empty list is returned if this is not defined.
    // #define DLMS_USE_ACCESS_SELECTOR

    // Defined when the external AES Hardware Security Module using is used.
    // The application must implement this method.
    // extern int gx_hsmAes(const unsigned char* input, const unsigned char* key, unsigned char* output, const size_t length);
    // Add DLMS_IGNORE_AES is AES table is not needed.
    // #define DLMS_USE_AES_HARDWARE_SECURITY_MODULE

    // Defined when the external CRC Hardware Security Module is used.
    // The application must implement this method.
    // extern uint16_t gx_hsmCrc(const unsigned char* data, const uint16_t length);
    // #define DLMS_USE_CRC_HARDWARE_SECURITY_MODULE

#ifdef ARDUINO
/////////////////////////////////////////////////////////////////////////////
//If Arduino is used.
#include "../ArduinoIgnore.h"
/////////////////////////////////////////////////////////////////////////////
#endif //ARDUINO

#ifdef ARDUINO_ARCH_AVR
/////////////////////////////////////////////////////////////////////////////
//If Arduino is used.
#include "../ArduinoIgnore.h"
#define DLMS_IGNORE_IP6_SETUP
#define DLMS_USE_EPOCH_TIME
#define DLMS_IGNORE_NOTIFY
#define GX_DLMS_MICROCONTROLLER
#define DLMS_IGNORE_HIGH_SHA256
#define DLMS_IGNORE_HIGH_SHA1
#define DLMS_IGNORE_HIGH_MD5
#define USE_PROGMEM
/////////////////////////////////////////////////////////////////////////////
#endif //ARDUINO_ARCH_AVR

#ifdef ARDUINO_ARCH_ESP32
/////////////////////////////////////////////////////////////////////////////
//If Arduino ESP32 is used.
#include "../ArduinoIgnore.h"
#define DLMS_IGNORE_IP6_SETUP
#define DLMS_USE_EPOCH_TIME
#define DLMS_IGNORE_NOTIFY
#define GX_DLMS_MICROCONTROLLER
#define DLMS_IGNORE_HIGH_SHA256
#define DLMS_IGNORE_HIGH_SHA1
#define DLMS_IGNORE_HIGH_MD5
/////////////////////////////////////////////////////////////////////////////
#endif //ARDUINO_ARCH_ESP32

#ifdef ARDUINO_ARCH_ESP8266
/////////////////////////////////////////////////////////////////////////////
//If Arduino ESP is used.
#include "../ArduinoIgnore.h"
#define ESP_PLATFORM
#define DLMS_IGNORE_IP6_SETUP
#define DLMS_USE_EPOCH_TIME
#define DLMS_IGNORE_NOTIFY
#define GX_DLMS_MICROCONTROLLER
#define DLMS_IGNORE_HIGH_SHA256
#define DLMS_IGNORE_HIGH_SHA1
#define DLMS_IGNORE_HIGH_MD5
/////////////////////////////////////////////////////////////////////////////
#endif //ARDUINO_ARCH_ESP8266

#ifdef  __cplusplus
}
#endif

#endif //GXIGNORE_H
