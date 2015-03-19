/**
   @file UI.h
   @brief Functions handling 7-segment and keypad usage.
   @date 2015/03/12
*/

#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"

// define the display mode
#define TEMP_MODE (int8_t)(-1)
#define TILT_MODE (int8_t)1

// special key defs
#define NO_INPUT 99
#define ENTER 22
#define STAR 21
#define D_KEY 13
#define C_KEY 12
#define B_KEY 11
#define A_KEY 10

/** @var
	@breif 0 for temperature, 1 for tilt
	*/
static int displayMode;

/** @var
		@brief A counter used to increment the digit display
	*/
static int TIM3_interrupt_count;

/** @var
		@brief Signal 
						- 0 = still waiting for user input
						- 1 = done reading -- ENTER presse
						- 2 = * pressed 
	*/
static int readStatus;

/**
		@brief Which digit to display
		@param val 0 to 3 for the 4 7-seg displays
	*/
void setTIM3_count(int val);
/**
		@brief Get which digit is currently displaying
		@retval int 0 to 3 for the 4 7-seg displays
	*/
int getTIM3_count(void);

/**
		@brief Set the value of the userInput, @ref userInput
		Only sets the value of the variable. Use @ref Keypad_read to get user input from keypad
		@param val The value to set the userInput
	*/
void setReadStatus(int val);
/**
		@brief Get the value of the userInput, @ref userInput
		@retval The value of the userInput
	*/
int getReadStatus(void);

/**
		@brief Set the display mode, @ref displayMode
		@param val The display mode
	*/
void setDisplayMode(int8_t val);
/**
		@brief Get the display mode, @ref displayMode
		@retval The value of the display mode
	*/
int8_t getDisplayMode(void);

/**
	*	@brief Display using 7-segment display
	*	
	*	- Handle both interger and floating point inputs
	*	- Separate input by digit 
	*	- Select digit to display
	*	- Call digit displays accordingly
	*
	* @param n The number to display
	*/
void Display(float n);

/** @brief Display a Zero	*/
void Zero(void);
/** @brief Display a One */
void One(void);
/** @brief Display a Two */
void Two(void);
/** @brief Display a Three */
void Three(void);
/** @brief Display a Four */
void Four(void);
/** @brief Display a Five */
void Five(void);
/** @brief Display a Six */
void Six(void);
/** @brief Display a Seven */
void Seven(void);
/** @brief Display an Eight */
void Eight(void);
/** @brief Display a Nine */
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
int Keypad_readDigit(void);

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
int Keypad_read(void);

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
