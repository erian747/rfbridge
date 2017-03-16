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
#define BSP_DBG_UART_TX           GPIO_MAKEPIN(0,2)
#define BSP_DBG_UART_RX           GPIO_MAKEPIN(0,3)

#define BSP_SLIP_UART_TX          GPIO_MAKEPIN(0,9)  // On=Low
#define BSP_SLIP_UART_RX          GPIO_MAKEPIN(0,10) // On=Low


// PB

#define BSP_I2C1_SCL              GPIO_MAKEPIN(1,6)  // Active low
#define BSP_I2C1_SDA              GPIO_MAKEPIN(1,7)  // Active low

#define BSP_SM_A1                 GPIO_MAKEPIN(1,12) // On=Low
#define BSP_SM_A2                 GPIO_MAKEPIN(1,13) // TIM1_CH1N, AF: 2
#define BSP_SM_B1                 GPIO_MAKEPIN(1,14) // TIM1_CH2N, AF: 2
#define BSP_SM_B2                 GPIO_MAKEPIN(1,15) // TIM1_CH3N, AF: 2

// PC
#define BSP_FAIL_LED              GPIO_MAKEPIN(2,13) // Active High




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
