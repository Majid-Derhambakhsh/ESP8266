/*
------------------------------------------------------------------------------
~ File   : ESP8266WiFi_conf.h
~ Author : Majid Derhambakhsh
~ Version: V0.0.0
~ Created: 08/04/2019 11:48:00 PM
~ Brief  :
~ Support: Majid.do16@gmail.com
------------------------------------------------------------------------------
~ Description:    Beta version

~ Attention  :
------------------------------------------------------------------------------
*/

#ifndef __ESP8266WiFi_CONF_H_
#define __ESP8266WiFi_CONF_H_

/* ~~~~~~~~~~~~~~~~ ESP Configuration ~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~ Timing Control Configuration ~~~~~~~~~~ */

#define _USE_SYSTICK
/* 
	Comment to disable systick timing.
*/

#define _GET_TICK()      HAL_GetTick()
/* 
	Set the function to get the value of systick timer.
	
	#define _GET_TICK()      SYSTICK_GetTick()
	
*/

/* ~~~~~~~~~~~~~~~ STM32 Configuration ~~~~~~~~~~~~~~ */

#define STM32F1

#define _UART_CONNECTION huart4
#define _UART_VECTOR     UART4

#endif /* __ESP8266WiFi_CONF_H_ */
