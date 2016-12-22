#include <stdint.h>

#ifndef FW_REV_STR
  #error FW_REV_STR must be defined
#endif

// Firmware revision
#define FWRXSTR(x) #x
#define FWRSTR(x) FWRXSTR(x)
const char FWREV_text[] = FWRSTR(FW_REV_STR);

// Parse revision string
void FWREV_get(uint16_t *high, uint16_t *mid, uint16_t *low)
{
  int pos = 0; // 0 = High, 1 = Med, 2 = Low
  int parsed[3] = {0,0,0};

  // For whole text string
  const char *cp;
  for(cp = FWREV_text; *cp != 0; cp++)
  {
    char c = *cp;
    // If a literal
    if((c >= '0') && (c <= '9'))
    {
      parsed[pos] = parsed[pos]*10 + (c-'0');
    }
    // If separator
    else if(c == '.')
    {
      pos++;
      if(pos > 2)
        break;
    }
  }

  *high = parsed[0];
  *mid = parsed[1];
  *low = parsed[2];
}

const char *FWREV_getString(void)
{
  return FWREV_text;
}

