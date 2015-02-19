#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "GPIO.h"
#include "ADC.h"
#include "kalman.h"
#include "temperature.h"

/**
	* @brief A flag indicating whether Systick interrupt has occured
	*/
static volatile uint_fast16_t ticks;

#define threshold_temp 45

// PWM
int period = 13000; // overall period of the duty cycle

int main(){
	ticks = 0; // Indicate an iterrupt
	int up = 1; // Indicate whether to increment or decrement the duty cycle
	float duty_cycle = 0.0; // the percentage of the "period" to count to
	float temp_ref = 34.0; // Reference Temp
	int LED_count = 0; // Choose which LED to enable
	
	// Variables to convert voltage to temperature
	float temp_C;
	
	//Initialize the kalman state
	kalman_state kstate = {0.0025, 5.0, 1100.0, 0.0, 0.0};
	
	//GPIO configuration
	configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
									GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_NOPULL);
	
	//ADC configuration
	configInit_common_ADC(ADC_Mode_Independent, ADC_Prescaler_Div2,
												ADC_DMAAccessMode_Disabled, ADC_TwoSamplingDelay_5Cycles);
	
	configInit_ADC(ADC1, RCC_APB2Periph_ADC1, ADC_Resolution_12b, DISABLE, DISABLE,
								 ADC_ExternalTrigConvEdge_None, ADC_DataAlign_Right, 1, ADC_Channel_16,
								 1, ADC_SampleTime_480Cycles);
	
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

			// Filtering
			if (Kalmanfilter_C(ADC1->DR, &f_output, &kstate) != 0) {
				continue;
			}
		
			// Convert to temperature
			temp_C = voltage2temp(f_output);
		}
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

			DisplaySingleLED(LED_count, GPIOD);
		}
		
		// Overheating alarm
		if (temp_C > threshold_temp){
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
