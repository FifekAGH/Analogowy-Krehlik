#ifndef BSP_ADC_HPP_
#define BSP_ADC_HPP_

#include <cstdint>
#include <cstdlib>

namespace bsp {

using OnConversionComplete = void (*)(uint32_t *values, size_t length);

namespace adc {

/**
 * Set the callback function to be called upon ADC conversion completion.
 *
 * @param callback Function pointer to the user-defined callback.
 */
void setOnConversionCompleteISR(OnConversionComplete callback);

/**
 * Request an ADC conversion for all 2 channels.
 * This function starts an ADC conversion in interrupt mode.
 */
void requestConversion();

/**
 * Read ADC value using polling method.
 *
 * @param channel ADC channel to read from.
 * @return Raw ADC value.
 */
uint32_t readPolling(uint8_t channel);

} // namespace adc

} // namespace bsp

#endif // BSP_ADC_HPP_