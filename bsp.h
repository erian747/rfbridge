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







// PA

#define BSP_DBG_UART_TX           GPIO_MAKEPIN(0,2)
#define BSP_DBG_UART_RX           GPIO_MAKEPIN(0,3)

#define BSP_315MHZ_RX             GPIO_MAKEPIN(0,8) // Active High
#define BSP_SLIP_UART_TX          GPIO_MAKEPIN(0,9)  // On=Low
#define BSP_SLIP_UART_RX          GPIO_MAKEPIN(0,10) // On=Low


// PB

#define BSP_LED_RX                GPIO_MAKEPIN(1,5)  // Active low
#define BSP_LED_TX                GPIO_MAKEPIN(1,6)  // Active low
#define BSP_LED_KEEPALIVE         GPIO_MAKEPIN(1,7)  // Active low
#define BSP_433MHZ_TX             GPIO_MAKEPIN(1,11)

#define BSP_433MHZ_RX             GPIO_MAKEPIN(1,15) // TIM1_CH3N, AF: 2

// PC
#define BSP_BLUE_PILL_LED         GPIO_MAKEPIN(2,13)


void BSP_init(void);




#ifdef __cplusplus
}
#endif

#endif
