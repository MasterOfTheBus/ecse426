#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

#define config_init 1

void configInit_GPIO(GPIO_TypeDef* GPIOx, uint32_t periph_GPIOx, uint32_t pins,
										 GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType, GPIOPuPd_TypeDef puPd);
void configInit_common_ADC(uint32_t mode, uint32_t prescaler,
													 uint32_t DMAAccessMode, uint32_t twoSamplingDelay);
void configInit_ADC(uint32_t periph_ADCx, uint32_t resolution, FunctionalState scanConvMode,
										FunctionalState contConvMode, uint32_t externalTrigConvEdge,
										uint32_t externalTrigConv, uint32_t dataAlign,
										uint8_t nbrOfConversion);