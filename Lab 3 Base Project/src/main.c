#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "tilt_detection.h"
#include "interrupt.h"
#include "kalman.h"
#include "UI.h"
#include "stm32f4xx_tim.h"

int main(){

/**
	*	@brief Tilt detection using STM32F407VG (Accelerometer version M8997B)
	*	
	*	- Measure tilt angles in 2 dimensions (roll around the x-axis and pitch around the y-axis)
	*	- Real time measurements with at least 100 data points per second (frequency = 100Hz, period = 0.01s)
	*	- MEMS driver for M8997B : LIS302DL (can be found under Accelerometer_Drivers)
	*/
	
	//------------------Configuation and Initialization----------------------------
	Accel_InitConfig(LIS302DL_LOWPOWERMODE_ACTIVE, // power on the mems sensor
									 LIS302DL_DATARATE_100, // Data rate at 100 Hz as specified
									 LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE,  // Enable all the axes
									 LIS302DL_SENSITIVITY_2_3G, // We won't get up to 9 g's of force in this lab
									 LIS302DL_SELFTEST_NORMAL); // self test?
	
	InitInterrupt();
	
	
/**
	*	@brief Sensor Calibration
	*	
	*	- Use reset button to allow user to calibrate sensor
	*/
	calibrateSensor();
	
/**
	*	@brief Data Filtering
	*	
	*	- Use Kalman filter to process data
	* - Kalman state variables: q = 
	*														r = 
	*														x =
	*														k =
	*														p =
	*/		

	kalman_state kstate_X = {1,1,0,0,0};
	kalman_state kstate_Y = {1,1,0,0,0};
	kalman_state kstate_Z = {1,1,0,0,0};


/**
	*	@brief User interface
	*	
	*	- Read user input from keypad	(pins right to left)
	*																PE6(black)	PC13(purple)	PE4(blue)		PE5(green)	PB4(yellow)		PB5(orange)		PD1(red)	PD2(brown)
	*	- Translate numbers to be displayed into control signals
	*	- Send signals out through 7-segment display (pins from right to left)
	*																PB11(black)	PB12(purple)	PB13(blue)	PB14(green)	PB15(yellow)	PD8(orange)		PD9(red)	PD10(brown)
	*																PE7(black)	PE8(purple)		PE9(blue)		PE10(green)	PE11(yellow)	PE12(orange)	PE13(red)	PE14(brown)															
	*/
	Timer_config(	40000,								// currently set at default
								TIM_CounterMode_Up,
								500, 
								TIM_CKD_DIV1, 
								0x0000); 

	// configure I/O for 7-segment display
	configInit_GPIO(GPIOB, RCC_AHB1Periph_GPIOB,
									GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_DOWN);
									
	configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
									GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_DOWN);

	configInit_GPIO(GPIOE, RCC_AHB1Periph_GPIOE,
									GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_DOWN);
	
	EXTI_GenerateSWInterrupt(EXTI_Line0); // generate an interrupt to initialize the process
	
	while(1){
			if (getITStatus()) {
				setITStatus(0);

				uint8_t xyz[3];
				float xyz_fl[3];
				getXYZData(xyz);
				printf("%i, %i, %i\n", xyz[0], xyz[1], xyz[2]);

				float f_xyz[3];

				Kalmanfilter_C(xyz_fl[0], &f_xyz[0], &kstate_X); // X
				Kalmanfilter_C(xyz_fl[1], &f_xyz[1], &kstate_Y); // Y
				Kalmanfilter_C(xyz_fl[2], &f_xyz[2], &kstate_Z); // Z
				
				normalize(xyz, xyz_fl);
				printf("%f, %f, %f\n", xyz_fl[0], xyz_fl[1], xyz_fl[2]);

				//float tilt = getTilt(ALPHA, f_xyz);

			}

		Display();

	}

	return 0;
}

/**
	*	@brief Interrupt Handler
	*	
	*	- MEMs sensor controled interrupts
	*	- Used to control:	when data is ready
	*											when 7-segment display should be updated
	*/
