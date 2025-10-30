#include <Arduino.h>
#include <unity.h>
#include "pitch.h"

void setUp(void) {}
void tearDown(void) {}

void test_calculatePitch(void)
{
    float distance = 35.0;
    float freq = calculatePitch(distance);
    TEST_ASSERT_FLOAT_WITHIN(freq * 0.05, 700.0, freq);
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_calculatePitch);
    UNITY_END();
}

void loop() {}
