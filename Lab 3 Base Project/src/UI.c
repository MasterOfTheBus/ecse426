#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "UI.h"

int timerValue;	
int digits[3] = {0};
int i;
int m;
int decimal;
int setTime = 160;
int result = 0;
int readDigit;
int displayUserInput = 0;
int keypadWait = 10;
float numDisplay;
int userInput;
int count;
int lastResult;


void Timer_config(uint16_t Prescaler,
									uint16_t CounterMode,
									uint32_t Period,
									uint16_t ClockDivision,
									uint8_t RepetitionCounter){
	//Enable Timer3									
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
										
	//Timer configuration
	TIM_TimeBaseInitTypeDef timer_init;
	timer_init.TIM_Prescaler = Prescaler;
	timer_init.TIM_CounterMode = CounterMode;
	timer_init.TIM_Period = Period;
	timer_init.TIM_ClockDivision = ClockDivision;
	timer_init.TIM_RepetitionCounter = RepetitionCounter;
	
	TIM_TimeBaseInit(TIM3, &timer_init);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // enable interrupt
}

	
void EnableTimerInterrupt(){
	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = TIM3_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0x02;
	nvic_init.NVIC_IRQChannelSubPriority = 0x02;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
}


void TIM3_IRQHandler(){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		Display(numDisplay,userInput);
		
		//TIM3_interrupt = 1;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
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

}

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
			else
				Nine();
}

void Display(float n, int input){
	
	if (input !=500 ){
	 m = input;
		decimal = 0;
	}
	else{
		m = (int)n; 			// extract integer part
		n = n-(float)(m);		// extract floating point part
		
		if (m>=100){
			decimal = 0;					// ignore any floating point
		}
		else{
			decimal = 1;
			m = m*10 + (int)(n*10);
		}
	}
		
		for (i=0; i<3;i++){
			digits[i] = (int)(m%10);
			m = (m-(m%10))/10;
		}
		printf ("%i %i %i",digits[2], digits[1], digits[0]);

		// Get hardware timer value
		timerValue = TIM_GetCounter(TIM3);
		
		
		// Set first digit
			while (TIM_GetCounter(TIM3) < (timerValue+1*setTime)){
			GPIO_WriteBit(GPIOE, GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET); // Release other select lines
			GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_7, Bit_SET);		// Select digit 1
			GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_RESET);	// Reset decimal point
			
			displayDigit(2);
		}
		
		
		// Set second digit
		while (TIM_GetCounter(TIM3) < (timerValue+2*setTime)){
			GPIO_WriteBit(GPIOE, GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET); // Release other select lines
			GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_SET);		// Select digit 2
			
			if (decimal){
				GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_SET);	// Set decimal point
			}

			displayDigit(1);
		}
		
		// Set third digit
		while (TIM_GetCounter(TIM3) < (timerValue+3*setTime)){
			GPIO_WriteBit(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_14, Bit_RESET); // Release other select lines
			GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
			GPIO_WriteBit(GPIOE, GPIO_Pin_12, Bit_SET);		// Select digit 3
			GPIO_WriteBit(GPIOE, GPIO_Pin_13 , Bit_RESET);	// Reset decimal point
	
			displayDigit(0);
		}
		
		// Set degree
		GPIO_WriteBit(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14, Bit_RESET); // Release other select lines
		GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_SET);	// Select degree
		GPIO_WriteBit(GPIOD, GPIO_Pin_10 , Bit_SET);	// Turn on degree LED

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
	


	




void Keypad_readDigit(){
	
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
		result = 99;
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
				result = 10; // A
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
				result = 2;
				configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOC,
											GPIO_Pin_14,
											GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
											GPIO_PuPd_DOWN);
				GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
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
				result = 11; // B
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
				result = 12; // C
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
				result = 21; // Calibrate
			}
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_SET){
				result = 22; // ENTER
			}
			else if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == Bit_SET){
				result = 13; // D
			}
			else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
				result = 0;
			}
			
		}
	}
	
}



void Keypad_read(){
	userInput = 0;
	lastResult = 99;
	count = 0;
	readDigit = 1;
	while (result != 22 || result != 21){
		Keypad_readDigit();
		if (result != lastResult && result < 10 && count > 5){
						
				if (readDigit == 1){
					userInput = userInput + result*100;
				} else if (readDigit == 2){
					userInput = userInput + result*10;
				} else if (readDigit == 3) {
					userInput = userInput + result;
				} else {
					// ignore after 4 digits...
				}	
			count = 0;
			readDigit++;
		} 
		
		else if (result == 99){
			count++;
		}
		
		else if (result == 21){
			userInput=500;
		}
				
		else{
			
		}
		lastResult = result;	

		//Display(result, 500);
	}
			// Scale back!
			if (readDigit == 3){
				userInput = userInput/100;
			} else if (readDigit == 4){
				userInput = userInput/10;
			} 
			// done reading from user input
			configInit_GPIO(GPIOD, RCC_AHB1Periph_GPIOC,
										GPIO_Pin_13,
										GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP,
										GPIO_PuPd_DOWN);
			GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_SET);

			return;
}

void correctionOutput(int8_t upDown, uint8_t angleType) {
	if (angleType == 2) {
		return;
	}
	
	
}
