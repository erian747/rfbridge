#define MODULE_NAME RC_RX
#include <string.h>
#include <stdio.h>
#include "bsp.h"
#include "mcal.h"
#include "trace.h"


#include "cbuf.h"
//#define RC_RX_TEST 0



#define RX_POLL_PERIOD_US 25

#define TIME_TABLE_SIZE 256


#ifndef RC_RX_TEST
static tim_t *rcTim=0;
#endif




static volatile uint8_t time_table_length;
static volatile uint8_t time_table_idx;
static volatile uint8_t rcTxPin;

static volatile uint8_t rcRepeat;

static volatile uint8_t rcState;
static volatile uint8_t rcTxPinState = 0;


#define rx_fifo_SIZE 256
volatile struct {
  uint16_t     m_getIdx;
  uint16_t     m_putIdx;
  uint16_t     m_entry[rx_fifo_SIZE];
} rx_fifo;




#define RX_IDLE_TIME (50000 / RX_POLL_PERIOD_US)


static uint8_t prev_rx_pin_state = 0;
static uint16_t rx_pin_stable_count = RX_IDLE_TIME; // Start in idle state



#if RC_RX_TEST
static uint16_t test_idx = 0;
static int test_period_counter = 0;
static uint8_t test_pin_state = 0;


static void test_init(void)
{
  test_period_counter = tx_time_table[0];
  test_idx = 1;
  test_pin_state = 1;
}

static uint8_t simulated_rc_pin(void)
{
  if(test_period_counter <= 0) {
    if(test_idx < tx_time_table_length) {
      test_pin_state = !test_pin_state; // Toggle pin
      test_period_counter = tx_time_table[test_idx];
      test_idx++; // Advance to next period
    } else {
      test_pin_state = 0; // Idle low
    }
  } else {
    test_period_counter-=RX_POLL_PERIOD_US;
  }
  return test_pin_state;
}

#define rx_rc_pin_get() simulated_rc_pin()
#else
#define rx_rc_pin_get() GPIO_read(BSP_433MHZ_RX) // GPIO_read(BSP_433MHZ_RX)
#endif

static uint32_t repeat_supress_tmr = 0;
static uint8_t rx_found_valid = 0;


#define DIG_FILTER_MAX 6

#define DIG_FILTER_VALUE_LOW 2
#define DIG_FILTER_VALUE_HIGH 4

static uint8_t dig_filter = 0;

static void timer_cb(void *ctx)
{
  if(repeat_supress_tmr != 0) {
    repeat_supress_tmr--;
  }
  uint8_t din = rx_rc_pin_get();

  // If rx pin have not changed state
  if(prev_rx_pin_state == din) {

    // dont roll over
    if(rx_pin_stable_count < RX_IDLE_TIME) {// If not in idle state

      rx_pin_stable_count++;
/*
      if(rx_pin_stable_count >= RX_IDLE_TIME) {// No activity for 50ms, become idle

        if(!CBUF_IsFull(rx_fifo)) {
          CBUF_Push(rx_fifo, 0); // Signal reset state machine
          rx_found_valid = 0;
          TTRACE(TTRACE_WARN, "rc_timer_cb: Return to idle\n");
        } else {
          TTRACE(TTRACE_WARN, "rc_timer_cb: FIFO full\n");
        }
      }
*/
    }

  } else { // Changed state

    if(rx_pin_stable_count > 4) {
      uint32_t us = 0;
      if(rx_pin_stable_count < RX_IDLE_TIME) {
        us = rx_pin_stable_count * RX_POLL_PERIOD_US;
      }
      CBUF_Push(rx_fifo, us);
      prev_rx_pin_state = din;  // Update prev pin state
      rx_pin_stable_count = 0; // Reset stable counter
    }
  }

}


#define ANSLUT_PULSE_HIGH 250
#define ANSLUT_PULSE_ONE_LOW 250
#define ANSLUT_PULSE_ZERO_LOW 1450 //1250
#define ANSLUT_PULSE_SYNC_LOW 2750 //2500
#define ANSLUT_PULSE_PAUSE_LOW 10750

#define nexa_is_short_pulse(_usec) (_usec >= (ANSLUT_PULSE_HIGH-150) && _usec <= (ANSLUT_PULSE_HIGH+125))
#define nexa_is_long_pulse(_usec) (_usec > (ANSLUT_PULSE_ZERO_LOW-350) && _usec < (ANSLUT_PULSE_ZERO_LOW+350))

#define bm_add_bit(bm, b)  bm = (bm << 1) | ((b) != 0)
#define bm_get_bit(bm, b)  ((bm & (1 << b)) != 0)


typedef struct
{
  uint8_t state;
  uint16_t hl;
  uint16_t ll;
  uint32_t data;
  uint32_t correct_decodes;
  uint16_t sync_len;
} ev1527_t;



