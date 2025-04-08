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

#ifndef APDU_H
#define APDU_H

#include "dlmssettings.h"
#include "bytebuffer.h"

#ifdef  __cplusplus
extern "C" {
#endif
#ifndef DLMS_IGNORE_CLIENT
        /**
     * Generates Aarq.
     */
    int apdu_generateAarq(
        dlmsSettings* settings,
        gxByteBuffer* data);
#endif //DLMS_IGNORE_CLIENT

    /**
     * Parse APDU.
     */
    int apdu_parsePDU(
        dlmsSettings* settings,
        gxByteBuffer* buff,
        DLMS_ASSOCIATION_RESULT* result,
        unsigned char* diagnostic,
        unsigned char* command);

#ifndef DLMS_IGNORE_SERVER
    /**
     * Server generates AARE message.
     */
    int apdu_generateAARE(
        dlmsSettings* settings,
        gxByteBuffer* data,
        DLMS_ASSOCIATION_RESULT result,
        unsigned char diagnostic,
        gxByteBuffer* errorData,
        gxByteBuffer* encryptedData,
        unsigned char command);
#endif //DLMS_IGNORE_SERVER

    /**
    * Generate user information.
    *
    * @param settings
    *            DLMS settings
    * @param cipher
    * @param data
    *            Generated user information.
    */
    int apdu_generateUserInformation(
        dlmsSettings* settings,
        gxByteBuffer* data);

    int apdu_getUserInformation(
        dlmsSettings* settings,
        gxByteBuffer* data,
        unsigned char command);

    /**
     * Verify User Information from PDU.
     */
    int apdu_verifyUserInformation(
        dlmsSettings* settings,
        gxByteBuffer* data);

#ifdef  __cplusplus
}
#endif

#endif //APDU_H
