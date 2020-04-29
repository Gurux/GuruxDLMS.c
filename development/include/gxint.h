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

#ifndef GXINT_H
#define GXINT_H

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif
#ifndef uint16_t
#define uint16_t unsigned short
#endif //uint16_t
#ifndef uint32_t
#define uint32_t unsigned long
#endif //uint32_t
#ifndef uint64_t
#define uint64_t unsigned long long
#endif //uint64_t

#ifndef int16_t
#define int16_t short
#endif //uint16_t
#ifndef int32_t
#define int32_t long
#endif //int32_t
#ifndef int64_t
#define int64_t long long
#endif //int64_t

#ifdef  __cplusplus
}
#endif

#endif //GXINT_H
