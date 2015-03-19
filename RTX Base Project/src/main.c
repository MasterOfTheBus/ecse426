/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>

#include "kalman.h"
#include "interrupt.h"
#include "tilt_detection.h"

#include "ADC.h"
#include "temperature.h"
#include "timer.h"

#include "UI.h"

#define TILT_SIGNAL (uint32_t)1
#define TEMPERATURE_SIGNAL (uint32_t)2
#define EOC_SIGNAL (uint32_t)4

// variables to store the tilt and temperature
float temp_C;
float tilt;

// flashing count
uint8_t danger;
uint16_t dangerCount;

//// Mutexes
//osMutexDef(MutexTemp);
//osMutexDef(MutexTilt);
osMutexDef(MutexDisplay);
osMutexDef(MutexFlash);

//osMutexId tempC_mutex;
//osMutexId tilt_mutex;
osMutexId display_mutex;
osMutexId flash_mutex;

// ID for thread
osThreadId GetTilt_thread;
osThreadId GetTemp_thread;

// kalman state for temperature
kalman_state kstate = {0.0025, 5.0, 1100.0, 0.0, 0.0};

// kalman state for accelerometer
kalman_state kstate_X = {0.025, 5, 0, 0, 0};
kalman_state kstate_Y = {0.025, 5, 0, 0, 0};
kalman_state kstate_Z = {0.025, 5, 0, 0, 0};

void GetTemp(void const *argument) {
	while (1) {
		osEvent evt = osSignalWait (TEMPERATURE_SIGNAL, osWaitForever);
		if (evt.status == osEventSignal) {
			osSignalClear (GetTemp_thread, TILT_SIGNAL);
			ADC_SoftwareStartConv(ADC1);
			evt = osSignalWait (EOC_SIGNAL, osWaitForever);
			if (evt.status == osEventSignal) {
				osSignalClear (GetTemp_thread, EOC_SIGNAL);
				float f_output;

				// Get Temerature and Filtering
				Kalmanfilter_C((float)ADC_GetConversionValue(ADC1), &f_output, &kstate);

				//printf("filtered: %f\n", f_output);
				// Convert to temperature
				temp_C = voltage2temp(f_output);
				
				osMutexWait(flash_mutex, osWaitForever);
				danger = ((temp_C >= THRESHOLD_TEMP) ? 1 : 0);
				osMutexRelease(flash_mutex);

				printf("temp: %f\n", temp_C);
			}
		}
	}
}

void GetTilt(void const *argument) {
	while (1) {
		osEvent evt = osSignalWait (TILT_SIGNAL, osWaitForever);
    if (evt.status == osEventSignal)  {
        osSignalClear (GetTilt_thread, TILT_SIGNAL);

				int32_t xyz[3]; // get the acc data
				LIS302DL_ReadACC(xyz);
				//printf("%i, %i, %i\n", xyz[0], xyz[1], xyz[2]);

				float xyz_float[3];

				normalize(xyz, xyz_float); // normalize the data

				// filter the data
				Kalmanfilter_C(xyz_float[0], &xyz_float[0], &kstate_X); // X
				Kalmanfilter_C(xyz_float[1], &xyz_float[1], &kstate_Y); // Y
				Kalmanfilter_C(xyz_float[2], &xyz_float[2], &kstate_Z); // Z

				tilt = getTilt(BETA, xyz_float);
				//setNumDisplay(tilt);
				printf("tilt: %f\n", tilt);

		}
	}
}

void ReadKeypad(void const *argument){
	int digit = Keypad_read();				// check keypad
	if (digit != NO_INPUT) {
		printf("user input: %i\n", digit);
	}
	
	if (digit == ENTER) {
		osMutexWait(display_mutex, osWaitForever);
		setDisplayMode(getDisplayMode() * (-1));
		osMutexRelease(display_mutex);
	}
}

void Display7Segment(void const *argument){
	if(getTIM3_count() == 3) {
		setTIM3_count(0);
	} else {
		setTIM3_count(getTIM3_count()+1);
	}

	float toDisplay = 0;
	osStatus status = osMutexWait(display_mutex, osWaitForever);
	if (getDisplayMode() == TEMP_MODE) {
		//status = osMutexWait(tempC_mutex, osWaitForever);
		toDisplay = temp_C;
		//osMutexRelease(tempC_mutex);
	} else if (getDisplayMode() == TILT_MODE) {
		//status = osMutexWait(tempC_mutex, osWaitForever);
		toDisplay = tilt;
		//osMutexRelease(tilt_mutex);
	}
	osMutexRelease(display_mutex);

	osMutexWait(flash_mutex, osWaitForever);
	if (danger) {
		dangerCount++;
		if (dangerCount < 50) {
			toDisplay = -1;
		}
		if (dangerCount > 100) {
			dangerCount = 0;
		}
	}
	osMutexRelease(flash_mutex);
		
	Display(toDisplay);
}

void DisplayLED(void const *argument){
	while(1){
		// Turn LEDs on according to user input
/*		if (userInput == 1){
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
		} else if (userInput == 2){
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
		} else if (userInput == 3){
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
		}else if (userInput == 4){
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
		}*/
		
		// Turn LEDs off based PWM...
	
	}
}
osThreadDef(GetTilt, osPriorityNormal, 1, 1000);
osThreadDef(GetTemp, osPriorityNormal, 1, 1000);
// Timer defs
osTimerDef(DisplayTimer, Display7Segment);
osTimerDef(KeypadTimer, ReadKeypad);

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS

	// ID for timer
	osTimerId display_timer;
	osTimerId keypad_timer;
	
	// Create the mutexes
//	tempC_mutex = osMutexCreate(osMutex(MutexTemp));
//	tilt_mutex = osMutexCreate(osMutex(MutexTilt));
	display_mutex = osMutexCreate(osMutex(MutexDisplay));
	flash_mutex = osMutexCreate(osMutex(MutexFlash));
	
  // initialize peripherals here
	
	// Accelerometer Configuration
	Accel_InitConfig();
	InitInterrupt();

	//ADC configuration
	Timer_config();
	EnableTimerInterrupt();
	configInit_common_ADC();
	configInit_ADC();
	configInit_ADC_Int();
	
	GPIO_config();
	
	//Enable temperatur sensor - Set the TSVREFE bit
	ADC_TempSensorVrefintCmd(ENABLE);
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);

	GetTilt_thread = osThreadCreate(osThread(GetTilt), NULL);

	GetTemp_thread = osThreadCreate(osThread(GetTemp), NULL);
	
	// Create the timers
	display_timer = osTimerCreate(osTimer(DisplayTimer), osTimerPeriodic, NULL);
	
	keypad_timer = osTimerCreate(osTimer(KeypadTimer), osTimerPeriodic, NULL);
	
	setDisplayMode(TEMP_MODE);
	
	danger = 0;
	dangerCount = 0;
	
	EXTI_GenerateSWInterrupt(EXTI_Line0); // generate an interrupt to initialize the sampling process
	
	osTimerStart(display_timer, 7); // start timer execution
	
	osTimerStart(keypad_timer, 20); // start keypad reading
	
	osKernelStart ();                         // start thread execution 
}

// Interrupt Handlers

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		osSignalSet (GetTilt_thread, TILT_SIGNAL);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void TIM3_IRQHandler(){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		osSignalSet (GetTemp_thread, TEMPERATURE_SIGNAL);

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void ADC_IRQHandler() {
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
		osSignalSet (GetTemp_thread, EOC_SIGNAL);
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}
