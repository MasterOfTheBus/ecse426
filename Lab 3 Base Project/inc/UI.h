#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"

/**
	@brief Initialize the TIM Time Base Init struct
	
	@params Prescaler
	@params Counter mode
	@params Period
	@params Clock division
	@params Repetition counter
	*/
extern float numDisplay;
static int TIM3_interrupt;
extern int userInput;
static int angleDisplay;

/**
	*	@brief Hardware Timer
	*	
	*	- Control 7-segment multiplexing operations 
	*	- Delay generated by TIM3
	*	- Use specific bus clock (RCC driver): TimerClockingFrequency / (Period x prescalar) = desired rate
	*/	
void Timer_config(uint16_t Prescaler,
									uint16_t CounterMode,
									uint32_t Period,
									uint16_t ClockDivision,
									uint8_t RepetitionCounter);

/**
	*	@brief Hardware Timer Interrupt
	*	
	*	- Enable timer to throw interrupt once it has reached maximum counter value
	*/
void EnableTimerInterrupt(void);

/**
	*	@brief Hardware Timer Interrupt Handler
	*	
	*	- At every interrupt, display reading or user input
	*/
void TIM3_IRQHandler(void);

/**
	*	@brief Display using 7-segment display
	*	
	*	- Handle both interger and floating point inputs
	*	- Separate input by digit 
	*	- Select digit to display
	*	- Call digit displays accordingly
	*
	*/
void Display(float n, int input);

/**
	*	@brief 7-segment display digit definition
	*	
	*	- Handle numerical characters from 0 to 9
	*
	*/
void Zero(void);
void One(void);
void Two(void);
void Three(void);
void Four(void);
void Five(void);
void Six(void);
void Seven(void);
void Eight(void);
void Nine(void);

/**
	*	@brief Read user input from keypad
	*	
	*	- Read user input from keypad	(pins LEFT TO RIGHT)
	*																PE6(brown)	PC13(red)	PE4(orange)		PE5(yellow)	PB4(green)		PB5(blue)		PD1(purple)	PD2(black)
	*	- Special mappings:	
	*				A ------ value = 10 --- Equivalent to HEX representation (not used)
	*				B ------ value = 11 --- Equivalent to HEX representation (not used)
	*				C ------ value = 12 --- Equivalent to HEX representation (not used)
	*				D ------ value = 13 --- Equivalent to HEX representation (not used)
	*				# ------ value = 22 --- Used as ENTER
	*				*	------ value = 21	---	Used for calibration (not used)
	*				idle --- value = 99 --- Result when nothing is pressed
	*	
	*/
void Keypad_readDigit(void);

/**
	*	@brief Assembling user input
	*	
	*	- Looking for user input
	*	-	Update when change detected
	*	-	Handle keypad bounce problem by counting the number if consecutive idle
	*	-	Exit when ENTER is pressed
	*	- Special mappings interpretation:	
	*				A ------ value = 10 --- Equivalent to HEX representation (not used)
	*				B ------ value = 11 --- Equivalent to HEX representation (not used)
	*				C ------ value = 12 --- Equivalent to HEX representation (not used)
	*				D ------ value = 13 --- Equivalent to HEX representation (not used)
	*				# ------ value = 22 --- Used as ENTER --- Exit the function
	*				*	------ value = 21	---	Used for calibration (not used)
	*				idle --- value = 99 --- Result when nothing is pressed --- Continue waiting for input
	*	
	*/
void Keypad_read(void);

/**
	*	@brief Configure GPIO for 7-segment display
	*	
	*	- Send signals out through 7-segment display (pins from LEFT TO RIGHT)
	*																PB11(black)	PB12(purple)	PB13(blue)	PB14(green)	PB15(yellow)	PD8(orange)		PD9(red)	PD10(brown)
	*																PE7(black)	PE8(purple)		PE9(blue)		PE10(green)	PE11(yellow)	PE12(orange)	PE13(red)	PE14(brown)															
	*
	*	
	*/
void GPIO_config(void);

/**
  * @brief Configure a GPIO port and Initialize it.
  *
	* The first two parameters correspond to selecting the specific GPIO port and bus.
	* The remaining parameters are used to set the values of the GPIO initilization struct (GPIO_InitTypeDef).
	*
  * @param GPIOx Specifies the GPIO port to enable
  * @param periph_GPIOx Specifies the GPIO port to enable for the bus clock.
  * @param pins Specifies the GPIO pins to be configured.
  * @param mode Specifies the operating mode for the selected pins.
  * @param speed Specifies the speed for the selected pins.
  * @param oType Specifies the operating output type for the selected pins.
  * @param puPd Specifies the operating Pull-up/Pull down for the selected pins.
	* @retval none
  */
void configInit_GPIO(GPIO_TypeDef* GPIOx,
										 uint32_t periph_GPIOx,
										 uint32_t pins,
										 GPIOMode_TypeDef mode,
										 GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType,
										 GPIOPuPd_TypeDef puPd);

/**
	@brief Display the animation to indicate the direction to tilt board to match desired tilt angle

	If angleType is @ref ALPHA then the direction is up/down. If angleType is @ref BETA then the direction is right/left.

	@param upDown The direction to tilt the board. Value of 1 indicates up/right, value of -1 indicates down/left, value of 0 indicates matching.
	*/
void correctionOutput(int8_t upDown);
