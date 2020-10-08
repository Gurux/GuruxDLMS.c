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
#include "../dlms/include/client.h"
#include "connection.h"

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Winsock.h> //Add support for sockets
#endif

//Make connection using TCP/IP connection.
int com_makeConnect(
    clientConnection* connection,
    const char* address,
    int port);

//Make connection serial port connection.
int com_open(
    clientConnection* connection,
    const char* port,
    unsigned char iec);

//Start server.
int com_startServer(
    clientConnection* connection,
    unsigned char trace,
    int port);

// Read DLMS Data frame from the device.
int readDLMSPacket(
    clientConnection* connection,
    gxByteBuffer* data,
    gxReplyData* reply);

int com_readDataBlock(
    clientConnection* connection,
    message* messages,
    gxReplyData* reply);

//Read next block.
int com_readNextBlock(
    clientConnection* connection,
    gxReplyData* reply);


//Close connection to the meter.
int com_close(
    clientConnection* connection);

//Initialize connection to the meter.
int com_initializeConnection(
    clientConnection* connection);

/*
//Get Association view.
int com_getAssociationView(
    clientConnection *connection);
    */
    //Read object.
int com_read(
    clientConnection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    gxReplyData* reply);

//Write object.
int com_write(
    clientConnection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* value);

int com_method(
    clientConnection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params);

/**
* Read objects.
*/
int com_readList(
    clientConnection* connection,
    gxArray* list);

int com_readRowsByEntry(
    clientConnection* connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count,
    gxReplyData* reply);

int com_readRowsByRange(
    clientConnection* connection,
    gxProfileGeneric* object,
    unsigned long start,
    unsigned long end,
    gxReplyData* reply);

#ifdef  __cplusplus
}
#endif

#endif
