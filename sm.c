#include <stddef.h>
#include <string.h>
#include "bsp.h"
#include "blf.h"

static const uint8_t output_pattern[] = {
  1, 2, 4, 8
};

#define output_pattern_len (sizeof(output_pattern) / sizeof(output_pattern[0]))

typedef struct {
  int speed;
  int steps_remain;
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
  output(output_pattern[sm->os]);
  sm->os++;
  if(sm->os >= output_pattern_len) {
    sm->os = 0;
    sm->steps_remain--;
    if(sm->steps_remain == 0) {
      return;
    }
  }
  BLF_startCbTimer(t, timer_cb, sm, 20);
}

void sm_run(uint8_t dir, int speed, int steps)
{
  sm_t *sm = &stepmotor;
  if(steps == 0) {
    return;
  }
  if(sm->steps_remain == 0) {
    sm->steps_remain = steps;
    sm->speed = speed;
    sm->dir = dir;
    BLF_startCbTimer(&timer, timer_cb, sm, 20);
  }
}


void sm_init(void)
{
  sm_t *sm = &stepmotor;
  memset(&sm, 0, sizeof(sm));
  BLF_initCbTimer(&timer);
}


