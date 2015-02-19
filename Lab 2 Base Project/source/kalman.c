#include "kalman.h"

/**
  * @brief  Computes the next iteration of the Kalman State and the filtered output.
	* @param  input: The next input value to be filtered.
	* @param	output: The computed filter output for the current iteration.
	* @param	kstate: The struct storing the Kalman filter state parameters.
  * @retval An integer 0 for successful filtering, and 1 for an error
  */
int Kalmanfilter_C (float input, float* output, kalman_state* kstate) {
	kstate->p = kstate->p + kstate->q;
	kstate->k = kstate->p / (kstate->p + kstate->r);
	kstate->x = kstate->x + kstate->k * (input - kstate->x);
	if (kstate->x != kstate->x) { // check for overflow, underflow
		return 1;
	}
	kstate->p = (1 - kstate->k) * kstate->p;
	*output = kstate->x;
	return 0;
}