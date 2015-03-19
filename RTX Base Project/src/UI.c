#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "UI.h"

//int timerValue;	
int digits[3] = {0};
//int setTime = 160;
int result = 0;
//int readDigit = 1;
//int displayUserInput = 0;
//int keypadWait = 10;
int count = 0;
int lastResult = 99;

int correctionCount=0;

void setTIM3_count(int val) {
	TIM3_interrupt_count = val;
}

int getTIM3_count(void) {
	return (TIM3_interrupt_count);
}

void setReadStatus(int val) {
	readStatus = val;
}

int getReadStatus(void) {
	return (readStatus);
}
	
void setDisplayMode(int8_t val) {
	displayMode = val;
}

int8_t getDisplayMode(void) {
	return displayMode;
}

void configInit_GPIO(GPIO_TypeDef* GPIOx,
										 uint32_t periph_GPIOx,
										 uint32_t pins,
										 GPIOMode_TypeDef mode,
										 GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType,
										 GPIOPuPd_TypeDef puPd){
	//Enable GPIO clock
	RCC_AHB1PeriphClockCmd(periph_GPIOx, ENABLE);
	
	//GPIO configuration
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = pins;
	gpio.GPIO_Mode = mode;
	gpio.GPIO_Speed = speed;
	gpio.GPIO_OType = oType;
	gpio.GPIO_PuPd = puPd;
	
	GPIO_Init(GPIOx, &gpio);
										 											 
}

void GPIO_config(){
	// configure I/O for 7-segment display
	configInit_GPIO(GPIOB, RCC_AHB1Periph_GPIOB,
									GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_DOWN);
									
	configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
									GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_DOWN);

	configInit_GPIO(GPIOE, RCC_AHB1Periph_GPIOE,
									GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_DOWN);
	
	// on board LEDs to display up and down
	configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
									GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
									GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
									GPIO_PuPd_DOWN);

	TIM3_interrupt_count = 0;
}

// Display helper method to select the digit to display
void displayDigit(int digit) {
			if (digits[digit]==0)
				Zero();
			else if (digits[digit]==1)
				One();
			else if (digits[digit]==2)
				Two();
			else if (digits[digit]==3)
				Three();
			else if (digits[digit]==4)
				Four();
			else if (digits[digit]==5)
				Five();
			else if (digits[digit]==6)
				Six();
			else if (digits[digit]==7)
				Seven();
			else if (digits[digit]==8)
				Eight();
			else if (digits[digit]==9)
				Nine();
			else
				Off();
}

void Display(float n){
	uint8_t decimal;
	if (n < 0) {
		uint8_t i;
		for (i = 0; i < 3; i++) {
			digits[i] = 10;
		}
	} else {

		int m = (int)n; 			// extract integer part
		n = n-(float)(m);		// extract floating point part
		
		if (m>=100){
			decimal = 0;					// ignore any floating point
		}
		else if (m >= 10){
			decimal = 1;
			m = m*10 + (int)(n*10);
		} else {
			decimal = 2;
			m = m* 100 + (int)(n*100);
		}

		uint8_t i;
		for (i=0; i<3;i++){
			digits[i] = (int)(m%10);
			m = (m-(m%10))/10;
		}
	}
		//printf ("%i %i %i",digits[2], digits[1], digits[0]);

		// Get hardware timer value
		//timerValue = TIM_GetCounter(TIM3);
		
		// Set first digit
		if (TIM3_interrupt_count == 0) {
			
		
		//	while (TIM_GetCounter(TIM3) < (timerValue+1*setTime)){

			GPIO_WriteBit(GPIOE, GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET); // Release other select lines
			GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_7, Bit_SET);		// Select digit 1
			GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_RESET);	// Reset decimal point
			if (decimal == 2) {
				GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_SET);	// Set decimal point
			}
			displayDigit(2);
		}
		
		//printf("second loop\n");
		// Set second digit
		if (TIM3_interrupt_count == 1) {
			GPIO_WriteBit(GPIOE, GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET); // Release other select lines
			GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_SET);		// Select digit 2
			GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_RESET);	// Reset decimal point
			if (decimal == 1){
				GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_SET);	// Set decimal point
			}

			displayDigit(1);
		}
		//printf("third loop\n");
		// Set third digit
		if (TIM3_interrupt_count == 2) {
		//while (TIM_GetCounter(TIM3) < (timerValue+3*setTime)){
			//printf("get counter3: %i\t %i\n", TIM_GetCounter(TIM3), (timerValue+3*setTime));
			GPIO_WriteBit(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_14, Bit_RESET); // Release other select lines
			GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_12, Bit_SET);		// Select digit 3
			GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_RESET);	// Reset decimal point
	
			displayDigit(0);
		}
		
		if (TIM3_interrupt_count == 3) {
			if (n >= 0) {
				// Set degree
				GPIO_WriteBit(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET); // Release other select lines
				GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_SET);	// Select degree
				GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_SET);	// Turn on degree LED
			} else {
				Off();
			}
		}
}

void Off() {
	// Turn off
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10, Bit_RESET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13, Bit_RESET);
}

void Zero(){

	// Turn on:
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_12 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_RESET);

}

void One(){
	
	// Turn on:
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_14, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10 , Bit_RESET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11 , Bit_RESET);
	
}

void Two(){
	
	// Turn on:
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13, Bit_SET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_14 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10 , Bit_RESET);

}

