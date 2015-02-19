#include "ADC.h"

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
