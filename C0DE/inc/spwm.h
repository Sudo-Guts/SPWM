#ifndef SPWM_H
#define SPWM_H

#include <stdint.h>

void spwm_init(void);
/* Frecuencia en decimas de Hz (100 = 10.0 Hz), modulacion 0..230. */
void spwm_set(uint16_t freq_dhz, uint8_t modulation);

#endif
