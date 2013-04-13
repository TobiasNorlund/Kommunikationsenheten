/*
 * Kommunikationsenheten.c
 *
 * Created: 4/9/2013 11:13:54 AM
 *  Author: tobno568
 */ 

#include <stdint.h>
#include <avr/io.h>
#include "../../TSEA27-include/UART/uart.h"
#include "../../TSEA27-include/message.h"


int main(void)
{
	// TODO: Initiera SPI
	
	// Initiera UART
	UART_init();
	
	
	while(1){
		
		// 1. Finns det data från PC:n? (UART)
		if(UART_hasMessage()){
			
			// Läs in meddelandet
			uint8_t msg;
			UART_readMessage(&msg);
			
			// Kolla om det är en begäran om nödstop
			if(getMessageType(&msg) == TYPE_EMERGENCY_STOP){
				// TODO: Nödstoppa!
			}else{
				
				// Lägg msg i SPI-TX-kö...
			}
		}
		
		// 2. Finns det data från styrenheten? (SPI)
		
		// TODO
		
	}
	return 0;
}