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
#include <atmel_start.h>
#include "dlms\include\cosem.h"
//#include "dlms\connection.h"
#include "dlms\communication.h"

long time_current(void)
{
	return (long)time(NULL);
}

long time_elapsed(void)
{
	return 0;
}

void time_now(
gxtime* value)
{
}

unsigned char svr_isTarget(
	dlmsSettings *settings,
	unsigned long serverAddress,
	unsigned long clientAddress)
{
	 
	return 0;
}

int svr_connected(
	dlmsServerSettings *settings)
{
	
	return 0;
}

int main(void)
{	
	int ret;
	connection con;
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	con_init(&con, GX_TRACE_LEVEL_ERROR);
	cl_init(&con.settings, 1, 16, 1, DLMS_AUTHENTICATION_NONE, NULL, DLMS_INTERFACE_TYPE_HDLC);
		
	usart_sync_get_io_descriptor(&USART_0, &con.io);
	usart_sync_set_mode(&USART_0, USART_MODE_SYNCHRONOUS);

	usart_sync_set_baud_rate(&USART_0, 9600);
	usart_sync_set_character_size(&USART_0, USART_CHARACTER_SIZE_8BITS);
	usart_sync_set_parity(&USART_0, USART_PARITY_NONE);
	usart_sync_set_stopbits(&USART_0, USART_STOP_BITS_ONE );	
	usart_sync_enable(&USART_0);

	gxClock clock;
	ret = cosem_init(&clock.base, DLMS_OBJECT_TYPE_CLOCK, "0.0.1.0.0.255");

	//Initialize connection.
	ret = com_initializeConnection(&con);
	if (ret != DLMS_ERROR_CODE_OK)
	{
		return ret;
	}
	//Read clock.
	ret = com_read(&con, &clock.base, 2);
	if (ret != DLMS_ERROR_CODE_OK)
	{
		return ret;
	}
	com_close(&con);
	return ret;
}
