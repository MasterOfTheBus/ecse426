#include "stm32f4xx.h"                  // Device header

void configInit_GPIO(GPIO_TypeDef* GPIOx, uint32_t periph_GPIOx, uint32_t pins,
										 GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType, GPIOPuPd_TypeDef puPd);
void DisplaySingleLED(int LED_num, GPIO_TypeDef* GPIOx);
