#ifndef FAN_ACTUATOR_H
#define FAN_ACTUATOR_H

#include <zephyr/device.h>

int fan_actuator_on(const struct device *dev);
int fan_actuator_off(const struct device *dev);
int fan_actuator_get_state(const struct device *dev);

#endif