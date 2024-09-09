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

#include <string.h>
#include "../include/gx509Certificate.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#include "../include/gxdefine.h"
#include "../include/helpers.h"
#include "../include/errorcodes.h"
#include "../include/asn1Parser.h"

void gx509Certificate_init(gx509Certificate* cert)
{
    cert->keyUsage = DLMS_KEY_USAGE_NONE;
    memset(cert->systemTitle, 0, sizeof(cert->systemTitle));
    cert->signatureAlgorithm = ECC_P256;
    cert->validFrom = cert->validTo = 0;
    bi_init(&cert->serialNumber);
    pub_init(&cert->publicKey);
}

void gx509Certificate_clear(gx509Certificate* cert)
{
    cert->keyUsage = DLMS_KEY_USAGE_NONE;
    memset(cert->systemTitle, 0, sizeof(cert->systemTitle));
    cert->signatureAlgorithm = ECC_P256;
    cert->validFrom = cert->validTo = 0;
    bi_clear(&cert->serialNumber);
    pub_clear(&cert->publicKey);
}

int gx509Certificate_validate(gx509Certificate* cert, uint32_t time)
{
    if ((cert->keyUsage & (DLMS_KEY_USAGE_KEY_AGREEMENT | DLMS_KEY_USAGE_DIGITAL_SIGNATURE)) == 0 ||
        memcmp(cert->systemTitle, EMPTY_SYSTEM_TITLE, sizeof(EMPTY_SYSTEM_TITLE)) == 0 ||
        (time != 0 && time < cert->validFrom) ||
        (time != 0 && time > cert->validTo) ||
        cert->serialNumber.size == 0 ||
        cert->publicKey.rawValue.size == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return 0;
}

int gx509Certificate_parse(gx509Certificate* cert, 
    DLMS_X509_CERTIFICATE_DATA info,
    const char* data, 
    uint16_t index, 
    uint16_t length,
    gxByteBuffer* out)
{
    int ret;
    if ((ret = hlp_fromBase64(data + index, length, out)) == 0)
    {
        ret = asn1Parser_parsex509Certificate(cert, out, info, 0, 0);       
    }
    return ret;
}

int gx509Certificate_fromBytes(gx509Certificate* cert, 
    DLMS_X509_CERTIFICATE_DATA info, 
    gxByteBuffer* data)
{
    int ret = asn1Parser_parsex509Certificate(cert, data, info, 0, 0);
    data->position = 0;
    return ret;
}

int gx509Certificate_fromDer(gx509Certificate* cert, 
    DLMS_X509_CERTIFICATE_DATA info,
    const char* data)
{
    unsigned char buff[DLMS_X509_CETRIFICATE_MAX_SIZE];
    gxByteBuffer bb;
    BB_ATTACH(bb, buff, 0);
    return gx509Certificate_parse(cert, info, data, 0, (uint16_t)strlen(data), &bb);
}

int gx509Certificate_fromPem2(gx509Certificate* cert,
    DLMS_X509_CERTIFICATE_DATA info,
    const char* data,
    gxByteBuffer *bb)
{
    const char* START = GET_STR_FROM_EEPROM("CERTIFICATE-----\n");
    const char* END = GET_STR_FROM_EEPROM("-----END");
    int start = strstr(data, START) - data;
    int end = strstr(data, END) - data;
    if (start == -1 || end == -1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    unsigned char buff[DLMS_X509_CETRIFICATE_MAX_SIZE];
    gxByteBuffer tmp;
    BB_ATTACH(tmp, buff, 0);
    int ret = gx509Certificate_parse(cert, info, data, start + 17, end - start - 17, &tmp);
    if (ret == DLMS_ERROR_CODE_FALSE && bb != NULL)
    {
        bb_empty(bb);
        ret = bb_set(bb, tmp.data + tmp.position, bb_available(&tmp));
    }
    return ret;
}

int gx509Certificate_fromPem(gx509Certificate* cert, 
    DLMS_X509_CERTIFICATE_DATA info,
    const char* data)
{
    return gx509Certificate_fromPem2(cert, info, data, NULL);
 }

int gx509Certificate_getTag(gxByteBuffer* data, DLMS_X509_CERTIFICATE_DATA tag, gxByteBuffer* st)
{
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t size = data->size;
    uint32_t pos = data->position;
#else
    uint16_t size = data->size;
    uint16_t pos = data->position;
#endif
    int ret = asn1Parser_parsex509Certificate(NULL, data, tag, 0, 0);
    if (ret == DLMS_ERROR_CODE_FALSE)
    {
        ret = bb_attach(st, data->data + data->position, data->size - data->position, data->size - data->position);
    }
    data->position = pos;
    data->size = size;
    return ret;
}


int gx509Certificate_getSystemTitle(gxByteBuffer* data, gxByteBuffer* st)
{
    return gx509Certificate_getTag(data, DLMS_X509_CERTIFICATE_DATA_SYSTEM_TITLE, st);
}

int gx509Certificate_getValidFrom(gxByteBuffer* data, gxByteBuffer* st)
{
    return gx509Certificate_getTag(data, DLMS_X509_CERTIFICATE_DATA_VALID_FROM, st);
}

int gx509Certificate_getValidTo(gxByteBuffer* data, gxByteBuffer* st)
{
    return gx509Certificate_getTag(data, DLMS_X509_CERTIFICATE_DATA_VALID_TO, st);
}

int gx509Certificate_getSerialNumber(gxByteBuffer* data, gxByteBuffer* sn)
{
    return gx509Certificate_getTag(data, DLMS_X509_CERTIFICATE_DATA_SERIAL_NUMBER, sn);
}

int gx509Certificate_getIssuer(gxByteBuffer* data, gxByteBuffer* issuer)
{
    return gx509Certificate_getTag(data, DLMS_X509_CERTIFICATE_DATA_ISSUER, issuer);
}

int gx509Certificate_getSubject(gxByteBuffer* data, gxByteBuffer* subject)
{
    return gx509Certificate_getTag(data, DLMS_X509_CERTIFICATE_DATA_SUBJECT, subject);
}

int gx509Certificate_getSubjectAltName(gxByteBuffer* data, gxByteBuffer* subject)
{
    return gx509Certificate_getTag(data, DLMS_X509_CERTIFICATE_DATA_SUBJECT_ALT_NAME, subject);
}

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
int gx509Certificate_load(gx509Certificate* cert, 
    const char* path)
{
    char buff[DLMS_X509_CETRIFICATE_MAX_SIZE];
    uint16_t length = sizeof(buff);
    int ret = hlp_load(path, buff, &length);
    if (ret == 0)
    {
        ret = gx509Certificate_fromPem(cert, DLMS_X509_CERTIFICATE_DATA_NONE, buff);
    }
    return ret;
}

int gx509Certificate_load2(const char* path,
    DLMS_X509_CERTIFICATE_DATA info,
    gxByteBuffer* value)
{
    value->size = value->position = 0;
    uint16_t length = value->capacity;
    int ret = hlp_load(path, (char*) value->data, &length);
    if (ret == 0)
    {
        ret = gx509Certificate_fromPem2(NULL, info, (const char*) value->data, value);
    }
    return ret;
}

#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
