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
#include "console.h"
#include "rc_tx.h"
// NEXA home code 01001011101111001000010010 = 0x12EF212

extern void rc_send_nexa(int pin, uint32_t code, uint8_t unit, uint8_t cmd);

static uint8_t console_on = 0;

//-------------------------------------------------------------------------------------------------------
// Rusta SC5262

#define SC5262_SHORT_PULSE_US 430 // A
#define SC5262_LONG_PULSE_US  (SC5262_SHORT_PULSE_US * 3) // B
#define SC5262_SYNC_PULSE_US (SC5262_SHORT_PULSE_US * 31) // C
#define SC5262_REPEAT 10


static char *sc5262_bit(char *p, uint8_t val)
{
  if(val == 0) {
    *p++ = 'A';
    *p++ = 'B';
    *p++ = 'A';
    *p++ = 'B';
  } else if(val == 1) {
    *p++ = 'B';
    *p++ = 'A';
    *p++ = 'B';
    *p++ = 'A';
  } else { // f
    *p++ = 'A';
    *p++ = 'B';
    *p++ = 'B';
    *p++ = 'A';
  }
  return p;
}


static void rusta_enocde_raw(uint8_t group, uint8_t sw, uint8_t cmd, uint8_t repeats)
{
  char raw_cmd[128];
  char *p = raw_cmd;

  p += sprintf(p, "%d,%d,%d,%d:", SC5262_SHORT_PULSE_US, SC5262_LONG_PULSE_US, SC5262_SYNC_PULSE_US, repeats);
  p = sc5262_bit(p,group == 1 ? 0 : 0xf);
  p = sc5262_bit(p,group == 2 ? 0 : 0xf);
  p = sc5262_bit(p,group == 3 ? 0 : 0xf);
  p = sc5262_bit(p,group == 4 ? 0 : 0xf);

  p = sc5262_bit(p,sw == 1 ? 0 : 0xf);
  p = sc5262_bit(p,sw == 2 ? 0 : 0xf);
  p = sc5262_bit(p,sw == 3 ? 0 : 0xf);
  p = sc5262_bit(p,sw == 4 ? 0 : 0xf);

  p = sc5262_bit(p,0xf); // Unused address
  p = sc5262_bit(p,0xf); // Unused data
  p = sc5262_bit(p,0xf); // Command MSB
  p = sc5262_bit(p, cmd ? 0xf : 0); // Command LSB
  *p++ = 'A';
  *p++ = 'C';
  *p++ = 0;
  TTRACE(TTRACE_INFO, "RAW: %s\n", raw_cmd);
  rc_tx_raw(raw_cmd, strlen(raw_cmd));
}



//-------------------------------------------------------------------------------------------------------
// Nexa / Anslut / Proove

#if 0
#define ANSLUT_PULSE_HIGH 250
#define ANSLUT_PULSE_ONE_LOW 250
#define ANSLUT_PULSE_ZERO_LOW 1450 //1250
#define ANSLUT_PULSE_SYNC_LOW 2750 //2500
#define ANSLUT_PULSE_PAUSE_LOW 10750

#define ANSLUT_REPEAT 6
#define NEXA_REPEAT 5


static uint16_t *anslut_bit(uint16_t *p, uint8_t val)
{
  if(val != 0) {
    *p++ = ANSLUT_PULSE_HIGH;      // 0
    *p++ = ANSLUT_PULSE_ZERO_LOW;
    *p++ = ANSLUT_PULSE_HIGH;      // 1
    *p++ = ANSLUT_PULSE_ONE_LOW;
  } else {
    *p++ = ANSLUT_PULSE_HIGH;      // 1
    *p++ = ANSLUT_PULSE_ONE_LOW;
    *p++ = ANSLUT_PULSE_HIGH;      // 0
    *p++ = ANSLUT_PULSE_ZERO_LOW;
  }
  return p;
}

static void rc_send_anslut_proove_nexa(int pin, uint32_t code, uint8_t unit, uint8_t cmd, uint8_t channel_dev, uint8_t repeat)
{
  uint16_t *p = tx_time_table;
  *p++ = ANSLUT_PULSE_HIGH;
  *p++ = ANSLUT_PULSE_SYNC_LOW; // SYNC

  uint16_t n;
  for(n = 0; n < 26; n++) { // TxCode 26 bits
    uint8_t b = (code & ((uint32_t)1 << 25)) != 0;
    //uint8_t b = (code & 0x80000000) != 0;
    code <<=1;
    p = anslut_bit(p, b);
  }

  p = anslut_bit(p, 0); // Group
  if(cmd) {
    //p = anslut_bit(p, 1); // Group
    p = anslut_bit(p, 1); // Command on
  } else {
    //p = anslut_bit(p, 0); // Group
    p = anslut_bit(p, 0); // Command of
  }
  p = anslut_bit(p, (channel_dev >> 1) & 1); // Channel
  p = anslut_bit(p, (channel_dev >> 0) & 1); // Channel

  p = anslut_bit(p, (unit >> 1) & 1); // Dev/Unit
  p = anslut_bit(p, (unit >> 0) & 1); // Dev/Unit
  *p++ = ANSLUT_PULSE_HIGH;
  *p++ = ANSLUT_PULSE_PAUSE_LOW;

  tx_time_table_length = p - tx_time_table;
  //send_common(pin, repeat, (p - tx_time_table));
}
#endif


void ttrace_key_cb(char data) {
    if(console_on) {
      console_insert(data);
      return;
    }

    if(data == '1') {
//    rc_send_nexa(0, 0x12EF212, 0, lampState);

    } else if(data == 'h') {
      TTRACE(TTRACE_INFO, "RFBridge rev: %s\nCommands:\n", FWREV_getString());
      TTRACE(TTRACE_INFO, "h : Help menu\n");

    } else if(data == 'e') {
      rusta_enocde_raw(2, 2, 1,3);
    } else if(data == 'f') {
      rusta_enocde_raw(2, 2, 0,3);
    } else if(data == 'c') {
      console_on = 1;
    } else {
    }


  }





  void testcmd_init(void) {
  }
