/*
 * BMS_Timing.h
 *
 *  Created on: 17-Jan-2017
 *      Author: NIKHIL
 */

#ifndef BMS_TIMING_H_
#define BMS_TIMING_H_

#include <TIMER_API.h>

#define NORMAL_MODE_40ms_PERIOD 							40
#define NORMAL_MODE_1_SECONDS								(1000/NORMAL_MODE_40ms_PERIOD)
#define NORMAL_MODE_2_SECONDS								(2*_1_SECONDS)
#define NORMAL_MODE_3_SECONDS								(3*_1_SECONDS)
#define NORMAL_MODE_4_SECONDS								(4*_1_SECONDS)
#define NORMAL_MODE_5_SECONDS								(5*_1_SECONDS)
#define NORMAL_MODE_10_SECONDS								(2* _5_SECONDS)

#define LOW_POWER_MODE_40ms_PERIOD 							8

#define NORMAL_MODE_500_MILLIS								(_1_SECONDS/2)
#define LOW_CONSUMPTION_DELAY								(60*_1_SECONDS)
#define LOW_CONSUMPTION_DELAY_AFTER_WAKEUP					(LOW_CONSUMPTION_DELAY/6)
#define CHARGE_TIME_DELAY									(1 * LOW_CONSUMPTION_DELAY)
#define DISCHARGE_TIME_DELAY								(1 * LOW_CONSUMPTION_DELAY)

#define MCU_GO_TO_SLEEP_DELAY								_5_SECONDS
#define SHORT_PERIOD										_500_MILLIS
#define LONG_PEROID											_2_SECONDS
#define DEBUG_FUNCTION_ENABLE_PERIOD						_5_SECONDS
#define FACTORY_DEFAULT_PEROID								_10_SECONDS

#define _1SEC_PERIOD										1000


extern bool _25Hz_Flag,_1Hz_Flag;

extern uint8_t Loop_Rate_Counter, Loop_Rate_Log_Counter;

/* Prototypes for the function defined in the BMS_Timing.c file */
void BMS_Timers_Init();
double Get_System_Time_Seconds();
uint64_t Get_System_Time_Millis();
void TIM2_PeriodElapsedCallback(void);
void TIM6_PeriodElapsedCallback(void);
//void TIM7_PeriodElapsedCallback(void);

#endif /* BMS_TIMING_H_ */

