#include <Arduino.h>
#include <unity.h>
#include "adc.h"

void setUp(void) {}
void tearDown(void) {}

void test_readVolumeLevel(void)
{
    ADC_Init();
    uint8_t value = ADC_GetValue(); // verwacht 0–255
    TEST_ASSERT_TRUE(value <= 255);
    TEST_ASSERT_TRUE(value >= 0);
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_readVolumeLevel);
    UNITY_END();
}

void loop() {}
