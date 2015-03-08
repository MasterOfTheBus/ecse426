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

void getXYZData(uint8_t data[]) {
	LIS302DL_Read(&data[0], LIS302DL_OUT_X_ADDR, 1);
	LIS302DL_Read(&data[1], LIS302DL_OUT_Y_ADDR, 1);
	LIS302DL_Read(&data[2], LIS302DL_OUT_Z_ADDR, 1);
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
