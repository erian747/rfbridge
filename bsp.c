#define MODULE_NAME BSP
#include <stdint.h>
#include "bsp.h"
#include "mcal.h"
#include "bican_node.h"
#include "blf.h"
#include "trace.h"
#include "analog.h"
#include "button.h"


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
  {BSP_LED_KEEPALIVE, 1},
  {BSP_LED_RX, 1},
  {BSP_LED_TX, 1},
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





/*
// Channels to convert
static const gpio_t adcChannels[] =
{
  //[BSP_ADC_CH_NTC_BOARD] = BSP_ADC_NTC_BOARD,
  //[BSP_ADC_CH_SUPPLY_VOLTAGE] = BSP_ADC_SUPPLY_10V,
  [BSP_ADC_CH_CURR1] = BSP_ADC_CURR1,
  [BSP_ADC_CH_CURR2] = BSP_ADC_CURR2,
  [BSP_ADC_CH_CURR3] = BSP_ADC_CURR3,
  [BSP_ADC_CH_DIM3] = BSP_ADC_DIM3,
  [BSP_ADC_CH_DIM2] = BSP_ADC_DIM2,
  [BSP_ADC_CH_DIM1] = BSP_ADC_DIM1,
};
*/

void BSP_init(void)
{

  //RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;


  // Common GPIO config
  const GPIO_config_t gpioCfg[] =
	{
/*
#ifdef DALI_TEST
    {BSP_UART1_TX,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	             .param = 0x1}, 	// USART1_TX (Debug)
    {BSP_UART1_RX,  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP,           .param = 0x1}, 	// USART1_RX (Debug)
    {BSP_DALI_RX,   .mode = GPIO_MODE_INPUT | GPIO_INPUT_PULL_UP,        .param = 0x0},
    {BSP_DALI_TX,  .mode = GPIO_MODE_OUTPUT, 	                           .param = 0x0},
#else

 //   {BSP_ADC_SUPPLY_10V,      .mode = GPIO_MODE_ANALOG,                      .param = 0},
   // {BSP_ADC_NTC_BOARD,       .mode = GPIO_MODE_ANALOG,                      .param = 0},
    {BSP_ADC_DIM1,            .mode = GPIO_MODE_ANALOG,                      .param = 0},
    {BSP_ADC_DIM2,            .mode = GPIO_MODE_ANALOG,                      .param = 0},
    {BSP_ADC_DIM3,            .mode = GPIO_MODE_ANALOG,                      .param = 0},
    {BSP_ADC_CURR1,           .mode = GPIO_MODE_ANALOG,                      .param = 0},
    {BSP_ADC_CURR2,           .mode = GPIO_MODE_ANALOG,                      .param = 0},
    {BSP_ADC_CURR3,           .mode = GPIO_MODE_ANALOG,                      .param = 0},
*/
    // USER uart
    {BSP_DBG_UART_TX,  .mode = GPIO_MODE_AF, 	                              .param = 0x0}, 	// USART2_TX (Debug)
    {BSP_DBG_UART_RX,  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP,           .param = 0x0}, 	// USART2_RX (Debug)
    {BSP_SLIP_UART_TX,  .mode = GPIO_MODE_AF, 	                             .param = 0x0}, 	// USART1_TX
    {BSP_SLIP_UART_RX,  .mode = GPIO_MODE_INPUT | GPIO_INPUT_PULL_UP,        .param = 0x0}, 	// USART1_RX
    {BSP_433MHZ_RX,   .mode = GPIO_MODE_INPUT,                                .param = 0x0},

   // {BSP_RC_TX_PIN,        .mode = GPIO_MODE_OUTPUT,                       .param = 0},


    // SPI2
    //{BSP_SPI2_CLK,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	                 .param = 0x5},
    //{BSP_SPI2_MISO,  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP,              .param = 0x5},
    //{BSP_SPI2_MOSI,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	               .param = 0x5},

     // SPI3
    //{BSP_SPI3_CLK,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	                 .param = 0x6},
    //{BSP_SPI3_MISO,  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP,              .param = 0x6},
    //{BSP_SPI3_MOSI,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	               .param = 0x6},
/*
    // DIM PWM
    {BSP_DIM_PWM1,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	                 .param = 0x2},
    {BSP_DIM_PWM2,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	                 .param = 0x2},
    {BSP_DIM_PWM3,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	                 .param = 0x2},

    {BSP_CURR_PWM1,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	               .param = 0x1},
    {BSP_CURR_PWM2,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	               .param = 0x1},
    {BSP_CURR_PWM3,  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	               .param = 0x1},

    // Sync
    {BSP_SYNC_ENABLE,  .mode = GPIO_MODE_OUTPUT, 	                           .param = 0x01},
    {BSP_SYNC_DRIVE,   .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP, 	             .param = 0x2},
    {BSP_SYNC_SENSE,   .mode = GPIO_MODE_AF | GPIO_MODE_INPUT,               .param = 0x4},
*/
    // Overlay / Status relay
    {BSP_LED_RX,           .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_OD,           .param = 1},
    {BSP_LED_TX,           .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_OD,           .param = 1},
    {BSP_LED_KEEPALIVE,   .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_OD,            .param = 1},
   // {BSP_DISCO_LED_ORANGE,     .mode = GPIO_MODE_OUTPUT,                      .param = 0},

  };
  GPIO_configPins(gpioCfg, sizeof(gpioCfg) / sizeof(gpioCfg[0]));

  // Startup delay to allow devices to get fully powered
  volatile uint32_t n;
  for(n = 0; n < ((uint32_t)1<<16); n++);

  //BUTTON_init(2);

}
