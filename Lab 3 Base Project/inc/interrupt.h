#include "stm32f4xx_exti.h"
#include "misc.h"

static int accel_interrupt;

void setITStatus(int set);
int getITStatus(void);
void InitInterrupt(void);
void EXTI0_IRQHandler(void);
