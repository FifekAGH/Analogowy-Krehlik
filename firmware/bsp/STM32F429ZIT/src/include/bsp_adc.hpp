#ifndef BSP_ADC_HPP_
#define BSP_ADC_HPP_

#include <cstdint>

namespace bsp {

namespace adc {

uint32_t readPooling(uint8_t channel);

} // namespace adc

} // namespace bsp

#endif // BSP_ADC_HPP_