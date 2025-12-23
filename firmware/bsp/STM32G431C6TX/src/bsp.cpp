#include "include/bsp.hpp"

#include "stm32g4xx_hal.h"

extern "C" void SystemClock_Config(void);

namespace bsp {

void init() {
  HAL_Init();
  SystemClock_Config();
}

} // namespace bsp