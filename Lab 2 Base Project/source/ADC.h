#include "stm32f4xx.h"                  // Device header

void configInit_common_ADC(uint32_t mode, uint32_t prescaler,
													 uint32_t DMAAccessMode, uint32_t twoSamplingDelay);
void configInit_ADC(uint32_t periph_ADCx, uint32_t resolution, FunctionalState scanConvMode,
										FunctionalState contConvMode, uint32_t externalTrigConvEdge,
										uint32_t externalTrigConv, uint32_t dataAlign,
										uint8_t nbrOfConversion);
