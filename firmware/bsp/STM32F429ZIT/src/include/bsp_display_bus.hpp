#ifndef BSP_I2C_HPP_
#define BSP_I2C_HPP_

#include <cstdint>

namespace bsp::displayBus {

void write(uint8_t address, uint8_t memoryAddress, const uint8_t *data, uint16_t length);

void read(uint8_t address, uint8_t memoryAddress, uint8_t *data, uint16_t length);

} // namespace bsp::displayBus

#endif // BSP_DISPLAY_BUS_HPP_