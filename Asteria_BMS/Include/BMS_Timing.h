/*
 * BMS_Timing.h
 *
 *  Created on: 17-Jan-2017
 *      Author: NIKHIL
 */

#ifndef BMS_TIMING_H_
#define BMS_TIMING_H_

#include <TIMER_API.h>

#define NORMAL_MODE_5ms_PERIOD 							10
#define NORMAL_MODE_50ms_PERIOD 							(5*NORMAL_MODE_5ms_PERIOD)

#define NORMAL_MODE_1_SECONDS								(1000/NORMAL_MODE_50ms_PERIOD)
#define NORMAL_MODE_2_SECONDS								(2*NORMAL_MODE_1_SECONDS)
#define NORMAL_MODE_3_SECONDS								(3*NORMAL_MODE_1_SECONDS)
#define NORMAL_MODE_4_SECONDS								(4*NORMAL_MODE_1_SECONDS)
#define NORMAL_MODE_5_SECONDS								(5*NORMAL_MODE_1_SECONDS)
#define NORMAL_MODE_10_SECONDS								(2* NORMAL_MODE_5_SECONDS)

#define LOW_POWER_MODE_50ms_PERIOD 							1
#define LOW_POWER_MODE_1SEC_PERIOD 							20

#define NORMAL_MODE_500_MILLIS							(NORMAL_MODE_1_SECONDS/2)
#define LOW_CONSUMPTION_DELAY_SECONDS					(60*NORMAL_MODE_1_SECONDS)
#define LOW_CONSUMPTION_DELAY_AFTER_WAKEUP			(LOW_CONSUMPTION_DELAY_SECONDS/6)
#define CHARGE_TIME_DELAY									(1 * LOW_CONSUMPTION_DELAY_SECONDS)
#define DISCHARGE_TIME_DELAY								(1 * LOW_CONSUMPTION_DELAY_SECONDS)

#define MCU_GO_TO_SLEEP_DELAY								NORMAL_MODE_5_SECONDS
#define SHORT_PERIOD											NORMAL_MODE_500_MILLIS
#define LONG_PEROID											NORMAL_MODE_2_SECONDS
#define DEBUG_FUNCTION_ENABLE_PERIOD					NORMAL_MODE_5_SECONDS
#define FACTORY_DEFAULT_PERIOD							(NORMAL_MODE_2_SECONDS * 4)

#define _1SEC_PERIOD											2000


extern bool _50ms_Flag,_1Sec_Flag,_100ms_Flag;

extern uint8_t Loop_Rate_Counter, Loop_Rate_Log_Counter;

/* Prototypes for the function defined in the BMS_Timing.c file */
void BMS_Timers_Init();
double Get_System_Time_Seconds();
uint64_t Get_System_Time_Millis();
void TIM2_PeriodElapsedCallback(void);
void TIM6_PeriodElapsedCallback(void);
//void TIM7_PeriodElapsedCallback(void);

#endif /* BMS_TIMING_H_ */

