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
#include <Winsock2.h> //Add support for sockets
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


#define HDLC_HEADER_SIZE 17

#define HDLC_BUFFER_SIZE 128
#define PDU_BUFFER_SIZE 1024
#define WRAPPER_BUFFER_SIZE 8 + PDU_BUFFER_SIZE
unsigned char snframeBuff[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
unsigned char snpduBuff[PDU_BUFFER_SIZE];
unsigned char lnframeBuff[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE];
unsigned char lnpduBuff[PDU_BUFFER_SIZE];
unsigned char sn47frameBuff[WRAPPER_BUFFER_SIZE];
unsigned char sn47pduBuff[PDU_BUFFER_SIZE];
unsigned char ln47frameBuff[WRAPPER_BUFFER_SIZE];
unsigned char ln47pduBuff[PDU_BUFFER_SIZE];

int startServers(int port, int trace)
{
    int ret;
    connection snHdlc, lnHdlc, snWrapper, lnWrapper, lniec;
    //Initialize DLMS settings.
    svr_init(&snHdlc.settings, 0, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, snframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, snpduBuff, PDU_BUFFER_SIZE);
    //Set master key (KEK) to 1111111111111111.
    unsigned char KEK[16] = { 0x31,0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31 };
    BB_ATTACH(snHdlc.settings.base.kek, KEK, sizeof(KEK));
    svr_InitObjects(&snHdlc.settings);
    //Start server
    if ((ret = svr_start(&snHdlc, port)) != 0)
    {
        return ret;
    }
    printf("Short Name DLMS Server in port %d.\n", port);
    printf("Example connection settings:\n");
    printf("Gurux.DLMS.Client.Example.Net -r sn -h localhost -p %d\n", port);
    printf("----------------------------------------------------------\n");
    //Initialize DLMS settings.
    svr_init(&lnHdlc.settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, lnframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, lnpduBuff, PDU_BUFFER_SIZE);
    //We have several server that are using same objects. Just copy them.
    oa_copy(&lnHdlc.settings.base.objects, &snHdlc.settings.base.objects);
    //Start server
    if ((ret = svr_start(&lnHdlc, port + 1)) != 0)
    {
        return ret;
    }
    printf("Logical Name DLMS Server in port %d.\n", port + 1);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d\n", port + 1);
    printf("----------------------------------------------------------\n");
    //Initialize DLMS settings.
    svr_init(&snWrapper.settings, 0, DLMS_INTERFACE_TYPE_WRAPPER, WRAPPER_BUFFER_SIZE, PDU_BUFFER_SIZE, sn47frameBuff, WRAPPER_BUFFER_SIZE, sn47pduBuff, PDU_BUFFER_SIZE);
    //We have several server that are using same objects. Just copy them.
    oa_copy(&snWrapper.settings.base.objects, &snHdlc.settings.base.objects);
    //Start server
    if ((ret = svr_start(&snWrapper, port + 2)) != 0)
    {
        return ret;
    }
    printf("Short Name DLMS Server with IEC 62056-47 in port %d.\n", port + 2);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -r sn -h localhost -p %d -i WRAPPER\n", port + 2);
    printf("----------------------------------------------------------\n");
    //Initialize DLMS settings.
    svr_init(&lnWrapper.settings, 1, DLMS_INTERFACE_TYPE_WRAPPER, WRAPPER_BUFFER_SIZE, PDU_BUFFER_SIZE, ln47frameBuff, WRAPPER_BUFFER_SIZE, ln47pduBuff, PDU_BUFFER_SIZE);
    //We have several server that are using same objects. Just copy them.
    oa_copy(&lnWrapper.settings.base.objects, &snHdlc.settings.base.objects);
    //Start server
    if ((ret = svr_start(&lnWrapper, port + 3)) != 0)
    {
        return ret;
    }
    snHdlc.trace = lnHdlc.trace = snWrapper.trace = lnWrapper.trace = trace;
    printf("Logical Name DLMS Server with IEC 62056-47 in port %d.\n", port + 3);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d -i WRAPPER\n", port + 3);

    //Initialize DLMS settings.
    svr_init(&lniec.settings, 1, DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, lnframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, lnpduBuff, PDU_BUFFER_SIZE);
    //Set flag id.
    memcpy(lniec.settings.flagId, "GRX", 3);
    //We have several server that are using same objects. Just copy them.
    oa_copy(&lniec.settings.base.objects, &snHdlc.settings.base.objects);
    //Start server
    if ((ret = svr_start(&lniec, port + 4)) != 0)
    {
        return ret;
    }
    printf("Hdlc With Mode E Logical Name DLMS Server in port %d.\n", port + 4);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d -i HdlcWithModeE\n", port + 4);
    printf("----------------------------------------------------------\n");

    printf("----------------------------------------------------------\n");
    printf("Authentication levels:\n");
    printf("None: Client address 16 (0x10)\n");
    printf("Low: Client address 17 (0x11)\n");
    printf("High GMac: Client address 1 (0x1)\n");
    printf("High Pre-established: Client address 2 (0x2)\n");
    printf("High ECDSA: Client address 3 (0x3)\n");
    printf("----------------------------------------------------------\n");
    println("System Title", &snHdlc.settings.base.cipher.systemTitle);
    println("Authentication key", &snHdlc.settings.base.cipher.authenticationKey);
    println("Block cipher key", &snHdlc.settings.base.cipher.blockCipherKey);
    println("Client System title", snHdlc.settings.base.preEstablishedSystemTitle);
    println("Master key (KEK)", &snHdlc.settings.base.kek);
    printf("----------------------------------------------------------\n");
    printf("Press Enter to close application.\r\n");
    uint32_t lastMonitor = 0;
    while (1)
    {
        //Monitor values only once/second.
        if (time_current() - lastMonitor > 1)
        {
            lastMonitor = time_current();
            if ((ret = svr_limiterAll(&snHdlc.settings, lastMonitor)) != 0)
            {
                printf("snHdlc limiter failed.\r\n");
            }
            if ((ret = svr_limiterAll(&lnHdlc.settings, lastMonitor)) != 0)
            {
                printf("lnHdlc limiter failed.\r\n");
            }
            if ((ret = svr_limiterAll(&snWrapper.settings, lastMonitor)) != 0)
            {
                printf("snWrapper limiter failed.\r\n");
            }
            if ((ret = svr_limiterAll(&lnWrapper.settings, lastMonitor)) != 0)
            {
                printf("lnWrapper limiter failed.\r\n");
            }
            if ((ret = svr_monitorAll(&snHdlc.settings)) != 0)
            {
                printf("snHdlc monitor failed.\r\n");
            }
            if ((ret = svr_monitorAll(&lnHdlc.settings)) != 0)
            {
                printf("lnHdlc monitor failed.\r\n");
            }
            if ((ret = svr_monitorAll(&snWrapper.settings)) != 0)
            {
                printf("snWrapper monitor failed.\r\n");
            }
            if ((ret = svr_monitorAll(&lnWrapper.settings)) != 0)
            {
                printf("lnWrapper monitor failed.\r\n");
            }
        }
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        if (_kbhit()) {
            char c = _getch();
            if (c == '\r')
            {
                printf("Closing the server.\n");
                break;
            }
        }
#else
        char ch = _getch();
        if (ch == '\n')
        {
            printf("Closing the server.\n");
            break;
        }
#endif
    }
    con_close(&snHdlc);
    con_close(&snWrapper);
    con_close(&lnWrapper);
    return 0;
}

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
    int opt, port = 4060, trace = 0;
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
    char* p = strrchr(DATAFILE, '\\');
    *p = '\0';
    strcpy(IMAGEFILE, DATAFILE);
    strcpy(TRACEFILE, DATAFILE);
    //Add empty file name. This is removed when data is updated.
    strcat(IMAGEFILE, "\\empty.bin");
    strcat(DATAFILE, "\\data.csv");
    strcat(TRACEFILE, "\\trace.txt");
#else
    char* p = strrchr(DATAFILE, '/');
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
    FILE* f = NULL;
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
    startServers(port, trace);
#if defined(_WIN32) || defined(_WIN64)//Windows

    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif

    return 0;
}

