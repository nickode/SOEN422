// hal_interman.c -- Interrupt Management Interface
/**Author: Pierre-Alexis Barras
 * ID: 40022016
 *
 * Produced for academic purposes.
 * All rights reseved. 2020
 */

#include "hal_interman.h"
#include "bsl_interman.h"


void Interrupt_Disable(void){
	u16 * addr = SREG;
	*addr &= ~(1 << InterBit);
}

void Interrupt_Enable(void){
	u16 * addr = SREG;
	*addr |= (1 << InterBit);
}

u16 Interrupt_SaveAndDisable(void){
	u16 savestate = *SREG;
	Interrupt_Disable();
	
	return savestate;
}

void Interrupt_Restore(u16 flags){
	u16 * addr = SREG;
	*addr = flags;
}

//optional
void Interrupt_SetVector(u8 number, u32 handlerAddr){
	//code here..
	u8 p = 0;
	p++;
}

//optional
u32 Interrupt_GetVector(u8 number){
	//code here..
	return 0;
}
