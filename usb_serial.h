#ifndef USB_SERIAL_H
#define USB_SERIAL_H
#include <stdint.h>

typedef void (*usb_serial_rx_cb_t)(void *ctx, uint8_t *data, size_t len);
void usb_serial_write(const uint8_t *data, size_t len);
void usb_serial_set_rx_cb(usb_serial_rx_cb_t cb, void *ctx);
void usb_serial_init(void);


#endif
