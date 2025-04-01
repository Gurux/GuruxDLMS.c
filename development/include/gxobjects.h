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

#ifndef GXOBJECTS_H
#define GXOBJECTS_H

#include "gxignore.h"
#include "gxint.h"
#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <ws2tcpip.h>
#else //Linux includes.
#ifndef DLMS_IGNORE_IP6_SETUP
#if __has_include(<netinet/in.h>)
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#else
typedef struct in6_addr {
    union {
        unsigned char Byte[16];
        uint16_t      Word[8];
    } u;
} IN6_ADDR, * PIN6_ADDR;
#endif //__STDC_VERSION__ >= 199901L
#define IN6_ADDR struct in6_addr

#endif //DLMS_IGNORE_IP6_SETUP
#endif
#include "enums.h"
#include "variant.h"
#include "date.h"
#include "gxarray.h"
#include "gxkey.h"
#include "gxdefine.h"

#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "privateKey.h"
#include "gx509Certificate.h"
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#ifdef  __cplusplus
extern "C" {
#endif
    typedef enum
    {
        /*
         * Write attribute.
         */
        DLMS_SCRIPT_ACTION_TYPE_WRITE = 1,
        /*
         * Execute specific method
        */
        DLMS_SCRIPT_ACTION_TYPE_EXECUTE = 2
    } DLMS_SCRIPT_ACTION_TYPE;

    typedef enum
    {
        /*
         First in first out
         When circle buffer is full first item is removed.
        */
        DLMS_SORT_METHOD_FIFO = 1,
        /*
         Last in first out.
         When circle buffer is full last item is removed.
        */
        DLMS_SORT_METHOD_LIFO = 2,
        /*
         Largest is first.
        */
        DLMS_SORT_METHOD_LARGEST = 3,
        /*
         Smallest is first.
        */
        DLMS_SORT_METHOD_SMALLEST = 4,
        /*
         Nearst to zero is first.
        */
        DLMS_SORT_METHOD_NEAREST_TO_ZERO = 5,
        /*
         Farest from zero is first.
        */
        DLMS_SORT_METHOD_FAREST_FROM_ZERO = 6
    } DLMS_SORT_METHOD;

    typedef enum
    {
        /// Not defined
        DLMS_CLOCK_BASE_NONE,
        /// Internal Crystal
        DLMS_CLOCK_BASE_CRYSTAL,
        /// Mains frequency 50 Hz,
        DLMS_CLOCK_BASE_FREQUENCY_50,
        /// Mains frequency 60 Hz,
        DLMS_CLOCK_BASE_FREQUENCY_60,
        /// Global Positioning System.
        DLMS_CLOCK_BASE_GPS,
        /// Radio controlled.
        DLMS_CLOCK_BASE_RADIO
    } DLMS_CLOCK_BASE;

    typedef enum
    {
        /*
         * The device never connects.
         */
        DLMS_AUTO_CONNECT_MODE_NO_AUTO_CONNECT = 0,
        /*
         * Auto dialling allowed anytime,
        */
        DLMS_AUTO_CONNECT_MODE_AUTO_DIALLING_ALLOWED_ANYTIME = 1,
        /*
         * Auto dialling allowed within the validity time of the calling window.
        */
        DLMS_AUTO_CONNECT_MODE_AUTO_DIALLING_ALLOWED_CALLING_WINDOW = 2,
        /*
         * Regular auto dialling allowed within the validity time
         * of the calling window; alarm initiated auto dialling allowed anytime,
        */
        DLMS_AUTO_CONNECT_MODE_REGULAR_AUTO_DIALLING_ALLOWED_CALLING_WINDOW = 3,
        /*
         * SMS sending via Public Land Mobile Network (PLMN),
        */
        DLMS_AUTO_CONNECT_MODE_SMS_SENDING_PLMN = 4,
        /*
         * SMS sending via PSTN.
        */
        DLMS_AUTO_CONNECT_MODE_SMS_SENDING_PSTN = 5,
        /*
         * Email sending.
        */
        DLMS_AUTO_CONNECT_MODE_EMAIL_SENDING = 6,
        /*
        * The device is permanently connected to the communication network.
        */
        DLMS_AUTO_CONNECT_MODE_PERMANENTLY_CONNECT = 101,
        /*
        * The device is permanently connected to the communication network. No
        * connection possible outside the calling window.
        */
        DLMS_AUTO_CONNECT_MODE_CONNECT_WITH_CALLING_WINDOW = 102,
        /*
        * The device is permanently connected to the communication network.
        * Connection is possible as soon as the connect method is invoked.
        */
        DLMS_AUTO_CONNECT_MODE_CONNECT_INVOKED = 103,
        /*
        * The device is usually disconnected. It connects to the communication
        * network as soon as the connect method is invoked
        */
        DLMS_AUTO_CONNECT_MODE_DISCONNECT_CONNECT_INVOKED = 104,
        /*
        * (200..255) manufacturer specific modes
        */
        DLMS_AUTO_CONNECT_MODE_MANUFACTURE_SPESIFIC = 200
    } DLMS_AUTO_CONNECT_MODE;

    typedef enum
    {
        DLMS_AUTO_ANSWER_STATUS_INACTIVE = 0,
        DLMS_AUTO_ANSWER_STATUS_ACTIVE = 1,
        DLMS_AUTO_ANSWER_STATUS_LOCKED = 2
    } DLMS_AUTO_ANSWER_STATUS;

    typedef enum
    {
        // Line dedicated to the device.
        DLMS_AUTO_ANSWER_MODE_DEVICE = 0,
        // Shared line management with a limited number of calls allowed. Once the number of calls is reached,
        // the window status becomes inactive until the next start date, whatever the result of the call,
        DLMS_AUTO_ANSWER_MODE_CALL = 1,
        // Shared line management with a limited number of successful calls allowed. Once the number of
        //// successful communications is reached, the window status becomes inactive until the next start date,
        DLMS_AUTO_ANSWER_MODE_CONNECTED = 2,
        // Currently no modem connected.
        DLMS_AUTO_ANSWER_MODE_NONE = 3
    } DLMS_AUTO_ANSWER_MODE;

    /*
     Defines the protocol used by the meter on the port.
    */
    typedef enum
    {
        /*
         Protocol according to IEC 62056-21 (modes A�E),
        */
        DLMS_OPTICAL_PROTOCOL_MODE_DEFAULT = 0,
        /*
         Protocol according to IEC 62056-46.
         Using this enumeration value all other attributes of this IC are not applicable.
        */
        DLMS_OPTICAL_PROTOCOL_MODE_NET = 1,
        /*
         Protocol not specified. Using this enumeration value,
         ProposedBaudrate is used for setting the communication speed on the port.
         All other attributes are not applicable.
        */
        DLMS_OPTICAL_PROTOCOL_MODE_UNKNOWN = 2
    } DLMS_OPTICAL_PROTOCOL_MODE;

    typedef enum
    {
        /*
         Minimium time is 20 ms.
        */
        DLMS_LOCAL_PORT_RESPONSE_TIME_20_MS = 0,
        /*
         Minimium time is 200 ms.
        */
        DLMS_LOCAL_PORT_RESPONSE_TIME_200_MS = 1
    } DLMS_LOCAL_PORT_RESPONSE_TIME;

    /*
     *
     * Defines the baud rates.
     */
    typedef enum
    {
        /*
        Baudrate is 300.
        */
        DLMS_BAUD_RATE_300 = 0,
        /*
         Baudrate is 600.
        */
        DLMS_BAUD_RATE_600,
        /*
         Baudrate is 1200.
        */
        DLMS_BAUD_RATE_1200,
        /*
         Baudrate is 2400.
        */
        DLMS_BAUD_RATE_2400,
        /*
         Baudrate is 4800.
        */
        DLMS_BAUD_RATE_4800,
        /*
         Baudrate is 9600.
        */
        DLMS_BAUD_RATE_9600,
        /*
         Baudrate is 19200.
        */
        DLMS_BAUD_RATE_19200,
        /*
         Baudrate is 38400.
        */
        DLMS_BAUD_RATE_38400,
        /*
         Baudrate is 57600.
        */
        DLMS_BAUD_RATE_57600,
        /*
         Baudrate is 115200.
        */
        DLMS_BAUD_RATE_115200
    } DLMS_BAUD_RATE;

    typedef enum
    {
        /*
         * The output_state is set to false and the consumer is disconnected.
         */
        DLMS_CONTROL_STATE_DISCONNECTED = 0,
        /*
         * The output_state is set to 1 and the consumer is connected.
        */
        DLMS_CONTROL_STATE_CONNECTED,
        /*
         * The output_state is set to false and the consumer is disconnected.
        */
        DLMS_CONTROL_STATE_READY_FOR_RECONNECTION
    } DLMS_CONTROL_STATE;

    /*
     * Configures the behaviour of the disconnect control object for all
    triggers, i.e. the possible state transitions.
     */
    typedef enum
    {
        /*
         * The disconnect control object is always in 'connected' state,
        */
        DLMS_CONTROL_MODE_NONE = 0,
        /*
         * Disconnection: Remote (b, c), manual (f), local (g)
         * Reconnection: Remote (d), manual (e).
        */
        DLMS_CONTROL_MODE_MODE_1,
        /*
         * Disconnection: Remote (b, c), manual (f), local (g)
         * Reconnection: Remote (a), manual (e).
        */
        DLMS_CONTROL_MODE_MODE_2,
        /*
         * Disconnection: Remote (b, c), manual (-), local (g)
         * Reconnection: Remote (d), manual (e).
        */
        DLMS_CONTROL_MODE_MODE_3,
        /*
         * Disconnection: Remote (b, c), manual (-), local (g)
         * Reconnection: Remote (a), manual (e)
        */
        DLMS_CONTROL_MODE_MODE_4,
        /*
         * Disconnection: Remote (b, c), manual (f), local (g)
         * Reconnection: Remote (d), manual (e), local (h),
        */
        DLMS_CONTROL_MODE_MODE_5,
        /*
         * Disconnection: Remote (b, c), manual (-), local (g)
         * Reconnection: Remote (d), manual (e), local (h)
        */
        DLMS_CONTROL_MODE_MODE_6,
        /*
         * Disconnection: Remote(b, c), manual(-), local(g)
         * Reconnection: Remote (a, i), manual (e), local (h)
         */
        DLMS_CONTROL_MODE_MODE_7,
    } DLMS_CONTROL_MODE;

    /*
    PPP Authentication Type
     */
    typedef enum
    {
        /*
         No authentication.
        */
        DLMS_PPP_AUTHENTICATION_TYPE_NONE = 0,
        /*
         PAP Login
        */
        DLMS_PPP_AUTHENTICATION_TYPE_PAP = 1,
        /*
         CHAP-algorithm
        */
        DLMS_PPP_AUTHENTICATION_TYPE_CHAP = 2
    } DLMS_PPP_AUTHENTICATION_TYPE;

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_DESTINATION_LENGTH];
        uint16_t size;
    }gxDestination;
#endif //DLMS_IGNORE_MALLOC

    //Access modes are saved structure because they are not needed on server side and we want to save memory.
    /*
    ---------------------------------------------------------------------------
    */
    typedef struct
    {
        gxByteBuffer attributeAccessModes;
        gxByteBuffer methodAccessModes;
    }gxAccess;

    typedef struct
    {
        uint16_t objectType;
        unsigned char version;
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        uint16_t shortName;
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME
        unsigned char logicalName[6];
        gxAccess* access;
    } gxObject;

    //Returns object's logical name.
    const unsigned char* obj_getLogicalName(gxObject* target);

    typedef struct
    {
        gxObject* key;
        unsigned char value;
    } gxListItem;

    typedef struct
    {
        gxObject** data;
        uint16_t capacity;
        uint16_t size;
#if !(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
        uint16_t position;
#endif //!(defined(GX_DLMS_MICROCONTROLLER) || defined(DLMS_IGNORE_MALLOC))
    } objectArray;

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSData
#ifndef DLMS_IGNORE_DATA
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        dlmsVARIANT value;
    } gxData;
#endif //DLMS_IGNORE_DATA

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSRegister
#ifndef DLMS_IGNORE_REGISTER
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        dlmsVARIANT value;
        signed char scaler;
        unsigned char unit;
        unsigned char unitRead;
    } gxRegister;
#endif // DLMS_IGNORE_REGISTER

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSExtendedRegister
#ifndef DLMS_IGNORE_EXTENDED_REGISTER
    typedef struct
    {
        gxObject base;
        dlmsVARIANT value;
        signed char scaler;
        unsigned char unit;
        gxtime captureTime;
        dlmsVARIANT status;
    } gxExtendedRegister;
#endif //DLMS_IGNORE_EXTENDED_REGISTER

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSDemandRegister
#ifndef DLMS_IGNORE_DEMAND_REGISTER
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        dlmsVARIANT currentAverageValue;
        dlmsVARIANT lastAverageValue;
        signed char scaler;
        unsigned char unit;
        unsigned char unitRead;
        dlmsVARIANT status;
        gxtime captureTime;
        gxtime startTimeCurrent;
        uint16_t numberOfPeriods;
        uint32_t period;
    } gxDemandRegister;
#endif // DLMS_IGNORE_DEMAND_REGISTER

#ifdef DLMS_IGNORE_OBJECT_POINTERS
    typedef struct
    {
        uint16_t objectType;
        unsigned char logicalName[6];
    } gxObjectDefinition;
#endif //DLMS_IGNORE_OBJECT_POINTERS

