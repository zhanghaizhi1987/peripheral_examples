/**************************************************************************//**
 * @main_series1_GG11.c
 * @brief Demonstrates USART0 as SPI master.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#define TX_BUFFER_SIZE   10
#define RX_BUFFER_SIZE   TX_BUFFER_SIZE

uint8_t TxBuffer[TX_BUFFER_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
uint8_t RxBuffer[RX_BUFFER_SIZE] = {0};

/**************************************************************************//**
 * @brief Initialize USART0
 *****************************************************************************/
void initUSART0 (void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART0, true);

  // Configure GPIO mode
  GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull, 0); // US0_CLK is push pull
  GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull, 1); // US0_CS is push pull
  GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 1); // US0_TX (MOSI) is push pull
  GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 1);    // US0_RX (MISO) is input

  // Start with default config, then modify as necessary
  USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
  config.master       = true;            // master mode
  config.baudrate     = 1000000;         // CLK freq is 1 MHz
  config.autoCsEnable = true;            // CS pin controlled by hardware, not firmware
  config.clockMode    = usartClockMode0; // clock idle low, sample on rising/first edge
  config.msbf         = true;            // send MSB first
  config.enable       = usartDisable;    // Make sure to keep USART disabled until it's all set up
  USART_InitSync(USART0, &config);

  // Set USART pin locations
  USART0->ROUTELOC0 = (USART_ROUTELOC0_CLKLOC_LOC0) | // US0_CLK       on location 0 = PE12 per datasheet section 6.4 = EXP Header pin 8
                      (USART_ROUTELOC0_CSLOC_LOC0)  | // US0_CS        on location 0 = PE13 per datasheet section 6.4 = EXP Header pin 10
                      (USART_ROUTELOC0_TXLOC_LOC0)  | // US0_TX (MOSI) on location 0 = PE10 per datasheet section 6.4 = EXP Header pin 4
                      (USART_ROUTELOC0_RXLOC_LOC0);   // US0_RX (MISO) on location 0 = PE11 per datasheet section 6.4 = EXP Header pin 6

  // Enable USART pins
  USART0->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

  // Enable USART0
  USART_Enable(USART0, usartEnable);
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  uint32_t i;

  // Initialize chip
  CHIP_Init();

  // Initialize USART0 as SPI slave
  initUSART0();

  while(1)
  {
    for (i = 0; i < TX_BUFFER_SIZE; i++)
    {
     RxBuffer[i] = USART_SpiTransfer(USART0, TxBuffer[i]); 
    }

    // Place breakpoint here and observe RxBuffer
    // RxBuffer should contain 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9
  } 
}

