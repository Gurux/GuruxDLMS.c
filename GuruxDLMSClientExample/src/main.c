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
#include <stdlib.h>
#include <assert.h>
#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <conio.h>
#include "../include/getopt.h"
#include <process.h>//Add support for threads
#if _MSC_VER > 1000
#include <crtdbg.h>
#endif
//Windows doesn't implement strcasecmp. It uses strcmpi.
#define strcasecmp _strcmpi
#else
#include <string.h> /* memset */
#include <unistd.h>
#include <strings.h>
#endif

#include "../include/communication.h"

//Client don't need this.
unsigned char svr_isTarget(
    dlmsSettings* settings,
    unsigned long serverAddress,
    unsigned long clientAddress)
{
    return 0;
}

//Client don't need this.
int svr_connected(
    dlmsServerSettings* settings) {
    return 0;

}

//Client don't need this.
DLMS_ACCESS_MODE svr_getAttributeAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    return DLMS_ACCESS_MODE_READ_WRITE;
}

//Client don't need this.
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
    dlmsSettings* settings,
    gxObject* obj,
    unsigned char index)
{
    return DLMS_METHOD_ACCESS_MODE_ACCESS;
}

/**
* Updates clock and reads it.
*/
int com_updateClock(connection* connection)
{
    int ret;
    gxClock clock;
    unsigned char ln[] = { 0,0,1,0,0,255 };
    INIT_OBJECT(clock, DLMS_OBJECT_TYPE_CLOCK, ln);
    //Initialize connection.
    ret = com_initializeConnection(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    //Write clock.
    time_init(&clock.time, 2000, 1, 1, 0, 0, 0, 0, 0x8000);
    ret = com_write(connection, &clock.base, 2);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }

    //Read clock.
    ret = com_read(connection, &clock.base, 2);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    //Print clock.
    ret = time_print(NULL, &clock.time);
    return ret;
}

/**
* Calls disconnect method.
*/
int disconnect(connection* connection)
{
    int ret;
    gxDisconnectControl dc;
    unsigned char ln[] = { 0,0,96,3,10,255 };
    INIT_OBJECT(dc, DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln);
    //Call Disconnect action.
    ret = com_method(connection, &dc.base, 1, NULL);
    return ret;
}

/*Read DLMS meter using TCP/IP connection.*/
int readTcpIpConnection(
    connection* connection,
    const char* address,
    const int port,
    char* readObjects,
    const char* invocationCounter)
{
    int ret;
#if defined(_WIN32) || defined(_WIN64)//Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        // Tell the user that we could not find a usable WinSock DLL.
        return 1;
    }
#endif
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("Connecting to %s:%d\n", address, port);
    }
    //Make connection to the meter.
    ret = com_makeConnect(connection, address, port, 5000);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        con_close(connection);
        com_close(connection);
        return ret;
    }
    if (readObjects != NULL)
    {
        if ((ret = com_updateInvocationCounter(connection, invocationCounter)) == 0 &&
            (ret = com_initializeConnection(connection)) == 0 &&
            (ret = com_getAssociationView(connection)) == 0)
        {
            int index;
            unsigned char buff[200];
            gxObject* obj = NULL;
            char* p2, * p = readObjects;
            do
            {
                if (p != readObjects)
                {
                    ++p;
                }

                p2 = strchr(p, ':');
                *p2 = '\0';
                ++p2;
#if defined(_WIN32) || defined(_WIN64)//Windows
                sscanf_s(p2, "%d", &index);
#else
                sscanf(p2, "%d", &index);
#endif
                hlp_setLogicalName(buff, p);
                if ((ret = oa_findByLN(&connection->settings.objects, DLMS_OBJECT_TYPE_NONE, buff, &obj)) == 0)
                {
                    if (obj == NULL)
                    {
                        printf("Object '%s' not found from the association view.\n", p);
                        break;
                    }
                    com_readValue(connection, obj, index);
                }
            } while ((p = strchr(p2, ',')) != NULL);
        }
    }
    else
    {
        //Initialize connection.
        if ((ret = com_updateInvocationCounter(connection, invocationCounter)) != 0 ||
            (ret = com_initializeConnection(connection)) != 0 ||
            (ret = com_readAllObjects(connection)) != 0)
            //Read all objects from the meter.
        {
            //Error code is returned at the end of the function.
        }
    }
    //Close connection.
    com_close(connection);
    con_close(connection);
    return ret;
}

