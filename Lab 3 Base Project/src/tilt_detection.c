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
	
	/* Required delay for the MEMS Accelerometre: Turn-on time = 3/Output data Rate 
                                                             = 3/100 = 30ms */
	int i = 0;
  while(i<30) {
		i++;
	}
	
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
					y_mat[i + j * rows] = -1;
				} else if (j >= rows * 4 / 6) {
					y_mat[i + j * rows] = 1;
				} else {
					y_mat[i + j * rows] = 0;
				}
			} else if (i == 1) {
				if (j >= rows * 3 / 6 && j < rows * 4 / 6) {
					y_mat[i + j * rows] = -1;
				} else if (j >= rows * 2 / 6 && j < rows * 4 / 6) {
					y_mat[i + j * rows] = 1;
				} else {
					y_mat[i + j * rows] = 0;
				}
			} else if (i == 2) {
				if (j >= rows / 6 && j < rows * 2 / 6) {
					y_mat[i + j * rows] = -1;
				} else if (j < rows / 6) {
					y_mat[i + j * rows] = 1;
				} else {
					y_mat[i + j * rows] = 0;
				}
			}
		}
	}
}

void calibrateSensor() {
	// initialize all the matrices to use
	arm_mat_init_f32(&calParams, 4, 3, cal_data);
	
	arm_matrix_instance_f32 Y;
	Y.numCols = 3;
	Y.numRows = NUM_CALIBRATION_SAMPLES * 6;
	float y_data[3 * Y.numRows];
	fillYMatrix(y_data, Y.numCols, Y.numRows);
	int i = 0;
	for (; i < Y.numRows; i++) {
		printf("%f %f %f\n", y_data[i * Y.numRows], y_data[i * Y.numRows+1], y_data[i * Y.numRows+2]);
	}
	
	Y.pData = y_data;
	
	arm_matrix_instance_f32 w;
	w.numCols = 4;
	w.numRows = NUM_CALIBRATION_SAMPLES * 6;
	//float w_data[4 * w.numRows];
	float w_data[] = {228, 26, 223, 1, 228, 26, 82, 1, 227, 83, 26, 1, 229, 227, 27, 1, 172, 26, 24, 1, 27, 27, 22, 1};
	
	// collect accelerometer data at 6 positions	
	// Zb down, Zb up, Yb down, Yb up, Xb down, Xb up
#if 0 // the actual code, but test with some dummy values
	int j = 1;
	int i = 0;
	for (; j <= 6; j++) {
		// pause for user to adjust position
		int k = 0;
		printf("adjust position\n");
		
		for (; i < j * NUM_CALIBRATION_SAMPLES; i++) {
			uint8_t data[3];
			getXYZData(data);
		
			int j = 0;
			for (; j < 3; j++) {
				w_data[i] = (float)data[j];
				i++;
			}
			w_data[j] = 1;
		}
	}
#endif
	
	w.pData = w_data;
		
	//-----------least square method to obtain 12 parameters
	// wT
	arm_matrix_instance_f32 w_T;
	float wT_data[4 * NUM_CALIBRATION_SAMPLES * 6];
	arm_mat_init_f32(&w_T, 3, 4, wT_data);
	
	arm_mat_trans_f32(&w, &w_T);
	
	// wT * w
	arm_matrix_instance_f32 temp;
	float temp_data[16]; // 4 * 4 square matrix from w times its transpose
	arm_mat_init_f32(&temp, 4, 4, temp_data);
	arm_mat_mult_f32(&w_T, &w, &temp);	
	
	// [wT * w] inverse
	arm_mat_inverse_f32(&temp, &temp);
	
	// [wT * w] inverse * wT
	arm_mat_mult_f32(&temp, &w_T, &w_T); // the 4x4 times the transpose will result in a matrix with the same dimensions as the transpose
	
	// [wT * w] inverse * wT * Y
	arm_mat_mult_f32(&w_T, &Y, &calParams);
	printf("calParams rows: %i, calParams cols: %i\n", calParams.numRows, calParams.numCols);

	i = 0;
	for (; i < 4; i++) {
		printf("%f %f %f\n", calParams.pData[i * 4], calParams.pData[i * 4 + 1], calParams.pData[i * 4 + 2]);
	}
}

void getXYZData(uint8_t* data) {
	LIS302DL_Read(&data[0], LIS302DL_OUT_X_ADDR, 1);
	LIS302DL_Read(&data[1], LIS302DL_OUT_Y_ADDR, 1);
	LIS302DL_Read(&data[2], LIS302DL_OUT_Z_ADDR, 1);
}

void normalize(uint8_t* data, float* output) {
	float w_data[] = {data[0], data[1], data[2], 1.0};
	arm_matrix_instance_f32 w;
	w.numCols = 4;
	w.numRows = 1;
	w.pData = w_data;
	
	arm_matrix_instance_f32 result;
	arm_mat_mult_f32(&w, &calParams, &result);
	
	output = result.pData;
}

float getTilt(uint8_t type, float xyz[]) {
	int i = 0;
	for (; i < 3; i++) {
		float temp = calcAcceleration(xyz[i]);
		printf("accel: %f\n", temp);
	}
	
	float top;
	float bot;
	if (type == ALPHA) {
		top = xyz[0];
		bot = xyz[1];
	} else if (type == BETA) {
		top = xyz[1];
		bot = xyz[0];
	} else {
		return (-1);
	}
	
	return (atan(top / (sqrt(bot * bot + xyz[2] * xyz[2]))));
}

float calcAcceleration(float input) {
	uint8_t crtl;
	LIS302DL_Read(&crtl, LIS302DL_CTRL_REG1_ADDR, 1);
	// taken from ReadACC of lis302dl.c
	switch(crtl & 0x20) 
    {
    /* FS bit = 0 ==> Sensitivity typical value = 18milligals/digit*/ 
    case 0x00:
        return (int32_t)(LIS302DL_SENSITIVITY_2_3G *  (int8_t)input);
      break;
    /* FS bit = 1 ==> Sensitivity typical value = 72milligals/digit*/ 
    case 0x20:
        return (int32_t)(LIS302DL_SENSITIVITY_9_2G * (int8_t)input);        
      break;
    default:
      break;
    }
		return 0;
}
