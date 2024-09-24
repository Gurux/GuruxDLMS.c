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
#include "../../development/include/gxkey.h"
#include "../../development/include/converters.h"
#include "../../development/include/cosem.h"
#include "../../development/include/gxserializer.h"

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

//Make connection using TCP/IP connection.
int com_makeConnect(connection* connection, const char* address, int port, int waitTime)
{
    int ret;
    //create socket.
    int family = com_isIPv6Address(address) ? AF_INET6 : AF_INET;
    connection->socket = socket(family, SOCK_STREAM, IPPROTO_IP);
    if (connection->socket == -1)
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
                closesocket(connection->socket);
#else
                ret = errno;
                close(connection->socket);
#endif
                connection->socket = -1;
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
            closesocket(connection->socket);
#else
            ret = errno;
            close(connection->socket);
#endif
            connection->socket = -1;
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        };
        add = (struct sockaddr*)&addrIP6;
        addSize = sizeof(struct sockaddr_in6);
    }

    //Set timeout.
#if defined(_WIN32) || defined(_WIN64)//If Windows
    setsockopt(connection->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&waitTime, sizeof(waitTime));
#else
    struct timeval tv;
    tv.tv_sec = waitTime / 1000;
    tv.tv_usec = 0;
    setsockopt(connection->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif //
    //Connect to the meter.
    ret = connect(connection->socket, add, addSize);
    if (ret == -1)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        ret = WSAGetLastError();
        closesocket(connection->socket);
        printf("Connection failed: %d\n", ret);
#else
        ret = errno;
        close(connection->socket);
        printf("Connection failed: %d, %s\n", ret, strerror(ret));
#endif
        connection->socket = -1;
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
    connection* connection,
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
        if (!ClearCommError(connection->comPort, &RecieveErrors, &comstat))
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
        if (!ReadFile(connection->comPort, connection->data.data + connection->data.size, cnt, &bytesRead, &connection->osReader))
        {
            ret = GetLastError();
            if (ret != ERROR_IO_PENDING)
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            }
            //Wait until data is actually read
            if (WaitForSingleObject(connection->osReader.hEvent, connection->waitTime) != WAIT_OBJECT_0)
            {
                return DLMS_ERROR_CODE_RECEIVE_FAILED;
            }
            if (!GetOverlappedResult(connection->comPort, &connection->osReader, &bytesRead, TRUE))
            {
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | GetLastError();
            }
        }
