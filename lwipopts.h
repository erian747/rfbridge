/**
 * @file
 *
 * lwIP Options Configuration
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__
/*
#if defined(DEBUG)
  #define LWIP_DEBUG
#endif
*/
/*				
#define RAW_DEBUG				LWIP_DBG_ON
#define IP_DEBUG				LWIP_DBG_ON
#define IP6_DEBUG				  LWIP_DBG_ON
#define ICMP_DEBUG				LWIP_DBG_ON
#define NETIF_DEBUG				LWIP_DBG_ON
#define ETHARP_DEBUG				LWIP_DBG_ON
#define TCP_DEBUG				LWIP_DBG_ON
#define TCP_RST_DEBUG				LWIP_DBG_ON
#define TCP_DEBUG_PCB_LISTS			LWIP_DBG_ON
#define TCP_INPUT_DEBUG				LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG			LWIP_DBG_ON
#define TCPIP_DEBUG				LWIP_DBG_ON
#define UDP_DEBUG				LWIP_DBG_ON
#define PBUF_DEBUG				LWIP_DBG_ON
#define TCP_CWND_DEBUG				LWIP_DBG_ON
#define PPP_DEBUG LWIP_DBG_ON
*/
/*#define MQTT_DEBUG				LWIP_DBG_ON */

#define LWIP_DBG_MIN_LEVEL 0
#define LWIP_DBG_TYPES_ON			(LWIP_DBG_ON | LWIP_DBG_TRACE | LWIP_DBG_STATE)

#define LWIP_NETIF_LOOPBACK			0
#define LWIP_NETIF_API				0
#define LWIP_NETIF_HWADDRHINT 0
#define LWIP_NETIF_HOSTNAME  1
#define LWIP_NETIF_STATUS_CALLBACK 1



#define CHECKSUM_GEN_IP                 	1
#define CHECKSUM_GEN_UDP                	1
#define CHECKSUM_GEN_TCP                	1
#define CHECKSUM_CHECK_IP               	0
#define CHECKSUM_CHECK_UDP              	0
#define CHECKSUM_CHECK_TCP              	0


/* TCP options */
/* #define TCP_MSS				(536)
#define TCP_SND_BUF			(2*TCP_MSS)
#define TCP_SND_QUEUELEN	 (2 * TCP_SND_BUF/TCP_MSS)
#define TCP_WND				 (2*TCP_MSS)
*/
#define TCP_QUEUE_OOSEQ 0 /* Don't queue out of order segments */



/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
#include "lwip/debug.h"


#define MEM_LIBC_MALLOC                 0

/*
   -----------------------------------------------
   ---------- Platform specific porting ----------
   -----------------------------------------------
*/
#define LWIP_RAND() ((uint32_t)rand())

/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT            0

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                          1

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   4

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
/* #define MEM_SIZE                        (1 << 12) */


#define MEMCPY memcpy

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#define MEMP_NUM_PBUF                   8

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#define MEMP_NUM_RAW_PCB                4

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB                2

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB                2

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB_LISTEN         0

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_SEG                16

/**
 * MEMP_NUM_REASSDATA: the number of simultaneously IP packets queued for
 * reassembly (whole packets, not fragments!)
 */
#define MEMP_NUM_REASSDATA              1

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#define PBUF_POOL_SIZE                  8

/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
*/
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#define LWIP_ARP                        0


/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
#define LWIP_IPV4                       1

/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#define IP_FORWARD                      0

/**
 * IP_OPTIONS: Defines the behavior for IP options.
 *      IP_OPTIONS_ALLOWED==0: All packets with IP options are dropped.
 *      IP_OPTIONS_ALLOWED==1: IP options are allowed (but not parsed).
 */
#define IP_OPTIONS_ALLOWED              0

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#define IP_REASSEMBLY                   0

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#define IP_FRAG                         0

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#define IP_REASS_MAXAGE                 3

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#define IP_REASS_MAX_PBUFS              4

/**
 * IP_FRAG_USES_STATIC_BUF==1: Use a static MTU-sized buffer for IP
 * fragmentation. Otherwise pbufs are allocated and reference the original
    * packet data to be fragmented.
*/
#define IP_FRAG_USES_STATIC_BUF         0

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#define IP_DEFAULT_TTL                  255

/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#define LWIP_ICMP                       1

/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW                        0

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#define LWIP_DHCP                       0


/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#define LWIP_AUTOIP                     0

/*
   ----------------------------------
   ---------- SNMP options ----------
   ----------------------------------
*/
/**
 * LWIP_SNMP==1: Turn on SNMP module. UDP must be available for SNMP
 * transport.
 */
#define LWIP_SNMP                       0

/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#define LWIP_IGMP                       0

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#define LWIP_DNS                        0

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
/**
 * LWIP_UDP==1: Turn on UDP.
 */