#ifndef DLMS_IGNORE_REGISTER_ACTIVATION
    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        //Name length.
        unsigned char length;
        //Name of the mask.
        unsigned char name[MAX_REGISTER_ACTIVATION_MASK_NAME_LENGTH];
        //Index count
        unsigned char count;
        //Mask index list.
        unsigned char indexes[9];
#else
        gxByteBuffer name;
        gxByteBuffer indexes;
#endif //DLMS_IGNORE_MALLOC
    } gxRegisterActivationMask;

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSRegisterActivation
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
        objectArray registerAssignment;
#else
        gxArray registerAssignment;
#endif //DLMS_IGNORE_OBJECT_POINTERS
        gxArray maskList;
        gxByteBuffer activeMask;
    } gxRegisterActivation;
#endif //DLMS_IGNORE_REGISTER_ACTIVATION

#if !(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA) && defined(DLMS_IGNORE_PUSH_SETUP) && defined(DLMS_IGNORE_PARAMETER_MONITOR))
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSProfileGeneric
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCompactData
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSParameterMonitor
    typedef struct
    {
        signed char attributeIndex;
        uint16_t dataIndex;
#ifdef DLMS_IGNORE_OBJECT_POINTERS
        unsigned char logicalName[6];
        uint16_t objectType;
#else
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxObject* target;
#endif //defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#endif //DLMS_IGNORE_OBJECT_POINTERS
    } gxTarget;


    //---------------------------------------------------------------------------
    //deprecated: User gxTarget instead of gxCaptureObject.
    typedef struct
    {
        char attributeIndex;
        uint16_t dataIndex;
    } gxCaptureObject;

#endif //!(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA) && defined(DLMS_IGNORE_PUSH_SETUP) && defined(DLMS_IGNORE_PARAMETER_MONITOR))

#ifndef DLMS_IGNORE_PROFILE_GENERIC
#ifndef DLMS_IGNORE_MALLOC
    //Create capture object with given attribute and data indexes.
    gxTarget* co_init(unsigned char attributeIndex, unsigned char dataIndex);
#endif //DLMS_IGNORE_MALLOC

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSProfileGeneric
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

#ifndef DLMS_IGNORE_MALLOC
        //There is no buffer is malloc is not used.
        //Reason for this is that we don't know the structure of the data and where example octect string is saved.
        //User must do this manually!
        gxArray buffer;
#endif //DLMS_IGNORE_MALLOC
        gxArray captureObjects;
        uint32_t capturePeriod;
        DLMS_SORT_METHOD sortMethod;
        gxObject* sortObject;
        uint32_t profileEntries;
        uint32_t entriesInUse;
        signed char sortObjectAttributeIndex;
        uint16_t sortObjectDataIndex;
        //Executed time. This is for internal use.
        uint32_t executedTime;
    } gxProfileGeneric;
#endif //DLMS_IGNORE_PROFILE_GENERIC

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSClock
#ifndef DLMS_IGNORE_CLOCK
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        DLMS_CLOCK_BASE clockBase : 8;
        signed char deviation;
        unsigned char enabled;
        gxtime end;
        DLMS_CLOCK_STATUS status : 8;
        gxtime begin;
        int16_t timeZone;
        gxtime time;
        gxtime presetTime;
    } gxClock;

#ifndef DLMS_IGNORE_CLOCK
    //Update Daylight Saving time flag if DST is used.
    void clock_updateDST(
        gxClock* object,
        gxtime* value);

    //Convert UTC date time to meter date time.
    int clock_utcToMeterTime(
        gxClock* object,
        gxtime* value);
#endif // DLMS_IGNORE_CLOCK

#endif //DLMS_IGNORE_CLOCK

#ifndef DLMS_IGNORE_SCRIPT_TABLE
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSScriptTable
    typedef struct
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* target;
#else
        uint16_t objectType;
        unsigned char logicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        DLMS_SCRIPT_ACTION_TYPE type;
        dlmsVARIANT parameter;
        signed char index;
    } gxScriptAction;

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSScriptTable
    typedef struct
    {
        //Script ID.
        uint16_t id;
        //Script actions.
        gxArray actions;
    } gxScript;

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSScriptTable
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray scripts;
    } gxScriptTable;
#endif //DLMS_IGNORE_SCRIPT_TABLE

#ifndef DLMS_IGNORE_SCHEDULE
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSchedule
    */
    typedef struct
    {
        /*
        * Schedule entry index.
        */
        uint16_t index;

        /*
        * Is Schedule entry enabled.
        */
        unsigned char enable;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxScriptTable* scriptTable;
#else
        /*
        * Logical name of the Script table object.
        */
        unsigned char logicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS

        /*
        * Script identifier of the script to be executed.
        */
        uint16_t scriptSelector;

        /*
        *
        */
        gxtime switchTime;

        /*
        * Defines a period in minutes, in which an entry shall be processed after power fail.
        */
        uint16_t validityWindow;

        /*
        * BitArray days of the week on which the entry is valid.
        */
        DLMS_WEEKDAYS execWeekdays : 7;

        /*
        * Perform the link to the IC Special days table, day_id.
        */
        bitArray execSpecDays;

        /*
        * Date starting period in which the entry is valid.
        */
        gxtime beginDate;

        /*
        * Date starting period in which the entry is valid.
        */
        gxtime endDate;
    }
    gxScheduleEntry;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSchedule
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*
        * Specifies the scripts to be executed at given times.
        */
        gxArray entries; //gxScheduleEntry
    } gxSchedule;
#endif //DLMS_IGNORE_SCHEDULE

#ifndef DLMS_IGNORE_SPECIAL_DAYS_TABLE
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSpecialDaysTable
    */
    typedef struct
    {
        uint16_t index;
        gxtime date;
        unsigned char dayId;
    } gxSpecialDay;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSpecialDaysTable
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray entries;//gxSpecialDay[]
    } gxSpecialDaysTable;
#endif //DLMS_IGNORE_SPECIAL_DAYS_TABLE

#ifndef DLMS_IGNORE_SECURITY_SETUP
    typedef struct
    {
#ifndef DLMS_IGNORE_CLIENT
        // Used certificate entity.
        DLMS_CERTIFICATE_ENTITY entity;

        // Used certificate type.
        DLMS_CERTIFICATE_TYPE type;

#ifdef DLMS_IGNORE_MALLOC
        // Certificate serial number.
        char serialNumber[MAX_CERTIFICATE_SERIAL_NUMBER_LENGTH];

        // Certificate issuer.
        char issuer[MAX_CERTIFICATE_ISSUER_LENGTH];

        // Certificate subject.
        char subject[MAX_CERTIFICATE_SUBJECT_LENGTH];
        // Certificate subject alt name.
        char subjectAltName[MAX_CERTIFICATE_SUBJECT_ALT_LENGTH];
#else
        // Certificate serial number.
        char* serialNumber;

        // Certificate issuer.
        char* issuer;

        // Certificate subject.
        char* subject;
        // Certificate subject alt name.
        char* subjectAltName;
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_IGNORE_CLIENT
#ifndef DLMS_IGNORE_SERVER
        //Server side sertificate. 
        gxByteBuffer cert;
#ifdef DLMS_IGNORE_MALLOC
        unsigned char CERTIFICATE_BUFFER[DLMS_X509_CETRIFICATE_MAX_SIZE];
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_IGNORE_SERVER
    }gxCertificateInfo;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSecuritySetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        DLMS_SECURITY_POLICY securityPolicy;
        DLMS_SECURITY_SUITE securitySuite;
        gxByteBuffer serverSystemTitle;
        gxByteBuffer clientSystemTitle;
        gxArray certificates;
        //Minimum client invocation counter value. Server can use this.
#ifdef DLMS_COSEM_INVOCATION_COUNTER_SIZE64
        uint64_t minimumInvocationCounter;
#else
        uint32_t minimumInvocationCounter;
#endif //DLMS_COSEM_INVOCATION_COUNTER_SIZE64
        //Block cipher key.
#ifdef DLMS_IGNORE_MALLOC
        unsigned char guek[32];
#else
        gxByteBuffer guek;
#endif //DLMS_IGNORE_MALLOC
        //Broadcast block cipher key.
#ifdef DLMS_IGNORE_MALLOC
        unsigned char gbek[32];
#else
        gxByteBuffer gbek;
#endif //DLMS_IGNORE_MALLOC

        //Authentication key.
#ifdef DLMS_IGNORE_MALLOC
        unsigned char gak[32];
#else
        gxByteBuffer gak;
#endif //DLMS_IGNORE_MALLOC

#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
        //Signing key of the server.
        gxPrivateKey signingKey;

        //Key agreement key of the server.
        gxPrivateKey keyAgreementKey;

        //TLS pair of the server.
        gxPrivateKey tlsKey;

#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
    } gxSecuritySetup;
#endif //DLMS_IGNORE_SECURITY_SETUP

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        //User ID.
        unsigned char id;
        //User name
        char name[MAX_USER_NAME_LENGTH];
    }gxUser;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAssociationShortName
    */
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        objectArray objectList;
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        gxSecuritySetup* securitySetup;
#else
        unsigned char securitySetupReference[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        gxByteBuffer secret;
        /*
        * User list.
        */
        gxArray userList;
        /*
        * Current user.
        */
#ifdef DLMS_IGNORE_MALLOC
        gxUser currentUser;
#else
        gxKey2 currentUser;
#endif //DLMS_IGNORE_MALLOC
    } gxAssociationShortName;
#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifndef DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAssociationLogicalName
    */
    typedef struct
    {
        unsigned char logicalName[6];
        unsigned char jointIsoCtt;
        unsigned char country;
        uint16_t countryName;
        unsigned char identifiedOrganization;
        unsigned char dlmsUA;
        unsigned char applicationContext;
        unsigned char contextId;
    } gxApplicationContextName;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAssociationLogicalName
    */
    typedef struct
    {
        DLMS_CONFORMANCE conformance;
        uint16_t maxReceivePduSize;
        uint16_t maxSendPduSize;
        unsigned char dlmsVersionNumber;
        signed char qualityOfService;
        gxByteBuffer cypheringInfo;
    } gxXDLMSContextType;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAssociationLogicalName
    */
    typedef struct
    {
        unsigned char jointIsoCtt;
        unsigned char country;
        uint16_t countryName;
        unsigned char identifiedOrganization;
        unsigned char dlmsUA;
        unsigned char authenticationMechanismName;
        DLMS_AUTHENTICATION mechanismId;
    } gxAuthenticationMechanismName;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAssociationLogicalName
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        DLMS_ASSOCIATION_STATUS associationStatus;
        objectArray objectList;
        signed char clientSAP;
        uint16_t serverSAP;
        gxApplicationContextName applicationContextName;
        gxXDLMSContextType xDLMSContextInfo;
        gxAuthenticationMechanismName authenticationMechanismName;
        gxByteBuffer secret;
#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        gxSecuritySetup* securitySetup;
#else
        unsigned char securitySetupReference[6];
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_SECURITY_SETUP))
        /*
        * User list.
        */
        gxArray userList;
        /*
        * Current user.
        */
#ifdef DLMS_IGNORE_MALLOC
        gxUser currentUser;
#else
        gxKey2 currentUser;
#endif //DLMS_IGNORE_MALLOC
    } gxAssociationLogicalName;

#endif //DLMS_IGNORE_ASSOCIATION_LOGICAL_NAME

#ifndef DLMS_IGNORE_SAP_ASSIGNMENT

    typedef struct
    {
        unsigned char value[MAX_SAP_ITEM_NAME_LENGTH];
        uint16_t size;
    }gxSapItemName;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSapAssignment
    */
    typedef struct
    {
        uint16_t id;
#ifdef DLMS_IGNORE_MALLOC
        gxSapItemName name;
#else
        gxByteBuffer name;
#endif //DLMS_IGNORE_MALLOC
    } gxSapItem;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSapAssignment
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray sapAssignmentList;//List of gxSapItems.
    } gxSapAssignment;

#endif //DLMS_IGNORE_SAP_ASSIGNMENT

#ifndef DLMS_IGNORE_IMAGE_TRANSFER

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char data[MAX_IMAGE_IDENTIFICATION_LENGTH];
        uint16_t size;
    }gxImageIdentification;

    typedef struct
    {
        unsigned char data[MAX_IMAGE_SIGNATURE_LENGTH];
        uint16_t size;
    }gxImageSignature;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSImageTransfer
    */
    typedef struct
    {
        uint32_t size;

#ifdef DLMS_IGNORE_MALLOC
        gxImageIdentification identification;
        gxImageSignature signature;
#else
        gxByteBuffer identification;
        gxByteBuffer signature;
#endif //DLMS_IGNORE_MALLOC
    } gxImageActivateInfo;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSImageTransfer
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        uint32_t imageBlockSize;
        bitArray imageTransferredBlocksStatus;
        uint32_t imageFirstNotTransferredBlockNumber;
        unsigned char imageTransferEnabled;
        DLMS_IMAGE_TRANSFER_STATUS imageTransferStatus;
        gxArray imageActivateInfo; // gxImageActivateInfo
    } gxImageTransfer;
#endif //DLMS_IGNORE_IMAGE_TRANSFER

