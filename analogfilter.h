#ifndef ANALOGFILTER_H
#define ANALOGFILTER_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct analogfilter_t analogfilter_t;
  struct analogfilter_t
  {
    void *ctx;
    int32_t scale;
    int32_t offset;
    struct analogfilter_t *next;
    void (*start)(void *opaque);
    void (*stop)(void *opaque);
    void (*input)(struct analogfilter_t *filter, uint16_t *raw);
    void (*output)(struct analogfilter_t *filter, uint16_t *raw);
    uint32_t enabled;
  };

 
static inline int32_t analogfilter_scale(analogfilter_t *self, int32_t value)
{
  return (value * self->scale) + self->offset;
}

static inline void analogfilter_set_scale(analogfilter_t *self, int32_t scale)
{
  self->scale = scale; //* BSP_ADC_REF_VOLTAGE;  
}

static inline void analogfilter_set_offset(analogfilter_t *self, int32_t offset)
{
  self->offset = offset;  
}

void analogfilter_init(analogfilter_t *self, void (*start)(void *opaque), void (*stop)(void *opaque), void *ctx);

#ifdef __cplusplus
}
#endif



#endif

