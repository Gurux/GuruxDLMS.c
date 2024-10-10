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
#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <ws2tcpip.h>
#include <Windows.h> //Add support for serial port functions.
#else
#define INVALID_HANDLE_VALUE -1
#include <unistd.h>
#include <stdlib.h>
#include <errno.h> //Add support for sockets
#include <netdb.h> //Add support for sockets
#include <sys/types.h> //Add support for sockets
#include <sys/socket.h> //Add support for sockets
#include <netinet/in.h> //Add support for sockets
#include <arpa/inet.h> //Add support for sockets
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "../include/communication.h"
#include "../dlms/include/gxkey.h"
#include "../dlms/include/converters.h"
#include "../dlms/include/cosem.h"
#include "../dlms/include/gxserializer.h"
#include "../dlms/include/gxget.h"

//Returns current time.
//If you are not using operating system you have to implement this by yourself.
//Reason for this is that all compilers's or HWs don't support time at all.
void time_now(gxtime* value)
{
    time_initUnix(value, (unsigned long)time(NULL));
}

//Check is IP address IPv6 or IPv4 address.
unsigned char com_isIPv6Address(const char* pAddress)
{
    return strstr(pAddress, ":") != NULL;
}

//Make clientConnection using TCP/IP clientConnection.
int com_makeConnect(clientConnection* clientConnection, const char* address, int port, int waitTime)
{
    int ret;
    //create socket.
    int family = com_isIPv6Address(address) ? AF_INET6 : AF_INET;
    clientConnection->socket = socket(family, SOCK_STREAM, IPPROTO_IP);
    if (clientConnection->socket == -1)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        int err = WSAGetLastError();
        printf("Socket creation failed: %d\r\n", err);
#else
        int err = errno;
        printf("Socket creation failed: %d, %s\r\n", err, strerror(err));
#endif
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    struct sockaddr* add;
    int addSize;
    struct sockaddr_in6 addrIP6;
    struct sockaddr_in addIP4;
    if (family == AF_INET)
    {
        addIP4.sin_port = htons(port);
        addIP4.sin_family = AF_INET;
        addIP4.sin_addr.s_addr = inet_addr(address);
        addSize = sizeof(struct sockaddr_in);
        add = (struct sockaddr*)&addIP4;
        //If address is give as name
        if (addIP4.sin_addr.s_addr == INADDR_NONE)
        {
            struct hostent* Hostent = gethostbyname(address);
            if (Hostent == NULL)
            {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
                ret = WSAGetLastError();
                closesocket(clientConnection->socket);
#else
                ret = errno;
                close(clientConnection->socket);
#endif
                clientConnection->socket = -1;
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            };
            addIP4.sin_addr = *(struct in_addr*)(void*)Hostent->h_addr_list[0];
        };
    }
    else
    {
        memset(&addrIP6, 0, sizeof(struct sockaddr_in6));
        addrIP6.sin6_port = htons(port);
        addrIP6.sin6_family = AF_INET6;
        ret = inet_pton(family, address, &(addrIP6.sin6_addr));
        if (ret == -1)
        {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
            ret = WSAGetLastError();
            closesocket(clientConnection->socket);
#else
            ret = errno;
            close(clientConnection->socket);
#endif
            clientConnection->socket = -1;
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        };
        add = (struct sockaddr*)&addrIP6;
        addSize = sizeof(struct sockaddr_in6);
    }

    //Set timeout.
#if defined(_WIN32) || defined(_WIN64)//If Windows
    setsockopt(clientConnection->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&waitTime, sizeof(waitTime));
#else
    struct timeval tv;
    tv.tv_sec = waitTime / 1000;
    tv.tv_usec = 0;
    setsockopt(clientConnection->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif //
    //Connect to the meter.
    ret = connect(clientConnection->socket, add, addSize);
    if (ret == -1)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        ret = WSAGetLastError();
        closesocket(clientConnection->socket);
        printf("Connection failed: %d\n", ret);
#else
        ret = errno;
        close(clientConnection->socket);
        printf("Connection failed: %d, %s\n", ret, strerror(ret));
#endif
        clientConnection->socket = -1;
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    };
    return DLMS_ERROR_CODE_OK;
}

#if defined(_WIN32) || defined(_WIN64)//If Windows
int com_setCommState(HANDLE hWnd, LPDCB DCB)
{
    if (!SetCommState(hWnd, DCB))
    {
        DWORD err = GetLastError(); //Save occured error.
        if (err == 995)
        {
            COMSTAT comstat;
            unsigned long RecieveErrors;
            if (!ClearCommError(hWnd, &RecieveErrors, &comstat))
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | GetLastError();
            }
            if (!SetCommState(hWnd, DCB))
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | GetLastError();
            }
        }
        else
        {
            //If USB to serial port converters do not implement this.
            if (err != ERROR_INVALID_FUNCTION)
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | err;
            }
        }
    }
    return DLMS_ERROR_CODE_OK;
}
#endif

