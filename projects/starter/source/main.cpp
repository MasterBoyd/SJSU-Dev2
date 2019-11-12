// DO NOT MODIFY THIS PROJECT OR FILE!
// Copy this folder and modify that copied project folder.

// Prefer to include files using the following method:
// https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes

// The start of your program. Unlike programs that run on your computer, this
// main does not take argv and argc parameters, although such a thing may be
// added in the future.

#include <stdint.h>
#include <stdio.h>
#include "utility/time.hpp"
#include "L0_Platform/stm32f10x/stm32f10x.h"

int main()
{
  ///////////// Setup GPIO /////////////

  // Setup LED GPIO
  // Power I/O PortC Peripheral
  RCC->APB2ENR = (1 << 4);
  // Set as output with 10Mhz
  GPIOC->CRH |= (1 << 20);
  GPIOC->BSRR = (1 << 29);

  // Reset Port C Pin 13
  GPIOC->BSRR = (1 << 29);
  sjsu::Delay(1s);

  // Uart1Setup();

  // Set Port C Pin 13
  GPIOC->BSRR = (1 << 13);
  sjsu::Delay(1s);

  while (1)
  {
    // Reset Port C Pin 13
    GPIOC->BSRR = (1 << 29);

    printf("Hello World");
    sjsu::Delay(250ms);

    // Set Port C Pin 13
    GPIOC->BSRR = (1 << 13);

    sjsu::Delay(250ms);
  }

  return 0;
}
