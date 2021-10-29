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

#ifndef GX_MEM
#define GX_MEM
#include "gxignore.h"
#if !defined(DLMS_USE_CUSTOM_MALLOC) && !defined(DLMS_IGNORE_MALLOC)

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdlib.h> // malloc and free needs this or error is generated.
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif

#ifndef gxfree
#define gxfree(p)            free(p)
#endif

#ifndef gxmalloc
#define gxmalloc(s)          malloc(s)
#endif

#ifndef gxcalloc
#define gxcalloc(p, s)       calloc(p, s)
#endif


#ifndef gxrealloc
#define gxrealloc(p, s)      realloc(p, s)
#endif

#ifdef  __cplusplus
}
#endif
#endif //!defined(DLMS_USE_CUSTOM_MALLOC) && !defined(DLMS_IGNORE_MALLOC)
#endif //GX_MEM
