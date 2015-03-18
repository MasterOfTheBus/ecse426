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
void configInit_common_ADC(uint32_t mode,
													 uint32_t prescaler,
													 uint32_t DMAAccessMode,
													 uint32_t twoSamplingDelay);

/**
	* @brief Initialize the specific ADC component and enable it.
	*
		The periph_ADCx parameter corresponds to the parameter in the RCC_AHB1PeriphClockCmd function.

		The parameters resolution, scanConvMode, contConvMode, externalTrigConvEdge, externalTrigConv, dataAlign and nbrOfConversion all
		correspond to the parameters of the ADC_InitTypeDef struct used to initialize the specific ADC component.

		The parameters ADCx, channel, rank, and sampleTime correspond to the parameters of the ADC_RegularChannelConfig funtion used to configure
		the ADC channels.
	*
	* @param ADCx Where x can be 1, 2 or 3 to select the ADC peripheral.
	* @param periph_ADCx Specifies the AHB1 peripheral to gates its clock.
	* @param resolution Configures the ADC resolution dual mode.
	* @param scanConvMode Specifies whether the conversion is performed in Scan (multichannels) 
                                               or Single (one channel) mode.
												This parameter can be set to ENABLE or DISABLE
	* @param contConvMode Specifies whether the conversion is performed in Continuous or Single mode.
                        This parameter can be set to ENABLE or DISABLE.
	* @param externalTrigConvEdge Select the external trigger edge and enable the trigger of a regular group.
	* @param externalTrigConv Select the external event used to trigger the start of conversion of a regular group.
	* @param dataAlign Specifies whether the ADC data  alignment is left or right.
	* @param nbrOfConversion Specifies the number of ADC conversions
                                               that will be done using the sequencer for
                                               regular channel group.
                                               This parameter must range from 1 to 16.
	* @param channel The ADC channel to configure.
	* @param rank The rank in the regular group sequencer.
  *          		This parameter must be between 1 to 16.
	* @param sampleTime The sample time value to be set for the selected channel.
	* @retval None.
	*/
void configInit_ADC(ADC_TypeDef* ADCx,
										uint32_t periph_ADCx,
										uint32_t resolution,
										FunctionalState scanConvMode,
										FunctionalState contConvMode,
										uint32_t externalTrigConvEdge,
										uint32_t externalTrigConv,
										uint32_t dataAlign,
										uint8_t nbrOfConversion,
										uint8_t channel,
										uint8_t rank,
										uint8_t sampleTime);

/**
	* @brief Get the reading from the temperature sensor.
	*
	* @retval The value from the temperature sensor in Volts
	*/
uint32_t getTemp(void);
