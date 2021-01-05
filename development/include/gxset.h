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

#ifndef COSEM_SET_H
#define COSEM_SET_H

#ifdef  __cplusplus
extern "C" {
#endif

#if defined(DLMS_IGNORE_MALLOC) || defined(DLMS_COSEM_EXACT_DATA_TYPES)
#include "../include/gxsetignoremalloc.h"
#else
#include "../include/gxsetmalloc.h"
#endif //DLMS_IGNORE_MALLOC
#include "gxobjects.h"
#include "dlmssettings.h"
    int cosem_setValue(dlmsSettings* settings, gxValueEventArg *e);

    /////////////////////////////////////////////////////////////////////////
    // The season profile is sorted according to start date
    // (in increasing order).
    int cosem_orderSeasonProfile(gxArray* profile);
#ifdef  __cplusplus
}
#endif

#endif//COSEM_SET_H
