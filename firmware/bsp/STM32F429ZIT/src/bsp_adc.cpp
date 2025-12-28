#include "bsp_adc.hpp"

#include "adc.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc_ex.h"

namespace bsp::adc {

uint32_t readPooling(uint8_t) {
  HAL_ADC_Start(&hadc3);
  if (HAL_OK == HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY)) {
    return HAL_ADC_GetValue(&hadc3);
  }
  return 0;
}

}; // namespace bsp::adc