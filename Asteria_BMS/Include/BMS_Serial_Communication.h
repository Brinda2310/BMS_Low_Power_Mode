/*
 * BMS_Serial_Communication.h
 *
 *  Created on: 18-Jan-2017
 *      Author: NIKHIL
 */

#ifndef BMS_SERIAL_COMMUNICATION_H_
#define BMS_SERIAL_COMMUNICATION_H_

#include <USART_API.h>

#define LOW_POWER_MODE_BAUD_RATE							9600
#define NORMAL_POWER_MODE_BUAD_RATE						LOW_POWER_MODE_BAUD_RATE

/* Variable to use the debug functionality when SOC/SOH logic is not used */
extern bool Debug_COM_Enable;

/* Prototypes for the function defined in BMS_Serial_Communication.c file */
void BMS_Debug_COM_Init(void);
void BMS_Debug_COM_Read_Data(uint8_t *RxBuffer,uint16_t Size);
void BMS_Debug_COM_Write_Data(void *TxBuffer,uint16_t Size);

#endif /* BMS_SERIAL_COMMUNICATION_H_ */
