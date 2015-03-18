/**
   @file tilt_detection.h
   @brief Handles accelerometer data sampling.
	 
	 Assumes usage of the LIS302DL MEMS sensor
	 Includes functions to normalize the raw data from the sensor and calculate the tilt angle based on acceleration values along the three axes.
	 
   @date 2015/03/12
	*/

#include "stm32f4xx_conf.h"
#include "lis302dl.h"
#include "arm_math.h"

/** @define
		@brief Macro to control calibration
	 
	 Used to control offline calibration. Set calibrate to 1 to enable calibration at startup.
	 Set calibrate to 0 to use calibration parameters previously acquired.
	 
	 Calibrate should be set to 0 since @ref calibrateSensor does not include functionality to 
	 collect new data for the 6 positions along the axes.
	*/
#define calibrate 0

/** @define
		@brief Constant used to specify angle type as the pitch
	*/
#define ALPHA (uint8_t)0

/** @define
		@brief Constant used to specify angle type as the roll
	*/
#define BETA (uint8_t)1

// calibration parameters
/** @var
		@brief The array storing the normalization values
		
		The values are previously obtained from offline calibration using the Least Squares Method.
		Reacquire values for different sensors.
	*/
#if calibrate
static float cal_data[12];
#else	
// calibration values from matlab
static float cal_data[] = {-0.001007704542329, -0.000023576795574, -0.000020500331472,
													 -0.000005549672267, -0.001001988618073, -0.000000555682904,
													 0.000001981760320, -0.000006760475090, 0.000964570821189,
													 -0.520517014244453, 0.477262260417352, -0.440876330953729};
#endif

/** @var
		@brief The matrix storing the normalization values.

		It is a 4x3 matrix with the values of @ref cal_data
	*/
static arm_matrix_instance_f32 calParams;

/**
	@brief Initialize the mems configurations
													 
	The function activates writing to the status registers when Data Ready.
	
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

	Uses the relation X = inverse( transpose(w) * w ) * transpose(w) * Y and solves for X.
	Y is the predefined values along +/- x, y, z axes. W is acquired data corresponding to the positions in Y.
	X is the calibrated normalization matrix values.

	@ref calibrate must have value of 1 in order to calibrate
	*/
void calibrateSensor(void);

/**
	@brief Normalize the accelerometer data

	Apply the calibration parameters to the data to scale to 1g.
	Uses the relation Y = w * X to solve for Y, where Y is the normalized data, w is the accelerometer raw data,
	X is the normalization matrix stored in @ref calParams.

	@param data The accelerometer data
	@param result The normalized data
*/
void normalize(int32_t* data, float* result);

/**
	@brief Get the tilt of the board

	@params type Specify alpha or beta tilt.
	@params xyz the acceleration values for x, y, and z respectively
	@retval Return the tilt angle
	*/
float getTilt(uint8_t type, float xyz[]);

/**
	@brief Indicate the direction to rotate the board to match a desired tilt angle

	If the board is calculating the pitch, up indicates raising the side opposite the USB link.
	If the board is calculating the roll, up indicates raising the side to the left of the USB link when looking into the link

	@param tilt The board's current angle
	@param inputTilt The desired angle
	@param currentTiltType The tilt angle type.
	@retval The direction to tilt the board: 1 for up, -1 for down, 0 for matching.
*/
int8_t tiltCorrection(float tilt, float inputTilt, uint8_t* currentTiltType);
