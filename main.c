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

extern void sm_init(void);
void sm_run(uint8_t dir, int speed, int steps);

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

  usart_if_t *debugUart = USART_create(1);
#if defined(TTRACE_ON)
    // Set output UART for debug messages
    TTRACE_setOutputUart(debugUart);
#endif


  sm_init();
  sm_run(0, 10, 1000000);

  // Start scheduler
  BLF_schedule();


  while (1) {
    TTRACE_process();

//    config_poll();
  }

  return 0;
}




