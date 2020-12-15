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
#ifndef GXDEFINE_H
#define GXDEFINE_H

#ifdef  __cplusplus
extern "C" {
#endif

//Define max user name length.
#define MAX_USER_NAME_LENGTH 20

//Define max destination length.
#define MAX_DESTINATION_LENGTH 20

//Define max SAP logical device name length.
#define MAX_SAP_LOGICAL_DEVICE_NAME_LENGTH 20

//Define max season profile name length.
#define MAX_SEASON_PROFILE_NAME_LENGTH 20

//Define max season profile name length.
#define MAX_SEASON_PROFILE_NAME_LENGTH 20

//Define max season profile week name length.
#define MAX_SEASON_PROFILE_WEEK_NAME_LENGTH 20

//Define max week profile name length.
#define MAX_SEASON_WEEK_PROFILE_NAME_LENGTH 20

//Define max IP4 setup IP option data length.
#define MAX_IP4_SETUP_IP_OPTION_DATA_LENGTH 20

//Define max SAP item name length.
#define MAX_SAP_ITEM_NAME_LENGTH 20

//Define max modem initialization string length.
#define MAX_MODEM_INITIALIZATION_STRING_LENGTH 20

//Define max modem profile string length.
#define MAX_MODEM_PROFILE_STRING_LENGTH 20

//Define max certificate serial number length.
#define MAX_CERTIFICATE_SERIAL_NUMBER_LENGTH 20

//Define max certificate issuer length.
#define MAX_CERTIFICATE_ISSUER_LENGTH 40

//Define max certificate subject length.
#define MAX_CERTIFICATE_SUBJECT_LENGTH 20

//Define max certificate subject alt length.
#define MAX_CERTIFICATE_SUBJECT_ALT_LENGTH 40

//Define max certificate subject alt length.
#define MAX_CAPTURE_DEFINITION_ELEMENT_LENGTH 100

//Define max certificate subject alt length.
#define MAX_CHARGE_TABLE_INDEX_LENGTH 100

//Define max certificate subject alt length.
#define MAX_TOKEN_GATEWAY_DESCRIPTION_LENGTH 20

//Define max image identification length.
#define MAX_IMAGE_IDENTIFICATION_LENGTH 20

//Define image signature length.
#define MAX_IMAGE_SIGNATURE_LENGTH 20

//Define max push setup target length.
#define MAX_PUSH_SETUP_TARGET_LENGTH 20

//Define max currency name length.
#define MAX_CURRENCY_NAME_LENGTH 20

//Define max client PDU size.
#define MAX_CLIENT_PDU_SIZE 200

//Define max capture object buffer size.
#define MAX_CAPTURE_OBJECT_BUFFER_SIZE 100

//Define max challenge size.
#define MAX_CHALLENGE_SIZE 64

//Define max register activation mask name length.
#define MAX_REGISTER_ACTIVATION_MASK_NAME_LENGTH 10

//Define max reporting system item name length.
#define MAX_REPORTING_SYSTEM_ITEM_LENGTH 20

//Define max SAP item count. This is used when data is serialized.
#define MAX_SAP_ITEM_SERIALIZE_COUNT 5
#ifdef  __cplusplus
}
#endif

#endif //GXDEFINE_H
