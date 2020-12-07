// bsl_interman.c -- Interrupt Management Interface
/**Author: Pierre-Alexis Barras
 * ID: 40022016
 *
 * Produced for academic purposes.
 * All rights reseved. 2020
 */

#include "bsl_interman.h"

//returns the contents of the AVR Status Register
u16 GetStatusReg(void){ 
	return SREG[0]; 
}
