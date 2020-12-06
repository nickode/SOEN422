// File added by Nicolas Samaha
#include "hal_Loader.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static bool hal_Loader(u8 &mem)
{
	char c = VMIn_GetByte();
}

// Initialize receiver and stop flag bit for UART
void VMIn_Init()
{
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0); // Enable Receiver bit and Stop flag bit
}

// Function for receiving transmitted byte/char from the serial port.
char VMIn_GetByte()
{
	while (!(UCSR0A & (1 << RXC0))); // Wait for byte to be received
	char rxByte = UDR0;
	return rxByte;
}

ISR(USART_RX_vect)
{
    rxBuffer[rxWritePos] = UDR0;

    rxWritePos++;

    if (rxWritePos >= RX_BUFFER_SIZE)
    {
        rxWritePos = 0;
    }
}