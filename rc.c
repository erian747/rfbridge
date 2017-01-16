#define MODULE_NAME RC
#include <string.h>
#include <stdio.h>
#include "bsp.h"
#include "mcal.h"
#include "trace.h"


#include "cbuf.h"
//#include "rtl_433_devices.h"

//#define RC_RX_TEST 0



#define RX_POLL_PERIOD_US 25

#define TIME_TABLE_SIZE 256


#ifndef RC_RX_TEST
static tim_t *rcTim=0;
static pwm_t *rcPwm=0;
#endif

static uint16_t tx_time_table[TIME_TABLE_SIZE];

static volatile uint8_t tx_time_table_length;
static volatile uint8_t tx_time_table_idx;


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




//#define GPIO_read(pin) ((GPIO_REG_READ(GPIO_IN_ADDRESS) & ((uint32_t)1 << pin)) == 0) // Inverted by 5 to 3.3v level shifter transistor




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
#ifndef RC_RX_TEST
static void pwm_cb(void *ctx)
{
  //if(rcState == RC_STATE_TX)
  //{
  tx_time_table_idx++;
  if(tx_time_table_idx < tx_time_table_length) {
    rcTxPinState = !rcTxPinState;
    GPIO_write(BSP_RC_TX_PIN, rcTxPinState);
    PWM_setDelay(rcPwm, tx_time_table[tx_time_table_idx]);
    //hw_timer_arm(time_table[time_table_idx]);
  } else {
    if(rcRepeat) { // Repeat transmission
      rcRepeat--;
      tx_time_table_idx = 0;
      rcTxPinState = 1;
      GPIO_write(BSP_RC_TX_PIN, 1);
      PWM_setDelay(rcPwm, tx_time_table[tx_time_table_idx]);
      //hw_timer_arm(time_table[time_table_idx]);
    } else { // Done
      TTRACE(TTRACE_INFO, "RC_TX: Tx finished\n");
      //PWM_disable(rcPwm);
      // Call done callback
      // if(rc_done_cb != 0)
      //   rc_done_cb(rc_cb_ctx);
    }
  }
  //}
}
#endif

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
#define rx_rc_pin_get() GPIO_read(BSP_315MHZ_RX)
#endif

static uint32_t repeat_supress_tmr = 0;
static uint8_t rx_found_valid = 0;

static void timer_cb(void *ctx)
{
  if(repeat_supress_tmr != 0) {
    repeat_supress_tmr--;
  }
  uint8_t pv = rx_rc_pin_get();
  // If rx pin have not changed state
  if(prev_rx_pin_state == pv) {
    // dont roll over
    if(rx_pin_stable_count < RX_IDLE_TIME) {// If not in idle state

      rx_pin_stable_count++;
      if(rx_pin_stable_count >= RX_IDLE_TIME) {// No activity for 50ms, become idle

        if(pv != 0) {
          TTRACE(TTRACE_WARN, "rc_timer_cb: Expected low level when idle\n");
        }
        if(!CBUF_IsFull(rx_fifo) && rx_found_valid) {
          CBUF_Push(rx_fifo, 0); // Signal reset state machine
          rx_found_valid = 0;
        }
      }
    }

  } else { // Changed state
    uint32_t us = rx_pin_stable_count * RX_POLL_PERIOD_US;
    if(us >= 250 && us <= 25000) {
      rx_found_valid = 1;
      CBUF_Push(rx_fifo, us);
    } else if(rx_found_valid) {
      CBUF_Push(rx_fifo, 0);
    }
    prev_rx_pin_state = pv;  // Update prev pin state
    rx_pin_stable_count = 0; // Reset stable counter
  }

}
#ifndef RC_RX_TEST
static void send_common(int pin, uint8_t repeat, uint16_t length)
{
  //rcState = RC_STATE_TX;
  tx_time_table_idx = 0;
  tx_time_table_length = length;
  rcRepeat = repeat;
  rcTxPin = pin;

  rcTxPinState = 1;
  // Set and configure pin as output
  GPIO_write(BSP_RC_TX_PIN, 1);
  // Enable timer
  //RTC_REG_WRITE(FRC1_CTRL_ADDRESS, DIVDED_BY_16 | FRC1_ENABLE_TIMER | TM_EDGE_INT);
  //hw_timer_arm(time_table[0]); // First value is used directly
  PWM_setDelay(rcPwm, tx_time_table[0]);
}
#endif




