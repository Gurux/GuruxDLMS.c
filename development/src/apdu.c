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

#include "../include/gxignore.h"
#include "../include/gxmem.h"
#include "../include/enums.h"
#include "../include/helpers.h"
#include "../include/apdu.h"
#include "../include/errorcodes.h"
#include "../include/ciphering.h"

#ifndef DLMS_IGNORE_CLIENT
/**
 * Retrieves the string that indicates the level of authentication, if any.
 */
int apdu_getAuthenticationString(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    unsigned char p[] = { 0x60, 0x85, 0x74, 0x05, 0x08, 0x02 };
    gxByteBuffer callingAuthenticationValue;
    // If authentication is used.
    if (settings->authentication != DLMS_AUTHENTICATION_NONE)
    {
        // Add sender ACSE-requirements field component.
        bb_setUInt8(data, (unsigned short)BER_TYPE_CONTEXT
            | (char)PDU_TYPE_SENDER_ACSE_REQUIREMENTS);
        bb_setUInt8(data, 2);
        bb_setUInt8(data, BER_TYPE_BIT_STRING
            | BER_TYPE_OCTET_STRING);
        bb_setUInt8(data, 0x80);

        bb_setUInt8(data, (unsigned short)BER_TYPE_CONTEXT
            | (char)PDU_TYPE_MECHANISM_NAME);
        // Len
        bb_setUInt8(data, 7);
        // OBJECT IDENTIFIER
        bb_set(data, p, 6);
        bb_setUInt8(data, settings->authentication);
        // Add Calling authentication information.
        if (settings->authentication == DLMS_AUTHENTICATION_LOW)
        {
            if (settings->password.size != 0)
            {
                callingAuthenticationValue = settings->password;
            }
        }
        else
        {
            callingAuthenticationValue = settings->ctoSChallenge;
        }
        // 0xAC
        bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AUTHENTICATION_VALUE);
        // Len
        bb_setUInt8(data, (unsigned char)(2 + callingAuthenticationValue.size));
        // Add authentication information.
        bb_setUInt8(data, BER_TYPE_CONTEXT);
        // Len.
        bb_setUInt8(data, (unsigned char)callingAuthenticationValue.size);
        bb_set2(data, &callingAuthenticationValue, 0, callingAuthenticationValue.size);
    }
    return 0;
}
#endif //DLMS_IGNORE_CLIENT

