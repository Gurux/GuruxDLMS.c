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
#include "../../development/include/gxobjects.h"

#include <stdlib.h> // malloc and free needs this or error is generated.
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)//Windows includes
#if _MSC_VER > 1400
#include <crtdbg.h>
#endif
#include <tchar.h>
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

/**
* Close socket.
*/
int closeServer(int* s)
{
    if (*s != -1)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        closesocket(*s);
#else
        close(*s);
#endif
        *s = -1;
    }
    return 0;
}

/**
* Connect to Push listener.
*/
int connectServer(const char* address, int port, int* s)
{
    int ret;
    struct sockaddr_in add;
    //create socket.
    *s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (*s == -1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    add.sin_port = htons(port);
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = inet_addr(address);
    //If address is give as name
    if(add.sin_addr.s_addr == INADDR_NONE)
    {
        struct hostent *Hostent = gethostbyname(address);
        if (Hostent == NULL)
        {
#if defined(_WIN32) || defined(_WIN64)//If Windows
            int err = WSAGetLastError();
#else
            int err = errno;
#endif
            closeServer(s);
            return err;
        };
        add.sin_addr = *(struct in_addr*)(void*)Hostent->h_addr_list[0];
    };

    //Connect to the meter.
    ret = connect(*s, (struct sockaddr*)&add, sizeof(struct sockaddr_in));
    if (ret == -1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    };
    return DLMS_ERROR_CODE_OK;
}

int test()
{
    int key = 0, pos, ret = 0, socket = -1;
    int port = 4059;
    dlmsSettings cl;
    gxPushSetup p;
    gxClock clock;
    connection con;
    message messages;
    gxByteBuffer *bb;
    mes_init(&messages);
    memset(&p, 0, sizeof(gxPushSetup));
    p.base.objectType = DLMS_OBJECT_TYPE_PUSH_SETUP;
    hlp_setLogicalName(p.base.logicalName, "0.7.25.9.0.255");
    memset(&clock, 0, sizeof(gxClock));
    clock.base.objectType = DLMS_OBJECT_TYPE_CLOCK;
    hlp_setLogicalName(clock.base.logicalName, "0.0.1.0.0.255");
    cl_init(&cl, 1, 1, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_WRAPPER);
    svr_init(&con.settings, 1, DLMS_INTERFACE_TYPE_WRAPPER);
    arr_push(&p.pushObjectList, key_init(&p, co_init(2, 0)));
    arr_push(&p.pushObjectList, key_init(&clock, co_init(2, 0)));

    ///////////////////////////////////////////////////////////////////////
    //Create Gurux DLMS server component for Short Name and start listen events.
    ret = svr_listen(&con, port);
    if (ret != 0)
    {
        return ret;
    }
    printf("Listening DLMS Push IEC 62056-47 messages on port %d.\r\n", port);
    printf("Press X to close and Enter to send a Push message.\r\n");
    while ((key = getchar()) != 'X' && key != 'x')
    {
        if (key == 10)
        {
            printf("Sending Push message.");
            if ((ret = connectServer("localhost", port, &socket)) != 0)
            {
                break;
            }
            time_now(&clock.time);
            if ((ret = notify_generatePushSetupMessages(&cl, NULL, &p, &messages)) != 0)
            {
                mes_clear(&messages);
                break;
            }
            for (pos = 0; pos != messages.size; ++pos)
            {
                bb = messages.data[pos];
                if ((ret = send(socket, bb->data, bb->size, 0)) == -1)
                {
                    mes_clear(&messages);
                    break;
                }
            }
            mes_clear(&messages);
            closeServer(&socket);
        }
    }
    mes_clear(&messages);
    obj_clear(&p.base);
    cl_clear(&cl);
    svr_clear(&con.settings);
    return con_close(&con);
}

#if defined(_WIN32) || defined(_WIN64)//Windows includes
int _tmain(int argc, _TCHAR* argv[])
#else
int main( int argc, char* argv[] )
#endif
{
#if defined(_WIN32) || defined(_WIN64)//Windows includes
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 1;
    }
#endif
    test();
#if defined(_WIN32) || defined(_WIN64)//Windows
    WSACleanup();
#if _MSC_VER > 1400
    _CrtDumpMemoryLeaks();
#endif
#endif

    return 0;
}

