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

#include "../include/communication.h"
#include "../dlms/include/gxkey.h"
#include "../dlms/include/converters.h"

//Trace is saved for this buffer.
char hex[2048];
//Received data.
gxByteBuffer frameData;

#include <stdio.h>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64)//Windows includes
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

//Make connection using TCP/IP connection.
int com_makeConnect(clientConnection * connection, const char* address, int port)
{
    int ret;
    struct sockaddr_in add;
    //create socket.
    connection->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (connection->socket == -1)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    add.sin_port = htons(port);
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = inet_addr(address);
    //If address is give as name
    if (add.sin_addr.s_addr == INADDR_NONE)
    {
        struct hostent* hostent = gethostbyname(address);
        if (hostent == NULL)
        {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
            int err = WSAGetLastError();
            closesocket(connection->socket);
#else
            int err = h_errno;
            close(connection->socket);
#endif
            connection->socket = -1;
            return err;
        };
        add.sin_addr = *(struct in_addr*)(void*)hostent->h_addr_list[0];
    };
    //Connect to the meter.
    ret = connect(connection->socket, (struct sockaddr*) & add, sizeof(struct sockaddr_in));
    if (ret == -1)
    {
#if defined(_WIN32) || defined(_WIN64)//Windows includes
        closesocket(connection->socket);
#else
        close(connection->socket);
#endif
        connection->socket = -1;
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
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
                return GetLastError();
            }
            if (!SetCommState(hWnd, DCB))
            {
                return GetLastError();
            }
        }
        else
        {
            //If USB to serial port converters do not implement this.
            if (err != ERROR_INVALID_FUNCTION)
            {
                return err;
            }
        }
    }
    return DLMS_ERROR_CODE_OK;
}
#endif

