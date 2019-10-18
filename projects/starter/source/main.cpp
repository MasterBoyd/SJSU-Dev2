// DO NOT MODIFY THIS PROJECT OR FILE!
// Copy this folder and modify that copied project folder.

// Prefer to include files using the following method:
// https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes

// The start of your program. Unlike programs that run on your computer, this
// main does not take argv and argc parameters, although such a thing may be
// added in the future.

#include <stdint.h>
#include "utility/time.hpp"
#include "L0_Platform/stm32f10x/stm32f10x.h"

// #ifdef __cplusplus
// #define __I volatile /*!< defines 'read only' permissions      */
// #else
// #define __I volatile const /*!< defines 'read only' permissions      */
// #endif
// #define __O volatile  /*!< defines 'write only' permissions     */
// #define __IO volatile /*!< defines 'read / write' permissions   */
// #include <stdint.h>

// /*------------------------ General Purpose and Alternate Function IO
// ---------*/ typedef struct
// {
//   __IO uint32_t CRL;
//   __IO uint32_t CRH;
//   __IO uint32_t IDR;
//   __IO uint32_t ODR;
//   __IO uint32_t BSRR;
//   __IO uint32_t BRR;
//   __IO uint32_t LCKR;
// } GPIO_TypeDef;

// typedef struct
// {
//   __IO uint32_t CR;
//   __IO uint32_t CFGR;
//   __IO uint32_t CIR;
//   __IO uint32_t APB2RSTR;
//   __IO uint32_t APB1RSTR;
//   __IO uint32_t AHBENR;
//   __IO uint32_t APB2ENR;
//   __IO uint32_t APB1ENR;
//   __IO uint32_t BDCR;
//   __IO uint32_t CSR;

// #ifdef STM32F10X_CL
//   __IO uint32_t AHBRSTR;
//   __IO uint32_t CFGR2;
// #endif /* STM32F10X_CL */

// #if defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || \
//     defined(STM32F10X_HD_VL)
//   uint32_t RESERVED0;
//   __IO uint32_t CFGR2;
// #endif /* STM32F10X_LD_VL || STM32F10X_MD_VL || STM32F10X_HD_VL */
// } RCC_TypeDef;

// #define PERIPH_BASE ((uint32_t)0x40000000)
// /*!< Peripheral base address in the alias region */

// #define APB1PERIPH_BASE PERIPH_BASE
// #define APB2PERIPH_BASE (PERIPH_BASE + 0x10000)
// #define AHBPERIPH_BASE (PERIPH_BASE + 0x20000)

// #define GPIOA_BASE (APB2PERIPH_BASE + 0x0800)
// #define GPIOB_BASE (APB2PERIPH_BASE + 0x0C00)
// #define GPIOC_BASE (APB2PERIPH_BASE + 0x1000)
// #define GPIOD_BASE (APB2PERIPH_BASE + 0x1400)
// #define GPIOE_BASE (APB2PERIPH_BASE + 0x1800)
// #define GPIOF_BASE (APB2PERIPH_BASE + 0x1C00)
// #define GPIOG_BASE (APB2PERIPH_BASE + 0x2000)

// #define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
// #define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
// #define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)
// #define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)
// #define GPIOE ((GPIO_TypeDef *)GPIOE_BASE)
// #define GPIOF ((GPIO_TypeDef *)GPIOF_BASE)
// #define GPIOG ((GPIO_TypeDef *)GPIOG_BASE)

// #define RCC_BASE (AHBPERIPH_BASE + 0x1000)
// #define RCC ((RCC_TypeDef *)RCC_BASE)

// // *((GPIO_TypeDef *)(0x4001'1000))->BSRR
// #define DEAY_TICKS 100000

void UartWrite(char data_send)
{
  USART2->DR = data_send;
  While(USART->CR2 & (1 << 3)){};
}

int main()
{
  ///////////// Setup GPIO /////////////

  // Setup LED GPIO
  RCC->APB2ENR = 0x10;
  GPIOC->CRH |= (1 << 20);  // bits 20 & 21 = 01
  GPIOC->BSRR = (1 << 29);

  ///////////// Setup UART /////////////

  //// Power on Peripherals ////
  RCC->APB2ENR |= (1 << 4);   // Power I/OPortA Peripheral
  RCC->APB1ENR |= (1 << 17);  // Reset USART2 Peripheral

  //// Configure Pin Modes ////
  // 12 = TX2 with Alternate function push/pull
  // Set Output with 10 Mhz
  GPIOA->CRH &= ~((1 << 16) & (1 << 17)) GPIOA->CRH |= (1 << 16);
  // bits 18 & 19 = 10'b (Alternate function push/pull)
  GPIOA->CRH &= ~((1 << 18) & (1 << 19)) GPIOA->CRH |= (1 << 19);

  // 13 = RX2 with Input floating or pulled up
  // Set as Input
  GPIOA->CRH &= ~((1 << 20) & (1 << 21));
  // bits 22 & 23 = 10'b (Input with Pull Up/Pull Down)
  GPIOA->CRH &= ~((1 << 22) & (1 << 23)) GPIOA->CRH |= (1 << 23);

  //// Configure UART2 ////
  // Enable the USART by writing the UE bit in USART_CR1 register to 1.
  USART2->CR1 |= (1 << 13);
  // Clear M bit so 8 bit word length (with no parity bit) is used.
  USART2->CR1 &= ~(1 << 12);
  // Clears bits 12 & 13 to select 1 stop bit.
  USART2->CR2 &= ~(3 << 12);
  // Select the desired baud rate using the USART_BRR register.
  // Reset the first byte
  USART2->BRR &= ~(0x00FF);
  // Baud = fpk / (16 * USARTDIV)
  // fpk = 36Mhz
  // USART_BSS DIV_Mantissa = 58(decimal) or 0x3A
  //           DIV_Fraction = 9 (decimal) or 0x09
  USART2->BRR |= (0x09 | (0x3A << 4));

  // Set the TE bit in USART_CR1 to send an idle frame as first
  // transmission.
  USART2->CR2 |= (1 << 3);

  while (1)
  {
    // Reset Port C Pin 13
    GPIOC->BSRR = (1 << 29);

    UartWrite("H");
    UartWrite("E");
    UartWrite("L");
    UartWrite("L");
    UartWrite("O");
    UartWrite("W");
    UartWrite("o");
    UartWrite("r");
    UartWrite("l");
    UartWrite("d");

    sjsu::Delay(1s);

    // Set Port C Pin 13
    GPIOC->BSRR = (1 << 13);

    sjsu::Delay(1s);
  }

  return 0;
}
