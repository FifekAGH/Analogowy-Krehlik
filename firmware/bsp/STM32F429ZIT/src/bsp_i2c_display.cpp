#include "bsp_display_bus.hpp"
#include "i2c.h"
#include "stm32f4xx_hal.h"

namespace bsp::displayBus {

void write(uint8_t address, uint8_t memoryAddress, const uint8_t *data, uint16_t length) {
  HAL_I2C_Mem_Write(&hi2c3, address << 1, memoryAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *)data, length, HAL_MAX_DELAY);
}

void read(uint8_t address, uint8_t memoryAddress, uint8_t *data, uint16_t length) {
  HAL_I2C_Mem_Read(&hi2c3, address << 1, memoryAddress, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

} // namespace bsp::displayBus
