#include "actuator_logic.h"

bool decide_fan_state(int32_t temp, int32_t threshold)
{
    return temp >= threshold;
}