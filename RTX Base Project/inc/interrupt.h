/**
   @file interrupt.h
   @brief Configure accelerometer interrupt settings and interrupt handler.
   @date 2015/03/12
*/

#include "stm32f4xx_exti.h"
#include "misc.h"

/** @var
		@brief The flag indicating that an interrupt has occured.
	*/
static int interrupt;

/** @var
		@brief Variable specifying which exti line to check for the status bit
	*/
static uint32_t exti_line;

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
	Sets the @ref interrupt flag
*/
void EXTI0_IRQHandler(void);
