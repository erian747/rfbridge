#ifndef CONFIG_H
#define CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#define CONFIG_MAX_NR_ERROR_COUNTERS 7
typedef struct
{
  uint16_t dimLevel;
  uint16_t cntStartup;
  uint32_t cntTempErr;
  uint32_t cntUvlo;
  uint32_t reserved2[5];
  uint32_t magicNumber;
} bpsled_config_t;

extern bpsled_config_t bpsled_cfg;

void config_init(void);
void config_poll(void);
void config_set_defaults(void);
uint8_t config_at_defaults(void);
void config_store(void);

#ifdef __cplusplus
}
#endif

#endif

