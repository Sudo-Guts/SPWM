#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <avr/io.h>

void pwm_init(void);
void pwm_driver_enable(uint8_t enabled);

/* Solo para usar dentro de ISR(TIMER1_OVF_vect):
 * los tres registros son double-buffered en Fast PWM. */
static inline void pwm_write_uvw(uint8_t u, uint8_t v, uint8_t w)
{
    OCR1A = u;
    OCR1B = v;
    OCR2B = w;
}

#endif
