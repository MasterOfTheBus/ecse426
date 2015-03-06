#include "stm32f4xx_exti.h"
#include "misc.h"

static volatile int accel_interrupt;

void InitInterrupt(void);
void EXTI0_IRQHandler(void);
