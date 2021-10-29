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

#ifndef COSEM_INVOKE_H
#define COSEM_INVOKE_H

#ifdef  __cplusplus
extern "C" {
#endif
#include "gxignore.h"
#ifndef DLMS_IGNORE_SERVER

#include "gxobjects.h"
#include "dlmssettings.h"

    int cosem_invoke(
        dlmsServerSettings* settings,
        gxValueEventArg* e);

#ifndef DLMS_IGNORE_COMPACT_DATA
    /*
    * Copies the values of the objects to capture into the buffer by reading
    * capture objects.
    */
    int cosem_captureCompactData(
        dlmsSettings* settings,
        gxCompactData* object);
#endif //DLMS_IGNORE_COMPACT_DATA

#ifndef DLMS_IGNORE_SCRIPT_TABLE
    int invoke_ScriptTable(
        dlmsServerSettings* settings,
        gxValueEventArg* e);
#endif //DLMS_IGNORE_SCRIPT_TABLE

#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_SERVER

#endif //COSEM_INVOKE_H
