/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>

//#include "thread.h"

#include "kalman.h"
#include "interrupt.h"
#include "tilt_detection.h"

//#include "ADC.h"
//#include "temperature.h"
//#include "timer.h"

#define TILT_SIGNAL (uint32_t)1

	// ID for thread
	osThreadId GetTilt_thread;
//osThreadId GetTemp_thread;

// kalman state for temperature
//kalman_state kstate = {0.0025, 5.0, 1100.0, 0.0, 0.0};

// kalman state for accelerometer
kalman_state kstate_X = {0.025, 5, 0, 0, 0};
kalman_state kstate_Y = {0.025, 5, 0, 0, 0};
kalman_state kstate_Z = {0.025, 5, 0, 0, 0};

/*void GetTemp(void const *argument) {
	while (1) {
		if (getTimInt()) {
			setTimInt(0);				//Reset tick
			float f_output;
			
			// Get Temerature and Filtering
			Kalmanfilter_C((float)getTemp(), &f_output, &kstate);

			printf("filtered: %f\n", f_output);
			// Convert to temperature
			float temp_C = voltage2temp(f_output);
			printf("temp: %f\n", temp_C);
		}
		//osDelay(20);
	}
}*/

void GetTilt(void const *argument) {
	while (1) {
		osEvent evt = osSignalWait (TILT_SIGNAL, osWaitForever);
    if (evt.status == osEventSignal)  {
        osSignalClear (GetTilt_thread, TILT_SIGNAL);

				int32_t xyz[3]; // get the acc data
				LIS302DL_ReadACC(xyz);
				printf("%i, %i, %i\n", xyz[0], xyz[1], xyz[2]);

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
		
		//osDelay(10);
	}
}

osThreadDef(GetTilt, osPriorityNormal, 1, 0);
//osThreadDef(GetTemp, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
//	// ID for thread
//	osThreadId GetTilt_thread;
//	osThreadId GetTemp_thread;
	
  // initialize peripherals here
	Accel_InitConfig(LIS302DL_LOWPOWERMODE_ACTIVE, // power on the mems sensor
									 LIS302DL_DATARATE_100, // Data rate at 100 Hz as specified
									 LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE,  // Enable all the axes
									 LIS302DL_SENSITIVITY_2_3G, // We won't get up to 9 g's of force in this lab, only tilting
																							// sensitivity is the gain of the sensor; average of +1g and -1g direction
																							// adjust raw data by 18mg/digit
									 LIS302DL_SELFTEST_NORMAL); // self test off
																							// when enabled, will generate a defined actuation force
																							// if output signal is within defined parameters, then the sensor is working
	
	InitInterrupt(LIS302DL_SPI_INT1_PIN,
								LIS302DL_SPI_INT1_GPIO_PORT,
								EXTI_PortSourceGPIOE,
								EXTI_PinSource0,
								EXTI_Line0,
								EXTI0_IRQn,
								0x01, 0x02);

//	Timer_config(	1000,								// prescaler
//								TIM_CounterMode_Up,
//							  280, 									// period
//								TIM_CKD_DIV1, 
//								0);
//	EnableTimerInterrupt();
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	GetTilt_thread = osThreadCreate(osThread(GetTilt), NULL);
	if (GetTilt_thread == NULL) {
		printf("failed to create\n");
	}
	//GetTemp_thread = osThreadCreate(osThread(GetTemp), NULL);
	
	EXTI_GenerateSWInterrupt(EXTI_Line0); // generate an interrupt to initialize the sampling process
	
	osKernelStart ();                         // start thread execution 
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		osSignalSet (GetTilt_thread, TILT_SIGNAL);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
