#ifndef __LWIP_CC_H__
#define __LWIP_CC_H__

#define U16_F	"d"
#define S16_F	"d"
#define X16_F	"x"
#define U32_F	"d"
#define S32_F	"d"
#define X32_F	"x"
#define SZT_F 	"uz"

#ifndef BYTE_ORDER
  #define BYTE_ORDER LITTLE_ENDIAN
#endif

#define PACK_STRUCT_STRUCT __attribute__((packed))

#if !defined(__linux__) /* For hardware target build */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "trace.h"

#define s8_t int8_t
#define s16_t int16_t
#define s32_t int32_t
#define u8_t uint8_t
#define u16_t uint16_t
#define u32_t uint32_t

#define TTRACE_MAP(...) TTRACE_OUTPUT(TTRACE_INFO, __VA_ARGS__);                              
#define LWIP_PLATFORM_DIAG(x) TTRACE_MAP x
extern void lwip_assert(const char *message);

#define LWIP_PLATFORM_ASSERT(x) lwip_assert(x)

typedef unsigned int mem_ptr_t;

#else /* for Linux build */

/* Define generic types used in lwIP */
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   int     u32_t;
typedef signed     int     s32_t;

typedef unsigned long mem_ptr_t;

#include <string.h>
#include <sys/time.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)	do {printf x;} while(0)

#ifdef LWIP_UNIX_EMPTY_ASSERT
#define LWIP_PLATFORM_ASSERT(x)
#else
#define LWIP_PLATFORM_ASSERT(x) do {printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); fflush(NULL); abort();} while(0)
#endif


typedef unsigned long mem_ptr_t;
#endif

#define ENSROK                  (_SIGN 0) /* DNS server returned answer with no data */
#define ENSRNODATA              (_SIGN 160) /* DNS server returned answer with no data */
#define ENSRFORMERR             (_SIGN 161) /* DNS server claims query was misformatted */
#define ENSRSERVFAIL            (_SIGN 162) /* DNS server returned general failure */
#define ENSRNOTFOUND            (_SIGN 163) /* Domain name not found */
#define ENSRNOTIMP              (_SIGN 164) /* DNS server does not implement requested operation */
#define ENSRREFUSED             (_SIGN 165) /* DNS server refused query */
#define ENSRBADQUERY            (_SIGN 166) /* Misformatted DNS query */
#define ENSRBADNAME             (_SIGN 167) /* Misformatted domain name */
#define ENSRBADFAMILY           (_SIGN 168) /* Unsupported address family */
#define ENSRBADRESP             (_SIGN 169) /* Misformatted DNS reply */
#define ENSRCONNREFUSED         (_SIGN 170) /* Could not contact DNS servers */
#define ENSRTIMEOUT             (_SIGN 171) /* Timeout while contacting DNS servers */
#define ENSROF                  (_SIGN 172) /* End of file */
#define ENSRFILE                (_SIGN 173) /* Error reading file */
#define ENSRNOMEM               (_SIGN 174) /* Out of memory */
#define ENSRDESTRUCTION         (_SIGN 175) /* Application terminated lookup */
#define ENSRQUERYDOMAINTOOLONG  (_SIGN 176) /* Domain name is too long */
#define ENSRCNAMELOOP           (_SIGN 177) /* Domain name is too long */

struct sio_status_s;
typedef struct sio_status_s sio_status_t;
#define sio_fd_t sio_status_t*
#define __sio_fd_t_defined

#endif /* __LWIP_CC_H__ */
