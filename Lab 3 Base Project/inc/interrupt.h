/**
   @file interrupt.h
   @brief Configure accelerometer interrupt settings and interrupt handler.
   @date 2015/03/12
*/

#include "stm32f4xx_exti.h"
#include "misc.h"

static int accel_interrupt;

/**
	@brief Set the value of the interrupt flag
	
	@param set The value to set
*/
void setITStatus(int set);

/**
	@brief Get the value of the interrupt flag
	
	@retval The value of the interrupt flag
*/
int getITStatus(void);

/**
	@brief Configuration and Initialization of the external interrupt
*/
void InitInterrupt(void);

/**
	@brief The external interrupt handler
*/
void EXTI0_IRQHandler(void);
