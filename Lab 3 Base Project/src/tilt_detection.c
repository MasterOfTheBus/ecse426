#include "tilt_detection.h"
#include "math.h"
#include <stdio.h>

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
}

void fillYMatrix(float* y_mat, int cols, int rows) {
	int i = 0;
	for (; i < cols; i++) {
		int j = 0;
		for (; j < rows; j++) {
			if (i == 0) {
				if (j >= rows * 5 / 6) {
					y_mat[i + j * cols] = -1;
				} else if (j >= rows * 4 / 6) {
					y_mat[i + j * cols] = 1;
				} else {
					y_mat[i + j * cols] = 0;
				}
			} else if (i == 1) {
				if (j >= rows * 3 / 6 && j < rows * 4 / 6) {
					y_mat[i + j * cols] = -1;
				} else if (j >= rows * 2 / 6 && j < rows * 4 / 6) {
					y_mat[i + j * cols] = 1;
				} else {
					y_mat[i + j * cols] = 0;
				}
			} else if (i == 2) {
				if (j >= rows / 6 && j < rows * 2 / 6) {
					y_mat[i + j * cols] = -1;
				} else if (j < rows / 6) {
					y_mat[i + j * cols] = 1;
				} else {
					y_mat[i + j * cols] = 0;
				}
			}
		}
	}
}

void calculateParameters(arm_matrix_instance_f32 w, arm_matrix_instance_f32 Y) {
	//-----------least square method to obtain 12 parameters
	// wT
	arm_matrix_instance_f32 w_T;
	float wT_data[4 * NUM_CALIBRATION_SAMPLES * 6];
	arm_mat_init_f32(&w_T, 4, NUM_CALIBRATION_SAMPLES * 6, wT_data);
	
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
	float mult_inv_data[4 * NUM_CALIBRATION_SAMPLES * 6]; // inverse keeps same dimensions
	arm_mat_init_f32(&multInverse, 4, NUM_CALIBRATION_SAMPLES * 6, mult_inv_data);
	arm_mat_mult_f32(&inverse, &w_T, &multInverse); // the 4x4 times the transpose will result in a matrix with the same dimensions as the transpose

	// [wT * w] inverse * wT * Y
	arm_mat_mult_f32(&multInverse, &Y, &calParams);

	
	printf("params\n");
	int i = 0;
	for (; i < 12; i++) {
		printf("%f ", calParams.pData[i]);
		if ((i+1) %3==0) {
			printf("\n");
		}
	}
	printf("\n");
}

void calibrateSensor() {
	// initialize all the matrices to use
	arm_mat_init_f32(&calParams, 4, 3, cal_data);
	
	arm_matrix_instance_f32 Y;
	float y_data[3 * NUM_CALIBRATION_SAMPLES * 6];
	fillYMatrix(y_data, 3, 6);
	arm_mat_init_f32(&Y, 6, 3, y_data);
	
	arm_matrix_instance_f32 w;

	//float w_data[4 * NUM_CALIBRATION_SAMPLES * 6];
	float w_data[] = {-504, 468, 1494, 1,
										-504, 486, -594, 1,
										-522, -522, 414, 1,
										-540, 1476, 396, 1,
										-1512, 468, 396, 1,
										486, 486, 432, 1};
	
	// collect accelerometer data at 6 positions	
	// Zb down, Zb up, Yb down, Yb up, Xb down, Xb up
#if 0 // the actual code, but test with some dummy values
	int j = 1;
	int i = 0;
	for (; j <= 6; j++) {
		// TODO: pause for user to adjust position
		printf("adjust position\n");
		
		for (; i < j * NUM_CALIBRATION_SAMPLES; i++) {
			int32_t data[3];
			LIS302DL_ReadACC(data);
		
			int k = 0;
			for (; k < 3; k++) {
				w_data[i] = (float)data[k];
				i++;
			}
			w_data[i] = 1;
		}
	}
#endif

	arm_mat_init_f32(&w, NUM_CALIBRATION_SAMPLES * 6, 4, w_data);
	
	calculateParameters(w, Y);
}

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
	
	if (xyz[2] > 0) {
		if (degrees < 0) {
			degrees += 360;
		}
	} else if (xyz[2] < 0) {
			degrees = 180 - degrees;
	}
	
	return (degrees);
}
