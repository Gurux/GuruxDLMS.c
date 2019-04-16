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


typedef struct
{
    /**
    * Used security.
    */
    DLMS_SECURITY security;

    /**
    * Is data encrypted.
    */
    unsigned char encrypt;

    /**
    * Block cipher key.
    */
    gxByteBuffer blockCipherKey;

    /**
    * System title.
    */
    gxByteBuffer systemTitle;

    /**
    * Invocation (Frame) counter.
    */
    unsigned long invocationCounter;
    gxByteBuffer authenticationKey;

    //Dedicated key.
    gxByteBuffer *dedicatedKey;

} ciphering;


void cip_init(ciphering* target);
void cip_clear(ciphering* target);
/**
* Encrypt data.
*/
int cip_encrypt(
    ciphering *settings,
    DLMS_SECURITY security,
    DLMS_COUNT_TYPE type,
    unsigned long frameCounter,
    unsigned char tag,
    gxByteBuffer *systemTitle,
    gxByteBuffer *key,
    gxByteBuffer *input,
    gxByteBuffer *output);

/**
* Decrypt data.
*/
int cip_decrypt(
    ciphering *settings,
    gxByteBuffer *title,
    gxByteBuffer *key,
    gxByteBuffer *data,
    DLMS_SECURITY *security);

// Encrypt data using AES RFC3394.
int cip_encryptKey(
    gxByteBuffer* kek,
    gxByteBuffer* data,
    gxByteBuffer* output);

// Decrypt data using AES RFC3394.
//Returns DLMS_ERROR_CODE_FALSE if data is not encrypted with the key.
int cip_decryptKey(
    gxByteBuffer* kek,
    gxByteBuffer* data,
    gxByteBuffer* output);

#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_HIGH_GMAC
#endif //CHIPPERING_H
