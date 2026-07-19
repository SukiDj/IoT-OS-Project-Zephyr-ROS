#define DT_DRV_COMPAT vendor_fan_actuator

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/logging/log.h>
#include "fan_actuator.h"

LOG_MODULE_REGISTER(fan_actuator, LOG_LEVEL_INF);

struct fan_actuator_config {
    struct gpio_dt_spec gpio;
};

static int fan_actuator_init(const struct device *dev)
{
    const struct fan_actuator_config *cfg = dev->config;

    if (!gpio_is_ready_dt(&cfg->gpio)) {
        LOG_ERR("Fan actuator GPIO not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&cfg->gpio, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure fan actuator GPIO");
        return ret;
    }

    LOG_INF("Fan actuator driver initialized");
    return 0;
}

int fan_actuator_on(const struct device *dev)
{
    const struct fan_actuator_config *cfg = dev->config;
    return gpio_pin_set_dt(&cfg->gpio, 1);
}

int fan_actuator_off(const struct device *dev)
{
    const struct fan_actuator_config *cfg = dev->config;
    return gpio_pin_set_dt(&cfg->gpio, 0);
}

int fan_actuator_get_state(const struct device *dev)
{
    const struct fan_actuator_config *cfg = dev->config;

    // Ocitavamo stvarno stanje pina iz GPIO emulatora
    const struct device *port_dev = cfg->gpio.port;
    return gpio_emul_output_get(port_dev, cfg->gpio.pin);
}

#define FAN_ACTUATOR_INIT(inst)                                    \
    static const struct fan_actuator_config fan_actuator_cfg_##inst = { \
        .gpio = GPIO_DT_SPEC_INST_GET(inst, gpios),                \
    };                                                              \
    DEVICE_DT_INST_DEFINE(inst,                                     \
                          fan_actuator_init,                        \
                          NULL,                                     \
                          NULL,                                     \
                          &fan_actuator_cfg_##inst,                 \
                          POST_KERNEL,                               \
                          CONFIG_GPIO_INIT_PRIORITY,                \
                          NULL);

DT_INST_FOREACH_STATUS_OKAY(FAN_ACTUATOR_INIT)