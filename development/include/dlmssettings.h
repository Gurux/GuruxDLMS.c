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

#ifndef DLMS_SETTINGS_H
#define DLMS_SETTINGS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "enums.h"
#include "variant.h"
#include "objectarray.h"
#include "message.h"
#include "gxvalueeventargs.h"
#include "replydata.h"
#include "ciphering.h"

#ifndef DLMS_IGNORE_PLC

    typedef struct
    {
        unsigned char responseProbability;
        uint16_t allowedTimeSlots;
        unsigned char discoverReportInitialCredit;
        unsigned char icEqualCredit;
    }gxPlcRegister;

    // PLC communication settings.
    typedef struct
    {
        gxByteBuffer systemTitle;

        /**
         * Initial credit (IC) tells how many times the frame must be repeated.
         * Maximum value is 7.
         */
        unsigned char initialCredit;
        /**
         * The current credit (CC) initial value equal to IC and automatically
         * decremented by the MAC layer after each repetition. Maximum value is 7.
         */
        unsigned char currentCredit;

        /**
         * Delta credit (DC) is used by the system management application entity
         * (SMAE) of the Client for credit management, while it has no meaning for a
         * Server or a REPEATER. It represents the difference(IC-CC) of the last
         * communication originated by the system identified by the DA address to
         * the system identified by the SA address. Maximum value is 3.
         */
        unsigned char deltaCredit;
        /**
         * Source MAC address.
         */
        uint16_t macSourceAddress;
        /**
         * Destination MAC address.
         */
        uint16_t macDestinationAddress;
        /**
         * Response probability.
         */
        unsigned char responseProbability;
        /**
         * Allowed time slots.
         */
        uint16_t allowedTimeSlots;
        /**
         * Server saves client system title.
         */
        gxByteBuffer clientSystemTitle;
    }gxPlcSettings;
