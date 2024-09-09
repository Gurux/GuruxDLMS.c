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
#include "../include/asn1Parser.h"
#include "../include/helpers.h"

#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

uint32_t asn1Parser_getUtcTime(const char* dateString)
{
    gxtime time;
    uint32_t year = 2000 + hlp_stringToInt2(dateString, dateString + 2);
    unsigned char month = (unsigned char)hlp_stringToInt2(dateString + 2, dateString + 4);
    unsigned char day = (unsigned char)hlp_stringToInt2(dateString + 4, dateString + 6);
    unsigned char hour = (unsigned char)hlp_stringToInt2(dateString + 6, dateString + 8);
    unsigned char minute = (unsigned char)hlp_stringToInt2(dateString + 8, dateString + 10);
    time_init(&time, year, month, day, hour, minute, 0, 0, 0);
    return time_toUnixTime2(&time);
}

int asn1Parser_fromByteArray(
    gx509Certificate* cert,
    gxByteBuffer* data,
    DLMS_X509_CERTIFICATE_DATA info,
    uint8_t level,
    uint8_t index,
    unsigned char** objectIdentifier)
{
    int ret = 0;
    uint16_t length;
    unsigned char type;
    uint8_t index2 = 0;
    if ((ret = bb_getUInt8(data, &type)) == 0 &&
        (ret = hlp_getObjectCount2(data, &length)) == 0)
    {
        if (length > bb_available(data))
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        uint32_t start = data->position;
        switch (type)
        {
        case BER_TYPE_CONSTRUCTED | BER_TYPE_SEQUENCE:
        case BER_TYPE_CONSTRUCTED | BER_TYPE_CONTEXT:
        case BER_TYPE_CONSTRUCTED | BER_TYPE_CONTEXT | 1:
        case BER_TYPE_CONSTRUCTED | BER_TYPE_CONTEXT | 2:
        case BER_TYPE_CONSTRUCTED | BER_TYPE_CONTEXT | 3:
        case BER_TYPE_CONSTRUCTED | BER_TYPE_CONTEXT | 4:
        case BER_TYPE_CONSTRUCTED | BER_TYPE_CONTEXT | 5:
        {
            ++level;
            if (cert == NULL)
            {
                if ((info == DLMS_X509_CERTIFICATE_DATA_ISSUER && level == 3 && index == 3) ||
                    (info == DLMS_X509_CERTIFICATE_DATA_SUBJECT && level == 3 && index == 5))
                {
                    //return Issuer raw.
                    data->position -= 1 + hlp_getObjectCountSizeInBytes(length);
                    data->size = 1 + hlp_getObjectCountSizeInBytes(length) + data->position + length;
                    //Return false to indicate that the value has found.
                    return DLMS_ERROR_CODE_FALSE;
                }
            }
            while (data->position < start + length)
            {
                if ((ret = asn1Parser_fromByteArray(cert, data, info, 
                    level, index2, objectIdentifier)) != 0)
                {
                    break;
                }
                ++index2;
            }
            if (level == 0 && index2 != 3)
            {
                //If not X.509 certifiate.
                return DLMS_ERROR_CODE_INVALID_CERTIFICATE;
            }
        }
        break;
        case BER_TYPE_CONTEXT:
        case BER_TYPE_CONTEXT | 1:
        case BER_TYPE_CONTEXT | 2:
        case BER_TYPE_CONTEXT | 3:
        case BER_TYPE_CONTEXT | 4:
        case BER_TYPE_CONTEXT | 5:
        case BER_TYPE_CONTEXT | 6:
            data->position += length;
            break;

        case BER_TYPE_CONSTRUCTED | BER_TYPE_SET:
            if ((ret = asn1Parser_fromByteArray(cert, data, 
                info, 1 + level, index2, objectIdentifier)) != 0)
            {
                break;
            }
            break;
        case BER_TYPE_INTEGER:
            if (length == 1)
            {
                if (level == 3 && index == 0)
                {
                    //Version must be 3 with DLMS meters.
                    if (data->data[data->position] != 2)
                    {
                        return DLMS_ERROR_CODE_INVALID_CERTIFICATE;
                    }
                }
            }
            else
            {
                if (level == 2 && index == 1)
                {
                    //Serial number.
                    if (cert != NULL)
                    {
                        cert->serialNumber.size = length / sizeof(uint32_t);
                        memset(cert->serialNumber.data + cert->serialNumber.size, 0, sizeof(uint32_t));
                        memcpy(cert->serialNumber.data, data->data + data->position, length);
                        if (length % sizeof(uint32_t) != 0)
                        {
                            ++cert->serialNumber.size;
                        }
                    }
                    else
                    {
                        if (info == DLMS_X509_CERTIFICATE_DATA_SERIAL_NUMBER)
                        {
                            data->position -= 1 + hlp_getObjectCountSizeInBytes(length);
                            data->size = 1 + hlp_getObjectCountSizeInBytes(length) + data->position + length;
                            //Return false to indicate that the value has found.
                            return DLMS_ERROR_CODE_FALSE;
                        }
                    }
                }
            }
            data->position += length;
            break;
        case BER_TYPE_OBJECT_IDENTIFIER:
            *objectIdentifier = data->data + data->position;
            if (cert != NULL && level == 3 && index == 0)
            {
                //Signature algrythm.
                static unsigned char SHA256_WITH_ECDSA[] = { 0x2A, 0x86, 0x48, 0xCE, 0x3d, 0x04, 0x03, 0x02 };
                static unsigned char SHA384_WITH_ECDSA[] = { 0x2A, 0x86, 0x48, 0xCE, 0x3d, 0x04, 0x03, 0x02 };
                if (memcmp(data->data + data->position, SHA256_WITH_ECDSA, sizeof(SHA256_WITH_ECDSA)) == 0)
                {
                    cert->signatureAlgorithm = ECC_P256;
                }
                else if (memcmp(data->data + data->position, SHA384_WITH_ECDSA, sizeof(SHA384_WITH_ECDSA)) == 0)
                {
                    cert->signatureAlgorithm = ECC_P384;
                }
                else
                {
                    ret = DLMS_ERROR_CODE_INVALID_CERTIFICATE;
                }
            }
            data->position += length;
            break;
        case BER_TYPE_UTF8_STRING:
        case BER_TYPE_PRINTABLE_STRING:
        {
            static unsigned char ID[] = { 0x55, 0x4, 0x3 };
            if (level == 5 && length == 16 && *objectIdentifier != NULL &&
                memcmp(*objectIdentifier, ID, sizeof(ID)) == 0)
            {
                if (cert != NULL)
                {
                    uint16_t count = 7;
                    // Don't handle return value because only 8 bytes are needed and 
                    // hlp_hexToBytes2 returns DLMS_ERROR_CODE_OUTOFMEMORY.
                    hlp_hexToBytes2((const char*)(data->data + data->position), cert->systemTitle, &count);
                }
                else
                {
                    if (info == DLMS_X509_CERTIFICATE_DATA_SYSTEM_TITLE)
                    {
                        data->position -= 1 + hlp_getObjectCountSizeInBytes(length);
                        data->size = 1 + hlp_getObjectCountSizeInBytes(length) + data->position + length;
                        //Return false to indicate that the value has found.
                        return DLMS_ERROR_CODE_FALSE;
                    }
                }
            }
            data->position += length;
        }
        break;
        case BER_TYPE_IA5_STRING:
            data->position += length;
            break;
        case BER_TYPE_UTC_TIME:
            if (cert != NULL)
            {
                if (level == 3 && index == 0)
                {
                    cert->validFrom = asn1Parser_getUtcTime((const char*)(data->data + data->position));
                }
                else if (level == 3 && index == 1)
                {
                    cert->validTo = asn1Parser_getUtcTime((const char*)(data->data + data->position));
                }
            }
            data->position += length;
            break;
        case BER_TYPE_GENERALIZED_TIME:
            data->position += length;
            break;
        case BER_TYPE_BOOLEAN:
            data->position += length;
            break;
        case BER_TYPE_OCTET_STRING:
        {
            type = data->data[data->position];
            if (type == (BER_TYPE_CONSTRUCTED | BER_TYPE_SEQUENCE) ||
                type == (BER_TYPE_BIT_STRING))
            {
                if ((ret = asn1Parser_fromByteArray(cert, data, info,
                    level, index, objectIdentifier)) != 0)
                {
                    break;
                }
            }
            else
            {
                data->position += length;
            }
        }
        break;
        case BER_TYPE_BIT_STRING:
            if (level == 3 && index == 1)
            {
                gxByteBuffer bb;
                //Ignore Pad bits.
                bb_attach(&bb, data->data + data->position + 1, length - 1, length - 1);
                ret = pub_fromRawBytes(&cert->publicKey, &bb);
            }
            else
            {
                static unsigned char KEYUSAGE_ID[] = { 0x55, 0x1D, 0xF };
                if (level == 5 && *objectIdentifier != NULL &&
                    memcmp(*objectIdentifier, KEYUSAGE_ID, sizeof(KEYUSAGE_ID)) == 0)
                {
                    cert->keyUsage = hlp_swapBits(*(data->data + data->position + 1));
                }
            }
            data->position += length;
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    return ret;
}

int asn1Parser_parsex509Certificate(
    gx509Certificate* cert,
    gxByteBuffer* data,
    DLMS_X509_CERTIFICATE_DATA target,
    uint8_t level,
    uint8_t index)
{
    unsigned char* objectIdentifier = NULL;
    int ret = asn1Parser_fromByteArray(cert, data, 
        target, level, index, &objectIdentifier);
    return ret;
}

#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
