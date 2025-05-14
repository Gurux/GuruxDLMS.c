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
#ifndef DLMS_IGNORE_AES
#include <string.h> // CBC mode, for memset
#include <stdint.h>
#include "../include/errorcodes.h"
#include "../include/gxaes.h"

#if defined(USE_AVR) || defined(ARDUINO_ARCH_AVR)
//If AVR is used.
#include <avr/pgmspace.h>
#endif //defined(USE_AVR) || defined(ARDUINO_ARCH_AVR)

#ifdef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
#include "../include/ciphering.h"
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE

#define Nb 4

#ifndef USE_PROGMEM
    static const unsigned char __SBOX[256] = {
#else
    static const unsigned char __SBOX[256] PROGMEM = {
#endif //USE_PROGMEM
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

#ifndef USE_PROGMEM
    static const unsigned char __RS_BOX[256] = {
#else
    static const unsigned char __RS_BOX[256] PROGMEM = {
#endif //USE_PROGMEM
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

    // The round constant word array, __R_CON[i], contains the values given by
    // x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
#ifndef USE_PROGMEM
    static const unsigned char __R_CON[15] = {
#else
    static const unsigned char __R_CON[15] PROGMEM = {
#endif //USE_PROGMEM
        0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
        0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D };

typedef unsigned char state_t[4][4];

static unsigned char getSBoxValue(
    unsigned char encrypt,
    unsigned char offset)
{
#ifdef ARDUINO_ARCH_AVR
    //If Arduino is used data is read from flash like this.
    if (encrypt)
    {
        return pgm_read_byte_near(__SBOX + offset);
    }
    return pgm_read_byte_near(__RS_BOX + offset);
#else
    if (encrypt)
    {
        return __SBOX[offset];
    }
    return __RS_BOX[offset];
#endif //ARDUINO_ARCH_AVR
}

static unsigned char getRCon(unsigned char offset)
{
#ifdef ARDUINO_ARCH_AVR
    //If Arduino is used data is read from flash like this.
    return pgm_read_byte_near(__R_CON + offset);
#else
    return __R_CON[offset];
#endif //ARDUINO_ARCH_AVR
}

    unsigned char gxaes_xtime(unsigned char x)
    {
        return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
    }

    unsigned char gxaes_multiply(unsigned char x, unsigned char y)
    {
        return (((y & 1) * x) ^
            ((y >> 1 & 1) * gxaes_xtime(x)) ^
            ((y >> 2 & 1) * gxaes_xtime(gxaes_xtime(x))) ^
            ((y >> 3 & 1) * gxaes_xtime(gxaes_xtime(gxaes_xtime(x)))) ^
            ((y >> 4 & 1) * gxaes_xtime(gxaes_xtime(gxaes_xtime(gxaes_xtime(x))))));
    }

    void gxaes_subBytes(unsigned char encrypt,
        state_t* state)
    {
        unsigned char i, j;
        for (i = 0; i < 4; ++i)
        {
            for (j = 0; j < 4; ++j)
            {
                (*state)[j][i] = getSBoxValue(encrypt, (*state)[j][i]);
            }
        }
    }

    void gxaes_shiftRows(state_t* state) {
        // Rotate first row 1 columns to left
        unsigned char temp = (*state)[0][1];
        (*state)[0][1] = (*state)[1][1];
        (*state)[1][1] = (*state)[2][1];
        (*state)[2][1] = (*state)[3][1];
        (*state)[3][1] = temp;

        // Rotate second row 2 columns to left
        temp = (*state)[0][2];
        (*state)[0][2] = (*state)[2][2];
        (*state)[2][2] = temp;

        temp = (*state)[1][2];
        (*state)[1][2] = (*state)[3][2];
        (*state)[3][2] = temp;

        // Rotate third row 3 columns to left
        temp = (*state)[0][3];
        (*state)[0][3] = (*state)[3][3];
        (*state)[3][3] = (*state)[2][3];
        (*state)[2][3] = (*state)[1][3];
        (*state)[1][3] = temp;
    }

    void gxaes_mixColumns(state_t* state)
    {
        unsigned char i, Tmp, Tm, t;
        for (i = 0; i < 4; ++i)
        {
            t = (*state)[i][0];
            Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];
            Tm = (*state)[i][0] ^ (*state)[i][1];
            Tm = gxaes_xtime(Tm);
            (*state)[i][0] ^= Tm ^ Tmp;
            Tm = (*state)[i][1] ^ (*state)[i][2];
            Tm = gxaes_xtime(Tm);
            (*state)[i][1] ^= Tm ^ Tmp;
            Tm = (*state)[i][2] ^ (*state)[i][3];
            Tm = gxaes_xtime(Tm);
            (*state)[i][2] ^= Tm ^ Tmp;
            Tm = (*state)[i][3] ^ t;
            Tm = gxaes_xtime(Tm);
            (*state)[i][3] ^= Tm ^ Tmp;
        }
    }

    void gxaes_addRoundKey(
        state_t* state,
        const unsigned char* roundKey,
        unsigned char round)
    {
        unsigned char i, j;
        for (i = 0; i < 4; ++i)
        {
            for (j = 0; j < 4; ++j)
            {
                (*state)[i][j] ^= roundKey[round * Nb * 4 + i * Nb + j];
            }
        }
    }

    static void gxaes_rotWord(unsigned char* word) {
        unsigned char tmp = word[0];
        word[0] = word[1];
        word[1] = word[2];
        word[2] = word[3];
        word[3] = tmp;
    }

    static void gxaes_subWord(unsigned char* word)
    {
        unsigned char pos;
        for (pos = 0; pos < 4; ++pos)
        {
            word[pos] = getSBoxValue(1, word[pos]);
        }
    }

    static void gxaes_invShiftRows(state_t* state)
    {
        // Rotate first row 1 columns to right
        unsigned char temp = (*state)[3][1];
        (*state)[3][1] = (*state)[2][1];
        (*state)[2][1] = (*state)[1][1];
        (*state)[1][1] = (*state)[0][1];
        (*state)[0][1] = temp;

        // Rotate second row 2 columns to right
        temp = (*state)[0][2];
        (*state)[0][2] = (*state)[2][2];
        (*state)[2][2] = temp;

        temp = (*state)[1][2];
        (*state)[1][2] = (*state)[3][2];
        (*state)[3][2] = temp;

        // Rotate third row 3 columns to right
        temp = (*state)[0][3];
        (*state)[0][3] = (*state)[1][3];
        (*state)[1][3] = (*state)[2][3];
        (*state)[2][3] = (*state)[3][3];
        (*state)[3][3] = temp;
    }

    static void gxaes_invMixColumns(state_t* state)
    {
        unsigned char i, a, b, c, d;
        for (i = 0; i < 4; ++i)
        {
            a = (*state)[i][0];
            b = (*state)[i][1];
            c = (*state)[i][2];
            d = (*state)[i][3];

            (*state)[i][0] = gxaes_multiply(a, 0x0e) ^ gxaes_multiply(b, 0x0b) ^ gxaes_multiply(c, 0x0d) ^ gxaes_multiply(d, 0x09);
            (*state)[i][1] = gxaes_multiply(a, 0x09) ^ gxaes_multiply(b, 0x0e) ^ gxaes_multiply(c, 0x0b) ^ gxaes_multiply(d, 0x0d);
            (*state)[i][2] = gxaes_multiply(a, 0x0d) ^ gxaes_multiply(b, 0x09) ^ gxaes_multiply(c, 0x0e) ^ gxaes_multiply(d, 0x0b);
            (*state)[i][3] = gxaes_multiply(a, 0x0b) ^ gxaes_multiply(b, 0x0d) ^ gxaes_multiply(c, 0x09) ^ gxaes_multiply(d, 0x0e);
        }
    }

    void gxaes_keyExpansion(
        const DLMS_AES aes,
        const unsigned char* key,
        unsigned char* roundKeys)
    {
        unsigned char i, Nr, Nk;
        unsigned char temp[4];
        if (aes == DLMS_AES_256)
        {
            Nr = 14;
            Nk = 8;
        }
        else
        {
            Nr = 10;
            Nk = 4;
        }

        //Copy original key.
        memcpy(roundKeys, key, 4 * Nk);
        i = Nk;
        while (i < 4 * (Nr + 1)) {
            memcpy(temp, roundKeys + 4 * (i - 1), 4);
            if (i % Nk == 0) {
                gxaes_rotWord(temp);
                gxaes_subWord(temp);
                temp[0] ^= getRCon(i / Nk);
            }
            else if (Nk > 6 && (i % Nk) == 4)
            {
                gxaes_subWord(temp);
            }

            roundKeys[4 * i + 0] = roundKeys[4 * (i - Nk) + 0] ^ temp[0];
            roundKeys[4 * i + 1] = roundKeys[4 * (i - Nk) + 1] ^ temp[1];
            roundKeys[4 * i + 2] = roundKeys[4 * (i - Nk) + 2] ^ temp[2];
            roundKeys[4 * i + 3] = roundKeys[4 * (i - Nk) + 3] ^ temp[3];
            ++i;
        }
    }

    void gxaes_cipher(
        const DLMS_AES aes,
        unsigned char encrypt,
        const unsigned char* input,
        const unsigned char* roundKeys,
        unsigned char* output)
    {
        unsigned char pos, Nr;
        if (aes == DLMS_AES_256)
        {
            Nr = 14;
        }
        else
        {
            Nr = 10;
        }
        state_t* state = (state_t*)output;
        memcpy(state, input, 16);
        gxaes_addRoundKey(state, roundKeys, encrypt ? 0 : Nr);
        if (encrypt)
        {
            for (pos = 1; pos != Nr; ++pos)
            {
                gxaes_subBytes(encrypt, state);
                gxaes_shiftRows(state);
                gxaes_mixColumns(state);
                gxaes_addRoundKey(state, roundKeys, pos);
            }
            gxaes_subBytes(encrypt, state);
            gxaes_shiftRows(state);
        }
        else
        {
            for (pos = Nr - 1; pos >= 1; --pos)
            {
                gxaes_invShiftRows(state);
                gxaes_subBytes(encrypt, state);
                gxaes_addRoundKey(state, roundKeys, pos);
                gxaes_invMixColumns(state);
            }
            gxaes_invShiftRows(state);
            gxaes_subBytes(encrypt, state);
        }
        gxaes_addRoundKey(state, roundKeys, encrypt ? Nr : 0);
    }

    int gxaes_encrypt(const DLMS_AES aes,
        gxByteBuffer* data,
        gxByteBuffer* secret,
        gxByteBuffer* output)
    {
#ifdef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
        return gx_hsmAesEncrypt(aes, data, secret, output);
#else
        int ret = 0;
#ifdef GX_DLMS_MICROCONTROLLER
        static unsigned char gx_roundKeys[240];
#else
        unsigned char gx_roundKeys[240];
#endif //GX_DLMS_MICROCONTROLLER
        unsigned char size = aes == DLMS_AES_128 ? 16 : 32;
        unsigned char KEY[32];
        unsigned char DATA[32];
        if (bb_size(secret) > size ||
            bb_size(data) > size ||
            bb_size(secret) == 0 ||
            bb_size(data) == 0)
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        else if (data == output ||
            (ret = bb_capacity(output, bb_size(output) + size)) == 0)
        {
            memcpy(KEY, secret->data, secret->size);
            memset(KEY + secret->size, 0, size - secret->size);
            memcpy(DATA, data->data, data->size);
            memset(DATA + data->size, 0, size - data->size);
            gxaes_keyExpansion(aes, KEY, gx_roundKeys);
            gxaes_cipher(aes, 1, DATA, gx_roundKeys,
                output->data + (data == output ? 0 : output->size));
            if (data != output)
            {
                output->size += size;
            }
        }
        return ret;
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
    }

    int gxaes_decrypt(
        const DLMS_AES aes,
        gxByteBuffer* data,
        gxByteBuffer* secret,
        gxByteBuffer* output)
    {
#ifdef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
        return gx_hsmAesDecrypt(aes, data, secret, output);
#else
        int ret = 0;
#ifdef GX_DLMS_MICROCONTROLLER
        static unsigned char gx_roundKeys[240];
#else
        unsigned char gx_roundKeys[240];
#endif //GX_DLMS_MICROCONTROLLER
        unsigned char size = aes == DLMS_AES_128 ? 16 : 32;
        unsigned char KEY[32];
        unsigned char DATA[32];
        if (bb_size(secret) > size ||
            bb_size(data) > size)
        {
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        else if (data == output ||
            (ret = bb_capacity(output, bb_size(output) + size)) == 0)
        {
            memcpy(KEY, secret->data, secret->size);
            memset(KEY, secret->size, size - secret->size);
            memcpy(DATA, data->data, data->size);
            memset(DATA, data->size, size - data->size);
            gxaes_keyExpansion(aes, KEY, gx_roundKeys);
            gxaes_cipher(aes, 0, DATA, gx_roundKeys, output->data +
                    (data == output ? 0 : output->size));
            if (data != output)
            {
                output->size += size;
            }
        }
        return ret;
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
    }
#endif //DLMS_IGNORE_AES
