/**
   @file timer.h
   @brief Configure Hardware Timer parameters for the ADC component.
   @date 2015/03/23
*/

#include "stm32f4xx_conf.h"

/** @var The timer interrupt flag*/
static int TIM3_interrupt;

/**
	@brief Configure the timer for 50 Hz operation
	
	Frequency is Bus frequency / (prescaler * period)
	For a frequency of 50 Hz on a 42 MHz bus,	prescaler is then 1000, and period is 840
	*/
void Timer_config(void);

/**
	@brief Configure and enable the NVIC component to handle the Timer interrupt
	
	Has the second highest priority since the sampling frequency of temperature is lower than accelerometer sampling
	*/
void EnableTimerInterrupt(void);

/**
	@brief Set the value of the timer interrupt flag
	
	@param val The value to set
	*/
void setTimInt(int val);

/**
	@brief Get the value of the timer interrupt flag
	
	@retval int The value of the timer interrupt flag
	*/
int getTimInt(void);

/**
	@brief Sets the value of the interrupt flag or signals the timer thread
	*/
void TIM3_IRQHandler(void);
