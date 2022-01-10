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
#include "../../development/include/notify.h"

#include <stdlib.h> // malloc and free needs this or error is generated.
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Winsock.h> //Add support for sockets
#include <process.h>//Add support for threads
#else //Linux
#define INVALID_HANDLE_VALUE -1
#include <unistd.h>
#include <stdlib.h>
#include <errno.h> //Add support for sockets
#include <netdb.h> //Add support for sockets
#include <sys/types.h> //Add support for sockets
#include <sys/socket.h> //Add support for sockets
#include <netinet/in.h> //Add support for sockets
#include <arpa/inet.h> //Add support for sockets
#include <termios.h> // POSIX terminal control definitions
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> // string function definitions
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h> // File control definitions
#include <errno.h> // Error number definitions
#endif

unsigned char svr_isTarget(
    dlmsSettings* settings,
    uint32_t serverAddress,
    uint32_t clientAddress)
{
    //Empty skeleton. This is added because server implementation needs this.
    return 0;
}

void time_now(
    gxtime* value, unsigned char meterTime)
{
    time_initUnix(value, time(NULL));
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    //Empty skeleton. This is added because server implementation needs this.
    return DLMS_ACCESS_MODE_NONE;
}

/**
* Get method access level.
*/
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    //Empty skeleton. This is added because server implementation needs this.
    return DLMS_METHOD_ACCESS_MODE_NONE;
}

