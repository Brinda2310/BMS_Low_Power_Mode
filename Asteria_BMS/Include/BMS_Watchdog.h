/*
 * BMS_Watchdog.h
 *
 *  Created on: 25-Oct-2017
 *      Author: NIKHIL
 */

#ifndef BMS_WATCHDOG_H_
#define BMS_WATCHDOG_H_

#include <IWDG_API.h>

#define _2_SECONDS_WDG			2
#define _3_SECONDS_WDG			3
#define _5_SECONDS_WDG			5

extern bool BMS_Watchdog_Enable;

/* Prototypes for the functions defined in the BMS_Watchdog.c file */
void BMS_watchdog_Init(void);
void BMS_Watchdog_Refresh(void);

#endif /* BMS_WATCHDOG_H_ */
