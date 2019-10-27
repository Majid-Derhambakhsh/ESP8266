/*
------------------------------------------------------------------------------
~ File   : ESP8266WiFi.h
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

#ifndef __ESP8266WiFi_H_
#define __ESP8266WiFi_H_

/*----------------------------------------------------------*/

#ifdef __cplusplus

extern "C"
{
	
#endif /* __cplusplus */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Include ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

#include <stdlib.h> /* Import standard library */
#include <stdio.h>  /* Import standard IO library */
#include <stdint.h> /* Import standard integer library */

#include "ESP8266WiFi_conf.h" /* Import config file */
#include "BUFFER/buffer.h" /* Import buffer library */

/*----------------------------------------------------------*/

#ifdef __CODEVISIONAVR__  /* Check compiler */

#include <delay.h>       /* Import delay library */

/*----------------------------------------------------------*/

#elif defined(__GNUC__)  /* Check compiler */

#include <util/delay.h>  /* Import delay library */

/*----------------------------------------------------------*/

#elif defined(USE_HAL_DRIVER)  /* Check compiler */

/* --------------- Check Mainstream series --------------- */

	#ifdef STM32F0
		#include "stm32f0xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F1)
		#include "stm32f1xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F2)
		#include "stm32f2xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F3)
		#include "stm32f3xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F4)
		#include "stm32f4xx_hal.h"       /* Import HAL library */
	#elif defined(STM32F7)
		#include "stm32f7xx_hal.h"       /* Import HAL library */
	#elif defined(STM32G0)
		#include "stm32g0xx_hal.h"       /* Import HAL library */
	#elif defined(STM32G4)
		#include "stm32g4xx_hal.h"       /* Import HAL library */

	/* ------------ Check High Performance series ------------ */

	#elif defined(STM32H7)
		#include "stm32h7xx_hal.h"       /* Import HAL library */

	/* ------------ Check Ultra low power series ------------- */

	#elif defined(STM32L0)
		#include "stm32l0xx_hal.h"       /* Import HAL library */
	#elif defined(STM32L1)
		#include "stm32l1xx_hal.h"       /* Import HAL library */
	#elif defined(STM32L5)
		#include "stm32l5xx_hal.h"       /* Import HAL library */
	#elif defined(STM32L4)
		#include "stm32l4xx_hal.h"       /* Import HAL library */
	#elif defined(STM32H7)
		#include "stm32h7xx_hal.h"       /* Import HAL library */
	#else
	#endif /* STM32F1 */

	/* ------------------------------------------------------- */

	#if defined ( __ICCARM__ ) /* ICCARM Compiler */

	#pragma diag_suppress=Pe177   /* Disable 'unused function' warning */
	#pragma diag_suppress=Pa082   /* Disable 'volatile accesses is undefined' warning */

	#elif defined   (  __GNUC__  ) /* GNU Compiler */

	#pragma diag_suppress 177     /* Disable 'unused function' warning */

	#endif /* __ICCARM__ */

/*----------------------------------------------------------*/

#else                     /* Compiler not found */

#error Microcontroller Library not supported  /* Show error */

#endif /* __CODEVISIONAVR__ */

/*----------------------------------------------------------*/

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Defines ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ---------------------- Information length ---------------------- */

#define _MAX_PWD_LENGTH           64 /* Password length */
#define _MAX_SSID_LENGTH          32 /* SSID length */
#define _MAC_ADDRESS_LENGTH       17 /* Mac Address length */
#define _IP_ADDRESS_LENGTH        15 /* IP Address length */

/* ------------------- ESP Configuration value -------------------- */

#define _MAXIMUM_AP_QUANTITY      10       /* Maximum value for check AP */
#define _MAX_BAUDRATE             4608000U /* ESP - Maximum USART BaudRate */
#define _MIN_BAUDRATE             110U     /* ESP - Minimum USART BaudRate */
#define _BAUDRATE_DEF             115200U  /* ESP - Default USART BaudRate */
#define _MAX_TX_POWER             82U      /* ESP - Maximum TX Power */
#define _MAX_TIME_START_DISCOVER  86400UL  /* Maximum time for start discover by WeChat */

/* --------------------- ESP Command length ----------------------- */

#define _GSLP_CMD_LENGTH                  16 /* Enter deep-sleep mode */
#define _ATE_CMD_LENGTH                   7  /* AT commands echo or not */
#define _UART_CMD_LENGTH                  26 /* UART configuration */
#define _UART_CUR_DEF_CMD_LENGTH          30 /* UART current-default configuration */
#define _SLEEP_CMD_LENGTH                 13 /* Sleep mode */
#define _RFPOWER_CMD_LENGTH               16 /* Set maximum value of RF TX Power */
#define _CWMODE_CMD_LENGTH                14 /* Wi-Fi mode(sta/AP/sta+AP) */
#define _CWMODE_CUR_DEF_CMD_LENGTH        18 /* Wi-Fi current-default mode(sta/AP/sta+AP) */

#define _CWJAP_CMD_LENGTH                 ((_MAX_SSID_LENGTH) + (_MAX_PWD_LENGTH) + (16)) /* Connect to AP */
#define _CWJAP_CUR_DEF_CMD_LENGTH         ((_CWJAP_CMD_LENGTH) + (4))                     /* Connect to AP */

#define _CWSAP_CMD_LENGTH                 18 /* Set configuration of ESP8266 soft-AP */
#define _CWDHCP_CMD_LENGTH                15 /* Enable/Disable DHCP */
#define _CWDHCP_CUR_DEF_CMD_LENGTH        19 /* Enable/Disable DHCP, */
#define _CWDHCPS_CUR_DEF_CMD_LENGTH       60 /* Set IP range of DHCP server */
#define _CWAUTOCONN_CMD_LENGTH            17 /* Connect to AP automatically when power on */
#define _WPS_CMD_LENGTH                   10 /* Set WPS function */
#define _CWSTARTSMART_CMD_LENGTH          20 /* Start SmartConfig */
#define _CWSTARTDISCOVER_TIME_CMD_LENGTH  8  /* Time interval that ESP8266 sends packet */
#define _MDNS_CMD_LENGTH                  12 /* Set MDNS function */

#define _CWLIF_IP_START_INDEX             10 /* Index of IP Address in buffer */

/* ------------------------ ESP Response -------------------------- */

#define _RESPONSE_1_OK  1 /* Value for check status */
#define _RESPONSE_2_OK  2 /* Value for check status */

#define _OK_RESPONSE    "OK"    /* Response x */
#define _FAIL_RESPONSE  "FAIL"  /* Response x */
#define _ERROR_RESPONSE "ERROR" /* Response x */

/* ---------------------------- Public ---------------------------- */

#define _TWO_CHAR       2
#define _ZERO_ASCII_VAL 48

#ifndef _TRUE

 #define _TRUE    1 * True value */

#endif

#ifndef NULL

 #define NULL    '\0' /* Null value */

#endif

/* ---------------------------- Types ----------------------------- */

#ifndef ENUM_U8_T

#define ENUM_U8_T(ENUM_NAME) Enum_##ENUM_NAME; typedef uint8_t ENUM_NAME /* Config enum size */

#endif

/* ---------------------- Define by compiler ---------------------- */

#ifdef __CODEVISIONAVR__  /* Check compiler */

#define _Delay(t)      delay_ms(t); /* Set function */

/* ...................................................... */

#elif defined(__GNUC__)  /* Check compiler */

#define _Delay(t)      _delay_ms(t); /* Set function */

/* ...................................................... */

#elif defined(USE_HAL_DRIVER)  /* Check compiler */

#define _TRANSMIT_DATA(_data)     HAL_UART_Transmit(&_UART_CONNECTION , _data , 1 , 10 ); /* Set function */
#define _Delay(t)                 HAL_Delay(t); /* Set function */

/* ...................................................... */

#else                     /* Compiler not found */

#endif /* __CODEVISIONAVR__ */

/* ---------------------------------------------------------------- */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Variables ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

extern uint8_t usart_received_data; /* Variable for get received data from USART */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Enum ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

typedef enum /* Sleep modes */
{
	
	_SLEEP_MODE_DISABLE = 0, /* Disable sleep mode value */
	_LIGHT_SLEEP_MODE   = 1, /* Light sleep mode value */
	_MODEM_SLEEP_MODE   = 2  /* Modem sleep mode value */
	
}SleepMode_TypeDef;

typedef enum /* AT commands echo modes */
{
	
	_ATE_DISABLE = 0, /* Switch echo off */
	_ATE_ENABLE  = 1  /* Switch echo on */
	
}SwitchEchoMode_TypeDef;

typedef enum /* Data bits */
{
	
	_DATA_BITS_5 = 5, /* 5 bits data */
	_DATA_BITS_6 = 6, /* 6 bits data */
	_DATA_BITS_7 = 7, /* 7 bits data */
	_DATA_BITS_8 = 8  /* 8 bits data */
	
}UartDataBits_TypeDef;

typedef enum /* Stop bits */
{
	
	_STOP_BITS_1   = 1, /* 1 bit stop bit */
	_STOP_BITS_1_5 = 2, /* 1.5 bit stop bit */
	_STOP_BITS_2   = 3  /* 2 bit stop bit */
	
}UartStopBits_TypeDef;

typedef enum /* Parity modes */
{
	
	_PARITY_NONE = 0, /* None parity */
	_PARITY_ODD  = 1, /* Odd parity */
	_PARITY_EVEN = 2  /* Even parity */
	
}UartParity_TypeDef;

typedef enum /* Flow control modes */
{
	
	_FLOW_CONTROL_DISABLE        = 0, /* Disable flow control */
	_FLOW_CONTROL_ENABLE_RTS     = 1, /* Enable RTS */
	_FLOW_CONTROL_ENABLE_CTS     = 2, /* Enable CTS */
	_FLOW_CONTROL_ENABLE_RTS_CTS = 3  /* Enable both RTS and CTS */
	
}UartFlowControl_TypeDef;

typedef enum /* WiFi modes */
{
	
	_WIFI_MODE_STATION              = 1, /* WiFi station mode */
	_WIFI_MODE_SOFT_AP              = 2, /* WiFi softAP mode */
	_WIFI_MODE_SOFT_AP_STATION      = 3 /* WiFi softAP + station mode */
	
}WiFiMode_TypeDef;

typedef enum /* Connect To AP Response */
{
	
	_CONNECTING_TIMEOUT_ERROR = 1, /* Error code */
	_WRONG_PASSWORD_ERROR     = 2, /* Error code */
	_CAN_NOT_FOUND_TARGET_AP  = 3, /* Error code */
	_CONNECT_FAIL			  = 4, /* Error code */
	_WIFI_DISCONNECTED		  = 5, /* WiFi connection status */
	_WIFI_CONNECTED 		  = 6, /* WiFi connection status */
	_WIFI_IP_ERROR			  = 7, /* WiFi IP status */
	_WIFI_GOT_IP			  = 8  /* WiFi IP status */
	
}ConnectToAPResponse_TypeDef;

typedef enum /* Encryption type */
{
	
	_OPEN		  = 0, /* Open network */
	_WEP		  = 1, /* WEP Security */
	_WPA_PSK	  = 2, /* WPA_PSK Security */
	_WPA2_PSK	  = 3, /* WPA2_PSK Security */
	_WPA_WPA2_PSK = 4  /* WPA_WPA2_PSK Security */
	
}Encryption_TypeDef;

typedef enum /* SoftAP SSID Hidden */
{
	
	_SHOW_SSID  = 0, /* broadcast SSID of ESP8266 soft-AP */
	_HIDE_SSID  = 1  /* do not broadcast SSID of ESP8266 soft-AP */
	
}SoftAPHidden_TypeDef;

typedef enum /* DHCP modes */
{
	
	_DHCP_SOFT_AP          = 0, /* set ESP8266 soft-AP */
	_DHCP_STATION          = 1, /* set ESP8266 station */
	_DHCP_SOFT_AP_STATION  = 2  /* set both softAP and station */
	
}DHCPMode_TypeDef;

typedef enum /* DHCP status */
{
	
	_DHCP_DISABLE  = 0, /* Disable DHCP */
	_DHCP_ENABLE   = 1  /* Enable DHCP */
	
}DHCPStatus_TypeDef;

typedef enum /* Auto Connect status */
{
	
	_AUTO_CONNECT_DISABLE  = 0, /* do NOT auto-connect to AP when power on */
	_AUTO_CONNECT_ENABLE   = 1  /* connect to AP automatically when power on */
	
}AutoConnectStatus_TypeDef;

typedef enum /* Smart Config modes */
{
	
	_SMART_CONFIG_ESP_TOUCH          = 1, /* ESP-Touch */
	_SMART_CONFIG_AIR_KISS           = 2, /* AirKiss */
	_SMART_CONFIG_ESP_TOUCH_AIR_KISS = 3  /* ESP-Touch + AirKiss */
	
}SmartConfig_TypeDef;

typedef enum /* WPS status */
{
	
	_WPS_DISABLE  = 0, /* stop WPS function */
	_WPS_ENABLE   = 1  /* start WPS function */
	
}WPSStatus_TypeDef;

typedef enum /* MDNS status */
{
	
	_MDNS_DISABLE  = 0, /* disable MDNS function */
	_MDNS_ENABLE   = 1  /* enable MDNS function */
	
}MDNSStatus_TypeDef;

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Struct ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

typedef struct /* Structure for config ESP */
{
	
	uint32_t                Baudrate;    /* ESPxxxx UART BaudRate */
	
	UartDataBits_TypeDef    DataBits;    /* ESPxxxx UART Data Bits */
	UartStopBits_TypeDef    StopBits;    /* ESPxxxx UART Stop Bits */
	UartParity_TypeDef      Parity;      /* ESPxxxx UART Parity */
	UartFlowControl_TypeDef FlowControl; /* ESPxxxx UART Flow Control */
	
}ESP_InitTypeDef;

typedef struct /* Structure for save WiFi status */
{
	
	uint8_t WiFi_Status; /* Variable for check WiFi connection status */
	uint8_t IP_Status; /* Variable for check IP status */
	
}WiFi_StatusTypeDef;

typedef struct /* Structure for save APs information */
{
	
	uint8_t NetworkChannel;         /* Channel of AP */
	uint8_t NetworkEncryption;      /* Security status of AP */
	
	int8_t  NetworkRSSI;            /* Signal strength of AP */
	int32_t NetworkFreqOffset;      /* Frequency offset of AP , unit: KHz */
	int32_t NetworkFreqCalibration; /* Calibration for frequency offset */
	
	char    *NetworkSSID;           /* SSID of AP */
	char    *NetworkMAC;            /* MAC address of AP */
	
}APInfo_TypeDef;

typedef struct /* Structure for config softAP */
{
	
	uint8_t              Channel;     /* Channel id */
	uint8_t              MaxStation;  /* Maximum count of stations that allowed to connect to ESP8266 soft-AP - range: [1, 4] */
	
	SoftAPHidden_TypeDef SSID_Hidden; /* Broadcast SSID by default */
	Encryption_TypeDef   Encryption;  /* Encryption type */
	
	char                 *SSID;       /* ESP8266 softAPâ€™ SSID */
	char                 *Password;   /* ESP8266 softAPâ€™ Password - range: 8 ~ 64 bytes ASCII*/
	
}SoftAP_InitTypeDef;

/* ............ By HAL DRIVER ............ */

#ifdef USE_HAL_DRIVER

 extern UART_HandleTypeDef _UART_CONNECTION;

#endif /* USE_HAL_DRIVER */

/* ....................................... */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Prototypes ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Basic Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

uint8_t ESP8266_IsDeviceReady(uint32_t time_out);
/*
	Guide   :
			 Parameters    :
							~ time_out : Timeout duration.
							
			 Return Values :
							_TRUE / _FALSE
			
	Example :
			 uint8_t result;
			 
			 result = ESP8266_IsDeviceReady(10000);
			 
*/

uint8_t ESP8266_Restart(uint32_t time_out);
/*
	Guide   :
			 Parameters    :
							~ time_out : Timeout duration.
							
			 Return Values :
							_TRUE / _FALSE
			
	Example :
			 uint8_t result;
			 
			 result = ESP8266_Restart(10000);
			 
*/

uint8_t ESP8266_DeepSleepModeConfig(uint16_t time, uint32_t time_out);
/*
	Guide   :
			 Parameters    :
							~ time     : Sleep time duration.
							~ time_out : Timeout duration.
							
			 Return Values :
							_TRUE / _FALSE
			
	Example :
			 uint8_t result;
			 
			 result = ESP8266_DeepSleepModeConfig(100, 10000);
			 
*/

uint8_t ESP8266_SwitchEchoConfig(SwitchEchoMode_TypeDef enable, uint32_t time_out);
/*
	Guide   :
			 Parameters    :
							~ enable   : Switch echo mode.
							~ time_out : Timeout duration.
							
			 Return Values :
							_TRUE / _FALSE
			
	Example :
			 uint8_t result;
			 
			 result = ESP8266_SwitchEchoConfig(_ATE_ENABLE, 10000);
			 
*/

uint8_t ESP8266_Restore(uint32_t time_out);
/*
	Guide   :
			 Parameters    :
							~ time_out : Timeout duration.
							
			 Return Values :
							_TRUE / _FALSE
			
	Example :
			 uint8_t result;
			 
			 result = ESP8266_Restore(10000);
			 
*/

uint8_t ESP8266_UartConfig(ESP_InitTypeDef *uart_conf, uint32_t time_out);
/*
	Guide   :
			 Parameters    :
							~ uart_conf : Pointer to struct for config uart of esp module.
							~ time_out  : Timeout duration.
							
			 Return Values :
							_TRUE / _FALSE
			
	Example :
			 uint8_t result;
			 
			 ESP_InitTypeDef x;
			 
			 x.Baudrate    = 9600;
			 x.DataBits    = _DATA_BITS_8;
			 x.StopBits    = _STOP_BITS_1;
			 x.Parity      = _PARITY_NONE;
			 x.FlowControl = _FLOW_CONTROL_DISABLE;
			 
			 result = ESP8266_UartConfig(&x, 10000);
			 
*/

uint8_t ESP8266_UartConfig_Current(ESP_InitTypeDef *uart_conf, uint32_t time_out);

uint8_t ESP8266_UartConfig_Default(ESP_InitTypeDef *uart_conf, uint32_t time_out);

uint8_t ESP8266_GetSleepMode(uint32_t time_out);

uint8_t ESP8266_SleepModeConfig(SleepMode_TypeDef sleep_mode, uint32_t time_out);

uint8_t ESP8266_RFTXPowerConfig(uint8_t tx_power, uint32_t time_out);

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Wi-Fi Related Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

uint8_t ESP8266_GetWiFiMode(WiFiMode_TypeDef *wifi_mode, uint32_t time_out);
/*
	Guide   :
			 Parameters    :
							~ wifi_mode : Pointer to get Wi-Fi mode.
							~ time_out  : Timeout duration.
							
			 Return Values :
							_TRUE / _FALSE
			
	Example :
				uint8_t result;
				WiFiMode_TypeDef WiFiMode;
				
				result = ESP8266_GetWiFiMode(&WiFiMode, 10000);
				
					result   :
							  _TRUE or _FALSE
					WiFiMode :
							  _WIFI_MODE_STATION
							  _WIFI_MODE_SOFT_AP
							  _WIFI_MODE_SOFT_AP_STATION
				
*/

uint8_t ESP8266_WiFiModeConfig(WiFiMode_TypeDef wifi_mode, uint32_t time_out);

uint8_t ESP8266_GetWiFiMode_Current(WiFiMode_TypeDef *wifi_mode, uint32_t time_out);

uint8_t ESP8266_WiFiModeConfig_Current(WiFiMode_TypeDef wifi_mode, uint32_t time_out);

uint8_t ESP8266_GetWiFiMode_Default(WiFiMode_TypeDef *wifi_mode, uint32_t time_out);

uint8_t ESP8266_WiFiModeConfig_Default(WiFiMode_TypeDef wifi_mode, uint32_t time_out);

uint8_t ESP8266_ConnectToAP(WiFi_StatusTypeDef *result, char *ssid, char *pwd, uint32_t time_out);

uint8_t ESP8266_GetConnectedAP(char *ssid, char *bssid, uint8_t *channel, int8_t *rssi, uint32_t time_out);

uint8_t ESP8266_ConnectToAP_Current(WiFi_StatusTypeDef *result, char *ssid, char *pwd, uint32_t time_out);

uint8_t ESP8266_GetConnectedAP_Current(char *ssid, char *bssid, uint8_t *channel, int8_t *rssi, uint32_t time_out);

uint8_t ESP8266_ConnectToAP_Default(WiFi_StatusTypeDef *result, char *ssid, char *pwd, uint32_t time_out);

uint8_t ESP8266_GetConnectedAP_Default(char *ssid, char *bssid, uint8_t *channel, int8_t *rssi, uint32_t time_out);

/* void ESP8266_GetAvailableAP(APInfo_TypeDef *ap_list,uint32_t time_out) {} */

uint8_t ESP8266_DisconnectFromAP(uint32_t time_out);

uint8_t ESP8266_SoftAPConfig(SoftAP_InitTypeDef *sap_cfg, uint32_t time_out);

uint8_t ESP8266_GetSoftAPConfig(char *ssid, char *pwd, uint8_t *chl, Encryption_TypeDef *ecn, uint8_t *max_conn, SoftAPHidden_TypeDef *ssid_hidden, uint32_t time_out);

uint8_t ESP8266_SoftAPConfig_Current(SoftAP_InitTypeDef *sap_cfg, uint32_t time_out);

uint8_t ESP8266_GetSoftAPConfig_Current(char *ssid, char *pwd, uint8_t *chl, Encryption_TypeDef *ecn, uint8_t *max_conn, SoftAPHidden_TypeDef *ssid_hidden, uint32_t time_out);

uint8_t ESP8266_SoftAPConfig_Default(SoftAP_InitTypeDef *sap_cfg, uint32_t time_out);

uint8_t ESP8266_GetSoftAPConfig_Default(char *ssid, char *pwd, uint8_t *chl, Encryption_TypeDef *ecn, uint8_t *max_conn, SoftAPHidden_TypeDef *ssid_hidden, uint32_t time_out);

uint8_t ESP8266_GetStationsInfo(char *ip, char *mac, uint32_t time_out);

uint8_t ESP8266_DHCPConfig(DHCPMode_TypeDef mode, DHCPStatus_TypeDef stat, uint32_t time_out);

uint8_t ESP8266_DHCPConfig_Current(DHCPMode_TypeDef mode, DHCPStatus_TypeDef stat, uint32_t time_out);

uint8_t ESP8266_DHCPConfig_Default(DHCPMode_TypeDef mode, DHCPStatus_TypeDef stat, uint32_t time_out);

uint8_t ESP8266_DHCPServerConfig_Current(DHCPMode_TypeDef enable, uint16_t lease_time, char *start_ip, char *end_ip, uint32_t time_out);

uint8_t ESP8266_GetDHCPServerConfig_Current(uint16_t *lease_time, char *start_ip, char *end_ip,uint32_t time_out);

uint8_t ESP8266_DHCPServerConfig_Default(DHCPMode_TypeDef enable, uint16_t lease_time, char *start_ip, char *end_ip, uint32_t time_out);

uint8_t ESP8266_GetDHCPServerConfig_Default(uint16_t *lease_time, char *start_ip, char *end_ip,uint32_t time_out);

uint8_t ESP8266_AutoConnectToAPConfig(AutoConnectStatus_TypeDef enable, uint32_t time_out);

uint8_t ESP8266_MACAddressOfStationConfig(char *mac_add, uint32_t time_out);

uint8_t ESP8266_GetMACAddressOfStation(char *mac, uint32_t time_out);

uint8_t ESP8266_MACAddressOfStationConfig_Current(char *mac_add, uint32_t time_out);

uint8_t ESP8266_GetMACAddressOfStation_Current(char *mac, uint32_t time_out);

uint8_t ESP8266_MACAddressOfStationConfig_Default(char *mac_add, uint32_t time_out);

uint8_t ESP8266_GetMACAddressOfStation_Default(char *mac, uint32_t time_out);

uint8_t ESP8266_MACAddressOfSoftAPConfig(char *mac_add, uint32_t time_out);

uint8_t ESP8266_GetMACAddressOfSoftAP(char *mac, uint32_t time_out);

uint8_t ESP8266_MACAddressOfSoftAPConfig_Current(char *mac_add, uint32_t time_out);

uint8_t ESP8266_GetMACAddressOfSoftAP_Current(char *mac, uint32_t time_out);

uint8_t ESP8266_MACAddressOfSoftAPConfig_Default(char *mac_add, uint32_t time_out);

uint8_t ESP8266_GetMACAddressOfSoftAP_Default(char *mac, uint32_t time_out);

uint8_t ESP8266_IPAddressOfStationConfig(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_GetIPAddressOfStation(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_IPAddressOfStationConfig_Current(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_GetIPAddressOfStation_Current(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_IPAddressOfStationConfig_Default(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_GetIPAddressOfStation_Default(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_IPAddressOfSoftAPConfig(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_GetIPAddressOfSoftAP(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_IPAddressOfSoftAPConfig_Current(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_GetIPAddressOfSoftAP_Current(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_IPAddressOfSoftAPConfig_Default(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_GetIPAddressOfSoftAP_Default(char *ip, char *gateway, char *netmask, uint32_t time_out);

uint8_t ESP8266_StartSmartConfig(SmartConfig_TypeDef type, uint32_t time_out);

uint8_t ESP8266_StopSmartConfig(uint32_t time_out);

uint8_t ESP8266_StartFoundByWeChat(char *wechat_number, char *dev_type, uint32_t time, uint32_t time_out);

uint8_t ESP8266_StopFoundByWeChat(uint32_t time_out);

uint8_t ESP8266_WPSConfig(WPSStatus_TypeDef enable, uint32_t time_out);

uint8_t ESP8266_MDNSConfig(MDNSStatus_TypeDef enable, char *hostname, char *server_name, char *server_port, uint32_t time_out);

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ TCP/IP Related Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ End Of The Program ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

#ifdef __cplusplus

}

#endif /* __cplusplus */

#endif /* __ESP8266WiFi_H_ */
