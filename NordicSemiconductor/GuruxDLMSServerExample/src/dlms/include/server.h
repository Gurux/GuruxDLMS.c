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

#ifndef SERVER_H
#define SERVER_H

#include "gxignore.h"
#ifndef DLMS_IGNORE_SERVER

#ifdef  __cplusplus
extern "C" {
#endif

#include "serverevents.h"
#include "dlms.h"

    int svr_initialize(
        dlmsServerSettings* settings);

    int svr_handleRequest(
        dlmsServerSettings* settings,
        gxByteBuffer* data,
        gxByteBuffer* reply);

    int svr_handleRequest2(
        dlmsServerSettings* settings,
        unsigned char* buff,
        unsigned short size,
        gxByteBuffer* reply);

    //Handle only one received byte.
    int svr_handleRequest3(
        dlmsServerSettings* settings,
        unsigned char data,
        gxByteBuffer* reply);

    void svr_reset(
        dlmsServerSettings* settings);

    /*
    Run the background processes.
    */
    int svr_run(
        dlmsServerSettings* settings);

#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    /**
    * Update short names.
    */
    int svr_updateShortNames(
        dlmsServerSettings* settings,
        unsigned char force);
#endif  //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_SERVER
#endif //SERVER_H
