#include "bsp.hpp"
#include "bsp_adc.hpp"
#include "bsp_led.hpp"
#include "gui.hpp"
#include <cstdint>
#include <cstdlib>

namespace {

volatile double rawAdcValue = 0;

double filterAdc(uint32_t newValue) {
  double value = static_cast<double>(newValue);
  static double filteredValue = 0.0f;
  const double alpha = 0.01;
  filteredValue = alpha * value + (1 - alpha) * filteredValue;
  return filteredValue;
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
double convertRawToCurrent(double rawValue) {
  double gain = 672.0f;
  double maxAdcReading = 65535.0f;
  double referenceVoltage = 3.3f;
  double shuntResistance = 1e4f;
  double voltage = (rawValue / maxAdcReading) * (referenceVoltage / gain);
  return (voltage * 1e9f) / shuntResistance; // in nA
}

double convertRawToVoltage(double rawValue) {
  double maxAdcReading = 65535.0f;
  double referenceVoltage = 3.3f;
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
    double voltage = convertRawToVoltage(rawAdcValue);
    gui::setVoltage(voltage);

    double current = convertRawToCurrent(rawAdcValue);
    gui::setCurrent(current);

    gui::refresh();
    bsp::led::toggle();
  }
}
