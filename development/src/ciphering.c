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
#ifndef DLMS_IGNORE_HIGH_GMAC

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <assert.h>
#endif

#if defined(USE_AVR) || defined(ARDUINO_ARCH_AVR)
//If AVR is used.
#include <avr/pgmspace.h>
#endif //#if defined(USE_AVR) || defined(ARDUINO_ARCH_AVR)

#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <string.h>
#include "../include/enums.h"
#include "../include/errorcodes.h"
#include "../include/ciphering.h"
#include "../include/helpers.h"
#include "../include/gxaes.h"

#if defined(DLMS_DEBUG) || defined(DLMS_NOTIFY_AUTHENTICATION_ERROR)
#include "../include/serverevents.h"
#endif //defined(DLMS_DEBUG) || defined(DLMS_NOTIFY_AUTHENTICATION_ERROR)

static unsigned char AUTHENTICATION_KEY_SIZE(DLMS_AES aes)
{
    return aes == DLMS_AES_128 ? 16 : 32;
}

#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE

/*Initialize GCM.*/
static int gxgcm_init(
    const DLMS_AES aes,
    unsigned char* cipherKey,
    unsigned char* roundKeys)
{
    gxaes_keyExpansion(aes, cipherKey, roundKeys);
    return 0;
}
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE

// Increase the block counter.
static void gxgcm_increaseBlockCounter(gxByteBuffer* nonceAndCounter,
    uint32_t counter)
{
    nonceAndCounter->size = 12;
    bb_setUInt32(nonceAndCounter, counter);
}

static int gxgcm_transformBlock(
    const DLMS_AES aes,
    const unsigned char* systemTitle,
    const uint32_t counter,
    unsigned char* data,
    const uint32_t length,
    uint32_t algorithmInitialblockCounter,
    unsigned char* roundKeys)
{
    gxByteBuffer nonceAndCounter;
    unsigned char NONSE[16];
    bb_attach(&nonceAndCounter, NONSE, 0, 16);
    bb_set(&nonceAndCounter, systemTitle, 8);
    bb_setUInt32(&nonceAndCounter, counter);
    bb_setUInt32(&nonceAndCounter, algorithmInitialblockCounter);

    uint16_t inputOffset = 0;
    uint32_t pos, pos2, count;
    unsigned char counterModeBlock[16] = { 0 };
    for (pos = 0; pos < length; pos += 16)
    {
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
        gxaes_cipher(aes, 1, NONSE, roundKeys, counterModeBlock);
#else
        gxByteBuffer cbm, secret;
        BB_ATTACH(cbm, counterModeBlock, 0);
        bb_attach(&secret, roundKeys, aes == DLMS_AES_128 ? 16 : 32, 
            aes == DLMS_AES_128 ? 16 : 32);
        gx_hsmAesEncrypt(aes, &nonceAndCounter, &secret, &cbm);
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
        gxgcm_increaseBlockCounter(&nonceAndCounter, ++algorithmInitialblockCounter);
        count = length - pos < 16 ? length - pos : 16;
        for (pos2 = 0; pos2 != count; ++pos2)
        {
            data[inputOffset] ^= counterModeBlock[pos2];
            ++inputOffset;
        }
    }
    return 0;
}

// Make Xor for 128 bits.
static void gxgcm_xor(unsigned char* block, unsigned char* value)
{
    unsigned char pos;
    for (pos = 0; pos != 16; ++pos)
    {
        block[pos] ^= value[pos];
    }
}

// Shift block to right.
static void gxgcm_shiftRight(unsigned char* block)
{
    uint32_t val = GETU32(block + 12);
    val >>= 1;
    if (block[11] & 0x01)
    {
        val |= 0x80000000;
    }
    PUT32(block + 12, val);

    val = GETU32(block + 8);
    val >>= 1;
    if (block[7] & 0x01)
        val |= 0x80000000;
    PUT32(block + 8, val);

    val = GETU32(block + 4);
    val >>= 1;
    if (block[3] & 0x01)
        val |= 0x80000000;
    PUT32(block + 4, val);

    val = GETU32(block);
    val >>= 1;
    PUT32(block, val);
}

