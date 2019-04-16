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

#ifndef GXHELPERS_H
#define GXHELPERS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "bytebuffer.h"
#include "variant.h"

  //Get UInt32.
#define GETU32(pt) (((unsigned long)(pt)[0] << 24) | \
                    ((unsigned long)(pt)[1] << 16) | \
                    ((unsigned long)(pt)[2] <<  8) | \
                    ((unsigned long)(pt)[3]))

//Set Int32 as Big Endian value.
#define PUT32(ct, st) { \
                    (ct)[0] = (unsigned char)((st) >> 24); \
                    (ct)[1] = (unsigned char)((st) >> 16); \
                    (ct)[2] = (unsigned char)((st) >>  8); \
                    (ct)[3] = (unsigned char)(st); }

    //Check byte order.
    unsigned char hlp_isBigEndian(void);

#ifndef GX_DLMS_MICROCONTROLLER
    char* hlp_getErrorMessage(short err);
#endif //GX_DLMS_MICROCONTROLLER

    //Returns items count. Use hlp_getObjectCount22. 
    int hlp_getObjectCount(gxByteBuffer* buff);

    //Returns items count. 
    int hlp_getObjectCount2(
        gxByteBuffer* buff, 
        unsigned short* count);

    //Get count size in bytes.
    unsigned char hlp_getObjectCountSizeInBytes(unsigned long count);

    // Set count of items.
    int hlp_setObjectCount(
        unsigned long count,
        gxByteBuffer* buff);

    /**
    * Convert byte array to hex string.
    */
    char* hlp_bytesToHex(const unsigned char* pBytes, int count);

    /**
    * Convert hex string to byte array.
    */
    int hlp_hexToBytes(const char* str, unsigned char** arr, unsigned char* count);

    //Set logical name from string.
    int hlp_setLogicalName(unsigned char ln[6], const char* name);

#ifndef GX_DLMS_MICROCONTROLLER
    void hlp_trace(unsigned char* data, int index, int count, unsigned char send);
    //Get Logical Name from string.
    int hlp_parseLogicalName(gxByteBuffer* value, unsigned char ln[6]);

    //Set logical name from string.
    int hlp_setLogicalName2(dlmsVARIANT* ln, const char* name);

    int hlp_getLogicalNameToString(const unsigned char value[6], char* ln);

    int hlp_appendLogicalName(gxByteBuffer* bb, const unsigned char value[6]);

    //Print logical name to cout.
    int hlp_printLogicalName(
        //Format.
        const char* format, 
        //Logical name.
        const unsigned char value[6]);
#endif


    void hlp_replace(gxByteBuffer* str, char oldCh, char newCh);

    double hlp_getScaler(int scaler);

    /**
    * Get data type in bytes.
    *
    * @param type
    *            Data type.
    * @return Size of data type in bytes.
    */
    int hlp_getDataTypeSize(DLMS_DATA_TYPE type);

	/**
	* Convert integer to string.
	*
	* @param str
	*            Parsed string.
	* @param strsize
	*            String size.
	* @param value
	*            Integer value.
	* @param isSigned
	*            Is value signed number.
	* @return Length of the string or -1 if error has occurred.
	*/
	int hlp_intToString(
		char *str, 
		int bufsize, 
		long value, 
		unsigned char isSigned);

	/**
	* Convert string to integer.
	*
	* @param str
	*            Parsed string.
	* @return Value of string as integer.
	*/
	long hlp_stringToInt(
		const char *str);
	/**
	* Convert integer to string.
	*
	* @param str
	*            Parsed string.
	* @param strsize
	*            String size.
	* @param value
	*            Integer value.
	* @param isSigned
	*            Is value signed number.
	* @return Length of the string or -1 if error has occurred.
	*/
	int hlp_int64ToString(
		char *str, 
		int bufsize, 
		long long value, 
		unsigned char isSigned);

	/**
	* Convert string to integer64.
	*
	* @param str
	*            Parsed string.
	* @return Value of string as integer.
	*/
	long long hlp_stringToInt64(
		const char *str);

	/**
	* Random generator using Linear-feedback shift register.
	* https://en.wikipedia.org/wiki/Linear-feedback_shift_register
	*
	* @return Random number.
	*/
	unsigned char hlp_rand(void);

#ifdef  __cplusplus
}
#endif

#endif //GXHELPERS_H
