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

#ifndef GX509CERTIFICATE_H
#define GX509CERTIFICATE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxignore.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "bytebuffer.h"   
#include "bigInteger.h"     
#include "publicKey.h"     
#include "enums.h"     

    typedef enum
    {
        /*All data is returned.*/
        DLMS_X509_CERTIFICATE_DATA_NONE,
        /*System title start index is returned.*/
        DLMS_X509_CERTIFICATE_DATA_SYSTEM_TITLE,
        /*Valid from start index is returned.*/
        DLMS_X509_CERTIFICATE_DATA_VALID_FROM,
        /*System title valid to index is returned.*/
        DLMS_X509_CERTIFICATE_DATA_VALID_TO,
        /*Serial number start index is returned.*/
        DLMS_X509_CERTIFICATE_DATA_SERIAL_NUMBER,
        /*Issuer start index is returned.*/
        DLMS_X509_CERTIFICATE_DATA_ISSUER,
        /*Subject start index is returned.*/
        DLMS_X509_CERTIFICATE_DATA_SUBJECT,
        /*Subject Alt Namestart index is returned.*/
        DLMS_X509_CERTIFICATE_DATA_SUBJECT_ALT_NAME,
    }DLMS_X509_CERTIFICATE_DATA;

    // Key Usage.
    typedef enum
    {
        // Key is not used.
        DLMS_KEY_USAGE_NONE = 0,
        // Digital signature.        
        DLMS_KEY_USAGE_DIGITAL_SIGNATURE = 0x1,
        // Key agreement.        
        DLMS_KEY_USAGE_KEY_AGREEMENT = 0x10,
    }DLMS_KEY_USAGE;

    /*X.509 v3 certificate.
    https://tools.ietf.org/html/rfc5280
    */
    typedef struct
    {
        bigInteger serialNumber;
        ECC signatureAlgorithm;
        /*Certificate validity starts.*/
        uint32_t validFrom;
        /*Certificate validity ends.*/
        uint32_t validTo;
        /*System title from the subject CN.*/
        unsigned char systemTitle[8];
        DLMS_KEY_USAGE keyUsage;
        gxPublicKey publicKey;
    } gx509Certificate;

    /*Initialize X.509 v3 certificate.
        cert: X.509 certificate.
    */
    void gx509Certificate_init(gx509Certificate* cert);

    /*Clear X.509 v3 certificate.
        cert: X.509 certificate.
    */
    void gx509Certificate_clear(gx509Certificate* cert);

    /*Validate X.509 v3 certificate.
        cert: X.509 certificate.
        time: Current time. Time validation is ignored if it is zero.
    */
    int gx509Certificate_validate(gx509Certificate* cert, uint32_t time);

    /*Load X.509 v3 certificate from bytes.
        cert: X.509 certificate.
    */
    int gx509Certificate_fromBytes(gx509Certificate* cert, 
        DLMS_X509_CERTIFICATE_DATA info, 
        gxByteBuffer* data);

    /*Load X.509 v3 certificate from DER.
        cert: X.509 certificate.
    */
    int gx509Certificate_fromDer(gx509Certificate* cert, 
        DLMS_X509_CERTIFICATE_DATA info, 
        const char* data);

    /*Load X.509 v3 certificate from PEM.*/
    int gx509Certificate_fromPem(gx509Certificate* cert, 
        DLMS_X509_CERTIFICATE_DATA info, 
        const char* data);

    /*Get serial number from X.509 v3 certificate.*/
    int gx509Certificate_getSerialNumber(gxByteBuffer* data, 
        gxByteBuffer* sn);

    /*Get System title from X.509 v3 certificate.*/
    int gx509Certificate_getSystemTitle(gxByteBuffer* data,
        gxByteBuffer* st);

    /*Get Valid from value from X.509 v3 certificate.*/
    int gx509Certificate_getValidFrom(gxByteBuffer* data,
        gxByteBuffer* st);

    /*Get Valid to value from X.509 v3 certificate.*/
    int gx509Certificate_getValidTo(gxByteBuffer* data,
        gxByteBuffer* st);

    /*Get issuer from X.509 v3 certificate.*/
    int gx509Certificate_getIssuer(gxByteBuffer* data, 
        gxByteBuffer* issuer);

    /*Get subject from X.509 v3 certificate.*/
    int gx509Certificate_getSubject(gxByteBuffer* data, 
        gxByteBuffer* subject);

    /*Get subject Alt Name from X.509 v3 certificate.*/
    int gx509Certificate_getSubjectAltName(gxByteBuffer* data, 
        gxByteBuffer* subject);

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
    /*Load X.509 v3 certificate.
        cert: X.509 certificate.
    */
    int gx509Certificate_load(gx509Certificate* cert, 
        const char* path);

    /*Load spesific data from X.509 v3 certificate.
        cert: X.509 certificate.
    */
    int gx509Certificate_load2(const char* path,
        DLMS_X509_CERTIFICATE_DATA info, 
        gxByteBuffer* value);

#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#ifdef  __cplusplus
}
#endif

#endif //GX509CERTIFICATE_H