#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIECLocalPortSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxByteBuffer password1;
        gxByteBuffer password2;
        gxByteBuffer password5;
        DLMS_OPTICAL_PROTOCOL_MODE defaultMode;
        DLMS_BAUD_RATE defaultBaudrate;
        DLMS_BAUD_RATE proposedBaudrate;
        DLMS_LOCAL_PORT_RESPONSE_TIME responseTime;
        gxByteBuffer deviceAddress;
    } gxLocalPortSetup;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_ACTIVITY_CALENDAR

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_SEASON_PROFILE_NAME_LENGTH];
        uint16_t size;
    }gxSeasonProfileName;
#endif //DLMS_IGNORE_MALLOC

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_SEASON_PROFILE_WEEK_NAME_LENGTH];
        uint16_t size;
    }gxSeasonProfileWeekName;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSActivityCalendar
    */
    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        gxSeasonProfileName name;
#else
        gxByteBuffer name;
#endif //DLMS_IGNORE_MALLOC
        gxtime start;
#ifdef DLMS_IGNORE_MALLOC
        gxSeasonProfileWeekName weekName;
#else
        gxByteBuffer weekName;
#endif //DLMS_IGNORE_MALLOC
    } gxSeasonProfile;

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_SEASON_WEEK_PROFILE_NAME_LENGTH];
        uint16_t size;
    }gxWeekProfileName;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSActivityCalendar
    */
    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        gxWeekProfileName name;
#else
        gxByteBuffer name;
#endif //DLMS_IGNORE_MALLOC
        unsigned char monday;
        unsigned char tuesday;
        unsigned char wednesday;
        unsigned char thursday;
        unsigned char friday;
        unsigned char saturday;
        unsigned char sunday;
    } gxWeekProfile;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSActivityCalendar
    */
    typedef struct
    {
        gxtime startTime;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* script;
#else
        unsigned char scriptLogicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t scriptSelector;
    } gxDayProfileAction;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSActivityCalendar
    */
    typedef struct
    {
        unsigned char dayId;
        gxArray daySchedules; //gxDayProfileAction
    } gxDayProfile;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSActivityCalendar
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        gxByteBuffer calendarNameActive;
        gxByteBuffer calendarNamePassive;
        gxArray seasonProfileActive; // gxSeasonProfile
        gxArray weekProfileTableActive;// gxWeekProfile
        gxArray dayProfileTableActive;// gxDayProfile
        gxArray seasonProfilePassive;// gxSeasonProfile
        gxArray dayProfileTablePassive;// gxDayProfile
        gxArray weekProfileTablePassive;// gxWeekProfile
        //Activate passive calendar time.
        gxtime time;
        //Executed time. This is for internal use.
        uint32_t executedTime;
    } gxActivityCalendar;

#endif // DLMS_IGNORE_ACTIVITY_CALENDAR

#if !(defined(DLMS_IGNORE_LIMITER) && defined(DLMS_IGNORE_REGISTER_MONITOR))
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSRegisterMonitor
    */
    typedef struct
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxScriptTable* script;
#else
        unsigned char logicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t scriptSelector;
    } gxActionItem;

#endif //!(defined(DLMS_IGNORE_LIMITER) && defined(DLMS_IGNORE_REGISTER_MONITOR))

#ifndef DLMS_IGNORE_REGISTER_MONITOR
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSRegisterMonitor
    */
    typedef struct
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* target;
#else
        uint16_t objectType;
        unsigned char logicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        signed char attributeIndex;
    } gxMonitoredValue;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSRegisterMonitor
    */
    typedef struct
    {
        gxActionItem actionUp;
        gxActionItem actionDown;
    } gxActionSet;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSRegisterMonitor
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray actions; //gxActionSet
        gxMonitoredValue monitoredValue;
        //List of threshold values.
        variantArray thresholds; //dlmsVARIANT
        //List of last values. This is needed so script is not invoked several times.
        variantArray lastValues;
    } gxRegisterMonitor;

#endif //DLMS_IGNORE_REGISTER_MONITOR

#ifndef DLMS_IGNORE_ACTION_SCHEDULE
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSActionSchedule
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxScriptTable* executedScript;
#else
        unsigned char executedScriptLogicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint16_t executedScriptSelector;
        DLMS_SINGLE_ACTION_SCHEDULE_TYPE type;
        gxArray executionTime; //gxTime
        //Executed time. This is for internal use.
        uint32_t executedTime;
    } gxActionSchedule;

#endif //DLMS_IGNORE_ACTION_SCHEDULE

#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIecHdlcSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        uint16_t inactivityTimeout;
        uint16_t deviceAddress;
        uint16_t maximumInfoLengthTransmit;
        DLMS_BAUD_RATE communicationSpeed;
        unsigned char windowSizeTransmit;
        unsigned char windowSizeReceive;
        uint16_t interCharachterTimeout;
        uint16_t maximumInfoLengthReceive;
    } gxIecHdlcSetup;

#endif //DLMS_IGNORE_IEC_HDLC_SETUP

#ifndef DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIecTwistedPairSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*
         * Working mode.
         */
        DLMS_IEC_TWISTED_PAIR_SETUP_MODE mode;

        /*
         * Communication speed.
         */
        DLMS_BAUD_RATE speed;

        /*
         * List of Primary Station Addresses.
         */
        gxByteBuffer primaryAddresses;

        /*
         * List of the TAB(i) for which the real equipment has been programmed in
         * the case of forgotten station call.
         */
        gxByteBuffer tabis;
    } gxIecTwistedPairSetup;
#endif //DLMS_IGNORE_IEC_TWISTED_PAIR_SETUP

#ifndef DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSMbusSlavePortSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        DLMS_BAUD_RATE defaultBaud;
        DLMS_BAUD_RATE availableBaud;
        DLMS_ADDRESS_STATE addressState;
        unsigned char busAddress;
    } gxMbusSlavePortSetup;

#endif //DLMS_IGNORE_MBUS_SLAVE_PORT_SETUP


#ifndef DLMS_IGNORE_MBUS_DIAGNOSTIC
    /*Last changed value.*/
    typedef struct
    {
        /*Attribute ID.*/
        unsigned char attributeId;
        gxtime timeStamp;
    }gxCaptureTime;

    typedef struct
    {
        /*Client ID.*/
        unsigned char clientId;
        /*Counter.*/
        uint32_t counter;
        /*Time stamp.*/
        gxtime timeStamp;
    }gxBroadcastFrameCounter;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSMbusDiagnostic
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /*Received signal strength in dBm.*/
        unsigned char receivedSignalStrength;
        /*Currently used channel ID.*/
        unsigned char channelId;
        /*Link status.*/
        DLMS_MBUS_LINK_STATUS linkStatus;
        /*Broadcast frame counters*/
        gxArray broadcastFrames;
        /*Transmitted frames.*/
        uint32_t transmissions;
        /*Received frames with a correct checksum.*/
        uint32_t receivedFrames;
        /*Received frames with a incorrect checksum.*/
        uint32_t failedReceivedFrames;
        /* Last time when receivedSignalStrength, linkStatus, transmissions,
        receivedFrames or failedReceivedFrames was changed.*/
        gxCaptureTime captureTime;
    } gxMbusDiagnostic;
#endif //DLMS_IGNORE_MBUS_DIAGNOSTIC

#ifndef DLMS_IGNORE_MBUS_PORT_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSMBusPortSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /*References an M-Bus communication port setup object describing
        the physical capabilities for wired or wireless communication.*/
        unsigned char profileSelection[6];
        /*Communication status of the M-Bus node.*/
        DLMS_MBUS_PORT_COMMUNICATION_STATE portCommunicationStatus;
        /*M-Bus data header type.*/
        DLMS_MBUS_DATA_HEADER_TYPE dataHeaderType;
        /*he primary address of the M-Bus slave device.*/
        unsigned char primaryAddress;
        /*Identification Number element of the data header.*/
        uint32_t identificationNumber;
        /*Manufacturer Identification element.*/
        uint16_t manufacturerId;
        /*M-Bus version.*/
        unsigned char mBusVersion;
        /*Device type.*/
        DLMS_MBUS_METER_TYPE deviceType;
        uint16_t maxPduSize;
        gxArray listeningWindow;
    } gxMBusPortSetup;
#endif //DLMS_IGNORE_MBUS_PORT_SETUP

#ifndef DLMS_IGNORE_UTILITY_TABLES
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSUtilityTables
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        uint16_t tableId;
        gxByteBuffer buffer;
    } gxUtilityTables;
#endif //DLMS_IGNORE_UTILITY_TABLES

#ifndef DLMS_IGNORE_MODEM_CONFIGURATION

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_MODEM_INITIALIZATION_STRING_LENGTH];
        uint16_t size;
    }gxModemInitialisationString;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSModemConfiguration
    */
    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        gxModemInitialisationString request;
        gxModemInitialisationString response;
#else
        gxByteBuffer request;
        gxByteBuffer response;
#endif //DLMS_IGNORE_MALLOC
        uint16_t delay;
    } gxModemInitialisation;

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_MODEM_PROFILE_STRING_LENGTH];
        uint16_t size;
    }gxModemProfile;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSModemConfiguration
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray initialisationStrings; //gxModemInitialisation
        gxArray modemProfile; // gxByteBuffer
        DLMS_BAUD_RATE communicationSpeed;
    } gxModemConfiguration;

#endif //DLMS_IGNORE_MODEM_CONFIGURATION

    typedef struct
    {
        gxtime first;
        gxtime second;
    }gxTimePair;

#ifndef DLMS_IGNORE_AUTO_ANSWER
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAutoAnswer
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        unsigned char numberOfRingsInListeningWindow;
        unsigned char numberOfRingsOutListeningWindow;
        DLMS_AUTO_ANSWER_MODE mode;
        gxArray listeningWindow;
        DLMS_AUTO_ANSWER_STATUS status;
        unsigned char numberOfCalls;
    } gxAutoAnswer;

#endif //DLMS_IGNORE_AUTO_ANSWER

#ifndef DLMS_IGNORE_AUTO_CONNECT
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAutoConnect
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        DLMS_AUTO_CONNECT_MODE mode;
        gxArray callingWindow;
        //Array of destination strings.
        gxArray destinations;
        uint16_t repetitionDelay;
        unsigned char repetitions;
        //Executed time. This is for internal use.
        uint32_t executedTime;
    } gxAutoConnect;
#endif //DLMS_IGNORE_AUTO_CONNECT

#ifndef DLMS_IGNORE_TCP_UDP_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTcpUdpSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        uint16_t port;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* ipSetup;
#else
        //Logical name.
        unsigned char ipReference[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        unsigned char maximumSimultaneousConnections;
        uint16_t inactivityTimeout;
        uint16_t maximumSegmentSize;
    } gxTcpUdpSetup;

#endif //DLMS_IGNORE_TCP_UDP_SETUP

#ifndef DLMS_IGNORE_IP4_SETUP
#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_IP4_SETUP_IP_OPTION_DATA_LENGTH];
        uint16_t size;
    }gxip4SetupIpOptionData;
#endif //DLMS_IGNORE_MALLOC
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSip4SetupIpOption
    */
    typedef struct
    {
        DLMS_IP_OPTION_TYPE type;
        unsigned char length;
#ifdef DLMS_IGNORE_MALLOC
        gxip4SetupIpOptionData data;
#else
        gxByteBuffer data;
#endif //DLMS_IGNORE_MALLOC
    } gxip4SetupIpOption;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIp4Setup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* dataLinkLayer;
#else
        unsigned char dataLinkLayerReference[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        uint32_t ipAddress;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxArray multicastIPAddress;
#else
        variantArray multicastIPAddress;
#endif //DLMS_IGNORE_MALLOC
        gxArray ipOptions; //gxip4SetupIpOption
        uint32_t subnetMask;
        uint32_t gatewayIPAddress;
        unsigned char useDHCP;
        uint32_t primaryDNSAddress;
        uint32_t secondaryDNSAddress;
        dlmsVARIANT value;
    } gxIp4Setup;
#endif //DLMS_IGNORE_IP4_SETUP

#ifndef DLMS_IGNORE_IP6_SETUP

    // Enumerated Address config modes.
    typedef enum {
        // Auto Configuration.
        DLMS_IP6_ADDRESS_CONFIG_MODE_AUTO,
        // DHCP v6.
        DLMS_IP6_ADDRESS_CONFIG_MODE_DHCP_V6,
        // Manual
        DLMS_IP6_ADDRESS_CONFIG_MODE_MANUAL,
        // Neighbour Discovery.
        DLMS_IP6_ADDRESS_CONFIG_MODE_NEIGHBOUR_DISCOVERY
    }DLMS_IP6_ADDRESS_CONFIG_MODE;

    typedef struct
    {
        /**
        * Gives the maximum number of router solicitation retries to be performed
        * by a node if the expected router advertisement has not been received.
        */
        unsigned char maxRetry;
        /**
        * Gives the waiting time in milliseconds between two successive router
        * solicitation retries.
        */
        uint16_t retryWaitTime;
        /**
        * Gives the router advertisement transmission period in seconds.
        */
        uint32_t sendPeriod;
    }gxNeighborDiscoverySetup;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIp6Setup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* dataLinkLayer;
#else
        unsigned char dataLinkLayerReference[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        DLMS_IP6_ADDRESS_CONFIG_MODE addressConfigMode;
        gxArray unicastIPAddress; //List<IN6_ADDR>
        gxArray multicastIPAddress; //List<IN6_ADDR>
        gxArray gatewayIPAddress; //List<IN6_ADDR>
        IN6_ADDR primaryDNSAddress;
        IN6_ADDR secondaryDNSAddress;
        unsigned char trafficClass;
        gxArray neighborDiscoverySetup;//List<gxNeighborDiscoverySetup>
    } gxIp6Setup;
