/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>

#include "kalman.h"
#Include "tilt_detection.h"

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

void AccelerationGet(void const *argument) {
	while (1) {
		if (getITStatus()) {
				setITStatus(0);

				int32_t xyz[3]; // get the acc data
				LIS302DL_ReadACC(xyz);
				//printf("%i, %i, %i\n", xyz[0], xyz[1], xyz[2]);
				
				float xyz_float[3];

				normalize(xyz, xyz_float); // normalize the data
				
				float f_xyz[3];

				// filter the data
				Kalmanfilter_C(xyz_float[0], &f_xyz[0], &kstate_X); // X
				Kalmanfilter_C(xyz_float[1], &f_xyz[1], &kstate_Y); // Y
				Kalmanfilter_C(xyz_float[2], &f_xyz[2], &kstate_Z); // Z

				tilt = getTilt(angleType, f_xyz);
				setNumDisplay(tilt);
				printf("tilt: %f\n", tilt);

		}
	}
}

void AccelerationDisplay(void const *argument) {
	
}

osThreadDef(Blinky, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
int main (void) {
	// ----------------------Initialization----------------------
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
	
	// kalman state for accelerometer
	kalman_state kstate_X = {0.025, 5, 0, 0, 0};
	kalman_state kstate_Y = {0.025, 5, 0, 0, 0};
	kalman_state kstate_Z = {0.025, 5, 0, 0, 0};
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// ID for thread
	osThreadId	Blinky_thread;
	
  // initialize peripherals here
	Blinky_GPIO_Init();
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	Blinky_thread = osThreadCreate(osThread(Blinky), NULL);
	
	osKernelStart ();                         // start thread execution 
}


