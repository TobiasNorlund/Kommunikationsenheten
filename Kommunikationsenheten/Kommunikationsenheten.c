/*
 * Kommunikationsenheten.c
 *
 * Created: 4/9/2013 11:13:54 AM
 *  Author: tobno568
 */ 


#include <avr/io.h>

#define CONFIG_SPI_SLAVE

#include "include/commands.h"
#include "include/SPI/spi.h"


int main(void)
{
	SPI_SLAVE_init();
	
	uint8_t* len = 0;
	uint8_t msgR[16];
    if(1)
    {
		SPI_SLAVE_read(msgR, len);
        SPI_SLAVE_write(msgR, *len);

    }
}