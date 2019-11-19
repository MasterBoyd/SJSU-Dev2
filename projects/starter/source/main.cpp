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

char Uart1Write(char data_send)
{
  // transfer data to DR register
  USART1->DR = data_send;
  // wait until data transmission is complete
  while ((USART1->SR & (1 << 7)) == 0)
  {
  };
}

void Uart1Setup()
{
  // make sure the relevant pins are appropriately set up.
  RCC->APB2ENR |= (1 << 2);      // enable clock for GPIOA
  RCC->APB2ENR |= (1 << 14);     // enable clock for USART1
  GPIOA->CRH &= ~(0b1111 << 4);  // Tx (PA9) alt. out push-pull
  GPIOA->CRH |= (0b1001 << 4);   // Tx (PA9) alt. out push-pull
  GPIOA->CRH |= (0b0100 << 8);   // Rx (PA10) in floating
  // USART1->CR3 |= (1 << 9);
  // 72Mhz/(9600*16) = 468.75    Man = 468 Div = 12 Hex = 0x1d4c = 8741
  // 72Mhz/(115200*16) = 39.0625 Man = 39 Div = 1   Hex = 0x391
  USART1->BRR = 0;
  USART1->BRR |= (13 << 4);  // Mantissa
  // USART1->BRR |= (15 << 0);    // Div
  // CR1 bit 12 M = bit Default of 0 means 1 start bit, 8 data bits, n stop bits
  // CR2 bits 13:12 Stop bit of 0 mean 1 stop bit
  USART1->CR1 |= (1 << 13);  // UE enable
  USART1->CR1 |= (1 << 3);   // TX enable
  USART1->CR1 |= (1 << 2);   // RX enable
}

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

    printf("Hello");
    sjsu::Delay(250ms);

    // Set Port C Pin 13
    GPIOC->BSRR = (1 << 13);

    sjsu::Delay(250ms);
  }

  return 0;
}
