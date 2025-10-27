/*
 * main.c - Theremin Project (bare metal)
 * ---------------------------------------
 * Lineaire pitch volgens technisch ontwerp:
 *   f = FREQ_MAX - ((distance * (FREQ_MAX - FREQ_MIN)) / MAX_DIST_CM)
 * Volume-simulatie via potmeter: toonhoogte wordt iets mee geschaald.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#include "usart.h"
#include "twi.h"
#include "hd44780pcf8574.h"
#include "ultrasonic.h"
#include "buzzer.h"
#include "filter.h"
#include "adc.h"
#include "7seg.h"

/* ---------- Config / constants ---------- */
#define LCD_ADDR 0x27
#define MAX_DIST_CM 65
#define FREQ_MIN 230U
#define FREQ_MAX 1400U
#define FILTER_MIN 1U
#define FILTER_MAX 15U
#define BTN_DOWN PD4
#define BTN_UP PD5

/* ---------- Helper functies ---------- */
static void usart_send_str(const char *s)
{
    while (*s)
        USART_Transmit((uint8_t)*s++);
}
static void usart_send_uint(uint16_t v)
{
    char buf[6];
    int n = snprintf(buf, sizeof(buf), "%u", (unsigned)v);
    for (int i = 0; i < n; ++i)
        USART_Transmit((uint8_t)buf[i]);
}

/* ---------- Buttons ---------- */
static void Buttons_Init(void)
{
    DDRD &= ~((1 << BTN_DOWN) | (1 << BTN_UP));
    PORTD |= (1 << BTN_DOWN) | (1 << BTN_UP);
}
static void Buttons_Handle(void)
{
    if (!(PIND & (1 << BTN_UP)))
    {
        _delay_ms(25);
        if (!(PIND & (1 << BTN_UP)))
        {
            uint8_t size = Filter_GetSize();
            if (size < FILTER_MAX)
                Filter_SetSize(size + 1);
            while (!(PIND & (1 << BTN_UP)))
                _delay_ms(5);
        }
    }
    if (!(PIND & (1 << BTN_DOWN)))
    {
        _delay_ms(25);
        if (!(PIND & (1 << BTN_DOWN)))
        {
            uint8_t size = Filter_GetSize();
            if (size > FILTER_MIN)
                Filter_SetSize(size - 1);
            while (!(PIND & (1 << BTN_DOWN)))
                _delay_ms(5);
        }
    }
}

/* ---------- Main ---------- */
int main(void)
{
    USART_Init();
    ADC_Init();
    TWI_Init();
    Ultrasonic_Init();
    Buzzer_Init();
    Filter_Init(5);
    SevenSeg_Init();

    HD44780_PCF8574_Init(LCD_ADDR);
    HD44780_PCF8574_DisplayOn(LCD_ADDR);
    HD44780_PCF8574_DisplayClear(LCD_ADDR);
    HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 0);
    HD44780_PCF8574_DrawString(LCD_ADDR, "Theremin Ready");

    Buttons_Init();
    sei();

    _delay_ms(800);
    HD44780_PCF8574_DisplayClear(LCD_ADDR);

    char line1[17], line2[17];

    while (1)
    {
        Ultrasonic_Trigger();
        _delay_ms(50);

        if (Ultrasonic_IsReady())
        {
            uint16_t distance = Ultrasonic_GetDistance();
            Filter_AddValue(distance);
            uint16_t filtered = Filter_GetMedian();
            if (filtered > MAX_DIST_CM)
                filtered = MAX_DIST_CM;

            /* Frequentie volgens technisch ontwerp (lineair) */
            uint32_t span = (uint32_t)FREQ_MAX - (uint32_t)FREQ_MIN;
            uint16_t freq = (uint16_t)((uint32_t)FREQ_MAX -
                                       ((uint32_t)filtered * span / (uint32_t)MAX_DIST_CM));

            /* Volume uitlezen en opslaan */
            uint8_t volume = ADC_GetValue();
            Buzzer_SetVolume(volume);

            /* 🎵 Software volume-simulatie:
             * schaal frequentie licht mee met potmeter
             * (0.5x tot 1.0x toonhoogte)
             */
            float scale = 0.5f + (volume / 510.0f); // 0..255 -> 0.5..1.0
            uint16_t effectiveFreq = (uint16_t)(freq * scale);
            if (effectiveFreq < FREQ_MIN)
                effectiveFreq = FREQ_MIN;
            if (effectiveFreq > FREQ_MAX)
                effectiveFreq = FREQ_MAX;

            Buzzer_Update(effectiveFreq);

            /* LCD bijwerken */
            snprintf(line1, sizeof(line1), "Dist:%3ucm", (unsigned)filtered);
            snprintf(line2, sizeof(line2), "Freq:%4uHz V:%3u", (unsigned)effectiveFreq, (unsigned)volume);
            HD44780_PCF8574_DisplayClear(LCD_ADDR);
            _delay_ms(2);
            HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 0);
            HD44780_PCF8574_DrawString(LCD_ADDR, line1);
            HD44780_PCF8574_PositionXY(LCD_ADDR, 0, 1);
            HD44780_PCF8574_DrawString(LCD_ADDR, line2);

            /* 7-seg: filtergrootte */
            SevenSeg_DisplayHex(Filter_GetSize());

            /* Debug */
            usart_send_str("Dist=");
            usart_send_uint(filtered);
            usart_send_str("cm  Freq=");
            usart_send_uint(effectiveFreq);
            usart_send_str("Hz  Vol=");
            usart_send_uint(volume);
            usart_send_str("  Fil=");
            usart_send_uint(Filter_GetSize());
            usart_send_str("\r\n");

            Buttons_Handle();
        }

        _delay_ms(100);
    }
    return 0;
}