void Three(){
	
	// Turn on;
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10 , Bit_RESET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_11, Bit_RESET);

}

void Four(){
	
	// Turn on;
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_13 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_RESET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_RESET);
	
}


void Five(){
	
	// Turn on;
	GPIO_WriteBit(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_RESET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_11, Bit_RESET);
	
}

void Six(){
	
	// Turn on;
	GPIO_WriteBit(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_RESET);
	
}

void Seven(){
	
	// Turn on:
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_12 | GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10 , Bit_RESET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11 , Bit_RESET);
	
}
void Eight(){
	
	// Turn on;
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_RESET);
	
}	

void Nine(){
	
	// Turn on;
	GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_SET);
	// Turn off:
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_RESET);
	GPIO_WriteBit(GPIOE, GPIO_Pin_11, Bit_RESET);	
}	

int Keypad_readDigit(){
	
	configInit_GPIO(GPIOE, RCC_AHB1Periph_GPIOE,
										GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,
										GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
										GPIO_PuPd_DOWN);
	configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
										GPIO_Pin_13,
										GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
										GPIO_PuPd_DOWN);
	// Set the colomns high
	GPIO_WriteBit(GPIOE, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, Bit_SET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);	
	
	
	// Read the rows
	configInit_GPIO(GPIOB, RCC_AHB1Periph_GPIOB,
										GPIO_Pin_4 | GPIO_Pin_5,
										GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
										GPIO_PuPd_DOWN);	
	configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
										GPIO_Pin_1 | GPIO_Pin_2,
										GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
										GPIO_PuPd_DOWN);	
										
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)== Bit_RESET && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == Bit_RESET && GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1) == Bit_RESET &&	GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2) == Bit_RESET){
		// Nothing is pressed
		result = NO_INPUT;
	} 
	
	else{
	
		// First row
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_SET){
			// Set this row to output
			configInit_GPIO(GPIOB, RCC_AHB1Periph_GPIOB,
											GPIO_Pin_4,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOE, RCC_AHB1Periph_GPIOE,
											GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
											GPIO_Pin_13,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == Bit_SET){
				result = 1;
			}
			
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_SET){
				result = 3;
			}
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == Bit_SET){
				result = A_KEY; // A
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
				result = 2;
			}
		
		} 
		// Second row
		else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == Bit_SET){
			// Set this row to output
			configInit_GPIO(GPIOB, RCC_AHB1Periph_GPIOB,
											GPIO_Pin_5,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOE, RCC_AHB1Periph_GPIOE,
											GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
											GPIO_Pin_13,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == Bit_SET){
				result = 4;
			}
			
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_SET){
				result = 6;
			}
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == Bit_SET){
				result = B_KEY; // B
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
				result = 5;
			}
			
		} 
		
		// Third row
		else if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1) == Bit_SET){
			// Set this row to output
			configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
											GPIO_Pin_1,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOD, GPIO_Pin_1, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOE, RCC_AHB1Periph_GPIOE,
											GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
											GPIO_Pin_13,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == Bit_SET){
				result = 7;
			}

			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_SET){
				result = 9;
			}
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == Bit_SET){
				result = C_KEY; // C
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
				result = 8;
			}
		
		}
		
		// Fourth row
		else{
			// Set this row to output
			configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOD,
											GPIO_Pin_2,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);	
			// Set this row high
			GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);			
			
			// Read the colomns
			configInit_GPIO(GPIOE, RCC_AHB1Periph_GPIOE,
											GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			configInit_GPIO(GPIOC, RCC_AHB1Periph_GPIOC,
											GPIO_Pin_13,
											GPIO_Mode_IN, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == Bit_SET){
				result = STAR; // Calibrate
			}
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_SET){
				result = ENTER; // ENTER
			}
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == Bit_SET){
				result = D_KEY; // D
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
				result = 0;
			}
			
		}
	}
	return result;
}

int Keypad_read(){
	Keypad_readDigit();
	int userInput = NO_INPUT;
	if (result == lastResult){	// Key not released
		return userInput;													// Do nothing...
	}	else if (result == NO_INPUT){		// no signal detected --- NO_INPUT 
		//count++;								// counter for bouncing
	}	else if(result <= ENTER){ // Handling key bouncing 
			userInput = result;			// Update userInput
			count = 0; // Reset counter 
	} 
	lastResult = result; // keep history
	return userInput;
}
#if 0
void correctionOutput(int8_t upDown) {
	if (upDown == 0) {
		return;
	}
	// display
			GPIO_WriteBit(GPIOE, GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET); // Release other select lines
			GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_7, Bit_SET);		// Select digit 1
			GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_RESET);	// Reset decimal point
	if (correctionCount < 100) {
		// bottom line
			GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, Bit_RESET);
			GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_11, Bit_RESET);
	} else if (correctionCount < 200) {
			// middle line
			GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, Bit_RESET);
			GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_RESET);
	} else if (correctionCount < 300) {
			// top line
			GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15, Bit_RESET);
			GPIO_WriteBit(GPIOD, GPIO_Pin_8 | GPIO_Pin_10, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_9 | GPIO_Pin_11, Bit_RESET);
	}

	// adjust for next value
	if (upDown == 1) {
		correctionCount++;
		if (correctionCount > 300) {
			correctionCount = 0;
		}
	} else if (upDown == -1) {
		correctionCount--;
		if (correctionCount < 0) {
			correctionCount = 300;
		}
	}
}
#endif
