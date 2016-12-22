#ifndef NETWORK_H
#define NETWORK_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdint.h>

void network_init(void);
struct netif *network_netif(void);
#ifdef	__cplusplus
}
#endif

#endif /* NETWORK_H */
