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
#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    connection snHdlc, lnHdlc, snWrapper, lnWrapper;
#else
    connection lnHdlc;
#endif //!defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)

#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    //Initialize DLMS settings.
    svr_init(&snHdlc.settings, 0, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, snframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, snpduBuff, PDU_BUFFER_SIZE);
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
#endif //!defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    //Initialize DLMS settings.
    svr_init(&lnHdlc.settings, 1, DLMS_INTERFACE_TYPE_HDLC, HDLC_BUFFER_SIZE, PDU_BUFFER_SIZE, lnframeBuff, HDLC_HEADER_SIZE + HDLC_BUFFER_SIZE, lnpduBuff, PDU_BUFFER_SIZE);
    //We have several server that are using same objects. Just copy them.
#if defined(DLMS_INDIAN_STANDARD) || defined(DLMS_ITALIAN_STANDARD)
    svr_InitObjects(&lnHdlc.settings);
#else
    oa_copy(&lnHdlc.settings.base.objects, &snHdlc.settings.base.objects);
#endif
    //Start server
    if ((ret = svr_start(&lnHdlc, port + 1)) != 0)
    {
        return ret;
    }
    printf("Logical Name DLMS Server in port %d.\n", port + 1);
    printf("Example connection settings:\n");
    printf("GuruxDLMSClientExample -h localhost -p %d\n", port + 1);
    printf("----------------------------------------------------------\n");
#if !defined(DLMS_INDIAN_STANDARD) && !defined(DLMS_ITALIAN_STANDARD)
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
    printf("GuruxDLMSClientExample -r sn -h localhost -p %d -w\n", port + 2);
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
    printf("GuruxDLMSClientExample -h localhost -p %d -w\n", port + 3);
    printf("Press Enter to close application.\r\n");
#endif //!defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    getchar();
#if !defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    con_close(&snHdlc);
    con_close(&snWrapper);
    con_close(&lnWrapper);
#endif //!defined(DLMS_INDIAN_STANDARD) &&  !defined(DLMS_ITALIAN_STANDARD)
    con_close(&lnHdlc);
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
    startServers(port, trace);
#if defined(_WIN32) || defined(_WIN64)//Windows

    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif

    return 0;
}

