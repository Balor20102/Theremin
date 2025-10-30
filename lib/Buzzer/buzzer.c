

#ifdef __AVR__
#include <avr/io.h>
#endif
#include "buzzer.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static uint16_t currentFreq = 0;
static uint8_t currentVolume = 0;

void Buzzer_Init(void)
{
    DDRD |= (1 << PD3); // OC2B output

    // Timer2 instellen op CTC-mode (OCR2A als TOP)
    // COM2B0 = toggle OC2B on match
    // WGM21 = 1 (CTC), WGM22 = 1 (OCR2A top)
    TCCR2A = (1 << COM2B0) | (1 << WGM21);
    TCCR2B = (1 << WGM22) | (1 << CS22); // prescaler 64
}

void Buzzer_SetFrequency(uint16_t freq)
{
    if (freq < 20)
        freq = 20;
    if (freq > 4000)
        freq = 4000;

    // f = F_CPU / (2 * prescaler * (1 + OCR2A))
    // => OCR2A = (F_CPU / (2 * prescaler * f)) - 1
    uint32_t top = (F_CPU / (2UL * 64UL * freq)) - 1;
    if (top > 255)
        top = 255;

    OCR2A = (uint8_t)top;
    currentFreq = freq;
}

void Buzzer_Update(uint16_t targetFreq)
{
    // vloeiend glijden naar nieuwe toonhoogte
    if (targetFreq > currentFreq)
        currentFreq += (targetFreq - currentFreq) / 3;
    else
        currentFreq -= (currentFreq - targetFreq) / 3;

    Buzzer_SetFrequency(currentFreq);
}

void Buzzer_SetVolume(uint8_t vol)
{
    currentVolume = vol;
    // Softwarematig: geen echte amplitudecontrole
}

uint8_t Buzzer_GetVolume(void)
{
    return currentVolume;
}
