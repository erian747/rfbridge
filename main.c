#define MODULE_NAME MAIN

#include <stdio.h>
#include <stdlib.h>
#include "mcal.h"
#include "trace.h"
#include "blf.h"
#include "bsp.h"
#include "fwrev.h"
#include "console.h"
#include "config.h"
#include "crashdump.h"
#include "rc_tx.h"
#include "rc_rx.h"


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

  usart_if_t *debugUart = USART_create(0);
#if defined(TTRACE_ON)
    // Set output UART for debug messages
    TTRACE_setOutputUart(debugUart);
#endif


  rc_tx_init();
  rc_rx_init();

  extern void usb_serial_init(void);
  usb_serial_init();

  console_init();
  // Start scheduler
  BLF_schedule();


  while (1) {
    TTRACE_process();

//    config_poll();
    rc_rx_poll();
  }

  return 0;
}




