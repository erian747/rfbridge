/** @file crashdump.h
  * @author EA
  * @brief  SW crash handler, stores crash data to flash
  */


/** @addtogroup Application
  * @{
  */


/** @defgroup CrashDump CrashDump
  * @brief Crashdump writes are generated either by an assert, see bpm_assert.h or by a fault detected by the Cortex-M3 core
  *  
  *  Crashdump memory layout:
  *
  *  u32 magicNumber, see CRASHD_MAGIC_NUMBER
  *  u32 crashcounter, number of crashes detected
  *  u32 scb.icsr, VECTACTIVE field indicates if this crash was caused by an ASSERT(2) or HardFault(3)
  *  u32 scb.hfsr
  *  u32 scb.cfsr
  *  u32 scb.mmfar
  *  u32 scb.bfar
  *  u32 scb.shcsr
  *  u32 stack_start
  *  u32 stack_length
  *  u32 stackdata[stack_length]
  *  {
  *    u32 r4, r5, r6, r7, r8, r9, r10, r11, r0, r1, r2, r3, r12, lr, pc, psr... other pushed stackdata
  *  }
  * @{
  */

#ifndef CRASHDUMP_H
#define CRASHDUMP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



/**
  * @brief  Initialize CrashDump
  */
void CRASHDUMP_init(void);



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
