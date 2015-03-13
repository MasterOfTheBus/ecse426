/**
   @file tilt_detection.h
   @brief Configure accelerometer settings and accelerometer calibration.
   @date 2015/03/12
*/

#include "stm32f4xx_conf.h"
#include "lis302dl.h"
#include "arm_math.h"

/** @define
		@brief Macro to control calibration
	 
	 Used to control offline calibration. Set calibrate to 1 to enable calibration at startup.
	 Set calibrate to 0 to use calibration parameters previously acquired.
 */
#define calibrate 0

/** @define
		@brief Alpha angle, pitch
*/
#define ALPHA (uint8_t)0

/** @define
		@brief Beta angle, roll
 */
#define BETA (uint8_t)1

// calibration parameters
#if calibrate
static float cal_data[12];
#else	
// calibration values from matlab
static float cal_data[] = {-0.001007704542329, -0.000023576795574, -0.000020500331472,
													 -0.000005549672267, -0.001001988618073, -0.000000555682904,
													 0.000001981760320, -0.000006760475090, 0.000964570821189,
													 -0.520517014244453, 0.477262260417352, -0.440876330953729};
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
	@brief Calibrate the sensor

	@ref calibrate must have value of 1 in order to calibrate
	*/
void calibrateSensor(void);

/**
	@brief Normalize the accelerometer data

	Apply the calibration parameters to the data to scale to 1g

	@param data The accelerometer data
	@param result The normalized data
*/
void normalize(int32_t* data, float* result);

/**
	@brief Get the tilt of the board

	@params type Specify alpha or beta tilt.
	@retval Return the tilt angle
	*/
float getTilt(uint8_t type, float xyz[]);

/**
	@brief Indicate board rotation direction to match desired tilt angle
	
	@param tilt The board's current angle
	@param inputTilt The desired angle
	@param currentTiltType The tilt angle type.
	@retval The direction to tilt the board: 1 for up, -1 for down, 0 for matching.
*/
int8_t tiltCorrection(float tilt, float inputTilt, uint8_t* currentTiltType);
