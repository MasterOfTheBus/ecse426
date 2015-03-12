#include "stm32f4xx_conf.h"
#include "lis302dl.h"
#include "arm_math.h"

#define calibrate 0

#define ALPHA (uint8_t)0
#define BETA (uint8_t)1

// calibration parameters
#if calibrate
static float cal_data[12];
#else	
// calibration completed
// values from matlab
static float cal_data[] = {-0.001000548970869, 0.000009174814798, -0.000017855471758,
													 -0.000009008599985, -0.001000996022111, 0.000008455943239,
													 0.000000436148874, -0.000008625098424, 0.000956949776224,
													 -0.512170657749237, 0.485857723616614, -0.418036663694971};
#endif
static arm_matrix_instance_f32 calParams;

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
void calibrateSensor(void);

void normalize(int32_t* data, float* result);

/**
	@brief Get the tilt of the board

	@params type Specify alpha or beta tilt.
	@retval Return the tilt angle
	*/
float getTilt(uint8_t type, float xyz[]);
