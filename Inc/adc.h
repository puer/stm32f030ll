#if !defined(__ADC_H)
#define __ADC_H
#include "stm32f0xx_ll_adc.h"
#include "stm32f0xx_ll_dma.h"

#define ADBufferSize 16
extern uint16_t ADC_ConvertedValue[ADBufferSize];
extern void start_adc_dma(ADC_TypeDef *adc, DMA_TypeDef *dma);

#endif // __ADC_H