void rc_send_nexa(int pin, uint32_t code, uint8_t unit, uint8_t cmd)
{
  rc_send_anslut_proove_nexa(pin, code, unit, cmd, 0, NEXA_REPEAT);
}

void rc_send_anslut(int pin, uint32_t code, uint8_t unit, uint8_t cmd)
{
  rc_send_anslut_proove_nexa(pin, code, unit, cmd, 3, ANSLUT_REPEAT);
}


#define nexa_is_short_pulse(_usec) (_usec >= (ANSLUT_PULSE_HIGH-100) && _usec <= (ANSLUT_PULSE_HIGH+100))
#define nexa_is_long_pulse(_usec) (_usec > (ANSLUT_PULSE_ZERO_LOW-300) && _usec < (ANSLUT_PULSE_ZERO_LOW+300))

#define bm_add_bit(bm, b)  bm = (bm << 1) | ((b) != 0)
#define bm_get_bit(bm, b)  ((bm & (1 << b)) != 0)
extern void mqtt_link_publish_rcrx(const char *s);


static uint8_t ev1527_state=0;
static uint32_t ev1527_data=0;
static uint16_t hl, ll;

static int correct_decodes = 0;

/*
static void domoticz_send(uint32_t data)
{

  // { "idx" : 3, "nvalue" : variable 0 or 1 }' -t 'domoticz/in'


  uint8_t turn_on = 0;
  if(data == 0x681a37)
    turn_on = 1;

  char buff[64];
  snprintf(buff, 64, "{ \"idx\" : 3, \"nvalue\" : %d }", turn_on);
  TTRACE(TTRACE_INFO, "Sending domiticz cmd %s", buff);

  mqtt_link_publish_rctx(buff);
}
*/

static int ev1527_decode(uint16_t pw, char *s, int sl)
{
  int res = 0;
  if(pw == 0) {
    if(ev1527_state != 0) {
      ev1527_state = 0;
    }
  } else if(pw > 12000 && pw < 14000) {
    TTRACE(TTRACE_INFO, "EV1527: Sync detected enter data state\n");
    ev1527_data = 0;
    ev1527_state = 2;
  } else if(ev1527_state >= 2) {
    if(pw > 1500) {
      TTRACE(TTRACE_INFO, "EV1527: To long data pulse %dus in state %d\n", pw, ev1527_state);
      ev1527_state = 0;
    } else {

      if(ev1527_state & 1) {
        bm_add_bit(ev1527_data, pw > 800);
      }
      ev1527_state++;

      if(ev1527_state & 1) {
        hl = pw;
      } else {
        ll = pw;
      }

      if(ev1527_state >= (24*2)+2) {
        correct_decodes++;
        TTRACE(TTRACE_INFO, "EV1527: 0x%x lo %d, high %d, n: %d\n", ev1527_data, ll, hl, correct_decodes);
        snprintf(s, sl, "EV1527: %lu",ev1527_data);
        res = 1;
        ev1527_state = 0;
      }
    }
  }
  return res;
}

static uint8_t nexa_state=0;
static uint32_t nexa_data=0;

static void nexa_decode(uint16_t pw)
{
  if(pw == 0) {
    nexa_state = 0;
  } else if(nexa_state == 0) {
    if(pw < 300 && pw > 200) { // Sync high
      nexa_state++;
    } else {
      nexa_state = 255;
    }
  } else if(nexa_state == 1) {
    if(pw < 2900 && pw > 2400 ) {// Sync low
      nexa_data = 0;
      nexa_state = 4;
    } else {
      nexa_state = 0;
    }
  } else if(nexa_state == 255) { // Ignore negative pulse
    nexa_state = 0;
  }

  else {
    if((nexa_state & 1) == 0) {
      if(pw < 200 || pw > 300) {
        nexa_state = 255;  // High pulse expected to be 250us
      }
    } else if((nexa_state & 3) == 1) {
      bm_add_bit(nexa_data, nexa_is_long_pulse(pw));
    }

    else {
      if(bm_get_bit(nexa_data, 0) != nexa_is_short_pulse(pw)) { // If long low pulse where detected (1) in state&3=1, expect this to be short
        nexa_state = 0;
      }
    }

    nexa_state++;
    if(nexa_state >= (128+4)) {
      TTRACE(TTRACE_INFO, "NEXA: 0x%x\n", nexa_data);
      TTRACE(TTRACE_INFO, "NEXA: house code 0x%x, unit %d, cmd: %d, channel: %d\n",
             nexa_data >> 6, nexa_data & 3, (nexa_data>>4) & 3, (nexa_data>>2) & 3);

      nexa_state = 0;
    }

  }
}

