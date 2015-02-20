#include "kalman.h"

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
