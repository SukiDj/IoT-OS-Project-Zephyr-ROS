#ifndef ACTUATOR_LOGIC_H
#define ACTUATOR_LOGIC_H

#include <stdbool.h>
#include <stdint.h>

bool decide_fan_state(int32_t temp, int32_t threshold);

#endif