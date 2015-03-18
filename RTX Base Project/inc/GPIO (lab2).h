/**
   @file GPIO.h
   @brief Configure and Initialize the GPIO and set LEDs.
   @date 2015/02/20
*/

#include "stm32f4xx.h"

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
  * @brief Select one of the four LEDs to turn on.
  *
  The LEDs are hardwired to GPIO port D. The modulo of @p LED_num is taken to
  determine which LED to light up.

	The modulo values correspond accordingly:
  - 0: GPIO pin 12
  - 1: GPIO pin 13
  - 2: GPIO pin 14
  - 3: GPIO pin 15
	
  * @param LED_num A number selecting the LED to display.
	* @retval None.
  */
void DisplaySingleLED(int LED_num);
