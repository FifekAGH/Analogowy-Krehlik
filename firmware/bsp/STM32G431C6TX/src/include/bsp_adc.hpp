#ifndef BSP_ADC_HPP_
#define BSP_ADC_HPP_

#include <array>
#include <cstdint>
#include <cstdlib>

namespace bsp {

namespace adc {

typedef void (*OnConversion)(uint32_t value);

/**
 * Calibrate the ADC.
 *
 * @return true if calibration was successful, false otherwise.
 */
bool calibrate(void);

/**
 * Read ADC value using polling method.
 *
 * @param channel ADC channel to read from.
 * @return Raw ADC value.
 */
uint32_t readChannelPolling(uint8_t channel);

void readChannel(uint8_t channel, OnConversion callback);

} // namespace adc

} // namespace bsp

#endif // BSP_ADC_HPP_