#include "interrupt.h"
#include "lis302dl.h"
#include <stdio.h>

void setITStatus(int set) {
	accel_interrupt = set;
}

int getITStatus() {
	return accel_interrupt;
}

void InitInterrupt() {
	accel_interrupt = 0;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Configure GPIO PINs to detect Interrupts */
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_INT1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; // input from GPIO
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // match sample rate
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // by default read 0, not floating
  GPIO_Init(LIS302DL_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
  
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
	
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = EXTI_Line0; // line 0
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt; // configuring an interrupt
	exti_init.EXTI_Trigger = EXTI_Trigger_Rising; // rising for active high
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);
	
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = EXTI0_IRQn; // exti line 0
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0x01; // highest priority
	nvic_init.NVIC_IRQChannelSubPriority = 0x01; // highest priority
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);	
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		accel_interrupt = 1;

		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
