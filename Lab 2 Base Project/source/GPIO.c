#include "GPIO.h"

void configInit_GPIO(GPIO_TypeDef* GPIOx,
										 uint32_t periph_GPIOx,
										 uint32_t pins,
										 GPIOMode_TypeDef mode,
										 GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType,
										 GPIOPuPd_TypeDef puPd) {
	
	//Enable GPIO clock
	RCC_AHB1PeriphClockCmd(periph_GPIOx, ENABLE);
	
	//GPIO configuration
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = pins;
	gpio.GPIO_Mode = mode;
	gpio.GPIO_Speed = speed;
	gpio.GPIO_OType = oType;
	gpio.GPIO_PuPd = puPd;
	
	GPIO_Init(GPIOx, &gpio);
}

void DisplaySingleLED(int LED_count) {
	if (LED_count % 4 == 0){
				GPIO_SetBits(GPIOD, GPIO_Pin_12);
				GPIO_ResetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			}
			if (LED_count % 4 == 1){
				GPIO_SetBits(GPIOD, GPIO_Pin_13);
				GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
			}
			if (LED_count % 4 == 2){
				GPIO_SetBits(GPIOD, GPIO_Pin_14);
				GPIO_ResetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_15);
			}
			if (LED_count % 4 == 3){
				GPIO_SetBits(GPIOD, GPIO_Pin_15);
				GPIO_ResetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_12);
			}
}
