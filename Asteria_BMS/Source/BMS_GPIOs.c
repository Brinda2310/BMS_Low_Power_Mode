/*
 * BMS_GPIOs.c
 *
 *  Created on: 26-Apr-2017
 *      Author: NIKHIL
 */

#include <BMS_GPIOs.h>
#include <BMS_Serial_Communication.h>
#include <BMS_ASIC.h>

/**
 * @brief  Function to initialize the switch functionality connected to respective GPIO of MCU
 * @param  None
 * @retval None
 */
void BMS_Switch_Init()
{
	GPIO_Init(BMS_SWITCH_PORT,BMS_SWITCH,GPIO_INPUT,PULLUP);
}

/**
 * @brief  Function to read the switch status. Switch is pulled up with internal resistor,
 * 		   If pressed pin status will be LOW and if not pressed it will remain to high state
 * @param  None
 * @retval PRESSED		: Switch is pressed
 * 		   NOT_PRESSED	: Switch is not pressed
 */
uint8_t BMS_Read_Switch_Status()
{
	if(GPIO_Read(BMS_SWITCH_PORT,BMS_SWITCH) == PIN_HIGH)
	{
		return PRESSED;
	}
	else
	{
		return NOT_PRESSED;
	}
}

/**
 * @brief  Function to initialize the LEDs connected to respective GPIOs of MCU
 * @param  None
 * @retval None
 */
void BMS_SOH_SOC_LEDs_Init()
{
	GPIO_Init(LED1_PORT, LED_1, GPIO_OUTPUT, NOPULL);
//	GPIO_Init(LED2_PORT, LED_2, GPIO_OUTPUT, NOPULL);
//	GPIO_Init(LED3_PORT, LED_3, GPIO_OUTPUT, NOPULL);
	GPIO_Init(LED4_PORT, LED_4, GPIO_OUTPUT, NOPULL);

	GPIO_Write(LED1_PORT, LED_1, PIN_HIGH);
//	GPIO_Write(LED2_PORT, LED_2, PIN_HIGH);
//	GPIO_Write(LED3_PORT, LED_3, PIN_HIGH);
	GPIO_Write(LED4_PORT, LED_4, PIN_HIGH);
}

/**
 * @brief  Function to initialize the status and Error LEDs connected on board
 * @param  None
 * @retval None
 */
void BMS_Status_Error_LED_Init()
{
#if BOARD_STATUS_ERROR_LED == ENABLE
	GPIO_Init(LED5_PORT, LED_5, GPIO_OUTPUT, NOPULL);
	GPIO_Init(LED6_PORT, LED_6, GPIO_OUTPUT, NOPULL);

	GPIO_Write(LED5_PORT, LED_5, PIN_HIGH);
	GPIO_Write(LED6_PORT, LED_6, PIN_HIGH);
#endif
}

/**
 * @brief  Function to toggle the status and Error LEDs connected on board
 * @param  None
 * @retval None
 */
void BMS_Status_Error_LED_Toggle()
{
#if BOARD_STATUS_ERROR_LED == ENABLE
	GPIO_Write(LED1_PORT, LED_1, PIN_TOGGLE);
	GPIO_Write(LED4_PORT, LED_4, PIN_TOGGLE);
	GPIO_Write(LED5_PORT, LED_5, PIN_TOGGLE); // Red LED
	GPIO_Write(LED6_PORT, LED_6, PIN_TOGGLE);
#endif
}

/**
 * @brief  Function to show the LED pattern upon switch press; It shows SOC and SOH based
 * 		   on time for which switch is pressed
 * @param  Pattern_Type		: Type of pattern to be shown on LEDs (SOC/SOH)
 * @param  Status			: Type of status (HIDE_STATUS/SHOW_STATUS)
 * @retval None
 */
