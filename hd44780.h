#ifndef HD44780_H
#define HD44780_H


extern void hd44780_init   (void);
extern void hd44780_clear      (void);
extern void hd44780_putchar    (char c);
extern void hd44780_set_cursor     (int column, int line);
extern void hd44780_print      (const char *string);
extern void hd44780_bargraph   (int value, int size);
extern void hd44780_bargraphXY (int pos_x, int pos_y, int value);

#endif
