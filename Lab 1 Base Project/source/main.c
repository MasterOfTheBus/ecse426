#include <stdio.h>
#include "arm_math.h" // may have to add the rest of the base project components

#if 0
#include "array.h"
#endif
#if 1
#include "test.h"
#endif

#define c 0
#define as 1

#define custom 1
#define library 0

//typedef signed long int32_t;

typedef struct kalman_state kalman_state;
struct kalman_state {
	float q; // process noise covariance
	float r; // measurement noise covariance
	float x; // estimated value
	float p; // estimation error covariance
	float k; // adaptive Kalman filter gain
};

extern int Kalmanfilter_asm(float* OutputArray, float* InputArray, int Length, kalman_state* kstate);
int Kalmanfilter_C(float* InputArray, float* OutputArray, int Length, kalman_state* kstate);

int Kalmanfilter_C (float* InputArray, float* OutputArray, int Length, kalman_state* kstate) {
	int i = 0;
	while (i < Length) {
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		if (kstate->x != kstate->x) {
			return 1;
		}
		kstate->p = (1 - kstate->k) * kstate->p;
		OutputArray[i] = kstate->x;
		i++;
	}
	return 0;
}

int main()
{
	float output[length];
	//int length = 1257;
	kalman_state kstate;
	kstate.q = 0.1;
	kstate.r = 0.1;
	kstate.x = testVector[0];
	kstate.p = 0.1;
	kstate.k = 0;

	// calling the assembly
#if as
	Kalmanfilter_asm(output, testVector, length, &kstate);
#endif
	// calling the c
#if c
	Kalmanfilter_C(testVector, output, length, &kstate);
#endif

	return 0;
}




