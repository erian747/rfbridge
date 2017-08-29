#ifndef BLF_CONFIG
#define BLF_CONFIG

#include <stdint.h>
#include "mcal_config.h"

// Enable debug asserts
#define BLF_DEBUG 1

#define BLF_PORT_CORTEXM 1
#define BLF_CORTEXM_SYSFREQ MCU_SYSFREQ

// Timer usage
#define BLF_USE_TIMERS 1

// Use callback timers
#define BLF_USE_CB_TIMERS 1

// Use threads
#define BLF_USE_THREADS 1
#define BLF_USE_SEMAPHORES 1

#define BLF_THREAD_PRIORITIES 3
#define BLF_MAX_NR_THREADS 4

#define BLF_MAX_NR_SEMAPHORES_PER_PRIORITY 8

// Delegates
#define BLF_USE_DELEGATES 0

// Available memory for framework in bytes
#define BLF_STATHEAP_SIZE 6144 //1536

// Maximum size of a memory pool buffer
enum
{
  BLF_POOL_BUF_MAX_SIZE_4   = 1,
  BLF_POOL_BUF_MAX_SIZE_8   = 2,
  BLF_POOL_BUF_MAX_SIZE_16  = 3,
  BLF_POOL_BUF_MAX_SIZE_32  = 4,
  BLF_POOL_BUF_MAX_SIZE_64  = 5,
  BLF_POOL_BUF_MAX_SIZE_128 = 6,
  BLF_POOL_BUF_MAX_SIZE_256 = 7,
  BLF_POOL_BUF_MAX_SIZE_512  = 8,
  BLF_POOL_BUF_MAX_SIZE_1024 = 9,
  BLF_POOL_BUF_MAX_SIZE_2048 = 10,
};
// Set the maximum buffer size
#define BLF_NR_MEMORY_POOLS BLF_POOL_BUF_MAX_SIZE_1024

//#define BLF_ASSERT(expr) if(!(expr)) { *(volatile uint32_t *)0xE000ED04 = 0x80000000; };

#endif

