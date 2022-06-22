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

#include "../dlms/include/bytebuffer.h"
#include "../dlms/include/dlmssettings.h"

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Windows.h> //Add support for serial port functions.
#else //If Linux
#if defined(__linux__)
#include <pthread.h>
#endif//Linux
#endif

//Define is HDLC or WRAPPER framing used.
//#define USE_HDLC

static const unsigned int RECEIVE_BUFFER_SIZE = 200;

#define PDU_BUFFER_SIZE 5000

#define HDLC_HEADER_SIZE 17
#define WRAPPER_FRAME_SIZE 8 + PDU_BUFFER_SIZE
#define HDLC_SIZE 128
#define WRAPPER_SIZE 1024

#ifdef USE_HDLC
#define FRAME_SIZE HDLC_HEADER_SIZE + HDLC_SIZE
#else
#define FRAME_SIZE HDLC_HEADER_SIZE + WRAPPER_SIZE
#endif //USE_HDLC

unsigned char SERVER_PDU[10 + PDU_BUFFER_SIZE];
unsigned char SERVER_FRAME[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];
unsigned char SERVER_REPLY_PDU[FRAME_SIZE];

unsigned char CLIENT_PDU[PDU_BUFFER_SIZE];
unsigned char CLIENT_REPLY_PDU[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];

//Push needs own buffer so it don't cause problems with on-going communication.
unsigned char PUSH_PDU[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];
unsigned char PUSH_FRAME[HDLC_HEADER_SIZE + PDU_BUFFER_SIZE];

//Push messages are sent using this socket.
int pushSocket;

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
        //If receiver thread is closing.
        unsigned char closing;
        dlmsServerSettings settings;
    } connection;

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
        //Received data is read here from the serial port or TCP/IP socket.
        gxByteBuffer data;
        //Receiver thread handle.
        int receiverThread;
        //If receiver thread is closing.
        unsigned char closing;

        dlmsSettings settings;
    } clientConnection;

    int svr_listen(
        connection* con,
        unsigned short port);

    //Close connection..
    int con_close(
        connection* con);


    void con_initializeBuffers(
        clientConnection* connection,
        int size);

    //Initialize connection settings.
    void conCl_init(
        clientConnection* con,
        GX_TRACE_LEVEL trace);

    //Close connection..
    void conCl_close(clientConnection* con);

#ifdef  __cplusplus
}
#endif

#endif //CONNECTION_H
