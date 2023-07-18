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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#ifdef  __cplusplus
extern "C" {
#endif
#include "../include/gxignore.h"
#include "dlmssettings.h"

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    /**
    * SN Parameters
    */
    typedef struct
    {
        /**
         * DLMS settings.
         */
        dlmsSettings *settings;
        /**
         * DLMS command.
         */
        DLMS_COMMAND command;
        /**
         * Encrypted DLMS command.
         */
        DLMS_COMMAND encryptedCommand;
        /**
         * Request type.
         */
		unsigned char requestType;
        /**
         * Attribute descriptor.
         */
        gxByteBuffer* attributeDescriptor;
        /**
         * Data.
         */
        gxByteBuffer* data;
        /**
         * Send date and time. This is used in Data notification messages.
         */
#ifdef DLMS_USE_EPOCH_TIME
        uint32_t time;
#else
        struct tm* time;
#endif //DLMS_USE_EPOCH_TIME
        /**
         * Item Count.
         */
        int count;

        /**
         * Are there more data to send or more data to receive.
         */
        unsigned char multipleBlocks;

        /**
        * Is this last block.
        */
        unsigned char lastBlock;
        /**
         * Block index.
         */
        uint16_t blockIndex;
        //Serialize data to this PDU.
#ifdef DLMS_IGNORE_MALLOC
        gxByteBuffer* serializedPdu;
#endif //DLMS_IGNORE_MALLOC
    } gxSNParameters;

    void params_initSN(
        gxSNParameters *target,
        dlmsSettings *settings,
        DLMS_COMMAND command,
        int count,
		unsigned char commandType,
        gxByteBuffer* attributeDescriptor,
        gxByteBuffer* data,
        DLMS_COMMAND encryptedCommand);

#endif //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    /**
     * LN Parameters
     */
    typedef struct
    {
        /**
         * DLMS settings.
         */
        dlmsSettings *settings;
        /**
         * DLMS command.
         */
        DLMS_COMMAND command;
        /**
        * Encrypted DLMS command.
        */
        DLMS_COMMAND encryptedCommand;
        /**
         * Request type.
         */
		unsigned char requestType;
        /**
         * Attribute descriptor.
         */
        gxByteBuffer* attributeDescriptor;
        /**
         * Data.
         */
        gxByteBuffer* data;
        /**
         * Send date and time. This is used in Data notification messages.
         */
#ifdef DLMS_USE_EPOCH_TIME
        uint32_t time;
#else
        struct tm* time;
#endif // DLMS_USE_EPOCH_TIME
        /**
         * Reply status.
         */
        unsigned char status;
        /**
         * Are there more data to send or more data to receive.
         */
        unsigned char multipleBlocks;
        /**
         * Is this last block in send.
         */
        unsigned char lastBlock;
        /**
         * Block index.
         */
        uint32_t blockIndex;
        /**
        * Received invoke ID.
        */
        unsigned char invokeId;
        //Serialize data to this PDU.
        gxByteBuffer* serializedPdu;
    } gxLNParameters;

    void params_initLN(
        gxLNParameters *target,
        dlmsSettings* settings,
        unsigned char invokeId,
        DLMS_COMMAND command,
        unsigned char commandType,
        gxByteBuffer* attributeDescriptor,
        gxByteBuffer* data,
        unsigned char status,
        DLMS_COMMAND encryptedCommand,
        unsigned char multipleBlocks,
        unsigned char lastBlock);

#ifdef  __cplusplus
}
#endif

#endif //PARAMETERS_H
