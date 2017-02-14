/*
 * RTC_API.c
 *
 *  Created on: 09-Feb-2017
 *      Author: NIKHIL
 */

#include "RTC_API.h"

#ifdef BMS_VERSION
	RTC_HandleTypeDef RtcHandle;
	RTC_TimeTypeDef TimeStruct;
#endif

uint8_t RTC_Init()
{
	uint8_t Result = RESULT_OK;
#ifdef BMS_VERSION
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	 __HAL_RCC_PWR_CLK_ENABLE();
	  HAL_PWR_EnableBkUpAccess();

	#ifdef RTC_CLOCK_SOURCE_LSE
	  RCC_OscInitStruct.OscillatorType 	=  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	  RCC_OscInitStruct.PLL.PLLState 	= RCC_PLL_NONE;
	  RCC_OscInitStruct.LSEState 		= RCC_LSE_ON;
	  RCC_OscInitStruct.LSIState 		= RCC_LSI_OFF;
	  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
		  Result = RESULT_ERROR;
	  }

	  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	  {
		  Result = RESULT_ERROR;
	  }
	#elif defined (RTC_CLOCK_SOURCE_LSI)
	  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
		  Result = RESULT_ERROR;
	  }

	  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	  {
		  Result = RESULT_ERROR;
	  }
	#endif /*RTC_CLOCK_SOURCE_LSE*/

	/* Enable RTC Clock */
	__HAL_RCC_RTC_ENABLE();

	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

	RtcHandle.Instance 				= RTC;
	RtcHandle.Init.HourFormat 		= RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv 	= RTC_ASYNCH_PREDIV;
	RtcHandle.Init.SynchPrediv		= RTC_SYNCH_PREDIV;
	RtcHandle.Init.OutPut 			= RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity 	= RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType 		= RTC_OUTPUT_TYPE_OPENDRAIN;

	if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
	{
		Result = RESULT_ERROR;
	}
#endif
	return Result;
}

uint8_t RTC_Set_Date(uint8_t *Weekday, uint8_t *Date,uint8_t *Month,uint8_t *Year)
{
	uint8_t Result = RESULT_OK;
#ifdef BMS_VERSION
	RTC_DateTypeDef  DateStruct;
	DateStruct.Year 	= *Year;							// Year : 2017
	DateStruct.Month 	= *Month;
	DateStruct.Date 	= *Date;							// Date : 8
	DateStruct.WeekDay  = *Weekday;

//	DateStruct.Year 	= 0x17;							// Year : 2017
//	DateStruct.Month 	= RTC_MONTH_FEBRUARY;
//	DateStruct.Date 	= 0x10;							// Date : 8
//	DateStruct.WeekDay  = RTC_WEEKDAY_FRIDAY;

	if(HAL_RTC_SetDate(&RtcHandle,&DateStruct,RTC_FORMAT_BCD) != HAL_OK)
	{
		Result = RESULT_ERROR;
	}
#endif
	return Result;
}

uint8_t RTC_Get_Date(uint8_t *Day,uint8_t *Date,uint8_t *Month, uint8_t *Year)
{
	uint8_t Result = RESULT_OK;
#ifdef BMS_VERSION
	RTC_DateTypeDef DateStruct;
	HAL_StatusTypeDef Res;
	Res = HAL_RTC_GetDate(&RtcHandle, &DateStruct, RTC_FORMAT_BIN);
	*Day = DateStruct.WeekDay;
	*Date = DateStruct.Date;
	*Month = DateStruct.Month;
	*Year = DateStruct.Year;
	if(Res != HAL_OK)
	{
		Result = RESULT_ERROR;
	}

#endif
	return Result;
}

uint8_t RTC_Set_Time(uint8_t *Hours,uint8_t *Minutes,uint8_t *Seconds)
{
	uint8_t Result = RESULT_OK;
#ifdef BMS_VERSION
	RTC_TimeTypeDef TimeStruct;
	TimeStruct.Hours = *Hours;
	TimeStruct.Minutes = *Minutes;
	TimeStruct.Seconds = *Seconds;							// 30 seconds

//	TimeStruct.Hours = 0x10;
//	TimeStruct.Minutes = 0x48;
//	TimeStruct.Seconds = 0x00;							// 30 seconds
	TimeStruct.TimeFormat = RTC_HOURFORMAT12_AM;
	TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	TimeStruct.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&RtcHandle, &TimeStruct, RTC_FORMAT_BCD) != HAL_OK)
	{
		Result = RESULT_ERROR;
	}
#endif
	return Result;
}

uint8_t RTC_Get_Time(uint8_t *Hours,uint8_t *Minutes,uint8_t *Seconds)
{
	uint8_t Result = RESULT_OK;
#ifdef BMS_VERSION
	HAL_StatusTypeDef Res;

	Res = HAL_RTC_GetTime(&RtcHandle, &TimeStruct, RTC_FORMAT_BIN);

	*Hours = TimeStruct.Hours;
	*Minutes = TimeStruct.Minutes;
	*Seconds = TimeStruct.Seconds;

	if(Res != HAL_OK)
	{
		Result = RESULT_ERROR;
	}
#endif
	return Result;

}

uint8_t RTC_AlarmConfig(uint8_t *Day,uint8_t * Hours,uint8_t * Minutes, uint8_t *Seconds)
{
	uint8_t Result = RESULT_OK;
#ifdef BMS_VERSION

  RTC_AlarmTypeDef salarmstructure;

 /* RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
  salarmstructure.Alarm = RTC_ALARM_A;
  salarmstructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
  salarmstructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  salarmstructure.AlarmTime.Hours = 0x11;
  salarmstructure.AlarmTime.Minutes = 0x00;
  salarmstructure.AlarmTime.Seconds = 0x00;
  salarmstructure.AlarmTime.SubSeconds = 0x56;

  if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BCD) != HAL_OK)
  {
	 Result = RESULT_ERROR;
  }
#endif
  return Result;
}

void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
  /* User can write his own functionality when alarm is fired*/

}
void RTC_TimeShow(uint8_t* showtime)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char*)showtime,"%02d:%02d:%02d\r\n",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
}
