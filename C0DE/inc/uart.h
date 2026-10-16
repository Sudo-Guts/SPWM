#ifndef UART_H
#define UART_H
#include <stdint.h>
void uart_init(void);
uint8_t uart_getc_nonblock(char *out);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_put_frequency(uint16_t d_hz);
#endif
