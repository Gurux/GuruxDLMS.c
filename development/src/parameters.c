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

#include "../include/parameters.h"
#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME

void params_initSN(
    gxSNParameters *target,
    dlmsSettings *settings,
    DLMS_COMMAND command,
    int count,
	unsigned char commandType,
    gxByteBuffer* attributeDescriptor,
    gxByteBuffer* data,
    DLMS_COMMAND encryptedCommand)
{
    target->settings = settings;
    target->blockIndex = (uint16_t)settings->blockIndex;
    target->command = command;
    target->encryptedCommand = encryptedCommand;
    target->count = count;
    target->requestType = commandType;
    target->attributeDescriptor = attributeDescriptor;
    target->data = data;
    target->multipleBlocks = 0;
    target->time = 0;
    target->lastBlock = 1;
#ifdef DLMS_IGNORE_MALLOC
    target->serializedPdu = settings->serializedPdu;
#endif //DLMS_IGNORE_MALLOC
}
#endif // DLMS_IGNORE_ASSOCIATION_SHORT_NAME

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
    unsigned char lastBlock)
{
    target->invokeId = invokeId;
    target->settings = settings;
    target->blockIndex = settings->blockIndex;
    target->command = command;
    target->encryptedCommand = encryptedCommand;
    target->requestType = commandType;
    target->attributeDescriptor = attributeDescriptor;
    target->data = data;
    target->time = 0;
    target->status = status;
    target->multipleBlocks = multipleBlocks;
    target->lastBlock = lastBlock;
    //Serialize data to this PDU.
#ifdef DLMS_IGNORE_MALLOC
    target->serializedPdu = settings->serializedPdu;
#endif //DLMS_IGNORE_MALLOC
}
