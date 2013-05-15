/**
 * TSEA27 Elektronikprojekt
 *
 * IDENTIFIERING
 *
 * Modul: Kommunikationsenheten
 * Filnamn: kommunikationsenheten.c
 * Skriven av: T. Norlund, J. Källström
 * Datum: 2013-05-15
 * Version: 1.0
 *
 * BESKRIVNING
 *
 * Main-fil för kommunikationsenheten
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
	// Initiera SPI som slav
	SPI_SLAVE_init();
	
	// Initiera UART
	UART_init();
	
	// Sätt på interrupts
	sei();
	
	// Skapa meddelandebuffert för meddelanden som ska skickas till styrenheten
	CircularBuffer messageQueue;
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
	
	uint8_t stopped = 0;

	while(1){
		
		// Kolla om det har kommit ett helt meddelande från PCn
		if(UART_readMessage(uartMsg,&uartType,&uartLen)){
			// Kolla om det är ett nödstoppsanrop
			if(uartType == TYPE_EMERGENCY_STOP){
				SETBIT(PORTB, PORTB3);
				stopped=1;
			}else{
				// Annars, lägg hela meddelandet i meddelandebuffern
				cbWrite(&messageQueue, (uartType<<5)|uartLen);
				for(uint8_t i=0; i < uartLen; i++)
				{
					cbWrite(&messageQueue, uartMsg[i]);
				}
			}
			if(uartType == TYPE_CHANGE_PARM && stopped==1)
			{
				CLEARBIT(PORTB, PORTB3);
				stopped=0;
			}
		}
		
		// Kolla om vi har tagit emot ett helt meddelande från styrenheten
		if(SPI_SLAVE_read(spiMsg, &spiType, &spiLen))
		{	
			if(spiType == TYPE_REQUEST_PC_MESSAGE)
			{
				// Om det är en efterfrågan av PC-meddelanden, skicka det första meddelandet i buffern, om det finns något
				if(cbBytesUsed(&messageQueue)!=0)				
				{				
					uint8_t head = cbPeek(&messageQueue);
					uint8_t len = head&0b00011111;
					uint8_t type = 0b11100000&head;
					type = type>>5;
					if(len < cbBytesUsed(&messageQueue) && cbBytesUsed(&messageQueue) != 0)
					{
						head = cbRead(&messageQueue); // Läs headern igen
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
				}
				else
				{
					SPI_SLAVE_write(spiMsgR, TYPE_NO_PC_MESSAGES, 0);
				}			
			}else if(spiType == TYPE_MAP_DATA)
			{
				// Skicka kartdata till PC
				UART_writeMessage(spiMsg, spiType, spiLen);
			}
			else if(spiType == TYPE_DEBUG_DATA)
			{
				// Skicka debugdata till PC
				UART_writeMessage(spiMsg, spiType, spiLen);
			}
			else
			{

			}
		}

 	}
	return 0;
}
