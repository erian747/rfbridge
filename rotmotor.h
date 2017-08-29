#ifndef ROTMOTOR_H
#define ROTMOTOR_H
#include <stdint.h>

typedef enum
{
  ROTMOTOR_STOP,
  ROTMOTOR_FORWARD,
  ROTMOTOR_BACK,
} rotmotor_cmd_t;

void rotmotor_timer(void);
void rotmotor_init(void);
void rotmotor_control(rotmotor_cmd_t cmd);

#endif

