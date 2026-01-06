#include "bsp_adc.hpp"

#include "adc.h"
#include "stm32g4xx_hal_adc.h"
#include "stm32g4xx_hal_adc_ex.h"
#include "stm32g4xx_hal_def.h"
#include <cstdint>
#include <cstring>

enum {
  ADC_CHANNEL_COUNT = 2,
};

namespace {
uint32_t getChannel(uint8_t channel) {
  switch (channel) {
  case 0:
    return ADC_CHANNEL_1;
  case 1:
    return ADC_CHANNEL_2;
  default:
    return ADC_CHANNEL_VREFINT;
  };
}

} // namespace

namespace bsp::adc {

bool calibrate(void) {
  if (HAL_OK != HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED)) {
    return false;
  }
  return true;
}

uint32_t readChannelPolling(uint8_t channel) {
  HAL_ADC_Start(&hadc1);
  if (HAL_OK == HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY)) {
    return HAL_ADC_GetValue(&hadc1);
  }
  return 0;
}

}; // namespace bsp::adc