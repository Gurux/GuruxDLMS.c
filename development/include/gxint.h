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
#if !defined(_WIN32) && !defined(_WIN64) && !defined(__linux__) && !defined(ARDUINO) && !(__STDC_VERSION__ >= 199901L)
#ifdef  __cplusplus
extern "C" {
#endif
#if !defined(uint16_t) && !defined(_UINT16_T_DECLARED)
#define uint16_t unsigned short
#endif //!defined(uint16_t) && !defined(_UINT16_T_DECLARED)
#if !defined(uint32_t) && !defined(_UINT32_T_DECLARED)
#define uint32_t unsigned int
#endif //!defined(uint32_t) && !defined(_UINT32_T_DECLARED)
#if !defined(uint64_t) && !defined(_UINT64_T_DECLARED)
#define uint64_t unsigned long long
#endif //#!defined(uint64_t) && !defined(_UINT64_T_DECLARED)

#if !defined(int16_t) && !defined(_INT16_T_DECLARED)
#define int16_t short
#endif //!defined(int16_t) && !defined(_INT16_T_DECLARED)
#if !defined(int32_t) && !defined(_INT32_T_DECLARED)
#define int32_t int
#endif //!defined(int32_t) && !defined(_INT32_T_DECLARED)
#if !defined(int64_t) && !defined(_INT64_T_DECLARED)
#define int64_t long long
#endif //!defined(int64_t) && !defined(_INT64_T_DECLARED)

#ifdef  __cplusplus
}
#endif
#endif //!defined(_WIN32) && !defined(_WIN64) && !defined(__linux__) && !defined(ARDUINO)
#endif //GXINT_H
