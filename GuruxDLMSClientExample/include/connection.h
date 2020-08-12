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

#ifndef MEDIA_H
#define MEDIA_H

#include "../../development/include/bytebuffer.h"
#include "../../development/include/dlmssettings.h"

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Winsock2.h> //Add support for sockets
#include <Windows.h> //Add support for serial port functions.
#endif

static const unsigned int RECEIVE_BUFFER_SIZE = 200;

#ifdef  __cplusplus
extern "C" {
#endif

    typedef struct
    {
        //Is trace used.
        GX_TRACE_LEVEL trace;
        //Socked handle.
        int socket;
        //Serial port handle.
#if defined(_WIN32) || defined(_WIN64)// If Windows
        HANDLE comPort;
        OVERLAPPED		osWrite;
        OVERLAPPED		osReader;
#else //If Linux
        int comPort;
#endif
        unsigned long   waitTime;
        //Received data.
        gxByteBuffer data;
        //Receiver thread handle.
        int receiverThread;
        //If receiver thread is closing.
        unsigned char closing;

        dlmsSettings settings;
    } connection;

    void con_initializeBuffers(
        connection* connection,
        int size);

    //Initialize connection settings.
    void con_init(
        connection* con,
        GX_TRACE_LEVEL trace);

    //Close connection..
    void con_close(connection* con);

#ifdef  __cplusplus
}
#endif

#endif //MEDIA_H
