/*
------------------------------------------------------------------------------
~ File   : ESP8266WiFi.c
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

#include "ESP8266WiFi.h"

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Variables ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

uint8_t resp_stat           = _FALSE; /* Variable for get status */
uint8_t usart_received_data = 0; /* Variable for get received data from usart */

/* ~~~~~~~~~~~~~~~ Timing ~~~~~~~~~~~~~~~ */

volatile uint32_t tick_start       = 0; /* Variable for get system time */
volatile uint16_t time_out_control = 0; /* Variable for timing control */

/* ~~~~~~~~~~~~~~~ Buffer ~~~~~~~~~~~~~~~ */

Buffer_TypeDef esp_data_buffer; /* Create buffer for received data */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

static void ESP8266_TransmitCommand(char *str) /* Function for transmit AT Command */
{
	
	while ( *str != NULL ) /* Check value */
	{
		_TRANSMIT_DATA((uint8_t *)str); /* Transmit data */
		str++; /* Select next data */
	}
	
	/* Function End */
}

static uint8_t ESP8266_CheckSingleResponse(uint8_t *resp, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint8_t  resp_status = _FALSE; /* Variable for get status */
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ------------- Loading ------------- */
	
	time_out_control = 0; /* Reset value */
	
	/* ------------------------------------------- */
	
	#ifdef _USE_SYSTICK
	
	tick_start = _GET_TICK(); /* Get start time */
	
	resp_status = Buffer_Compare(&esp_data_buffer, resp, &resp_indx); /* Check value */
	
	time_out_control = (_GET_TICK() - tick_start); /* Calculating previous instructions time */
	
	#else
	resp_status = Buffer_Compare(&esp_data_buffer, resp, &resp_indx); /* Check value */
	
	#endif /* _USE_SYSTICK */
	
	/* ------------------------------------------- */
	
	while ( (resp_status != _TRUE) && (time_out_control < time_out) ) /* Data control */
	{
		
		#ifdef _USE_SYSTICK
		
		tick_start = _GET_TICK(); /* Get start time */
		
		resp_status = Buffer_Compare(&esp_data_buffer, resp, &resp_indx); /* Check value */
		
		time_out_control += (_GET_TICK() - tick_start); /* Calculating previous instructions time */
		
		#else
		
		resp_status = Buffer_Compare(&esp_data_buffer, resp, &resp_indx); /* Check value */
		
		time_out_control++; /* Increment time out */
		
		_Delay(1); /* Time out waiting */
		
		#endif /* _USE_SYSTICK */
		
	}
	
	return resp_status;
	
	/* Function End */
}

static uint8_t ESP8266_CheckMultiResponse(uint8_t *resp1, uint8_t *resp2, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint8_t  resp_status  = _FALSE; /* Variable for get status */
	uint8_t  resp1_status = _FALSE; /* Variable for get status */
	uint8_t  resp2_status = _FALSE; /* Variable for get status */
	uint16_t resp_indx  = _FALSE; /* Variable for get index of response */
	
	/* ------------- Loading ------------- */
	
	time_out_control = 0; /* Reset value */
	
	/* ------------------------------------------- */
	
	#ifdef _USE_SYSTICK
	
	tick_start = _GET_TICK(); /* Get start time */
	
	resp1_status = Buffer_Compare(&esp_data_buffer, resp1, &resp_indx); /* Check value */
	resp2_status = Buffer_Compare(&esp_data_buffer, resp2, &resp_indx); /* Check value */
	
	time_out_control = (_GET_TICK() - tick_start); /* Calculating previous instructions time */
	
	#else
	
	resp1_status = Buffer_Compare(&esp_data_buffer, resp1, &resp_indx); /* Check value */
	resp2_status = Buffer_Compare(&esp_data_buffer, resp2, &resp_indx); /* Check value */
	
	#endif /* _USE_SYSTICK */
	
	/* ------------------------------------------- */
	
	while ( (resp1_status != _TRUE) && (resp2_status != _TRUE) && (time_out_control < time_out) ) /* Data control */
	{
		
		#ifdef _USE_SYSTICK
		
		tick_start = _GET_TICK(); /* Get start time */
		
		resp1_status = Buffer_Compare(&esp_data_buffer, resp1, &resp_indx); /* Check value */
		resp2_status = Buffer_Compare(&esp_data_buffer, resp2, &resp_indx); /* Check value */
		
		time_out_control += (_GET_TICK() - tick_start); /* Calculating previous instructions time */
		
		#else
		
		resp1_status = Buffer_Compare(&esp_data_buffer, resp1, &resp_indx); /* Check value */
		resp2_status = Buffer_Compare(&esp_data_buffer, resp2, &resp_indx); /* Check value */
		
		time_out_control++; /* Increment time out */
		
		_Delay(1); /* Time out waiting */
		
		#endif /* _USE_SYSTICK */
		
	}
	
	/* ------------------------------------------- */
	
	if (resp1_status == _TRUE)
	{
		resp_status = _RESPONSE_1_OK;
	}
	else if (resp2_status == _TRUE)
	{
		resp_status = _RESPONSE_2_OK;
	}
	else
	{
		resp_status = _FALSE;
	}
	
	return resp_status;
	
	/* Function End */
}

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Basic Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

