/*
 * =============================================================================
 *  Proyecto : Inversor Trifasico SPWM
 * 	Archivo  : C0DE/src/main.c
 * 	Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *
 *
 * =============================================================================
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "pwm.h"
#include "spwm.h"
#include "control.h"
#include "uart.h"

#ifndef SPWM_BENCH_TEST
#define SPWM_BENCH_TEST 0
#endif

#if !SPWM_BENCH_TEST
static void print_status(void)
{
    uart_puts("\r\nEstado: ");

    switch (control_get_state()) {
        case STATE_STOPPED:
            uart_puts("STOPPED");
        break;

        case STATE_PRECHARGE:
            uart_puts("PRECHARGE");
        break;

        case STATE_RUNNING:
            uart_puts("RUNNING");
        break;

        case STATE_STOPPING:
            uart_puts("STOPPING");
        break;
    }

    uart_puts("  actual=");
    uart_put_frequency(control_get_actual_dhz());
    uart_puts(" Hz  objetivo=");
    uart_put_frequency(control_get_target_dhz());
    uart_puts(" Hz\r\n");
}
#endif

int main(void)
{
    cli();

    spwm_init();
    pwm_init();

#if !SPWM_BENCH_TEST
    uart_init();
#endif

    control_init();

    sei();

#if SPWM_BENCH_TEST
    /* Frecuencia objetivo: 10.0 Hz = 100 dHz. */
    const int16_t delta_dhz = (int16_t)100 - (int16_t)control_get_target_dhz();

    control_adjust_target(delta_dhz);
    control_start();
#endif

    while (1) {
        control_poll();

#if !SPWM_BENCH_TEST
        char c;

        if (!uart_getc_nonblock(&c))
            continue;

        switch (c) {
            case 'r':
                control_start();
            break;

            case 's':
                control_stop();
            break;

            case 'x':
                control_cutoff();
            break;

            case '+':
                control_adjust_target(10);
            break;

            case '-':
                control_adjust_target(-10);
            break;

            case '?':
                print_status();
            break;

            default:
            break;
        }
#endif
    }
}
