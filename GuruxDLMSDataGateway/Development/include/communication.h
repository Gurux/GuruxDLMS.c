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
#include "../dlms/include/cosem.h"
#include "connection.h"

//Make clientConnection using TCP/IP clientConnection.
int com_makeConnect(
    clientConnection* clientConnection,
    const char* address,
    int port,
    int waitTime);

//Make clientConnection serial port clientConnection.
int com_open(
    clientConnection* clientConnection,
    const char* port);

//Start server.
int com_startServer(
    clientConnection* clientConnection,
    unsigned char trace,
    int port);

// Read DLMS Data frame from the device.
int readDLMSPacket(
    clientConnection* clientConnection,
    gxByteBuffer* data,
    gxReplyData* reply);

int com_readDataBlock(
    clientConnection* clientConnection,
    message* messages,
    gxReplyData* reply);

//Close clientConnection to the meter.
int com_disconnect(
    clientConnection* clientConnection);

//Close clientConnection to the meter and close the communcation channel.
int com_close(
    clientConnection* clientConnection);

//Initialize optical head.
int com_initializeOpticalHead(
    clientConnection* clientConnection);


//Read Invocation counter (frame counter) from the meter and update it.
int com_updateInvocationCounter(
    clientConnection* clientConnection,
    const char* invocationCounter);

//Initialize clientConnection to the meter.
int com_initializeConnection(
    clientConnection* clientConnection);

//Get Association view.
int com_getAssociationView(
    clientConnection* clientConnection,
    const char* outputFile);

//Read object.
int com_read(
    clientConnection* clientConnection,
    gxObject* object,
    unsigned char attributeOrdinal);

//Read object.
int com_read2(
    clientConnection* clientConnection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* value);

//Send keepalive message.
int com_getKeepAlive(
    clientConnection* clientConnection);

//Write object.
int com_write(
    clientConnection* clientConnection,
    gxObject* object,
    unsigned char attributeOrdinal);

int com_method(
    clientConnection* clientConnection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params,
    dlmsVARIANT* value);

#ifdef  __cplusplus
}
#endif

#endif
