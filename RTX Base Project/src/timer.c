#include "timer.h"

void Timer_config(){
	//Enable Timer3									
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
										
	//Timer configuration
	// 42Mhz / (1000 * 840) = 50Hz
	TIM_TimeBaseInitTypeDef timer_init;
	timer_init.TIM_Prescaler = 1000;
	timer_init.TIM_CounterMode = TIM_CounterMode_Up;
	timer_init.TIM_Period = 840;
	timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
	timer_init.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3, &timer_init);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // enable interrupt
}
									
void EnableTimerInterrupt(){
	TIM3_interrupt = 0;
	
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = TIM3_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0x02;
	nvic_init.NVIC_IRQChannelSubPriority = 0x02;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
}


void setTimInt(int val) {
	TIM3_interrupt = val;
}

int getTimInt(void) {
	return (TIM3_interrupt);
}

//void TIM3_IRQHandler(){
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
//		TIM3_interrupt = 1;

//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//	}
//}
