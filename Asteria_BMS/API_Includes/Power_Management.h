/*
 * Power_Management.h
 *
 *  Created on: 07-Apr-2017
 *      Author: NIKHIL
 */

#ifndef POWER_MANAGEMENT_H_
#define POWER_MANAGEMENT_H_

#include <Hardware_Config.h>

/* Variable which becomes true as soon as MCU enters the sleep mode. So that after waking up from
 * sleep it can start from the point where it left off */
extern volatile bool Wakeup_From_Sleep,Sleep_Mode;
extern uint8_t Reset_Source;
extern uint8_t Sleep_Mode_Entered;
extern uint8_t MCU_Power_Mode;

enum ResetSources
{
	SOFTWARE = 0, WATCHDOG, HARDWARE
};

enum Power_Modes
{
	REGULAR_POWER_MODE = 0, LOW_POWER_MODE
};

void MCU_Sleep_Mode_Init();
void MCU_Enter_Sleep_Mode();
void MCU_Exit_Sleep_Mode();
void SystemClock_Decrease(void);
void Set_System_Clock_Frequency(void);
uint8_t Get_Reset_Source();
void Enter_Normal_Mode(void);
void Enter_LP_Mode(void);

#endif /* POWER_MANAGEMENT_H_ */