/**
* Code application context name.
*
* @param settings
*            DLMS settings->
* @param data
*            Byte buffer where data is saved.
* @param cipher
*            Is ciphering settings->
*/
int apdu_generateApplicationContextName(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    //ProtocolVersion
    if (settings->protocolVersion != NULL)
    {
        unsigned char pos, len = (unsigned char)strlen(settings->protocolVersion);
        int value;
        bitArray ba;
        ba_init(&ba);
        bb_setUInt8(data, BER_TYPE_CONTEXT | PDU_TYPE_PROTOCOL_VERSION);
        bb_setUInt8(data, 2);
        bb_setUInt8(data, (unsigned char)(8 - len));
        for (pos = 0; pos != 8; ++pos)
        {
            ba_set(&ba, settings->protocolVersion[pos] == '0' ? 0 : 1);
        }
        ba_toInteger(&ba, &value);
        bb_setUInt8(data, (unsigned char)value);
        ba_clear(&ba);
    }
    unsigned char ciphered;
    // Application context name tag
    bb_setUInt8(data, (BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME));
    // Len
    bb_setUInt8(data, 0x09);
    bb_setUInt8(data, BER_TYPE_OBJECT_IDENTIFIER);
    // Len
    bb_setUInt8(data, 0x07);
#ifndef DLMS_IGNORE_HIGH_GMAC
    ciphered = isCiphered(&settings->cipher);
#else 
    ciphered = 0;
#endif //DLMS_IGNORE_HIGH_GMAC
    bb_setUInt8(data, 0x60);
    bb_setUInt8(data, 0x85);
    bb_setUInt8(data, 0x74);
    bb_setUInt8(data, 0x05);
    bb_setUInt8(data, 0x08);
    bb_setUInt8(data, 0x01);
    if (settings->useLogicalNameReferencing)
    {
        if (ciphered)
        {
            bb_setUInt8(data, 0x03);
        }
        else
        {
            bb_setUInt8(data, 0x01);
        }
    }
    else
    {
        if (ciphered)
        {
            bb_setUInt8(data, 0x04);
        }
        else
        {
            bb_setUInt8(data, 0x02);
        }
    }
    // Add system title.
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (!settings->server && (ciphered || settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC))
    {
        if (settings->cipher.systemTitle.size == 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        // Add calling-AP-title
        bb_setUInt8(data, (BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | 6));
        // LEN
        bb_setUInt8(data, (unsigned char)(2 + settings->cipher.systemTitle.size));
        bb_setUInt8(data, BER_TYPE_OCTET_STRING);
        // LEN
        bb_setUInt8(data, (unsigned char)settings->cipher.systemTitle.size);
        bb_set(data, settings->cipher.systemTitle.data, settings->cipher.systemTitle.size);
    }
#endif //DLMS_IGNORE_HIGH_GMAC

    //Add CallingAEInvocationId.
    if (!settings->server && settings->userId != -1 && settings->cipher.security != DLMS_SECURITY_NONE)
    {
        bb_setUInt8(data, (BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AE_INVOCATION_ID));
        //LEN
        bb_setUInt8(data, 3);
        bb_setUInt8(data, BER_TYPE_INTEGER);
        //LEN
        bb_setUInt8(data, 1);
        bb_setUInt8(data, (unsigned char)settings->userId);
    }
    return 0;
}

/**
 * Generate User information initiate request.
 *
 * @param settings
 *            DLMS settings->
 * @param cipher
 * @param data
 */
int apdu_getInitiateRequest(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    gxByteBuffer bb;
    // Tag for xDLMS-Initiate request
    bb_setUInt8(data, DLMS_COMMAND_INITIATE_REQUEST);
    // Usage field for the response allowed component.

#ifdef DLMS_IGNORE_HIGH_GMAC
    bb_setUInt8(data, 0);
#else
    // Usage field for dedicated-key component.
    if (settings->cipher.security == DLMS_SECURITY_NONE || settings->cipher.dedicatedKey == NULL || settings->cipher.dedicatedKey->size == 0)
    {
        bb_setUInt8(data, 0);
    }
    else
    {
        bb_setUInt8(data, 1);
        hlp_setObjectCount(settings->cipher.dedicatedKey->size, data);
        bb_set(data, settings->cipher.dedicatedKey->data, settings->cipher.dedicatedKey->size);
    }
#endif //DLMS_IGNORE_HIGH_GMAC

    // encoding of the response-allowed component (bool DEFAULT TRUE)
    // usage flag (FALSE, default value TRUE conveyed)
    bb_setUInt8(data, 0);

    // Usage field of the proposed-quality-of-service component.
    if (settings->qualityOfService == 0)
    {
        // Not used
        bb_setUInt8(data, 0x00);
    }
    else
    {
        bb_setUInt8(data, 0x01);
        bb_setUInt8(data, settings->qualityOfService);
    }

    bb_setUInt8(data, settings->dlmsVersionNumber);
    // Tag for conformance block
    bb_setUInt8(data, 0x5F);
    bb_setUInt8(data, 0x1F);
    // length of the conformance block
    bb_setUInt8(data, 0x04);
    // encoding the number of unused bits in the bit string
    bb_setUInt8(data, 0x00);

    bb_init(&bb);
    bb_setUInt32(&bb, settings->proposedConformance);
    bb_set2(data, &bb, 1, 3);
    bb_clear(&bb);
    bb_setUInt16(data, settings->maxPduSize);
    return 0;
}

#ifndef DLMS_IGNORE_CLIENT
/**
 * Generate user information.
 *
 * @param settings
 *            DLMS settings->
 * @param cipher
 * @param data
 *            Generated user information.
 */
int apdu_generateUserInformation(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    int ret = 0;
    bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION);
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (!isCiphered(&settings->cipher))
#endif //DLMS_IGNORE_HIGH_GMAC
    {
        // Length for AARQ user field
        bb_setUInt8(data, 0x10);
        // Coding the choice for user-information (Octet STRING, universal)
        bb_setUInt8(data, BER_TYPE_OCTET_STRING);
        // Length
        bb_setUInt8(data, 0x0E);
        if ((ret = apdu_getInitiateRequest(settings, data)) != 0)
        {
            return ret;
        }
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    else
    {
        gxByteBuffer tmp, crypted;
        bb_init(&tmp);
        if ((ret = apdu_getInitiateRequest(settings, &tmp)) != 0)
        {
            bb_clear(&tmp);
            return ret;
        }
        bb_init(&crypted);
        ret = cip_encrypt(
            &settings->cipher,
            settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            settings->cipher.invocationCounter,
            DLMS_COMMAND_GLO_INITIATE_REQUEST,
            &settings->cipher.systemTitle,
            &settings->cipher.blockCipherKey,
            &tmp,
            &crypted);
        if (ret == 0)
        {
            // Length for AARQ user field
            bb_setUInt8(data, (unsigned char)(2 + crypted.size));
            // Coding the choice for user-information (Octet string, universal)
            bb_setUInt8(data, BER_TYPE_OCTET_STRING);
            bb_setUInt8(data, (unsigned char)crypted.size);
            bb_set2(data, &crypted, 0, crypted.size);
        }
        bb_clear(&tmp);
        bb_clear(&crypted);
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return ret;
}
#endif //DLMS_IGNORE_CLIENT

/**
 * Parse User Information from PDU.
 */
int apdu_parseUserInformation(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    int ret;
    unsigned char tmp[3];
    gxByteBuffer bb;
    unsigned char response;
    unsigned short pduSize;
    unsigned char ch, len, tag;
    unsigned long v;
    if ((ret = bb_getUInt8(data, &len)) != 0)
    {
        return ret;
    }
    if (data->size - data->position < len)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    // Encoding the choice for user information
    if ((ret = bb_getUInt8(data, &tag)) != 0)
    {
        return ret;
    }
    if (tag != 0x4)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(data, &len)) != 0)
    {
        return ret;
    }
    // Tag for xDLMS-Initate.response
    if ((ret = bb_getUInt8(data, &tag)) != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (tag == DLMS_COMMAND_GLO_INITIATE_RESPONSE ||
        tag == DLMS_COMMAND_GLO_INITIATE_REQUEST)
    {
        data->position = (data->position - 1);
        if ((ret = cip_decrypt(&settings->cipher,
            &settings->sourceSystemTitle,
            &settings->cipher.blockCipherKey,
            data,
            &settings->cipher.security)) != 0)
        {
            return ret;
        }
        if ((ret = bb_getUInt8(data, &tag)) != 0)
        {
            return ret;
        }
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    response = tag == DLMS_COMMAND_INITIATE_RESPONSE;
    if (response)
    {
        // Optional usage field of the negotiated quality of service
        // component
        if ((ret = bb_getUInt8(data, &tag)) != 0)
        {
            return ret;
        }
        if (tag != 0)
        {
            if ((ret = bb_getUInt8(data, &settings->qualityOfService)) != 0)
            {
                return ret;
            }
        }
    }
    else if (tag == DLMS_COMMAND_INITIATE_REQUEST)
    {
        // Optional usage field of the negotiated quality of service
        // component
        if ((ret = bb_getUInt8(data, &tag)) != 0)
        {
            return ret;
        }
        // Dedicated key.
#ifdef DLMS_IGNORE_HIGH_GMAC

#else
        if (tag != 0)
        {
            if ((ret = bb_getUInt8(data, &len)) != 0)
            {
                return ret;
            }
            if (settings->cipher.dedicatedKey == NULL)
            {
                settings->cipher.dedicatedKey = gxmalloc(sizeof(gxByteBuffer));
                bb_init(settings->cipher.dedicatedKey);
            }
            else
            {
                bb_clear(settings->cipher.dedicatedKey);
            }
            bb_set2(settings->cipher.dedicatedKey, data, data->position, len);
        }
        else if (settings->cipher.dedicatedKey != NULL)
        {
            bb_clear(settings->cipher.dedicatedKey);
            gxfree(settings->cipher.dedicatedKey);
            settings->cipher.dedicatedKey = NULL;
        }
#endif //DLMS_IGNORE_HIGH_GMAC
        // Optional usage field of the negotiated quality of service
        // component
        if ((ret = bb_getUInt8(data, &tag)) != 0)
        {
            return ret;
        }
        // Skip if used.
        if (tag != 0)
        {
            if ((ret = bb_getUInt8(data, &len)) != 0)
            {
                return ret;
            }
            data->position = (data->position + len);
        }
        // Optional usage field of the proposed quality of service component
        if ((ret = bb_getUInt8(data, &tag)) != 0)
        {
            return ret;
        }
        // Skip if used.
        if (tag != 0)
        {
            if ((ret = bb_getUInt8(data, &settings->qualityOfService)) != 0)
            {
                return ret;
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    // Get DLMS version number.
    if (settings->server)
    {
        if ((ret = bb_getUInt8(data, &ch)) != 0)
        {
            return ret;
        }
        settings->dlmsVersionNumber = ch;
    }
    else
    {
        if ((ret = bb_getUInt8(data, &ch)) != 0)
        {
            return ret;
        }
        if (ch < 6)
        {
            //Invalid DLMS version number.
            return DLMS_ERROR_CODE_INVALID_VERSION_NUMBER;
        }
    }

    // Tag for conformance block
    if ((ret = bb_getUInt8(data, &tag)) != 0)
    {
        return ret;
    }
    if (tag != 0x5F)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    // Old Way...
    tag = data->data[data->position];
    if (tag == 0x1F)
    {
        data->position = (data->position + 1);
    }
    if ((ret = bb_getUInt8(data, &len)) != 0)
    {
        return ret;
    }
    // The number of unused bits in the bit string.
    if ((ret = bb_getUInt8(data, &tag)) != 0)
    {
        return ret;
    }
    bb_get(data, tmp, 3);
    bb_init(&bb);
    bb_setInt8(&bb, 0);
    bb_set(&bb, tmp, 3);
    if ((ret = bb_getUInt32(&bb, &v)) != 0)
    {
        return ret;
    }
    bb_clear(&bb);
    if (settings->server)
    {
        settings->negotiatedConformance = (DLMS_CONFORMANCE)(v & settings->proposedConformance);
    }
    else
    {
        settings->negotiatedConformance = (DLMS_CONFORMANCE)v;
    }
    if (settings->server)
    {
        if ((ret = bb_getUInt16(data, &pduSize)) != 0)
        {
            return ret;
        }
        //If client asks too high PDU.
        if (pduSize > settings->maxServerPDUSize)
        {
            pduSize = settings->maxServerPDUSize;
        }
        settings->maxPduSize = pduSize;
    }
    else
    {
        if ((ret = bb_getUInt16(data, &pduSize)) != 0)
        {
            return ret;
        }
        settings->maxPduSize = pduSize;
    }
    if (response)
    {
        // VAA Name
        unsigned short vaa;
        if ((ret = bb_getUInt16(data, &vaa)) != 0)
        {
            return ret;
        }
        if (vaa == 0x0007)
        {
            // If LN
            if (!settings->useLogicalNameReferencing)
            {
                //Invalid VAA.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
        }
        else if (vaa == 0xFA00)
        {
            // If SN
            if (settings->useLogicalNameReferencing)
            {
                //Invalid VAA.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
        }
        else
        {
            // Unknown VAA.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    return 0;
}

/**
 * Parse application context name.
 *
 * @param settings
 *            DLMS settings->
 * @param buff
 *            Received data->
 */
int apdu_parseApplicationContextName(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    unsigned char* ciphered)
{
    int ret;
    unsigned char len, ch;
    // Get length.
    if ((ret = bb_getUInt8(buff, &len)) != 0)
    {
        return ret;
    }
    if (buff->size - buff->position < len)
    {
        //Encoding failed. Not enough data->
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x6)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->server)
    {
        settings->cipher.security = DLMS_SECURITY_NONE;
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    // Object ID length.
    if ((ret = bb_getUInt8(buff, &len)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x60)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x85)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x74)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x05)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x08)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x01)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (settings->useLogicalNameReferencing)
    {
        *ciphered = ch == 3;
        if (ch == 1 || *ciphered)
        {
            return 0;
        }
        return DLMS_ERROR_CODE_FALSE;
    }
    *ciphered = ch == 4;
    if (ch == 2 || *ciphered)
    {
        return 0;
    }
    return DLMS_ERROR_CODE_FALSE;
}

int apdu_validateAare(
    dlmsSettings* settings,
    gxByteBuffer* buff)
{
    int ret;
    unsigned char tag;
    if ((ret = bb_getUInt8(buff, &tag)) != 0)
    {
        return ret;
    }
    if (settings->server)
    {
        if (tag != (BER_TYPE_APPLICATION
            | BER_TYPE_CONSTRUCTED
            | PDU_TYPE_PROTOCOL_VERSION))
        {
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
    }
    else
    {
        if (tag != (BER_TYPE_APPLICATION
            | BER_TYPE_CONSTRUCTED
            | PDU_TYPE_APPLICATION_CONTEXT_NAME))
        {
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
    }
    return 0;
}

int apdu_updatePassword(
    dlmsSettings* settings,
    gxByteBuffer* buff)
{
    int ret;
    unsigned char ch, len;
    if ((ret = bb_getUInt8(buff, &len)) != 0)
    {
        return ret;
    }

    // Get authentication information.
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x80)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(buff, &len)) != 0)
    {
        return ret;
    }
    if (settings->authentication == DLMS_AUTHENTICATION_LOW)
    {
        bb_clear(&settings->password);
        bb_set2(&settings->password, buff, buff->position, len);
    }
    else
    {
        bb_clear(&settings->ctoSChallenge);
        bb_set2(&settings->ctoSChallenge, buff, buff->position, len);
    }
    return 0;
}

int apdu_updateAuthentication(
    dlmsSettings* settings,
    gxByteBuffer* buff)
{
    int ret;
    unsigned char ch;
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }

    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x60)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x85)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x74)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x05)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x08)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x02)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = bb_getUInt8(buff, &ch)) != 0)
    {
        return ret;
    }
    if (ch > DLMS_AUTHENTICATION_HIGH_SHA256)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    //If MD5 is not used.
