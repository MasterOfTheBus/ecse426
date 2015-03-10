#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"

/**
	@brief Initialize the TIM Time Base Init struct
	
	@params Prescaler
	@params Counter mode
	@params Period
	@params Clock division
	@params Repetition counter
	*/
static float numDisplay;
static int TIM3_interrupt;

void Timer_config(uint16_t Prescaler,
									uint16_t CounterMode,
									uint32_t Period,
									uint16_t ClockDivision,
									uint8_t RepetitionCounter);

void EnableTimerInterrupt(void);

void TIM3_IRQHandler(void);

void Display(float n);

void Zero(void);
void One(void);
void Two(void);
void Three(void);
void Four(void);
void Five(void);
void Six(void);
void Seven(void);
void Eight(void);
void Nine(void);

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
void GPIO_config(void);
void configInit_GPIO(GPIO_TypeDef* GPIOx,
										 uint32_t periph_GPIOx,
										 uint32_t pins,
										 GPIOMode_TypeDef mode,
										 GPIOSpeed_TypeDef speed,
										 GPIOOType_TypeDef oType,
										 GPIOPuPd_TypeDef puPd);