#endif //DLMS_IGNORE_PLC

    typedef struct
    {
        // Is custom challenges used. If custom challenge is used new challenge is
        // not generated if it is Set. This is for debugging purposes.
        unsigned char customChallenges;

        // Client to server challenge.
        gxByteBuffer ctoSChallenge;

        // Server to Client challenge.
        gxByteBuffer stoCChallenge;

        unsigned char sourceSystemTitle[8];

        // Invoke ID.
        unsigned char invokeID;

        //Long Invoke ID.
        int longInvokeID;

        // Priority.
        DLMS_PRIORITY priority;

        // Service class.
        DLMS_SERVICE_CLASS serviceClass;

        // Client address.
        uint16_t clientAddress;
        //Server address.
        uint32_t serverAddress;
        unsigned char useLogicalNameReferencing;
        DLMS_INTERFACE_TYPE interfaceType;
        DLMS_AUTHENTICATION authentication;
        gxByteBuffer password;
#ifndef DLMS_IGNORE_MALLOC
        gxByteBuffer kek;
#else
        unsigned char kek[16];
#endif //DLMS_IGNORE_MALLOC
        /**
        * DLMS version number.
        */
        unsigned char dlmsVersionNumber;

        /**
        * Max PDU size used in communicating.
        */
        uint16_t maxPduSize;

        /**
        * The PDU size proposed by the client.
        */
        uint16_t clientPduSize;

        /**
        * Max PDU size that server uses. Client can ask anything, but server will decide.
        */
        uint16_t maxServerPDUSize;

        /**
        * HDLC sender frame sequence number.
        */
        unsigned char senderFrame;

        /**
        * HDLC receiver block sequence number.
        */
        unsigned char receiverFrame;
        unsigned char server;
        unsigned char isAuthenticationRequired;

        // The client side defines which services the client wants to use.
        // The server side defines available services.
        DLMS_CONFORMANCE proposedConformance;

        // Defines the services agreed upon between the client and the server, 
        // based on the client's needs and the server's capabilities.
        DLMS_CONFORMANCE negotiatedConformance;

        // On the server side defines what conformances the client proposed.
        // This information is needed with secure release.
        DLMS_CONFORMANCE clientProposedConformance;

        //Used max info TX.
        uint16_t maxInfoTX;
        //Used max info RX.
        uint16_t maxInfoRX;
        //Used max window size in TX.
        unsigned char windowSizeTX;
        //Used max window size in RX.
        unsigned char windowSizeRX;

        // Initialize PDU size that is restored after the connection is closed.
        uint16_t initializePduSize;
        //Initialized max info TX.
        uint16_t initializeMaxInfoTX;
        //Initialized max info RX.
        uint16_t initializeMaxInfoRX;
        //Initialized max window size in TX.
        unsigned char initializeWindowSizeTX;
        //Initialized max window size in RX.
        unsigned char initializeWindowSizeRX;
#ifndef DLMS_IGNORE_PLC
        //PLC settings.
        gxPlcSettings plcSettings;
#endif //DLMS_IGNORE_PLC

        //List of internal COSEM objects.
        //Objects in this list are not added to assocaition view.
        //Objects can be used to save internal data.
        objectArray internalObjects;

        //List of COSEM objects.
        objectArray objects;

        // Block packet index.
        uint32_t blockIndex;
        //Is connected to the meter.
        DLMS_CONNECTION_STATE connected;

#ifndef DLMS_IGNORE_HIGH_GMAC
        ciphering cipher;
#endif //DLMS_IGNORE_HIGH_GMAC

        int16_t userId;

        /**
        *  Protocol version.
        */
        unsigned char protocolVersion;

        unsigned char qualityOfService;
        //Pre-established Application Associations system title.

#ifndef DLMS_IGNORE_MALLOC
        gxByteBuffer* preEstablishedSystemTitle;
#else
        unsigned char preEstablishedSystemTitle[8];
#endif //DLMS_IGNORE_MALLOC

        //Client serializes data to this PDU when malloc is not used or heap size is limited.
        gxByteBuffer* serializedPdu;
        //Auto increase Invoke ID.
        unsigned char autoIncreaseInvokeID;
        //Client adds objects that are not found from the association view here so they are released when client is clear.
        objectArray releasedObjects;

        /////////////////////////////////////////////////////////////////////////
        // Expected security suite.
        // If Expected security suite is set client can't connect with other security suite.
        unsigned char expectedSecuritySuite;
        /////////////////////////////////////////////////////////////////////////
        // Expected security policy.
        // If Expected security policy is set client can't connect with other security policies.
        unsigned char expectedSecurityPolicy;
        /////////////////////////////////////////////////////////////////////////
        // Expected Invocation(Frame) counter value.
        // Expected Invocation counter is not check if value is zero.
#ifdef DLMS_COSEM_INVOCATION_COUNTER_SIZE64
        uint64_t* expectedInvocationCounter;
#else
        uint32_t* expectedInvocationCounter;
#endif //DLMS_COSEM_INVOCATION_COUNTER_SIZE64
        /////////////////////////////////////////////////////////////////////////
        // Expected client system title.
        unsigned char* expectedClientSystemTitle;
    } dlmsSettings;

    typedef struct
    {
        /**
        * Executed command.
        */
        DLMS_COMMAND command;

        /**
         * Targets.
         */
        gxValueEventCollection targets;

#ifndef DLMS_IGNORE_MALLOC
        /**
         * Extra data from PDU.
         */
        gxByteBuffer data;
#endif //DLMS_IGNORE_MALLOC
    } gxLongTransaction;

    typedef struct
    {
        dlmsSettings base;

        gxReplyData info;
        /**
        * Received data frame.
        */
        gxByteBuffer receivedData;

        /**
        * Long get or read transaction information.
        */
        gxLongTransaction transaction;

        /**
        * Is server initialized.
        */
        unsigned char initialized;
#ifndef DLMS_IGNORE_IEC_HDLC_SETUP
        gxIecHdlcSetup* hdlc;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_IEC_LOCAL_PORT_SETUP
        gxLocalPortSetup* localPortSetup;
#endif //DLMS_IGNORE_IEC_LOCAL_PORT_SETUP

#ifndef DLMS_IGNORE_TCP_UDP_SETUP
        gxTcpUdpSetup* wrapper;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
#ifndef DLMS_IGNORE_CLOCK
        gxClock* defaultClock;
#endif //DLMS_IGNORE_CLOCK

        //Time when last frame was received. HDLC framing is using this.
        uint32_t dataReceived;
        //Time when last byte was received. HDLC framing is using this.
        uint32_t frameReceived;
        //Server is using push client address when sending push messages. Client address is used if PushAddress is zero.
        uint16_t pushClientAddress;
#ifndef DLMS_IGNORE_IEC
        /*Flag ID is used when server is operating using optical probe.*/
        unsigned char flagId[3];
#endif //DLMS_IGNORE_IEC
    } dlmsServerSettings;

    //Initialize server.
    void svr_init(
        dlmsServerSettings* settings,
        unsigned char useLogicalNameReferencing,
        DLMS_INTERFACE_TYPE interfaceType,
        //Max frame size.
        uint16_t frameSize,
        //Max PDU size.
        uint16_t pduSize,
        //Buffer where received frames are saved.
        unsigned char* frameBuffer,
        //Size of frame buffer.
        uint16_t frameBufferSize,
        //PDU Buffer.
        unsigned char* pduBuffer,
        //Size of PDU buffer.
        uint16_t pduBufferSize);

    //Initialize client.
    void cl_init(
        dlmsSettings* settings,
        unsigned char useLogicalNameReferencing,
        uint16_t clientAddress,
        uint32_t serverAddress,
        DLMS_AUTHENTICATION authentication,
        const char* password,
        DLMS_INTERFACE_TYPE interfaceType);

    //Clear DLMS settings.
    void cl_clear(
        dlmsSettings* settings);

    void svr_clear(
        dlmsServerSettings* settings);

#ifndef DLMS_IGNORE_PLC
    // Set default values for PLC.
    void plc_reset(
        dlmsSettings* settings);
#endif //DLMS_IGNORE_PLC

    //Reset block index.
    void resetBlockIndex(
        dlmsSettings* settings);

    void resetFrameSequence(
        dlmsSettings* settings);

#ifndef DLMS_IGNORE_HDLC_CHECK
    unsigned char checkFrame(
        dlmsSettings* settings,
        unsigned char frame);
#endif //DLMS_IGNORE_HDLC_CHECK

    void updateInvokeId(
        dlmsServerSettings* settings, unsigned char value);

    // Increase receiver sequence.
    //
    // value: Frame value.
    // Increased receiver frame sequence.
    unsigned char increaseReceiverSequence(
        unsigned char value);

    // Increase sender sequence.
    //
    // value : Frame value.
    // Increased sender frame sequence.
    unsigned char increaseSendSequence(
        unsigned char value);

    unsigned char getNextSend(
        dlmsSettings* settings, unsigned char first);

    unsigned char getReceiverReady(
        dlmsSettings* settings);

    unsigned char getKeepAlive(
        dlmsSettings* settings);

#ifndef DLMS_IGNORE_HIGH_GMAC
    unsigned char isCiphered(
        ciphering* cipher);
#endif //DLMS_IGNORE_HIGH_GMAC

    void trans_init(gxLongTransaction* trans);

    void trans_clear(gxLongTransaction* trans);

#ifdef  __cplusplus
}
#endif

#endif //DLMS_SETTINGS_H