#endif //DLMS_IGNORE_IP6_SETUP

#ifndef DLMS_IGNORE_MAC_ADDRESS_SETUP

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSMacAddressSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxByteBuffer macAddress;
    } gxMacAddressSetup;
#endif //DLMS_IGNORE_MAC_ADDRESS_SETUP

#ifndef DLMS_IGNORE_PPP_SETUP
    typedef enum
    {
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_MAX_REC_UNIT = 1,
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_ASYNC_CONTROL_CHAR_MAP = 2,
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_AUTH_PROTOCOL = 3,
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_MAGIC_NUMBER = 5,
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_PROTOCOL_FIELD_COMPRESSION = 7,
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_ADDRESS_AND_CTR_COMPRESSION = 8,
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_FCS_ALTERNATIVES = 9,
        DLMS_PPP_SETUP_LCP_OPTION_TYPE_CALLBACK = 13
    } DLMS_PPP_SETUP_LCP_OPTION_TYPE;

    typedef enum
    {
        DLMS_PPP_SETUP_IPCP_OPTION_TYPE_IPCOMPRESSIONPROTOCOL = 2,
        DLMS_PPP_SETUP_IPCP_OPTION_TYPE_PREFLOCALIP = 3,
        DLMS_PPP_SETUP_IPCP_OPTION_TYPE_PREFPEERIP = 20,
        DLMS_PPP_SETUP_IPCP_OPTION_TYPE_GAO = 21,
        DLMS_PPP_SETUP_IPCP_OPTION_TYPE_USIP = 22
    } DLMS_PPP_SETUP_IPCP_OPTION_TYPE;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPppSetup
    */
    typedef struct
    {
        DLMS_PPP_SETUP_LCP_OPTION_TYPE type;
        unsigned char length;
        dlmsVARIANT data;
    } gxpppSetupLcpOption;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPppSetup
    */
    typedef struct
    {
        DLMS_PPP_SETUP_IPCP_OPTION_TYPE type;
        unsigned char length;
        dlmsVARIANT data;
    } gxpppSetupIPCPOption;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPppSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray ipcpOptions; // gxpppSetupIPCPOption
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* phy;
#else
        unsigned char PHYReference[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        gxArray lcpOptions; //CGXDLMSPppSetupLcpOption
        gxByteBuffer userName;
        gxByteBuffer password;
        DLMS_PPP_AUTHENTICATION_TYPE authentication;
    } gxPppSetup;
#endif //DLMS_IGNORE_PPP_SETUP

#ifndef DLMS_IGNORE_GPRS_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSGPRSSetup
    */
    typedef struct
    {
        unsigned char precedence;
        unsigned char delay;
        unsigned char reliability;
        unsigned char peakThroughput;
        unsigned char meanThroughput;
    } gxQualityOfService;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSGPRSSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxByteBuffer apn;
        uint16_t pinCode;
        gxQualityOfService defaultQualityOfService;
        gxQualityOfService requestedQualityOfService;
    } gxGPRSSetup;
#endif //DLMS_IGNORE_GPRS_SETUP

#ifndef DLMS_IGNORE_SMTP_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSmtpSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
    } gxSmtpSetup;
#endif //DLMS_IGNORE_SMTP_SETUP

#ifndef DLMS_IGNORE_GSM_DIAGNOSTIC
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSGsmDiagnostic
    */
    typedef struct
    {
        /*
        * Cell ID.
        */
        uint32_t cellId;

        /*
        * Location area code (LAC).
        */
        uint16_t locationId;

        /*
        * Signal quality.
        */
        unsigned char signalQuality;
        /*
        * Bit Error Rate.
        */
        unsigned char ber;
        /*
        * Mobile Country Code.
        */
        uint16_t mobileCountryCode;

        /*
        * Mobile Network Code.
        */
        uint16_t mobileNetworkCode;

        /*
        * Absolute radio frequency channel number.
        */
        uint32_t channelNumber;
    }gxGSMCellInfo;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSGsmDiagnostic
    */
    typedef struct
    {
        /*
        * Four-byte cell ID.
        */
        uint32_t cellId;

        /*
        * Signal quality.
        */
        unsigned char signalQuality;
    }gxAdjacentCell;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSGsmDiagnostic
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /*
        * Name of network operator.
        */
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxByteBuffer operatorName;
#else
        char* operatorName;
#endif //DLMS_IGNORE_MALLOC

        /*
        * Registration status of the modem.
        */
        DLMS_GSM_STATUS status;

        /*
        * Registration status of the modem.
        */
        DLMS_GSM_CIRCUIT_SWITCH_STATUS circuitSwitchStatus;

        /*
        * Registration status of the modem.
        */
        DLMS_GSM_PACKET_SWITCH_STATUS packetSwitchStatus;

        /*
        * Registration status of the modem.
        */
        gxGSMCellInfo cellInfo;

        /*
        * Adjacent cells.
        */
        gxArray adjacentCells; //<GXAdjacentCell*>

        /*
        * Date and time when the data have been last captured.
        */
        gxtime captureTime;
    } gxGsmDiagnostic;
#endif //DLMS_IGNORE_GSM_DIAGNOSTIC

#ifndef DLMS_IGNORE_REGISTER_TABLE
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSRegisterTable
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        variantArray tableCellValues;
        gxArray tableCellDefinition;
        signed char scaler;
        unsigned char unit;
        unsigned char unitRead;
    } gxRegisterTable;
#endif //DLMS_IGNORE_REGISTER_TABLE

#ifndef DLMS_IGNORE_STATUS_MAPPING
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSStatusMapping
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        //TODO:
    } gxStatusMapping;
#endif //DLMS_IGNORE_STATUS_MAPPING

#ifndef DLMS_IGNORE_DISCONNECT_CONTROL
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSDisconnectControl
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        unsigned char outputState;
        DLMS_CONTROL_STATE controlState;
        DLMS_CONTROL_MODE controlMode;
    } gxDisconnectControl;
#endif //DLMS_IGNORE_DISCONNECT_CONTROL

#ifndef DLMS_IGNORE_LIMITER
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSLimiter
    */
    typedef struct
    {
        uint16_t id;
        gxtime activationTime;
        uint32_t duration;
    } gxEmergencyProfile;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSLimiter
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        gxObject* monitoredValue;
        //Selected attribute index of monitored value.
        signed char selectedAttributeIndex;

        dlmsVARIANT thresholdActive;
        dlmsVARIANT thresholdNormal;
        dlmsVARIANT thresholdEmergency;
        uint32_t minOverThresholdDuration;
        uint32_t minUnderThresholdDuration;
        gxEmergencyProfile emergencyProfile;
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxArray emergencyProfileGroupIDs;
#else
        variantArray emergencyProfileGroupIDs;
#endif //DLMS_IGNORE_MALLOC
        unsigned char emergencyProfileActive;
        gxActionItem actionOverThreshold;
        gxActionItem actionUnderThreshold;
        /*Threshold activation time in seconds.
        This is internally used and it's not serialized.*/
        uint32_t activationTime;
        /*Is threshold over or under.
        This is internally used and it's not serialized.*/
        unsigned char overThreshold;
    } gxLimiter;
#endif //DLMS_IGNORE_LIMITER

#ifndef DLMS_IGNORE_MBUS_CLIENT

    typedef enum
    {
        DLMS_MBUS_ENCRYPTION_KEY_STATUS_NO_ENCRYPTION_KEY = 0,
        DLMS_MBUS_ENCRYPTION_KEY_STATUS_ENCRYPTION_KEY_SET,
        DLMS_MBUS_ENCRYPTION_KEY_STATUS_ENCRYPTION_KEY_TRANSFERRED,
        DLMS_MBUS_ENCRYPTION_KEY_STATUS_ENCRYPTION_KEY_SET_AND_TRANSFERRED,
        DLMS_MBUS_ENCRYPTION_KEY_STATUS_ENCRYPTION_KEY_INUSE
    }DLMS_MBUS_ENCRYPTION_KEY_STATUS;

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_CAPTURE_DEFINITION_ELEMENT_LENGTH];
        uint16_t size;
    }gxCaptureDefinitionElement;

    typedef struct
    {
        gxCaptureDefinitionElement data;
        gxCaptureDefinitionElement value;
    }gxCaptureDefinition;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSMBusClient
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        uint32_t capturePeriod;
        unsigned char primaryAddress;
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* mBusPort;
#else
        unsigned char mBusPortReference[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        gxArray captureDefinition;
        uint32_t identificationNumber;
        uint16_t manufacturerID;
        unsigned char dataHeaderVersion;
        unsigned char deviceType;
        unsigned char accessNumber;
        unsigned char status;
        unsigned char alarm;
        uint16_t configuration;
        DLMS_MBUS_ENCRYPTION_KEY_STATUS encryptionKeyStatus;
    } gxMBusClient;
#endif //DLMS_IGNORE_MBUS_CLIENT

    typedef enum
    {
        DLMS_PROTECTION_TYPE_AUTHENTICATION = 1,
        DLMS_PROTECTION_TYPE_ENCRYPTION = 2,
        DLMS_PROTECTION_TYPE_AUTHENTICATION_ENCRYPTION = 3
    } DLMS_PROTECTION_TYPE;

#ifndef DLMS_IGNORE_PUSH_SETUP
    /*This structure is used to count repetition delay for the next push message.*/
    typedef struct
    {
        /**
        * The minimum delay until a next push attempt is started in seconds.
        */
        uint16_t min;

        /**
         * Calculating the next delay.
         */
        uint16_t exponent;

        /**
         * The maximum delay until a next push attempt is started in seconds.
         */
        uint16_t max;

    }gxRepetitionDelay;

    /*Data protection identified key.*/
    typedef struct
    {
        /*Data protection key type.*/
        DLMS_DATA_PROTECTION_IDENTIFIED_KEY_TYPE keyType;
    }gxDataProtectionIdentifiedKey;

    /*Data protection wrapped key.*/
    typedef struct
    {
        /*Data protectionKey type.*/
        DLMS_DATA_PROTECTION_WRAPPED_KEY_TYPE keyType;

        /*Key ciphered data.*/
        gxByteBuffer key;
    }gxDataProtectionWrappeddKey;

    /*Data protection agreed key.*/
    typedef struct
    {
        /*Key parameters.*/
        gxByteBuffer parameters;

        /*Key ciphered data.*/
        gxByteBuffer data;
    }gxDataProtectionAgreedKey;

    /*Data protection Key.*/
    typedef struct
    {
        /*Data protectionKey type.*/
        DLMS_DATA_PROTECTION_KEY_TYPE dataProtectionKeyType;

        /*Identified key parameters.*/
        gxDataProtectionIdentifiedKey identifiedKey;

        /*Wrapped key parameters.*/
        gxDataProtectionWrappeddKey wrappedKey;
        /*Agreed key parameters.*/
        gxDataProtectionAgreedKey agreedKey;
    }gxDataProtectionKey;

    typedef struct
    {
        /*Protection type.*/
        DLMS_PROTECTION_TYPE protectionType;

        /**
         * Transaction Id.
         */
        gxByteBuffer transactionId;

        /**
         * Originator system title.
         */
        unsigned char originatorSystemTitle[8];

        /**
         * Recipient system title.
         */
        unsigned char recipientSystemTitle[8];

        /**
         * Other information.
         */
        gxByteBuffer otherInformation;

        /**
         * Key info.
         */
        gxDataProtectionKey keyInfo;
    }gxPushProtectionParameters;

    /*Push confirmation parameters.*/
    typedef struct
    {
        /**
     * Confirmation start date. Fields of date-time not specified are not used.
     */
        gxtime startDate;
        /**
         * Confirmation time interval in seconds. Disabled, if zero.
         */
        uint32_t interval;
    }gxPushConfirmationParameter;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPushSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray pushObjectList;

        DLMS_SERVICE_TYPE service;

        //Destination.
        gxByteBuffer destination;

        DLMS_MESSAGE_TYPE message;

        gxArray communicationWindow;
        uint16_t randomisationStartInterval;
        unsigned char numberOfRetries;
        /* Repetition delay for version #0 and #1.*/
        uint16_t repetitionDelay;
        /* Repetition delay for Version #2.*/
        gxRepetitionDelay repetitionDelay2;
        /*The logical name of a communication port setup object.*/
#ifdef DLMS_IGNORE_MALLOC
        unsigned char portReference[6];
#else
        gxObject* portReference;
#endif //DLMS_IGNORE_MALLOC
        /*Push client SAP.*/
        signed char pushClientSAP;
        /*Push protection parameters.*/
        gxArray pushProtectionParameters;
        /*Push operation method.*/
        DLMS_PUSH_OPERATION_METHOD pushOperationMethod;
        gxPushConfirmationParameter confirmationParameters;
        /*Last confirmation date time.*/
        gxtime lastConfirmationDateTime;
        //Executed time. This is for internal use.
        uint32_t executedTime;
    } gxPushSetup;
#endif //DLMS_IGNORE_PUSH_SETUP

#ifndef DLMS_IGNORE_PARAMETER_MONITOR

    typedef struct
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* target;
#else
        DLMS_OBJECT_TYPE type;
        //Logical name.
        unsigned char logicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        signed char attributeIndex;
        dlmsVARIANT value;
    } gxChangedParameter;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSParameterMonitor
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        //Changed parameter.
        gxChangedParameter changedParameter;

        // Capture time.
        gxtime captureTime;

        // Changed Parameter
        gxArray parameters;
    } gxParameterMonitor;
