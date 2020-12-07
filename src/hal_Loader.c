// File added by Nicolas Samaha
#include "hal_Loader.h"
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned long hal_Loader(u8* mem)
{
	char c;

	// Reading bytes coming from serial loader
	while ((c = VMIn_GetByte()) != 0x00)
	{	
		// If downloading, fill the memory with program contents
		if(downloading)
		{
			mem[memWritePos++] = c;
		}
		// Else, continue filling packet
		else
		{
			packet[packetReadPos++] = c;

			// If downloading, enable the download flag
			if(packetReadPos == 2 && c == Download)
			{
				downloading = true;
			}

			// If pinging, simply return success
			if(packetReadPos == 2 && c == Ping)
			{
				return Success;
			}

			// Check if memory contains contents, else return InvalidAddr
			if(packetReadPos == 2 && c == Run)
			{
				if(memWritePos != 0)
				{
					return Success;
				}
				else
				{
					return InvalidAddr;
				}
				
			}

		}

	}

	packetReadPos = 0;
	memWritePos = 0;

	// Finished receiving via UART
	return Sucess;

}

// Initialize receiver and stop flag bit for UART
void VMIn_Init()
{
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0); // Enable Receiver bit and Stop flag bit
	sei();
}

// Function for receiving transmitted byte/char from the serial port.
char VMIn_GetByte()
{
	while (!(UCSR0A & (1 << RXC0))); // Wait for byte to be received
	char rxByte = UDR0;
	return rxByte;
}