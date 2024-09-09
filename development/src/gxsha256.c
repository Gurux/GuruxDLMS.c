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
#ifndef DLMS_IGNORE_HIGH_SHA256
#include <string.h>
#include "../include/gxsha256.h"

const uint32_t sha256_k[64] =
{ 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA256_F1(x) (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))

#define SHA2_PACK32(str, x)                   \
{                                             \
    *(x) =   ((uint32_t) *((str) + 3)      )    \
           | ((uint32_t) *((str) + 2) <<  8)    \
           | ((uint32_t) *((str) + 1) << 16)    \
           | ((uint32_t) *((str) + 0) << 24);   \
}

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

void gxsha256_transform(uint32_t* h, const unsigned char* message, uint32_t messageLength)
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;
    unsigned char pos;
    for (pos = 0; pos < 16; ++pos)
    {
        SHA2_PACK32(&message[pos << 2], &w[pos]);
    }
    for (pos = 16; pos < 64; pos++)
    {
        w[pos] = SHA256_F4(w[pos - 2]) + w[pos - 7] + SHA256_F3(w[pos - 15]) + w[pos - 16];
    }
    for (pos = 0; pos < 8; pos++)
    {
        wv[pos] = h[pos];
    }
    for (pos = 0; pos < 64; pos++) {
        t1 = wv[7] + SHA256_F2(wv[4]) + SHA2_CH(wv[4], wv[5], wv[6])
            + sha256_k[pos] + w[pos];
        t2 = SHA256_F1(wv[0]) + SHA2_MAJ(wv[0], wv[1], wv[2]);
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
        h[pos] += wv[pos];
    }
}

int gxsha256_hash(gxByteBuffer* data, gxByteBuffer* digest)
{
    int ret = 0;
    uint32_t len = data->size;
    uint32_t position = data->position;
    uint32_t h[8] = { 0x6a09e667, 0xbb67ae85,
        0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c,
        0x1f83d9ab, 0x5be0cd19 };

    unsigned char block[64];
    unsigned char pos, size = sizeof(block);
    unsigned char bidsAdded = 0;
    while (!bidsAdded)
    {
        if (bb_available(data) < 64)
        {
            size = (unsigned char)bb_available(data);
        }
        memcpy(block, data->data + data->position, size);
        memset(block + size, 0, sizeof(block) - size);
        if (bb_available(data) < 64 && size != 0)
        {
            // Append a bit 1
            block[size] = 0x80;
        }
        if (bb_available(data) < 56)
        {
            bidsAdded = 1;
            //Add bit length to the end of last block.
            uint64_t len_b = (uint64_t)len;
            len_b <<= 3;
            SHA2_UNPACK64(len_b, block + sizeof(block) - 8);
        }
        gxsha256_transform(h, block, size);
        data->position += size;
    }
    digest->size = 0;
    if ((ret = bb_capacity(digest, 32)) == 0)
    {
        for (pos = 0; pos < 8; ++pos)
        {
            if ((ret = bb_setUInt32(digest, h[pos])) != 0)
            {
                break;
            }
        }
    }
    data->position = position;
    return ret;
}

#endif //DLMS_IGNORE_HIGH_SHA256
