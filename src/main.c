/*
 * main.c - Theremin Project (bare metal)
 * ---------------------------------------
 * Hardware:
 *   - Ultrasonic: TRIG D9 (PB1), ECHO D8 (PB0)
 *   - Buzzer:     D3 (PD3 / OC2B)
 *   - Buttons:    D4 (down), D5 (up)
 *   - Potmeter:   A0 (ADC0)
 *   - LCD:        PCF8574 @ 0x27
 *   - 7-segment:  PCF8574 @ 0x21 en 0x39 (0 = segment aan)
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

// ---------- CONSTANTEN ----------
#define LCD_ADDR 0x27
#define SEG_ADDR1 0x21
#define SEG_ADDR2 0x39
#define MAX_DIST_CM 65
#define FREQ_MIN 230
#define FREQ_MAX 1400
#define FILTER_MIN 1
#define FILTER_MAX 15
#define BTN_DOWN PD4
#define BTN_UP PD5

// ---------- PROTOTYPES ----------
void Seg7_DisplayHex(uint8_t number);
void FilterButtons_Init(void);
void FilterButtons_Check(void);

// ---------- GLOBAAL ----------
volatile uint8_t filterSize = 5;

// ==============================================================
int main(void)
{
    // --- Init hardware ---
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

    _delay_ms(800);
    HD44780_PCF8574_DisplayClear(LCD_ADDR);

    while (1)
    {
        // --- 1. Ultrasone meting ---
        Ultrasonic_Trigger();
        _delay_ms(50);

        if (Ultrasonic_IsReady())
        {
            uint16_t distance = Ultrasonic_GetDistance();
            Filter_AddValue(distance);
            uint16_t filtered = Filter_GetMedian();
            if (filtered > MAX_DIST_CM)
                filtered = MAX_DIST_CM;

            // --- 2. Frequentie ---
            uint16_t freq = FREQ_MAX - ((filtered * (FREQ_MAX - FREQ_MIN)) / MAX_DIST_CM);
            Buzzer_SetFrequency(freq);

            // --- 3. Volume ---
            uint8_t volume = ADC_GetValue();
            OCR2B = volume;

            // --- 4. LCD ---
            char line1[17];
            char line2[17];
            snprintf(line1, sizeof(line1), "Dist:%3ucm", filtered);
            snprintf(line2, sizeof(line2), "Freq:%4uHz", freq);

            HD44780_PCF8574_DisplayClear(LCD_ADDR);
            HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 0);
            HD44780_PCF8574_DrawString(LCD_ADDR, line1);
            HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 1);
            HD44780_PCF8574_DrawString(LCD_ADDR, line2);

            // --- 5. 7-seg: filtergrootte ---
            Seg7_DisplayHex(Filter_GetSize());

            // --- 6. USART debug ---
            char msg[64];
            snprintf(msg, sizeof(msg),
                     "Dist=%ucm  Freq=%uHz  Vol=%u  Filter=%u\r\n",
                     filtered, freq, volume, Filter_GetSize());
            for (char *p = msg; *p; p++)
                USART_Transmit(*p);

            // --- 7. Knoppen ---
            FilterButtons_Check();
        }

        _delay_ms(100);
    }
    return 0;
}

// ==============================================================
void FilterButtons_Init(void)
{
    DDRD &= ~((1 << BTN_DOWN) | (1 << BTN_UP)); // ingangen
    PORTD |= (1 << BTN_DOWN) | (1 << BTN_UP);   // interne pull-ups
}

void FilterButtons_Check(void)
{
    // knop omhoog (D5)
    if (!(PIND & (1 << BTN_UP)))
    {
        _delay_ms(25);
        if (!(PIND & (1 << BTN_UP)))
        {
            uint8_t size = Filter_GetSize();
            if (size < FILTER_MAX)
            {
                Filter_SetSize(size + 1);
                USART_Transmit('U');
            }
            while (!(PIND & (1 << BTN_UP)))
                ;
        }
    }

    // knop omlaag (D4)
    if (!(PIND & (1 << BTN_DOWN)))
    {
        _delay_ms(25);
        if (!(PIND & (1 << BTN_DOWN)))
        {
            uint8_t size = Filter_GetSize();
            if (size > FILTER_MIN)
            {
                Filter_SetSize(size - 1);
                USART_Transmit('D');
            }
            while (!(PIND & (1 << BTN_DOWN)))
                ;
        }
    }
}

// ==============================================================
void Seg7_DisplayHex(uint8_t number)
{
    static const uint8_t segmap[16] = {
        0b11000000, // 0
        0b11111001, // 1
        0b10100100, // 2
        0b10110000, // 3
        0b10011001, // 4
        0b10010010, // 5
        0b10000010, // 6
        0b11111000, // 7
        0b10000000, // 8
        0b10010000, // 9
        0b10001000, // A
        0b10000011, // b
        0b10100111, // C
        0b10100001, // d
        0b10000110, // E
        0b10001110  // F
    };

    if (number > 15)
        number = 15;

    uint8_t pattern = segmap[number];

    // Stuur naar beide PCF's
    TWI_MT_Start();
    TWI_Transmit_SLAW(SEG_ADDR1);
    TWI_Transmit_Byte(pattern);
    TWI_Stop();

    TWI_MT_Start();
    TWI_Transmit_SLAW(SEG_ADDR2);
    TWI_Transmit_Byte(pattern);
    TWI_Stop();
}
