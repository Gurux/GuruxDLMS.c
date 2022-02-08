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
#include "../include/serverevents.h"

#ifndef DLMS_IGNORE_CLIENT
/**
 * Retrieves the string that indicates the level of authentication, if any.
 */
int apdu_getAuthenticationString(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
    int ret = 0;
    gxByteBuffer* callingAuthenticationValue = NULL;
    if (settings->authentication != DLMS_AUTHENTICATION_NONE
#ifndef DLMS_IGNORE_HIGH_GMAC
        || settings->cipher.security != DLMS_SECURITY_NONE
#endif //DLMS_IGNORE_HIGH_GMAC
        )
    {
        unsigned char p[] = { 0x60, 0x85, 0x74, 0x05, 0x08, 0x02 };
        // Add sender ACSE-requirements field component.
        if ((ret = bb_setUInt8(data, (uint16_t)BER_TYPE_CONTEXT | (char)PDU_TYPE_SENDER_ACSE_REQUIREMENTS)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, BER_TYPE_BIT_STRING | BER_TYPE_OCTET_STRING)) != 0 ||
            (ret = bb_setUInt8(data, 0x80)) != 0 ||
            (ret = bb_setUInt8(data, (uint16_t)BER_TYPE_CONTEXT | (char)PDU_TYPE_MECHANISM_NAME)) != 0 ||
            // Len
            (ret = bb_setUInt8(data, 7)) != 0 ||
            // OBJECT IDENTIFIER
            (ret = bb_set(data, p, 6)) != 0 ||
            (ret = bb_setUInt8(data, settings->authentication)) != 0)
        {
            //Error code is returned at the end of the function.
        }
    }
    // If authentication is used.
    if (settings->authentication != DLMS_AUTHENTICATION_NONE)
    {
        // Add Calling authentication information.
        if (settings->authentication == DLMS_AUTHENTICATION_LOW)
        {
            if (settings->password.size != 0)
            {
                callingAuthenticationValue = &settings->password;
            }
        }
        else
        {
            callingAuthenticationValue = &settings->ctoSChallenge;
        }
        // 0xAC
        if ((ret = bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AUTHENTICATION_VALUE)) == 0 &&
            // Len
            (ret = bb_setUInt8(data, (unsigned char)(2 + bb_size(callingAuthenticationValue)))) == 0 &&
            // Add authentication information.
            (ret = bb_setUInt8(data, BER_TYPE_CONTEXT)) == 0 &&
            // Len.
            (ret = bb_setUInt8(data, (unsigned char)bb_size(callingAuthenticationValue))) == 0)
        {
            if (callingAuthenticationValue != NULL)
            {
                ret = bb_set(data, callingAuthenticationValue->data, bb_size(callingAuthenticationValue));
            }
        }
    }
    return ret;
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
    int ret;
    //ProtocolVersion
    if (settings->protocolVersion != 0)
    {
        if ((ret = bb_setUInt8(data, BER_TYPE_CONTEXT | PDU_TYPE_PROTOCOL_VERSION)) != 0 ||
            (ret = bb_setUInt8(data, 2)) != 0 ||
            //Un-used bits.
            (ret = bb_setUInt8(data, 2)) != 0 ||
            (ret = bb_setUInt8(data, settings->protocolVersion)) != 0)
        {
            return ret;
        }
    }
    unsigned char ciphered;
#ifndef DLMS_IGNORE_HIGH_GMAC
    ciphered = isCiphered(&settings->cipher);
#else
    ciphered = 0;
#endif //DLMS_IGNORE_HIGH_GMAC

    // Application context name tag
    if ((ret = bb_setUInt8(data, (BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME))) != 0 ||
        // Len
        (ret = bb_setUInt8(data, 0x09)) != 0 ||
        (ret = bb_setUInt8(data, BER_TYPE_OBJECT_IDENTIFIER)) != 0 ||
        // Len
        (ret = bb_setUInt8(data, 0x07)) != 0 ||
        (ret = bb_setUInt8(data, 0x60)) != 0 ||
        (ret = bb_setUInt8(data, 0x85)) != 0 ||
        (ret = bb_setUInt8(data, 0x74)) != 0 ||
        (ret = bb_setUInt8(data, 0x05)) != 0 ||
        (ret = bb_setUInt8(data, 0x08)) != 0 ||
        (ret = bb_setUInt8(data, 0x01)) != 0)
    {
        return ret;
    }

    if (settings->useLogicalNameReferencing)
    {
        if ((ret = bb_setUInt8(data, ciphered ? 0x03 : 0x01)) != 0)
        {
            return ret;
        }
    }
    else
    {
        if ((ret = bb_setUInt8(data, ciphered ? 0x04 : 0x02)) != 0)
        {
            return ret;
        }
    }
    // Add system title.
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (!settings->server && (ciphered || settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC))
    {
#ifndef DLMS_IGNORE_MALLOC
        if (settings->cipher.systemTitle.size == 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif //DLMS_IGNORE_MALLOC
        // Add calling-AP-title
        if ((ret = bb_setUInt8(data, (BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | 6))) != 0 ||
            // LEN
            (ret = bb_setUInt8(data, (unsigned char)(2 + 8))) != 0 ||
            (ret = bb_setUInt8(data, BER_TYPE_OCTET_STRING)) != 0 ||
            // LEN
            (ret = bb_setUInt8(data, (unsigned char)8)) != 0 ||
#ifndef DLMS_IGNORE_MALLOC
            (ret = bb_set(data, settings->cipher.systemTitle.data, 8)) != 0)
#else
            (ret = bb_set(data, settings->cipher.systemTitle, 8)) != 0)
#endif //DLMS_IGNORE_MALLOC
        {
            return ret;
        }
    }
    //Add CallingAEInvocationId.
    if (!settings->server && settings->userId != -1 && settings->cipher.security != DLMS_SECURITY_NONE)
    {
        if ((ret = bb_setUInt8(data, (BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AE_INVOCATION_ID))) != 0 ||
            //LEN
            (ret = bb_setUInt8(data, 3)) != 0 ||
            (ret = bb_setUInt8(data, BER_TYPE_INTEGER)) != 0 ||
            //LEN
            (ret = bb_setUInt8(data, 1)) != 0 ||
            (ret = bb_setUInt8(data, (unsigned char)settings->userId)) != 0)
        {
            return ret;
        }
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return 0;
}

