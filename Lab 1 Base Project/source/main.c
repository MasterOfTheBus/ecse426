#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

// use these to change testing parameters
#define demo 1
#define assembly 1
#define print 0 // print the statistics functions
#define printOutput 1 // print the output

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

// prototypes
extern int Kalmanfilter_asm(float* OutputArray, float* InputArray, int Length, kalman_state* kstate);
int Kalmanfilter_C(float* InputArray, float* OutputArray, int Length, kalman_state* kstate);
void subtractVectors(float* vectorA, float* vectorB, float* difference, int length);
void average(float* vector, int length, float* average);
void standardDeviation(float* vector, int length, float* std_dev);
void findCorrelation(float* vectorA, float* vectorB, float* correlation, int length);
void findConvolution(float* vectorA, float* vectorB, float* convolution, int length);

int Kalmanfilter_C (float* InputArray, float* OutputArray, int Length, kalman_state* kstate) {
	int i = 0;
	while (i < Length) {
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		if (kstate->x != kstate->x) {
			printf("NaN\n");
			return 1;
		}
		kstate->p = (1 - kstate->k) * kstate->p;
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

void average(float* vector, int length, float* avg) {
	int i = 0;
	while (i < length) {
		*avg += vector[i];
		i++;
	}
	*avg /= length;
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

/*
 * Both the commented and non commented code does not match with the CMSIS-DSP correlation function
 * However, both get the same value
 */
void findCorrelation(float* vectorA, float* vectorB, float* correlation, int length) {
	// invert vectorB and simply pass to convolution
	// will be slower than calculating indices for array
	//int i = 0;
	/*float tempVector[1257];
	while (i < 1257) {
		tempVector[i] = vectorB[1257 - i - 1];
		i++;
	}*//*
	while (i < length/2) {
		float temp = vectorB[length - i - 1];
		vectorB[length - i - 1] = vectorB[i];
		vectorB[i] = temp;
		i++;
	}
	findConvolution(vectorA, vectorB, correlation, length);
	while (i < length/2) {
		float temp = vectorB[length - i - 1];
		vectorB[length - i - 1] = vectorB[i];
		vectorB[i] = temp;
		i++;
	}*/
	
	
	int n = 0;
	int resultLength = length + length - 2;
	
	while (n < resultLength) {
		int k = 0;
		correlation[n] = 0.0;
		while (k < length) {
			int index = length - (n - k) - 1;
			correlation[n] += vectorA[k] * (((index) < 0 || (index) >= length) ? 0 : vectorB[index]);
			k++;
		}
		n++;
	}
}

void findConvolution(float* vectorA, float* vectorB, float* convolution, int length) {
	int n = 0;
	int resultLength = length + length - 2;
	
	while (n < resultLength) {
		int k = 0;
		convolution[n] = 0.0;
		while (k < length) {
			convolution[n] += vectorA[k] * (((n -k) < 0 || (n - k) >= length) ? 0 : vectorB[n-k]);
			k++;
		}
		n++;
	}
}

// for part III
// declaring in main did not allocate memory for the variables
	float difference[1257];
	float avg;
	float std_dev;
	float correlation[2 * 1257 - 2];
	float convolution[2 * 1257 - 2];

int main()
{
#if demo
	int length = 1257;
#endif
	float output[length];
	
	
	
	// initialize the kalman state
	kalman_state kstate;
	kstate.q = 0.005;
	kstate.r = 5.0;
	kstate.x = 0.0;
	kstate.p = 0.0;
	kstate.k = 0.0;

#if assembly
	// calling the assembly
	Kalmanfilter_asm(output, testVector, length, &kstate);
#else
	// calling the c
	if (Kalmanfilter_C(testVector, output, length, &kstate) == 1) {
		printf("Bad calculation\n");
	}
#endif

	int i = 0;
#if printOutput
	while (i < length) {
		printf("%f\n", output[i]);
		i++;
	}
#endif

	//------------------------------------------------------------------------
	// implemented statistics functions
	// the output is separated because the arm library was printing a different value when printing
	// in a block at the end
	subtractVectors(testVector, output, difference, length);

#if print
	i = 0;
	while (i < 20) {
		printf("%f - %f = %f\n", testVector[i], output[i], difference[i]);
		i++;
	}
#endif
	
	average(difference, length, &avg);
	
#if print
		printf("avg: %f\n", avg);
#endif
	
	standardDeviation(difference, length, &std_dev);
	
#if print
	printf("std_dev: %f\n", std_dev);
#endif
	
	findCorrelation(testVector, output, correlation, length);
	
#if print
	i = 0;
	while (i < 20) {
		printf("correlation: %f\n", correlation[i]);
		i++;
	}
#endif
	
	findConvolution(testVector, output, convolution, length);

#if print
	i = 0;
	while (i < 20) {
		printf("convolution: %f\n", convolution[i]);
		i++;
	}
#endif

	//---------------------------------------------------------------
	printf("-----------------------------------------------------\n\n\n");

	// CMSIS DSP library functions
	arm_sub_f32(testVector, output, difference, length);
	
#if print
	i = 0;
	while (i < 20) {
		printf("%f - %f = %f\n", testVector[i], output[i], difference[i]);
		i++;
	}
#endif
	
	arm_mean_f32(difference, length, &avg);
	
#if print
		printf("avg: %f\n", avg);
#endif
	
	arm_std_f32(difference, length, &std_dev);
	
#if print
	printf("std_dev: %f\n", std_dev);
#endif
	
	arm_correlate_f32(testVector, length, output, length, correlation);
	
#if print
	i = 0;
	while (i < 20) {
		printf("correlation: %f\n", correlation[i]);
		i++;
	}
#endif
	
	arm_conv_f32(testVector, length, output, length, convolution);

#if print
	i = 0;
	while (i < 20) {
		printf("convolution: %f\n", convolution[i]);
		i++;
	}
#endif

	return 0;
}