#ifdef DLMS_IGNORE_HIGH_MD5
    if (ch == DLMS_AUTHENTICATION_HIGH_MD5)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
#endif //DLMS_IGNORE_HIGH_MD5

    //If SHA1 is not used.
#ifdef DLMS_IGNORE_HIGH_SHA1
    if (ch == DLMS_AUTHENTICATION_HIGH_SHA1)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
#endif //DLMS_IGNORE_HIGH_SHA1

    //If SHA256 is not used.
#ifdef DLMS_IGNORE_HIGH_SHA256
    if (ch == DLMS_AUTHENTICATION_HIGH_SHA256)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
#endif //DLMS_IGNORE_HIGH_SHA256

    //If GMAC is not used.
#ifdef DLMS_IGNORE_HIGH_GMAC
    if (ch == DLMS_AUTHENTICATION_HIGH_GMAC)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    settings->authentication = (DLMS_AUTHENTICATION)ch;
    return 0;
}

int apdu_getUserInformation(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    int ret = 0;
    gxByteBuffer bb;
    bb_clear(data);
    bb_setUInt8(data, DLMS_COMMAND_INITIATE_RESPONSE); // Tag for xDLMS-Initiate
    //NegotiatedQualityOfService
    if (settings->qualityOfService == 0)
    {
        // Not used.
        bb_setUInt8(data, 0x00);
    }
    else
    {
        bb_setUInt8(data, 0x01);
        bb_setUInt8(data, settings->qualityOfService);
    }
    // DLMS Version Number
    bb_setUInt8(data, 06);
    bb_setUInt8(data, 0x5F);
    bb_setUInt8(data, 0x1F);
    // length of the conformance block
    bb_setUInt8(data, 0x04);
    // encoding the number of unused bits in the bit string
    bb_setUInt8(data, 0x00);
    bb_init(&bb);
    bb_setUInt32(&bb, settings->negotiatedConformance);
    bb_set2(data, &bb, 1, 3);
    bb_clear(&bb);
    bb_setUInt16(data, settings->maxPduSize);
    // VAA Name VAA name (0x0007 for LN referencing and 0xFA00 for SN)
    if (settings->useLogicalNameReferencing)
    {
        bb_setUInt16(data, 0x0007);
    }
    else
    {
        bb_setUInt16(data, 0xFA00);
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (isCiphered(&settings->cipher))
    {
        gxByteBuffer tmp;
        bb_init(&tmp);
        bb_set2(&tmp, data, data->position, data->size - data->position);
        bb_clear(data);
        ret = cip_encrypt(&settings->cipher,
            settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            settings->cipher.invocationCounter,
            DLMS_COMMAND_GLO_INITIATE_RESPONSE,
            &settings->cipher.systemTitle,
            &settings->cipher.blockCipherKey,
            &tmp,
            data);
        bb_clear(&tmp);
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return ret;
}
#ifndef DLMS_IGNORE_CLIENT
int apdu_generateAarq(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long offset;
#else
    unsigned short offset;
#endif
    int ret;
    // AARQ APDU Tag
    bb_setUInt8(data, BER_TYPE_APPLICATION | BER_TYPE_CONSTRUCTED);
    // Length is updated later.
    offset = data->size;
    bb_setUInt8(data, 0);
    ///////////////////////////////////////////
    // Add Application context name.
    if ((ret = apdu_generateApplicationContextName(settings, data)) != 0)
    {
        return ret;
    }
    if ((ret = apdu_getAuthenticationString(settings, data)) != 0)
    {
        return ret;
    }
    if ((ret = apdu_generateUserInformation(settings, data)) != 0)
    {
        return ret;
    }
    bb_setUInt8ByIndex(data, offset, (unsigned char)(data->size - offset - 1));
    return 0;
}
#endif //DLMS_IGNORE_CLIENT

int apdu_handleResultComponent(DLMS_SOURCE_DIAGNOSTIC value)
{
    int ret;
    switch ((int)value)
    {
    case DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN:
        ret = DLMS_ERROR_CODE_NO_REASON_GIVEN;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED:
        ret = DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED:
        ret = DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED:
        ret = DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE:
        ret = DLMS_ERROR_CODE_AUTHENTICATION_FAILURE;
        break;
    default:
        //OK.
        ret = 0;
        break;
    }
    return ret;
}

int apdu_parseProtocolVersion(dlmsSettings* settings,
    gxByteBuffer* buff)
{
    unsigned char cnt, unusedBits, value;
    int ret;
    if ((ret = bb_getUInt8(buff, &cnt)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &unusedBits)) != 0)
    {
        return ret;
    }
    if ((ret = bb_getUInt8(buff, &value)) != 0)
    {
        return ret;
    }
    bitArray bb;
    ba_init(&bb);
    ba_copy(&bb, &value, 8 - unusedBits);
    char* pv = ba_toString(&bb);
    if (strcmp(pv, "100001") != 0)
    {
        gxfree(pv);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (settings->protocolVersion != NULL)
    {
        gxfree(settings->protocolVersion);
    }
    settings->protocolVersion = pv;
    ba_clear(&bb);
    return 0;
}

int apdu_parsePDU(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    DLMS_ASSOCIATION_RESULT* result,
    DLMS_SOURCE_DIAGNOSTIC* diagnostic)
{
    unsigned char ciphered;
    unsigned short size;
    unsigned char len;
    unsigned char tag;
    int ret;
    *result = DLMS_ASSOCIATION_RESULT_ACCEPTED;
    *diagnostic = DLMS_SOURCE_DIAGNOSTIC_NONE;
    // Get AARE tag and length
    if ((ret = apdu_validateAare(settings, buff)) != 0)
    {
        return ret;
    }
    if ((ret = hlp_getObjectCount2(buff, &size)) != 0)
    {
        return ret;
    }
    if (size > buff->size - buff->position)
    {
        //Encoding failed. Not enough data->
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    while (buff->position < buff->size)
    {
        if ((ret = bb_getUInt8(buff, &tag)) != 0)
        {
            return ret;
        }
        switch (tag)
        {
            //0xA1
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME:
        {
            if ((ret = apdu_parseApplicationContextName(settings, buff, &ciphered)) != 0)
            {
                *diagnostic = DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
        }
        break;
        // 0xA2
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_TITLE:
            // Get len.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if (len != 3)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            // Choice for result.
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
                return ret;
            }
            if (settings->server)
            {
                //Ignore if client sends CalledAPTitle.
                if (tag != BER_TYPE_OCTET_STRING)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                // Get len.
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    return ret;
                }
                if (ciphered && len != 8)
                {
                    *diagnostic = DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_TITLE_NOT_RECOGNIZED;
                    *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                    return 0;
                }
                buff->position += len;
            }
            else
            {
                if (tag != BER_TYPE_INTEGER)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                // Get len.
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    return ret;
                }
                if (len != 1)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    return ret;
                }
                *result = (DLMS_ASSOCIATION_RESULT)tag;
            }
            break;
            // 0xA3 SourceDiagnostic
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AE_QUALIFIER:
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            // ACSE service user tag.
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if (settings->server)
            {
                //Ignore if client sends CalledAEQualifier.
                if (tag != BER_TYPE_OCTET_STRING)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                if (ciphered && len != 8)
                {
                    *diagnostic = DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_TITLE_NOT_RECOGNIZED;
                    *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                    return 0;
                }
                buff->position += len;
            }
            else
            {
                // Result source diagnostic component.
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    return ret;
                }
                if (tag != BER_TYPE_INTEGER)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    return ret;
                }
                if (len != 1)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    return ret;
                }
                *diagnostic = (DLMS_SOURCE_DIAGNOSTIC)tag;
            }
            break;
            // 0xA4 Result
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_INVOCATION_ID:
            // Get len.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if (settings->server)
            {
                if (len != 3)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                // ACSE service user tag.
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    return ret;
                }
                if (tag != BER_TYPE_INTEGER)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    return ret;
                }

                //Ignore if client sends CalledAEQualifier.
                buff->position += len;
            }
            else
            {
                if (len != 0xA)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                // Choice for result (Universal, Octet string type)
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    return ret;
                }
                if (tag != BER_TYPE_OCTET_STRING)
                {
                    return DLMS_ERROR_CODE_INVALID_TAG;
                }
                // responding-AP-title-field
                // Get len.
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    return ret;
                }
                bb_clear(&settings->sourceSystemTitle);
                bb_set2(&settings->sourceSystemTitle, buff, buff->position, len);
            }
            break;
            // 0xA6 Client system title.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AP_TITLE:
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if (ciphered && len != 8)
            {
                *diagnostic = DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_TITLE_NOT_RECOGNIZED;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
            bb_clear(&settings->sourceSystemTitle);
            bb_set2(&settings->sourceSystemTitle, buff, buff->position, len);
            break;
            // 0xAA Server system title.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_SENDER_ACSE_REQUIREMENTS:
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            bb_clear(&settings->stoCChallenge);
            bb_set2(&settings->stoCChallenge, buff, buff->position, len);
            break;
            //Client AEInvocationId.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AE_INVOCATION_ID:
            //Client CalledAeInvocationId.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AE_INVOCATION_ID://0xA5
            //Server RespondingAEInvocationId.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | 7://0xA7
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if (ciphered)
            {
                settings->userId = len;
            }
            break;
            //  0x8A or 0x88
        case (unsigned short)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_SENDER_ACSE_REQUIREMENTS:
        case (unsigned short)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_CALLING_AP_INVOCATION_ID:
            // Get sender ACSE-requirements field component.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
                return ret;
            }
            if (len != 2)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
                return ret;
            }
            if (tag != BER_TYPE_OBJECT_DESCRIPTOR)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            //Get only value because client app is sending system title with LOW authentication.
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
                return ret;
            }
            break;
            //  0x8B or 0x89
        case (unsigned short)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_MECHANISM_NAME:
        case (unsigned short)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_CALLING_AE_INVOCATION_ID:
            if ((ret = apdu_updateAuthentication(settings, buff)) != 0)
            {
                return ret;
            }
            if (settings->server && settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC && settings->sourceSystemTitle.size != 8)
            {
                *diagnostic = DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_TITLE_NOT_RECOGNIZED;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
            break;
            // 0xAC
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AUTHENTICATION_VALUE:
            if ((ret = apdu_updatePassword(settings, buff)) != 0)
            {
                return ret;
            }
            break;
            // 0xBE
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION:
            //Check result component. Some meters are returning invalid user-information if connection failed.
            if (*result != DLMS_ASSOCIATION_RESULT_ACCEPTED
                && *diagnostic != DLMS_SOURCE_DIAGNOSTIC_NONE)
            {
                return apdu_handleResultComponent(*diagnostic);
            }
            if ((ret = apdu_parseUserInformation(settings, buff)) != 0)
            {
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                *diagnostic = DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN;
                return 0;
            }
            break;
        case BER_TYPE_CONTEXT: //0x80
            if ((ret = apdu_parseProtocolVersion(settings, buff)) != 0)
            {
                *diagnostic = DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
            break;
        default:
            // Unknown tags.
            if (buff->position < buff->size)
            {
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    return ret;
                }
                buff->position = (buff->position + len);
            }
            break;
        }
    }
    //All meters don't send user-information if connection is failed.
    //For this reason result component is check again.
    return apdu_handleResultComponent(*diagnostic);
}

