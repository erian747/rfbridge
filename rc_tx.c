#define MODULE_NAME RC_TX
#include <string.h>
#include <stdio.h>
#include "bsp.h"
#include "mcal.h"
#include "trace.h"
#include "blf.h"


#define TIME_TABLE_SIZE 256
static pwm_t *rc_pwm = NULL;

static volatile uint8_t tx_busy = 0;
static uint16_t tx_time_table[TIME_TABLE_SIZE];
static volatile uint8_t tx_time_table_length;
static volatile uint8_t tx_time_table_idx;

static uint16_t tx_pin_state;
static uint16_t tx_repeats;
static gpio_t tx_gpio;

static BLFQueue * tx_queue = NULL;


static void pwm_cb(void *ctx)
{
  tx_time_table_idx++;
  if(tx_time_table_idx < tx_time_table_length) {
    tx_pin_state = !tx_pin_state;
    GPIO_write(tx_gpio, tx_pin_state);
    PWM_setDelay(rc_pwm, tx_time_table[tx_time_table_idx]);
  } else {
    if(tx_repeats) { // Repeat transmission
      tx_repeats--;
      tx_time_table_idx = 0;
      tx_pin_state = 1;
      GPIO_write(tx_gpio, 1);
      PWM_setDelay(rc_pwm, tx_time_table[tx_time_table_idx]);

    } else { // Done
      TTRACE(TTRACE_INFO, "RC_TX: Tx finished: pulses %d\n", tx_time_table_idx);
      GPIO_write(tx_gpio, 0);
      tx_busy = 0;
    }
  }
}


static void send_common(gpio_t pin, uint8_t repeat, uint16_t length)
{
  tx_busy = 1;
  tx_time_table_idx = 0;
  tx_time_table_length = length;
  tx_repeats = repeat;
  tx_gpio = pin;

  tx_pin_state = 1;
  // Set and configure pin as output
  GPIO_write(tx_gpio, 1);

  // Enable timer
  PWM_setDelay(rc_pwm, tx_time_table[0]);
}

static int decode_and_send(const char *str, size_t length)
{
  const char *p = str;
  int got_defines = 0;
  int value = 0;
  int lidx = 0, pidx = 0;
  int repeats = 0;
  int ok = 1;
  uint16_t lengths[8];
  uint16_t *pulses = tx_time_table;

  while(p < (str + length) && ok) {

    char c = *p++;

    if(got_defines == 0) {
      if(c == ':') {
        repeats = value;
        if(repeats > 20) {
          ok = 0;
        }
        got_defines = 1;
      } else if(c == ',') {
        if(value > 100000 || lidx >= 8) {
          ok = 0;
        } else {
          lengths[lidx++] = value;
          value = 0;
        }
      } else if(c >= '0' && c <= '9') {
        value = value * 10 + (c - '0');
      }
    } else {
      if(c >= 'A' && c <= 'Z') {
        int pt = c - 'A';
        if(pt > lidx) {
          ok = 0;
          break;
        }
        if(pidx < TIME_TABLE_SIZE) {
          pulses[pidx++] = lengths[pt];
        } else {
          ok = 0;
        }
      }
    }

  }
  if(ok && pidx > 0) {
    send_common(BSP_433MHZ_TX, repeats, pidx);
  }
  return ok ? 0 : -1;
}
//-------------------------------------------------------------------------------------------------------
// Send raw pulse string

/* Example string: 430,1290,13330,3: ABBAABABABBAABBAABBAABABABBAABBAABBAABBAABBAABBAAC
    Three pulse lengths defined, A = 430, B = 1290 and C = 13300
    3 Repeats
    Data, A, B, C represents the pulselengths defined first in string

*/
int rc_tx_raw(const char *str)
{

  if(BLF_queueLength(tx_queue) > 8) {
    return -1;
  }
  char *qi = (char *)BLF_alloc(strlen(str) + 1);
  strcpy(qi, str);
  BLF_appendQueue(tx_queue, qi);
  return 0;
}




void rc_tx_init(void)
{
  TTRACE(TTRACE_INFO, "RC_TX init\n");
  tx_queue = BLF_createQueue();

  tim_t *pwmTim = TIM_create(0);
  TIM_configure(pwmTim, 1000000, TIM_PERIOD_MAX);
  rc_pwm = PWM_create(pwmTim, 0);
  PWM_configure(rc_pwm, TIM_MODE_DELAY, pwm_cb, 0);
  TIM_start(pwmTim);
}


void rc_tx_poll(void)
{
  if(tx_busy) {
    return;
  }
  char *qi = BLF_takeFirstInQueue(tx_queue);
  if(qi == NULL) {
    return;
  }
  decode_and_send(qi, strlen(qi));
  BLF_free(qi);
}
