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

#ifndef GXAES1
#define GXAES1

#include "gxignore.h"
#ifndef DLMS_IGNORE_AES

#ifdef  __cplusplus
extern "C" {
#endif


#ifndef CBC
#define CBC 1
#endif

#ifndef ECB
#define ECB 1
#endif

#define AES128 1
    //#define AES192 1
    //#define AES256 1

#if defined(ECB) && (ECB == 1)

    void gxaes_ecb_encrypt(const unsigned char* input, const unsigned char* key, unsigned char *output, const size_t length);
    void gxaes_ecb_decrypt(const unsigned char* input, const unsigned char* key, unsigned char *output, const size_t length);

#endif // #if defined(ECB) && (ECB == !)

#if defined(CBC) && (CBC == 1)
    void gxaes_cbc_encrypt(unsigned char* output, unsigned char* input, uint32_t length, const unsigned char* key, const unsigned char* iv);
    void gxaes_cbc_decrypt(unsigned char* output, unsigned char* input, uint32_t length, const unsigned char* key, const unsigned char* iv);

#endif // #if defined(CBC) && (CBC == 1)

#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_AES
#endif //GXAES1
