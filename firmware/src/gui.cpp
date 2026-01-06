#include "gui.hpp"
#include "bsp.hpp"

#include "ssd1306_display.hpp"
#include "ssd1306_display_fonts.hpp"

#include <cstdint>
#include <format>
#include <string>

namespace {
char buffer[128];
}

namespace gui {

void init(void) { ssd1306::Init(); }

void showCredits(void) {
  ssd1306::Fill(ssd1306::Black);
  ssd1306::SetCursor(5, 12);
  ssd1306::WriteString("PicoAmmeter", ssd1306::Font_16x15, ssd1306::White);
  ssd1306::UpdateScreen();
  bsp::delayMs(1000);

  // ssd1306::Fill(ssd1306::Black);
  // ssd1306::SetCursor(0, 0);
  // ssd1306::WriteString("by: Krzysztof Sikora,", ssd1306::Font_6x8, ssd1306::White);
  // ssd1306::SetCursor(0, 9);
  // ssd1306::WriteString("Dominik Michalczyk,", ssd1306::Font_6x8, ssd1306::White);
  // ssd1306::SetCursor(0, 18);
  // ssd1306::WriteString("Filip Kulka", ssd1306::Font_6x8, ssd1306::White);
  // ssd1306::UpdateScreen();
  // bsp::delayMs(1000);
}

void setCurrent(double current) {
  bool isInNanoAmps = (current > 9.999 && current < 999.9);
  bool isInPicoAmps = (current <= 9.999);

  ssd1306::Fill(ssd1306::Black);
  ssd1306::SetCursor(0, 8);

  if (isInPicoAmps) {
    std::snprintf(buffer, sizeof(buffer), "%0.3f nA", current);
  } else if (isInNanoAmps) {
    std::snprintf(buffer, sizeof(buffer), "%0.1f nA", current);
  } else {
    std::snprintf(buffer, sizeof(buffer), "err");
  }

  ssd1306::WriteString(buffer, ssd1306::Font_16x24, ssd1306::White);
}

void setVoltage(float voltage) {
  ssd1306::Fill(ssd1306::Black);
  ssd1306::SetCursor(0, 8);
  std::snprintf(buffer, sizeof(buffer), "%0.3f V", voltage);
  ssd1306::WriteString(buffer, ssd1306::Font_16x24, ssd1306::White);
}

void refresh(void) { ssd1306::UpdateScreen(); }

} // namespace gui