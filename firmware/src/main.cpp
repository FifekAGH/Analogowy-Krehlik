#include "bsp.hpp"

int main(void) {
  bsp::init();

  while (true) {
    volatile static int counter = 0;
    counter++;
  }
}