#include "display.hpp"

#include "ssd1306_display.hpp"
#include <string>

namespace display {

void init(void) { ssd1306::init(); }

void update(void) { ssd1306::update(); }

void showValue(float value) {
  // todo
  ssd1306::drawString(std::to_string(value).c_str(), 0, 0, NULL);
}

} // namespace display