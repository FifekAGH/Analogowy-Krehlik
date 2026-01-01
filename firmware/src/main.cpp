#include "bsp.hpp"
#include "bsp_adc.hpp"
#include "bsp_led.hpp"
#include "gui.hpp"
#include <cstdint>
#include <cstdlib>

enum {
  GUI_UPDATE_INTERVAL_MS = 20,
};

namespace {

/**
 * Convert raw ADC reading to current in nA
 *
 * Using the formula:
 * V_shunt = (Raw_ADC / Max_ADC_Reading) * (V_ref / Gain)
 * I = V_shunt / R_shunt
 *
 * @param rawValue Raw ADC reading.
 * @return Current in nA.
 */
float convertRawToCurrent(uint32_t rawValue) {
  float gain = 10000.0f;
  float maxAdcReading = 65535.0f;
  float referenceVoltage = 3.3f;
  float shuntResistance = 1e3f;

  float voltage = (static_cast<float>(rawValue) / maxAdcReading) * (referenceVoltage / gain);
  float current = voltage / shuntResistance; // in Amperes
  return current * 1e9f;                     // convert to nA
}

} // namespace

int main(void) {
  bsp::init();
  gui::init();

  gui::showCredits();
  while (true) {
    bsp::delayMs(GUI_UPDATE_INTERVAL_MS);
    uint32_t rawAdcValue = bsp::adc::readChannelPolling(1);
    float current = convertRawToCurrent(rawAdcValue);
    gui::setCurrent(current);
    gui::refresh();
    bsp::led::toggle();
  }
}
