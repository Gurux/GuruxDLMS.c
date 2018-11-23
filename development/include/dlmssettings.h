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

    typedef struct
    {
        // Is custom challenges used. If custom challenge is used new challenge is
        // not generated if it is Set. This is for debugging purposes.
        unsigned char customChallenges;

        // Client to server challenge.
        gxByteBuffer ctoSChallenge;

        // Server to Client challenge.
        gxByteBuffer stoCChallenge;

        gxByteBuffer sourceSystemTitle;
        
        // Invoke ID.
        unsigned char invokeID;

        //Long Invoke ID.
        int longInvokeID;

        // Priority.
        DLMS_PRIORITY priority;

        // Service class.
        DLMS_SERVICE_CLASS serviceClass;

        // Client address.
        unsigned short clientAddress;
        //Server address.
        unsigned short serverAddress;

        unsigned char useLogicalNameReferencing;
        DLMS_INTERFACE_TYPE interfaceType;
        DLMS_AUTHENTICATION authentication;
        gxByteBuffer password;
        gxByteBuffer kek;
        /**
         * Long data count.
         */
        unsigned short count;

        /**
         * Long data index.
         */
        unsigned short index;

        /**
         * Long data position. This is used to save position of long read/write.
         */
        unsigned short position;

        /**
        * DLMS version number.
        */
        unsigned char dlmsVersionNumber;

        /**
        * Max PDU size used in communicating.
        */
        unsigned short maxPduSize;

        /**
        * Max PDU size that server uses. Client can ask anything, but server will decide.
        */
        unsigned short maxServerPDUSize;

        /**
        * HDLC sender frame sequence number.
        */
        unsigned char senderFrame;

        /**
        * HDLC receiver block sequence number.
        */
        unsigned char receiverFrame;
        /**
        * Server functionality is not supported at the moment in ANSI C version.
        */
        unsigned char server;
        unsigned char isAuthenticationRequired;

        //When connection is made client tells what kind of services it want's to use.
        DLMS_CONFORMANCE proposedConformance;

        // Functionality what server can offer.
        DLMS_CONFORMANCE negotiatedConformance;

        //Used max info TX.
        unsigned short maxInfoTX;
        //Used max info RX.
        unsigned short maxInfoRX;
        //Used max window size in TX.
        unsigned char windowSizeTX;
        //Used max window size in RX.
        unsigned char windowSizeRX;
        objectArray objects;

        // Block packet index.
        unsigned long blockIndex;
        //Is connected to the meter.
        DLMS_CONNECTION_STATE connected;

#ifndef DLMS_IGNORE_HIGH_GMAC
        ciphering cipher;
#endif //DLMS_IGNORE_HIGH_GMAC

        short userId;

        /**
        *  Protocol version.
        */
        char* protocolVersion;

        unsigned char qualityOfService;
        //Pre-established Application Associations system title.
        gxByteBuffer *preEstablishedSystemTitle;
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

        /**
         * Extra data from PDU.
         */
        gxByteBuffer data;
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
        gxIecHdlcSetup *hdlc;
#endif //DLMS_IGNORE_IEC_HDLC_SETUP
#ifndef DLMS_IGNORE_TCP_UDP_SETUP
        gxTcpUdpSetup *wrapper;
#endif //DLMS_IGNORE_TCP_UDP_SETUP
        //Time when last frame was received. HDLC framing is using this.
        long dataReceived;
        //Time when last byte was received. HDLC framing is using this.
        long frameReceived;
    } dlmsServerSettings;

    //Initialize server.
    void svr_init(
        dlmsServerSettings* settings,
        unsigned char useLogicalNameReferencing,
        DLMS_INTERFACE_TYPE interfaceType,
        //Max frame size.
        unsigned short frameSize,
        //Max PDU size.
        unsigned short pduSize,
        //Buffer where received frames are saved.
        unsigned char* frameBuffer,
        //Size of frame buffer.
        unsigned short frameBufferSize,
        //PDU Buffer.
        unsigned char* pduBuffer,
        //Size of PDU buffer.
        unsigned short pduBufferSize);

    //Initialize client.
    void cl_init(
        dlmsSettings* settings,
        unsigned char useLogicalNameReferencing,
        int clientAddress,
        int serverAddress,
        DLMS_AUTHENTICATION authentication,
        const char* password,
        DLMS_INTERFACE_TYPE interfaceType);

    //Clear DLMS settings.
    void cl_clear(
        dlmsSettings* settings);

    void svr_clear(
        dlmsServerSettings* settings);

    //Reset block index.
    void resetBlockIndex(
        dlmsSettings* settings);

    void resetFrameSequence(
        dlmsSettings* settings);

    unsigned char checkFrame(
        dlmsSettings* settings,
        unsigned char frame);

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
