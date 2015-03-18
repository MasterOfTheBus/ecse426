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

// kalman state for temperature
kalman_state kstate = {0.0025, 5.0, 1100.0, 0.0, 0.0};

// kalman state for accelerometer
kalman_state kstate_X = {0.025, 5, 0, 0, 0};
kalman_state kstate_Y = {0.025, 5, 0, 0, 0};
kalman_state kstate_Z = {0.025, 5, 0, 0, 0};

void Blinky_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}

void Blinky(void const *argument){
	while(1){
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		printf("hello world\n");
		osDelay(250);
	}
}

void GetTemp(void const *argument) {
	while (1) {
		if (getTimInt()) {
			 setTimInt(0);				//Reset tick
			float f_output;
			
			// Get Temerature and Filtering
			if (Kalmanfilter_C(getTemp(), &f_output, &kstate) != 0) {
				continue;
			}
			printf("filtered: %f\n", f_output);
			// Convert to temperature
			float temp_C = voltage2temp(f_output);
			printf("temp: %f\n", temp_C);
		}
	}
}

void GetTilt(void const *argument) {
	while (1) {

		if (getITStatus()) {
				setITStatus(0);

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
		osDelay(250);
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


osThreadDef(Blinky, osPriorityNormal, 1, 0);
osThreadDef(GetTilt, osPriorityNormal, 1, 0);
osThreadDef (ReadKeypad, osPriorityNormal, 1, 0);


/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// ID for thread
	osThreadId	Blinky_thread;
	osThreadId GetTilt_thread;
	osThreadId ReadKeypad_thread;
	
  // initialize peripherals here
	Blinky_GPIO_Init();

	Accel_InitConfig(LIS302DL_LOWPOWERMODE_ACTIVE, // power on the mems sensor
									 LIS302DL_DATARATE_100, // Data rate at 100 Hz as specified
									 LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE,  // Enable all the axes
									 LIS302DL_SENSITIVITY_2_3G, // We won't get up to 9 g's of force in this lab, only tilting
																							// sensitivity is the gain of the sensor; average of +1g and -1g direction
																							// adjust raw data by 18mg/digit
									 LIS302DL_SELFTEST_NORMAL); // self test off
																							// when enabled, will generate a defined actuation force
																							// if output signal is within defined parameters, then the sensor is working
	
	InitInterrupt(LIS302DL_SPI_INT1_PIN, LIS302DL_SPI_INT1_GPIO_PORT, EXTI_PortSourceGPIOE, EXTI_PinSource0, EXTI_Line0, EXTI0_IRQn, 0x01, 0x01);
	
	EXTI_GenerateSWInterrupt(EXTI_Line0); // generate an interrupt to initialize the sampling process
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	Blinky_thread = osThreadCreate(osThread(Blinky), NULL);
	//GetTilt_thread = osThreadCreate(osThread(GetTilt), NULL);
	
	ReadKeypad_thread = osThreadCreate(osThread(ReadKeypad), NULL);
	
	osKernelStart ();                         // start thread execution 
}


