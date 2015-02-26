#include "interrupt.h"

void InitInterrupt() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_Line0);
	
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = EXTI_Line0; // line 0
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt; // configuring an interrupt
	exti_init.EXTI_Trigger = EXTI_Trigger_Rising; // rising for active high
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);
	
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = EXTI0_IRQn; // exti line 0
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0; // highest priority
	nvic_init.NVIC_IRQChannelSubPriority = 0; // highest priority
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
}
