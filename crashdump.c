/** @file crashdump.c
 * @author  EA
 * @brief  Crash dump module
 *
 */
 
/** @addtogroup Application
  * @{
  */

/** @defgroup CrashDump CrashDump
  * @brief Stores CPU registers and stack to flash when a SW crash situation arises
  * @{
  */
#ifdef STM32F4XX
  #include "stm32/stm32f4xx.h"
#elif defined(STM32F0XX)
  #include "stm32f0xx.h"
#else
  #include "stm32/stm32f10x.h"
#endif


#include "mcal.h"



/**
  * @brief  Flash protection key 1
  */
#define FLASH_KEY1 0x45670123
/**
  * @brief  Flash protection key 2
  */
#define FLASH_KEY2 0xCDEF89AB

#define DUMP_TO_UART 1

#ifdef STM32F4XX
  #define CRASHD_FLASH_AREA_START 0x080E0000
#else
  #define CRASHD_FLASH_AREA_START 0x08010000
#endif

/**
  * @brief Number used to check sanity of log data and that CRASHD_init has been executed
  */
#define CRASHD_MAGIC_NUMBER 0xBABADADA




/**
  * @brief Indicates that CRASHD_init has been executed
  */
static uint32_t initialized = 0;

/**
  * @brief Cortex-M3 registers to save in addition to CPU registers and stack
  */
#if defined(STM32F0XX)
static const volatile uint32_t * const core_regs[] = {&SCB->ICSR, &SCB->SHCSR};
#else
static const volatile uint32_t * const core_regs[] = {&SCB->ICSR, &SCB->HFSR, &SCB->CFSR, &SCB->MMFAR, &SCB->BFAR, &SCB->SHCSR};
#endif

/**
  * @brief Called when a Cortex-M3 HardFault or NMI exception occurs
  */
#if defined(__CC_ARM)
void __ASM HardFault_Handler(void)
{
  PRESERVE8
  EXPORT HardFault_Handler
  IMPORT CRASHD_store
  // Push rest of the registers to stack, R0-R3 and R12 are pushed by core when exception vector is called
  push {r4-r11}
  // Get MSP, use as argument for CRASHD_store
  mrs r0, msp
  // Store to flash
  bl CRASHD_store 
}

#else

extern uint32_t _estack __attribute__((weak));

void __attribute__((naked)) HardFault_Handler(void) 
{
  __asm volatile
  (
#if !defined(STM32F0XX)		
		" push {r4-r11}                                             \n"
#endif		
    " mrs r0, msp                                               \n"
    " ldr r2, store_address_const                        		    \n"
    " bx r2                                                     \n"
    " store_address_const: .word CRASHD_store    								\n"
  );
}

#endif




#ifdef DUMP_TO_UART


#define DUMP_UART USART1

static void uart_sendstr(const char *str)
{
  while(*str != 0)
  {
#if defined(STM32F0XX)
	  while((DUMP_UART->ISR & USART_ISR_TXE) == 0) {}
	  DUMP_UART->TDR = *str++;
#else
    while((DUMP_UART->SR & USART_SR_TXE) == 0) {}
    DUMP_UART->DR = *str++;
#endif

    
  }
}




static char *uint_to_dec(uint32_t num, char *bf)
{
  // End terminate
  *bf-- = 0;
  uint32_t a = num;

  if(a == 0)
    *bf-- = '0';

  while(a)
  {
    *bf-- =  (a % 10) + '0';
    a /= 10;
  }
  return bf+1;
}

static char *int_to_dec(int32_t num, char *bf)
{
  uint32_t a = num > 0 ? num : -num;
  // End terminate
  *bf-- = 0;

  if(a == 0)
    *bf-- = '0';

  while(a)
  {
    *bf-- =  (a % 10) + '0';
    a /= 10;
  }

  if(num < 0)
    *bf-- = '-';

  return bf+1;
}

static char *uint_to_hex(uint32_t num, char cs, char *bf)
{
  // End terminate
  *bf-- = 0;

  uint32_t a = num;

  if(a == 0)
    *bf-- = '0';

  while(a)
  {
    uint8_t nib = (a & 0xf);
    *bf-- = nib + ((nib > 9) ? (cs-10) : '0');
    a >>= 4;
  }
  return bf+1;
}

// Output string type
typedef struct
{
  char *data;
  uint8_t length;
  uint8_t maxLen;
} outstr_t;


// Write char to length limited string
static void writeToStr(outstr_t *os, char ch)
{
  if((os->length+1) < os->maxLen)
  {
    os->data[os->length] = ch;
  }
  os->length++;
}

// Put
static void putchw(outstr_t *os, uint8_t n, char pc, char* bf)
{
  char* p=bf;
  while (*p++ && n > 0)
    n--;
  // Leading pad
  while (n-- > 0)
  writeToStr(os,pc);

  while(*bf != 0)
  {
    writeToStr(os,*bf);
    bf++;
  }
}


