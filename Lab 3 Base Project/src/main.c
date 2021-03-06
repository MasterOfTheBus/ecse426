#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "tilt_detection.h"
#include "interrupt.h"
#include "kalman.h"
#include "UI.h"
#include "stm32f4xx_tim.h"
float n=000;

#define ALPHA_ONLY 0 // test for ALPHA angle only

// adjust for any input greater than 360
int adjustInput(int input) {
	return ((input <= 360) ? input : (input % 360));
}

int main(){
	
	float tilt;
	uint8_t angleType = ALPHA;
	uint8_t prevType = angleType;

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
									 LIS302DL_SENSITIVITY_2_3G, // We won't get up to 9 g's of force in this lab, only tilting
																							// sensitivity is the gain of the sensor; average of +1g and -1g direction
																							// adjust raw data by 18mg/digit
									 LIS302DL_SELFTEST_NORMAL); // self test off
																							// when enabled, will generate a defined actuation force
																							// if output signal is within defined parameters, then the sensor is working
	
	InitInterrupt(LIS302DL_SPI_INT1_PIN, LIS302DL_SPI_INT1_GPIO_PORT, EXTI_PortSourceGPIOE, EXTI_PinSource0, EXTI_Line0, EXTI0_IRQn, 0x01, 0x01);
	
	
/**
	*	@brief Sensor Calibration
	*	
	*	- Use reset button to allow user to calibrate sensor
	*/
#if calibrate
	calibrateSensor();
#endif
	
/**
	*	@brief Data Filtering
	*	
	*	- Use Kalman filter to process data
	* - Kalman state variables: q = 0.025
	*														r = 5
	*														x = 0
	*														k = 0
	*														p = 0
	*/		

	kalman_state kstate_X = {0.025, 5, 0, 0, 0};
	kalman_state kstate_Y = {0.025, 5, 0, 0, 0};
	kalman_state kstate_Z = {0.025, 5, 0, 0, 0};

	
/**
	*	@brief Configure GPIO
	*	
	*	- GPIO for 7-segment display
	* - GPIO for on board LED
	*/		
	GPIO_config();	
	
/**
	*	@brief Configure hardware timer
	*	
	*	- Use hardware timer TIM3
	*	-	Define timer parameters
	*	-	Enable hardware interrupt 
	*
	*/	
	Timer_config(	800,								// prescaler
								TIM_CounterMode_Up,
								400, 									// period
								TIM_CKD_DIV1, 
								0); 
	setNumDisplay(n);
	EXTI_GenerateSWInterrupt(EXTI_Line0); // generate an interrupt to initialize the sampling process

	//Collect alpha
	GPIO_WriteBit(GPIOD, GPIO_Pin_13 | GPIO_Pin_15, Bit_SET);
	Keypad_read();

	// store user input in variable and reset user input
	int alphaTilt = adjustInput(getUserInput());
	printf("alphaTilt: %i\n", alphaTilt);
	GPIO_WriteBit(GPIOD, GPIO_Pin_13 | GPIO_Pin_15, Bit_RESET);	

	// Collect beta
	GPIO_WriteBit(GPIOD, GPIO_Pin_12 | GPIO_Pin_14, Bit_SET);
	Keypad_read();

	// store user input in variable and reset user input
	int betaTilt = adjustInput(getUserInput());
	printf("betaTilt: %i\n", betaTilt);
	GPIO_WriteBit(GPIOD, GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET);

	EnableTimerInterrupt();


#if !ALPHA_ONLY
	setAngleDisplay(0);
#endif
	
	while(1){

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

			int8_t upDown;
#if !ALPHA_ONLY
			float inputTilt;
			if (angleType != 2) {
				inputTilt = ((angleType == ALPHA) ? alphaTilt : betaTilt);
				// upDown, 1 for up, -1 for down, 0 for done
				prevType = angleType;
				upDown = tiltCorrection(tilt, inputTilt, &angleType);
				if (upDown == 0) {
					GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_SET);
				}
								
			} else {
				GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_RESET); // toggle an LED to indicate that ALPHA has been found
				angleType = prevType;
				setAngleDisplay(1);
			}
#endif

			// display interrupt
			if (getTimInt()) {
				setTimInt(0);
				
				int count = getTimIntCount();
				if (count > 3) {
					setTimIntCount(0);
				} else {
					setTimIntCount(count+1);
				}
				
				// check if displaying angles or correction animation
				if (getAngleDisplay()) {
					Display(getNumDisplay());

				} else {
					correctionOutput(upDown);
				}
			}
			
	}
	return 0;
}




