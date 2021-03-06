#include "interrupt.h"

void setITStatus(int set) {
	interrupt = set;
}

int getITStatus() {
	return interrupt;
}

void InitInterrupt(uint32_t GPIO_pin,
									 GPIO_TypeDef * GPIO_port,
									 uint8_t exti_port_src,
									 uint8_t exti_pin_src,
									 uint32_t EXTI_line,
									 uint8_t nvic_channel,
									 uint8_t priority,
									 uint8_t subpriority) {
	interrupt = 0;
	exti_line = EXTI_line;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Configure GPIO PINs to detect Interrupts */
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_pin; // The pin to read interrupt data from; the accelerometer in this case
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; // input from GPIO
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // floating
  GPIO_Init(GPIO_port, &GPIO_InitStructure);
  
	SYSCFG_EXTILineConfig(exti_port_src, exti_pin_src); // the interrupt port is GPIOE
	
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = EXTI_line; // line 0
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt; // configuring an interrupt
	exti_init.EXTI_Trigger = EXTI_Trigger_Rising; // rising for active high
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);
	
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = nvic_channel; // exti line 0 matching exti
	nvic_init.NVIC_IRQChannelPreemptionPriority = priority; // high priority
	nvic_init.NVIC_IRQChannelSubPriority = subpriority; // high priority
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);	
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(exti_line) != RESET) {
		interrupt = 1;
		EXTI_ClearITPendingBit(exti_line);
	}
}