static void gxgcm_multiplyH(unsigned char* y, unsigned char* h)
{
    unsigned char i, j;
    unsigned char tmp[16];
    unsigned char z[16] = { 0 };
    memcpy(tmp, h, 16);
    //Loop every byte.
    for (i = 0; i != 16; ++i)
    {
        //Loop every bit.
        for (j = 0; j != 8; j++)
        {
            if ((y[i] & (1 << (7 - j))) != 0)
            {
                gxgcm_xor(z, &tmp[0]);
            }
            //If last bit.
            if ((tmp[15] & 0x01) != 0)
            {
                gxgcm_shiftRight(&tmp[0]);
                tmp[0] ^= 0xe1;
            }
            else
            {
                gxgcm_shiftRight(&tmp[0]);
            }
        }
    }
    memcpy(y, z, 16);
}

// Count GHASH.
static void gxgcm_getGHash(
    const DLMS_SECURITY security,
    const DLMS_AES aes,
    unsigned char tag,
    unsigned char* authenticationKey,
    gxByteBuffer* value,
    uint32_t lenA,
    uint32_t lenC,
    unsigned char* Y, 
    unsigned char* key)
{
    unsigned char EMPTY[16] = { 0 };
    unsigned char H[32] = { 0 };
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
    gxaes_cipher(aes, 1, EMPTY, key, H);
#else
    gxByteBuffer e, h, secret;
    BB_ATTACH(e, EMPTY, sizeof(EMPTY));
    BB_ATTACH(h, H, 0);
    bb_attach(&secret, key, (aes == DLMS_AES_128) ? 16 : 32, (aes == DLMS_AES_128) ? 16 : 32);
    gx_hsmAesEncrypt(aes, &e, &secret, &h);
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
    uint32_t cnt, pos;
    unsigned char X[16];
    //Handle tag and authentication key.
    memset(X, 0, 16);
    memset(Y, 0, 16);
    X[0] = tag;
    memcpy(X + 1, authenticationKey, 15);
    gxgcm_xor(Y, X);
    gxgcm_multiplyH(Y, H);
    uint32_t available = AUTHENTICATION_KEY_SIZE(aes);
    if (available > 15)
    {
        available -= 15;
        if (available > 16)
        {
            //If key size is 32 bytes.
            available = 16;
            memcpy(&X[0], authenticationKey + 15, available);
            gxgcm_xor(Y, X);
            gxgcm_multiplyH(Y, H);
            available = AUTHENTICATION_KEY_SIZE(aes) - 31;
            //Add the authentication key remaining.
            memcpy(&X[0], authenticationKey + 31, available);
        }
        else
        {
            //Add the authentication key remaining.
            memcpy(&X[0], authenticationKey + 15, available);
        }
    }
    //Count appended zeroes to fill data to 128 bits.
    if (security == DLMS_SECURITY_AUTHENTICATION)
    {
        //Plain text.
    }
    else
    {
        //Plain text.
        memset(X + available, 0, 16 - available);
        available = 0;
        gxgcm_xor(Y, X);
        gxgcm_multiplyH(Y, H);
    }
    for (pos = 0; pos < bb_available(value); pos += 16)
    {
        memset(X + available, 0, 16 - available);
        cnt = bb_available(value);
        cnt -= pos;
        if (cnt > 16 - available)
        {
            cnt = 16 - available;
        }
        memcpy(X + available, value->data + value->position + pos, cnt);
        gxgcm_xor(Y, X);
        gxgcm_multiplyH(Y, H);
    }
    if (bb_available(value) + available > pos)
    {
        memset(X + available, 0, 16 - available);
        memcpy(X, value->data + value->position + pos - available, available);
        gxgcm_xor(Y, X);
        gxgcm_multiplyH(Y, H);
    }
    PUT32(X, 0L);
    PUT32(X + 4, lenA);
    PUT32(X + 8, 0L);
    PUT32(X + 12, lenC);
    gxgcm_xor(Y, X);
    gxgcm_multiplyH(Y, H);
}

