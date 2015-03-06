#include "stm32f4xx_conf.h"
#include "lis302dl.h"

#define ALPHA (uint8_t)0
#define BETA (uint8_t)1

/**
	@brief Initialize the mems struct
	
	@params Power Power Mode of the sensor 
	@params DataRate
	@params Axes
	@params FullScale
	@params SelfTest
	*/
void Accel_InitConfig(uint8_t Power,
											uint8_t DataRate,
											uint8_t Axes,
											uint8_t FullScale,
											uint8_t SelfTest);

/**

	*/
void getXYZData(uint8_t data[]);
/**
	@brief Get the tilt of the board

	@params type Specify alpha or beta tilt.
	@retval Return the tilt angle
	*/
int getTilt(uint8_t type, int xyz[]);
int32_t calcAcceleration(uint8_t *buffer, uint8_t crtl);
