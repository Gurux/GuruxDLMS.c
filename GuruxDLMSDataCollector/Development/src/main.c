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
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include "../include/getopt.h"
#include <tchar.h>
#include <conio.h>
#include <Winsock.h> //Add support for sockets
#include <time.h>
#include <process.h>//Add support for threads
#else //Linux includes.
#define closesocket close
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h> //Add support for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#endif

#include "../include/exampleserver.h"
#include "../include/connection.h"
#include "../dlms/include/cosem.h"
#include "../dlms/include/gxaes.h"

//DLMS settings.
dlmsServerSettings settings;
unsigned char TMP[HDLC_HEADER_SIZE + HDLC_SIZE];

//Space for client password.
unsigned char PASSWORD[20];
//Space for client challenge.
unsigned char C2S_CHALLENGE[64];
//Space for server challenge.
unsigned char S2C_CHALLENGE[64];

gxValueEventArg events[10] = { 0 };

gxByteBuffer reply;

void showHelp()
{
    printf("Gurux DLMS example Server implements DLMS/COSEM data collector.\r\n");
    printf(" -t [Error, Warning, Info, Verbose] Trace messages.\r\n");
    printf(" -S \t serial port.\r\n");
}

#if defined(_WIN32) || defined(_WIN64)//Windows includes
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    int opt, port = 4061, trace = 0;
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 1;
    }
#endif
    char* serialPort = NULL;
    while ((opt = getopt(argc, argv, "t:p:S:")) != -1)
    {
        switch (opt)
        {
        case 't':
            //Trace.
            if (strcmp("Error", optarg) == 0)
                trace = GX_TRACE_LEVEL_ERROR;
            else  if (strcmp("Warning", optarg) == 0)
                trace = GX_TRACE_LEVEL_WARNING;
            else  if (strcmp("Info", optarg) == 0)
                trace = GX_TRACE_LEVEL_INFO;
            else  if (strcmp("Verbose", optarg) == 0)
                trace = GX_TRACE_LEVEL_VERBOSE;
            else  if (strcmp("Off", optarg) == 0)
                trace = GX_TRACE_LEVEL_OFF;
            else
            {
                printf("Invalid trace option '%s'. (Error, Warning, Info, Verbose, Off)", optarg);
                return 1;
            }
            break;
        case 'p':
            //Port.
            port = atoi(optarg);
            break;
        case 'S':
            serialPort = optarg;
            break;
        case '?':
        {
            if (optarg[0] == 'p')
            {
                printf("Missing mandatory port option.\n");
            }
            else if (optarg[0] == 't')
            {
                printf("Missing mandatory trace option.\n");
            }
            else
            {
                showHelp();
                return 1;
            }
        }
        break;
        default:
            showHelp();
            return 1;
        }
    }
    if (serialPort == NULL)
    {
        printf("Serial port is missing.\n");
        return 1;
    }
    unsigned char data;
    struct sockaddr_in client;
    int ret, ls, s;
    struct sockaddr_in add = { 0 };
#if defined(_WIN32) || defined(_WIN64)//If Windows
    int len;
#else //If Linux
    socklen_t len;
#endif
    bb_attach(&reply, SERVER_REPLY_PDU, 0, sizeof(SERVER_REPLY_PDU));
    //Start server using logical name referencing and HDLC framing.
#ifdef USE_HDLC
    svr_init(&settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_SIZE, PDU_BUFFER_SIZE, SERVER_FRAME, sizeof(SERVER_FRAME), SERVER_PDU, sizeof(SERVER_PDU));
#else
    svr_init(&settings, 1, DLMS_INTERFACE_TYPE_WRAPPER, HDLC_SIZE, PDU_BUFFER_SIZE, SERVER_FRAME, sizeof(SERVER_FRAME), SERVER_PDU, sizeof(SERVER_PDU));
#endif //USE_HDLC
    //Allow non-ciphered pre-established connections.
    settings.info.preEstablished = 1;
    //Readlist and write list are not supported.
    settings.base.proposedConformance &= ~DLMS_CONFORMANCE_MULTIPLE_REFERENCES;
    //Allocate space for client password.
    BB_ATTACH(settings.base.password, PASSWORD, 0);
    //Allocate space for client challenge.
    BB_ATTACH(settings.base.ctoSChallenge, C2S_CHALLENGE, 0);
    //Allocate space for server challenge.
    BB_ATTACH(settings.base.stoCChallenge, S2C_CHALLENGE, 0);

    //Allocate space for read list.
    vec_attach(&settings.transaction.targets, events, 0, sizeof(events) / sizeof(events[0]));

    //Add COSEM objects.
    if ((ret = svr_InitObjects(&settings, serialPort, port)) != 0)
    {
        //TODO: Show error.
        return ret;
    }
    //Start server
    if ((ret = svr_initialize(&settings)) != 0)
    {
        //TODO: Show error.
        return ret;
    }
#ifdef USE_HDLC
    printf("Logical Name DLMS Server using HDLC in port %d.\n", port);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d\n", port);
#else
    printf("Logical Name DLMS Server using WRAPPER in port %d.\n", port);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d -w\n", port);
#endif //USE_HDLC

    ls = socket(AF_INET, SOCK_STREAM, 0);
    add.sin_port = htons(port);
    add.sin_addr.s_addr = htonl(INADDR_ANY);
    add.sin_family = AF_INET;
    if ((ret = bind(ls, (struct sockaddr*) & add, sizeof(add))) == -1)
    {
        return -1;
    }

    while (1)
    {
        if ((ret = listen(ls, 1)) == -1)
        {
            //socket listen failed.
            return -1;
        }
        len = sizeof(client);
        s = accept(ls, (struct sockaddr*) & client, &len);
        while (1)
        {
            //Update push socket.
            pushSocket = s;
            //Read one char at the time.
            if ((ret = recv(s, (char*)& data, 1, 0)) == -1)
            {
                pushSocket = -1;
#if defined(_WIN32) || defined(_WIN64)//If Windows
                closesocket(s);
                s = INVALID_SOCKET;
#else //If Linux
                close(s);
                s = -1;
#endif
                break;
            }
            //If client closes the connection.
            if (ret == 0)
            {
                pushSocket = -1;
#if defined(_WIN32) || defined(_WIN64)//If Windows
                closesocket(s);
                s = INVALID_SOCKET;
#else //If Linux
                close(s);
                s = -1;
#endif
                break;
            }
            if (svr_handleRequest3(&settings, data, &reply) != 0)
            {
#if defined(_WIN32) || defined(_WIN64)//If Windows
                closesocket(s);
                s = INVALID_SOCKET;
#else //If Linux
                close(s);
                s = -1;
#endif
                break;
            }
            if (reply.size != 0)
            {
                if (send(s, (const char*)reply.data, reply.size - reply.position, 0) == -1)
                {
#if defined(_WIN32) || defined(_WIN64)//If Windows
                    closesocket(s);
                    s = INVALID_SOCKET;
#else //If Linux
                    close(s);
                    s = -1;
#endif
                    break;
                }
                printf("TX: ");
                bb_toHexString2(&reply, TMP, sizeof(TMP));
                printf(TMP);
                printf("\r");
                bb_clear(&reply);
            }
        }
    }

#if defined(_WIN32) || defined(_WIN64)//Windows
    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif
    return 0;
}