#endif //DLMS_IGNORE_PARAMETER_MONITOR

#ifndef DLMS_IGNORE_WIRELESS_MODE_Q_CHANNEL
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSWirelessModeQChannel
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
    } gxWirelessModeQChannel;

#endif //DLMS_IGNORE_WIRELESS_MODE_Q_CHANNEL

#ifndef DLMS_IGNORE_MBUS_MASTER_PORT_SETUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSMBusMasterPortSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        DLMS_BAUD_RATE commSpeed;
    } gxMBusMasterPortSetup;

#endif //DLMS_IGNORE_MBUS_MASTER_PORT_SETUP

    //Global key types.
    typedef enum
    {
        /*
        * Global unicast encryption key. <br>
        * Client and server uses this message to send Ephemeral Public Key to other
        * party.
        */
        DLMS_GLOBAL_KEY_TYPE_UNICAST_ENCRYPTION,
        /*
         * Global broadcast encryption key.
        */
        DLMS_GLOBAL_KEY_TYPE_BROADCAST_ENCRYPTION,
        /*
         * Authentication key.
        */
        DLMS_GLOBAL_KEY_TYPE_AUTHENTICATION,
        /*
         * Key Encrypting Key, also known as Master key.
        */
        DLMS_GLOBAL_KEY_TYPE_KEK
    }DLMS_GLOBAL_KEY_TYPE;

#ifndef DLMS_IGNORE_DATA_PROTECTION
    typedef enum
    {
        //Used with identified_key_info_options.
        DLMS_KEY_INFO_TYPE_IDENTIFIED_KEY = 0,
        //Used with wrapped_key_info_options.
        DLMS_KEY_INFO_TYPE_WRAPPED_KEY = 1,
        //Used with agreed_key_info_options.
        DLMS_KEY_INFO_TYPE_AGREED_KEY = 2
    } DLMS_KEY_INFO_TYPE;

    typedef enum
    {
        DLMS_IDENTIFIED_KEY_INFO_OPTIONS_GLOBAL_UNICAST_ENCRYPTION_KEY = 0,
        DLMS_IDENTIFIED_KEY_INFO_OPTIONS_GLOBAL_BROADCAST_ENCRYPTION_KEY = 1
    } DLMS_IDENTIFIED_KEY_INFO_OPTIONS;

    typedef enum
    {
        DLMS_RESTRICTION_TYPE_NONE = 0,
        //Restriction by date.
        DLMS_RESTRICTION_TYPE_DATE = 1,
        //Restriction by entry.
        DLMS_RESTRICTION_TYPE_ENTRY = 2
    } DLMS_RESTRICTION_TYPE;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMS
    */
    typedef struct
    {
        unsigned char id;
        //Key ciphered data.
        gxByteBuffer data;
    } gxWrappedKeyInfoOptions;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMS
    */
    typedef struct
    {
        gxByteBuffer parameters;
        gxByteBuffer cipheredData;
    }
    gxAgreedKeyInfoOptions;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMS
    */
    typedef struct
    {
        DLMS_KEY_INFO_TYPE type;
        DLMS_IDENTIFIED_KEY_INFO_OPTIONS identifiedOptions;
        gxWrappedKeyInfoOptions wrappedKeyoptions;
        gxAgreedKeyInfoOptions agreedOptions;
    } gxKeyinfo;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMS
    */
    typedef struct
    {
        //Transaction id.
        gxByteBuffer id;
        //Originator_system_title
        gxByteBuffer originator;
        //Recipient System title.
        gxByteBuffer recipient;
        //Other information
        gxByteBuffer information;
        gxKeyinfo info;
    } gxProtectionOptions;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMS
    */
    typedef struct
    {
        DLMS_PROTECTION_TYPE type;
        gxProtectionOptions options;
    } gxProtectionParameter;

#endif //DLMS_IGNORE_DATA_PROTECTION

#ifndef DLMS_IGNORE_EVENT
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSEvent
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
    } gxEvent;
#endif //DLMS_IGNORE_EVENT

#ifndef DLMS_IGNORE_ZIG_BEE_SAS_STARTUP
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSZigBeeSasStartup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
    } gxZigBeeSasStartup;

#endif //DLMS_IGNORE_ZIG_BEE_SAS_STARTUP

#ifndef DLMS_IGNORE_ZIG_BEE_SAS_JOIN
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSZigBeeSasJoin
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
    } gxZigBeeSasJoin;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_JOIN

#ifndef DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSZigBeeSasApsFragmentation
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
    } gxZigBeeSasApsFragmentation;
#endif //DLMS_IGNORE_ZIG_BEE_SAS_APS_FRAGMENTATION

#ifndef DLMS_IGNORE_DATA_PROTECTION
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSDataProtection
    */
    typedef struct
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* target;
#else
        DLMS_OBJECT_TYPE type;
        //Logical name.
        unsigned char logicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        //attribute index.
        int attributeIndex;
        //data index
        int dataIndex;
    }gxDataProtectionObject;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSDataProtection
    */
    typedef struct
    {
        gxObject base;
        gxByteBuffer protection_buffer;
        objectArray protectionObjectList;
        gxArray protectionParametersGet; //gxProtectionParameter.
        gxArray protectionParametersSet; //gxProtectionParameter.
        DLMS_REQUIRED_PROTECTION requiredProtection;
    } gxDataProtection;
#endif //DLMS_IGNORE_DATA_PROTECTION

#ifndef DLMS_IGNORE_ACCOUNT
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccountModeAndStatus
    */
    typedef struct
    {
        //CREDIT_MODE = 1, PREPAYMENT_MODE = 2
        unsigned char paymentMode;
        // NEW_INACTIVE_ACCOUNT = 1, ACCOUNT_ACTIVE = 2, ACCOUNT_CLOSED = 3
        unsigned char accountStatus;
    }gxAccountModeAndStatus;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCreditChargeConfiguration
    */
    typedef struct
    {
        unsigned char creditReference[6];
        unsigned char chargeReference[6];
        DLMS_CREDIT_COLLECTION_CONFIGURATION collectionConfiguration;
    }gxCreditChargeConfiguration;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGatewayConfiguration
    */
    typedef struct
    {
        unsigned char creditReference[6];
        unsigned char tokenProportion;
    }gxTokenGatewayConfiguration;

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        char value[MAX_CURRENCY_NAME_LENGTH];
        uint16_t size;
    }gxCurrencyName;
#endif //DLMS_IGNORE_MALLOC

    /*
    Used currency.
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
    */
    typedef struct
    {
        // Currency name.
#ifdef DLMS_IGNORE_MALLOC
        gxCurrencyName name;
#else
        gxByteBuffer name;
#endif //DLMS_IGNORE_MALLOC
        // Currency scale.
        signed char scale;
        // Currency unit.
        DLMS_CURRENCY unit;
    }gxCurrency;

    /*
    * Online help:<br/>
    * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*
        * Payment mode.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        DLMS_ACCOUNT_PAYMENT_MODE paymentMode;

        /*
        * Account status.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        DLMS_ACCOUNT_STATUS accountStatus;

        /*
        * Credit In Use.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        unsigned char currentCreditInUse;

        /*
        * Credit status.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        DLMS_ACCOUNT_CREDIT_STATUS currentCreditStatus;

        /*
        * The available_credit attribute is the sum of the positive current credit
        * amount values in the instances of the Credit class. <br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        int32_t availableCredit;

        /*
        * Amount to clear.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        int32_t amountToClear;

        /*
        * Conjunction with the amount to clear, and is included in the description
        * of that attribute.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        int32_t clearanceThreshold;

        /*
        * Aggregated debt.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        int32_t aggregatedDebt;

        /*
        * Credit references.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        gxArray creditReferences;//List<String>

        /*
        * Charge references.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        gxArray chargeReferences;//List<String>
        /*
        * Credit charge configurations.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        gxArray creditChargeConfigurations;//List<GXCreditChargeConfiguration>

        /*
        * Token gateway configurations.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        gxArray tokenGatewayConfigurations;//List<GXTokenGatewayConfiguration>
        /*
        * Account activation time.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        gxtime accountActivationTime;

        /*
        * Account closure time.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        gxtime accountClosureTime;

        /*
        * Currency.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        gxCurrency currency;
        /*
        * Low credit threshold.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        int32_t lowCreditThreshold;
        /*
        * Next credit available threshold.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        int32_t nextCreditAvailableThreshold;
        /*
        * Max Provision.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        uint16_t maxProvision;

        /*
        * Max provision period.<br/>
        * Online help:<br/>
        * http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSAccount
        */
        int32_t maxProvisionPeriod;
    } gxAccount;
#endif //DLMS_IGNORE_ACCOUNT

#ifndef DLMS_IGNORE_CREDIT
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCredit
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        int32_t currentCreditAmount;
        DLMS_CREDIT_TYPE type;
        unsigned char priority;
        int32_t warningThreshold;
        int32_t limit;
        DLMS_CREDIT_CONFIGURATION creditConfiguration;
        unsigned char status;
        int32_t presetCreditAmount;
        int32_t creditAvailableThreshold;
        gxtime period;
    } gxCredit;

#endif //DLMS_IGNORE_CREDIT

#ifndef DLMS_IGNORE_CHARGE
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCharge
    */
    typedef struct
    {
        signed char commodityScale;
        signed char priceScale;
    } gxChargePerUnitScaling;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCharge
    */
    typedef struct
    {
#ifndef DLMS_IGNORE_OBJECT_POINTERS
        gxObject* target;
#else
        DLMS_OBJECT_TYPE type;
        unsigned char logicalName[6];
#endif //DLMS_IGNORE_OBJECT_POINTERS
        signed char attributeIndex;
    } gxCommodity;

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char data[MAX_CHARGE_TABLE_INDEX_LENGTH];
        uint16_t size;
    }gxChargeTableIndex;
#endif //DLMS_IGNORE_MALLOC

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCharge
    */
    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        gxChargeTableIndex index;
#else
        gxByteBuffer index;
#endif //DLMS_IGNORE_MALLOC
        int16_t chargePerUnit;
    } gxChargeTable;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCharge
    */
    typedef struct
    {
        gxChargePerUnitScaling chargePerUnitScaling;
        gxCommodity commodity;
        gxArray chargeTables;
    } gxUnitCharge;

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCharge
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        int32_t totalAmountPaid;

        DLMS_CHARGE_TYPE chargeType;
        unsigned char priority;
        gxUnitCharge unitChargeActive;
        gxUnitCharge unitChargePassive;
        gxtime unitChargeActivationTime;
        uint32_t period;
        DLMS_CHARGE_CONFIGURATION chargeConfiguration;
        gxtime lastCollectionTime;
        int32_t lastCollectionAmount;
        int32_t totalAmountRemaining;
        uint16_t proportion;
    } gxCharge;
#endif //DLMS_IGNORE_CHARGE

#ifndef DLMS_IGNORE_TOKEN_GATEWAY
#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        unsigned char value[MAX_TOKEN_GATEWAY_DESCRIPTION_LENGTH];
        uint16_t size;
    }gxTokenGatewayDescription;
#endif //DLMS_IGNORE_MALLOC

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        // Token.
        //  Online help:<br/>
        //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
        gxByteBuffer token;

        // Time.
        //  Online help:<br/>
        //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
        gxtime time;

        // Descriptions.
        //  Online help:<br/>
        //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
        gxArray descriptions;

        // Token Delivery method.
        //  Online help:<br/>
        //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
        DLMS_TOKEN_DELIVERY deliveryMethod;

        //  Token status code.
        //  Online help:<br/>
        //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
        DLMS_TOKEN_STATUS_CODE status;

        // Token data value.
        //  Online help:<br/>
        //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTokenGateway
        bitArray dataValue;
    } gxTokenGateway;
#endif //DLMS_IGNORE_TOKEN_GATEWAY

#ifndef DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSZigBeeNetworkControl
    */
    typedef struct
    {
        gxByteBuffer macAddress;
        DLMS_ZIG_BEE_STATUS status;
        char maxRSSI;
        char averageRSSI;
        char minRSSI;
        unsigned char maxLQI;
        unsigned char averageLQI;
        unsigned char minLQI;
        gxtime lastCommunicationDateTime;
        unsigned char numberOfHops;
        unsigned char transmissionFailures;
        unsigned char transmissionSuccesses;
        unsigned char applicationVersion;
        unsigned char stackVersion;
    } gxActiveDevice;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSZigBeeNetworkControl
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        unsigned char enableDisableJoining;
        uint16_t joinTimeout;
        gxArray activeDevices;
    } gxZigBeeNetworkControl;

#endif //DLMS_IGNORE_ZIG_BEE_NETWORK_CONTROL

#ifndef DLMS_IGNORE_LLC_SSCS_SETUP

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSLlcSscsSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /*Address assigned to the service node during its registration by the base node.*/
        uint16_t serviceNodeAddress;
        /* Base node address to which the service node is registered.*/
        uint16_t baseNodeAddress;

    } gxLlcSscsSetup;
