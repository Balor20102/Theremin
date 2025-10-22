/*
 * main.c - Theremin Project (bare metal)
 * ---------------------------------------
 * Hardware:
 *   - Ultrasonic sensor: TRIG D9 (PB1), ECHO D8 (PB0, ICP1)
 *   - Buzzer: D3 (PD3 / OC2B)
 *   - Filter buttons: D4 (PD4), D5 (PD5)
 *   - Potmeter: A0 (ADC0)
 *   - LCD: via PCF8574 (I2C -> A4=SDA, A5=SCL, address 0x27)
 *   - 7-segment display: via tweede PCF8574 (address 0x20)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "usart.h"
#include "twi.h"
#include "hd44780pcf8574.h"
#include "ultrasonic.h"
#include "buzzer.h"
<<<<<<< HEAD
#include "adc.h"

    // --- Configuratieconstanten ---
    == == ==
    =
#include "filter.h"
#include "adc.h"

#define LCD_ADDR 0x27
#define SEG_ADDR 0x20
#define MAX_DISTANCE_CM 65
        >>>>>>> filters
#define FREQ_MIN 230
#define FREQ_MAX 1400

    // ---------- Prototypes ----------
    void
    Seg7_DisplayNumber(uint8_t number);
void FilterButtons_Init(void);
void FilterButtons_Check(void);

// ---------- Globale variabelen ----------
volatile uint8_t filterSize = 5;

// ---------- Hoofdprogramma ----------
int main(void)
{
<<<<<<< HEAD
    // Initialisaties
    USART_Init();
    Ultrasonic_Init();
    Buzzer_Init();
    == == == =
                 // Init alle hardwaremodules
        USART_Init();
    Ultrasonic_Init();
    Buzzer_Init();
    Filter_Init(filterSize);
>>>>>>> filters
    ADC_Init();
    TWI_Init();

<<<<<<< HEAD
    sei(); // interrupts aanzetten
    == == == =
                 HD44780_PCF8574_Init(LCD_ADDR);
    HD44780_PCF8574_DisplayOn(LCD_ADDR);
    HD44780_PCF8574_DisplayClear(LCD_ADDR);
    HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 0);
    HD44780_PCF8574_DrawString(LCD_ADDR, "Theremin Ready");

    FilterButtons_Init();
    sei();
>>>>>>> filters

    _delay_ms(1000);
    HD44780_PCF8574_DisplayClear(LCD_ADDR);

    while (1)
    {
<<<<<<< HEAD
        // Trigger een afstandsmeting
        == == == =
                     // ---- 1. Ultrasone meting ----
>>>>>>> filters
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

            // ---- 7. Knoppen controleren ----
            FilterButtons_Check();
        }

        _delay_ms(100);
    }
    return 0;
}

// ---------- Functie: Filterknoppen ----------
void FilterButtons_Init(void)
{
    DDRD &= ~((1 << PD4) | (1 << PD5)); // ingangen
    PORTD |= (1 << PD4) | (1 << PD5);   // interne pull-ups
}

void FilterButtons_Check(void)
{
    if (!(PIND & (1 << PD5)))
    {
        _delay_ms(20); // debounce
        if (!(PIND & (1 << PD5)))
        {
            uint8_t size = Filter_GetSize();
            if (size < 15)
            {
                Filter_SetSize(size + 1);
            }
            while (!(PIND & (1 << PD5)))
                ; // wacht tot losgelaten
        }
    }

    if (!(PIND & (1 << PD4)))
    {
        _delay_ms(20);
        if (!(PIND & (1 << PD4)))
        {
            uint8_t size = Filter_GetSize();
            if (size > 1)
            {
                Filter_SetSize(size - 1);
            }
            while (!(PIND & (1 << PD4)))
                ;
        }
    }
}
