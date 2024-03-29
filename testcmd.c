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

// NEXA code 01001011101111001000010010 = 0x12EF212
#define NEXA_TEST_CODE 0x12EF212

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
  rc_tx_raw(raw_cmd);
}



//-------------------------------------------------------------------------------------------------------
// Nexa / Anslut / Proove


//#define ANSLUT_PULSE_HIGH 250
//#define ANSLUT_PULSE_ONE_LOW 250
#define ANSLUT_PULSE_SHORT 250 // A
#define ANSLUT_PULSE_ZERO_LOW 1450 // B
#define ANSLUT_PULSE_SYNC_LOW 2750 //C
#define ANSLUT_PULSE_PAUSE_LOW 10750 // D

#define ANSLUT_REPEAT 6
#define NEXA_REPEAT 5


static char *anslut_bit(char *p, uint8_t val)
{
  if(val != 0) {
    *p++ = 'A';      // 0
    *p++ = 'B';
    *p++ = 'A';      // 1
    *p++ = 'A';
  } else {
    *p++ = 'A';      // 1
    *p++ = 'A';
    *p++ = 'A';      // 0
    *p++ = 'B';
  }
  return p;
}

static void rc_send_anslut_proove_nexa(uint32_t code, uint8_t unit, uint8_t cmd, uint8_t channel_dev, uint8_t repeat)
{
  char raw_cmd[256];
  char *p = raw_cmd;

  p += sprintf(p, "%d,%d,%d,%d,%d:", ANSLUT_PULSE_SHORT, ANSLUT_PULSE_ZERO_LOW, ANSLUT_PULSE_SYNC_LOW, ANSLUT_PULSE_PAUSE_LOW, repeat);

  *p++ = 'A';
  *p++ = 'C'; // SYNC

  uint16_t n;
  for(n = 0; n < 26; n++) { // TxCode 26 bits
    uint8_t b = (code & ((uint32_t)1 << 25)) != 0;
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
  *p++ = 'A';
  *p++ = 'D';
  *p++ = 0;
  TTRACE(TTRACE_INFO, "RAW: %s\n", raw_cmd);
  rc_tx_raw(raw_cmd);
}


void ttrace_key_cb(char data)
{
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
    //rusta_enocde_raw(2, 2, 1,3);
    rc_send_anslut_proove_nexa(NEXA_TEST_CODE, 0, 1, 0, NEXA_REPEAT);
  } else if(data == 'f') {
    rc_send_anslut_proove_nexa(NEXA_TEST_CODE, 0, 0, 0, NEXA_REPEAT);
    //rusta_enocde_raw(2, 2, 0,3);
  } else if(data == 'c') {
    console_on = 1;
  } else {
  }


}





void testcmd_init(void)
{
}
