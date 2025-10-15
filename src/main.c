#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "usart.h"
#include "ultrasonic.h"
#include "buzzer.h"

#include "adc.h"

#define FREQ_MIN 230
#define FREQ_MAX 1400
#define MAX_DISTANCE_CM 65

int main(void)
{
    USART_Init();
    Ultrasonic_Init();
    Buzzer_Init();
    Filter_Init(5);
    ADC_Init();

    sei();

    while (1)
    {
        Ultrasonic_Trigger();
        _delay_ms(60);

        if (Ultrasonic_IsReady())
        {
            uint16_t distance = Ultrasonic_GetDistance();
            Filter_AddValue(distance);
            uint16_t filtered = Filter_GetMedian();

            if (filtered > MAX_DISTANCE_CM)
            {
                filtered = MAX_DISTANCE_CM;
            }

            uint16_t frequency = FREQ_MAX - (filtered * (FREQ_MAX - FREQ_MIN) / MAX_DISTANCE_CM);
            Buzzer_SetFrequency(frequency);

            uint8_t volume = ADC_GetValue();
            OCR2B = volume; // volume instellen

            char buffer[64];
            sprintf(buffer, "Afstand: %ucm | Freq: %uHz | Vol: %u\r\n",
                    filtered, frequency, volume);
            for (char *p = buffer; *p; p++)
            {
                USART_Transmit(*p);
            }
        }
    }
}
