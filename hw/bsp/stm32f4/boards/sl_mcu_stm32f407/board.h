/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

/* metadata:
   name: STM32 F407 Streamline Custom
*/

#ifndef BOARD_H_
#define BOARD_H_

#ifdef __cplusplus
 extern "C" {
#endif

// Enable PA9/PA10 as the debug log UART (USART1)
#define UART_ID               1

#define PINID_LED      0
#define PINID_BUTTON   1
#define PINID_UART_TX  2
#define PINID_UART_RX  3

#define VBUS_SENSE_EN  0

static board_pindef_t board_pindef[] = {
  { // LED on PC13
    .port = GPIOC,
    .pin_init = { .Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_HIGH, .Alternate = 0 },
    .active_state = 0 // 0 if active low (common for C13), change to 1 if active high
  },
  { // BUTTON (Keeping PE4 from original configuration)
    .port = GPIOE,
    .pin_init = { .Pin = GPIO_PIN_4, .Mode = GPIO_MODE_INPUT, .Pull = GPIO_PULLUP, .Speed = GPIO_SPEED_HIGH, .Alternate = 0 },
    .active_state = 0
  },
  { // UART1 TX on PA9
    .port = GPIOA,
    .pin_init = { .Pin = GPIO_PIN_9, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_PULLUP, .Speed = GPIO_SPEED_HIGH, .Alternate = GPIO_AF7_USART1 },
    .active_state = 0
  },
  { // UART1 RX on PA10
    .port = GPIOA,
    .pin_init = { .Pin = GPIO_PIN_10, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_PULLUP, .Speed = GPIO_SPEED_HIGH, .Alternate = GPIO_AF7_USART1 },
    .active_state = 0
  },
};

//--------------------------------------------------------------------+
// RCC Clock Configuration (16 MHz Crystal)
//--------------------------------------------------------------------+
static inline void board_clock_init(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Optimize power consumption */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with 16 MHz HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  
  // Math for 16 MHz HSE -> 168 MHz SysClock & 48 MHz USB Clock:
  RCC_OscInitStruct.PLL.PLLM = 8;    // 16 MHz / 8 = 2 MHz (PLL input frequency)
  RCC_OscInitStruct.PLL.PLLN = 336;  // 2 MHz * 336 = 336 MHz (VCO frequency)
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // 336 MHz / 2 = 168 MHz (System Clock)
  RCC_OscInitStruct.PLL.PLLQ = 7;    // 336 MHz / 7 = 48 MHz (USB Clock)
  
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure bus dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4; // 42 MHz Max
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; // 84 MHz Max
  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

static inline void board_vbus_set(uint8_t rhport, bool state) {
  (void) rhport; (void) state;
}

#ifdef __cplusplus
 }
#endif

#endif /* BOARD_H_ */