void BMS_Show_LED_Pattern(uint8_t Pattern_Type,uint8_t Status)
{
	float Battery_Health = 0.0;

	static uint8_t Blink_Count = 0;

	if(Pattern_Type == SOC && Status == SHOW_STATUS)
	{
		Battery_Health = Get_BMS_Capacity_Remaining();

		if(Battery_Health >= 15.0f)
		{
			GPIO_Write(LED6_PORT,LED_6,PIN_LOW);
		}
		else if(Battery_Health < 15.0f)
		{
			if(Blink_Count >= 10)
			{
				Blink_Count = 0;
				GPIO_Write(LED6_PORT,LED_6,PIN_TOGGLE);
			}
			else
			{
				Blink_Count++;
			}
		}

		if(Battery_Health >= 35.0f)
		{
			GPIO_Write(LED5_PORT,LED_5,PIN_LOW);
		}
		else if (Battery_Health >= 25.0f && Battery_Health < 35.0f)
		{
			if(Blink_Count >= 10)
			{
				Blink_Count = 0;
				GPIO_Write(LED5_PORT,LED_5,PIN_TOGGLE);
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Battery_Health < 25.0f)
		{
			GPIO_Write(LED5_PORT,LED_5,PIN_HIGH);
		}

		if(Battery_Health >= 55.0f)
		{
			GPIO_Write(LED4_PORT,LED_4,PIN_LOW);
		}
		else if (Battery_Health >= 45.0f && Battery_Health < 55.0f)
		{
			if(Blink_Count >= 10)
			{
				Blink_Count = 0;
				GPIO_Write(LED4_PORT,LED_4,PIN_TOGGLE);
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Battery_Health < 45.0f)
		{
			GPIO_Write(LED4_PORT,LED_4,PIN_HIGH);
		}

		if(Battery_Health >= 75.0f)
		{
			GPIO_Write(LED1_PORT,LED_1,PIN_LOW);
		}
		else if (Battery_Health >= 65.0f && Battery_Health < 75.0f)
		{
			if(Blink_Count >= 10)
			{
				Blink_Count = 0;
				GPIO_Write(LED1_PORT,LED_1,PIN_TOGGLE);
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Battery_Health < 65.0f)
		{
			GPIO_Write(LED1_PORT,LED_1,PIN_HIGH);
		}
	}
	else if (Pattern_Type == SOH && Status == SHOW_STATUS)
	{
		uint8_t Num_Cycles = Get_BMS_Num_Charge_Cycles();

		if(Num_Cycles < 20)
		{
			GPIO_Write(LED4_PORT,LED_4,PIN_LOW);
		}
		else if (Num_Cycles >= 20 && Num_Cycles <40)
		{
			if(Blink_Count >= 10)
			{
				GPIO_Write(LED4_PORT, LED_4, PIN_TOGGLE);
				Blink_Count = 0;
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Num_Cycles >= 40)
		{
			GPIO_Write(LED4_PORT,LED_4,PIN_HIGH);
		}


		if(Num_Cycles < 60)
		{
			GPIO_Write(LED1_PORT,LED_1,PIN_LOW);
		}
		else if (Num_Cycles >= 60 && Num_Cycles < 80)
		{
			if(Blink_Count >= 10)
			{
				GPIO_Write(LED1_PORT, LED_1, PIN_TOGGLE);
				Blink_Count = 0;
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Num_Cycles >= 80)
		{
			GPIO_Write(LED1_PORT,LED_1,PIN_HIGH);
		}

		if(Num_Cycles < 100)
		{
			GPIO_Write(LED5_PORT,LED_5,PIN_LOW);
		}
		else if (Num_Cycles >= 100 && Num_Cycles < 120)
		{
			if(Blink_Count >= 10)
			{
				GPIO_Write(LED5_PORT, LED_5, PIN_TOGGLE);
				Blink_Count = 0;
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Num_Cycles >= 120)
		{
			GPIO_Write(LED5_PORT,LED_5,PIN_HIGH);
		}

		if(Num_Cycles < 140)
		{
			GPIO_Write(LED6_PORT,LED_6,PIN_LOW);
		}
		else if (Num_Cycles >= 140 && Num_Cycles < 160)
		{
			if(Blink_Count >= 10)
			{
				GPIO_Write(LED6_PORT, LED_6, PIN_TOGGLE);
				Blink_Count = 0;
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Num_Cycles >= 160)
		{
			GPIO_Write(LED6_PORT,LED_6,PIN_HIGH);
		}

		if(Num_Cycles < 180)
		{
			GPIO_Write(LED6_PORT,LED_6,PIN_LOW);
		}
		else if (Num_Cycles >= 180 && Num_Cycles < 200)
		{
			if(Blink_Count >= 10)
			{
				GPIO_Write(LED6_PORT, LED_6, PIN_TOGGLE);
				Blink_Count = 0;
			}
			else
			{
				Blink_Count++;
			}
		}
		else if (Num_Cycles >= 200)
		{
			if(Blink_Count >= 2)
			{
				Blink_Count = 0;
				GPIO_Write(LED6_PORT,LED_6,PIN_TOGGLE);
			}
			else
			{
				Blink_Count++;
			}
		}
	}

	if(Status == HIDE_STATUS)
	{
		GPIO_Write(LED1_PORT, LED_1, PIN_HIGH);
		GPIO_Write(LED5_PORT, LED_5, PIN_HIGH);
		GPIO_Write(LED4_PORT, LED_4, PIN_HIGH);
		GPIO_Write(LED6_PORT, LED_6, PIN_HIGH);
	}
}
