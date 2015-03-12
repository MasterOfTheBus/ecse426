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
	@brief Calibrate the sensor

	@ref calibrate must have value of 1 in order to calibrate
	*/
void calibrateSensor(void);

/**
	@brief Normalize the accelerometer data

	Apply the calibration parameters to the data

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
