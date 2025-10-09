/*
 * Example Serial code using SPI C library
 */
#include <avr/io.h>
#include <cspi.h>

int main(void) {
	// init serial
	SPI_SlaveInit();

	// loop
	while(1) {
		// check for input
		uint8_t b = SPI_SlaveReceive();

		// return b to master (one cycle later)
		SPDR = b;
	}
	
	// never reached
	return(0);
}
