#include "ssd1306_display_port.hpp"
#include "ssd1306_display_registers.hpp"

#include "bsp_display_bus.hpp"

namespace ssd1306 {

void writeCommand(uint8_t command) { bsp::displayBus::write(SSD1306_I2C_ADDR, &command, 1); }

void writeData(uint8_t *data, uint16_t size) {
  uint8_t toWrite = 0x40;
  bsp::displayBus::write(SSD1306_I2C_ADDR, &toWrite, 1);
  bsp::displayBus::write(SSD1306_I2C_ADDR, data, size);
}

} // namespace ssd1306
