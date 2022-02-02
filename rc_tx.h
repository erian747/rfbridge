#ifndef PLC_TX_H
#define PLC_TX_H

int rc_tx_raw(const char *str);
void rc_tx_init(void);
void rc_tx_poll(void);
#endif
