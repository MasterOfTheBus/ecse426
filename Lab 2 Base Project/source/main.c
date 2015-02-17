#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

static volatile uint_fast16_t ticks;

// Define the Kalman filter
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
// Variables to convert voltage to temperature
float step_size = ((3.6)/4096);
float v_25 = 0.76;
float avg_slope = 0.025;

// Reference Temp
float temp_ref = 30;
int LED_count = 0;


int main(){
	ticks = 0;
	float v_sense;
	float temp_C;
	
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
	//GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	
	//Set SysTick to 168MHz/50Hz
	SysTick_Config(SystemCoreClock / 50);
	
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
		
		printf("ADC: %i ", ADC1->DR);

		// Filtering
		if (Kalmanfilter_C(ADC1->DR, &f_output, &kstate) == 0) {
			printf("filtered: %f\n", f_output);
		}
		
		// Convert to temperature
		v_sense = f_output*step_size;
		temp_C = ((v_sense-v_25)/avg_slope)+25;
		printf("v_sense: %f\n", v_sense);
		printf("Temperature: %f C \n", temp_C);
		
		// Temperature display
		if(temp_C <= 40){
			
			if (temp_C-temp_ref >= 2){
				LED_count ++;
				temp_ref += 2;
			}
			if (temp_ref-temp_C >= 2){
				LED_count--;
				temp_ref -= 2;
			}
					
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
		
		// Overheating alarm
		if (temp_C > 40){
			
		}
		
			
		
		
		
		
	}
	return 0;
	
}

void SysTick_Handler() {
	ticks = 1;
}
