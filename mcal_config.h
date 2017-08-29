
// Configuration for MCAL version
#define MCAL_CONFIG_REV_HIGH  3
#define MCAL_CONFIG_REV_MID   0
#define MCAL_CONFIG_REV_LOW   0

#define MCAL_USE_FPU         0
#define MCAL_USE_HSE_CRYSTAL 1
#define MCAL_USE_LSE_CRYSTAL 1

#define MCU_SYSFREQ 24000000

#define MCAL_TIMER_USAGE_MASK 0x0000f
#define MCAL_USART_USAGE_MASK 0x03
#define MCAL_SPI_USAGE_MASK   2
#define MCAL_I2C_USAGE_MASK   1
#define MCAL_ADC_USAGE_MASK   0
#define MCAL_CAN_USAGE_MASK   0
#define MCAL_USB_USAGE_MASK   0

#define MCAL_EXTI_USAGE			  0
#define MCAL_RTC_USAGE        1
#define MCAL_NVM_USAGE        0

#define MCAL_USE_ASSERT 			1

#define MCAL_USE_TRACE				1

//------------------------------------------------------------------------
// GPIO options
#define MCAL_GPIO_INTERNAL_PORTS 5

//------------------------------------------------------------------------
// UART options
#define  MCAL_USART_RX_IRQ_PRIO 1

//------------------------------------------------------------------------
// Timer options
#define MCAL_TIMER_IRQ_PRIO 0


//------------------------------------------------------------------------
// SPI options
#define MCAL_SPI_IRQ_PRIO 1
#define MCAL_SPI_MASTER_TIMER 2

//------------------------------------------------------------------------
// I2C options
#define MCAL_I2C_MASTER_TIMER 1
#define MCAL_I2C_IRQ_PRIO  1

//------------------------------------------------------------------------
// RTC options
#define MCAL_RTC_USE_DATE  0

//------------------------------------------------------------------------
// ADC options

#define MCAL_ADC_IRQ_PRIO 2
// ADC reference voltage in mV
#define MCAL_ADC_REF_VOLTAGE 3300



//------------------------------------------------------------------------
// Traceing



#include "trace.h"

#define MCAL_TRACE TTRACE

#define MCAL_TRACE_INFO   TTRACE_INFO
#define MCAL_TRACE_WARN   TTRACE_WARN
#define MCAL_TRACE_ERROR  TTRACE_ERROR