#define LWIP_UDP                        1

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
/**
 * LWIP_TCP==1: Turn on TCP.
 */
#define LWIP_TCP                        1

#define LWIP_LISTEN_BACKLOG             0

/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
/**
 * PBUF_LINK_HLEN: the number of bytes that should be allocated for a
 * link level header. The default is 14, the standard value for
 * Ethernet.
 */
#define PBUF_LINK_HLEN                  0

/**
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. The default is
 * designed to accomodate single full size TCP frame in one pbuf, including
 * TCP_MSS, IP header, and link header.
*
 */

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1) and loopif.c
 */
#define LWIP_HAVE_LOOPIF                0

/*
   ------------------------------------
   ---------- SLIPIF options ----------
   ------------------------------------
*/
#define LWIP_HAVE_SLIPIF                0

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    0

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     0

#define LWIP_COMPAT_SOCKETS             0

/*
   ---------------------------------------
   ---------- IPv6 options ---------------
   ---------------------------------------
*/
/**
 * LWIP_IPV6==1: Enable IPv6
 */
#ifndef LWIP_IPV6
#define LWIP_IPV6                       0
#endif

/**
 * LWIP_IPV6_NUM_ADDRESSES: Number of IPv6 addresses per netif.
 */
#ifndef LWIP_IPV6_NUM_ADDRESSES
#define LWIP_IPV6_NUM_ADDRESSES         3
#endif

/**
 * LWIP_IPV6_FORWARD==1: Forward IPv6 packets across netifs
 */
#ifndef LWIP_IPV6_FORWARD
#define LWIP_IPV6_FORWARD               0
#endif

/**
 * LWIP_ICMP6==1: Enable ICMPv6 (mandatory per RFC)
 */
#ifndef LWIP_ICMP6
#define LWIP_ICMP6                      (LWIP_IPV6)
#endif

/**
 * LWIP_ICMP6_DATASIZE: bytes from original packet to send back in
 * ICMPv6 error messages.
 */
#ifndef LWIP_ICMP6_DATASIZE
#define LWIP_ICMP6_DATASIZE             8
#endif

/**
 * LWIP_ICMP6_HL: default hop limit for ICMPv6 messages
 */
#ifndef LWIP_ICMP6_HL
#define LWIP_ICMP6_HL                   255
#endif

/**
 * LWIP_ICMP6_CHECKSUM_CHECK==1: verify checksum on ICMPv6 packets
 */
#ifndef LWIP_ICMP6_CHECKSUM_CHECK
#define LWIP_ICMP6_CHECKSUM_CHECK       0
#endif

/**
 * LWIP_IPV6_MLD==1: Enable multicast listener discovery protocol.
 */
#ifndef LWIP_IPV6_MLD
#define LWIP_IPV6_MLD                   (0)
#endif

/**
 * MEMP_NUM_MLD6_GROUP: Max number of IPv6 multicast that can be joined.
 */
#ifndef MEMP_NUM_MLD6_GROUP
#define MEMP_NUM_MLD6_GROUP             2
#endif

/**
 * LWIP_IPV6_FRAG==1: Fragment outgoing IPv6 packets that are too big.
 */
#ifndef LWIP_IPV6_FRAG
#define LWIP_IPV6_FRAG                  0
#endif

/**
 * LWIP_IPV6_REASS==1: reassemble incoming IPv6 packets that fragmented
 */
#ifndef LWIP_IPV6_REASS
#define LWIP_IPV6_REASS                 (0)
#endif

/**
 * LWIP_ND6_QUEUEING==1: queue outgoing IPv6 packets while MAC address
 * is being resolved.
 */
#ifndef LWIP_ND6_QUEUEING
#define LWIP_ND6_QUEUEING               (0)
#endif

#define MEMP_NUM_SYS_TIMEOUT 4

/**
 * MEMP_NUM_ND6_QUEUE: Max number of IPv6 packets to queue during MAC resolution.
 */
#ifndef MEMP_NUM_ND6_QUEUE
#define MEMP_NUM_ND6_QUEUE              20
#endif

/**
 * LWIP_ND6_NUM_NEIGHBORS: Number of entries in IPv6 neighbor cache
 */
#ifndef LWIP_ND6_NUM_NEIGHBORS
#define LWIP_ND6_NUM_NEIGHBORS          10
#endif

/**
 * LWIP_ND6_NUM_DESTINATIONS: number of entries in IPv6 destination cache
 */
#ifndef LWIP_ND6_NUM_DESTINATIONS
#define LWIP_ND6_NUM_DESTINATIONS       10
#endif

/**
 * LWIP_ND6_NUM_PREFIXES: number of entries in IPv6 on-link prefixes cache
 */
