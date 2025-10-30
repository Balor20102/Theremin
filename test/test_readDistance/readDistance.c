#include <Arduino.h>
#include <unity.h>
#include "ultrasonic.h"

void setUp(void) {}
void tearDown(void) {}

void test_readDistance(void)
{
    // Simuleer een afstandsmeting
    uint16_t measured = Ultrasonic_GetDistance(); // verwacht echte functie
    TEST_ASSERT_UINT16_WITHIN(1, 10, measured);   // verwacht ~10 cm
}

void setup()
{
    delay(2000); // Wacht op seriële verbinding
    UNITY_BEGIN();
    RUN_TEST(test_readDistance);
    UNITY_END();
}

void loop() {}
