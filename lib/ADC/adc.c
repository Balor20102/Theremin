#include "adc.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t adc_value = 0;

void adc_init_freerun(void)
{
    // ADMUX: REFS0=1 (AVcc), ADLAR=1 (left adjust), MUX = PIN_POT_ADC_CH (assume 0)
    ADMUX = (1 << REFS0) | (1 << ADLAR) | (0 & 0x0F);

    // ADCSRA: ADEN=1, ADSC=1 (start), ADATE=1 (auto trigger), ADIE=1 (interrupt), prescaler = 128
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // ADCSRB: free running (ADTS = 000)
    ADCSRB = 0;
}

uint8_t adc_get_last(void)
{
    return adc_value;
}

ISR(ADC_vect)
{
    // ADLAR=1 => ADCH holds top 8 bits
    adc_value = ADCH;
    // application: write to OCR2B elsewhere (or directly here if desired)
}
