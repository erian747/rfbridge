#define MODULE_NAME BSP
#include <stdint.h>
#include "bsp.h"
#include "mcal.h"
#include "bican_node.h"
#include "blf.h"
#include "trace.h"
#include "analog.h"
#include "iir_filter.h"
#include "button.h"
#include "stm32f0xx.h"

#define SUPPLY_VOLTAGE_UVLO    8000 // mv
#define SUPPLY_VOLTAGE_PGOODL  8200 // mv
#define SUPPLY_VOLTAGE_PGOODH  13500 // mv
#define SUPPLY_VOLTAGE_OVP    14000 // mv

#define BOARD_TEMP_NORMAL        750 // 0.1C
#define BOARD_TEMP_MAX           990 // 0.1C

typedef struct
{
  gpio_t pin;
  uint8_t polarity;
} bsp_led_t;

static const bsp_led_t leds[] =
{
  {BSP_FAIL_LED, 1},
};

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


void BSP_led_control(gpio_t led, uint8_t newState)
{
  uint32_t n;
  for(n = 0; n < (sizeof(leds) / sizeof(leds[0])); n++)
  {
    if(leds[n].pin == led)
    {
      GPIO_write(led, leds[n].polarity == (newState != 0));
    }
  }
}




void BSP_init(void)
{

  //RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;


  // Common GPIO config
  const GPIO_config_t gpioCfg[] =
	{

    // USER uart
    {BSP_DBG_UART_TX,  .mode = GPIO_MODE_AF, 	                              .param = 0x0}, 	// USART2_TX (Debug)
    {BSP_DBG_UART_RX,  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP,           .param = 0x0}, 	// USART2_RX (Debug)
    {BSP_SLIP_UART_TX,  .mode = GPIO_MODE_AF, 	                             .param = 0x0}, 	// USART1_TX
    {BSP_SLIP_UART_RX,  .mode = GPIO_MODE_INPUT | GPIO_INPUT_PULL_UP,        .param = 0x0}, 	// USART1_RX


    {BSP_SM_A1,     .mode = GPIO_MODE_OUTPUT, 	                           .param = 0x00},
    {BSP_SM_A2,     .mode = GPIO_MODE_OUTPUT, 	                           .param = 0x00},
    {BSP_SM_B1,     .mode = GPIO_MODE_OUTPUT, 	                           .param = 0x00},
    {BSP_SM_B2,     .mode = GPIO_MODE_OUTPUT, 	                           .param = 0x00},
    // I2C
    {BSP_I2C1_SCL,           .mode = GPIO_MODE_AF | GPIO_OUTPUT_OD,         .param = 1},
    {BSP_I2C1_SCL,           .mode = GPIO_MODE_AF | GPIO_OUTPUT_OD,         .param = 1},
    // Led
    {BSP_FAIL_LED,   .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_PP,             .param = 1},

  };
  GPIO_configPins(gpioCfg, sizeof(gpioCfg) / sizeof(gpioCfg[0]));

  // Startup delay to allow devices to get fully powered
  volatile uint32_t n;
  for(n = 0; n < ((uint32_t)1<<16); n++);

  //BUTTON_init(2);

}
