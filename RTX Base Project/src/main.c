/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>

#include "kalman.h"
#include "interrupt.h"
#include "tilt_detection.h"

#include "ADC.h"
#include "temperature.h"
#include "timer.h"

#include "UI.h"

#define TILT_SIGNAL (uint32_t)1
#define TEMPERATURE_SIGNAL (uint32_t)2
#define EOC_SIGNAL (uint32_t)4

	// ID for thread
	osThreadId GetTilt_thread;
	osThreadId GetTemp_thread;


// kalman state for temperature
kalman_state kstate = {0.0025, 5.0, 1100.0, 0.0, 0.0};

// kalman state for accelerometer
kalman_state kstate_X = {0.025, 5, 0, 0, 0};
kalman_state kstate_Y = {0.025, 5, 0, 0, 0};
kalman_state kstate_Z = {0.025, 5, 0, 0, 0};

void GetTemp(void const *argument) {
	while (1) {
		osEvent evt = osSignalWait (TEMPERATURE_SIGNAL, osWaitForever);
		if (evt.status == osEventSignal) {
			osSignalClear (GetTemp_thread, TILT_SIGNAL);
			ADC_SoftwareStartConv(ADC1);
			evt = osSignalWait (EOC_SIGNAL, osWaitForever);
			if (evt.status == osEventSignal) {
				osSignalClear (GetTemp_thread, EOC_SIGNAL);
				float f_output;

				// Get Temerature and Filtering
				Kalmanfilter_C((float)ADC_GetConversionValue(ADC1), &f_output, &kstate);

				//printf("filtered: %f\n", f_output);
				// Convert to temperature
				float temp_C = voltage2temp(f_output);
				printf("temp: %f\n", temp_C);
			}
		}
	}
}

void GetTilt(void const *argument) {
	while (1) {
		osEvent evt = osSignalWait (TILT_SIGNAL, osWaitForever);
    if (evt.status == osEventSignal)  {
        osSignalClear (GetTilt_thread, TILT_SIGNAL);

				int32_t xyz[3]; // get the acc data
				LIS302DL_ReadACC(xyz);
				//printf("%i, %i, %i\n", xyz[0], xyz[1], xyz[2]);

				float xyz_float[3];

				normalize(xyz, xyz_float); // normalize the data

				// filter the data
				Kalmanfilter_C(xyz_float[0], &xyz_float[0], &kstate_X); // X
				Kalmanfilter_C(xyz_float[1], &xyz_float[1], &kstate_Y); // Y
				Kalmanfilter_C(xyz_float[2], &xyz_float[2], &kstate_Z); // Z

				float tilt = getTilt(BETA, xyz_float);
				//setNumDisplay(tilt);
				printf("tilt: %f\n", tilt);

		}
	}
}

void ReadKeypad(void const *argument){
	while(1){
		Keypad_read();				// check keypad
		osDelay(100);					
	}
}

void Display7Segment(void const *argument){
	while(1){
	//	Display();
	}
}

osThreadDef(GetTilt, osPriorityNormal, 1, 0);
osThreadDef(GetTemp, osPriorityNormal, 1, 0);
osThreadDef (ReadKeypad, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// ID for thread
	osThreadId ReadKeypad_thread;
	
  // initialize peripherals here
	
	// Accelerometer Configuration
	Accel_InitConfig();
	InitInterrupt();

	//ADC configuration
	Timer_config();
	EnableTimerInterrupt();
	configInit_common_ADC();
	configInit_ADC();
	configInit_ADC_Int();
	
	//Enable temperatur sensor - Set the TSVREFE bit
	ADC_TempSensorVrefintCmd(ENABLE);
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	GetTilt_thread = osThreadCreate(osThread(GetTilt), NULL);

	GetTemp_thread = osThreadCreate(osThread(GetTemp), NULL);
	
	ReadKeypad_thread = osThreadCreate(osThread(ReadKeypad), NULL);
	
	EXTI_GenerateSWInterrupt(EXTI_Line0); // generate an interrupt to initialize the sampling process
	
	osKernelStart ();                         // start thread execution 
}

// Interrupt Handlers

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		osSignalSet (GetTilt_thread, TILT_SIGNAL);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void TIM3_IRQHandler(){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		osSignalSet (GetTemp_thread, TEMPERATURE_SIGNAL);

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void ADC_IRQHandler() {
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
		osSignalSet (GetTemp_thread, EOC_SIGNAL);
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}
