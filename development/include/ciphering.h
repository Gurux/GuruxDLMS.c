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

#ifndef CHIPPERING_H
#define CHIPPERING_H
#include "gxignore.h"
#ifndef DLMS_IGNORE_HIGH_GMAC
#ifdef  __cplusplus
extern "C" {
#endif

#include "bytebuffer.h"
#include "enums.h"
#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
#include "privateKey.h"
#include "publicKey.h"
#endif

    typedef struct
    {
        /* Used security.*/
        DLMS_SECURITY security;

        /* Used security suite.*/
        DLMS_SECURITY_SUITE suite;
        /*Used security policy.*/
        DLMS_SECURITY_POLICY securityPolicy;

        /*Is data encrypted.*/
        unsigned char encrypt;

        /**
        * Block cipher key.
        */
#ifndef DLMS_IGNORE_MALLOC
        gxByteBuffer blockCipherKey;
#else
        unsigned char blockCipherKey[32];
#endif //DLMS_IGNORE_MALLOC

        /**
        * Broadcast block cipher key.
        */
#ifndef DLMS_IGNORE_MALLOC
        gxByteBuffer broadcastBlockCipherKey;
#else
        unsigned char broadcastBlockCipherKey[32];
#endif //DLMS_IGNORE_MALLOC

        /**
        * System title.
        */
#ifndef DLMS_IGNORE_MALLOC
        gxByteBuffer systemTitle;
#else
        unsigned char systemTitle[8];
#endif //DLMS_IGNORE_MALLOC

        /**
        * Invocation (Frame) counter.
        */
        uint32_t invocationCounter;
#ifndef DLMS_IGNORE_MALLOC
        gxByteBuffer authenticationKey;
#else
        unsigned char authenticationKey[32];
#endif //DLMS_IGNORE_MALLOC

        //Dedicated key.
#ifndef DLMS_IGNORE_MALLOC
        gxByteBuffer* dedicatedKey;
#else
        unsigned char dedicatedKey[32];
#endif //DLMS_IGNORE_MALLOC
        //Is data send as a broadcast or unicast.
        unsigned char broadcast;

#if defined(DLMS_SECURITY_SUITE_1) || defined(DLMS_SECURITY_SUITE_2)
        /*Used certificate type.*/
        DLMS_CERTIFICATE_TYPE certificateType;
        /*Transaction Id.*/
        unsigned char* TransactionId;
        /*Private key to used to encrypt the data.*/
        gxPrivateKey* privateKey;
        /*Public key to used to decrypt the data.*/
        gxPublicKey PublicKey;
#endif
    } ciphering;


    void cip_init(ciphering* target);
    void cip_clear(ciphering* target);
    /**
    * Encrypt data.
    */
#ifndef DLMS_IGNORE_MALLOC
    int cip_encrypt(
        ciphering* settings,
        DLMS_SECURITY security,
        DLMS_COUNT_TYPE type,
        uint32_t frameCounter,
        unsigned char tag,
        const unsigned char* systemTitle,
        gxByteBuffer* key,
        gxByteBuffer* input);
#else
    int cip_encrypt(
        ciphering* settings,
        DLMS_SECURITY security,
        DLMS_COUNT_TYPE type,
        uint32_t frameCounter,
        unsigned char tag,
        const unsigned char* systemTitle,
        unsigned char* key,
        gxByteBuffer* input);
#endif //DLMS_IGNORE_MALLOC

    /**
    * Decrypt data.
    */
#ifndef DLMS_IGNORE_MALLOC
    int cip_decrypt(
        ciphering* settings,
        unsigned char* title,
        gxByteBuffer* key,
        gxByteBuffer* data,
        DLMS_SECURITY* security,
        DLMS_SECURITY_SUITE* suite,
        uint64_t* invocationCounter);
#else
    int cip_decrypt(
        ciphering* settings,
        unsigned char* title,
        unsigned char* key,
        gxByteBuffer* data,
        DLMS_SECURITY* security,
        DLMS_SECURITY_SUITE* suite,
        uint64_t* invocationCounter);
#endif //DLMS_IGNORE_MALLOC

    // Encrypt data using AES RFC3394.
    int cip_encryptKey(
        unsigned char* kek,
        //KEK size.
        unsigned char size,
        gxByteBuffer* data,
        gxByteBuffer* output);

    // Decrypt data using AES RFC3394.
    //Returns DLMS_ERROR_CODE_FALSE if data is not encrypted with the key.
    int cip_decryptKey(
        unsigned char* kek,
        //KEK size.
        unsigned char size,
        gxByteBuffer* data,
        gxByteBuffer* output);

    //Show decoded PDU. 
    // This can be used to show decoded PDU. 
    //In default this is not used.
    //It's used after DLMS_TRACE_PDU is defined.
#ifdef DLMS_TRACE_PDU
/*
* Show send or received PDU without ciphering. This can be use in debugging.
* encrypt: If true, the data is encrypt (send) and if false, it's decrypted (received).
* pdu: decrypted Pdu.
*/
    extern void cip_tracePdu(
        unsigned char encrypt,
        gxByteBuffer* pdu);
#endif //DLMS_TRACE_PDU

#ifdef DLMS_USE_AES_HARDWARE_SECURITY_MODULE
    /*Called to encrypt or decrypt the data using external AES Hardware Security Module.*/
    extern int gx_hsmAes(const unsigned char* input, const unsigned char* key, unsigned char* output, const size_t length);
#endif //DLMS_USE_AES_HARDWARE_SECURITY_MODULE

#ifdef DLMS_USE_CRC_HARDWARE_SECURITY_MODULE
    /*Called to encrypt or decrypt the data using external CRC Hardware Security Module.*/
    extern uint16_t gx_hsmCrc(const unsigned char* data, const uint16_t length);
#endif //DLMS_USE_CRC_HARDWARE_SECURITY_MODULE

#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_HIGH_GMAC
#endif //CHIPPERING_H
