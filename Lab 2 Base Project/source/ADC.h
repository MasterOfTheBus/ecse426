/**
   @file ADC.h
   @brief Configure and Initialize the Analog to Digital Converter and sample temperature.
   @date 2015/02/20
*/

#include "stm32f4xx.h"

/**
	* @brief Initialize the ADC common struct and enable the common ADC component.
	*
	* @param mode Corresponds to @ref ADC_CommonInitTypeDef.ADC_Mode
	* @param prescaler Corresponds to @ref ADC_CommonInitTypeDef.ADC_Prescaler
	* @param DMAAccessMode Corresponds to @ref ADC_CommonInitTypeDef.ADC_DMAAccessMode
	* @param twoSamplingDelay Corresponds to @ref ADC_CommonInitTypeDef.ADC_TwoSamplingDelay
	* @retval None.
	*/
void configInit_common_ADC(uint32_t mode,
													 uint32_t prescaler,
													 uint32_t DMAAccessMode,
													 uint32_t twoSamplingDelay);

/**
	* @brief Initialize the specific ADC component and enable it.
	*
	* @param ADCx Corresponds to the ADCx parameter of @ref ADC_RegularChannelConfig
	* @param periph_ADCx Corresponds to the RCC_AHB1Periph parameter of @ref RCC_AHB1PeriphClockCmd
	* @param resolution Corresponds to @ref ADC_InitTypeDef.ADC_Resolution
	* @param scanConvMode Corresponds to @ref ADC_InitTypeDef.ADC_ScanConvMode
	* @param contConvMode Corresponds to @ref ADC_InitTypeDef.ADC_ContinuousConvMode
	* @param externalTrigConvEdge Corresponds to @ref ADC_InitTypeDef.ADC_ExternalTrigConvEdge
	* @param externalTrigConv Corresponds to @ref ADC_InitTypeDef.ADC_ExternalTrigConv
	* @param dataAlign Corresponds to @ref ADC_InitTypeDef.ADC_DataAlign
	* @param nbrOfConversion Corresponds to @ref ADC_InitTypeDef.ADC_NbrOfConversion
	* @param channel Corresponds to the ADC_Channel parameter of @ref ADC_RegularChannelConfig
	* @param rank Corresponds to the Rank parameter of @ref ADC_RegularChannelConfig
	* @param sampleTime Corresponds to the ADC_SampleTime parameter of @ref ADC_RegularChannelConfig
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
