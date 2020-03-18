#define MODULE_NAME TESTCMD
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "blf.h"
#include "trace.h"
#include "analog.h"
#include "bsp.h"
#include "mcal.h"
#include "config.h"
#include "fwrev.h"
// NEXA home code 01001011101111001000010010 = 0x12EF212

extern void rc_send_nexa(int pin, uint32_t code, uint8_t unit, uint8_t cmd);

static uint8_t lampState = 0;

extern void mqtt_link_publish(void);

void ttrace_key_cb(char data)
{
  // Control lamp
  if(data == '1')
  {
    lampState = !lampState;
    TTRACE(TTRACE_INFO, "Lamp set to: %s\n", lampState ? "On" : "Off");
    rc_send_nexa(0, 0x12EF212, 0, lampState);

  }
  else if(data == 'h')
  {
    TTRACE(TTRACE_INFO, "GSL node rev: %s\nCommands:\n", FWREV_getString());
    TTRACE(TTRACE_INFO, "1 : Toggle lamp output\n");
    TTRACE(TTRACE_INFO, "h : Help menu\n");

  }
  else if(data == 'p')
  {
    //mqtt_link_publish();
  }
  else
  {
  }


}





void testcmd_init(void)
{
}
