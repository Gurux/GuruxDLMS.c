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

#ifndef GXAES
#define GXAES

#include "gxignore.h"
#ifndef DLMS_IGNORE_AES
#include "bytebuffer.h"
#include "enums.h"

#ifdef  __cplusplus
extern "C" {
#endif   
    void gxaes_keyExpansion(
        const DLMS_AES eas,
        const unsigned char* key,
        unsigned char* roundKeys);

    void gxaes_cipher(const DLMS_AES eas, 
        unsigned char encrypt,
        const unsigned char* input,
        const unsigned char* roundKeys,
        unsigned char* output);

    /* Encrypt the AES data.*/
    int gxaes_encrypt(const DLMS_AES eas,
        gxByteBuffer* data,
        gxByteBuffer* secret, 
        gxByteBuffer* output);

    /* Decrypt the AES data.*/
    int gxaes_decrypt(const DLMS_AES eas,
        gxByteBuffer* data,
        gxByteBuffer* secret,
        gxByteBuffer* output);   

#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_AES
#endif //GXAES
