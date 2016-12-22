
#include <stdint.h>
#include "analogfilter.h"


void analogfilter_init(analogfilter_t *self, void (*start)(void *opaque), void (*stop)(void *opaque), void *ctx)
{
  self->output = 0;
  self->input = 0;
  self->start = start;
  self->stop = stop;
  self->enabled = 0;
  self->ctx = ctx;
  self->scale = 1;
  self->offset = 0;
  
}

