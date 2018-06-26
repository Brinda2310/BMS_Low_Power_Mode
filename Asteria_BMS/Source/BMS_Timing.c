/*
 * BMS_Timing.c
 *
 *  Created on: 17-Jan-2017
 *      Author: NIKHIL
 */

#include <BMS_Timing.h>
#include <BMS_Watchdog.h>

/* Flags to monitor 25Hz and 1Hz loop; 25Hz flag will be true after every 40ms time period;
 * 1Hz flag will be true after every one second */
bool _50ms_Flag = false,_1Sec_Flag = false,_100ms_Flag = false;

/* Variable to count to number of 40ms duration to achieve other tumer delays */
static volatile int16_t Global_5ms_Counter = 0;

/* variables to log the loop rate on SD card; Loop_Rate_Counter will be incremented after every 40ms duration
 * in main.c file */
uint8_t Loop_Rate_Counter = 0, Loop_Rate_Log_Counter = 0;

/**
 * @brief  Function to initialize the timer to 40mS. Timer value can be changed by changing the macro value
 * 		   defined in BMS_Timing.h file
 * @param  None
 * @retval None
 */
void BMS_Timers_Init()
{
	if(MCU_Power_Mode == REGULAR_POWER_MODE)
	{
		/* Timer value set to 5ms i.e. interrupt will occur at every 5ms and makes various flags true in ISR */
		Timer_Init(TIMER_2,NORMAL_MODE_5ms_PERIOD);

		/* Configure timer in low power mode with the value defined in macro */
		Timer_Init(TIMER_6,_1SEC_PERIOD);
	}
	else if (MCU_Power_Mode == LOW_POWER_MODE)
	{
		/* Timer value set to 50ms i.e. interrupt will occur at every 50ms and makes the flag true in ISR */
		Timer_Init(TIMER_2,LOW_POWER_MODE_50ms_PERIOD);

		/* Configure timer in low power mode with the value defined in macro */
		Timer_Init(TIMER_6,LOW_POWER_MODE_1SEC_PERIOD);
	}
}

/**
 * @brief  Function to return the time from boot in seconds
 * @param  None
 * @retval The timer value in seconds
 */
double Get_System_Time_Seconds()
{
	return (SysTickCounter/1000);
}

/**
 * @brief  Function to return the time from boot in milliseconds
 * @param  None
 * @retval The timer value in milliseconds
 */
uint64_t Get_System_Time_Millis()
{
	return SysTickCounter;
}

/**
 * @brief  Function callback for 25Hz timer interrupt
 * @param  None
 * @retval None
 */
void TIM2_PeriodElapsedCallback()
{
	static uint8_t Lcl_1Sec_Count = 0,Lcl_100ms_Count = 0;

	Global_5ms_Counter++;

	if(MCU_Power_Mode == LOW_POWER_MODE)
	{
		BMS_Watchdog_Refresh();
	}

	if(MCU_Power_Mode == REGULAR_POWER_MODE)
	{
		if(Global_5ms_Counter >= 10)
		{
			Global_5ms_Counter = 0;
			Lcl_100ms_Count++;
			/* This variable is used in the main loop for 20Hz tasks */
			_50ms_Flag = true;
		}

		if(Lcl_100ms_Count >= 2)
		{
			Lcl_1Sec_Count++;
			Lcl_100ms_Count = 0;
			_100ms_Flag = true;
		}

		/* Count the 40ms durations to create one second delay and the same flag is used in main loop for 1Hz tasks */
		if(Lcl_1Sec_Count>= 10)
		{
			_1Sec_Flag = true;
			Lcl_1Sec_Count = 0;
		}
	}
	else if (MCU_Power_Mode == LOW_POWER_MODE)
	{
		_50ms_Flag = true;
		if(Global_5ms_Counter >= 20)
		{
			_1Sec_Flag = true;
			Global_5ms_Counter = 0;
		}
	}
}

/**
 * @brief  Function callback for 1Hz timer interrupt
 * @param  None
 * @retval None
 */
void TIM6_PeriodElapsedCallback(void)
{
	Loop_Rate_Log_Counter = Loop_Rate_Counter;
	Loop_Rate_Counter = 0;
}