#else
        //Get bytes available.
        ret = ioctl(connection->comPort, FIONREAD, &cnt);
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
        bytesRead = read(connection->comPort, connection->data.data + connection->data.size, cnt);
        if (bytesRead == 0xFFFF)
        {
            //If there is no data on the read buffer.
            if (errno == EAGAIN)
            {
                if (readTime > connection->waitTime)
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
        connection->data.size += (unsigned short)bytesRead;
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
        if (connection->data.size > 5)
        {
            //Some optical strobes can return extra bytes.
            for (pos = connection->data.size - 1; pos != lastReadIndex; --pos)
            {
                if (connection->data.data[pos] == eop)
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
    connection* connection)
{
    unsigned short baudRate;
    int ret = 0, len, pos;
    unsigned char ch;
    //In Linux serial port name might be very long.
    char buff[50];
#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = { 0 };
    unsigned long sendSize = 0;
    if (connection->comPort == INVALID_HANDLE_VALUE)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(connection->comPort, &dcb))
    {
        ret = GetLastError();
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    dcb.fBinary = 1;
    dcb.fOutX = dcb.fInX = 0;
    //Abort all reads and writes on Error.
    dcb.fAbortOnError = 1;
    if (connection->settings.interfaceType == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
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
    if ((ret = com_setCommState(connection->comPort, &dcb)) != 0)
    {
        return ret;
    }
#else
    struct termios options;
    memset(&options, 0, sizeof(options));
    options.c_iflag = 0;
    options.c_oflag = 0;
    if (connection->settings.interfaceType == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
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
    if (tcsetattr(connection->comPort, TCSAFLUSH, &options) != 0)
    {
        printf("Failed to Open port. tcsetattr failed.\r");
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#endif
    if (connection->settings.interfaceType == DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E)
    {
#if _MSC_VER > 1000
        strcpy_s(buff, 50, "/?!\r\n");
#else
        strcpy(buff, "/?!\r\n");
#endif
        len = strlen(buff);
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\nTX:\t");
            for (pos = 0; pos != len; ++pos)
            {
                printf("%.2X ", buff[pos]);
            }
            printf("\n");
        }
#if defined(_WIN32) || defined(_WIN64)
        ret = WriteFile(connection->comPort, buff, len, &sendSize, &connection->osWrite);
        if (ret == 0)
        {
            DWORD err = GetLastError();
            //If error occurs...
            if (err != ERROR_IO_PENDING)
            {
                return DLMS_ERROR_CODE_SEND_FAILED;
            }
            //Wait until data is actually sent
            WaitForSingleObject(connection->osWrite.hEvent, INFINITE);
        }
#else //#if defined(__LINUX__)
        ret = write(connection->comPort, buff, len);
        if (ret != len)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
#endif
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\nRX:\t");
        }
        //Read reply data.
        if (com_readSerialPort(connection, '\n') != 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\r\n");
        }
        if (bb_getUInt8(&connection->data, &ch) != 0 || ch != '/')
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
        //Get used baud rate.
        if ((ret = bb_getUInt8ByIndex(&connection->data, 4, &ch)) != 0)
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
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\nTX:\t");
            for (pos = 0; pos != len; ++pos)
            {
                printf("%.2X ", buff[pos]);
            }
            printf("\n");
        }
#if defined(_WIN32) || defined(_WIN64)//Windows
        ret = WriteFile(connection->comPort, buff, len, &sendSize, &connection->osWrite);
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
            WaitForSingleObject(connection->osWrite.hEvent, INFINITE);
        }
#else //#if defined(__LINUX__)
        ret = write(connection->comPort, buff, len);
        if (ret != len)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
#endif
#if defined(_WIN32) || defined(_WIN64)//Windows
        //This sleep is in standard. Do not remove.
        Sleep(1000);
        dcb.BaudRate = baudRate;
        if ((ret = com_setCommState(connection->comPort, &dcb)) != 0)
        {
            return ret;
        }
        printf("New baudrate %d\r\n", dcb.BaudRate);
        len = 6;
        if ((ret = com_readSerialPort(connection, '\n')) != 0)
        {
            printf("Read %d\r\n", ret);
            return ret;
        }
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = NOPARITY;
        if ((ret = com_setCommState(connection->comPort, &dcb)) != 0)
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
        if (tcsetattr(connection->comPort, TCSAFLUSH, &options) != 0)
        {
            printf("Failed to Open port. tcsetattr failed.\r");
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#endif
    }
    return ret;
}

int com_open(
    connection* connection,
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
    //Open serial port for read / write. Port can't share.
    connection->comPort = CreateFileA(buff,
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (connection->comPort == INVALID_HANDLE_VALUE)
    {
        ret = GetLastError();
        printf("Failed to open serial port: \"%s\"\n", buff);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
#else //#if defined(__LINUX__)
    // read/write | not controlling term | don't wait for DCD line signal.
    connection->comPort = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (connection->comPort == -1) // if open is unsuccessful.
    {
        ret = errno;
        printf("Failed to open serial port: %s\n", port);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
    if (!isatty(connection->comPort))
    {
        ret = errno;
        printf("Failed to Open port %s. This is not a serial port.\n", port);
        return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
    }
#endif
    return com_initializeOpticalHead(connection);
}

int sendData(connection* connection, gxByteBuffer* data)
{
    int ret = 0;
#if defined(_WIN32) || defined(_WIN64)//Windows
    unsigned long sendSize = 0;
#endif
    if (connection->comPort != INVALID_HANDLE_VALUE)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows
        ret = WriteFile(connection->comPort, data->data, data->size, &sendSize, &connection->osWrite);
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
            ret = WaitForSingleObject(connection->osWrite.hEvent, connection->waitTime);
            if (ret != 0)
            {
                ret = GetLastError();
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            }
            //Read bytes in output buffer. Some USB converts require this.
            if (!ClearCommError(connection->comPort, &RecieveErrors, &comstat))
            {
                ret = GetLastError();
                return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
            }
        }
#else
        ret = write(connection->comPort, data->data, data->size);
        if (ret != data->size)
        {
            ret = errno;
            return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | ret;
        }
#endif
    }
    else
    {
        if (send(connection->socket, (const char*)data->data, data->size, 0) == -1)
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

int readData(connection* connection, gxByteBuffer* data, int* index)
{
    int ret = 0;
    if (connection->comPort != INVALID_HANDLE_VALUE)
    {
        if ((ret = com_readSerialPort(connection, 0x7E)) != 0)
        {
            return ret;
        }
    }
    else
    {
        uint32_t cnt = connection->data.capacity - connection->data.size;
        if (cnt < 1)
        {
            return DLMS_ERROR_CODE_OUTOFMEMORY;
        }
        if ((ret = recv(connection->socket, (char*)connection->data.data + connection->data.size, cnt, 0)) == -1)
        {
            return DLMS_ERROR_CODE_RECEIVE_FAILED;
        }
        connection->data.size += ret;
    }
    if (connection->trace > GX_TRACE_LEVEL_INFO)
    {
        char* hex = hlp_bytesToHex(connection->data.data + *index, connection->data.size - *index);
        if (*index == 0)
        {
            printf("\nRX:\t %s", hex);
        }
        else
        {
            printf(" %s", hex);
        }
        free(hex);
        *index = connection->data.size;
    }
    return 0;
}

// Read DLMS Data frame from the device.
int readDLMSPacket(
    connection* connection,
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
    connection->data.size = 0;
    connection->data.position = 0;
    if (connection->trace == GX_TRACE_LEVEL_VERBOSE)
    {
        hex = bb_toHexString(data);
        printf("\nTX:\t%s\n", hex);
        free(hex);
    }
    if ((ret = sendData(connection, data)) != 0)
    {
        return ret;
    }
    //Loop until packet is complete.
    unsigned char pos = 0;
    unsigned char isNotify;
    gxReplyData notify;
    reply_init(&notify);
    do
    {
        if ((ret = readData(connection, &connection->data, &index)) != 0)
        {
            if (ret != DLMS_ERROR_CODE_RECEIVE_FAILED || pos == 3)
            {
                break;
            }
            ++pos;
            printf("\nData send failed. Try to resend %d/3\n", pos);
            if ((ret = sendData(connection, data)) != 0)
            {
                break;
            }
        }
        else
        {
            ret = cl_getData2(&connection->settings, &connection->data, reply, &notify, &isNotify);
            if (ret != 0 && ret != DLMS_ERROR_CODE_FALSE)
            {
                break;
            }
            if (isNotify)
            {
                gxByteBuffer bb;
                bb_init(&bb);
                var_toString(&notify.dataValue, &bb);
                char* tmp = bb_toString(&bb);
                printf("Notification received: %s", tmp);
                free(tmp);
                bb_clear(&bb);
            }
        }
    } while (reply->complete == 0);
    reply_clear(&notify);
    if (connection->trace == GX_TRACE_LEVEL_VERBOSE)
    {
        printf("\n");
    }
    return ret;
}

int com_readDataBlock(
    connection* connection,
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
        if ((ret = readDLMSPacket(connection, messages->data[pos], reply)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        //Check is there errors or more data from server
        while (reply_isMoreData(reply))
        {
            if ((ret = cl_receiverReady(&connection->settings, reply->moreData, &rr)) != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&rr);
                return ret;
            }
            if ((ret = readDLMSPacket(connection, &rr, reply)) != DLMS_ERROR_CODE_OK)
            {
                bb_clear(&rr);
                return ret;
            }
            bb_clear(&rr);
        }
    }
    return ret;
}


//Close connection to the meter.
int com_disconnect(
    connection* connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    reply_init(&reply);
    mes_init(&msg);
    if ((ret = cl_disconnectRequest(&connection->settings, &msg)) != 0 ||
        (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
    {
        //Show error but continue close.
        printf("Close failed.");
    }
    reply_clear(&reply);
    mes_clear(&msg);
    return ret;
}

//Close connection to the meter.
int com_close(
    connection* connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    //If client is closed.
    if (!connection->settings.server)
    {
        reply_init(&reply);
        mes_init(&msg);
        if ((ret = cl_releaseRequest2(&connection->settings, &msg,
            connection->settings.cipher.security != DLMS_SECURITY_NONE)) != 0 ||
            (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
        {
            //Show error but continue close.
            printf("Release failed.");
        }
        reply_clear(&reply);
        mes_clear(&msg);

        if ((ret = cl_disconnectRequest(&connection->settings, &msg)) != 0 ||
            (ret = com_readDataBlock(connection, &msg, &reply)) != 0)
        {
            //Show error but continue close.
            printf("Close failed.");
        }
        reply_clear(&reply);
        mes_clear(&msg);
    }
    if (connection->socket != -1)
    {
        connection->closing = 1;
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        closesocket(connection->socket);
#else
        close(connection->socket);
#endif
        connection->socket = -1;
    }
    else if (connection->comPort != INVALID_HANDLE_VALUE)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        CloseHandle(connection->comPort);
        connection->comPort = INVALID_HANDLE_VALUE;
        CloseHandle(connection->osReader.hEvent);
        CloseHandle(connection->osWrite.hEvent);
#else
        close(connection->comPort);
#endif
        connection->comPort = INVALID_HANDLE_VALUE;
    }
    cl_clear(&connection->settings);
    return ret;
}

//Read Invocation counter (frame counter) from the meter and update it.
int com_updateInvocationCounter(
    connection* connection,
    const char* invocationCounter)
{
    int ret = DLMS_ERROR_CODE_OK;
    //Read frame counter if GeneralProtection is used.
    if (invocationCounter != NULL && connection->settings.cipher.security != DLMS_SECURITY_NONE)
    {
        message messages;
        gxReplyData reply;
        unsigned short add = connection->settings.clientAddress;
        DLMS_AUTHENTICATION auth = connection->settings.authentication;
        DLMS_SECURITY security = connection->settings.cipher.security;
        gxByteBuffer* preEstablishedSystemTitle = connection->settings.preEstablishedSystemTitle;
        gxByteBuffer challenge;
        bb_init(&challenge);
        bb_set(&challenge, connection->settings.ctoSChallenge.data, connection->settings.ctoSChallenge.size);
        connection->settings.clientAddress = 16;
        connection->settings.preEstablishedSystemTitle = NULL;
        connection->settings.authentication = DLMS_AUTHENTICATION_NONE;
        connection->settings.cipher.security = DLMS_SECURITY_NONE;
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("updateInvocationCounter\r\n");
        }
        mes_init(&messages);
        reply_init(&reply);
        //Get meter's send and receive buffers size.
        if ((ret = cl_snrmRequest(&connection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
            (ret = cl_parseUAResponse(&connection->settings, &reply.data)) != 0)
        {
            bb_clear(&challenge);
            mes_clear(&messages);
            reply_clear(&reply);
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("SNRMRequest failed %s\r\n", hlp_getErrorMessage(ret));
            }
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
        if ((ret = cl_aarqRequest(&connection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
            (ret = cl_parseAAREResponse(&connection->settings, &reply.data)) != 0)
        {
            bb_clear(&challenge);
            mes_clear(&messages);
            reply_clear(&reply);
            if (ret == DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
            {
                if (connection->trace > GX_TRACE_LEVEL_OFF)
                {
                    printf("Use Logical Name referencing is wrong. Change it!\r\n");
                }
                return ret;
            }
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("AARQRequest failed %s\r\n", hlp_getErrorMessage(ret));
            }
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
        if (connection->settings.maxPduSize == 0xFFFF)
        {
            con_initializeBuffers(connection, connection->settings.maxPduSize);
        }
        else
        {
            //Allocate 50 bytes more because some meters count this wrong and send few bytes too many.
            con_initializeBuffers(connection, 50 + connection->settings.maxPduSize);
        }
        gxData d;
        cosem_init(BASE(d), DLMS_OBJECT_TYPE_DATA, invocationCounter);
        if ((ret = com_read(connection, BASE(d), 2)) == 0)
        {
            connection->settings.cipher.invocationCounter = 1 + var_toInteger(&d.value);
            if (connection->trace > GX_TRACE_LEVEL_WARNING)
            {
                printf("Invocation counter: %u (0x%X)\r\n",
                    connection->settings.cipher.invocationCounter,
                    connection->settings.cipher.invocationCounter);
            }
            //It's OK if this fails.
            com_disconnect(connection);
            connection->settings.clientAddress = add;
            connection->settings.authentication = auth;
            connection->settings.cipher.security = security;
            bb_clear(&connection->settings.ctoSChallenge);
            bb_set(&connection->settings.ctoSChallenge, challenge.data, challenge.size);
            bb_clear(&challenge);
            connection->settings.preEstablishedSystemTitle = preEstablishedSystemTitle;
            if (dlms_usePreEstablishedConnection(&connection->settings))
            {
                connection->settings.negotiatedConformance |= DLMS_CONFORMANCE_GENERAL_PROTECTION;
            }
        }
    }
    return ret;
}

//Initialize connection to the meter.
int com_initializeConnection(
    connection* connection)
{
    message messages;
    gxReplyData reply;
    int ret = 0;
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("InitializeConnection\r\n");
    }

    mes_init(&messages);
    reply_init(&reply);
    //Get meter's send and receive buffers size.
    if ((ret = cl_snrmRequest(&connection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(&connection->settings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        if (connection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("SNRMRequest failed %s\r\n", hlp_getErrorMessage(ret));
        }
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    if (connection->settings.preEstablishedSystemTitle == NULL)
    {
        if ((ret = cl_aarqRequest(&connection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
            (ret = cl_parseAAREResponse(&connection->settings, &reply.data)) != 0)
        {
            if (ret == DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED)
            {
                if (connection->trace > GX_TRACE_LEVEL_OFF)
                {
                    printf("Use Logical Name referencing is wrong. Change it!\r\n");
                }
            }
            else if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                if (ret == (DLMS_ERROR_TYPE_EXCEPTION_RESPONSE | DLMS_EXCEPTION_SERVICE_ERROR_INVOCATION_COUNTER_ERROR))
                {
                    //If invocation counter value is too low.
                    //Get invocation counter value.
                    uint32_t value = 0;
                    bb_getUInt32(&reply.data, &value);
                    printf("Connection failed. Expected invocation counter value: %u \r\n", value);
                }
                else
                {
                    printf("AARQRequest failed %s\r\n", hlp_getErrorMessage(ret));
                }
            }
            mes_clear(&messages);
            reply_clear(&reply);
            return ret;
        }
        mes_clear(&messages);
        reply_clear(&reply);
        if (connection->settings.maxPduSize == 0xFFFF)
        {
            con_initializeBuffers(connection, connection->settings.maxPduSize);
        }
        else
        {
            //Allocate 50 bytes more because some meters count this wrong and send few bytes too many.
            con_initializeBuffers(connection, 50 + connection->settings.maxPduSize);
        }

        // Get challenge Is HLS authentication is used.
        if (connection->settings.authentication > DLMS_AUTHENTICATION_LOW)
        {
            if ((ret = cl_getApplicationAssociationRequest(&connection->settings, &messages)) != 0 ||
                (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
                (ret = cl_parseApplicationAssociationResponse(&connection->settings, &reply.data)) != 0)
            {
                mes_clear(&messages);
                reply_clear(&reply);
                return ret;
            }
            mes_clear(&messages);
            reply_clear(&reply);
        }
    }
    else
    {
        //Allocate buffers for pre-established connection.
        con_initializeBuffers(connection, connection->settings.maxPduSize);
    }
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
int com_loadAssociationView(connection* connection, const char* outputFile)
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
            if (connection->settings.useLogicalNameReferencing)
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
            oa_clear(&connection->settings.objects, 1);
            ret = ser_loadObjects(&connection->settings, &serializerSettings, CURRENT_ASSOCIATION, sizeof(CURRENT_ASSOCIATION) / sizeof(CURRENT_ASSOCIATION[0]));
            fclose(f);
            if (ret == 0)
            {
                if (connection->settings.useLogicalNameReferencing)
                {
                    oa_move(&connection->settings.objects, &ln.objectList);
                    obj_clear(BASE(ln));
                }
                else
                {
                    oa_move(&connection->settings.objects, &sn.objectList);
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
int com_getAssociationView(connection* connection, const char* outputFile)
{
    //If association view is already read.
    if (connection->settings.objects.size != 0)
    {
        return 0;
    }
    int ret;
    message data;
    gxReplyData reply;
    if (outputFile != NULL)
    {
        //Load settings.
        ret = com_loadAssociationView(connection, outputFile);
        if (ret != DLMS_ERROR_CODE_FALSE)
        {
            return ret;
        }
    }
    printf("GetAssociationView\r\n");
    mes_init(&data);
    reply_init(&reply);
    /*
    if ((ret = cl_getObjectsRequest(&connection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_parseObjects(&connection->settings, &reply.data)) != 0)
    {
        printf("GetObjects failed %s\r\n", hlp_getErrorMessage(ret));
    }
    */
    // Parse object one at the time. This can be used if there is a limited amount of the memory available.
    // Only needed object can be created.
    if ((ret = cl_getObjectsRequest(&connection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0)
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
            if ((ret = cl_parseNextObject(&connection->settings, &reply.data, &obj)) != 0)
            {
                break;
            }
            if (connection->settings.useLogicalNameReferencing && obj.objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME && memcmp(obj.logicalName, CURRENT_LN, sizeof(CURRENT_LN)) == 0)
            {
                gxAssociationLogicalName* ln = (gxAssociationLogicalName*)malloc(sizeof(gxAssociationLogicalName));
                INIT_OBJECT((*ln), obj.objectType, obj.logicalName);
                ln->base.shortName = obj.shortName;
                oa_push(&connection->settings.objects, (gxObject*)ln);
                CURRENT_ASSOCIATION[0] = (gxObject*)ln;
            }
            else if (!connection->settings.useLogicalNameReferencing && obj.objectType == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME && memcmp(obj.logicalName, CURRENT_LN, sizeof(CURRENT_LN)) == 0)
            {
                gxAssociationShortName* sn = (gxAssociationShortName*)malloc(sizeof(gxAssociationShortName));
                INIT_OBJECT((*sn), obj.objectType, obj.logicalName);
                sn->base.shortName = obj.shortName;
                oa_push(&connection->settings.objects, (gxObject*)sn);
                CURRENT_ASSOCIATION[0] = (gxObject*)sn;
            }
            else if (obj.objectType == DLMS_OBJECT_TYPE_DATA)
            {
                gxData* data = (gxData*)malloc(sizeof(gxData));
                INIT_OBJECT((*data), obj.objectType, obj.logicalName);
                data->base.shortName = obj.shortName;
                oa_push(&connection->settings.objects, (gxObject*)data);
            }
            else
            {
                gxObject* pObj = NULL;
                if ((ret = cosem_createObject(obj.objectType, &pObj)) == 0 &&
                    (ret = cosem_init2(pObj, obj.objectType, obj.logicalName)) == 0)
                {
                    pObj->shortName = obj.shortName;
                    pObj->version = obj.version;
                    oa_push(&connection->settings.objects, pObj);
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
                if (connection->settings.useLogicalNameReferencing)
                {
                    oa_copy(&((gxAssociationLogicalName*)CURRENT_ASSOCIATION[0])->objectList, &connection->settings.objects);
                }
                else
                {
                    NON_SERIALIZED_OBJECTS[0].objectType = DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME;
                    oa_copy(&((gxAssociationShortName*)CURRENT_ASSOCIATION[0])->objectList, &connection->settings.objects);
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
    connection* connection,
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

int com_getKeepAlive(
    connection* connection)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_getKeepAlive(&connection->settings, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0)
    {
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_write(
    connection* connection,
    gxObject* object,
    unsigned char attributeOrdinal)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_write(&connection->settings, object, attributeOrdinal, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0)
    {
        com_reportError("Write failed", object, attributeOrdinal, ret);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_writeList(
    connection* connection,
    gxArray* list)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxByteBuffer bb, rr;
    message messages;
    gxReplyData reply;
    if (list->size != 0)
    {
        mes_init(&messages);
        if ((ret = cl_writeList(&connection->settings, list, &messages)) != 0)
        {
            printf("writeList failed %s\r\n", hlp_getErrorMessage(ret));
        }
        else
        {
            reply_init(&reply);
            bb_init(&rr);
            bb_init(&bb);
            //Send data.
            for (pos = 0; pos != messages.size; ++pos)
            {
                //Send data.
                reply_clear(&reply);
                if ((ret = readDLMSPacket(connection, messages.data[pos], &reply)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                //Check is there errors or more data from server
                while (reply_isMoreData(&reply))
                {
                    //NOTE! Set ignore value to true because list is parsed differently than normal read.
                    reply.ignoreValue = 1;
                    if ((ret = cl_receiverReady(&connection->settings, reply.moreData, &rr)) != DLMS_ERROR_CODE_OK ||
                        (ret = readDLMSPacket(connection, &rr, &reply)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    bb_clear(&rr);
                }
                bb_set2(&bb, &reply.data, reply.data.position, -1);
            }
            bb_clear(&bb);
            bb_clear(&rr);
            reply_clear(&reply);
        }
        mes_clear(&messages);
    }
    return ret;
}

int com_method(
    connection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params)
{
    int ret;
    message messages;
    gxReplyData reply;
    mes_init(&messages);
    reply_init(&reply);
    if ((ret = cl_method(&connection->settings, object, attributeOrdinal, params, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0)
    {
        printf("Method failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}

int com_method2(
    connection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    unsigned char* value,
    uint32_t length)
{
    int ret;
    message messages;
    gxReplyData reply;
    mes_init(&messages);
    reply_init(&reply);
    if ((ret = cl_method2(&connection->settings, object, attributeOrdinal, value, length, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0)
    {
        printf("Method failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}


int com_method3(
    connection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    gxByteBuffer* value)
{
    return com_method2(connection, object, attributeOrdinal, value->data, value->size);
}

int com_updateHighLevelPassword(
    connection* connection,
    gxAssociationLogicalName* object)
{
    int ret;
    dlmsVARIANT tmp;
    var_init(&tmp);
    tmp.byteArr = &object->secret;
    if (tmp.byteArr == NULL || tmp.byteArr->size == 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    //Secret is not copied and for that reason position is set to zero.
    tmp.byteArr->position = 0;
    tmp.vt = DLMS_DATA_TYPE_OCTET_STRING;
    ret = com_method(connection, &object->base, 1, &tmp);
    tmp.byteArr->position = 0;
    var_clear(&tmp);
    return ret;
}

//Read objects.
int com_readList(
    connection* connection,
    gxArray* list)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxByteBuffer bb, rr;
    message messages;
    gxReplyData reply;
    if (list->size != 0)
    {
        mes_init(&messages);
        if ((ret = cl_readList(&connection->settings, list, &messages)) != 0)
        {
            printf("ReadList failed %s\r\n", hlp_getErrorMessage(ret));
        }
        else
        {
            reply_init(&reply);
            //NOTE! Set ignore value to true because list is parsed differently than normal read.
            reply.ignoreValue = 1;
            bb_init(&rr);
            bb_init(&bb);
            //Send data.
            for (pos = 0; pos != messages.size; ++pos)
            {
                //Send data.
                reply_clear(&reply);
                if ((ret = readDLMSPacket(connection, messages.data[pos], &reply)) != DLMS_ERROR_CODE_OK)
                {
                    break;
                }
                //Check is there errors or more data from server
                while (reply_isMoreData(&reply))
                {
                    //NOTE! Set ignore value to true because list is parsed differently than normal read.
                    reply.ignoreValue = 1;
                    if ((ret = cl_receiverReady(&connection->settings, reply.moreData, &rr)) != DLMS_ERROR_CODE_OK ||
                        (ret = readDLMSPacket(connection, &rr, &reply)) != DLMS_ERROR_CODE_OK)
                    {
                        break;
                    }
                    bb_clear(&rr);
                }
                bb_set2(&bb, &reply.data, reply.data.position, -1);
            }
            if (ret == 0)
            {
                ret = cl_updateValues(&connection->settings, list, &bb);
            }
            bb_clear(&bb);
            bb_clear(&rr);
            reply_clear(&reply);
        }
        mes_clear(&messages);
    }
    return ret;
}

int com_readRowsByEntry(
    connection* connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByEntry(&connection->settings, object, index, count, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_updateValue(&connection->settings, (gxObject*)object, 2, &reply.dataValue)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

int com_readRowsByEntry2(
    connection* connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count,
    unsigned short colStartIndex,
    unsigned short colEndIndex)
{
    int ret;
    message data;
    gxReplyData reply;
    gxByteBuffer bb;
    char* str;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByEntry2(&connection->settings, object, index, count, colStartIndex, colEndIndex, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    else
    {
        bb_init(&bb);
        var_toString(&reply.dataValue, &bb);
        str = bb_toString(&bb);
        printf(str);
        bb_clear(&bb);
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
int com_readRowsByRange(
    connection* connection,
    gxProfileGeneric* object,
    struct tm* start,
    struct tm* end)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByRange(&connection->settings, object, start, end, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_updateValue(&connection->settings, (gxObject*)object, 2, &reply.dataValue)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
int com_readRowsByRange2(
    connection* connection,
    gxProfileGeneric* object,
    gxtime* start,
    gxtime* end)
{
    int ret;
    message data;
    gxReplyData reply;
    mes_init(&data);
    reply_init(&reply);
    if ((ret = cl_readRowsByRange2(&connection->settings, object, start, end, &data)) != 0 ||
        (ret = com_readDataBlock(connection, &data, &reply)) != 0 ||
        (ret = cl_updateValue(&connection->settings, (gxObject*)object, 2, &reply.dataValue)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&data);
    reply_clear(&reply);
    return ret;
}


///////////////////////////////////////////////////////////////////////////////////
//Read scalers and units. They are static so they are read only once.
int com_readScalerAndUnits(
    connection* connection)
{
    gxObject* obj;
    int ret, pos;
    objectArray objects;
    gxArray list;
    gxObject* object;
    DLMS_OBJECT_TYPE types[] = { DLMS_OBJECT_TYPE_EXTENDED_REGISTER, DLMS_OBJECT_TYPE_REGISTER, DLMS_OBJECT_TYPE_DEMAND_REGISTER };
    oa_init(&objects);
    //Find registers and demand registers and read them.
    ret = oa_getObjects2(&connection->settings.objects, types, 3, &objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    if ((connection->settings.negotiatedConformance & DLMS_CONFORMANCE_MULTIPLE_REFERENCES) != 0)
    {
        arr_init(&list);
        //Try to read with list first. All meters do not support it.
        for (pos = 0; pos != connection->settings.objects.size; ++pos)
        {
            ret = oa_getByIndex(&connection->settings.objects, pos, &obj);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                oa_empty(&objects);
                arr_clear(&list);
                return ret;
            }
            if (obj->objectType == DLMS_OBJECT_TYPE_REGISTER ||
                obj->objectType == DLMS_OBJECT_TYPE_EXTENDED_REGISTER)
            {
                arr_push(&list, key_init(obj, (void*)3));
            }
            else if (obj->objectType == DLMS_OBJECT_TYPE_DEMAND_REGISTER)
            {
                arr_push(&list, key_init(obj, (void*)4));
            }
        }
        ret = com_readList(connection, &list);
        arr_clear(&list);
    }
    //If read list failed read items one by one.
    if (ret != 0)
    {
        for (pos = 0; pos != objects.size; ++pos)
        {
            ret = oa_getByIndex(&objects, pos, &object);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                oa_empty(&objects);
                return ret;
            }
            ret = com_read(connection, object, object->objectType == DLMS_OBJECT_TYPE_DEMAND_REGISTER ? 4 : 3);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                oa_empty(&objects);
                return ret;
            }
        }
    }
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
//Read profile generic columns. They are static so they are read only once.
int com_readProfileGenericColumns(
    connection* connection)
{
    int ret, pos;
    objectArray objects;
    gxObject* object;
    oa_init(&objects);
    ret = oa_getObjects(&connection->settings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        oa_empty(&objects);
        return ret;
    }
    for (pos = 0; pos != objects.size; ++pos)
    {
        ret = oa_getByIndex(&objects, pos, &object);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ret = com_read(connection, object, 3);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
    }
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
//Read profile generics rows.
int com_readProfileGenerics(
    connection* connection)
{
    gxtime startTime, endTime;
    int ret, pos;
    char str[50];
    char ln[25];
    char* data = NULL;
    gxByteBuffer ba;
    objectArray objects;
    gxProfileGeneric* pg;
    oa_init(&objects);
    ret = oa_getObjects(&connection->settings.objects, DLMS_OBJECT_TYPE_PROFILE_GENERIC, &objects);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        //Do not clear objects list because it will free also objects from association view list.
        oa_empty(&objects);
        return ret;
    }
    bb_init(&ba);
    for (pos = 0; pos != objects.size; ++pos)
    {
        ret = oa_getByIndex(&objects, pos, (gxObject**)&pg);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        //Read entries in use.
        ret = com_read(connection, (gxObject*)pg, 7);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 7);
            }
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        //Read entries.
        ret = com_read(connection, (gxObject*)pg, 8);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 8);
            }
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("Entries: %d/%d\r\n", pg->entriesInUse, pg->profileEntries);
        }
        //If there are no columns or rows.
        if (pg->entriesInUse == 0 || pg->captureObjects.size == 0)
        {
            continue;
        }
        //Read capture period.
        ret = com_read(connection, (gxObject*)pg, 4);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 8);
            }
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        //Read Sort method.
        ret = com_read(connection, (gxObject*)pg, 5);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s attribute index %d\r\n", str, ln, 8);
            }
            //Do not clear objects list because it will free also objects from association view list.
            oa_empty(&objects);
            return ret;
        }
        //Read first row from Profile Generic.
        ret = com_readRowsByEntry(connection, pg, 1, 1);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s rows by entry\r\n", str, ln);
            }
        }
        else
        {
            if (connection->trace > GX_TRACE_LEVEL_WARNING)
            {
                bb_init(&ba);
                obj_rowsToString(&ba, &pg->buffer);
                data = bb_toString(&ba);
                bb_clear(&ba);
                printf("%s\r\n", data);
                free(data);
            }
        }
        //Read last day from Profile Generic.
        time_now(&startTime);
        endTime = startTime;
        time_clearTime(&startTime);
        ret = com_readRowsByRange2(connection, pg, &startTime, &endTime);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            if (connection->trace > GX_TRACE_LEVEL_OFF)
            {
                printf("Failed to read object %s %s rows by entry\r\n", str, ln);
            }
        }
        else
        {
            if (connection->trace > GX_TRACE_LEVEL_WARNING)
            {
                bb_init(&ba);
                obj_rowsToString(&ba, &pg->buffer);
                data = bb_toString(&ba);
                bb_clear(&ba);
                printf("%s\r\n", data);
                free(data);
            }
        }
    }
    //Do not clear objects list because it will free also objects from association view list.
    oa_empty(&objects);
    return ret;
}

int com_readValue(connection* connection, gxObject* object, unsigned char index)
{
    int ret;
    char* data = NULL;
    char ln[25];
    ret = hlp_getLogicalNameToString(object->logicalName, ln);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("-------- Reading Object %s %s\r\n", obj_typeToString2(object->objectType), ln);
    }
    ret = com_read(connection, object, index);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        if (connection->trace > GX_TRACE_LEVEL_OFF)
        {
            printf("Failed to read object %s %s attribute index %d\r\n", obj_typeToString2(object->objectType), ln, index);
        }
        //Return error if not DLMS error.
        if (ret != DLMS_ERROR_CODE_READ_WRITE_DENIED)
        {
            return ret;
        }
    }
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        ret = obj_toString(object, &data);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        if (data != NULL)
        {
            printf("%s", data);
            free(data);
            data = NULL;
        }
    }
    return 0;
}

// This function reads ALL objects that meter have excluded profile generic objects.
// It will loop all object's attributes.
int com_readValues(connection* connection)
{
    gxByteBuffer attributes;
    unsigned char ch;
    char* data = NULL;
    char ln[25];
    gxObject* object;
    unsigned long index;
    int ret = 0, pos;
    bb_init(&attributes);
    for (pos = 0; pos != connection->settings.objects.size; ++pos)
    {
        ret = oa_getByIndex(&connection->settings.objects, pos, &object);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        ///////////////////////////////////////////////////////////////////////////////////
        // Profile generics are read later because they are special cases.
        // (There might be so lots of data and we so not want waste time to read all the data.)
        if (object->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
        {
            continue;
        }
        ret = hlp_getLogicalNameToString(object->logicalName, ln);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("-------- Reading Object %s %s\n", obj_typeToString2(object->objectType), ln);
        }
        ret = obj_getAttributeIndexToRead(object, &attributes);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            break;
        }
        for (index = 0; index < attributes.size; ++index)
        {
            ret = bb_getUInt8ByIndex(&attributes, index, &ch);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            ret = com_read(connection, object, ch);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                if (connection->trace > GX_TRACE_LEVEL_OFF)
                {
                    printf("Failed to read object %s %s attribute index %d\r\n", obj_typeToString2(object->objectType), ln, ch);
                }
                //Return error if not DLMS error.
                if (ret != DLMS_ERROR_CODE_READ_WRITE_DENIED)
                {
                    continue;
                }
                ret = 0;
            }
        }
        if (ret != 0)
        {
            break;
        }
        bb_clear(&attributes);
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            ret = obj_toString(object, &data);
            if (ret != DLMS_ERROR_CODE_OK)
            {
                break;
            }
            if (data != NULL)
            {
                printf("%s", data);
                free(data);
                data = NULL;
            }
        }
    }
    bb_clear(&attributes);
    return ret;
}

int com_readAllObjects(connection* connection, const char* outputFile)
{
    //Get objects from the meter and read them.
    int ret = com_getAssociationView(connection, outputFile);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readScalerAndUnits(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ///////////////////////////////////////////////////////////////////////////////////
    //Read Profile Generic columns.
    ret = com_readProfileGenericColumns(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readValues(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    ret = com_readProfileGenerics(connection);
    if (ret != DLMS_ERROR_CODE_OK)
    {
        return ret;
    }
    return ret;
}