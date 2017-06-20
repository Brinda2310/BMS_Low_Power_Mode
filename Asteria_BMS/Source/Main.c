/**
 ******************************************************************************
 * @file    Main.c
 * @author  Nikhil Ingale
 * @version V1.0.0
 * @date    27-May-2017
 * @brief   Default main function.
 ******************************************************************************
*/
#include <BMS_ASIC.h>
#include <BMS_Data_Log.h>
#include <BMS_Serial_Communication.h>
#include <BMS_GPIOs.h>
#include <BMS_Timing.h>
#include <Power_Management.h>
#include <AP_Communication.h>

const uint8_t BMS_Firmware_Version[3] =
{
		1,			// Major release version--modified when code is being merged to Master branch.
		0,			// Current stable code release-- modified when code is being merged to Develop branch.
		1			// Beta code release--modified when code is being merged to test_develop branch.
};

/* Variable to keep the track of time elapsed when switch is pressed for short duration i.e. 2 seconds */
uint16_t Switch_Press_Time_Count = 0;
/* Variable to keep the track of time when there is no current consumption so as to force the ISL to sleep */
uint16_t BMS_Sleep_Time_Count = 0;
/* Variable to keep the track of time when ISL goes to sleep so as to put MCU in sleep mode */
uint16_t MCU_Sleep_Time_Count = 0;

/* Variable to keep the track of time since charging is started and increment the cycles accordingly */
uint16_t Charge_Time_Count = 0;
/* Variable to keep the track of time since discharging is started and increment the cycles accordingly */
uint16_t Discharge_Time_Count = 0;

/* Variable to avoid multiple increments of the same cycle either charging or discharging */
bool Update_Pack_Cycles = false;
uint8_t Start_Charging = -1;

/* Debug code variables definition; to be removed after testing */
char Buffer[200],Length = 0;
uint8_t RecData = 0;

/* Variable to hold the timing value to force the BMS IC to sleep mode; Values are set using macros defined
 * in BMS_Timing.h file. If MCU is awaken from sleep mode then check load presence for 10 seconds
 * otherwise check load presence for 1 minute(normal operation) */
uint16_t Timer_Value = 0;

/* Debug variable to be removed after testing */
bool ISL_Sleep = false;

