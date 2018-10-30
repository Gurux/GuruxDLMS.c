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

#include "../include/connection.h"
#include <stdlib.h> // malloc and free needs this or error is generated.
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)//Windows includes
#else
#include <string.h> // string function definitions
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h> // File control definitions
#include <errno.h> // Error number definitions
#include <termios.h> // POSIX terminal control definitions
#include <time.h>   // time calls
#endif

//Initialize connection buffers.
void con_initializeBuffers(connection* connection, int size)
{
    if (size == 0)
    {
        bb_clear(&connection->data);
    }
    else
    {
        bb_capacity(&connection->data, size);
    }
}

//Initialize connection settings.
void con_init(connection* con, GX_TRACE_LEVEL trace)
{
    con->trace = trace;
    //Reply wait time is 5 seconds.
    con->waitTime = 5000;
#if defined(_WIN32) || defined(_WIN64)//If Windows
    con->comPort = INVALID_HANDLE_VALUE;
    memset(&con->osReader, 0, sizeof(OVERLAPPED));
    memset(&con->osWrite, 0, sizeof(OVERLAPPED));
    con->osReader.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
    con->osWrite.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
#else
    con->comPort = -1;
#endif
    con->socket = -1;
    con->receiverThread = -1;
    con->closing = 0;
    bb_init(&con->data);
    bb_capacity(&con->data, 500);
}

//Close connection..
void con_close(connection* con)
{
#if defined(_WIN32) || defined(_WIN64)//If Windows
    con->comPort = INVALID_HANDLE_VALUE;
#else
    if (con->comPort != -1)
    {
        int ret = close(con->comPort);
        if (ret < 0)
        {
            printf("Failed to close port.\r\n");
        }
        con->comPort = -1;
    }
#endif
    con->socket = -1;
    bb_clear(&con->data);
    con->closing = 0;
    con_initializeBuffers(con, 0);
}
