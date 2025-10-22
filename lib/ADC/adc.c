#include "adc.h"
#include <avr/io.h>

void ADC_Init(void)
{
    ADMUX = (1 << REFS0);                               // AVcc referentie
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // prescaler 64
}

uint8_t ADC_GetValue(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
    {
        // wachten
    }
    return ADCH; // 8-bit waarde
}
