/**
 ******************************************************************************
 * @file    Main.c
 * @author  Nikhil Ingale
 * @version V1.0.2
 * @date    25-Nov-2017
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
#include <BMS_Watchdog.h>

#define TEST_DEBUG_GPS_INFO									/* Character A*/
#define TEST_DEBUG_START_TIME								/* Character B*/
#define TEST_DEBUG_ALL_PACK_DATA							/* Character C*/
#define TEST_DEBUG_PACK_CURRENT_ADJ_CD_RATE					/* Character D*/
#define TEST_DEBUG_CAPACITY_USED_REMAINING_TOTAL			/* Character E*/
#define TEST_DEBUG_C_D_TOTAL_PACK_CYLES						/* Character F*/
#define TEST_DEBUG_HEALTH_I2C_ERROR							/* Character G*/
#define TEST_DEBUG_TEMPERATURE								/* Character H*/
#define TEST_DEBUG_WATCHDOG_TEST							/* Character I*/
#define TEST_DEBUG_CODE_RESET								/* Character J*/
//#define TEST_CHARGE_DISCHARGE_SOFTWARE					/* Character K*/
#define TEST_DEBUG_LOG_FILE_INFO							/* Character N*/
#define TEST_DEBUG_STOP_LOG									/* Character O*/

#define TEST_DEBUG_WATCHDOG_RESET_TIME						2100

#define _2_SECONDS_TIME										50		/* Time for which SOC to be shown (50 * 40ms) */
#define _1_SECONDS_TIME										(_2_SECONDS_TIME/2)

const uint8_t BMS_Firmware_Version[3] =
{
		1,			// Major release version--modified when code is being merged to Master branch.
		0,			// Current stable code release-- modified when code is being merged to Develop branch.
		5			// Beta code release--modified when code is being merged to test_develop branch.
};

/* Variable to keep the track of time elapsed when switch is pressed for short duration i.e. 2 seconds */
uint32_t Switch_Press_Time_Count = 0;

/* Variable to keep the track of time when there is no current consumption so as to force the ISL to sleep */
uint32_t BMS_Sleep_Time_Count = 0;

/* Variable to keep the track of time when ISL goes to sleep so as to put MCU in sleep mode */
uint32_t MCU_Sleep_Time_Count = 0;

/* Variable to keep the track of time since charging is started and increment the cycles accordingly */
uint32_t Charge_Time_Count = 0;

/* Variable to keep the track of time since discharging is started and increment the cycles accordingly */
uint32_t Discharge_Time_Count = 0;

/* Variable to avoid multiple increments of the same cycle either charging or discharging */
bool Update_Pack_Cycles = false;

/* Debug code variables definition; Allocate the buffer size only if debugging is to be done */
char Buffer[400];
uint8_t RecData = 0;

/* This variable counts the time for which log was unsuccessful; If it more than 125ms then SD card is
 * reinitialized */
uint8_t Log_Init_Counter = 0 ;

/* These flags are used to indicate the external button has been pressed for more than specified time
 * so that SOC or SOH status can be shown on LEDs */
bool SOC_Flag = false,SOH_Flag = false,Debug_Mode_Function = false;
bool Display_SOH = false,Display_SOC = false;

/* Variable to hold the timing value to force the BMS IC to sleep mode; Values are set using macros defined
 * in BMS_Timing.h file. If MCU is awaken from sleep mode then check load presence for 10 seconds
 * otherwise check load presence for 1 minute(normal operation) */
uint16_t Timer_Value = 0,Time_Count = 0;

/* This flag is used for re-initialization of SD card in case of it's non presence in the slot */
bool SD_Card_ReInit = false;

/* This variable monitors the logging is happening on SD card or not. If it is true then there is no any problem for logging otherwise
 * there is some problem in the logging which will be displayed over USART */
bool Log_Status = false,Log_Stopped = false;

/* These flags are used to test the software logic for charging and discharging pack cycles. These flags will be true only if user sends 'K'
 * or 'L' character over USART. Once it is sent, charge/discharge timers will start counting till the max time, after which it will considered as cycle */
#ifdef TEST_CHARGE_DISCHARGE_SOFTWARE
	bool Start_Charging = false,Start_Discharging = false;
#endif

/* This flag becomes true when all the configuration parameters are written to the EEPROM of ISL ASIC */
bool BMS_Configuration_OK = false;

uint8_t Critical_Batt_V_Counter = 0;

int main(void)
{
	/* Configure the sysTick interrupt to 1mS(default) and Set the NVIC group priority to 4 */
	HAL_Init();

	/* Configure the system clock frequency (Peripherals clock) to 80MHz */
	Set_System_Clock_Frequency();

	/* Delay of 1000 milliSeconds is required to make sure BMS is not polled before it's POR cycle otherwise
	 * BMS I2C will be locked */
	Delay_Millis(1000);

	/* Initialize the timer to 40mS(25Hz) and the same is used to achieve different loop rates */
	BMS_Timers_Init();

	BMS_SOH_SOC_LEDs_Init();

	/* Initialize the USART to 115200 baud rate to debug the code */
	BMS_Debug_COM_Init();

	uint8_t Data[20] = "Hello Bytes\r";
	uint8_t Data1[20] = "Data1 Packet\r";

	while(1)
	{
		BMS_Debug_COM_Read_Data(&RecData,1);

		if(RecData == 'A' && MCU_Power_Mode != REGULAR_POWER_MODE)
		{
			Enter_Normal_Mode();
			BMS_Debug_COM_Write_Data(Data1, 13);
		}
		else if (RecData == 'B' && MCU_Power_Mode != LOW_POWER_MODE)
		{
			Enter_LP_Mode();
			BMS_Debug_COM_Write_Data(Data, 13);
		}

		RecData = 0;

		/* This flag will be true after every 40ms(25Hz) in timer application file */
		if (_25Hz_Flag == true)
		{
			_25Hz_Flag = false;
		}
		/* 1Hz loop which displays the information on USART. It is used for debugging purpose only
		 * (inputs are provided as per the test cases) */
		if(_1Hz_Flag == true)
		{
			_1Hz_Flag = false;
		}
	}
}
