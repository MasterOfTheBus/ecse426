typedef struct pwm pwm;

struct pwm {
	int period;
	float duty_cycle; // the percentage of the "period" to count to
	int up; // Indicate whether to increment or decrement the duty cycle
};

void adjustDutyCycle(pwm* pwm_vals);
