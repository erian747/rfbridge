#define MODULE_NAME BSP
#include <stdint.h>
#include "bsp.h"
#include "mcal.h"
#include "blf.h"
#include "trace.h"


// Newlib assert stub
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    (void)file;
    (void)line;
    (void)func;
    (void)failedexpr;
    volatile int loop = 1;
    __asm volatile ("bkpt 0");
    do { ; } while( loop );
}

void MCAL_assertFailure(void) {
  MCAL_LOCK_IRQ();
  while(1);
}



void BSP_init(void)
{

  // Common GPIO config
  const GPIO_config_t gpioCfg[] =
	{
    {BSP_DBG_UART_TX,  .mode = GPIO_MODE_AF, 	                               .param = 0x0}, 	// USART2_TX (Debug)
    {BSP_DBG_UART_RX,  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP,            .param = 0x0}, 	// USART2_RX (Debug)
    {BSP_SLIP_UART_TX, .mode = GPIO_MODE_AF, 	                               .param = 0x0}, 	// USART1_TX
    {BSP_SLIP_UART_RX, .mode = GPIO_MODE_INPUT | GPIO_INPUT_PULL_UP,         .param = 0x0}, 	// USART1_RX
    {BSP_433MHZ_RX,    .mode = GPIO_MODE_INPUT,                              .param = 0x0},
    {BSP_433MHZ_TX,    .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_PP,            .param = 0x0},

    // LED:s
    {BSP_BLUE_PILL_LED,    .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_PP,        .param = 1},
    {BSP_LED_RX,           .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_OD,        .param = 1},
    {BSP_LED_TX,           .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_OD,        .param = 1},
    {BSP_LED_KEEPALIVE,    .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_OD,        .param = 1},

  };
  GPIO_configPins(gpioCfg, sizeof(gpioCfg) / sizeof(gpioCfg[0]));

  // Startup delay to allow devices to get fully powered
  volatile uint32_t n;
  for(n = 0; n < ((uint32_t)1<<16); n++);

}
