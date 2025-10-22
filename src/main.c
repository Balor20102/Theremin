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
#include "filter.h"
#include "adc.h"

#define LCD_ADDR 0x27
#define SEG_ADDR 0x20
#define MAX_DISTANCE_CM 65
#define FREQ_MIN 230
#define FREQ_MAX 1400

// ---------- Prototypes ----------
void Seg7_DisplayNumber(uint8_t number);
void FilterButtons_Init(void);
void FilterButtons_Check(void);

// ---------- Globale variabelen ----------
volatile uint8_t filterSize = 5;

// ---------- Hoofdprogramma ----------
int main(void)
{
    // Init alle hardwaremodules
    USART_Init();
    Ultrasonic_Init();
    Buzzer_Init();
    Filter_Init(filterSize);
    ADC_Init();
    TWI_Init();

    HD44780_PCF8574_Init(LCD_ADDR);
    HD44780_PCF8574_DisplayOn(LCD_ADDR);
    HD44780_PCF8574_DisplayClear(LCD_ADDR);
    HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 0);
    HD44780_PCF8574_DrawString(LCD_ADDR, "Theremin Ready");

    FilterButtons_Init();
    sei();

    _delay_ms(1000);
    HD44780_PCF8574_DisplayClear(LCD_ADDR);

    while (1)
    {
        // ---- 1. Ultrasone meting ----
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

            // ---- 2. Frequentie berekenen ----
            uint16_t freq = FREQ_MAX - ((filtered * (FREQ_MAX - FREQ_MIN)) / MAX_DISTANCE_CM);
            Buzzer_SetFrequency(freq);

            // ---- 3. Volume uitlezen ----
            uint8_t volume = ADC_GetValue();
            OCR2B = volume;

            // ---- 4. LCD bijwerken ----
            HD44780_PCF8574_DisplayClear(LCD_ADDR);
            HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 0);

            char line1[16];
            sprintf(line1, "Dist:%3ucm", filtered);
            HD44780_PCF8574_DrawString(LCD_ADDR, line1);

            HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 1);
            char line2[16];
            sprintf(line2, "Freq:%4uHz", freq);
            HD44780_PCF8574_DrawString(LCD_ADDR, line2);

            // ---- 5. 7-seg display filtergrootte ----
            Seg7_DisplayNumber(Filter_GetSize());

            // ---- 6. Debug over USART ----
            char debug[64];
            sprintf(debug, "Dist=%ucm  Freq=%uHz  Vol=%u  Filter=%u\r\n",
                    filtered, freq, volume, Filter_GetSize());
            for (char *p = debug; *p; p++)
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

// ---------- Functie: 7-segment via PCF8574 ----------
void Seg7_DisplayNumber(uint8_t number)
{
    static const uint8_t segmap[10] = {
        0b00111111, // 0
        0b00000110, // 1
        0b01011011, // 2
        0b01001111, // 3
        0b01100110, // 4
        0b01101101, // 5
        0b01111101, // 6
        0b00000111, // 7
        0b01111111, // 8
        0b01101111  // 9
    };

    if (number > 9)
    {
        number = 9;
    }

    uint8_t pattern = segmap[number];

    TWI_MT_Start();
    TWI_Transmit_SLAW(SEG_ADDR);
    TWI_Transmit_Byte(pattern);
    TWI_Stop();
}
