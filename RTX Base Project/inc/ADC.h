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
	* @param mode Configures the ADC to operate in independent or multi mode.
	* @param prescaler Select the frequency of the clock to the ADC. The clock is common for all the ADCs.
	* @param DMAAccessMode Configures the Direct memory access mode for multi ADC mode.
	* @param twoSamplingDelay Configures the Delay between 2 sampling phases.
	* @retval None.
	*/
void configInit_common_ADC(void);

/**
	* @brief Initialize the specific ADC component and enable it.
	*
		The periph_ADCx parameter corresponds to the parameter in the RCC_AHB1PeriphClockCmd function.

		The parameters resolution, scanConvMode, contConvMode, externalTrigConvEdge, externalTrigConv, dataAlign and nbrOfConversion all
		correspond to the parameters of the ADC_InitTypeDef struct used to initialize the specific ADC component.

		The parameters ADCx, channel, rank, and sampleTime correspond to the parameters of the ADC_RegularChannelConfig funtion used to configure
		the ADC channels.
	*

	* @retval None.
	*/
void configInit_ADC(void);

/**
	@brief Initialize NVIC for an EOC conversion of the ADC
	*/
void configInit_ADC_Int(void);

/**
	* @brief Get the reading from the temperature sensor.
	*
	* @retval The value from the temperature sensor in Volts
	*/
uint32_t getTemp(void);
