#include "ADC.h"
#include "misc.h"
void configInit_common_ADC() {
	
	ADC_CommonInitTypeDef adc_common_init;
	adc_common_init.ADC_Mode = ADC_Mode_Independent;
	adc_common_init.ADC_Prescaler = ADC_Prescaler_Div2;
	adc_common_init.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	adc_common_init.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&adc_common_init); //Initialization
}

void configInit_ADC() {
											
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
											
	ADC_InitTypeDef adc_init;
		
	adc_init.ADC_Resolution = ADC_Resolution_12b;
	adc_init.ADC_ScanConvMode = DISABLE;
	adc_init.ADC_ContinuousConvMode = DISABLE;
	adc_init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//adc_init.ADC_ExternalTrigConv = externalTrigConv;
	adc_init.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &adc_init); //Initialization
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); //Setting Channel and ADC

	ADC_Cmd(ADC1, ENABLE); //Enable Module - Set the ADON bit
											
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}
										
void configInit_ADC_Int() {
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = ADC_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0x01;
	nvic_init.NVIC_IRQChannelSubPriority = 0x01;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
}

uint32_t getTemp() {
		// Sampling and converting
		ADC_SoftwareStartConv(ADC1); 														//Starting Conversion - set the SWSTART bit to 1
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 	//Wait until EOC is set
		ADC_ClearFlag (ADC1, ADC_FLAG_EOC); 										//Reset EOC
		return (ADC_GetConversionValue(ADC1));									//Result available in ADC1->DR
}
