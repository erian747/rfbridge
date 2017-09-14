#define MODULE_NAME MAIN

#include "hd44780.h"
#include <stdio.h>
#include <stdlib.h>
#include "mcal.h"
#include "trace.h"
#include "blf.h"
#include "network.h"
#include "bsp.h"
#include "fwrev.h"
#include "tsl2561.h"
#include "config.h"
#include "crashdump.h"
#include "usb/hid_keyboard.h"
#include "sm.h"
#include "ws2812b.h"
#include "solc.h"
#include "button.h"
extern void suncatcher_init(void);



void HardFault_Handler(void)
{
  while(1);
}

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
  WDG_init(2000);
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
  //ws2812b_init(rgb_data, sizeof(rgb_data));
  // Start watchdog

#ifndef DEBUG
  WDG_start();
#endif


  hd44780_init();

	//ANALOG_init();
	BUTTON_init(1);

  solc_t *solc = solc_init();

  //hid_keyboard_t *kbd = hid_keyboard_create();

  usart_if_t *debugUart = USART_create(1);
#if defined(TTRACE_ON)
    // Set output UART for debug messages
    TTRACE_setOutputUart(debugUart);
#endif

  //sm_init();
  //suncatcher_init();


  //sm_run(0, 10, 10000000);
  // Start scheduler
  BLF_schedule();


  while (1) {
    TTRACE_process();
    solc_bg(solc);
  }

  return 0;
}




