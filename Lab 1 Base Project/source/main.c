#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h" // may have to add the rest of the base project components

#define demo 1
#define assembly 0
#define custom 0

#if demo
#include "array.h"
#else
#include "test.h"
#endif

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
void subtractVectors(float* vectorA, float* vectorB, float* difference, int length);
void average(float* vector, int length, float* average);
void standardDeviation(float* vector, int length, float* std_dev);
void findCorrelation(float* vectorA, float* vectorB, float* correlation, int length);
void findConvolution(float* vectorA, float* vectorB, float* convolution, int length);

int Kalmanfilter_C (float* InputArray, float* OutputArray, int Length, kalman_state* kstate) {
	int i = 0;
	float test = 0.0;
	while (i < Length) {
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		if (kstate->x != kstate->x) {
			printf("NaN\n");
			return 1;
		}
		kstate->p = (1 - kstate->k) * kstate->p;
		test = kstate->x;
		OutputArray[i] = kstate->x;
		i++;
	}
	return 0;
}

void subtractVectors(float* vectorA, float* vectorB, float* difference, int length) {
	int i = 0;
	while (i < length) {
		difference[i] = vectorA[i] - vectorB[i];
		i++;
	}
}

void average(float* vector, int length, float* average) {
	int i = 0;
	while (i < length) {
		*average += vector[i];
		i++;
	}
	*average /= length;
}

void standardDeviation(float* vector, int length, float* std_dev) {
	float sumOfSquares = 0;
	float sum = 0;
	int i = 0;
	while (i < length) {
		sumOfSquares += vector[i] * vector[i];
		sum += vector[i];
		i++;
	}
	arm_sqrt_f32(((sumOfSquares - (sum * sum) / length) / (length - 1)), std_dev);
}

void findCorrelation(float* vectorA, float* vectorB, float* correlation, int length) {
	int i = 0;
	int resultLength = 2 * length - 2;
	while (i < resultLength) {
		int j = 0;
		while (j < length) {
			int addition = vectorA[j];
			int bInd = abs(j - i);
			addition *= ((bInd >= length) ? 0 : vectorB[bInd]);
			correlation[i] += addition;
		}
	}
}

void findConvolution(float* vectorA, float* vectorB, float* convolution, int length) {
	int i = 0;
	int resultLength = length + length - 2;
	while (i < resultLength) {
		int j = 0;
		while (j < length) {
			int addition = vectorA[j];
			int bInd = abs(i - j);
			addition *= ((bInd >= length) ? 0 : vectorB[bInd]);
			convolution[i] += addition;
		}
	}
}

int main()
{
#if demo
	int length = 75;//1257;
#endif
	float output[length];
	
	// for part III
	float difference[length];
	float average;
	float std_dev;
	float correlation;
	float convolution;
	
	kalman_state kstate;
	kstate.q = 0.1;
	kstate.r = 0.1;
	kstate.x = testVector[0];
	kstate.p = 0.1;
	kstate.k = 0;

#if assembly
	// calling the assembly
	Kalmanfilter_asm(output, testVector, length, &kstate);
#else
	// calling the c
	Kalmanfilter_C(testVector, output, length, &kstate);
#endif
	int i = 0;
	for (; i< length; i++) {
		printf("%f\n", output[i]);
	}

#if 0
#if custom
	subtractVectors(testVector, output, difference, length);
	
	average(difference, length, &average);
	standardDeviation(difference, length, &std_dev);
	
	findCorrelation(testVector, output, &correlation, length);
	
	findConvolution(testVector, output, &convolution, length);
#else
	arm_sub_f32(testVector, output, difference, length);
	
	arm_mean_f32(difference, length, &average);
	
	arm_std_f32(difference, length, &std_dev);
	
	arm_correlate_f32(testVector, length, output, length, &correlation);
	
	arm_conv_f32(testVector, length, output, length, &convolution);
#endif
#endif

	return 0;
}
