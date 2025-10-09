/*
 * Example Serial code using SPI C library
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <usart.h>
#include <cspi.h>

int main(void) {
	uint8_t b = 'A';

	// init USART
	USART_Init();

	// init serial
	SPI_MasterInit();

	// loop
	while(1) {
		// show returned value
		USART_Transmit('s');
		USART_Transmit(b);

		// check for input
		SPI_MasterTransmit(b);

		// show received value
		USART_Transmit('r');
		USART_Transmit(SPDR);
		USART_Transmit('\r');
		USART_Transmit('\n');

		// reset
		if (b++ >= 'Z') {
			b = 'A';
		}

		// wait a while
		_delay_ms(500);
	}
	
	// never reached
	return(0);
}
