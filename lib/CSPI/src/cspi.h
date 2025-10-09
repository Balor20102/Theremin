/*
 * Simple serial SPI C library
 */
#ifndef CSPI_H
#define CSPI_H

void SPI_MasterInit(void);
void SPI_MasterTransmit(uint8_t b);
void SPI_SlaveInit(void);
uint8_t SPI_SlaveReceive(void);

#endif