int com_readSerialPort(
    clientConnection* clientConnection,
    unsigned char eop)
{
    //Read reply data.
    int ret, cnt = 1, pos;
    unsigned char eopFound = 0;
    int lastReadIndex = 0;
#if defined(_WIN32) || defined(_WIN64)//Windows
    unsigned long RecieveErrors;
    COMSTAT comstat;
    DWORD bytesRead = 0;
#else
    unsigned short bytesRead = 0;
    unsigned short readTime = 0;
#endif
    do
    {
#if defined(_WIN32) || defined(_WIN64)//Windows
        //We do not want to read byte at the time.
        if (!ClearCommError(clientConnection->comPort, &RecieveErrors, &comstat))
        {
            ret = GetLastError();
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        }
        bytesRead = 0;
        cnt = 1;
        //Try to read at least one byte.
        if (comstat.cbInQue > 0)
        {
            cnt = comstat.cbInQue;
        }
        if (!ReadFile(clientConnection->comPort, clientConnection->data.data + clientConnection->data.size, cnt, &bytesRead, &clientConnection->osReader))
        {
            ret = GetLastError();
            if (ret != ERROR_IO_PENDING)
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            }
            //Wait until data is actually read
            if (WaitForSingleObject(clientConnection->osReader.hEvent, clientConnection->waitTime) != WAIT_OBJECT_0)
            {
                return DLMS_ERROR_CODE_RECEIVE_FAILED;
            }
            if (!GetOverlappedResult(clientConnection->comPort, &clientConnection->osReader, &bytesRead, TRUE))
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | GetLastError();
            }
        }
#else
        //Get bytes available.
        ret = ioctl(clientConnection->comPort, FIONREAD, &cnt);
        //If driver is not supporting this functionality.
        if (ret < 0)
        {
            cnt = RECEIVE_BUFFER_SIZE;
        }
        else if (cnt == 0)
        {
            //Try to read at least one byte.
            cnt = 1;
        }
        //If there is more data than can fit to buffer.
        if (cnt > RECEIVE_BUFFER_SIZE)
        {
            cnt = RECEIVE_BUFFER_SIZE;
        }
        bytesRead = read(clientConnection->comPort, clientConnection->data.data + clientConnection->data.size, cnt);
        if (bytesRead == 0xFFFF)
        {
            //If there is no data on the read buffer.
            if (errno == EAGAIN)
            {
                if (readTime > clientConnection->waitTime)
                {
                    return DLMS_ERROR_CODE_RECEIVE_FAILED;
                }
                readTime += 100;
                bytesRead = 0;
            }
            else
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | errno;
            }
        }
#endif
        clientConnection->data.size += (unsigned short)bytesRead;
        //Note! Some USB converters can return true for ReadFile and Zero as bytesRead.
        //In that case wait for a while and read again.
        if (bytesRead == 0)
        {
#if defined(_WIN32) || defined(_WIN64)//Windows
            Sleep(100);
#else
            usleep(100000);
#endif
            continue;
        }
        //Search eop.
        if (clientConnection->data.size > 5)
        {
            //Some optical strobes can return extra bytes.
            for (pos = clientConnection->data.size - 1; pos != lastReadIndex; --pos)
            {
                if (clientConnection->data.data[pos] == eop)
                {
                    eopFound = 1;
                    break;
                }
            }
            lastReadIndex = pos;
        }
    } while (eopFound == 0);
    return DLMS_ERROR_CODE_OK;
}

