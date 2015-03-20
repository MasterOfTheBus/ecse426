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
	@brief Configuration and Initialization of the external interrupt specifically for the accelerometer
	
	The GPIO pin used is Pin 0 on Port E corresponding to the sensor's INT1 pin
	The GPIO input mode is set to GPIO_Mode_IN since the processor will be reading the interrupt from the sensor.
	Since the GPIO pin used is pin 0, the External Interrupt line is correspondingly EXTI_Line0.
	The interrupt signal is rising.
	The priority of the interrupt is set to 0x03 since this sampling rate is the fastest.
*/
void InitInterrupt(void);

/**
	@brief The external interrupt handler
	Sets the interrupt flag.
	Or it can also signal the thread that will sample the accelerometer data
*/
void EXTI0_IRQHandler(void);
