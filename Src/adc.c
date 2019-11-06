#include "adc.h"

uint16_t ADC_ConvertedValue[ADBufferSize];
// uint16_t ADC_Calibration = 0;

void start_adc_dma(ADC_TypeDef *adc, DMA_TypeDef *dma)
{
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, ADBufferSize);

    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(adc, LL_ADC_DMA_REG_REGULAR_DATA));

    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)ADC_ConvertedValue);

    // save dma setting before calibration and set back after calibration is finished.
    uint32_t dma_transfer = LL_ADC_REG_GetDMATransfer(adc);
    LL_ADC_REG_SetDMATransfer(adc, LL_ADC_REG_DMA_TRANSFER_NONE);

    LL_ADC_StartCalibration(adc);

    while (LL_ADC_IsCalibrationOnGoing(adc))
        ;
    // ADC_Calibration = LL_ADC_REG_ReadConversionData8(adc);

    LL_ADC_REG_SetDMATransfer(adc, dma_transfer);

    LL_ADC_Enable(adc);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    LL_ADC_REG_StartConversion(adc);
}