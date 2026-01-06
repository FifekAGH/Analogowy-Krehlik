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
double convertRawToCurrent(uint32_t rawValue) {
  double gain = 683.94f;
  double maxAdcReading = 65535.0;
  double referenceVoltage = 3.3;
  double shuntResistance = 1e4;

  double voltage = (static_cast<double>(rawValue) / maxAdcReading) * (referenceVoltage / gain);
  return (voltage * 1e9) / shuntResistance; // in nA
}

} // namespace

int main(void) {
  bsp::init();
  gui::init();

  gui::showCredits();

  bool isCalibrated = bsp::adc::calibrate();
  if (!isCalibrated) {
    bsp::led::toggle();
    bsp::delayMs(5 * 1000);
    bsp::reset();
  }

  while (true) {
    bsp::delayMs(GUI_UPDATE_INTERVAL_MS);
    uint32_t rawAdcValue = bsp::adc::readChannelPolling(1);
    double current = convertRawToCurrent(rawAdcValue);
    gui::setCurrent(current);
    // double voltage = (static_cast<double>(rawAdcValue) / 65535.0) * 3.3;
    // gui::setVoltage(voltage);
    gui::refresh();
    bsp::led::toggle();
  }
}
