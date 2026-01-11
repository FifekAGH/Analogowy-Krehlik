#include "SEGGER_RTT.h"
#include "bsp.hpp"
#include "bsp_adc.hpp"
#include "bsp_led.hpp"
#include "gui.hpp"

namespace {

typedef struct {
  float b0, b1, b2;
  float a1, a2;
  float z1, z2;
} Biquad;

float offset = 0;
float rawAdcValue = 0;
Biquad biquad;

void BiquadInit(Biquad *biquad, float b0, float b1, float b2, float a1, float a2) {
  biquad->b0 = b0;
  biquad->b1 = b1;
  biquad->b2 = b2;
  biquad->a1 = a1;
  biquad->a2 = a2;
  biquad->z1 = 0;
  biquad->z2 = 0;
}

float BiquadProcess(Biquad *biquad, float in) {
  float out = biquad->b0 * in + biquad->z1;
  biquad->z1 = biquad->b1 * in - biquad->a1 * out + biquad->z2;
  biquad->z2 = biquad->b2 * in - biquad->a2 * out;
  return out;
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

  //  Create a Biquad low-pass filter with a cutoff frequency of 2 Hz and a sample rate of 12 kHz
  BiquadInit(&biquad, 2.9830186087320927e-7f, 5.966037217464185e-7f, 2.9830186087320927e-7f, -1.9985806862052193f, 0.9985818794126626f);
  bsp::adc::readChannel([](uint32_t value) { rawAdcValue = BiquadProcess(&biquad, static_cast<float>(value)); });

  while (true) {
    bsp::adc::disableInterrupts();
    float filteredAdcValue = rawAdcValue;
    bsp::adc::enableInterrupts();

    float voltage = convertRawToVoltage(filteredAdcValue - offset);
    gui::setVoltage(voltage);

    float current = convertRawToCurrent(filteredAdcValue - offset);
    gui::setCurrent(current);

    gui::refresh();
    bsp::led::toggle();
  }
}