#ifndef DLMS_IGNORE_HIGH_GMAC
unsigned char useDedicatedKey(dlmsSettings* settings)
{
    if (settings->cipher.dedicatedKey == NULL)
    {
        return 0;
    }
#ifndef DLMS_IGNORE_MALLOC
    return settings->cipher.dedicatedKey->size == 16;
#else
    return memcmp(settings->cipher.dedicatedKey, EMPTY_KEY, sizeof(EMPTY_KEY)) != 0;
#endif //DLMS_IGNORE_MALLOC
}
#endif //DLMS_IGNORE_HIGH_GMAC

// Reserved for internal use.
int apdu_getConformanceFromArray(gxByteBuffer* data, uint32_t* value)
{
    int ret;
    unsigned char v;
    uint32_t tmp;
    if ((ret = bb_getUInt8(data, &v)) == 0)
    {
        *value = hlp_swapBits(v);
        if ((ret = bb_getUInt8(data, &v)) == 0)
        {
            tmp = hlp_swapBits(v);
            tmp <<= 8;
            *value |= tmp;
            if ((ret = bb_getUInt8(data, &v)) == 0)
            {
                tmp = hlp_swapBits(v);
                tmp <<= 16;
                *value |= tmp;
            }
        }
    }
    return ret;
}

// Reserved for internal use.
int apdu_setConformanceToArray(uint32_t value, gxByteBuffer* data)
{
    int ret;
    if ((ret = bb_setUInt8(data, hlp_swapBits((unsigned char)(value & 0xFF)))) == 0)
    {
        value >>= 8;
        if ((ret = bb_setUInt8(data, hlp_swapBits((unsigned char)(value & 0xFF)))) == 0)
        {
            value >>= 8;
            ret = bb_setUInt8(data, hlp_swapBits((unsigned char)(value & 0xFF)));
        }
    }
    return ret;
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
    int ret;
    // Tag for xDLMS-Initiate request
    bb_setUInt8(data, DLMS_COMMAND_INITIATE_REQUEST);
    // Usage field for the response allowed component.

#ifdef DLMS_IGNORE_HIGH_GMAC
    bb_setUInt8(data, 0);
#else
    // Usage field for dedicated-key component.
    if (settings->cipher.security == DLMS_SECURITY_NONE || !useDedicatedKey(settings))
    {
        bb_setUInt8(data, 0);
    }
    else
    {
        bb_setUInt8(data, 1);
#ifndef DLMS_IGNORE_MALLOC
        hlp_setObjectCount(settings->cipher.dedicatedKey->size, data);
        bb_set(data, settings->cipher.dedicatedKey->data, settings->cipher.dedicatedKey->size);
#else
        hlp_setObjectCount(8, data);
        bb_set(data, settings->cipher.dedicatedKey, 8);
#endif //DLMS_IGNORE_MALLOC

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
    if ((ret = bb_setUInt8(data, settings->dlmsVersionNumber)) != 0 ||
        // Tag for conformance block
        (ret = bb_setUInt8(data, 0x5F)) != 0 ||
        (ret = bb_setUInt8(data, 0x1F)) != 0 ||
        // length of the conformance block
        (ret = bb_setUInt8(data, 0x04)) != 0 ||
        // encoding the number of unused bits in the bit string
        (ret = bb_setUInt8(data, 0x00)) != 0 ||
        (ret = apdu_setConformanceToArray(settings->proposedConformance, data)) != 0 ||
        (ret = bb_setUInt16(data, settings->maxPduSize)) != 0)
    {
        return ret;
    }
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
        unsigned char cmd = DLMS_COMMAND_GLO_INITIATE_REQUEST;
        gxByteBuffer crypted;
#ifndef DLMS_IGNORE_MALLOC
        BYTE_BUFFER_INIT(&crypted);
#else
        unsigned char tmp[25 + 12];
        bb_attach(&crypted, tmp, 0, sizeof(tmp));
#endif //DLMS_IGNORE_MALLOC

        if ((ret = apdu_getInitiateRequest(settings, &crypted)) != 0)
        {
            return ret;
        }
        if ((settings->proposedConformance & DLMS_CONFORMANCE_GENERAL_PROTECTION) != 0)
        {
            if (!useDedicatedKey(settings))
            {
                cmd = DLMS_COMMAND_GENERAL_GLO_CIPHERING;
            }
            else
            {
                cmd = DLMS_COMMAND_GENERAL_DED_CIPHERING;
            }
        }
#ifndef DLMS_IGNORE_MALLOC
        ret = cip_encrypt(
            &settings->cipher,
            settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            settings->cipher.invocationCounter,
            cmd,
            settings->cipher.systemTitle.data,
            &settings->cipher.blockCipherKey,
            &crypted);
#else
        ret = cip_encrypt(
            &settings->cipher,
            settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            settings->cipher.invocationCounter,
            cmd,
            settings->cipher.systemTitle,
            settings->cipher.blockCipherKey,
            &crypted);
#endif //DLMS_IGNORE_MALLOC
        if (ret == 0)
        {
            ++settings->cipher.invocationCounter;
            // Length for AARQ user field
            if ((ret = bb_setUInt8(data, (unsigned char)(2 + crypted.size))) != 0 ||
                // Coding the choice for user-information (Octet string, universal)
                (ret = bb_setUInt8(data, BER_TYPE_OCTET_STRING)) != 0 ||
                (ret = bb_setUInt8(data, (unsigned char)crypted.size)) != 0 ||
                (ret = bb_set2(data, &crypted, 0, crypted.size)) != 0)
            {
                //Error code is returned at the end of the function.
            }
        }
#ifndef DLMS_IGNORE_MALLOC
        bb_clear(&crypted);
#endif //DLMS_IGNORE_MALLOC
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
    gxByteBuffer* data,
    unsigned char ciphered,
    unsigned char* command)
{
    int ret;
    unsigned char response;
    uint16_t pduSize;
    unsigned char ch, len, tag;
    uint32_t v;
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
    DLMS_SECURITY security;
    DLMS_SECURITY_SUITE suite;
    uint64_t invocationCounter;
    if (tag == DLMS_COMMAND_GLO_INITIATE_RESPONSE ||
        tag == DLMS_COMMAND_GLO_INITIATE_REQUEST ||
        tag == DLMS_COMMAND_DED_INITIATE_RESPONSE ||
        tag == DLMS_COMMAND_DED_INITIATE_REQUEST ||
        tag == DLMS_COMMAND_GENERAL_GLO_CIPHERING ||
        tag == DLMS_COMMAND_GENERAL_DED_CIPHERING)
    {
        *command = (unsigned char)tag;
        data->position = (data->position - 1);
#ifndef DLMS_IGNORE_MALLOC
        if ((ret = cip_decrypt(&settings->cipher,
            settings->sourceSystemTitle,
            &settings->cipher.blockCipherKey,
            data,
            &security,
            &suite,
            &invocationCounter)) != 0)
        {
            return ret;
        }
#else
        if ((ret = cip_decrypt(&settings->cipher,
            settings->sourceSystemTitle,
            settings->cipher.blockCipherKey,
            data,
            &security,
            &suite,
            &invocationCounter)) != 0)
        {
            return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
        }
#endif //DLMS_IGNORE_MALLOC

        if (settings->expectedSecurityPolicy != 0xFF && security != settings->expectedSecurityPolicy << 4)
        {
            return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
        }
        if (settings->expectedSecuritySuite != 0xFF && suite != settings->expectedSecuritySuite)
        {
            return DLMS_ERROR_CODE_INVALID_SECURITY_SUITE;
        }
        if (settings->expectedInvocationCounter != NULL)
        {
            if (invocationCounter < 1 + *settings->expectedInvocationCounter)
            {
                return DLMS_ERROR_CODE_INVOCATION_COUNTER_TOO_SMALL;
            }
#ifdef DLMS_COSEM_INVOCATION_COUNTER_SIZE64
            *settings->expectedInvocationCounter = 1 + invocationCounter;
#else
            *settings->expectedInvocationCounter = (uint32_t)(1 + invocationCounter);
#endif //DLMS_COSEM_INVOCATION_COUNTER_SIZE64

        }
        //If client system title doesn't match.
        if (settings->expectedClientSystemTitle != NULL &&
            memcmp(settings->expectedClientSystemTitle, EMPTY_SYSTEM_TITLE, 8) != 0 &&
            memcmp(settings->sourceSystemTitle, settings->expectedClientSystemTitle, 8) != 0)
        {
            return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
        }

        settings->cipher.security = security;
        settings->cipher.suite = suite;
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
            //Return error if ciphering is not used.
            if (!ciphered || settings->cipher.security != DLMS_SECURITY_AUTHENTICATION_ENCRYPTION)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            if ((ret = bb_getUInt8(data, &len)) != 0)
            {
                return ret;
            }
            if (len != 16)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
#ifndef DLMS_IGNORE_MALLOC
            if (settings->cipher.dedicatedKey == NULL)
            {
                settings->cipher.dedicatedKey = gxmalloc(sizeof(gxByteBuffer));
                BYTE_BUFFER_INIT(settings->cipher.dedicatedKey);
            }
            else
            {
                bb_clear(settings->cipher.dedicatedKey);
            }
            bb_set2(settings->cipher.dedicatedKey, data, data->position, len);
#else
            hlp_setObjectCount(8, data);
            bb_get(data, settings->cipher.dedicatedKey, len);
#endif //DLMS_IGNORE_MALLOC
        }
        else
        {
#ifndef DLMS_IGNORE_MALLOC
            if (settings->cipher.dedicatedKey != NULL)
            {
                bb_clear(settings->cipher.dedicatedKey);
                gxfree(settings->cipher.dedicatedKey);
                settings->cipher.dedicatedKey = NULL;
            }
#else
            memset(settings->cipher.dedicatedKey, 0, 8);
#endif //DLMS_IGNORE_MALLOC
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
            if ((ret = bb_getUInt8(data, &tag)) != 0)
            {
                return ret;
            }
        }
        // Optional usage field of the proposed quality of service component
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
    if ((ret = apdu_getConformanceFromArray(data, &v)) != 0)
    {
        return ret;
    }
    if (settings->server)
    {
        settings->negotiatedConformance = (DLMS_CONFORMANCE)(v & settings->proposedConformance);
        //Remove general protection if ciphered connection is not used.
        if (!ciphered && (settings->negotiatedConformance & DLMS_CONFORMANCE_GENERAL_PROTECTION) != 0)
        {
            settings->negotiatedConformance &= ~DLMS_CONFORMANCE_GENERAL_PROTECTION;
        }
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
        uint16_t vaa;
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
    if ((ret = bb_getUInt8(buff, &len)) == 0 &&
        // Get authentication information.
        (ret = bb_getUInt8(buff, &ch)) == 0)
    {
        if (ch != 0x80)
        {
            ret = DLMS_ERROR_CODE_INVALID_TAG;
        }
        else if ((ret = bb_getUInt8(buff, &len)) == 0)
        {
            if (settings->authentication == DLMS_AUTHENTICATION_LOW)
            {
                if ((ret = bb_clear(&settings->password)) != 0 ||
                    (ret = bb_set2(&settings->password, buff, buff->position, len)) != 0)
                {
                    //Error code is returned at the end of the function.
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("updating password failed. "), ret);
#endif //DLMS_DEBUG
                }
            }
            else
            {
                if (len < 8 || len > 64)
                {
                    ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                else
                {
                    if ((ret = bb_clear(&settings->ctoSChallenge)) != 0 ||
                        (ret = bb_set2(&settings->ctoSChallenge, buff, buff->position, len)) != 0)
                    {
                        //Error code is returned at the end of the function.
                    }
                }
            }
        }
    }
    return ret;
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
    switch (ch)
    {
    case DLMS_AUTHENTICATION_NONE:
    case DLMS_AUTHENTICATION_LOW:
    case DLMS_AUTHENTICATION_HIGH:
#ifndef DLMS_IGNORE_HIGH_MD5
    case DLMS_AUTHENTICATION_HIGH_MD5:
#endif //DLMS_IGNORE_HIGH_MD5
#ifndef DLMS_IGNORE_HIGH_SHA1
    case DLMS_AUTHENTICATION_HIGH_SHA1:
#endif //DLMS_IGNORE_HIGH_SHA1
#ifndef DLMS_IGNORE_HIGH_SHA256
    case DLMS_AUTHENTICATION_HIGH_SHA256:
#endif //DLMS_IGNORE_HIGH_SHA256
#ifndef DLMS_IGNORE_HIGH_GMAC
    case DLMS_AUTHENTICATION_HIGH_GMAC:
#endif //DLMS_IGNORE_HIGH_GMAC
        break;
    default:
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    settings->authentication = (DLMS_AUTHENTICATION)ch;
    return 0;
}

int apdu_getUserInformation(
    dlmsSettings* settings,
    gxByteBuffer* data,
    unsigned char command)
{
    int ret = 0;
    // Tag for xDLMS-Initiate
    if ((ret = bb_setUInt8(data, DLMS_COMMAND_INITIATE_RESPONSE)) != 0)
    {
        return ret;
    }
    //NegotiatedQualityOfService
    if (settings->qualityOfService == 0)
    {
        // Not used.
        if ((ret = bb_setUInt8(data, 0x00)) != 0)
        {
            return ret;
        }
    }
    else
    {
        if ((ret = bb_setUInt8(data, 0x01)) != 0 ||
            (ret = bb_setUInt8(data, settings->qualityOfService)) != 0)
        {
            return ret;
        }
    }
    // DLMS Version Number
    if ((ret = bb_setUInt8(data, 06)) != 0 ||
        (ret = bb_setUInt8(data, 0x5F)) != 0 ||
        (ret = bb_setUInt8(data, 0x1F)) != 0 ||
        // length of the conformance block
        (ret = bb_setUInt8(data, 0x04)) != 0 ||
        // encoding the number of unused bits in the bit string
        (ret = bb_setUInt8(data, 0x00)) != 0 ||
        (ret = apdu_setConformanceToArray(settings->negotiatedConformance, data)) != 0 ||
        (ret = bb_setUInt16(data, settings->maxPduSize)) != 0)
    {
        return ret;
    }
    // VAA Name VAA name (0x0007 for LN referencing and 0xFA00 for SN)
    if ((ret = bb_setUInt16(data, settings->useLogicalNameReferencing ? 0x0007 : 0xFA00)) != 0)
    {
        return ret;
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (isCiphered(&settings->cipher))
    {
        unsigned char cmd;
        if (command == DLMS_COMMAND_GENERAL_DED_CIPHERING)
        {
            cmd = DLMS_COMMAND_GENERAL_DED_CIPHERING;
        }
        else if (command == DLMS_COMMAND_GENERAL_GLO_CIPHERING)
        {
            cmd = DLMS_COMMAND_GENERAL_GLO_CIPHERING;
        }
        else
        {
            cmd = DLMS_COMMAND_GLO_INITIATE_RESPONSE;
        }
#ifndef DLMS_IGNORE_MALLOC
        ret = cip_encrypt(&settings->cipher,
            settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            settings->cipher.invocationCounter,
            cmd,
            settings->cipher.systemTitle.data,
            &settings->cipher.blockCipherKey,
            data);
#else
        ret = cip_encrypt(&settings->cipher,
            settings->cipher.security,
            DLMS_COUNT_TYPE_PACKET,
            settings->cipher.invocationCounter,
            cmd,
            settings->cipher.systemTitle,
            settings->cipher.blockCipherKey,
            data);
#endif //DLMS_IGNORE_MALLOC
    }
#endif //DLMS_IGNORE_HIGH_GMAC
    return ret;
}
#ifndef DLMS_IGNORE_CLIENT
int apdu_generateAarq(
    dlmsSettings* settings,
    gxByteBuffer* data)
{
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t offset;
#else
    uint16_t offset;
#endif
    int ret;
    // Length is updated later.
    offset = data->size + 1;
    // AARQ APDU Tag
    if ((ret = bb_setUInt8(data, BER_TYPE_APPLICATION | BER_TYPE_CONSTRUCTED)) == 0 &&
        (ret = bb_setUInt8(data, 0)) == 0 &&
        ///////////////////////////////////////////
        // Add Application context name.
        (ret = apdu_generateApplicationContextName(settings, data)) == 0 &&
        (ret = apdu_getAuthenticationString(settings, data)) == 0 &&
        (ret = apdu_generateUserInformation(settings, data)) == 0)
    {
        return bb_setUInt8ByIndex(data, offset, (unsigned char)(data->size - offset - 1));
    }
    return ret;
}
#endif //DLMS_IGNORE_CLIENT

int apdu_handleResultComponent(unsigned char value)
{
    int ret;
    switch (value)
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
    if (unusedBits > 8)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_getUInt8(buff, &value)) != 0)
    {
        return ret;
    }
    //Protocol version must be 100001.
    //This is not checked in client side because some meters are returning wrong value here.
    if (settings->server && value != 0x84)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    settings->protocolVersion = value;
    return 0;
}

int apdu_parsePDU(
    dlmsSettings* settings,
    gxByteBuffer* buff,
    DLMS_ASSOCIATION_RESULT* result,
    unsigned char* diagnostic,
    unsigned char* command)
{
    unsigned char ciphered = 0;
    uint16_t size;
    unsigned char len;
    unsigned char tag;
    int ret;
    *result = DLMS_ASSOCIATION_RESULT_ACCEPTED;
    *diagnostic = DLMS_SOURCE_DIAGNOSTIC_NONE;
#ifndef DLMS_IGNORE_SERVER
    typedef enum
    {
        DLMS_AFU_MISSING_NONE = 0x0,
        DLMS_AFU_MISSING_SENDER_ACSE_REQUIREMENTS = 0x1,
        DLMS_AFU_MISSING_MECHANISM_NAME = 0x2,
        DLMS_AFU_MISSING_CALLING_AUTHENTICATION_VALUE = 0x4
    }
    DLMS_AFU_MISSING;
    DLMS_AFU_MISSING afu = DLMS_AFU_MISSING_NONE;
#endif //DLMS_IGNORE_SERVER
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
    while (ret == 0 && buff->position < buff->size)
    {
        if ((ret = bb_getUInt8(buff, &tag)) != 0)
        {
            break;
        }
        switch (tag)
        {
            //0xA1
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME:
        {
            if ((ret = apdu_parseApplicationContextName(settings, buff, &ciphered)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("apdu_parseApplicationContextName "), ret);
#endif //DLMS_DEBUG
                * diagnostic = DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
#ifndef DLMS_IGNORE_SERVER
            if (ciphered)
            {
                afu = (DLMS_AFU_MISSING)(DLMS_AFU_MISSING_SENDER_ACSE_REQUIREMENTS | DLMS_AFU_MISSING_MECHANISM_NAME | DLMS_AFU_MISSING_CALLING_AUTHENTICATION_VALUE);
            }
#endif //DLMS_IGNORE_SERVER
        }
        break;
        // 0xA2
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_TITLE:
            // Get len.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP title "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (len != 3)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP title "), -1);
#endif //DLMS_DEBUG
                ret = DLMS_ERROR_CODE_INVALID_TAG;
                break;
            }
            // Choice for result.
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP title "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (settings->server)
            {
                //Ignore if client sends CalledAPTitle.
                if (tag != BER_TYPE_OCTET_STRING)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP title "), -1);
#endif //DLMS_DEBUG
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                // Get len.
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP title "), -1);
#endif //DLMS_DEBUG
                    break;
                }
                buff->position += len;
            }
            else
            {
                if (tag != BER_TYPE_INTEGER)
                {
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                // Get len.
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    break;
                }
                if (len != 1)
                {
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    break;
                }
                *result = (DLMS_ASSOCIATION_RESULT)tag;
            }
            break;
            // 0xA3 SourceDiagnostic
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AE_QUALIFIER:
            if ((ret = bb_getUInt8(buff, &len)) != 0 ||
                // ACSE service user tag.
                (ret = bb_getUInt8(buff, &tag)) != 0 ||
                (ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE qualifier. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (settings->server)
            {
                //Ignore if client sends CalledAEQualifier.
                if (tag != BER_TYPE_OCTET_STRING)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE qualifier. "), -1);
#endif //DLMS_DEBUG
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                buff->position += len;
            }
            else
            {
                // Result source diagnostic component.
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    break;
                }
                if (tag != BER_TYPE_INTEGER)
                {
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    break;
                }
                if (len != 1)
                {
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    break;
                }
                *diagnostic = (DLMS_SOURCE_DIAGNOSTIC)tag;
            }
            break;
            // 0xA4 Result
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_INVOCATION_ID:
            // Get len.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP invocationID. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (settings->server)
            {
                if (len != 3)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP invocationID. "), -1);
#endif //DLMS_DEBUG
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                // ACSE service user tag.
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP invocationID. "), -1);
#endif //DLMS_DEBUG
                    break;
                }
                if (tag != BER_TYPE_INTEGER)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP invocationID. "), -1);
#endif //DLMS_DEBUG
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AP invocationID. "), -1);
#endif //DLMS_DEBUG
                    break;
                }
                //Ignore if client sends CalledAEQualifier.
                buff->position += len;
            }
            else
            {
                if (len != 0xA)
                {
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                // Choice for result (Universal, Octet string type)
                if ((ret = bb_getUInt8(buff, &tag)) != 0)
                {
                    break;
                }
                if (tag != BER_TYPE_OCTET_STRING)
                {
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                // responding-AP-title-field
                // Get len.
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    break;
                }
                if ((ret = bb_get(buff, settings->sourceSystemTitle, len)) != 0)
                {
                    break;
                }
                //If system title is invalid.
                if (len != 8)
                {
                    memset(settings->sourceSystemTitle, 0, 8);
                }
            }
            break;
            // 0xA6 Client system title.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AP_TITLE:
            if ((ret = bb_getUInt8(buff, &len)) != 0 ||
                (ret = bb_getUInt8(buff, &tag)) != 0 ||
                (ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid client system title. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (ciphered && len != 8)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid client system title. "), -1);
#endif //DLMS_DEBUG
                * diagnostic = DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_TITLE_NOT_RECOGNIZED;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
            if ((ret = bb_get(buff, settings->sourceSystemTitle, len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid client system title. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            break;
            // 0xAA Server system title.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_SENDER_ACSE_REQUIREMENTS:
            if ((ret = bb_getUInt8(buff, &len)) != 0 ||
                (ret = bb_getUInt8(buff, &tag)) != 0 ||
                (ret = bb_getUInt8(buff, &len)) != 0 ||
                (ret = bb_clear(&settings->stoCChallenge)) != 0 ||
                (ret = bb_set2(&settings->stoCChallenge, buff, buff->position, len)) != 0)
            {
                break;
            }
            break;
            //Client AE Invocation id.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AE_INVOCATION_ID:
            if ((ret = bb_getUInt8(buff, &len)) != 0 ||
                (ret = bb_getUInt8(buff, &tag)) != 0 ||
                (ret = bb_getUInt8(buff, &len)) != 0 ||
                (ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            break;
            //Client CalledAeInvocationId.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AE_INVOCATION_ID://0xA5
            if (settings->server)
            {
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                    break;
                }
                //Invalid length.
                if (len != 3)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                    break;
                }
                //Invalid length.
                if (len != 2)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                    break;
                }
                //Invalid tag length.
                if (len != 1)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                    break;
                }
                //Get value.
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                    break;
                }
            }
            else
            {
                if ((ret = bb_getUInt8(buff, &len)) != 0 ||
                    (ret = bb_getUInt8(buff, &tag)) != 0 ||
                    (ret = bb_getUInt8(buff, &len)) != 0 ||
                    (ret = bb_getUInt8(buff, &len)) != 0)
                {
                    break;
                }
                if (ciphered)
                {
                    settings->userId = len;
                }
            }
            break;
            //Server RespondingAEInvocationId.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AE_QUALIFIER://0xA7
            if ((ret = bb_getUInt8(buff, &len)) != 0 ||
                (ret = bb_getUInt8(buff, &tag)) != 0 ||
                (ret = bb_getUInt8(buff, &len)) != 0 ||
                (ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Responding AE Invocation ID. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (ciphered && len == 0)
            {
                settings->userId = len;
            }
            break;
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AP_INVOCATION_ID://0xA8
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Calling AP Invocation ID. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (tag != 3)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Calling AP Invocation ID. "), -1);
#endif //DLMS_DEBUG
                ret = DLMS_ERROR_CODE_INVALID_TAG;
                break;
            }
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Calling AP Invocation ID. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (len != 2)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Calling AP Invocation ID. "), -1);
#endif //DLMS_DEBUG
                ret = DLMS_ERROR_CODE_INVALID_TAG;
                break;
            }
            //Invalid tag length.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Calling AP Invocation ID. "), -1);
#endif //DLMS_DEBUG
                return ret;
            }
            if (len != 1)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Calling AP Invocation ID. "), -1);
#endif //DLMS_DEBUG
                ret = DLMS_ERROR_CODE_INVALID_TAG;
                break;
            }
            //Get value.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid Calling AP Invocation ID. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            break;
            //  0x8A or 0x88
        case (uint16_t)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_SENDER_ACSE_REQUIREMENTS:
        case (uint16_t)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_CALLING_AP_INVOCATION_ID:
            // Get sender ACSE-requirements field component.
            if ((ret = bb_getUInt8(buff, &len)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid sender ACSE-requirements field. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (len != 2)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid sender ACSE-requirements field. "), -1);
#endif //DLMS_DEBUG
                ret = DLMS_ERROR_CODE_INVALID_TAG;
                break;
            }
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid sender ACSE-requirements field. "), -1);
#endif //DLMS_DEBUG
                break;
            }
            if (tag != BER_TYPE_OBJECT_DESCRIPTOR)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid sender ACSE-requirements field. "), -1);
#endif //DLMS_DEBUG
                ret = DLMS_ERROR_CODE_INVALID_TAG;
                break;
            }
            //Get only value because client app is sending system title with LOW authentication.
            if ((ret = bb_getUInt8(buff, &tag)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid sender ACSE-requirements field. "), -1);
#endif //DLMS_DEBUG
                break;
            }
#ifndef DLMS_IGNORE_SERVER
            if (ciphered && tag == 0x80)
            {
                afu &= ~DLMS_AFU_MISSING_SENDER_ACSE_REQUIREMENTS;
            }
#endif //DLMS_IGNORE_SERVER
            break;
            //  0x8B or 0x89
        case (uint16_t)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_MECHANISM_NAME:
        case (uint16_t)BER_TYPE_CONTEXT | (unsigned char)PDU_TYPE_CALLING_AE_INVOCATION_ID:
            if ((ret = apdu_updateAuthentication(settings, buff)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid mechanism name. "), ret);
#endif //DLMS_DEBUG
                break;
            }
#ifndef DLMS_IGNORE_HIGH_GMAC
            unsigned char invalidSystemTitle;
            invalidSystemTitle = memcmp(settings->sourceSystemTitle, EMPTY_SYSTEM_TITLE, 8) == 0;
            if (settings->server && settings->authentication == DLMS_AUTHENTICATION_HIGH_GMAC && invalidSystemTitle)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid mechanism name. "), -1);
#endif //DLMS_DEBUG
                * diagnostic = DLMS_SOURCE_DIAGNOSTIC_CALLING_AP_TITLE_NOT_RECOGNIZED;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
#endif //DLMS_IGNORE_HIGH_GMAC
#ifndef DLMS_IGNORE_SERVER
            if (ciphered)
            {
                afu &= ~DLMS_AFU_MISSING_MECHANISM_NAME;
            }
#endif //DLMS_IGNORE_SERVER
            break;
            // 0xAC
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AUTHENTICATION_VALUE:
            if ((ret = apdu_updatePassword(settings, buff)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid password. "), ret);
#endif //DLMS_DEBUG
                break;
            }
#ifndef DLMS_IGNORE_SERVER
            if (ciphered)
            {
                afu &= ~DLMS_AFU_MISSING_CALLING_AUTHENTICATION_VALUE;
            }
#endif //DLMS_IGNORE_SERVER
            break;
            // 0xBE
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION:
            //Check result component. Some meters are returning invalid user-information if connection failed.
            if (*result != DLMS_ASSOCIATION_RESULT_ACCEPTED
                && *diagnostic != DLMS_SOURCE_DIAGNOSTIC_NONE)
            {
                if ((ret = apdu_handleResultComponent(*diagnostic)) != 0)
                {
#ifdef DLMS_DEBUG
                    svr_notifyTrace(GET_STR_FROM_EEPROM("Invalid result component. "), ret);
#endif //DLMS_DEBUG
                }
                return ret;
            }
            if ((ret = apdu_parseUserInformation(settings, buff, ciphered, command)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("parseUserInformation. "), ret);
#endif //DLMS_DEBUG
                if (ret == DLMS_ERROR_CODE_INVOCATION_COUNTER_TOO_SMALL ||
                    ret == DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR ||
                    ret == DLMS_ERROR_CODE_INVALID_SECURITY_SUITE)
                {
                    return ret;
                }
                if (ciphered)
                {
                    ret = DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
                }
                else
                {
                    //Return confirmed service error.
                    ret = DLMS_ERROR_CODE_INVALID_TAG;
                }
                break;
            }
            break;
        case BER_TYPE_CONTEXT: //0x80
            if ((ret = apdu_parseProtocolVersion(settings, buff)) != 0)
            {
#ifdef DLMS_DEBUG
                svr_notifyTrace(GET_STR_FROM_EEPROM("parseProtocolVersion. "), ret);
#endif //DLMS_DEBUG
                * diagnostic = 0x80 | DLMS_ACSE_SERVICE_PROVIDER_NO_COMMON_ACSE_VERSION;
                *result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
                return 0;
            }
            break;
        default:
            // Unknown tags.
#ifdef DLMS_DEBUG
            svr_notifyTrace(GET_STR_FROM_EEPROM("Unknown tag. "), -1);
#endif //DLMS_DEBUG
            if (buff->position < buff->size)
            {
                if ((ret = bb_getUInt8(buff, &len)) != 0)
                {
                    break;
                }
                buff->position = (buff->position + len);
            }
            break;
        }
    }
    if (ret == 0)
    {
#ifndef DLMS_IGNORE_SERVER
        if (settings->server && afu != 0 && 
            *result == DLMS_ASSOCIATION_RESULT_ACCEPTED &&
            !(
                afu == DLMS_AFU_MISSING_CALLING_AUTHENTICATION_VALUE &&
                settings->authentication == DLMS_AUTHENTICATION_NONE))
        {
#ifdef DLMS_DEBUG
            switch (afu)
            {
            case DLMS_AFU_MISSING_SENDER_ACSE_REQUIREMENTS:
                svr_notifyTrace(GET_STR_FROM_EEPROM("Sender ACSE requirements is missing."), -1);
                break;
            case DLMS_AFU_MISSING_MECHANISM_NAME:
                svr_notifyTrace(GET_STR_FROM_EEPROM("Mechanism name is missing."), -1);
                break;
            case DLMS_AFU_MISSING_CALLING_AUTHENTICATION_VALUE:
                svr_notifyTrace(GET_STR_FROM_EEPROM("Calling authentication value is missing."), -1);
                break;
            case DLMS_AFU_MISSING_NONE:
                break;
            }
#endif //DLMS_DEBUG
            * result = DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED;
            *diagnostic = DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
            return 0;
        }
#endif //DLMS_IGNORE_SERVER
        //All meters don't send user-information if connection is failed.
        //For this reason result component is check again.
        if ((ret = apdu_handleResultComponent(*diagnostic)) != 0)
        {
#ifdef DLMS_DEBUG
            svr_notifyTrace(GET_STR_FROM_EEPROM("handleResultComponent."), ret);
#endif //DLMS_DEBUG
        }
#ifndef DLMS_IGNORE_HIGH_GMAC
        //Check that user is not trying to connect without ciphered connection.
        if (ret == 0 && settings->expectedSecurityPolicy != 0xFF)
        {
            if (settings->cipher.security != settings->expectedSecurityPolicy << 4)
            {
                return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
            }
        }
#endif //DLMS_IGNORE_HIGH_GMAC
    }
    return ret;
}

#ifndef DLMS_IGNORE_SERVER
int apdu_generateAARE(
    dlmsSettings* settings,
    gxByteBuffer* data,
    DLMS_ASSOCIATION_RESULT result,
    unsigned char diagnostic,
    gxByteBuffer* errorData,
    gxByteBuffer* encryptedData,
    unsigned char command)
{
    int ret;
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t offset = data->size;
#else
    uint16_t offset = data->size;
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
    // len
    bb_setUInt8(data, 5);
    // Tag
    if ((diagnostic & 0x80) == 0)
    {
        bb_setUInt8(data, 0xA1);
    }
    else
    {
        bb_setUInt8(data, 0xA2);
        diagnostic &= ~0x80;
    }
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
        bb_setUInt8(data, 2 + 8);
        bb_setUInt8(data, BER_TYPE_OCTET_STRING);
        bb_setUInt8(data, 8);
#ifndef DLMS_IGNORE_MALLOC
        bb_set(data, settings->cipher.systemTitle.data, 8);
#else
        bb_set(data, settings->cipher.systemTitle, 8);
#endif //DLMS_IGNORE_MALLOC
    }
#endif //LMS_IGNORE_HIGH_GMAC

    //Add CalledAEInvocationId.
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (settings->userId != -1 && settings->cipher.security != DLMS_SECURITY_NONE)
#else
    if (settings->userId != -1)
#endif
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
        if (settings->stoCChallenge.size == 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        // Add server ACSE-requirenents field component.
        if ((ret = bb_setUInt8(data, 0x88)) != 0 ||
            (ret = bb_setUInt8(data, 0x02)) != 0 || // Len.
            (ret = bb_setUInt16(data, 0x0780)) != 0 ||
            // Add tag.
            (ret = bb_setUInt8(data, 0x89)) != 0 ||
            (ret = bb_setUInt8(data, 0x07)) != 0 || // Len
            (ret = bb_setUInt8(data, 0x60)) != 0 ||
            (ret = bb_setUInt8(data, 0x85)) != 0 ||
            (ret = bb_setUInt8(data, 0x74)) != 0 ||
            (ret = bb_setUInt8(data, 0x05)) != 0 ||
            (ret = bb_setUInt8(data, 0x08)) != 0 ||
            (ret = bb_setUInt8(data, 0x02)) != 0 ||
            (ret = bb_setUInt8(data, settings->authentication)) != 0 ||
            // Add tag.
            (ret = bb_setUInt8(data, 0xAA)) != 0 ||
            (ret = bb_setUInt8(data, (unsigned char)(2 + settings->stoCChallenge.size))) != 0 || // Len
            (ret = bb_setUInt8(data, BER_TYPE_CONTEXT)) != 0 ||
            (ret = bb_setUInt8(data, (unsigned char)settings->stoCChallenge.size)) != 0 ||
            (ret = bb_set(data, settings->stoCChallenge.data, settings->stoCChallenge.size)) != 0)
        {
            return ret;
        }
    }
#ifndef DLMS_IGNORE_HIGH_GMAC
    if (result == DLMS_ASSOCIATION_RESULT_ACCEPTED || !isCiphered(&settings->cipher))
#endif //DLMS_IGNORE_HIGH_GMAC
    {
        gxByteBuffer tmp;
        unsigned char buff[200];
        // Add User Information. Tag 0xBE
        bb_setUInt8(data, BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION);
        bb_attach(&tmp, buff, 0, sizeof(buff));
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
                if ((ret = apdu_getUserInformation(settings, &tmp, command)) != 0)
                {
                    bb_clear(&tmp);
                    return ret;
                }
            }
        }

        if ((ret = bb_setUInt8(data, (unsigned char)(2 + tmp.size))) != 0 ||
            // Coding the choice for user-information (Octet STRING, universal)
            (ret = bb_setUInt8(data, BER_TYPE_OCTET_STRING)) != 0 ||
            // Length
            (ret = bb_setUInt8(data, (unsigned char)tmp.size)) != 0 ||
            (ret = bb_set2(data, &tmp, 0, tmp.size)) != 0)
        {
            return ret;
        }
    }
    return bb_setUInt8ByIndex(data, (offset + 1), (unsigned char)(data->size - offset - 2));
}
#endif //DLMS_IGNORE_SERVER
