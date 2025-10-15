#include "ultrasonic.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#define TRIG_PIN PB1
#define ECHO_PIN PB0

#define TIMER1_TICK_US 0.5f
#define SOUND_SPEED_DIVISOR 58.0f

volatile uint16_t icr_start = 0;
volatile uint32_t echo_ticks = 0;
volatile bool measurement_ready = false;

ISR(TIMER1_CAPT_vect)
{
    uint16_t captured_value = ICR1;

    if (TCCR1B & (1 << ICES1))
    {
        icr_start = captured_value;
        TCCR1B &= ~(1 << ICES1); // volgende op dalende flank
    }
    else
    {
        uint16_t delta;
        if (captured_value >= icr_start)
        {
            delta = captured_value - icr_start;
        }
        else
        {
            delta = (0xFFFF - icr_start + captured_value + 1);
        }

        echo_ticks = delta;
        measurement_ready = true;
        TCCR1B |= (1 << ICES1); // terug naar stijgende flank
    }
}

void Ultrasonic_Init(void)
{
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    PORTB &= ~(1 << TRIG_PIN);

    TCCR1A = 0;
    TCCR1B = (1 << CS11) | (1 << ICES1);
    TIMSK1 = (1 << ICIE1);
}

void Ultrasonic_Trigger(void)
{
    PORTB &= ~(1 << TRIG_PIN);
    _delay_us(2);
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);
}

bool Ultrasonic_IsReady(void)
{
    return measurement_ready;
}

uint16_t Ultrasonic_GetDistance(void)
{
    cli();
    uint32_t ticks = echo_ticks;
    measurement_ready = false;
    sei();

    float duration_us = ticks * TIMER1_TICK_US;
    uint16_t distance_cm = (uint16_t)(duration_us / SOUND_SPEED_DIVISOR + TIMER1_TICK_US);
    return distance_cm;
}
