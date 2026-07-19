#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>
#include "actuator_logic.h"
#include "fan_actuator.h"

LOG_MODULE_REGISTER(iot_app, LOG_LEVEL_INF);

static const struct device *fan_dev = DEVICE_DT_GET(DT_NODELABEL(fan0));

// "Senzor": ADC kanal definisan u overlay-u
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

// Deljeno stanje, zaštićeno mutex-om
static struct k_mutex state_mutex;
static int32_t current_temp_c = 20;
static int32_t threshold_c = CONFIG_IOT_TEMP_THRESHOLD_C;
static bool fan_on;

// Sinhronizacija između thread-ova
K_SEM_DEFINE(sensor_ready_sem, 0, 1);

#define SENSOR_STACK_SIZE 1024
#define ACTUATOR_STACK_SIZE 1024
#define SENSOR_PRIORITY 5
#define ACTUATOR_PRIORITY 5

static void sensor_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    int16_t buf;
    struct adc_sequence sequence = {
        .buffer = &buf,
        .buffer_size = sizeof(buf),
    };
    adc_sequence_init_dt(&adc_channel, &sequence);

    int setup_ret = adc_channel_setup_dt(&adc_channel);
    LOG_INF("adc_channel_setup_dt ret=%d", setup_ret);

    static int32_t sim_mv = 500;
    static int8_t sim_dir = 1;

    while (1) {
        // Ubrizgavamo sledecu simuliranu vrednost napona u emulator
        sim_mv += sim_dir * 150;
        if (sim_mv >= 3000 || sim_mv <= 200) {
            sim_dir = -sim_dir;
        }
        int set_ret = adc_emul_const_value_set(adc_channel.dev, adc_channel.channel_id, sim_mv);

        int32_t val_mv = 0;
        int read_ret = adc_read_dt(&adc_channel, &sequence);
        if (read_ret == 0) {
            val_mv = buf;
            adc_raw_to_millivolts_dt(&adc_channel, &val_mv);
        }

        int32_t temp = 15 + (val_mv % 26);

        k_mutex_lock(&state_mutex, K_FOREVER);
        current_temp_c = temp;
        k_mutex_unlock(&state_mutex);

        LOG_INF("ADC raw=%d -> temp=%dC", val_mv, temp);

        k_sem_give(&sensor_ready_sem);
        k_msleep(2000);
    }
}

static void actuator_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    while (1) {
        k_sem_take(&sensor_ready_sem, K_FOREVER);

        int32_t temp, thr;
        k_mutex_lock(&state_mutex, K_FOREVER);
        temp = current_temp_c;
        thr = threshold_c;
        k_mutex_unlock(&state_mutex);

        bool should_be_on = decide_fan_state(temp, thr);

        if (should_be_on != fan_on) {
            fan_on = should_be_on;
            if (fan_on) {
                fan_actuator_on(fan_dev);
            } else {
                fan_actuator_off(fan_dev);
            }
            LOG_INF("Fan actuator readback: pin state = %d", fan_actuator_get_state(fan_dev));
        }
    }
}

K_THREAD_DEFINE(sensor_tid, SENSOR_STACK_SIZE, sensor_thread, NULL, NULL, NULL,
                 SENSOR_PRIORITY, 0, 0);
K_THREAD_DEFINE(actuator_tid, ACTUATOR_STACK_SIZE, actuator_thread, NULL, NULL, NULL,
                 ACTUATOR_PRIORITY, 0, 0);

// Shell komande: interaktivno menjanje praga i čitanje stanja
static int cmd_set_threshold(const struct shell *sh, size_t argc, char **argv)
{
    int32_t new_thr = atoi(argv[1]);
    k_mutex_lock(&state_mutex, K_FOREVER);
    threshold_c = new_thr;
    k_mutex_unlock(&state_mutex);
    shell_print(sh, "Novi prag postavljen na %d C", new_thr);
    return 0;
}

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
    k_mutex_lock(&state_mutex, K_FOREVER);
    shell_print(sh, "Temp=%dC Prag=%dC Ventilator=%s",
                current_temp_c, threshold_c, fan_on ? "ON" : "OFF");
    k_mutex_unlock(&state_mutex);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_iot,
    SHELL_CMD_ARG(set_threshold, NULL, "Postavi prag temperature <C>", cmd_set_threshold, 2, 0),
    SHELL_CMD(status, NULL, "Prikazi trenutno stanje", cmd_status),
    SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(iot, &sub_iot, "IoT demo komande", NULL);

int main(void)
{
    k_mutex_init(&state_mutex);
    LOG_INF("Zephyr IoT Demo pokrenut (native_sim)");
    return 0;
}