#endif //DLMS_IGNORE_LLC_SSCS_SETUP

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPrimeNbOfdmPlcPhysicalLayerCounters
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*Number of bursts received on the physical layer for which the CRC was incorrect.*/
        uint16_t crcIncorrectCount;

        /* Number of bursts received on the physical layer for which the CRC was correct, but the Protocol field of PHY header had invalid value.*/
        uint16_t crcFailedCount;

        /*Number of times when PHY layer received new data to transmit.*/
        uint16_t txDropCount;

        /*Number of times when the PHY layer received new data on the channel.*/
        uint16_t rxDropCount;
    }gxPrimeNbOfdmPlcPhysicalLayerCounters;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_PHYSICAL_LAYER_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPrimeNbOfdmPlcMacSetup
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*PIB attribute 0x0010.*/
        unsigned char macMinSwitchSearchTime;

        /*PIB attribute 0x0011.*/
        unsigned char macMaxPromotionPdu;

        /*PIB attribute 0x0012.*/
        unsigned char macPromotionPduTxPeriod;

        /*PIB attribute 0x0013.*/
        unsigned char macBeaconsPerFrame;

        /*PIB attribute 0x0014.*/
        unsigned char macScpMaxTxAttempts;

        /*PIB attribute 0x0015.*/
        unsigned char macCtlReTxTimer;

        /*PIB attribute 0x0018.*/
        unsigned char macMaxCtlReTx;
    }gxPrimeNbOfdmPlcMacSetup;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_SETUP

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS
    /*
   ---------------------------------------------------------------------------
   Online help:
   http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPrimeNbOfdmPlcMacFunctionalParameters
   */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*LNID allocated to this node at time of its registration.*/
        int16_t lnId;

        /*LSID allocated to this node at the time of its promotion.*/
        unsigned char lsId;

        /*SID of the switch node through which this node is connected to the subnetwork.*/
        unsigned char sId;

        /*Subnetwork address to which this node is registered.*/
        gxByteBuffer sna;

        /*Present functional state of the node.*/

        DLMS_MAC_STATE state;

        /*The SCP length, in symbols, in present frame.*/
        int16_t scpLength;

        /*Level of this node in subnetwork hierarchy.*/
        unsigned char nodeHierarchyLevel;

        /*Number of beacon slots provisioned in present frame structure.*/
        unsigned char beaconSlotCount;

        /*Beacon slot in which this device’s switch node transmits its beacon.*/
        unsigned char beaconRxSlot;

        /*Beacon slot in which this device transmits its beacon.*/
        unsigned char beaconTxSlot;

        /*Number of frames between receptions of two successive beacons.*/
        unsigned char beaconRxFrequency;

        /*Number of frames between transmissions of two successive beacons.*/
        unsigned char beaconTxFrequency;

        /*This attribute defines the capabilities of the node.*/
        DLMS_MAC_CAPABILITIES capabilities;
    }gxPrimeNbOfdmPlcMacFunctionalParameters;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_FUNCTIONAL_PARAMETERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPrimeNbOfdmPlcMacCounters
    */
    typedef struct
    {
        /*Base class where class is derived.*/
        gxObject base;

        /*Count of successfully transmitted MSDUs.*/
        uint32_t txDataPktCount;

        /*Count of successfully received MSDUs whose destination address was this node.*/
        uint32_t rxDataPktCount;

        /*Count of successfully transmitted MAC control packets.*/
        uint32_t txCtrlPktCount;

        /*Count of successfully received MAC control packets whose destination was this node.*/
        uint32_t rxCtrlPktCount;

        /*Count of failed CSMA transmit attempts.*/
        uint32_t csmaFailCount;

        /*Count of number of times this node has to back off SCP transmission due to channel busy state.*/
        uint32_t csmaChBusyCount;
    }gxPrimeNbOfdmPlcMacCounters;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_COUNTERS

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA
    /*Multicast switching table.*/
    typedef struct
    {
        /*LCID of multicast group.*/
        signed char id;
        /*Number of child nodes.*/
        int16_t members;
    }gxMacMulticastEntry;

    /*MAC direct table element.*/
    typedef struct
    {
        /*SID of switch through which the source service node is connected.*/
        int16_t sourceSId;

        /*NID allocated to the source service node.*/
        int16_t sourceLnId;
        /*LCID allocated to this connection at the source.*/
        int16_t sourceLcId;

        /*SID of the switch through which the destination service node is connected.*/
        int16_t destinationSId;
        /*NID allocated to the destination service node.*/
        int16_t destinationLnId;

        /*LCID allocated to this connection at the destination.*/
        int16_t destinationLcId;
        /*Entry DID is the EUI-48 of the direct switch.*/
        unsigned char did[6];
    }gxMacDirectTable;

    /*MAC available switch.*/
    typedef struct
    {
        /*EUI-48 of the subnetwork.*/
        gxByteBuffer sna;

        /*SID of this switch.*/
        int16_t lsId;

        /*Level of this switch in subnetwork hierarchy.*/
        signed char level;

        /*The received signal level for this switch.*/
        signed char rxLevel;
        /*The signal to noise ratio for this switch.*/
        signed char rxSnr;
    }gxMacAvailableSwitch;

    /*MAC PHY communication parameters.*/
    typedef struct
    {
        /*EUI is the EUI-48 of the other device.*/
        unsigned char eui[6];

        /*The tx power of GPDU packets sent to the device.*/
        signed char txPower;

        /*The Tx coding of GPDU packets sent to the device.*/
        signed char txCoding;

        /*The Rx coding of GPDU packets received from the device.*/
        signed char rxCoding;

        /*The Rx power level of GPDU packets received from the device.*/
        signed char rxLvl;

        /*SNR of GPDU packets received from the device.*/
        signed char snr;

        /*The number of times the Tx power was modified.*/
        signed char txPowerModified;

        /*The number of times the Tx coding was modified.*/
        signed char txCodingModified;

        /*The number of times the Rx coding was modified.*/
        signed char rxCodingModified;
    }gxMacPhyCommunication;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPrimeNbOfdmPlcMacNetworkAdministrationData
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*List of entries in multicast switching table.*/
        gxArray multicastEntries;//gxMacMulticastEntry[]

        /*Switch table.*/
        gxArray switchTable;//uint16_t[]

        /*List of entries in multicast switching table.*/
        gxArray directTable; //gxMacDirectTable[]

        /*List of available switches.*/
        gxArray availableSwitches;//gxMacAvailableSwitch[]

        /*List of PHY communication parameters.*/
        gxArray communications; //gxMacPhyCommunication[]

    }gxPrimeNbOfdmPlcMacNetworkAdministrationData;
#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_MAC_NETWORK_ADMINISTRATION_DATA

#ifndef DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION
    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSPrimeNbOfdmPlcApplicationsIdentification
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*Textual description of the firmware version running on the device.*/
        gxByteBuffer firmwareVersion;

        /*Unique vendor identifier assigned by PRIME Alliance.*/
        uint16_t vendorId;

        /*Vendor assigned unique identifier for specific product.*/
        uint16_t productId;
    }gxPrimeNbOfdmPlcApplicationsIdentification;

#endif //DLMS_IGNORE_PRIME_NB_OFDM_PLC_APPLICATIONS_IDENTIFICATION

#ifndef DLMS_IGNORE_G3_PLC_MAC_LAYER_COUNTERS

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSG3PlcMacLayerCounters
    */
    typedef struct
    {
        /*Base class where class is derived.*/
        gxObject base;
        /*Successfully transmitted data packets.*/
        uint32_t txDataPacketCount;
        /*Successfully received data packets.*/
        uint32_t rxDataPacketCount;
        /*Successfully transmitted command packets.*/
        uint32_t txCmdPacketCount;
        /*Successfully received command packets.*/
        uint32_t rxCmdPacketCount;
        /*Counts the number of times when CSMA backoffs reach macMaxCSMABackoffs.*/
        uint32_t cSMAFailCount;
        /*Counts the number of times when an ACK is not received while transmitting a unicast data frame.*/
        uint32_t cSMANoAckCount;
        /*Number of frames received with bad CRC.*/
        uint32_t badCrcCount;
        /*The number of broadcast frames sent.*/
        uint32_t txDataBroadcastCount;
        /*Successfully received broadcast packets.*/
        uint32_t rxDataBroadcastCount;
    }gxG3PlcMacLayerCounters;
#endif //DLMS_IGNORE_G3_PLC_MAC_LAYER_COUNTERS

#ifndef DLMS_IGNORE_G3_PLC_MAC_SETUP

    typedef struct
    {
        //Transmitter gain size.
        unsigned char size;
        //Gain
        unsigned char value[MAX_G3_MAC_NEIGHBOUR_TABLE_GAIN_ITEM_SIZE];
    }gxNeighbourTableTransmitterGain;

    typedef struct
    {
        //Transmitter tone map.
        unsigned char size;
        //Tone map
        unsigned char value[MAX_G3_MAC_NEIGHBOUR_TABLE_TONE_MAP_ITEM_SIZE];
    }gxNeighbourTableToneMap;

    typedef struct
    {
        /*
        MAC Short Address of the node.
        */
        uint16_t shortAddress;
        /*
        Is Payload Modulation scheme used.
        */
        unsigned char payloadModulationScheme;

        /*Frequency sub-band can be used for communication with the device.*/
#ifdef DLMS_IGNORE_MALLOC
        gxNeighbourTableToneMap toneMap[MAX_G3_MAC_NEIGHBOUR_TABLE_TONE_MAP_SIZE];
#else
        bitArray toneMap;
#endif //DLMS_IGNORE_MALLOC
        /*Modulation type.*/
        DLMS_G3_PLC_MODULATION modulation;
        /*Tx Gain.*/
        signed char txGain;
        /*Tx Gain resolution.*/
        DLMS_G3_PLC_GAIN_RESOLUTION txRes;
        /*Transmitter gain for each group of tones represented by one valid bit of the tone map.*/
#ifdef DLMS_IGNORE_MALLOC
        gxNeighbourTableTransmitterGain txCoeff[MAX_G3_MAC_NEIGHBOUR_TABLE_GAIN_SIZE];
#else
        bitArray txCoeff;
#endif //DLMS_IGNORE_MALLOC

        /*Link Quality Indicator.*/
        unsigned char lqi;
        /*Phase difference in multiples of 60 degrees between the mains phase of the local node
        and the neighbour node. */
        signed char phaseDifferential;
        /*Remaining time in minutes until which the tone map response parameters in the neighbour table are considered valid.*/
        unsigned char tmrValidTime;
        /*Optinal data.*/
        unsigned char noData;
    }gxNeighbourTable;

    typedef struct
    {
        // The 16-bit address the device is using to communicate through the PAN.
        uint16_t shortAddress;
        // Link Quality Indicator.
        unsigned char lqi;
        // Valid time.
        unsigned char validTime;
    }gxMacPosTable;

    typedef struct
    {
        //Key ID.
        unsigned char id;
        //Key
        unsigned char key[MAX_G3_MAC_KEY_TABLE_KEY_SIZE];
    }gxG3MacKeyTable;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSG3PlcMacSetup
    */
    typedef struct
    {
        /*Base class where class is derived.*/
        gxObject base;
        uint16_t shortAddress;
        uint16_t rcCoord;
        uint16_t panId;
        gxArray keyTable;
        uint32_t frameCounter;
        bitArray toneMask;
        uint8_t tmrTtl;
        uint8_t maxFrameRetries;
        uint8_t neighbourTableEntryTtl;
        gxArray neighbourTable;
        uint8_t highPriorityWindowSize;
        uint8_t cscmFairnessLimit;
        uint8_t beaconRandomizationWindowLength;
        uint8_t macA;
        uint8_t macK;
        uint8_t minCwAttempts;
        uint8_t cenelecLegacyMode;
        uint8_t fccLegacyMode;
        uint8_t maxBe;
        uint8_t maxCsmaBackoffs;
        uint8_t minBe;
        /*If true, MAC uses maximum contention window.*/
        unsigned char macBroadcastMaxCwEnabled;
        /*Attenuation of the output level in dB.*/
        unsigned char macTransmitAtten;
        /*The neighbour table contains some information
          about all the devices within the POS of the device.
        */
        gxArray macPosTable;
        /*Duplicate frame detection time in seconds.*/
        unsigned char macDuplicateDetectionTTL;
    }gxG3PlcMacSetup;
#endif //DLMS_IGNORE_G3_PLC_MAC_SETUP

