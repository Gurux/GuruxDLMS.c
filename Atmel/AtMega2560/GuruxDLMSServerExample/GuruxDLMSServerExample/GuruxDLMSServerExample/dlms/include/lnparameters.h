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

#ifndef LN_PARAMETERS_H
#define LN_PARAMETERS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "dlmssettings.h"
#include "bytebuffer.h"
#include "enums.h"

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
     * Request type.
     */
    int requestType;
    /**
     * Attribute descriptor.
     */
    gxByteBuffer attributeDescriptor;
    /**
     * Data.
     */
    gxByteBuffer m_Data;

    /**
     * Send date and time. This is used in Data notification messages.
     */
#ifdef DLMS_USE_EPOCH_TIME
    uint32_t time;
#else
    struct tm time;
#endif //DLMS_USE_EPOCH_TIME
    /**
     * Reply status.
     */
    int status;
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
    int blockIndex;
} lnParameters;

#ifdef  __cplusplus
}
#endif

#endif //LN_PARAMETERS_H