uint8_t ESP8266_IsDeviceReady(uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_Restart(uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+RST\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_DeepSleepModeConfig(uint16_t time, uint32_t time_out)
{
	/* ------------- Create Variable ------------- */
	
	char deep_sleep_cmd[_GSLP_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(deep_sleep_cmd,"AT+GSLP=%u\r\n",time); /* Loading command */
	
	ESP8266_TransmitCommand(deep_sleep_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_SwitchEchoConfig(SwitchEchoMode_TypeDef enable, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char switch_echo_conf_cmd[_ATE_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(switch_echo_conf_cmd,"ATE%u\r\n",enable); /* Loading command */
	
	ESP8266_TransmitCommand(switch_echo_conf_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_Restore(uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+RESTORE\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_UartConfig(ESP_InitTypeDef *uart_conf, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char uart_conf_cmd[_UART_CMD_LENGTH]; /* String for load command */
	
	/* ------------- Set values ------------- */
	
	if ( (uart_conf->Baudrate < _MIN_BAUDRATE) || (uart_conf->Baudrate > _MAX_BAUDRATE) ) /* Check value */
	{
		uart_conf->Baudrate = _BAUDRATE_DEF; /* Set value */
	}
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(uart_conf_cmd,"AT+UART=%u,%u,%u,%u,%u\r\n",uart_conf->Baudrate,uart_conf->DataBits,uart_conf->StopBits,uart_conf->Parity,uart_conf->FlowControl); /* Loading command */
	
	ESP8266_TransmitCommand(uart_conf_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_UartConfig_Current(ESP_InitTypeDef *uart_conf, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char uart_conf_cmd[_UART_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ------------- Set values ------------- */
	
	if ( (uart_conf->Baudrate < _MIN_BAUDRATE) || (uart_conf->Baudrate > _MAX_BAUDRATE) ) /* Check value */
	{
		uart_conf->Baudrate = _BAUDRATE_DEF; /* Set value */
	}
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(uart_conf_cmd,"AT+UART_CUR=%u,%u,%u,%u,%u\r\n",uart_conf->Baudrate,uart_conf->DataBits,uart_conf->StopBits,uart_conf->Parity,uart_conf->FlowControl); /* Loading command */
	
	ESP8266_TransmitCommand(uart_conf_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_UartConfig_Default(ESP_InitTypeDef *uart_conf, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char uart_conf_cmd[_UART_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ------------- Set values ------------- */
	
	if ( (uart_conf->Baudrate < _MIN_BAUDRATE) || (uart_conf->Baudrate > _MAX_BAUDRATE) ) /* Check value */
	{
		uart_conf->Baudrate = _BAUDRATE_DEF; /* Set value */
	}
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(uart_conf_cmd,"AT+UART_DEF=%u,%u,%u,%u,%u\r\n",uart_conf->Baudrate,uart_conf->DataBits,uart_conf->StopBits,uart_conf->Parity,uart_conf->FlowControl); /* Loading command */
	
	ESP8266_TransmitCommand(uart_conf_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetSleepMode(uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+SLEEP?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		if ( Buffer_Compare(&esp_data_buffer, "+SLEEP:1", &resp_indx) == _TRUE ) /* Check value */
		{
			resp_stat = _LIGHT_SLEEP_MODE; /* Return sleep mode value */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+SLEEP:2", &resp_indx) == _TRUE ) /* Check value */
		{
			resp_stat = _MODEM_SLEEP_MODE; /* Return sleep mode value */
		}
		else{}
		
	}
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_SleepModeConfig(SleepMode_TypeDef sleep_mode, uint32_t time_out)
{
	/* ------------- Create Variable ------------- */
	
	char sleep_mode_cmd[_SLEEP_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(sleep_mode_cmd,"AT+SLEEP=%u\r\n",sleep_mode); /* Loading command */
	
	ESP8266_TransmitCommand(sleep_mode_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_RFTXPowerConfig(uint8_t tx_power, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char tx_power_cmd[_RFPOWER_CMD_LENGTH]; /* String for load command */
	
	/* ------------- Set values ------------- */
	
	if ( tx_power > _MAX_TX_POWER ) /* Check value */
	{
		tx_power = _MAX_TX_POWER; /* Set value */
	}
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(tx_power_cmd,"AT+RFPOWER=%u\r\n",tx_power); /* Loading command */
	
	ESP8266_TransmitCommand(tx_power_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Wi-Fi Related Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

uint8_t ESP8266_GetWiFiMode(WiFiMode_TypeDef *wifi_mode, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWMODE?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		if ( Buffer_Compare(&esp_data_buffer, "+CWMODE:1", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_STATION; /* Return WiFi mode value */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWMODE:2", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_SOFT_AP; /* Return WiFi mode value */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWMODE:3", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_SOFT_AP_STATION; /* Return WiFi mode value */
		}
		else{}
		
	}
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_WiFiModeConfig(WiFiMode_TypeDef wifi_mode, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char wifi_mode_cmd[_CWMODE_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(wifi_mode_cmd,"AT+CWMODE=%u\r\n",wifi_mode); /* Loading command */
	
	ESP8266_TransmitCommand(wifi_mode_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetWiFiMode_Current(WiFiMode_TypeDef *wifi_mode, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWMODE_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		if ( Buffer_Compare(&esp_data_buffer, "+CWMODE_CUR:1", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_STATION; /* Return WiFi mode value */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWMODE_CUR:2", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_SOFT_AP; /* Return WiFi mode value */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWMODE_CUR:3", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_SOFT_AP_STATION; /* Return WiFi mode value */
		}
		else{}
		
	}
	
	return resp_stat; /* Return false value */
	
	/* Function End */
}

uint8_t ESP8266_WiFiModeConfig_Current(WiFiMode_TypeDef wifi_mode, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char wifi_mode_cmd[_CWMODE_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(wifi_mode_cmd,"AT+CWMODE_CUR=%u\r\n",wifi_mode); /* Loading command */
	
	ESP8266_TransmitCommand(wifi_mode_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetWiFiMode_Default(WiFiMode_TypeDef *wifi_mode, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWMODE_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		if ( Buffer_Compare(&esp_data_buffer, "+CWMODE_DEF:1", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_STATION; /* Return WiFi mode value */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWMODE_DEF:2", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_SOFT_AP; /* Return WiFi mode value */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWMODE_DEF:3", &resp_indx) == _TRUE ) /* Check value */
		{
			*wifi_mode = _WIFI_MODE_SOFT_AP_STATION; /* Return WiFi mode value */
		}
		else{}
		
	}
	
	return resp_stat; /* Return false value */
	
	/* Function End */
}

uint8_t ESP8266_WiFiModeConfig_Default(WiFiMode_TypeDef wifi_mode, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char wifi_mode_cmd[_CWMODE_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(wifi_mode_cmd,"AT+CWMODE_DEF=%u\r\n",wifi_mode); /* Loading command */
	
	ESP8266_TransmitCommand(wifi_mode_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_ConnectToAP(WiFi_StatusTypeDef *result, char *ssid, char *pwd, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWJAP=\""); /* Transmit command */
	ESP8266_TransmitCommand(ssid); /* Transmit SSID */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(pwd); /* Transmit Password */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */

	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_FAIL_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		
		resp_stat = _TRUE; /* Set status */
		
		if ( Buffer_Compare(&esp_data_buffer, "WIFI CONNECTED", &resp_indx) == _TRUE ) /* Check value */
		{
			result->WiFi_Status = _WIFI_CONNECTED; /* Set status */
		}
		
		if ( Buffer_Compare(&esp_data_buffer, "WIFI GOT IP", &resp_indx) == _TRUE ) /* Check value */
		{
			result->IP_Status = _WIFI_GOT_IP; /* Set status */
		}
		
	}
	else if ( resp_stat == _RESPONSE_2_OK ) /* Check value */
	{
		
		resp_stat = _FALSE; /* Set status */
		
		result->IP_Status = _WIFI_IP_ERROR; /* Set status */
		
		if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:1", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CONNECTING_TIMEOUT_ERROR; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:2", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _WRONG_PASSWORD_ERROR; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:3", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CAN_NOT_FOUND_TARGET_AP; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:4", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CONNECT_FAIL; /* Set status */
		}
		else{}
		
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetConnectedAP(char *ssid, char *bssid, uint8_t *channel, int8_t *rssi, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ssid  = '\0'; /* Set null */
	*bssid = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWJAP?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* .......................... Read SSID .......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ssid = ap_info[start_indx]; /* Write to user array */
			ssid++; /* Select next character */
			
		}
		
		*ssid = '\0'; /* Set null */
		
		/* .......................... Read BSSID ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read password from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( start_indx = 1 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*bssid = ap_info[start_indx]; /* Write to user array */
			bssid++; /* Select next character */
			
		}
		
		*bssid = '\0'; /* Set null */
		
		/* ........................ Read Channel ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		ap_info[end_indx] = '\0';
		
		*channel = atoi(ap_info); /* Write to user variable */
		
		/* .......................... Read RSSI .......................... */
		
		Buffer_Compare(&esp_data_buffer, "O", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		ap_info[end_indx - 1] = '\0';
		
		*rssi = atoi(ap_info); /* Write to user variable */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_ConnectToAP_Current(WiFi_StatusTypeDef *result, char *ssid, char *pwd, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWJAP_CUR=\""); /* Transmit command */
	ESP8266_TransmitCommand(ssid); /* Transmit SSID */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(pwd); /* Transmit Password */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */

	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_FAIL_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		
		resp_stat = _TRUE; /* Set status */
		
		if ( Buffer_Compare(&esp_data_buffer, "WIFI CONNECTED", &resp_indx) == _TRUE ) /* Check value */
		{
			result->WiFi_Status = _WIFI_CONNECTED; /* Set status */
		}
		
		if ( Buffer_Compare(&esp_data_buffer, "WIFI GOT IP", &resp_indx) == _TRUE ) /* Check value */
		{
			result->IP_Status = _WIFI_GOT_IP; /* Set status */
		}
		
	}
	else if ( resp_stat == _RESPONSE_2_OK ) /* Check value */
	{
		
		resp_stat = _FALSE; /* Set status */
		
		result->IP_Status = _WIFI_IP_ERROR; /* Set status */
		
		if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:1", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CONNECTING_TIMEOUT_ERROR; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:2", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _WRONG_PASSWORD_ERROR; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:3", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CAN_NOT_FOUND_TARGET_AP; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:4", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CONNECT_FAIL; /* Set status */
		}
		else{}
		
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetConnectedAP_Current(char *ssid, char *bssid, uint8_t *channel, int8_t *rssi, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ssid  = '\0'; /* Set null */
	*bssid = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWJAP_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* .......................... Read SSID .......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ssid = ap_info[start_indx]; /* Write to user array */
			ssid++; /* Select next character */
			
		}
		
		*ssid = '\0'; /* Set null */
		
		/* .......................... Read BSSID ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read password from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( start_indx = 1 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*bssid = ap_info[start_indx]; /* Write to user array */
			bssid++; /* Select next character */
			
		}
		
		*bssid = '\0'; /* Set null */
		
		/* ........................ Read Channel ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		ap_info[end_indx] = '\0';
		
		*channel = atoi(ap_info); /* Write to user variable */
		
		/* .......................... Read RSSI .......................... */
		
		Buffer_Compare(&esp_data_buffer, "O", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		ap_info[end_indx - 1] = '\0';
		
		*rssi = atoi(ap_info); /* Write to user variable */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_ConnectToAP_Default(WiFi_StatusTypeDef *result, char *ssid, char *pwd, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	uint16_t resp_indx = _FALSE; /* Variable for get index of response */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWJAP_DEF=\""); /* Transmit command */
	ESP8266_TransmitCommand(ssid); /* Transmit SSID */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(pwd); /* Transmit Password */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_FAIL_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		
		resp_stat = _TRUE; /* Set status */
		
		if ( Buffer_Compare(&esp_data_buffer, "WIFI CONNECTED", &resp_indx) == _TRUE ) /* Check value */
		{
			result->WiFi_Status = _WIFI_CONNECTED; /* Set status */
		}
		
		if ( Buffer_Compare(&esp_data_buffer, "WIFI GOT IP", &resp_indx) == _TRUE ) /* Check value */
		{
			result->IP_Status = _WIFI_GOT_IP; /* Set status */
		}
		
	}
	else if ( resp_stat == _RESPONSE_2_OK ) /* Check value */
	{
		
		resp_stat = _FALSE; /* Set status */
		
		result->IP_Status = _WIFI_IP_ERROR; /* Set status */
		
		if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:1", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CONNECTING_TIMEOUT_ERROR; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:2", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _WRONG_PASSWORD_ERROR; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:3", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CAN_NOT_FOUND_TARGET_AP; /* Set status */
		}
		else if ( Buffer_Compare(&esp_data_buffer, "+CWJAP:4", &resp_indx) == _TRUE ) /* Check error */
		{
			result->WiFi_Status = _CONNECT_FAIL; /* Set status */
		}
		else{}
		
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetConnectedAP_Default(char *ssid, char *bssid, uint8_t *channel, int8_t *rssi, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ssid  = '\0'; /* Set null */
	*bssid = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWJAP_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* .......................... Read SSID .......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ssid = ap_info[start_indx]; /* Write to user array */
			ssid++; /* Select next character */
			
		}
		
		*ssid = '\0'; /* Set null */
		
		/* .......................... Read BSSID ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read password from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( start_indx = 1 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*bssid = ap_info[start_indx]; /* Write to user array */
			bssid++; /* Select next character */
			
		}
		
		*bssid = '\0'; /* Set null */
		
		/* ........................ Read Channel ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		ap_info[end_indx] = '\0';
		
		*channel = atoi(ap_info); /* Write to user variable */
		
		/* .......................... Read RSSI .......................... */
		
		Buffer_Compare(&esp_data_buffer, "O", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		ap_info[end_indx - 1] = '\0';
		
		*rssi = atoi(ap_info); /* Write to user variable */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

/* void ESP8266_GetAvailableAP(APInfo_TypeDef *ap_list,uint32_t time_out) {} */

uint8_t ESP8266_DisconnectFromAP(uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWQAP\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_SoftAPConfig(SoftAP_InitTypeDef *sap_cfg, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_sap_cmd[_CWSAP_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_sap_cmd,"\",%u,%u,%u,%u\r\n", sap_cfg->Channel,sap_cfg->Encryption,
	sap_cfg->MaxStation, sap_cfg->SSID_Hidden); /* Loading command */

	ESP8266_TransmitCommand("AT+CWSAP=\""); /* Transmit command */
	ESP8266_TransmitCommand(sap_cfg->SSID); /* Transmit SSID */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(sap_cfg->Password); /* Transmit password */
	ESP8266_TransmitCommand(conf_sap_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetSoftAPConfig(char *ssid, char *pwd, uint8_t *chl, Encryption_TypeDef *ecn, uint8_t *max_conn, SoftAPHidden_TypeDef *ssid_hidden, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char sap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ssid  = '\0'; /* Set null */
	*pwd   = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWSAP?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* .......................... Read SSID .......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ssid = sap_info[start_indx]; /* Write to user array */
			ssid++; /* Select next character */
			
		}
		
		*ssid = '\0'; /* Set null */
		
		/* ........................ Read Password ........................ */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read password from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( start_indx = 1 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*pwd = sap_info[start_indx]; /* Write to user array */
			pwd++; /* Select next character */
			
		}
		
		*pwd = '\0'; /* Set null */
		
		/* ........................ Read Channel ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		sap_info[end_indx] = '\0';
		
		*chl = atoi(sap_info); /* Write to user variable */
		
		/* ........................ Read Encryption ......................... */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read Encryption from buffer */
		
		*ecn = (Encryption_TypeDef)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
		/* ...................... Read Max Connection ....................... */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read Max Connection from buffer */
		
		*max_conn = (uint8_t)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
		/* ........................ Read SSID Hidden ........................ */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read SSID Hidden from buffer */
		
		*ssid_hidden = (SoftAPHidden_TypeDef)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_SoftAPConfig_Current(SoftAP_InitTypeDef *sap_cfg, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_sap_cmd[_CWSAP_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_sap_cmd,"\",%u,%u,%u,%u\r\n", sap_cfg->Channel,sap_cfg->Encryption,
	sap_cfg->MaxStation, sap_cfg->SSID_Hidden); /* Loading command */

	ESP8266_TransmitCommand("AT+CWSAP_CUR=\""); /* Transmit command */
	ESP8266_TransmitCommand(sap_cfg->SSID); /* Transmit SSID */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(sap_cfg->Password); /* Transmit password */
	ESP8266_TransmitCommand(conf_sap_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetSoftAPConfig_Current(char *ssid, char *pwd, uint8_t *chl, Encryption_TypeDef *ecn, uint8_t *max_conn, SoftAPHidden_TypeDef *ssid_hidden, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char sap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ssid  = '\0'; /* Set null */
	*pwd   = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWSAP_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* .......................... Read SSID .......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ssid = sap_info[start_indx]; /* Write to user array */
			ssid++; /* Select next character */
			
		}
		
		*ssid = '\0'; /* Set null */
		
		/* ........................ Read Password ........................ */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read password from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( start_indx = 1 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*pwd = sap_info[start_indx]; /* Write to user array */
			pwd++; /* Select next character */
			
		}
		
		*pwd = '\0'; /* Set null */
		
		/* ........................ Read Channel ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		sap_info[end_indx] = '\0';
		
		*chl = atoi(sap_info); /* Write to user variable */
		
		/* ........................ Read Encryption ......................... */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read Encryption from buffer */
		
		*ecn = (Encryption_TypeDef)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
		/* ...................... Read Max Connection ....................... */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read Max Connection from buffer */
		
		*max_conn = (uint8_t)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
		/* ........................ Read SSID Hidden ........................ */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read SSID Hidden from buffer */
		
		*ssid_hidden = (SoftAPHidden_TypeDef)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_SoftAPConfig_Default(SoftAP_InitTypeDef *sap_cfg, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_sap_cmd[_CWSAP_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_sap_cmd,"\",%u,%u,%u,%u\r\n", sap_cfg->Channel,sap_cfg->Encryption,
	sap_cfg->MaxStation, sap_cfg->SSID_Hidden); /* Loading command */

	ESP8266_TransmitCommand("AT+CWSAP_DEF=\""); /* Transmit command */
	ESP8266_TransmitCommand(sap_cfg->SSID); /* Transmit SSID */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(sap_cfg->Password); /* Transmit password */
	ESP8266_TransmitCommand(conf_sap_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetSoftAPConfig_Default(char *ssid, char *pwd, uint8_t *chl, Encryption_TypeDef *ecn, uint8_t *max_conn, SoftAPHidden_TypeDef *ssid_hidden, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char sap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ssid  = '\0'; /* Set null */
	*pwd   = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWSAP_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* .......................... Read SSID .......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ssid = sap_info[start_indx]; /* Write to user array */
			ssid++; /* Select next character */
			
		}
		
		*ssid = '\0'; /* Set null */
		
		/* ........................ Read Password ........................ */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read password from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx -= _TWO_CHAR; /* Delete additional index */
		
		for ( start_indx = 1 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*pwd = sap_info[start_indx]; /* Write to user array */
			pwd++; /* Select next character */
			
		}
		
		*pwd = '\0'; /* Set null */
		
		/* ........................ Read Channel ......................... */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		sap_info[end_indx] = '\0';
		
		*chl = atoi(sap_info); /* Write to user variable */
		
		/* ........................ Read Encryption ......................... */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read Encryption from buffer */
		
		*ecn = (Encryption_TypeDef)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
		/* ...................... Read Max Connection ....................... */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read Max Connection from buffer */
		
		*max_conn = (uint8_t)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
		/* ........................ Read SSID Hidden ........................ */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)sap_info,_TWO_CHAR); /* Read SSID Hidden from buffer */
		
		*ssid_hidden = (SoftAPHidden_TypeDef)(sap_info[0] - _ZERO_ASCII_VAL); /* Write to user variable */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetStationsInfo(char *ip, char *mac, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx  = _FALSE; /* Variable for get index of response */
	uint16_t ip_end_indx = _FALSE; /* Variable for get index of response */
	
	char ip_mac_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ip  = '\0'; /* Set null */
	*mac = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWLIF\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		if ( Buffer_Compare(&esp_data_buffer, ",", &ip_end_indx) == _TRUE ) /* Check value */
		{
			
			Buffer_Compare(&esp_data_buffer, "AT+CWLIF", &start_indx); /* Get received data index */
			
			/* ........... Read IP Address ........... */
			
			Read_From_Buffer(&esp_data_buffer,(uint8_t *)ip_mac_info,ip_end_indx); /* Read IP Address from buffer */
			
			ip_end_indx--; /* Delete additional index */
			start_indx += _CWLIF_IP_START_INDEX; /* Set ip start index */
			
			for ( ; start_indx < ip_end_indx ; start_indx++) /* Loop for set data */
			{
				
				*ip = ip_mac_info[start_indx]; /* Write to user array */
				ip++; /* Select next character */
				
			}
			
			*ip = '\0'; /* Set null */
			
			/* ........... Read MAC Address ........... */
			
			Read_From_Buffer(&esp_data_buffer,(uint8_t *)ip_mac_info,_MAC_ADDRESS_LENGTH); /* Read MAC Address from buffer */
			
			for (start_indx = 0 ; start_indx < _MAC_ADDRESS_LENGTH ; start_indx++) /* Loop for set data */
			{
				
				*mac = ip_mac_info[start_indx]; /* Write to user array */
				mac++; /* Select next character */
				
			}
			
			*mac = '\0'; /* Set null */
			
		}
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_DHCPConfig(DHCPMode_TypeDef mode, DHCPStatus_TypeDef stat, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_dhcp_cmd[_CWDHCP_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_dhcp_cmd,"AT+CWDHCP=%u,%u\r\n", mode , stat); /* Loading command */
	
	ESP8266_TransmitCommand(conf_dhcp_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_DHCPConfig_Current(DHCPMode_TypeDef mode, DHCPStatus_TypeDef stat, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_dhcp_cmd[_CWDHCP_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_dhcp_cmd,"AT+CWDHCP_CUR=%u,%u\r\n", mode , stat); /* Loading command */
	
	ESP8266_TransmitCommand(conf_dhcp_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_DHCPConfig_Default(DHCPMode_TypeDef mode, DHCPStatus_TypeDef stat, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_dhcp_cmd[_CWDHCP_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_dhcp_cmd,"AT+CWDHCP_DEF=%u,%u\r\n", mode , stat); /* Loading command */
	
	ESP8266_TransmitCommand(conf_dhcp_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_DHCPServerConfig_Current(DHCPMode_TypeDef enable, uint16_t lease_time, char *start_ip, char *end_ip, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_dhcps_cmd[_CWDHCPS_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_dhcps_cmd,"AT+CWDHCPS_CUR=%u,%u,\"%s\",\"%s\"\r\n", enable, lease_time, start_ip, end_ip); /* Loading command */
	
	ESP8266_TransmitCommand(conf_dhcps_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetDHCPServerConfig_Current(uint16_t *lease_time, char *start_ip, char *end_ip,uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char dhcp_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*start_ip = '\0'; /* Set null */
	*end_ip   = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWDHCPS_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ...................... Read Lease Time ........................ */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, ":", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)dhcp_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		dhcp_info[end_indx] = '\0';
		
		*lease_time = atoi(&dhcp_info[start_indx]); /* Write to user variable */
		
		/* ........................ Read Start IP ........................ */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)dhcp_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*start_ip = dhcp_info[start_indx]; /* Write to user array */
			start_ip++; /* Select next character */
			
		}
		
		*start_ip = '\0'; /* Set null */
		
		/* ......................... Read End IP ......................... */
		
		Buffer_Compare(&esp_data_buffer, "O", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)dhcp_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*end_ip = dhcp_info[start_indx]; /* Write to user array */
			end_ip++; /* Select next character */
			
		}
		
		*end_ip = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_DHCPServerConfig_Default(DHCPMode_TypeDef enable, uint16_t lease_time, char *start_ip, char *end_ip, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char conf_dhcps_cmd[_CWDHCPS_CUR_DEF_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(conf_dhcps_cmd,"AT+CWDHCPS_DEF=%u,%u,\"%s\",\"%s\"\r\n", enable, lease_time, start_ip, end_ip); /* Loading command */
	
	ESP8266_TransmitCommand(conf_dhcps_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetDHCPServerConfig_Default(uint16_t *lease_time, char *start_ip, char *end_ip,uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char dhcp_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*start_ip = '\0'; /* Set null */
	*end_ip   = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWDHCPS_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ...................... Read Lease Time ........................ */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		Buffer_Compare(&esp_data_buffer, ":", &start_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)dhcp_info,end_indx); /* Read channel from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		dhcp_info[end_indx] = '\0';
		
		*lease_time = atoi(&dhcp_info[start_indx]); /* Write to user variable */
		
		/* ........................ Read Start IP ........................ */
		
		Buffer_Compare(&esp_data_buffer, ",", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)dhcp_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*start_ip = dhcp_info[start_indx]; /* Write to user array */
			start_ip++; /* Select next character */
			
		}
		
		*start_ip = '\0'; /* Set null */
		
		/* ......................... Read End IP ......................... */
		
		Buffer_Compare(&esp_data_buffer, "O", &end_indx); /* Get received data index */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)dhcp_info,end_indx); /* Read SSID from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*end_ip = dhcp_info[start_indx]; /* Write to user array */
			end_ip++; /* Select next character */
			
		}
		
		*end_ip = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_AutoConnectToAPConfig(AutoConnectStatus_TypeDef enable, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char auto_cn_cmd[_CWAUTOCONN_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(auto_cn_cmd,"AT+CWAUTOCONN=%u\r\n",enable); /* Loading command */
	
	ESP8266_TransmitCommand(auto_cn_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_MACAddressOfStationConfig(char *mac_add, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTAMAC=\""); /* Transmit command */
	ESP8266_TransmitCommand(mac_add); /* Transmit mac address */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetMACAddressOfStation(char *mac, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	
	char mac_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*mac = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTAMAC?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................ Read MAC Address ........................ */
		
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,start_indx); /* Read MAC Address from buffer */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,_MAC_ADDRESS_LENGTH); /* Read MAC Address from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		for ( start_indx = 0 ; start_indx < _MAC_ADDRESS_LENGTH ; start_indx++) /* Loop for set data */
		{
			
			*mac = mac_info[start_indx]; /* Write to user array */
			mac++; /* Select next character */
			
		}
		
		*mac = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_MACAddressOfStationConfig_Current(char *mac_add, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTAMAC_CUR=\""); /* Transmit command */
	ESP8266_TransmitCommand(mac_add); /* Transmit mac address */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetMACAddressOfStation_Current(char *mac, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	
	char mac_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*mac = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTAMAC_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................ Read MAC Address ........................ */
		
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,start_indx); /* Read MAC Address from buffer */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,_MAC_ADDRESS_LENGTH); /* Read MAC Address from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		for ( start_indx = 0 ; start_indx < _MAC_ADDRESS_LENGTH ; start_indx++) /* Loop for set data */
		{
			
			*mac = mac_info[start_indx]; /* Write to user array */
			mac++; /* Select next character */
			
		}
		
		*mac = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_MACAddressOfStationConfig_Default(char *mac_add, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTAMAC_DEF=\""); /* Transmit command */
	ESP8266_TransmitCommand(mac_add); /* Transmit mac address */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetMACAddressOfStation_Default(char *mac, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	
	char mac_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*mac = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTAMAC_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................ Read MAC Address ........................ */
		
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,start_indx); /* Read MAC Address from buffer */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,_MAC_ADDRESS_LENGTH); /* Read MAC Address from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		for ( start_indx = 0 ; start_indx < _MAC_ADDRESS_LENGTH ; start_indx++) /* Loop for set data */
		{
			
			*mac = mac_info[start_indx]; /* Write to user array */
			mac++; /* Select next character */
			
		}
		
		*mac = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_MACAddressOfSoftAPConfig(char *mac_add, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAPMAC=\""); /* Transmit command */
	ESP8266_TransmitCommand(mac_add); /* Transmit mac address */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetMACAddressOfSoftAP(char *mac, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	
	char mac_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*mac = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAPMAC?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................ Read MAC Address ........................ */
		
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,start_indx); /* Read MAC Address from buffer */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,_MAC_ADDRESS_LENGTH); /* Read MAC Address from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		for ( start_indx = 0 ; start_indx < _MAC_ADDRESS_LENGTH ; start_indx++) /* Loop for set data */
		{
			
			*mac = mac_info[start_indx]; /* Write to user array */
			mac++; /* Select next character */
			
		}
		
		*mac = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_MACAddressOfSoftAPConfig_Current(char *mac_add, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAPMAC_CUR=\""); /* Transmit command */
	ESP8266_TransmitCommand(mac_add); /* Transmit mac address */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetMACAddressOfSoftAP_Current(char *mac, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	
	char mac_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*mac = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAPMAC_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................ Read MAC Address ........................ */
		
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,start_indx); /* Read MAC Address from buffer */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,_MAC_ADDRESS_LENGTH); /* Read MAC Address from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		for ( start_indx = 0 ; start_indx < _MAC_ADDRESS_LENGTH ; start_indx++) /* Loop for set data */
		{
			
			*mac = mac_info[start_indx]; /* Write to user array */
			mac++; /* Select next character */
			
		}
		
		*mac = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_MACAddressOfSoftAPConfig_Default(char *mac_add, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAPMAC_DEF=\""); /* Transmit command */
	ESP8266_TransmitCommand(mac_add); /* Transmit mac address */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetMACAddressOfSoftAP_Default(char *mac, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	
	char mac_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*mac = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAPMAC_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................ Read MAC Address ........................ */
		
		Buffer_Compare(&esp_data_buffer, "\"", &start_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,start_indx); /* Read MAC Address from buffer */
		
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)mac_info,_MAC_ADDRESS_LENGTH); /* Read MAC Address from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		for ( start_indx = 0 ; start_indx < _MAC_ADDRESS_LENGTH ; start_indx++) /* Loop for set data */
		{
			
			*mac = mac_info[start_indx]; /* Write to user array */
			mac++; /* Select next character */
			
		}
		
		*mac = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_IPAddressOfStationConfig(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTA=\""); /* Transmit command */
	ESP8266_TransmitCommand(ip); /* Transmit ip address */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(gateway); /* Transmit gateway */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(netmask); /* Transmit netmask */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetIPAddressOfStation(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ip      = '\0'; /* Set null */
	*gateway = '\0'; /* Set null */
	*netmask = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTA?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................... Read IP ........................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ip = ap_info[start_indx]; /* Write to user array */
			ip++; /* Select next character */
			
		}
		
		*ip = '\0'; /* Set null */
		
		/* ........................ Read Gateway ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*gateway = ap_info[start_indx]; /* Write to user array */
			gateway++; /* Select next character */
			
		}
		
		*gateway = '\0'; /* Set null */
		
		/* ........................ Read Netmask ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*netmask = ap_info[start_indx]; /* Write to user array */
			netmask++; /* Select next character */
			
		}
		
		*netmask = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_IPAddressOfStationConfig_Current(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTA_CUR=\""); /* Transmit command */
	ESP8266_TransmitCommand(ip); /* Transmit ip address */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(gateway); /* Transmit gateway */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(netmask); /* Transmit netmask */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetIPAddressOfStation_Current(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ip      = '\0'; /* Set null */
	*gateway = '\0'; /* Set null */
	*netmask = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTA_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................... Read IP ........................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ip = ap_info[start_indx]; /* Write to user array */
			ip++; /* Select next character */
			
		}
		
		*ip = '\0'; /* Set null */
		
		/* ........................ Read Gateway ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*gateway = ap_info[start_indx]; /* Write to user array */
			gateway++; /* Select next character */
			
		}
		
		*gateway = '\0'; /* Set null */
		
		/* ........................ Read Netmask ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*netmask = ap_info[start_indx]; /* Write to user array */
			netmask++; /* Select next character */
			
		}
		
		*netmask = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_IPAddressOfStationConfig_Default(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTA_DEF=\""); /* Transmit command */
	ESP8266_TransmitCommand(ip); /* Transmit ip address */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(gateway); /* Transmit gateway */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(netmask); /* Transmit netmask */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetIPAddressOfStation_Default(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ip      = '\0'; /* Set null */
	*gateway = '\0'; /* Set null */
	*netmask = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPSTA_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................... Read IP ........................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ip = ap_info[start_indx]; /* Write to user array */
			ip++; /* Select next character */
			
		}
		
		*ip = '\0'; /* Set null */
		
		/* ........................ Read Gateway ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*gateway = ap_info[start_indx]; /* Write to user array */
			gateway++; /* Select next character */
			
		}
		
		*gateway = '\0'; /* Set null */
		
		/* ........................ Read Netmask ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*netmask = ap_info[start_indx]; /* Write to user array */
			netmask++; /* Select next character */
			
		}
		
		*netmask = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_IPAddressOfSoftAPConfig(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAP=\""); /* Transmit command */
	ESP8266_TransmitCommand(ip); /* Transmit ip address */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(gateway); /* Transmit gateway */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(netmask); /* Transmit netmask */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetIPAddressOfSoftAP(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ip      = '\0'; /* Set null */
	*gateway = '\0'; /* Set null */
	*netmask = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAP?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................... Read IP ........................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ip = ap_info[start_indx]; /* Write to user array */
			ip++; /* Select next character */
			
		}
		
		*ip = '\0'; /* Set null */
		
		/* ........................ Read Gateway ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*gateway = ap_info[start_indx]; /* Write to user array */
			gateway++; /* Select next character */
			
		}
		
		*gateway = '\0'; /* Set null */
		
		/* ........................ Read Netmask ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*netmask = ap_info[start_indx]; /* Write to user array */
			netmask++; /* Select next character */
			
		}
		
		*netmask = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_IPAddressOfSoftAPConfig_Current(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAP_CUR=\""); /* Transmit command */
	ESP8266_TransmitCommand(ip); /* Transmit ip address */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(gateway); /* Transmit gateway */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(netmask); /* Transmit netmask */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetIPAddressOfSoftAP_Current(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ip      = '\0'; /* Set null */
	*gateway = '\0'; /* Set null */
	*netmask = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAP_CUR?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................... Read IP ........................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ip = ap_info[start_indx]; /* Write to user array */
			ip++; /* Select next character */
			
		}
		
		*ip = '\0'; /* Set null */
		
		/* ........................ Read Gateway ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*gateway = ap_info[start_indx]; /* Write to user array */
			gateway++; /* Select next character */
			
		}
		
		*gateway = '\0'; /* Set null */
		
		/* ........................ Read Netmask ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*netmask = ap_info[start_indx]; /* Write to user array */
			netmask++; /* Select next character */
			
		}
		
		*netmask = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_IPAddressOfSoftAPConfig_Default(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAP_DEF=\""); /* Transmit command */
	ESP8266_TransmitCommand(ip); /* Transmit ip address */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(gateway); /* Transmit gateway */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(netmask); /* Transmit netmask */
	ESP8266_TransmitCommand("\"\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_GetIPAddressOfSoftAP_Default(char *ip, char *gateway, char *netmask, uint32_t time_out)
{
	/* ---------------- Variable -------------- */
	
	uint16_t start_indx = _FALSE; /* Variable for get index of response */
	uint16_t end_indx   = _FALSE; /* Variable for get index of response */
	
	char ap_info[_BUFFER_LENGTH]; /* Array for sort data */
	
	/* -------------- Load value -------------- */
	
	*ip      = '\0'; /* Set null */
	*gateway = '\0'; /* Set null */
	*netmask = '\0'; /* Set null */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CIPAP_DEF?\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckSingleResponse(_OK_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _TRUE ) /* Check value */
	{
		
		/* ........................... Read IP ........................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*ip = ap_info[start_indx]; /* Write to user array */
			ip++; /* Select next character */
			
		}
		
		*ip = '\0'; /* Set null */
		
		/* ........................ Read Gateway ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*gateway = ap_info[start_indx]; /* Write to user array */
			gateway++; /* Select next character */
			
		}
		
		*gateway = '\0'; /* Set null */
		
		/* ........................ Read Netmask ......................... */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		Buffer_Compare(&esp_data_buffer, "\"", &end_indx); /* Get received data index */
		Read_From_Buffer(&esp_data_buffer,(uint8_t *)ap_info,end_indx); /* Read from buffer */
		
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		
		end_indx--; /* Delete additional index */
		
		for ( start_indx = 0 ; start_indx < end_indx ; start_indx++) /* Loop for set data */
		{
			
			*netmask = ap_info[start_indx]; /* Write to user array */
			netmask++; /* Select next character */
			
		}
		
		*netmask = '\0'; /* Set null */
		
	}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_StartSmartConfig(SmartConfig_TypeDef type, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char start_smart_conf[_CWSTARTSMART_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(start_smart_conf,"AT+CWSTARTSMART=%u\r\n", type); /* Loading command */

	ESP8266_TransmitCommand(start_smart_conf); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_StopSmartConfig(uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWSTOPSMART\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_StartFoundByWeChat(char *wechat_number, char *dev_type, uint32_t time, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char start_discover_time[_CWSTARTDISCOVER_TIME_CMD_LENGTH]; /* String for load command */
	
	/* ------------- Check values ------------- */
	
	if (time > _MAX_TIME_START_DISCOVER) /* Check time */
	{
		time = _MAX_TIME_START_DISCOVER; /* Set time */
	}
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(start_discover_time,"%u\r\n", time); /* Loading command */
	
	ESP8266_TransmitCommand("AT+CWSTARTDISCOVER=\""); /* Transmit command */
	ESP8266_TransmitCommand(wechat_number); /* Transmit WeChat number */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(dev_type); /* Transmit device type */
	ESP8266_TransmitCommand("\","); /* Transmit command */
	ESP8266_TransmitCommand(start_discover_time); /* Transmit time */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_StopFoundByWeChat(uint32_t time_out)
{
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	ESP8266_TransmitCommand("AT+CWSTOPDISCOVER\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_WPSConfig(WPSStatus_TypeDef enable, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char wps_conf_cmd[_WPS_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(wps_conf_cmd,"AT+WPS=%u\r\n",enable); /* Loading command */
	
	ESP8266_TransmitCommand(wps_conf_cmd); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

uint8_t ESP8266_MDNSConfig(MDNSStatus_TypeDef enable, char *hostname, char *server_name, char *server_port, uint32_t time_out)
{
	/* --------------- Variable --------------- */
	
	char mdns_conf[_MDNS_CMD_LENGTH]; /* String for load command */
	
	/* ---------- Transmit Commands ----------- */
	
	Clean_Buffer(&esp_data_buffer); /* Clean buffer */
	
	sprintf(mdns_conf,"AT+MDNS=%u,\"", enable); /* Loading command */
	
	ESP8266_TransmitCommand(mdns_conf); /* Transmit time */
	ESP8266_TransmitCommand(hostname); /* Transmit WeChat number */
	ESP8266_TransmitCommand("\",\""); /* Transmit command */
	ESP8266_TransmitCommand(server_name); /* Transmit device type */
	ESP8266_TransmitCommand("\","); /* Transmit command */
	ESP8266_TransmitCommand(server_port); /* Transmit device type */
	ESP8266_TransmitCommand("\r\n"); /* Transmit command */
	
	/* ---------------------------------------- */
	
	resp_stat = ESP8266_CheckMultiResponse(_OK_RESPONSE,_ERROR_RESPONSE,time_out); /* Check ESP response */
	
	/* ---------------------------------------- */
	
	if ( resp_stat == _RESPONSE_1_OK ) /* Check value */
	{
		resp_stat = _TRUE; /* Set status */
	}
	else if (resp_stat == _RESPONSE_2_OK)
	{
		resp_stat = _FALSE;
	}
	else{}
	
	/* ---------------------------------------- */
	
	return resp_stat;
	
	/* Function End */
}

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ TCP/IP Related Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Usart Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

#ifdef USE_HAL_DRIVER

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	if(huart->Instance == _UART_VECTOR)
	{
		Save_In_Buffer(&esp_data_buffer,usart_received_data);
		HAL_UART_Receive_IT(&_UART_CONNECTION , &usart_received_data , 1);
	}

}

#endif /* USE_HAL_DRIVER */

/* Program End */
