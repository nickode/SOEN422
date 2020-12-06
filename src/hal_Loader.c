// File added by Nicolas Samaha
#include "hal_Loader.h"
#include <avr/io.h>

static bool hal_Loader(u8 mem)
{
	while (memWritePos < 11)
	{
		// Read the next byte
		char c = VMIn_GetByte();

		// Check if non-zero and currently reading packet
		if (c != 0x00 && readingPacket == false)
		{
			readingPacket = true;
			mem[memWritePos++] = c;
		}
		else if (c == 0x00 && readingPacket == true)
		{
			readingPacket = false;
			return Success;
		}
		
	}

	// if bigger than 11 bytes, wrong command.
	return InvalidCmd;
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