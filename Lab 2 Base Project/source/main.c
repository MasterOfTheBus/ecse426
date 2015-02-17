#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

static volatile uint_fast16_t ticks;

typedef struct kalman_state kalman_state;
struct kalman_state {
	float q; // process noise covariance
	float r; // measurement noise covariance
	float x; // estimated value
	float p; // estimation error covariance
	float k; // adaptive Kalman filter gain
};

int Kalmanfilter_C(float input, float* output, kalman_state* kstate);

int Kalmanfilter_C (float input, float* output, kalman_state* kstate) {
	kstate->p = kstate->p + kstate->q;
	kstate->k = kstate->p / (kstate->p + kstate->r);
	kstate->x = kstate->x + kstate->k * (input - kstate->x);
	if (kstate->x != kstate->x) { // check for overflow, underflow
		return 1;
	}
	kstate->p = (1 - kstate->k) * kstate->p;
	*output = kstate->x;
	return 0;
}

int main(){
	ticks = 0;
	
	//Initialize the kalman state
	kalman_state kstate = {0.5, 0.5, 0.5, 0.5, 0.5};
	
	//Enable GPIO clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//GPIO configuration
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
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
	
	//Set SysTick to 168MHz/50Hz
	SysTick_Config(SystemCoreClock / 50);
	int counter;
	counter=1;
	
	//Enable temperatur sensor
	ADC_TempSensorVrefintCmd(ENABLE);
	
	while(1){
		float f_output;
		
		while (!ticks); 	//Waiting for interrupt
		ticks = 0;				//Reset tick
		
		// Sampling and converting
		ADC_SoftwareStartConv(ADC1); 														//Starting Conversion, waiting for it to finish, clearing the flag, reading the result
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 	//Wait until EOC is set
		ADC_ClearFlag (ADC1, ADC_FLAG_EOC); 										//Reset EOC
		ADC_GetConversionValue(ADC1);														//Result available in ADC1->DR
		
		if (Kalmanfilter_C(ADC1->DR, &f_output, &kstate) == 0) {
			printf("filtered: %f\n", f_output);
		}
		
		printf("%i ------ count: %i\n", ADC1->DR, counter);
		counter++;
	}
	
	return 0;
}

void SysTick_Handler() {
	ticks = 1;
}