static int gxgcm_getTag(
    const DLMS_SECURITY security,
    const DLMS_AES aes,
    DLMS_COUNT_TYPE type,
    const unsigned char* systemTitle,
    const uint32_t frameCounter,
    unsigned char* authenticationKey,
    const unsigned char tag,
    gxByteBuffer* value,
    gxByteBuffer* aTag,
    unsigned char* key)
{
    int ret;
    //Length of the crypted data.
    uint32_t lenC = 0;
    //Length of the authenticated data.
    uint32_t lenA = (1 + AUTHENTICATION_KEY_SIZE(aes)) * 8;
    if (security == DLMS_SECURITY_AUTHENTICATION)
    {
        //If data is not ciphered.
        lenA += 8 * bb_available(value);
    }
    else
    {
        lenC = 8 * bb_available(value);
    }
    unsigned char hash[16];
    gxgcm_getGHash(security, aes, tag, authenticationKey,
        value, lenA, lenC, &hash[0], key);
    ret = gxgcm_transformBlock(aes, systemTitle, frameCounter,
        hash, sizeof(hash), 1,
        key);
    if (ret == 0)
    {
        if (type == DLMS_COUNT_TYPE_TAG)
        {
            aTag->size = 0;
        }
        ret = bb_set(aTag, &hash[0], 12);
    }
    return ret;
}

void cip_init(ciphering* target)
{
    target->invocationCounter = 0;
    target->suite = DLMS_SECURITY_SUITE_V0;
    target->security = DLMS_SECURITY_NONE;
    target->securityPolicy = DLMS_SECURITY_POLICY_NOTHING;
    target->encrypt = 0;
#ifndef DLMS_IGNORE_MALLOC
    BYTE_BUFFER_INIT(&target->blockCipherKey);
    bb_set(&target->blockCipherKey, DEFAULT_BLOCK_CIPHER_KEY, sizeof(DEFAULT_BLOCK_CIPHER_KEY));
    BYTE_BUFFER_INIT(&target->broadcastBlockCipherKey);
    bb_set(&target->broadcastBlockCipherKey, DEFAULT_BROADCAST_BLOCK_CIPHER_KEY, sizeof(DEFAULT_BROADCAST_BLOCK_CIPHER_KEY));
    BYTE_BUFFER_INIT(&target->systemTitle);
    bb_set(&target->systemTitle, DEFAULT_SYSTEM_TITLE, sizeof(DEFAULT_SYSTEM_TITLE));
    BYTE_BUFFER_INIT(&target->authenticationKey);
    bb_set(&target->authenticationKey, DEFAULT_AUTHENTICATION_KEY, sizeof(DEFAULT_AUTHENTICATION_KEY));
    target->dedicatedKey = NULL;
#else
    memcpy(target->blockCipherKey, DEFAULT_BLOCK_CIPHER_KEY, sizeof(DEFAULT_BLOCK_CIPHER_KEY));
    memcpy(target->broadcastBlockCipherKey, DEFAULT_BROADCAST_BLOCK_CIPHER_KEY, sizeof(DEFAULT_BROADCAST_BLOCK_CIPHER_KEY));
    memcpy(target->systemTitle, DEFAULT_SYSTEM_TITLE, sizeof(DEFAULT_SYSTEM_TITLE));
    memcpy(target->authenticationKey, DEFAULT_AUTHENTICATION_KEY, sizeof(DEFAULT_AUTHENTICATION_KEY));
    memset(target->dedicatedKey, 0, 16);
#endif //DLMS_IGNORE_MALLOC
    target->broadcast = 0;
}

