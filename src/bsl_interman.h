// bsl_interman.h -- Interrupt Management Interface
/**Author: Pierre-Alexis Barras
 * ID: 40022016
 *
 * Produced for academic purposes.
 * All rights reseved. 2020
 */
#ifndef __bsl_Interrupt_h
#define __bsl_Interrupt_h
#include "_stdtype.h"

//Values Taken from ATMega328p Data sheet
#define InterAdd	0x5f	//address of the SREG register
#define InterBit	7		//Position of the IE bit
#define SREG		((u16 *)InterAdd)	//macro pointer to SREG

u16 GetStatusReg(void);

#endif
