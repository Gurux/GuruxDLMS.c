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

#include "../include/gxecdsa.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "../include/helpers.h"
#include "../include/errorcodes.h"
#include "../include/bigInteger.h"
#include "../include/eccPoint.h"
#include "../include/curve.h"
#include "../include/shamirs.h"
#ifdef DLMS_SECURITY_SUITE_1
#include "../include/gxsha256.h"
#endif //DLMS_SECURITY_SUITE_1
#ifdef DLMS_SECURITY_SUITE_2
#include "../include/gxsha384.h"
#endif //DLMS_SECURITY_SUITE_2

#ifdef DLMS_SECURITY_SUITE_1
#define HASH_SIZE 32
#else
#define HASH_SIZE 48
#endif //DLMS_SECURITY_SUITE_1

int gxecdsa_getSchemeSize(ECC scheme)
{
    return scheme == ECC_P256 ? 32 : 48;
}

// Generate random number.
int gxecdsa_getRandomNumber(bigInteger* N, gxByteBuffer* value)
{
    int ret = bb_capacity(value, 4 * N->size);
    unsigned char val;
    for (uint16_t pos = 0; pos != (uint16_t)value->capacity; ++pos)
    {
        val = hlp_rand();
        if (val == 0 && pos == 0)
        {
            val = (1 + hlp_rand()) % 100;
        }
        bb_setUInt8(value, val);
    }
    return ret;
}

int gxecdsa_verify(gxPublicKey* key,
    gxByteBuffer* signature,
    gxByteBuffer* data)
{
    int ret;
    unsigned char BUFFER[HASH_SIZE];
    gxByteBuffer bb;
    BB_ATTACH(bb, BUFFER, 0);
    if (key->rawValue.size == 0)
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (key->scheme == ECC_P256)
    {
#ifdef DLMS_SECURITY_SUITE_1
        ret = gxsha256_hash(data, &bb);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_SECURITY_SUITE_1
    }
#ifdef DLMS_SECURITY_SUITE_2
    else if (key->scheme == ECC_P384)
    {
        ret = gxsha384_hash(data, bb);
    }
#endif //DLMS_SECURITY_SUITE_2
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (ret == 0)
    {
        bigInteger u1;
        bi_init(&u1);
        gxCurve curve;
        if ((ret = curve_init(&curve, key->scheme)) == 0 &&
            (ret = bi_fromByteBuffer(&u1, &bb)) == 0)
        {
            int size = gxecdsa_getSchemeSize(key->scheme);
            //u2 = sigR
            bigInteger u2, sigR;
            bi_init(&u2);
            bi_init(&sigR);
            if ((ret = bb_subArray(signature, 0, size, &bb)) == 0 &&
                (ret = bi_fromByteBuffer(&u2, &bb)) == 0)
            {
                ret = bb_subArray(signature, size, size, &bb);
                //w = sigS;
                bigInteger w;
                bi_init(&w);
                if ((ret = bi_fromByteBuffer(&w, &bb)) == 0)
                {
                    bi_inv(&w, &curve.n);
                    bi_multiply(&u1, &w);
                    bi_mod(&u1, &curve.n);
                    bi_copy(&sigR, &u2);
                    bi_multiply(&u2, &w);
                    bi_mod(&u2, &curve.n);
                    gxEccPoint tmp;
                    if ((ret = eccPoint_init(&tmp)) == 0 &&
                        (ret = shamirs_trick(&curve, key, &tmp, &u1, &u2)) == 0)
                    {
                        bi_mod(&tmp.x, &curve.n);
                        ret = bi_compare(&tmp.x, &sigR) == 0 ? 0 : DLMS_ERROR_CODE_VERIFY_FAILED;
                    }
                }
            }
        }
    }
    return ret;
}