/////////////////////////////////////////////////////////////////////////////
//Client has made connection to the server.
/////////////////////////////////////////////////////////////////////////////
int svr_connected(
    dlmsServerSettings* settings)
{
    //Empty skeleton. This is added because server implementation needs this.
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_preRead(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
void svr_postRead(
    dlmsSettings* settings,
    gxValueEventCollection* args)
{
}

//Initialize connection buffers.
void con_initializeBuffers(connection* connection, int size)
{
    if (size == 0)
    {
        bb_clear(&connection->data);
    }
    else
    {
        //Allocate 50 bytes more because some meters count this wrong and send few bytes too many.
        bb_capacity(&connection->data, 50 + size);
    }
}

unsigned char isConnected(connection* con)
{
#if defined(_WIN32) || defined(_WIN64)//If Windows
    return con->socket != INVALID_SOCKET;
#else //If Linux
    return con->socket != -1;
#endif
}

void ListenerThread(void* pVoid)
{
    int socket;
    connection* con = (connection*)pVoid;
    struct sockaddr_in add;
    int ret;
    char tmp[10];
#if defined(_WIN32) || defined(_WIN64)//If Windows
    int len;
    int AddrLen = sizeof(add);
#else //If Linux
    socklen_t len;
    socklen_t AddrLen = sizeof(add);
#endif
    char* info;
    int pos;
    gxKey* it;
    char* buff = NULL;
    char ln[25];
    gxByteBuffer bb, senderInfo;
    gxReplyData data;
    struct sockaddr_in client;
    gxArray list;
    arr_init(&list);
    //Get buffer data
    bb_init(&senderInfo);
    bb_init(&bb);
    bb_capacity(&bb, 2048);
    memset(&client, 0, sizeof(client));
    reply_init(&data);
    while (isConnected(con))
    {
        len = sizeof(client);
        bb_empty(&senderInfo);
        socket = accept(con->socket, (struct sockaddr*)&client, &len);
        if (isConnected(con))
        {
            if ((ret = getpeername(socket, (struct sockaddr*)&add, &AddrLen)) == -1)
            {
                bb_empty(&bb);
#if defined(_WIN32) || defined(_WIN64)//If Windows
                closesocket(socket);
                socket = INVALID_SOCKET;
#else //If Linux
                close(socket);
                socket = -1;
#endif
                continue;
                //Notify error.
            }
            info = inet_ntoa(add.sin_addr);
            bb_set(&senderInfo, (unsigned char*)info, (unsigned long)strlen(info));
            bb_setInt8(&senderInfo, ':');
#if _MSC_VER > 1000
            _ltoa_s(add.sin_port, tmp, 10, 10);
#else
            sprintf(tmp, "%d", add.sin_port);
#endif
            bb_set(&senderInfo, (unsigned char*)tmp, (unsigned long)strlen(tmp));
            while (isConnected(con))
            {
                //If client is left wait for next client.
                if ((ret = recv(socket, (char*)
                    bb.data + bb.size,
                    bb.capacity - bb.size, 0)) == -1)
                {
                    bb_empty(&bb);
                    //Notify error.
#if defined(_WIN32) || defined(_WIN64)//If Windows
                    closesocket(socket);
                    socket = INVALID_SOCKET;
#else //If Linux
                    close(socket);
                    socket = -1;
#endif
                    break;
                }
                //If client is closed the connection.
                if (ret == 0)
                {
                    bb_empty(&bb);
#if defined(_WIN32) || defined(_WIN64)//If Windows
                    closesocket(socket);
                    socket = INVALID_SOCKET;
#else //If Linux
                    close(socket);
                    socket = -1;
#endif
                    break;
                }
                bb.size = bb.size + ret;
                if (notify_getData(&con->settings.base, &bb, &data) != 0)
                {
                    bb_empty(&bb);
#if defined(_WIN32) || defined(_WIN64)//If Windows
                    closesocket(socket);
                    socket = INVALID_SOCKET;
#else //If Linux
                    close(socket);
                    socket = -1;
#endif
                    break;
                }
                // If all data is received.
                if (data.complete)
                {
                    bb_empty(&bb);
                    if (data.moreData == DLMS_DATA_REQUEST_TYPES_NONE)
                    {
                        gxPushSetup ps;
                        {
                            const unsigned char ln[6] = { 0,7,25,9,0,255 };
                            INIT_OBJECT(ps, DLMS_OBJECT_TYPE_PUSH_SETUP, ln);
                        }
                        gxClock clock;
                        {
                            const unsigned char ln[6] = { 0,0,1,0,0,255 };
                            INIT_OBJECT(clock, DLMS_OBJECT_TYPE_CLOCK, ln);
                        }
                        arr_push(&ps.pushObjectList, key_init(&ps, co_init(2, 0)));
                        arr_push(&ps.pushObjectList, key_init(&clock, co_init(2, 0)));
                        gxArray items;
                        arr_init(&items);
                        if (notify_getPushValues(&con->settings.base, &ps, data.dataValue.Arr, &items) == 0)
                        {
                            gxKey* k;
                            //gxObject* obj;
                            char* str = NULL;
                            for (pos = 0; pos != ps.pushObjectList.size; ++pos)
                            {
                                if ((ret = arr_getByIndex(&ps.pushObjectList, pos, (void**)&k)) != 0 ||
                                    (ret = obj_toString((gxObject*)k->key, &str)) != 0)
                                {
                                    break;
                                }
                                printf("%s\r\n", str);
                                free(str);
                            }
                        }
                        obj_clearPushObjectList(&items);
                        va_toString(data.dataValue.Arr, &bb);
                        bb.data[bb.size] = 0;
                        ++bb.size;
                        printf("%s", bb.data);
                        ret = notify_parsePush(&con->settings.base, data.dataValue.Arr, &list);
                        if (ret != 0)
                        {
                            arr_clear(&list);
                            break;
                        }
                        // Print received data.
                        for (pos = 0; pos != list.size; ++pos)
                        {
                            if ((ret = arr_getByIndex(&list, pos, (void**)&it)) != 0)
                            {
                                break;
                            }

                            // Print LN and value.
                            if ((ret = obj_toString((gxObject*)it->key, &buff)) != 0)
                            {
                                arr_clear(&list);
                                free(buff);
                                buff = NULL;
                                break;
                            }
                            hlp_getLogicalNameToString(((gxObject*)it->key)->logicalName, ln);
                            printf("%s : %s\r\n", ln, buff);
                            free(buff);
                            buff = NULL;
                        }
                        reply_clear(&data);
                        arr_clear(&list);
                    }
                }
            }
        }
    }
    arr_clear(&list);
    bb_clear(&bb);
    reply_clear(&data);
    bb_clear(&senderInfo);
}

#if defined(_WIN32) || defined(_WIN64)//If Windows
#else //If Linux
void* UnixListenerThread(void* pVoid)
{
    ListenerThread(pVoid);
    return NULL;
}
#endif

//Initialize connection settings.
int svr_listen(
    connection* con,
    unsigned short port)
{
    struct sockaddr_in add = { 0 };
    int fFlag = 1;
    int ret;
    //Reply wait time is 5 seconds.
    con->waitTime = 5000;
#if defined(_WIN32) || defined(_WIN64)//If Windows
    con->comPort = INVALID_HANDLE_VALUE;
    memset(&con->osReader, 0, sizeof(OVERLAPPED));
    memset(&con->osWrite, 0, sizeof(OVERLAPPED));
    con->osReader.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
    con->osWrite.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
    con->receiverThread = INVALID_HANDLE_VALUE;
#else
    con->comPort = -1;
    con->receiverThread = -1;
#endif
    con->socket = -1;
    con->closing = 0;
    bb_init(&con->data);
    bb_capacity(&con->data, 50);

    con->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (!isConnected(con))
    {
        //socket creation.
        return -1;
    }
    if (setsockopt(con->socket, SOL_SOCKET, SO_REUSEADDR, (char*)&fFlag, sizeof(fFlag)) == -1)
    {
        //setsockopt.
        return -1;
    }
    add.sin_port = htons(port);
    add.sin_addr.s_addr = htonl(INADDR_ANY);
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    add.sin_family = AF_INET;
#else
    add.sin_family = AF_INET;
#endif
    if ((ret = bind(con->socket, (struct sockaddr*)&add, sizeof(add))) == -1)
    {
        //bind;
        return -1;
    }
    if ((ret = listen(con->socket, 1)) == -1)
    {
        //socket listen failed.
        return -1;
    }
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    con->receiverThread = (HANDLE)_beginthread(ListenerThread, 0, (LPVOID)con);
#else
    ret = pthread_create(&con->receiverThread, NULL, UnixListenerThread, (void*)con);
#endif
    return ret;
}

//Close connection.
int con_close(
    connection* con)
{
    if (isConnected(con))
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        closesocket(con->socket);
        con->socket = INVALID_SOCKET;
        if (con->receiverThread != INVALID_HANDLE_VALUE)
        {
            int ret = WaitForSingleObject(con->receiverThread, 5000);
            con->receiverThread = INVALID_HANDLE_VALUE;
        }
#else
        close(con->socket);
        con->socket = -1;
        void* res;
        pthread_join(con->receiverThread, (void**)&res);
        free(res);
#endif

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
    cl_clear(&con->settings.base);
    return 0;
}

