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

#include "../include/privateKey.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)

#include "../include/bytebuffer.h"
#include "../include/errorcodes.h"
#include "../include/bigInteger.h"
#include "../include/curve.h"
#include "../include/shamirs.h"

void priv_init(gxPrivateKey* key)
{
    key->scheme = ECC_P256;
    BB_ATTACH(key->rawValue, key->buff, 0);
    pub_init(&key->publicKey);
}
void priv_clear(gxPrivateKey* key)
{
    key->scheme = ECC_P256;
    bb_clear(&key->rawValue);
    pub_clear(&key->publicKey);
}

int priv_fromRawBytes(
    gxByteBuffer* value,
    gxPrivateKey* key)
{
    int ret = 0;
    //If private key is given
    if (bb_size(value) == 32)
    {
        key->scheme = ECC_P256;
    }
    else if (bb_size(value) == 48)
    {
        key->scheme = ECC_P384;
    }
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (ret == 0)
    {
        //Clear the public key associated with the private key.
        pub_clear(&key->publicKey);
        bb_clear(&key->rawValue);
        ret = bb_set(&key->rawValue, value->data, value->size);
    }
    return ret;
}

int priv_getPublicKey(gxPrivateKey* key, gxPublicKey* pub)
{
    int ret = 0;
    if (bb_size(&key->publicKey.rawValue) == 0)
    {
        //Public key = private key multiple by curve.G.
        bigInteger pk;
        bigInteger tmp;
        gxCurve curve;
        bi_init(&pk);
        bi_init(&tmp);
        if ((ret = bi_fromByteBuffer(&pk, &key->rawValue)) == 0 &&
            (ret = curve_init(&curve, key->scheme)) == 0)
        {
            gxEccPoint p;
            eccPoint_init(&p);
            ret = shamirs_pointMulti(&curve, &p, &curve.g, &pk);
            if (ret == 0)
            {
                int size = key->scheme == ECC_P256 ? 32 : 48;
                gxByteBuffer bb;
                gxByteBuffer tmp;
#ifdef GX_DLMS_MICROCONTROLLER
#ifdef DLMS_SECURITY_SUITE_1
                static unsigned char TMP[32];
#else
                static unsigned char TMP[48];
#endif //DLMS_SECURITY_SUITE_1
                static unsigned char BB[PUBLIC_KEY_SIZE];
#else                
#ifdef DLMS_SECURITY_SUITE_1
                static unsigned char TMP[32];
#else
                static unsigned char TMP[48];
#endif //DLMS_SECURITY_SUITE_1
                unsigned char BB[PUBLIC_KEY_SIZE];
#endif //GX_DLMS_MICROCONTROLLER
                BB_ATTACH(bb, BB, 0);
                BB_ATTACH(tmp, TMP, 0);
                //key is un-compressed format.
                if ((ret = bb_setUInt8(&bb, 4)) == 0 &&
                    (ret = bi_toArray(&p.x, &tmp)) == 0 &&
                    (ret = bb_set(&bb, tmp.data + bb_size(&tmp) % size, size)) == 0 &&
                    (ret = bb_clear(&tmp)) == 0 &&
                    (ret = bi_toArray(&p.y, &tmp)) == 0 &&
                    (ret = bb_set(&bb, tmp.data + bb_size(&tmp) % size, size)) == 0)
                {
                    ret = pub_fromRawBytes(&key->publicKey, &bb);
                }
            }
        }
    }
    if (ret == 0 && pub != &key->publicKey)
    {
        pub->scheme = key->publicKey.scheme;
        bb_clear(&pub->rawValue);
        ret = bb_set(&pub->rawValue, key->publicKey.rawValue.data, key->publicKey.rawValue.size);
    }
    return ret;
}
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
