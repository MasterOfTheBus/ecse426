#include "tilt_detection.h"
#include "math.h"

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
}

int getTilt(uint8_t type) {
	uint8_t buffer_num[6];
	uint8_t buffer_denom[6];
	uint8_t buffer_z[6];
	uint8_t crtl;

	LIS302DL_Read(&crtl, LIS302DL_CTRL_REG1_ADDR, 1);	
	LIS302DL_Read(buffer_z, LIS302DL_OUT_Z_ADDR, 6);
	if (type == ALPHA) {
		LIS302DL_Read(buffer_num, LIS302DL_OUT_X_ADDR, 6);
		LIS302DL_Read(buffer_denom, LIS302DL_OUT_Y_ADDR, 6);
	} else if (type == BETA) {
		LIS302DL_Read(buffer_num, LIS302DL_OUT_Y_ADDR, 6);
		LIS302DL_Read(buffer_denom, LIS302DL_OUT_X_ADDR, 6);
	} else {
		return (-1);
	}
	

}

int32_t calcAcceleration(uint8_t *buffer, uint8_t crtl) {
#if 0
	switch(crtl & 0x20) 
    {
    /* FS bit = 0 ==> Sensitivity typical value = 18milligals/digit*/ 
    case 0x00:
        *out =(int32_t)(LIS302DL_SENSITIVITY_2_3G *  (int8_t)*buffer);
      break;
    /* FS bit = 1 ==> Sensitivity typical value = 72milligals/digit*/ 
    case 0x20:
        *out =(int32_t)(LIS302DL_SENSITIVITY_9_2G * (int8_t)*buffer);        
      break;
    default:
      break;
    }
#endif
}
