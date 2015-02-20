#include "stm32f4xx.h"                  // Device header

/**
  * @brief Configure a GPIO port and Initialize it.
  *
  * @param GPIOx The GPIO port to configure and initialize. x can take values ""
  * @param periph_GPIOx The GPIO port to specify to initialize the bus clock
  * @param pins The pins of the GPIO to initialize
  * @param mode 
  * @param speed
  * @param oType
  * @param puPd
  */
void configInit_GPIO(GPIO_TypeDef* GPIOx, uint32_t periph_GPIOx, uint32_t pins,
										 GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType, GPIOPuPd_TypeDef puPd);

/**
  * @brief Select one of the four LEDs to turn on.
  *
  * The LEDs are hardwired to GPIO port D. The modulo of @p LED_num is taken to
  * determine which LED to light up.
  * 0:
  * 1:
  * 2:
  * 3:
  * @param LED_num A number selecting the LED to display.
  */
void DisplaySingleLED(int LED_num);
