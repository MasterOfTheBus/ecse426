/**
	* @brief The Kalman filter state parameters definition
	*/
typedef struct kalman_state kalman_state;
struct kalman_state {
	float q; // process noise covariance
	float r; // measurement noise covariance
	float x; // estimated value
	float p; // estimation error covariance
	float k; // adaptive Kalman filter gain
};

// Kalman filter function prototype
int Kalmanfilter_C(float input, float* output, kalman_state* kstate);
