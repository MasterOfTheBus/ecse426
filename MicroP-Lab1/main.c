#include <stdio.h>
//#include "arm_math.h" // may have to add the rest of the base project components

//typedef signed long int32_t;

typedef struct kalman_state kalman_state;
struct kalman_state {
	float q; // process noise covariance
	float r; // measurement noise covariance
	float x; // estimated value
	float p; // estimation error covariance
	float k; // adaptive Kalman filter gain
};

int Kalmanfilter_C(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);

int Kalmanfilter_C (float* InputArray, float* OutputArray, kalman_state* kstate, int Length) {
	int i = 0;
	while (i < Length) {
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		kstate->p = (1 - kstate->k) * kstate->p;
		OutputArray[i] = kstate->x;
		i++;
	}
	return 0;
}

int main()
{
	
	float input[3];
	float output[3];
	int length = 3;
	kalman_state kstate;
	kstate.q = 0.1;
	kstate.r = 0.1;
	kstate.p = 0.1;
	kstate.k = 0;
	kstate.x = 0;
	
	input[0] = 1.0;
	input[1] = 1.2;
	input[2] = 1.4;
	
	kstate.x = input[0];
	
	// calling the assembly
	
	// calling the c
	Kalmanfilter_C(input, output, &kstate, length);

	return 0;
}




