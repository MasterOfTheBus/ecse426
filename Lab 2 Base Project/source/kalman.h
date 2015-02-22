/**
   @file kalman.h
   @brief Configure Kalman filter parameters and perform filtering.
   @date 2015/02/20
*/

/** @struct kalman_state_struct
	* @brief The Kalman filter state parameters definition
	*/
/**
	* @typedef kalman_state
	*	@brief Typedef of kalman_state_struct
	*/
typedef struct kalman_state_struct {
    float q; /**< process noise covariance */
    float r; /**< measurement noise covariance */
    float x; /**< estimated value */
    float p; /**< estimation error covariance */
    float k; /**< adaptive Kalman filter gain */
} kalman_state;

/**
  * @brief  Computes the next iteration of the Kalman State and the filtered output.
 
 	 The process of filtering is performed by calculating the next values of @ref kalman_state
	 @code
	 p = p + q
	 k = p / (p + r)
	 x = x + k * (input - x)
	 p = (1 - k) * p
	 @endcode
	 Output is then set to the value of x

	* @param  input: The next input value to be filtered.
	* @param	output: The computed filter output for the current iteration.
	* @param	kstate: The struct storing the Kalman filter state parameters.
  * @retval An integer 0 for successful filtering, and 1 for an error
  */
int Kalmanfilter_C(float input, float* output, kalman_state* kstate);
