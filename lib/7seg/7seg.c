#ifdef __AVR__
#include <avr/io.h>
#endif
#include "7seg.h"
#include "twi.h"

// I2C-adressen voor de PCF8574
#define SEG_ADDR1 0x21
#define SEG_ADDR2 0x39

// Segmentpatronen: 0 = LED aan, 1 = LED uit
static const uint8_t segmap[16] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4 (gecorrigeerd)
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

void SevenSeg_Init(void)
{
    TWI_Init(); // Zorg dat I²C actief is
}

void SevenSeg_DisplayHex(uint8_t number)
{
    if (number > 15)
        number = 15;

    uint8_t pattern = segmap[number];

    // Stuur naar beide PCF8574's (GameShield gebruikt 2 voor dubbele helderheid)
    TWI_MT_Start();
    TWI_Transmit_SLAW(SEG_ADDR1);
    TWI_Transmit_Byte(pattern);
    TWI_Stop();

    TWI_MT_Start();
    TWI_Transmit_SLAW(SEG_ADDR2);
    TWI_Transmit_Byte(pattern);
    TWI_Stop();
}
