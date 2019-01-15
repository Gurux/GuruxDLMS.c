//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL:  $
//
// Version:         $Revision:  $,
//                  $Date:  $
//                  $Author: $
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>
#include <string.h>
#include "include/client.h"
#include "connection.h"

// Read DLMS Data frame from the device.
int readDLMSPacket(
    connection *connection,
    gxByteBuffer* data,
    gxReplyData* reply);

int com_readDataBlock(
    connection *connection,
    message* messages,
    gxReplyData* reply);

//Close connection to the meter.
int com_close(
    connection *connection);

//Initialize connection to the meter.
int com_initializeConnection(
    connection *connection);

//Get Association view.
int com_getAssociationView(
    connection *connection);

//Read object.
int com_read(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal);

//Write object.
int com_write(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal);

int com_method(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params);

/**
* Read objects.
*/
int com_readList(
    connection *connection,
    gxArray* list);

int com_readRowsByEntry(
    connection *connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count);

int com_readRowsByRange(
    connection *connection,
    gxProfileGeneric* object,
    struct tm* start,
    struct tm* end);

int com_readValue(
    connection *connection,
    gxObject* object,
    unsigned char index);

//Read all objects from the meter.
int com_readAllObjects(connection *connection);

#ifdef  __cplusplus
}
#endif

#endif
