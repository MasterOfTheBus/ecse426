#include "timer.h"

void Timer_config(uint16_t Prescaler,
									uint16_t CounterMode,
									uint32_t Period,
									uint16_t ClockDivision,
									uint8_t RepetitionCounter){
	//Enable Timer3									
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
										
	//Timer configuration
	TIM_TimeBaseInitTypeDef timer_init;
	timer_init.TIM_Prescaler = Prescaler;
	timer_init.TIM_CounterMode = CounterMode;
	timer_init.TIM_Period = Period;
	timer_init.TIM_ClockDivision = ClockDivision;
	timer_init.TIM_RepetitionCounter = RepetitionCounter;
	
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

void TIM3_IRQHandler(){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		TIM3_interrupt = 1;

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}