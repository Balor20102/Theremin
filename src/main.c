#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "usart.h"
#include "ultrasonic.h"
#include "buzzer.h"
#include "adc.h"

// --- Configuratieconstanten ---
#define FREQ_MIN 230
#define FREQ_MAX 1400
#define MAX_DISTANCE_CM 65

int main(void)
{
    // Initialisaties
    USART_Init();
    Ultrasonic_Init();
    Buzzer_Init();
    ADC_Init();

    sei(); // interrupts aanzetten

    while (1)
    {
        // Trigger een afstandsmeting
        Ultrasonic_Trigger();
        _delay_ms(60);

        // Als de meting klaar is
        if (Ultrasonic_IsReady())
        {
            uint16_t distance = Ultrasonic_GetDistance();

            // Limiteer afstand
            if (distance > MAX_DISTANCE_CM)
            {
                distance = MAX_DISTANCE_CM;
            }

            // Bereken frequentie binnen bereik
            uint16_t frequency = FREQ_MAX - (distance * (FREQ_MAX - FREQ_MIN) / MAX_DISTANCE_CM);
            Buzzer_SetFrequency(frequency);

            // Lees volume (ADC potmeter)
            uint8_t volume = ADC_GetValue();
            OCR2B = volume; // PWM duty-cycle aanpassen voor volume

            // Toon waarden via USART
            char buffer[64];
            sprintf(buffer, "Afstand: %ucm | Freq: %uHz | Vol: %u\r\n",
                    distance, frequency, volume);

            for (char *p = buffer; *p; p++)
            {
                USART_Transmit(*p);
            }
        }
    }
}
