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

#include <string.h> /* memset */
#include "../include/gxmem.h"
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include "../include/replydata.h"

/**
* @return Is more data available.
*/
unsigned char reply_isMoreData(gxReplyData* reply)
{
    return reply->moreData != DLMS_DATA_REQUEST_TYPES_NONE;
}

/**
* Initialize reply structure.
*/
void reply_init(gxReplyData* reply)
{
    reply->invokeId = 0;
    reply->commandType = 0;
    reply->moreData = DLMS_DATA_REQUEST_TYPES_NONE;
    reply->encryptedCommand = reply->command = DLMS_COMMAND_NONE;
    BYTE_BUFFER_INIT(&reply->data);
    reply->complete = 0;
    var_init(&reply->dataValue);
    reply->totalCount = 0;
    reply->readPosition = 0;
    reply->packetLength = 0;
    reply->peek = 0;
    reply->ignoreValue = 0;
    reply->dataType = DLMS_DATA_TYPE_NONE;
    reply->cipherIndex = 0;
#ifdef DLMS_USE_EPOCH_TIME
    reply->time = 0;
#else
    memset(&reply->time, 0, sizeof(struct tm));
#endif // DLMS_USE_EPOCH_TIME
    reply->preEstablished = 0;
    reply->blockNumber = 0;
    reply->blockNumberAck = 0;
    reply->streaming = 0;
    reply->windowSize = 0;
    reply->serverAddress = 0;
    reply->clientAddress = 0;
}

void reply_clear2(gxReplyData* reply, unsigned char clearData)
{
    reply->invokeId = 0;
    reply->moreData = DLMS_DATA_REQUEST_TYPES_NONE;
    reply->encryptedCommand = reply->command = DLMS_COMMAND_NONE;
    if (clearData)
    {
        bb_clear(&reply->data);
        reply->preEstablished = 0;
    }
    reply->complete = 0;
    var_clear(&reply->dataValue);
    reply->totalCount = 0;
    reply->readPosition = 0;
    reply->packetLength = 0;
    reply->peek = 0;
    reply->ignoreValue = 0;
    reply->dataType = DLMS_DATA_TYPE_NONE;
    reply->cipherIndex = 0;
#ifdef DLMS_USE_EPOCH_TIME
    reply->time = 0;
#else
    memset(&reply->time, 0, sizeof(struct tm));
#endif // DLMS_USE_EPOCH_TIME
    reply->blockNumber = 0;
    reply->blockNumberAck = 0;
    reply->streaming = 0;
    reply->windowSize = 0;
    reply->serverAddress = 0;
    reply->clientAddress = 0;
}

void reply_clear(gxReplyData* reply)
{
    reply_clear2(reply, 1);
}
