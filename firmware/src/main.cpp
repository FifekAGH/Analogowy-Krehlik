#include "bsp.hpp"
#include "bsp_adc.hpp"
#include "bsp_led.hpp"
#include "gui.hpp"
#include <cstdint>
#include <cstdlib>

enum {
  GUI_UPDATE_INTERVAL_MS = 100,
};

namespace {

volatile bool adcConversionComplete = false;
volatile uint32_t rawValueChannel0 = 0;
volatile uint32_t rawValueChannel1 = 0;

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

void onConversionCompleteISR(uint32_t *values, size_t length) {
  if (1 > length) {
    return;
  }

  rawValueChannel0 = values[0];
  adcConversionComplete = true;
}

} // namespace

int main(void) {
  bsp::init();
  gui::init();

  gui::showCredits();
  bsp::adc::setOnConversionCompleteISR(onConversionCompleteISR);
  bsp::adc::requestConversion();
  while (true) {
    if (adcConversionComplete) {
      adcConversionComplete = false;
      float current = current + convertRawToCurrent(rawValueChannel0);
      gui::setCurrent(current);
      gui::refresh();
      bsp::led::toggle();
      bsp::adc::requestConversion();
    }
  }
}
