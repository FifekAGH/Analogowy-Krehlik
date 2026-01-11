#include "bsp.hpp"
#include "bsp_adc.hpp"
#include "bsp_led.hpp"
#include "gui.hpp"
#include <cstdint>
#include <cstdlib>

namespace {

volatile float rawAdcValue = 0;

float filterAdc(uint32_t newValue) {
  float x = static_cast<float>(newValue);

  static float y1 = 0.0f;
  static float y2 = 0.0f;

  const float alpha = 0.001f;

  y1 = alpha * x + (1.0f - alpha) * y1;
  y2 = alpha * y1 + (1.0f - alpha) * y2;

  return y2;
}

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
float convertRawToCurrent(float rawValue) {
  float gain = 672.0f;
  float maxAdcReading = 65535.0f;
  float referenceVoltage = 3.3f;
  float shuntResistance = 1e4f;
  float voltage = (rawValue / maxAdcReading) * (referenceVoltage / gain);
  return (voltage * 1e9f) / shuntResistance; // in nA
}

float convertRawToVoltage(float rawValue) {
  float maxAdcReading = 65535.0f;
  float referenceVoltage = 3.3f;
  return (rawValue / maxAdcReading) * referenceVoltage;
}

} // namespace

int main(void) {
  bsp::init();
  gui::init();

  bool isCalibrated = bsp::adc::calibrate();
  if (!isCalibrated) {
    bsp::led::toggle();
    bsp::delayMs(5 * 1000);
    bsp::reset();
  }

  bsp::adc::readChannel(1, [](uint32_t value) { rawAdcValue = filterAdc(value); });

  while (true) {
    float voltage = convertRawToVoltage(rawAdcValue);
    gui::setVoltage(voltage);

    float current = convertRawToCurrent(rawAdcValue);
    gui::setCurrent(current);

    gui::refresh();
    bsp::led::toggle();
  }
}
