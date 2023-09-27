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
#include "../../development/include/client.h"
#include "../../development/include/cosem.h"
#include "connection.h"

//Make connection using TCP/IP connection.
int com_makeConnect(
    connection *connection,
    const char* address,
    int port,
    int waitTime);

//Make connection serial port connection.
int com_open(
    connection *connection,
    const char* port);

//Start server.
int com_startServer(
    connection *connection,
    unsigned char trace,
    int port);

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
int com_disconnect(
    connection* connection);

//Close connection to the meter and close the communcation channel.
int com_close(
    connection *connection);

//Initialize optical head.
int com_initializeOpticalHead(
    connection* connection);


//Read Invocation counter (frame counter) from the meter and update it.
int com_updateInvocationCounter(
    connection* connection,
    const char* invocationCounter);

//Initialize connection to the meter.
int com_initializeConnection(
    connection *connection);

//Get Association view.
int com_getAssociationView(
    connection *connection,
    const char* outputFile);

//Read object.
int com_read(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal);

//Send keepalive message.
int com_getKeepAlive(
    connection* connection);

//Write object.
int com_write(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal);

/**
* write multiple objects with one message.
*/
int com_writeList(
    connection* connection,
    gxArray* list);

int com_method(
    connection *connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params);

int com_method2(
    connection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    unsigned char* value,
    uint32_t length);

int com_method3(
    connection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    gxByteBuffer* value);

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

int com_readRowsByEntry2(
    connection *connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count,
    unsigned short colStartIndex,
    unsigned short colEndIndex);

int com_readRowsByRange(
    connection *connection,
    gxProfileGeneric* object,
    struct tm* start,
    struct tm* end);

int com_readRowsByRange2(
    connection* connection,
    gxProfileGeneric* object,
    gxtime* start,
    gxtime* end);

int com_readValue(
    connection *connection,
    gxObject* object,
    unsigned char index);

//This function reads ALL objects that meter have. It will loop all object's attributes.
int com_readAllObjects(
    connection *connection,
    const char* outputFile);


// Update high level password.
int com_updateHighLevelPassword(
    connection* connection,
    gxAssociationLogicalName* object);

#ifdef  __cplusplus
}
#endif

#endif
