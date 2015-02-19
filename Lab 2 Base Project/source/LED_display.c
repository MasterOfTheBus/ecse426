#include "LED_display.h"

void DisplaySingleLED(int LED_count, GPIO_TypeDef* GPIOx) {
	if (LED_count % 4 == 0){
				GPIO_SetBits(GPIOx, GPIO_Pin_12);
				GPIO_ResetBits(GPIOx, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
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