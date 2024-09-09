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

#include <stddef.h>
#include "../include/gxPkcs10.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "../include/errorcodes.h"
#include "../include/gxecdsa.h"
#include "../include/helpers.h"

int pkcs10_createCertificateSigningRequest(
    gxPrivateKey* priv, 
    const unsigned char* subject,
    gxByteBuffer* value)
{
    int ret = 0;
#ifdef GX_DLMS_MICROCONTROLLER
    static char system_title[16];
#else
    char system_title[16];
#endif //GX_DLMS_MICROCONTROLLER
    uint16_t size = priv->scheme == ECC_P256 ? 32 : 48;
    bb_clear(value);
    if (subject == NULL || priv == NULL || priv->rawValue.size == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (priv->publicKey.rawValue.size == 0)
    {
        ret = priv_getPublicKey(priv, &priv->publicKey);
    }
    if (ret == 0)
    {
        //CertificationRequestInfo
        bb_setUInt8(value, 0x30);
        bb_setUInt8(value, 0x7D);
        //Version
        bb_setUInt8(value, 0x2);
        bb_setUInt8(value, 0x1);
        bb_setUInt8(value, 0x0);
        //Subject name SEQUENCE.
        bb_setUInt8(value, 0x30);
        bb_setUInt8(value, 0x1B);
        bb_setUInt8(value, 0x31);
        bb_setUInt8(value, 0x19);
        bb_setUInt8(value, 0x30);
        bb_setUInt8(value, 0x17);
        //OBJECT IDENTIFIER
        bb_setUInt8(value, 0x06);
        bb_setUInt8(value, 0x03);
        bb_setUInt8(value, 0x55);
        bb_setUInt8(value, 0x04);
        bb_setUInt8(value, 0x03);
        //CN.
        bb_setUInt8(value, 0x0C);
        bb_setUInt8(value, sizeof(system_title));
        hlp_bytesToHex3(subject, 8, system_title, sizeof(system_title), 0);
        bb_set(value, (unsigned char*)system_title, sizeof(system_title));
        //SubjectPublicKeyInfo SEQUENCE
        bb_setUInt8(value, 0x30);
        bb_setUInt8(value, 0x59);
        bb_setUInt8(value, 0x30);
        bb_setUInt8(value, 0x13);
        //OBJECT IDENTIFIER 1.2.840.10045.2.1
        bb_setUInt8(value, 0x06);
        bb_setUInt8(value, 0x07);
        bb_setUInt8(value, 0x2A);
        bb_setUInt8(value, 0x86);
        bb_setUInt8(value, 0x48);
        bb_setUInt8(value, 0xCE);
        bb_setUInt8(value, 0x3D);
        bb_setUInt8(value, 0x02);
        bb_setUInt8(value, 0x01);
        //OBJECT IDENTIFIER 1.2.840.10045.3.1.7 prime256v1                     
        bb_setUInt8(value, 0x06);
        bb_setUInt8(value, 0x08);
        bb_setUInt8(value, 0x2A);
        bb_setUInt8(value, 0x86);
        bb_setUInt8(value, 0x48);
        bb_setUInt8(value, 0xCE);
        bb_setUInt8(value, 0x3D);
        bb_setUInt8(value, 0x03);
        bb_setUInt8(value, 0x01);
        bb_setUInt8(value, 0x07);
        //Public key.
        bb_setUInt8(value, BER_TYPE_BIT_STRING);
        bb_setUInt8(value, 1 + priv->publicKey.rawValue.size);
        bb_setUInt8(value, 0x00);
        bb_set(value, priv->publicKey.rawValue.data, priv->publicKey.rawValue.size);
        //Attributes 0 elements.
        bb_setUInt8(value, 0xA0);
        bb_setUInt8(value, 0x0);
        gxByteBuffer signature;
#ifdef GX_DLMS_MICROCONTROLLER
        static unsigned char SIGNATURE[0x40];
#else
        unsigned char SIGNATURE[0x40];
#endif //GX_DLMS_MICROCONTROLLER
        BB_ATTACH(signature, SIGNATURE, 0);
        //Sign version, subject, objectIdentifier, attributes and signature algorithm.
        if ((ret = gxecdsa_sign(priv, value, &signature)) == 0)
        {
            //SignatureAlgorithm AlgorithmIdentifier
            bb_setUInt8(value, 0x30);
            bb_setUInt8(value, 0x0A);
            bb_setUInt8(value, 0x06);
            bb_setUInt8(value, 0x08);
            bb_setUInt8(value, 0x2A);
            bb_setUInt8(value, 0x86);
            bb_setUInt8(value, 0x48);
            bb_setUInt8(value, 0xCE);
            bb_setUInt8(value, 0x3D);
            bb_setUInt8(value, 0x04);
            bb_setUInt8(value, 0x03);
            bb_setUInt8(value, 0x02);
            //Signature BIT STRING.
            bb_setUInt8(value, BER_TYPE_BIT_STRING);
            //Sign length.
            bb_setUInt8(value, 7 + signature.size);
            //Pad bits.
            bb_setUInt8(value, 0x00);
            //X
            bb_setUInt8(value, 0x30);
            bb_setUInt8(value, 0x44);
            bb_setUInt8(value, 0x02);
            bb_setUInt8(value, 0x20);
            bb_set(value, signature.data, size);
            //Y
            bb_setUInt8(value, 0x02);
            bb_setUInt8(value, 0x20);
            if ((ret = bb_set(value, signature.data + size, size)) == 0)
            {
                bb_clear(&signature);
                size = value->size;
                uint16_t len = 1 + hlp_getObjectCountSizeInBytes(value->size);
                if ((ret = bb_move(value, 0, len, bb_size(value))) == 0)
                {
                    value->size = 0;
                    //BerType.Constructed | BerType.Sequence
                    bb_setUInt8(value, 0x30);
                    //Length
                    if ((ret = hlp_setObjectCount(size, value)) == 0)
                    {
                        value->size += size;
                    }
                }
            }
        }
    }
    return ret;
}
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
