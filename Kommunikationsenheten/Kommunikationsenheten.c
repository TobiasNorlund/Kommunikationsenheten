/*
 * Kommunikationsenheten.c
 *
 * Created: 4/9/2013 11:13:54 AM
 *  Author: tobno568
 */ 

#include <stdint.h>
#include <avr/io.h>
#include "../../TSEA27-include/utils.h"
#include "../../TSEA27-include/SPI/spi_slave.h"
#include "../../TSEA27-include/UART/uart.h"
#include "../../TSEA27-include/message.h"
#include "../../TSEA27-include/circularbuffer.h"


int main(void)
{
	SPI_SLAVE_init();
	UART_init();
	sei();
	CircularBuffer messageQueue; // Buffer för mottagna meddelanden via uart
	cbInit(&messageQueue, 32);
	uint8_t spiMsg[32];
	uint8_t spiType;
	uint8_t spiLen;
	
	uint8_t spiMsgR[32];
	uint8_t spiTypeR;
	uint8_t spiLenR;
	
	uint8_t uartMsg[16];
	uint8_t uartType;
	uint8_t uartLen;
	
	SETBIT(DDRB, DDB3);
	CLEARBIT(PORTB, PORTB3);
		
	while(1){
		if(UART_readMessage(uartMsg,&uartType,&uartLen)){
			if(uartType == TYPE_EMERGENCY_STOP){
				SETBIT(PORTB, PORTB3);
			}else{
				cbWrite(&messageQueue, (uartType<<5)|uartLen);
				for(uint8_t i=0; i < uartLen; i++)
				{
					cbWrite(&messageQueue, uartMsg[i]);
				}
			}
		}
		
		if(SPI_SLAVE_read(spiMsg, &spiType, &spiLen))
		{			
			if(spiType == TYPE_REQUEST_PC_MESSAGE)
			{
				uint8_t head = cbPeek(&messageQueue);
				uint8_t len = head&0b00011111;
				uint8_t type = 0b11100000&head;
				type = type>>5;
				if(len < cbBytesUsed(&messageQueue) && cbBytesUsed(&messageQueue) != 0)
				{
					head = cbRead(&messageQueue); //read head again, to sync
					for(uint8_t i = 0; i < len; i++)
					{
						spiMsgR[i]=cbRead(&messageQueue);
					}
					SPI_SLAVE_write(spiMsgR, type, len);	
				}
				else
				{
					SPI_SLAVE_write(spiMsgR, TYPE_NO_PC_MESSAGES, 0);
				}				
			}else if(spiType == 2){ //send to pc TODO
					
			}
		}		
	}
	return 0;
}