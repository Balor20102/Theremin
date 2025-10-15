#include "buzzer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define BUZZER_PIN PD3

#define TIMER0_BASE_FREQ 31250UL

volatile bool buzzer_state = false;

ISR(TIMER0_COMPA_vect)
{
    if (buzzer_state)
    {
        TCCR2A &= ~(1 << COM2B1); // PWM uit
        buzzer_state = false;
    }
    else
    {
        TCCR2A |= (1 << COM2B1); // PWM aan
        buzzer_state = true;
    }
}

void Buzzer_Init(void)
{
    DDRD |= (1 << BUZZER_PIN);

    // Timer2 -> Fast PWM (volume)
    TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2B1);
    TCCR2B = (1 << CS22) | (1 << CS21); // prescaler 64
    OCR2B = 128;

    // Timer0 -> CTC (toon)
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS02);
    TIMSK0 = (1 << OCIE0A);

    OCR0A = 100; // standaardwaarde
}

void Buzzer_SetFrequency(uint16_t frequency)
{
    uint32_t half_ticks = TIMER0_BASE_FREQ / frequency;

    if (half_ticks < 2)
    {
        half_ticks = 2;
    }

    if (half_ticks > 255)
    {
        half_ticks = 255;
    }

    uint8_t ocr_value = (uint8_t)(half_ticks - 1);

    cli();
    OCR0A = ocr_value;
    sei();
}
