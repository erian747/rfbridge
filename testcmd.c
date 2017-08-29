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
#include "sm.h"


void ttrace_key_cb(char data)
{
  // Control lamp
  if(data == '1')
  {
  }
  else if(data == 'h')
  {
    TTRACE(TTRACE_INFO, "Suncatcher rev: %s\nCommands:\n", FWREV_getString());
    TTRACE(TTRACE_INFO, "s : Run motor for some steps\n");
    TTRACE(TTRACE_INFO, "h : Help menu\n");

  }
  else if(data == 'p') {
  }
  else if(data == 's') {
  //  sm_run(0, 100, 2);
  }
  else
  {
  }


}





void testcmd_init(void)
{
}
