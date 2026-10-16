/*
 * =============================================================================
 *  Proyecto : Inversor Trifasico SPWM
 * 	Archivo  : C0DE/inc/board.h
 * 	Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Definicion de los pines de la placa
 *
 * =============================================================================
 */

#ifndef BOARD_H
#define BOARD_H

#include <avr/io.h>

#if !defined(F_CPU) || F_CPU != 16000000UL
#error "Esta version requiere F_CPU = 16000000UL y reloj AVR fisico de 16 MHz"
#endif

/* Tres entradas IN: OC1A/PB1, OC1B/PB2 y OC2B/PD3. */
/* Pin SD de los tres IR2109: PD4 -> HIGH=habilitado, LOW=apagado. */
/* Colocar pull-down externo de 10 kOhm de SD a COM. */
#define DRIVER_SD_DDR DDRD
#define DRIVER_SD_PORT PORTD
#define DRIVER_SD_BIT PD4

/* 16 MHz / 8 / 256 = 7812.5 Hz, Fast PWM de 8 bits. */
#define PWM_HZ_NUM 15625UL
#define PWM_HZ_DEN 2UL
#define PWM_MID 128U

#define FREQ_MAX_DHZ 500U   /* 50.0 Hz */
#define FREQ_START_DHZ 100U /* 10.0 Hz */
#define VF_MAX_MOD 230U     /* m ~= 0.90 en el punto maximo */
#define RAMP_STEP_DHZ 1U    /* 0.1 Hz cada 20 ms = 5 Hz/s */
#define CONTROL_PERIOD_MS 20U
#define BOOTSTRAP_SETTLE_MS 100U

#endif
