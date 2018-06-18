/*
 * Power_Management.c
 *
 *  Created on: 10-Apr-2017
 *      Author: NIKHIL
 */
#include <Power_Management.h>
#include <BMS_Data_Log.h>
#include <BMS_GPIOs.h>
#include <BMS_ASIC.h>
#include <BMS_Serial_Communication.h>
#include <BMS_Timing.h>

/* Variable which becomes true only when MCU wake up from sleep mode either from Vref pin or
 * from external switch press */
volatile bool Wakeup_From_Sleep = false,Sleep_Mode = false;
uint8_t Reset_Source = 0xFF;

uint8_t MCU_Power_Mode = REGULAR_POWER_MODE;

uint8_t Sleep_Mode_Entered = false;

void MCU_Enter_Sleep_Mode()
{
#ifdef BMS_VERSION
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_PWR_CLK_ENABLE();

	RCC->AHB1SMENR = 0x0;
	RCC->AHB2SMENR = 0x0;
	RCC->AHB3SMENR = 0x0;

	RCC->APB1SMENR1 = 0x0;
	RCC->APB1SMENR2 = 0x0;
	RCC->APB2SMENR = 0x0;

	GPIO_InitStruct.Pin = MCU_WAKEUP_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode = WAKEUP_EDGE;

	/* Enable GPIOA clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	NVIC_SetPriority((IRQn_Type) (EXTI9_5_IRQn), 0x03);
	HAL_NVIC_EnableIRQ((IRQn_Type) (EXTI9_5_IRQn));

	/* Reduce the System clock to below 2 MHz */
	SystemClock_Decrease();

	HAL_SuspendTick();

	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#endif
}

void MCU_Exit_Sleep_Mode()
{
#ifdef BMS_VERSION
    HAL_PWREx_DisableLowPowerRunMode();
    /* Reinitialize all the peripherals as they were disabled before MCU going to sleep; But sleep mode holds
     * the global variables values to their previous state as before going to sleep */
	/* Configure the sysTick interrupt to 1mS(default) and Set the NVIC group priority to 4 */
	HAL_Init();

	/* Configure the system clock frequency (Peripherals clock) to 80MHz */
	Set_System_Clock_Frequency();

	/* Initialize the timer to 40mS and the same is used to achieve different loop rates */
	BMS_Timers_Init();

	/* Initialize the Status and Error LEDs connected on the BMS board */
	BMS_Status_Error_LED_Init();

	/* Initialize the status LEDs which indicates the SOC and SOH */
	if(Debug_COM_Enable == false)
	{
		BMS_SOH_SOC_LEDs_Init();
	}
	{
		/* Initialize the USART to 115200 baud rate to debug the code */
		BMS_Debug_COM_Init();
	}

	/* Configure the switch as input to wake up the BMS in case of sleep and same will be used
	 * to show the SOC and SOH on status LEDs*/
	BMS_Switch_Init();

	/* Configure the ISL94203 I2C communication to 100KHz */
	BMS_ASIC_Init();

#endif
}

static void SystemPower_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStruct;

	/* Enable Power Clock */
    __HAL_RCC_PWR_CLK_ENABLE();

	/* Enable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	/* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
	/* Note: Debug using ST-Link is not possible during the execution of this   */
	/*       example because communication between ST-link and the device       */
	/*       under test is done through UART. All GPIO pins are disabled (set   */
	/*       to analog input mode) including  UART I/O pins.           */
	GPIO_InitStruct.Pin = GPIO_PIN_All;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/* Disable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
}

void Enter_Normal_Mode(void)
{
	if(MCU_Power_Mode != REGULAR_POWER_MODE)
	{
		_25Hz_Flag = false;

		MCU_Power_Mode = REGULAR_POWER_MODE;

		USART_Reset(USART_1);

		/* Check whether regulator is in main mode or low power mode. As soon as
		 * MCU enters the main mode, regular operation should be initiated */
		HAL_PWREx_DisableLowPowerRunMode();

		/* Configure the system clock frequency (Peripherals clock) to 80MHz */
		Set_System_Clock_Frequency();

		/* Delay of 100 milliSeconds is required to make sure BMS is not polled before it's POR cycle otherwise
		 * BMS I2C will be locked */
		Delay_Millis(100);

		/* Initialize the timer to 40mS(25Hz) and the same is used to achieve different loop rates */
		BMS_Timers_Init();

		/* Initialize the status LEDs which indicates the SOC and SOH */
		if(Debug_COM_Enable == false)
		{
			BMS_SOH_SOC_LEDs_Init();
		}
		{
			/* Initialize the USART to 115200 baud rate to debug the code */
			BMS_Debug_COM_Init();
		}

		BMS_Log_Init();

		/* Configure the ISL94203 I2C communication to 100KHz */
		BMS_ASIC_Init();

		/* Resume Tick interrupt if disabled prior to Low Power Run mode entry */
//		HAL_ResumeTick();
	}
}

void Enter_LP_Mode(void)
{
	if(MCU_Power_Mode != LOW_POWER_MODE)
	{
		_25Hz_Flag = false;

		Stop_Log();

		MCU_Power_Mode = LOW_POWER_MODE;

		USART_Reset(USART_1);

		/* Configure the system Power */
		SystemPower_Config();

		/* Reduce the System clock to below 2 MHz */
		SystemClock_Decrease();

		/* Suspend Tick increment for power consumption purposes */
//		HAL_SuspendTick();

		/* Initialize the timer to 40mS(25Hz) and the same is used to achieve different loop rates */
		BMS_Timers_Init();

		/* Initialize the status LEDs which indicates the SOC and SOH */
//		BMS_SOH_SOC_LEDs_Init();

		/* Initialize the USART to 115200 baud rate to debug the code */
		BMS_Debug_COM_Init();

		/* Enter LP RUN Mode */
		HAL_PWREx_EnableLowPowerRunMode();
	}
}


void SystemClock_Decrease(void)
{
#if BMS_VERSION == BMS_V1
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};

	/* MultiSpeedInternal clock select */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;

	/* MSI = 1MHz for low power mode of MCU */
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_1;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2
	   clocks dividers */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

	/* Disable HSI to reduce power consumption since MSI is used from that point */
	__HAL_RCC_HSI_DISABLE();
#endif
}

void Set_System_Clock_Frequency(void)
{
#ifdef BMS_VERSION
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };

	/* MSI is enabled after System reset, activate PLL with MSI as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	/* The peripheral frequency is set to Frequency = ((RCC_MSI_RANGE * PLLN)/PLLR) */
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 8;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV8;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;

	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);

#endif
}

uint8_t Get_Reset_Source()
{
#ifdef BMS_VERSION
	if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == SET)
	{
		Reset_Source = WATCHDOG;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) == SET)
	{
		Reset_Source = SOFTWARE;
	}

	__HAL_RCC_CLEAR_RESET_FLAGS();

#endif
	return Reset_Source;
}
/* ISR which handles the wake up of MCU from sleep mode and resumes the operation from where it
 * had left off */
void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(MCU_WAKEUP_PIN);
  /* Once MCU is awaken either by external switch press or by Vref of BMS IC, resume the operation of
   * MCU where it had left off. This flag makes sure that this sequence is not repeated unless it
   * is triggered by external event again */
  if(Wakeup_From_Sleep == false && Sleep_Mode == true)
  {
	  MCU_Exit_Sleep_Mode();
	  Wakeup_From_Sleep = true;
	  Sleep_Mode = false;
  }
}

