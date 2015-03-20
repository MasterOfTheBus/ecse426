#include "tilt_detection.h"
#include "math.h"

void Accel_InitConfig() {
												
	LIS302DL_InitTypeDef accel_init;
	accel_init.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE; // power on the mems sensor
  accel_init.Output_DataRate = LIS302DL_DATARATE_100; // Data rate at 100 Hz as specified
  accel_init.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE; // Enable all the axes
  accel_init.Full_Scale = LIS302DL_SENSITIVITY_2_3G; // We won't get up to 9 g's of force in this lab, only tilting
																							// sensitivity is the gain of the sensor; average of +1g and -1g direction
																							// adjust raw data by 18mg/digit
	accel_init.Self_Test = LIS302DL_SELFTEST_NORMAL; // self test off
																							// when enabled, will generate a defined actuation force
																							// if output signal is within defined parameters, then the sensor is working
	LIS302DL_Init(&accel_init);
	
	uint8_t ctrl = 0x04; // Set the value of control register 3 in order to setup interrupts on a data ready signal
	LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);
												
	arm_mat_init_f32(&calParams, 4, 3, cal_data);
}

void normalize(int32_t* data, float* output) {
	float w_data[] = {data[0], data[1], data[2], 1.0}; // create the raw data vector [x, y, z, 1]
	arm_matrix_instance_f32 w;
	arm_mat_init_f32(&w, 1, 4, w_data);
	
	arm_matrix_instance_f32 result;
	arm_mat_init_f32(&result, 1, 3, output);
	arm_mat_mult_f32(&w, &calParams, &result); // multiply raw data by the normalization matrix
}

float getTilt(uint8_t type, float xyz[]) {
	float top;
	float bot;
	if (type == ALPHA || type == 2) { // pitch
		top = xyz[0];
		bot = xyz[1];
	} else if (type == BETA) { // roll
		top = xyz[1];
		bot = xyz[0];
	} else {
		return (-1);
	}
	
	// calculate the degrees based on the formula
	float degrees = (atan(top / (sqrt(bot * bot + xyz[2] * xyz[2]))) * 180 / PI);
	
	// adjust degrees for quadrant
	if (xyz[2] > 0) {
		if (degrees < 0) {
			degrees += 360;
		}
	} else if (xyz[2] < 0) {
			degrees = 180 - degrees;
	}
	
	return (degrees);
}

int8_t tiltCorrection(float tilt, float inputTilt, uint8_t* currentTiltType) {
	if (inputTilt - tilt <= 5 && inputTilt - tilt >= -5) {
		// indicate which angle to correct for
		if (*currentTiltType == ALPHA) {
			*currentTiltType = BETA;
		} else {
			*currentTiltType = 2;
		}
		return 0;
	} else {
		/*
			If the difference between the angles is less than 180 degrees, rotate up (1)
			Else rotate down (-1)
		
			Check which tilt angle is larger first
		*/
		if (inputTilt > tilt) {
			return ((inputTilt - tilt <= 180) ? 1 : -1);
		} else {
			return ((tilt - inputTilt <= 180) ? -1 : 1);
		}
	}
}
