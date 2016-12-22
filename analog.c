#define MODULE_NAME ANALOG

#include "mcal.h"
#include "trace.h"
#include "blf.h"
#include "analogfilter.h"
#include "analog.h"
#include "analog_config.h"

#ifndef ANALOG_SLOW_FILTER_UPDATE_MS
  #define ANALOG_SLOW_FILTER_UPDATE_MS 10
#endif

#ifndef ANALOG_USE_MUX
  #define ANALOG_USE_MUX 0
#endif

#ifndef ANALOG_USE_SLOW_FILTERS
  #define ANALOG_USE_SLOW_FILTERS 0
#endif

#ifndef ANALOG_USE_TRIGGER_TIMER
  #define ANALOG_USE_TRIGGER_TIMER 0
#endif

// Events
enum
{
  SLOW_IND = BLF_FIRST_USER_SIG,
};

struct analog_t
{
#if ANALOG_USE_TRIGGER_TIMER
  tim_t *adcTriggTim;
#endif  
  adc_t *adc;
  analogfilter_t *fastfilters;
#if ANALOG_USE_SLOW_FILTERS
  BLFTimer *timer;
  analogfilter_t *slowfilters;
#endif
#if ANALOG_USE_MUX
  uint8_t amuxCh;
  uint8_t amuxOffset;
#endif
  uint16_t rawData[];
};

// Process input for all filters in chain
static inline void filter_process(analogfilter_t *filter, uint16_t *rawData)
{
  while(filter != 0)
  {
    if(filter->enabled)
      filter->input(filter, rawData);
    filter = filter->next;
  }
}


// Process fast filters, 100us period
static void adc_done_cb(void *ctx)
{
  analog_t *self = (analog_t *)ctx;

  filter_process(self->fastfilters, self->rawData);

#if ANALOG_USE_MUX
  self->rawData[self->amuxCh + self->amuxOffset] = self->rawData[BSP_ADC_CH_AMUX];
  self->amuxCh++;
  if(self->amuxCh >= ANALOG_AMUX_CHANNELS)
    self->amuxCh = 0;

  GPIO_write(BSP_AMUX_CTRL0, self->amuxCh & 1);
  GPIO_write(BSP_AMUX_CTRL1, self->amuxCh & 2);
#if ANALOG_AMUX_CHANNELS > 4
  GPIO_write(BSP_AMUX_CTRL2, self->amuxCh & 4);
#endif

#endif
}





int32_t ANALOG_getValue(analog_t *self, uint32_t id)
{
  return (BSP_ADC_REF_VOLTAGE * self->rawData[id]) / 4095;
}

uint16_t ANALOG_getRawValue(analog_t *self, uint32_t id)
{
  return self->rawData[id];
}



void ANALOG_addFilter(analog_t *self, analogfilter_t *filter, analog_filterspeed_t speed)
{
  MCAL_LOCK_IRQ();
#if ANALOG_USE_SLOW_FILTERS
  if(speed == ANALOG_FILTERSPEED_SLOW)
  {
    filter->next = self->slowfilters;
    self->slowfilters = filter;
  }
  else
#endif
  {
    filter->next = self->fastfilters;
    self->fastfilters = filter;
  }
  MCAL_UNLOCK_IRQ();
}



static void runningFilterProcess(analog_t *self, BLFTimer *evt);

static BLFState running =
{
  [BLF_ENTRY_SIG] = 0,
  [SLOW_IND] = (BLFEventHandler)runningFilterProcess,
};


static void runningFilterProcess(analog_t *self, BLFTimer *evt)
{
#if ANALOG_USE_SLOW_FILTERS
  filter_process(self->slowfilters, self->rawData);

  //Restart timer
  BLF_startTimer(evt, ANALOG_SLOW_FILTER_UPDATE_MS);
#endif
}


// Init function
static void init(analog_t *self, void *evt)
{
#if ANALOG_USE_SLOW_FILTERS
  // Start background timer
  BLF_startTimer(self->timer, ANALOG_SLOW_FILTER_UPDATE_MS);
#endif
  ADC_start(self->adc, adc_done_cb, self);
#if ANALOG_USE_TRIGG_TIMER
  // Start trigg timer
  TIM_start(self->adcTriggTim);
#endif

  BLF_transit(self, running);
}



analog_t *ANALOG_create(const gpio_t *channels, uint8_t length)
{

  // Calculate length of output buffer
  uint8_t bufferLength = length * 2;
#if ANALOG_USE_MUX
  bufferLength += ANALOG_AMUX_CHANNELS * 2;
#endif

  // Create thread
  analog_t *self = (analog_t *)BLF_createThread(sizeof(*self)+bufferLength, 0, (BLFEventHandler)init, 0);
  self->fastfilters = 0;

#if ANALOG_USE_MUX
  self->amuxCh = 0;
  self->amuxOffset = length;
#endif

#if ANALOG_USE_SLOW_FILTERS
  self->slowfilters = 0;
  // Create slow filter process timer
  self->timer = BLF_createTimer(self, SLOW_IND, 0);
#endif
  BLF_memset(self->rawData, 0, bufferLength);

  self->adc = ADC_create(0);

#if ANALOG_USE_TRIGGER_TIMER
  self->adcTriggTim = TIM_create(ANALOG_TRIGGER_TIMER);
  // Run at 1MHz, count to 100 to trigg every 100us
  TIM_configure(self->adcTriggTim , 1000000, 100);
  //pwm_t *adcTriggPwmChannel = PWM_create(self->adcTriggTim , BSP_ADC_TRIGG_TIMER_CH);
  //PWM_configure(adcTriggPwmChannel, TIM_MODE_PWM, 0, self); // Use PWM mode
  //PWM_setDuty(adcTriggPwmChannel, TIM_PERIOD_MAX / 2); // Set 50% duty cycle
#endif
  
	ADC_configure(self->adc, channels, length, ANALOG_TRIGGER_IDENTIFIER, self->rawData);


  return self;
}
