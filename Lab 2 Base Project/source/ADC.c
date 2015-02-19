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

void configInit_ADC(ADC_TypeDef* ADCx, uint32_t periph_ADCx, uint32_t resolution, FunctionalState scanConvMode,
										FunctionalState contConvMode, uint32_t externalTrigConvEdge, uint32_t dataAlign,
										uint8_t nbrOfConversion, uint8_t channel, uint8_t rank, uint8_t sampleTime) {
											
	RCC_APB2PeriphClockCmd(periph_ADCx, ENABLE);
											
	ADC_InitTypeDef adc_init;
		
	adc_init.ADC_Resolution = resolution;
	adc_init.ADC_ScanConvMode = scanConvMode;
	adc_init.ADC_ContinuousConvMode = contConvMode;
	adc_init.ADC_ExternalTrigConvEdge = externalTrigConvEdge;
	adc_init.ADC_DataAlign = dataAlign;
	adc_init.ADC_NbrOfConversion = nbrOfConversion;
	ADC_Init(ADC1, &adc_init); //Initialization
	
	ADC_RegularChannelConfig(ADCx, channel, rank, sampleTime); //Setting Channel and ADC

	ADC_Cmd(ADCx, ENABLE); //Enable Module - Set the ADON bit
}

uint32_t getTemp() {
		// Sampling and converting
		ADC_SoftwareStartConv(ADC1); 														//Starting Conversion - set the SWSTART bit to 1
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 	//Wait until EOC is set
		ADC_ClearFlag (ADC1, ADC_FLAG_EOC); 										//Reset EOC
		return (ADC_GetConversionValue(ADC1));														//Result available in ADC1->DR
}
