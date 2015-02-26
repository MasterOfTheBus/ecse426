#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "tilt_detection.h"
#include "interrupt.h"

int main(){

	//------------------Configuation and Initialization----------------------------
	uint8_t temp;
	Accel_InitConfig(LIS302DL_LOWPOWERMODE_ACTIVE, // power on the mems sensor
									 LIS302DL_DATARATE_100, // Data rate at 100 Hz as specified
									 LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE,  // Enable all the axes
									 LIS302DL_SENSITIVITY_2_3G, // We won't get up to 9 g's of force in this lab
									 temp); // self test?
	
	InitInterrupt();
	
	while(1){
		// interrupt
		if (EXTI_GetFlagStatus(EXTI_Line0) == SET) {
			EXTI_ClearFlag(EXTI_Line0);
			
			// sample
			int xyz[3];
			getTilt(ALPHA, xyz);
			
			printf("x: %i, y: %i, z: %i\n", xyz[0], xyz[1], xyz[2]);
			
			// logic
		}
	}
	
	return 0;
}
