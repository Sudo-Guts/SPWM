#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "board.h"
#include "pwm.h"
#include "spwm.h"
#include "sine_lut.h"

/* DDS de 32 bits:
 * 0.1 Hz / 7812.5 Hz * 2^32 ~= 54975.5814 -> 54976.
 * Error a 50 Hz de aprox. 0.00038 Hz.
 * Las sumas 0x55555555 y 0xAAAAAAAA equivalen a 120 y 240 grados.
 */
#define DDS_STEP_PER_DHZ 54976UL
#define SHIFT_120 0x55555555UL
#define SHIFT_240 0xAAAAAAAAUL

static volatile uint32_t phase_step;
static volatile uint8_t amplitude;
static uint32_t phase_acc;

void spwm_init(void)
{
    phase_acc = 0;
    spwm_set(0, 0);
}

void spwm_set(uint16_t freq_dhz, uint8_t modulation)
{
    if (freq_dhz > FREQ_MAX_DHZ) freq_dhz = FREQ_MAX_DHZ;
    if (modulation > VF_MAX_MOD) modulation = VF_MAX_MOD;

    const uint32_t step = (uint32_t)freq_dhz * DDS_STEP_PER_DHZ;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        phase_step = step;
        amplitude = modulation;
    }
}

static inline uint8_t scale_sample(uint8_t index, uint8_t amp)
{
    const int16_t centered = (int16_t)pgm_read_byte(&sine_lut[index]) - 128;
    const int16_t scaled = (int16_t)((centered * (int16_t)amp) / 256);
    return (uint8_t)(128 + scaled);
}

ISR(TIMER1_OVF_vect)
{
    const uint32_t p = phase_acc;
    const uint8_t amp = amplitude;

    const uint8_t u = scale_sample((uint8_t)(p >> 24), amp);
    const uint8_t v = scale_sample((uint8_t)((p + SHIFT_240) >> 24), amp);
    const uint8_t w = scale_sample((uint8_t)((p + SHIFT_120) >> 24), amp);

    pwm_write_uvw(u, v, w);
    phase_acc = p + phase_step;
}