static int ev1527_decode(ev1527_t *self, uint16_t pw, char *s, int sl, uint32_t min_sync_len, uint32_t short_long_delimiter)
{
  int res = 0;
  if(pw <  100 || pw > 30000) {
    if(self->state != 0) {
      TTRACE(TTRACE_INFO, "EV1527: Wrong pulse length %dus, resetting to idle state from state %d\n", pw, self->state);
    }
    self->state = 0;
    GPIO_write(BSP_LED_RX, 1);
  } else if(pw > min_sync_len && pw < (min_sync_len+3000)) {

    self->sync_len = pw;
    self->data = 0;
    self->state = 2;
    self->hl = self->ll = 0;
  } else if(self->state >= 2) {
    if(pw > short_long_delimiter*2) {
      if(self->state > 2) {
        TTRACE(TTRACE_INFO, "EV1527: To long data pulse %dus in state %d\n", pw, self->state);
      }
      self->state = 0;
      GPIO_write(BSP_LED_RX, 1);
    } else {
      if(self->state == 2) {
        TTRACE(TTRACE_INFO, "EV1527: Sync + first pulse received, sync pw=%d\n", self->sync_len);
      }
      if(self->state & 1) {
        bm_add_bit(self->data, pw > short_long_delimiter);
      }
      if(pw > short_long_delimiter) {
          self->hl += pw;
      } else {
          self->ll += pw;
      }

      self->state++;

      // Light up RX led if at least some bits are received
      if(self->state == 24) {
        GPIO_write(BSP_LED_RX, 0);
      }
      else if(self->state >= (24*2)+2) {
        self->correct_decodes++;
        TTRACE(TTRACE_INFO, "EV1527: 0x%x lo %d, high %d, n: %d\n", self->data, self->ll, self->hl, self->correct_decodes);
        snprintf(s, sl, "EV1527: %lu",self->data);
        self->hl /= 24;
        self->ll /= 24; // Take average
        res = 1;
        GPIO_write(BSP_LED_RX, 1);
        self->state = 0;
      }
    }
  }
  return res;
}

static uint8_t nexa_state=0;
static uint32_t nexa_data=0;

static int nexa_decode(uint16_t pw, char *s, int sl)
{
  int res = 0;
  if(pw == 0) {
    nexa_state = 0;
  } else if(pw < 2900 && pw > 2300 ) {// Sync low
    nexa_data = 0;
    nexa_state = 4;
  } else if(nexa_state >= 4) {
    if((nexa_state & 1) == 0) {
      if(nexa_is_short_pulse(pw) == 0) {
        nexa_state = 0;  // High pulse expected to be short
      }
    } else if((nexa_state & 3) == 1) {
      bm_add_bit(nexa_data, nexa_is_long_pulse(pw));
    } else {
      if(bm_get_bit(nexa_data, 0) != nexa_is_short_pulse(pw)) { // If long low pulse where detected (1) in state&3=1, expect this to be short
        nexa_state = 0;
      }
    }

    nexa_state++;
    if(nexa_state >= (128+4)) {
      TTRACE(TTRACE_INFO, "NEXA: 0x%x\n", nexa_data);
      TTRACE(TTRACE_INFO, "NEXA: house code 0x%x, unit %d, cmd: %d, channel: %d\n",
             nexa_data >> 6, nexa_data & 3, (nexa_data>>4) & 3, (nexa_data>>2) & 3);
      snprintf(s, sl, "NEXA: %lu",nexa_data);
      res = 1;
      nexa_state = 0;
    }

  }
  return res;
}
/*
From:
  https://forum.pilight.org/Thread-Elro-Flamingo-FA20RF-Smoke-detector
Test Button press 1

--[RESULTS]--

hardware:       433gpio
pulse:          3
rawlen:         52
binlen:         13
pulselen:       393

Raw code:
8253 786 786 1572 786 2751 786 2751 786 2751 786 1572 786 1572 786 1572 786 2751 786 2751 786 1572 786 2751 786 2751
786 2751 786 1572 786 1572 786 2751 786 2751 786 1572 786 2751 786 2751 786 1572 786 2751 786 2751 786 1179 786 13362
Binary code:
1111111111111

Test Button press 2

--[RESULTS]--

hardware:       433gpio
pulse:          4
rawlen:         52
binlen:         13
pulselen:       393

Raw code:
8253 786 786 1572 786 2751 786 2751 786 2751 786 1572 786 1572 786 1572 786 2751 786 2751 786 1572 786 2751 786 2751
786 2751 786 1572 786 1572 786 2751 786 2751 786 1572 786 2751 786 2751 786 1572 786 2751 786 2751 786 1572 786 13362
Binary code:
1111111111111

Smoke Alert

--[RESULTS]--

hardware:       433gpio
pulse:          2
rawlen:         52
binlen:         13
pulselen:       391

Raw code:
8211 782 782 1564 782 2737 782 2737 782 2737 782 1564 782 1564 782 1564 782 2737 782 2737 782 1564 782 2737 782 2737
782 2737 782 1564 782 1564 782 2737 782 2737 782 1564 782 2737 782 2737 782 1564 782 2737 782 2737 782 1564 782 13294
*/