/*Read DLMS meter using serial port connection.*/
int readSerialPort(
    connection* connection,
    const char* port,
    unsigned char iec,
    char* readObjects,
    char* invocationCounter)
{
    int ret;
    ret = com_open(connection, port, iec);
    if (ret == 0 && readObjects != NULL)
    {
        if ((ret = com_initializeOpticalHead(connection, iec)) == 0 &&
            (ret = com_updateInvocationCounter(connection, invocationCounter)) == 0 &&
            (ret = com_initializeOpticalHead(connection, iec)) == 0 &&
            (ret = com_initializeConnection(connection)) == 0 &&
            (ret = com_getAssociationView(connection)) == 0)
        {
            int index;
            unsigned char buff[200];
            gxObject* obj = NULL;
            char* p2, * p = readObjects;
            do
            {
                if (p != readObjects)
                {
                    ++p;
                }

                p2 = strchr(p, ':');
                *p2 = '\0';
                ++p2;
#if defined(_WIN32) || defined(_WIN64)//Windows
                sscanf_s(p2, "%d", &index);
#else
                sscanf(p2, "%d", &index);
#endif
                hlp_setLogicalName(buff, p);
                oa_findByLN(&connection->settings.objects, DLMS_OBJECT_TYPE_NONE, buff, &obj);
                com_readValue(connection, obj, index);
            } while ((p = strchr(p2, ',')) != NULL);
        }
    }
    else if (ret == 0)
    {
        //Initialize connection.
        if ((ret = com_initializeOpticalHead(connection, iec)) != 0 ||
            (ret = com_updateInvocationCounter(connection, invocationCounter)) != 0 ||
            (ret = com_initializeOpticalHead(connection, iec)) != 0 ||
            (ret = com_initializeConnection(connection)) != 0 ||
            (ret = com_readAllObjects(connection)) != 0)
            //Read all objects from the meter.
        {
            //Error code is returned at the end of the function.
        }
    }
    //Close connection.
    com_close(connection);
    con_close(connection);
    return ret;
}

static void ShowHelp()
{
    printf("GuruxDlmsSample reads data from the DLMS/COSEM device.\n");
    printf("GuruxDlmsSample -h [Meter IP Address] -p [Meter Port No] -c 16 -s 1 -r SN\n");
    printf(" -h \t host name or IP address.\n");
    printf(" -p \t port number or name (Example: 1000).\n");
    printf(" -S \t serial port.\n");
    printf(" -i \t IEC is a start protocol.\n");
    printf(" -a \t Authentication (None, Low, High).\n");
    printf(" -P \t Password for authentication.\n");
    printf(" -c \t Client address. (Default: 16)\n");
    printf(" -s \t Server address. (Default: 1)\n");
    printf(" -n \t Server address as serial number.\n");
    printf(" -r \t [sn, sn]\t Short name or Logican Name (default) referencing is used.\n");
    printf(" -w \t WRAPPER profile is used. HDLC is default.\n");
    printf(" -t \t Trace messages.\n");
    printf(" -g \t Get selected object(s) with given attribute index. Ex -g \"0.0.1.0.0.255:1; 0.0.1.0.0.255:2\" \n");
    printf(" -C \t Security Level. (None, Authentication, Encrypted, AuthenticationEncryption)\n");
    printf(" -v \t Invocation counter data object Logical Name. Ex. 0.0.43.1.1.255\n");
    printf(" -I \t Auto increase invoke ID\n");
    printf(" -T \t System title that is used with chiphering. Ex -T 4775727578313233\n");
    printf(" -A \t Authentication key that is used with chiphering. Ex -A D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF\n");
    printf(" -B \t Block cipher key that is used with chiphering. Ex -B 000102030405060708090A0B0C0D0E0F\n");
    printf(" -D \t Dedicated key that is used with chiphering. Ex -D 00112233445566778899AABBCCDDEEFF\n");
    printf("Example:\n");
    printf("Read LG device using TCP/IP connection.\n");
    printf("GuruxDlmsSample -r SN -c 16 -s 1 -h [Meter IP Address] -p [Meter Port No]\n");
    printf("Read LG device using serial port connection.\n");
    printf("GuruxDlmsSample -r SN -c 16 -s 1 -sp COM1 -i\n");
    printf("Read Indian device using serial port connection.\n");
    printf("GuruxDlmsSample -S COM1 -c 16 -s 1 -a Low -P [password]\n");
}