int gxecdsa_sign(gxPrivateKey* key, 
    gxByteBuffer* data, 
    gxByteBuffer* signature)
{
    bb_empty(signature);
    int ret;
#if defined(GX_DLMS_BYTE_BUFFER_SIZE_32) || (!defined(GX_DLMS_MICROCONTROLLER) && (defined(_WIN32) || defined(_WIN64) || defined(__linux__)))
    uint32_t pos = data->position;
#else
    uint16_t pos = data->position;
#endif
    if (key->rawValue.size == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (key->scheme == ECC_P256)
    {
#ifdef DLMS_SECURITY_SUITE_1
        ret = gxsha256_hash(data, signature);
#else
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
#endif //DLMS_SECURITY_SUITE_1
    }
#ifdef DLMS_SECURITY_SUITE_2
    else
    {
        ret = gxsha384_hash(data, signature);
    }
#endif //DLMS_SECURITY_SUITE_2
    else
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (ret == 0)
    {
        data->position = pos;
        bigInteger msg;
        bi_init(&msg);
        if ((ret = bi_fromByteBuffer(&msg, signature)) == 0)
        {
            bb_clear(signature);
            gxByteBuffer tmp;
#ifdef GX_DLMS_MICROCONTROLLER
            static unsigned char RANDOM_NUMBER[PRIVATE_KEY_SIZE];
#else
            unsigned char RANDOM_NUMBER[PRIVATE_KEY_SIZE];
#endif //GX_DLMS_MICROCONTROLLER
            BB_ATTACH(tmp, RANDOM_NUMBER, 0);
            gxCurve curve;
            if ((ret = curve_init(&curve, key->scheme)) == 0 &&
                (ret = gxecdsa_getRandomNumber(&curve.n, &tmp)) == 0)
            {
                bigInteger k, pk;
                bi_init(&k);
                bi_init(&pk);
                if ((ret = bi_fromByteBuffer(&pk, &key->rawValue)) == 0 &&
                    (ret = bi_fromByteBuffer(&k, &tmp)) == 0)
                {
                    gxEccPoint R;
                    eccPoint_init(&R);
                    if ((ret = shamirs_pointMulti(&curve, &R, &curve.g, &k)) == 0 &&
                        (ret = bi_mod(&R.x, &curve.n)) == 0 &&
                        //s = (k ^ -1 * (e + d * r)) mod n
                        (ret = bi_multiply(&pk, &R.x)) == 0 &&
                        (ret = bi_addBigInteger(&pk, &msg)) == 0 &&
                        (ret = bi_inv(&k, &curve.n)) == 0 &&
                        (ret = bi_multiply(&pk, &k)) == 0 &&
                        (ret = bi_mod(&pk, &curve.n)) == 0 &&
                        (ret = bb_clear(&tmp)) == 0 &&
                        (ret = bi_toArray(&R.x, signature)) == 0 &&
                        (ret = bi_toArray(&pk, &tmp)) == 0)
                    {
                        ret = bb_set(signature, tmp.data, tmp.size);
                    }
                }
            }
        }
    }
    return ret;
}

int gxecdsa_generateKeyPair(
    ECC scheme,
    gxPublicKey* pub, gxPrivateKey* priv)
{
    gxByteBuffer rn;
#ifdef GX_DLMS_MICROCONTROLLER
    static unsigned char RANDOM_NUMBER[PRIVATE_KEY_SIZE];
#else
    unsigned char RANDOM_NUMBER[PRIVATE_KEY_SIZE];
#endif //GX_DLMS_MICROCONTROLLER
    BB_ATTACH(rn, RANDOM_NUMBER, 0);
    gxCurve curve;
    int ret;
    if ((ret = curve_init(&curve, scheme)) == 0 &&
        (ret = gxecdsa_getRandomNumber(&curve.n, &rn)) == 0 &&
        (ret = priv_fromRawBytes(&rn, priv)) == 0 &&
        (ret = priv_getPublicKey(priv, pub)) == 0)
    {
    }
    return ret;
}
#endif //defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