static uint8_t fl_state=0;
static uint32_t fl_data=0;

static int flamingo_decode(uint16_t pw, char *s, int sl)
{
  int res = 0;
  if(pw == 0) {
    fl_state = 0;
  } else if(pw < 9000 && pw > 7000 ) {// Sync
    fl_data = 0;
    fl_state = 2;
  } else if(fl_state >= 2) {
    if(pw >= 1200 && pw <= 1500) {
      fl_data <<= 1;
    } else if(pw >= 2400 && pw <= 2900) {
      fl_data = (fl_data << 1) | 1;
    }

    fl_state++;
    if(fl_state >= (50+2)) {
      TTRACE(TTRACE_INFO, "FLAMINGO: 0x%x\n", fl_data);

      snprintf(s, sl, "FLAMINGO: %lu",fl_data);
      res = 1;
      fl_state = 0;
    }

  }
  return res;
}


#define DBG_MAX_PULSES 64
static uint8_t dbg_state=0;
static uint16_t dbg_pulses[DBG_MAX_PULSES];

static uint16_t dbg_longest = 0;
static uint16_t dbg_shortest = 65535;

static int debug_decode(uint16_t pw, char *s, int sl)
{
  int res = 0;

  if(pw > dbg_longest) {
    dbg_longest = pw;
  }
  if(pw < dbg_shortest) {
    dbg_shortest = pw;
  }
  // If a sync or reset
  if(pw > 2000 || pw == 0) {
    // If at least this many pulses received
    if(dbg_state > 24) {
      // Print pulse times all pulses
      int o = snprintf(s, sl, "DBG: ");
      for(int n = 0; n < dbg_state && sl > o; n++) {
        o+= snprintf(s+o, sl-o, "%u,", dbg_pulses[n]);
      }
      // Indicate data received
      res = 1;
    }
    // If reset set state to 0 otherwise start receiving
    dbg_state = (pw == 0) ? 0 : 1;
  }
  // If not in reset state
  if(dbg_state > 0) {
    // Store pulse length
    if(dbg_state <= DBG_MAX_PULSES) {
      dbg_pulses[dbg_state-1] = pw;
    }
    dbg_state++;
  }
  return res;
}



static ev1527_t ev1527_slow, ev1527_fast;


void rc_start_rx(void)
{
  rx_pin_stable_count = 0;
  time_table_idx = 0;
  prev_rx_pin_state = 0;
}

// Previously decoded string
static char decs_prev[128] = {0};


static void rc_publish_if_no_repeat(char *decs)
{
  // If decode string is different or repeat suppress timer count down to zero
  if(strcmp(decs_prev, decs_prev) != 0 || repeat_supress_tmr == 0) {
    strcpy(decs_prev, decs);
    repeat_supress_tmr = 1000000 / RX_POLL_PERIOD_US;
  }
}

static char debug_s[256];

void rc_rx_poll(void)
{
  int res;
  char decs[128];
  while(!CBUF_IsEmpty(rx_fifo)) {
    uint16_t pw = CBUF_Pop(rx_fifo);
    // EV1527 with slow timing
/*
    res = ev1527_decode(&ev1527_slow, pw, decs, sizeof(decs), 13000, 950);
    if(res) {
      TTRACE(TTRACE_INFO, "RC_PARSE: Found EV1527 slow rc string %s\n", decs);
      rc_publish_if_no_repeat(decs);
    }

    // EV1527 with fast timing
    res = ev1527_decode(&ev1527_fast, pw, decs, sizeof(decs), 6000, 450);
    if(res) {
      TTRACE(TTRACE_INFO, "RC_PARSE: Found EV1527 fast rc string %s\n", decs);
      rc_publish_if_no_repeat(decs);
    }
    res = nexa_decode(pw, decs, sizeof(decs));
    if(res) {
      rc_publish_if_no_repeat(decs);
    }
    res = flamingo_decode(pw, decs, sizeof(decs));
    if(res) {
      rc_publish_if_no_repeat(decs);
    }
  */

    res = debug_decode(pw, debug_s, sizeof(debug_s));
    if(res) {
      TTRACE(TTRACE_INFO, "%s\n", debug_s);
    }
  }



}



void rc_rx_init(void)
{
  TTRACE(TTRACE_INFO, "RC_RX init\n");
  CBUF_Init(rx_fifo);

  rcTim = TIM_create(BSP_RC_TIMER);
  TIM_configure(rcTim, 1000000, RX_POLL_PERIOD_US);
  TIM_setOverFlowCallback(rcTim, timer_cb, 0);
  TIM_start(rcTim);

  rc_start_rx();
}
