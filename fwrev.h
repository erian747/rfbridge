
/** @file fwrev.h
 * @author  EA
 * @brief  FW revision parsing.
 * Handle fw revision
 *
 *//////////////////////////////////////////////////////////////////////////////

/** @addtogroup Utils
  * @{
  */

/** @defgroup FWREV FWREV
  * @brief Firmware revision module
  * @{
  */

#ifndef FWREV_H
#define FWREV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Firmware revision text string
 */ 
extern const char FWREV_text[];

/**
 * @brief  Get firmware revision
 * @param high Pointer to location where to store high fraction of fw revision
 * @param mid Pointer to location where to store middle fraction of fw revision
 * @param low Pointer to location where to store low fraction of fw revision
 */
void FWREV_get(uint16_t *high, uint16_t *mid, uint16_t *low);

/**
 * @brief  Get firmware revision string
 * @param None
 * @retval Pointer to null terminated string with firmware revision
 */
const char *FWREV_getString(void);
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

