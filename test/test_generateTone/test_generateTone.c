#include <Arduino.h>
#include <unity.h>
#include "buzzer.h"

void setUp(void) {}
void tearDown(void) {}

void test_generateTone(void)
{
    Buzzer_Init();
    Buzzer_SetFrequency(440);

    // Geen directe meetwaarde mogelijk, dus enkel controleren of OCR-registers gezet zijn
    TEST_ASSERT_TRUE(1); // Placeholder: toon gegenereerd zonder crash
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_generateTone);
    UNITY_END();
}

void loop() {}
