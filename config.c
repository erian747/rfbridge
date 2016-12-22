#define MODULE_NAME CONFIG

#include <string.h>
#include "config.h"
#include "mcal.h"
#include "crc.h"
#include "eeprom.h"

#define EEPROM_CONFIG_ID 1
#define EEPROM_CONFIG_VER 1
#define EEPROM_MAGIC_NUMBER 0xaabbccdd

bpsled_config_t bpsled_cfg;
static volatile uint8_t store_flag = 0;



void config_store(void)
{
  store_flag = 1;
}


void config_set_defaults(void)
{
  // Set Default config
  memset(&bpsled_cfg, 0, sizeof(bpsled_cfg));
  bpsled_cfg.dimLevel = 0xffff;
  bpsled_cfg.magicNumber = EEPROM_MAGIC_NUMBER;
  store_flag = 1;
}

void config_init(void)
{
  EEPROM_init();
  EEPROM_RegisterData(EEPROM_CONFIG_ID, EEPROM_CONFIG_VER, sizeof(bpsled_cfg));
  EEPROM_initializeEEPROM();
  
  
  if(EEPROM_Read(EEPROM_CONFIG_ID, (uint8_t *)&bpsled_cfg) == EEPROM_SUCCESS)
  {
    TTRACE(TTRACE_INFO,"CONFIG: Config successfully read from eeprom\n");
    if(bpsled_cfg.magicNumber != EEPROM_MAGIC_NUMBER)
    {
      TTRACE(TTRACE_WARN,"CONFIG: Config magic number mismatch, found 0x%x, expected: 0x%x\n", bpsled_cfg.magicNumber, EEPROM_MAGIC_NUMBER);
      config_set_defaults();
    }   
  }
  else
  {
    TTRACE(TTRACE_WARN,"CONFIG: Could not read config from eeprom\n");
    config_set_defaults();  
  }
}


void config_poll(void)
{

  MCAL_LOCK_IRQ();
  if(store_flag)
  {
    store_flag = 0;
    MCAL_UNLOCK_IRQ();
    EEPROM_Write(EEPROM_CONFIG_ID, (const uint8_t *)&bpsled_cfg);
    TTRACE(TTRACE_INFO,"CONFIG: Wrote config to EEPROM\n");
   }
  MCAL_UNLOCK_IRQ();
}
