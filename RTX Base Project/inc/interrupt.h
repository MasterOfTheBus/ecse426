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
	
	@param GPIO_pin The GPIO Pin to use
	@param GPIO_port The GPIO Port to use
	@param exti_port_src Specifies the GPIO port to configure the EXTI module for
	@param exti_pin_src Specifies the GPIO pin to configure the EXTI module for
	@param exti_line The EXTI line to use for the interrupt generation
	@param nvic_channel The NVIC channel to use to pass the interrupt
	@param priority The priority this interrupt will have
	@param subpriority The subpriority the interrupt will have in its priority group
*/
void InitInterrupt(uint32_t GPIO_pin,
									 GPIO_TypeDef * GPIO_port,
									 uint8_t exti_port_src,
									 uint8_t exti_pin_src,
									 uint32_t exti_line,
									 uint8_t nvic_channel,
									 uint8_t priority,
									 uint8_t subpriority);

/**
	@brief The external interrupt handler
	Sets the @ref interrupt flag
*/
void EXTI0_IRQHandler(void);
