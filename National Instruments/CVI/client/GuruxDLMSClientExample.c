//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

// Include files

#include <ansi_c.h>
#include <stdlib.h>
#include <assert.h>
#include "Include/getopt.h"
#include "Include/communication.h"
//Windows doesn't implement strcasecmp. It uses strcmpi.
#define strcasecmp strcmp
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

/// HIFN  Explain how to use this command-line tool.
/// HIPAR name/The name of the command-line tool.
static void usage (char *name)
{
	fprintf (stderr, "usage: %s <argument>\n", name);
	fprintf (stderr, "A short summary of the functionality.\n");
	fprintf (stderr, "    <argument>    is an argument\n");
	exit (1);
}

//==============================================================================
// Global variables

//==============================================================================
// Global functions

static void ShowHelp()
{
    printf("GuruxDlmsSample reads data from the DLMS/COSEM device.\n");
    printf("GuruxDlmsSample -h [Meter IP Address] -p [Meter Port No] -c 16 -s 1 -r SN\n");
    printf(" -h \t host name or IP address.\n");
    printf(" -p \t port number or name (Example: 1000).\n");
    printf(" -S \t serial port.\n");
    printf(" -a \t Authentication (None, Low, High, HighMd5, HighSha1, HighGmac, HighSha256).\n");
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
    printf(" -o \t Cache association view to make reading faster. Ex. -o C:\\device.xml");
    printf(" -T \t System title that is used with chiphering. Ex -T 4775727578313233\n");
    printf(" -A \t Authentication key that is used with chiphering. Ex -A D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF\n");
    printf(" -B \t Block cipher key that is used with chiphering. Ex -B 000102030405060708090A0B0C0D0E0F\n");
    printf(" -D \t Dedicated key that is used with chiphering. Ex -D 00112233445566778899AABBCCDDEEFF\n");
    printf(" -i \t Used communication interface. Ex. -i WRAPPER.");
    printf(" -m \t Used PLC MAC address. Ex. -m 1.");
    printf("Example:\n");
    printf("Read LG device using TCP/IP connection.\n");
    printf("GuruxDlmsSample -r SN -c 16 -s 1 -h [Meter IP Address] -p [Meter Port No]\n");
    printf("Read LG device using serial port connection.\n");
    printf("GuruxDlmsSample -r SN -c 16 -s 1 -sp COM1 -i\n");
    printf("Read Indian device using serial port connection.\n");
    printf("GuruxDlmsSample -S COM1 -c 16 -s 1 -a Low -P [password]\n");
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
                sscanf(p2, "%d", &index);
                hlp_setLogicalName(buff, p);
                if ((ret = oa_findByLN(&connection->settings.objects, DLMS_OBJECT_TYPE_NONE, buff, &obj)) == 0)
                {
                    if (obj == NULL)
                    {
                        printf("Object '%s' not found from the association view.\n", p);
                        break;
                    }
                    com_readValue(connection, obj, (unsigned char)index);
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
    char* p, * readObjects = NULL, * outputFile = NULL;
    int index, a, b, c, d, e, f;
    char* invocationCounter = NULL;
    while ((opt = getopt(argc, argv, "h:p:c:s:r:i:It:a:p:wP:g:S:C:v:T:A:B:D:l:F:o:")) != -1)
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
            //Interface
            if (strcasecmp("HDLC", optarg) == 0)
                con.settings.interfaceType = DLMS_INTERFACE_TYPE_HDLC;
            else  if (strcasecmp("WRAPPER", optarg) == 0)
                con.settings.interfaceType = DLMS_INTERFACE_TYPE_WRAPPER;
            else  if (strcasecmp("HdlcModeE", optarg) == 0)
                con.settings.interfaceType = DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E;
            else  if (strcasecmp("Plc", optarg) == 0)
                con.settings.interfaceType = DLMS_INTERFACE_TYPE_PLC;
            else if (strcasecmp("PlcHdlc", optarg) == 0)
                con.settings.interfaceType = DLMS_INTERFACE_TYPE_PLC_HDLC;
            else if (strcasecmp("PlcPrime", optarg) == 0)
                con.settings.interfaceType = DLMS_INTERFACE_TYPE_PLC_PRIME;
            else if (strcasecmp("Pdu", optarg) == 0)
                con.settings.interfaceType = DLMS_INTERFACE_TYPE_PDU;
            else
            {
                printf("Invalid interface option '%s'. (HDLC, WRAPPER, HdlcModeE, Plc, PlcHdlc)", optarg);
                return 1;
            }
            //Update PLC settings.
#ifndef DLMS_IGNORE_PLC
            plc_reset(&con.settings);
#endif //DLMS_IGNORE_PLC
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
            bb_addHexString(&con.settings.cipher.blockCipherKey, optarg);
            if (con.settings.cipher.blockCipherKey.size != 16)
            {
                printf("Invalid block cipher key '%s'.", optarg);
                return 1;
            }
            break;
        case 'D':
            con.settings.cipher.dedicatedKey = (gxByteBuffer*)malloc(sizeof(gxByteBuffer));
            bb_init(con.settings.cipher.dedicatedKey);
            bb_addHexString(con.settings.cipher.dedicatedKey, optarg);
            if (con.settings.cipher.dedicatedKey->size != 16)
            {
                printf("Invalid dedicated key '%s'.", optarg);
                return 1;
            }
            break;
        case 'F':
            con.settings.cipher.invocationCounter = atol(optarg);
            break;
        case 'o':
            outputFile = optarg;
            break;
        case 'v':
            invocationCounter = optarg;
            if ((ret = sscanf(optarg, "%d.%d.%d.%d.%d.%d", &a, &b, &c, &d, &e, &f)) != 6)
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
                if ((ret = sscanf(p, "%d.%d.%d.%d.%d.%d:%d", &a, &b, &c, &d, &e, &f, &index)) != 7)
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
        case 'c':
            con.settings.clientAddress = atoi(optarg);
            break;
        case 's':
            con.settings.serverAddress = atoi(optarg);
            break;
        case 'l':
            con.settings.serverAddress = cl_getServerAddress(atoi(optarg), con.settings.serverAddress, 0);
            break;
        case 'n':
            //TODO: Add support for serial number. con.settings.serverAddress = cl_getServerAddress(atoi(optarg));
            break;
        case 'm':
            con.settings.plcSettings.macDestinationAddress = atoi(optarg);
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
    else
    {
        printf("Missing mandatory connection information for TCP/IP connection.\n");
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
int main (int argc, char *argv[])
{
	int ret = connectMeter(argc, argv);
    if (ret != 0)
    {
        ShowHelp();
    }
	return ret;
}

