#ifndef TRACE_CONFIG_H
#define TRACE_CONFIG_H

/* UART, CALLBACK or ITM */
#define TTRACE_USE_UART

/* Application */
#define TRACE_ENABLE_BSP              TTRACE_INFO
#define TRACE_ENABLE_MAIN             TTRACE_INFO
#define TRACE_ENABLE_TESTCMD          TTRACE_INFO
#define TRACE_ENABLE_TSL2561          TTRACE_DEBUG

/* Drivers */
#define TRACE_ENABLE_CONFIG        TTRACE_INFO
/* MCU Driver */
#define TRACE_ENABLE_MCAL_RCC     TTRACE_WARN
#endif
