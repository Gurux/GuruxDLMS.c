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

#ifndef CONNECTION_H
#define CONNECTION_H

#include "../../development/include/bytebuffer.h"
#include "../../development/include/dlmssettings.h"

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Windows.h> //Add support for serial port functions.
#else //If Linux
#if defined(__linux__)
#include <pthread.h>
#endif//Linux
#endif

static const unsigned int RECEIVE_BUFFER_SIZE = 200;

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct
{
    //Is trace used.
    unsigned char trace;
#if defined(_WIN32) || defined(_WIN64)//If Windows
    //Socked handle.
    SOCKET socket;
    //Serial port handle.
    HANDLE comPort;
    OVERLAPPED		osWrite;
    OVERLAPPED		osReader;

    //Receiver thread handle.
    HANDLE receiverThread;
#else //If Linux.
    //Socked handle.
    int socket;
    //Serial port handle.
    int comPort;
    //Receiver thread handle.
    pthread_t receiverThread;
#endif
    unsigned long   waitTime;
    //Received data.
    gxByteBuffer data;
    //If receiver thread is closing.
    unsigned char closing;
    dlmsServerSettings settings;
} connection;

void con_initializeBuffers(
    connection* connection,
    int size);

int svr_listen(
    connection* con,
    unsigned short port);

//Close connection..
int con_close(
    connection* con);

#ifdef  __cplusplus
}
#endif

#endif //CONNECTION_H