void cip_clear(ciphering* target)
{
    target->invocationCounter = 1;
    target->security = DLMS_SECURITY_NONE;
    target->encrypt = 0;
#ifndef DLMS_IGNORE_MALLOC
    bb_clear(&target->blockCipherKey);
    bb_clear(&target->broadcastBlockCipherKey);
    bb_clear(&target->systemTitle);
    bb_clear(&target->authenticationKey);
    if (target->dedicatedKey != NULL)
    {
        bb_clear(target->dedicatedKey);
        gxfree(target->dedicatedKey);
        target->dedicatedKey = NULL;
    }
#else
    memset(target->blockCipherKey, 0, sizeof(DEFAULT_BLOCK_CIPHER_KEY));
    memset(target->broadcastBlockCipherKey, 0, sizeof(DEFAULT_BROADCAST_BLOCK_CIPHER_KEY));
    memset(target->systemTitle, 0, 8);
    memset(target->authenticationKey, 0, sizeof(DEFAULT_AUTHENTICATION_KEY));
    memset(target->dedicatedKey, 0, sizeof(DEFAULT_BLOCK_CIPHER_KEY));
#endif //DLMS_IGNORE_MALLOC
}

#define GET_TAG(s)(s->broadcast ? 0x40 : 0) | s->security | s->suite