#ifndef LWIP_ND6_NUM_PREFIXES
#define LWIP_ND6_NUM_PREFIXES           5
#endif

/**
 * LWIP_ND6_NUM_ROUTERS: number of entries in IPv6 default router cache
 */
#ifndef LWIP_ND6_NUM_ROUTERS
#define LWIP_ND6_NUM_ROUTERS            3
#endif

/**
 * LWIP_ND6_MAX_MULTICAST_SOLICIT: max number of multicast solicit messages to send
 * (neighbor solicit and router solicit)
 */
#ifndef LWIP_ND6_MAX_MULTICAST_SOLICIT
#define LWIP_ND6_MAX_MULTICAST_SOLICIT  3
#endif

/**
 * LWIP_ND6_MAX_UNICAST_SOLICIT: max number of unicast neighbor solicitation messages
 * to send during neighbor reachability detection.
 */
#ifndef LWIP_ND6_MAX_UNICAST_SOLICIT
#define LWIP_ND6_MAX_UNICAST_SOLICIT    0
#endif

/**
 * Unused: See ND RFC (time in milliseconds).
 */
#ifndef LWIP_ND6_MAX_ANYCAST_DELAY_TIME
#define LWIP_ND6_MAX_ANYCAST_DELAY_TIME 1000
#endif

/**
 * Unused: See ND RFC
 */
#ifndef LWIP_ND6_MAX_NEIGHBOR_ADVERTISEMENT
#define LWIP_ND6_MAX_NEIGHBOR_ADVERTISEMENT  3
#endif

/**
 * LWIP_ND6_REACHABLE_TIME: default neighbor reachable time (in milliseconds).
 * May be updated by router advertisement messages.
 */
#ifndef LWIP_ND6_REACHABLE_TIME
#define LWIP_ND6_REACHABLE_TIME         30000
#endif

/**
 * LWIP_ND6_RETRANS_TIMER: default retransmission timer for solicitation messages
 */
#ifndef LWIP_ND6_RETRANS_TIMER
#define LWIP_ND6_RETRANS_TIMER          1000
#endif

/**
 * LWIP_ND6_DELAY_FIRST_PROBE_TIME: Delay before first unicast neighbor solicitation
 * message is sent, during neighbor reachability detection.
 */
#ifndef LWIP_ND6_DELAY_FIRST_PROBE_TIME
#define LWIP_ND6_DELAY_FIRST_PROBE_TIME 5000
#endif

/**
 * LWIP_ND6_ALLOW_RA_UPDATES==1: Allow Router Advertisement messages to update
 * Reachable time and retransmission timers, and netif MTU.
 */
#ifndef LWIP_ND6_ALLOW_RA_UPDATES
#define LWIP_ND6_ALLOW_RA_UPDATES      0
#endif

/**
 * LWIP_IPV6_SEND_ROUTER_SOLICIT==1: Send router solicitation messages during
 * network startup.
 */
#ifndef LWIP_IPV6_SEND_ROUTER_SOLICIT
#define LWIP_IPV6_SEND_ROUTER_SOLICIT   0
#endif

/**
 * LWIP_ND6_TCP_REACHABILITY_HINTS==1: Allow TCP to provide Neighbor Discovery
 * with reachability hints for connected destinations. This helps avoid sending
 * unicast neighbor solicitation messages.
 */
#ifndef LWIP_ND6_TCP_REACHABILITY_HINTS
#define LWIP_ND6_TCP_REACHABILITY_HINTS 0
#endif

/**
 * LWIP_IPV6_AUTOCONFIG==1: Enable stateless address autoconfiguration as per RFC 4862.
 */
#ifndef LWIP_IPV6_AUTOCONFIG
#define LWIP_IPV6_AUTOCONFIG            (1)
#endif

/**
 * LWIP_IPV6_DUP_DETECT_ATTEMPTS: Number of duplicate address detection attempts.
 */
#ifndef LWIP_IPV6_DUP_DETECT_ATTEMPTS
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   0
#endif

/**
 * LWIP_IPV6_DHCP6==1: enable DHCPv6 stateful address autoconfiguration.
 */
#ifndef LWIP_IPV6_DHCP6
#define LWIP_IPV6_DHCP6                 0
#endif

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#define LWIP_STATS                      0
/*
   ---------------------------------
   ---------- PPP options ----------
   ---------------------------------
*/
/**
 * PPP_SUPPORT==1: Enable PPP.
 */
#define PPP_SUPPORT                     0
#define PPPOS_SUPPORT                   0
#define PPP_IPV4_SUPPORT                0
#define MD5_SUPPORT                     0
#define PAP_SUPPORT                     0
#define CHAP_SUPPORT                    0
/* Misc */

#define LWIP_TIMEVAL_PRIVATE 0

#endif /* __LWIPOPTS_H__ */
