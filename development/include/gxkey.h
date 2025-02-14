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

#ifndef GXKEY_H
#define GXKEY_H
#include "gxignore.h"
#include "gxint.h"
#ifdef  __cplusplus
extern "C" {
#endif
#ifndef DLMS_IGNORE_MALLOC

typedef struct {
    void *key;
    void *value;
} gxKey;

typedef struct {
    unsigned char key;
    void *value;
} gxKey2;

typedef struct {
    uint32_t key;
    void* value;
} gxKey3;

//Make key.
    gxKey* key_init(void* key, void* value);

    gxKey2* key_init2(unsigned char key, void* value);

    gxKey3* key_init3(uint32_t key, void* value);

#endif //DLMS_IGNORE_MALLOC
#ifdef  __cplusplus
}
#endif
#endif //GXKEY_H
