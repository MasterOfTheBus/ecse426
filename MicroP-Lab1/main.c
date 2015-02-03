#include <stdio.h>
//#include "arm_math.h" // may have to add the rest of the base project components

#if 1
#include "test.h"
#endif

#define c 0
#define as 1

//typedef signed long int32_t;

typedef struct kalman_state kalman_state;
struct kalman_state {
	float q; // process noise covariance
	float r; // measurement noise covariance
	float x; // estimated value
	float p; // estimation error covariance
	float k; // adaptive Kalman filter gain
};

extern int Kalmanfilter_asm(float* InputArray, float* OutputArray, int Length, kalman_state* kstate);
int Kalmanfilter_C(float* InputArray, float* OutputArray, int Length, kalman_state* kstate);

int Kalmanfilter_C (float* InputArray, float* OutputArray, int Length, kalman_state* kstate) {
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
	float output[length];
	
	kalman_state kstate;
	kstate.q = 0.1;
	kstate.r = 0.1;
	kstate.x = testArray[0];
	kstate.p = 0.1;
	kstate.k = 0;
	
	// calling the assembly
#if as
	Kalmanfilter_asm(testArray, output, length, &kstate);
#endif
	// calling the c
#if c
	Kalmanfilter_C(testArray, output, length, &kstate);
#endif
	
	return 0;
}