enum
{
  S_TEXT,
  S_FORMAT_START,
};

static int dbg_snprintf(const char *fmt, const uint32_t *args)
{
  char outStr[128];
	uint32_t maxLength = sizeof(outStr);
	
	const char *parseStr = fmt;
  outstr_t os;
  os.maxLen = maxLength;
  os.data = outStr;
  os.length = 0;
  char bf[12];

  uint8_t state = S_TEXT;

  // Default pad with space
  char padChar = ' ';
  uint8_t width = 0;

  while(*parseStr != 0)
  {
    char ch = *parseStr++;

    if(state == S_TEXT)
    {
      if(ch == '%')
      state = S_FORMAT_START;
      else
      writeToStr(&os, ch);
    }
    else
    {
      switch(ch)
      {
        case 'c' :
        {
          writeToStr(&os, *args++);    //lint !e40
          state = S_TEXT;
        } break;

        case 'u' :
        {
          char *s_ptr = uint_to_dec(*args++,&bf[11]);  //lint !e40, !e78, !e530
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;

        case 'i' :
        case 'd' :
        {
          char *s_ptr = int_to_dec(*args++, &bf[11]);   //lint !e40, !e78
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;

        // Hex start found
        case 'x' :
        {
          char *s_ptr = uint_to_hex(*args++,'a',&bf[11]);  //lint !e40, !e78
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;

        // HEX in captials start found
        case 'X' :
        {
          char *s_ptr = uint_to_hex(*args++,'A',&bf[11]);  //lint !e40, !e78
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;
        // String
        case 's' :
        {
          char *str = (char *)*args++;
          while(*str != 0)
            writeToStr(&os,*str++);  
                  
        } break;
        case '0' :
        {
          // Pad with zeroes
          padChar = ch;
        } break;

        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case '8' :
        case '9' :
        {
          width = ch - '0';
        } break;

        // Ignore precision
        case '.' :
        {
        } break;

        // Unknown parameter flag
        default :
        {
          state = S_TEXT;
          writeToStr(&os, ch);
        }
      }
    }
  }
  // Terminate
  if((os.length + 1) < os.maxLen)
  {
    os.data[os.length] = 0;
  }
  else
  {
    os.data[os.maxLen-1] = 0;
  }

  uart_sendstr(os.data);
	return (int)os.length;
}





void __attribute__((used)) CRASHD_store(uint32_t stackStart)
{
  // Print core registers
	dbg_snprintf("\n------ Crash dump -------\n",0);
	dbg_snprintf("\nCore regs: ", 0);
  uint32_t n;
  for(n = 0; n < (sizeof(core_regs) / sizeof(core_regs[0])); n++)
  {
    dbg_snprintf(" %x", (const uint32_t *)core_regs[n]);
  }
	dbg_snprintf("\nStack start: %x\n", &stackStart);
	   
    // Get end of stack
#if defined(__CC_ARM)
		uint32_t stackEnd = (uint32)Image$$SRAM$$ZI$$Limit;
#else
    uint32_t stackEnd = (uint32_t)&_estack;
#endif	 
  // Calculate length of stack
  uint32_t stackLength = (stackEnd - stackStart); 
	
	dbg_snprintf("Stack length: %x\n", &stackLength);
#if defined(STM32F0XX)
  const char *reg_names[] = {"r0", "r1", "r2", "r3", "r12", "lr", "pc", "psr"};
#else
  const char *reg_names[] = {"r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r0", "r1", "r2", "r3", "r12", "lr", "pc", "psr"};
#endif
  // Dump pushed regs
  for(n = 0; n < (sizeof(reg_names) / sizeof(reg_names[0])); n++)
  {
	  uint32_t r = ((uint32_t *)stackStart)[n];
	  dbg_snprintf(reg_names[n], 0);
		dbg_snprintf(": %x\n",&r);
	}
  
  // Dump stack data
  dbg_snprintf("Stack dump\n", 0);
  for(; n < (stackLength/4); n++)
  {
	  uint32_t r = ((uint32_t *)stackStart)[n];
		dbg_snprintf("%08x ",&r);
	}
  // Lock further execution
  while(1) { }

}

#else // DUMP_TO_UART


/**
  * @brief Write data to flash
  * This function is provided instead of the one found in nvm.c because interrupts cannot be handled during an exception
  * @param flashAddr Pointer pointed to by flashAddr is updated with the new position in flash after the data that was written
  * @param data Pointer to data to be written
  * @param length length of data
  */
static void addToFlashArea(uint16_t **flashAddr, const void *data, uint16_t length)
{
  for(uint32_t n = 0; n < (length / 2); n++)
  {
    // Wait for pending programming to finish
    while(MCAL_IO_READ(FLASH->SR) & FLASH_SR_BSY) { }
#ifdef STM32F4XX
	  // Clear status bits
		FLASH->SR = 0xf3;
    // Enable programming flash wordwise
    MCAL_IO_WRITE(FLASH->CR, FLASH_CR_PG | FLASH_CR_PSIZE_0);
#else
		MCAL_IO_WRITE(FLASH->CR, FLASH_CR_PG);
#endif
    // Do a dummy read to compensate for bug in the flash-controller
    (void)MCAL_IO_READ(FLASH->SR);

    // Program word
    **flashAddr = ((const uint16_t *)data)[n];
    (*flashAddr)++;  
  }
}


#ifdef STM32F4XX


static void eraseArea(void)
{
	// Clear status bits
	FLASH->SR = 0xf3;

	// Set sector erase bit, set sector 11 (last)
	FLASH->CR = FLASH_CR_SER | (11 << 3);

	// Start erase
	FLASH->CR |= FLASH_CR_STRT;

	// Do a dummy read to compensate for bug in the flash-controller
	(void)FLASH->SR;

	// Wait for erase to be finished
	while(MCAL_IO_READ(FLASH->SR) & FLASH_SR_BSY);
    
	// Clear Page erase bit
	FLASH->CR = 0;	
}

#else
static void eraseArea(void)
{
	// Clear status bits
	FLASH->SR = FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR;

	// Set page erase bit
	FLASH->CR = FLASH_CR_PER;

	// Set address of page
	FLASH->AR = (uint32_t)CRASHD_FLASH_AREA_START;

	// Start erasing
	FLASH->CR = FLASH_CR_PER | FLASH_CR_STRT;

	// Do a dummy read to compensate for bug in the flash-controller
	(void)FLASH->SR;

	// Wait for erase to be finished
	while(MCAL_IO_READ(FLASH->SR) & FLASH_SR_BSY);
    
	// Clear Page erase bit
	FLASH->CR = 0;	
}
#endif


// Store dumplog to flash area
void CRASHD_store(uint32_t stackStart)
{
  // Check that CRASHD_init has been executed
  if(initialized == CRASHD_MAGIC_NUMBER)
  {
    // Get pointer to area
    uint32_t *dumpArea = (uint32_t *)CRASHD_FLASH_AREA_START;
    // Get size of area
    uint16_t length = 1024;

    // Enable access to flash writes
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;


    uint32_t crashCounter = 0;

    // Scan crashdump area
    for(uint32_t n = 0; n < length / 4; n++)
    {
      // If non-empty, crashdump area may already contain a crashdump
      if(dumpArea[n] != 0xffffffff) 
      {
        // Get stored crashCounter if magic number matches
        if(dumpArea[0] == CRASHD_MAGIC_NUMBER)
          crashCounter = dumpArea[1] + 1; // +1 for this crash
    
        // Erase area
        eraseArea();
        break;
      }
    }

    // Start writes at the beginning of dumpArea, 
    // flashWriterPos is updated with new position after each call to addToFlashArea
    uint16_t *flashWriterPos = (uint16_t *)dumpArea;
    uint32_t r = CRASHD_MAGIC_NUMBER; 

    // Write magic number
    addToFlashArea(&flashWriterPos, &r, sizeof(r));
    
    // Write crashCounter
    addToFlashArea(&flashWriterPos, &crashCounter, sizeof(crashCounter));

    // Write core registers
    for(uint32_t n = 0; n < (sizeof(core_regs) / sizeof(core_regs[0])); n++)
    {
      r = *core_regs[n];
      addToFlashArea(&flashWriterPos, &r, sizeof(r));
    }

    // Write Stack start
    addToFlashArea(&flashWriterPos, &stackStart, sizeof(stackStart));
    
    // Get end of stack
#if defined(__CC_ARM)
		uint32_t stackEnd = (uint32)Image$$SRAM$$ZI$$Limit;
#else
    uint32_t stackEnd = (uint32_t)&_estack;
#endif	 
    // Calculate length of stack
    uint32_t stackLength = (stackEnd - stackStart); 
   
    // Write Stack length
    addToFlashArea(&flashWriterPos, &stackLength, sizeof(stackLength));

    // Calculate number of remaining bytes in area
    uint16_t remaningBytesInArea = length - ((uint8_t *)flashWriterPos - (uint8_t *)dumpArea);

    // Clamp stack length to fit into crashdump area
    if(stackLength > remaningBytesInArea)
      stackLength = remaningBytesInArea;
      
    // Write Stack data
    addToFlashArea(&flashWriterPos, (void *)stackStart, stackLength);

  }
  // Lock further execution
  while(1) { }

}
#endif




// Initalize crashdump
void CRASHDUMP_init(void)
{
  // Indicate that crashdump initialization has been executed
  initialized = CRASHD_MAGIC_NUMBER;
}

/**
  * @}
  */
/**
  * @}
  */



