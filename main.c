#define MODULE_NAME MAIN

#include <stdio.h>
#include <stdlib.h>
#include "mcal.h"
#include "trace.h"
#include "blf.h"
#include "network.h"
#include "bsp.h"
#include "fwrev.h"

#include "config.h"
#include "crashdump.h"
#include "usb/hid_keyboard.h"

extern void mqtt_link_init(void);
extern void rc_timer_init(void);
extern void rc_poll(void);
/**
  * @brief Program entry point
  * @param None
  * @retval None
  */
#if defined(__arm__)
int main(void)
#else
int target_main(void)
#endif
{
  //CRASHDUMP_init();
  //WDG_init(2000);
  // Init MCU
  MCAL_init();
  // Init framework
  BLF_init();
  // Init Text Trace
#if defined(TTRACE_ON) && defined(__arm__)
  TTRACE_init();
#endif


  // Init configuration

 // config_init();



  BSP_init();
  // Start watchdog
/*
#ifndef DEBUG
  WDG_start();
#endif
*/

  //hid_keyboard_t *kbd = hid_keyboard_create();

  usart_if_t *debugUart = USART_create(0);
#if defined(TTRACE_ON)
    // Set output UART for debug messages
    TTRACE_setOutputUart(debugUart);
#endif


  rc_timer_init();

  //network_init();

  //mqtt_link_init();
  // Start scheduler
  BLF_schedule();


  while (1) {
    TTRACE_process();

//    config_poll();
    rc_poll();
  }

  return 0;
}