#ifndef DLMS_IGNORE_SERVER
int apdu_generateAARE(
    dlmsSettings* settings,
    gxByteBuffer* data,
    DLMS_ASSOCIATION_RESULT result,
    DLMS_SOURCE_DIAGNOSTIC diagnostic,
    gxByteBuffer* errorData,
    gxByteBuffer* encryptedData)
{
    int ret;
    gxByteBuffer tmp;
#if !defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__))
    unsigned long offset = data->size;
#else
    unsigned short offset = data->size;
#endif
    // Set AARE tag and length 0x61
    bb_setUInt8(data, BER_TYPE_APPLICATION | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME);
    // Length is updated later.
    bb_setUInt8(data, 0);
    if ((ret = apdu_generateApplicationContextName(settings, data)) != 0)
    {
        return ret;
    }
    // Result 0xA2
    bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | BER_TYPE_INTEGER);
    bb_setUInt8(data, 3); // len
    // Tag
    bb_setUInt8(data, BER_TYPE_INTEGER);
    // Choice for result (INTEGER, universal)
    bb_setUInt8(data, 1); // Len
    // ResultValue
    bb_setUInt8(data, result);
    // SourceDiagnostic
    bb_setUInt8(data, 0xA3);
    bb_setUInt8(data, 5); // len
    bb_setUInt8(data, 0xA1); // Tag
    bb_setUInt8(data, 3); // len
    bb_setUInt8(data, 2); // Tag
    // Choice for result (INTEGER, universal)
    bb_setUInt8(data, 1); // Len
    // diagnostic
    bb_setUInt8(data, diagnostic);
    // SystemTitle
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC
        || isCiphered(&settings->cipher))
    {
        bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_INVOCATION_ID);
        bb_setUInt8(data, (unsigned char)(2 + settings->cipher.systemTitle.size));
        bb_setUInt8(data, BER_TYPE_OCTET_STRING);
        bb_setUInt8(data, (unsigned char)settings->cipher.systemTitle.size);
        bb_set(data, settings->cipher.systemTitle.data, settings->cipher.systemTitle.size);
    }