int main(void)
{
	/* Configure the sysTick interrupt to 1mS(default) and Set the NVIC group priority to 4 */
	HAL_Init();

	/* Configure the system clock frequency (Peripherals clock) to 80MHz */
	Set_System_Clock_Frequency();

	/* Delay of 2 Seconds is required to make sure BMS is not polled before it's POR cycle otherwise
	 * BMS I2C will be locked */
	Delay_Millis(2000);

	/* Initialize the timer to 33mS(30Hz) and the same is used to achieve different loop rates */
	BMS_Timers_Init();

	/* Initialize the USART to 115200 baud rate to debug the code */
#if DEBUG_COM == ENABLE
	BMS_Debug_COM_Init();
#endif
	/* Initialize the status LEDs which indicates the SOC and SOH */
	BMS_Status_LEDs_Init();

	/* Configure the switch as input to wake up the BMS in case of sleep and same will be used
	 * to show the SOC and SOH on status LEDs*/
	BMS_Switch_Init();

	/* Configure the ISL94203 I2C communication to 100KHz */
	BMS_ASIC_Init();

	/* Initialize the communication between AP and BMS; Current version of BMS supports SMBUS protocol */
	AP_COM_Init(AP_COM_SMBUS_MODE);

	/* Initialize the RTC and set the RTC time and date to the date and time received from GPS */
	RTC_Init();

	/* Set the current gain in the BMS ASIC register. After having number of iterations and analyzing
	 * the curves we will decide which gain is suitable for which current range(Amperes) */
	BMS_Set_Current_Gain(CURRENT_GAIN_5X);

	/* Read the pack voltage to calculate the battery capacity used/remaining */
	BMS_Read_Pack_Voltage();

	/* Create the LOG file on SD card by reading the count from log summary file */
	if(BMS_Log_Init() == RESULT_OK)
	{
#if  DEBUG_MANDATORY == ENABLE
		BMS_Debug_COM_Write_Data("Log_file_Created\r", 17);
#endif
	}
	else
	{
#if DEBUG_MANDATORY == ENABLE
		BMS_Debug_COM_Write_Data("SD Card Not Present\r", 20);
#endif
	}

	/* Calculate the battery capacity used and remaining so that same value will be used to estimate
	 * next values */
	BMS_Estimate_Initial_Capacity();

	/* Initial state of charging/discharging required to calculate the pack cycles(C/D) used */
	if(Get_BMS_Charge_Discharge_Status() == CHARGING)
	{
		Last_Charge_Disharge_Status = CHARGING;
	}
	else
	{
		Last_Charge_Disharge_Status = DISCHARGING;
	}

	/* Debug delay to print the battery capacity remaining on USART */
	Delay_Millis(5);

	/* Debug code just to see the correct value of battery capacity remaining which can be seen on USART */
#if DEBUG_OPTIONAL == ENABLE
	Length = sprintf(Buffer,"%f",Get_BMS_Capacity_Remaining());
	BMS_Debug_COM_Write_Data(Buffer, Length);
#endif

	/* Every time when MCU restarts, set the load check timer value to 1 minute; If MCU is awaken from sleep
	 * mode then set this value to 10 seconds; If load is not present for 10 seconds then force BMS to
	 * sleep mode again */
	Timer_Value = LOW_CONSUMPTION_DELAY;

	while(1)
	{
		/* If MCU is awaken from sleep mode then we have to reinitialize all the peripherals */
		if(Wakeup_From_Sleep == true)
		{
			/* This variable decides the time after which MCU will go to sleep if load is not present */
			Timer_Value = LOW_CONSUMPTION_DELAY_AFTER_WAKEUP;

			/* Initialize the communication between AP and BMS; Current version of BMS supports SMBUS protocol */
			AP_COM_Init(AP_COM_SMBUS_MODE);

			/* Initialize the RTC and set the RTC time and date to the date and time received from GPS */
			RTC_Init();

			/* Set the current gain in the BMS ASIC register. After having number of iterations and
			 * analyzing the curves we will decide which gain is suitable for which current range(Amperes) */
			BMS_Set_Current_Gain(CURRENT_GAIN_5X);

			/* Read the pack voltage to calculate the battery capacity used/remaining */
			BMS_Read_Pack_Voltage();

			/* Create the LOG file on SD card by reading the count from log summary file; Debug code is
			 * to be removed after testing; We can decide to show some statuses on LEDs if there is
			 * problem with the SD card */
			if(BMS_Log_Init() == RESULT_OK)
			{
		#if  DEBUG_MANDATORY == ENABLE
				BMS_Debug_COM_Write_Data("Log_file_Created\r", 17);
		#endif
			}
			else
			{
		#if DEBUG_MANDATORY == ENABLE
				BMS_Debug_COM_Write_Data("SD Card Not Present\r", 20);
		#endif
			}
			/* Debug delay to print the USART string */
			Delay_Millis(10);
			/* Calculate the battery capacity used and remaining so that same value will be used to estimate
			 * next values */
			BMS_Estimate_Initial_Capacity();

			/* Initial state of charging/discharging required to calculate the pack cycles used */
			if(Get_BMS_Charge_Discharge_Status() == CHARGING)
			{
				Last_Charge_Disharge_Status = CHARGING;
			}
			else
			{
				Last_Charge_Disharge_Status = DISCHARGING;
			}
			/* This flag must be cleared to avoid reinitializing all the peripherals again and again */
			Wakeup_From_Sleep = false;
			ISL_Sleep = false;

			/* Debug code; To be removed after testing */
#if DEBUG_MANDATORY == ENABLE
			BMS_Debug_COM_Write_Data("Wake up from sleep\r",19);
#endif
		}

#if DEBUG_MANDATORY == ENABLE
		/* Debug code to be removed after testing */
		BMS_Debug_COM_Read_Data(&RecData,1);

		if(RecData == 'A')
		{
			/* Debug code to set the last charge/discharge status to charging to see whether discharge
			 * pack cycles are getting updated properly or not;Because discharging cycles will be updated
			 * only if last state was charging */
			Last_Charge_Disharge_Status = CHARGING;
		}
		RecData = 0;
#endif

		/* This flag will be true after every 33mS(30Hz) in timer application file */
		if (_30Hz_Flag == true)
		{
			/* If switch is pressed then start counting the time */
			if (BMS_Read_Switch_Status() == PRESSED)
			{
				Switch_Press_Time_Count++;
			}
			else
			{
				/* As soon as switch is released, check for timer count value. If count is in between
				 * 500ms and 1 seconds then show the SOC status on LEDs and if count is more than 2
				 * seconds then show SOH status on LEDs */
				if(Switch_Press_Time_Count >= SHORT_PERIOD && Switch_Press_Time_Count <= LONG_PEROID)
				{
					BMS_Show_LED_Pattern(SOC);
				}
				else if(Switch_Press_Time_Count >= LONG_PEROID)
				{
					BMS_Show_LED_Pattern(SOH);
				}

				/* If switch is immediately released then reset time count to zero */
				Switch_Press_Time_Count = 0;
			}

			/* Query the BMS data at 30Hz; All cell voltages, pack voltage, pack current, pack temperature
			 * all status flags and calculate the battery capacity used */
			BMS_Read_Cell_Voltages();
			BMS_Read_Pack_Voltage();
			BMS_Read_Pack_Current();
			BMS_Read_Pack_Temperature();
			BMS_Read_RAM_Status_Register();
			BMS_Estimate_Capacity_Used();

			/* Debug LED status to check MCU is working correctly; To be removed after testing is done */
			BMS_Status_LED_Toggle();

			/* If current consumption is less than 200mA and BMS IC is not in sleep mode then start
			 * counting the timer value */
		if(Sleep_Mode_Funtionality == ENABLE)
		{
			if(((uint16_t)Get_BMS_Pack_Current() < MINIMUM_CURRENT_CONSUMPTION) && Status_Flag.BMS_In_Sleep == NO)
			{
				BMS_Sleep_Time_Count++;

				/* If MCU is awaken by external switch or load then check the load only for 10 seconds.
				 * If load is not present for continuous 10 seconds then force BMS IC again to sleep mode
				 * and if load is present then check the presence of load for continuous 1 minute */
				if(BMS_Sleep_Time_Count >= Timer_Value)
				{
					BMS_Sleep_Time_Count = 0;
					/* Set the corresponding flag in BMS IC to force it to sleep mode */
					BMS_Force_Sleep();
				}
			}
			/* If some load is present then always clear the timer counts to zero */
			else if (((uint16_t)Get_BMS_Pack_Current() > MINIMUM_CURRENT_CONSUMPTION))
			{
			   /* If BMS consumes more than 200mA in between then reset the time count to zero */
				BMS_Sleep_Time_Count = 0;
				/* Debug flag to be removed after testing is completed */
				ISL_Sleep = false;
				/* If load is present then change the timer check value for sleep to 1 minute */
				Timer_Value = LOW_CONSUMPTION_DELAY;
			}

			/* If BMS IC is forced to sleep mode then start counting the timer value; If BMS IC goes to
			 * sleep mode immediately after wake up then MCU will go to sleep after 5 seconds */
			if(Status_Flag.BMS_In_Sleep == YES)
			{
				ISL_Sleep = true;
				MCU_Sleep_Time_Count++;
				/* When BMS IC goes to sleep then MCU also goes to sleep mode after 5 Seconds */
				if(MCU_Sleep_Time_Count >= MCU_GO_TO_SLEEP_DELAY)
				{
					MCU_Sleep_Time_Count = 0;
					/* This flag makes sure that controller is wake up from sleep mode only */
					Sleep_Mode = true;
					/* Debug code to be removed after testing is completed */
#if DEBUG_MANDATORY == ENABLE
					BMS_Debug_COM_Write_Data("MCU Went to sleep\r",18);
					Delay_Millis(5);
#endif
					/* Configures the external trigger events which will wake up the MCU and then goes to
					 * sleep mode */
					MCU_Enter_Sleep_Mode();
				}
			}
			/* If BMS IC is not in sleep mode then always reset the timer count to zero so as to get
			 * exact timer value for next iterations */
			else
			{
				MCU_Sleep_Time_Count = 0;
				/* Debug flag to be removed after testing */
				ISL_Sleep = false;
			}
		}

			/* Once MCU wakes up, it starts the execution from where it had left off thats why it is necessary
			 * to check whether MCU started from sleep mode or executing it normal way */
			if(Wakeup_From_Sleep == false)
			{
				/* If external charger is connected to the BMS then keep continuous track of it*/
				if(Get_BMS_Charge_Discharge_Status() == CHARGING)
				{
					/* Make the count used for charging to zero to get the exact duration of 5mins while
					 * executing the discharge section of the code */
					Discharge_Time_Count = 0;
					/* If current coming into the pack is more than 1amperes then start counting the time */
					if(Get_BMS_Pack_Current() > CHARGE_CURRENT_CONSUMPTION && Update_Pack_Cycles == false)
					{
						/* Debug variable to be removed after testing */
						Start_Charging = 1;
						Charge_Time_Count++;
					}
					else
					{
						/* BMS is charging but with value less than mentioned(1A)then reset the timer
						 * count to zero */
						Charge_Time_Count = 0;
					}

					/* If current coming into the pack is more than 1amperes for more than 5mins(CHARGE_TIME_DELAY),
					 * then increment the charge cycles count and make the status to of variable to true so as to
					 * keep track of last state of the pack i.e. charging/discharging  */
					if(Charge_Time_Count >= CHARGE_TIME_DELAY && Last_Charge_Disharge_Status != CHARGING)
					{
						BMS_Data.Pack_Charge_Cycles++;
						Update_Pack_Cycles = true;
						Last_Charge_Disharge_Status = CHARGING;
						BMS_Update_Pack_Cycles();
					}
				}
				/* If status of the BMS is discharging then keep continuous track of it */
				else if (Get_BMS_Charge_Discharge_Status() == DISCHARGING)
				{
					/* Make the count used for charging to zero to get the exact duration of 5mins while
					 * executing the charging section of the code */
					Charge_Time_Count = 0;
					/* If current going out of the pack is more than 1 amperes then start counting the time */
					if(Get_BMS_Pack_Current() > DISCHARGE_CURRENT_CONSUMPTION && Update_Pack_Cycles == false)
					{
						/* Debug variable to be removed from the code */
						Start_Charging = 2;
						Discharge_Time_Count++;
					}
					else
					{
						Discharge_Time_Count = 0;
					}

					/* If discharge current is more than 1 amperes for more than 5 minutes then increment
					 * the discharge cycles count by ensuring that the previous state of the pack was
					 * not discharging */
					if(Discharge_Time_Count >= DISCHARGE_TIME_DELAY && Last_Charge_Disharge_Status != DISCHARGING)
					{
						BMS_Data.Pack_Discharge_Cycles++;
						Update_Pack_Cycles = true;
						Last_Charge_Disharge_Status = DISCHARGING;
						BMS_Update_Pack_Cycles();
					}
				}
				else
				{
					/* If BMS is in low power consumption mode then clear all the timer counts and
					 * do not update any cycles(C/D) count */
					Charge_Time_Count = 0;
					Discharge_Time_Count = 0;
					Update_Pack_Cycles = false;
				}
			}
			_30Hz_Flag = false;
		}
		/* Log the BMS variables on SD card at 1Hz;Make sure that MCU has initialized all the peripherals
		 * before using it.After wake up this flag will be true and will become false once all peripherals
		 * are initialized properly */
		if(_1Hz_Flag == true && Wakeup_From_Sleep == false)
		{
#if DEBUG_MANDATORY == ENABLE
			/* Debug code to check whether discharge cycles are getting updated properly or not along
			 * with last charge discharge status */
//			Length = sprintf(Buffer,"%d*%d\r",Last_Charge_Disharge_Status,(int)BMS_Data.Pack_Discharge_Cycles);
//			BMS_Debug_COM_Write_Data(Buffer,Length);

			/* Debug code to be removed after testing is done;
			 * If BMS IC is in sleep mode then throw "Sleep Mode" string on USART otherwise throw
			 * "Non Sleep Mode" string on USART */
			if(Get_BMS_Sleep_Mode_Status() == SLEEP_MODE)
			{
				BMS_Debug_COM_Write_Data("Sleep Mode\r",11);
			}
			else if (Get_BMS_Sleep_Mode_Status() == NON_SLEEP_MODE)
			{
				BMS_Debug_COM_Write_Data("Non Sleep Mode\r",15);
				Delay_Millis(5);
			}

			/* Debug code to be removed after testing is done;
			 * Check whether charging/discharge current is more than 1A or not */
			if(Start_Charging == 1)
			{
				Start_Charging = 6;
				BMS_Debug_COM_Write_Data("Charging\r",9);
			}
			else if (Start_Charging == 2)
			{
				Start_Charging = 7;
				BMS_Debug_COM_Write_Data("Discharging\r",12);
				Delay_Millis(5);
			}

			/* Debug code to be removed after testing is done;
			 * If BMS IC is not in sleep mode then only show all the data on USART */
			if(ISL_Sleep == false)
			{
				uint8_t Length = sprintf(Buffer,"Pack_Voltage = %0.3fV\r",Get_BMS_Pack_Voltage());
				BMS_Debug_COM_Write_Data(Buffer,Length);
				Delay_Millis(5);

				Length = sprintf(Buffer,"Pack_Current = %0.3fmA\r",Get_BMS_Pack_Current());
				BMS_Debug_COM_Write_Data(Buffer,Length);
				Delay_Millis(5);

//				Length = sprintf(Buffer,"Pack_Temp = %0.3f Degrees\r",Get_BMS_Pack_Temperature());
//				BMS_Debug_COM_Write_Data(Buffer,Length1);
//				Delay_Millis(3);
//
//				uint16_t Length = 0;
//				Length += sprintf(Buffer, "Batt used = %0.3fmAH\r", Get_BMS_Capacity_Used());
//				Length += sprintf(&Buffer[Length], "Batt remaining = %0.3f\r", Get_BMS_Capacity_Remaining());
//				Length += sprintf(Buffer, "Cell1_V = %0.3fV\r", Get_Cell1_Voltage());
//				Length += sprintf(&Buffer[Length], "Cell2_V = %0.3fV\r",Get_Cell2_Voltage());
//				Length += sprintf(&Buffer[Length], "Cell3_V = %0.3fV\r",Get_Cell3_Voltage());
//				Length += sprintf(&Buffer[Length], "Cell6_V = %0.3fV\r",Get_Cell6_Voltage());
//				Length += sprintf(&Buffer[Length], "Cell7_V = %0.3fV\r",Get_Cell7_Voltage());
//				Length += sprintf(&Buffer[Length], "Cell8_V = %0.3fV\r",Get_Cell8_Voltage());

//				BMS_Debug_COM_Write_Data(Buffer, Length);
//				Delay_Millis(10);
			}
#endif
			if (Log_All_Data() == RESULT_OK)
			{
				/* Debug code to be removed after testing; If there is any problem with SD card while
				 * logging then code should reinitialize the logging in the same file as it is not
				 * re-powered (yet to implement)*/
#if DEBUG_MANDATORY == ENABLE
				BMS_Debug_COM_Write_Data("Written\r",8);
#endif
			}
			else
			{
#if DEBUG_MANDATORY == ENABLE
				BMS_Debug_COM_Write_Data("Write Error\r",12);
#endif
			}
			BMS_Debug_COM_Write_Data("MCU Awake\r",10);
			_1Hz_Flag = false;
		}

		/* Check for any request is received from AP; Also check for any data is received from AP which
		 * may be used to update the BMS RTC and GPS timings */
		if(Wakeup_From_Sleep == false)
		{
			Check_AP_Request();
		}
	}
}

