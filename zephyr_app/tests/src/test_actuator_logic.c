#include <zephyr/ztest.h>
#include "../../src/actuator_logic.h"

ZTEST_SUITE(actuator_logic_tests, NULL, NULL, NULL, NULL, NULL);

ZTEST(actuator_logic_tests, test_fan_on_above_threshold)
{
    bool result = decide_fan_state(35, 30);
    zassert_true(result, "Fan should turn ON when temp >= threshold");
}

ZTEST(actuator_logic_tests, test_fan_off_below_threshold)
{
    bool result = decide_fan_state(20, 30);
    zassert_false(result, "Fan should turn OFF when temp < threshold");
}

ZTEST(actuator_logic_tests, test_fan_on_at_exact_threshold)
{
    bool result = decide_fan_state(30, 30);
    zassert_true(result, "Fan should turn ON when temp == threshold");
}