#include "bsp.hpp"
#include "bsp_adc.hpp"
#include "bsp_led.hpp"
#include "display.hpp"

int main(void) {
  bsp::init();
  display::init();

  while (true) {
    uint32_t value = bsp::adc::readPooling(1);
    bsp::led::toggle();
    display::update();

    bsp::delayMs(1000);
  }
}