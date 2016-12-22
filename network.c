#define MODULE_NAME NETWORK
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcal.h"
#include "trace.h"
#include "blf.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/igmp.h"
#include "lwip/ip_addr.h"
#include "config.h"
#include "crashdump.h"
//#include "ethernetif.h"
#include "ethernet.h"
//#include "svc_telnet.h"
#include "netif/ppp/pppos.h"
#include "slip.h"
#include "bsp.h"

static BLFCbTimer lwipTimer;

void lwip_assert(const char *message)
{
#ifdef DEBUG
  MCAL_LOCK_IRQ();
  __asm volatile ("bkpt 0");
#else
  //CRASHDUMP_INVOKE();
#endif
  while(1);
}

uint32_t sys_now(void)
{
  return BLF_ticks();
}

u32_t sys_jiffies(void)
{
  return BLF_ticks();
}

//---------------------------------------------------------------------------------
// PPP client interface
#if PPP_SUPPORT

static u32_t ppp_output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx) {
  return len; //uart_write(UART, data, len);
}

static void ppp_status_cb(ppp_pcb *pcb, int err_code, void *ctx) {
  struct netif *pppif = ppp_netif(pcb);
  LWIP_UNUSED_ARG(ctx);

  switch(err_code) {
    case PPPERR_NONE: {
#if LWIP_DNS
      ip_addr_t ns;
#endif /* LWIP_DNS */
      TTRACE(TTRACE_INFO, "status_cb: Connected\n");
#if PPP_IPV4_SUPPORT
      TTRACE(TTRACE_INFO, "   our_ipaddr  = %s\n", ipaddr_ntoa(&pppif->ip_addr));
      TTRACE(TTRACE_INFO, "   his_ipaddr  = %s\n", ipaddr_ntoa(&pppif->gw));
      TTRACE(TTRACE_INFO, "   netmask     = %s\n", ipaddr_ntoa(&pppif->netmask));
#if LWIP_DNS
      ns = dns_getserver(0);
      TTRACE(TTRACE_INFO, "   dns1        = %s\n", ipaddr_ntoa(&ns));
      ns = dns_getserver(1);
      TTRACE(TTRACE_INFO, "   dns2        = %s\n", ipaddr_ntoa(&ns));
#endif /* LWIP_DNS */
#endif /* PPP_IPV4_SUPPORT */
#if PPP_IPV6_SUPPORT
      TTRACE(TTRACE_INFO, "   our6_ipaddr = %s\n", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
      break;
    }
    case PPPERR_PARAM: {
      TTRACE(TTRACE_INFO, "status_cb: Invalid parameter\n");
      break;
    }
    case PPPERR_OPEN: {
      TTRACE(TTRACE_INFO, "status_cb: Unable to open PPP session\n");
      break;
    }
    case PPPERR_DEVICE: {
      TTRACE(TTRACE_INFO, "status_cb: Invalid I/O device for PPP\n");
      break;
    }
    case PPPERR_ALLOC: {
      TTRACE(TTRACE_INFO, "status_cb: Unable to allocate resources\n");
      break;
    }
    case PPPERR_USER: {
      TTRACE(TTRACE_INFO, "status_cb: User interrupt\n");
      break;
    }
    case PPPERR_CONNECT: {
      TTRACE(TTRACE_INFO, "status_cb: Connection lost\n");
      break;
    }
    case PPPERR_AUTHFAIL: {
      TTRACE(TTRACE_INFO, "status_cb: Failed authentication challenge\n");
      break;
    }
    case PPPERR_PROTOCOL: {
      TTRACE(TTRACE_INFO, "status_cb: Failed to meet protocol\n");
      break;
    }
    case PPPERR_PEERDEAD: {
      TTRACE(TTRACE_INFO, "status_cb: Connection timeout\n");
      break;
    }
    case PPPERR_IDLETIMEOUT: {
      TTRACE(TTRACE_INFO, "status_cb: Idle Timeout\n");
      break;
    }
    case PPPERR_CONNECTTIME: {
      TTRACE(TTRACE_INFO, "status_cb: Max connect time reached\n");
      break;
    }
    case PPPERR_LOOPBACK: {
      TTRACE(TTRACE_INFO, "status_cb: Loopback detected\n");
      break;
    }
    default: {
      TTRACE(TTRACE_INFO, "status_cb: Unknown error code %d\n", err_code);
      break;
    }
  }

/*
 * This should be in the switch case, this is put outside of the switch
 * case for example readability.
 */

  if (err_code == PPPERR_NONE) {
    return;
  }

  /* ppp_close() was previously called, don't reconnect */
  if (err_code == PPPERR_USER) {
    /* ppp_free(); -- can be called here */
    return;
  }

  /*
   * Try to reconnect in 30 seconds, if you need a modem chatscript you have
   * to do a much better signaling here ;-)
   */
  ppp_connect(pcb, 30);
  /* OR ppp_listen(pcb); */
}

struct netif pppif;

static void ppp_ifc_init(void)
{
  ppp_pcb * pcb = pppos_create(&pppif, ppp_output_cb,
  ppp_status_cb, 0);
}

#endif
//---------------------------------------------------------------
// SLIP
// Usage in linux
// sudo slattach -d -p slip -s 115200 /dev/ttyUSB0
// sudo ifconfig sl0 192.168.4.1 pointopoint 192.168.4.2
// sudo sysctl -w net.ipv4.ip_forward=1
// sudo iptables -A FORWARD -i slo -o wlp2s0b1 -j ACCEPT
// sudo iptables -t nat -A POSTROUTING -o wlp2s0b1 -j MASQUERADE

struct netif slipif;

extern err_t slipif_init(struct netif *netif, int baudRate);

static err_t slip_init_fn(struct netif *netif)
{
  slipif_init(netif, 115200);
  return ERR_OK;
}

static void slip_ifc_init(void)
{
  usart_if_t *uart = USART_create(0);
  slip_if_t *slip_driver = SLIP_create(uart);
  ip_addr_t ipaddr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  IP_ADDR4(&ipaddr, 192,168,4,2);
  IP_ADDR4(&netmask, 255,255,255,0);
  IP_ADDR4(&gw, 192,168,4,1);
  netif_add(&slipif, &ipaddr, &netmask, &gw, slip_driver, slip_init_fn, ip_input);

  netif_set_default(&slipif);
  // Bring up interface
  netif_set_up(&slipif);
}
/*
static const struct mdns_service services[] = {
    {
        .name = "\x06_iperf\x04_tcp\x05local",
        .port = IPERF_SERVER_PORT,
    },
    {
        .name = "\x05_echo\x04_tcp\x05local",
        .port = 7,
    },
    {
        .name = "\x07_telnet\x04_tcp\x05local",
        .port = 23,
    },
};

static const char *txt_records[] = {
    "product=LWIP Example",
    "version=0.0.1",
    NULL
};
*/
static uint8_t heartbeat_led_stat;

static void lwip_timer_cb(BLFCbTimer *timer, void *ctx)
{
  // Execute LWIP timers
  sys_check_timeouts();
  // Restart timer
  BLF_startCbTimer(&lwipTimer, lwip_timer_cb, NULL, 100);

  GPIO_write(BSP_LED_KEEPALIVE, heartbeat_led_stat & 2);
  heartbeat_led_stat++;
}

static struct netif netif;

struct netif *network_netif(void)
{
  return &netif;
};
static void netif_status(struct netif *n)
{
    if (n->flags & NETIF_FLAG_UP) {
        TTRACE(TTRACE_INFO,"Interface Up\n");
        char ip_buf[16];
        ipaddr_ntoa_r(&n->ip_addr, ip_buf, sizeof(ip_buf));
        TTRACE(TTRACE_INFO,"IP Address: %s\n", ip_buf);
        ipaddr_ntoa_r(&n->netmask, ip_buf, sizeof(ip_buf));
        TTRACE(TTRACE_INFO,"Net Mask:   %s\n", ip_buf);
        ipaddr_ntoa_r(&n->gw, ip_buf, sizeof(ip_buf));
        TTRACE(TTRACE_INFO,"Gateway:    %s\n", ip_buf);
/*
        const char *speed = "10Mb/s";
        if (ETH->MACCR & ETH_MACCR_FES)
            speed = "100Mb/s";

        const char *duplex = "Half";
        if (ETH->MACCR & ETH_MACCR_DM)
            duplex = "Full";

        TTRACE(TTRACE_INFO, "  Mode:       %s  %s Duplex\n", speed, duplex);
*/
    } else {
        TTRACE(TTRACE_INFO, "Interface Down.\n");
    }
}


void network_init(void)
{

  // Init lwip
  lwip_init();

  // Setup and start timer for lwip
  BLF_initCbTimer(&lwipTimer);
  BLF_startCbTimer(&lwipTimer, lwip_timer_cb, NULL, 100);

  //ppp_ifc_init();
  slip_ifc_init();
/*
  if((config_data.ip_opt & CONFIG_DATA_IP_OPT_MANUL_ASSIGN_IP) == 0)
  {
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
  }
  else
  {
    ip4_addr_copy(ipaddr, config_data.ip);
    ip4_addr_copy(netmask, config_data.mask);
    ip4_addr_copy(gw, config_data.gw);
  }


  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  netif.hostname ="LP340";
*/
  /*  Registers the default network interface.*/
  //netif_set_default(&netif);

  /*  When the netif is fully configured this function must be called.*/
  //netif_set_up(&netif);
  //netif_set_status_callback(&netif, netif_status);
/*
  igmp_start(&netif);

  err_t res = mdns_responder_init(&netif, services, sizeof(services) / sizeof(*services),
                    txt_records);

  if(res != ERR_OK)
    TTRACE(TTRACE_INFO, "Error initializing mdns responder.\n");
*/
//  if((config_data.ip_opt & CONFIG_DATA_IP_OPT_MANUL_ASSIGN_IP) == 0)
//    dhcp_start(&netif);

//  telnet_start(23);


}

