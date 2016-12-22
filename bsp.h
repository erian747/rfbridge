#ifndef BSP_H
#define BSP_H
#include <stdint.h>
#include "mcal_gpio.h"
#ifdef __cplusplus
extern "C" {
#endif

// HW Timers

#define BSP_RC_TIMER        2
#define BSP_MODBUS_TIMER       3 // TRGO connected to ADC trigger



#define BSP_ADC_REF_VOLTAGE 3300

// ADC channels
enum
{
  BSP_ADC_CH_NTC_BOARD,
  BSP_ADC_CH_SUPPLY_VOLTAGE,
  BSP_ADC_CH_DIM2,
  BSP_ADC_CH_CURR1,
  BSP_ADC_CH_CURR2,
  BSP_ADC_CH_CURR3,
  BSP_ADC_CH_DIM3,
  BSP_ADC_CH_DIM1,
};



// PA
#define BSP_USER_BTN              GPIO_MAKEPIN(0,0)
#define BSP_RC_RX_PIN             GPIO_MAKEPIN(0,1)
#define BSP_DBG_UART_TX           GPIO_MAKEPIN(0,2)
#define BSP_DBG_UART_RX           GPIO_MAKEPIN(0,3)
#define BSP_ADC_CURR2             GPIO_MAKEPIN(0,4)
#define BSP_ADC_CURR3             GPIO_MAKEPIN(0,5)
#define BSP_ADC_DIM3              GPIO_MAKEPIN(0,6)
#if !SYNC_USE_TIMER_CAPTURE
  #define BSP_ADC_DIM2              GPIO_MAKEPIN(0,7)
#else
  #define BSP_SYNC_SENSE            GPIO_MAKEPIN(0,7) // TIM17_CH1, AF: 0 TIM14_CH1, AF: 4
#endif
#define BSP_315MHZ_RX             GPIO_MAKEPIN(0,8) // Active High
#define BSP_SLIP_UART_TX          GPIO_MAKEPIN(0,9)  // On=Low
#define BSP_SLIP_UART_RX          GPIO_MAKEPIN(0,10) // On=Low
#define BSP_DIP_SW3_0             GPIO_MAKEPIN(0,11) // On=Low
#define BSP_DIP_SW3_1             GPIO_MAKEPIN(0,12) // On=Low
#define BSP_PUSH_DIM_SENSE        GPIO_MAKEPIN(0,15) // On=Low

// PB
#define BSP_ADC_DIM1              GPIO_MAKEPIN(1,0)
#define BSP_CURR_PWM3             GPIO_MAKEPIN(1,1) // TIM3_CH4, AF: 1
#define BSP_CURR_PWM1             GPIO_MAKEPIN(1,4) // TIM3_CH1, AF: 1
#define BSP_LED_RX                GPIO_MAKEPIN(1,5)  // Active low
#define BSP_LED_TX                GPIO_MAKEPIN(1,6)  // Active low
#define BSP_LED_KEEPALIVE         GPIO_MAKEPIN(1,7)  // Active low

#define BSP_SYNC_DRIVE            GPIO_MAKEPIN(1,8) // TIM16_CH1, AF1
#define BSP_SYNC_ENABLE           GPIO_MAKEPIN(1,9)
#define BSP_DIP_SW4_0             GPIO_MAKEPIN(1,10) // On=Low
#define BSP_DIP_SW4_1             GPIO_MAKEPIN(1,11) // On=Low
#define BSP_DIP_SW4_2             GPIO_MAKEPIN(1,12) // On=Low
#define BSP_DIM_PWM1              GPIO_MAKEPIN(1,13) // TIM1_CH1N, AF: 2
#define BSP_DIM_PWM2              GPIO_MAKEPIN(1,14) // TIM1_CH2N, AF: 2
#define BSP_DIM_PWM3              GPIO_MAKEPIN(1,15) // TIM1_CH3N, AF: 2

// PC
#define BSP_RC_TX_PIN             GPIO_MAKEPIN(2,3)

#define BSP_SPI3_CLK              GPIO_MAKEPIN(2,10) // AF:6
#define BSP_SPI3_MISO             GPIO_MAKEPIN(2,11) // AF:6
#define BSP_SPI3_MOSI             GPIO_MAKEPIN(2,12) // AF:6
#define BSP_FAIL_LED              GPIO_MAKEPIN(2,13) // Active High
#define BSP_DIP_SW1_0             GPIO_MAKEPIN(2,14) // On=Low
#define BSP_DIP_SW1_1             GPIO_MAKEPIN(2,15) // On=Low



void BSP_init(void);


// Interfaces
enum
{
  BSP_LED_GREEN    = 0x01,
  BSP_LED_BLUE     = 0x02,
  BSP_LED_ORANGE   = 0x04,
  BSP_LED_RED      = 0x08,
 // BSP_LED_GREEN   = 0x10,
};


#ifdef __cplusplus
}
#endif

#endif
