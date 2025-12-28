#ifndef SSD1306_DISPLAY_PORT_HPP_
#define SSD1306_DISPLAY_PORT_HPP_

#include "bsp_display_bus.hpp"

#include <cstdint>

namespace ssd1306 {

void writeCommand(uint8_t command);

void writeData(uint8_t *data, uint16_t size);

} // namespace ssd1306

#endif