int com_initializeOpticalHead(
    clientConnection* clientConnection)
{
    unsigned short baudRate;
    int ret = 0, len, pos;
    unsigned char ch;
    //In Linux serial port name might be very long.
    char buff[50];
#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = { 0 };
    unsigned long sendSize = 0;
    if (clientConnection->comPort == INVALID_HANDLE_VALUE)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(clientConnection->comPort, &dcb))
    {
        ret = GetLastError();
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    dcb.fBinary = 1;
    dcb.fOutX = dcb.fInX = 0;
    //Abort all reads and writes on Error.
    dcb.fAbortOnError = 1;
    if (clientConnection->settings.interfaceType == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
    {
        dcb.BaudRate = 300;
        dcb.ByteSize = 7;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = EVENPARITY;
    }
    else
    {
        dcb.BaudRate = 9600;
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = NOPARITY;
    }
    if ((ret = com_setCommState(clientConnection->comPort, &dcb)) != 0)
    {
        return ret;
    }
#else
    struct termios options;
    memset(&options, 0, sizeof(options));
    options.c_iflag = 0;
    options.c_oflag = 0;
    if (clientConnection->settings.interfaceType == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
    {
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS7;
        //Set Baud Rates
        cfsetospeed(&options, B300);
        cfsetispeed(&options, B300);
    }
    else
    {
        // 8n1, see termios.h for more information
        options.c_cflag = CS8 | CREAD | CLOCAL;
        /*
        options.c_cflag &= ~PARENB
        options.c_cflag &= ~CSTOPB
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        */
        //Set Baud Rates
        cfsetospeed(&options, B9600);
        cfsetispeed(&options, B9600);
    }
    options.c_lflag = 0;
    options.c_cc[VMIN] = 1;
    //How long we are waiting reply charachter from serial port.
    options.c_cc[VTIME] = 5;

    //hardware flow control is used as default.
    //options.c_cflag |= CRTSCTS;
    if (tcsetattr(clientConnection->comPort, TCSAFLUSH, &options) != 0)
    {
        printf("Failed to Open port. tcsetattr failed.\r");
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#endif
    if (clientConnection->settings.interfaceType == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
    {
#if _MSC_VER > 1000
        strcpy_s(buff, 50, "/?!\r\n");
#else
        strcpy(buff, "/?!\r\n");
#endif
        len = strlen(buff);
        if (clientConnection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\nTX:\t");
            for (pos = 0; pos != len; ++pos)
            {
                printf("%.2X ", buff[pos]);
            }
            printf("\n");
        }
#if defined(_WIN32) || defined(_WIN64)
        ret = WriteFile(clientConnection->comPort, buff, len, &sendSize, &clientConnection->osWrite);
        if (ret == 0)
        {
            DWORD err = GetLastError();
            //If error occurs...
            if (err != ERROR_IO_PENDING)
            {
                return DLMS_ERROR_CODE_SEND_FAILED;
            }
            //Wait until data is actually sent
            WaitForSingleObject(clientConnection->osWrite.hEvent, INFINITE);
        }
#else //#if defined(__LINUX__)
        ret = write(clientConnection->comPort, buff, len);
        if (ret != len)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
#endif
        if (clientConnection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\nRX:\t");
        }
        //Read reply data.
        if (com_readSerialPort(clientConnection, '\n') != 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if (clientConnection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\r\n");
        }
        if (bb_getUInt8(&clientConnection->data, &ch) != 0 || ch != '/')
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
        //Get used baud rate.
        if ((ret = bb_getUInt8ByIndex(&clientConnection->data, 4, &ch)) != 0)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
        switch (ch)
        {
        case '0':
#if defined(_WIN32) || defined(_WIN64)
            baudRate = 300;
#else
            baudRate = B300;
#endif
            break;
        case '1':
#if defined(_WIN32) || defined(_WIN64)
            baudRate = 600;
#else
            baudRate = B600;
#endif
            break;
        case '2':
#if defined(_WIN32) || defined(_WIN64)
            baudRate = 1200;
#else
            baudRate = B1200;
#endif
            break;
        case '3':
#if defined(_WIN32) || defined(_WIN64)
            baudRate = 2400;
#else
            baudRate = B2400;
#endif
            break;
        case '4':
#if defined(_WIN32) || defined(_WIN64)
            baudRate = 4800;
#else
            baudRate = B4800;
#endif
            break;
        case '5':
#if defined(_WIN32) || defined(_WIN64)
            baudRate = 9600;
#else
            baudRate = B9600;
#endif
            break;
        case '6':
#if defined(_WIN32) || defined(_WIN64)
            baudRate = 19200;
#else
            baudRate = B19200;
#endif
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        //Send ACK
        buff[0] = 0x06;
        //Send Protocol control character
        buff[1] = '2';// "2" HDLC protocol procedure (Mode E)
        buff[2] = (char)ch;
        buff[3] = '2';
        buff[4] = (char)0x0D;
        buff[5] = 0x0A;
        len = 6;
        if (clientConnection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\nTX:\t");
            for (pos = 0; pos != len; ++pos)
            {
                printf("%.2X ", buff[pos]);
            }
            printf("\n");
        }
#if defined(_WIN32) || defined(_WIN64)//Windows
        ret = WriteFile(clientConnection->comPort, buff, len, &sendSize, &clientConnection->osWrite);
        if (ret == 0)
        {
            int err = GetLastError();
            //If error occurs...
            if (err != ERROR_IO_PENDING)
            {
                printf("WriteFile %d\r\n", err);
                return DLMS_ERROR_CODE_SEND_FAILED;
            }
            //Wait until data is actually sent
            WaitForSingleObject(clientConnection->osWrite.hEvent, INFINITE);
        }
#else //#if defined(__LINUX__)
        ret = write(clientConnection->comPort, buff, len);
        if (ret != len)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
#endif
#if defined(_WIN32) || defined(_WIN64)//Windows
        //This sleep is in standard. Do not remove.
        Sleep(1000);
        dcb.BaudRate = baudRate;
        if ((ret = com_setCommState(clientConnection->comPort, &dcb)) != 0)
        {
            return ret;
        }
        printf("New baudrate %d\r\n", dcb.BaudRate);
        len = 6;
        if ((ret = com_readSerialPort(clientConnection, '\n')) != 0)
        {
            printf("Read %d\r\n", ret);
            return ret;
        }
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = NOPARITY;
        if ((ret = com_setCommState(clientConnection->comPort, &dcb)) != 0)
        {
            return ret;
        }
#else
        //This sleep is in standard. Do not remove.
        usleep(1000000);
        // 8n1, see termios.h for more information
        options.c_cflag = CS8 | CREAD | CLOCAL;
        //Set Baud Rates
        cfsetospeed(&options, baudRate);
        cfsetispeed(&options, baudRate);
        if (tcsetattr(clientConnection->comPort, TCSAFLUSH, &options) != 0)
        {
            printf("Failed to Open port. tcsetattr failed.\r");
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif
    }
    return ret;
}

int com_open(
    clientConnection* clientConnection,
    const char* port)
{
    int ret;
    //In Linux serial port name might be very long.
#if defined(_WIN32) || defined(_WIN64)
    char buff[50];
#if _MSC_VER > 1000
    sprintf_s(buff, 50, "\\\\.\\%s", port);
#else
    sprintf(buff, "\\\\.\\%s", port);
#endif
    //Close connection if it's open.
    if (clientConnection->comPort != INVALID_HANDLE_VALUE)
    {
        com_close(clientConnection);
    }

    //Open serial port for read / write. Port can't share.
    clientConnection->comPort = CreateFileA(buff,
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (clientConnection->comPort == INVALID_HANDLE_VALUE)
    {
        ret = GetLastError();
        printf("Failed to open serial port: \"%s\"\n", buff);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
#else //#if defined(__LINUX__)
    // read/write | not controlling term | don't wait for DCD line signal.
    clientConnection->comPort = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (clientConnection->comPort == -1) // if open is unsuccessful.
    {
        ret = errno;
        printf("Failed to open serial port: %s\n", port);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    if (!isatty(clientConnection->comPort))
    {
        ret = errno;
        printf("Failed to Open port %s. This is not a serial port.\n", port);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
#endif
    return com_initializeOpticalHead(clientConnection);
}

int sendData(clientConnection* clientConnection, gxByteBuffer* data)
{
    int ret = 0;
#if defined(_WIN32) || defined(_WIN64)//Windows
    unsigned long sendSize = 0;
#endif
    if (clientConnection->comPort != INVALID_HANDLE_VALUE)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows
        ret = WriteFile(clientConnection->comPort, data->data, data->size, &sendSize, &clientConnection->osWrite);
        if (ret == 0)
        {
            COMSTAT comstat;
            unsigned long RecieveErrors;
            DWORD err = GetLastError();
            //If error occurs...
            if (err != ERROR_IO_PENDING)
            {
                ret = GetLastError();
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            }
            //Wait until data is actually sent
            ret = WaitForSingleObject(clientConnection->osWrite.hEvent, clientConnection->waitTime);
            if (ret != 0)
            {
                ret = GetLastError();
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            }
            //Read bytes in output buffer. Some USB converts require this.
            if (!ClearCommError(clientConnection->comPort, &RecieveErrors, &comstat))
            {
                ret = GetLastError();
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            }
        }
#else
        ret = write(clientConnection->comPort, data->data, data->size);
        if (ret != data->size)
        {
            ret = errno;
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        }
#endif
    }
    else
    {
        if (send(clientConnection->socket, (const char*)data->data, data->size, 0) == -1)
        {
#if defined(_WIN32) || defined(_WIN64)//If Windows
            ret = WSAGetLastError();
#else
            ret = errno;
#endif
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        }
    }
    return 0;
}

int readData(clientConnection* clientConnection, gxByteBuffer* data, int* index)
{
    int ret = 0;
    if (clientConnection->comPort != INVALID_HANDLE_VALUE)
    {
        if ((ret = com_readSerialPort(clientConnection, 0x7E)) != 0)
        {
            return ret;
        }
    }
    else
    {
        uint32_t cnt = clientConnection->data.capacity - clientConnection->data.size;
        if (cnt < 1)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        if ((ret = recv(clientConnection->socket, (char*)clientConnection->data.data + clientConnection->data.size, cnt, 0)) == -1)
        {
            return DLMS_ERROR_CODE_RECEIVE_FAILED;
        }
        clientConnection->data.size += ret;
    }
    if (clientConnection->trace > GX_TRACE_LEVEL_INFO)
    {
        char* hex = hlp_bytesToHex(clientConnection->data.data + *index, clientConnection->data.size - *index);
        if (*index == 0)
        {
            printf("\nRX:\t %s", hex);
        }
        else
        {
            printf(" %s", hex);
        }
        free(hex);
        *index = clientConnection->data.size;
    }
    return 0;
}

// Read DLMS Data frame from the device.
int readDLMSPacket(
    clientConnection* clientConnection,
    gxByteBuffer* data,
    gxReplyData* reply)
{
    char* hex;
    int index = 0, ret;
    if (data->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    reply->complete = 0;
    clientConnection->data.size = 0;
    clientConnection->data.position = 0;
    if (clientConnection->trace == GX_TRACE_LEVEL_VERBOSE)
    {
        hex = bb_toHexString(data);
        printf("\nTX:\t%s\n", hex);
        free(hex);
    }
    if ((ret = sendData(clientConnection, data)) != 0)
    {
        return ret;
    }
    //Loop until packet is complete.
    unsigned char pos = 0;
    do
    {
        if ((ret = readData(clientConnection, &clientConnection->data, &index)) != 0)
        {
            if (ret != DLMS_ERROR_CODE_RECEIVE_FAILED || pos == 3)
            {
                break;
            }
            ++pos;
            printf("\nData send failed. Try to resend %d/3\n", pos);
            if ((ret = sendData(clientConnection, data)) != 0)
            {
                break;
            }
        }
        else
        {
            ret = cl_getData(&clientConnection->settings, &clientConnection->data, reply);
            if (ret != 0 && ret != DLMS_ERROR_CODE_FALSE)
            {
                break;
            }
        }
    } while (reply->complete == 0);
    if (clientConnection->trace == GX_TRACE_LEVEL_VERBOSE)
    {
        printf("\n");
    }
    return ret;
}

int com_readDataBlock(
    clientConnection* clientConnection,
    message* messages,
    gxReplyData* reply)
{
    gxByteBuffer rr;
    int pos, ret = DLMS_ERROR_CODE_OK;
    //If there is no data to send.
    if (messages->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    bb_init(&rr);
    //Send data.
    for (pos = 0; pos != messages->size; ++pos)
    {
        //Send data.
        if ((ret = readDLMSPacket(clientConnection, messages->data[pos], reply)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        //Check is there errors or more data from server
        while (reply_isMoreData(reply))
        {
            if ((ret = cl_receiverReady(&clientConnection->settings, reply->moreData, &rr)) != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&rr);
                return ret;
            }
            if ((ret = readDLMSPacket(clientConnection, &rr, reply)) != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&rr);
                return ret;
            }
            bb_clear(&rr);
        }
    }
    return ret;
}


//Close clientConnection to the meter.
int com_disconnect(
    clientConnection* clientConnection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    reply_init(&reply);
    mes_init(&msg);
    if ((ret = cl_disconnectRequest(&clientConnection->settings, &msg)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &msg, &reply)) != 0)
    {
        //Show error but continue close.
        printf("Close failed.");
    }
    reply_clear(&reply);
    mes_clear(&msg);
    return ret;
}

//Close clientConnection to the meter.
int com_close(
    clientConnection* clientConnection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    //If client is closed.
    if (!clientConnection->settings.server)
    {
        reply_init(&reply);
        mes_init(&msg);
        if ((ret = cl_releaseRequest(&clientConnection->settings, &msg)) != 0 ||
            (ret = com_readDataBlock(clientConnection, &msg, &reply)) != 0)
        {
            //Show error but continue close.
            printf("Release failed.");
        }
        reply_clear(&reply);
        mes_clear(&msg);

        if ((ret = cl_disconnectRequest(&clientConnection->settings, &msg)) != 0 ||
            (ret = com_readDataBlock(clientConnection, &msg, &reply)) != 0)
        {
            //Show error but continue close.
            printf("Close failed.");
        }
        reply_clear(&reply);
        mes_clear(&msg);
    }
    if (clientConnection->socket != -1)
    {
        clientConnection->closing = 1;
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        closesocket(clientConnection->socket);
#else
        close(clientConnection->socket);
#endif
        clientConnection->socket = -1;
    }
    else if (clientConnection->comPort != INVALID_HANDLE_VALUE)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        CloseHandle(clientConnection->comPort);
        clientConnection->comPort = INVALID_HANDLE_VALUE;
        //Don't close osReader or osWrite
#else
        close(clientConnection->comPort);
#endif
        clientConnection->comPort = INVALID_HANDLE_VALUE;
    }
    cl_clear(&clientConnection->settings);
    return ret;
}

//Read Invocation counter (frame counter) from the meter and update it.
int com_updateInvocationCounter(
    clientConnection* clientConnection,
    const char* invocationCounter)
{
    int ret = DLMS_ERROR_CODE_OK;
    //Read frame counter if GeneralProtection is used.
    if (invocationCounter != NULL && clientConnection->settings.cipher.security != DLMS_SECURITY_NONE)
    {
        message messages;
        gxReplyData reply;
        unsigned short add = clientConnection->settings.clientAddress;
        DLMS_AUTHENTICATION auth = clientConnection->settings.authentication;
        DLMS_SECURITY security = clientConnection->settings.cipher.security;
        gxByteBuffer challenge;
        bb_init(&challenge);
        bb_set(&challenge, clientConnection->settings.ctoSChallenge.data, clientConnection->settings.ctoSChallenge.size);
        clientConnection->settings.clientAddress = 16;
        clientConnection->settings.authentication = DLMS_AUTHENTICATION_NONE;
        clientConnection->settings.cipher.security = DLMS_SECURITY_NONE;
        if (clientConnection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("updateInvocationCounter\r\n");
        }
        mes_init(&messages);
        reply_init(&reply);
        //Get meter's send and receive buffers size.
        if ((ret = cl_snrmRequest(&clientConnection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(clientConnection, &messages, &reply)) != 0 ||
            (ret = cl_parseUAResponse(&clientConnection->settings, &reply.data)) != 0)
        {
            bb_clear(&challenge);
            mes_clear(&messages);
            reply_clear(&reply);
            if (clientConnection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("SNRMRequest failed %s\r\n", hlp_getErrorMessage(ret));
            }
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
        if ((ret = cl_aarqRequest(&clientConnection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(clientConnection, &messages, &reply)) != 0 ||
            (ret = cl_parseAAREResponse(&clientConnection->settings, &reply.data)) != 0)
        {
            bb_clear(&challenge);
            mes_clear(&messages);
            reply_clear(&reply);
            if (ret == DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
            {
                if (clientConnection->trace > GX_TRACE_LEVEL_OFF)
                {
                    printf("Use Logical Name referencing is wrong. Change it!\r\n");
                }
                return ret;
            }
            if (clientConnection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("AARQRequest failed %s\r\n", hlp_getErrorMessage(ret));
            }
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
        if (clientConnection->settings.maxPduSize == 0xFFFF)
        {
            con_initializeBuffers(clientConnection, clientConnection->settings.maxPduSize);
        }
        else
        {
            //Allocate 50 bytes more because some meters count this wrong and send few bytes too many.
            con_initializeBuffers(clientConnection, 50 + clientConnection->settings.maxPduSize);
        }
        gxData d;
        cosem_init(BASE(d), DLMS_OBJECT_TYPE_DATA, invocationCounter);
        if ((ret = com_read(clientConnection, BASE(d), 2)) == 0)
        {
            clientConnection->settings.cipher.invocationCounter = 1 + var_toInteger(&d.value);
            if (clientConnection->trace > GX_TRACE_LEVEL_WARNING)
            {
                printf("Invocation counter: %lu (0x%lX)\r\n",
                    clientConnection->settings.cipher.invocationCounter,
                    clientConnection->settings.cipher.invocationCounter);
            }
            //It's OK if this fails.
            com_disconnect(clientConnection);
            clientConnection->settings.clientAddress = add;
            clientConnection->settings.authentication = auth;
            clientConnection->settings.cipher.security = security;
            bb_clear(&clientConnection->settings.ctoSChallenge);
            bb_set(&clientConnection->settings.ctoSChallenge, challenge.data, challenge.size);
            bb_clear(&challenge);
        }
    }
    return ret;
}

//Initialize clientConnection to the meter.
int com_initializeConnection(
    clientConnection* clientConnection)
{
    message messages;
    gxReplyData reply;
    int ret = 0;
    if (clientConnection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("InitializeConnection\r\n");
    }

    mes_init(&messages);
    reply_init(&reply);
    //Get meter's send and receive buffers size.
    if ((ret = cl_snrmRequest(&clientConnection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(&clientConnection->settings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        if (clientConnection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("SNRMRequest failed %s\r\n", hlp_getErrorMessage(ret));
        }
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    if ((ret = cl_aarqRequest(&clientConnection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &messages, &reply)) != 0 ||
        (ret = cl_parseAAREResponse(&clientConnection->settings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        if (ret == DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
        {
            if (clientConnection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Use Logical Name referencing is wrong. Change it!\r\n");
            }
            return ret;
        }
        if (clientConnection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("AARQRequest failed %s\r\n", hlp_getErrorMessage(ret));
        }
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    if (clientConnection->settings.maxPduSize == 0xFFFF)
    {
        con_initializeBuffers(clientConnection, clientConnection->settings.maxPduSize);
    }
    else
    {
        //Allocate 50 bytes more because some meters count this wrong and send few bytes too many.
        con_initializeBuffers(clientConnection, 50 + clientConnection->settings.maxPduSize);
    }
/*
    // Get challenge Is HLS authentication is used.
    if (clientConnection->settings.authentication > DLMS_AUTHENTICATION_LOW)
    {
        if ((ret = cl_getApplicationAssociationRequest(&clientConnection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(clientConnection, &messages, &reply)) != 0 ||
            (ret = cl_parseApplicationAssociationResponse(&clientConnection->settings, &reply.data)) != 0)
        {
            mes_clear(&messages);
            reply_clear(&reply);
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
    }
    */
    return DLMS_ERROR_CODE_OK;
}

//Report error on output;
void com_reportError(const char* description,
    gxObject* object,
    unsigned char attributeOrdinal, int ret)
{
    char ln[25];
    hlp_getLogicalNameToString(object->logicalName, ln);
    printf("%s %s %s:%d %s\r\n", description, obj_typeToString2(object->objectType), ln, attributeOrdinal, hlp_getErrorMessage(ret));
}

static unsigned char CURRENT_LN[] = { 0, 0, 40, 0, 0, 0xFF };

//Association view is read from association logical name object.
int com_loadAssociationView(clientConnection* clientConnection, const char* outputFile)
{
    int ret = 0;
    if (outputFile != NULL)
    {
        //Load settings.
#if _MSC_VER > 1400
        FILE* f = NULL;
        fopen_s(&f, outputFile, "rb");
#else
        FILE* f = fopen(outputFile, "rb");
#endif
        if (f != NULL)
        {
            gxObject* CURRENT_ASSOCIATION[1] = { NULL };
            gxAssociationLogicalName ln;
            gxAssociationShortName sn;
            gxSerializerSettings serializerSettings;
            ser_init(&serializerSettings);
            serializerSettings.stream = f;
            gxSerializerIgnore NON_SERIALIZED_OBJECTS[] = { IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, GET_ATTRIBUTE_EXCEPT(2)) };
            serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
            serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
            if (clientConnection->settings.useLogicalNameReferencing)
            {
                INIT_OBJECT(ln, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, CURRENT_LN);
                CURRENT_ASSOCIATION[0] = BASE(ln);
            }
            else
            {
                NON_SERIALIZED_OBJECTS[0].objectType = DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME;
                INIT_OBJECT(sn, DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME, CURRENT_LN);
                CURRENT_ASSOCIATION[0] = BASE(sn);
            }
            oa_clear(&clientConnection->settings.objects, 1);
            ret = ser_loadObjects(&clientConnection->settings, &serializerSettings, CURRENT_ASSOCIATION, sizeof(CURRENT_ASSOCIATION) / sizeof(CURRENT_ASSOCIATION[0]));
            fclose(f);
            if (ret == 0)
            {
                if (clientConnection->settings.useLogicalNameReferencing)
                {
                    oa_move(&clientConnection->settings.objects, &ln.objectList);
                    obj_clear(BASE(ln));
                }
                else
                {
                    oa_move(&clientConnection->settings.objects, &sn.objectList);
                    obj_clear(BASE(sn));
                }
            }
        }
        else
        {
            //Return false, if file doesn't exist.
            ret = DLMS_ERROR_CODE_FALSE;
        }
    }
    return ret;
}

//Get Association view.
int com_getAssociationView(clientConnection* clientConnection, const char* outputFile)
{
    //If association view is already read.
    if (clientConnection->settings.objects.size != 0)
    {
        return 0;
    }
    int ret;
    message data;
    gxReplyData reply;
    if (outputFile != NULL)
    {
        //Load settings.
        ret = com_loadAssociationView(clientConnection, outputFile);
        if (ret != DLMS_ERROR_CODE_FALSE)
        {
            return ret;
        }
    }
    printf("GetAssociationView\r\n");
    mes_init(&data);
    reply_init(&reply);
    /*
    if ((ret = cl_getObjectsRequest(&clientConnection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &data, &reply)) != 0 ||
        (ret = cl_parseObjects(&clientConnection->settings, &reply.data)) != 0)
    {
        printf("GetObjects failed %s\r\n", hlp_getErrorMessage(ret));
    }
    */
    // Parse object one at the time. This can be used if there is a limited amount of the memory available.
    // Only needed object can be created.
    if ((ret = cl_getObjectsRequest(&clientConnection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &data, &reply)) != 0)
    {
        printf("cl_getObjectsRequest failed %s\r\n", hlp_getErrorMessage(ret));
    }
    else
    {
        gxObject* CURRENT_ASSOCIATION[1] = { NULL };
        uint16_t pos, count;
        if ((ret = cl_parseObjectCount(&reply.data, &count)) != 0)
        {
            printf("cl_parseObjectCount failed %s\r\n", hlp_getErrorMessage(ret));
        }
        gxObject obj;
        for (pos = 0; pos != count; ++pos)
        {
            memset(&obj, 0, sizeof(gxObject));
            if ((ret = cl_parseNextObject(&clientConnection->settings, &reply.data, &obj)) != 0)
            {
                break;
            }
            if (clientConnection->settings.useLogicalNameReferencing && obj.objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME && memcmp(obj.logicalName, CURRENT_LN, sizeof(CURRENT_LN)) == 0)
            {
                gxAssociationLogicalName* ln = (gxAssociationLogicalName*)malloc(sizeof(gxAssociationLogicalName));
                INIT_OBJECT((*ln), obj.objectType, obj.logicalName);
                ln->base.shortName = obj.shortName;
                oa_push(&clientConnection->settings.objects, (gxObject*)ln);
                CURRENT_ASSOCIATION[0] = (gxObject*)ln;
            }
            else if (!clientConnection->settings.useLogicalNameReferencing && obj.objectType == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME && memcmp(obj.logicalName, CURRENT_LN, sizeof(CURRENT_LN)) == 0)
            {
                gxAssociationShortName* sn = (gxAssociationShortName*)malloc(sizeof(gxAssociationShortName));
                INIT_OBJECT((*sn), obj.objectType, obj.logicalName);
                sn->base.shortName = obj.shortName;
                oa_push(&clientConnection->settings.objects, (gxObject*)sn);
                CURRENT_ASSOCIATION[0] = (gxObject*)sn;
            }
            else if (obj.objectType == DLMS_OBJECT_TYPE_DATA)
            {
                gxData* data = (gxData*)malloc(sizeof(gxData));
                INIT_OBJECT((*data), obj.objectType, obj.logicalName);
                data->base.shortName = obj.shortName;
                oa_push(&clientConnection->settings.objects, (gxObject*)data);
            }
            else
            {
                gxObject* pObj = NULL;
                if ((ret = cosem_createObject(obj.objectType, &pObj)) == 0 &&
                    (ret = cosem_init2(pObj, obj.objectType, obj.logicalName)) == 0)
                {
                    pObj->shortName = obj.shortName;
                    oa_push(&clientConnection->settings.objects, pObj);
                }
            }
        }
        if (outputFile != NULL && CURRENT_ASSOCIATION[0] != NULL)
        {
            //Write settings.
#if _MSC_VER > 1400
            FILE* f = NULL;
            fopen_s(&f, outputFile, "wb");
#else
            FILE* f = fopen(outputFile, "wb");
#endif
            if (f != NULL)
            {
                gxSerializerSettings serializerSettings;
                ser_init(&serializerSettings);
                serializerSettings.stream = f;
                gxSerializerIgnore NON_SERIALIZED_OBJECTS[] = { IGNORE_ATTRIBUTE_BY_TYPE(DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, GET_ATTRIBUTE_EXCEPT(2)) };
                serializerSettings.ignoredAttributes = NON_SERIALIZED_OBJECTS;
                serializerSettings.count = sizeof(NON_SERIALIZED_OBJECTS) / sizeof(NON_SERIALIZED_OBJECTS[0]);
                if (clientConnection->settings.useLogicalNameReferencing)
                {
                    oa_copy(&((gxAssociationLogicalName*)CURRENT_ASSOCIATION[0])->objectList, &clientConnection->settings.objects);
                }
                else
                {
                    NON_SERIALIZED_OBJECTS[0].objectType = DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME;
                    oa_copy(&((gxAssociationShortName*)CURRENT_ASSOCIATION[0])->objectList, &clientConnection->settings.objects);
                }
                ret = ser_saveObjects(&serializerSettings, CURRENT_ASSOCIATION, sizeof(CURRENT_ASSOCIATION) / sizeof(CURRENT_ASSOCIATION[0]));
                fclose(f);
            }
        }
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}


//Read object.
int com_read(
    clientConnection* connection,
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_read(&connection->settings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_updateValue(&connection->settings, object, attributeOrdinal, &reply.dataValue)) != 0)
    {
        com_reportError("ReadObject failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

//Read object.
int com_read2(
    clientConnection* clientConnection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* value)
{
    int ret;
    gxValueEventArg e;
    message data;
    gxReplyData reply;
    ve_init(&e);
    mes_init(&data);
    reply_init(&reply);
    //Ignore values and get just the bytes.
    reply.ignoreValue = 1;
    var_clear(value);
    if ((ret = cl_read(&clientConnection->settings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &data, &reply)) != 0)
    {
        com_reportError("ReadObject failed", object, attributeOrdinal, ret);
    }
    value->vt = DLMS_DATA_TYPE_OCTET_STRING;
    value->byteArr = (gxByteBuffer*)malloc(sizeof(gxByteBuffer));
    if (value->byteArr == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    value->byteArr->data = reply.data.data;
    value->byteArr->size = reply.data.size;
    value->byteArr->capacity = reply.data.capacity;
    value->byteArr->position = reply.data.position;
    //Clear reply data.
    bb_init(&reply.data);
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_getKeepAlive(
    clientConnection* clientConnection)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_getKeepAlive(&clientConnection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &data, &reply)) != 0)
    {
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_write(
    clientConnection* clientConnection,
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_write(&clientConnection->settings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &data, &reply)) != 0)
    {
        com_reportError("Write failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_method(
    clientConnection* clientConnection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params,
    dlmsVARIANT* value)
{
    int ret;
    message messages;
    gxReplyData reply;
    mes_init(&messages);
    reply_init(&reply);
    if ((ret = cl_method(&clientConnection->settings, object, attributeOrdinal, params, &messages)) != 0 ||
        (ret = com_readDataBlock(clientConnection, &messages, &reply)) != 0)
    {
        printf("Method failed %s\r\n", hlp_getErrorMessage(ret));
    }
    //Copy reply data as a byte array.
    value->vt = DLMS_DATA_TYPE_OCTET_STRING;
    value->byteArr = (gxByteBuffer*)malloc(sizeof(gxByteBuffer));
    if (value->byteArr == NULL)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    value->byteArr->data = reply.data.data;
    value->byteArr->size = reply.data.size;
    value->byteArr->position = reply.data.position;
    value->byteArr->capacity = reply.data.capacity;
    //Add error code before the data.
    bb_move(value->byteArr, 0, 1, value->byteArr->size);
    bb_setUInt8ByIndex(value->byteArr, 0, ret);
    //Clear reply data.
    bb_init(&reply.data);
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}