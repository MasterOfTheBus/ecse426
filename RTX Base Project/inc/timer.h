#include "stm32f4xx_conf.h"

static int TIM3_interrupt;

void Timer_config(void);					
void EnableTimerInterrupt(void);
void setTimInt(int val);
int getTimInt(void);
void TIM3_IRQHandler(void);
