#include "board.h"
#include "pwm.h"

void pwm_driver_enable(uint8_t enabled)
{
    if (enabled) {
        DRIVER_SD_PORT |= _BV(DRIVER_SD_BIT);
    } else {
        DRIVER_SD_PORT &= (uint8_t)~_BV(DRIVER_SD_BIT);
    }
}

void pwm_init(void)
{
    /* SD=0 antes de configurar como salida. Pull-down fisico obligatorio. */
    DRIVER_SD_PORT &= (uint8_t)~_BV(DRIVER_SD_BIT);
    DRIVER_SD_DDR |= _BV(DRIVER_SD_BIT);

    /* Iniciar OC1A, OC1B, OC2B como salida baja; el driver sigue apagado. */
    PORTB &= (uint8_t)~(_BV(PB1) | _BV(PB2));
    PORTD &= (uint8_t)~_BV(PD3);
    DDRB |= _BV(DDB1) | _BV(DDB2);
    DDRD |= _BV(DDD3);

    /* Sincronizar Timer1 y Timer2, incluido el prescaler separado de T2.
     * Timer2 AS2=0: usa el mismo reloj de CPU, no reloj asincrono. */
    GTCCR = _BV(TSM) | _BV(PSRSYNC) | _BV(PSRASY);
    ASSR &= (uint8_t)~_BV(AS2);

    TCCR1A = 0;
    TCCR1B = 0;
    TCCR2A = 0;
    TCCR2B = 0;

    /* Fast PWM 8-bit Timer1: modo 5 (WGM12=1, WGM10=1).
     * No inversor en los canales A y B. */
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
    TCCR1B = _BV(WGM12) | _BV(CS11); /* clk/8 */

    /* Fast PWM Timer2: modo 3 (WGM21=1, WGM20=1), OC2B no inversor. */
    TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS21);             /* clk/8 */

    OCR1A = PWM_MID;
    OCR1B = PWM_MID;
    OCR2B = PWM_MID;
    TCNT1 = 0;
    TCNT2 = 0;

    /* Un unico ISR para calcular/actualizar las tres referencias. */
    TIFR1 = _BV(TOV1);
    TIMSK1 = _BV(TOIE1);

    /* Liberar juntos ambos prescalers y contadores. */
    GTCCR = 0;
}
