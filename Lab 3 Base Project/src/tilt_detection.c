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
												
	// The low level init helper method in the init function doesn't initialize with the speed that we want
	// re-init GPIO port e
	/* Configure GPIO PINs to detect Interrupts */
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_INT1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // match sample rate
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN; // by default read 0, not floating
  GPIO_Init(LIS302DL_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_INT2_PIN;
  GPIO_Init(LIS302DL_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);
}

int getTilt(uint8_t type, int xyz[]) {
	uint8_t *buffer_num;
	uint8_t *buffer_denom;
	uint8_t *buffer_z;
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
	
	xyz[0] = *buffer_num;
	xyz[1] = *buffer_denom;
	xyz[2] = *buffer_z;
	
	return 0;
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
		return 0;
}