#ifndef DLMS_IGNORE_G3_PLC_6LO_WPAN

    // The routing configuration element specifies all parameters linked to the routing mechanism described in ITU-T G.9903:2014.
    typedef struct
    {
        // Maximum time that a packet is expected to take to reach any node from any node in seconds. 
        //
        // PIB attribute: 0x11.     
        unsigned char netTraversalTime;

        // Maximum time-to-live of a routing table entry (in minutes). 
        //
        // PIB attribute: 0x12.
        uint16_t routingTableEntryTtl;

        // A weight factor for the Robust Mode to calculate link cost. 
        //
        // PIB attribute: 0x13.
        unsigned char kr;

        // A weight factor for modulation to calculate link cost.
        //
        // PIB attribute: 0x14.     
        unsigned char km;

        // A weight factor for number of active tones to calculate link cost. 
        //
        // PIB attribute: 0x15. 
        unsigned char kc;

        // A weight factor for LQI to calculate route cost.
        //
        // PIB attribute: 0x16. 
        unsigned char kq;

        //  A weight factor for hop to calculate link cost. 
        //
        // PIB attribute: 0x17.
        unsigned char kh;

        // A weight factor for the number of active routes in the routing table to calculate link cost. 
        //
        // PIB attribute: 0x1B. 
        unsigned char krt;

        // The number of RREQ retransmission in case of RREP reception time out. 
        //
        // PIB attribute: 0x18.
        unsigned char rReqRetries;

        // The number of seconds to wait between two consecutive RREQ – RERR generations.
        //
        // PIB attribute: 0x19.     
        unsigned char rReqReqWait;

        // Maximum time-to-live of a blacklisted neighbour entry (in minutes). 
        //
        // PIB attribute: 0x1F.
        uint16_t blacklistTableEntryTtl;

        //  If TRUE, the RREQ shall be generated with its 'unicast RREQ'.
        //
        // PIB attribute: 0x0D.     
        unsigned char unicastRreqGenEnable;

        // Enable the sending of RLCREQ frame by the device.
        //
        // PIB attribute: 0x09.     
        unsigned char rlcEnabled;

        // It represents an additional cost to take into account a possible asymmetry in the link.
        //
        // PIB attribute: 0x0A.     
        unsigned char addRevLinkCost;
    }gxRoutingConfiguration;

    typedef struct
    {
        // Address of the destination.
        uint16_t destinationAddress;

        // Address of the next hop on the route towards the destination.
        uint16_t nextHopAddress;

        // Cumulative link cost along the route towards the destination.
        uint16_t routeCost;

        // Number of hops of the selected route to the destination.
        unsigned char hopCount;

        // Number of weak links to destination.
        unsigned char weakLinkCount;

        // Remaining time in minutes until when this entry in the routing table is considered valid.
        uint16_t validTime;
    } gxRoutingTable;

    typedef struct
    {

        // Corresponds to the 4-bit context information used for source and destination addresses (SCI, DCI).
        unsigned char cid;

        //The length of the carried context
        unsigned char contextLength;

        // Context.
        unsigned char context[16];

        // Indicates if the context is valid for use in compression.
        unsigned char compression;

        // Remaining time in minutes during which the context information table is considered valid. It is updated upon reception of the advertised context. 
        uint16_t validLifetime;
    }gxContextInformationTable;

    /*Contains the list of the blacklisted neighbours.*/
    typedef struct
    {
        /*Blacklisted neighbour address.*/
        uint16_t neighbourAddress;
        /*Remaining time in minutes until which this entry in
        the blacklisted neighbour table is considered valid.*/
        uint16_t validTime;
    }gxBlacklistTable;

    typedef struct
    {
        //  Source address of a broadcast packet.
        uint16_t sourceAddress;

        //  The sequence number contained in the BC0 header
        unsigned char sequenceNumber;

        //  Remaining time in minutes until when this entry in the broadcast log table is considered valid.
        uint16_t validTime;
    }gxBroadcastLogTable;

    /*
       ---------------------------------------------------------------------------
       Online help:
       http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSG3Plc6LoWPAN
       */
    typedef struct
    {
        /*Base class where class is derived.*/
        gxObject base;
        // Defines the maximum number of hops to be used by the routing algorithm.

        //
        // PIB attribute 0x0F.
        unsigned char maxHops;

        // The weak link value defines the LQI value below which a link to a neighbour is considered as a weak link.
        //
        // PIB attribute 0x1A.
        unsigned char weakLqiValue;

        // The minimum security level to be used for incoming and outgoing adaptation frames.
        //
        // PIB attribute 0x00.
        unsigned char securityLevel;

        //  Contains the list of prefixes defined on this PAN
        //
        // PIB attribute 0x01.
        gxByteBuffer prefixTable;

        // The routing configuration element specifies all parameters linked to the routing mechanism described in ITU-T G.9903:2014.
        gxArray routingConfiguration;

        // Maximum time to live of an adpBroadcastLogTable entry (in minutes).
        //
        // PIB attribute 0x02.
        uint16_t broadcastLogTableEntryTtl;

        // Routing table
        // PIB attribute 0x03.
        gxArray routingTable;

        //  Contains the context information associated to each CID extension field.
        //
        // PIB attribute 0x07.
        gxArray contextInformationTable;

        // Contains the list of the blacklisted neighbours.Key is 16-bit address of the blacklisted neighbour.
        // Value is Remaining time in minutes until which this entry in the blacklisted neighbour table is considered valid.
        //
        // PIB attribute 0x1E.
        gxArray blacklistTable;

        //  Broadcast log table
        //
        // PIB attribute 0x0B.
        gxArray broadcastLogTable;

        // Contains the group addresses to which the device belongs.
        //
        // PIB attribute 0x0E.
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxArray groupTable;
#else
        variantArray groupTable;
#endif //DLMS_IGNORE_MALLOC

        //  Network join timeout in seconds for LBD
        //
        // PIB attribute 0x20.
        uint16_t maxJoinWaitTime;

        // Timeout for path discovery in seconds.
        //
        // PIB attribute 0x21.
        unsigned char pathDiscoveryTime;

        // Index of the active GMK to be used for data transmission.
        //
        // PIB attribute 0x22.
        unsigned char activeKeyIndex;

        // Metric Type to be used for routing purposes.
        //
        // PIB attribute 0x3.
        unsigned char metricType;

        // Defines the short address of the coordinator.
        //
        // PIB attribute 0x8.
        uint16_t coordShortAddress;

        // Is default routing (LOADng) disabled.
        //
        // PIB attribute 0xF0.
        unsigned char disableDefaultRouting;

        // Defines the type of the device connected to the modem.
        //
        // PIB attribute 0x10.
        DLMS_PAN_DEVICE_TYPE deviceType;

        // If true, the default route will be created.
        //
        // PIB attribute 0x24
        unsigned char defaultCoordRouteEnabled;

        // List of the addresses of the devices for which this LOADng 
        // router is providing connectivity.
        //
        // PIB attribute 0x23.
#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
        gxArray destinationAddress;
#else
        variantArray destinationAddress;
#endif //DLMS_IGNORE_MALLOC
        //  PIB attribute 0x04.
        unsigned char lowLQI;
        //  PIB attribute 0x04.
        unsigned char highLQI;
    }gxG3Plc6LoWPAN;

#endif //DLMS_IGNORE_G3_PLC_6LO_WPAN

#ifndef DLMS_IGNORE_FUNCTION_CONTROL

    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        // Function name.
        unsigned char name[MAX_FUNCTION_NAME_LENGTH];
        // Function name size.
        uint16_t size;
#else
        //function name.
        gxByteBuffer name;
#endif //DLMS_IGNORE_MALLOC
        /*Function status.*/
        uint8_t status;
    }functionStatus;

    typedef struct
    {
#ifdef DLMS_IGNORE_MALLOC
        // Function name.
        unsigned char name[MAX_FUNCTION_NAME_LENGTH];
        // Function name size.
        uint16_t nameSize;
#else
        //function name.
        gxByteBuffer name;
#endif //DLMS_IGNORE_MALLOC
        /*Target objects.*/

#if !defined(DLMS_IGNORE_OBJECT_POINTERS) && !defined(DLMS_IGNORE_MALLOC) && !defined(DLMS_COSEM_EXACT_DATA_TYPES)
        objectArray functionSpecifications;
#else        
        gxObject* functionSpecifications[MAX_FUNCTION_TARGET_LENGTH];
        // Function specifications size.
        uint16_t functionSpecificationsSize;
#endif //DLMS_IGNORE_OBJECT_POINTERS
    }functionalBlock;
    /*
       ---------------------------------------------------------------------------
       Online help:
       http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSG3Plc6LoWPAN
    */
    typedef struct
    {
        gxObject base;
        /*The current status of each functional block.*/
        gxArray activationStatus;
        /*Function list.*/
        gxArray functions;
    }gxFunctionControl;

    //Clear activation status of the function control.
    int obj_clearActivationStatus(gxArray* list);

    //Clear function list of the function control.
    int obj_clearFunctionList(gxArray* list);

#endif //DLMS_IGNORE_FUNCTION_CONTROL

#ifndef DLMS_IGNORE_ARRAY_MANAGER
    typedef struct
    {
        // Target object.
        gxObject* target;
        // Attribute Index of DLMS object.
        signed char attributeIndex;
    }gxTargetObject;

    typedef struct
    {
        /// Array object ID.
        unsigned char id;
        /// <summary>
        /// Array objects.
        /// </summary>
        gxTargetObject element;
    }gxArrayManagerItem;

    /*
      ---------------------------------------------------------------------------
      Online help:
      http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSArrayManager
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        gxArray elements;
    }gxArrayManager;

#endif //DLMS_IGNORE_ARRAY_MANAGER

#ifndef DLMS_IGNORE_COMPACT_DATA

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSCompactData
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        gxByteBuffer buffer;
        gxArray captureObjects; //gxkey<gxObject*, gxTarget*>
        unsigned char templateId;
        gxByteBuffer templateDescription;
        DLMS_CAPTURE_METHOD captureMethod;
    } gxCompactData;
#endif //DLMS_IGNORE_COMPACT_DATA

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSArbitrator
#ifndef DLMS_IGNORE_ARBITRATOR
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /**
        * Requested actions.
        */
        gxArray actions; //List<gxActionItem>;

        /**
         * Permissions for each actor to request actions.
         */
        gxArray permissionsTable; //List<bitArray>
        /**
         * Weight allocated for each actor and to each possible action of that
         * actor.
         */
        gxArray weightingsTable; //List<List<UInt16>>
        /**
         * The most recent requests of each actor.
         */
        gxArray mostRecentRequestsTable; //List<bitArray>
        /**
         * The number identifies a bit in the Actions.
         */
        unsigned char lastOutcome;
    } gxArbitrator;
#endif //DLMS_IGNORE_ARBITRATOR

#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIec8802LlcType1Setup
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * Maximum number of octets in a UI PDU.
        */
        uint16_t maximumOctetsUiPdu;
    } gxIec8802LlcType1Setup;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE1_SETUP
#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIec8802LlcType2Setup
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * Transmit Window Size K.
        */
        unsigned char transmitWindowSizeK;
        /**
        * Transmit Window Size RW.
        */
        unsigned char transmitWindowSizeRW;
        /**
        * Maximum octets in I Pdu N1.
        */
        uint16_t maximumOctetsPdu;
        /**
        * Maximum number of transmissions, N2.
        */
        unsigned char maximumNumberTransmissions;
        /**
        * Acknowledgement timer in seconds.
        */
        uint16_t acknowledgementTimer;
        /**
        * P-bit timer in seconds.
        */
        uint16_t bitTimer;
        /**
        * Reject timer.
        */
        uint16_t rejectTimer;
        /**
        * Busy state timer.
        */
        uint16_t busyStateTimer;
    } gxIec8802LlcType2Setup;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE2_SETUP

#ifndef DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSIec8802LlcType3Setup
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * Maximum number of octets in an ACn command PDU, N3.
        */
        uint16_t maximumOctetsACnPdu;
        /**
        * Maximum number of times in transmissions N4.
        */
        unsigned char maximumTransmissions;
        /**
        * Acknowledgement time, T1.
        */
        uint16_t acknowledgementTime;
        /**
        * Receive lifetime variable, T2.
        */
        uint16_t receiveLifetime;
        /**
        * Transmit lifetime variable, T3.
        */
        uint16_t transmitLifetime;
    } gxIec8802LlcType3Setup;
#endif //DLMS_IGNORE_IEC_8802_LLC_TYPE3_SETUP

#ifndef DLMS_IGNORE_SFSK_ACTIVE_INITIATOR
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSFSKActiveInitiator
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * System title of active initiator.
        */
        gxByteBuffer systemTitle;
        /**
        * MAC address of active initiator.
        */
        uint16_t macAddress;
        /**
        * L SAP selector of active initiator.
        */
        unsigned char lSapSelector;
    } gxSFSKActiveInitiator;
#endif //DLMS_IGNORE_SFSK_ACTIVE_INITIATOR

#ifndef DLMS_IGNORE_SFSK_MAC_COUNTERS
    typedef struct
    {
        uint16_t first;
        uint32_t second;
    }gxUint16PairUint32;

    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSFSKMacCounters
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * List of synchronization registers.
        */
        gxArray synchronizationRegister; //List<gxUint16PairUint32>

        uint32_t physicalLayerDesynchronization;
        uint32_t timeOutNotAddressedDesynchronization;

        uint32_t timeOutFrameNotOkDesynchronization;

        uint32_t writeRequestDesynchronization;
        uint32_t wrongInitiatorDesynchronization;
        /**
        * List of broadcast frames counter.
        */
        gxArray broadcastFramesCounter; //list<gxUint16PairUint32>
        /**
        * Repetitions counter.
        */
        uint32_t repetitionsCounter;
        /**
        * Transmissions counter.
        */
        uint32_t transmissionsCounter;
        /**
        * CRC OK frames counter.
        */
        uint32_t crcOkFramesCounter;
        /**
        * CRC NOK frames counter.
        */
        uint32_t crcNOkFramesCounter;
    } gxFSKMacCounters;
#endif //DLMS_IGNORE_SFSK_MAC_COUNTERS

#ifndef DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSFSKMacSynchronizationTimeouts
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * Search initiator timeout.
        */
        uint16_t searchInitiatorTimeout;
        /**
        * Synchronization confirmation timeout.
        */
        uint16_t synchronizationConfirmationTimeout;
        /**
        * Time out not addressed.
        */
        uint16_t timeOutNotAddressed;
        /**
        * Time out frame not OK.
        */
        uint16_t timeOutFrameNotOK;
    } gxSFSKMacSynchronizationTimeouts;
#endif //DLMS_IGNORE_SFSK_MAC_SYNCHRONIZATION_TIMEOUTS

