#include <Arduino.h>
#include <unity.h>
#include "filter.h"

void setUp(void) {}
void tearDown(void) {}

void test_applyFilter(void)
{
    Filter_Init(5);
    uint16_t dataset[5] = {20, 21, 200, 22, 23};
    for (uint8_t i = 0; i < 5; i++)
        Filter_AddValue(dataset[i]);

    uint16_t result = Filter_GetMedian();
    TEST_ASSERT_EQUAL_UINT16(22, result);
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_applyFilter);
    UNITY_END();
}

void loop() {}
