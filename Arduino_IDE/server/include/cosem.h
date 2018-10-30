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

#ifndef COSEM_H
#define COSEM_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gxobjects.h"

  int cosem_createObject(
      DLMS_OBJECT_TYPE type,
      gxObject** object);

  int cosem_createObject2(
      DLMS_OBJECT_TYPE type,
      const char* ln,
      gxObject** object);

  int cosem_setLogicalName(
      gxObject* object,
      unsigned char* value);

  int cosem_init(
      gxObject* object,
      DLMS_OBJECT_TYPE type,
      const char* logicalNameString);
    
  int cosem_init2(
    gxObject* object,
    DLMS_OBJECT_TYPE type,
    const unsigned char* ln);

#ifdef  __cplusplus
}
#endif

#endif //COSEM_H
