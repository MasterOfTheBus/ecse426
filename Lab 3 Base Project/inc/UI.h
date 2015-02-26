#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"

/**
	@brief Initialize the TIM Time Base Init struct
	
	@params Prescaler
	@params Counter mode
	@params Period
	@params Clock division
	@params Repetition counter
	*/


void Timer_config(uint16_t Prescaler,
									uint16_t CounterMode,
									uint32_t Period,
									uint16_t ClockDivision,
									uint8_t RepetitionCounter);