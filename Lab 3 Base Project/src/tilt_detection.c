#include "tilt_detection.h"
#include "math.h"

#if calibrate
#include <stdio.h>
#endif

void Accel_InitConfig(uint8_t Power,
											uint8_t DataRate,
											uint8_t Axes,
											uint8_t FullScale,
											uint8_t SelfTest) {
												
	LIS302DL_InitTypeDef accel_init;
	accel_init.Power_Mode = Power;
  accel_init.Output_DataRate = DataRate;
  accel_init.Axes_Enable = Axes;
  accel_init.Full_Scale = FullScale;
	accel_init.Self_Test = SelfTest;
	LIS302DL_Init(&accel_init);
	
	uint8_t ctrl = 0x04;
	LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);
												
#if !calibrate
	arm_mat_init_f32(&calParams, 4, 3, cal_data);
#endif
}

#if calibrate
void calculateParameters(arm_matrix_instance_f32 w, arm_matrix_instance_f32 Y) {
	//-----------least square method to obtain 12 parameters
	// wT
	arm_matrix_instance_f32 w_T;
	float wT_data[24]; // 4 * 6 matrix
	arm_mat_init_f32(&w_T, 4, 6, wT_data);
	
	arm_mat_trans_f32(&w, &w_T);

	// wT * w
	arm_matrix_instance_f32 multTrans;
	float multTrans_data[16]; // 4 * 4 square matrix from w times its transpose
	arm_mat_init_f32(&multTrans, 4, 4, multTrans_data);
	arm_mat_mult_f32(&w_T, &w, &multTrans);	

	// [wT * w] inverse
	arm_matrix_instance_f32 inverse;
	float inverse_data[16]; // inverse keeps same dimensions
	arm_mat_init_f32(&inverse, 4, 4, inverse_data);
	arm_mat_inverse_f32(&multTrans, &inverse);

	// [wT * w] inverse * wT
	arm_matrix_instance_f32 multInverse;
	float mult_inv_data[24]; // 4x4 multiplied by 4x6 results in a 4x4
	arm_mat_init_f32(&multInverse, 4, 6, mult_inv_data);
	arm_mat_mult_f32(&inverse, &w_T, &multInverse); // the 4x4 times the transpose will result in a matrix with the same dimensions as the transpose

	// [wT * w] inverse * wT * Y
	arm_mat_mult_f32(&multInverse, &Y, &calParams);
	
	int i = 0;
	for (; i < 12; i++) {
		printf("%f ", calParams.pData[i]);
		if ((i+1) % 3 == 0) {
			printf("\n");
		}
	}
	printf("\n");
}

void calibrateSensor() {
	// initialize all the matrices to use
	arm_mat_init_f32(&calParams, 4, 3, cal_data);
	
	// normalized vectors at 6 positions
	arm_matrix_instance_f32 Y;
	float y_data[] = {0, 0, 1,
										0, 0, -1,
										0, 1, 0,
										0, -1, 0,
										1, 0, 0,
										-1, 0, 0};
	arm_mat_init_f32(&Y, 6, 3, y_data);
	
	arm_matrix_instance_f32 w;

	// Zb down, Zb up, Yb down, Yb up, Xb down, Xb up
	// previously acquired calibration data
	float w_data[] = {-504, 468, 1494, 1,
										-504, 486, -594, 1,
										-522, -522, 414, 1,
										-540, 1476, 396, 1,
										-1512, 468, 396, 1,
										486, 486, 432, 1};

	arm_mat_init_f32(&w, 6, 4, w_data);
	
	calculateParameters(w, Y);
}
#endif

void normalize(int32_t* data, float* output) {
	float w_data[] = {data[0], data[1], data[2], 1.0};
	arm_matrix_instance_f32 w;
	arm_mat_init_f32(&w, 1, 4, w_data);
	
	arm_matrix_instance_f32 result;
	arm_mat_init_f32(&result, 1, 3, output);
	arm_mat_mult_f32(&w, &calParams, &result);
}

float getTilt(uint8_t type, float xyz[]) {
	float top;
	float bot;
	if (type == ALPHA) { // pitch
		top = xyz[0];
		bot = xyz[1];
	} else if (type == BETA) { // roll
		top = xyz[1];
		bot = xyz[0];
	} else {
		return (-1);
	}
	
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
		// indicate a new angle to correct for
		if (*currentTiltType == ALPHA) {
			*currentTiltType = BETA;
		} else {
			*currentTiltType = 2;
		}
		return 0;
	} else {
		if (inputTilt > tilt) {
			return ((inputTilt - tilt <= 180) ? 1 : -1);
		} else {
			return ((tilt - inputTilt <= 180) ? -1 : 1);
		}
	}
}
