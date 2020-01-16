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
#include "../../development/include/cosem.h"
#include "../../development/include/gxaes.h"

//DLMS settings.
dlmsServerSettings settings;

#define HDLC_HEADER_SIZE 17
#define WRAPPER_BUFFER_SIZE 8 + PDU_BUFFER_SIZE

#define HDLC_BUFFER_SIZE 128
#define PDU_BUFFER_SIZE 1024

unsigned char pdu[PDU_BUFFER_SIZE];
unsigned char frame[11 + PDU_BUFFER_SIZE];
unsigned char replyFrame[11 + HDLC_BUFFER_SIZE];
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
    printf("Gurux DLMS example Server implements four DLMS/COSEM devices.\r\n");
    printf(" -t [Error, Warning, Info, Verbose] Trace messages.\r\n");
    printf(" -p Start port number. Default is 4060.\r\n");
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

    strcpy(DATAFILE, argv[0]);
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    char *p = strrchr(DATAFILE, '\\');
    *p = '\0';
    strcpy(IMAGEFILE, DATAFILE);
    strcpy(TRACEFILE, DATAFILE);
    //Add empty file name. This is removed when data is updated.
    strcat(IMAGEFILE, "\\empty.bin");
    strcat(DATAFILE, "\\data.csv");
    strcat(TRACEFILE, "\\trace.txt");
#else
    char *p = strrchr(DATAFILE, '/');
    *p = '\0';
    strcpy(IMAGEFILE, DATAFILE);
    strcpy(TRACEFILE, DATAFILE);
    //Add empty file name.
    strcat(IMAGEFILE, "/empty.bin");
    strcat(DATAFILE, "/data.csv");
    strcat(TRACEFILE, "/trace.txt");
#endif

    // Clear trace file
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#if _MSC_VER > 1400
    FILE * f = NULL;
    fopen_s(&f, TRACEFILE, "w");
#else
    FILE* f = fopen(TRACEFILE, "w");
#endif
    fclose(f);
#endif //defined(_WIN32) || defined(_WIN64) || defined(__linux__)

    while ((opt = getopt(argc, argv, "t:p:")) != -1)
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
    unsigned char data;
    struct sockaddr_in client;
    int ret, ls, s;
    struct sockaddr_in add = { 0 };
#if defined(_WIN32) || defined(_WIN64)//If Windows
    int len;
    int AddrLen = sizeof(add);
#else //If Linux
    socklen_t len;
    socklen_t AddrLen = sizeof(add);
#endif
    bb_attach(&reply, replyFrame, 0, sizeof(replyFrame));
    //Start server using logical name referencing and HDLC framing.
    svr_init(&settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, frame, sizeof(frame), pdu, sizeof(pdu));
    //Allocate space for client password.
    BB_ATTACH(settings.base.password, PASSWORD, 0);
    //Allocate space for client challenge.
    BB_ATTACH(settings.base.ctoSChallenge, C2S_CHALLENGE, 0);
    //Allocate space for server challenge.
    BB_ATTACH(settings.base.stoCChallenge, S2C_CHALLENGE, 0);
    //Set master key (KEK) to 1111111111111111.
    unsigned char KEK[16] = {0x31,0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31};
    memcpy(settings.base.kek, KEK, sizeof(KEK));
    //Allocate space for read list.
    vec_attach(&settings.transaction.targets, events, 0, sizeof(events) / sizeof(events[0]));

    //Add COSEM objects.
    svr_InitObjects(&settings);
    //Start server
    if ((ret = svr_initialize(&settings)) != 0)
    {
        //TODO: Show error.
        return ret;
    }
    char buff[512];
    printf("Logical Name DLMS Server in port %d.\n", port);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d\n", port);
    printf("----------------------------------------------------------\n");
    println("System Title", settings.base.cipher.systemTitle, sizeof(settings.base.cipher.systemTitle));
    println("Authentication key", settings.base.cipher.authenticationKey, sizeof(settings.base.cipher.authenticationKey));
    println("Block cipher key", settings.base.cipher.blockCipherKey, sizeof(settings.base.cipher.blockCipherKey));
    println("Client System title", settings.base.preEstablishedSystemTitle, sizeof(settings.base.preEstablishedSystemTitle));
    println("Master key (KEK)", settings.base.kek, sizeof(settings.base.kek));
    printf("----------------------------------------------------------\n");
    printf("Press Enter to close application.\r\n");
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
            //Read one char at the time.
            if ((ret = recv(s, (char*)& data, 1, 0)) == -1)
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
            //If client closes the connection.
            if (ret == 0)
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
                bb_toHexString2(&reply, buff, sizeof(buff));
                printf(buff);
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