//Validate authentication tag.
static int cip_validateTag(
    ciphering* settings,
    const DLMS_AES aes,
    unsigned char tag,
    const unsigned char* systemTitle,
    uint32_t frameCounter,
    gxByteBuffer* input,
    unsigned char* key)
{
    int ret;
    unsigned char TAG[12];
    gxByteBuffer ATag;
    BB_ATTACH(ATag, TAG, 0);
    if (bb_available(input) < 13)
    {
        ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    else
    {
        input->size -= 12;
        //Tag is converted to security.
        if ((ret = gxgcm_getTag(tag & 0x30, aes, DLMS_COUNT_TYPE_TAG,
            systemTitle, frameCounter,
#ifdef DLMS_IGNORE_MALLOC
            settings->authenticationKey,
#else
            settings->authenticationKey.data,
#endif //DLMS_IGNORE_MALLOC
            tag, input, &ATag, key)) == 0)
        {
            if (memcmp(input->data + input->size, ATag.data, 12) != 0)
            {
#ifdef DLMS_NOTIFY_AUTHENTICATION_ERROR
                svr_authenticationError();
#endif //DLMS_NOTIFY_AUTHENTICATION_ERROR
                ret = DLMS_ERROR_CODE_INVALID_TAG;
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_MALLOC
int cip_crypt(
    ciphering* settings,
    const DLMS_SECURITY security,
    DLMS_COUNT_TYPE type,
    uint32_t frameCounter,
    unsigned char tag,
    const unsigned char* systemTitle,
    gxByteBuffer* key,
    gxByteBuffer* input,
    unsigned char encrypt)
#else
int cip_crypt(
    ciphering* settings,
    const DLMS_SECURITY security,
    DLMS_COUNT_TYPE type,
    uint32_t frameCounter,
    unsigned char tag,
    const unsigned char* systemTitle,
    unsigned char* key,
    gxByteBuffer* input,
    unsigned char encrypt)
#endif //DLMS_IGNORE_MALLOC
{
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
    //AES 128 uses 176 round key(11 * 16) and 256 uses 240 (15 * 16).
#ifndef GX_DLMS_MICROCONTROLLER
    unsigned char gx_roundKeys[240];
#else
    static unsigned char gx_roundKeys[240];
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
#endif //GX_DLMS_MICROCONTROLLER
    int ret = 0;
    DLMS_AES aes = settings->suite == DLMS_SECURITY_SUITE_V2 ? DLMS_AES_256 : DLMS_AES_128;
    if (ret == 0)
    {
        switch (security)
        {
        case DLMS_SECURITY_AUTHENTICATION:
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
#ifndef DLMS_IGNORE_MALLOC
            gxgcm_init(aes, key->data, gx_roundKeys);
#else
            gxgcm_init(aes, key, gx_roundKeys);
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
            if (encrypt)
            {
                //The authentication tag is validated in a prior step.
                ret = gxgcm_getTag(security, aes, type, systemTitle,
                    frameCounter,
#ifdef DLMS_IGNORE_MALLOC
                    settings->authenticationKey,
#else
                    settings->authenticationKey.data,
#endif //DLMS_IGNORE_MALLOC
                    tag, input, input,
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                    gx_roundKeys
#else
#ifndef DLMS_IGNORE_MALLOC
                    key->data
#else
                    key
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                );
            }
            else
            {
                //Validate authentication tag.
                ret = cip_validateTag(
                    settings,
                    aes,
                    tag,
                    systemTitle,
                    frameCounter,
                    input,
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                    gx_roundKeys
#else
#ifndef DLMS_IGNORE_MALLOC
                    key->data
#else
                    key
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                );
            }
            break;
        case DLMS_SECURITY_ENCRYPTION:
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
#ifndef DLMS_IGNORE_MALLOC
            gxgcm_init(aes, key->data, gx_roundKeys);
#else
            gxgcm_init(aes, key, gx_roundKeys);
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
            ret = gxgcm_transformBlock(aes, systemTitle, frameCounter,
                input->data + input->position, bb_available(input), 2,
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                gx_roundKeys
#else
#ifndef DLMS_IGNORE_MALLOC
                key->data
#else
                key
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
            );
            break;
        case DLMS_SECURITY_AUTHENTICATION_ENCRYPTION:
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
#ifndef DLMS_IGNORE_MALLOC
            gxgcm_init(aes, key->data, gx_roundKeys);
#else
            gxgcm_init(aes, key, gx_roundKeys);
#endif //DLMS_IGNORE_MALLOC  
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
            //Validate authentication tag.
            if (encrypt == 0)
            {
                ret = cip_validateTag(
                    settings,
                    aes,
                    tag,
                    systemTitle,
                    frameCounter,
                    input,
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                    gx_roundKeys
#else
#ifndef DLMS_IGNORE_MALLOC
                    key->data
#else
                    key
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                );
            }
            if (ret == 0 &&
                (ret = gxgcm_transformBlock(aes, systemTitle, frameCounter,
                    input->data + input->position, bb_available(input), 2,
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                    gx_roundKeys
#else
#ifndef DLMS_IGNORE_MALLOC
                    key->data
#else
                    key
#endif //DLMS_IGNORE_MALLOC  
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                )) == 0)
            {
                if (encrypt)
                {
                    //The authentication tag is validated in a prior step.
                    ret = gxgcm_getTag(
                        security, aes, type, systemTitle, frameCounter,
#ifdef DLMS_IGNORE_MALLOC
                        settings->authenticationKey,
#else
                        settings->authenticationKey.data,
#endif //DLMS_IGNORE_MALLOC
                        GET_TAG(settings), input, input,
#ifndef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                        gx_roundKeys
#else
#ifndef DLMS_IGNORE_MALLOC
                        key->data
#else
                        key
#endif //DLMS_IGNORE_MALLOC
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE
                    );
                }
            }
            break;
        default:
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if (ret == 0 && encrypt)
        {
            ++settings->invocationCounter;
        }
        if (ret == 0 && encrypt && type == DLMS_COUNT_TYPE_PACKET)
        {
            //Nonse must be 20 bytes because it's used later.
#ifdef GX_DLMS_MICROCONTROLLER
            static unsigned char NONSE[20] = { 0 };
#else
            unsigned char NONSE[20] = { 0 };
#endif //GX_DLMS_MICROCONTROLLER
            gxByteBuffer nonse;
            bb_attach(&nonse, NONSE, 0, sizeof(NONSE));
            if ((ret = bb_setUInt8(&nonse, tag)) == 0)
            {
                if (tag == DLMS_COMMAND_GENERAL_GLO_CIPHERING ||
                    tag == DLMS_COMMAND_GENERAL_DED_CIPHERING)
                {
                    hlp_setObjectCount(8, &nonse);
                    bb_set(&nonse, systemTitle, 8);
                }
                tag = (settings->broadcast ? 0x40 : 0) | settings->security | settings->suite;
                if ((ret = hlp_setObjectCount(5 + input->size, &nonse)) == 0 &&
                    (ret = bb_setUInt8(&nonse, tag)) == 0 &&
                    (ret = bb_setUInt32(&nonse, frameCounter)) == 0 &&
                    (ret = bb_insert(nonse.data, nonse.size, input, 0)) == 0)
                {
                }
            }
        }
    }
    return ret;
}

#ifndef DLMS_IGNORE_MALLOC
int cip_encrypt(
    ciphering* settings,
    DLMS_SECURITY security,
    DLMS_COUNT_TYPE type,
    uint32_t frameCounter,
    unsigned char tag,
    const unsigned char* systemTitle,
    gxByteBuffer* key,
    gxByteBuffer* input)
#else
int cip_encrypt(
    ciphering* settings,
    DLMS_SECURITY security,
    DLMS_COUNT_TYPE type,
    uint32_t frameCounter,
    unsigned char tag,
    const unsigned char* systemTitle,
    unsigned char* key,
    gxByteBuffer* input)
#endif //DLMS_IGNORE_MALLOC
{
#ifdef DLMS_DEBUG
    svr_notifyTrace5(GET_STR_FROM_EEPROM("System title: "), systemTitle, 8);
#ifndef DLMS_IGNORE_MALLOC
    svr_notifyTrace5(GET_STR_FROM_EEPROM("Block cipher key: "), key->data, key->size);
    svr_notifyTrace5(GET_STR_FROM_EEPROM("Authentication key: "), settings->authenticationKey.data, settings->authenticationKey.size);
#else
    svr_notifyTrace5(GET_STR_FROM_EEPROM("Block cipher key: "), key, 16);
    svr_notifyTrace5(GET_STR_FROM_EEPROM("Authentication key: "), settings->authenticationKey, 16);
#endif //DLMS_IGNORE_MALLOC

#endif //DLMS_DEBUG
#ifndef DLMS_IGNORE_MALLOC
    unsigned char keySize = settings->suite == DLMS_SECURITY_SUITE_V2 ? 32 : 16;
    if (settings->security == DLMS_SECURITY_NONE ||
        bb_available(&settings->authenticationKey) != keySize)
    {
        //Invalid system title.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#endif //DLMS_IGNORE_MALLOC
    return cip_crypt(
        settings,
        security,
        type,
        frameCounter,
        tag,
        systemTitle,
        key,
        input,
        1);
}

#ifndef DLMS_IGNORE_MALLOC
int cip_decrypt(
    ciphering* settings,
    unsigned char* title,
    gxByteBuffer* key,
    gxByteBuffer* data,
    DLMS_SECURITY* security,
    DLMS_SECURITY_SUITE* suite,
    uint64_t* invocationCounter)
#else
int cip_decrypt(
    ciphering* settings,
    unsigned char* title,
    unsigned char* key,
    gxByteBuffer* data,
    DLMS_SECURITY* security,
    DLMS_SECURITY_SUITE* suite,
    uint64_t* invocationCounter)
#endif //DLMS_IGNORE_MALLOC
{
#ifdef GX_DLMS_MICROCONTROLLER
    static unsigned char systemTitle[8];
#else
    unsigned char systemTitle[8];
#endif //GX_DLMS_MICROCONTROLLER
    uint16_t length;
    int ret;
    unsigned char ch;
    uint32_t frameCounter, index = data->position;
    DLMS_COMMAND cmd;
    if (data == NULL || bb_available(data) < 2)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return ret;
    }
    cmd = (DLMS_COMMAND)ch;
    switch (cmd)
    {
    case DLMS_COMMAND_GENERAL_GLO_CIPHERING:
    case DLMS_COMMAND_GENERAL_DED_CIPHERING:
        if ((ret = hlp_getObjectCount2(data, &length)) != 0)
        {
            return ret;
        }
        if (length != 0)
        {
            if (length != 8)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            bb_get(data, systemTitle, length);
            title = systemTitle;
        }
        break;
    case DLMS_COMMAND_GLO_INITIATE_REQUEST:
    case DLMS_COMMAND_GLO_INITIATE_RESPONSE:
    case DLMS_COMMAND_GLO_READ_REQUEST:
    case DLMS_COMMAND_GLO_READ_RESPONSE:
    case DLMS_COMMAND_GLO_WRITE_REQUEST:
    case DLMS_COMMAND_GLO_WRITE_RESPONSE:
    case DLMS_COMMAND_GLO_GET_REQUEST:
    case DLMS_COMMAND_GLO_GET_RESPONSE:
    case DLMS_COMMAND_GLO_SET_REQUEST:
    case DLMS_COMMAND_GLO_SET_RESPONSE:
    case DLMS_COMMAND_GLO_METHOD_REQUEST:
    case DLMS_COMMAND_GLO_METHOD_RESPONSE:
    case DLMS_COMMAND_GLO_EVENT_NOTIFICATION_REQUEST:
    case DLMS_COMMAND_DED_INITIATE_REQUEST:
    case DLMS_COMMAND_DED_INITIATE_RESPONSE:
    case DLMS_COMMAND_DED_GET_REQUEST:
    case DLMS_COMMAND_DED_GET_RESPONSE:
    case DLMS_COMMAND_DED_SET_REQUEST:
    case DLMS_COMMAND_DED_SET_RESPONSE:
    case DLMS_COMMAND_DED_METHOD_REQUEST:
    case DLMS_COMMAND_DED_METHOD_RESPONSE:
    case DLMS_COMMAND_DED_EVENT_NOTIFICATION:
        break;
    default:
        return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
    }
    if ((ret = hlp_getObjectCount2(data, &length)) != 0)
    {
        return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
    }
    if ((ret = bb_getUInt8(data, &ch)) != 0)
    {
        return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
    }
    *security = (DLMS_SECURITY)(ch & 0x30);
    if (suite != NULL)
    {
        *suite = (DLMS_SECURITY_SUITE)(ch & 0x3);
        if (settings->security != DLMS_SECURITY_NONE &&
            *suite != settings->suite)
        {
            return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
        }
    }
    //If Key_Set or authentication or encryption is not used.
    if ((settings->broadcast && (ch & 0x40) == 0) ||
        (!settings->broadcast && (ch & 0x40) != 0) ||
        *security == DLMS_SECURITY_NONE ||
        //If security level is set.
        (settings->security != DLMS_SECURITY_NONE &&
            *security != settings->security))
    {
        return DLMS_ERROR_CODE_INVALID_DECIPHERING_ERROR;
    }
    if ((ret = bb_getUInt32(data, &frameCounter)) != 0)
    {
        return ret;
    }
    if (invocationCounter != NULL)
    {
        *invocationCounter = frameCounter;
    }
    ret = cip_crypt(
        settings,
        *security,
        DLMS_COUNT_TYPE_DATA,
        frameCounter,
        ch,
        title,
        key,
        data,
        0);
    if (ret == 0)
    {
        //Remove ciphering part from the data.
        ret = bb_move(data, data->position, index, bb_available(data));
        data->position = index;
    }
    return ret;
}

static const unsigned char WRAP_IV[] = { 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6 };

int cip_encryptKey(
    gxByteBuffer* kek,
    gxByteBuffer* data,
    gxByteBuffer* output)
{
    int ret;
    unsigned char BUF[16] = { 0 };
    unsigned char BLOCK[32 + 8] = { 0 };
    unsigned char n, j, i;
    gxByteBuffer bb, block;
    BB_ATTACH(bb, BUF, 16);
    BB_ATTACH(block, BLOCK, 0);

    if (kek == NULL || (kek->size != 16 && kek->size != 32) ||
        data == NULL || (data->size != 16 && data->size != 32) ||
        output == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_capacity(output, output->size + data->size + sizeof(WRAP_IV))) != 0)
    {
        return ret;
    }
    DLMS_AES aes = kek->size == 16 ? DLMS_AES_128 : DLMS_AES_256;
    // Amount of 64-bit blocks.
    n = (unsigned char)(data->size >> 3);
    memcpy(BLOCK, WRAP_IV, sizeof(WRAP_IV));
    memcpy(BLOCK + sizeof(WRAP_IV), data->data, data->size);
    for (j = 0; j != 6; j++)
    {
        for (i = 1; i <= n; i++)
        {
            memcpy(BUF, BLOCK, 8);
            memcpy(BUF + sizeof(WRAP_IV), BLOCK + 8 * i, 8);
            if ((ret = gxaes_encrypt(aes, &bb, kek, &bb)) != 0)
            {
                break;
            }
            unsigned int t = n * j + i;
            for (int k = 1; t != 0; k++)
            {
                unsigned char v = (unsigned char)t;
                BUF[sizeof(WRAP_IV) - k] ^= v;
                t = (int)((unsigned int)t >> 8);
            }
            memcpy(BLOCK, BUF, 8);
            memcpy(BLOCK + (8 * i), BUF + 8, 8);
        }
    }
    return bb_set(output, BLOCK, 8 + data->size);
}

int cip_decryptKey(
    gxByteBuffer* kek,
    gxByteBuffer* data,
    gxByteBuffer* output)
{
    int ret;
    unsigned char A[8];
    unsigned char BUF[16];
    signed char j, i;
    unsigned char k, v, n;
    gxByteBuffer a, buf;
    BB_ATTACH(a, A, 0);
    BB_ATTACH(buf, BUF, 16);
    uint16_t t;
    DLMS_AES aes;
    if (bb_size(kek) == 16)
    {
        aes = DLMS_AES_128;
    }
    else if (bb_size(kek) == 32)
    {
        aes = DLMS_AES_256;
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    // Amount of 64-bit blocks.
    n = (unsigned char)(bb_size(data) >> 3);
    if (kek == NULL || bb_size(kek) != 16 || bb_size(data) != n * 8 ||
        output == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = bb_capacity(output, bb_size(data) > sizeof(WRAP_IV) ?
        bb_size(data) - sizeof(WRAP_IV) : sizeof(WRAP_IV))) != 0)
    {
        return ret;
    }
    output->size = 16;
    output->position = 0;
    memcpy(A, data->data, sizeof(WRAP_IV));
    memcpy(output->data, data->data + sizeof(WRAP_IV), data->size - sizeof(WRAP_IV));
    if (--n == 0)
    {
        n = 1;
    }
    for (j = 5; j >= 0; --j)
    {
        for (i = n; i >= 1; --i)
        {
            memcpy(BUF, A, sizeof(WRAP_IV));
            memcpy(BUF + sizeof(WRAP_IV), output->data + 8 * (i - 1), 8);
            t = n * j + i;
            for (k = 1; t != 0; k++)
            {
                v = (unsigned char)t;
                BUF[sizeof(WRAP_IV) - k] ^= v;
                t = (uint16_t)(t >> 8);
            }
            if ((ret = gxaes_decrypt(aes, &buf, kek, &buf)) != 0)
            {
                break;
            }
            memcpy(A, BUF, 8);
            memcpy(output->data + 8 * (i - 1), BUF + 8, 8);
        }
    }
    if (memcmp(A, WRAP_IV, sizeof(WRAP_IV)) != 0)
    {
        output->size = 0;
        return DLMS_ERROR_CODE_FALSE;
    }
    return 0;
}

#endif //DLMS_IGNORE_HIGH_GMAC
