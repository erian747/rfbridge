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
#include "usb_serial.h"

static void usb_serial_rx_cb(void *ctx, uint8_t *data, size_t len)
{
  while(len--) {
    console_insert(*data++);
  }
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

  usb_serial_init();

  console_init();

  usb_serial_set_rx_cb(usb_serial_rx_cb, NULL);
  // Start scheduler
  BLF_schedule();


  while (1) {
    TTRACE_process();

    rc_rx_poll();
    rc_tx_poll();
  }

  return 0;
}