#endif //LMS_IGNORE_HIGH_GMAC

    //Add CalledAEInvocationId.
    if (settings->userId != -1 && settings->cipher.security != DLMS_SECURITY_NONE)
    {
        bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AE_QUALIFIER);
        //LEN
        bb_setUInt8(data, 3);
        bb_setUInt8(data, BER_TYPE_INTEGER);
        //LEN
        bb_setUInt8(data, 1);
        bb_setUInt8(data, (unsigned char)settings->userId);
    }
    if (settings->authentication > DLMS_AUTHENTICATION_LOW)
    {
        // Add server ACSE-requirenents field component.
        bb_setUInt8(data, 0x88);
        bb_setUInt8(data, 0x02); // Len.
        bb_setUInt16(data, 0x0780);
        // Add tag.
        bb_setUInt8(data, 0x89);
        bb_setUInt8(data, 0x07); // Len
        bb_setUInt8(data, 0x60);
        bb_setUInt8(data, 0x85);
        bb_setUInt8(data, 0x74);
        bb_setUInt8(data, 0x05);
        bb_setUInt8(data, 0x08);
        bb_setUInt8(data, 0x02);
        bb_setUInt8(data, settings->authentication);
        // Add tag.
        bb_setUInt8(data, 0xAA);
        bb_setUInt8(data, (unsigned char)(2 + settings->stoCChallenge.size)); // Len
        bb_setUInt8(data, BER_TYPE_CONTEXT);
        bb_setUInt8(data, (unsigned char)settings->stoCChallenge.size);
        bb_set(data, settings->stoCChallenge.data, settings->stoCChallenge.size);
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (result == DLMS_ASSOCIATION_RESULT_ACCEPTED || !isCiphered(&settings->cipher))
#endif //DLMS_IGNORE_HIGH_GMAC
    {
        // Add User Information. Tag 0xBE
        bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION);
        bb_init(&tmp);
#ifndef DLMS_IGNORE_HIGH_GMAC
        if (encryptedData != NULL && encryptedData->size != 0)
        {
            bb_capacity(&tmp, 2 + encryptedData->size);
            bb_setUInt8(&tmp, DLMS_COMMAND_GLO_INITIATE_RESPONSE);
            hlp_setObjectCount(encryptedData->size, &tmp);
            bb_set2(&tmp, encryptedData, 0, encryptedData->size);
        }
        else
#endif //DLMS_IGNORE_HIGH_GMAC
        {
            if (errorData != NULL && errorData->size != 0)
            {
                bb_set2(&tmp, errorData, 0, errorData->size);
            }
            else
            {
                if ((ret = apdu_getUserInformation(settings, &tmp)) != 0)
                {
                    bb_clear(&tmp);
                    return ret;
                }
            }
        }

        bb_setUInt8(data, (unsigned char)(2 + tmp.size));
        // Coding the choice for user-information (Octet STRING, universal)
        bb_setUInt8(data, BER_TYPE_OCTET_STRING);
        // Length
        bb_setUInt8(data, (unsigned char)tmp.size);
        bb_set2(data, &tmp, 0, tmp.size);
        bb_clear(&tmp);
    }
    bb_setUInt8ByIndex(data, (offset + 1), (unsigned char)(data->size - offset - 2));
    return 0;
}
#endif //DLMS_IGNORE_SERVER