#include "bsp_adc.hpp"

#include "adc.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_adc_ex.h"

namespace {

bsp::OnConversionComplete userCallback = nullptr;

} // namespace

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (ADC3 == hadc->Instance) {
    if (userCallback != nullptr) {
      uint32_t value = HAL_ADC_GetValue(hadc);
      userCallback(&value, 1);
    }
  }
}

namespace bsp::adc {

void setOnConversionCompleteISR(OnConversionComplete callback) { userCallback = callback; }

void requestConversion() { HAL_ADC_Start_IT(&hadc3); }

uint32_t readPolling(uint8_t) {
  HAL_ADC_Start(&hadc3);
  if (HAL_OK == HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY)) {
    return HAL_ADC_GetValue(&hadc3);
  }
  return 0;
}

}; // namespace bsp::adc