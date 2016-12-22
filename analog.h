
/** @file analog.h
 * @author  EA
 * @brief  Analog input module.
 * Handles reading of analog values from ADC and steering of external multiplexer
 *
 *//////////////////////////////////////////////////////////////////////////////

/** @addtogroup Devices
  * @{
  */

/** @defgroup Analog Analog
  * @brief Analog module
  * @{
  */

#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>
#include "mcal_gpio.h"
#ifdef __cplusplus
extern "C" {
#endif


#define ANALOG_SCALE_FROM_RAW(raw, scale, resolution) \
  (BSP_ADC_REF_VOLTAGE * (scale) * (int32_t)(raw)) / ((4095*1000) / resolution);





typedef enum
{
  ANALOG_FILTERSPEED_SLOW,
  ANALOG_FILTERSPEED_FAST
} analog_filterspeed_t;

typedef struct analog_t analog_t;

 /**
 * @brief Return scaled analog respresenting pin voltage in mV
 * @param self Handle
 * @param id Analog channel id
 * @retval Scaled value
 */
int32_t ANALOG_getValue(analog_t *self, uint32_t id);
 /**
 * @brief Return raw value converted by ADC
 * @param self Handle
 * @param id Analog channel id
 * @retval Converted value
 */
uint16_t ANALOG_getRawValue(analog_t *self, uint32_t id);

typedef struct analogfilter_t analogfilter_t;

void ANALOG_addFilter(analog_t *self, analogfilter_t *filter, analog_filterspeed_t speed);

/**
 * @brief Create analog handler
 * @param channels List of channels to convert
 * @param length Length of channel list
 * @retval Handle
 */
analog_t *ANALOG_create(const gpio_t *channels, uint8_t length);

#ifdef __cplusplus
}
#endif


#endif

/**
  * @}
  */

/**
  * @}
  */
