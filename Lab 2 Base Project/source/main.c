#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "kalman.h"
#include "temperature.h"

/**
	* @brief A flag indicating whether Systick interrupt has occured
	*/
static volatile uint_fast16_t ticks;

// Variables to convert voltage to temperature
float v_sense;
float temp_C;

// Reference Temp
float temp_ref = 34;
#define threshold_temp 20
int LED_count = 0;

// PWM
int period = 15000; // 0.02s => 168000000*0.02 pulses

int main(){
	ticks = 0;
	int counter = 0;
	int up = 1;
	float duty_cycle = 0.0;
	
	//Initialize the kalman state
	kalman_state kstate = {0.0025, 5.0, 1100.0, 0.0, 0.0};
	
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
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); //Setting Channel and ADC

	ADC_Cmd(ADC1, ENABLE); //Enable Module - Set the ADON bit
	
	//Enable temperatur sensor - Set the TSVREFE bit
	ADC_TempSensorVrefintCmd(ENABLE);
	
	//Set SysTick to 168MHz/50Hz
	SysTick_Config(SystemCoreClock / 50);
	
	while(1){
		if (ticks) {
			ticks = 0;				//Reset tick
			float f_output;
		
			// Sampling and converting
			ADC_SoftwareStartConv(ADC1); 														//Starting Conversion - set the SWSTART bit to 1
			while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 	//Wait until EOC is set
			ADC_ClearFlag (ADC1, ADC_FLAG_EOC); 										//Reset EOC
			ADC_GetConversionValue(ADC1);														//Result available in ADC1->DR
		
			printf("ADC: %i ", ADC1->DR);

			// Filtering
			if (Kalmanfilter_C(ADC1->DR, &f_output, &kstate) == 0) {
				printf("filtered: %f\n", f_output);
			} else {
				continue;
			}
		
			// Convert to temperature
			v_sense = (f_output*step_size);
			temp_C = voltage2temp(f_output);
			printf("v_sense: %f\n", v_sense);
			printf("Temperature: %f C \n", temp_C);
		}
		counter++;
		// Temperature display
		if(temp_C <= threshold_temp){
			
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
		if (temp_C > threshold_temp){
			printf("counter: %i      duty cycle: %f\n", counter, duty_cycle);
			
			/*if (counter == 0) {
				GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			} else if (counter == duty_cycle*period) {
				GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			} else if (counter >= period) {*/
				counter = -1;
				if (up) {
					if (duty_cycle < 1.0) {
						duty_cycle+=0.1;
					} else {
						up = 0;
					}
				} else {
					if (duty_cycle > 0) {
						duty_cycle-=0.1;
					} else {
						up = 1;
					}
				}
			//}

			float i;
			GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			for(i=0.0;i<(period*duty_cycle);i++);
			GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			for(i=0.0;i<((period)*(1.0-(duty_cycle)));i++);
		}
	}
	return 0;
	
}

/**
	* @brief Set the value of ticks to 1 to indicate the start of a new period.
	*/
void SysTick_Handler() {
	ticks = 1;

}