int connectMeter(int argc, char* argv[])
{
    connection con;
    gxByteBuffer item;
    bb_init(&item);
    con_init(&con, GX_TRACE_LEVEL_INFO);
    //Initialize settings using Logical Name referencing and HDLC.
    cl_init(&con.settings, 1, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
    int ret, opt = 0;
    int port = 0;
    char* address = NULL;
    char* serialPort = NULL;
    unsigned char iec = 0;
    char* p, * readObjects = NULL;
    int index, a, b, c, d, e, f;
    char* invocationCounter = NULL;
    while ((opt = getopt(argc, argv, "h:p:c:s:r:iIt:a:p:wP:g:S:C:v:T:A:B:D:")) != -1)
    {
        switch (opt)
        {
        case 'w':
            con.settings.interfaceType = DLMS_INTERFACE_TYPE_WRAPPER;
            break;
        case 'r':
            if (strcasecmp("sn", optarg) == 0)
            {
                con.settings.useLogicalNameReferencing = 0;
            }
            else if (strcasecmp("ln", optarg) == 0)
            {
                con.settings.useLogicalNameReferencing = 1;
            }
            else
            {
                printf("Invalid reference option.\n");
                return 1;
            }
            break;
        case 'h':
            //Host address.
            address = optarg;
            break;
        case 't':
            //Trace.
            if (strcasecmp("Error", optarg) == 0)
                con.trace = GX_TRACE_LEVEL_ERROR;
            else  if (strcasecmp("Warning", optarg) == 0)
                con.trace = GX_TRACE_LEVEL_WARNING;
            else  if (strcasecmp("Info", optarg) == 0)
                con.trace = GX_TRACE_LEVEL_INFO;
            else  if (strcasecmp("Verbose", optarg) == 0)
                con.trace = GX_TRACE_LEVEL_VERBOSE;
            else  if (strcasecmp("Off", optarg) == 0)
                con.trace = GX_TRACE_LEVEL_OFF;
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
        case 'P':
            bb_init(&con.settings.password);
            bb_addString(&con.settings.password, optarg);
            break;
        case 'i':
            //IEC.
            iec = 1;
            break;
        case 'I':
            // AutoIncreaseInvokeID.
            con.settings.autoIncreaseInvokeID = 1;
            break;
        case 'C':
            if (strcasecmp("None", optarg) == 0)
            {
                con.settings.cipher.security = DLMS_SECURITY_NONE;
            }
            else if (strcasecmp("Authentication", optarg) == 0)
            {
                con.settings.cipher.security = DLMS_SECURITY_AUTHENTICATION;
            }
            else if (strcasecmp("Encryption", optarg) == 0)
            {
                con.settings.cipher.security = DLMS_SECURITY_ENCRYPTION;
            }
            else if (strcasecmp("AuthenticationEncryption", optarg) == 0)
            {
                con.settings.cipher.security = DLMS_SECURITY_AUTHENTICATION_ENCRYPTION;
            }
            else
            {
                printf("Invalid Ciphering option '%s'. (None, Authentication, Encryption, AuthenticationEncryption)", optarg);
                return 1;
            }
            break;
        case 'T':
            bb_clear(&con.settings.cipher.systemTitle);
            bb_addHexString(&con.settings.cipher.systemTitle, optarg);
            if (con.settings.cipher.systemTitle.size != 8)
            {
                printf("Invalid system title '%s'.", optarg);
                return 1;
            }
            break;
        case 'A':
            bb_clear(&con.settings.cipher.authenticationKey);
            bb_addHexString(&con.settings.cipher.authenticationKey, optarg);
            if (con.settings.cipher.authenticationKey.size != 16)
            {
                printf("Invalid authentication key '%s'.", optarg);
                return 1;
            }
            break;
        case 'B':
            bb_clear(&con.settings.cipher.blockCipherKey);
            bb_addString(&con.settings.cipher.blockCipherKey, optarg);
            if (con.settings.cipher.blockCipherKey.size != 16)
            {
                printf("Invalid block cipher key '%s'.", optarg);
                return 1;
            }
            break;
        case 'D':
            con.settings.cipher.dedicatedKey = (gxByteBuffer*)malloc(sizeof(gxByteBuffer));
            bb_init(con.settings.cipher.dedicatedKey);
            bb_addString(con.settings.cipher.dedicatedKey, optarg);
            if (con.settings.cipher.dedicatedKey->size != 16)
            {
                printf("Invalid dedicated key '%s'.", optarg);
                return 1;
            }
            break;
        case 'v':
            invocationCounter = optarg;
#if defined(_WIN32) || defined(_WIN64)//Windows
            if ((ret = sscanf_s(optarg, "%d.%d.%d.%d.%d.%d", &a, &b, &c, &d, &e, &f)) != 6)
#else
            if ((ret = sscanf(optarg, "%d.%d.%d.%d.%d.%d", &a, &b, &c, &d, &e, &f)) != 6)
#endif
            {
                ShowHelp();
                return 1;
            }
            break;
        case 'g':
            //Get (read) selected objects.
            p = optarg;
            do
            {
                if (p != optarg)
                {
                    ++p;
                }
#if defined(_WIN32) || defined(_WIN64)//Windows
                if ((ret = sscanf_s(p, "%d.%d.%d.%d.%d.%d:%d", &a, &b, &c, &d, &e, &f, &index)) != 7)
#else
                if ((ret = sscanf(p, "%d.%d.%d.%d.%d.%d:%d", &a, &b, &c, &d, &e, &f, &index)) != 7)
#endif
                {
                    ShowHelp();
                    return 1;
                }
            } while ((p = strchr(p, ',')) != NULL);
            readObjects = optarg;
            break;
        case 'S':
            serialPort = optarg;
            break;
        case 'a':
            if (strcasecmp("None", optarg) == 0)
            {
                con.settings.authentication = DLMS_AUTHENTICATION_NONE;
            }
            else if (strcasecmp("Low", optarg) == 0)
            {
                con.settings.authentication = DLMS_AUTHENTICATION_LOW;
            }
            else if (strcasecmp("High", optarg) == 0)
            {
                con.settings.authentication = DLMS_AUTHENTICATION_HIGH;
            }
            else if (strcasecmp("HighMd5", optarg) == 0)
            {
                con.settings.authentication = DLMS_AUTHENTICATION_HIGH_MD5;
            }
            else if (strcasecmp("HighSha1", optarg) == 0)
            {
                con.settings.authentication = DLMS_AUTHENTICATION_HIGH_SHA1;
            }
            else if (strcasecmp("HighGmac", optarg) == 0)
            {
                con.settings.authentication = DLMS_AUTHENTICATION_HIGH_GMAC;
            }
            else if (strcasecmp("HighSha256", optarg) == 0)
            {
                con.settings.authentication = DLMS_AUTHENTICATION_HIGH_SHA256;
            }
            else
            {
                printf("Invalid Authentication option. (None, Low, High, HighMd5, HighSha1, HighGmac, HighSha256)\n");
                return 1;
            }
            break;
        case 'o':
            break;
        case 'c':
            con.settings.clientAddress = atoi(optarg);
            break;
        case 's':
            con.settings.serverAddress = atoi(optarg);
            break;
        case '?':
        {
            if (optarg[0] == 'c')
            {
                printf("Missing mandatory client option.\n");
            }
            else if (optarg[0] == 's')
            {
                printf("Missing mandatory server option.\n");
            }
            else if (optarg[0] == 'h')
            {
                printf("Missing mandatory host name option.\n");
            }
            else if (optarg[0] == 'p')
            {
                printf("Missing mandatory port option.\n");
            }
            else if (optarg[0] == 'r')
            {
                printf("Missing mandatory reference option.\n");
            }
            else if (optarg[0] == 'a')
            {
                printf("Missing mandatory authentication option.\n");
            }
            else if (optarg[0] == 'S')
            {
                printf("Missing mandatory Serial port option.\n");
            }
            else if (optarg[0] == 'g') {
                printf("Missing mandatory OBIS code option.\n");
            }
            else if (optarg[0] == 'C') {
                printf("Missing mandatory Ciphering option.\n");
            }
            else if (optarg[0] == 'v') {
                printf("Missing mandatory invocation counter logical name option.\n");
            }
            else if (optarg[0] == 'T') {
                printf("Missing mandatory system title option.");
            }
            else if (optarg[0] == 'A') {
                printf("Missing mandatory authentication key option.");
            }
            else if (optarg[0] == 'B') {
                printf("Missing mandatory block cipher key option.");
            }
            else if (optarg[0] == 'D') {
                printf("Missing mandatory dedicated key option.");
            }
            else
            {
                return 1;
            }
        }
        break;
        default:
            return 1;
        }
    }

    if (port != 0 || address != NULL)
    {
        if (port == 0)
        {
            printf("Missing mandatory port option.\n");
            return 1;
        }
        if (address == NULL)
        {
            printf("Missing mandatory host option.\n");
            return 1;
        }
        ret = readTcpIpConnection(&con, address, port, readObjects, invocationCounter);
    }
    else if (serialPort != NULL)
    {
        ret = readSerialPort(&con, serialPort, iec, readObjects, invocationCounter);
    }
    else
    {
        printf("Missing mandatory connection information for TCP/IP or serial port connection.\n");
        return 1;
    }

    //Clear objects.
    if (ret != 0)
    {
        printf("%s\n", hlp_getErrorMessage(ret));
    }
    else
    {
        printf("All items are read.\n");
    }
    cl_clear(&con.settings);
    return 0;
}

int main(int argc, char* argv[])
{
    int ret = connectMeter(argc, argv);
    if (ret != 0)
    {
        ShowHelp();
    }
#if defined(_WIN32) || defined(_WIN64)//Windows
    WSACleanup();
#if _MSC_VER > 1000
    //Show memory leaks.
    _CrtDumpMemoryLeaks();
#endif //_MSC_VER
#endif
    return ret;
}