int com_readSerialPort(
    clientConnection* connection,
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
            return DLMS_ERROR_CODE_SEND_FAILED;
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
                return DLMS_ERROR_CODE_RECEIVE_FAILED;
            }
            //Wait until data is actually read
            if (WaitForSingleObject(connection->osReader.hEvent, connection->waitTime) != WAIT_OBJECT_0)
            {
                ret = GetLastError();
                return DLMS_ERROR_CODE_RECEIVE_FAILED;

            }
            if (!GetOverlappedResult(connection->comPort, &connection->osReader, &bytesRead, TRUE))
            {
                ret = GetLastError();
                return DLMS_ERROR_CODE_RECEIVE_FAILED;
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
            //If connection is closed.
            else if (errno == EBADF)
            {
                return DLMS_ERROR_CODE_RECEIVE_FAILED;
            }
            else
            {
                return DLMS_ERROR_CODE_RECEIVE_FAILED;
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

int com_open(
    clientConnection* connection,
    const char* port,
    unsigned char iec)
{
    unsigned short baudRate;
    int ret, len, pos;
    unsigned char ch;
    //In Linux serial port name might be very long.
    char buff[50];
#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = { 0 };
    unsigned long sendSize = 0;
#if _MSC_VER > 1000
    sprintf_s(buff, 50, "\\\\.\\%s", port);
#else
    sprintf(buff, "\\\\.\\%s", port);
#endif
    if (connection->osReader.hEvent == INVALID_HANDLE_VALUE)
    {
        connection->osReader.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
    }
    if (connection->osWrite.hEvent == INVALID_HANDLE_VALUE)
    {
        connection->osWrite.hEvent = CreateEvent(NULL, 1, FALSE, NULL);
    }

    //Open serial port for read / write. Port can't share.
    connection->comPort = CreateFileA(buff,
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (connection->comPort == INVALID_HANDLE_VALUE)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(connection->comPort, &dcb))
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    dcb.fBinary = 1;
    dcb.fOutX = dcb.fInX = 0;
    //Abort all reads and writes on Error.
    dcb.fAbortOnError = 1;
    if (iec)
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
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
#else //#if defined(__LINUX__)
    struct termios options;
    // read/write | not controlling term | don't wait for DCD line signal.
    connection->comPort = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (connection->comPort == -1) // if open is unsuccessful.
    {
        printf("Failed to Open port: %s\n", port);
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    else
    {
        if (!isatty(connection->comPort))
        {
            printf("Failed to Open port %s. This is not a serial port.\n", port);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }

        memset(&options, 0, sizeof(options));
        options.c_iflag = 0;
        options.c_oflag = 0;
        if (iec)
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
    }
#endif
    if (iec)
    {
#if _MSC_VER > 1000
        strcpy_s(buff, 50, "/?!\r\n");
#else
        strcpy(buff, "/?!\r\n");
#endif
        len = strlen(buff);
        if (connection->trace > GX_TRACE_LEVEL_WARNING)
        {
            printf("\r\nTX:\t");
            for (pos = 0; pos != len; ++pos)
            {
                printf("%.2X ", buff[pos]);
            }
            printf("\r\n");
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
            printf("\r\nRX:\t");
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
            printf("\r\nTX:\t");
            for (pos = 0; pos != len; ++pos)
            {
                printf("%.2X ", buff[pos]);
            }
            printf("\r\n");
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
            printf("GXSetCommState %d\r\n", ret);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        printf("New baudrate %d\r\n", dcb.BaudRate);
        len = 6;
        if ((ret = com_readSerialPort(connection, '\n')) != 0)
        {
            printf("Read %d\r\n", ret);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = NOPARITY;
        if ((ret = com_setCommState(connection->comPort, &dcb)) != 0)
        {
            printf("GXSetCommState %d\r\n", ret);
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
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
    return DLMS_ERROR_CODE_OK;
}

// Read DLMS Data frame from the device.
int readDLMSPacket(
    clientConnection* connection,
    gxByteBuffer* data,
    gxReplyData* reply)
{
    char hex[1000];
#if defined(_WIN32) || defined(_WIN64)//Windows
    unsigned long sendSize = 0;
#endif
    int index = 0, ret = DLMS_ERROR_CODE_OK, cnt;
    if (data->size == 0)
    {
        return DLMS_ERROR_CODE_OK;
    }
    reply->complete = 0;
//    connection->data.size = 0;
//    connection->data.position = 0;
    if (connection->trace > GX_TRACE_LEVEL_INFO)
    {
        bb_toHexString2(data, hex, sizeof(hex));
        printf("TX:\t%s\r\n", hex);
    }
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
                return DLMS_ERROR_CODE_SEND_FAILED;
            }
            //Wait until data is actually sent
            ret = WaitForSingleObject(connection->osWrite.hEvent, connection->waitTime);
            if (ret != 0)
            {
                DWORD err = GetLastError();
                return DLMS_ERROR_CODE_SEND_FAILED;
            }
            //Read bytes in output buffer. Some USB converts require this.
            if (!ClearCommError(connection->comPort, &RecieveErrors, &comstat))
            {
                return DLMS_ERROR_CODE_SEND_FAILED;
            }
        }
#else
        ret = write(connection->comPort, data->data, data->size);
        if (ret != data->size)
        {
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
#endif
    }
    else
    {
        if (send(connection->socket, (const char*)data->data, data->size, 0) == -1)
        {
            //If error has occurred
            printf("DLMS_ERROR_CODE_SEND_FAILED");
            return DLMS_ERROR_CODE_SEND_FAILED;
        }
    }
    //Loop until packet is complete.
    do
    {
        if (connection->comPort != INVALID_HANDLE_VALUE)
        {
            if (com_readSerialPort(connection, 0x7E) != 0)
            {
                return DLMS_ERROR_CODE_SEND_FAILED;
            }
        }
        else
        {
            cnt = connection->data.capacity - connection->data.size;
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
            hlp_bytesToHex2(connection->data.data + index, (unsigned short)(connection->data.size - index), hex, sizeof(hex));
            if (index == 0)
            {
                printf("RX:\t %s", hex);
            }
            else
            {
                printf(" %s", hex);
            }
            index = connection->data.size;
        }
        ret = cl_getData(&connection->settings, &connection->data, reply);
        if (ret != 0 && ret != DLMS_ERROR_CODE_FALSE)
        {
            break;
        }
    } while (reply->complete == 0);
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("\r\n");
    }
    return ret;
}

int com_readDataBlock(
    clientConnection* connection,
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
#ifdef DLMS_IGNORE_MALLOC
    unsigned char data[40];
    bb_attach(&rr, data, 0, sizeof(data));
#else
    bb_init(&rr);
#endif //DLMS_IGNORE_MALLOC
    //Send data.
    for (pos = 0; pos != messages->size; ++pos)
    {
        //Send data.
        if ((ret = readDLMSPacket(connection, messages->data[pos], reply)) != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }

        //Check is there errors or more data from server
        while ((reply->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
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

int com_readNextBlock(
    clientConnection* connection,
    gxReplyData* reply)
{
    gxByteBuffer rr;
    int ret = DLMS_ERROR_CODE_OK;
#ifdef DLMS_IGNORE_MALLOC
    unsigned char data[40];
    bb_attach(&rr, data, 0, sizeof(data));
#else
    bb_init(&rr);
#endif //DLMS_IGNORE_MALLOC
    //Check is there errors or more data from server
    reply->moreData = DLMS_DATA_REQUEST_TYPES_BLOCK;
    do
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
    } while ((reply->moreData & DLMS_DATA_REQUEST_TYPES_FRAME) != 0);
    return ret;
}

//Close connection to the meter.
int com_close(
    clientConnection* connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    gxReplyData reply;
    message msg;
    //If client is closed.
    if (!connection->settings.server)
    {
        reply_init(&reply);
#ifdef DLMS_IGNORE_MALLOC
        gxByteBuffer bb;
        bb_attach(&bb, CLIENT_PDU, 0, sizeof(CLIENT_PDU));
        gxByteBuffer* tmp[] = { &bb };
        mes_attach(&msg, tmp, 1);
        bb_attach(&reply.data, CLIENT_REPLY_PDU, 0, sizeof(CLIENT_REPLY_PDU));

#else
        mes_init(&msg);
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cl_releaseRequest(&connection->settings, &msg)) != 0 ||
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
        memset(&connection->osReader, 0, sizeof(OVERLAPPED));
        memset(&connection->osWrite, 0, sizeof(OVERLAPPED));
        connection->osReader.hEvent = INVALID_HANDLE_VALUE;
        connection->osWrite.hEvent = INVALID_HANDLE_VALUE;
#else
        close(connection->comPort);
#endif
        connection->comPort = INVALID_HANDLE_VALUE;
    }
    cl_clear(&connection->settings);
    return ret;
}

//Initialize connection to the meter.
int com_initializeConnection(
    clientConnection* connection)
{
    int ret = DLMS_ERROR_CODE_OK;
    message messages;
    gxReplyData reply;
    gxByteBuffer bb;
    BB_ATTACH(bb, CLIENT_REPLY_PDU, 0);
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
    if (connection->trace > GX_TRACE_LEVEL_WARNING)
    {
        printf("InitializeConnection\r\n");
    }
    reply_init(&reply);
    reply.data = *connection->settings.serializedPdu;
    bb_clear(&reply.data);
    //Get meter's send and receive buffers size.
    if ((ret = cl_snrmRequest(&connection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
        (ret = cl_parseUAResponse(&connection->settings, &reply.data)) != 0)
    {
        mes_clear(&messages);
        reply_clear(&reply);
        return ret;
    }
    mes_clear(&messages);
    reply_clear(&reply);
    if ((ret = cl_aarqRequest(&connection->settings, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
        (ret = cl_parseAAREResponse(&connection->settings, &reply.data)) != 0)
    {
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
    // Get challenge Is HLS authentication is used.
    if (connection->settings.authentication > DLMS_AUTHENTICATION_LOW)
    {
        if ((ret = cl_getApplicationAssociationRequest(&connection->settings, &messages)) != 0 ||
            (ret = com_readDataBlock(connection, &messages, &reply)) != 0 ||
            (ret = cl_parseApplicationAssociationResponse(&connection->settings, &reply.data)) != 0)
        {
        }
        mes_clear(&messages);
        reply_clear(&reply);
    }
    return ret;
}

//Report error on output;
void com_reportError(const char* description,
    gxObject* object,
    unsigned char attributeOrdinal, int ret)
{
    char ln[25];
    char type[30];
    hlp_getLogicalNameToString(object->logicalName, ln);
    obj_typeToString(object->objectType, type);
    printf("%s %s %s:%d %s\r\n", description, type, ln, attributeOrdinal, hlp_getErrorMessage(ret));
}

//Read object.
int com_read(
    clientConnection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    gxReplyData* reply)
{
    int ret;
    message messages;
#ifdef DLMS_IGNORE_MALLOC
    gxByteBuffer bb;
    BB_ATTACH(bb, CLIENT_REPLY_PDU, 0);
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
#else
    mes_init(&data);
#endif //DLMS_IGNORE_MALLOC
    if ((ret = cl_read(&connection->settings, object, attributeOrdinal, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, reply)) != 0)
    {
        com_reportError("ReadObject failed", object, attributeOrdinal, ret);
    }
    return ret;
}

int com_write(
    clientConnection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* value)
{
    int ret;
    message messages;
    gxReplyData reply;
    reply_init(&reply);
    reply.data = *connection->settings.serializedPdu;
    unsigned char byteArray = value->vt == DLMS_DATA_TYPE_OCTET_STRING || value->vt == DLMS_DATA_TYPE_ARRAY || value->vt == DLMS_DATA_TYPE_STRUCTURE;
#ifdef DLMS_IGNORE_MALLOC
    gxByteBuffer bb;
    BB_ATTACH(bb, CLIENT_REPLY_PDU, 0);
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
#else
    mes_init(&data);
#endif //DLMS_IGNORE_MALLOC
    if ((ret = cl_writeLN(&connection->settings, object->logicalName, object->objectType, attributeOrdinal, value, byteArray, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0)
    {
        com_reportError("Write failed", object, attributeOrdinal, ret);
    }
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}

int com_method(
    clientConnection* connection,
    gxObject* object,
    unsigned char attributeOrdinal,
    dlmsVARIANT* params)
{
    int ret;
    message messages;
    gxReplyData reply;
    reply_init(&reply);
#ifdef DLMS_IGNORE_MALLOC
    unsigned char buff[PDU_BUFFER_SIZE];
    gxByteBuffer bb;
    bb_attach(&bb, buff, 0, sizeof(buff));
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
    unsigned char replydata[PDU_BUFFER_SIZE];
    bb_attach(&reply.data, replydata, 0, sizeof(replydata));
#else
    mes_init(&messages);
#endif //DLMS_IGNORE_MALLOC
    if ((ret = cl_method(&connection->settings, object, attributeOrdinal, params, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, &reply)) != 0)
    {
        printf("Method failed %s\r\n", hlp_getErrorMessage(ret));
    }
    mes_clear(&messages);
    reply_clear(&reply);
    return ret;
}

//Read objects.
int com_readList(
    clientConnection* connection,
    gxArray* list)
{
    int pos, ret = DLMS_ERROR_CODE_OK;
    gxByteBuffer rr;
    message messages;
    gxReplyData reply;
    if (list->size != 0)
    {
        reply_init(&reply);
#ifdef DLMS_IGNORE_MALLOC
        unsigned char buff[PDU_BUFFER_SIZE];
        gxByteBuffer bb;
        bb_attach(&bb, buff, 0, sizeof(buff));
        gxByteBuffer* tmp[] = { &bb };
        mes_attach(&messages, tmp, 1);
        unsigned char replydata[PDU_BUFFER_SIZE];
        bb_attach(&reply.data, replydata, 0, sizeof(replydata));
#else
        mes_init(&messages);
#endif //DLMS_IGNORE_MALLOC
        if ((ret = cl_readList(&connection->settings, list, &messages)) != 0)
        {
            printf("ReadList failed %s\r\n", hlp_getErrorMessage(ret));
        }
        else
        {
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
    clientConnection* connection,
    gxProfileGeneric* object,
    unsigned long index,
    unsigned long count,
    gxReplyData* reply)
{
    int ret;
    message messages;
#ifdef DLMS_IGNORE_MALLOC
    gxByteBuffer bb;
    BB_ATTACH(bb, CLIENT_REPLY_PDU, 0);
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
#else
    mes_init(&data);
#endif //DLMS_IGNORE_MALLOC
    if ((ret = cl_readRowsByEntry(&connection->settings, object, index, count, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, reply)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
int com_readRowsByRange(
    clientConnection* connection,
    gxProfileGeneric* object,
    unsigned long start,
    unsigned long end,
    gxReplyData* reply)
{
    int ret;
    message messages;
#ifdef DLMS_IGNORE_MALLOC
    gxByteBuffer bb;
    BB_ATTACH(bb, CLIENT_REPLY_PDU, 0);
    gxByteBuffer* tmp[] = { &bb };
    mes_attach(&messages, tmp, 1);
#else
    mes_init(&data);
#endif //DLMS_IGNORE_MALLOC
    if ((ret = cl_readRowsByRange(&connection->settings, object, start, end, &messages)) != 0 ||
        (ret = com_readDataBlock(connection, &messages, reply)) != 0)
    {
        printf("ReadObject failed %s\r\n", hlp_getErrorMessage(ret));
    }
    return ret;
}
