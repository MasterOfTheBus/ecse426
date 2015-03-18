#include "stm32f4xx_conf.h"

static int TIM3_interrupt;

void Timer_config(uint16_t Prescaler,
									uint16_t CounterMode,
									uint32_t Period,
									uint16_t ClockDivision,
									uint8_t RepetitionCounter);					
void EnableTimerInterrupt(void);
void setTimInt(int val);
int getTimInt(void);
void TIM3_IRQHandler(void);
