#include "config_init.h"

void configInit_GPIO(GPIO_TypeDef* GPIOx, uint32_t periph_GPIOx, uint32_t pins,
										 GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType, GPIOPuPd_TypeDef puPd) {
	
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

void configInit_common_ADC(uint32_t mode, uint32_t prescaler,
													 uint32_t DMAAccessMode, uint32_t twoSamplingDelay) {
	
	ADC_CommonInitTypeDef adc_common_init;
	adc_common_init.ADC_Mode = mode;
	adc_common_init.ADC_Prescaler = prescaler;
	adc_common_init.ADC_DMAAccessMode = DMAAccessMode;
	adc_common_init.ADC_TwoSamplingDelay = twoSamplingDelay;
	ADC_CommonInit(&adc_common_init); //Initialization
}
/*
void configInit_ADC() {
	ADC_InitTypeDef adc_init;
}*/
