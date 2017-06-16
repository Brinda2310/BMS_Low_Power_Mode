/*
 * BMS_Timing.h
 *
 *  Created on: 17-Jan-2017
 *      Author: NIKHIL
 */

#ifndef BMS_TIMING_H_
#define BMS_TIMING_H_

#include "TIMER_API.h"

#define _33ms_PERIOD 			33
#define _1_SECONDS				(1000/_33ms_PERIOD)
#define _2_SECONDS				(2*_1_SECONDS)
#define _3_SECONDS				(3*_1_SECONDS)
#define _4_SECONDS				(4*_1_SECONDS)
#define _5_SECONDS				(5*_1_SECONDS)

#define LOW_CONSUMPTION_DELAY	(60*_5_SECONDS)
#define CHARGE_TIME_DELAY		LOW_CONSUMPTION_DELAY
#define DISCHARGE_TIME_DELAY	LOW_CONSUMPTION_DELAY

#define MCU_GO_TO_SLEEP_DELAY	(4*_5_SECONDS)
#define SHORT_PERIOD			_1_SECONDS
#define LONG_PEROID				_2_SECONDS

extern bool _30Hz_Flag,_1Hz_Flag;

void BMS_Timers_Init();
uint64_t Get_System_Time();
void TIM2_PeriodElapsedCallback(void);

//void TIM6_PeriodElapsedCallback(void);
//void TIM7_PeriodElapsedCallback(void);

#endif /* BMS_TIMING_H_ */

