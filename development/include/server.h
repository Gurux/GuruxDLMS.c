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
        uint16_t size,
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
        dlmsServerSettings* settings,
        //Current EPOCH time.
        uint32_t time,
        //Next EPOCH execution time.
        uint32_t* next);

#ifndef DLMS_IGNORE_REGISTER_MONITOR
    /*
   The server verifies that value of monitored object don't cross the thresholds.
   Selected script is invoked if value is passed.
   */
    int svr_monitor(dlmsServerSettings* settings, gxRegisterMonitor* object);
    /*
   The server verifies that values of monitored objects don't cross the thresholds.
   Selected script is invoked if value is passed.
   */
    int svr_monitorAll(dlmsServerSettings* settings);
#endif //DLMS_IGNORE_REGISTER_MONITOR


#ifndef DLMS_IGNORE_ASSOCIATION_SHORT_NAME
    /**
    * Update short names.
    */
    int svr_updateShortNames(
        dlmsServerSettings* settings,
        unsigned char force);
#endif  //DLMS_IGNORE_ASSOCIATION_SHORT_NAME

    //Check is client changing the settings with action.
    //This can be used to check is meter data changed.
    //Return value is saved atribute index or zero if nothing hasn't change.
    uint32_t svr_isChangedWithAction(DLMS_OBJECT_TYPE objectType, unsigned char methodIndex);
#ifdef  __cplusplus
}
#endif
#endif //DLMS_IGNORE_SERVER
#endif //SERVER_H
