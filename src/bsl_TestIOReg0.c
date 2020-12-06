/* bsl_TestIOReg0.c -- Test #0 - for I/O Register
//
// Copyright (C) 2020 by Michel de Champlain
//
*/

#include "hal.h"       // Hal_Init()
#include "bsl_IOReg.h"

#include <avr/io.h>
#include <util/delay.h>

#define MS_DELAY 1000

#define PortBDir  0x24L
#define PortBData 0x25L

int main(void ) {
    Hal_Init();

    // Set onboard LED for output.
    // DDRB |= (1 << PB5);
    bsl_IOReg_Write(PortBDir, bsl_IOReg_Read(PortBDir) | (u32)((u16)(1 << 5)) );


    while(1) {
        // Turn the LED on.
        bsl_IOReg_Write(PortBData, bsl_IOReg_Read(PortBData) | (u32)((u16)(1 << 5)) );

        _delay_ms(MS_DELAY);

        // Turn the LED off.
        bsl_IOReg_Write(PortBData, bsl_IOReg_Read(PortBData) & ~((u32)((u16)(1 << 5))) );

        _delay_ms(MS_DELAY);
    }
}
