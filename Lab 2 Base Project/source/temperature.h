/**
   @file temperature.h
   @brief Convert voltage value into temperature
   @date 2015/02/20
*/

// Variables to convert voltage to temperature

/** \def step_size
	* @brief The multiplier value to convert the digital reading to analog
	*/
#define step_size (float)((3.0)/4096)
	
/** \def v_25
	* @brief The value the temperature sensor returns at 25 degrees celsius
	*/
#define v_25 (float)0.76

/** \def avg_slope
	* @brief The average slope of the termperature vs. sensor voltage curve
	*/
#define avg_slope (float)0.0025

/**
	* @brief Converts the voltage reading from the temperature sensor into a temperature value in celsius.
	
	The temperature is calculated from the voltage input with the following equations
	@code
	v_sense = voltage * step_size
	temperature in celsius = ((v_sense - v_25) / avg_slope) + 25
	@endcode

	* @param voltage The voltage reading from the temperature sensor.
	* @retval The converted temperature value in celsius.
	*/
float voltage2temp(float voltage);
