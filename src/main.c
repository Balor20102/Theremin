#include <avr/io.h>
#include <avr/interrupt.h>

#include <twi.h>

int main(void)
{
    // Initialize TWI (I2C) as a slave with address 0x42
    twi_init_slave(0x42);

    // Enable global interrupts
    sei();

    while (1)
    {
        // Main loop can perform other tasks
        // TWI events are handled in the interrupt service routines
    }

    return 0;
}