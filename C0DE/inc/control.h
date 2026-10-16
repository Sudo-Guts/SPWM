#ifndef CONTROL_H
#define CONTROL_H
#include <stdint.h>

typedef enum {
    STATE_STOPPED = 0,
    STATE_PRECHARGE,
    STATE_RUNNING,
    STATE_STOPPING
} inverter_state_t;

void control_init(void);
void control_poll(void);
void control_start(void);
void control_stop(void);
void control_cutoff(void);
void control_adjust_target(int16_t delta_dhz);
inverter_state_t control_get_state(void);
uint16_t control_get_target_dhz(void);
uint16_t control_get_actual_dhz(void);

#endif
