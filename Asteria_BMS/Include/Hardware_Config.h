/*
 * Hardware_Config.h
 *
 *  Created on: 17-Jan-2017
 *      Author: NIKHIL
 */

#ifndef HARDWARE_CONFIG_H_
#define HARDWARE_CONFIG_H_

#define BMS_V1											1
//#define BMS_V2										2
#define BMS_VERSION										BMS_V1

#if BMS_VERSION != BMS_V1
	#undef BMS_VERSION
#endif

#define UNUSED(x) ((void)(x))

#if BMS_VERSION == BMS_V1
	#include "stm32l4xx.h"
	#include "stm32l4xx_hal.h"
	#include "stm32l4xx_it.h"

#define DISABLE											0
#define ENABLE											!DISABLE

/********************************************* GPIO Related Macros ****************************************************/
#define BOARD_LED					GPIO_PIN_3
#define SD_CARD_CS					GPIO_PIN_4
#define SD_CARD_DETECT				GPIO_PIN_2

/********************************************* Timer Related Macros ****************************************************/
#define _100uS_PRESCALAR								7999

#define PWM_FUNCTION									DISABLE
#define TIM1_PWM_PRESCALAR								1000000
#define TIM2_PWM_PRESCALAR								1000000

#define NUM_OF_TIMERS									4

#define TIM1_HANDLE_INDEX								0
#define TIM2_HANDLE_INDEX								1
#define TIM15_HANDLE_INDEX								2
#define TIM16_HANDLE_INDEX								3

#define NUM_OF_PWM_TIMERS								4
#define TIM1_CHANNEL1									ENABLE
#define TIM1_CHANNEL2									DISABLE
#define TIM1_CHANNEL3									DISABLE
#define TIM1_CHANNEL4									DISABLE

#define TIM2_CHANNEL1									DISABLE
#define TIM2_CHANNEL1_REMAP								DISABLE

#define TIM2_CHANNEL2									DISABLE
#define TIM2_CHANNEL2_REMAP								DISABLE

#define TIM2_CHANNEL3									DISABLE
#define TIM2_CHANNEL4									DISABLE

#define TIM15_CHANNEL1									DISABLE
#define TIM15_CHANNEL2									DISABLE

#define TIM16_CHANNEL1									DISABLE

#define TIMER2_INT_MODE									ENABLE
#define TIMER2_PRIORITY									0
#define TIMER2_SUBPRIORITY								0

#define TIMER6_INT_MODE									ENABLE
#define TIMER6_PRIORITY									1
#define TIMER6_SUBPRIORITY								0

#define TIMER7_INT_MODE									ENABLE
#define TIMER7_PRIORITY									2
#define TIMER7_SUBPRIORITY								0


/********************************************* USART Related Macros ****************************************************/
#define NUM_OF_USARTS									2

#define USART1_REMAP									DISABLE
#define USART1_INT_MODE									ENABLE
#define USART1_PRIORITY									3
#define USART1_SUBPRIORITY								0

#define USART2_INT_MODE									ENABLE
#define USART2_PRIORITY									4
#define USART2_SUBPRIORITY								0

#define USART1_HANDLE_INDEX								0
#define USART2_HANDLE_INDEX								1


/********************************************* I2C Related Macros ****************************************************/
#define NUM_OF_I2C_BUSES								2

#define I2C1_REMAP										DISABLE
#define I2C1_INT_MODE									DISABLE
#define I2C1_PRIOIRTY									5
#define I2C1_SUBPRIOIRTY								0

#define I2C3_REMAP										DISABLE
#define I2C3_INT_MODE									DISABLE
#define I2C3_PRIOIRTY									6
#define I2C3_SUBPRIOIRTY								0

#define I2C1_HANDLE_INDEX								0
#define I2C3_HANDLE_INDEX								1

#define I2C_100KHZ										0x90942027
#define I2C_400KHZ										0x90310309
#define I2C_1MHZ										0x40200204
#define I2C_DATA_TIMEOUT								100

/********************************************* SPI Related Macros ****************************************************/

#define NUM_OF_SPI_BUSES								2

#define SPI1_REMAP										DISABLE
#define SPI1_PRIORITY									7
#define SPI1_SUBPRIORITY								0
#define SPI1_HANDLE_INDEX								0

/*
 * SPI1_CLK  PA1
 * SPI1_CLK  PA5
 * SPI1_CLK  PB3
 *
 * SPI1_MISO PA6
 * SPI1_MISO PA11
 * SPI1_MISO PAB4
 *
 * SPI1_MOSI PA7
 * SPI1_MOSI PA12
 * SPI1_MOSI PB5
 *
 * */
#if SPI1_REMAP == DISABLE
	#define SPI1_CLK										GPIO_PIN_5
	#define SPI1_MISO										GPIO_PIN_6
	#define SPI1_MOSI										GPIO_PIN_7
#elif SPI1_REMAP == ENABLE
	#define SPI1_CLK										GPIO_PIN_5
	#define SPI1_MISO										GPIO_PIN_11
	#define SPI1_MOSI										GPIO_PIN_7
#endif

#define SPI3_REMAP										DISABLE
#define SPI3_PRIORITY									8
#define SPI3_SUBPRIORITY								0
#define SPI3_HANDLE_INDEX								1
#define SPI3_HANDLE_INDEX								1

#define SPI1_POLLING_MODE								ENABLE
#define SPI1_INT_MODE									DISABLE
#define SPI1_DMA_MODE									DISABLE

#define SPI3_POLLING_MODE								DISABLE
#define SPI3_INT_MODE									DISABLE
#define SPI3_DMA_MODE									DISABLE

#define SPI_DATA_TIMEOUT								3000


#else
#define I2C_100KHZ										0
#define I2C_400KHZ										0
#define I2C_1MHZ										0

#endif

enum Results
{
	RESULT_ERROR = 0,RESULT_OK = 1
};

#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"


#endif /* HARDWARE_CONFIG_H_ */
