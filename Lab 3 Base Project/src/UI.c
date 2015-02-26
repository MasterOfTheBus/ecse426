#include "UI.h"
#include "GPIO.h"

/**
	*	@brief Initialize user interface using GPIO
	*	
	*	- Read user input from keypad	(pins right to left)
	*																PE6(black)	PC13(purple)	PE4(blue)		PE5(green)	PB4(yellow)		PB5(orange)		PD1(red)	PD2(brown)
	*	- Translate numbers to be displayed into control signals
	*	- Send signals out through 7-segment display (pins from right to left)
	*																PB11(black)	PB12(purple)	PB13(blue)	PB14(green)	PB15(yellow)	PD8(orange)		PD9(red)	PD10(brown)
	*																PE7(black)	PE8(purple)		PE9(blue)		PE10(green)	PE11(yellow)	PE12(orange)	PE13(red)	PE14(brown)															
	*
	*	Must add the pin layout~
	*/