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
#ifndef DLMS_IGNORE_HIGH_SHA384
#include <string.h>
#include "../include/gxsha384.h"

const uint64_t sha384_k[] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA364_F1(x) (SHA2_ROTR(x,  28) ^ SHA2_ROTR(x, 34) ^ SHA2_ROTR(x, 39))
#define SHA364_F2(x) (SHA2_ROTR(x,  14) ^ SHA2_ROTR(x, 18) ^ SHA2_ROTR(x, 41))
#define SHA364_F3(x) (SHA2_ROTR(x,  1) ^ SHA2_ROTR(x, 8) ^ SHA2_SHFR(x, 7))
#define SHA364_F4(x) (SHA2_ROTR(x, 19) ^ SHA2_ROTR(x, 61) ^ SHA2_SHFR(x, 6))
#define SHA2_UNPACK64(x, str)                 \
{                                             \
    *((str) + 7) = (unsigned char) ((x));       \
    *((str) + 6) = (unsigned char) ((x) >>  8);       \
    *((str) + 5) = (unsigned char) ((x) >> 16);       \
    *((str) + 4) = (unsigned char) ((x) >> 24);       \
    *((str) + 3) = (unsigned char) ((x) >> 32);       \
    *((str) + 2) = (unsigned char) ((x) >> 40);       \
    *((str) + 1) = (unsigned char) ((x) >> 48);       \
    *((str) + 0) = (unsigned char) ((x) >> 56);       \
}
#define SHA2_PACK64(str, x)                   \
{                                             \
    *(x) =   ((uint64_t) *((str) + 7)      )  \
           | ((uint64_t) *((str) + 6) <<  8)  \
           | ((uint64_t) *((str) + 5) << 16)  \
           | ((uint64_t) *((str) + 4) << 24)  \
           | ((uint64_t) *((str) + 3) << 32)  \
           | ((uint64_t) *((str) + 2) << 40)  \
           | ((uint64_t) *((str) + 1) << 48)  \
           | ((uint64_t) *((str) + 0) << 56); \
}

void gxsha384_transform(uint32_t* h, const unsigned char* message, uint32_t messageLength)
{
    uint64_t w[80];
    uint64_t wv[8];
    uint64_t t1, t2;
    unsigned char pos;
    for (pos = 0; pos < 16; pos++)
    {
        SHA2_PACK64(&message[pos << 3], &w[pos]);
    }
    for (pos = 16; pos < 80; pos++)
    {
        w[pos] = SHA364_F3(w[pos - 15]) + w[pos - 7] + SHA364_F4(w[pos - 2]) + w[pos - 16];
    }
    for (pos = 0; pos < 8; pos++)
    {
        wv[pos] = h[pos];
    }
    for (pos = 0; pos < 80; pos++) {
        t1 = wv[7] + SHA364_F2(wv[4]) + SHA2_CH(wv[4], wv[5], wv[6])
            + sha384_k[pos] + w[pos];
        t2 = SHA364_F1(wv[0]) + SHA2_MAJ(wv[0], wv[1], wv[2]);
        wv[7] = wv[6];
        wv[6] = wv[5];
        wv[5] = wv[4];
        wv[4] = wv[3] + t1;
        wv[3] = wv[2];
        wv[2] = wv[1];
        wv[1] = wv[0];
        wv[0] = t1 + t2;
    }
    for (pos = 0; pos < 8; pos++)
    {
        h[pos] += (uint32_t)wv[pos];
    }
}

int gxsha384_hash(gxByteBuffer* data, gxByteBuffer* digest)
{
    uint32_t len = data->size;
    uint32_t position = data->position;
    uint64_t h[8] = {
        0xcbbb9d5dc1059ed8,
        0x629a292a367cd507,
        0x9159015a3070dd17,
        0x152fecd8f70e5939,
        0x67332667ffc00b31,
        0x8eb44a8768581511,
        0xdb0c2e0d64f98fa7,
        0x47b5481dbefa4fa4 };
    unsigned char block[128];
    unsigned char pos, size = sizeof(block);
    unsigned char bidsAdded = 0;
    unsigned char eofAdded = 0;
    while (!bidsAdded)
    {
        if (bb_available(data) < 128)
        {
            size = (unsigned char)bb_available(data);
        }
        memcpy(block, data->data + data->position, size);
        memset(block + size, 0, sizeof(block) - size);
        if (bb_available(data) < 128 && !eofAdded)
        {
            // Append a bit 1.
            block[size] = 0x80;
            eofAdded = 1;
        }
        if (bb_available(data) < 112)
        {
            bidsAdded = 1;
            //Add bit length to the end of last block.
            uint64_t len_b = len;
            len_b <<= 3;
            SHA2_UNPACK64(len_b, block + sizeof(block) - 8);
        }
        gxsha384_transform((uint32_t*)h, block, size);
        data->position += size;
    }
    int ret = bb_capacity(digest, 48);
    if (ret == 0)
    {
        digest->size = 0;
        for (pos = 0; pos < 6; ++pos)
        {
            bb_setUInt64(digest, h[pos]);
        }
        digest->size = 48;
    }
    data->position = position;
    return ret;
}

#endif //DLMS_IGNORE_HIGH_SHA384
