/**
   @file PWM.h
   @brief Configure Pulse Width Modulation settings and adjust the duty cycle.
	 
	 Pulse Width Modulation (PWM) allows for a fading-in/fading-out effect.
	 A period is set and a counter will increment until it reaches a duty cycle value less than or equal to the duration of the period.
	 Within the duty cycle, a value is set (ie. LEDs are turned on). For the rest of the period, the value is reset.
	 Every period, the duty cycle is adjusted. The duty cycle changes the intensity of the LEDs, while adjusting the duty cycle allows for the
	 fading effect.
	 
   @date 2015/02/20
*/

/** @struct pwm_struct
	* @brief The parameters for the pulse width modulation.
	*/
/**
	* @typedef pwm
	*	@brief Typedef of pwm_struct
	*/
typedef struct pwm_struct {
	int period; /**< The total period of the pwm */
	float duty_cycle; /**< the percentage of the period to count to */
	int up; /**< Indicate whether to increment or decrement the duty cycle */
} pwm;

/**
	* @brief Increment or decrement the duty cycle percentage when a duty cycle has completed
	*
	* If the duty cycle value reaches 100% of the period, then the duty cycle will be set to decrement.
	* If the duty cycle value reaches 0% of the period, then the duty cycle will be set to increment.
	*
	* @param pwm_vals The pwm struct storing the duty cycle to adjust
	* @retval None.
	*/
void adjustDutyCycle(pwm* pwm_vals);