#if 0
static void nexa_decode(const uint16_t *rb, uint16_t rawLength)
{
  // Decode message
  char outputstr[128];
  memset(outputstr, 0, sizeof(outputstr));
  int startPos, s;
  for(startPos = 0; startPos <= (rawLength - 124); startPos++) {
    char c;
    char *os = outputstr;

    for(s = 0; s < 128; s+=4) {
      c = 0;
      if(nexa_is_short_pulse(rb[s+startPos])) {
        if(nexa_is_short_pulse(rb[s+startPos+1]) &&  nexa_is_short_pulse(rb[s+startPos+2]) && nexa_is_long_pulse(rb[s+startPos+3])) {
          c = '0';
        } else if (nexa_is_long_pulse(rb[s+startPos+1]) &&  nexa_is_short_pulse(rb[s+startPos+2]) && nexa_is_short_pulse(rb[s+startPos+3])) {
          c = '1';
        }
      }
      if(c == 0) {
        break;
      }
      *os++ = c;
    }
    if(c != 0) {
      break;
    }
  }
  TTRACE(TTRACE_INFO, "RC_PARSE: Found nexa rc string %s\n", outputstr);
}
#endif


void rc_start_rx(void)
{
  rx_pin_stable_count = 0;
  time_table_idx = 0;
  prev_rx_pin_state = 0;
}

// Previously decoded string
static char decs_prev[32] = {0};

void rc_poll(void)
{
  int res;
  char decs[32];
  while(!CBUF_IsEmpty(rx_fifo)) {
    uint16_t pw = CBUF_Pop(rx_fifo);
    res = ev1527_decode(pw, decs, sizeof(decs));
    if(res) {
      // If decode string is different or repeat suppress timer count down to zero
      if(strcmp(decs_prev, decs_prev) != 0 || repeat_supress_tmr == 0) {
        mqtt_link_publish_rcrx(decs);
        strcpy(decs_prev, decs);
        repeat_supress_tmr = 1000000 / RX_POLL_PERIOD_US;
      }
    }
  }

}



void rc_timer_init(void)
{
  CBUF_Init(rx_fifo);

  rcTim = TIM_create(BSP_RC_TIMER);
  TIM_configure(rcTim, 1000000, RX_POLL_PERIOD_US);
  TIM_setOverFlowCallback(rcTim, timer_cb, 0);

  tim_t *pwmTim = TIM_create(0);
  TIM_configure(pwmTim, 1000000, TIM_PERIOD_MAX);
  rcPwm = PWM_create(pwmTim, 0);
  PWM_configure(rcPwm, TIM_MODE_DELAY, pwm_cb, 0);
  TIM_start(pwmTim);
  TIM_start(rcTim);

  rc_start_rx();

  /*

    TTRACE(TTRACE_INFO, "Starting test with nexa sequence\n");
    rc_send_anslut_proove_nexa(0, 0x12121212, 0, 1, 3, 6);
    test_init();
    int n;
    for(n = 0; n < (100000 / RX_POLL_PERIOD_US); n++) // Run for 100ms
    {
      timer_cb(0);
    }
  */
  /*
    for(n = 0; n < tx_time_table_length/2; n++)
    {
      pulse_data.pulse[n] = tx_time_table[n*2];
      pulse_data.gap[n] = tx_time_table[n*2+1];

    }
    pulse_data.num_pulses = tx_time_table_length/2;
    pulse_analyzer(&pulse_data, 1000);
  */
}