#ifndef DLMS_IGNORE_SFSK_PHY_MAC_SETUP
    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.CGXDLMSSFSKPhyMacSetUp
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * Initiator electrical phase.
        */
        DLMS_INITIATOR_ELECTRICAL_PHASE initiatorElectricalPhase;

        /**
        * Delta electrical phase.
        */
        DLMS_DELTA_ELECTRICAL_PHASE deltaElectricalPhase;

        /**
        * Corresponds to the maximum allowed gain bound to be used by the server
        * system in the receiving mode. The default unit is dB.
        */
        unsigned char maxReceivingGain;
        /**
        * Corresponds to the maximum attenuation bound to be used by the server
        * system in the transmitting mode.The default unit is dB.
        */
        unsigned char maxTransmittingGain;
        /**
        * Intelligent search initiator process. If the value of the initiator
        * signal is above the value of this attribute, a fast synchronization
        * process is possible.
        */
        unsigned char searchInitiatorThreshold;
        /**
        * Mark frequency required for S-FSK modulation.
        */
        uint32_t markFrequency;
        /**
        * Space frequency required for S-FSK modulation.
        */
        uint32_t spaceFrequency;
        /**
        * Mac Address.
        */
        uint16_t macAddress;

        /**
        * MAC group addresses.
        */
        gxArray macGroupAddresses; //List<uint16_t>
        /**
        * Specifies are all frames repeated.
        */
        DLMS_REPEATER repeater;
        /**
        * Repeater status.
        */
        unsigned char repeaterStatus;
        /**
        * Min delta credit.
        */
        unsigned char minDeltaCredit;

        /**
        * Initiator MAC address.
        */
        uint16_t initiatorMacAddress;

        /**
        * Synchronization locked/unlocked state.
        */
        unsigned char synchronizationLocked;
        /**
        * Transmission speed supported by the physical device.
        */
        DLMS_BAUD_RATE transmissionSpeed;
    } gxSFSKPhyMacSetUp;
#endif //DLMS_IGNORE_SFSK_PHY_MAC_SETUP

#ifndef DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        //Reporting system list
        unsigned char name[MAX_REPORTING_SYSTEM_ITEM_LENGTH];
        uint16_t size;
    }gxReportingSystemItem;
#endif //DLMS_IGNORE_MALLOC


    //---------------------------------------------------------------------------
    // Online help:
    //  http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSSFSKReportingSystemList
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;
        /**
        * Contains the system titles of the server systems which have made a
        * DiscoverReport request and which have not already been registered.
        */
        gxArray reportingSystemList;
    } gxSFSKReportingSystemList;
#endif //DLMS_IGNORE_SFSK_REPORTING_SYSTEM_LIST

#ifndef DLMS_IGNORE_LTE_MONITORING

    /// <summary>
    /// Lte coverage enhancement.
    /// </summary>
    typedef enum
    {
        // CE Mode A in LTE Cat M1 and CE Level 0 in NB-Iot.
        DLMS_LTE_COVERAGE_ENHANCEMENT_LEVEL0 = 0,
        // CE Mode B in LTE Cat M1 and CE Level 1 in NB-Iot.
        DLMS_LTE_COVERAGE_ENHANCEMENT_LEVEL1,
        // CE Level 2 in NB-Iot.
        DLMS_LTE_COVERAGE_ENHANCEMENT_LEVEL2
    }DLMS_LTE_COVERAGE_ENHANCEMENT;

    // Network parameters for the LTE network
    typedef struct
    {
        // T3402 timer in seconds.
        uint16_t t3402;

        // T3412 timer in seconds.
        uint16_t t3412;

        // T3412ext2 timer in seconds.
        uint32_t t3412ext2;

        // Power saving mode active timer timer in 0,01 seconds.
        uint16_t t3324;

        // Extended idle mode DRX cycle timer in 0,01 seconds.
        uint32_t teDRX;

        // DRX paging time window timer in seconds.
        uint16_t tPTW;

        // The minimum required Rx level in the cell in dBm.
        signed char qRxlevMin;

        // The minimum required Rx level in enhanced coverage CE Mode A.
        signed char qRxlevMinCE;

        // The minimum required Rx level in enhanced coverage CE Mode B.
        signed char qRxLevMinCE1;
    }gxLteNetworkParameters;

    // Quality of service of the LTE network.
    typedef struct
    {
        // Signal quality.
        signed char signalQuality;

        // Signal level.
        signed char signalLevel;

        // Signal to noise ratio.
        signed char signalToNoiseRatio;

        // Coverage enhancement.
        DLMS_LTE_COVERAGE_ENHANCEMENT coverageEnhancement;
    }gxLteQualityOfService;

    // Online help:
    // https://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSLteMonitoring
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        // Network parameters for the LTE network.
        gxLteNetworkParameters networkParameters;

        // Quality of service of the LTE network.
        gxLteQualityOfService qualityOfService;
    } gxLteMonitoring;
#endif //DLMS_IGNORE_LTE_MONITORING

#ifndef DLMS_IGNORE_NTP_SETUP

#ifdef DLMS_IGNORE_MALLOC
    typedef struct
    {
        // Authentication id.
        uint32_t id;
        // The size of the authentication key.
        uint16_t size;
        // Authentication key.
        unsigned char key[MAX_AUTHENTICATION_KEY_LENGTH];
    }gxNtpKey;
#endif //DLMS_IGNORE_SERVER

    // Online help:
    // https://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSNtpSetup
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        /*Is NTP time synchronisation active.*/
        unsigned char activated;

        /*NTP server address.*/
        gxByteBuffer serverAddress;

        /*UDP port related to this protocol.*/
        uint16_t port;

        /*The authentication mode.*/
        DLMS_NTP_AUTHENTICATION_METHOD authentication;

        /*Symmetric keys for authentication.*/
        gxArray keys;

        /*Client key(NTP server public key).*/
        gxByteBuffer clientKey;
    } gxNtpSetup;
#endif //DLMS_IGNORE_NTP_SETUP

#ifdef DLMS_ITALIAN_STANDARD

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTariffPlan
    */
    typedef enum
    {
        // 1st january of every year.
        DLMS_WEEKLY_ACTIVATION_JANUARY_1,
        // 6st january of every year.
        DLMS_WEEKLY_ACTIVATION_JANUARY_6,
        // Easter Monday of of every year.
        DLMS_WEEKLY_ACTIVATION_EASTER_MONDAY,
        // 25th April of every year
        DLMS_WEEKLY_ACTIVATION_APRIL_25,
        // 1st May of every year.
        DLMS_WEEKLY_ACTIVATION_MAY_1,
        // 2nd June of every year.
        DLMS_WEEKLY_ACTIVATION_JUNE_2,
        // 15th August of every year
        DLMS_WEEKLY_ACTIVATION_AUGUST_15,
        // 1st November of every year.
        DLMS_WEEKLY_ACTIVATION_NOVEMBER_1,
        // 8th December of every year
        DLMS_WEEKLY_ACTIVATION_DECEMBER_8,
        // 25th December of every year.
        DLMS_WEEKLY_ACTIVATION_DECEMBER_25,
        //  26th December of every year.
        DLMS_WEEKLY_ACTIVATION_DECEMBER_26
    }DLMS_WEEKLY_ACTIVATION;


    /*
    The default tariff.
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTariffPlan
    */
    typedef enum
    {
        // Not active.
        DLMS_DEFAULT_TARIFF_BAND_NONE = 0,
        // Tariff band 1.
        DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_1,
        // Tariff band 2.
        DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_2,
        // Tariff band 3.
        DLMS_DEFAULT_TARIFF_BAND_TARIFF_BAND_3
    }DLMS_DEFAULT_TARIFF_BAND;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTariffPlan
    */
    typedef struct
    {
        // Start hour of the interval.
        unsigned char startHour;
        // Tariff used in the current interval
        DLMS_DEFAULT_TARIFF_BAND intervalTariff;
        // Is interval used.
        unsigned char useInterval;
    } gxInterval;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTariffPlan
    */
    typedef struct
    {
        // Day of month when the season will become active.
        unsigned char dayOfMonth;
        // Month of the year when the season will become active.
        unsigned char month;
        // Working day intervals.
        gxInterval workingDayIntervals[5];
        // Saturday intervals.
        gxInterval saturdayIntervals[5];
        // Saturday intervals.
        gxInterval holidayIntervals[5];
    } gxBandDescriptor;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTariffPlan
    */
    typedef struct
    {
        // Is tariff plan enabled.
        unsigned char defaultTariffBand;
        // Winter season.
        gxBandDescriptor winterSeason;
        // Summer season.
        gxBandDescriptor summerSeason;
        // Seasons.
        bitArray weeklyActivation;
        // Seasons.
#ifdef DLMS_IGNORE_MALLOC
        gxArray specialDays; //UInt16[]
#else 
        variantArray specialDays; //UInt16[]
#endif //DLMS_IGNORE_MALLOC
    } gxPlan;

    /*
    ---------------------------------------------------------------------------
    Online help:
    http://www.gurux.fi/Gurux.DLMS.Objects.GXDLMSTariffPlan
    */
    typedef struct
    {
        /*
        * Base class where class is derived.
        */
        gxObject base;

        // Calendar Name.
        gxByteBuffer calendarName;
        //Is tariff plan enabled.
        unsigned char enabled;
        // Tariff plan.
        gxPlan plan;
        // Activation date and time.
        gxtime activationTime;
    } gxTariffPlan;

#endif //DLMS_ITALIAN_STANDARD

    void obj_clear(gxObject* object);

    int obj_clearProfileGenericBuffer(gxArray* buffer);

    int obj_clearSapList(gxArray* buffer);

#if !(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA) && defined(DLMS_IGNORE_PUSH_SETUP))
    int obj_clearPushObjectList(gxArray* buffer);
    /////////////////////////////////////////////////////////////////////////
    // Clear capture object list.
    //
    // captureObjects: List of objects to clear.
    int obj_clearProfileGenericCaptureObjects(gxArray* captureObjects);
#endif //!(defined(DLMS_IGNORE_PROFILE_GENERIC) && defined(DLMS_IGNORE_COMPACT_DATA))

    int obj_clearSeasonProfile(gxArray* list);

    int obj_clearWeekProfileTable(gxArray* list);

    int obj_clearDayProfileTable(gxArray* list);

    int obj_clearRegisterMonitorActions(gxArray* list);

    int obj_clearModemConfigurationInitialisationStrings(gxArray* list);

    int obj_clearScheduleEntries(gxArray* list);
    int obj_clearScriptTable(gxArray* list);

#if defined(DLMS_SECURITY_SUITE_1) ||defined(DLMS_SECURITY_SUITE_2) 
    int obj_clearCertificates(gxArray* list);
#endif //defined(DLMS_SECURITY_SUITE_1) ||defined(DLMS_SECURITY_SUITE_2) 

#if !(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES))
    int obj_clearRegisterActivationAssignment(objectArray* list);
#else
    int obj_clearRegisterActivationAssignment(gxArray* list);
#endif //!(defined(DLMS_IGNORE_OBJECT_POINTERS) || defined(DLMS_IGNORE_MALLOC))

    int obj_clearRegisterActivationMaskList(gxArray* list);

    int obj_clearIP4SetupOptions(gxArray* list);

    int obj_clearPPPSetupIPCPOptions(gxArray* list);
    int obj_clearPPPSetupLCPOptions(gxArray* list);

    int obj_clearActiveDevices(gxArray* list);

    //Clear bit array list.
    int obj_clearBitArrayList(gxArray* list);

    //Clear variant array list.
    int obj_clearVariantList(gxArray* list);

    int obj_clearChargeTables(gxArray* list);

    int obj_clearCreditChargeConfigurations(gxArray* list);

    int obj_clearTokenGatewayConfigurations(gxArray* list);

    //Get amount of attributes in COSEM object.
    unsigned char obj_attributeCount(
        gxObject* object);

    //Get amount of methods in COSEM object.
    unsigned char obj_methodCount(
        gxObject* object);

    int obj_getAttributeIndexToRead(
        gxObject* object,
        gxByteBuffer* ba);

    //Returns collection of attributes to read.
    int obj_getAttributeIndexToRead(
        gxObject* object,
        gxByteBuffer* ba);

#ifndef DLMS_IGNORE_DATA_PROTECTION
    void clear_ProtectionParameter(gxProtectionParameter* target);
    void init_ProtectionParameter(gxProtectionParameter* target);
#endif //DLMS_IGNORE_DATA_PROTECTION

#ifndef DLMS_IGNORE_PARAMETER_MONITOR
    int obj_clearParametersList(gxArray* buffer);
#endif //DLMS_IGNORE_PARAMETER_MONITOR

    int obj_clearCertificateInfo(gxArray* arr);

    //Clear user list.
    int obj_clearUserList(
        gxArray* list);

    //Clear neighbour table.
    int obj_clearNeighbourTable(
        gxArray* list);

    //Clear available switches.
    int obj_clearAvailableSwitches(
        gxArray* list);

    //Clear byte buffer list.
    int obj_clearByteBufferList(
        gxArray* list);

#if _CVI_ //If LabWindows/CVI
#define BASE(X) &X.base
#define INIT_OBJECT(X, Y, Z) cosem_init4(&X, sizeof(X), Y, Z)
#else
#define BASE(X) &X.base
#define INIT_OBJECT(X, Y, Z) cosem_init4(&X.base, sizeof(X), Y, Z)
#endif //_CVI_


#define SET_OCTET_STRING(X, V, S) memcpy(X.value, V, S);X.size=S

#ifdef  __cplusplus
}
#endif

#endif //GXOBJECTS_H
