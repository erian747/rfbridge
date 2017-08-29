#include <stddef.h>
#include <string.h>
#include "mcal.h"
#include "bsp.h"
#include "blf.h"

static const uint8_t output_pattern_full_step[] = {
  9, 5, 6, 0xa, //9, 3, 6, 0xc,
};

static const uint8_t output_pattern_half_step[] = {
 // 1, 3, 2, 6, 4, 0xc, 8, 9
  1, 5, 2, 6, 4, 0xa, 8, 9
};

#define output_pattern output_pattern_full_step
#define output_pattern_len (sizeof(output_pattern) / sizeof(output_pattern[0]))

typedef struct {
  int speed;
  int patterns_remain;
  uint8_t dir;
  uint8_t os;
} sm_t;

static BLFCbTimer timer;
static sm_t stepmotor;

static void output(uint8_t pattern)
{
  GPIO_write(BSP_SM_A1, pattern & 1);
  GPIO_write(BSP_SM_A2, pattern & 2);
  GPIO_write(BSP_SM_B1, pattern & 4);
  GPIO_write(BSP_SM_B2, pattern & 8);
}




static void timer_cb(BLFCbTimer *t, void *ctx)
{
  sm_t *sm = (sm_t *)ctx;
  sm->os++;
  if(sm->os >= output_pattern_len) {
    sm->os = 0;
    sm->patterns_remain--;
    if(sm->patterns_remain == 0) {
      output(0);
      return;
    }
  }
  uint8_t pi = sm->dir ? (sm->os) : (3-sm->os);
  output(output_pattern[pi]);
  BLF_startCbTimer(t, timer_cb, sm, 1000 / sm->speed);
}

void sm_run(uint8_t dir, int speed, int steps)
{
  sm_t *sm = &stepmotor;
  if(steps == 0) {
    return;
  }
  if(speed > 1000) {
    speed = 1000;
  }
  if(sm->patterns_remain == 0) {
    sm->patterns_remain = steps;
    sm->speed = speed;
    sm->dir = dir;

    uint8_t pi = sm->dir ? (sm->os) : (3-sm->os);
    output(output_pattern[pi]);
    BLF_startCbTimer(&timer, timer_cb, sm, 1000 / speed);
  }
}


void sm_init(void)
{
  sm_t *sm = &stepmotor;
  memset(&sm, 0, sizeof(sm));

  tim_t *t = TIM_create(BSP_SM_TIMER);
  TIM_configure(t, 36000000, 1024);

  pwm_t *pwma = PWM_create(t, 0);
  pwm_t *pwmb = PWM_create(t, 1);
  PWM_configure(pwma, TIM_MODE_PWM, 0, 0);
  PWM_configure(pwmb, TIM_MODE_PWM, 0, 0);

  PWM_setDuty(pwma, PWM_MAX_DUTY * 0.5);
  PWM_setDuty(pwmb, PWM_MAX_DUTY * 0.5);
  PWM_outputEnable(pwma, 1);
  PWM_outputEnable(pwmb, 1);
  TIM_start(t);


  BLF_initCbTimer(&timer);
}


