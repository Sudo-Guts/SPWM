/*
 * =============================================================================
 *  Proyecto : Inversor Trifasico SPWM
 * 	Archivo  : C0DE/src/control.c
 * 	Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *
 *
 * =============================================================================
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "board.h"
#include "control.h"
#include "pwm.h"
#include "spwm.h"

static volatile uint16_t elapsed_ms;
static inverter_state_t state;
static uint16_t target_dhz;
static uint16_t actual_dhz;
static uint16_t last_update_ms;
static uint8_t precharge_ticks;

ISR(TIMER0_COMPA_vect)
{
    ++elapsed_ms;
}

void control_init(void)
{
    state = STATE_STOPPED;
    target_dhz = FREQ_START_DHZ;
    actual_dhz = 0;
    elapsed_ms = 0;
    last_update_ms = 0;
    precharge_ticks = 0;
    pwm_driver_enable(0);
    spwm_set(0, 0);

    /* Timer0 CTC, 1 kHz: 16 MHz / 64 / (249+1) = 1000 Hz. */
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS01) | _BV(CS00);
    OCR0A = 249;
    TCNT0 = 0;
    TIMSK0 = _BV(OCIE0A);
}

static uint8_t vf_modulation(uint16_t frequency_dhz)
{
    /* V/f lineal normalizado (NO calibrado para tension nominal del motor). */
    const uint32_t scaled = (uint32_t)frequency_dhz * VF_MAX_MOD;
    return (uint8_t)((scaled + FREQ_MAX_DHZ / 2U) / FREQ_MAX_DHZ);
}

void control_start(void)
{
    if (state != STATE_STOPPED)
        return;

    actual_dhz = 0;
    spwm_set(0, 0);

    precharge_ticks = (uint8_t)(BOOTSTRAP_SETTLE_MS / CONTROL_PERIOD_MS);

    state = STATE_PRECHARGE;

#if !SPWM_BENCH_TEST
    /* Modo normal: habilitar drivers IR2109. */
    pwm_driver_enable(1);
#endif
}

void control_stop(void)
{
    if (state == STATE_RUNNING)
        state = STATE_STOPPING;
    else if (state == STATE_PRECHARGE)
        control_cutoff();
}

void control_cutoff(void)
{
    /* LOW en SD apaga ambas salidas HO y LO de cada IR2109. */
    pwm_driver_enable(0);
    actual_dhz = 0;
    spwm_set(0, 0);
    state = STATE_STOPPED;
}

void control_adjust_target(int16_t delta_dhz)
{
    int32_t v = (int32_t)target_dhz + delta_dhz;
    if (v < 0)
        v = 0;
    if (v > (int32_t)FREQ_MAX_DHZ)
        v = (int32_t)FREQ_MAX_DHZ;
    target_dhz = (uint16_t)v;
}

inverter_state_t control_get_state(void)
{
    return state;
}
uint16_t control_get_target_dhz(void)
{
    return target_dhz;
}
uint16_t control_get_actual_dhz(void)
{
    return actual_dhz;
}

void control_poll(void)
{
    uint16_t now;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        now = elapsed_ms;
    }
    if ((uint16_t)(now - last_update_ms) < CONTROL_PERIOD_MS)
        return;
    last_update_ms = now;

    switch (state) {
        case STATE_STOPPED:
            return;
        case STATE_PRECHARGE:
            if (precharge_ticks)
                --precharge_ticks;
            if (!precharge_ticks)
                state = STATE_RUNNING;
            return;
        case STATE_RUNNING:
            if (actual_dhz < target_dhz) {
                ++actual_dhz;
            } else if (actual_dhz > target_dhz) {
                --actual_dhz;
            }
        break;
        case STATE_STOPPING:
            if (actual_dhz)
                --actual_dhz;
            if (!actual_dhz) {
                control_cutoff();
                return;
            }
        break;
    }

    spwm_set(actual_dhz, vf_modulation(actual_dhz));
}
