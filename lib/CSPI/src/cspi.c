/*
 * Simple serial SPI C library
 */
#include <avr/io.h>
#include <cspi.h>

void SPI_MasterInit(void)
{
        /* Set SS, MOSI and SCK output, all others input */
        DDRB |= (1<<DDB2)|(1<<DDB3)|(1<<DDB5);

        /* Enable SPI, Master, set clock rate fck/16 */
        SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_SlaveInit(void)
{
        /* Set MISO output, all others input */
        DDRB |= (1<<DDB4);

        /* Enable SPI */
        SPCR = (1<<SPE);
}

void SPI_MasterTransmit(uint8_t b)
{
        /* Start transmission */
        SPDR = b;

        /* Wait for transmission complete */
        while(!(SPSR & (1<<SPIF)));
}

uint8_t SPI_SlaveReceive(void)
{
        /* Wait for reception complete */
        while(!(SPSR & (1<<SPIF)));

        /* Return Data Register */
        return SPDR;
}

