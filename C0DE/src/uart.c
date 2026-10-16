#include <avr/io.h>
#include <stdint.h>
#include "uart.h"

void uart_init(void)
{
    /* 16 MHz, U2X=1, UBRR=51 -> 38461.5 baudios. */
    UCSR0A = _BV(U2X0);
    UBRR0H = 0;
    UBRR0L = 51;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8N1 */
}

uint8_t uart_getc_nonblock(char *out)
{
    if (!(UCSR0A & _BV(RXC0))) return 0;
    *out = (char)UDR0;
    return 1;
}

void uart_putc(char c)
{
    while (!(UCSR0A & _BV(UDRE0))) { }
    UDR0 = (uint8_t)c;
}

void uart_puts(const char *s)
{
    while (*s) uart_putc(*s++);
}

void uart_put_frequency(uint16_t d_hz)
{
    /* Imprime 0.0 .. 50.0 sin printf y sin usar coma flotante. */
    uint16_t whole = d_hz / 10U;
    if (whole >= 10U) uart_putc((char)('0' + whole / 10U));
    uart_putc((char)('0' + whole % 10U));
    uart_putc('.');
    uart_putc((char)('0' + d_hz % 10U));
}
