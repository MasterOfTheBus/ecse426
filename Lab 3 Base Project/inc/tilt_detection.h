#include "stm32f4xx_conf.h"
#include "lis302dl.h"
#include "arm_math.h"


#define ALPHA (uint8_t)0
#define BETA (uint8_t)1
#define NUM_CALIBRATION_SAMPLES 1

// calibration parameters
static float cal_data[12];
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

/**

	*/
void getXYZData(uint8_t* data);

void normalize(uint8_t* data, float* result);

/**
	@brief Get the tilt of the board

	@params type Specify alpha or beta tilt.
	@retval Return the tilt angle
	*/
float getTilt(uint8_t type, float xyz[]);
float calcAcceleration(float input);
