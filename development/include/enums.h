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

#ifndef ENUMS_H
#define ENUMS_H

#include "gxignore.h"

#ifdef  __cplusplus
extern "C" {
#endif

static const unsigned char DEFAULT_BROADCAST_BLOCK_CIPHER_KEY[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
static const unsigned char DEFAULT_BLOCK_CIPHER_KEY[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static const unsigned char DEFAULT_SYSTEM_TITLE[] = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };
static const unsigned char DEFAULT_AUTHENTICATION_KEY[] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
                                                            0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF
};

    // Server sender frame sequence starting number.
    // This buffer is also used for save challenge.
#define PDU_MAX_HEADER_SIZE 70

#define CIPHERING_HEADER_SIZE 22 //7 + 12 + 3

    typedef enum
    {
        DLMS_ASSOCIATION_RESULT_ACCEPTED = 0,
        DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED = 1,
        DLMS_ASSOCIATION_RESULT_TRANSIENT_REJECTED = 2
    } DLMS_ASSOCIATION_RESULT;

    typedef enum
    {
        //Connection succeeded.
        DLMS_SOURCE_DIAGNOSTIC_NONE = 0,
        //Connection failed. No reason is given.
        DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN = 1,
        //Connection failed. Application context name is not supported.
        DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED = 2,
        //Client system title is missing.
        DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_TITLE_NOT_RECOGNIZED = 3,
        //Connection failed. No reason is given.
        DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED = 4,
        //Connection failed. Calling AE qualifier not recognized.
        DLMS_SOURCE_DIAGNOSTIC_CALLING_AE_QUALIFIER_NOT_RECOGNIZED = 5,
        //Connection failed. Calling AE invocation identifier not recognized.
        DLMS_SOURCE_DIAGNOSTIC_CALLING_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED = 6,
        //Connection failed. Called AP title not recognized.
        DLMS_SOURCE_DIAGNOSTIC_CALLED_AP_TITLE_NOT_RECOGNIZED = 7,
        //Connection failed. Called AP invocation identifier not recognized
        DLMS_SOURCE_DIAGNOSTIC_CALLED_AP_INVOCATION_IDENTIFIER_NOT_RECOGNIZED = 8,
        //Connection failed. Called AE qualifier not recognized.
        DLMS_SOURCE_DIAGNOSTIC_CALLED_AE_QUALIFIER_NOT_RECOGNIZED = 9,
        //Connection failed. Called AE invocation identifier not recognized.
        DLMS_SOURCE_DIAGNOSTIC_CALLED_AE_INVOCATION_IDENTIFIER_NOT_RECOGNIZED = 10,
        //Connection failed.  Authentication mechanism name not recognised.
        DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED = 11,
        //Connection failed. Authentication mechanism name required.
        DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED = 12,
        //Connection failed. Authentication failure.
        DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE = 13,
        //Connection failed. Authentication required.
        DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED = 14
    } DLMS_SOURCE_DIAGNOSTIC;

    typedef enum
    {
        DLMS_ACSE_SERVICE_PROVIDER_NULL = 0,
        DLMS_ACSE_SERVICE_PROVIDER_NO_REASON_GIVEN = 1,
        DLMS_ACSE_SERVICE_PROVIDER_NO_COMMON_ACSE_VERSION = 2
    }DLMS_ACSE_SERVICE_PROVIDER;

    // Defines whether or not the device has been assigned an address
    // since last power up of the device.
    typedef enum
    {
        // Not assigned an address yet.
        DLMS_ADDRESS_STATE_NONE,
        // Assigned an address either by manual setting, or by automated method.
        DLMS_ADDRESS_STATE_ASSIGNED
    } DLMS_ADDRESS_STATE;

    // Security policy Enforces authentication and/or encryption algorithm provided with security_suite.
    typedef enum
    {
        /*
        * Security is not used.
        */
        DLMS_SECURITY_POLICY_NOTHING = 0,
        /*
         All messages to be authenticated. This enumeration values are used in Security Setup version 0.
        */
        DLMS_SECURITY_POLICY_AUTHENTICATED = 1,
        /*
         All messages to be encrypted. This enumeration values are used in Security Setup version 0.
        */
        DLMS_SECURITY_POLICY_ENCRYPTED = 2,
        /*
         All messages to be authenticated and encrypted. This enumeration values are used in Security Setup version 0.
        */
        DLMS_SECURITY_POLICY_AUTHENTICATED_ENCRYPTED = 3,
        /*
        * Request is authenticated. This enumeration values are used in Security Setup version 1.
        */
        DLMS_SECURITY_POLICY_AUTHENTICATED_REQUEST = 0x4,
        /*
        * Request is encrypted. This enumeration values are used in Security Setup version 1.
        */
        DLMS_SECURITY_POLICY_ENCRYPTED_REQUEST = 0x8,
        /*
        * Request is digitally signed. This enumeration values are used in Security Setup version 1.
        */
        DLMS_SECURITY_POLICY_DIGITALLY_SIGNED_REQUEST = 0x10,
        /*
        * Response is authenticated. This enumeration values are used in Security Setup version 1.
        */
        DLMS_SECURITY_POLICY_AUTHENTICATED_RESPONSE = 0x20,
        /*
        * Response is encrypted. This enumeration values are used in Security Setup version 1.
        */
        DLMS_SECURITY_POLICY_ENCRYPTED_RESPONSE = 0x40,
        /*
        * Response is digitally signed. This enumeration values are used in Security Setup version 1.
        */
        DLMS_SECURITY_POLICY_DIGITALLY_SIGNED_RESPONSE = 0x80
    } DLMS_SECURITY_POLICY;

    //Security suite Specifies authentication, encryption and key wrapping algorithm.
    typedef enum
    {
        /*
         AES-GCM-128 for authenticated encryption and AES-128 for key wrapping.
        */
        DLMS_SECURITY_SUITE_V0 = 0,
        /*
         AES-GCM-128 authenticated encryption, ECDSA P-256 digital signature, ECDH P-256 key agreement, SHA-256 hash, V.44 compression and AES-128 key wrap.
        */
        DLMS_SECURITY_SUITE_V1 = 1,
        /*
            AES-GCM-256 authenticated encryption, ECDSA P-384 digital signature, ECDH P-384 key agreement, SHA-384 hash, V.44 compression and AES-256 key wrap
        */
        DLMS_SECURITY_SUITE_V2 = 2
    } DLMS_SECURITY_SUITE;

    typedef enum
    {
        DLMS_SERVICE_TYPE_TCP = 0,
        DLMS_SERVICE_TYPE_UDP = 1,
        DLMS_SERVICE_TYPE_FTP = 2,
        DLMS_SERVICE_TYPE_SMTP = 3,
        DLMS_SERVICE_TYPE_SMS = 4,
        DLMS_SERVICE_TYPE_HDLC = 5,
        DLMS_SERVICE_TYPE_M_BUS = 6,
        DLMS_SERVICE_TYPE_ZIG_BEE = 7,
        /*DLMS Gateway.*/
        DLMS_SERVICE_TYPE_DLMS_GATEWAY = 8,
        /*Reliable CoAP.*/
        DLMS_SERVICE_TYPE_RELIABLE_COAP = 9,
        /*Unreliable CoAP.*/
        DLMS_SERVICE_TYPE_UNRELIABLE_COAP = 10
    } DLMS_SERVICE_TYPE;

    typedef enum
    {
        DLMS_MESSAGE_TYPE_COSEM_APDU = 0,
        DLMS_MESSAGE_TYPE_COSEM_APDU_XML = 1,
        DLMS_MESSAGE_TYPE_MANUFACTURER_SPESIFIC = 128
    } DLMS_MESSAGE_TYPE;

    /*Push operation method defines what service class is used with push messages.*/
    typedef enum
    {
        /**
        * Unconfirmed, retry on supporting protocol layer failure.
        */
        DLMS_PUSH_OPERATION_METHOD_UNCONFIRMED_FAILURE = 0,
        /**
         * Unconfirmed, retry on missing supporting protocol layer confirmation.
         */
        DLMS_PUSH_OPERATION_METHOD_UNCONFIRMED_MISSING,
        /**
         * Confirmed, retry on missing confirmation.
         */
        DLMS_PUSH_OPERATION_METHOD_CONFIRMED
    } DLMS_PUSH_OPERATION_METHOD;

    /*Data protection identified key types.*/
    typedef enum
    {
        /*Global unicast encryption key.*/
        DLMS_DATA_PROTECTION_IDENTIFIED_KEY_TYPE_UNICAST_ENCRYPTION,
        /*Global broadcast encryption key.*/
        DLMS_DATA_PROTECTION_IDENTIFIED_KEY_TYPE_BROADCAST_ENCRYPTION
    }DLMS_DATA_PROTECTION_IDENTIFIED_KEY_TYPE;

    /*Data protection wrapped key types.*/
    typedef enum
    {
        /*Master key.*/
        DLMS_DATA_PROTECTION_WRAPPED_KEY_TYPE_MASTER_KEY = 0
    }DLMS_DATA_PROTECTION_WRAPPED_KEY_TYPE;

    /*Enumerates data protection key types.*/
    typedef enum
    {
        /*Identified key.*/
        DLMS_DATA_PROTECTION_KEY_TYPE_IDENTIFIED,
        /*Wrapped key.*/
        DLMS_DATA_PROTECTION_KEY_TYPE_WRAPPED,
        /*Agreed.*/
        DLMS_DATA_PROTECTION_KEY_TYPE_AGREED
    }DLMS_DATA_PROTECTION_KEY_TYPE;

    typedef enum
    {
        DLMS_CLOCK_STATUS_OK = 0x0,
        DLMS_CLOCK_STATUS_INVALID_VALUE = 0x1,
        DLMS_CLOCK_STATUS_DOUBTFUL_VALUE = 0x2,
        DLMS_CLOCK_STATUS_DIFFERENT_CLOCK_BASE = 0x4,
        DLMS_CLOCK_STATUS_RESERVED1 = 0x8,
        DLMS_CLOCK_STATUS_RESERVED2 = 0x10,
        DLMS_CLOCK_STATUS_RESERVED3 = 0x20,
        DLMS_CLOCK_STATUS_RESERVED4 = 0x40,
        DLMS_CLOCK_STATUS_DAYLIGHT_SAVE_ACTIVE = 0x80,
        //Skip clock status on write.
        DLMS_CLOCK_STATUS_SKIP = 0xFF
    } DLMS_CLOCK_STATUS;

    // DataType extra info.
    typedef enum
    {
        // No extra info.
        DLMS_DATE_TIME_EXTRA_INFO_NONE = 0x0,
        // Daylight savings begin.
        DLMS_DATE_TIME_EXTRA_INFO_DST_BEGIN = 0x1,
        // Daylight savings end.
        DLMS_DATE_TIME_EXTRA_INFO_DST_END = 0x2,
        // Last day of month.
        DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY = 0x4,
        // 2nd last day of month
        DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY2 = 0x8,
    }DLMS_DATE_TIME_EXTRA_INFO;

    /*
     * Used priority.
     */
    typedef enum
    {
        // Normal priority.
        DLMS_PRIORITY_NORMAL = 0,

        // High priority.
        DLMS_PRIORITY_HIGH = 1
    } DLMS_PRIORITY;

    typedef enum
    {
        DLMS_SERVICE_CLASS_UN_CONFIRMED = 0,
        DLMS_SERVICE_CLASS_CONFIRMED = 1
    } DLMS_SERVICE_CLASS;

    typedef enum tagDLMS_OBJECT_TYPE
    {
        DLMS_OBJECT_TYPE_NONE = 0,
        DLMS_OBJECT_TYPE_DATA = 1,
        DLMS_OBJECT_TYPE_REGISTER = 3,
        DLMS_OBJECT_TYPE_EXTENDED_REGISTER = 4,
        DLMS_OBJECT_TYPE_DEMAND_REGISTER = 5,
        DLMS_OBJECT_TYPE_REGISTER_ACTIVATION = 6,
        DLMS_OBJECT_TYPE_PROFILE_GENERIC = 7,
        DLMS_OBJECT_TYPE_CLOCK = 8,
#ifndef DLMS_IGNORE_SCRIPT_TABLE
        DLMS_OBJECT_TYPE_SCRIPT_TABLE = 9,
#endif //DLMS_IGNORE_SCRIPT_TABLE
#ifndef DLMS_IGNORE_SCHEDULE
        DLMS_OBJECT_TYPE_SCHEDULE = 10,
#endif //DLMS_IGNORE_SCHEDULE
        DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE = 11,
        DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME = 12,
        DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME = 15,
        DLMS_OBJECT_TYPE_SAP_ASSIGNMENT = 17,
        DLMS_OBJECT_TYPE_IMAGE_TRANSFER = 18,
        DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP = 19,
        DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR = 20,
        DLMS_OBJECT_TYPE_REGISTER_MONITOR = 21,
        DLMS_OBJECT_TYPE_ACTION_SCHEDULE = 22,
        DLMS_OBJECT_TYPE_IEC_HDLC_SETUP = 23,
        DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP = 24,
        DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP = 25,
        DLMS_OBJECT_TYPE_UTILITY_TABLES = 26,
        DLMS_OBJECT_TYPE_MODEM_CONFIGURATION = 27,
#ifndef DLMS_IGNORE_SCRIPT_AUTO_ANSWER
        DLMS_OBJECT_TYPE_AUTO_ANSWER = 28,
#endif //DLMS_IGNORE_SCRIPT_AUTO_ANSWER
#ifndef DLMS_IGNORE_SCRIPT_AUTO_CONNECT
        DLMS_OBJECT_TYPE_AUTO_CONNECT = 29,
#endif //DLMS_IGNORE_SCRIPT_AUTO_CONNECT
        DLMS_OBJECT_TYPE_TCP_UDP_SETUP = 41,
        DLMS_OBJECT_TYPE_IP4_SETUP = 42,
        DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP = 43,
        DLMS_OBJECT_TYPE_PPP_SETUP = 44,
        DLMS_OBJECT_TYPE_GPRS_SETUP = 45,
        DLMS_OBJECT_TYPE_SMTP_SETUP = 46,
        DLMS_OBJECT_TYPE_GSM_DIAGNOSTIC = 47,
        DLMS_OBJECT_TYPE_IP6_SETUP = 48,
        DLMS_OBJECT_TYPE_REGISTER_TABLE = 61,
        DLMS_OBJECT_TYPE_COMPACT_DATA = 62,
        DLMS_OBJECT_TYPE_STATUS_MAPPING = 63,
        DLMS_OBJECT_TYPE_SECURITY_SETUP = 64,
        DLMS_OBJECT_TYPE_DISCONNECT_CONTROL = 70,
        DLMS_OBJECT_TYPE_LIMITER = 71,
        DLMS_OBJECT_TYPE_MBUS_CLIENT = 72,

        DLMS_OBJECT_TYPE_PUSH_SETUP = 40,
        /**
        * S-FSK Phy MAC Setup
        */
        DLMS_OBJECT_TYPE_SFSK_PHY_MAC_SETUP = 50,

        /*
        * S-FSK Active initiator.
        */
        DLMS_OBJECT_TYPE_SFSK_ACTIVE_INITIATOR = 51,
        /*
        * S-FSK MAC synchronization timeouts
        */
        DLMS_OBJECT_TYPE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS = 52,

        /*
        * S-FSK MAC Counters.
        */
        DLMS_OBJECT_TYPE_SFSK_MAC_COUNTERS = 53,

        /*
        * IEC 61334-4-32 LLC setup.
        */
        DLMS_OBJECT_TYPE_IEC_61334_4_32_LLC_SETUP = 55,

        /*
        * S-FSK Reporting system list.
        */
        DLMS_OBJECT_TYPE_SFSK_REPORTING_SYSTEM_LIST = 56,

        /*
        * ISO/IEC 8802-2 LLC Type 1 setup.
        */
        DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE1_SETUP = 57,

        /*
        * ISO/IEC 8802-2 LLC Type 2 setup.
        */
        DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE2_SETUP = 58,
        /*
        * ISO/IEC 8802-2 LLC Type 3 setup.
        */
        DLMS_OBJECT_TYPE_IEC_8802_LLC_TYPE3_SETUP = 59,

        DLMS_OBJECT_TYPE_WIRELESS_MODE_Q_CHANNEL = 73,
        DLMS_OBJECT_TYPE_MBUS_MASTER_PORT_SETUP = 74,
        DLMS_OBJECT_TYPE_MBUS_PORT_SETUP = 76,
        DLMS_OBJECT_TYPE_MBUS_DIAGNOSTIC = 77,
        DLMS_OBJECT_TYPE_PARAMETER_MONITOR = 65,
        /*
        * Arbitrator
        */
        DLMS_OBJECT_TYPE_ARBITRATOR = 68,
        /*
        * Addresses that are provided by the base node during the opening of the
        * convergence layer.
        */
        DLMS_OBJECT_TYPE_LLC_SSCS_SETUP = 80,

        /*
        * Counters related to the physical layers exchanges.
        */
        DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS = 81,

        /*
        * A necessary parameters to set up and manage the PRIME NB OFDM PLC MAC
        * layer.
        */
        DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_SETUP = 82,

        /*
        * Functional behaviour of MAC.
        */
        DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS = 83,

        /*
        * Statistical information on the operation of the MAC layer for management
        * purposes.
        */
        DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_COUNTERS = 84,

        /*
        * Parameters related to the management of the devices connected to the
        * network.
        */
        DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA = 85,

        /*
        * Identification information related to administration and maintenance of
        * PRIME NB OFDM PLC devices.
        */
        DLMS_OBJECT_TYPE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION = 86,

        /*
        * G3-PLC MAC layer counters
        */
        DLMS_OBJECT_TYPE_G3_PLC_MAC_LAYER_COUNTERS = 90,

        /*
        * G3-PLC MAC setup.
        */
        DLMS_OBJECT_TYPE_G3_PLC_MAC_SETUP = 91,

        /*
        * G3-PLC 6LoWPAN.
        */
        DLMS_OBJECT_TYPE_G3_PLC_6LO_WPAN = 92,

        /*
        * NTP setup.
        */
        DLMS_OBJECT_TYPE_NTP_SETUP = 100,
        /*
        * Function control.
        */
        DLMS_OBJECT_TYPE_FUNCTION_CONTROL = 122,

        // Communication port protection.
        DLMS_OBJECT_TYPE_COMMUNICATION_PORT_PROTECTION = 124,

        // LTE monitoring.
        DLMS_OBJECT_TYPE_LTE_MONITORING = 151,

        // CoAP setup.
        DLMS_OBJECT_TYPE_COAP_SETUP = 152,

        // CoAP diagnostic.
        DLMS_OBJECT_TYPE_COAP_DIAGNOSTIC = 153,

        // G3-PLC Hybrid RF MAC layer counters.
        DLMS_OBJECT_TYPE_G3_PLC_HYBRID_RF_MAC_LAYER_COUNTERS = 160,

        // G3-PLC Hybrid RF MAC setup.
        DLMS_OBJECT_TYPE_G3_PLC_HYBRID_RF_MAC_SETUP = 161,

        // G3-PLC Hybrid 6LoWPAN adaptation layer setup.
        DLMS_OBJECT_TYPE_G3_PLC_HYBRID_6LOWPAN_ADAPTATION_LAYER_SETUP = 162,

        /*
        * Configure a ZigBee PRO device with information necessary to create or
        * join the network.
        */
        DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP = 101,
        /*
        * Configure the behavior of a ZigBee PRO device on joining or loss of
        * connection to the network.
        */
        DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN = 102,
        /*
        * Configure the fragmentation feature of ZigBee PRO transport layer.
        */
        DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION = 103,

        DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL = 104,

        DLMS_OBJECT_TYPE_DATA_PROTECTION = 30,
        DLMS_OBJECT_TYPE_ACCOUNT = 111,
        DLMS_OBJECT_TYPE_CREDIT = 112,
        DLMS_OBJECT_TYPE_CHARGE = 113,
        DLMS_OBJECT_TYPE_TOKEN_GATEWAY = 115,
        DLMS_OBJECT_TYPE_ARRAY_MANAGER = 123,
        // Tariff Plan (Piano Tariffario) is used in Italian standard UNI/TS 11291-11.
        DLMS_OBJECT_TYPE_TARIFF_PLAN = 8192,
    } DLMS_OBJECT_TYPE;

    typedef enum
    {
        HDLC_INFO_MAX_INFO_TX = 0x5,
        HDLC_INFO_MAX_INFO_RX = 0x6,
        HDLC_INFO_WINDOW_SIZE_TX = 0x7,
        HDLC_INFO_WINDOW_SIZE_RX = 0x8
    } HDLC_INFO;

    typedef enum
    {
        DEFAULT_MAX_INFO_TX = 128,
        DEFAULT_MAX_INFO_RX = 128,
        DEFAULT_MAX_WINDOW_SIZE_TX = 1,
        DEFAULT_MAX_WINDOW_SIZE_RX = 1
    } DEFAULT_MAX;

    typedef enum
    {
        DLMS_DATA_TYPE_NONE = 0,
        DLMS_DATA_TYPE_ARRAY = 1,
        DLMS_DATA_TYPE_STRUCTURE = 2,
        DLMS_DATA_TYPE_BOOLEAN = 3,
        DLMS_DATA_TYPE_BIT_STRING = 4,
        DLMS_DATA_TYPE_INT32 = 5,
        DLMS_DATA_TYPE_UINT32 = 6,
        DLMS_DATA_TYPE_OCTET_STRING = 9,
        DLMS_DATA_TYPE_STRING = 10,
        DLMS_DATA_TYPE_STRING_UTF8 = 12,
        DLMS_DATA_TYPE_BINARY_CODED_DESIMAL = 13,
        DLMS_DATA_TYPE_INT8 = 15,
        DLMS_DATA_TYPE_INT16 = 16,
        DLMS_DATA_TYPE_UINT8 = 17,
        DLMS_DATA_TYPE_UINT16 = 18,
        DLMS_DATA_TYPE_COMPACT_ARRAY = 19,
        DLMS_DATA_TYPE_INT64 = 20,
        DLMS_DATA_TYPE_UINT64 = 21,
        DLMS_DATA_TYPE_ENUM = 22,
        DLMS_DATA_TYPE_FLOAT32 = 23,
        DLMS_DATA_TYPE_FLOAT64 = 24,
        DLMS_DATA_TYPE_DATETIME = 25,
        DLMS_DATA_TYPE_DATE = 26,
        DLMS_DATA_TYPE_TIME = 27,
        DLMS_DATA_TYPE_DELTA_INT8 = 28,
        DLMS_DATA_TYPE_DELTA_INT16 = 29,
        DLMS_DATA_TYPE_DELTA_INT32 = 30,
        DLMS_DATA_TYPE_DELTA_UINT8 = 31,
        DLMS_DATA_TYPE_DELTA_UINT16 = 32,
        DLMS_DATA_TYPE_DELTA_UINT32 = 33,
        DLMS_DATA_TYPE_BYREF = 0x80
    } DLMS_DATA_TYPE;

    typedef enum
    {
        // <summary>
        // No access.
        // </summary>
        DLMS_ACCESS_MODE_NONE = 0,
        // <summary>
        // The client is allowed only reading from the server.
        // </summary>
        DLMS_ACCESS_MODE_READ = 1,
        // <summary>
        // The client is allowed only writing to the server.
        // </summary>
        DLMS_ACCESS_MODE_WRITE = 2,
        // <summary>
        // The client is allowed both reading from the server and writing to it.
        // </summary>
        DLMS_ACCESS_MODE_READ_WRITE = 3,
        DLMS_ACCESS_MODE_AUTHENTICATED_READ = 4,
        DLMS_ACCESS_MODE_AUTHENTICATED_WRITE = 5,
        DLMS_ACCESS_MODE_AUTHENTICATED_READ_WRITE = 6
    } DLMS_ACCESS_MODE;

    /*The DLMS_METHOD_ACCESS_MODE enumerates the method access modes.*/
    typedef enum
    {
        // No access.
        DLMS_METHOD_ACCESS_MODE_NONE = 0x0,
        // Access.
        DLMS_METHOD_ACCESS_MODE_ACCESS = 0x1,
        // Authenticated Access.
        DLMS_METHOD_ACCESS_MODE_AUTHENTICATED_ACCESS = 0x2,
        // Authenticated request is allowed.
        DLMS_METHOD_ACCESS_MODE_AUTHENTICATED_REQUEST = 0x4,
        // Encrypted request is allowed.
        DLMS_METHOD_ACCESS_MODE_ENCRYPTED_REQUEST = 0x8,
        // Digitally signed request is allowed.
        DLMS_METHOD_ACCESS_MODE_DIGITALLY_SIGNED_REQUEST = 0x10,
        // Authenticated response is allowed.
        DLMS_METHOD_ACCESS_MODE_AUTHENTICATED_RESPONSE = 0x20,
        // Encrypted response is allowed.
        DLMS_METHOD_ACCESS_MODE_ENCRYPTED_RESPONSE = 0x40,
        // Digitally signed response is allowed.
        DLMS_METHOD_ACCESS_MODE_DIGITALLY_SIGNED_RESPONSE = 0x80
    } DLMS_METHOD_ACCESS_MODE;

    typedef enum
    {
        DLMS_ASSOCIATION_STATUS_NON_ASSOCIATED = 0,
        DLMS_ASSOCIATION_STATUS_ASSOCIATION_PENDING = 1,
        DLMS_ASSOCIATION_STATUS_ASSOCIATED = 2
    } DLMS_ASSOCIATION_STATUS;


    /*
     * Enumerates Action request types.
     */
    typedef enum
    {
        /*
        * Normal action.
        */
        DLMS_ACTION_COMMAND_TYPE_NORMAL = 1,
        /*
        * Next block.
        */
        DLMS_ACTION_COMMAND_TYPE_NEXT_BLOCK = 2,
        /*
        * Action with list.
        */
        DLMS_ACTION_COMMAND_TYPE_WITH_LIST = 3,
        /*
        * Action with first block.
        */
        DLMS_ACTION_COMMAND_TYPE_WITH_FIRST_BLOCK = 4,
        /*
        * Action with list and first block.
        */
        DLMS_ACTION_COMMAND_TYPE_WITH_LIST_AND_FIRST_BLOCK = 5,
        /*
        * Action with block.
        */
        DLMS_ACTION_COMMAND_TYPE_WITH_BLOCK = 6
    } DLMS_ACTION_COMMAND_TYPE;

    typedef enum
    {
        /*
        If this option is present, the device shall be allowed to send security,
        compartmentation, handling restrictions and TCC (closed user group)
        parameters within its IP Datagrams. The value of the IP-Option-
        Length Field must be 11, and the IP-Option-Data shall contain the
        value of the Security, Compartments, Handling Restrictions and
        Transmission Control Code values, as specified in STD0005 / RFC791.
        */
        DLMS_IP_OPTION_TYPE_SECURITY = 0x82,
        /*
        If this option is present, the device shall supply routing information to be
        used by the gateways in forwarding the datagram to the destination, and to
        record the route information.
        The IP-Option-length and IP-Option-Data values are specified in STD0005 / RFC 791.
        */
        DLMS_IP_OPTION_TYPE_LOOSE_SOURCE_AND_RECORD_ROUTE = 0x83,
        /*
        If this option is present, the device shall supply routing information to be
        used by the gateways in forwarding the datagram to the destination, and to
        record the route information.
        The IP-Option-length and IP-Option-Data values are specified in STD0005 / RFC 791.
        */
        DLMS_IP_OPTION_TYPE_STRICT_SOURCE_AND_RECORD_ROUTE = 0x89,
        /*
        If this option is present, the device shall as well:
        send originated IP Datagrams with that option, providing means
        to record the route of these Datagrams;
        as a router, send routed IP Datagrams with the route option
        adjusted according to this option.
        The IP-Option-length and IP-Option-Data values are specified in
        STD0005 / RFC 791.
        */
        DLMS_IP_OPTION_TYPE_RECORD_ROUTE = 0x07,
        /*
        If this option is present, the device shall as well:
        send originated IP Datagrams with that option, providing means
        to time-stamp the datagram in the route to its destination;
        as a router, send routed IP Datagrams with the time-stamp option
        adjusted according to this option.
        The IP-Option-length and IP-Option-Data values are specified in STD0005 / RFC 791.
        */
        DLMS_IP_OPTION_TYPE_INTERNET_TIMESTAMP = 0x44
    } DLMS_IP_OPTION_TYPE;

    typedef enum
    {
        /*
        Size of execution_time = 1. Wildcard in date allowed.
        */
        DLMS_SINGLE_ACTION_SCHEDULE_TYPE1 = 1,
        /*
        Size of execution_time = n.
        All time values are the same, wildcards in date not allowed.
        */
        DLMS_SINGLE_ACTION_SCHEDULE_TYPE2 = 2,
        /*
        Size of execution_time = n.
        All time values are the same, wildcards in date are allowed,
        */
        DLMS_SINGLE_ACTION_SCHEDULE_TYPE3 = 3,
        /*
        Size of execution_time = n.
        Time values may be different, wildcards in date not allowed,
        */
        DLMS_SINGLE_ACTION_SCHEDULE_TYPE4 = 4,
        /*
        Size of execution_time = n.
        Time values may be different, wildcards in date are allowed
        */
        DLMS_SINGLE_ACTION_SCHEDULE_TYPE5 = 5
    } DLMS_SINGLE_ACTION_SCHEDULE_TYPE;

    typedef enum
    {
        DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED,
        DLMS_IMAGE_TRANSFER_STATUS_INITIATED,
        DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_INITIATED,
        DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL,
        DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED,
        DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_INITIATED,
        DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_SUCCESSFUL,
        DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_FAILED
    } DLMS_IMAGE_TRANSFER_STATUS;

    typedef enum
    {
        /*
        * Transport security is not used.
        */
        DLMS_SECURITY_NONE = 0,

        /*
        * Authentication security is used.
        */
        DLMS_SECURITY_AUTHENTICATION = 0x10,

        /*
        * Encryption security is used.
        */
        DLMS_SECURITY_ENCRYPTION = 0x20,

        /*
        * Authentication and Encryption security are used.
        */
        DLMS_SECURITY_AUTHENTICATION_ENCRYPTION = 0x30,
    } DLMS_SECURITY;

    typedef enum
    {
        DLMS_COUNT_TYPE_TAG = 0x1,
        DLMS_COUNT_TYPE_DATA = 2,
        DLMS_COUNT_TYPE_PACKET = 3
    } DLMS_COUNT_TYPE;

    /*
    * HDLC frame types.
    */
    typedef enum
    {
        /*
        * I-Frame. Information frames are used to transport user data.
        */
        HDLC_FRAME_TYPE_I_FRAME = 0x0,
        /*
        * S-frame. Supervisory Frames are used for flow and error control.
        * Rejected, RNR and RR.
        */
        HDLC_FRAME_TYPE_S_FRAME = 0x1,
        /*
        * U-frame. Unnumbered frames are used for link management. Example SNRM and
        * UA.
        */
        HDLC_FRAME_TYPE_U_FRAME = 0x3
    } HDLC_FRAME_TYPE;

    /*
    *BER encoding enumeration values.
    */
    typedef enum
    {
        /*
        * End of Content.
        */
        BER_TYPE_EOC = 0x00,
        /*
        * Boolean.
        */
        BER_TYPE_BOOLEAN = 0x1,
        /*
        * Integer.
        */
        BER_TYPE_INTEGER = 0x2,
        /*
        * Bit String.
        */
        BER_TYPE_BIT_STRING = 0x3,
        /*
        * Octet string.
        */
        BER_TYPE_OCTET_STRING = 0x4,
        /*
        * Null value.
        */
        BER_TYPE_NULL = 0x5,
        /*
        * Object identifier.
        */
        BER_TYPE_OBJECT_IDENTIFIER = 0x6,
        /*
        * Object Descriptor.
        */
        BER_TYPE_OBJECT_DESCRIPTOR = 7,
        /*
        * External
        */
        BER_TYPE_EXTERNAL = 8,
        /*
        * Real (float).
        */
        BER_TYPE_REAL = 9,
        /*
        * Enumerated.
        */
        BER_TYPE_ENUMERATED = 10,
        /*
        * Utf8 String.
        */
        BER_TYPE_UTF8_STRING = 12,
        /*
        * Sequence.
        */
        BER_TYPE_SEQUENCE = 16,
        /*
        * Set.
        */
        BER_TYPE_SET = 17,
        /*
        * Numeric string.
        */
        BER_TYPE_NUMERIC_STRING = 18,
        /*
        * Printable string.
        */
        BER_TYPE_PRINTABLE_STRING = 19,
        /*
        * Teletex string.
        */
        BER_TYPE_TELETEX_STRING = 20,
        /*
        * Videotex string.
        */
        BER_TYPE_VIDEOTEX_STRING = 21,
        /*
        * Ia5 string
        */
        BER_TYPE_IA5_STRING = 22,
        /*
        * Utc time.
        */
        BER_TYPE_UTC_TIME = 23,
        /*
        * Generalized time.
        */
        BER_TYPE_GENERALIZED_TIME = 24,
        /*
        * Graphic string.
        */
        BER_TYPE_GRAPHIC_STRING = 25,
        /*
        * Visible string.
        */
        BER_TYPE_VISIBLE_STRING = 26,
        /*
        * General string.
        */
        BER_TYPE_GENERAL_STRING = 27,
        /*
        * Universal string.
        */
        BER_TYPE_UNIVERSAL_STRING = 28,
        /*
        * Bmp string.
        */
        BER_TYPE_BMP_STRING = 30,
        /*
        * Application class.
        */
        BER_TYPE_APPLICATION = 0x40,
        /*
        * Context class.
        */
        BER_TYPE_CONTEXT = 0x80,
        /*
        * Private class.
        */
        BER_TYPE_PRIVATE = 0xc0,
        /*
        * Constructed.
        */
        BER_TYPE_CONSTRUCTED = 0x20
    } BER_TYPE;

    /*
     * APDU types.
     */
    typedef enum
    {
        /*
        * IMPLICIT BIT STRING {version1  = 0} DEFAULT {version1}
        */
        PDU_TYPE_PROTOCOL_VERSION = 0,

        /*
        * Application-context-name
        */
        PDU_TYPE_APPLICATION_CONTEXT_NAME = 1,

        /*
        * AP-title OPTIONAL
        */
        PDU_TYPE_CALLED_AP_TITLE = 2,

        /*
        * AE-qualifier OPTIONAL.
        */
        PDU_TYPE_CALLED_AE_QUALIFIER = 3,

        /*
        * AP-invocation-identifier OPTIONAL.
        */
        PDU_TYPE_CALLED_AP_INVOCATION_ID = 4,

        /*
        * AE-invocation-identifier OPTIONAL
        */
        PDU_TYPE_CALLED_AE_INVOCATION_ID = 5,

        /*
        * AP-title OPTIONAL
        */
        PDU_TYPE_CALLING_AP_TITLE = 6,

        /*
        * AE-qualifier OPTIONAL
        */
        PDU_TYPE_CALLING_AE_QUALIFIER = 7,

        /*
        * AP-invocation-identifier OPTIONAL
        */
        PDU_TYPE_CALLING_AP_INVOCATION_ID = 8,

        /*
        * AE-invocation-identifier OPTIONAL
        */
        PDU_TYPE_CALLING_AE_INVOCATION_ID = 9,

        /*
        * The following field shall not be present if only the kernel is used.
        */
        PDU_TYPE_SENDER_ACSE_REQUIREMENTS = 10,

        /*
        * The following field shall only be present if the authentication
        * functional unit is selected.
        */
        PDU_TYPE_MECHANISM_NAME = 11,

        /*
        * The following field shall only be present if the authentication
        * functional unit is selected.
        */
        PDU_TYPE_CALLING_AUTHENTICATION_VALUE = 12,

        /*
        * Implementation-data.
        */
        PDU_TYPE_IMPLEMENTATION_INFORMATION = 29,

        /*
        * Association-information OPTIONAL
        */
        PDU_TYPE_USER_INFORMATION = 30
    } PDU_TYPE;

    /*
     * HDLC control frame types.
     */
    typedef enum
    {
        /*
        * Receive is ready.
        */
        HDLC_CONTROL_FRAME_RECEIVE_READY = 0,
        /*
        * Receive is not ready.
        */
        HDLC_CONTROL_FRAME_RECEIVE_NOT_READY = 0x1,
        /*
        * Frame is rejected.
        */
        HDLC_CONTROL_FRAME_REJECT = 2,
        /*
        * Frame is selective rejected. Not all meters support this.
        */
        HDLC_CONTROL_FRAME_SELECTIVE_REJECT = 3
    } HDLC_CONTROL_FRAME;

    /*
     * Authentication enumerates the authentication levels.
     */
    typedef enum
    {
        /*
        * No authentication is used.
        */
        DLMS_AUTHENTICATION_NONE = 0,

        /*
        * Low authentication is used.
        */
        DLMS_AUTHENTICATION_LOW = 1,
#ifndef DLMS_IGNORE_AES
        /*
        * High authentication is used. Because DLMS/COSEM specification does not
        * specify details of the HLS mechanism we have implemented Indian standard.
        * Texas Instruments also uses this.
        */
        DLMS_AUTHENTICATION_HIGH = 2,
#endif //DLMS_IGNORE_AES
        /*
        * High authentication is used. Password is hashed with MD5.
        */
        DLMS_AUTHENTICATION_HIGH_MD5 = 3,

        /*
        * High authentication is used. Password is hashed with SHA1.
        */
        DLMS_AUTHENTICATION_HIGH_SHA1 = 4,

#ifndef DLMS_IGNORE_HIGH_GMAC
        /*
        * High authentication is used. Password is hashed with GMAC.
        */
        DLMS_AUTHENTICATION_HIGH_GMAC = 5,
#endif //DLMS_IGNORE_HIGH_GMAC

        /*
        * High authentication is used. Password is hashed with SHA-256.
        */
        DLMS_AUTHENTICATION_HIGH_SHA256 = 6,
        /*
        * High authentication is used. Password is hashed with ECDSA.
        */
        DLMS_AUTHENTICATION_HIGH_ECDSA = 7
    } DLMS_AUTHENTICATION;

    //Interface type that is used.
    typedef enum
    {
        //General
        DLMS_INTERFACE_TYPE_HDLC = 0,
        // IEC 62056-47 COSEM transport layers for IPv4 networks
        DLMS_INTERFACE_TYPE_WRAPPER = 0x1,
        /**
        * Plain PDU is returned.
        */
        DLMS_INTERFACE_TYPE_PDU,
        /**
        * Wireless M-Bus frame.
        */
        DLMS_INTERFACE_TYPE_WIRELESS_MBUS,
        /**
        * IEC 62056-21 E-Mode is used to initialize communication before moving to
        * HDLC protocol.
        */
        DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E,
        /**
        * PLC Logical link control (LLC) profile is used with IEC 61334-4-32
        * connectionless LLC sublayer.
        *
        * Blue Book: 10.4.4.3.3 The connectionless LLC sublayer.
        */
        DLMS_INTERFACE_TYPE_PLC,
        /**
        * PLC Logical link control (LLC) profile is used with HDLC.
        *
        * Blue Book: 10.4.4.3.4 The HDLC based LLC sublayer.
        */
        DLMS_INTERFACE_TYPE_PLC_HDLC,
        /**
        * LowPower Wide Area Networks (LPWAN) profile is used.
        */
        DLMS_INTERFACE_TYPE_LPWAN,
        /**
        * Wi-SUN FAN mesh network is used.
        */
        DLMS_INTERFACE_TYPE_WI_SUN,
        /**
        * OFDM PLC PRIME is defined in IEC 62056-8-4.
        */
        DLMS_INTERFACE_TYPE_PLC_PRIME
    } DLMS_INTERFACE_TYPE;

    typedef enum
    {
        DLMS_DATA_REQUEST_TYPES_NONE = 0x0,
        DLMS_DATA_REQUEST_TYPES_FRAME = 0x1,
        DLMS_DATA_REQUEST_TYPES_BLOCK = 0x2,
        DLMS_DATA_REQUEST_TYPES_GBT = 0x4
    } DLMS_DATA_REQUEST_TYPES;

    /*
     * DLMS commands.
     */
    typedef enum
    {
        /*
        * No command to execute.
        */
        DLMS_COMMAND_NONE = 0,

        /*
        * Initiate request.
        */
        DLMS_COMMAND_INITIATE_REQUEST = 0x1,

        /*
        * Initiate response.
        */
        DLMS_COMMAND_INITIATE_RESPONSE = 0x8,

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        /*
        * Read request.
        */
        DLMS_COMMAND_READ_REQUEST = 0x5,
        /*
        * Read response.
        */
        DLMS_COMMAND_READ_RESPONSE = 0xC,
        /*
        * Write request.
        */
        DLMS_COMMAND_WRITE_REQUEST = 0x6,

        /*
        * Write response.
        */
        DLMS_COMMAND_WRITE_RESPONSE = 0xD,
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

        /*
        * Get request.
        */
        DLMS_COMMAND_GET_REQUEST = 0xC0,

        /*
        * Get response.
        */
        DLMS_COMMAND_GET_RESPONSE = 0xC4,

        /*
        * Set request.
        */
        DLMS_COMMAND_SET_REQUEST = 0xC1,

        /*
        * Set response.
        */
        DLMS_COMMAND_SET_RESPONSE = 0xC5,

        /*
        * Action request.
        */
        DLMS_COMMAND_METHOD_REQUEST = 0xC3,

        /*
        * Action response.
        */
        DLMS_COMMAND_METHOD_RESPONSE = 0xC7,

        /*
        HDLC Disconnect Mode.
        Responder for Disconnect Mode request.
        */
        DLMS_COMMAND_DISCONNECT_MODE = 0x1f,

        /*
        * HDLC Unacceptable Frame.
        */
        DLMS_COMMAND_REJECTED = 0x97,

        /*
        * SNRM request.
        */
        DLMS_COMMAND_SNRM = 0x93,

        /*
        * UA request.
        */
        DLMS_COMMAND_UA = 0x73,

        /*
        * AARQ request.
        */
        DLMS_COMMAND_AARQ = 0x60,

        /*
        * AARE request.
        */
        DLMS_COMMAND_AARE = 0x61,

        /*
        * Disconnect request for HDLC framing.
        */
        DLMS_COMMAND_DISC = 0x53,

        /*
        * Disconnect request.
        */
        DLMS_COMMAND_RELEASE_REQUEST = 0x62,

        /*
        * Disconnect response.
        */
        DLMS_COMMAND_RELEASE_RESPONSE = 0x63,

        /*
        * Confirmed Service Error.
        */
        DLMS_COMMAND_CONFIRMED_SERVICE_ERROR = 0x0E,

        /*
        * Exception Response.
        */
        DLMS_COMMAND_EXCEPTION_RESPONSE = 0xD8,

        /*
        * General Block Transfer.
        */
        DLMS_COMMAND_GENERAL_BLOCK_TRANSFER = 0xE0,

        /*
        * Access Request.
        */
        DLMS_COMMAND_ACCESS_REQUEST = 0xD9,
        /*
        * Access Response.
        */
        DLMS_COMMAND_ACCESS_RESPONSE = 0xDA,
        /*
        * Data Notification request.
        */
        DLMS_COMMAND_DATA_NOTIFICATION = 0x0F,

        /*
        * Event Notification request.
        */
        DLMS_COMMAND_EVENT_NOTIFICATION = 0xC2,

        /*
        * Information Report request.
        */
        DLMS_COMMAND_INFORMATION_REPORT = 0x18,

#ifndef DLMS_IGNORE_HIGH_GMAC
        /*
        * Glo get request.
        */
        DLMS_COMMAND_GLO_GET_REQUEST = 0xC8,

        /*
        * Glo get response.
        */
        DLMS_COMMAND_GLO_GET_RESPONSE = 0xCC,

        /*
        * Glo set request.
        */
        DLMS_COMMAND_GLO_SET_REQUEST = 0xC9,

        /*
        * Glo set response.
        */
        DLMS_COMMAND_GLO_SET_RESPONSE = 0xCD,

        /*
        * Glo event notification request.
        */
        DLMS_COMMAND_GLO_EVENT_NOTIFICATION_REQUEST = 0xCA,

        /*
        * Glo method request.
        */
        DLMS_COMMAND_GLO_METHOD_REQUEST = 0xCB,

        /*
        * Glo method response.
        */
        DLMS_COMMAND_GLO_METHOD_RESPONSE = 0xCF,

        /*
        * Glo Initiate request.
        */
        DLMS_COMMAND_GLO_INITIATE_REQUEST = 0x21,
        /*
        * Glo read request.
        */
        DLMS_COMMAND_GLO_READ_REQUEST = 37,

        /*
        * Glo write request.
        */
        DLMS_COMMAND_GLO_WRITE_REQUEST = 38,
        /*
        * Glo Initiate response.
        */
        DLMS_COMMAND_GLO_INITIATE_RESPONSE = 40,
        /*
        * Glo read response.
        */
        DLMS_COMMAND_GLO_READ_RESPONSE = 44,
        /*
        * Glo write response.
        */
        DLMS_COMMAND_GLO_WRITE_RESPONSE = 45,

        /*
        * Glo Confirmed Service Error..
        */
        DLMS_COMMAND_GLO_CONFIRMED_SERVICE_ERROR = 46,

        /*
        * General GLO ciphering.
        */
        DLMS_COMMAND_GENERAL_GLO_CIPHERING = 0xDB,
        /*
        * General DED ciphering.
        */
        DLMS_COMMAND_GENERAL_DED_CIPHERING = 0xDC,
        /*
        * General ciphering.
        */
        DLMS_COMMAND_GENERAL_CIPHERING = 0xDD,

        /*
        * Ded get request.
        */
        DLMS_COMMAND_DED_GET_REQUEST = 0xD0,

        /*
        * Ded get response.
        */
        DLMS_COMMAND_DED_GET_RESPONSE = 0xD4,

        /*
        * Ded set request.
        */
        DLMS_COMMAND_DED_SET_REQUEST = 0xD1,

        /*
        * Ded set response.
        */
        DLMS_COMMAND_DED_SET_RESPONSE = 0xD5,

        /*
        * Ded event notification request.
        */
        DLMS_COMMAND_DED_EVENT_NOTIFICATION = 0xD2,

        /*
        * Ded method request.
        */
        DLMS_COMMAND_DED_METHOD_REQUEST = 0xD3,

        /*
        * Ded method response.
        */
        DLMS_COMMAND_DED_METHOD_RESPONSE = 0xD7,

        /*
        * Ded read request.
        */
        DLMS_COMMAND_DED_READ_REQUEST = 69,

        /*
        * Ded write request.
        */
        DLMS_COMMAND_DED_WRITE_REQUEST = 70,

        /*
        * Ded read response.
        */
        DLMS_COMMAND_DED_READ_RESPONSE = 76,
        /*
        * Ded write response.
        */
        DLMS_COMMAND_DED_WRITE_RESPONSE = 77,
        /*
        * Glo Confirmed Service Error..
        */
        DLMS_COMMAND_DED_CONFIRMED_SERVICE_ERROR = 78,

        /*Ded initiate request.*/
        DLMS_COMMAND_DED_INITIATE_REQUEST = 65,
        /*Ded initiate response.*/
        DLMS_COMMAND_DED_INITIATE_RESPONSE = 72,

        /*Unnumbered information UI frame*/
        DLMS_COMMAND_UNNUMBERED_INFORMATION_UI = 0x3,
        /*Final unnumbered information UI*/
        DLMS_COMMAND_UNNUMBERED_INFORMATION_UI_FINAL = 0x13,

#endif //DLMS_IGNORE_HIGH_GMAC
        /*
        * Request message from client to gateway.
        */
        DLMS_COMMAND_GATEWAY_REQUEST = 0xE6,

        /*
        * Response message from gateway to client.
        */
        DLMS_COMMAND_GATEWAY_RESPONSE = 0xE7,

        // PLC discover request.
        DLMS_COMMAND_DISCOVER_REQUEST = 0x1D,
        // PLC discover report.
        DLMS_COMMAND_DISCOVER_REPORT = 0x1E,
        // PLC register request.
        DLMS_COMMAND_REGISTER_REQUEST = 0x1C,
        // PLC ping request.
        DLMS_COMMAND_PING_REQUEST = 0x19,
        // PLC ping response.
        DLMS_COMMAND_PING_RESPONSE = 0x1A,
        // PLC repeat call request.
        DLMS_COMMAND_REPEAT_CALL_REQUEST = 0x1F
    } DLMS_COMMAND;

    /*
    * Confirmed service error tells when error has occurred.
    */
    typedef enum
    {
        /*
        * Error has occurred on initialize.
        */
        DLMS_CONFIRMED_SERVICE_ERROR_INITIATE_ERROR = 1,
        /*
        * Error has occurred on read.
        */
        DLMS_CONFIRMED_SERVICE_ERROR_READ = 5,
        /*
        * Error has occurred on write.
        */
        DLMS_CONFIRMED_SERVICE_ERROR_WRITE = 6
    } DLMS_CONFIRMED_SERVICE_ERROR;

    /*
     * Access describes access errors.
     */
    typedef enum
    {
        /*
        * Other error.
        */
        DLMS_ACCESS_OTHER = 0,
        /*
        * Scope of access violated.
        */
        DLMS_ACCESS_SCOPE_OF_ACCESS_VIOLATED = 1,
        /*
        * Object access is invalid.
        */
        DLMS_ACCESS_OBJECT_ACCESS_INVALID = 2,
        /*
        * Hardware fault.
        */
        DLMS_ACCESS_HARDWARE_FAULT = 3,
        /*
        * Object is unavailable.
        */
        DLMS_ACCESS_OBJECT_UNAVAILABLE = 4
    } DLMS_ACCESS;

    /*
     * Application reference describes application errors.
     */
    typedef enum
    {
        /*
        * Other error is occurred.
        */
        DLMS_APPLICATION_REFERENCE_OTHER = 0,
        /*
        * Time elapsed.
        */
        DLMS_APPLICATION_REFERENCE_TIME_ELAPSED = 1,
        /*
        * Application unreachable.
        */
        DLMS_APPLICATION_REFERENCE_APPLICATION_UNREACHABLE = 2,
        /*
        * Application reference is invalid.
        */
        DLMS_APPLICATION_REFERENCE_APPLICATION_REFERENCE_INVALID = 3,
        /*
        * Application context unsupported.
        */
        DLMS_APPLICATION_REFERENCE_APPLICATION_CONTEXT_UNSUPPORTED = 4,
        /*
        * Provider communication error.
        */
        DLMS_APPLICATION_REFERENCE_PROVIDER_COMMUNICATION_ERROR = 5,
        /*
        * Deciphering error.
        */
        DLMS_APPLICATION_REFERENCE_DECIPHERING_ERROR = 6
    } DLMS_APPLICATION_REFERENCE;


    /*
     * Definition describes definition errors.
     */
    typedef enum
    {
        /*
        * Other error.
        */
        DLMS_DEFINITION_OTHER = 0,
        /*
        * Object is Undefined.
        */
        DLMS_DEFINITION_OBJECT_UNDEFINED = 1,
        /*
        * Object class inconsistent.
        */
        DLMS_DEFINITION_OBJECT_CLASS_INCONSISTENT = 2,
        /*
        * Object attribute inconsistent.
        */
        DLMS_DEFINITION_OBJECT_ATTRIBUTE_INCONSISTENT = 3
    } DLMS_DEFINITION;

    /*
     * DLMS service errors.
     *
     * @author Gurux Ltd.
     */
    typedef enum
    {
        /*
        * Operation is not possible
        */
        DLMS_EXCEPTION_SERVICE_ERROR_OPERATION_NOT_POSSIBLE = 1,
        /*
        * Service is not supported.
        */
        DLMS_EXCEPTION_SERVICE_ERROR_SERVICE_NOT_SUPPORTED = 2,
        /*
        * Other reason.
        */
        DLMS_EXCEPTION_SERVICE_ERROR_OTHER_REASON = 3,
        /*
        * PDU is too long.
        */
        DLMS_EXCEPTION_SERVICE_ERROR_PDU_TOO_LONG = 4,
        /*
        * Ciphering failed.
        */
        DLMS_EXCEPTION_SERVICE_ERROR_DECIPHERING_ERROR = 5,
        /*
        * Invocation counter is invalid.
        */
        DLMS_EXCEPTION_SERVICE_ERROR_INVOCATION_COUNTER_ERROR = 6
    } DLMS_EXCEPTION_SERVICE_ERROR;

    /*
     * Hardware resource describes hardware errors.
     */
    typedef enum
    {
        /*
        * Other hardware resource error.
        */
        DLMS_HARDWARE_RESOURCE_OTHER = 0,
        /*
        * Memory is unavailable.
        */
        DLMS_HARDWARE_RESOURCE_MEMORY_UNAVAILABLE = 1,
        /*
        * Processor resource is unavailable.
        */
        DLMS_HARDWARE_RESOURCE_PROCESSOR_RESOURCE_UNAVAILABLE = 2,
        /*
        * Mass storage is unavailable.
        */
        DLMS_HARDWARE_RESOURCE_MASS_STORAGE_UNAVAILABLE = 3,
        /*
        * Other resource is unavailable.
        */
        DLMS_HARDWARE_RESOURCE_OTHER_RESOURCE_UNAVAILABLE = 4
    } DLMS_HARDWARE_RESOURCE;


    /*
     * Initiate describes onitiate errors.
     */
    typedef enum
    {
        /*
        * Other error.
        */
        DLMS_INITIATE_OTHER = 0,
        /*
        * Dlms version is too low.
        */
        DLMS_INITIATE_DLMS_VERSION_TOO_LOW = 1,
        /*
        * Incompatible conformance.
        */
        DLMS_INITIATE_INCOMPATIBLE_CONFORMANCE = 2,
        /*
        * Pdu size is too short.
        */
        DLMS_INITIATE_PDU_SIZE_TOOSHORT = 3,
        /*
        * Refused by the VDE handler.
        */
        DLMS_INITIATE_REFUSED_BY_THE_VDE_HANDLER = 4
    } DLMS_INITIATE;

    /*
     * LoadDataSet describes load dataset errors.
     */
    typedef enum
    {
        /*
        * Other error.
        */
        DLMS_LOAD_DATASET_OTHER = 0,
        /*
        * Primitive out of sequence.
        */
        DLMS_LOAD_DATASET_PRIMITIVE_OUT_OF_SEQUENCE = 1,
        /*
        * Not loadable.
        */
        DLMS_LOAD_DATASET_NOT_LOADABLE = 2,
        /*
        * Dataset size is too large.
        */
        DLMS_LOAD_DATASET_SIZE_TOO_LARGE = 3,
        /*
        * Not awaited segment.
        */
        DLMS_LOAD_DATASET_NOT_AWAITED_SEGMENT = 4,
        /*
        * Interpretation failure.
        */
        DLMS_LOAD_DATASET_INTERPRETATION_FAILURE = 5,
        /*
        * Storage failure.
        */
        DLMS_LOAD_DATASET_STORAGE_FAILURE = 6,
        /*
        * Dataset not ready.
        */
        DLMS_LOAD_DATASET_NOT_READY = 7
    } DLMS_LOAD_DATASET;

    /*
     * Task describes load task errors.
     */
    typedef enum
    {
        /*
        * Other error.
        */
        DLMS_TASK_OTHER = 0,
        /*
        * No remote control.
        */
        DLMS_TASK_NO_REMOTE_CONTROL = 1,
        /*
        * Ti is stopped.
        */
        DLMS_TASK_TI_STOPPED = 2,
        /*
        * TI is running.
        */
        DLMS_TASK_TI_RUNNIN = 3,
        /*
        * TI is unusable.
        */
        DLMS_TASK_TI_UNUSABLE = 4
    } DLMS_TASK;


    /*
     * VdeState error describes Vde state errors.
     */
    typedef enum
    {
        /*
        * Other error.
        */
        DLMS_VDE_STATE_ERROR_OTHER = 0,
        /*
        * No DLMS context.
        */
        DLMS_VDE_STATE_ERROR_NO_DLMS_CONTEXT = 1,
        /*
        * Loading dataset error.
        */
        DLMS_VDE_STATE_ERROR_LOADING_DATASET = 2,
        /*
        * Status No change.
        */
        DLMS_VDE_STATE_ERROR_STATUS_NO_CHANGE = 3,
        /*
        * Status Inoperable.
        */
        DLMS_VDE_STATE_ERROR_STATUS_INOPERABLE = 4
    } DLMS_VDE_STATE_ERROR;

    /*
     * Enumerates Get request and response types.
     */
    typedef enum
    {
        /*
        * Normal Get.
        */
        DLMS_GET_COMMAND_TYPE_NORMAL = 1,
        /*
        * Next data block.
        */
        DLMS_GET_COMMAND_TYPE_NEXT_DATA_BLOCK = 2,
        /*
        * Get request with list.
        */
        DLMS_GET_COMMAND_TYPE_WITH_LIST = 3
    } DLMS_GET_COMMAND_TYPE;

    /*
     * Service describes service errors.
     */
    typedef enum
    {
        /*
        * Other error.
        */
        DLMS_SERVICE_OTHER = 0,
        /*
        * PDU size is wrong.
        */
        DLMS_SERVICE_PDU_SIZE = 1,
        /*
        * Service is unsupported.
        */
        DLMS_SERVICE_UNSUPPORTED = 2
    } DLMS_SERVICE;

    /*
     * ServiceError enumerates service errors.
     */
    typedef enum
    {
        /*
        * Application error.
        */
        DLMS_SERVICE_ERROR_APPLICATION_REFERENCE = 0,
        /*
        * Hardware error.
        */
        DLMS_SERVICE_ERROR_HARDWARE_RESOURCE = 1,
        /*
        * Vde state error.
        */
        DLMS_SERVICE_ERROR_VDE_STATE_ERROR = 2,
        /*
        * Service error.
        */
        DLMS_SERVICE_ERROR_SERVICE = 3,
        /*
        * Definition error.
        */
        DLMS_SERVICE_ERROR_DEFINITION = 4,
        /*
        * Access error.
        */
        DLMS_SERVICE_ERROR_ACCESS = 5,
        /*
        * Initiate error.
        */
        DLMS_SERVICE_ERROR_INITIATE = 6,
        /*
        * LoadDataSet error.
        */
        DLMS_SERVICE_ERROR_LOAD_DATASET = 7,
        /*
        * Task error.
        */
        DLMS_SERVICE_ERROR_TAS = 8,
        /*
        * Other error describes manufacturer specific error code.
        */
        DLMS_SERVICE_ERROR_OTHER_ERROR = 9
    } DLMS_SERVICE_ERROR;

    /*
     * Enumerates Set request types.
     */
    typedef enum
    {
        /*
        * Normal Set.
        */
        DLMS_SET_COMMAND_TYPE_NORMAL = 1,
        /*
        * Set with first data block.
        */
        DLMS_SET_COMMAND_TYPE_FIRST_DATABLOCK = 2,
        /*
        * Set with data block.
        */
        DLMS_SET_COMMAND_TYPE_WITH_DATABLOCK = 3,
        /*
        * Set with list .
        */
        DLMS_SET_COMMAND_TYPE_WITH_LIST = 4,
        /*
        * Set with list and first data block.
        */
        DLMS_SET_COMMAND_TYPE_WITH_LIST_AND_WITH_FIRST_DATA_BLOCK = 5
    } DLMS_SET_COMMAND_TYPE;

    /*
     * Enumerates single read response types.
     */
    typedef enum
    {
        /*
        * Normal data.
        */
        DLMS_SINGLE_READ_RESPONSE_DATA = 0,
        /*
        * Error has occurred on read.
        */
        DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR = 1,
        /*
        * Return data as blocks.
        */
        DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT = 2,
        /*
        * Return block number.
        */
        DLMS_SINGLE_READ_RESPONSE_BLOCK_NUMBER = 3
    } DLMS_SINGLE_READ_RESPONSE;

    /*
     * Enumerates single write response types.
     */
    typedef enum
    {
        /*
        * Write succeeded.
        */
        DLMS_SINGLE_WRITE_RESPONSE_SUCCESS = 0,
        /*
        * Write error has occurred.
        */
        DLMS_SINGLE_WRITE_RESPONSE_DATA_ACCESS_ERROR = 1,
        /*
        * Get next block.
        */
        DLMS_SINGLE_WRITE_RESPONSE_BLOCK_NUMBER = 2
    } DLMS_SINGLE_WRITE_RESPONSE;

    /*
     * Enumerates how data is access on read or write.
     */
    typedef enum
    {
        /*
        * Read data using SN.
        */
        DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME = 2,
        /*
        * Get data using parameterized access.
        */
        DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS = 4,
        /*
        * Get next block.
        */
        DLMS_VARIABLE_ACCESS_SPECIFICATION_BLOCK_NUMBER_ACCESS = 5,
        /*
        * Read data as blocks.
        */
        DLMS_VARIABLE_ACCESS_SPECIFICATION_READ_DATA_BLOCK_ACCESS = 6,
        /*
        * Write data as blocks.
        */
        DLMS_VARIABLE_ACCESS_SPECIFICATION_WRITE_DATA_BLOCK_ACCESS = 7
    } DLMS_VARIABLE_ACCESS_SPECIFICATION;

    /*
* Enumerates all conformance bits.
* https://www.gurux.fi/Gurux.DLMS.Conformance
*/
    typedef enum
    {
        /*
        * Reserved zero conformance bit.
        */
        DLMS_CONFORMANCE_NONE = 0,
        /*
        * Reserved zero conformance bit.
        */
        DLMS_CONFORMANCE_RESERVED_ZERO = 0x1,

        /*
        * General protection conformance bit.
        */
        DLMS_CONFORMANCE_GENERAL_PROTECTION = 0x2,

        /*
        * General block transfer conformance bit.
        */
        DLMS_CONFORMANCE_GENERAL_BLOCK_TRANSFER = 0x4,
        /*
        * Read conformance bit.
        */
        DLMS_CONFORMANCE_READ = 0x8,
        /*
        * Write conformance bit.
        */
        DLMS_CONFORMANCE_WRITE = 0x10,
        /*
        * Un confirmed write conformance bit.
        */
        DLMS_CONFORMANCE_UN_CONFIRMED_WRITE = 0x20,
        /*
        Delta value encoding.
        */
        DLMS_CONFORMANCE_DELTA_VALUE_ENCODING = 0x40,
        /*
        * Reserved seven conformance bit.
        */
        DLMS_CONFORMANCE_RESERVED_SEVEN = 0x80,
        /*
        * Attribute 0 supported with set conformance bit.
        */
        DLMS_CONFORMANCE_ATTRIBUTE_0_SUPPORTED_WITH_SET = 0x100,
        /*
        * Priority mgmt supported conformance bit.
        */
        DLMS_CONFORMANCE_PRIORITY_MGMT_SUPPORTED = 0x200,
        /*
        * Attribute 0 supported with get conformance bit.
        */
        DLMS_CONFORMANCE_ATTRIBUTE_0_SUPPORTED_WITH_GET = 0x400,
        /*
        * Block transfer with get or read conformance bit.
        */
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ = 0x800,
        /*
        * Block transfer with set or write conformance bit.
        */
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE = 0x1000,
        /*
        * Block transfer with action conformance bit.
        */
        DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION = 0x2000,
        /*
        * multiple references conformance bit.
        */
        DLMS_CONFORMANCE_MULTIPLE_REFERENCES = 0x4000,
        /*
        * Information report conformance bit.
        */
        DLMS_CONFORMANCE_INFORMATION_REPORT = 0x8000,
        /*
        * Data notification conformance bit.
        */
        DLMS_CONFORMANCE_DATA_NOTIFICATION = 0x10000,
        /*
        * Access conformance bit.
        */
        DLMS_CONFORMANCE_ACCESS = 0x20000,
        /*
        * Parameterized access conformance bit.
        */
        DLMS_CONFORMANCE_PARAMETERIZED_ACCESS = 0x40000,
        /*
        * Get conformance bit.
        */
        DLMS_CONFORMANCE_GET = 0x80000,
        /*
        * Set conformance bit.
        */
        DLMS_CONFORMANCE_SET = 0x100000,
        /*
        * Selective access conformance bit.
        */
        DLMS_CONFORMANCE_SELECTIVE_ACCESS = 0x200000,
        /*
        * Event notification conformance bit.
        */
        DLMS_CONFORMANCE_EVENT_NOTIFICATION = 0x400000,
        /*
        * Action conformance bit.
        */
        DLMS_CONFORMANCE_ACTION = 0x800000
    }DLMS_CONFORMANCE;

    typedef enum
    {
        /*
        If this option is present, the device shall be allowed to send security,
        compartmentation, handling restrictions and TCC (closed user group)
        parameters within its IP Datagrams. The value of the IP-Option-
        Length Field must be 11, and the IP-Option-Data shall contain the
        value of the Security, Compartments, Handling Restrictions and
        Transmission Control Code values, as specified in STD0005 / RFC791.
        */
        IP_OPTION_TYPE_DLMS_SECURITY = 0x82,
        /*
        If this option is present, the device shall supply routing information to be
        used by the gateways in forwarding the datagram to the destination, and to
        record the route information.
        The IP-Option-length and IP-Option-Data values are specified in STD0005 / RFC 791.
        */
        IP_OPTION_TYPE_LOOSE_SOURCE_AND_RECORD_ROUTE = 0x83,
        /*
        If this option is present, the device shall supply routing information to be
        used by the gateways in forwarding the datagram to the destination, and to
        record the route information.
        The IP-Option-length and IP-Option-Data values are specified in STD0005 / RFC 791.
        */
        IP_OPTION_TYPE_STRICT_SOURCE_AND_RECORD_ROUTE = 0x89,
        /*
        If this option is present, the device shall as well:
        send originated IP Datagrams with that option, providing means
        to record the route of these Datagrams;
        as a router, send routed IP Datagrams with the route option
        adjusted according to this option.
        The IP-Option-length and IP-Option-Data values are specified in
        STD0005 / RFC 791.
        */
        IP_OPTION_TYPE_RECORD_ROUTE = 0x07,
        /*
        If this option is present, the device shall as well:
        send originated IP Datagrams with that option, providing means
        to time-stamp the datagram in the route to its destination;
        as a router, send routed IP Datagrams with the time-stamp option
        adjusted according to this option.
        The IP-Option-length and IP-Option-Data values are specified in STD0005 / RFC 791.
        */
        IP_OPTION_TYPE_INTERNET_TIMESTAMP = 0x44
    }IP_OPTION_TYPE;

    // Enumerates Set response types.
    typedef enum
    {
        /*
        * Normal set response.
        */
        DLMS_SET_RESPONSE_TYPE_NORMAL = 1,
        /*
        * Set response in data blocks.
        */
        DLMS_SET_RESPONSE_TYPE_DATA_BLOCK = 2,
        /*
        * Set response in last data block.
        */
        DLMS_SET_RESPONSE_TYPE_LAST_DATA_BLOCK = 3,
        /*
        * Set response in last data block with list.
        */
        DLMS_SET_RESPONSE_TYPE_LAST_DATA_BLOCK_WITH_LIST = 4,
        /*
        * Set with list response.
        */
        DLMS_SET_RESPONSE_TYPE_WITH_LIST = 5
    }DLMS_SET_RESPONSE_TYPE;

    /*
    * Enumerates all Unit constants.
    */
    typedef enum
    {
        /*
        * No Unit.
        */
        DLMS_UNIT_NONE = 0,
        /*
        * Year.
        */
        DLMS_UNIT_YEAR = 1,

        /*
        * Month.
        */
        DLMS_UNIT_MONTH = 2,

        /*
        * Week.
        */
        DLMS_UNIT_WEEK = 3,

        /*
        * Day.
        */
        DLMS_UNIT_DAY = 4,

        /*
        * Hour.
        */
        DLMS_UNIT_HOUR = 5,

        /*
        * Minute.
        */
        DLMS_UNIT_MINUTE = 6,

        /*
        * Second.
        */
        DLMS_UNIT_SECOND = 7,

        /*
        * Phase angle degree.
        */
        DLMS_UNIT_PHASE_ANGLE_DEGREE = 8,
        /*
        * Temperature T degree centigrade, rad*180/p.
        */
        DLMS_UNIT_TEMPERATURE = 9,
        /*
        * Local currency.
        */
        DLMS_UNIT_LOCAL_CURRENCY = 10,
        /*
        * Length l meter m.
        */
        DLMS_UNIT_LENGTH = 11,
        /*
        * Speed v m/s.
        */
        DLMS_UNIT_SPEED = 12,
        /*
        * Volume V m3.
        */
        DLMS_UNIT_VOLUME_CUBIC_METER = 13,
        /*
        * Corrected volume m3.
        */
        DLMS_UNIT_CORRECTED_VOLUME = 14,
        /*
        * Volume flux m3/60*60s.
        */
        DLMS_UNIT_VOLUME_FLUX_HOUR = 15,
        /*
        * Corrected volume flux m3/60*60s.
        */
        DLMS_UNIT_CORRECTED_VOLUME_FLUX_HOUR = 16,
        /*
        * Volume flux m3/24*60*60s.
        */
        DLMS_UNIT_VOLUME_FLUX_DAY = 17,
        /*
        * Corrected volume flux m3/24*60*60s.
        */
        DLMS_UNIT_CORRECTED_VOLUME_FLUX_DAY = 18,
        /*
        * Volume 10-3 m3.
        */
        DLMS_UNIT_VOLUME_LITER = 19,
        /*
        * Mass m kilogram kg.
        */
        DLMS_UNIT_MASS_KG = 20,
        /*
        * return "Force F newton N.
        */
        DLMS_UNIT_FORCE = 21,
        /*
        * Energy newtonmeter J = Nm = Ws.
        */
        DLMS_UNIT_ENERGY = 22,
        /*
        * Pressure p pascal N/m2.
        */
        DLMS_UNIT_PRESSURE_PASCAL = 23,
        /*
        * Pressure p bar 10-5 N/m2.
        */
        DLMS_UNIT_PRESSURE_BAR = 24,
        /*
        * Energy joule J = Nm = Ws.
        */
        DLMS_UNIT_ENERGY_JOULE = 25,
        /*
        * Thermal power J/60*60s.
        */
        DLMS_UNIT_THERMAL_POWER = 26,
        /*
        * Active power P watt W = J/s.
        */
        DLMS_UNIT_ACTIVE_POWER = 27,
        /*
        * Apparent power S.
        */
        DLMS_UNIT_APPARENT_POWER = 28,
        /*
        * Reactive power Q.
        */
        DLMS_UNIT_REACTIVE_POWER = 29,
        /*
        * Active energy W*60*60s.
        */
        DLMS_UNIT_ACTIVE_ENERGY = 30,
        /*
        * Apparent energy VA*60*60s.
        */
        DLMS_UNIT_APPARENT_ENERGY = 31,
        /*
        * Reactive energy var*60*60s.
        */
        DLMS_UNIT_REACTIVE_ENERGY = 32,
        /*
        * Current I ampere A.
        */
        DLMS_UNIT_CURRENT = 33,
        /*
        * Electrical charge Q coulomb C = As.
        */
        DLMS_UNIT_ELECTRICAL_CHARGE = 34,
        /*
        * Voltage.
        */
        DLMS_UNIT_VOLTAGE = 35,
        /*
        * Electrical field strength E V/m.
        */
        DLMS_UNIT_ELECTRICAL_FIELD_STRENGTH = 36,
        /*
        * Capacity C farad C/V = As/V.
        */
        DLMS_UNIT_CAPACITY = 37,
        /*
        * Resistance R ohm = V/A.
        */
        DLMS_UNIT_RESISTANCE = 38,
        /*
        * Resistivity.
        */
        DLMS_UNIT_RESISTIVITY = 39,
        /*
        * Magnetic flux F weber Wb = Vs.
        */
        DLMS_UNIT_MAGNETIC_FLUX = 40,
        /*
        * Induction T tesla Wb/m2.
        */
        DLMS_UNIT_INDUCTION = 41,
        /*
        * Magnetic field strength H A/m.
        */
        DLMS_UNIT_MAGNETIC = 42,
        /*
        * Inductivity L henry H = Wb/A.
        */
        DLMS_UNIT_INDUCTIVITY = 43,
        /*
        * Frequency f.
        */
        DLMS_UNIT_FREQUENCY = 44,
        /*
        * Active energy meter constant 1/Wh.
        */
        DLMS_UNIT_ACTIVE = 45,
        /*
        * Reactive energy meter constant.
        */
        DLMS_UNIT_REACTIVE = 46,
        /*
        * Apparent energy meter constant.
        */
        DLMS_UNIT_APPARENT = 47,
        /*
        * V260*60s.
        */
        DLMS_UNIT_V260 = 48,
        /*
        * A260*60s.
        */
        DLMS_UNIT_A260 = 49,
        /*
        * Mass flux kg/s.
        */
        DLMS_UNIT_MASS_KG_PER_SECOND = 50,
        /*
        * Unit is Conductance siemens 1/ohm.
        */
        DLMS_UNIT_CONDUCTANCE = 51,
        /*
        * Temperature in Kelvin.
        */
        DLMS_UNIT_KELVIN = 52,
        /*
        * 1/(V2h) RU2h , volt-squared hour meter constant or pulse value.
        */
        DLMS_UNIT_RU2H = 53,
        /*
        * 1/(A2h) RI2h , ampere-squared hour meter constant or pulse value.
        */
        DLMS_UNIT_RI2H = 54,
        /*
        * 1/m3 RV , meter constant or pulse value  = volume).
        */
        DLMS_UNIT_CUBIC_METER_RV = 55,
        /*
        * Percentage.
        */
        DLMS_UNIT_PERCENTAGE = 56,
        /*
        * Ah ampere hours.
        */
        DLMS_UNIT_AMPERE_HOURS = 57,
        /*
        * Wh/m3 energy per volume 3,6*103 J/m3.
        */
        DLMS_UNIT_ENERGY_PER_VOLUME = 60,
        /*
        * J/m3 calorific value, wobbe.
        */
        DLMS_UNIT_WOBBE = 61,
        /*
        * Mol % molar fraction of gas composition mole percent  = Basic gas
        * composition unit).
        */
        DLMS_UNIT_MOLE_PERCENT = 62,
        /*
        * g/m3 mass density, quantity of material.
        */
        DLMS_UNIT_MASS_DENSITY = 63,
        /*
        * Dynamic viscosity pascal second  = Characteristic of gas stream).
        */
        DLMS_UNIT_PASCAL_SECOND = 64,
        /*
        * J/kg Specific energy NOTE The amount of energy per unit of mass of a
        * substance Joule / kilogram m2 . kg . s -2 / kg = m2.
        */
        DLMS_UNIT_JOULE_KILOGRAM = 65,
        /*
        * Pressure, gram per square centimeter.
        */
        DLMS_UNIT_PRESSURE_GRAM_PER_SQUARE_CENTIMETER = 66,
        /*
        * Pressure, atmosphere.
        */
        DLMS_UNIT_PRESSURE_ATMOSPHERE = 67,
        /*
        * Signal strength, dB milliwatt (e.g. of GSM radio systems)
        */
        DLMS_UNIT_SIGNAL_STRENGTH_MILLI_WATT = 70,
        /*
        * Signal strength, dB microvolt.
        */
        DLMS_UNIT_SIGNAL_STRENGTH_MICRO_VOLT = 71,
        /*
        * Logarithmic unit that expresses the ratio between two values of a physical quantity
        */
        DLMS_UNIT_DB = 72,
        /*
        * Length in inches.
        */
        DLMS_UNIT_INCH = 128,
        /*
        * Foot (Length).
        */
        DLMS_UNIT_FOOT = 129,
        /*
        * Pound (mass).
        */
        DLMS_UNIT_POUND = 130,
        /*
        * Fahrenheit.
        */
        DLMS_UNIT_FAHRENHEIT = 131,
        /*
        * Rankine.
        */
        DLMS_UNIT_RANKINE = 132,
        /*
        * Square inch.
        */
        DLMS_UNIT_SQUARE_INCH = 133,
        /*
        * Square foot.
        */
        DLMS_UNIT_SQUARE_FOOT = 134,
        /*
        * Acre.
        */
        DLMS_UNIT_ACRE = 135,
        /*
        * Cubic inch.
        */
        DLMS_UNIT_CUBIC_INCH = 136,
        /*
        * Cubic foot.
        */
        DLMS_UNIT_CUBIC_FOOT = 137,
        /*
        * Acre foot.
        */
        DLMS_UNIT_ACRE_FOOT = 138,
        /*
        * Gallon (imperial).
        */
        DLMS_UNIT_GALLON_IMPERIAL = 139,
        /*
        *  Gallon (US).
        */
        DLMS_UNIT_GALLON_US = 140,
        /*
        * Pound force.
        */
        DLMS_UNIT_POUND_FORCE = 141,
        /*
        * Pound force per square inch.
        */
        DLMS_UNIT_POUND_FORCE_PER_SQUARE_INCH = 142,
        /*
        * Pound per cubic foot.
        */
        DLMS_UNIT_POUND_PER_CUBIC_FOOT = 143,
        /*
        * Pound per (foot second).
        */
        DLMS_UNIT_POUND_PER_FOOT_SECOND = 144,
        /*
        * Square foot per second.
        */
        DLMS_UNIT_SQUARE_FOOT_PER_SECOND = 145,
        /*
        * British thermal unit.
        */
        DLMS_UNIT_BRITISH_THERMAL_UNIT = 146,
        /*
        * Therm EU.
        */
        DLMS_UNIT_THERM_EU = 147,
        /*
        * Therm US.
        */
        DLMS_UNIT_THERM_US = 148,
        /*
        * British thermal unit per pound.
        */
        DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_POUND = 149,
        /*
        * British thermal unit per cubic foot.
        */
        DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_CUBIC_FOOT = 150,
        /*
        * Cubic feet.
        */
        DLMS_UNIT_CUBIC_FEET = 151,
        /*
        * Foot per second.
        */
        DLMS_UNIT_FOOT_PER_SECOND = 152,
        /*
        * Cubic foot per second.
        */
        DLMS_UNIT_CUBIC_FOOT_PER_SECOND = 153,
        /*
        * Cubic foot per min.
        */
        DLMS_UNIT_CUBIC_FOOT_PER_MIN = 154,
        /*
        * Cubic foot per hour.
        */
        DLMS_UNIT_CUBIC_FOOT_PER_HOUR = 155,
        /*
        * Cubic foot per day
        */
        DLMS_UNIT_CUBIC_FOOT_PER_DAY = 156,
        /*
        * Acre foot per second.
        */
        DLMS_UNIT_ACRE_FOOT_PER_SECOND = 157,
        /*
        * Acre foot per min.
        */
        DLMS_UNIT_ACRE_FOOT_PER_MIN = 158,
        /*
        *  Acre foot per hour.
        */
        DLMS_UNIT_ACRE_FOOT_PER_HOUR = 159,
        /*
        *  Acre foot per day.
        */
        DLMS_UNIT_ACRE_FOOT_PER_DAY = 160,
        /*
        * Imperial gallon.
        */
        DLMS_UNIT_IMPERIAL_GALLON = 161,
        /*
        * Imperial gallon per second.
        */
        DLMS_UNIT_IMPERIAL_GALLON_PER_SECOND = 162,
        /*
        * Imperial gallon per min.
        */
        DLMS_UNIT_IMPERIAL_GALLON_PER_MIN = 163,
        /*
        * Imperial gallon per hour.
        */
        DLMS_UNIT_IMPERIAL_GALLON_PER_HOUR = 164,
        /*
        * Imperial gallon per day.
        */
        DLMS_UNIT_IMPERIAL_GALLON_PER_DAY = 165,
        /*
        * US gallon.
        */
        DLMS_UNIT_US_GALLON = 166,
        /*
        * US gallon per second.
        */
        DLMS_UNIT_US_GALLON_PER_SECOND = 167,
        /*
        * US gallon per min.
        */
        DLMS_UNIT_US_GALLON_PER_MIN = 168,
        /*
        * US gallon per hour.
        */
        DLMS_UNIT_US_GALLON_PER_HOUR = 169,
        /*
        * US gallon per day.
        */
        DLMS_UNIT_US_GALLON_PER_DAY = 170,
        /*
        * British thermal unit per second.
        */
        DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_SECOND = 171,
        /*
        * British thermal unit per minute.
        */
        DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_MIN = 172,
        /*
        * British thermal unit per hour.
        */
        DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_HOUR = 173,
        /*
        * British thermal unit per day.
        */
        DLMS_UNIT_BRITISH_THERMAL_UNIT_PER_DAY = 174,
        /*
        * Other Unit.
        */
        DLMS_UNIT_OTHER = 254,
        /*
        * No Unit.
        */
        DLMS_UNIT_NO_UNIT = 255
    }DLMS_UNIT;


    /*
    * Specifies trace levels.
    *
    */
    typedef enum {
        /*
        * Output no tracing and debugging messages.
        */
        GX_TRACE_LEVEL_OFF,

        /*
        * Output error-handling messages.
        */
        GX_TRACE_LEVEL_ERROR,

        /*
        * Output warnings and error-handling messages.
        */
        GX_TRACE_LEVEL_WARNING,

        /*
        * Output informational messages, warnings, and error-handling messages.
        */
        GX_TRACE_LEVEL_INFO,

        /*
        * Output all debugging and tracing messages.
        */
        GX_TRACE_LEVEL_VERBOSE
    }GX_TRACE_LEVEL;


    /*
    * Defines the GSM status.
    */
    typedef enum {
        /*
        * Not registered.
        */
        DLMS_GSM_STATUS_NONE,
        /*
        * Registered, home network.
        */
        DLMS_GSM_STATUS_HOME_NETWORK,
        /*
        * Not registered, but MT is currently searching a new operator to register
        * to.
        */
        DLMS_GSM_STATUS_SEARCHING,
        /*
        * Registration denied.
        */
        DLMS_GSM_STATUS_DENIED,
        /*
        * Unknown.
        */
        DLMS_GSM_STATUS_UNKNOWN,
        /*
        * Registered, roaming.
        */
        DLMS_GSM_STATUS_ROAMING
    }DLMS_GSM_STATUS;


    /*
    * GSM circuit switced status.
    */
    typedef enum {
        /*
        * Inactive.
        */
        DLMS_GSM_CIRCUIT_SWITCH_STATUS_INACTIVE,
        /*
        * Incoming call.
        */
        DLMS_GSM_CIRCUIT_SWITCH_STATUS_INCOMING_CALL,
        /*
        * Active.
        */
        DLMS_GSM_CIRCUIT_SWITCH_STATUS_ACTIVE
    }DLMS_GSM_CIRCUIT_SWITCH_STATUS;


    /*
    * Packet switched status of the modem.
    */
    typedef enum {
        /*
        * Inactive
        */
        DLMS_GSM_PACKET_SWITCH_STATUS_INACTIVE,
        /*
        * GPRS
        */
        DLMS_GSM_PACKET_SWITCH_STATUS_GPRS,
        /*
        * EDGE
        */
        DLMS_GSM_PACKET_SWITCH_STATUS_EDGE,
        /*
        * UMTS
        */
        DLMS_GSM_PACKET_SWITCH_STATUS_UMTS,
        /*
        * HSDPA
        */
        DLMS_GSM_PACKET_SWITCH_STATUS_HSDPA,
        /*
        * LTE
        */
        DLMS_GSM_PACKET_SWITCH_STATUS_LTE,
        /*
        * CDMA
        */
        DLMS_GSM_PACKET_SWITCH_STATUS_CDMA
    }DLMS_GSM_PACKET_SWITCH_STATUS;

    /*
    * Enumerates payment Modes.<br/>
    * Online help:<br/>
    * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
    */
    typedef enum
    {
        /*
        * Credit mode.
        */
        DLMS_ACCOUNT_PAYMENT_MODE_CREDIT = 1,
        /*
        * Prepayment mode.
        */
        DLMS_ACCOUNT_PAYMENT_MODE_PREPAYMENT = 2
    }DLMS_ACCOUNT_PAYMENT_MODE;

    /*
    * Enumerates account status modes.<br/>
    * Online help:<br/>
    * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
    */
    typedef enum
    {
        /*
        * New (inactive) account.
        */
        DLMS_ACCOUNT_STATUS_NEW_INACTIVE_ACCOUNT = 1,
        /*
        * Account active.
        */
        DLMS_ACCOUNT_STATUS_ACTIVE = 2,
        /*
        * Account closed.
        */
        DLMS_ACCOUNT_STATUS_CLOSED = 3
    }DLMS_ACCOUNT_STATUS;

    /*
* Enumerates account credit status modes.<br/>
* Online help:<br/>
* http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
*/
    typedef enum {
        /*
        * In credit.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_NONE = 0x0,
        /*
        * In credit.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_IN_CREDIT = 0x1,

        /*
        * Low credit.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_LOW_CREDIT = 0x2,

        /*
        * Next credit enabled.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_NEXT_CREDIT_ENABLED = 0x4,

        /*
        * Next credit selectable.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_NEXT_CREDIT_SELECTABLE = 0x8,

        /*
        * Credit reference list.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_CREDIT_REFERENCE_LIST = 0x10,

        /*
        * Selectable credit in use.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_SELECTABLE_CREDIT_IN_USE = 0x20,

        /*
        * Out of credit.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_OUT_OF_CREDIT = 0x40,

        /*
        * Reserved.
        */
        DLMS_ACCOUNT_CREDIT_STATUS_RESERVED = 0x80
    }DLMS_ACCOUNT_CREDIT_STATUS;

    /*
    * Enumerates payment Modes.<br/>
    * Online help:<br/>
    * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
    */
    typedef enum {
        /*
        * Time.
        */
        DLMS_CURRENCY_TIME,
        /*
        * Consumption.
        */
        DLMS_CURRENCY_CONSUMPTION,
        /*
        * Monetary.
        */
        DLMS_CURRENCY_MONETARY
    }DLMS_CURRENCY;

    typedef enum
    {
        DLMS_REQUIRED_PROTECTION_NONE = 0,
        //Authenticated request.
        DLMS_REQUIRED_PROTECTION_AUTHENTICATED_REQUEST = 0x4,
        //Encrypted request.
        DLMS_REQUIRED_PROTECTION_ENCRYPTED_REQUEST = 0x8,
        //Digitally signed request
        DLMS_REQUIRED_PROTECTION_DIGITALLY_SIGNED_REQUEST = 0x10,
        //Authenticated response.
        DLMS_REQUIRED_PROTECTION_AUTHENTICATED_RESPONSE = 0x20,
        //Encrypted response.
        DLMS_REQUIRED_PROTECTION_ENCRYPTED_RESPONSE = 0x40,
        //Digitally signed response
        DLMS_REQUIRED_PROTECTION_DIGITALLY_SIGNED_RESPONSE = 0x80,
    }DLMS_REQUIRED_PROTECTION;

    typedef enum
    {
        /*
        * None.
        */
        DLMS_CREDIT_COLLECTION_CONFIGURATION_NONE = 0,
        /*
        * Collect when supply disconnected.
        */
        DLMS_CREDIT_COLLECTION_CONFIGURATION_DISCONNECTED = 0x1,
        /*
        * Collect in load limiting periods.
        */
        DLMS_CREDIT_COLLECTION_CONFIGURATION_LOAD_LIMITING = 0x2,
        /*
        * Collect in friendly credit periods.
        */
        DLMS_CREDIT_COLLECTION_CONFIGURATION_FRIENDLY_CREDIT = 0x4
    }DLMS_CREDIT_COLLECTION_CONFIGURATION;

    // Enumerates payment Modes.
    typedef enum
    {
        // Credit mode.
        DLMS_PAYMENT_MODE_CREDIT = 1,

        // Prepayment mode.
        DLMS_PAYMENT_MODE_PREPAYMENT = 2
    }DLMS_PAYMENT_MODE;


    // Enumerates token status codes.
    //  Online help:<br/>
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
    typedef enum
    {
        // Token format result OK.
        FORMAT_OK,

        // Authentication result OK.
        AUTHENTICATION_OK,

        // Validation result OK.
        VALIDATION_OK,

        // Token execution result OK.
        TOKEN_EXECUTION_OK,

        // Token format failure.
        DLMS_TOKEN_STATUS_CODE_TOKEN_FORMAT_FAILURE,

        // Authentication failure.
        DLMS_TOKEN_STATUS_CODE_AUTHENTICATION_FAILURE,

        // Validation result failure.
        DLMS_TOKEN_STATUS_CODE_VALIDATION_RESULT_FAILURE,

        // Token execution result failure.
        DLMS_TOKEN_STATUS_CODE_TOKEN_EXECUTION_RESULT_FAILURE,

        // Token received and not yet processed.
        DLMS_TOKEN_STATUS_CODE_TOKEN_RECEIVED
    }DLMS_TOKEN_STATUS_CODE;

    // Enumerates token delivery methods.
    //  Online help:<br/>
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
    typedef enum
    {
        // Via remote communications.
        DLMS_TOKEN_DELIVERY_REMOTE,

        // Via local communications.
        DLMS_TOKEN_DELIVERY_LOCAL,

        // Via manual entry.
        DLMS_TOKEN_DELIVERY_MANUAL
    }DLMS_TOKEN_DELIVERY;

    /*
    * Enumerates application context name.
    */
    typedef enum {
        /*
        * Logical name.
        */
        DLMS_APPLICATION_CONTEXT_NAME_LOGICAL_NAME = 1,
        /*
        * Short name.
        */
        DLMS_APPLICATION_CONTEXT_NAME_SHORT_NAME = 2,
        /*
        * Logical name with ciphering.
        */
        DLMS_APPLICATION_CONTEXT_NAME_LOGICAL_NAME_WITH_CIPHERING = 3,
        /*
        * Short name with ciphering.
        */
        DLMS_APPLICATION_CONTEXT_NAME_SHORT_NAME_WITH_CIPHERING = 4
    }DLMS_APPLICATION_CONTEXT_NAME;

    // Certificate entity.
    typedef enum {
        // Certificate entity is server.
        DLMS_CERTIFICATE_ENTITY_SERVER = 0,
        // Certificate entity is client.
        DLMS_CERTIFICATE_ENTITY_CLIENT,
        // Certificate entity is certification authority.
        DLMS_CERTIFICATE_ENTITY_CERTIFICATION_AUTHORITY,
        // Certificate entity is other.
        DLMS_CERTIFICATE_ENTITY_OTHER
    }DLMS_CERTIFICATE_ENTITY;

    // Certificate type.
    typedef enum {
        // Certificate type is digital signature.
        DLMS_CERTIFICATE_TYPE_DIGITAL_SIGNATURE = 0,
        // Certificate type is key agreement.
        DLMS_CERTIFICATE_TYPE_KEY_AGREEMENT = 1,
        // Certificate type is TLS.
        DLMS_CERTIFICATE_TYPE_TLS = 2,
        // Certificate type is other.
        DLMS_CERTIFICATE_TYPE_OTHER = 3
    }DLMS_CERTIFICATE_TYPE;

    // Connection types.
    typedef enum {
        //Connection is not made for the meter.
        DLMS_CONNECTION_STATE_NONE = 0,
        //Connection is made for HDLC level.
        DLMS_CONNECTION_STATE_HDLC = 1,
        //Connection is made for DLMS level.
        DLMS_CONNECTION_STATE_DLMS = 2,
        //Connection is made for optical IEC 62056-21 level.
        DLMS_CONNECTION_STATE_IEC = 4
    }DLMS_CONNECTION_STATE;

    // Used ECC scheme.
    typedef enum {
        //ECC-P256 domain parameters are used.
        ECC_P256 = 0,
        //ECC-384 domain parameters are used.
        ECC_P384 = 1
    }ECC;
    typedef enum {
        //Data is captured with Capture-method.
        DLMS_CAPTURE_METHOD_INVOKE,
        //Data is captured upon reading.
        DLMS_CAPTURE_METHOD_IMPLICIT
    }DLMS_CAPTURE_METHOD;

    /**
     * Present functional state of the node.
     */
    typedef enum
    {
        /**
        * Disconnected.
        */
        DLMS_MAC_STATE_DISCONNECTED,
        /**
        * Terminal.
        */
        DLMS_MAC_STATE_TERMINAL,
        /**
        * Switch.
        */
        DLMS_MAC_STATE_SWITCH,
        /**
        * Base.
        */
        DLMS_MAC_STATE_BASE
    }DLMS_MAC_STATE;

    /**
     * Present functional state of the node.
     */
    typedef enum
    {
        DLMS_MAC_CAPABILITIES_NONE = 0,
        /**
        * Switch capable.
        */
        DLMS_MAC_CAPABILITIES_SWITCH_CAPABLE = 1,
        /**
        * Packet aggregation.
        */
        DLMS_MAC_CAPABILITIES_PACKET_AGGREGATION = 2,
        /**
        * Contention free period.
        */
        DLMS_MAC_CAPABILITIES_CONTENTION_FREE_PERIOD = 4,
        /**
        * Direct connection.
        */
        DLMS_MAC_CAPABILITIES_DIRECT_CONNECTION = 8,
        /**
        * Multicast.
        */
        DLMS_MAC_CAPABILITIES_MULTICAST = 0x10,
        /**
        * PHY Robustness Management.
        */
        DLMS_MAC_CAPABILITIES_PHY_ROBUSTNESS_MANAGEMENT = 0x20,
        /**
        * ARQ.
        */
        DLMS_MAC_CAPABILITIES_ARQ = 0x40,
        /**
        * Reserved for future use.
        */
        DLMS_MAC_CAPABILITIES_RESERVED_FOR_FUTURE_USE = 0x80,
        /**
        * Direct Connection Switching.
        */
        DLMS_MAC_CAPABILITIES_DIRECT_CONNECTION_SWITCHING = 0x100,
        /**
        * Multicast Switching Capability.
        */
        DLMS_MAC_CAPABILITIES_MULTICAST_SWITCHING_CAPABILITY = 0x200,
        /**
        * PHY Robustness Management Switching Capability.
        */
        DLMS_MAC_CAPABILITIES_PHY_ROBUSTNESS_MANAGEMENT_SWITCHING_CAPABILITY = 0x400,
        /**
        * ARQ Buffering Switching Capability.
        */
        DLMS_MAC_CAPABILITIES_ARQ_BUFFERING_SWITCHING_CAPABILITY = 0x800
    }
    DLMS_MAC_CAPABILITIES;

    /*
    * Enumerates credit types.
    * Online help:
    * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCredit
    */
    typedef enum
    {
        /*
        * Token credit.
        */
        DLMS_CREDIT_TYPE_TOKEN = 0,
        /*
        * Reserved credit.
        */
        DLMS_CREDIT_TYPE_RESERVED,
        /*
        * Emergency credit.
        */
        DLMS_CREDIT_TYPE_EMERGENCY,
        /*
        * TimeBased credit.
        */
        DLMS_CREDIT_TYPE_TIME_BASED,
        /*
        * Consumption based credit.
        */
        DLMS_CREDIT_TYPE_CONSUMPTION_BASED
    }DLMS_CREDIT_TYPE;

    /*
    * Enumerates credit status values.
    * Online help:
    * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCredit
    */
    typedef enum
    {
        /*
        * Enabled state.
        */
        DLMS_CREDIT_STATUS_ENABLED,
        /*
        * Selectable state.
        */
        DLMS_CREDIT_STATUS_SELECTABLE,
        /*
        * Selected/Invoked state.
        */
        DLMS_CREDIT_STATUS_INVOKED,
        /*
        * In use state.
        */
        DLMS_CREDIT_STATUS_IN_USE,
        /*
        * Consumed state.
        */
        DLMS_CREDIT_STATUS_CONSUMED
    }DLMS_CREDIT_STATUS;

    /*
     * Enumerated Credit configuration values.
     * Online help:
     * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
     */
    typedef enum
    {
        /*
        * None.
        */
        DLMS_CREDIT_CONFIGURATION_NONE = 0x0,
        /*
        * Requires visual indication
        */
        DLMS_CREDIT_CONFIGURATION_VISUAL = 0x1,
        /*
        * Requires confirmation before it can be selected/invoked
        */
        DLMS_CREDIT_CONFIGURATION_CONFIRMATION = 0x2,
        /*
        * Requires the credit amount to be paid back.
        */
        DLMS_CREDIT_CONFIGURATION_PAID_BACK = 0x4,
        /*
        * Resettable.
        */
        DLMS_CREDIT_CONFIGURATION_RESETTABLE = 0x8,
        /*
        * Able to receive credit amounts from tokens.
        */
        DLMS_CREDIT_CONFIGURATION_TOKENS = 0x10
    }DLMS_CREDIT_CONFIGURATION;

    /*
    * Enumerates account credit status modes.
    * Online help:
    * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCharge
    */
    typedef enum
    {
        /*
        * Consumption based collection.
        */
        DLMS_CHARGE_TYPE_CONSUMPTION_BASED_COLLECTION = 0,
        /*
        * Time based collection.
        */
        DLMS_CHARGE_TYPE_TIME_BASED_COLLECTION,
        /*
        * Payment based collection.
        */
        DLMS_CHARGE_TYPE_PAYMENT_EVENT_BASED_COLLECTION
    }DLMS_CHARGE_TYPE;

    // Defines the weekdays.
    typedef enum
    {
        /// No day of week is selected.
        DLMS_WEEKDAYS_NONE = 0,
        // Indicates Monday.
        DLMS_WEEKDAYS_MONDAY = 0x1,
        // Indicates Tuesday.
        DLMS_WEEKDAYS_TUESDAY = 0x2,
        // Indicates Wednesday.
        DLMS_WEEKDAYS_WEDNESDAY = 0x4,
        // Indicates Thursday.
        DLMS_WEEKDAYS_THURSDAY = 0x8,
        // Indicates Friday.
        DLMS_WEEKDAYS_FRIDAY = 0x10,
        // Indicates Saturday.
        DLMS_WEEKDAYS_SATURDAY = 0x20,
        // Indicates Sunday.
        DLMS_WEEKDAYS_SUNDAY = 0x40
    }DLMS_WEEKDAYS;

    // Charge configuration enumeration types.
    typedef enum
    {
        //NONE
        DLMS_CHARGE_CONFIGURATION_NONE = 0,
        // Percentage based collection.
        DLMS_CHARGE_CONFIGURATION_PERCENTAGE_BASED_COLLECTION = 0x1,
        // Continuous collection.
        DLMS_CHARGE_CONFIGURATION_CONTINUOUS_COLLECTION = 0x2
    }DLMS_CHARGE_CONFIGURATION;

    // DLMS state errors.
    typedef enum
    {
        //Invalid value.
        DLMS_EXCEPTION_STATE_ERROR_SERVICE_INVALID = 0,
        //Service is not allowed.
        DLMS_EXCEPTION_STATE_ERROR_SERVICE_NOT_ALLOWED = 1,
        //Unknown service.
        DLMS_EXCEPTION_STATE_ERROR_SERVICE_UNKNOWN = 2
    }DLMS_EXCEPTION_STATE_ERROR;


    //MIB variable initiator electrical phase.
    typedef enum
    {
        /**
        * Not defined.
        */
        DLMS_INITIATOR_ELECTRICAL_PHASE_NOT_DEFINED,
        /**
        * Phase 1.
        */
        DLMS_INITIATOR_ELECTRICAL_PHASE_1,
        /**
        * Phase 2.
        */
        DLMS_INITIATOR_ELECTRICAL_PHASE_2,
        /**
        * Phase 3.
        */
        DLMS_INITIATOR_ELECTRICAL_PHASE_3
    }DLMS_INITIATOR_ELECTRICAL_PHASE;

    /**
    * MIB variable initiator electrical phase.
    */
    typedef enum
    {
        /**
        * Not defined.
        */
        DLMS_DELTA_ELECTRICAL_PHASE_NOT_DEFINED,
        /**
        * The server system is connected to the same phase as the client system.
        */
        DLMS_DELTA_ELECTRICAL_PHASE_SAME,

        DLMS_DELTA_ELECTRICAL_PHASE_DEGREES_60,

        DLMS_DELTA_ELECTRICAL_PHASE_DEGREES_120,

        DLMS_DELTA_ELECTRICAL_PHASE_DEGREES_180,

        DLMS_DELTA_ELECTRICAL_PHASE_DEGREES_MINUS_120,

        DLMS_DELTA_ELECTRICAL_PHASE_DEGREES_MINUS_60
    }DLMS_DELTA_ELECTRICAL_PHASE;


    /**
    * Repeater enumerator values.
    */
    typedef enum
    {
        /**
        * Newer repeater.
        */
        DLMS_REPEATER_NEVER,
        /**
        * Always repeater.
        */
        DLMS_REPEATER_ALWAYS,
        /**
        * Dynamic repeater.
        */
        DLMS_REPEATER_DYNAMIC
    }DLMS_REPEATER;


    /**
    * IEC Twisted pair setup working mode.
    */
    typedef enum
    {
        /**
        * The interface ignores all received frames.
        */
        DLMS_IEC_TWISTED_PAIR_SETUP_MODE_INACTIVE,

        /**
        * Always active.
        */
        DLMS_IEC_TWISTED_PAIR_SETUP_MODE_ACTIVE
    }DLMS_IEC_TWISTED_PAIR_SETUP_MODE;

    /**
    * PLC Source address enumerations.
    */
    typedef enum
    {
        DLMS_PLC_SOURCE_ADDRESS_INITIATOR = 0xC00,
        DLMS_PLC_SOURCE_ADDRESS_NEW = 0xFFE
    }DLMS_PLC_SOURCE_ADDRESS;

    // PLC Destination address enumerations.
    typedef enum
    {
        DLMS_PLC_DESTINATION_ADDRESS_ALL_PHYSICAL = 0xFFF
    }DLMS_PLC_DESTINATION_ADDRESS;

    /**
    * PLC HDLC Source address enumerations.
    */
    typedef enum
    {
        // Initiator.
        DLMS_PLC_HDLC_SOURCE_ADDRESS_INITIATOR = 0xC01,
    }DLMS_PLC_HDLC_SOURCE_ADDRESS;

    /**
    * PLC data link data commands.
    * */
    typedef enum
    {
        DLMS_PLC_DATA_LINK_DATA_REQUEST = 0x90
    }DLMS_PLC_DATA_LINK_DATA;

    //  Sequence number of MAC sub frame.
    typedef enum
    {
        DLMS_PLC_MAC_SUB_FRAMES_ONE = 0x6C6C,
        DLMS_PLC_MAC_SUB_FRAMES_TWO = 0x3A3A,
        DLMS_PLC_MAC_SUB_FRAMES_THREE = 0x5656,
        DLMS_PLC_MAC_SUB_FRAMES_FOUR = 0x7171,
        DLMS_PLC_MAC_SUB_FRAMES_FIVE = 0x1D1D,
        DLMS_PLC_MAC_SUB_FRAMES_SIX = 0x4B4B,
        DLMS_PLC_MAC_SUB_FRAMES_SEVEN = 0x2727,
    }DLMS_PLC_MAC_SUB_FRAMES;

    // M-Bus command.
    typedef enum
    {
        DLMS_MBUS_COMMAND_SND_NR = 0x44,
        DLMS_MBUS_COMMAND_SND_UD2 = 0x43,
        DLMS_MBUS_COMMAND_RSP_UD = 0x08
    } DLMS_MBUS_COMMAND;

    // M-Bus meter type.
    typedef enum
    {
        // Other meter.
        DLMS_MBUS_METER_TYPE_OTHER = 0,
        // Oil meter.
        DLMS_MBUS_METER_TYPE_OIL = 1,
        // Energy meter.
        DLMS_MBUS_METER_TYPE_ENERGY = 2,
        // Gas meter.
        DLMS_MBUS_METER_TYPE_GAS = 3,
        // Heat meter.
        DLMS_MBUS_METER_TYPE_HEAT = 4,
        // Steam meter.
        DLMS_MBUS_METER_TYPE_STEAM = 5,
        // Hot water meter.
        DLMS_MBUS_METER_TYPE_HOT_WATER = 6,
        // Water meter.
        DLMS_MBUS_METER_TYPE_WATER = 7,
        // Heat cost allocator meter.
        DLMS_MBUS_METER_TYPE_HEAT_COST_ALLOCATOR = 8,
        // Gas mode 2 meter.
        DLMS_MBUS_METER_TYPE_GAS_MODE2 = 10,
        // Heat mode 2 meter.
        DLMS_MBUS_METER_TYPE_HEAT_MODE2 = 11,
        /// Hot water mode 2 meter.
        DLMS_MBUS_METER_TYPE_HOT_WATER_MODE2 = 12,
        /// Water mode 2 meter.
        DLMS_MBUS_METER_TYPE_WATER_MODE2 = 13,
        // Heat cost allocator mode 2 meter.
        DLMS_MBUS_METER_TYPE_HEAT_COST_ALLOCATOR_MODE2 = 14,
        // Unknown meter type.
        DLMS_MBUS_METER_TYPE_UNKNOWN = 0x0F
    }DLMS_MBUS_METER_TYPE;

    // Defines the M-Bus port communication status.
    typedef enum
    {
        // No access.
        DLMS_MBUS_PORT_COMMUNICATION_STATE_NO_ACCESS = 0,
        // Temporary no access
        DLMS_MBUS_PORT_COMMUNICATION_STATE_TEMPORARY_NO_ACCESS,
        // Limited access
        DLMS_MBUS_PORT_COMMUNICATION_STATE_LIMITED_ACCESS,
        // Unlimited access.
        DLMS_MBUS_PORT_COMMUNICATION_STATE_UNLIMITED_ACCESS,
        // wM-Bus.
        DLMS_MBUS_PORT_COMMUNICATION_STATE_WMBUS,
    }DLMS_MBUS_PORT_COMMUNICATION_STATE;

    // Defines the M-Bus data header types.
    typedef enum
    {
        // M-Bus data header is not used.
        DLMS_MBUS_DATA_HEADER_TYPE_NONE = 0,
        // Short data header is used.
        DLMS_MBUS_DATA_HEADER_TYPE_SHORT,
        // Long data header is used.
        DLMS_MBUS_DATA_HEADER_TYPE_LONG
    }DLMS_MBUS_DATA_HEADER_TYPE;

    // M-Bus control info.
    typedef enum
    {
        // Long M-Bus data header present, direction master to slave
        DLMS_MBUS_CONTROL_INFO_LONG_HEADER_MASTER = 0x60,
        // Short M-Bus data header present, direction master to slave
        DLMS_MBUS_CONTROL_INFO_SHORT_HEADER_MASTER = 0x61,
        // Long M-Bus data header present, direction slave to master
        DLMS_MBUS_CONTROL_INFO_LONG_HEADER_SLAVE = 0x7C,
        // Short M-Bus data header present, direction slave to master
        DLMS_MBUS_CONTROL_INFO_SHORT_HEADER_SLAVE = 0x7D,
        // M-Bus short Header.
        DLMS_MBUS_CONTROL_INFO_SHORT_HEADER = 0x7A,
        // M-Bus long Header.
        DLMS_MBUS_CONTROL_INFO_LONG_HEADER = 0x72
    }DLMS_MBUS_CONTROL_INFO;

    /*M-Bus link status.*/
    typedef enum
    {
        /*Data never received.*/
        DLMS_MBUS_LINK_STATUS_NONE,
        /* Normal operation.*/
        DLMS_MBUS_LINK_STATUS_NORMAL,
        /* Link temporarily interrupted.*/
        DLMS_MBUS_LINK_STATUS_TEMPORARILY_INTERRUPTED,
        /*Link permanently interrupted.*/
        DLMS_MBUS_LINK_STATUS_PERMANENTLY_INTERRUPTED,
    }DLMS_MBUS_LINK_STATUS;

    // Encryption modes.
    typedef enum
    {
        // Encryption is not used.
        DLMS_MBUS_ENCRYPTION_MODE_NONE,
        //  AES with Counter Mode (CTR) noPadding and IV.
        DLMS_MBUS_ENCRYPTION_MODE_AES_128,
        //  DES with Cipher Block Chaining Mode (CBC).
        DLMS_MBUS_ENCRYPTION_MODE_DES_CBC,
        //  DES with Cipher Block Chaining Mode (CBC) and Initial Vector.
        DLMS_MBUS_ENCRYPTION_MODE_DES_CBC_IV,
        // AES with Cipher Block Chaining Mode (CBC) and Initial Vector.
        DLMS_MBUS_ENCRYPTION_MODE_AES_CBC_IV = 5,
        // AES 128 with Cipher Block Chaining Mode (CBC) and dynamic key and Initial Vector with 0.
        DLMS_MBUS_ENCRYPTION_MODE_AES_CBC_IV_0 = 7,
        // TLS
        DLMS_MBUS_ENCRYPTION_MODE_TLS = 13
    }DLMS_MBUS_ENCRYPTION_MODE;

    // Enumerates modulation types.
    typedef enum
    {
        // Robust Mode.
        DLMS_G3_PLC_MODULATION_ROBUST_MODE,
        // DBPSK.
        DLMS_G3_PLC_MODULATION_DBPSK,
        //  DQPSK.
        DLMS_G3_PLC_MODULATION_DQPSK,
        // D8PSK.
        DLMS_G3_PLC_MODULATION_D8PSK,
        // 16-QAM.
        DLMS_G3_PLC_MODULATION_QAM16
    } DLMS_G3_PLC_MODULATION;

    // Enumerates gain resolution steps.
    typedef enum
    {
        // Step is 6 dB.
        DLMS_G3_PLC_GAIN_RESOLUTION_DB6,
        // Step is 3 dB.
        DLMS_G3_PLC_GAIN_RESOLUTION_DB3
    }DLMS_G3_PLC_GAIN_RESOLUTION;

    // Enumerates the type of the device connected to the modem.
    typedef enum
    {
        // PAN device.
        DLMS_PAN_DEVICE_TYPE_DEVICE,
        //  PAN coordinator.
        DLMS_PAN_DEVICE_TYPE_COORDINATOR,
        // Not Defined.
        DLMS_PAN_DEVICE_TYPE_NOT_DEFINED
    }DLMS_PAN_DEVICE_TYPE;


    // Defines the ZigBee status enumeration values.
    typedef enum
    {
        /// <summary>
        /// Authorised on PAN.
        /// </summary>
        DLMS_ZIG_BEE_STATUS_AUTHORISED = 0x1,
        /// <summary>
        /// Actively reporting on PAN.
        /// </summary>
        DLMS_ZIG_BEE_STATUS_REPORTING = 0x2,
        /// <summary>
        /// Unauthorised on PAN but has reported
        /// </summary>
        DLMS_ZIG_BEE_STATUS_UNAUTHORISED = 0x4,
        /// <summary>
        /// Authorised after swap-out.
        /// </summary>
        DLMS_ZIG_BEE_STATUS_AUTHORISED_SWAP_OUT = 0x8,
        /// <summary>
        /// SEP Transmitting.
        /// </summary>
        DLMS_ZIG_BEE_STATUS_SEP_TRANSMITTING = 0x10
    }DLMS_ZIG_BEE_STATUS;

    /*Encoding of selective access parameters with data_index.*/
    typedef enum
    {
        /*Selective access is not used.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_NONE = 0x0,
        /*Last number of entries.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_ENTRIES_LAST = 0x1,
        /*Number of entries after last confirmation.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_ENTRIES_LAST_CONFIRMATION = 0x2,
        /*Complete number of minutes after last confirmation.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_MINUTES_LAST_CONFIRMATION = 0x3,
        /*Complete number of hours after last confirmation.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_HOURS_LAST_CONFIRMATION = 0x4,
        /*Complete number of days after last confirmation.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_DAYS_LAST_CONFIRMATION = 0x5,
        /*Complete number of months after last confirmation.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_MONTHS_LAST_CONFIRMATION = 0x6,
        /*Last complete number of minutes including the current minute.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_MINUTES = 0x7,
        /*Last complete number of hours including the current hour.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_HOURS = 0x8,
        /*Last complete number of days including the current day.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_DAYS = 0x9,
        /*Last complete number of months including the current month.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_MONTHS = 0xA,
        /*Last number of seconds.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_SECONDS = 0xB,
        /*Last complete number of minutes.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_MINUTES = 0xC,
        /*Last complete number of hours.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_HOURS = 0xD,
        /*Last complete number of days.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_DAYS = 0xE,
        /*Last complete number of months.*/
        DLMS_SELECTIVE_ACCESS_PARAMETER_COMPLETE_MONTHS = 0xF
    }DLMS_SELECTIVE_ACCESS_PARAMETER;

    /*Defines NTP authentication methods.*/
    typedef enum
    {
        /*No security is used.*/
        DLMS_NTP_AUTHENTICATION_METHOD_NO_SECURITY = 0,
        /*Shared secrets are used.*/
        DLMS_NTP_AUTHENTICATION_METHOD_SHARED_SECRETS,
        /*IFF auto key is used.*/
        DLMS_NTP_AUTHENTICATION_METHOD_AUTOKEY_IFF
    }
    DLMS_NTP_AUTHENTICATION_METHOD;

    /*IPv6 Address type to add.*/
    typedef enum
    {
        // Unicast address.
        DLMS_IPV6_ADDRESS_TYPE_UNICAST,
        // Multicast address.
        DLMS_IPV6_ADDRESS_TYPE_MULTICAST,
        // Gateway address.
        DLMS_IPV6_ADDRESS_TYPE_GATEWAY
    }
    DLMS_IPV6_ADDRESS_TYPE;
#ifdef  __cplusplus
}
#endif

#endif //ENUMS_H
