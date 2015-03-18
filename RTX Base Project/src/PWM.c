#include "PWM.h"

void adjustDutyCycle(pwm* pwm_vals) {
	if (pwm_vals->up) {
		if (pwm_vals->duty_cycle < 1.0) {
			pwm_vals->duty_cycle+=0.1;
		} else {
			pwm_vals->up = 0;
		}
	} else {
		if (pwm_vals->duty_cycle > 0.0) {
			pwm_vals->duty_cycle-=0.1;
		} else {
			pwm_vals->up = 1;
		}
	}
}
