/*
 * gx-conf.h
 *
 *  Created on: 26/01/2021
 *      Author: alexrayne <alexraynepe196@gmail.com>
 * --------------------------------------------------------------------------------------
 * dummy Gurux library config file.
 * this should be redeclared at application path
 */

#ifndef GURUX_DLMS_C_DEVELOPMENT_INCLUDE_GX_CONF_H_
#define GURUX_DLMS_C_DEVELOPMENT_INCLUDE_GX_CONF_H_


/* Uncomment defines to ignore non-needed parts to make image size smaller. */

// #define DLMS_IGNORE_HDLC
// #define DLMS_IGNORE_WRAPPER
// #define DLMS_IGNORE_PLC
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
// #define DLMS_ITALIAN_STANDARD
// #define DLMS_IGNORE_LLC_SSCS_SETUP
// #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS
// #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP
// #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
// #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS
// #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
// #define DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
// #define DLMS_IGNORE_ARBITRATOR

// #define DLMS_IGNORE_MALLOC
// #define DLMS_USE_CUSTOM_MALLOC

// #define DLMS_IGNORE_OBJECT_POINTERS

// #define DLMS_IGNORE_FLOAT32
// #define DLMS_IGNORE_FLOAT64

//Use EPOCH time. This can be used to improve memory usage.
// #define DLMS_USE_EPOCH_TIME

//Use UTC time zone. Read more: https://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSClock
// #define DLMS_USE_UTC_TIME_ZONE

// #define DLMS_IGNORE_SET
// #define DLMS_IGNORE_ACTION

// String converters are not used.
// #define DLMS_IGNORE_STRING_CONVERTER

// COSEM objects are using DLMS standard data types.
// Using exact data types will save memory, but reading failes if meter returns wrong data type,
// ex. Int8 when data type should be UInt8.
// #define DLMS_COSEM_EXACT_DATA_TYPES

//Framework send debug information that can be used in debugging.
// #define DLMS_DEBUG

//Defined if AVR is used.
// #define USE_AVR

//#define ARDUINO_ARCH_AVR

//Defined if program memory (flash) is used instead of SRAM.
// #define USE_PROGMEM



#endif /* GURUX_DLMS_C_DEVELOPMENT_INCLUDE_GX_CONF_H_ */
