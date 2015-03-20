/**
   @file ADC.h
   @brief Configure and Initialize the Analog to Digital Converter and sample temperature.
   @date 2015/02/20
*/

#include "stm32f4xx.h"

/**
	* @brief Initialize the ADC common struct and enable the common ADC component.
	*
		The parameters all correspond to the parameters of the ADC_CommonInitTypeDef struct.
	*
		ADC_Mode is set as independent to only take the value from a single ADC unit.
		There is no need for DMA so it is disabled.
	* @retval None.
	*/
void configInit_common_ADC(void);

/**
	* @brief Initialize the specific ADC component and enable it.
	*
		The bit resolution of the returned values are 12 bits.
		Only single conversion is necessary, so Continuous scan mode is disabled.
		Additionally, the number of conversions is 1.
		The End of Conversion Interrupt is also configured.
	*

	* @retval None.
	*/
void configInit_ADC(void);

/**
		@brief Initialize NVIC for an EOC conversion of the ADC
	
		Has the highest priority so that the value can be returned as fast as possible.
	*/
void configInit_ADC_Int(void);

/**
	* @brief Get the reading from the temperature sensor.
	*
	* @retval The value from the temperature sensor in Volts
	*/
uint32_t getTemp(void);
