#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

int main(){
	//Enable GPIO clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//GPIO configuration
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd =GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOD, &gpio);
	
	//ADC configuration
	ADC_InitTypeDef adc_init;
	ADC_CommonInitTypeDef adc_common_init;
	adc_common_init.ADC_Mode = ADC_Mode_Independent;
	adc_common_init.ADC_Prescaler = ADC_Prescaler_Div2;
	adc_common_init.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	adc_common_init.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&adc_common_init); //Initialization
	
	adc_init.ADC_Resolution = ADC_Resolution_12b;
	adc_init.ADC_ScanConvMode = DISABLE;
	adc_init.ADC_ContinuousConvMode = DISABLE;
	adc_init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc_init.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &adc_init); //Initialization
	
	ADC_Cmd(ADC1, ENABLE); //Enable Module
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); //Setting Channel and ADC
	
	//Turn on LEDs
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	
	while(1){
	}
	
	return 0;
}
